/*H********************************************************************************/
/*!
    \File netconnstadia.cpp

    \Description
        Provides network setup and teardown support. Does not actually create any
        kind of network connections.

    \Copyright
        Copyright (c) 2020 Electronic Arts Inc.

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ggp_c/authentication.h>
#include <ggp_c/feature_restriction.h>
#include <ggp_c/profile.h>
#include <ggp_c/version.h>

#include "IEAUser/IEAUser.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/crypt/crypthash.h"
#include "DirtySDK/crypt/cryptsha2.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtycert.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protohttp.h"
#include "DirtySDK/proto/protossl.h"
#include "DirtySDK/proto/protoupnp.h"
#include "DirtySDK/util/base64.h"
#include "DirtySDK/util/jsonparse.h"
#include "netconncommon.h"
#include "netconnlocaluser.h"

/*** Defines **********************************************************************/

//! UPNP port
#define NETCONN_DEFAULT_UPNP_PORT       (3659)

//! default ttl for jwt in seconds
#define NETCONN_TOKEN_EXPIRY            (3600 * 6)

//! allowable clock drift when checking expiration
#define NETCONN_TOKEN_CLOCKDRIFT        (60 * 10)

//! maximum number of keys we can support
#define NETCONN_MAX_JWK                 (16)

//! url to pull the openid configuration
#define NETCONN_OPENID_URL              ("https://stadia.google.com/.well-known/openid-configuration")

#define NETCONN_STADIAENV_DEV           ("DEV")         //!< development environment
#define NETCONN_STADIAENV_CERT          ("CERT")        //!< certification environment
#define NETCONN_STADIAENV_PROD          ("RELEASE")     //!< production environment

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! token acquisition state
typedef enum NetConnTokenStateE
{
    ST_TOKEN_INVALID = 0,   //!< token has not yet been requested
    ST_TOKEN_FAILED,        //!< token acquisition failed
    ST_TOKEN_INPROG,        //!< token acquisition in progress
    ST_TOKEN_VALID          //!< token valid
} NetConnTokenStateE;

//! the public key information for the jwt issuer to used for validation of the signature
typedef struct NetConnIssuerPublicKeyT
{
    char strAlg[32];        //!< algorithm this key is used for
    char strKeyId[128];     //!< identifier for the key used for lookup later
    uint8_t aModulus[256];  //!< modulus data of the rsa key
    uint8_t aExponent[3];   //!< exponent data of the rsa key
} NetConnIssuerPublicKeyT;

//! the format of the decoded jwt after it has been parsed
typedef struct NetConnJwtT
{
    struct
    {
        char strAlg[32];    //!< algorithm used for the signature (alg)
        char strType[32];   //!< the type of token (typ)
        char strKeyId[128]; //!< key id used to pull the public key from the openid config (kid)
    } Header;
    struct
    {
        char strIssuer[32];     //!< issuer of the jwt (iss)
        char strAudience[64];   //!< audience of the jwt (aud)
        char strSubject[32];    //!< subject of the jwt (sub)
        char strEnvironment[32];//!< environment associated with the jwt (s_env)
        char strAppId[64];      //!< application id associated wit the jwt (s_app_id)
        uint64_t uIssueTime;    //!< when the jwt was issued (iat)
        uint64_t uExpiryTime;   //!< when the jwt is set to expire (exp)
    } Payload;
    uint8_t aSignature[256];    //!< signature of the jwt (2048 bit rsa)
} NetConnJwtT;

typedef struct NetConnUserT
{
    GgpFuture ProfileFuture;        //!< future for profile request
    GgpProfile Profile;             //!< profile for the user
    bool bProfilePending;           //!< boolean to track validity of the future

    GgpFuture PlayerJwtFuture;      //!< future for jwt request
    GgpPlayerJwt *pPlayerJwt;       //!< player jwt (raw)
    int32_t iJwtSize;               //!< size of the jwt
    NetConnTokenStateE eJwtState;   //!< state of the player jwt
    NetConnJwtT Jwt;                //!< information parsed from jwt
    NetCritT Crit;                  //!< guard against multi-threaded access to processing of the user
} NetConnUserT;

//! private module state
typedef struct NetConnRefT
{
    NetConnCommonRefT Common;           //!< cross-platform netconn data (must come first!)
    NetConnLocalUserRefT *pLocalUser;   //!< local user container for IEAUser
    const EA::User::IEAUser *aEAUsers[NETCONN_MAXLOCALUSERS]; //!< EAUsers known by NetConn - populated when pIEAUserEventList is processed
    NetConnUserT aUsers[NETCONN_MAXLOCALUSERS]; //!< netconn specific data for the users

    enum
    {
        ST_INIT,                        //!< initialization
        ST_CONN,                        //!< bringing up network interface
        ST_IDLE,                        //!< active
    } eState;                           //!< internal connection state

    uint32_t uPlatEnv;                  //!< our platform environment parsed from the primary user's jwt
    uint32_t uConnStatus;               //!< connection status (surfaced to user)

    ProtoHttpRefT *pHttp;               //!< protohttp module state
    char strRecvBuf[16*1024];           //!< buffer for receiving http data
    NetConnIssuerPublicKeyT aKeys[NETCONN_MAX_JWK]; //!< keys used for validation of jwts
    int32_t iNumJwks;                   //!< number of keys currently parsed

    ProtoUpnpRefT *pProtoUpnp;          //!< protoupnp module state
    int32_t iPeerPort;                  //!< peer port to be opened by upnp; if zero, still find upnp router but don't open a port
    int32_t iNumProcCores;              //!< number of processor cores on the system
    int32_t iThreadCpuAffinity;         //!< cpu affinity used for our internal threads
} NetConnRefT;

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

//! global module ref
static NetConnRefT *_NetConn_pRef = NULL;

//! global sign CA needed to contact the googleapis
static const char _aGlobalSign[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n"
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n"
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n"
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n"
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n"
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n"
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n"
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n"
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n"
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n"
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n"
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n"
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n"
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n"
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n"
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n"
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n"
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n"
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n"
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n"
"-----END CERTIFICATE-----";

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdateConnStatus

    \Description
        Update the Connection Status and provide logging of status changes

    \Input *pRef             - pointer to net NetConn module ref
    \Input *uNewConnStatus   - the new conn status

    \Version 01/19/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnStatus(NetConnRefT *pRef, uint32_t uNewConnStatus)
{
    NetPrintf(("netconnstadia: netconn status changed to %C\n", uNewConnStatus));
    pRef->uConnStatus = uNewConnStatus;
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetProcLine

    \Description
        Parse a single line of the processor entry file.

    \Input *pLine   - pointer to line to parse
    \Input *pName   - [out] buffer to store parsed field name
    \Input iNameLen - size of name buffer
    \Input *pData   - [out] buffer to store parsed field data
    \Input iDataLen - size of data buffer

    \Version 04/26/2010 (jbrookes)
*/
/********************************************************************************F*/
static void _NetConnGetProcLine(const char *pLine, char *pName, int32_t iNameLen, char *pData, int32_t iDataLen)
{
    const char *pParse;

    // skip to end of field name
    for (pParse = pLine;  (*pParse != '\t') && (*pParse != ':'); pParse += 1)
        ;

    // copy field name
    ds_strsubzcpy(pName, iNameLen, pLine, pParse - pLine);

    // skip to field value
    for ( ; (*pParse == ' ') || (*pParse == '\t') || (*pParse == ':'); pParse += 1)
        ;

    // find end of field value
    for (pLine = pParse; (*pParse != '\n') && (*pParse != '\0'); pParse += 1)
        ;

    // copy field value
    ds_strsubzcpy(pData, iDataLen, pLine, pParse - pLine);
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetProcRecord

    \Description
        Parse a single proc record.

    \Input *pProcFile   - proc record file pointer

    \Output
        int32_t         - one=success, zero=no more entries

    \Version 04/26/2010 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _NetConnGetProcRecord(FILE *pProcFile)
{
    char strBuf[1024], strName[32], strValue[128], *pLine;
    while (((pLine = fgets(strBuf, sizeof(strBuf), pProcFile)) != NULL) && (strBuf[0] != '\n'))
    {
        _NetConnGetProcLine(strBuf, strName, sizeof(strName), strValue, sizeof(strValue));
    }
    return(pLine != NULL);
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetNumProcs

    \Description
        Get the number of processors on the system.

    \Output
        int32_t     - number of processors in system, or <=0 on failure

    \Version 04/26/2010 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _NetConnGetNumProcs(void)
{
    FILE *pFile = fopen("/proc/cpuinfo", "r");
    int32_t iNumProcs = -1;

    if (pFile != NULL)
    {
        for (iNumProcs = 0; _NetConnGetProcRecord(pFile) != 0; iNumProcs += 1)
            ;
        fclose(pFile);

        NetPrintf(("netconnstadia: parsed %d processor cores from cpuinfo\n", iNumProcs));
    }
    else
    {
        NetPrintf(("netconnstadia: could not open proc file\n"));
    }
    return(iNumProcs);
}

/*F********************************************************************************/
/*!
    \Function _NetConnRequestAuthToken

    \Description
        Request a JWT token for the player at the provided index

    \Input *pRef            - module state
    \Input iLocalUserIndex  - index of the user we are getting the JWT for

    \Output
        int32_t             - 0=in-progress already, 1=success

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _NetConnRequestAuthToken(NetConnRefT *pRef, int32_t iLocalUserIndex)
{
    NetConnUserT *pUser = &pRef->aUsers[iLocalUserIndex];
    if (pUser->eJwtState == ST_TOKEN_INPROG)
    {
        NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnstadia: ignoring request for new token as we are already in progress\n"));
        return(0);
    }

    // kick off the request to get the user's jwt
    pUser->PlayerJwtFuture = GgpGetJwtForPlayer(pRef->aEAUsers[iLocalUserIndex]->GetUserID(), NETCONN_TOKEN_EXPIRY, 0);
    pUser->eJwtState = ST_TOKEN_INPROG;
    NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnstadia: new token request for user = %llu\n", pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
    return(1);
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetJwtPublicKey

    \Description
        Retrieve a public key from the ones provided by google

    \Input *pRef                        - module state
    \Input *pKeyId                      - identifier for the key
    \Input *pAlg                        - key algorithm

    \Output
        const NetConnIssuerPublicKeyT * - NULL=key not found, non-NULL=found key

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static const NetConnIssuerPublicKeyT *_NetConnGetJwtPublicKey(const NetConnRefT *pRef, const char *pKeyId, const char *pAlg)
{
    const NetConnIssuerPublicKeyT *pResult = NULL;

    for (int32_t iKeyIndex = 0; iKeyIndex < pRef->iNumJwks; iKeyIndex += 1)
    {
        const NetConnIssuerPublicKeyT *pKey = &pRef->aKeys[iKeyIndex];
        if ((strncmp(pKey->strKeyId, pKeyId, sizeof(pKey->strKeyId)) == 0) && (strncmp(pKey->strAlg, pAlg, sizeof(pKey->strAlg)) == 0))
        {
            pResult = pKey;
            break;
        }
    }
    return(pResult);
}

/*F********************************************************************************/
/*!
    \Function _NetConnParseJwt

    \Description
        When we retrieve a JWT, this function will parse the base64 encoded data
        and validate its signature.

    \Input *pRef    - module state
    \Input *pInput  - jwt token
    \Input *pOutput - structure to write the parsed data to

    \Output
        int32_t     - zero=success, negative=failure

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _NetConnParseJwt(const NetConnRefT *pRef, const char *pInput, NetConnJwtT *pOutput)
{
    const NetConnIssuerPublicKeyT *pPublicKey;
    const char *pHeader, *pPayload, *pSignature;
    CryptSha2T Sha2;
    uint8_t aHashObj[CRYPTSHA256_HASHSIZE];
    char *pDecodeBuf;
    int32_t iDecodeSize, iResult;
    uint16_t *pJsonParseBuf;
    #if DIRTYCODE_LOGGING
    char strTimeBuf[64];
    #endif

    // assign the header to the beginning
    pHeader = pInput;
    // find the payload part
    if ((pPayload = strchr(pHeader, '.')) != NULL)
    {
        pPayload += 1;
    }
    else
    {
        NetPrintf(("netconnstadia: could not find the payload portion of the jwt\n"));
        return(-1);
    }
    // find the signature part
    if ((pSignature = strchr(pPayload, '.')) != NULL)
    {
        pSignature += 1;
    }
    else
    {
        NetPrintf(("netconnstadia: could not find the signature portion of the jwt\n"));
        return(-1);
    }

    // hash the header + payload (not included the '.' delimiter)
    CryptSha2Init(&Sha2, sizeof(aHashObj));
    CryptSha2Update(&Sha2, (const uint8_t *)pHeader, (int32_t)(pSignature - pHeader - 1));
    CryptSha2Final(&Sha2, aHashObj, sizeof(aHashObj));

    // base64url decode the header
    iDecodeSize = Base64DecodedSize((int32_t)(pPayload - pHeader - 1)) + 1;
    if ((pDecodeBuf = (char *)DirtyMemAlloc(iDecodeSize, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData)) == NULL)
    {
        NetPrintf(("netconnstadia: could not allocate a buffer for the decode base64 data\n"));
        return(-1);
    }
    ds_memclr(pDecodeBuf, iDecodeSize);
    // decode the header from base64 to json text
    if ((iResult = Base64DecodeUrl(pHeader, (int32_t)(pPayload - pHeader - 1), pDecodeBuf, iDecodeSize)) == 0)
    {
        DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
        NetPrintf(("netconnstadia: failed to decode the jwt header (%d)\n", iResult));
        return(-1);
    }
    // parse the header json
    pJsonParseBuf = JsonParse2(pDecodeBuf, iDecodeSize, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    if (pJsonParseBuf == NULL)
    {
        NetPrintf(("netconnstadia: failed to allocate for the jwt header parse table\n"));
        DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
        return(-1);
    }
    JsonGetString(JsonFind(pJsonParseBuf, "alg"), pOutput->Header.strAlg, sizeof(pOutput->Header.strAlg), "");
    JsonGetString(JsonFind(pJsonParseBuf, "typ"), pOutput->Header.strType, sizeof(pOutput->Header.strType), "");
    JsonGetString(JsonFind(pJsonParseBuf, "kid"), pOutput->Header.strKeyId, sizeof(pOutput->Header.strKeyId), "");
    DirtyMemFree(pJsonParseBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    // validate the header contents
    if ((strncmp(pOutput->Header.strAlg, "RS256", 5) != 0) || (strncmp(pOutput->Header.strType, "JWT", 3) != 0))
    {
        NetPrintf(("netconnstadia: the header did match the expected contents with (alg=%s, typ=%s)\n",
            pOutput->Header.strAlg, pOutput->Header.strType));
        return(-1);
    }
    // try to find the key in one that we pulled earlier in the boot flow from jwks_uri
    if ((pPublicKey = _NetConnGetJwtPublicKey(pRef, pOutput->Header.strKeyId, pOutput->Header.strAlg)) == NULL)
    {
        NetPrintf(("netconnstadia: could not find key matching id=%s\n", pOutput->Header.strKeyId));
        return(-1);
    }

    // base64url decode the payload
    iDecodeSize = Base64DecodedSize((int32_t)(pSignature - pPayload - 1)) + 1;
    if ((pDecodeBuf = (char *)DirtyMemAlloc(iDecodeSize, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData)) == NULL)
    {
        NetPrintf(("netconnstadia: could not allocate a buffer for the decode base64 data\n"));
        return(-1);
    }
    ds_memclr(pDecodeBuf, iDecodeSize);
    // decode the payload from base64 to json text
    if ((iResult = Base64DecodeUrl(pPayload, (int32_t)(pSignature - pPayload - 1), pDecodeBuf, iDecodeSize)) == 0)
    {
        DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
        NetPrintf(("netconnstadia: failed to decode the jwt payload (%d/%d)\n", iResult, iDecodeSize));
        return(-1);
    }
    // parse the payload json
    pJsonParseBuf = JsonParse2(pDecodeBuf, iDecodeSize, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    if (pJsonParseBuf == NULL)
    {
        NetPrintf(("netconnstadia: failed to allocate for the jwt payload parse table\n"));
        DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
        return(-1);
    }
    // standard jwt fields
    JsonGetString(JsonFind(pJsonParseBuf, "iss"), pOutput->Payload.strIssuer, sizeof(pOutput->Payload.strIssuer), "");
    JsonGetString(JsonFind(pJsonParseBuf, "aud"), pOutput->Payload.strAudience, sizeof(pOutput->Payload.strAudience), "");
    JsonGetString(JsonFind(pJsonParseBuf, "sub"), pOutput->Payload.strSubject, sizeof(pOutput->Payload.strSubject), "");
    pOutput->Payload.uIssueTime = JsonGetInteger(JsonFind(pJsonParseBuf, "iat"), 0);
    pOutput->Payload.uExpiryTime = JsonGetInteger(JsonFind(pJsonParseBuf, "exp"), 0);
    // custom jwt fields for google
    JsonGetString(JsonFind(pJsonParseBuf, "s_env"), pOutput->Payload.strEnvironment, sizeof(pOutput->Payload.strEnvironment), "");
    JsonGetString(JsonFind(pJsonParseBuf, "s_app_id"), pOutput->Payload.strAppId, sizeof(pOutput->Payload.strAppId), "");
    DirtyMemFree(pJsonParseBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    DirtyMemFree(pDecodeBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    // validate the payload contents
    if (difftime(time(NULL) - NETCONN_TOKEN_CLOCKDRIFT, pOutput->Payload.uExpiryTime) > 0)
    {
        NetPrintf(("netconnstadia: token validation failed due to expiration\n"));
        return(-1);
    }

    // base64url decode the signature
    if ((iResult = Base64DecodeUrl(pSignature, strlen(pSignature), (char *)pOutput->aSignature, sizeof(pOutput->aSignature))) == 0)
    {
        NetPrintf(("netconnstadia: failed to decode the jwt signature (%d/%d)\n", iResult, iDecodeSize));
        iResult = -1;
    }
    else if ((iResult = ProtoSSLPkcs1Verify(pOutput->aSignature, sizeof(pOutput->aSignature), aHashObj, sizeof(aHashObj), CRYPTHASH_SHA256, pPublicKey->aModulus, sizeof(pPublicKey->aModulus), pPublicKey->aExponent, sizeof(pPublicKey->aExponent))) == 0)
    {
        NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnstadia: validation of jwt succeeded, valid until %s\n",
                ds_secstostr(pOutput->Payload.uExpiryTime, TIMETOSTRING_CONVERSION_RFC_0822, FALSE, strTimeBuf, sizeof(strTimeBuf))));
    }
    else
    {
        NetPrintf(("netconnstadia: validation of signature failed\n"));
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateAuthRequests

    \Description
        Updates the pending auth requests for jwt tokens

    \Input *pRef    - module state

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateAuthRequests(NetConnRefT *pRef)
{
    for (int32_t iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        GgpStatus Status;
        int64_t iJwtSize;
        char strError[256];
        NetConnUserT *pUser = &pRef->aUsers[iLocalUserIndex];

        // acquire the lock
        if (!NetCritTry(&pUser->Crit))
        {
            continue;
        }

        // skip over requests not in progress
        if (pUser->eJwtState != ST_TOKEN_INPROG)
        {
            NetCritLeave(&pUser->Crit);
            continue;
        }

        // check to make sure the future will not block
        if (!GgpFutureIsReady(pUser->PlayerJwtFuture))
        {
            NetCritLeave(&pUser->Crit);
            continue;
        }
        NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnstadia: token request future for user %llu ready to be processed\n", pRef->aEAUsers[iLocalUserIndex]->GetUserID()));

        // get the result size for allocation
        iJwtSize = GgpFutureGetResultSize(pUser->PlayerJwtFuture, &Status);
        if (Status.status_code != kGgpStatusCode_Ok)
        {
            GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
            NetPrintf(("netconnstadia: getting the jwt result size failed with %d (msg=%s)\n", Status.status_code, strError));
            pUser->eJwtState = ST_TOKEN_FAILED;
            NetCritLeave(&pUser->Crit);
            continue;
        }

        // free any allocated memory from a previous request
        if (pUser->pPlayerJwt != NULL)
        {
            DirtyMemFree(pUser->pPlayerJwt, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
            pUser->pPlayerJwt = NULL;
        }

        // allocate memory for the result
        if ((pUser->pPlayerJwt = (GgpPlayerJwt *)DirtyMemAlloc((int32_t)iJwtSize, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData)) == NULL)
        {
            NetPrintf(("netconnstadia: failed to allocate space for the player's jwt token\n"));
            pUser->eJwtState = ST_TOKEN_FAILED;
            NetCritLeave(&pUser->Crit);
            continue;
        }
        pUser->iJwtSize = (int32_t)iJwtSize;

        // get result
        if (!GgpFutureGetResult(pUser->PlayerJwtFuture, pUser->pPlayerJwt, iJwtSize, &Status))
        {
            GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
            NetPrintf(("netconnstadia: getting the jwt result failed with %d (msg=%s)\n", Status.status_code, strError));
            DirtyMemFree(pUser->pPlayerJwt, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
            pUser->pPlayerJwt = NULL;
            pUser->eJwtState = ST_TOKEN_FAILED;
            NetCritLeave(&pUser->Crit);
            continue;
        }

        #if DIRTYCODE_LOGGING
        if (pRef->Common.iDebugLevel > 1)
        {
            NetPrintf(("netconnstadia: received jwt:\n"));
            NetPrintWrap(pUser->pPlayerJwt->jwt, 132);
        }
        #endif

        // process the jwt that we received
        pUser->eJwtState = (_NetConnParseJwt(pRef, pUser->pPlayerJwt->jwt, &pUser->Jwt) == 0) ? ST_TOKEN_VALID : ST_TOKEN_FAILED;

        // release the lock
        NetCritLeave(&pUser->Crit);
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateProfileRequests

    \Description
        Updates the pending profile requests

    \Input *pRef    - module state

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateProfileRequests(NetConnRefT *pRef)
{
    for (int32_t iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        GgpStatus Status;
        NetConnUserT *pUser = &pRef->aUsers[iLocalUserIndex];

        // acquire the lock
        if (!NetCritTry(&pUser->Crit))
        {
            continue;
        }

        // check to make sure the future is valid and will not block
        if (!pUser->bProfilePending || !GgpFutureIsReady(pUser->ProfileFuture))
        {
            NetCritLeave(&pUser->Crit);
            continue;
        }
        pUser->bProfilePending = false;

        // get profile result
        if (!GgpFutureGetResult(pUser->ProfileFuture, &pUser->Profile, sizeof(pUser->Profile), &Status))
        {
            char strError[256];

            GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
            NetPrintf(("netconnstadia: getting the profile result failed with %d (msg=%s)\n", Status.status_code, strError));
        }

        // release the lock
        NetCritLeave(&pUser->Crit);
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnParseJwkUri

    \Description
        Parse the "jwks_uri" from the openid configuration json response

    \Input *pRef    - module state
    \Input *pInpBuf - json response we are parsing
    \Input iInpLen  - length of the response
    \Input *pOutBuf - output buffer for the uri
    \Input iOutLen  - maximum length of the output buffer

    \Output
        const char *- pointer to the output buffer

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static const char *_NetConnUpdateConnParseJwkUri(NetConnRefT *pRef, const char *pInpBuf, int32_t iInpLen, char *pOutBuf, int32_t iOutLen)
{
    uint16_t *pJsonParseBuf;
    if ((pJsonParseBuf = JsonParse2(pInpBuf, iInpLen, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData)) == NULL)
    {
        NetPrintf(("netconnstadia: could not create json parse table for openid configuration response\n"));
        ds_memclr(pOutBuf, iOutLen);
        return(pOutBuf);
    }
    JsonGetString(JsonFind(pJsonParseBuf, "jwks_uri"), pOutBuf, iOutLen, "");
    DirtyMemFree(pJsonParseBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    return(pOutBuf);
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnParseKeys

    \Description
        Parses the public keys from the jwks_uri response

    \Input *pRef        - module state
    \Input *pInpBuf     - json response we are parsing
    \Input iInpLen      - length of the response
    \Input *pPublicKeys - array we are writing to
    \Input iMaxKeys     - maximum amount of keys we support

    \Output
        int32_t         - number of keys parsed

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _NetConnUpdateConnParseKeys(NetConnRefT *pRef, const char *pInpBuf, int32_t iInpLen, NetConnIssuerPublicKeyT *pPublicKeys, int32_t iMaxKeys)
{
    uint16_t *pJsonParseBuf;
    const char *pCurrent;
    int32_t iNumJwks = 0, iIndex = 0;
    if ((pJsonParseBuf = JsonParse2(pInpBuf, iInpLen, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData)) == NULL)
    {
        NetPrintf(("netconnstadia: could not parse the json for keys\n"));
        return(0);
    }
    while ((pCurrent = JsonFind2(pJsonParseBuf, NULL, "keys[", iIndex)) != NULL)
    {
        char strBase64[512];
        bool bError = false;
        NetConnIssuerPublicKeyT *pPublicKey = &pPublicKeys[iNumJwks];

        JsonGetString(JsonFind2(pJsonParseBuf, pCurrent, ".alg", iIndex), pPublicKey->strAlg, sizeof(pPublicKey->strAlg), "");
        JsonGetString(JsonFind2(pJsonParseBuf, pCurrent, ".kid", iIndex), pPublicKey->strKeyId, sizeof(pPublicKey->strKeyId), "");
        JsonGetString(JsonFind2(pJsonParseBuf, pCurrent, ".n", iIndex), strBase64, sizeof(strBase64), "");
        if (Base64DecodeUrl(strBase64, strlen(strBase64), (char *)pPublicKey->aModulus, sizeof(pPublicKey->aModulus)) == 0)
        {
            NetPrintf(("netconnstadia: failed to parse modulus data for kid=%s\n", pPublicKey->strKeyId));
            bError = true;
        }
        JsonGetString(JsonFind2(pJsonParseBuf, pCurrent, ".e", iIndex), strBase64, sizeof(strBase64), "");
        if (Base64DecodeUrl(strBase64, strlen(strBase64), (char *)pPublicKey->aExponent, sizeof(pPublicKey->aExponent)) == 0)
        {
            NetPrintf(("netconnstadia: failed to parse exponent data for kid=%s\n", pPublicKey->strKeyId));
            bError = true;
        }

        // if not error then move to next one
        if (!bError)
        {
            iNumJwks += 1;
        }

        iIndex += 1;
        // if we are over the max number of keys break out
        if (iIndex == iMaxKeys)
        {
            break;
        }
    }
    DirtyMemFree(pJsonParseBuf, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
    return(iNumJwks);
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnInitial

    \Description
        Handles the initial transition into the connection state and figures out
        which state to move to next

    \Input *pRef    - module state

    \Version 03/27/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnInitial(NetConnRefT *pRef)
{
    // if we have already parsed the the json web keys then move straight into figure out our environment
    if (pRef->iNumJwks > 0)
    {
        int32_t iLocalUserIndex;
        if ((iLocalUserIndex = NetConnQuery(NULL, NULL, 0)) < 0)
        {
            _NetConnUpdateConnStatus(pRef, '-act');
        }
        else
        {
            _NetConnRequestAuthToken(pRef, iLocalUserIndex);
            _NetConnUpdateConnStatus(pRef, '~env');
        }
    }
    // otherwise, start by pulling out the openid configuration
    else
    {
        if (ProtoHttpRequest(pRef->pHttp, NETCONN_OPENID_URL, NULL, 0, PROTOHTTP_REQUESTTYPE_GET) == 0)
        {
            _NetConnUpdateConnStatus(pRef, '~oid');
        }
        else
        {
            _NetConnUpdateConnStatus(pRef, '-err');
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnOpenID

    \Description
        Process the openid configuration request and make the request for the json
        web keys when complete

    \Input *pRef    - module state

    \Version 03/27/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnOpenID(NetConnRefT *pRef)
{
    int32_t iRecvResult;
    char strJwkUri[256];

    if ((iRecvResult = ProtoHttpRecvAll(pRef->pHttp, pRef->strRecvBuf, sizeof(pRef->strRecvBuf))) < 0)
    {
        if (iRecvResult != PROTOHTTP_RECVWAIT)
        {
            _NetConnUpdateConnStatus(pRef, '-err');
        }
        return;
    }

    // parse the jwk uris out and request it
    if (ProtoHttpRequest(pRef->pHttp, _NetConnUpdateConnParseJwkUri(pRef, pRef->strRecvBuf, iRecvResult, strJwkUri, sizeof(strJwkUri)), NULL, 0, PROTOHTTP_REQUESTTYPE_GET) == 0)
    {
       _NetConnUpdateConnStatus(pRef, '~jwk');
    }
    else
    {
        _NetConnUpdateConnStatus(pRef, '-jwk');
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnJwk

    \Description
        Process the json web keys request and kick off environment determination

    \Input *pRef    - module state

    \Version 03/27/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnJwk(NetConnRefT *pRef)
{
    int32_t iRecvResult, iLocalUserIndex;

    if ((iRecvResult = ProtoHttpRecvAll(pRef->pHttp, pRef->strRecvBuf, sizeof(pRef->strRecvBuf))) < 0)
    {
        if (iRecvResult != PROTOHTTP_RECVWAIT)
        {
            _NetConnUpdateConnStatus(pRef, '-err');
        }
        return;
    }

    // parse the keys to be used for jwt token validation
    pRef->iNumJwks = _NetConnUpdateConnParseKeys(pRef, pRef->strRecvBuf, iRecvResult, pRef->aKeys, NETCONN_MAX_JWK);

    // get the primary local index to use for pulling a jwt (which contains our environment information)
    if ((iLocalUserIndex = NetConnQuery(NULL, NULL, 0)) < 0)
    {
        _NetConnUpdateConnStatus(pRef, '-act');
    }
    else
    {
        _NetConnRequestAuthToken(pRef, iLocalUserIndex);
        _NetConnUpdateConnStatus(pRef, '~env');
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdateConnEnvironment

    \Description
        Process the environment determination using the data from the user's
        json web token

    \Input *pRef    - module state

    \Version 03/27/2020 (eesponda)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnEnvironment(NetConnRefT *pRef)
{
    int32_t iLocalUserIndex;
    NetConnUserT *pUser;

    // get the primary local index to use for pulling a jwt (which contains our environment information)
    if ((iLocalUserIndex = NetConnQuery(NULL, NULL, 0)) < 0)
    {
        _NetConnUpdateConnStatus(pRef, '-act');
        return;
    }
    pUser = &pRef->aUsers[iLocalUserIndex];

    // check the token status
    if (pUser->eJwtState == ST_TOKEN_VALID)
    {
        if (strncmp(pUser->Jwt.Payload.strEnvironment, NETCONN_STADIAENV_DEV, strlen(NETCONN_STADIAENV_DEV)) == 0)
        {
            pRef->uPlatEnv = NETCONN_PLATENV_TEST;
            NetPrintfVerbose((pRef->Common.iDebugLevel, 0, "netconnstadia: platform environment set to NETCONN_PLATENV_TEST\n"));
        }
        else if (strncmp(pUser->Jwt.Payload.strEnvironment, NETCONN_STADIAENV_CERT, strlen(NETCONN_STADIAENV_CERT)) == 0)
        {
            pRef->uPlatEnv = NETCONN_PLATENV_CERT;
            NetPrintfVerbose((pRef->Common.iDebugLevel, 0, "netconnstadia: platform environment set to NETCONN_PLATENV_CERT\n"));
        }
        else if (strncmp(pUser->Jwt.Payload.strEnvironment, NETCONN_STADIAENV_PROD, strlen(NETCONN_STADIAENV_PROD)) == 0)
        {
            pRef->uPlatEnv = NETCONN_PLATENV_PROD;
            NetPrintfVerbose((pRef->Common.iDebugLevel, 0, "netconnstadia: platform environment set to NETCONN_PLATENV_PROD\n"));
        }
        else
        {
            NetPrintf(("netconnstadia: unrecognized environment %s defaulting to NETCONN_PLATENV_PROD\n", pUser->Jwt.Payload.strEnvironment));
        }

        _NetConnUpdateConnStatus(pRef, '+onl');
    }
    else if (pUser->eJwtState == ST_TOKEN_FAILED)
    {
        _NetConnUpdateConnStatus(pRef, '-err');
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdate

    \Description
        Update status of NetConn module.  This function is called by NetConnIdle.

    \Input *pData   - pointer to NetConn module ref
    \Input uTick    - current tick counter

    \Version 07/18/2006 (jbrookes)
*/
/********************************************************************************F*/
static void _NetConnUpdate(void *pData, uint32_t uTick)
{
    NetConnRefT *pRef = (NetConnRefT *)pData;

    // perform idle processing
    SocketControl(NULL, 'idle', uTick, NULL, NULL);

    // update local users list
    NetConnLocalUserUpdate(pRef->pLocalUser);

    // update protohttp ref
    ProtoHttpUpdate(pRef->pHttp);

    // check the auth requests
    _NetConnUpdateAuthRequests(pRef);

    // check for profile requests
    _NetConnUpdateProfileRequests(pRef);

    // wait for network active status
    if (pRef->eState == NetConnRefT::ST_CONN)
    {
        uint32_t uSocketConnStatus = SocketInfo(NULL, 'conn', 0, NULL, 0);

        // when we reach +onl at the dirtynet level process our netconn level conn states
        if (uSocketConnStatus == '+onl')
        {
            // when we are in the conn state, we pull the openid configuration (if this has not been done yet)
            if (pRef->uConnStatus == '~con')
            {
                _NetConnUpdateConnInitial(pRef);
            }
            // when we finish pulling the openid configuration, we pull the keys from jwks_uri
            else if (pRef->uConnStatus == '~oid')
            {
                _NetConnUpdateConnOpenID(pRef);
            }
            // when we pulling the keys from the jwks_uri, we pull the jwt from the primary user to determine environment
            else if (pRef->uConnStatus == '~jwk')
            {
                _NetConnUpdateConnJwk(pRef);
            }
            // after we determine environment, we move to the +onl state
            else if (pRef->uConnStatus == '~env')
            {
                _NetConnUpdateConnEnvironment(pRef);
            }
        }

        if (pRef->uConnStatus == '+onl')
        {
            // discover upnp router information
            if (pRef->pProtoUpnp != NULL)
            {
                if (pRef->iPeerPort != 0)
                {
                    ProtoUpnpControl(pRef->pProtoUpnp, 'port', pRef->iPeerPort, 0, NULL);
                    ProtoUpnpControl(pRef->pProtoUpnp, 'macr', 'upnp', 0, NULL);
                }
                else
                {
                    ProtoUpnpControl(pRef->pProtoUpnp, 'macr', 'dscg', 0, NULL);
                }
            }

            pRef->eState = NetConnRefT::ST_IDLE;
        }
    }

    // update connection status while idle
    if (pRef->eState == NetConnRefT::ST_IDLE)
    {
        // update connection status if not already in an error state
        if ((pRef->uConnStatus >> 24) != '-')
        {
            uint32_t uSocketConnStat = SocketInfo(NULL, 'conn', 0, NULL, 0);

            if (pRef->uConnStatus != uSocketConnStat)
            {
                 _NetConnUpdateConnStatus(pRef, uSocketConnStat);
            }
        }
    }

    // if error status, go to idle state from any other state
    if ((pRef->eState != NetConnRefT::ST_IDLE) && (pRef->uConnStatus >> 24 == '-'))
    {
        pRef->eState = NetConnRefT::ST_IDLE;
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetConnShutdownInternal

    \Description
        Shutdown the network code and return to idle state for internal use

    \Input  pRef              - netconn ref
    \Input  uShutdownFlags    - shutdown configuration flags

    \Output
        int32_t        - negative=error, else zero

    \Version 1/13/2020 (tcho)
*/
/********************************************************************************F*/
int32_t _NetConnShutdownInternal(NetConnRefT *pRef, uint32_t uShutdownFlags)
{
    int32_t iResult = 0, iLocalUserIndex;

    // decrement and check the refcount
    if ((iResult = NetConnCommonCheckRef((NetConnCommonRefT*)pRef)) < 0)
    {
        return(iResult);
    }

    // destroy the upnp ref
    if (pRef->pProtoUpnp != NULL)
    {
        ProtoUpnpDestroy(pRef->pProtoUpnp);
        pRef->pProtoUpnp = NULL;
    }

    // destroy the dirtycert module
    DirtyCertDestroy();

    // destroy the protohttp module we used internally
    ProtoHttpDestroy(pRef->pHttp);

    // shut down protossl
    ProtoSSLClrCACerts();
    ProtoSSLShutdown();

    // remove netconn idle task
    NetConnIdleDel(_NetConnUpdate, pRef);

    // shut down Idle handler
    NetConnIdleShutdown();

    // cleanup memory from the user state
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        NetConnUserT *pUser = &pRef->aUsers[iLocalUserIndex];
        if (pUser->pPlayerJwt != NULL)
        {
            DirtyMemFree(pUser->pPlayerJwt, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
            pUser->pPlayerJwt = NULL;
        }

        NetCritKill(&pUser->Crit);
    }

    // free the local users
    if (pRef->pLocalUser != NULL)
    {
        NetConnLocalUserDestroy(pRef->pLocalUser);
    }

    // shutdown the network code
    SocketDestroy(0);

    // common shutdown (must come last as this frees the memory)
    NetConnCommonShutdown(&pRef->Common);
    _NetConn_pRef = NULL;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnAddLocalUser

    \Description
        Add a local user

    \Input *pLocalUserRef   - local user module reference
    \Input iLocalUserIndex  - local user index
    \Input *pIEAUser        - pointer to IEAUser object

    \Version 05/16/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnAddLocalUser(NetConnLocalUserRefT *pLocalUserRef, int32_t iLocalUserIndex, const EA::User::IEAUser *pIEAUser)
{
    NetConnRefT *pRef = (NetConnRefT *)pLocalUserRef->pNetConn;

    if (pRef->aEAUsers[iLocalUserIndex] == NULL)
    {
        pRef->aEAUsers[iLocalUserIndex] = pIEAUser;
        pRef->aEAUsers[iLocalUserIndex]->AddRef();

        NetPrintf(("netconnstadia: netconn user added at local user index %d - localUserId=0x%08x\n",
            iLocalUserIndex, pIEAUser->GetUserID()));

        // request the profile information which we use for NetConnQuery
        pRef->aUsers[iLocalUserIndex].Profile.player_id = pIEAUser->GetUserID();
        pRef->aUsers[iLocalUserIndex].ProfileFuture = GgpGetProfile(pIEAUser->GetUserID());
        pRef->aUsers[iLocalUserIndex].bProfilePending = true;
    }
    else
    {
        NetPrintf(("netconnstadia: failed to add EAUser at index %d with local user id 0x%08x because entry already used with local user id 0x%08x\n",
            iLocalUserIndex, pIEAUser->GetUserID(), pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRemoveLocalUser

    \Description
        Remove a local user

    \Input *pLocalUserRef   - local user module reference
    \Input iLocalUserIndex  - local user index
    \Input *pIEAUser        - pointer to IEAUser object

    \Version 05/16/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnRemoveLocalUser(NetConnLocalUserRefT *pLocalUserRef, int32_t iLocalUserIndex, const EA::User::IEAUser *pIEAUser)
{
    NetConnRefT *pRef = (NetConnRefT *)pLocalUserRef->pNetConn;

    if (pRef->aEAUsers[iLocalUserIndex] != NULL)
    {
        if (pIEAUser->GetUserID() == pRef->aEAUsers[iLocalUserIndex]->GetUserID())
        {
            NetPrintf(("netconnstadia: netconn user removed at local user index %d --> local user id: 0x%08x\n",
                iLocalUserIndex, pRef->aEAUsers[iLocalUserIndex]->GetUserID()));

            pRef->aEAUsers[iLocalUserIndex]->Release();
            pRef->aEAUsers[iLocalUserIndex] = NULL;

            ds_memclr(&pRef->aUsers[iLocalUserIndex], sizeof(*pRef->aUsers));
        }
        else
        {
            NetPrintf(("netconnstadia: failed to remove local user at index %d - local user ids do not match (0x%08x vs 0x%08x)\n",
                iLocalUserIndex, pIEAUser->GetUserID(), pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
        }
    }
    else
    {
        NetPrintf(("netconnstadia: failed to remove IEAUSER at index %d - no local user at that spot\n", iLocalUserIndex));
    }
}

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function    NetConnStartup

    \Description
        Bring the network connection module to life. Creates connection with IOP
        resources and gets things ready to go. Puts all device drivers into "probe"
        mode so they look for appropriate hardware. Does not actually start any
        network activity.

    \Input *pParams - startup parameters

    \Output
        int32_t     - zero=success, negative=failure

    \Notes
        NetConnRefT::iRefCount serves as a counter for the number of times
        NetConnStartup has been called. This allows us to track how many modules
        are using it and how many times we expect NetConnShutdown to the called.
        In the past we only allowed a single call to NetConnStartup but some
        libraries may need to networking without a guarentee that the game has
        already started it.

        pParams can contain the following terms:

        \verbatim
            -noupnp                 - disable upnp support
            -servicename            - set servicename <game-year-platform> required for SSL use
            -singlethreaded         - start DirtySock in single-threaded mode (typically when used in servers)
            -affinity=<mask as hex> - the cpu affinity for our internal threads
        \endverbatim

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStartup(const char *pParams)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iThreadPrio = 10;
    int32_t iRet = 0;
    int32_t iResult = 0, iLocalUserIndex;
    char strThreadCpuAffinity[16];

    // allow NULL params
    if (pParams == NULL)
    {
        pParams = "";
    }

    // debug display of input params
    NetPrintf(("netconnstadia: startup params='%s'\n", pParams));

    // common startup
    // pRef shall hold the address of the NetConnRefT after completion if no error occured
    iResult = NetConnCommonStartup(sizeof(*pRef), pParams, (NetConnCommonRefT**)(&pRef));

    // treat the result of the common startup, if already started simply early out
    if (iResult == NETCONN_ERROR_ALREADY_STARTED)
    {
        return(0);
    }
    // otherwise, if an error occured report it
    else if (iResult < 0)
    {
        return(iResult);
    }

    // local user startup
    if ((pRef->pLocalUser = NetConnLocalUserInit(pRef, _NetConnAddLocalUser, _NetConnRemoveLocalUser)) == NULL)
    {
        return(NETCONN_ERROR_NO_MEMORY);
    }

    pRef->eState = NetConnRefT::ST_INIT;
    pRef->iPeerPort = NETCONN_DEFAULT_UPNP_PORT;

    // check for singlethreaded mode
    if (strstr(pParams, "-singlethreaded"))
    {
        iThreadPrio = -1;
    }

    // get the thread cpu affinity setting from our startup params, defaulting to 0x0
    ds_memclr(strThreadCpuAffinity, sizeof(strThreadCpuAffinity));
    NetConnCopyParam(strThreadCpuAffinity, sizeof(strThreadCpuAffinity), "-affinity=", pParams, "0x0");
    pRef->iThreadCpuAffinity =(int32_t) strtol(strThreadCpuAffinity, NULL, 16);

    // create network instance
    if (SocketCreate(iThreadPrio, 0, pRef->iThreadCpuAffinity) != 0)
    {
        NetPrintf(("netconnstadia: unable to start up dirtysock\n"));
        _NetConnShutdownInternal(pRef, 0);
        return(NETCONN_ERROR_SOCKET_CREATE);
    }

    // create and configure dirtycert
    if (NetConnDirtyCertCreate(pParams))
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnstadia: unable to create dirtycert\n"));
        return(NETCONN_ERROR_DIRTYCERT_CREATE);
    }

    // start up protossl
    if (ProtoSSLStartup() < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnstadia: unable to start up protossl\n"));
        return(NETCONN_ERROR_PROTOSSL_CREATE);
    }
    ProtoSSLSetCACert((const uint8_t *)_aGlobalSign, sizeof(_aGlobalSign));

    // create protohttp module
    if ((pRef->pHttp = ProtoHttpCreate(4096)) == NULL)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnstadia: unable to create protohttp module\n"));
        return(NETCONN_ERROR_INTERNAL);
    }
    ProtoHttpControl(pRef->pHttp, 'spam', 0, 0, NULL);

    // create the upnp module
    if (!strstr(pParams, "-noupnp"))
    {
        pRef->pProtoUpnp = ProtoUpnpCreate();
        if (pRef->pProtoUpnp == NULL)
        {
            _NetConnShutdownInternal(pRef, 0);
            NetPrintf(("netconnstadia: unable to start up protoupnp\n"));
            return(NETCONN_ERROR_PROTOUPNP_CREATE);
        }
    }

    // initialize the user critical section
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        NetCritInit(&pRef->aUsers[iLocalUserIndex].Crit, "user crit");
    }

    // add netconn task handle
    if (NetConnIdleAdd(_NetConnUpdate, pRef) < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnstadia: unable to add netconn task handler\n"));
        return(NETCONN_ERROR_INTERNAL);
    }

    // save ref
    _NetConn_pRef = pRef;

    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function    NetConnQuery

    \Description
        Returns the index of the given gamertag. If the gamertag is not
        present, treat pList as a pointer to a player id, and find the user by
        player id. Otherwise return the index of the first logged in user.

    \Input *pGamertag   - gamer tag
    \Input *pList       - player id to get index of
    \Input iListSize    - unused

    \Output
        int32_t         - negative=gamertag not logged in, else index of user

    \Version 02/14/2020 (eesponda)
*/
/********************************************************************************F*/
int32_t NetConnQuery(const char *pGamertag, NetConfigRecT *pList, int32_t iListSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    if (pGamertag != NULL)
    {
        // Find the matching gamertag
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; iIndex += 1)
        {
            #if GGP_VERSION >= GGP_MAKE_VERSION(1, 50, 0)
            if (ds_strnicmp(pRef->aUsers[iIndex].Profile.stadia_name, pGamertag, sizeof(pRef->aUsers[iIndex].Profile.stadia_name)) == 0)
            #else
            if (ds_strnicmp(pRef->aUsers[iIndex].Profile.gamer_tag, pGamertag, sizeof(pRef->aUsers[iIndex].Profile.gamer_tag)) == 0)
            #endif
            {
                // We've found the right user, get their index
                return(iIndex);
            }
        }
    }
    else if (pList != NULL)
    {
        // Find the matching player_id
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; iIndex += 1)
        {
            if (memcmp(&pRef->aUsers[iIndex].Profile.player_id, pList, sizeof(pRef->aUsers[iIndex].Profile.player_id)) == 0)
            {
                // We've found the right user, get their index
                return(iIndex);
            }
        }
    }
    else
    {
        // Return the index of the first online user
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; iIndex += 1)
        {
            if ((pRef->aEAUsers[iIndex] != NULL) && pRef->aEAUsers[iIndex]->IsSignedIn())
            {
                // We've found an online user, get their index
                return(iIndex);
            }
        }
    }

    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    NetConnConnect

    \Description
        Used to bring the networking online with a specific configuration. Uses a
        configuration returned by NetConnQuery.

    \Input *pConfig - unused
    \Input *pOption - asciiz list of config parameters
                      "peerport=<port>" to specify peer port to be opened by upnp.
    \Input iData    - platform-specific

    \Output
        int32_t     - negative=error, zero=success

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnConnect(const NetConfigRecT *pConfig, const char *pOption, int32_t iData)
{
    int32_t iResult = 0;
    NetConnRefT *pRef = _NetConn_pRef;

    // check connection options, if present
    if (pRef->eState == NetConnRefT::ST_INIT)
    {
        // check for connect options
        if (pOption != NULL)
        {
            const char *pOpt;

            // check for specification of peer port
            if ((pOpt = strstr(pOption, "peerport=")) != NULL)
            {
                pRef->iPeerPort = (int32_t)strtol(pOpt+9, NULL, 10);
            }
        }
        NetPrintf(("netconnstadia: upnp peerport=%d %s\n",
            pRef->iPeerPort, (pRef->iPeerPort == NETCONN_DEFAULT_UPNP_PORT ? "(default)" : "(selected via netconnconnect param)")));

        // start up network interface
        SocketControl(NULL, 'conn', 0, NULL, NULL);

        // transition to connecting state
        pRef->eState = NetConnRefT::ST_CONN;
        _NetConnUpdateConnStatus(pRef, '~con');
    }
    else
    {
        NetPrintf(("netconnstadia: NetConnConnect() ignored because already connected!\n"));
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    NetConnDisconnect

    \Description
        Used to bring down the network connection. After calling this, it would
        be necessary to call NetConnConnect to bring the connection back up or
        NetConnShutdown to completely shutdown all network support.

    \Output
        int32_t     - negative=error, zero=success

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnDisconnect(void)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // shut down networking
    if (pRef->eState != NetConnRefT::ST_INIT)
    {
        // bring down network interface
        SocketControl(NULL, 'disc', 0, NULL, NULL);

        // reset status
        pRef->eState = NetConnRefT::ST_INIT;
        pRef->uConnStatus = 0;
    }

    // abort upnp operations
    if (pRef->pProtoUpnp != NULL)
    {
        ProtoUpnpControl(pRef->pProtoUpnp, 'abrt', 0, 0, NULL);
    }

    // done
    return(0);
}

/*F********************************************************************************/
/*!
    \Function NetConnControl

    \Description
        Set module behavior based on input selector.

    \Input  iControl    - input selector
    \Input  iValue      - selector input
    \Input  iValue2     - selector input
    \Input *pValue      - selector input
    \Input *pValue2     - selector input

    \Output
        int32_t         - selector result

    \Notes
        iControl can be one of the following:

        \verbatim
            snam: set DirtyCert service name
            spam: set the verbosity level
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonControl()

    \Version 1.0 04/27/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnControl(int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue, void *pValue2)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // make sure module is started before allowing any other control calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnstadia: warning - calling NetConnControl() while module is not initialized\n"));
        return(-1);
    }

    // set dirtycert service name
    if (iControl == 'snam')
    {
        return(DirtyCertControl('snam', 0, 0, pValue));
    }
    // set debug level
    if (iControl == 'spam')
    {
        // set debug level and fall-through
        pRef->Common.iDebugLevel = iValue;
    }
    // request refresh of jwt
    if (iControl == 'tick')
    {
        if ((iValue < 0) || (iValue >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnstadia: invalid user index %d passed to 'tick' control\n", iValue));
            return(-1);
        }
        // request the token
        return(_NetConnRequestAuthToken(pRef, iValue));
    }

    // pass through unhandled selectors to NetConnCommon
    return(NetConnCommonControl(&pRef->Common, iControl, iValue, iValue2, pValue, pValue2));
}

/*F********************************************************************************/
/*!
    \Function    NetConnStatus

    \Description
        Check general network connection status. Different selectors return
        different status attributes.

    \Input iKind    - status selector ('open', 'conn', 'onln')
    \Input iData    - (optional) selector specific
    \Input *pBuf    - (optional) pointer to output buffer
    \Input iBufSize - (optional) size of output buffer

    \Output
        int32_t     - selector specific

    \Notes
        iKind can be one of the following:

        \verbatim
            addr: ip address of client
            affn: thread cpu affinity setting
            bbnd: TRUE if broadband, else FALSE
            chat: TRUE if local user cannot chat, else FALSE
            conn: connection status: +onl=online, ~<code>=in progress, -<err>=NETCONN_ERROR_*
            envi: platform environment
            macx: MAC address of client (returned in pBuf)
            onln: true/false indication of whether network is operational
            open: true/false indication of whether network code running
            proc: number of processor cores on the system (Linux only)
            type: connection type: NETCONN_IFTYPE_* bitfield
            upnp: return protoupnp external port info, if available
            vers: return DirtySDK version
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonStatus()

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStatus(int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // initialize output buffer
    if (pBuf != NULL)
    {
        ds_memclr(pBuf, iBufSize);
    }

    // see if network code is initialized
    if (iKind == 'open')
    {
        return(pRef != NULL);
    }
    // return DirtySDK version
    if (iKind == 'vers')
    {
        return(DIRTYSDK_VERSION);
    }

    // make sure module is started before allowing any other status calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnstadia: warning - calling NetConnStatus() while module is not initialized\n"));
        return(-1);
    }

    // return the thread cpu affinity setting
    if (iKind == 'affn')
    {
        return(pRef->iThreadCpuAffinity);
    }

    // return broadband (TRUE/FALSE)
    if (iKind == 'bbnd')
    {
        return(TRUE);
    }
    // return ability of local user to chat
    if (iKind == 'chat')
    {
        // make sure it is a valid index
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            return(-1);
        }
        // make sure we have a valid user at that index
        if (pRef->aUsers[iData].Profile.player_id == 0)
        {
            return(-2);
        }
        // return if the feature is restricted
        return(GgpIsFeatureRestricted(pRef->aUsers[iData].Profile.player_id, kGgpGamerFeature_VoiceChat, NULL));
    }
    // connection status
    if (iKind == 'conn')
    {
        return(pRef->uConnStatus);
    }
    // platform environment
    if ((iKind == 'envi') && (pRef->uConnStatus == '+onl'))
    {
        return(pRef->uPlatEnv);
    }
    // get the GgpPlayerId
    if (iKind == 'gpid')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < sizeof(GgpPlayerId)))
        {
            // invalid buffer or buffer too small
            return(-2);
        }

        if (pRef->aUsers[iData].Profile.player_id == 0)
        {
            // no valid player id at the specified user index
            return(-3);
        }

        ds_memcpy(pBuf, &pRef->aUsers[iData].Profile.player_id, sizeof(pRef->aUsers[iData].Profile.player_id));
        return(0);
    }
    // see if connected to ISP/LAN
    if (iKind == 'onln')
    {
        return(pRef->uConnStatus == '+onl');
    }

    // return number of processor cores
    if (iKind == 'proc')
    {
        if (pRef->iNumProcCores == 0)
        {
            pRef->iNumProcCores = _NetConnGetNumProcs();
        }
        return(pRef->iNumProcCores);
    }
    if (iKind == 'tick')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnstadia: invalid user index %d passed to 'tick' status\n", iData));
            return(-1);
        }

        switch (pRef->aUsers[iData].eJwtState)
        {
            case ST_TOKEN_INVALID:
            {
                // request a new token since it is invalid
                _NetConnRequestAuthToken(pRef, iData);
                return(0);
            }
            case ST_TOKEN_FAILED:
            {
                // the token failure was reported, go back to invalid to allow a new request
                pRef->aUsers[iData].eJwtState = ST_TOKEN_INVALID;
                return(0);
            }
            case ST_TOKEN_INPROG:
            {
                return(0);
            }
            case ST_TOKEN_VALID:
            {
                // check the validity of the token
                if (difftime(time(NULL) - NETCONN_TOKEN_CLOCKDRIFT, pRef->aUsers[iData].Jwt.Payload.uExpiryTime) > 0)
                {
                    _NetConnRequestAuthToken(pRef, iData);
                    return(0);
                }
                else
                {
                    // validate the output size
                    if ((pBuf != NULL) && (iBufSize < (pRef->aUsers[iData].iJwtSize + 1)))
                    {
                        NetPrintf(("netconnstadia: user-provided buffer for jwt is too small (%d/%d)\n", iBufSize, (pRef->aUsers[iData].iJwtSize + 1)));
                        return(-1);
                    }
                    // copy the jwt if a buffer is provided and the previous check passed
                    if (pBuf != NULL)
                    {
                        ds_strnzcpy((char *)pBuf, pRef->aUsers[iData].pPlayerJwt->jwt, iBufSize);
                    }
                    return(pRef->aUsers[iData].iJwtSize + 1);
                }
            }
        }
    }
    // return interface type (more verbose)
    if (iKind == 'type')
    {
        return(NETCONN_IFTYPE_ETHER);
    }
    // return upnp addportmap info, if available
    if (iKind == 'upnp')
    {
        // if protoupnp is available, and we've added a port map, return the external port for the port mapping
        if ((pRef->pProtoUpnp != NULL) && (ProtoUpnpStatus(pRef->pProtoUpnp, 'stat', NULL, 0) & PROTOUPNP_STATUS_ADDPORTMAP))
        {
            return(ProtoUpnpStatus(pRef->pProtoUpnp, 'extp', NULL, 0));
        }
    }

    // pass unrecognized options to NetConnCommon
    return(NetConnCommonStatus(&pRef->Common, iKind, iData, pBuf, iBufSize));
}

/*F********************************************************************************/
/*!
    \Function    NetConnShutdown

    \Description
        Shutdown the network code and return to idle state.

    \Input  uShutdownFlags  - shutdown configuration flags

    \Output
        int32_t             - negative=error, else zero

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnShutdown(uint32_t uShutdownFlags)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // make sure we've been started
    if (pRef == NULL)
    {
        return(NETCONN_ERROR_NOTACTIVE);
    }

    // disconnect the interfaces
    NetConnDisconnect();

    return(_NetConnShutdownInternal(pRef, uShutdownFlags));
}

/*F********************************************************************************/
/*!
    \Function    NetConnSleep

    \Description
        Sleep the application for some number of milliseconds.

    \Input iMilliSecs    - number of milliseconds to block for

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
void NetConnSleep(int32_t iMilliSecs)
{
    usleep(iMilliSecs*1000);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnAddLocalUser

    \Description
        Use this function to tell netconn about a newly detected local user on the local console.

    \Input iLocalUserIndex  - index at which DirtySDK needs to insert this user it its internal user array
    \Input *pLocalUser      - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 01/16/2014 (mclouatre)
*/
/*************************************************************************************************F*/
int32_t NetConnAddLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserAdd(iLocalUserIndex, pLocalUser));
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnRemoveLocalUser

    \Description
        Use this function to tell netconn about a local user that no longer exists

    \Input iLocalUserIndex  - index in the internal DirtySDK user array at which the user needs to be cleared
    \Input *pLocalUser      - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 01/16/2014 (mclouatre)
*/
/*************************************************************************************************F*/
int32_t NetConnRemoveLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserRemove(iLocalUserIndex, pLocalUser));
}
