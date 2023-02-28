/*H********************************************************************************/
/*!

    \File protosslapple.c

    \Description
        This module is an SSL implementation that uses Apple's Secure
        Transport API.

    \Copyright
        Copyright (c) Electronic Arts 2002-2017
*/
/********************************************************************************H*/

/*** Includes *********************************************************************/

#include <Security/SecureTransport.h>
#include <Security/SecTrust.h>
#include <Security/SecCertificate.h>
#include <Security/SecImportExport.h>
#include <Security/SecIdentity.h>
#include <Availability.h>

#include "DirtySDK/crypt/cryptrand.h"

#include "DirtySDK/dirtydefs.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtynet.h"

#include "DirtySDK/util/base64.h"

#include "DirtySDK/proto/protossl.h"

#include "cryptrandpriv.h"

/*** Defines **********************************************************************/

// logging settings
#define DEBUG_RAW_DATA          (DIRTYCODE_LOGGING && 0)
#define DEBUG_RES_SESS          (DIRTYCODE_LOGGING && 0)

// ssl session constants
#define SSL_SESSION_HISTORY_MAX (32)
#define SSL_SESSION_ID_SIZE     (32)

// default capacity of store (enough for our baked in plus a handful)
#define CERT_STORE_DEFAULT_CAPACITY (4)

// min/default/max protocol versions supported
#define SSL3_VERSION_MIN        (PROTOSSL_VERSION_TLS1_0)
#define SSL3_VERSION            (PROTOSSL_VERSION_TLS1_2)
#define SSL3_VERSION_MAX        (PROTOSSL_VERSION_TLS1_2)

// internal states
#define ST_IDLE                 (0)
#define ST_ADDR                 (1)
#define ST_CONN                 (2)
#define ST_WAIT_CONN            (3)

#define ST3_SEND_HELLO          (4)
#define ST3_SEND_HELLO_VALIDATE (5)
#define ST3_SECURE              (6)
#define ST_UNSECURE             (7)

// failure conditions
#define ST_FAIL                 (0x1000)
#define ST_FAIL_DNS             (0x1001)
#define ST_FAIL_CONN            (0x1002)
#define ST_FAIL_CONN_NOCIPHER   (0x1003)
#define ST_FAIL_CERT_INVALID    (0x1004)
#define ST_FAIL_CERT_NOTRUST    (0x1005)
#define ST_FAIL_CERT_BADDATE    (0x1006)
#define ST_FAIL_SETUP           (0x1007)
#define ST_FAIL_SECURE          (0x1008)

// ALPN defines
#define SSL_ALPN_MAX_PROTOCOLS  (4) //!< max supported ALPN protocols

/*** Type Definitions *************************************************************/

//! secure session history, used for ssl resume
typedef struct SessionHistoryT
{
    uint32_t uSessionTick;                      //!< tick when session was last used
    struct sockaddr SessionAddr;                //!< addr of server session is owned by
    uint8_t aSessionId[SSL_SESSION_ID_SIZE];    //!< session id used to identify session
} SessionHistoryT;

//! global state
typedef struct ProtoSSLStateT
{
    //! critical section for locking access to state memory
    NetCritT StateCrit;

    //! previous session info, used for secure session share/resume
    SessionHistoryT aSessionHistory[SSL_SESSION_HISTORY_MAX];

    SecCertificateRef *pCertStore;  //!< global certificate store
    int32_t iNumCerts;              //!< current number of certs
    int32_t iMaxCertCapacity;       //!< current maximum capacity of store

    CFAllocatorRef pAllocator;      //!< custom allocator that redirects to DirtyMem
    int32_t iMemGroup;              //!< memgroup id
    void *pMemGroupUserData;        //!< memgroup data
} ProtoSSLStateT;

//! container for the baked in CAs
typedef struct ProtoSSLCertT
{
    const uint8_t *pCert;
    int32_t iCertSize;
} ProtoSSLCertT;

//! defines a configured cipher suite
typedef struct CipherSuiteT
{
    uint16_t uIdent;    //!< two-byte identifier
    uint32_t uId;       //!< PROTOSSL_CIPHER_*
    char strName[64];   //!< cipher name
} CipherSuiteT;

typedef struct SecureStateT
{
    SSLContextRef pContext;         //!< context uses for ssl operations
    const CipherSuiteT *pCipher;    //!< selected cipher suite

    char strAlpnProtocol[256];      //!< protocol negotiated using the alpn extension

    uint16_t uSslVersion;           //!< ssl version of the connection
    uint8_t bSessionResume;         //!< indicates session is resumed during handshake
    uint8_t _pad;
} SecureStateT;

//! module state
struct ProtoSSLRefT
{
    SocketT *pSock;                 //!< comm socket
    HostentT *pHost;                //!< host entry

    // module memory group
    int32_t iMemGroup;              //!< module mem group id
    void *pMemGroupUserData;        //!< user data associated with mem group

    char strHost[256];              //!< host that we connect to.
    struct sockaddr PeerAddr;       //!< peer info

    int32_t iState;                 //!< protocol state

    NetCritT SecureCrit;            //!< for guarding multithreaded access to secure state
    SecureStateT *pSecure;          //!< secure state reference

    ProtoSSLCertInfoT CertInfo;     //!< certificate info (used on failure)

    uint8_t *pCertificate;          //!< certificate
    int32_t iCertificateLen;        //!< certificate length

    char strCertificateKey[32];     //!< password for the certificate as SecureTransport requires

    uint32_t uEnabledCiphers;       //!< enabled ciphers
    int32_t iRecvBufSize;           //!< TCP recv buffer size; 0=default
    int32_t iSendBufSize;           //!< TCP send buffer size; 0=default
    int32_t iLastSocketError;       //!< Last socket error before closing the socket

    int32_t iMaxSendRate;           //!< max send rate (0=uncapped)
    int32_t iMaxRecvRate;           //!< max recv rate (0=uncapped)

    uint16_t uSslVersion;           //!< ssl version application wants us to use
    uint16_t uSslVersionMin;        //!< minimum ssl version application will accept
    uint8_t bSessionResumeEnabled;  //!< TRUE if session resume is enabled (default), else FALSE
    uint8_t bReuseAddr;             //!< if TRUE set SO_REUSEADDR
    int8_t  iVerbose;               //!< spam level
    uint8_t bNoDelay;               //!< enable TCP_NODELAY on the ssl socket
    uint8_t bKeepAlive;             //!< tcp keep-alive override; 0=default
    uint8_t _pad[3];

    uint32_t uKeepAliveTime;        //!< tcp keep-alive time; 0=default

    CFArrayRef pAlpnProtocols;      //!< array of CFStringRefs to pass to session, NULL if unset
};

/*** Variables ********************************************************************/

static ProtoSSLStateT *_ProtoSSL_pState = NULL;

//! ssl version name table
static const char *_SSL3_strVersionNames[] =
{
    "",
    "TLSv1",
    "TLSv1.1",
    "TLSv1.2",
    "TLSv1.3"
};

//! supported ssl cipher suites; see http://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-4
static const CipherSuiteT _SSL3_aCipherSuites[] =
{
    { TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384" },
    { TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256" },
    { TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_256_GCM_SHA384, "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384" },
    { TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256, "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256" },
    { TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256, "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256" },
    { TLS_RSA_WITH_AES_256_GCM_SHA384, PROTOSSL_CIPHER_RSA_WITH_AES_256_GCM_SHA384, "TLS_RSA_WITH_AES_256_GCM_SHA384" },
    { TLS_RSA_WITH_AES_128_GCM_SHA256, PROTOSSL_CIPHER_RSA_WITH_AES_128_GCM_SHA256, "TLS_RSA_WITH_AES_128_GCM_SHA256" },
    { TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, PROTOSSL_CIPHER_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256" },
    { TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384" },
    { TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256" },
    { TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA384, "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384" },
    { TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA256, "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256" },
    { TLS_RSA_WITH_AES_256_CBC_SHA256, PROTOSSL_CIPHER_RSA_WITH_AES_256_CBC_SHA256, "TLS_RSA_WITH_AES_256_CBC_SHA256" },
    { TLS_RSA_WITH_AES_128_CBC_SHA256, PROTOSSL_CIPHER_RSA_WITH_AES_128_CBC_SHA256, "TLS_RSA_WITH_AES_128_CBC_SHA256" },
    { TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_256_CBC_SHA, "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA" },
    { TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, PROTOSSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA" },
    { TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA, "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA" },
    { TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA, PROTOSSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA, "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA" },
    { TLS_RSA_WITH_AES_256_CBC_SHA, PROTOSSL_CIPHER_RSA_WITH_AES_256_CBC_SHA, "TLS_RSA_WITH_AES_256_CBC_SHA" },
    { TLS_RSA_WITH_AES_128_CBC_SHA, PROTOSSL_CIPHER_RSA_WITH_AES_128_CBC_SHA, "TLS_RSA_WITH_AES_128_CBC_SHA" }
};

// The 2048-bit public key modulus for 2015 GOS CA Cert signed with sha256 and an exponent of 65537
static const char _ProtoSSL_GOS2015ServerModulus2048[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIFVTCCBD2gAwIBAgIJAK+NJsjYxWDQMA0GCSqGSIb3DQEBCwUAMIHMMScwJQYD\n"
"VQQDEx5HT1MgMjAxNSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkxCzAJBgNVBAYTAlVT\n"
"MRMwEQYDVQQIEwpDYWxpZm9ybmlhMRUwEwYDVQQHEwxSZWR3b29kIENpdHkxHjAc\n"
"BgNVBAoTFUVsZWN0cm9uaWMgQXJ0cywgSW5jLjEdMBsGA1UECxMUR2xvYmFsIE9u\n"
"bGluZSBTdHVkaW8xKTAnBgkqhkiG9w0BCQEWGkdPU0RpcnR5c29ja1N1cHBvcnRA\n"
"ZWEuY29tMB4XDTE1MDExMzA5MzAxNVoXDTIwMDExMjA5MzAxNVowgcwxJzAlBgNV\n"
"BAMTHkdPUyAyMDE1IENlcnRpZmljYXRlIEF1dGhvcml0eTELMAkGA1UEBhMCVVMx\n"
"EzARBgNVBAgTCkNhbGlmb3JuaWExFTATBgNVBAcTDFJlZHdvb2QgQ2l0eTEeMBwG\n"
"A1UEChMVRWxlY3Ryb25pYyBBcnRzLCBJbmMuMR0wGwYDVQQLExRHbG9iYWwgT25s\n"
"aW5lIFN0dWRpbzEpMCcGCSqGSIb3DQEJARYaR09TRGlydHlzb2NrU3VwcG9ydEBl\n"
"YS5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDMbVS29OSE5yB2\n"
"AteXSHV+fvtDnD2hlkfBXQeLMHO/nf51lFUh0Ih0Zkyit/6fwDvwYKDbCDMrbvgC\n"
"BbmHnaxl1QadBejRtvXefaWkfYrLmTG2hZuizjnijGWqB/wVMwcA0XIVEw2HD1yi\n"
"XtDVv9kDMmKv9e9TNqg02raj7FxqwGf4vjefs8gt8DZKb2sG7reF8n9zbAGEg+Ta\n"
"RtAjmm3xd3wFgZBPakSDeDtxrRLASMhzifGYeHu0CEq66FlX4vwprL/1op1PLGTc\n"
"15IZHMX625LAkEuo6fINlBqyX90zrv9mkJeyqKUb+m9BsoS6UjSXStPHsj/d28mx\n"
"E4J36GrNAgMBAAGjggE2MIIBMjAdBgNVHQ4EFgQUIG5PT8/9501rZblXMe3TJTAX\n"
"hkMwggEBBgNVHSMEgfkwgfaAFCBuT0/P/edNa2W5VzHt0yUwF4ZDoYHSpIHPMIHM\n"
"MScwJQYDVQQDEx5HT1MgMjAxNSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkxCzAJBgNV\n"
"BAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRUwEwYDVQQHEwxSZWR3b29kIENp\n"
"dHkxHjAcBgNVBAoTFUVsZWN0cm9uaWMgQXJ0cywgSW5jLjEdMBsGA1UECxMUR2xv\n"
"YmFsIE9ubGluZSBTdHVkaW8xKTAnBgkqhkiG9w0BCQEWGkdPU0RpcnR5c29ja1N1\n"
"cHBvcnRAZWEuY29tggkAr40myNjFYNAwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0B\n"
"AQsFAAOCAQEAVUBj4PoeGzua+WXRB2+ISWKcqFyUwUrotkJsIaCSHW+Th1Q/OnJI\n"
"3Ad6se8W96JoGjXjWMtJeUa1wkey9L5Ek+VppbRD4n9DnAsPVGbnvmwby00RxmrU\n"
"0mdw/arydVvQ3Mp+cL31Aruv+Mh2TMlJ2ghORc1ZDfSOP7p1yECECSFTPFJB4gVO\n"
"J612cFq6Xl8wVSU2YHAAS5rUwR3BuFYIMyPsLcZhK2ZgpK4tTz6/UbIDkCioyleU\n"
"pKCsmCOKcBlhjUWytLOdRzOWMkntMpxYhsXBgG4T+D3LMspwmmfq93IMExSyGLkU\n"
"k+mEFzpz0aaPl32trGdasj4jdmdteC4HdQ==\n"
"-----END CERTIFICATE-----";

// list of baked in certs
const ProtoSSLCertT _ProtoSSL_aCerts[] =
{
    { (const uint8_t *)_ProtoSSL_GOS2015ServerModulus2048, sizeof(_ProtoSSL_GOS2015ServerModulus2048) }
};

/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _ProtoSSLConvertError

    \Description
        Tries to convert the SecureTransport SSL errors to an ProtoSSL error

    \Input iResult  - the result returned by the SecureTransport functions

    \Output
        int32_t     - the converted result, fails back to ST_FAIL

    \Version 05/15/2017 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoSSLConvertError(int32_t iResult)
{
    int32_t iConverted = ST_FAIL;

    switch (iResult)
    {
        // certificate data
        case errSSLBadCert:
        case errSSLPeerBadCert:
        case errSSLPeerUnsupportedCert:
        case errSSLXCertChainInvalid:
        {
            iConverted = ST_FAIL_CERT_INVALID;
            break;
        }

        // certificate trust
        case errSSLPeerCertRevoked:
        case errSSLPeerCertUnknown:
        case errSSLPeerUnknownCA:
        case errSSLUnknownRootCert:
        {
            iConverted = ST_FAIL_CERT_NOTRUST;
            break;
        }

        // certificate dates
        case errSSLCertExpired:
        case errSSLCertNotYetValid:
        case errSSLPeerCertExpired:
        {
            iConverted = ST_FAIL_CERT_BADDATE;
            break;
        }

        // cipher
        case errSSLNegotiation:
        case errSSLBadCipherSuite:
        {
            iConverted = ST_FAIL_CONN_NOCIPHER;
            break;
        }

        // any issues we expect when in handshake/secure state
        case errSSLBadConfiguration:
        case errSSLBadRecordMac:
        case errSSLBufferOverflow:
        case errSSLClosedAbort:
        case errSSLClosedGraceful:
        case errSSLClosedNoNotify:
        case errSSLConnectionRefused:
        case errSSLCrypto:
        case errSSLDecryptionFail:
        case errSSLFatalAlert:
        case errSSLHostNameMismatch:
        case errSSLIllegalParam:
        case errSSLInternal:
        case errSSLModuleAttach:
        case errSSLPeerAccessDenied:
        case errSSLPeerBadRecordMac:
        case errSSLPeerDecodeError:
        case errSSLPeerDecompressFail:
        case errSSLPeerDecryptError:
        case errSSLPeerDecryptionFail:
        case errSSLPeerExportRestriction:
        case errSSLPeerHandshakeFail:
        case errSSLPeerInsufficientSecurity:
        case errSSLPeerInternalError:
        case errSSLPeerNoRenegotiation:
        case errSSLPeerProtocolVersion:
        case errSSLPeerRecordOverflow:
        case errSSLPeerUnexpectedMsg:
        case errSSLPeerUserCancelled:
        case errSSLProtocol:
        case errSSLRecordOverflow:
        case errSSLSessionNotFound:
        case errSSLUnexpectedRecord:
        case errSSLWeakPeerEphemeralDHKey:
        {
            iConverted = ST_FAIL_SECURE;
            break;
        }

        // non failure cases
        case noErr:
        case errSSLWouldBlock:
        case errSSLPeerAuthCompleted:
        case errSSLClientCertRequested:
        case errSSLClientHelloReceived:
        {
            iConverted = 0;
            break;
        }

        default:
        {
            break;
        }
    }

    return(iConverted);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLRead

    \Description
        Performs the read/recv operations for the SSL socket

    \Input *pConnection - opaque ref we use for our socket
    \Input *pData       - buffer we are writing into
    \Input *pDataLen    - [in/out] size of the buffer and amount read

    \Output
        OSStatus        - noErr/errSSLWouldBlock=success, otherwise=failure

    \Version 05/15/2017 (eesponda)
*/
/********************************************************************************F*/
static OSStatus _ProtoSSLRead(SSLConnectionRef pConnection, void *pData, size_t *pDataLen)
{
    SocketT *pSock = (SocketT *)pConnection;
    char *pBuffer = pData;
    int32_t iLength = (int32_t)*pDataLen, iBytesRead = 0;
    OSStatus iStatusCode = noErr;

    while ((iBytesRead < iLength) && (iStatusCode == noErr))
    {
        int32_t iResult = SocketRecv(pSock, pBuffer+iBytesRead, iLength-iBytesRead, 0);
        if (iResult > 0)
        {
            iBytesRead += iResult;
        }
        else if (iResult == 0)
        {
            iStatusCode = errSSLWouldBlock;
        }
        else
        {
            iStatusCode = errSSLClosedAbort;
        }
    }

    #if DEBUG_RAW_DATA > 1
    NetPrintMem(pData, iBytesRead, "_ProtoSSLRead");
    #endif

    *pDataLen = iBytesRead;
    return(iStatusCode);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLWrite

    \Description
        Performs the write/send operations for the SSL socket

    \Input *pConnection - opaque ref we use for our socket
    \Input *pData       - buffer we are writing from
    \Input *pDataLen    - [in/out] size of the buffer and amount written

    \Output
        OSStatus        - noErr=success, otherwise=failure

    \Version 05/15/2017 (eesponda)
*/
/********************************************************************************F*/
static OSStatus _ProtoSSLWrite(SSLConnectionRef pConnection, const void *pData, size_t *pDataLen)
{
    int32_t iResult, iBytesWritten = 0;
    OSStatus iStatusCode = noErr;
    SocketT *pSock = (SocketT *)pConnection;

    if ((iResult = SocketSend(pSock, pData, (int32_t)*pDataLen, 0)) >= 0)
    {
        iBytesWritten += iResult;
    }
    else
    {
        // if the send failed then treat it as closed socket
        iStatusCode = errSSLClosedAbort;
    }

    #if DEBUG_RAW_DATA > 1
    NetPrintMem(pData, iBytesWritten, "_ProtoSSLWrite");
    #endif

    *pDataLen = iBytesWritten;
    return(iStatusCode);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLSessionHistorySet

    \Description
        Set entry in session history buffer

    \Input *pSessHistory    - session history entry to set
    \Input *pPeerAddr       - address of peer session is with
    \Input *pSessionId      - session id of session
    \Input uCurTick         - current tick count

    \Version 03/15/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoSSLSessionHistorySet(SessionHistoryT *pSessionHistory, struct sockaddr *pPeerAddr, const uint8_t *pSessionId, uint32_t uCurTick)
{
    // update peeraddr if it has changed
    if (SockaddrCompare(&pSessionHistory->SessionAddr, pPeerAddr))
    {
        NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: session history peer addr changed; updating\n"));
        ds_memcpy(&pSessionHistory->SessionAddr, pPeerAddr, sizeof(pSessionHistory->SessionAddr));
    }
    // update session id if it has changed
    if (memcmp(pSessionHistory->aSessionId, pSessionId, sizeof(pSessionHistory->aSessionId)))
    {
        NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: session history session id changed; updating\n"));
        ds_memcpy(&pSessionHistory->aSessionId, pSessionId, sizeof(pSessionHistory->aSessionId));
    }
    // touch access timestamp
    pSessionHistory->uSessionTick = uCurTick;
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLSessionHistoryGet

    \Description
        Returns pointer to specified session history entry, or NULL if not found.

    \Input *pPeerAddr   - address of peer session is with (may be null)
    \Input *pSessionId  - session id to look for (may be null)

    \Output
        SessionHistoryT * - session history entry, or NULL if not found

    \Version 03/15/2012 (jbrookes)
*/
/********************************************************************************F*/
static SessionHistoryT *_ProtoSSLSessionHistoryGet(struct sockaddr *pPeerAddr, const uint8_t *pSessionId)
{
    SessionHistoryT *pSessionInfo = NULL;
    ProtoSSLStateT *pState = _ProtoSSL_pState;
    const uint8_t aZeroSessionId[SSL_SESSION_ID_SIZE] = { 0 };
    int32_t iSession;

    if (pState == NULL)
    {
        NetPrintf(("protosslapple: warning, protossl global state not initialized, session history feature disabled\n"));
        return(NULL);
    }

    // exclude NULL session id
    if ((pSessionId != NULL) && (memcmp(pSessionId, aZeroSessionId, sizeof(aZeroSessionId)) == 0))
    {
        return(NULL);
    }

    // find session history
    for (iSession = 0; iSession < SSL_SESSION_HISTORY_MAX; iSession += 1)
    {
        SessionHistoryT *pSessionHistory = &pState->aSessionHistory[iSession];
        if ((pPeerAddr != NULL) && (SockaddrCompare(&pSessionHistory->SessionAddr, pPeerAddr) == 0))
        {
            pSessionInfo = pSessionHistory;
            break;
        }
        if ((pSessionId != NULL) && (memcmp(pSessionHistory->aSessionId, pSessionId, sizeof(pSessionHistory->aSessionId)) == 0))
        {
            pSessionInfo = pSessionHistory;
            break;
        }
    }

    // return session (or null to caller)
    return(pSessionInfo);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLSessionHistoryAdd

    \Description
        Save a new session in the session history buffer

    \Input *pPeerAddr   - address of peer session is with
    \Input *pSessionId  - session id for session

    \Version 03/15/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoSSLSessionHistoryAdd(struct sockaddr *pPeerAddr, const uint8_t *pSessionId)
{
    ProtoSSLStateT *pState = _ProtoSSL_pState;
    int32_t iSession, iSessionOldest, iTickDiffMax;
    uint32_t uCurTick = NetTick();

    if (pState == NULL)
    {
        NetPrintf(("protosslapple: warning, protossl global state not initialized, session history feature disabled\n"));
        return;
    }

    NetCritEnter(&pState->StateCrit);

    // see if we already have a session for this peer
    for (iSession = 0; iSession < SSL_SESSION_HISTORY_MAX; iSession += 1)
    {
        SessionHistoryT *pSessionHistory = &pState->aSessionHistory[iSession];
        if (SockaddrCompare(&pSessionHistory->SessionAddr, pPeerAddr) == 0)
        {
            NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: updating session history entry %d (addr=%A)\n", iSession, pPeerAddr));
            _ProtoSSLSessionHistorySet(pSessionHistory, pPeerAddr, pSessionId, uCurTick);
            NetCritLeave(&pState->StateCrit);
            return;
        }
    }

    // try to find an unallocated session
    for (iSession = 0; iSession < SSL_SESSION_HISTORY_MAX; iSession += 1)
    {
        SessionHistoryT *pSessionHistory = &pState->aSessionHistory[iSession];
        if (SockaddrInGetAddr(&pSessionHistory->SessionAddr) == 0)
        {
            NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: adding session history entry %d (addr=%A)\n", iSession, pPeerAddr));
            _ProtoSSLSessionHistorySet(pSessionHistory, pPeerAddr, pSessionId, uCurTick);
            NetCritLeave(&pState->StateCrit);
            return;
        }
    }

    // find the oldest session
    for (iSession = 0, iTickDiffMax = 0, iSessionOldest = 0; iSession < SSL_SESSION_HISTORY_MAX; iSession += 1)
    {
        SessionHistoryT *pSessionHistory = &pState->aSessionHistory[iSession];
        int32_t iTickDiff;

        // find least recently updated session
        if ((iTickDiff = NetTickDiff(uCurTick, pSessionHistory->uSessionTick)) > iTickDiffMax)
        {
            iTickDiffMax = iTickDiff;
            iSessionOldest = iSession;
        }
    }

    NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: replacing session history entry %d (addr=%A)\n", iSessionOldest, pPeerAddr));
    _ProtoSSLSessionHistorySet(&pState->aSessionHistory[iSessionOldest], pPeerAddr, pSessionId, uCurTick);

    NetCritLeave(&pState->StateCrit);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLSessionHistoryGetInfo

    \Description
        Check to see if we can find a session for the specified address or session
        id in our history buffer, and return sesession info if found.

    \Input *pSessionBuf - [out] storage for session history entry
    \Input *pPeerAddr   - address of peer session is with (may be null)
    \Input *pSessionId  - session id to look for (may be null)

    \Output
        SessionHistoryT * - session history entry, or NULL if not found

    \Version 03/15/2012 (jbrookes)
*/
/********************************************************************************F*/
static SessionHistoryT *_ProtoSSLSessionHistoryGetInfo(SessionHistoryT *pSessionBuf, struct sockaddr *pPeerAddr, const uint8_t *pSessionId)
{
    SessionHistoryT *pSessionInfo = NULL;
    ProtoSSLStateT *pState = _ProtoSSL_pState;

    if (pState == NULL)
    {
        NetPrintf(("protosslapple: warning, global protossl state not initialized, session history feature is disabled\n"));
        return(NULL);
    }

    // acquire access to session history
    NetCritEnter(&pState->StateCrit);

    // find session history
    if ((pSessionInfo = _ProtoSSLSessionHistoryGet(pPeerAddr, pSessionId)) != NULL)
    {
        // update timestamp
        pSessionInfo->uSessionTick = NetTick();
        // copy to caller
        ds_memcpy(pSessionBuf, pSessionInfo, sizeof(*pSessionBuf));
        pSessionInfo = pSessionBuf;
    }

    // release access to session history, return to caller
    NetCritLeave(&pState->StateCrit);
    return(pSessionInfo);
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLSetSockOpt

    \Description
        Set socket options

    \Input  *pState     - module state

    \Version 09/12/2012 (jbrookes) Refactored from ProtoSSLBind() and ProtoSSLConnect()
*/
/***************************************************************************F*/
static void _ProtoSSLSetSockOpt(ProtoSSLRefT *pState)
{
    // set debug level
    SocketControl(pState->pSock, 'spam', pState->iVerbose, NULL, NULL);

    // set recv/send buffer size?
    if (pState->iRecvBufSize != 0)
    {
        SocketControl(pState->pSock, 'rbuf', pState->iRecvBufSize, NULL, NULL);
    }
    if (pState->iSendBufSize != 0)
    {
        SocketControl(pState->pSock, 'sbuf', pState->iSendBufSize, NULL, NULL);
    }

    // set max send/recv rate?
    if (pState->iMaxRecvRate != 0)
    {
        SocketControl(pState->pSock, 'maxr', pState->iMaxRecvRate, NULL, NULL);
    }
    if (pState->iMaxSendRate != 0)
    {
        SocketControl(pState->pSock, 'maxs', pState->iMaxSendRate, NULL, NULL);
    }

    // set keep-alive options?
    if (pState->bKeepAlive != 0)
    {
        SocketControl(pState->pSock, 'keep', pState->bKeepAlive, &pState->uKeepAliveTime, &pState->uKeepAliveTime);
    }

    // set nodelay?
    if (pState->bNoDelay)
    {
        SocketControl(pState->pSock, 'ndly', TRUE, NULL, NULL);
    }

    // set reuseaddr
    if (pState->bReuseAddr)
    {
        SocketControl(pState->pSock, 'radr', TRUE, NULL, NULL);
    }
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLCleanupSecure

    \Description
        Cleanup the SecureTransport session

    \Input  *pState     - module state

    \Version 05/15/2017 (eesponda)
*/
/***************************************************************************F*/
static void _ProtoSSLCleanupSecure(ProtoSSLRefT *pState)
{
    // close the session
    SSLClose(pState->pSecure->pContext);

    // clean up the secure transport session
    CFRelease(pState->pSecure->pContext);
    pState->pSecure->pContext = NULL;

    // release memory
    DirtyMemFree(pState->pSecure, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    pState->pSecure = NULL;
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLResetSecureState

    \Description
        Reset secure state.  Does not affect the connection, if any.

    \Input *pState  - Reference pointer
    \Input iSecure  - secure status (0=disabled, 1=enabled)

    \Output
        int32_t     - SOCKERR_NONE on success, SOCKERR_NOMEM on failure

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLResetSecureState(ProtoSSLRefT *pState, int32_t iSecure)
{
    SecureStateT *pSecure;
    CFAllocatorRef pAllocator = _ProtoSSL_pState->pAllocator;

    // acquire access to secure state
    NetCritEnter(&pState->SecureCrit);

    // see if we need to get rid of secure state
    if (!iSecure && (pState->pSecure != NULL))
    {
        _ProtoSSLCleanupSecure(pState);
    }

    // see if we need to alloc secure state
    if (iSecure && (pState->pSecure == NULL))
    {
        pState->pSecure = (SecureStateT *)DirtyMemAlloc(sizeof(*pState->pSecure), PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        // create secure transport session
        if ((pState->pSecure->pContext = SSLCreateContext(pAllocator, kSSLClientSide, kSSLStreamType)) == NULL)
        {
            NetPrintf(("protosslapple: [%p] unable to allocate secure session context\n", pState));

            DirtyMemFree(pState->pSecure, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
            pState->pSecure = NULL;
        }
    }

    // reset secure state if present
    if ((pSecure = pState->pSecure) != NULL)
    {
        // set the callback functions for reading / writing
        SSLSetIOFuncs(pSecure->pContext, _ProtoSSLRead, _ProtoSSLWrite);
    }

    // release access to secure state
    NetCritLeave(&pState->SecureCrit);

    // return allocate error if secure wanted and failed
    return((iSecure && !pSecure) ? SOCKERR_NOMEM : SOCKERR_NONE);
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLResetState

    \Description
        Reset connection back to unconnected state (will disconnect from server).

    \Input *pState  - Reference pointer
    \Input iSecure  - to be completed

    \Output
        int32_t     - SOCKERR_NONE on success, SOCKERR_NOMEM on failure

    \Version 03/25/2004 (gschaefer)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLResetState(ProtoSSLRefT *pState, int32_t iSecure)
{
    // close socket if needed
    if (pState->pSock != NULL)
    {
        pState->iLastSocketError = SocketInfo(pState->pSock, 'serr', 0, NULL, 0);
        SocketClose(pState->pSock);
        pState->pSock = NULL;
    }

    // done with resolver record
    if (pState->pHost != NULL)
    {
        pState->pHost->Free(pState->pHost);
        pState->pHost = NULL;
    }

    // reset the state
    pState->iState = ST_IDLE;

    // reset secure state
    return(_ProtoSSLResetSecureState(pState, iSecure));
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLConvertVersion

    \Description
        Convert from our ssl version to the SecureTransport SSLProtocol

    \Input uIdent   - our ssl version

    \Output
        SSLProtocol - the enumeration of the ssl version

    \Notes
        We will use version TLS1.2 if we find no match

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static SSLProtocol _ProtoSSLConvertVersion(uint16_t uIdent)
{
    switch (uIdent)
    {
        case PROTOSSL_VERSION_TLS1_0:
            return(kTLSProtocol1);
        case PROTOSSL_VERSION_TLS1_1:
            return(kTLSProtocol11);
        case PROTOSSL_VERSION_TLS1_3:
            return(kTLSProtocol13);
        case PROTOSSL_VERSION_TLS1_2:
        default:
            return(kTLSProtocol12);
    }
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLConvertVersion2

    \Description
        Convert from the SecureTransport SSLProtocol to our ssl version

    \Input eProtocol    - the identifier used by SecureTransport

    \Output
        uint16_t        - ssl version we used internally

    \Notes
        We will use the highest protocol version if we find no match

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static uint16_t _ProtoSSLConvertVersion2(SSLProtocol eProtocol)
{
    switch (eProtocol)
    {
        case kTLSProtocol1:
            return(PROTOSSL_VERSION_TLS1_0);
        case kTLSProtocol11:
            return(PROTOSSL_VERSION_TLS1_1);
        case kTLSProtocol12:
        default:
            return(PROTOSSL_VERSION_TLS1_2);
    }
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLLoadCertificate

    \Description
        Load the pkcs12 certificate data from our state into the session

    \Input *pState  - module state

    \Notes
        In the event of a failure we just continue on without a
        certificate loaded

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static void _ProtoSSLLoadCertificate(ProtoSSLRefT *pState)
{
    int32_t iResult;
    CFDataRef pData;
    CFStringRef pPassword;
    CFDictionaryRef pOptions;
    SecIdentityRef pIdentity = NULL;
    CFArrayRef pImportResult = NULL, pCerts;
    CFTypeRef aCerts[1];
    const void *aValues[1], *aKeys[1];
    CFAllocatorRef pAllocator = _ProtoSSL_pState->pAllocator;

    // convert the buffer into something that the apple apis can understate
    if ((pData = CFDataCreate(pAllocator, pState->pCertificate, pState->iCertificateLen)) == NULL)
    {
        NetPrintf(("protosslapple: [%p] unable to create cfdata using certificate data\n", pState));
        return;
    }
    if ((pPassword = CFStringCreateWithCString(pAllocator, pState->strCertificateKey, kCFStringEncodingASCII)) == NULL)
    {
        NetPrintf(("protosslapple: [%p] unable to create string for certificate password\n", pState));

        CFRelease(pData);
        return;
    }
    // create a dictionary with the required options, currently you are required to use a password
    aKeys[0] = kSecImportExportPassphrase;
    aValues[0] = pPassword;
    if ((pOptions = CFDictionaryCreate(pAllocator, aKeys, aValues, 1, NULL, NULL)) == NULL)
    {
        NetPrintf(("protosslapple: [%p] unable to create dictionary with certificate load options\n", pState));

        CFRelease(pPassword);
        CFRelease(pData);
        return;
    }

    // try to import the certificate/key
    if ((iResult = SecPKCS12Import(pData, pOptions, &pImportResult)) != noErr)
    {
        NetPrintf(("protosslapple: [%p] failed to import the pkcs12 cert/key (%d/%s)\n", pState, iResult, DirtyErrGetName(iResult)));

        CFRelease(pOptions);
        CFRelease(pData);
        return;
    }
    // pull the identity out of the results and retain the pointer
    pIdentity = (SecIdentityRef)CFDictionaryGetValue(CFArrayGetValueAtIndex(pImportResult, 0), kSecImportItemIdentity);

    // release the memory we no longer need
    CFRelease(pOptions);
    CFRelease(pPassword);
    CFRelease(pData);

    // assign the identity and create the array as required by the API
    aCerts[0] = pIdentity;
    if ((pCerts = CFArrayCreate(pAllocator, aCerts, 1, &kCFTypeArrayCallBacks)) == NULL)
    {
        NetPrintf(("protosslapple: [%p] unable to allocate array needed for SSLSetCertificates\n", pState));
    }
    // load the certificate with the session
    else if ((iResult = SSLSetCertificate(pState->pSecure->pContext, pCerts)) != noErr)
    {
        NetPrintf(("protosslapple: [%p] set certificate failed (%d/%s)\n", pState, iResult, DirtyErrGetName(iResult)));
    }

    CFRelease(pCerts);
    CFRelease(pImportResult);
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLSetupHandshake

    \Description
        Setup the secure state to perform the handshake

    \Input *pState  - module state

    \Output
        int32_t     - ST3_SEND_HELLO=success, ST_FAIL_SETUP=failure

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLSetupHandshake(ProtoSSLRefT *pState)
{
    SessionHistoryT SessionHistory;
    SSLCipherSuite aCiphers[32];
    int32_t iNumCiphers, iCipher, iResult;
    SecureStateT *pSecure = pState->pSecure;

    // setup the connection
    SSLSetConnection(pSecure->pContext, pState->pSock);

    // if we have a previous secure session for this peer
    if ((_ProtoSSLSessionHistoryGetInfo(&SessionHistory, &pState->PeerAddr, NULL) != NULL) && (pState->bSessionResumeEnabled))
    {
        NetPrintfVerbose((DEBUG_RES_SESS, 0, "protosslapple: [%p] setting session id for resume\n", pState));

        #if DEBUG_RES_SESS && DEBUG_RAW_DATA
        NetPrintMem(SessionHistory.aSessionId, sizeof(SessionHistory.aSessionId), "SessionId (cached)");
        #endif

        pSecure->bSessionResume = TRUE;
    }
    else
    {
        CryptRandGet(SessionHistory.aSessionId, sizeof(SessionHistory.aSessionId));

        #if DEBUG_RES_SESS && DEBUG_RAW_DATA
        NetPrintMem(SessionHistory.aSessionId, sizeof(SessionHistory.aSessionId), "SessionId (generated)");
        #endif
    }

    // add the session identifier
    if ((iResult = SSLSetPeerID(pSecure->pContext, (const void *)SessionHistory.aSessionId, sizeof(SessionHistory.aSessionId))) != noErr)
    {
        NetPrintf(("protosslapple: [%p] failed to set session id (%s)\n", pState, DirtyErrGetName(iResult)));
        _ProtoSSLCleanupSecure(pState);
        return(ST_FAIL_SETUP);
    }

    // set the versions
    if ((iResult = SSLSetProtocolVersionMin(pSecure->pContext, _ProtoSSLConvertVersion(pState->uSslVersionMin))) != noErr)
    {
        NetPrintf(("protosslapple: [%p] failed to set min ssl version (%s)\n", pState, DirtyErrGetName(iResult)));
        _ProtoSSLCleanupSecure(pState);
        return(ST_FAIL_SETUP);
    }
    if ((iResult = SSLSetProtocolVersionMax(pSecure->pContext, _ProtoSSLConvertVersion(pState->uSslVersion))) != noErr)
    {
        NetPrintf(("protosslapple: [%p] failed to set max ssl version (%s)\n", pState, DirtyErrGetName(iResult)));
        _ProtoSSLCleanupSecure(pState);
        return(ST_FAIL_SETUP);
    }

    // configure the ciphers
    for (iCipher = 0, iNumCiphers = 0; iCipher < (signed)(sizeof(_SSL3_aCipherSuites)/sizeof(_SSL3_aCipherSuites[0])); iCipher += 1)
    {
        // skip disabled ciphers
        if ((_SSL3_aCipherSuites[iCipher].uId & pState->uEnabledCiphers) == 0)
        {
            continue;
        }
        // add cipher to list
        aCiphers[iNumCiphers] = _SSL3_aCipherSuites[iCipher].uIdent;
        iNumCiphers += 1;
    }
    if ((iResult = SSLSetEnabledCiphers(pSecure->pContext, aCiphers, iNumCiphers)) != noErr)
    {
        NetPrintf(("protosslapple: [%p] unable to configure ciphers (%s)\n", pState, DirtyErrGetName(iResult)));
        _ProtoSSLCleanupSecure(pState);
        return(ST_FAIL_SETUP);
    }

    // configure the certificate if necessary
    if (pState->pCertificate != NULL)
    {
        _ProtoSSLLoadCertificate(pState);
    }

    // configure the alpn protocols if set
    if (pState->pAlpnProtocols != NULL)
    {
        #if (__IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_11_0) || (__MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_13)
        SSLSetALPNProtocols(pSecure->pContext, pState->pAlpnProtocols);
        #endif
    }

    // enable TLS false start: https://tools.ietf.org/html/rfc7918
    SSLSetSessionOption(pSecure->pContext, kSSLSessionOptionFalseStart, TRUE);
    // enable 1/n-1 record splitting for BEAST attack mitigation (TLS 1.0, block ciphers)
    SSLSetSessionOption(pSecure->pContext, kSSLSessionOptionSendOneByteRecord, TRUE);
    // disable session renegotiation
    SSLSetSessionOption(pSecure->pContext, kSSLSessionOptionAllowRenegotiation, FALSE);
    // we break on the server authentication so we can load our own CAs before validating the server
    SSLSetSessionOption(pSecure->pContext, kSSLSessionOptionBreakOnServerAuth, TRUE);

    return(ST3_SEND_HELLO);
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLDoHandshake

    \Description
        Performs the handshake with the server

    \Input *pState  - module state

    \Output
        int32_t     - ST3_SEND_HELLO=pending, ST3_SEND_HELLO_VALIDATE=validate peer,
                      ST3_SECURE=success, otherwise=failure

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLDoHandshake(ProtoSSLRefT *pState)
{
    int32_t iResult, iCipher;
    SSLCipherSuite uId;
    SSLProtocol eProtocol = kTLSProtocol12;
    const void *pSessionId;
    size_t uSessionIdSize;
    SecureStateT *pSecure = pState->pSecure;
    CFArrayRef pAlpnProtocols = NULL;

    // perform the handshake via the SecureTransport API
    iResult = SSLHandshake(pSecure->pContext);

    /* if the handshake did not finish its work yet
       then we do not continue to the next states */
    if (iResult == errSSLWouldBlock)
    {
        return(pState->iState);
    }
    // if we are validating ourselves then move to the state so we can process that
    else if (iResult == errSSLPeerAuthCompleted)
    {
        return(ST3_SEND_HELLO_VALIDATE);
    }

    // handle handshake errors
    if (iResult != noErr)
    {
        NetPrintf(("protosslapple: [%p] handshake failed (%d/%s)\n", pState, iResult, DirtyErrGetName(iResult)));
        _ProtoSSLCleanupSecure(pState);
        return(_ProtoSSLConvertError(iResult));
    }

    // determine the negotiated cipher suite and set the cipher from the list
    SSLGetNegotiatedCipher(pSecure->pContext, &uId);
    for (iCipher = 0; iCipher < (signed)(sizeof(_SSL3_aCipherSuites)/sizeof(_SSL3_aCipherSuites[0])); iCipher += 1)
    {
        // skip non-matching ciphers
        if (uId != _SSL3_aCipherSuites[iCipher].uIdent)
        {
            continue;
        }

        // found a cipher
        pSecure->pCipher = &_SSL3_aCipherSuites[iCipher];
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] using cipher suite %s (ident=%u,%u)\n",
            pState, pSecure->pCipher->strName, (uint8_t)(uId>>8), (uint8_t)(uId)));
        break;
    }
    if (pSecure->pCipher == NULL)
    {
        NetPrintf(("protosslapple: [%p] no matching cipher\n", pState));
        /* server selecting a cipher we don't support results in a handshake failure alert;
           this should not happen and would indicate broken behaviour */
        return(ST_FAIL_CONN_NOCIPHER);
    }

    // determine the negotiated protocol version
    SSLGetNegotiatedProtocolVersion(pSecure->pContext, &eProtocol);
    pSecure->uSslVersion = _ProtoSSLConvertVersion2(eProtocol);
    NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] using protocol version %s\n", pState, _SSL3_strVersionNames[pSecure->uSslVersion&0xff]));

    // get the session id and save it to the history
    SSLGetPeerID(pSecure->pContext, &pSessionId, &uSessionIdSize);
    _ProtoSSLSessionHistoryAdd(&pState->PeerAddr, (const uint8_t *)pSessionId);

    #if (__IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_11_0) || (__MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_13)
    if (SSLCopyALPNProtocols(pSecure->pContext, &pAlpnProtocols) == noErr)
    {
        CFStringRef pProtocol = CFArrayGetValueAtIndex(pAlpnProtocols, 0);
        CFStringGetCString(pProtocol, pSecure->strAlpnProtocol, sizeof(pSecure->strAlpnProtocol), kCFStringEncodingASCII);
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] server negotiated protocol %s using alpn extension\n", pState, pSecure->strAlpnProtocol));
    }
    #else
    (void)pAlpnProtocols;
    #endif

    return(ST3_SECURE);
}

/*F**************************************************************************/
/*!
    \Function _ProtoSSLDoPeerValidate

    \Description
        Validates the peer certificate

    \Input *pState  - module state

    \Output
        int32_t     - ST3_SEND_HELLO=finish processing, otherwise=failure

    \Notes
        We do this processing in its own function since combined with the
        other handshake causes for a big spike. Moving this into a seperate
        state and function lets us cut it up into two parts.

    \Version 07/16/2018 (eesponda)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLDoPeerValidate(ProtoSSLRefT *pState)
{
    int32_t iResult, iCount;
    SecTrustRef pTrust;
    SecTrustResultType eTrustResult;
    CFArrayRef pCertArray;
    SecureStateT *pSecure = pState->pSecure;
    CFAllocatorRef pAllocator = _ProtoSSL_pState->pAllocator;

    // acquire the peer certificate information used for logging and validation
    SSLCopyPeerTrust(pSecure->pContext, &pTrust);

    // create an array to load the certificates with
    if ((pCertArray = CFArrayCreate(pAllocator, (const void **)_ProtoSSL_pState->pCertStore, _ProtoSSL_pState->iNumCerts, NULL)) != NULL)
    {
        // load our root certificates and cleanup array after we are finished
        if ((iResult = SecTrustSetAnchorCertificates(pTrust, pCertArray)) != noErr)
        {
            NetPrintf(("protosslapple: [%p] unable to load root certificates (%s)\n", pState, DirtyErrGetName(iResult)));
        }
        CFRelease(pCertArray);
    }
    else
    {
        NetPrintf(("protosslapple: [%p] unable to allow array for certificates needed to load root certs (%s)\n", pState));
    }

    // allow the API to load other root certs from the system root (users cannot add to this cert store)
    SecTrustSetAnchorCertificatesOnly(pTrust, FALSE);

    // perform the validation of the certificates
    SecTrustEvaluate(pTrust, &eTrustResult);

    // get the certificate information for logging
    if ((iCount = (int32_t)SecTrustGetCertificateCount(pTrust)) > 0)
    {
        SecCertificateRef pCert = SecTrustGetCertificateAtIndex(pTrust, iCount - 1);

        CFStringRef pCommon = SecCertificateCopySubjectSummary(pCert);
        CFStringGetCString(pCommon, pState->CertInfo.Ident.strCommon, sizeof(pState->CertInfo.Ident.strCommon), kCFStringEncodingASCII);
        CFRelease(pCommon);
    }

    // free the trust now that we are done with it
    CFRelease(pTrust);

    if ((eTrustResult == kSecTrustResultProceed) || (eTrustResult == kSecTrustResultUnspecified))
    {
        // if we are finished then go back into the handshaking state to finish
        return(ST3_SEND_HELLO);
    }
    else if (eTrustResult == kSecTrustResultRecoverableTrustFailure)
    {
        /* not trusted, for some reason other than being expired;
           could ask the user wether to allow the connection here */
        iResult = errSSLXCertChainInvalid;
    }
    else
    {
        // cannot use this certificate
        iResult = errSSLBadCert;
    }

    // handle validation errors
    NetPrintf(("protosslapple: [%p] handshake (validate) failed (%d/%s)\n", pState, iResult, DirtyErrGetName(iResult)));
    _ProtoSSLCleanupSecure(pState);
    return(_ProtoSSLConvertError(iResult));
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLFindPEMSignature

    \Description
        Find PEM signature in the input data, if it exists, and return a
        pointer to the encapsulated data.

    \Input *pCertData       - pointer to data to scan
    \Input iCertSize        - size of input data
    \Input *pSigText        - signature text to find

    \Output
        const uint8_t *     - pointer to data, or null if not found

    \Version 01/14/2009 (jbrookes)
*/
/***************************************************************************F*/
static const uint8_t *_ProtoSSLFindPEMSignature(const uint8_t *pCertData, int32_t iCertSize, const char *pSigText)
{
    int32_t iSigLen = (int32_t)strlen(pSigText);
    int32_t iCertIdx;

    for (iCertIdx = 0; iCertIdx < iCertSize; iCertIdx += 1)
    {
        if ((pCertData[iCertIdx] == *pSigText) && ((iCertSize - iCertIdx) >= iSigLen))
        {
            if (!strncmp((const char *)pCertData+iCertIdx, pSigText, iSigLen))
            {
                return(pCertData+iCertIdx);
            }
        }
    }
    return(NULL);
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLFindPEMCertificateData

    \Description
        Finds PEM signature in the input data, if it exists, and stores the
        offsets to the beginning and end of the embedded signature data.

    \Input *pCertData       - pointer to data to scan
    \Input iCertSize        - size of input data
    \Input **pCertBeg       - [out] - storage for index to beginning of certificate data
    \Input **pCertEnd       - [out] - storage for index to end of certificate data

    \Output
        int32_t             - 0=did not find certificate, else did find certificate

    \Version 01/14/2009 (jbrookes)
*/
/***************************************************************************F*/
static int32_t _ProtoSSLFindPEMCertificateData(const uint8_t *pCertData, int32_t iCertSize, const uint8_t **pCertBeg, const uint8_t **pCertEnd)
{
    static const char _strPemBeg[] = "-----BEGIN CERTIFICATE-----";
    static const char _strPemEnd[] = "-----END CERTIFICATE-----";
    static const char _str509Beg[] = "-----BEGIN X509 CERTIFICATE-----";
    static const char _str509End[] = "-----END X509 CERTIFICATE-----";

    // make sure "end-cert" occurs after start since we support bundles
    if (((*pCertBeg = _ProtoSSLFindPEMSignature(pCertData, iCertSize, _strPemBeg)) != NULL) &&
        ((*pCertEnd = _ProtoSSLFindPEMSignature(*pCertBeg, (int32_t)(pCertData+iCertSize-*pCertBeg), _strPemEnd)) != NULL))
    {
        *pCertBeg += strlen(_strPemBeg);
        return((int32_t)(*pCertEnd-*pCertBeg));
    }
    else if (((*pCertBeg = _ProtoSSLFindPEMSignature(pCertData, iCertSize, _str509Beg)) != NULL) &&
             ((*pCertEnd = _ProtoSSLFindPEMSignature(*pCertBeg, (int32_t)(pCertData+iCertSize-*pCertBeg), _str509End)) != NULL))
    {
        *pCertBeg += strlen(_str509Beg);
        return((int32_t)(*pCertEnd-*pCertBeg));
    }
    return(0);
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLAddCertificate

    \Description
        Add CA certificate to the global certificate store

    \Input pCert    - the certificate we are adding

    \Output
        int32_t     - number of certs added

    \Version 05/25/2017 (eesponda)
*/
/***************************************************************************F*/
static int32_t _ProtoSSLAddCertificate(SecCertificateRef pCert)
{
    ProtoSSLStateT *pState = _ProtoSSL_pState;

    // make sure we have the global state
    if (pState == NULL)
    {
        NetPrintf(("protosslapple: warning, protossl global state not initialized, loading certificates into store disabled\n"));
        return(0);
    }

    // check to see if we need to increase the size of the store
    if (pState->iNumCerts == pState->iMaxCertCapacity)
    {
        SecCertificateRef *pCertStore;
        pState->iMaxCertCapacity *= 2;

        // allocate memory for certificate store
        if ((pCertStore = (SecCertificateRef *)DirtyMemAlloc(sizeof(*pCertStore) * pState->iMaxCertCapacity, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
        {
            NetPrintf(("protosslapple: [%p] failed to allocate larger certificate store\n", pState));
            return(0);
        }
        ds_memcpy_s(pCertStore, sizeof(*pCertStore) * pState->iMaxCertCapacity, pState->pCertStore, sizeof(*pState->pCertStore) * pState->iNumCerts);

        // free old store and point to new memory
        DirtyMemFree(pState->pCertStore, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        pState->pCertStore = pCertStore;
    }

    pState->pCertStore[pState->iNumCerts] = pCert;
    pState->iNumCerts += 1;
    return(1);
}

/*F***************************************************************************/
/*!
    \Function _ProtoSSLLoadCACertificate

    \Description
        Create a SecCertificateRef from the certificate data

    \Input *pCertData       - the certificate data we are using to load the certificate
    \Input iCertSize        - size of the certificate data

    \Output
        SecCertificateRef   - loaded certificate data or NULL on failure

    \Version 05/25/2017 (eesponda)
*/
/***************************************************************************F*/
static SecCertificateRef _ProtoSSLLoadCACertificate(const uint8_t *pCertData, int32_t iCertSize)
{
    CFDataRef pData;
    SecCertificateRef pCert;
    CFAllocatorRef pAllocator = _ProtoSSL_pState->pAllocator;

    // allocate data from the cert
    if ((pData = CFDataCreate(pAllocator, pCertData, iCertSize)) == NULL)
    {
        NetPrintf(("protosslapple: failed to allocate memory for CFDataRef used for certificate loading\n"));
        return(NULL);
    }

    // create certificate from data
    pCert = SecCertificateCreateWithData(pAllocator, pData);

    // release the memory
    CFRelease(pData);
    return(pCert);
}

/*F*************************************************************************/
/*!
    \Function _ProtoSSLSetCACert

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all ProtoSSL modules for
        the lifetime of the application. This functional can add one or more
        PEM certificates or a single DER certificate.

    \Input *pCertData   - pointer to certificate data (PEM or DER)
    \Input iCertSize    - size of certificate data

    \Output
        int32_t         - negative=error, positive=count of CAs added

    \Notes
        The certificate must be in .DER (binary) or .PEM (base64-encoded)
        format.

    \Version 05/25/2017 (eesponda)
*/
/**************************************************************************F*/
static int32_t _ProtoSSLSetCACert(const uint8_t *pCertData, int32_t iCertSize)
{
    int32_t iResult, iCount = -1;
    uint8_t aCertBuffer[4096];
    const int32_t iMaxCertSize = (int32_t)sizeof(aCertBuffer);
    const uint8_t *pCertBeg, *pCertEnd;
    SecCertificateRef pCert;

    // process PEM signature if present
    if (_ProtoSSLFindPEMCertificateData(pCertData, iCertSize, &pCertBeg, &pCertEnd) == 0)
    {
        // no markers -- consume all the data
        pCertBeg = pCertData;
        pCertEnd = pCertData+iCertSize;
    }

    // remember remaining data for possible further parsing
    iCertSize -= pCertEnd-pCertData;
    pCertData = pCertEnd;

    // is the cert base64 encoded?
    if ((iResult = Base64Decode2((int32_t)(pCertEnd-pCertBeg), (const char *)pCertBeg, NULL)) > 0)
    {
        if (iResult > iMaxCertSize)
        {
            NetPrintf(("protosslapple: certificate exceeds maximum size\n"));
            return(-111);
        }
        // decode the cert
        Base64Decode2((int32_t)(pCertEnd-pCertBeg), (const char *)pCertBeg, (char *)aCertBuffer);
        pCertBeg = aCertBuffer;
        pCertEnd = pCertBeg+iResult;
    }

    // attempt to load certificate
    if ((pCert = _ProtoSSLLoadCACertificate(pCertBeg, (int32_t)(pCertEnd-pCertBeg))) != NULL)
    {
        // add certificate to CA list
        iCount = _ProtoSSLAddCertificate(pCert);
    }
    else
    {
        return(-113);
    }

    // if CA was PEM encoded and there is extra data, check for more CAs
    while ((iResult == 0) && (iCertSize > 0) && (_ProtoSSLFindPEMCertificateData(pCertData, iCertSize, &pCertBeg, &pCertEnd) != 0))
    {
        // remember remaining data for possible further parsing
        iCertSize -=  pCertEnd-pCertData;
        pCertData = pCertEnd;

        // cert must be base64 encoded
        if (((iResult = Base64Decode2((int32_t)(pCertEnd-pCertBeg), (const char *)pCertBeg, NULL)) <= 0) || (iResult > iMaxCertSize))
        {
            break;
        }
        Base64Decode2((int32_t)(pCertEnd-pCertBeg), (const char *)pCertBeg, (char *)aCertBuffer);

        // attempt to load certificate
        if ((pCert = _ProtoSSLLoadCACertificate(aCertBuffer, iResult)) == NULL)
        {
            continue;
        }

        // add certificate to CA list
        iCount += _ProtoSSLAddCertificate(pCert);
    }

    return(iCount);
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLAllocate

    \Description
        Allocates using our custom allocator which redirects to DirtyMem

    \Input uSize    - size of the allocation
    \Input Hint     - allocation hints (unused)
    \Input *pInfo   - allocator info (state data)

    \Output
        void *      - memory allocated or NULL if failure

    \Version 12/11/2018 (eesponda)
*/
/********************************************************************************F*/
static void *_ProtoSSLAllocate(CFIndex uSize, CFOptionFlags Hint, void *pInfo)
{
    ProtoSSLStateT *pState = (ProtoSSLStateT *)pInfo;
    const int32_t iSize = (int32_t)uSize;

    return(DirtyMemAlloc(iSize, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData));
}

/*F********************************************************************************/
/*!
    \Function _ProtoSSLDeallocate

    \Description
        Deallocates using our custom allocator which redirects to DirtyMem

    \Input *pPtr    - allocated memory we are freeing
    \Input *pInfo   - allocator info (state data)

    \Version 12/11/2018 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoSSLDeallocate(void *pPtr, void *pInfo)
{
    ProtoSSLStateT *pState = (ProtoSSLStateT *)pInfo;
    DirtyMemFree(pPtr, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

/*** Public Functions ********************************************************/


/*F***************************************************************************/
/*!
    \Function ProtoSSLStartup

    \Description
        Start up ProtoSSL.  Used to create global state shared across SSL refs.

    \Output
        int32_t         - negative=failure, else success

    \Version 09/14/2012 (jbrookes)
*/
/***************************************************************************F*/
int32_t ProtoSSLStartup(void)
{
    ProtoSSLStateT *pState;
    CFAllocatorContext Context;
    int32_t iMemGroup, iCert;
    void *pMemGroupUserData;

    // make sure we haven't already been called
    if (_ProtoSSL_pState != NULL)
    {
        NetPrintf(("protosslapple: global state already allocated\n"));
        return(-1);
    }

    // query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pState = (ProtoSSLStateT *)DirtyMemAlloc(sizeof(*pState), PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protosslapple: could not allocate global state\n"));
        return(-1);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->iMaxCertCapacity = CERT_STORE_DEFAULT_CAPACITY;

    // initialize custom allocator
    ds_memclr(&Context, sizeof(Context));
    Context.info = pState;
    Context.allocate = _ProtoSSLAllocate;
    Context.deallocate = _ProtoSSLDeallocate;
    if ((pState->pAllocator = CFAllocatorCreate(kCFAllocatorDefault, &Context)) == NULL)
    {
        NetPrintf(("protosslapple: could not allocate custom allocator state\n"));
        DirtyMemFree(pState, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData);
        return(-1);
    }

    // allocate memory for certificate store
    if ((pState->pCertStore = (SecCertificateRef *)DirtyMemAlloc(sizeof(*pState->pCertStore) * pState->iMaxCertCapacity, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protosslapple: could not allocate global certificate store\n"));
        CFRelease(pState->pAllocator);
        DirtyMemFree(pState, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData);
        return(-1);
    }
    ds_memclr(pState->pCertStore, sizeof(*pState->pCertStore) * pState->iMaxCertCapacity);

    // init global state crit
    NetCritInit(&pState->StateCrit, "ProtoSSL Global State");

    // initialize cryptrand module
    CryptRandInit();

    // save state ref
    _ProtoSSL_pState = pState;

    // load the baked in certs
    for (iCert = 0; iCert < (signed)(sizeof(_ProtoSSL_aCerts)/sizeof(_ProtoSSL_aCerts[0])); iCert += 1)
    {
        _ProtoSSLSetCACert(_ProtoSSL_aCerts[iCert].pCert, _ProtoSSL_aCerts[iCert].iCertSize);
    }

    return(0);
}

/*F***************************************************************************/
/*!
    \Function ProtoSSLShutdown

    \Description
        Shut down ProtoSSL.  Cleans up global state.

    \Version 09/14/2012 (jbrookes)
*/
/***************************************************************************F*/
void ProtoSSLShutdown(void)
{
    int32_t iCert;
    ProtoSSLStateT *pState = _ProtoSSL_pState;

    // make sure we haven't already been called
    if (pState == NULL)
    {
        NetPrintf(("protosslapple: global state not allocated\n"));
        return;
    }

    // shutdown cryptrand module
    CryptRandShutdown();

    // shut down, deallocate resources
    NetCritKill(&pState->StateCrit);
    for (iCert = 0; iCert < pState->iNumCerts; iCert += 1)
    {
        CFRelease(pState->pCertStore[iCert]);
        pState->pCertStore[iCert] = NULL;
    }

    // destroy custom allocator
    CFRelease(pState->pAllocator);
    pState->pAllocator = NULL;

    DirtyMemFree(pState->pCertStore, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    DirtyMemFree(pState, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);

    // clear state pointer
    _ProtoSSL_pState = NULL;
}

/*F***************************************************************************/
/*!
    \Function ProtoSSLCreate

    \Description
        Allocate an SSL connection and prepare for use

    \Output
        ProtoSSLRefT * - module state; NULL=failure

    \Version 03/08/2002 (gschaefer)
*/
/***************************************************************************F*/
ProtoSSLRefT *ProtoSSLCreate(void)
{
    ProtoSSLRefT *pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pState = (ProtoSSLRefT *)DirtyMemAlloc(sizeof(*pState), PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protosslapple: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->iLastSocketError = SOCKERR_NONE;
    pState->iVerbose = 1;
    pState->uSslVersion = SSL3_VERSION;
    pState->uSslVersionMin = SSL3_VERSION_MIN;
    pState->bSessionResumeEnabled = TRUE;

    // all ciphers enabled by default
    pState->uEnabledCiphers = PROTOSSL_CIPHER_ALL;

    // init secure state critical section
    NetCritInit(&pState->SecureCrit, "ProtoSSL Secure State");

    // return module state
    return(pState);
}

/*F**************************************************************************/
/*!
    \Function ProtoSSLReset

    \Description
        Reset connection back to unconnected state (will disconnect from server).

    \Input *pState      - module state reference

    \Version 03/08/2002 (gschaefer)
*/
/**************************************************************************F*/
void ProtoSSLReset(ProtoSSLRefT *pState)
{
    // reset to unsecure mode
    _ProtoSSLResetState(pState, 0);
}

/*F************************************************************************/
/*!
    \Function ProtoSSLDestroy

    \Description
        Destroy the module and release its state. Will disconnect from the
        server if required.

    \Input *pState      - module state reference

    \Version 03/08/2002 (gschaefer)
*/
/*************************************************************************F*/
void ProtoSSLDestroy(ProtoSSLRefT *pState)
{
    // reset to unsecure mode (free all secure resources)
    _ProtoSSLResetState(pState, 0);
    // release alpn array if valid
    if (pState->pAlpnProtocols != NULL)
    {
        CFRelease(pState->pAlpnProtocols);
    }
    // kill critical section
    NetCritKill(&pState->SecureCrit);
    // free remaining state
    DirtyMemFree(pState, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLAccept

    \Description
        Accept an incoming connection.

    \Input *pState      - module state reference
    \Input iSecure      - flag indicating use of secure connection: 1=secure, 0=unsecure
    \Input *pAddr       - where the client's address should be written
    \Input *pAddrlen    - the length of the client's address space

    \Output
        ProtoSSLRefT *  - accepted connection or NULL if not available

    \Version 10/27/2013 (jbrookes)
*/
/*************************************************************************F*/
ProtoSSLRefT *ProtoSSLAccept(ProtoSSLRefT *pState, int32_t iSecure, struct sockaddr *pAddr, int32_t *pAddrlen)
{
    return(NULL);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLBind

    \Description
        Create a socket bound to the given address.

    \Input *pState  - module state reference
    \Input *pAddr   - the IPv4 address
    \Input iAddrlen - the size of the IPv4 address.

    \Output
        int32_t     - SOCKERR_xxx (zero=success, negative=failure)

    \Version 03/03/2004 (sbevan)
*/
/*************************************************************************F*/
int32_t ProtoSSLBind(ProtoSSLRefT *pState, const struct sockaddr *pAddr, int32_t iAddrlen)
{
    // if we had a socket, get last error from it and close it
    if (pState->pSock != NULL)
    {
        pState->iLastSocketError = SocketInfo(pState->pSock, 'serr', 0, NULL, 0);
        SocketClose(pState->pSock);
    }

    // create the socket
    if ((pState->pSock = SocketOpen(AF_INET, SOCK_STREAM, 0)) == NULL)
    {
        return(SOCKERR_OTHER);
    }

    // set socket options
    _ProtoSSLSetSockOpt(pState);

    // do the bind, return result
    return(SocketBind(pState->pSock, pAddr, iAddrlen));
}

/*F**************************************************************************/
/*!
    \Function ProtoSSLConnect

    \Description
        Make a secure connection to an SSL server.

    \Input *pState  - module state reference
    \Input iSecure  - flag indicating use of secure connection (1=secure, 0=unsecure)
    \Input *pAddr   - textual form of address (1.2.3.4 or www.ea.com)
    \Input uAddr    - the IP address of the server (if not in textual form)
    \Input iPort    - the TCP port of the server (if not in textual form)

    \Output
        int32_t     - SOCKERR_xxx (zero=success, negative=failure)

    \Version 05/15/2017 (eesponda)
*/
/***************************************************************************F*/
int32_t ProtoSSLConnect(ProtoSSLRefT *pState, int32_t iSecure, const char *pAddr, uint32_t uAddr, int32_t iPort)
{
    int32_t iIndex;
    int32_t iError;

    // reset connection state
    if ((iError = _ProtoSSLResetState(pState, iSecure)) != SOCKERR_NONE)
    {
        return(iError);
    }

    // allocate the socket
    if ((pState->pSock = SocketOpen(AF_INET, SOCK_STREAM, 0)) == NULL)
    {
        return(SOCKERR_NORSRC);
    }

    // set socket options
    _ProtoSSLSetSockOpt(pState);

    // init peer structure
    SockaddrInit(&pState->PeerAddr, AF_INET);

    // clear previous cert info, if any
    ds_memclr(&pState->CertInfo, sizeof(pState->CertInfo));

    // handle default address case
    if (pAddr == NULL)
    {
        pAddr = "";
    }

    // parse the address string
    for (iIndex = 0; (pAddr[iIndex] != 0) && (pAddr[iIndex] != ':') && ((unsigned)iIndex < sizeof(pState->strHost)-1); ++iIndex)
    {
        // copy over to host
        pState->strHost[iIndex] = pAddr[iIndex];
    }
    pState->strHost[iIndex] = 0;

    // attempt to set host address
    SockaddrInSetAddrText(&pState->PeerAddr, pState->strHost);
    if (SockaddrInGetAddr(&pState->PeerAddr) == 0)
    {
        SockaddrInSetAddr(&pState->PeerAddr, uAddr);
    }

    // attempt to set peer address
    if (pAddr[iIndex] == ':')
    {
        SockaddrInSetPort(&pState->PeerAddr, atoi(pAddr+iIndex+1));
    }
    else
    {
        SockaddrInSetPort(&pState->PeerAddr, iPort);
    }

    // see if we need to start DNS request
    if (SockaddrInGetAddr(&pState->PeerAddr) == 0)
    {
        // do dns lookup prior to connect
        pState->pHost = SocketLookup(pState->strHost, 30*1000);
        pState->iState = ST_ADDR;
    }
    else
    {
        // set to connect state
        pState->iState = ST_CONN;
    }

    // if we are using secure then set the peer domain (for SNI?)
    if (pState->pSecure != NULL)
    {
        SSLSetPeerDomainName(pState->pSecure->pContext, pState->strHost, sizeof(pState->strHost));
    }

    // return error code
    return(SOCKERR_NONE);
}

/*F**************************************************************************/
/*!
    \Function ProtoSSLListen

    \Description
        Start listening for an incoming connection.

    \Input *pState  - module state reference
    \Input iBacklog - number of pending connections allowed

    \Output
        int32_t     - SOCKERR_xxx (zero=success, negative=failure)

    \Version 03/03/2004 (sbevan)
*/
/**************************************************************************F*/
int32_t ProtoSSLListen(ProtoSSLRefT *pState, int32_t iBacklog)
{
    return(SocketListen(pState->pSock, iBacklog));
}

/*F*************************************************************************************************/
/*!
    \Function ProtoSSLDisconnect

    \Description
        Disconnect from the server

    \Input *pState  - module state reference

    \Output
        int32_t     - SOCKERR_xxx (zero=success, negative=failure)

    \Version 03/08/2002 (gschaefer)
*/
/*************************************************************************************************F*/
int32_t ProtoSSLDisconnect(ProtoSSLRefT *pState)
{
    if (pState->pSock != NULL)
    {
        // send a close_notify alert as per http://tools.ietf.org/html/rfc5246#section-7.2.1
        if ((pState->pSecure != NULL) && (pState->iState == ST3_SECURE))
        {
            _ProtoSSLCleanupSecure(pState);
        }

        /* on the client we do an immediate hard
           close of the socket as the calling application that is polling to drive our operation
           may stop updating us once they have received all of the data, which would prevent us
           from closing the socket */
        SocketClose(pState->pSock);
        pState->pSock = NULL;
    }

    pState->iState = ST_IDLE;

    return(SOCKERR_NONE);
}

/*F*************************************************************************************************/
/*!
    \Function ProtoSSLUpdate

    \Description
        Give time to module to do its thing (should be called
        periodically to allow module to perform work). Calling
        once per 100ms or so should be enough.

        This is actually a collection of state functions plus
        the overall update function. They are kept together for
        ease of reading.

    \Input *pState - module state reference

    \Version 05/15/2017 (eesponda)
*/
/*************************************************************************************************F*/
void ProtoSSLUpdate(ProtoSSLRefT *pState)
{
    int32_t iResult;
    SecureStateT *pSecure = pState->pSecure;

    // resolve the address
    if (pState->iState == ST_ADDR)
    {
        // check for completion
        if (pState->pHost->Done(pState->pHost))
        {
            pState->iState = (pState->pHost->addr != 0) ? ST_CONN : ST_FAIL_DNS;
            SockaddrInSetAddr(&pState->PeerAddr, pState->pHost->addr);
            // free the record
            pState->pHost->Free(pState->pHost);
            pState->pHost = NULL;
        }
    }

    // see if we should start a connection
    if (pState->iState == ST_CONN)
    {
        // start the connection attempt
        if ((iResult = SocketConnect(pState->pSock, &pState->PeerAddr, sizeof(pState->PeerAddr))) == SOCKERR_NONE)
        {
            pState->iState = ST_WAIT_CONN;
        }
        else
        {
            pState->iState = ST_FAIL_CONN;
        }
    }

    // wait for connection
    if (pState->iState == ST_WAIT_CONN)
    {
        iResult = SocketInfo(pState->pSock, 'stat', 0, NULL, 0);
        if (iResult > 0)
        {
            if (pSecure != NULL)
            {
                // get access to secure state
                NetCritEnter(&pState->SecureCrit);

                pState->iState = _ProtoSSLSetupHandshake(pState);

                // release access to secure state
                NetCritLeave(&pState->SecureCrit);
            }
            else
            {
                pState->iState = ST_UNSECURE;
            }
        }
        else if (iResult < 0)
        {
            pState->iState = ST_FAIL_CONN;
        }
    }

    // do the handshake
    if (pState->iState == ST3_SEND_HELLO)
    {
        // get access to secure state
        NetCritEnter(&pState->SecureCrit);

        pState->iState = _ProtoSSLDoHandshake(pState);

        // release access to secure state
        NetCritLeave(&pState->SecureCrit);
    }
    // validate the peer cert if it's time
    else if (pState->iState == ST3_SEND_HELLO_VALIDATE)
    {
        // get access to the secure state
        NetCritEnter(&pState->SecureCrit);

        pState->iState = _ProtoSSLDoPeerValidate(pState);

        // release access to secure state
        NetCritLeave(&pState->SecureCrit);
    }

    if (pState->iState == ST3_SECURE)
    {
        SSLSessionState eState;

        // get access to secure state
        NetCritEnter(&pState->SecureCrit);

        // obtain the connected state from the session
        SSLGetSessionState(pSecure->pContext, &eState);

        // release access to secure state
        NetCritLeave(&pState->SecureCrit);

        // if we are no longer connected, then close
        if (eState != kSSLConnected)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] received close notification\n", pState));
            _ProtoSSLResetState(pState, 0);
        }
    }
}

/*F*************************************************************************************************/
/*!
    \Function ProtoSSLSend

    \Description
        Send data to the server over secure TCP connection (actually, whether the
        connection is secure or not is determined by the secure flag passed during
        the SSLConnect call).

    \Input *pState  - module state reference
    \Input *pBuffer - data to send
    \Input iLength  - length of data (if negative, input data is assumed to be null-terminated string)

    \Output
        int32_t     - negative=error, otherwise number of bytes sent

    \Version 05/15/2017 (eesponda)
*/
/*************************************************************************************************F*/
int32_t ProtoSSLSend(ProtoSSLRefT *pState, const char *pBuffer, int32_t iLength)
{
    int32_t iResult = SOCKERR_CLOSED;
    SecureStateT *pSecure = pState->pSecure;

    // allow easy string sends
    if (iLength < 0)
    {
        iLength = (int32_t)strlen(pBuffer);
    }
    // guard against zero-length sends, which can result in an invalid send condition with some stream (e.g. RC4) ciphers
    if (iLength == 0)
    {
        return(0);
    }

    // make sure connection established
    if (pState->iState == ST3_SECURE)
    {
        size_t uProcessed = 0;

        // get access to secure state
        NetCritEnter(&pState->SecureCrit);

        iResult = SSLWrite(pSecure->pContext, pBuffer, iLength, &uProcessed);
        iResult = _ProtoSSLConvertError(iResult) == 0 ? (int32_t)uProcessed : SOCKERR_CLOSED;

        // release access to secure state
        NetCritLeave(&pState->SecureCrit);
    }

    // handle unsecure sends
    if (pState->iState == ST_UNSECURE)
    {
        iResult = SocketSend(pState->pSock, pBuffer, iLength, 0);
    }

    // return the result
    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function ProtoSSLRecv

    \Description
        Receive data from the server

    \Input *pState  - module state reference
    \Input *pBuffer - receiver data
    \Input iLength  - maximum buffer length

    \Output
        int32_t     - negative=error, zero=nothing available, positive=bytes received

    \Version 05/15/2017 (eesponda)
*/
/*************************************************************************************************F*/
int32_t ProtoSSLRecv(ProtoSSLRefT *pState, char *pBuffer, int32_t iLength)
{
    SecureStateT *pSecure = pState->pSecure;
    int32_t iResult = 0;

    // handle secure receive
    if (pState->iState == ST3_SECURE)
    {
        size_t uProcessed = 0;

        // get access to secure state
        NetCritEnter(&pState->SecureCrit);

        iResult = SSLRead(pSecure->pContext, pBuffer, iLength, &uProcessed);

        /* if we received data but ended up getting FIN we don't want to treat this as an error
           the closing of the socket will be handled after we process all the data */
        iResult = ((uProcessed > 0) || (_ProtoSSLConvertError(iResult) == 0)) ?
            (int32_t)uProcessed : SOCKERR_CLOSED;

        // release access to secure state
        NetCritLeave(&pState->SecureCrit);
    }

    // handle unsecure receive
    if (pState->iState == ST_UNSECURE)
    {
        iResult = SocketRecv(pState->pSock, pBuffer, iLength, 0);
    }

    // return error if in failure state
    if (pState->iState >= ST_FAIL)
    {
        iResult = -1;
    }

    // terminate buffer if there is room
    if ((iResult > 0) && (iResult < iLength))
    {
        pBuffer[iResult] = 0;
    }

    // return the data size
    return(iResult);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLStat

    \Description
        Return the current module status (according to selector)

    \Input *pState  - module state reference
    \Input iSelect  - status selector ('conn'=return "am i connected" flag)
    \Input pBuffer  - buffer pointer
    \Input iLength  - buffer size

    \Output
        int32_t     - negative=error, zero=false, positive=true

    \Notes
        Selectors are:

        \verbatim
            SELECTOR    DESCRIPTION
            'addr'      Address of peer we are connecting/connected to
            'alpn'      Get the negotiated protocol using the ALPN extension from the secure state
            'cert'      Return SSL cert info (valid after 'fail')
            'ciph'      Cipher suite negotiated (string name in output buffer)
            'fail'      Return PROTOSSL_ERROR_* or zero if no error
            'hres'      Return hResult containing either the socket error or ssl error
            'htim'      Return timing of most recent SSL handshake in milliseconds
            'maxr'      Return max recv rate (0=uncapped)
            'maxs'      Return max send rate (0=uncapped)
            'resu'      Returns whether session was resumed or not
            'serr'      Return socket error
            'sock'      Copy SocketT ref to output buffer
            'stat'      Return like SocketInfo('stat')
            'vers'      Return current SSL version
        \endverbatim

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
int32_t ProtoSSLStat(ProtoSSLRefT *pState, int32_t iSelect, void *pBuffer, int32_t iLength)
{
    int32_t iResult = -1;

    // pass-through to SocketInfo(NULL,...)
    if (pState == NULL)
    {
        return(SocketInfo(NULL, iSelect, 0, pBuffer, iLength));
    }
    // return address of peer we are trying to connect to
    if (iSelect == 'addr')
    {
        if ((pBuffer != NULL) && (iLength == sizeof(pState->PeerAddr)))
        {
            ds_memcpy(pBuffer, &pState->PeerAddr, iLength);
        }
        return(SockaddrInGetAddr(&pState->PeerAddr));
    }
    // return alpn protocol negogiated if any
    if ((iSelect == 'alpn') && (pState->pSecure != NULL))
    {
        if (pBuffer != NULL)
        {
            ds_strnzcpy(pBuffer, pState->pSecure->strAlpnProtocol, iLength);
        }

        return(0);
    }
    // return certificate info (valid after 'fail' response)
    if ((iSelect == 'cert') && (pBuffer != NULL) && (iLength == sizeof(pState->CertInfo)))
    {
        ds_memcpy(pBuffer, &pState->CertInfo, sizeof(pState->CertInfo));
        return(0);
    }
    // return current cipher suite
    if ((iSelect == 'ciph') && (pState->pSecure != NULL) && (pState->pSecure->pCipher != NULL))
    {
        if (pBuffer != NULL)
        {
            ds_strnzcpy(pBuffer, pState->pSecure->pCipher->strName, iLength);
        }
        return(pState->pSecure->pCipher->uId);
    }
    // return failure code
    if (iSelect == 'fail')
    {
        if (pState->iState & ST_FAIL)
        {
            switch(pState->iState)
            {
                case ST_FAIL_DNS:
                    iResult = PROTOSSL_ERROR_DNS;
                    break;
                case ST_FAIL_CONN:
                    iResult = PROTOSSL_ERROR_CONN;
                    break;
                case ST_FAIL_CONN_NOCIPHER:
                    iResult = PROTOSSL_ERROR_CONN_NOCIPHER;
                    break;
                case ST_FAIL_CERT_INVALID:
                    iResult = PROTOSSL_ERROR_CERT_INVALID;
                    break;
                case ST_FAIL_CERT_NOTRUST:
                    iResult = PROTOSSL_ERROR_CERT_NOTRUST;
                    break;
                case ST_FAIL_CERT_BADDATE:
                    iResult = PROTOSSL_ERROR_CERT_BADDATE;
                    break;
                case ST_FAIL_SETUP:
                    iResult = PROTOSSL_ERROR_SETUP;
                    break;
                case ST_FAIL_SECURE:
                    iResult = PROTOSSL_ERROR_SECURE;
                    break;
                default:
                    iResult = PROTOSSL_ERROR_UNKNOWN;
                    break;
            }
        }
        else
        {
            iResult = 0;
        }
        return(iResult);
    }
    // return HRESULT containing either socker or ssl error
    if (iSelect == 'hres')
    {
        uint32_t uHresult;
        int32_t iSerr = ProtoSSLStat(pState, 'serr', NULL, 0);
        int32_t iFail = ProtoSSLStat(pState, 'fail', NULL, 0);

        if (iSerr < SOCKERR_CLOSED)
        {
            uHresult = DirtyErrGetHResult(DIRTYAPI_SOCKET, iSerr, TRUE);
        }
        else if (iFail != 0)
        {
            uHresult = DirtyErrGetHResult(DIRTYAPI_PROTO_SSL, iFail, TRUE);
        }
        else
        {
            uHresult = DirtyErrGetHResult(DIRTYAPI_PROTO_SSL, 0, FALSE);
        }
        return(uHresult);
    }
    // return configured max receive rate
    if (iSelect == 'maxr')
    {
        return(pState->iMaxRecvRate);
    }
    // return configured max send rate
    if (iSelect == 'maxs')
    {
        return(pState->iMaxSendRate);
    }
    // return whether session was resumed or not
    if ((iSelect == 'resu') && (pState->pSecure != NULL))
    {
        return(pState->pSecure->bSessionResume);
    }
    // return last socket error
    if (iSelect == 'serr')
    {
        // pass through to socket module if we have a socket, else return cached last error
        return((pState->pSock != NULL) ? SocketInfo(pState->pSock, iSelect, 0, pBuffer, iLength) : pState->iLastSocketError);
    }
    // return socket ref
    if (iSelect == 'sock')
    {
        if ((pBuffer == NULL) || (iLength != sizeof(pState->pSock)))
        {
            return(-1);
        }
        ds_memcpy(pBuffer, &pState->pSock, sizeof(pState->pSock));
        return(0);
    }
    // return current ssl version for the connection
    if ((iSelect == 'vers') && (pState->pSecure != NULL))
    {
        if (pBuffer != NULL)
        {
            ds_strnzcpy(pBuffer, _SSL3_strVersionNames[pState->pSecure->uSslVersion&0xff], iLength);
        }
        return(pState->pSecure->uSslVersion);
    }

    // only pass through if socket is valid
    if (pState->pSock != NULL)
    {
        // special processing for 'stat' selector
        if (iSelect == 'stat')
        {
            // if we're in a failure state, return error
            if (pState->iState >= ST_FAIL)
            {
               return(-1);
            }
            // don't check connected status until we are connected and done with secure negotiation (if secure)
            if (pState->iState < ST3_SECURE)
            {
               return(0);
            }
            // if we're connected (in ST_UNSECURE or ST3_SECURE state) fall through
        }

        // pass through request to the socket module
        iResult = SocketInfo(pState->pSock, iSelect, 0, pBuffer, iLength);
    }
    return(iResult);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLControl

    \Description
        ProtoSSL control function.  Different selectors control different behaviors.

    \Input *pState  - module state reference
    \Input iSelect  - control selector
    \Input iValue   - selector specific
    \Input iValue2  - selector specific
    \Input *pValue  - selector specific

    \Output
        int32_t     - selector specific

    \Notes
        Selectors are:

        \verbatim
            SELECTOR    DESCRIPTION
            'alpn'      Set the ALPN protocols (using IANA registerd named) as a comma delimited list
            'ciph'      Set enabled/disabled ciphers (PROTOSSL_CIPHER_*)
            'host'      Set remote host
            'maxr'      Set max recv rate (0=uncapped, default)
            'maxs'      Set max send rate (0=uncapped, default)
            'pass'      Set password for pkcs12 certifcate as currently required by SecureTransport
            'radr'      Set socket option for reuse address
            'rbuf'      Set socket recv buffer size (must be called before Connect())
            'resu'      Set whether session resume is enabled or disabled (default=1=enabled)
            'sbuf'      Set socket send buffer size (must be called before Connect())
            'scrt'      Set certificate in pkcs12 format (pValue=cert, iValue=len)
            'secu'      Start secure negotiation on an established unsecure connection
            'skep'      Set socket keep-alive settings (iValue=enable/disable, iValue2=keep-alive time/interval)
            'snod'      Set whether TCP_NODELAY option is enabled or disabled on the socket (must be called before Connect())
            'spam'      Set debug logging level (default=1)
            'vers'      Set client-requested SSL version (default=0x302, TLS1.1)
            'vmin'      Set minimum SSL version application will accept
        \endverbatim

    \Version 05/15/2017 (eesponda)
*/
/**************************************************************************F*/
int32_t ProtoSSLControl(ProtoSSLRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    int32_t iResult = -1;

    if (iSelect == 'alpn')
    {
        uint16_t uProtocol, uProtocol2;
        const char *pSrc;
        CFStringRef aAlpnProtocols[SSL_ALPN_MAX_PROTOCOLS];
        CFAllocatorRef pAllocator = _ProtoSSL_pState->pAllocator;

        if ((pSrc = (const char *)pValue) == NULL)
        {
            NetPrintf(("protosslapple: [%p] invalid ALPN extension protocol list provided\n", pState));
            return(-1);
        }
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] setting the ALPN extension protocols using %s\n", pState, pSrc));

        // parse the delimited list
        for (uProtocol = 0; uProtocol < SSL_ALPN_MAX_PROTOCOLS; uProtocol += 1)
        {
            char strName[256];
            if (ds_strsplit(pSrc, ',', strName, sizeof(strName), &pSrc) == 0)
            {
                break;
            }
            if ((aAlpnProtocols[uProtocol] = CFStringCreateWithCString(pAllocator, strName, kCFStringEncodingASCII)) == NULL)
            {
                break;
            }
        }

        // if we parsed any protocols, create the array for storage
        if (uProtocol > 0)
        {
            if (pState->pAlpnProtocols != NULL)
            {
                CFRelease(pState->pAlpnProtocols);
            }
            if ((pState->pAlpnProtocols = CFArrayCreate(pAllocator, (const void **)aAlpnProtocols, uProtocol, &kCFTypeArrayCallBacks)) == NULL)
            {
                NetPrintf(("protosslapple: [%p] unable to create array to hold alpn protocols\n", pState));
            }
        }

        // cleanup any temporaries allocated
        for (uProtocol2 = 0; uProtocol2 < uProtocol; uProtocol2 += 1)
        {
            CFRelease(aAlpnProtocols[uProtocol2]);
        }

        return(0);
    }
    if (iSelect == 'ciph')
    {
        pState->uEnabledCiphers = (uint32_t)iValue;
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] enabled ciphers=%d\n", pState, iValue));
        return(0);
    }
    if (iSelect == 'host')
    {
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] setting host to '%s'\n", pState, (const char *)pValue));
        ds_strnzcpy(pState->strHost, (const char *)pValue, sizeof(pState->strHost));
        return(0);
    }
    if (iSelect == 'maxr')
    {
        pState->iMaxRecvRate = iValue;
        if (pState->pSock != NULL)
        {
            SocketControl(pState->pSock, iSelect, iValue, NULL, NULL);
        }
        return(0);
    }
    if (iSelect == 'maxs')
    {
        pState->iMaxSendRate = iValue;
        if (pState->pSock != NULL)
        {
            SocketControl(pState->pSock, iSelect, iValue, NULL, NULL);
        }
        return(0);
    }
    if (iSelect == 'pass')
    {
        ds_strnzcpy(pState->strCertificateKey, (const char *)pValue, sizeof(pState->strCertificateKey));
        return(0);
    }
    if (iSelect == 'radr')
    {
        pState->bReuseAddr = TRUE;
        return(0);
    }
    if (iSelect == 'rbuf')
    {
        pState->iRecvBufSize = iValue;
        return(0);
    }
    if (iSelect == 'resu')
    {
        pState->bSessionResumeEnabled = iValue ? TRUE : FALSE;
        return(0);
    }
    if (iSelect == 'sbuf')
    {
        pState->iSendBufSize = iValue;
        return(0);
    }
    if (iSelect == 'scrt')
    {
        NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] setting cert (%d bytes)\n", pState, iValue));
        pState->pCertificate = (uint8_t *)pValue;
        pState->iCertificateLen = iValue;
        return(0);
    }
    if (iSelect == 'secu')
    {
        if (pState->iState != ST_UNSECURE)
        {
            NetPrintf(("protosslapple: [%p] cannot promote to a secure connection unless connected in unsecure state\n", pState));
            return(-1);
        }
        _ProtoSSLResetSecureState(pState, 1);
        pState->iState = ST3_SEND_HELLO;
        return(0);
    }
    if (iSelect == 'skep')
    {
        pState->bKeepAlive = (uint8_t)iValue;
        pState->uKeepAliveTime = (uint32_t)iValue2;
        return(0);
    }
    if (iSelect == 'snod')
    {
        pState->bNoDelay = (uint8_t)iValue;
        return(0);
    }
    if (iSelect == 'spam')
    {
        pState->iVerbose = (int8_t)iValue;
        return(0);
    }
    if (iSelect == 'vers')
    {
        uint32_t uSslVersion = DS_CLAMP(iValue, pState->uSslVersionMin, SSL3_VERSION_MAX);
        if (pState->uSslVersion != uSslVersion)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] setting sslvers to %s (0x%04x)\n", pState, _SSL3_strVersionNames[uSslVersion&0xff], uSslVersion));
            pState->uSslVersion = uSslVersion;
        }
        return(0);
    }
    if (iSelect == 'vmin')
    {
        uint32_t uSslVersionMin = DS_CLAMP(iValue, SSL3_VERSION_MIN, SSL3_VERSION_MAX);
        if (pState->uSslVersionMin != uSslVersionMin)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protosslapple: [%p] setting min sslvers to %s (0x%04x)\n", pState, _SSL3_strVersionNames[uSslVersionMin&0xff], uSslVersionMin));
            pState->uSslVersionMin = uSslVersionMin;
            // make sure requested version is at least minimum version
            ProtoSSLControl(pState, 'vers', pState->uSslVersion, 0, NULL);
        }
        return(0);
    }
    // if we have a socket ref, pass unhandled selector through
    if (pState->pSock != NULL)
    {
        iResult = SocketControl(pState->pSock, iSelect, iValue, pValue, NULL);
    }
    else
    {
        NetPrintf(("protosslapple: [%p] ProtoSSLControl('%C') unhandled\n", pState, iSelect));
    }
    return(iResult);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLSetCACert

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all ProtoSSL instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

    \Input *pCertData   - pointer to certificate data (PEM or DER)
    \Input iCertSize    - size of certificate data

    \Output
        int32_t         - negative=error, positive=count of CAs added

    \Notes
        The certificate must be in .DER (binary) or .PEM (base64-encoded)
        format.

    \Version 01/13/2009 (jbrookes)
*/
/**************************************************************************F*/
int32_t ProtoSSLSetCACert(const uint8_t *pCertData, int32_t iCertSize)
{
    return(_ProtoSSLSetCACert(pCertData, iCertSize));
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLSetCACert2

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all ProtoSSL instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

        This version of the function does not validate the CA at load time.
        The X509 certificate data will be copied and retained until the CA
        is validated, either by use of ProtoSSLValidateAllCA() or by the CA
        being used to validate a certificate.

    \Input *pCertData   - pointer to certificate data (PEM or DER)
    \Input iCertSize    - size of certificate data

    \Output
        int32_t         - negative=error, positive=count of CAs added

    \Notes
        The certificate must be in .DER (binary) or .PEM (base64-encoded)
        format.

    \Version 04/21/2011 (jbrookes)
*/
/**************************************************************************F*/
int32_t ProtoSSLSetCACert2(const uint8_t *pCertData, int32_t iCertSize)
{
    NetPrintf(("protosslapple: delayed validation is not supported, passing through to ProtoSSLSetCACert instead\n"));
    return(_ProtoSSLSetCACert(pCertData, iCertSize));
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLValidateAllCA

    \Description
        Validate all CA that have been added but not yet been validated.  Validation
        is a one-time process and disposes of the X509 certificate that is retained
        until validation.

    \Output
        int32_t         - zero on success; else the number of certs that could not be validated

    \Version 04/21/2011 (jbrookes)
*/
/**************************************************************************F*/
int32_t ProtoSSLValidateAllCA(void)
{
    return(0);
}

/*F*************************************************************************/
/*!
    \Function ProtoSSLClrCACerts

    \Description
        Clears all dynamic CA certs from the list.

    \Version 01/14/2009 (jbrookes)
*/
/**************************************************************************F*/
void ProtoSSLClrCACerts(void)
{
    int32_t iCert, iNumStatic;
    ProtoSSLStateT *pState = _ProtoSSL_pState;

    /* This code makes the following assumptions:
       1) There is at least one static cert.
       2) All static certs come first, followed by all dynamic certs. */

    if (pState == NULL)
    {
        NetPrintf(("protosslapple: warning, protossl global state not initialized, certificate store feature disabled\n"));
        return;
    }

    // set the index to the first dynamic slot
    iNumStatic = (signed)(sizeof(_ProtoSSL_aCerts)/sizeof(_ProtoSSL_aCerts[0]));

    // loop through dynamic certs releasing their memory
    for (iCert = iNumStatic; iCert < pState->iNumCerts; iCert += 1)
    {
        CFRelease(pState->pCertStore[iCert]);
        pState->pCertStore[iCert] = NULL;
    }
    pState->iNumCerts = iNumStatic;
}
