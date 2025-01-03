/*H********************************************************************************/
/*!
    \File protohttpps4.c

    \Description
        This module implements an HTTP client that can perform basic transactions
        (get/put) with an HTTP server using the PS4 SDK.  

    \Copyright
        Copyright (c) Electronic Arts 2016-2020. ALL RIGHTS RESERVED.

    \Notes
        Proxy servers are not supported in the PS4 implementation.

    \Version 0.5 07/05/2016 (amakoukji) First Version
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <net.h>
#include <libhttp.h>

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtythread.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/ps4/dirtycontextmanagerps4.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/proto/protossl.h"
#include "DirtySDK/proto/protohttp.h"
#include "DirtySDK/util/base64.h"

#include "dirtynetpriv.h" // SocketRate*

/*** Defines **********************************************************************/

//! default ProtoHttp timeouts
#define PROTOHTTP_TIMEOUT_DEFAULT       (30*1000)
#define PROTOHTTP_RESOLVE_RETRIES       (1)

//! default maximum allowed redirections
#define PROTOHTTP_MAXREDIRECT   (3)

//! size of "last-received" header cache
#define PROTOHTTP_HDRCACHESIZE  (1024)

//! protohttp revision number (maj.min)
#define PROTOHTTP_VERSION       (0x0103)          // update this for major bug fixes or protocol additions/changes

//! pool size, Sony only supports multiples of 16KiB
#define PROTOHTTP_POOLSIZE      (16*1024)
#define PROTOHTTP_SSL_POOLSIZE  (608 * 1024)

#define PROTOHTTP_KEEPALIVE_MAX_IDLE_DEFAULT (120*1000)  // 120 seconds

#define PROTOHTTP_OPTIONS_FLAGS_ALL (SCE_HTTPS_FLAG_SERVER_VERIFY|SCE_HTTPS_FLAG_CLIENT_VERIFY|SCE_HTTPS_FLAG_CN_CHECK|SCE_HTTPS_FLAG_NOT_AFTER_CHECK\
                                    |SCE_HTTPS_FLAG_NOT_BEFORE_CHECK|SCE_HTTPS_FLAG_KNOWN_CA_CHECK|SCE_HTTPS_FLAG_SESSION_REUSE|SCE_HTTPS_FLAG_SNI)


/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

typedef struct ProtoHttpCACertT
{
    uint8_t *pCACert;
    int32_t iCertSize;
    struct ProtoHttpCACertT *pNext;  //!< link to next cert in list
} ProtoHttpCACertT;

typedef struct ProtoHttpSendRequestArgT
{
    ProtoHttpRefT   *pState;
    const char      *pStrBuf;
    const char      *pPostData;
    int32_t          iSize;
    int32_t          iSizeFinal;
} ProtoHttpSendRequestArgT;

typedef struct ProtoHttpRedirectRequestT
{
    char *pRespHeader;
    uint32_t uRespHeaderLen;
} ProtoHttpRedirectRequestT;

typedef struct ProtoHttpStateT
{
    // list of CAs
    ProtoHttpCACertT *pProtoHttpPs4_CACerts;
    uint32_t uCaListVersion;

    // threading variables
    int32_t iMaxThreads;
    int32_t iCurrentThreads;
} ProtoHttpStateT;

//! http module state
struct ProtoHttpRefT
{
    int32_t iNetMemId;      //!< sce net context id
    int32_t iSslContextId;  //!< sce ssl context id
    int32_t iHttpContextId; //!< sce http context id
    int32_t iSceConnId;     //!< sce connection id
    int32_t iSceTemplateId; //!< sce template id
    int32_t iSceRequestId;  //!< sce template id
    SceHttpEpollHandle EpollHandle; //!< epoll handle
    size_t  iSceRespHeaderLength;
    char    *pSceRespHeader;
    uint32_t uCaListVersionLoaded;
    SceSslVersion Vers;
    uint32_t uSceHttpFlags;
    SceSslData PrivateKeyData;
    SceSslData ClientCertData;
    uint8_t bEpollInitialized;
    uint8_t bUsePrivateKey;
    uint8_t bUseClientCert;
    uint8_t bCAsApplied;
    const char *pData;    //!< original data pointer, used only for custom header callback

    volatile int32_t iThreadLife;            //!< worker thread life flag,  0 = dead, 1 = alive, 2 = shutting down
    DirtyConditionRefT *pPushDataThreadCond; //!< condition variable to wakeup worker thread
    NetCritT PushDataCrit;                   //!< mutex associated with pPushDataThreadCond
    int32_t iPushDataResult;                 //!< queued result from the worker thread
    ProtoHttpSendRequestArgT QueuedPushData; //!< queued data for worker thread
    uint8_t bPushDataResultAvailable;        //!< flag meaning data from thread is ready
    uint8_t bPushDataInputAvailable;         //!< flag meaning data to thread us ready
    ProtoHttpRedirectRequestT QueuedRedData; //!< queued redirection data for the main thread

    SocketRateT SendRate;    //!< send rate estimation data
    SocketRateT RecvRate;    //!< recv rate estimation data

    uint32_t uIdleTimeStart; //!< start tick when the last http operation was completed for a keep-alive request 
    int32_t  iIdleTimeMax;   //!< maximum time to idle on an open connection when keep-alive was specificed

    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;      //!< global callback for modifying request header
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;    //!< global callback for viewing recv header on recepit
    void *pCallbackRef;     //!< user ref for global callback

    ProtoHttpWriteCbT *pWriteCb;                    //!< optional data write callback
    ProtoHttpCustomHeaderCbT *pReqCustomHeaderCb;   //!< optional custom header callback
    ProtoHttpReceiveHeaderCbT *pReqReceiveHeaderCb; //!< optional receive header callback
    void *pUserData;                                //!< user data for callback

    // module memory group
    int32_t iMemGroup;      //!< module mem group id
    void *pMemGroupUserData;//!< user data associated with mem group

    NetCritT HttpCrit;      //!< critical section for guarding update from send/recv

    ProtoHttpRequestTypeE eRequestType;  //!< request type of current request
    int32_t iPort;          //!< server port
    int32_t iBasePort;      //!< base port (used for partial urls)
    int32_t iSecure;        //!< secure connection
    int32_t iBaseSecure;    //!< base security setting (used for partial urls)

    enum
    {
        ST_IDLE,            //!< idle
        ST_REDIR,           //!< processing redirection
        ST_SEND,            //!< sending buffered data
        ST_RESP,            //!< waiting for initial response (also sending any data not buffered if POST or PUT)
        ST_HEAD,            //!< getting header
        ST_BODY,            //!< getting body
        ST_DONE,            //!< transaction success
        ST_FAIL             //!< transaction failed
    } eState;               //!< current state

    int32_t iSslFail;       //!< ssl failure code, if any
    uint32_t uSslFailDet;   //!< ssl failure detail, if any
    int32_t iHresult;       //!< ssl hresult code, if any
    int32_t iHdrCode;       //!< result code
    int32_t iHdrDate;       //!< last modified date

    int32_t iHeadSize;      //!< size of head data
    int64_t iPostSize;      //!< amount of data being sent in a POST or PUT operation
    int64_t iBodySize;      //!< size of body data
    int64_t iBodyRcvd;      //!< size of body data received by caller
    int32_t iRecvSize;      //!< amount of data received by ProtoHttpRecvAll
    int32_t iRecvRslt;      //!< last receive result

    char *pInpBuf;          //!< input buffer
    int32_t iInpMax;        //!< maximum buffer size
    int32_t iInpOff;        //!< offset into buffer
    int32_t iInpLen;        //!< total length in buffer
    int64_t iInpCnt;        //!< ongoing count
    int32_t iInpOvr;        //!< input overflow amount
    int32_t iChkLen;        //!< chunk length (if chunked encoding)
    int32_t iHdrOff;        //!< temp offset used when receiving header

    int32_t iNumRedirect;   //!< number of redirections processed
    int32_t iMaxRedirect;   //!< maximum number of redirections allowed

    uint32_t uTimeout;      //!< protocol timeout
    uint32_t uTimer;        //!< timeout timer
    int32_t iResolveRetries;//!< resolve retry timout
    int32_t iKeepAlive;     //!< indicate if we should try to use keep-alive
    int32_t iKeepAliveDflt; //!< keep-alive default (keep-alive will be reset to this value; can be overridden by user)

    char *pAppendHdr;       //!< append header buffer pointer
    int32_t iAppendLen;     //!< size of append header buffer

    char strUserAgent[64];  //!< user agent string 

    char strRequestHdr[PROTOHTTP_HDRCACHESIZE];         //!< storage for most recent HTTP request header
    char strHdr[PROTOHTTP_HDRCACHESIZE]; //!< storage for most recently received HTTP header
    char strHost[256];      //!< server name
    char strBaseHost[256];  //!< base server name (used for partial urls)
    char strProxy[256];     //!< proxy server name/address (including port)
    char strUrl[2048];      //!< full url, includes host (or proxy host), port and url
    //char strFinalUrl[256];  //!< full url without proxy

    uint8_t bNoThreads;     //!< boolean indicating we were not able to start a worker thread
    uint8_t bWaitForData;   //!< boolean indicating we are waiting for a POST's data source to be specified
    uint8_t bTimeout;       //!< boolean indicating whether a timeout occurred or not
    uint8_t bChunked;       //!< if TRUE, transfer is chunked
    uint8_t bRecvEndChunk;  //!< if TRUE, we must deferred end chunk processing
    uint8_t bHeadOnly;      //!< if TRUE, only get header
    uint8_t bCloseHdr;      //!< server wants close after this
    uint8_t bClosed;        //!< connection has been closed
    uint8_t bConnOpen;      //!< connection is open
    uint8_t iVerbose;       //!< debug output verbosity
    uint8_t bVerifyHdr;     //!< perform header type verification
    uint8_t bHttp1_0;       //!< TRUE if HTTP/1.0, else FALSE
    uint8_t bCompactRecv;   //!< compact receive buffer
    uint8_t bInfoHdr;       //!< TRUE if a new informational header has been cached; else FALSE
    uint8_t bNewConnection; //!< TRUE if a new connection should be used, else FALSE (if using keep-alive), 
                            //   note that the Sony scehttplib can handle this on its own but we use similar logic to the protohttp.c implementation to maintain similar behavior
    uint8_t bReuseOnPost;   //!< TRUE if reusing a previously established connection on PUT/POST is allowed, else FALSE
    uint8_t bSetHttpsFlags; //!< TRUE if we should overwrite the default SCE_HTTPS_FLAG_* flags
    uint8_t bSetSslVers;    //!< TRUE if we wish to call sceHttpsSetSslVersion, FALSE otherwise
                            // see https://ps4.scedev.net/resources/documents/SDK/3.500/Http-Reference/0077.html for reference

};

/*** Function Prototypes **********************************************************/

static int32_t _ProtoHttpRedirectCallback(int request, int32_t statusCode, int32_t *method, const char *location, void *userArg);
static int32_t _ProtoHttpCookieRecvCallback(int request, const char *url, const char *cookieHeader, size_t headerLen, void *userArg);
static int32_t _ProtoHttpGetHeader(ProtoHttpRefT *pState);
static const uint8_t *_ProtoHttpFindPEMSignature(const uint8_t *pCertData, int32_t iCertSize, const char *pSigText);
static int32_t _ProtoHttpIsPEMFormat(const uint8_t *pCACert, int32_t iCertSize);

/*** Variables ********************************************************************/

// PEM formatting constants
static const char _strPemBeg[] = "-----BEGIN CERTIFICATE-----";
static const char _strPemEnd[] = "-----END CERTIFICATE-----";
static const char _str509Beg[] = "-----BEGIN X509 CERTIFICATE-----";
static const char _str509End[] = "-----END X509 CERTIFICATE-----";

// GOS Certs

// The 2048-bit public key modulus for 2015 GOS CA Cert signed with sha256 and an exponent of 65537
static char _ProtoSSL_GOS2015ServerModulus2048[] =
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

// Private variables

static const SceSslData _ProtoHttp_aGosCACerts[] = 
{
    // gos2015 CA
    {
        _ProtoSSL_GOS2015ServerModulus2048,
        sizeof(_ProtoSSL_GOS2015ServerModulus2048)
    }
};

// update this when PROTOHTTP_NUMREQUESTTYPES changes
static const char _ProtoHttp_strRequestNames[PROTOHTTP_NUMREQUESTTYPES][16] =
{
    "HEAD", "GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"
};

#if DIRTYCODE_LOGGING
//! ssl version name table
static const char *_HTTP_strVersionNames[] =
{
    "TLSv1",
    "TLSv1.1",
    "TLSv1.2"
};
#endif

static ProtoHttpStateT _ProtoHttp_State = {NULL, 0, 0, 0};


// Public variables


/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSendRequestImpl

    \Description
        Wraps the call to sceSendRequest. This can be called from the 
        worker thread or from the http thread.

    \Input *pState     - reference pointer
    \Input *pStrBuf    - pre-processing data to send
    \Input *pPostData  - post-processing data to send
    \Input  iSize      - pre-processing size of data
    \Input  iSizeFinal - post-processing size of data

    \Output 
        int32_t - < 0 error, otherwise the number of bytes processed

    \Version 05/09/2018 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSendRequestImpl(ProtoHttpRefT *pState, const char *pStrBuf, const char *pPostData, int32_t iSize, int32_t iSizeFinal)
{
    int32_t iReturn = 0;
    int32_t iResult = 0;

    // try and send some data
    iResult = sceHttpSendRequest(pState->iSceRequestId, pPostData, iSizeFinal);

    if (pState->eState == ST_FAIL)
    {
        // during sceHttpSendRequest's processing it might call a callback where we set the state to ST_FAIL
        // check this first to avoid processing as though a send occured
        pState->iSslFail = 0;
        pState->uSslFailDet = 0;
        pState->iHresult = 0;
        sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
        sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
        iReturn = -1;
    }
    else if (pState->eState == ST_REDIR)
    {
        iReturn = 0;
    }
    else if (iResult == 0)
    {
#if DIRTYCODE_LOGGING
        if (pState->iVerbose > 2)
        {
            NetPrintf(("protohttpps4: [%p] sent %d bytes\n", pState, iSizeFinal));
        }
        if (pState->iVerbose > 3)
        {
            NetPrintMem(pStrBuf, iSizeFinal, "http-send");
        }
#endif
        // sent data, so update timeout
        pState->uTimer = NetTick() + pState->uTimeout;

        // update data rate estimation
        SocketRateUpdate(&(pState->SendRate), iSizeFinal, "send");
        iReturn = iSizeFinal;

        if (pState->iPostSize == 0)
        {
            // if there's no data to push jump into the ST_RESP state
            // this is required because at a higher level we cannot distinguish between a return code 0 meaning 
            // try again, and a return code 0 meaning 0 bytes were successfully send
            pState->eState = ST_RESP;
        }
    }
    else if (iResult == SCE_HTTP_ERROR_EAGAIN || iResult == SCE_HTTP_ERROR_BUSY)
    {
        iReturn = 0;
        if (pState->bChunked && iSize == 0 && pPostData == NULL)
        {
            // we've received the end chunk but the scehttp library was unable to process it
            pState->bRecvEndChunk = TRUE;
        }
    }
    else if (iResult < 0)
    {
        NetPrintf(("protohttpps4: [%p] error %s sending %d bytes\n", pState, DirtyErrGetName(iResult), iSize));
        pState->eState = ST_FAIL;
        pState->iSslFail = 0;
        pState->uSslFailDet = 0;
        pState->iHresult = 0;
        sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
        sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
        iReturn = iResult;
    }

    return(iReturn);
}

/*F********************************************************************************/
/*!
    \Function    _ProtoHttpThread

    \Description
        Worker thread to handle data sending through sceHttpSendRequest.
        Even in asynchronous mode this call can take up to 30 ms.
    
    \Input  *pArg    - pointer to ProtoHttpRefT module state
    
    \Version 05/10/2018 (amakoukji)
*/
/********************************************************************************F*/
static void _ProtoHttpThread(void *pArg)
{
    ProtoHttpRefT *pState = (ProtoHttpRefT *)pArg;

    while (pState->iThreadLife == 1)
    {
        // wait for the condition variable to be signaled if there are no pending work to be done
        NetCritEnter(&pState->PushDataCrit);
        while ((pState->iThreadLife == 1)  && (pState->bPushDataInputAvailable == FALSE) )
        {
            DirtyConditionWait(pState->pPushDataThreadCond, &pState->PushDataCrit);
        }
        NetCritLeave(&pState->PushDataCrit);

        // process the data to be sent
        NetCritEnter(&pState->HttpCrit);
        if ((pState->bPushDataResultAvailable == FALSE) && (pState->bPushDataInputAvailable == TRUE))
        {
            // consume the input
            pState->bPushDataInputAvailable = FALSE;
            pState->iPushDataResult = _ProtoHttpSendRequestImpl(pState, pState->QueuedPushData.pStrBuf,
                                                                        pState->QueuedPushData.pPostData,
                                                                        pState->QueuedPushData.iSize,
                                                                        pState->QueuedPushData.iSizeFinal);
            
            // mark data as available
            pState->bPushDataResultAvailable = TRUE;
        }
        NetCritLeave(&pState->HttpCrit);
    }

    // mark worker thread as dead
    pState->iThreadLife = 0;
}

/*F********************************************************************************/
/*!
    \Function    _ProtoHttpReleaseWorkerThread

    \Description
        Sets the worker thread to terminate. 
        Does not wait for thread termination before exiting.
    
    \Input  *pState - module state
    
    \Version 05/10/2018 (amakoukji)
*/
/********************************************************************************F*/
static void _ProtoHttpReleaseWorkerThread(ProtoHttpRefT *pState)
{
    NetCritEnter(&pState->PushDataCrit);

    // set the thread life state to shutting down
    pState->iThreadLife = 2;
    NetCritEnter(NULL);
    --(_ProtoHttp_State.iCurrentThreads);
    NetCritLeave(NULL);

    // signal the worker thread to wake up
    if (!DirtyConditionSignal(pState->pPushDataThreadCond))
    {
        NetPrintf(("protohttpps4: [%p] failed to signal worker thread.\n", pState));
    }

    NetCritLeave(&pState->PushDataCrit);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpCreateContexts

    \Description
        Create Sony contexts needed for 1st party library 

    \Input *pState          - reference pointer

    \Version 05/01/2017 (amakoukji)
*/
/********************************************************************************F*/
void _ProtoHttpCreateContexts(ProtoHttpRefT *pState)
{
    int32_t iNetMemId, iSslContextId, iHttpContextId; // Sony library ids
    NetPrintf(("protohttpps4: [%p] creating http contexts\n", pState));

    // create the netpool for this ref
    if (pState->iNetMemId == -1)
    {
        iNetMemId = DirtyContextManagerCreateNetPoolContext("protohttpps4", (1400 * DirtyContextManagerStatus('mncs', 0, NULL, 0)));
        if (iNetMemId < 0)
        {
            NetPrintf(("protohttpps4: unable to allocate sceNetPool, error 0x%08x\n", iNetMemId));
            pState->iNetMemId = -1;
            pState->iSslContextId = -1;
            pState->iHttpContextId = -1;
            return;
        }
        pState->iNetMemId = iNetMemId;
    }

    // create the ssl context for this ref
    if (pState->iSslContextId == -1)
    {
        if ((iSslContextId = DirtyContextManagerCreateSslContext(PROTOHTTP_SSL_POOLSIZE)) < 0)
        {
            NetPrintf(("protohttpps4: unable to initialize sceSslContext, error 0x%08x\n", iSslContextId));
            DirtyContextManagerFreeNetPoolContext(iNetMemId);
            pState->iNetMemId = -1;
            pState->iSslContextId = -1;
            pState->iHttpContextId = -1;
            return;
        }
        pState->iSslContextId = iSslContextId;
    }

    // create the http context for this ref
    // note that we need extra room in this buffer since it deals not only with the request but the response
    // it has been observed that multiple requests with input near the buffer size can run the system out of  
    // memory quickly even if the previous requests are deleted successfully
    if (pState->iHttpContextId == -1)
    {
        if ((iHttpContextId = DirtyContextManagerCreateHttpContext(iNetMemId, iSslContextId, pState->iInpMax * 2)) < 0)
        {
            NetPrintf(("protohttpps4: unable to initialize sceHttpContext, error 0x%08x\n", iHttpContextId));
            DirtyContextManagerFreeSslContext(iSslContextId);
            DirtyContextManagerFreeNetPoolContext(iNetMemId);
            pState->iNetMemId = -1;
            pState->iSslContextId = -1;
            pState->iHttpContextId = -1;
            return;
        }
        pState->iHttpContextId = iHttpContextId;
    }  

    // set a flag indicating that CAs will need to be reapplied as the http context was recreated
    pState->bCAsApplied = FALSE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFreeContexts

    \Description
        Free Sony contexts needed for 1st party library .

    \Input *pState          - reference pointer

    \Version 05/01/2017 (amakoukji)
*/
/********************************************************************************F*/
void _ProtoHttpFreeContexts(ProtoHttpRefT *pState)
{
    NetPrintf(("protohttpps4: [%p] freeing http contexts\n", pState));

    // the template must be freed because it is linked to the http context
    if (pState->bEpollInitialized)
    {
        sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
        pState->bEpollInitialized = FALSE;
    }
    if (pState->iSceTemplateId != 0)
    {
        sceHttpDeleteTemplate(pState->iSceTemplateId);
    }

    if (pState->iHttpContextId != -1)
    {
        DirtyContextManagerFreeHttpContext(pState->iHttpContextId);
    }

    if (pState->iSslContextId != -1)
    {
        DirtyContextManagerFreeSslContext(pState->iSslContextId);
    }

    if (pState->iNetMemId != -1)
    {
        DirtyContextManagerFreeNetPoolContext(pState->iNetMemId);
    }

    pState->iSceTemplateId = 0;
    pState->iNetMemId = -1;
    pState->iSslContextId = -1;
    pState->iHttpContextId = -1;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpProtoSslToSceVers

    \Description
        Translate the protossl ssl version to the equivalent sce ssl version. 

    \Input iProtoSslVers - protossl sso version


    \Version 07/08/2016 (amakoukji)
*/
/********************************************************************************F*/
static SceSslVersion _ProtoHttpProtoSslToSceVers(int32_t iProtoSslVers)
{
    SceSslVersion retValue;
    switch (iProtoSslVers)
    {
        case PROTOSSL_VERSION_TLS1_0:
            retValue = SCE_SSL_VERSION_TLS1_0;
        break;
        case PROTOSSL_VERSION_TLS1_1:
            retValue = SCE_SSL_VERSION_TLS1_1;
        break;
        case PROTOSSL_VERSION_TLS1_2:
            retValue = SCE_SSL_VERSION_TLS1_2;
        break;
        default:
            retValue = SCE_SSL_VERSION_TLS1_2;
        break;
    }
    return(retValue);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpApplyCAs

    \Description
        Apply CAs

    \Input pState - module ref

    \Version 07/08/2016 (amakoukji)
*/
/********************************************************************************F*/
static void _ProtoHttpApplyCAs(ProtoHttpRefT *pState)
{
    int32_t iResult = 0;
    int32_t iCert = 0;
    const SceSslData* caList;
    SceSslData caCert;
    ProtoHttpCACertT *pCert = NULL;

    //this does protect _ProtoHttp_State from being read from in an unsafe way, this doesn't necissarily make _ProtoHttpApplyCAs thread safe (however that is not the goal)
    NetCritEnter(NULL); // protecting _ProtoHttp_State (1)
    // if we're not using the current CaListVersion, reload all the certs
    if (pState->uCaListVersionLoaded != _ProtoHttp_State.uCaListVersion || pState->bCAsApplied == FALSE)
    {
        NetCritLeave(NULL); // finished with _ProtoHttp_State (1)
        // assume iHttpContextId is set at this point
        if ((iResult = sceHttpsUnloadCert(pState->iHttpContextId)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpsUnloadCert failed with error %s\n", pState, DirtyErrGetName(iResult)));
        }

        // load GOS certs
        for (iCert = 0; iCert < (int32_t)(sizeof(_ProtoHttp_aGosCACerts)/sizeof(_ProtoHttp_aGosCACerts[0])); ++iCert)
        {
            caList = &_ProtoHttp_aGosCACerts[iCert];
            iResult = sceHttpsLoadCert(pState->iHttpContextId, 1, &caList, NULL, NULL);
            if (iResult < 0)
            {
                NetPrintf(("protohttpps4: [%p] sceHttpsLoadCert failed to load a GOS CA with error %s\n", pState, DirtyErrGetName(iResult)));
            }
        }

        // load custom certs
        NetCritEnter(NULL); // protecting _ProtoHttp_State (2)
        pCert = _ProtoHttp_State.pProtoHttpPs4_CACerts;
        while (pCert != NULL)
        {
            caCert.ptr = (char*)pCert->pCACert;
            caCert.size = pCert->iCertSize;
            caList = &caCert;
            iResult = sceHttpsLoadCert(pState->iHttpContextId, 1, &caList, NULL, NULL);
            if (iResult < 0)
            {
                NetPrintf(("protohttpps4: [%p] sceHttpsLoadCert failed to load a CA with error %s\n", pState, DirtyErrGetName(iResult)));
            }
            pCert = pCert->pNext;
        }

        pState->uCaListVersionLoaded = _ProtoHttp_State.uCaListVersion;
        pState->bCAsApplied = TRUE;
        NetCritLeave(NULL); // finished with _ProtoHttp_State (2)
    }
    else
    {
        NetCritLeave(NULL); // finished with _ProtoHttp_State (1)
    }

    if (pState->bUsePrivateKey)
    {
        if ((iResult = sceHttpsLoadCert(pState->iHttpContextId, 0, NULL, NULL, &pState->PrivateKeyData)) < 0)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsLoadCert failed with code %s\n", pState, DirtyErrGetName(iResult)));
        }
    }

    if (pState->bUseClientCert)
    {
        if ((iResult = sceHttpsLoadCert(pState->iHttpContextId, 0, NULL, &pState->ClientCertData, NULL)) < 0)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsLoadCert failed with code %s\n", pState, DirtyErrGetName(iResult)));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpCreateConnectionTemplate

    \Description
        Create and configure a sce template id

    \Input pState           - module ref

    \Version 07/08/2016 (amakoukji)
*/
/********************************************************************************F*/
static void _ProtoHttpCreateConnectionTemplate(ProtoHttpRefT *pState)
{
    int32_t iResult = 0;
    pState->iSceTemplateId = sceHttpCreateTemplate(pState->iHttpContextId, pState->strUserAgent, SCE_HTTP_VERSION_1_1, SCE_TRUE);
    if (pState->iSceTemplateId < 0)
    {
        NetPrintf(("protohttpps4: [%p] sceHttpCreateTemplate failed with code %s\n", pState, DirtyErrGetName(pState->iSceConnId)));
        pState->eState = ST_FAIL;
        pState->bClosed = TRUE;
        pState->iSceTemplateId = 0;
    }
    else
    {
        // template create, configure it
        NetPrintfVerbose((pState->iVerbose, 3, "protohttpps4: [%p] template created with id 0x%08x\n", pState, pState->iSceTemplateId));

        // set SSL version if required
        if (pState->bSetSslVers)
        {
            if ((iResult = sceHttpsSetSslVersion(pState->iSceTemplateId, pState->Vers)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsSetSslVersion failed with code %s\n", pState, DirtyErrGetName(iResult)));
                pState->bSetSslVers = FALSE;
                pState->eState = ST_FAIL;
                pState->bClosed = TRUE;
                sceHttpDeleteTemplate(pState->iSceTemplateId);
                pState->iSceTemplateId = 0;
            }
        }

        if (pState->bSetHttpsFlags)
        {
            if (pState->iSceTemplateId > 0)
            {
                // reset flags and apply the new state
                if ((iResult = sceHttpsDisableOption(pState->iSceTemplateId, PROTOHTTP_OPTIONS_FLAGS_ALL)) < 0)
                {
                    NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsDisableOption failed with code %s\n", pState, DirtyErrGetName(iResult)));
                    pState->bSetHttpsFlags = FALSE;
                    pState->eState = ST_FAIL;
                    pState->bClosed = TRUE;
                    sceHttpDeleteTemplate(pState->iSceTemplateId);
                    pState->iSceTemplateId = 0;
                }

                if ((iResult = sceHttpsEnableOption(pState->iSceTemplateId, pState->uSceHttpFlags)) < 0)
                {
                    NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsEnableOption failed with code %s, flags 0x%08x\n", pState, DirtyErrGetName(iResult), pState->uSceHttpFlags));
                    pState->bSetHttpsFlags = FALSE;
                    pState->eState = ST_FAIL;
                    pState->bClosed = TRUE;
                    sceHttpDeleteTemplate(pState->iSceTemplateId);
                    pState->iSceTemplateId = 0;
                }
            }
        }

        // set up callbacks
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetRedirectCallback(pState->iSceTemplateId, &_ProtoHttpRedirectCallback, (void*)pState)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetRedirectCallback failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
        
        // set up cookies processing
        if (pState->eState != ST_FAIL && (iResult =  sceHttpSetCookieRecvCallback(pState->iSceTemplateId, &_ProtoHttpCookieRecvCallback, (void*)pState)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetRedirectCallback failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
    
        // set the template as non-blocking
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetNonblock(pState->iSceTemplateId, SCE_TRUE)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetNonblock failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
    
        if (pState->eState != ST_FAIL)
        {
            if ((iResult = sceHttpCreateEpoll(pState->iHttpContextId, &pState->EpollHandle)) < 0)
            {
                NetPrintf(("protohttpps4: [%p] sceHttpCreateEpoll failed with code %s\n", pState, DirtyErrGetName(iResult)));
                pState->eState = ST_FAIL;
                pState->bClosed = TRUE;
                pState->bEpollInitialized = FALSE;
                sceHttpDeleteTemplate(pState->iSceTemplateId);
                pState->iSceTemplateId = 0;
            }
            else
            {
                // set the initialized flag if sceHttpCreateEpoll() succeeded
                pState->bEpollInitialized = TRUE;
            }
        }
            
    
        // set the Sony connection timeouts to a value 5 seconds higher than our timeout so they do not compete
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetConnectTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetConnectTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
    
        // set the receive timeout
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetRecvTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetRecvTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
    
        // set the name resolution timeout
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetResolveTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetResolveTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }

        // set the send timeout
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetSendTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetSendTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }

    
        // set the max name resolution retries
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetResolveRetry(pState->iSceTemplateId, (pState->iResolveRetries))) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetResolveRetry failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }

        /* disable gzip decompression, protohttp does not offer this type of functionality. we expect the client to do
           its own decrompression */
        if (pState->eState != ST_FAIL && (iResult = sceHttpSetInflateGZIPEnabled(pState->iSceTemplateId, SCE_HTTP_DISABLE)) < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpSetInflateGZIPEnabled failed with code %s\n", pState, DirtyErrGetName(iResult)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
            sceHttpDeleteTemplate(pState->iSceTemplateId);
            pState->iSceTemplateId = 0;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpApplyBaseUrl

    \Description
        Apply base url elements (if set) to any url elements not specified (relative
        url support).

    \Input *pState      - module state
    \Input *pKind       - parsed http kind ("http" or "https")
    \Input *pHost       - [in/out] parsed URL host
    \Input iHostSize    - size of pHost buffer
    \Input *pPort       - [in/out] parsed port
    \Input *pSecure     - [in/out] parsed security (0 or 1)
    \Input bPortSpecified - TRUE if a port is explicitly specified in the url, else FALSE

    \Output
        uint32_t        - non-zero if changed, else zero

    \Version 02/03/2010 (jbrookes)
*/
/********************************************************************************F*/
static uint32_t _ProtoHttpApplyBaseUrl(ProtoHttpRefT *pState, const char *pKind, char *pHost, int32_t iHostSize, int32_t *pPort, int32_t *pSecure, uint8_t bPortSpecified)
{
    uint8_t bChanged = FALSE;
    if ((*pHost == '\0') && (pState->strBaseHost[0] != '\0'))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: host not present; setting to %s\n", pState->strBaseHost));
        ds_strnzcpy(pHost, pState->strBaseHost, iHostSize);
        bChanged = TRUE;
    }
    if ((bPortSpecified == FALSE) && (pState->iBasePort != 0))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: port not present; setting to %d\n", pState->iBasePort));
        *pPort = pState->iBasePort;
        bChanged = TRUE;
    }
    if (*pKind == '\0')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: kind (protocol) not present; setting to %d\n", pState->iBaseSecure));
        *pSecure = pState->iBaseSecure;
        // if our port setting is default and incompatible with our security setting, override it
        if (((*pPort == 80) && (*pSecure == 1)) || ((*pPort == 443) && (*pSecure == 0)))
        {
            *pPort = *pSecure ? 443 : 80;
        }
        bChanged = TRUE;
    }
    return(bChanged);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpClose

    \Description
        Close connection to server, if open.

    \Input *pState      - module state
    \Input *pReason     - reason connection is being closed (for debug output)

    \Output
        None.

    \Version 10/07/2005 (jbrookes) First Version
*/
/********************************************************************************F*/
static void _ProtoHttpClose(ProtoHttpRefT *pState, const char *pReason)
{
    int32_t iResult = 0;
    if (pState->bClosed)
    {
        // already issued disconnect, don't need to do it again
        return;
    }

    NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] closing connection: %s\n", pState, pReason));

    if (pState->iSceRequestId > 0)
    {
        sceHttpUnsetEpoll(pState->iSceRequestId);
        iResult = sceHttpDeleteRequest(pState->iSceRequestId);
        if (iResult != SCE_OK)
        {
            NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
        }
        pState->iSceRequestId = 0;
    }
    if (pState->iSceConnId > 0)
    {
        sceHttpDeleteConnection(pState->iSceConnId);
        pState->iSceConnId = 0;
    }

    // free the 1st party contexts to not sit on the ressources
    _ProtoHttpFreeContexts(pState);

    pState->bCloseHdr = FALSE;
    pState->bConnOpen = FALSE;
    pState->bClosed = TRUE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpReset

    \Description
        Reset state before a transaction request.

    \Input  *pState     - reference pointer

    \Output
        None.

    \Version 11/22/2004 (jbrookes) First Version
*/
/********************************************************************************F*/
static void _ProtoHttpReset(ProtoHttpRefT *pState)
{
    int32_t iResult = 0;
    ds_memclr(pState->strHdr, sizeof(pState->strHdr));
    pState->eState = ST_IDLE;
    pState->iSslFail = 0;
    pState->uSslFailDet = 0;
    pState->iHresult = 0;
    pState->iHdrCode = -1;
    pState->iHdrDate = 0;
    pState->iHeadSize = 0;
    pState->iBodySize = pState->iBodyRcvd = 0;
    pState->iRecvSize = 0;
    pState->iInpOff = 0;
    pState->iInpLen = 0;
    pState->iInpOvr = 0;
    pState->iChkLen = 0;
    pState->iInpCnt = 0;
    pState->bTimeout = FALSE;
    pState->bChunked = FALSE;
    pState->bRecvEndChunk = FALSE;
    pState->bHeadOnly = FALSE;
    pState->pSceRespHeader = NULL;
    pState->iSceRespHeaderLength = 0;
    pState->bNoThreads = FALSE;
    pState->bPushDataResultAvailable = FALSE;
    pState->bPushDataInputAvailable = FALSE;

    if (pState->iSceRequestId > 0)
    {
        sceHttpUnsetEpoll(pState->iSceRequestId);
        iResult = sceHttpDeleteRequest(pState->iSceRequestId);
        if (iResult != SCE_OK)
        {
            NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
        }
        pState->iSceRequestId = 0;
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetAppendHeader

    \Description
        Set given string as append header, allocating memory as required.

    \Input *pState      - reference pointer
    \Input *pAppendHdr  - append header string

    \Output
        int32_t         - zero=success, else error

    \Version 11/11/2004 (jbrookes) Split/combined from ProtoHttpGet() and ProtoHttpPost()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSetAppendHeader(ProtoHttpRefT *pState, const char *pAppendHdr)
{
    int32_t iAppendBufLen, iAppendStrLen;

    // check for empty append string, in which case we free the buffer
    if ((pAppendHdr == NULL) || (*pAppendHdr == '\0'))
    {
        if (pState->pAppendHdr != NULL)
        {
            DirtyMemFree(pState->pAppendHdr, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
            pState->pAppendHdr = NULL;
        }
        pState->iAppendLen = 0;
        return(0);
    }

    // check to see if append header is already set
    if ((pState->pAppendHdr != NULL) && (!strcmp(pAppendHdr, pState->pAppendHdr)))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: ignoring set of append header '%s' that is already set\n", pAppendHdr));
        return(0);
    }

    // get append header length
    iAppendStrLen = (int32_t)strlen(pAppendHdr);
    // append buffer size includes null and space for \r\n if not included by submitter
    iAppendBufLen = iAppendStrLen + 3;

    // see if we need to allocate a new buffer
    if (iAppendBufLen > pState->iAppendLen)
    {
        if (pState->pAppendHdr != NULL)
        {
            DirtyMemFree(pState->pAppendHdr, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        }
        if ((pState->pAppendHdr = DirtyMemAlloc(iAppendBufLen, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) != NULL)
        {
            pState->iAppendLen = iAppendBufLen;
        }
        else
        {
            NetPrintf(("protohttpps4: could not allocate %d byte buffer for append header\n", iAppendBufLen));
            pState->iAppendLen = 0;
            return(-1);
        }
    }

    // copy append header
    ds_strnzcpy(pState->pAppendHdr, pAppendHdr, iAppendStrLen+1);

    // if append header is not \r\n terminated, do it here
    if (pState->pAppendHdr[iAppendStrLen-2] != '\r' || pState->pAppendHdr[iAppendStrLen-1] != '\n')
    {
        ds_strnzcat(pState->pAppendHdr, "\r\n", pState->iAppendLen);
    }
    return(0);
}

#if DIRTYCODE_LOGGING
/*F********************************************************************************/
/*!
    \Function _ProtoHttpDisplayHeader

    \Description
        Display header to debug output [DEBUG ONLY]

    \Input *pState      - reference pointer
    \Input *pInpBuf     - input buffer which contains our headers
    \Input iHdrLen      - length of the header  

    \Version 05/03/2010 (jbrookes) Refactored out of ProtoHttpUpdate()
*/
/********************************************************************************F*/
static void _ProtoHttpDisplayHeader(ProtoHttpRefT *pState, char *pInpBuf, int32_t iHdrLen)
{
    // if we just sent a header, display header to debug output
    if (pState->iVerbose > 1)
    {
        int32_t iRequestType;
        for (iRequestType = 0; iRequestType < PROTOHTTP_NUMREQUESTTYPES; iRequestType += 1)
        {
            if (!strncmp(pState->pInpBuf, _ProtoHttp_strRequestNames[iRequestType], strlen(_ProtoHttp_strRequestNames[iRequestType])))
            {
                char *pHdrEnd = pInpBuf + iHdrLen;
                char cHdrChr = *pHdrEnd;
                *pHdrEnd = '\0';
                NetPrintf(("protohttpps4: [%p] sent request:\n", pState));
                NetPrintfVerbose((pState->iVerbose, 2, "protohttpps4: [%p] tick=%u\n", pState, NetTick()));
                NetPrintWrap(pInpBuf, 80);
                *pHdrEnd = cHdrChr;
                break;
            }
        }
    }
}
#endif

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatRequestHeader

    \Description
        Format a request header based on given input data.

    \Input *pState      - reference pointer
    \Input *pUrl        - pointer to user-supplied url
    \Input *pHost       - pointer to hostname
    \Input iPort        - port, or zero if unspecified
    \Input iSecure      - 1=enabled, 0=disabled
    \Input *pRequest    - pointer to request type ("GET", "HEAD", "POST", "PUT")

    \Output
        int32_t         - zero=success, else error

    \Version 11/11/2004 (jbrookes) Split/combined from ProtoHttpGet() and ProtoHttpPost()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatRequestHeader(ProtoHttpRefT *pState, const char *pUrl, const char *pHost, int32_t iPort, int32_t iSecure, const char *pRequest)
{
    int32_t iHdrOff, iOffset = 0, iResult = 0;
    int32_t iInpMax = pState->iInpMax;
    char *pInpBuf = pState->pInpBuf, strKind[8];
    const char *pUrlSlash;
    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;
    void *pUserData;

    // re-parse to get url
    pUrl = ProtoHttpUrlParse(pUrl, strKind, sizeof(strKind), pState->strHost, sizeof(pState->strHost), &pState->iPort, &pState->iSecure);

    // if url is empty or isn't preceded by a slash, put one in
    pUrlSlash = (*pUrl != '/') ? "/" : "";
    
    /* On PS4 it is not possible the access and/or alter the entire header before sending to the server.  We still create the header
       as we normally do for the benefit of the custom header callback.  After the callback we re-parse the header and apply the
       individual header settings when possible (some headers can't be set). */

    // add HTTP/1.1 and crlf after url
    iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "%s %s%s HTTP/1.1\r\n", pRequest, pUrlSlash, pUrl);
    iHdrOff = iOffset;

    // append host 
    if ((pState->iSecure && (pState->iPort == 443)) || (pState->iPort == 80))
    {
        iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "Host: %s\r\n", pState->strHost);
    }
    else
    {
        iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "Host: %s:%d\r\n", pState->strHost, pState->iPort);
    }

    // disable keep-alive?
    if (pState->iKeepAlive == 0)
    {
        iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "Connection: Close\r\n");
    }

    // append user-agent, if not specified in append header, 
    // note that this has already been set in the template and cannot actually be changed at this point
    if ((pState->pAppendHdr == NULL) || !ds_stristr(pState->pAppendHdr, "User-Agent:"))
    {
        iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "User-Agent:%s", pState->strUserAgent);
    }
    
    // add append headers, if specified
    if ((pState->pAppendHdr != NULL) && (pState->pAppendHdr[0] != '\0'))
    {
        iOffset += ds_snzprintf(pInpBuf+iOffset, iInpMax-iOffset, "%s", pState->pAppendHdr);
    }

    // request level callback takes priority to global
    if ((pCustomHeaderCb = pState->pReqCustomHeaderCb) != NULL)
    {
        pUserData = pState->pUserData;
    }
    else
    {
        pCustomHeaderCb = pState->pCustomHeaderCb;
        pUserData = pState->pCallbackRef;
    }

    // call custom header format callback, if specified
    if (pCustomHeaderCb != NULL)
    {
        if ((iOffset = pCustomHeaderCb(pState, pInpBuf, iInpMax, NULL, 0, pUserData)) < 0)
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: custom header callback error %d\n", iOffset));
            return(iOffset);
        }
    }

    // save a copy of the header
    ds_strnzcpy(pState->strRequestHdr, pInpBuf, sizeof(pState->strRequestHdr));

    // recalculate length if request callback wants us to
    if (iOffset == 0)
    {
        iOffset = (int32_t)ds_strnlen(pInpBuf, iInpMax);
    }

    // parse through header list and set headers in sceHttp
    if (iOffset > 0)
    {
        char strName[256], strValue[1024*4];
        const char *pCurrentHdr = pState->pInpBuf+iHdrOff;

        for ( ; ProtoHttpGetNextHeader(pState, pCurrentHdr, strName, sizeof(strName), strValue, sizeof(strValue), &pCurrentHdr) == 0; )
        {
            // ignore headers we cannot set
            if (!ds_stricmp(strName, "content-length") || !ds_stricmp(strName, "host") || !ds_stricmp(strName, "proxy-connection"))
            {
                continue;
            }
            else if (!ds_stricmp(strName, "connection"))
            {
                // update keep-alive value - if not closed explicitely, consider it open as per HTTP 1.1 rfc https://tools.ietf.org/html/rfc7230#section-6.3
                pState->iKeepAlive = (!ds_stricmp(strValue, "close")) ? 0 : 1;
            }
            else if ((iResult = sceHttpAddRequestHeader(pState->iSceRequestId, strName, strValue, SCE_HTTP_HEADER_OVERWRITE)) < 0)
            {
                NetPrintf(("protohttpps4: unable to add header %s, error %s\n", strName, DirtyErrGetName(iResult)));
            }
        }
    }

    #if DIRTYCODE_LOGGING
    _ProtoHttpDisplayHeader(pState, pInpBuf, iOffset);
    #endif

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatRequest

    \Description
        Format a request into the local buffer.

    \Input *pState      - reference pointer
    \Input *pUrl        - pointer to user-supplied url
    \Input *pData       - pointer to data to include with request, or NULL
    \Input iDataLen     - size of data pointed to by pData, or zero if no data
    \Input eRequestType - type of request (PROTOHTTP_REQUESTTYPE_*)

    \Output
        int32_t         - bytes of userdata included in request

    \Version 10/07/2005 (jbrookes) Split/combined from ProtoHttpGet() and ProtoHttpPost()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatRequest(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int64_t iDataLen, ProtoHttpRequestTypeE eRequestType)
{
    char strHost[2048], strKind[8];
    int32_t iPort, iSecure;
    int32_t eState = pState->eState;
    uint8_t bPortSpecified;

    NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] %s %s\n", pState, _ProtoHttp_strRequestNames[eRequestType], pUrl));
    pState->eRequestType = eRequestType;

    // reset various state
    if (pState->eState != ST_IDLE)
    {
        _ProtoHttpReset(pState);
    }

    // assume we don't want a new connection to start with (if this is a pipelined request, don't override the original selection)
    if (pState->iInpLen == 0)
    {
        pState->bNewConnection = FALSE;
    }

    // copy the whole URL as provided
    ds_strnzcpy(pState->strUrl, pUrl, (int32_t)sizeof(pState->strUrl));
    
    // parse the url for kind, host, and port
    pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    /* Unable to support with sceHttp library
    if (pState->strProxy[0] == '\0')
    {
        pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
    }
    else
    {
        NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: using proxy server %s\n", pState->strProxy));
        pUrl2 = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
        ProtoHttpUrlParse2(pState->strProxy, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
        ds_snzprintf(pState->strFinalUrl, (int32_t)sizeof(pState->strFinalUrl), "%s://%s:%d%s", iSecure ? "https" : "http", strHost, iPort, pUrl2);
    }
    */

    // fill in any missing info (relative url) if available
    if (_ProtoHttpApplyBaseUrl(pState, strKind, strHost, sizeof(strHost), &iPort, &iSecure, bPortSpecified) != 0)
    {
        // copy the whole URL with base URL info prepended
        ds_snzprintf(pState->strUrl, (int32_t)sizeof(pState->strUrl), "%s://%s:%d%s", iSecure ? "https" : "http", strHost, iPort, pUrl);
        NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] %s %s\n", pState, _ProtoHttp_strRequestNames[eRequestType], pState->strUrl));
    }

    // determine if host, port, or security settings have changed since the previous request
    if ((iSecure != pState->iSecure) || (ds_stricmp(strHost, pState->strHost) != 0) || (iPort != pState->iPort))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] requesting new connection -- url change to %s\n", pState, strHost));

        // reset keep-alive
        pState->iKeepAlive = pState->iKeepAliveDflt;

        // save new server/port/security state
        ds_strnzcpy(pState->strHost, strHost, sizeof(pState->strHost));
        pState->iPort = iPort;
        pState->iSecure = iSecure;

        // make sure we use a new connection
        pState->bNewConnection = TRUE;
    }

    // check to see if previous connection (if any) is still active
    if ((pState->bNewConnection == FALSE) && pState->bClosed)
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] requesting new connection -- previous connection was closed\n", pState));
        pState->bNewConnection = TRUE;
    }

    // check to make sure we are in a known valid state
    if ((pState->bNewConnection == FALSE) && (eState != ST_IDLE) && (eState != ST_DONE))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] requesting new connection -- current state of %d does not allow connection reuse\n", pState, eState));
        pState->bNewConnection = TRUE;
    }

    // if executing put/post, check to see if connection reuse on request is allowed
    if ((pState->bNewConnection == FALSE) && (pState->bReuseOnPost == FALSE) && ((eRequestType == PROTOHTTP_REQUESTTYPE_PUT) || (eRequestType == PROTOHTTP_REQUESTTYPE_PATCH) || (eRequestType == PROTOHTTP_REQUESTTYPE_POST)))
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] requesting new connection -- reuse on put/post disabled\n", pState));
        pState->bNewConnection = TRUE;
    }

    // if using a proxy server, parse original url to get target host and port for Host header
    /* Unable to support with sceHttp library
    if (pState->strProxy[0] != '\0')
    {
        ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
    }
    */

    // write the user agent string for the template
    if (!ds_stristr(pState->pAppendHdr, "User-Agent:"))
    {
        ds_snzprintf(pState->strUserAgent, (int32_t)sizeof(pState->strUserAgent), "ProtoHttp %d.%d/DS %d.%d.%d.%d.%d (" DIRTYCODE_PLATNAME ")\r\n",
                     (PROTOHTTP_VERSION>>8)&0xff, PROTOHTTP_VERSION&0xff, DIRTYSDK_VERSION_YEAR, DIRTYSDK_VERSION_SEASON, DIRTYSDK_VERSION_MAJOR, DIRTYSDK_VERSION_MINOR, DIRTYSDK_VERSION_PATCH);
    }
    else
    {
        // assume the user agent string in the append headers is terminated by \r\n or the end of the append headers
        const char *pUserAgentStart = ds_stristr(pState->pAppendHdr, "User-Agent:") + strlen("User-Agent:");
        const char *pUserAgentEnd   = ds_stristr(pUserAgentStart, "\r\n");
        if (pUserAgentEnd == NULL)
        {
            ds_snzprintf(pState->strUserAgent, (int32_t)(sizeof(pState->strUserAgent)), "%s", pUserAgentStart);
        }
        else
        {
            ds_snzprintf(pState->strUserAgent, DS_MIN((int32_t)(pUserAgentEnd - pUserAgentStart) + 1, (int32_t)(sizeof(pState->strUserAgent))), "%s", pUserAgentStart);
        }
    }

    // if using a proxy server, parse original url to get target host and port for Host header
    /* Unable to support with sceHttp library
    if (pState->strProxy[0] != '\0')
    {
        ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
    }
    */ 

    if ((iDataLen < 0) && (pData == NULL))
    {
        // for a streaming post, return no data written
        iDataLen = 0;
    }
    else if (iDataLen > 0 && pData == NULL)
    {
        // special case
        // ProtoHttp allows the user to specify the location of the data later while specifying its total size right away
        pState->bWaitForData = TRUE;
    }


    // set headonly status
    pState->bHeadOnly = (eRequestType == PROTOHTTP_REQUESTTYPE_HEAD) ? TRUE : FALSE;
    return((int32_t)iDataLen);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSendRequest

    \Description
        Send a request (already formatted in buffer) to the server.

    \Input *pState      - reference pointer

    \Output
        int32_t         - number of payload data sent

    \Version 05/19/2009 (jbrookes) Split from _ProtoHttpFormatRequest()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSendRequest(ProtoHttpRefT *pState)
{
    int32_t iResult, iDataLen = 0;
    
    // see if we need a new connection
    // note that the Sony scehttplib can handle this on its own but we use similar logic to the protohttp.c implementation to maintain similar behavior
    if (pState->bNewConnection == TRUE)
    {
        // close the existing connection, if not already closed
        _ProtoHttpClose(pState, "new connection");

        // start connect
        NetPrintfVerbose((pState->iVerbose, 2, "protohttpps4: [%p] connect start (tick=%u)\n", pState, NetTick()));

        // create the 1st party http contexts at this point if necessary
        _ProtoHttpCreateContexts(pState);

        // create the template if none exists
        if (pState->iSceTemplateId <= 0)
        {
            _ProtoHttpCreateConnectionTemplate(pState);
        }
        
        // call the connection creation
        if (pState->iSceTemplateId > 0)
        {
            pState->iSceConnId = sceHttpCreateConnectionWithURL(pState->iSceTemplateId, pState->strUrl, pState->iKeepAlive > 0);
            if (pState->iSceConnId > 0)
            {
                NetPrintfVerbose((pState->iVerbose, 3, "protohttpps4: [%p] connection created with id 0x%08x\n", pState, pState->iSceConnId));
                pState->eState = ST_SEND;
                pState->bClosed = FALSE;
            }
            else
            {
                NetPrintf(("protohttpps4: [%p] sceHttpCreateConnectionWithURL failed with code %s\n", pState, DirtyErrGetName(pState->iSceConnId)));
                pState->eState = ST_FAIL;
                pState->bClosed = TRUE;
                pState->iSceConnId = 0;
            }
        }
        else
        {
            // sceTemplate was not created properly, impossible to create a request
            NetPrintf(("protohttpps4: [%p] unable to create a connection, template doesn't exist\n", pState));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            pState->iSceConnId = 0;
        }
    }
    else
    {
        // advance state
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] reusing previous connection (keep-alive)\n", pState));
        pState->eState = ST_SEND;
    }

    // set connection timeout
    pState->uTimer = NetTick() + pState->uTimeout;

    // reset idle timer
    pState->uIdleTimeStart = 0;

    // apply CAs
    _ProtoHttpApplyCAs(pState);

    // create the request
    if (pState->iSceConnId > 0)
    {
        pState->iSceRequestId = sceHttpCreateRequestWithURL2(pState->iSceConnId, _ProtoHttp_strRequestNames[pState->eRequestType], pState->strUrl, (uint64_t)pState->iPostSize);

        if (pState->iSceRequestId < 0)
        {
            NetPrintf(("protohttpps4: [%p] sceHttpCreateRequestWithURL failed with type %s and error code %s\n",
                pState, _ProtoHttp_strRequestNames[pState->eRequestType], DirtyErrGetName(pState->iSceRequestId)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            pState->iSceRequestId = 0;
            sceHttpDeleteConnection(pState->iSceConnId);
            pState->iSceConnId = 0;
        }
        else
        {
            NetPrintfVerbose((pState->iVerbose, 3, "protohttpps4: [%p] request created with id 0x%08x\n", pState, pState->iSceRequestId));
        }
    }

    if (pState->iSceRequestId > 0)
    {
        // format the request header
        if ((iResult = _ProtoHttpFormatRequestHeader(pState, pState->strUrl, pState->strHost, pState->iPort, pState->iSecure, 
                                                     _ProtoHttp_strRequestNames[pState->eRequestType])) < 0)
        {
            NetPrintf(("protohttpps4: [%p] failed to setup request headers %s\n", pState, DirtyErrGetName(pState->iSceConnId)));
            pState->eState = ST_FAIL;
            pState->bClosed = TRUE;
            iResult = sceHttpDeleteRequest(pState->iSceRequestId);
            if (iResult != SCE_OK)
            {
                NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
            }
            pState->iSceRequestId = 0;
            sceHttpDeleteConnection(pState->iSceConnId);
            pState->iSceConnId = 0;
        }

        if (pState->iSceRequestId > 0)
        {
            // append data to header?
            if ((pState->pData != NULL) && (pState->iPostSize > 0))
            {
                iDataLen = pState->iPostSize;

                // see how much data will fit into the buffer
                if (iDataLen > (pState->iInpMax - pState->iInpLen))
                {
                    iDataLen = (pState->iInpMax - pState->iInpLen);
                }

                ds_memcpy(pState->pInpBuf + pState->iInpLen, pState->pData, iDataLen);
                pState->iInpLen += iDataLen;
            }

            // setup chunking
            if (pState->iPostSize < 0)
            {
                sceHttpSetChunkedTransferEnabled(pState->iSceRequestId, TRUE);
                pState->bChunked = TRUE;
                pState->bRecvEndChunk = FALSE;
            }
            else
            {
                sceHttpSetChunkedTransferEnabled(pState->iSceRequestId, FALSE);
                pState->bChunked = FALSE;
            }

            // associate the Epoll
            if (pState->bEpollInitialized == TRUE)
            {
                if ((iResult = sceHttpSetEpoll(pState->iSceRequestId, pState->EpollHandle, NULL)) < 0)
                {
                    NetPrintf(("protohttpps4: [%p] sceHttpSetEpoll failed with code %s\n", pState, DirtyErrGetName(pState->iSceConnId)));
                    pState->eState = ST_FAIL;
                    pState->bClosed = TRUE;
                    sceHttpUnsetEpoll(pState->iSceRequestId);
                    iResult = sceHttpDeleteRequest(pState->iSceRequestId);
                    if (iResult != SCE_OK)
                    {
                        NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
                    }
                    pState->iSceRequestId = 0;
                    sceHttpDeleteConnection(pState->iSceConnId);
                    pState->iSceConnId = 0;
                }
            }
            else
            {
                NetPrintf(("protohttpps4: [%p] sceHttpSetEpoll failed with code %s\n", pState, DirtyErrGetName(pState->iSceConnId)));
                pState->eState = ST_FAIL;
                pState->bClosed = TRUE;
                iResult = sceHttpDeleteRequest(pState->iSceRequestId);
                if (iResult != SCE_OK)
                {
                    NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
                }
                pState->iSceRequestId = 0;
                sceHttpDeleteConnection(pState->iSceConnId);
                pState->iSceConnId = 0;
            }
        }
    }

    // if we requested a connection close, the server may not tell us, so remember it here
    if (pState->iKeepAlive == 0)
    {
        pState->bCloseHdr = TRUE;
    }

    // count the attempt
    pState->iKeepAlive += 1;

    // call the update routine just in case operation can complete
    ProtoHttpUpdate(pState);

    return(iDataLen);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpResizeBuffer

    \Description
        Resize the buffer

    \Input *pState      - reference pointer
    \Input iBufMax      - new buffer size

    \Output
        int32_t         - zero=success, else failure

    \Version 02/21/2011 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpResizeBuffer(ProtoHttpRefT *pState, int32_t iBufMax)
{
    int32_t iCopySize;
    char *pInpBuf;

    NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] resizing input buffer from %d to %d bytes\n", pState, pState->iInpMax, iBufMax));
    if ((pInpBuf = DirtyMemAlloc(iBufMax, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpps4: [%p] could not resize input buffer\n", pState));
        return(-1);
    }

    // calculate size of data to copy from old buffer to new
    if ((iCopySize = pState->iInpLen - pState->iInpOff) > iBufMax)
    {
        NetPrintf(("protohttpps4: [%p] warning; resize of input buffer is losing %d bytes of data\n", pState, iCopySize - iBufMax));
        iCopySize = iBufMax;
    }
    // copy valid contents of input buffer, if any, to new buffer
    ds_memcpy(pInpBuf, pState->pInpBuf+pState->iInpOff, iCopySize);

    // dispose of old buffer
    DirtyMemFree(pState->pInpBuf, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);

    // update buffer variables
    pState->pInpBuf = pInpBuf;
    pState->iInpOff = 0;
    pState->iInpLen = iCopySize;
    pState->iInpMax = iBufMax;

    // clear overflow count
    pState->iInpOvr = 0;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpCompactBuffer

    \Description
        Compact the buffer

    \Input *pState      - reference pointer

    \Output
        int32_t         - amount of space freed by compaction

    \Version 07/02/2009 (jbrookes) Extracted from ProtoHttpRecv()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpCompactBuffer(ProtoHttpRefT *pState)
{
    int32_t iCompacted = 0;
    // make sure it needs compacting
    if (pState->iInpOff > 0)
    {
        // compact the buffer
        if (pState->iInpOff < pState->iInpLen)
        {
            memmove(pState->pInpBuf, pState->pInpBuf+pState->iInpOff, pState->iInpLen-pState->iInpOff);
            iCompacted = pState->iInpOff;
        }
        pState->iInpLen -= pState->iInpOff;
        pState->iInpOff = 0;
        pState->bCompactRecv = FALSE;
    }
    return(iCompacted);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpParseHeader

    \Description
        Parse incoming HTTP header.  The HTTP header is presumed to be at the
        beginning of the input buffer.

    \Input  *pState     - reference pointer

    \Output
        int32_t         - negative=not ready or error, else success

    \Version 11/12/2004 (jbrookes) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpParseHeader(ProtoHttpRefT *pState)
{
    char *s = pState->pSceRespHeader;
    char *t = pState->pSceRespHeader+pState->iSceRespHeaderLength-3;
    char strTemp[128];
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;
    void *pUserData;

    // scan for blank line marking body start
    while ((s != t) && ((s[0] != '\r') || (s[1] != '\n') || (s[2] != '\r') || (s[3] != '\n')))
    {
        s++;
    }
    if (s == t)
    {
        // header is incomplete
        return(-1);
    }

    // save the head size
    pState->iHeadSize = (int32_t)(s+4-pState->pSceRespHeader);
    // terminate header for easy parsing
    s[2] = s[3] = 0;

    // make sure the header is valid
    if (pState->bVerifyHdr)
    {
        if (strncmp(pState->pSceRespHeader, "HTTP", 4))
        {
            // header is invalid
            NetPrintf(("protohttpps4: [%p] invalid result type\n", pState));
            pState->eState = ST_FAIL;
            return(-2);
        }
    }

    // detect if it is a 1.0 response
    pState->bHttp1_0 = !strncmp(pState->pSceRespHeader, "HTTP/1.0", 8);

    // parse header code
    pState->iHdrCode = ProtoHttpParseHeaderCode(pState->pSceRespHeader);
    
    // copy header to header cache
    ds_strnzcpy(pState->strHdr, pState->pSceRespHeader, sizeof(pState->strHdr));

    #if DIRTYCODE_LOGGING
    NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] received %d response (%d bytes)\n", pState, pState->iHdrCode, pState->iHeadSize));
    if (pState->iVerbose > 1)
    {
        NetPrintWrap(pState->pSceRespHeader, 80);
    }
    #endif

    // parse content-length field
    if (ProtoHttpGetHeaderValue(pState, pState->pSceRespHeader, "content-length", strTemp, sizeof(strTemp), NULL) != -1)
    {
        pState->iBodySize = ds_strtoll(strTemp, NULL, 10);
        pState->bChunked = FALSE;
    }
    else
    {
        pState->iBodySize = -1;
    }

    // parse last-modified header
    if (ProtoHttpGetHeaderValue(pState, pState->pSceRespHeader, "last-modified", strTemp, sizeof(strTemp), NULL) != -1)
    {
        pState->iHdrDate = (int32_t)ds_strtotime(strTemp);
    }
    else
    {
        pState->iHdrDate = 0;
    }

    // parse transfer-encoding header
    if (ProtoHttpGetHeaderValue(pState, pState->pSceRespHeader, "transfer-encoding", strTemp, sizeof(strTemp), NULL) != -1)
    {
        pState->bChunked = !ds_stricmp(strTemp, "chunked");
    }

    // parse connection header
    if (pState->bCloseHdr == FALSE)
    {
        ProtoHttpGetHeaderValue(pState, pState->pSceRespHeader, "connection", strTemp, sizeof(strTemp), NULL);
        pState->bCloseHdr = !ds_stricmp(strTemp, "close");
    }

    // note if it is an informational header
    pState->bInfoHdr = PROTOHTTP_GetResponseClass(pState->iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL;

    // request level callback takes priority to global
    if ((pReceiveHeaderCb = pState->pReqReceiveHeaderCb) != NULL)
    {
        pUserData = pState->pUserData;
    }
    else
    {
        pReceiveHeaderCb = pState->pReceiveHeaderCb;
        pUserData = pState->pCallbackRef;
    }

    // trigger recv header user callback, if specified
    if (pReceiveHeaderCb != NULL)
    {
        pReceiveHeaderCb(pState, pState->pSceRespHeader, (uint32_t)strlen(pState->pSceRespHeader), pUserData);
    }

    // clear the header as we done with it
    pState->pSceRespHeader = NULL;
    pState->iSceRespHeaderLength = 0;

    // header successfully parsed
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpProcessInfoHeader

    \Description
        Handles an informational response header (response code=1xx)

    \Input  *pState     - reference pointer

    \Output
        None.

    \Version 05/15/2008 (jbrookes) First Version.
*/
/********************************************************************************F*/
static void _ProtoHttpProcessInfoHeader(ProtoHttpRefT *pState)
{
    // ignore the response
    NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] ignoring %d response from server\n", pState, pState->iHdrCode));
    
    // reset state to process next header
    pState->eState = ST_HEAD;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpGetHeader

    \Description
        Handles an informational response header (response code=1xx)

    \Input  *pState     - reference pointer

    \Output
        error code from sceHttpGetAllResponseHeaders

    \Version 11/30/2016 (amakoukji) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpGetHeader(ProtoHttpRefT *pState)
{
    int32_t iResult = 0;
    if ((iResult = sceHttpGetAllResponseHeaders(pState->iSceRequestId, &pState->pSceRespHeader, &pState->iSceRespHeaderLength)) < 0)
    {
        sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
        sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
        NetPrintf(("protohttpps4: [%p] sceHttpGetAllResponseHeaders failed with code %s for request id %d\n", pState, DirtyErrGetName(iResult), pState->iSceRequestId));
    }

    return(iResult);
}

/*F***************************************************************************/
/*!
    \Function _ProtoHttpFindPEMSignature

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
static const uint8_t *_ProtoHttpFindPEMSignature(const uint8_t *pCertData, int32_t iCertSize, const char *pSigText)
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

/*F********************************************************************************/
/*!
    \Function _ProtoHttpIsPEMFormat

    \Description
        Handles an informational response header (response code=1xx)

    \Input  *pCACert     - cert data
    \Input   iCertSize   - size of cert data

    \Output
        beginning of cert data if PEM format expected by Sony Lib, 0 otherwise.

    \Version 01/17/2017 (amakoukji) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpIsPEMFormat(const uint8_t *pCACert, int32_t iCertSize)
{
    const uint8_t *pCertData = pCACert;
    const uint8_t *pCertBeg = NULL;
    const uint8_t *pCertEnd = NULL;

    // make sure "end-cert" occurs after start since we support bundles
    if (((pCertBeg = _ProtoHttpFindPEMSignature(pCertData, iCertSize, _strPemBeg)) != NULL) &&
        ((pCertEnd = _ProtoHttpFindPEMSignature(pCertBeg, (int32_t)(pCertData+iCertSize-pCertBeg), _strPemEnd)) != NULL))
    {
        pCertBeg += strlen(_strPemBeg);
        return((int32_t)(pCertEnd-pCertBeg));
    }
    else if (((pCertBeg = _ProtoHttpFindPEMSignature(pCertData, iCertSize, _str509Beg)) != NULL) &&
             ((pCertEnd = _ProtoHttpFindPEMSignature(pCertBeg, (int32_t)(pCertData+iCertSize-pCertBeg), _str509End)) != NULL))
    {
        pCertBeg += strlen(_str509Beg);
        return((int32_t)(pCertEnd-pCertBeg));
    }
    return(0);

}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRedirectCallback

    \Description
        Handle redirection callback (response code=3xx)

    \Input iRequest      - sce request id
    \Input iStatusCode   - status code 
    \Input *pMethod      - http method
    \Input *pLocation    - location
    \Input *pUserArg     - reference pointer

    \Notes
        This method determines whether the sceHttp lib should process the redirect. 
        In our implementation we never let Sony handle the redirection, instead we determine 
        if we should start a new request under the hood.

    \Version 07/20/2016 (amakoukji) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRedirectCallback(int32_t iRequest, int32_t iStatusCode, int32_t *pMethod, const char *pLocation, void *pUserArg)
{
    ProtoHttpRefT *pState = (ProtoHttpRefT*)pUserArg;
    int32_t iResult = 0;
    NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] processing redirect for request 0x%08x code %d\n", pState, iRequest, iStatusCode));
    
    if ((iResult = _ProtoHttpGetHeader(pState)) >= 0)
    {
        // trigger recv header user callback, if specified
        pState->iHdrCode = ProtoHttpParseHeaderCode(pState->pSceRespHeader);

        // save the response header for processing later
        pState->QueuedRedData.uRespHeaderLen = pState->iSceRespHeaderLength;
        if ((pState->QueuedRedData.pRespHeader = DirtyMemAlloc(pState->QueuedRedData.uRespHeaderLen+1, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) != NULL)
        {
            ds_strnzcpy(pState->QueuedRedData.pRespHeader, pState->pSceRespHeader, pState->QueuedRedData.uRespHeaderLen);
        }
        else
        {
            NetPrintf(("protohttpps4: [%p] unable to allocate space to save off the redirect headers to be processed later\n", pState));
            pState->eState = ST_FAIL;
            pState->QueuedRedData.pRespHeader = NULL;
            pState->QueuedRedData.uRespHeaderLen = 0;
            return(-1);
        }
    }
    
    if ((pState->iMaxRedirect == 0) || (++pState->iNumRedirect > pState->iMaxRedirect))
    {
        if (pState->iMaxRedirect == 0) 
        {
            NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] auto-redirection disabled\n", pState));
        }
        else
        {
            NetPrintf(("protohttpps4: [%p] maximum number of redirections (%d) exceeded\n", pState, pState->iMaxRedirect));
        }
        pState->eState = ST_DONE;
        pState->uIdleTimeStart = NetTick();
        return(-1);
    }

    // do not auto-redirect multiplechoices or notmodified responses
    if ((pState->iHdrCode == PROTOHTTP_RESPONSE_MULTIPLECHOICES) || (pState->iHdrCode == PROTOHTTP_RESPONSE_NOTMODIFIED))
    {
        pState->eState = ST_DONE;
        pState->uIdleTimeStart = NetTick();
        return(-1);
    }
    // do not auto-redirect responses that are not head or get requests, and are not a SEEOTHER response
    if ((pState->eRequestType != PROTOHTTP_REQUESTTYPE_GET) && (pState->eRequestType != PROTOHTTP_REQUESTTYPE_HEAD))
    {
        /* As per HTTP 1.1 RFC, 303 SEEOTHER POST requests may be auto-redirected to a GET requset.  302 FOUND responses
           to a POST request are not supposed to be auto-redirected; however, there is a note in the RFC indicating that
           this is a common client behavior, and it is additionally a common server behavior to use 302 even when automatic
           redirection is intended, as some clients do not support 303 SEEOTHER.  Therefore, we perform auto-redirection
           on 302 FOUND responses to POST requests with a GET request for compatibility with servers that choose this
           behavior */
        if ((pState->iHdrCode == PROTOHTTP_RESPONSE_FOUND) || (pState->iHdrCode == PROTOHTTP_RESPONSE_SEEOTHER))
        {
            pState->eRequestType = PROTOHTTP_REQUESTTYPE_GET;
            pState->iPostSize = 0;            
        }
        else
        {
            pState->eState = ST_DONE;
            pState->uIdleTimeStart = NetTick();
            return(-1);
        }
    }

    // clear the header as we are done
    pState->pSceRespHeader = NULL;
    pState->iSceRespHeaderLength = 0;

    // set the state to do a redirection
    pState->eState = ST_REDIR;

    return(-1);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpCookieRecvCallback

    \Description
        Handle recv cookie callback ("Set-Cookie:" header)

    \Input request       - sce request id
    \Input *url          - url tied to the cookie 
    \Input *cookieHeader - the cookie
    \Input headerLen     - cookie length
    \Input *userArg      - reference pointer

    \Notes
        This method determines whether the sceHttp lib should process the cookie. 
        In our implementation we never let Sony handle the this, this is because 
        our protohttp.c implementation does not and we aim for parity.

    \Version 12/01/2016 (amakoukji) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpCookieRecvCallback(int request, const char *url, const char *cookieHeader, size_t headerLen, void *userArg)
{
#if DIRTYCODE_DEBUG
    ProtoHttpRefT *pState = (ProtoHttpRefT*)userArg;
    NetPrintfVerbose((pState->iVerbose, 2, "protohttpps4: [%p] received cookie header, %d bytes\n", pState, headerLen));
    NetPrintfVerbose((pState->iVerbose, 3, "protohttpps4: [%p] cookie: %s\n", pState, cookieHeader));
#endif
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSend

    \Description
        Try and send some data.  If data is sent, the timout value is updated.

    \Input  *pState     - reference pointer
    \Input  *pStrBuf    - pointer to buffer to send from
    \Input  iSize       - amount of data to try and send

    \Output
        int32_t         - negative=error, else number of bytes sent

    \Version 11/18/2004 (jbrookes) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSend(ProtoHttpRefT *pState, const char *pStrBuf, int32_t iSize)
{
    int32_t iResult = 0, iReturn = 0;
    const void *pPostData = NULL;
    int32_t iSizeFinal = iSize;
    uint8_t bDataHandled = FALSE;
    
    // note the SCE HTTP LIB support DELETE sending data but the ProtoHttp Api does not, otherwise it would be included here
    // note pState->iPostSize < 0 means chunked, pState->iPostSize > 0 is a typical POST and pState->iPostSize == 0 means no payload
    if ((pState->eRequestType == PROTOHTTP_REQUESTTYPE_POST || pState->eRequestType == PROTOHTTP_REQUESTTYPE_PUT || pState->eRequestType == PROTOHTTP_REQUESTTYPE_PATCH) && pState->iPostSize != 0)
    {
        if (iSize == 0 && pState->bChunked == FALSE)
        {
            // if we're expecting to have some data to post but there's none skip this, sceHttpSendRequest will fail and the transfer will be in a bad state
            // the exception is for chunked transfers where this denotes the end of the data
            return(0);
        }
        else if (pState->bRecvEndChunk)
        {
            // process defered end chunk
            pPostData = NULL;
            pState->bRecvEndChunk = FALSE;
        }
        else
        {
            // otherwise we can data and an expected data size, setup the send
            pPostData = (const void *)pStrBuf;
        }
    }

    // implement rate limiting 
    iSizeFinal = SocketRateThrottle(&(pState->SendRate), SOCK_STREAM, iSize, "send");

    if (iSizeFinal != iSize)
    {
        NetPrintfVerbose((pState->iVerbose, 4, "protohttpps4: [%p] data send throttled by max send rate from %d to %d bytes.\n", pState, iSize, iSizeFinal));
    }

    if ((iSizeFinal > 0) || (iSize == 0 && iSizeFinal == 0))
    {
        uint8_t bThreadAlive    = (pState->iThreadLife == 1);
        bDataHandled = FALSE;
        NetCritEnter(NULL); // protecting _ProtoHttp_State (1)
        uint8_t bCanStartThread = ((_ProtoHttp_State.iMaxThreads > _ProtoHttp_State.iCurrentThreads) && (pState->pPushDataThreadCond != NULL) && (pState->iThreadLife == 0));
        uint8_t bTooManyThreads = ((_ProtoHttp_State.iMaxThreads > 0) && (_ProtoHttp_State.iMaxThreads <= _ProtoHttp_State.iCurrentThreads));

        // if the worker thread is alive or allowed to start; start it and process asynchronously, otherwise process the data synchronously
        if ((bThreadAlive == TRUE) || (bCanStartThread == TRUE))
        {
            iReturn = 0;

            // if the thread needs to be started, start it
            if ((bThreadAlive == FALSE) && (bCanStartThread == TRUE))
            {
                ++(_ProtoHttp_State.iCurrentThreads);   //presume the thread creation will be successful so we don't over commit _ProtoHttp_State.iMaxThreads
                NetCritLeave(NULL); // finished with _ProtoHttp_State (1)
                DirtyThreadConfigT ThreadConfig;
                ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
                ThreadConfig.pName = "ProtoHttp";
                ThreadConfig.iAffinity = NetConnStatus('affn', 0, NULL, 0);
                ThreadConfig.iPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
                ThreadConfig.iVerbosity = pState->iVerbose;

                // create thread
                pState->iThreadLife = 1;
                if ((iResult = DirtyThreadCreate(_ProtoHttpThread, pState, &ThreadConfig)) != 0)
                {
                    NetCritEnter(NULL);
                    --(_ProtoHttp_State.iCurrentThreads);   //correct the number of threads since this one didn't get made
                    NetCritLeave(NULL);
                    NetPrintf(("protohttpps4: [%p] unable to create http worker thread (err=%d)\n", pState, iResult));
                    pState->iThreadLife = 0;
                }
            }
            else
            {
                NetCritLeave(NULL); // finished with _ProtoHttp_State (1)
            }

            // verify that the thread is alive, if it is process the data
            if (pState->iThreadLife == 1)
            {
                /* note that for the below, if we received a result in iPushDataResult the pPostData pointer may no longer be valid, 
                   thus in this case regardless of the value of bPushDataInputAvailable we fall through to allow the user to process 
                   the result we received and pass us an updated pPostData the next time they call the http send method */

                // check output from thread
                if (pState->bPushDataResultAvailable == TRUE)
                {
                    // consume the result
                    iReturn = pState->iPushDataResult;
                    pState->bPushDataResultAvailable = FALSE;
                }
                // push input to thread if the thread is not already waiting to process queued data AND we did not receive a result above
                else if (pState->bPushDataInputAvailable == FALSE)
                {
                    pState->QueuedPushData.pStrBuf = pStrBuf;
                    pState->QueuedPushData.pPostData = pPostData;
                    pState->QueuedPushData.iSize = iSize;
                    pState->QueuedPushData.iSizeFinal = iSizeFinal;
                    pState->bPushDataInputAvailable = TRUE;
                }

                // signal the worker to wake it up
                NetCritEnter(&pState->PushDataCrit);
                if (!DirtyConditionSignal(pState->pPushDataThreadCond))
                {
                    NetPrintf(("protohttpps4: [%p] failed to signal worker thread.\n", pState));
                }
                NetCritLeave(&pState->PushDataCrit);

                // mark the data as processed
                bDataHandled = TRUE;
            }
        }
        else
        {
            NetCritLeave(NULL); // finished with _ProtoHttp_State (1)
        }
        
        if (bDataHandled == FALSE)
        {
            if (bTooManyThreads)
            {
                // if worker threads are being utilized but no more threads are allowed to spawn return an error 
                NetPrintf(("protohttpps4: [%p] unable to start worker thread.\n", pState));
                iReturn = PROTOHTTP_EAGAIN;
                pState->bNoThreads = TRUE;
            }
            else
            {
                // otherwise do the work in the current thread
                // as of PS4 SDK 5.5 for HTTPS requests this can take up to 30ms to complete
                iReturn = _ProtoHttpSendRequestImpl(pState, pStrBuf, pPostData, iSize, iSizeFinal);
            }
        }
    }
    else
    {
        iReturn = 0;
    }

    return(iReturn);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSendBuff

    \Description
        Send data queued in buffer.

    \Input  *pState     - reference pointer

    \Output
        int32_t         - negative=error, else number of bytes sent

    \Version 01/29/2010 (jbrookes) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSendBuff(ProtoHttpRefT *pState)
{
    int32_t iResult, iSentSize = 0;

    if ((iResult = _ProtoHttpSend(pState, pState->pInpBuf+pState->iInpOff, pState->iInpLen)) > 0)
    {
        pState->iInpOff += iResult;
        pState->iInpLen -= iResult;
        if (pState->iInpLen == 0)
        {
            pState->iInpOff = 0;
        }
        iSentSize = iResult;
    }
    else if (iResult < 0)
    {
        NetPrintf(("protohttpps4: [%p] failed to send request data (err=%s)\n", pState, DirtyErrGetName(iResult)));
        pState->iInpLen = 0;
        pState->eState = ST_FAIL;
        iSentSize = -1;
    }
    return(iSentSize);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRecvData

    \Description
        Try and receive some data.  If data is received, the timout value is
        updated.

    \Input *pState      - reference pointer
    \Input *pStrBuf     - [out] pointer to buffer to receive into
    \Input iSize        - amount of data to try and receive

    \Output
        int32_t         - negative=error, else success

    \Version 11/12/2004 (jbrookes) First Version.
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRecvData(ProtoHttpRefT *pState, char *pStrBuf, int32_t iSize)
{
    // if we have no buffer space, don't try to receive
    if (iSize == 0)
    {
        return(0);
    }

    // try and receive some data
    if ((pState->iRecvRslt = sceHttpReadData(pState->iSceRequestId, (void*)pStrBuf, (size_t)iSize)) > 0)
    {
        #if DIRTYCODE_LOGGING
        if (pState->iVerbose > 2)
        {
            NetPrintf(("protohttpps4: [%p] recv %d bytes\n", pState, pState->iRecvRslt));
        }
        if (pState->iVerbose > 3)
        {
            NetPrintMem(pStrBuf, pState->iRecvRslt, "http-recv");
        }
        #endif

        // received data, so update timeout
        pState->uTimer = NetTick() + pState->uTimeout;

        // update data rate estimation
        SocketRateUpdate(&(pState->RecvRate), pState->iRecvRslt, "recv");
    }
    return(pState->iRecvRslt);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpHeaderProcess

    \Description
        Check for header completion and process header data

    \Input *pState          - reference pointer

    \Version 05/03/2012 (jbrookes) Refactored out of ProtoHttpUpdate()
*/
/********************************************************************************F*/
static void _ProtoHttpHeaderProcess(ProtoHttpRefT *pState)
{
    // try parsing header
    if (_ProtoHttpParseHeader(pState) < 0)
    {
        // was there a prior SOCKERR_CLOSED error?
        if (pState->iRecvRslt < 0)
        {
            NetPrintf(("protohttpps4: [%p] ST_HEAD append got ST_FAIL (err=%d, len=%d)\n", pState, pState->iRecvRslt, pState->iInpLen));
            pState->eState = ST_FAIL;
        }
        // if the buffer is full, we don't have enough room to receive the header
        if (pState->iInpLen == pState->iInpMax)
        {
            if (pState->iInpOvr == 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] input buffer too small for response header\n", pState));
            }
            pState->iInpOvr = pState->iInpLen+1;
        }
        return;
    }

    /* workaround for non-compliant 1.0 servers - some 1.0 servers specify a
       Content Length of zero invalidly.  if the response is a 1.0 response
       and the Content Length is zero, and we've gotten body data, we set
       the Content Length to -1 (indeterminate) */
    if ((pState->bHttp1_0 == TRUE) && (pState->iBodySize == 0) && (pState->iInpCnt > 0))
    {
        pState->iBodySize = -1;
    }

    // handle final processing
    if ((pState->bHeadOnly == TRUE) || (pState->iHdrCode == PROTOHTTP_RESPONSE_NOCONTENT) || (pState->iHdrCode == PROTOHTTP_RESPONSE_NOTMODIFIED))
    {
        // only needed the header (or response did not include a body; see HTTP RFC 1.1 section 4.4) -- all done
        pState->eState = ST_DONE;
        pState->uIdleTimeStart = NetTick();
    }
    else if ((pState->iBodySize >= 0) && (pState->iInpCnt >= pState->iBodySize) && (!pState->bChunked))
    {
        // we got entire body with header -- all done
        pState->eState = ST_DONE;
        pState->uIdleTimeStart = NetTick();
    }
    else
    {
        // wait for rest of body
        pState->eState = ST_BODY;
        pState->uIdleTimeStart = NetTick();
    }

    // handle response code?
    if (PROTOHTTP_GetResponseClass(pState->iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL)
    {
        _ProtoHttpProcessInfoHeader(pState);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRecvBody

    \Description
        Attempt to recevive and process body data

    \Input *pState          - reference pointer

    \Output
        int32_t             - zero=no data available

    \Version 05/03/2012 (jbrookes) Refactored out of ProtoHttpUpdate()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRecvBody(ProtoHttpRefT *pState)
{
    int32_t iResult;

    // reset pointers if needed
    if ((pState->iInpLen > 0) && (pState->iInpOff == pState->iInpLen))
    {
        pState->iInpOff = pState->iInpLen = 0;
    }

    // check for data
    iResult = pState->iInpMax - pState->iInpLen;
    if (iResult <= 0)
    {
        // always return zero bytes since buffer is full
        return(0);
    }
    // implement rate limiting
    else if ((iResult = SocketRateThrottle(&pState->RecvRate, SOCK_STREAM, iResult, "recv")) == 0)
    {
        NetPrintfVerbose((pState->iVerbose, 4, "protohttpps4: [%p] data recv throttled by max recv rate.\n", pState));
        return(0);
    }
    else
    {
        // try to add to buffer
        iResult = _ProtoHttpRecvData(pState, pState->pInpBuf+pState->iInpLen, iResult);
    }
    if (iResult == 0)
    {
        // this marks the end of a chunked transaction
        pState->eState = ST_DONE;
        pState->uIdleTimeStart = NetTick();

        // if we doing a chunked / no content-length download, update our final body size (like protohttp)
        if (pState->iBodySize < 0)
        {
            pState->iBodySize = pState->iInpCnt;
        }
        return(0);
    }
    // check for connection close
    else if (iResult > 0)
    {
        // add to buffer
        pState->iInpLen += iResult;
        pState->iInpCnt += iResult;

        // check for end of body
        if ((pState->iBodySize >= 0) && (pState->iInpCnt >= pState->iBodySize))
        {
            pState->eState = ST_DONE;
            pState->uIdleTimeStart = NetTick();
        }
    }
    else if (iResult < 0 && iResult != SCE_HTTP_ERROR_BUSY && iResult != SCE_HTTP_ERROR_EAGAIN)
    {
        NetPrintf(("protohttpps4: [%p] ST_FAIL (err=%s)\n", pState, DirtyErrGetName(iResult)));
        pState->eState = ST_FAIL;
        pState->iSslFail = 0;
        pState->uSslFailDet = 0;
        pState->iHresult = 0;
        sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
        sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRecv

    \Description
        Return the actual url data.

    \Input *pState  - reference pointer
    \Input *pBuffer - buffer to store data in
    \Input iBufMin  - minimum number of bytes to return (returns zero if this number is not available)
    \Input iBufMax  - maximum number of bytes to return (buffer size)

    \Output
        int32_t     - negative=error, zero=no data available or bufmax <= 0, positive=number of bytes read

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRecv(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    int32_t iLen;

    // early out for failure result
    if (pState->eState == ST_FAIL)
        return(pState->bTimeout ? PROTOHTTP_TIMEOUT : PROTOHTTP_RECVFAIL);
    // check for input buffer too small for header
    if (pState->iInpOvr > 0)
        return(PROTOHTTP_MINBUFF);
    // waiting for data
    if ((pState->eState != ST_BODY) && (pState->eState != ST_DONE))
        return(PROTOHTTP_RECVWAIT);

    // if they only wanted head, thats all they get
    if (pState->bHeadOnly == TRUE)
        return(PROTOHTTP_RECVHEAD);

    // if they are querying only for done state when no more data is available to be read
    if((iBufMax == 0) && ((pState->eState == ST_DONE) && (pState->iBodyRcvd == pState->iBodySize)))
        return(PROTOHTTP_RECVDONE);

    // make sure range is valid
    if (iBufMax < 1)
        return(0);

    // clamp the range
    if (iBufMin < 1)
        iBufMin = 1;
    if (iBufMax < iBufMin)
        iBufMax = iBufMin;
    if (iBufMin > pState->iInpMax)
        iBufMin = pState->iInpMax;
    if (iBufMax > pState->iInpMax)
        iBufMax = pState->iInpMax;

    // see if we need to shift buffer
    if ((iBufMin > pState->iInpMax-pState->iInpOff) || (pState->bCompactRecv == TRUE))
    {
        // compact receive buffer
        _ProtoHttpCompactBuffer(pState);
        // give chance to get more data
        _ProtoHttpRecvBody(pState);
    }

    // figure out how much data is available
    if ((iLen = (pState->iInpLen - pState->iInpOff)) > iBufMax)
    {
        iLen = iBufMax;
    }

    // check for end of data
    if ((iLen == 0) && (pState->eState == ST_DONE))
    {
        return(PROTOHTTP_RECVDONE);
    }

    // see if there is enough to return
    if ((iLen >= iBufMin) || (pState->iInpCnt == pState->iBodySize))
    {
        // return data to caller
        if (pBuffer != NULL)
        {
            ds_memcpy(pBuffer, pState->pInpBuf+pState->iInpOff, iLen);

            #if DIRTYCODE_LOGGING
            if (pState->iVerbose > 3)
            {
                NetPrintf(("protohttpps4: [%p] read %d bytes\n", pState, iLen));
            }
            if (pState->iVerbose > 4)
            {
                NetPrintMem(pBuffer, iLen, "http-read");
            }
            #endif
        }
        pState->iInpOff += iLen;
        pState->iBodyRcvd += iLen;
        
        // return bytes read
        return(iLen);
    }

    // nothing available
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpWriteCbProcess

    \Description
        User write callback processing, if write callback is set

    \Input *pState          - reference pointer

    \Version 05/03/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpWriteCbProcess(ProtoHttpRefT *pState)
{
    ProtoHttpWriteCbInfoT WriteCbInfo;
    int32_t iResult;

    ds_memclr(&WriteCbInfo, sizeof(WriteCbInfo));
    WriteCbInfo.eRequestType = pState->eRequestType;
    WriteCbInfo.eRequestResponse = (ProtoHttpResponseE)pState->iHdrCode;

    /* download more data when the following are true: we are in the body state or we are in the done state but we haven't received
       everything. note, the body size is negative for chunked transfers when we haven't processed the end chunk. */
    if ((pState->eState == ST_BODY) || ((pState->eState == ST_DONE) && ((pState->iBodySize < 0) || (pState->iBodyRcvd < pState->iBodySize))))
    {
        char strTempRecv[1024];
        while ((iResult = _ProtoHttpRecv(pState, strTempRecv, 1, sizeof(strTempRecv))) > 0)
        {
            pState->pWriteCb(pState, &WriteCbInfo, strTempRecv, iResult, pState->pUserData);
        }
    }

    if (pState->eState > ST_BODY)
    {
        if (pState->eState == ST_DONE)
        {
            pState->pWriteCb(pState, &WriteCbInfo, "", pState->bHeadOnly ? PROTOHTTP_HEADONLY : PROTOHTTP_RECVDONE, pState->pUserData);
        }
        if (pState->eState == ST_FAIL)
        {
            pState->pWriteCb(pState, &WriteCbInfo, "", pState->bTimeout ? PROTOHTTP_TIMEOUT :PROTOHTTP_RECVFAIL, pState->pUserData);
        }
        pState->pWriteCb = NULL;
        pState->pReqCustomHeaderCb = NULL;
        pState->pReqReceiveHeaderCb = NULL;
        pState->pUserData = NULL;
    }
}

/*** Public Functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function ProtoHttpCreate

    \Description
        Allocate module state and prepare for use

    \Input iBufSize     - length of receive buffer

    \Output
        ProtoHttpRefT * - pointer to module state, or NULL

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
ProtoHttpRefT *ProtoHttpCreate(int32_t iBufSize)
{
    ProtoHttpRefT *pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;


    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // clamp the buffer size
    if (iBufSize < 4096)
    {
        iBufSize = 4096;
    }
        
    // allocate the resources
    if ((pState = DirtyMemAlloc(sizeof(*pState), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpps4: [%p] unable to allocate module state\n", pState));
        return(NULL);
    }
    ds_memclr(pState, sizeof(*pState));
    // save memgroup (will be used in ProtoHttpDestroy)
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->iNetMemId = -1; 
    pState->iSslContextId = -1;
    pState->iHttpContextId = -1;

    if ((pState->pInpBuf = DirtyMemAlloc(iBufSize, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpps4: [%p] unable to allocate protohttp buffer\n", pState));
        ProtoHttpDestroy(pState);
        return(NULL);
    }

    // set the loaded list version to -1 to be sure the GOS CAs get loaded
    pState->uCaListVersionLoaded = -1;

    // init crit
    NetCritInit(&pState->HttpCrit, "ProtoHttp");

    // save parms & set defaults
    pState->eState = ST_IDLE;
    pState->iInpMax = iBufSize;
    pState->uTimeout = PROTOHTTP_TIMEOUT_DEFAULT;
    pState->iResolveRetries = PROTOHTTP_RESOLVE_RETRIES;

    pState->bVerifyHdr = TRUE;
    pState->iVerbose = 1;
    pState->iMaxRedirect = PROTOHTTP_MAXREDIRECT;
    pState->Vers = SCE_SSL_VERSION_TLS1_2;
    // note: SCE_HTTPS_FLAGS_DEFAULT would normally be used but seems to contain an undeclared flag, use the equivalent
    pState->uSceHttpFlags = SCE_HTTPS_FLAG_SERVER_VERIFY | SCE_HTTPS_FLAG_CN_CHECK | SCE_HTTPS_FLAG_KNOWN_CA_CHECK | SCE_HTTPS_FLAG_SNI;
    pState->bSetSslVers = FALSE;
    pState->bSetHttpsFlags = FALSE;
    pState->bEpollInitialized = FALSE;

    pState->uIdleTimeStart = 0;
    pState->iIdleTimeMax = PROTOHTTP_KEEPALIVE_MAX_IDLE_DEFAULT;

    if ((pState->pPushDataThreadCond = DirtyConditionCreate("WakeupHttpWorkerThread")) == NULL)
    {
        // do not treat this as a fatal error, simple use the synchronous path
        NetPrintf(("protohttpps4: [%p]  failed to create condition, worker thread will not be spawned\n", pState));
    }
    NetCritInit(&pState->PushDataCrit, "WakeupHttpWorkerThreadMutex");
    
    // note: 1st party http contexts are created when a request is issued
    //       this helps avoid the issue of running out of net contexts and squatting on 
    //       system memory for long amounts of time when many Http refs are created and 
    //       left idle indefinately as is often the case with protohttpmanager.

    // return the state
    return(pState);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCallback

    \Description
        Set header callbacks.

    \Input *pState          - module state
    \Input *pCustomHeaderCb - pointer to custom send header callback (may be NULL)
    \Input *pReceiveHeaderCb- pointer to recv header callback (may be NULL)
    \Input *pCallbackRef    - user-supplied callback ref (may be NULL)

    \Notes
        The ProtHttpCustomHeaderCbt callback is used to allow customization of
        the HTTP header before sending.  It is more powerful than the append
        header functionality, allowing to make changes to any part of the header
        before it is sent.  The callback should return a negative code if an error
        occurred, zero can be returned if the application wants ProtoHttp to
        calculate the header size, or the size of the header can be returned if
        the application has already calculated it.  The header should *not* be
        terminated with the double \\r\\n that indicates the end of the entire
        header, as protohttp appends itself. The pData and iDataLen parameters are
        unused and we will pass NULL and 0 to them.

        The ProtoHttpReceiveHeaderCbT callback is used to view the header
        immediately on reception.  It can be used when the built-in header
        cache (retrieved with ProtoHttpStatus('htxt') is too small to hold
        the entire header received.  It is also possible with this method
        to view redirection response headers that cannot be retrieved
        normally.  This can be important if, for example, the application
        wishes to attach new cookies to a redirection response.  The
        custom response header and custom header callback can be used in
        conjunction to implement this type of functionality.

    \Version 1.0 02/16/2007 (jbrookes) First Version
*/
/********************************************************************************F*/
void ProtoHttpCallback(ProtoHttpRefT *pState, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pCallbackRef)
{
    pState->pCustomHeaderCb = pCustomHeaderCb;
    pState->pReceiveHeaderCb = pReceiveHeaderCb;
    pState->pCallbackRef = pCallbackRef;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDestroy

    \Description
        Destroy the module and release its state

    \Input *pState      - reference pointer

    \Output
        None.

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
void ProtoHttpDestroy(ProtoHttpRefT *pState)
{
    int32_t iResult = 0;

    if (pState->iThreadLife == 1)
    {
        _ProtoHttpReleaseWorkerThread(pState);
    }

    // wait for the thread to finish gracefully
    while (pState->iThreadLife > 0)
    {
        sceKernelUsleep(1);
    }

    if (pState->pPushDataThreadCond != NULL)
    {
        DirtyConditionDestroy(pState->pPushDataThreadCond);
        pState->pPushDataThreadCond = NULL;
    }

    NetCritKill(&pState->PushDataCrit);

    if (pState->pInpBuf != NULL)
    {
        DirtyMemFree(pState->pInpBuf, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }
    if (pState->pAppendHdr != NULL)
    {
        DirtyMemFree(pState->pAppendHdr, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }
    if (pState->QueuedRedData.pRespHeader != NULL)
    {
        DirtyMemFree(pState->QueuedRedData.pRespHeader, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }
    
    if (pState->iSceRequestId != 0)
    {
        sceHttpUnsetEpoll(pState->iSceRequestId);
        iResult = sceHttpDeleteRequest(pState->iSceRequestId);
        if (iResult != SCE_OK)
        {
            NetPrintf(("protohttpps4: [%p] unable to delete request %d, err=%s\n", pState, pState->iSceRequestId, DirtyErrGetName(iResult)));
        }
    }
    if (pState->iSceConnId != 0)
    {
        sceHttpDeleteConnection(pState->iSceConnId);
    }
    if (pState->iSceTemplateId != 0)
    {
        sceHttpDeleteTemplate(pState->iSceTemplateId);
    }
    if (pState->bEpollInitialized)
    {
        sceHttpDestroyEpoll(pState->iHttpContextId, pState->EpollHandle);
        pState->bEpollInitialized = FALSE;
    }

    NetCritKill(&pState->HttpCrit);
    _ProtoHttpFreeContexts(pState);
    DirtyMemFree(pState, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGet

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a transfer
        from the appropriate server.

    \Input *pState      - reference pointer
    \Input *pUrl        - the url to retrieve
    \Input bHeadOnly    - if TRUE only get header

    \Output
        int32_t         - negative=failure, else success

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpGet(ProtoHttpRefT *pState, const char *pUrl, uint32_t bHeadOnly)
{
    int32_t iResult = 0;

    // reset redirection count
    pState->iNumRedirect = 0;

    pState->iPostSize = 0;
    pState->pData = NULL;

    // format the request
    if (pUrl != NULL)
    {
        if ((iResult = _ProtoHttpFormatRequest(pState, pUrl, NULL, 0, bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET)) < 0)
        {
            return(iResult);
        }
        iResult = _ProtoHttpSendRequest(pState);

        // special case when there are no worker threads available, if we do not detect this here this will return 0 and moved to ST_FAIL 
        // the next time ProtoHttpUpdate() is called
        if (pState->bNoThreads == TRUE)
        {
            pState->bNoThreads = FALSE;
            return(PROTOHTTP_EAGAIN);
        }
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecv

    \Description
        Return the actual url data.

    \Input *pState  - reference pointer
    \Input *pBuffer - buffer to store data in
    \Input iBufMin  - minimum number of bytes to return (returns zero if this number is not available)
    \Input iBufMax  - maximum number of bytes to return (buffer size)

    \Output
        int32_t     - negative=error, zero=no data available or bufmax <= 0, positive=number of bytes read

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpRecv(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    int32_t iResult;

    NetCritEnter(&pState->HttpCrit);
    iResult = _ProtoHttpRecv(pState, pBuffer, iBufMin, iBufMax);
    NetCritLeave(&pState->HttpCrit);

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecvAll

    \Description
        Return all of the url data.

    \Input *pState  - reference pointer
    \Input *pBuffer - buffer to store data in
    \Input iBufSize - size of buffer

    \Output
        int32_t     - PROTOHTTP_RECV*, or positive=bytes in response

    \Version 12/14/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpRecvAll(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufSize)
{
    int32_t iRecvMax, iRecvResult;

    // try to read as much data as possible (leave one byte for null termination)
    for (iRecvMax = iBufSize-1; (iRecvResult = ProtoHttpRecv(pState, pBuffer + pState->iRecvSize, 1, iRecvMax - pState->iRecvSize)) > 0; )
    {
        // add to amount received
        pState->iRecvSize += iRecvResult;
    }

    // check response code
    if (iRecvResult == PROTOHTTP_RECVDONE)
    {
        // null-terminate response & return completion success
        if ((pBuffer != NULL) && (iBufSize > 0))
        {
            pBuffer[pState->iRecvSize] = '\0';
        }
        iRecvResult = pState->iRecvSize;
    }
    else if ((iRecvResult < 0) && (iRecvResult != PROTOHTTP_RECVWAIT))
    {
        // an error occurred
        NetPrintf(("protohttpps4: [%p] error %d receiving response\n", pState, iRecvResult));
    }
    else if (iRecvResult == 0)
    {
        iRecvResult = (pState->iRecvSize < iRecvMax) ? PROTOHTTP_RECVWAIT : PROTOHTTP_RECVBUFF;
    }

    // return result to caller
    return(iRecvResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPost

    \Description
        Initiate transfer of data to to the server via a HTTP POST command.

    \Input *pState      - reference pointer
    \Input *pUrl        - the URL that identifies the POST action.
    \Input *pData       - pointer to URL data (optional, may be NULL)
    \Input iDataSize    - size of data being uploaded (see Notes below)
    \Input bDoPut       - if TRUE, do a PUT instead of a POST

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Notes
        Any data that is not sent as part of the Post transaction should be sent
        with ProtoHttpSend().  ProtoHttpSend() should be called at poll rate (i.e.
        similar to how often ProtoHttpUpdate() is called) until all of the data has
        been sent.  The amount of data bytes actually sent is returned by the
        function.

        If pData is not NULL and iDataSize is less than or equal to zero, iDataSize
        will be recalculated as the string length of pData, to allow for easy string
        sending.

        If pData is NULL and iDataSize is negative, the transaction is assumed to
        be a streaming transaction and a chunked transfer will be performed.  A
        subsequent call to ProtoHttpSend() with iDataSize equal to zero will end
        the transaction.

    \Version 03/03/2004 (sbevan) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpPost(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int64_t iDataSize, uint32_t bDoPut)
{
    int32_t iDataSent;
    // allow for easy string sending
    if ((pData != NULL) && (iDataSize <= 0))
    {
        iDataSize = (int32_t)strlen(pData);
    }
    // save post size (or -1 to indicate that this is a variable-length stream)
    pState->iPostSize = iDataSize;
    pState->pData = pData;

    // make the request
    if ((iDataSent = _ProtoHttpFormatRequest(pState, pUrl, pData, iDataSize, bDoPut ? PROTOHTTP_REQUESTTYPE_PUT : PROTOHTTP_REQUESTTYPE_POST)) >= 0)
    {
        // send the request
        iDataSent = _ProtoHttpSendRequest(pState);

        // special case when there are no worker threads available, if we do not detect this here this will return 0 and moved to ST_FAIL 
        // the next time ProtoHttpUpdate() is called
        if (pState->bNoThreads == TRUE)
        {
            pState->bNoThreads = FALSE;
            return(PROTOHTTP_EAGAIN);
        }
    }
    return(iDataSent);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSend

    \Description
        Send data during an ongoing Post transaction.

    \Input *pState      - reference pointer
    \Input *pData       - pointer to data to send
    \Input iDataSize    - size of data being sent

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Version 11/18/2004 (jbrookes) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpSend(ProtoHttpRefT *pState, const char *pData, int32_t iDataSize)
{
    int32_t iResult;

    // handle use case where data is specified after the ProtoHttpPost() call
    if (pState->bWaitForData > 0 && pData != NULL)
    {
        pState->eState = ST_RESP;
        pState->bWaitForData = FALSE;
    }

    // make sure we are in a sending state
    if (pState->eState < ST_RESP)
    {
        // not ready to send data yet
        return(0);
    }
    else if (pState->eState != ST_RESP)
    {
        // if we're past ST_RESP, an error occurred.
        return(-1);
    }

    /* clamp to max ProtoHttp buffer size - even though
       we don't queue the data in the ProtoHttp buffer, this
       gives us a reasonable max size to send in one chunk */
    if (iDataSize > pState->iInpMax)
    {
        iDataSize = pState->iInpMax;
    }

    // get sole access to httpcrit
    NetCritEnter(&pState->HttpCrit);
    // send the data
    
    iResult = _ProtoHttpSend(pState, pData, iDataSize);
    // release access to httpcrit
    NetCritLeave(&pState->HttpCrit);

    // return result
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDelete

    \Description
        Request deletion of a server-based resource.

    \Input *pState      - reference pointer
    \Input *pUrl        - Url describing reference to delete

    \Output
        int32_t         - negative=failure, zero=success

    \Version 06/01/2009 (jbrookes) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpDelete(ProtoHttpRefT *pState, const char *pUrl)
{
    int32_t iResult;

    // reset redirection count
    pState->iNumRedirect = 0;

    // delete with data not supported
    pState->iPostSize = 0;
    pState->pData = NULL;

    // format the request
    if ((iResult = _ProtoHttpFormatRequest(pState, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_DELETE)) >= 0)
    {
        // issue the request
        iResult = _ProtoHttpSendRequest(pState);

        // special case when there are no worker threads available, if we do not detect this here this will return 0 and moved to ST_FAIL 
        // the next time ProtoHttpUpdate() is called
        if (pState->bNoThreads == TRUE)
        {
            pState->bNoThreads = FALSE;
            return(PROTOHTTP_EAGAIN);
        }
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpOptions

    \Description
        Request options from a server.

    \Input *pState      - reference pointer
    \Input *pUrl        - Url describing reference to get options on

    \Output
        int32_t         - negative=failure, zero=success

    \Version 07/14/2010 (jbrookes) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpOptions(ProtoHttpRefT *pState, const char *pUrl)
{
    int32_t iResult;

    // reset redirection count
    pState->iNumRedirect = 0;

    pState->iPostSize = 0;
    pState->pData = NULL;

    // format the request
    if ((iResult = _ProtoHttpFormatRequest(pState, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_OPTIONS)) >= 0)
    {
        // issue the request
        iResult = _ProtoHttpSendRequest(pState);

        // special case when there are no worker threads available, if we do not detect this here this will return 0 and moved to ST_FAIL 
        // the next time ProtoHttpUpdate() is called
        if (pState->bNoThreads == TRUE)
        {
            pState->bNoThreads = FALSE;
            return(PROTOHTTP_EAGAIN);
        }
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPatch

    \Description
        Initiate transfer of data to to the server via a HTTP PATCH command.

    \Input *pState      - reference pointer
    \Input *pUrl        - the URL that identifies the POST action.
    \Input *pData       - pointer to URL data (optional, may be NULL)
    \Input iDataSize    - size of data being uploaded (see Notes below)

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Notes
        Any data that is not sent as part of the Patch transaction should be sent
        with ProtoHttpSend().  ProtoHttpSend() should be called at poll rate (i.e.
        similar to how often ProtoHttpUpdate() is called) until all of the data has
        been sent.  The amount of data bytes actually sent is returned by the
        function.

        If pData is not NULL and iDataSize is less than or equal to zero, iDataSize
        will be recalculated as the string length of pData, to allow for easy string
        sending.

        If pData is NULL and iDataSize is negative, the transaction is assumed to
        be a streaming transaction and a chunked transfer will be performed.  A
        subsequent call to ProtoHttpSend() with iDataSize equal to zero will end
        the transaction.

    \Version 01/01/2017 (amakoukji) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpPatch(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int64_t iDataSize)
{
    int32_t iDataSent;
    // allow for easy string sending
    if ((pData != NULL) && (iDataSize <= 0))
    {
        iDataSize = (int32_t)strlen(pData);
    }
    // save post size (or -1 to indicate that this is a variable-length stream)
    pState->iPostSize = iDataSize;
    pState->pData = pData;

    // make the request
    if ((iDataSent = _ProtoHttpFormatRequest(pState, pUrl, pData, iDataSize, PROTOHTTP_REQUESTTYPE_PATCH)) >= 0)
    {
        // send the request
        iDataSent = _ProtoHttpSendRequest(pState);

        // special case when there are no worker threads available, if we do not detect this here this will return 0 and moved to ST_FAIL 
        // the next time ProtoHttpUpdate() is called
        if (pState->bNoThreads == TRUE)
        {
            pState->bNoThreads = FALSE;
            return(PROTOHTTP_EAGAIN);
        }
    }
    return(iDataSent);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRequestCb2

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a transfer
        from the appropriate server.  Use ProtoHttpRequest() macro wrapper if
        callback is not required.

    \Input *pState              - reference pointer
    \Input *pUrl                - the url to retrieve
    \Input *pData               - user data to send to server (PUT and POST only)
    \Input iDataSize            - size of user data to send to server (PUT and POST only)
    \Input eRequestType         - request type to make
    \Input *pWriteCb            - write callback (optional)
    \Input *pCustomHeaderCb     - custom header callback (optional)
    \Input *pReceiveHeaderCb    - receive header callback (optional)
    \Input *pUserData           - callback user data (optional)

    \Output
        int32_t         - negative=failure, zero=success, >0=number of data bytes sent (PUT and POST only)

    \Version 09/11/2011 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpRequestCb2(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int64_t iDataSize, ProtoHttpRequestTypeE eRequestType, ProtoHttpWriteCbT *pWriteCb, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pUserData)
{
    // save callbacks
    pState->pWriteCb = pWriteCb;
    pState->pReqCustomHeaderCb = pCustomHeaderCb;
    pState->pReqReceiveHeaderCb = pReceiveHeaderCb;
    pState->pUserData = pUserData;

    // make the request
    if ((eRequestType == PROTOHTTP_REQUESTTYPE_GET) || (eRequestType == PROTOHTTP_REQUESTTYPE_HEAD))
    {
        return(ProtoHttpGet(pState, pUrl, eRequestType == PROTOHTTP_REQUESTTYPE_HEAD));
    }
    else if ((eRequestType == PROTOHTTP_REQUESTTYPE_PUT) || (eRequestType == PROTOHTTP_REQUESTTYPE_POST))
    {
        return(ProtoHttpPost(pState, pUrl, pData, iDataSize, eRequestType == PROTOHTTP_REQUESTTYPE_PUT));
    }
    else if (eRequestType == PROTOHTTP_REQUESTTYPE_DELETE)
    {
        return(ProtoHttpDelete(pState, pUrl));
    }
    else if (eRequestType == PROTOHTTP_REQUESTTYPE_OPTIONS)
    {
        return(ProtoHttpOptions(pState, pUrl));
    }
    else if (eRequestType == PROTOHTTP_REQUESTTYPE_PATCH)
    {
        return(ProtoHttpPatch(pState, pUrl, pData, iDataSize));
    }
    else
    {
        NetPrintf(("protohttpps4: [%p] unrecognized request type %d\n", pState, eRequestType));
        return(-1);
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpAbort

    \Description
        Abort current operation, if any.

    \Input *pState      - reference pointer

    \Output
        None.

    \Version 12/01/2004 (jbrookes) First Version
*/
/********************************************************************************F*/
void ProtoHttpAbort(ProtoHttpRefT *pState)
{
    // acquire sole access to http crit
    NetCritEnter(&pState->HttpCrit);

    // terminate current connection, if any
    _ProtoHttpClose(pState, "abort");

    // reset state
    _ProtoHttpReset(pState);

    // release sole access to http crit
    NetCritLeave(&pState->HttpCrit);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetBaseUrl

    \Description
        Set base url that will be used for any relative url references.

    \Input *pState      - module state
    \Input *pUrl        - base url

    \Output
        None

    \Version 02/03/2010 (jbrookes)
*/
/********************************************************************************F*/
void ProtoHttpSetBaseUrl(ProtoHttpRefT *pState, const char *pUrl)
{
    char strHost[sizeof(pState->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url for kind, host, and port
    ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // set base info
    NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting base url to %s://%s:%d\n", pState, iSecure ? "https" : "http", strHost, iPort));
    ds_strnzcpy(pState->strBaseHost, strHost, sizeof(pState->strBaseHost));
    pState->iBasePort = iPort;
    pState->iBaseSecure = iSecure;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGetLocationHeader

    \Description
        Get location header from the input buffer.  The Location header requires
        some special processing.

    \Input *pState  - reference pointer
    \Input *pInpBuf - buffer holding header text
    \Input *pBuffer - [out] output buffer for parsed location header, null for size request
    \Input iBufSize - size of output buffer, zero for size request
    \Input **pHdrEnd- [out] pointer past end of parsed header (optional)

    \Output
        int32_t     - negative=not found or not enough space, zero=success, positive=size query result

    \Version 03/24/2009 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpGetLocationHeader(ProtoHttpRefT *pState, const char *pInpBuf, char *pBuffer, int32_t iBufSize, const char **pHdrEnd)
{
    const char *pLocHdr;
    int32_t iLocLen, iLocPreLen=0;

    // get a pointer to header
    if ((pLocHdr = ProtoHttpFindHeaderValue(pInpBuf, "location")) == NULL)
    {
        NetPrintf(("protohttpps4: [%p] location header we not found in the header list\n", pState));
        return(-1);
    }

    /* according to RFC location headers should be absolute, but some webservers respond with relative
       URL's.  we assume any url that does not include "://" is a relative url, and if we find one, we
       assume the request keeps the same security, port, and host as the previous request */
    if ((pState != NULL) && (!strstr(pLocHdr, "://")))
    {
        char strTemp[288]; // space for max DNS name (253 chars) plus max http url prefix

        // format http url prefix
        if ((pState->iSecure && (pState->iPort == 443)) || (pState->iPort == 80))
        {
            ds_snzprintf(strTemp, sizeof(strTemp), "%s://%s", pState->iSecure ? "https" : "http", pState->strHost);
        }
        else
        {
            ds_snzprintf(strTemp, sizeof(strTemp), "%s://%s:%d", pState->iSecure ? "https" : "http", pState->strHost, pState->iPort);
        }

        // make sure relative URL includes '/' as the first character, required when we format the redirection url
        if (*pLocHdr != '/')
        {
            ds_strnzcat(strTemp, "/", sizeof(strTemp));
        }

        // calculate url prefix length
        iLocPreLen = (int32_t)strlen(strTemp);

        // copy url prefix text if a buffer is specified
        if (pBuffer != NULL)
        {
            ds_strnzcpy(pBuffer, strTemp, iBufSize);
            pBuffer = (char *)((uint8_t *)pBuffer + iLocPreLen);
            iBufSize -= iLocPreLen;
        }
    }

    // extract location header and return size
    iLocLen = ProtoHttpExtractHeaderValue(pLocHdr, pBuffer, iBufSize, pHdrEnd);
    // if it's a size request add in (possible) url header length
    if ((pBuffer == NULL) && (iBufSize == 0))
    {
        iLocLen += iLocPreLen;
    }

    // return to caller
    return(iLocLen);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpControl

    \Description
        ProtoHttp control function.  Different selectors control different behaviors.

    \Input *pState  - reference pointer
    \Input iSelect  - control selector ('time')
    \Input iValue   - selector specific
    \Input iValue2  - selector specific
    \Input *pValue  - selector specific

    \Output
        int32_t     - selector specific

    \Notes
        Selectors are:

        \verbatim
            SELECTOR    DESCRIPTION
            'apnd'      The given buffer will be appended to future headers sent
                        by ProtoHttp.  Note that the User-Agent and Accept lines
                        in the default header will be replaced, so if these lines
                        are desired, they should be supplied in the append header.
            'disc'      Close current connection, if any.
            'hver'      Sets header type verification: zero=disabled, else enabled
            'ires'      Resize input buffer
            'keep'      Sets keep-alive; zero=disabled, else enabled
            'ktim'      Sets the keep-alive timeout in milliseconds (default=120s)
            'maxr'      Sets max recv rate (bytes/sec; zero=uncapped)
            'maxs'      Sets max send rate (bytes/sec; zero=uncapped)
            'mncs'      Sets the maximum Net Context shares in DirtyContextManager
            'mwrk'      Sets the maximum number of worker threads ProtoHttp can spawn
            'resr'      Sets ProtoHttp client max connect dns name resolution retries (1)
            'rmax'      Sets maximum number of redirections (default=3; 0=disable)
            'rput'      Sets connection-reuse on put/post (TRUE=enabled, default FALSE)
            'scrt'      Set certificate (pValue=cert, iValue=len)
            'skey'      Set private key (pValue=key, iValue=len)
            'shsf'      Sets the https options provided in iValue (bitwise flag). For possible values refer to libhttp\https.h. 
            'spam'      Sets debug output verbosity (0...n)
            'time'      Sets ProtoHttp client milliseconds (default=30s)
            'vers'      Set client-requested SSL version (default=0x302, TLS1.1)
        \endverbatim

        Unhandled selectors are passed on to ProtoSSL.

    \Version 11/12/2004 (jbrookes) First Version
*/
/*******************************************************************************F*/
int32_t ProtoHttpControl(ProtoHttpRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    int32_t iResult = 0;

    if (iSelect == 'apnd')
    {
        return(_ProtoHttpSetAppendHeader(pState, (const char *)pValue));
    }
    if (iSelect == 'disc')
    {
        _ProtoHttpClose(pState, "user request");
        return(0);
    }
    if (iSelect == 'hver')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] header type verification %s\n", pState, iValue ? "enabled" : "disabled"));
        pState->bVerifyHdr = iValue;
    }
    if (iSelect == 'ires')
    {
        return(_ProtoHttpResizeBuffer(pState, iValue));
    }
    if (iSelect == 'keep')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting keep-alive to %d\n", pState, iValue));
        pState->iKeepAlive = pState->iKeepAliveDflt = iValue;
        return(0);
    }
    if (iSelect == 'ktim')
    {
        if (iValue > 0)
        {
            NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting keep-alive timeout to %d ms\n", pState, iValue));
            pState->iIdleTimeMax = iValue;
            return(0);
        }
        return(-1);
    }
    if (iSelect == 'maxr')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting max recv rate to %d bytes/sec\n", pState, iValue));
        pState->RecvRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'maxs')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting max send rate to %d bytes/sec\n", pState, iValue));
        pState->SendRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'mncs')
    {
        DirtyContextManagerControl('mncs', iValue, iValue2, pValue);
    }
    if (iSelect == 'mwrk')
    {
        NetPrintf(("protohttpps4: [all] setting max worker threads to %d\n", iValue));
        NetCritEnter(NULL);
        _ProtoHttp_State.iMaxThreads = iValue;
        NetCritLeave(NULL);
        return(0);
    }
    if (iSelect == 'prxy')
    {
        NetPrintf(("protohttpps4: [%p] 'prxy' control selector is not supported on PS4\n", pState));
        return(-1);
    }
    if (iSelect == 'rmax')
    {
        pState->iMaxRedirect = iValue;
        return(0);
    }
    if (iSelect == 'rput')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] connection reuse on put/post %s\n", pState, iValue ? "enabled" : "disabled"));
        pState->bReuseOnPost = iValue ? TRUE : FALSE;
        return(0);
    }
    if (iSelect == 'spam')
    {
        pState->iVerbose = iValue;
        // fall through to protossl
    }
    if (iSelect == 'time')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] setting connection timeout to %d ms\n", pState, iValue));
        
        // limit the timeout value, otherwise sceHttpSendRequest may fail with SCE_NET_ERROR_EINVAL
        if (iValue > 300000)
        {
            iValue = 300000;
            NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] capping connection timeout to %d ms\n", pState, iValue));
        }

        pState->uTimeout = (unsigned)iValue;

        // set the system timeout to 5 seconds higher than our timeout to prevent competing
        if (pState->iSceTemplateId > 0)
        {
            if ((iResult = sceHttpSetConnectTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttp: [%p] sceHttpSetConnectTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
            if ((iResult = sceHttpSetRecvTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttp: [%p] sceHttpSetRecvTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
            if ((iResult = sceHttpSetResolveTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttp: [%p] sceHttpSetResolveTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
            if ((iResult = sceHttpSetSendTimeOut(pState->iSceTemplateId, (pState->uTimeout * 1000) + 5000000)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttp: [%p] sceHttpSetSendTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
        }
        return(0);
    }
    if (iSelect == 'resr')
    {
        NetPrintfVerbose((pState->iVerbose, 1, "protohttpps4: [%p] dns resolve retry attempts to %d\n", pState, iValue));
        pState->iResolveRetries = iValue;
       
        // set the connection timeout
        if (pState->iSceTemplateId > 0)
        {
            if ((iResult = sceHttpSetResolveRetry(pState->iSceTemplateId, pState->iResolveRetries)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpSetSendTimeOut failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
        }
        return(0);
    }

    // inherited from ProtoSSL, these now need to be handled here
    if (iSelect == 'scrt')
    {
        if (iValue > 0)
        {
            NetPrintf(("protohttpps4: setting cert (%d bytes)\n", iValue));
            pState->ClientCertData.size = (size_t)iValue;
            pState->ClientCertData.ptr = (char*)pValue;
            pState->bUseClientCert = TRUE;
        }
        else
        {
            NetPrintf(("protohttpps4: removing cert\n"));
            pState->bUseClientCert = FALSE;
        }
    }
    if (iSelect == 'skey')
    {
        if (iValue > 0)
        {
            NetPrintf(("protohttpps4: setting private key (%d bytes)\n", iValue));
            pState->PrivateKeyData.size = (size_t)iValue;
            pState->PrivateKeyData.ptr = (char*)pValue;
            pState->bUsePrivateKey = TRUE;
        }
        else
        {
            NetPrintf(("protohttpps4: removing private key\n", iValue));
            pState->bUsePrivateKey = FALSE;
        }
    }
    if (iSelect == 'shsf')
    {
        pState->bSetHttpsFlags = TRUE;
        if (iValue2 > 0)
        {
            pState->uSceHttpFlags |= (uint32_t)iValue;
        }
        else
        {
            pState->uSceHttpFlags &= ~((uint32_t)iValue);
        }
        
        if (pState->iSceTemplateId > 0)
        {
            // reset flags and apply the new state
            if ((iResult = sceHttpsDisableOption(pState->iSceTemplateId, PROTOHTTP_OPTIONS_FLAGS_ALL)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsDisableOption failed with code %s\n", pState, DirtyErrGetName(iResult)));
                pState->bSetHttpsFlags = FALSE;
            }

            if ((iResult = sceHttpsEnableOption(pState->iSceTemplateId, pState->uSceHttpFlags)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsEnableOption failed with code %s, flags 0x%08x\n", pState, DirtyErrGetName(iResult), pState->uSceHttpFlags));
                pState->bSetHttpsFlags = FALSE;
            }
        }
    }
    if (iSelect == 'vers')
    {
        NetPrintfVerbose((pState->iVerbose, 2, "protohttpps4: [%p] using SSL version %s\n", pState, _HTTP_strVersionNames[iValue&0xff]));
        pState->Vers = _ProtoHttpProtoSslToSceVers(iValue);
        pState->bSetSslVers = TRUE;

        if (pState->iSceTemplateId > 0)
        {
            if ((iResult = sceHttpsSetSslVersion(pState->iSceTemplateId, pState->Vers)) < 0)
            {
                NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] sceHttpsSetSslVersion failed with code %s\n", pState, DirtyErrGetName(iResult)));
                pState->bSetSslVers = FALSE;
            }
        }
    }

    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpStatus

    \Description
        Return status of current HTTP transfer.  Status type depends on selector.

    \Input *pState  - reference pointer
    \Input iSelect  - info selector (see Notes)
    \Input *pBuffer - [out] storage for selector-specific output
    \Input iBufSize - size of buffer

    \Output
        int32_t     - selector specific

    \Notes
        Selectors are:

    \verbatim
        SELECTOR    RETURN RESULT
        'addr'      returns target address, if available
        'body'      negative=failed or pending, else size of body (for 64bit size, get via pBuffer)
        'code'      negative=no response, else server response code (ProtoHttpResponseE)
        'data'      negative=failed, zero=pending, positive=amnt of data ready
        'date'      last-modified date, if available
        'done'      negative=failed, zero=pending, positive=done
        'essl'      returns protossl error state
        'head'      negative=failed or pending, else size of header
        'host'      current host copied to output buffer
        'hres'      return hResult containing either the socket error or ssl error or the http status code 
        'htxt'      current received http header text copied to output buffer
        'info'      copies most recent info header received, if any, to output buffer (one time only)
        'imax'      size of input buffer
        'iovr'      returns input buffer overflow size (valid on PROTOHTTP_MINBUFF result code)
        'maxr'      returns configured max recv rate (bytes/sec; zero=uncapped)
        'maxs'      returns configured max send rate (bytes/sec; zero=uncapped)
        'port'      current port
        'ratr'      return current recv rate estimation (bytes/sec)
        'rats'      return current send rate estimation (bytes/sec)
        'rtxt'      most recent http request header text copied to output buffer
        'rmax'      returns currently configured max redirection count
        'serr'      return socket error
        'time'      TRUE if the client timed out the connection, else FALSE
    \endverbatim

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpStatus(ProtoHttpRefT *pState, int32_t iSelect, void *pBuffer, int32_t iBufSize)
{


    // return protossl error state (we cache this since we reset the state when we disconnect on error)
    if (iSelect == 'essl')
    {
        return(pState->iSslFail);
    }
    // return current host
    if (iSelect == 'host')
    {
        ds_strnzcpy(pBuffer, pState->strHost, iBufSize);
        return(0);
    }
    
    //return hResult containing either the socket error or ssl error or the http status code 
    if (iSelect == 'hres')
    {
        if (pState->iHdrCode > 0)
        {
            return(DirtyErrGetHResult(DIRTYAPI_PROTO_HTTP, pState->iHdrCode, (pState->iHdrCode >= PROTOHTTP_RESPONSE_CLIENTERROR) ? TRUE : FALSE));
        }
        else
        {
            //note that this value is obtained from sceHttpGetLastErrno, reviewing the error codes in the documentation they are valid hResults
            return(pState->iHresult);
        }
    }

    // return size of input buffer
    if (iSelect == 'imax')
    {
        return(pState->iInpMax);
    }

    // return input overflow amount (valid after PROTOHTTP_MINBUFF result code)
    if (iSelect == 'iovr')
    {
        return(pState->iInpOvr);
    }

    // return configured max recv rate
    if (iSelect == 'maxr')
    {
        return(pState->RecvRate.uMaxRate);
    }

    // return configured max send rate
    if (iSelect == 'maxs')
    {
        return(pState->SendRate.uMaxRate);
    }

    // return current port
    if (iSelect == 'port')
    {
        return(pState->iPort);
    }

    // return current recv rate estimation
    if (iSelect == 'ratr')
    {
        return(pState->RecvRate.uCurRate);
    }

    // return current send rate estimation
    if (iSelect == 'rats')
    {
        return(pState->SendRate.uCurRate);
    }
    
    // return current redirection max
    if (iSelect == 'rmax')
    {
        return(pState->iMaxRedirect);
    }
    
    // done check: negative=failed, zero=pending, positive=done
    if (iSelect == 'done')
    {
        if (pState->eState == ST_FAIL)
            return(-1);
        if (pState->eState == ST_DONE)
            return(1);
        return(0);
    }

    // data check: negative=failed, zero=pending, positive=data ready
    if (iSelect == 'data')
    {
        if (pState->eState == ST_FAIL)
        {
            return(-1);
        }
        if ((pState->eState == ST_BODY) || (pState->eState == ST_DONE))
        {
            return(pState->iHeadSize + pState->iBodyRcvd);
        }
        return(0);
    }

    // return response code
    if (iSelect == 'code')
        return(pState->iHdrCode);

    // return timeout indicator
    if (iSelect == 'time')
        return(pState->bTimeout);

    // copies info header (if available) to output buffer
    if (iSelect == 'info')
    {
        if (pState->bInfoHdr)
        {
            if (pBuffer != NULL)
            {
                ds_strnzcpy(pBuffer, pState->strHdr, iBufSize);
            }
            pState->bInfoHdr = FALSE;
            return(pState->iHdrCode);
        }
        return(0);
    }

    // check the state
    if (pState->eState == ST_FAIL)
        return(-1);
    if ((pState->eState != ST_BODY) && (pState->eState != ST_DONE))
        return(-2);

    // parse the tokens
    if (iSelect == 'head')
    {
        return(pState->iHeadSize);
    }
    if (iSelect == 'body')
    {
        if ((pBuffer != NULL) && (iBufSize == sizeof(pState->iBodySize)))
        {
            ds_memcpy(pBuffer, &pState->iBodySize, iBufSize);
        }
        return((int32_t)pState->iBodySize);
    }
    if (iSelect == 'date')
    {
        return(pState->iHdrDate);
    }
    if (iSelect == 'htxt')
    {
        ds_strnzcpy(pBuffer, pState->strHdr, iBufSize);
        return(0);
    }
    if (iSelect == 'rtxt')
    {
        ds_strnzcpy((char *)pBuffer, pState->strRequestHdr, iBufSize);
        return(0);
    }
    // invalid token
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCheckKeepAlive

    \Description
        Checks whether a request for the given Url would be a keep-alive transaction.

    \Input *pState      - reference pointer
    \Input *pUrl        - Url to check

    \Output
        int32_t         - TRUE if a request to this Url would use keep-alive, else FALSE

    \Version 05/12/2009 (jbrookes) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpCheckKeepAlive(ProtoHttpRefT *pState, const char *pUrl)
{
    char strHost[sizeof(pState->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url
    pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // refresh open status
    if (pState->bConnOpen)
    {
        NetPrintfVerbose((pState->iVerbose, 0, "protohttpps4: [%p] check for keep-alive detected connection close\n", pState));
        pState->bConnOpen = FALSE;
    }

    // see if a request for this url would use keep-alive
    if (pState->bConnOpen && (pState->iKeepAlive > 0) && (pState->iPort == iPort) && (pState->iSecure == iSecure) && !ds_stricmp(pState->strHost, strHost))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpUpdate

    \Description
        Give time to module to do its thing (should be called periodically to
        allow module to perform work)

    \Input *pState      - reference pointer

    \Output
        None.

    \Version 04/12/2000 (gschaefer) First Version
*/
/********************************************************************************F*/
void ProtoHttpUpdate(ProtoHttpRefT *pState)
{
    int32_t iResult;
    SceHttpNBEvent nbEvent;
    ds_memclr(&nbEvent, sizeof(nbEvent));
    
    // acquire sole access to http crit
    NetCritEnter(&pState->HttpCrit);
    
    // poll the sce epoll handle
    if (pState->eState != ST_IDLE && pState->eState != ST_DONE && pState->eState != ST_FAIL)
    {
        if (NetTickDiff(NetTick(), pState->uTimer) >= 0)
        {
            NetPrintf(("protohttpps4: [%p] server timeout (state=%d)\n", pState, pState->eState));
            pState->eState = ST_FAIL;
            pState->bTimeout = TRUE;
        }
    }

    if (pState->eState != ST_IDLE && pState->eState != ST_DONE && pState->eState != ST_FAIL)
    {
        if (pState->iSceRequestId > 0 && pState->bEpollInitialized)
        {
            iResult = sceHttpWaitRequest(pState->EpollHandle, &nbEvent, 1, 0);
            if ( iResult > 0 && nbEvent.id == pState->iSceRequestId ) 
            {
                if ( nbEvent.events & (SCE_HTTP_NB_EVENT_SOCK_ERR | SCE_HTTP_NB_EVENT_HUP | SCE_HTTP_NB_EVENT_RESOLVER_ERR) ) 
                {
                    NetPrintf(("protohttpps4: [%p] sceHttpWaitRequest failed (err=0x%08x)\n", pState, iResult));
                    pState->eState = ST_FAIL;
                    pState->iSslFail = 0;
                    pState->uSslFailDet = 0;
                    pState->iHresult = 0;
                    sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
                    sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
                    NetPrintf(("protohttpps4: [%p] sceHttpWaitRequest failed with code 0x%08x for request id %d\n", pState, nbEvent.events, pState->iSceRequestId));
                }
            }
            else if (iResult < 0)
            {
                NetPrintf(("protohttpps4: [%p] sceHttpWaitRequest failed with code %s\n", pState, DirtyErrGetName(iResult)));
            }
        }
    }

    if (pState->eState == ST_REDIR)
    {
        char strLocation[2048];
        uint8_t bFail = FALSE;

        // if a redirection callback needs to be surfaced do it before processing redirections
        if (pState->pReceiveHeaderCb != NULL)
        {
            pState->pReceiveHeaderCb(pState, pState->QueuedRedData.pRespHeader, pState->QueuedRedData.uRespHeaderLen, pState->pCallbackRef);
        }
        
        // setup
        if (ProtoHttpGetLocationHeader(pState, pState->QueuedRedData.pRespHeader, strLocation, sizeof(strLocation), NULL) != -1)
        {
            if ((iResult = _ProtoHttpFormatRequest(pState, strLocation, NULL, 0, pState->eRequestType)) < 0)
            {
                bFail = TRUE;
            }
        }
        else
        {
            bFail = TRUE;
        }

        if (bFail)
        {
            pState->eState = ST_FAIL;
        }
        else
        {
            _ProtoHttpSendRequest(pState);
        }

        DirtyMemFree(pState->QueuedRedData.pRespHeader, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        pState->QueuedRedData.pRespHeader = NULL;
        pState->QueuedRedData.uRespHeaderLen = 0;
    }

    // send buffered header+data to webserver
    if (pState->eState == ST_SEND)
    {
        if (pState->bChunked)
        {
            // if we're sending chunked data, we have no data to send in the inital pass, jump into the ST_RESP state to allow ProtoHttpSend to start pushing data
            pState->eState = ST_RESP;
        }
        else if (((iResult = _ProtoHttpSendBuff(pState)) > 0) && (pState->iInpLen == 0))
        {
            pState->iInpOff = 0;
            pState->iHdrOff = 0;
            pState->eState = ST_RESP;
        }
    }

    // wait for initial response
    if (pState->eState == ST_RESP)
    {
        int32_t iStatusCode = 0;

        // try to send any remaining buffered data if there is any
        if (pState->iInpLen > 0 || pState->bRecvEndChunk)
        {
            _ProtoHttpSendBuff(pState);
        }

        // check if we are done sending buffered data
        iResult = sceHttpGetStatusCode(pState->iSceRequestId, &iStatusCode);
        if (iResult == SCE_HTTP_ERROR_BEFORE_SEND)
        {
            // not done sending, noop
        }
        else if (iResult < 0)
        {
            pState->eState = ST_FAIL;
            pState->iSslFail = 0;
            pState->uSslFailDet = 0;
            pState->iHresult = 0;
            sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
            sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
            NetPrintf(("protohttpps4: [%p] sceHttpGetStatusCode failed with code %s for request id %d\n", pState, DirtyErrGetName(iResult), pState->iSceRequestId));
        }
        // regardless of whether the header code is a success or failure
        else
        {
            NetPrintf(("protohttpps4: [%p] sceHttpGetStatusCode returned http code %d for request id %d, code %s\n", pState, iStatusCode, pState->iSceRequestId, DirtyErrGetName(iResult)));
            pState->eState = ST_HEAD;
        }
    }

    // try to receive header data
    if (pState->eState == ST_HEAD)
    {
        int32_t contentLengthType = 0;
        if ((iResult = _ProtoHttpGetHeader(pState)) < 0)
        {
            pState->eState = ST_FAIL;
            pState->iSslFail = 0;
            pState->uSslFailDet = 0;
            pState->iHresult = 0;
        }
        
        pState->iBodySize = 0ll;
        if ((iResult = sceHttpGetResponseContentLength(pState->iSceRequestId, &contentLengthType, (uint64_t*)(&(pState->iBodySize)))) < 0)
        {
            pState->eState = ST_FAIL;
            pState->iSslFail = 0;
            pState->uSslFailDet = 0;
            pState->iHresult = 0;
            sceHttpsGetSslError(pState->iSceRequestId, &pState->iSslFail, &pState->uSslFailDet);
            sceHttpGetLastErrno(pState->iSceRequestId, &pState->iHresult);
            NetPrintf(("protohttpps4: [%p] sceHttpGetResponseContentLength failed with code %s for request id %d\n", pState, DirtyErrGetName(iResult), pState->iSceRequestId));
        }
        if (contentLengthType != SCE_HTTP_CONTENTLEN_EXIST)
        {
            pState->iBodySize = -1ll;
        }
    }

    // check for header completion, process it
    if ((pState->eState == ST_HEAD) && (pState->iSceRespHeaderLength > 4))
    {
        _ProtoHttpHeaderProcess(pState);
    }

    // parse incoming body data
    while ((pState->eState == ST_BODY) && (_ProtoHttpRecvBody(pState) > 0))
        ;

    // write callback processing
    if (pState->pWriteCb != NULL)
    {
        _ProtoHttpWriteCbProcess(pState);
    }

    // force disconnect in failure state
    if (pState->eState == ST_FAIL)
    {
        _ProtoHttpClose(pState, "error");
    }

    // handle completion
    if (pState->eState == ST_DONE)
    {
        if (pState->bCloseHdr)
        {
            _ProtoHttpClose(pState, "server request");
        }
        else
        {
            // if we've exceeded the keep-alive timeout disconnect
            if (pState->bClosed != TRUE && pState->uIdleTimeStart != 0 && NetTickDiff(NetTick(), pState->uIdleTimeStart) > pState->iIdleTimeMax)
            {
                _ProtoHttpClose(pState, "keep-alive timeout");
                pState->uIdleTimeStart = 0;
            }
        }

        if (pState->iThreadLife == 1)
        {
            _ProtoHttpReleaseWorkerThread(pState);
            // do not wait for termination of the thread
        }
    }

    // release access to httpcrit
    NetCritLeave(&pState->HttpCrit);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetCACert

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all HTTP instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

    \Input *pCACert - pointer to certificate data
    \Input iCertSize- certificate size in bytes

    \Output
        int32_t     - negative=failure, zero=success

    \Version 01/13/2009 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert(const uint8_t *pCACert, int32_t iCertSize)
{
    return(ProtoHttpSetCACert2(pCACert, iCertSize));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetCACert2

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all HTTP instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

        This version of the function does not validate the CA at load time.
        The X509 certificate data will be copied and retained until the CA
        is validated, either by use of ProtoHttpValidateAllCA() or by the CA
        being used to validate a certificate.

    \Input *pCACert - pointer to certificate data
    \Input iCertSize- certificate size in bytes

    \Output
        int32_t     - negative=failure, zero=success

    \Version 04/21/2011 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert2(const uint8_t *pCACert, int32_t iCertSize)
{
    int32_t iMemGroup;
    int32_t iReturn = 1;    //start positive transition to 0 is normal, negative failure
    void *pMemGroupUserData;
    ProtoHttpCACertT *pCert = NULL;
    ProtoHttpCACertT *pNewCert = NULL;
    const uint8_t *_CACert = pCACert;
    int32_t _CertSize = iCertSize;
    uint8_t bNeedsEncoding = FALSE;

    // get memgroup settings for certificate blob
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // convert to PEM format complete with wrapping headers
    if (_ProtoHttpIsPEMFormat(pCACert, iCertSize) <= 0)
    {
        // assume that if we cannot find the headers this is binary cert data
        bNeedsEncoding = TRUE;
        _CertSize = Base64EncodedSize(_CertSize) + ds_strnlen(_strPemBeg, (int32_t)sizeof(_strPemBeg)) + ds_strnlen(_strPemEnd, (int32_t)sizeof(_strPemEnd)) + 3 ;
    }

    // check for duplicates
    NetCritEnter(NULL); // protecting _ProtoHttp_State
    pCert = _ProtoHttp_State.pProtoHttpPs4_CACerts;
    while (pCert != NULL)
    {
        if (pCert->iCertSize == _CertSize)
        {
            if (memcmp(_CACert, pCert->pCACert, _CertSize) == 0)
            {
                NetPrintf(("protohttpps4: ignoring redundant add of CA cert\n"));
                iReturn = 0;
                break;
            }
        }
        pCert = pCert->pNext;
    }
 
    // if its not a duplicate load it
    if (iReturn > 0)
    {
        // create memory
        pNewCert = (ProtoHttpCACertT*)DirtyMemAlloc(sizeof(ProtoHttpCACertT), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
        if (pNewCert != NULL)
        {
            pNewCert->pCACert = (uint8_t*)DirtyMemAlloc(_CertSize, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
            if (pNewCert->pCACert != NULL)
            {
                ds_memclr(pNewCert->pCACert, _CertSize);
                
                if (bNeedsEncoding)
                {
                    int32_t iCurrent = 0;
                    iCurrent += ds_snzprintf((char*)(pNewCert->pCACert), _CertSize - iCurrent, "%s\n", _strPemBeg);
                    iCurrent += Base64Encode2((char*)pCACert, iCertSize, (char*)(pNewCert->pCACert) + iCurrent, _CertSize - iCurrent);
                    iCurrent += ds_snzprintf((char*)(pNewCert->pCACert) + iCurrent, _CertSize - iCurrent, "\n%s\n", _strPemEnd);
                }
                else
                {
                    ds_memcpy_s(pNewCert->pCACert, _CertSize, _CACert, _CertSize);
                }
                pNewCert->iCertSize = _CertSize;
                pNewCert->pNext = NULL;

                // find the end of the list
                if (_ProtoHttp_State.pProtoHttpPs4_CACerts == NULL)
                {
                    _ProtoHttp_State.pProtoHttpPs4_CACerts = pNewCert;
                }
                else
                {
                    pCert = _ProtoHttp_State.pProtoHttpPs4_CACerts;
                    while (pCert->pNext != NULL)
                    {
                        pCert = pCert->pNext;
                    }
                    pCert->pNext = pNewCert;
                }

                // increment the version number
                ++(_ProtoHttp_State.uCaListVersion);
                iReturn = 0;
            }
            else
            {
                NetPrintf(("protohttpps4: failed to allocate memory for CA cert\n"));
                DirtyMemFree(pNewCert, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
                iReturn = -1;
            }
        }
        else
        {
            NetPrintf(("protohttpps4: failed to allocate memory for CA cert\n"));
            iReturn = -1;
        }
    }

    NetCritLeave(NULL); // finished with _ProtoHttp_State
    return(iReturn);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpValidateAllCA

    \Description
        Validate all CA that have been added but not yet been validated.  Validation
        is a one-time process and disposes of the X509 certificate that is retained
        until validation.

    \Output
        int32_t     - negative=failure, zero=success

    \Version 04/21/2011 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpValidateAllCA(void)
{
    // all certs are validated when added
    return(0);
}

/*F*************************************************************************/
/*!
    \Function ProtoHttpClrCACerts

    \Description
        Clears all dynamic CA certs from the list.

    \Version 01/14/2009 (jbrookes)
*/
/**************************************************************************F*/
void ProtoHttpClrCACerts(void)
{
    int32_t iMemGroup;
    void *pMemGroupUserData;

    NetCritEnter(NULL); // protecting _ProtoHttp_State
    ProtoHttpCACertT *pCert = _ProtoHttp_State.pProtoHttpPs4_CACerts;
  
    // get memgroup settings for certificate blob
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
  
    while (pCert != NULL)
    {
        ProtoHttpCACertT *pNext = pCert->pNext;
        if (pCert->pCACert)
        {
            DirtyMemFree(pCert->pCACert, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
        }
        DirtyMemFree(pCert, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
  
        pCert = pNext;
    }
    _ProtoHttp_State.pProtoHttpPs4_CACerts = NULL;

    // increment the version number
    ++(_ProtoHttp_State.uCaListVersion);

    NetCritLeave(NULL); // finished with _ProtoHttp_State
}
