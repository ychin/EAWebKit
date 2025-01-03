/*H********************************************************************************/
/*!
    \File protohttpgdk.c

    \Description
        This module implements the ProtoHttp API using Microsoft's WinHttp
        API.

    \Copyright
        Copyright (c) Electronic Arts 2019

    \Todo
        Add support for going back into the init state when a suspend event occurs

    \Version 10/02/2019 (eesponda)
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <stdlib.h>
#include <winhttp.h>
#include <iphlpapi.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/xboxone/dirtyauthxboxone.h"
#include "DirtySDK/dirtyvers.h"
#include "dirtynetpriv.h" // SocketRate*

#include "DirtySDK/proto/protohttp.h"

/*** Defines **********************************************************************/

//! default ProtoHttp timeout
#define PROTOHTTP_TIMEOUT_DEFAULT (30*1000)

//! default maximum allowed redirections
#define PROTOHTTP_MAXREDIRECT (3)

//! size of "last-received" header cache
#define PROTOHTTP_HDRCACHESIZE (1024)

//! protohttp revision number (maj.min)
#define PROTOHTTP_VERSION (0x0103)          // update this for major bug fixes or protocol additions/changes

/*** Macros ***********************************************************************/

//! macro that wraps the call to DirtyErrGetNameList for the callback status
#define PROTOHTTP_GetCallstackStatus(uStatus) DirtyErrGetNameList((uStatus), _ProtoHttp_aCallbackStatus)

/*** Type Definitions *************************************************************/

//! current state of the request
typedef enum ProtoHttpStateE
{
    PROTOHTTP_ST_INIT,  //!< waiting for the network to be ready for winhttp to finish init
    PROTOHTTP_ST_IDLE,  //!< idle, no requests pending
    PROTOHTTP_ST_AUTH,  //!< performing authentication
    PROTOHTTP_ST_CONN,  //!< creating the connection and request
    PROTOHTTP_ST_SEND,  //!< sending request body (if expected)
    PROTOHTTP_ST_RESP,  //!< waiting for response
    PROTOHTTP_ST_HEAD,  //!< receiving the headers
    PROTOHTTP_ST_BODY,  //!< receiving the body
    PROTOHTTP_ST_DONE,  //!< request is complete
    PROTOHTTP_ST_FAIL,  //!< request has failed for whatever reason
    PROTOHTTP_ST_ABRT,  //!< request is being aborted
    PROTOHTTP_ST_MAX    //!< maximum state for validation
} ProtoHttpStateE;

//! state of the async operation
typedef enum ProtoHttpAsyncOpStateE
{
    PROTOHTTP_ASYNC_OP_IDLE,        //!< waiting for a new operation
    PROTOHTTP_ASYNC_OP_PENDING,     //!< operation is currently pending
    PROTOHTTP_ASYNC_OP_GETRESULT    //!< operation is complete and result must be retrieved
} ProtoHttpAsyncOpStateE;

//! helps processing async operations that complete on the winhttp callback function
typedef struct ProtoHttpAsyncOpT
{
    uint32_t uResult;               //!< result of the operation
    ProtoHttpAsyncOpStateE eState;  //!< current state of the operation
} ProtoHttpAsyncOpT;

//! http module state
struct ProtoHttpRefT
{
    int32_t iMemGroup;                                  //!< module mem group id
    void *pMemGroupUserData;                            //!< user data associated with mem group

    NetCritT HttpCrit;                                  //!< critical section for guarding update from send/recv

    int32_t iNumRedirect;                               //!< current number of redirections
    int32_t iMaxRedirect;                               //!< maximum number of redirections allowed

    int32_t iKeepAlive;                                 //!< indicate if we should try to use keep-alive
    int32_t iKeepAliveDflt;                             //!< keep-alive default (keep-alive will be reset to this value; can be overridden by user)

    SocketRateT SendRate;                               //!< send rate estimation data
    SocketRateT RecvRate;                               //!< recv rate estimation data

    int8_t iVerbose;                                    //!< debug output level
    uint8_t bAuthenticate;                              //!< TRUE if we should enable auth
    uint8_t bGetAuthToken;                              //!< TRUE if we should retrieve an auth token to add to the Authorization header
    int8_t iAuthTokenIdx;                               //!< user index for which the token belongs

    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;          //!< global callback for modifying request header
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;        //!< global callback for viewing recv header on receipt
    void *pCallbackRef;                                 //!< user ref for global callback

    ProtoHttpWriteCbT *pWriteCb;                        //!< optional data write callback
    ProtoHttpCustomHeaderCbT *pReqCustomHeaderCb;       //!< optional custom header callback
    ProtoHttpReceiveHeaderCbT *pReqReceiveHeaderCb;     //!< optional receive header callback
    void *pUserData;                                    //!< user data for callback

    char *pAppendHdr;                                   //!< append header buffer pointer
    int32_t iAppendLen;                                 //!< size of append header buffer

    char *pInpBuf;                                      //!< input buffer
    int32_t iInpMax;                                    //!< maximum buffer size
    int32_t iInpOff;                                    //!< offset into buffer
    int32_t iInpLen;                                    //!< total length in buffer
    int32_t iInpCnt;                                    //!< ongoing count
    int32_t iHdrLen;                                    //!< length of header(s) queued for sending
    int32_t iHeadSize;                                  //!< length of the receive header(s)
    ProtoHttpStateE eState;                             //!< current state of the module

    wchar_t *pWInpBuf;                                  //!< wide character input buffer used to work with MS APIs
    int32_t iWInpMax;                                   //!< maximum buffer size for our wide char buffer
    int32_t iWInpOff;                                   //!< offset into our wide character buffer
    int32_t iWInpLen;                                   //!< total length into our wide character buffer

    char strTokenUrl[256];                              //!< token uri, in case different from hostname
    char strHost[256];                                  //!< server name
    char strBaseHost[256];                              //!< base server name (used for partial urls)
    char *pUrl;                                         //!< saved url when we are doing auth token requests

    char strHdr[PROTOHTTP_HDRCACHESIZE];                //!< storage for most recently received HTTP header
    char strRequestHdr[PROTOHTTP_HDRCACHESIZE];         //!< storage for most recent HTTP request header

    ProtoHttpRequestTypeE eRequestType;                 //!< request type of current request
    int32_t iPort;                                      //!< server port
    int32_t iBasePort;                                  //!< base port (used for partial urls)
    int32_t bSecure;                                    //!< secure connection
    int32_t bBaseSecure;                                //!< base security setting (used for partial urls)
    int32_t iRecvSize;                                  //!< protohttprecvall variable to track received size
    int64_t iPostSize;

    HINTERNET pHttpSession;                             //!< handle to the winhttp session
    HINTERNET pHttpConnection;                          //!< handle to the winhttp connection
    HINTERNET pHttpRequest;                             //!< handle to the winhttp request

    uint32_t uProtocolError;                            //!< request error from winhttp
    uint32_t uSecureError;                              //!< secure error from winhttp

    int32_t iHdrCode;                                   //!< parsed status code from the header
    int32_t iHdrDate;                                   //!< parsed date from the header
    int32_t iBodyRcvd;                                  //!< amount of the body received by the caller
    int64_t iBodySize;                                  //!< size of the body (-1 while unknown)

    uint8_t bVerifyHdr;                                 //!< TRUE if we should verify the header
    uint8_t bHttp1_0;                                   //!< TRUE if we are dealing with HTTP/1.0 server
    uint8_t bCloseHdr;                                  //!< TRUE if the server sent us connection: close in header
    uint8_t bInfoHdr;                                   //!< TRUE if this is INFORMATIONAL (100 status) response
    uint8_t bNewConnection;                             //!< TRUE if we are opening a new connection
    uint8_t bReuseOnPost;                               //!< TRUE if we will be re-using the connection on a PUT/POST
    uint8_t bHeadersAvailable;                          //!< TRUE if the headers are available to be parsed
    uint8_t bClosed;                                    //!< TRUE if the connection has been closed by the server
    uint8_t bPendingRequest;                            //!< TRUE if we queued up a request to complete after an abort
    uint8_t _pad[3];
    uint32_t uBytesAvailable;                           //!< number of bytes avaialble to read
    uint32_t uWinHttpFlags;                             //!< flags passed into WinHttpOpen

    ProtoHttpAsyncOpT ReadWriteOp;                      //!< async operation state that tracks are read/writes (don't happen at the same time)
    ProtoHttpAsyncOpT QueryOp;                          //!< async operation state that tracks querying for more data
    ProtoHttpAsyncOpT AbortOp;                          //!< async operation state that tracks aborts

};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

//! names of HTTP request types
static const char *_ProtoHttp_strRequestType[PROTOHTTP_NUMREQUESTTYPES] =
{
    "HEAD",
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "OPTIONS",
    "PATCH",
    "CONNECT"
};

//! wide-char version for use with the WinHttp APIs
static const wchar_t *_ProtoHttp_wstrRequestType[PROTOHTTP_NUMREQUESTTYPES] =
{
    L"HEAD",
    L"GET",
    L"POST",
    L"PUT",
    L"DELETE",
    L"OPTIONS",
    L"PATCH",
    L"CONNECT"
};

//! default flags for winhttp; require async and secure operating in XNetworking (secure includes async)
static uint32_t _ProtoHttp_uWinHttpFlags = WINHTTP_FLAG_SECURE_DEFAULTS;

#if DIRTYCODE_LOGGING
//! the different callback statuses we want to log
static const DirtyErrT _ProtoHttp_aCallbackStatus[] =
{
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_RESOLVING_NAME),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_NAME_RESOLVED),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SENDING_REQUEST),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_REQUEST_SENT),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_HANDLE_CREATED),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_DETECTING_PROXY),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_REDIRECT),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SECURE_FAILURE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_READ_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_REQUEST_ERROR),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SETTINGS_WRITE_COMPLETE),
    DIRTYSOCK_ErrorName(WINHTTP_CALLBACK_STATUS_SETTINGS_READ_COMPLETE),
    DIRTYSOCK_ListEnd()
};

//! string representation of the states for debug logging
static const char *_ProtoHttp_strStates[] =
{
    "PROTOHTTP_ST_INIT",
    "PROTOHTTP_ST_IDLE",
    "PROTOHTTP_ST_AUTH",
    "PROTOHTTP_ST_CONN",
    "PROTOHTTP_ST_SEND",
    "PROTOHTTP_ST_RESP",
    "PROTOHTTP_ST_HEAD",
    "PROTOHTTP_ST_BODY",
    "PROTOHTTP_ST_DONE",
    "PROTOHTTP_ST_FAIL",
    "PROTOHTTP_ST_ABRT"
};

//! ensures that the array and enum values line up
_STATIC_ASSERT((sizeof(_ProtoHttp_strStates)/sizeof(_ProtoHttp_strStates[0])) == PROTOHTTP_ST_MAX);
#endif

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAsyncOpInit

    \Description
        Move the async op back to the initial state in cases where the operation
        won't complete

    \Input *pAsyncOp    - the state of the async operation

    \Notes
        The common use for this function is when we start an operation but the
        API that would complete async returned an error synchronously

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpAsyncOpInit(ProtoHttpAsyncOpT *pAsyncOp)
{
    pAsyncOp->uResult = 0;
    pAsyncOp->eState = PROTOHTTP_ASYNC_OP_IDLE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAsyncOpStart

    \Description
        Move the async op into the pending state if it is possible to do (from INIT)

    \Input *pAsyncOp    - the state of the async operation

    \Output
        uint8_t         - TRUE=if we moved to pending state, FALSE=not in the init state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static uint8_t _ProtoHttpAsyncOpStart(ProtoHttpAsyncOpT *pAsyncOp)
{
    if (pAsyncOp->eState != PROTOHTTP_ASYNC_OP_IDLE)
    {
        return(FALSE);
    }
    pAsyncOp->eState = PROTOHTTP_ASYNC_OP_PENDING;
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAsyncOpComplete

    \Description
        Complete the async operation by saving the result and moving to the getresult state

    \Input *pAsyncOp    - the state of the async operation
    \Input uResult      - the result of the operation

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpAsyncOpComplete(ProtoHttpAsyncOpT *pAsyncOp, uint32_t uResult)
{
    pAsyncOp->uResult = uResult;
    pAsyncOp->eState = PROTOHTTP_ASYNC_OP_GETRESULT;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAsyncOpGetResult

    \Description
        Get the result of the async operation if possible

    \Input *pAsyncOp    - the state of the async operation
    \Input *pResult     - [out] the result of the operation

    \Output
        uint8_t         - TRUE=if the result is valid, FALSE=op is in init/pending state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static uint8_t _ProtoHttpAsyncOpGetResult(ProtoHttpAsyncOpT *pAsyncOp, uint32_t *pResult)
{
    if (pAsyncOp->eState == PROTOHTTP_ASYNC_OP_GETRESULT)
    {
        if (pResult != NULL)
        {
            *pResult = pAsyncOp->uResult;
        }
        ds_memclr(pAsyncOp, sizeof(*pAsyncOp));
        return(TRUE);
    }
    return(FALSE);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetState

    \Description
        Set the internal state

    \Input  *pHttpRef   - module state
    \Input eState       - state we are changing to

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpSetState(ProtoHttpRefT *pHttpRef, ProtoHttpStateE eState)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] changing state from %s to %s\n", pHttpRef,
                _ProtoHttp_strStates[pHttpRef->eState], _ProtoHttp_strStates[eState]));
    pHttpRef->eState = eState;
}

#if DIRTYCODE_LOGGING
/*F********************************************************************************/
/*!
    \Function _ProtoHttpDisplayHeader

    \Description
        Display header to debug output [DEBUG ONLY]

    \Input  *pHttpRef   - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpDisplayHeader(ProtoHttpRefT *pHttpRef)
{
    // if we just sent a header, display header to debug output
    if (pHttpRef->iVerbose > 1)
    {
        char *pHdrEnd = pHttpRef->pInpBuf + pHttpRef->iHdrLen;
        char cHdrChr = *pHdrEnd;
        *pHdrEnd = '\0';
        NetPrintf(("protohttpgdk: [%p] sent request:\n", pHttpRef));
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] tick=%u\n", pHttpRef, NetTick()));
        NetPrintWrap(pHttpRef->pInpBuf, 80);
        *pHdrEnd = cHdrChr;
    }
}
#endif

/*F********************************************************************************/
/*!
    \Function _ProtoHttpApplyBaseUrl

    \Description
        Apply base url elements (if set) to any url elements not specified (relative
        url support).

    \Input *pHttpRef        - module state
    \Input *pKind           - parsed http kind ("http" or "https")
    \Input *pHost           - [in/out] parsed URL host
    \Input iHostSize        - size of pHost buffer
    \Input *pPort           - [in/out] parsed port
    \Input *pSecure         - [in/out] parsed security (0 or 1)
    \Input bPortSpecified   - TRUE if a port is explicitly specified in the url, else FALSE

    \Output
        uint8_t             - non-zero if changed, else zero

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static uint8_t _ProtoHttpApplyBaseUrl(ProtoHttpRefT *pHttpRef, const char *pKind, char *pHost, int32_t iHostSize, int32_t *pPort, int32_t *pSecure, uint8_t bPortSpecified)
{
    uint8_t bChanged = FALSE;
    if ((*pHost == '\0') && (pHttpRef->strBaseHost[0] != '\0'))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] host not present; setting to %s\n", pHttpRef, pHttpRef->strBaseHost));
        ds_strnzcpy(pHost, pHttpRef->strBaseHost, iHostSize);
        bChanged = TRUE;
    }
    if ((bPortSpecified == FALSE) && (pHttpRef->iBasePort != 0))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] port not present; setting to %d\n", pHttpRef, pHttpRef->iBasePort));
        *pPort = pHttpRef->iBasePort;
        bChanged = TRUE;
    }
    if (*pKind == '\0')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] kind (protocol) not present; setting to %d\n", pHttpRef, pHttpRef->bBaseSecure));
        *pSecure = pHttpRef->bBaseSecure;
        // if our port setting is default and incompatible with our security setting, override it
        if (((*pPort == 80) && (*pSecure == TRUE)) || ((*pPort == 443) && (*pSecure == FALSE)))
        {
            *pPort = *pSecure ? 443 : 80;
        }
        bChanged = TRUE;
    }
    return(bChanged);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatRequestHeader

    \Description
        Format a request header based on given input data.

    \Input *pHttpRef    - module state
    \Input *pUrl        - pointer to user-supplied url
    \Input *pHost       - pointer to hostname
    \Input iPort        - port, or zero if unspecified
    \Input bSecure      - 1=enabled, 0=disabled
    \Input *pRequest    - pointer to request type
    \Input iDataLen     - size of included data; zero if none, negative if streaming put/post
    \Input *pAuthToken  - MS auth token if we acquired one

    \Output
        int32_t         - zero=success, else error

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatRequestHeader(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pHost, int32_t iPort, int32_t bSecure, const char *pRequest, int64_t iDataLen, const char *pAuthToken)
{
    int32_t iOffset = 0;
    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;
    void *pUserData;
    // if uri is empty or isn't preceded by a slash, put on in
    const char *pUrlSlash = (*pUrl != '/') ? "/" : "";

    // format request header
    iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "%s %s%s HTTP/1.1\r\n", pRequest, pUrlSlash, pUrl);
    if ((bSecure && (iPort == 443)) || (iPort == 80))
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Host: %s\r\n", pHost);
    }
    else
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Host: %s:%d\r\n", pHost, iPort);
    }
    if (iDataLen == PROTOHTTP_STREAM_BEGIN)
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Transfer-Encoding: Chunked\r\n");
    }
    else if ((iDataLen > 0) || (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_PUT) || (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_POST) || (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_PATCH))
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Content-Length: %qd\r\n", iDataLen);
    }
    if (pHttpRef->iKeepAlive == 0)
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Connection: Close\r\n");
    }
    if (pAuthToken != NULL)
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Authorization: %s\r\n", pAuthToken);
    }
    if ((pHttpRef->pAppendHdr == NULL) || (pHttpRef->pAppendHdr[0] == '\0'))
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "Accept: */*\r\n");
    }
    else
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "%s", pHttpRef->pAppendHdr);
    }

    // request level callback takes priority to global
    if ((pCustomHeaderCb = pHttpRef->pReqCustomHeaderCb) != NULL)
    {
        pUserData = pHttpRef->pUserData;
    }
    else
    {
        pCustomHeaderCb = pHttpRef->pCustomHeaderCb;
        pUserData = pHttpRef->pCallbackRef;
    }

    // call custom header format callback, if specified
    if (pCustomHeaderCb != NULL)
    {
        if ((iOffset = pCustomHeaderCb(pHttpRef, pHttpRef->pInpBuf, pHttpRef->iInpMax, NULL, 0, pUserData)) < 0)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] custom header callback error %d\n", pHttpRef, iOffset));
            return(iOffset);
        }
        if (iOffset == 0)
        {
            iOffset = (int32_t)strlen(pHttpRef->pInpBuf);
        }
    }

    // append header terminator (if necessary)
    if ((pHttpRef->pInpBuf[iOffset-2] != '\r') && (pHttpRef->pInpBuf[iOffset-1] != '\n'))
    {
        iOffset += ds_snzprintf(pHttpRef->pInpBuf+iOffset, pHttpRef->iInpMax-iOffset, "\r\n");
    }

    // make sure we were able to complete the header
    if (iOffset > pHttpRef->iInpMax)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] not enough buffer to format request header (have %d, need %d)\n", pHttpRef, pHttpRef->iInpMax, iOffset));
        return(PROTOHTTP_MINBUFF);
    }

    // save a copy of the header
    ds_strnzcpy(pHttpRef->strRequestHdr, pHttpRef->pInpBuf, sizeof(pHttpRef->strRequestHdr));

    // update buffer size
    pHttpRef->iInpLen = iOffset;

    // save updated header size
    pHttpRef->iHdrLen = pHttpRef->iInpLen;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpParseHeader

    \Description
        Parse incoming HTTP header.  The HTTP header is presumed to be at the
        beginning of the input buffer.

    \Input  *pHttpRef   - module state

    \Output
        int32_t         - negative=not ready or error, else success

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpParseHeader(ProtoHttpRefT *pHttpRef)
{
    char *s = pHttpRef->pInpBuf;
    char *t = pHttpRef->pInpBuf+pHttpRef->iInpLen-3;
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
    pHttpRef->iHeadSize = (int32_t)(s+4-pHttpRef->pInpBuf);
    // terminate header for easy parsing
    s[2] = s[3] = 0;

    // make sure the header is valid
    if (pHttpRef->bVerifyHdr)
    {
        if (strncmp(pHttpRef->pInpBuf, "HTTP", 4))
        {
            // header is invalid
            NetPrintf(("protohttpgdk: [%p] invalid result type\n", pHttpRef));
            _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
            return(-2);
        }
    }

    // detect if it is a 1.0 response
    pHttpRef->bHttp1_0 = !strncmp(pHttpRef->pInpBuf, "HTTP/1.0", 8);

    // parse header code
    pHttpRef->iHdrCode = ProtoHttpParseHeaderCode(pHttpRef->pInpBuf);

    #if DIRTYCODE_LOGGING
    NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttp: [%p] received %d response (%d bytes)\n", pHttpRef, pHttpRef->iHdrCode, pHttpRef->iHeadSize));
    if (pHttpRef->iVerbose > 1)
    {
        NetPrintWrap(pHttpRef->pInpBuf, 80);
    }
    #endif

    // parse content-length field
    if (ProtoHttpGetHeaderValue(pHttpRef, pHttpRef->pInpBuf, "content-length", strTemp, sizeof(strTemp), NULL) != -1)
    {
        pHttpRef->iBodySize = ds_strtoll(strTemp, NULL, 10);
    }
    else
    {
        pHttpRef->iBodySize = -1;
    }

    // parse last-modified header
    if (ProtoHttpGetHeaderValue(pHttpRef, pHttpRef->pInpBuf, "last-modified", strTemp, sizeof(strTemp), NULL) != -1)
    {
        pHttpRef->iHdrDate = (int32_t)ds_strtotime(strTemp);
    }
    else
    {
        pHttpRef->iHdrDate = 0;
    }

    // parse connection header
    if (pHttpRef->bCloseHdr == FALSE)
    {
        ProtoHttpGetHeaderValue(pHttpRef, pHttpRef->pInpBuf, "connection", strTemp, sizeof(strTemp), NULL);
        pHttpRef->bCloseHdr = !ds_stricmp(strTemp, "close");
    }

    // note if it is an informational header
    pHttpRef->bInfoHdr = PROTOHTTP_GetResponseClass(pHttpRef->iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL;

    // copy header to header cache
    ds_strnzcpy(pHttpRef->strHdr, pHttpRef->pInpBuf, sizeof(pHttpRef->strHdr));

    // request level callback takes priority to global
    if ((pReceiveHeaderCb = pHttpRef->pReqReceiveHeaderCb) != NULL)
    {
        pUserData = pHttpRef->pUserData;
    }
    else
    {
        pReceiveHeaderCb = pHttpRef->pReceiveHeaderCb;
        pUserData = pHttpRef->pCallbackRef;
    }

    // trigger recv header user callback, if specified
    if (pReceiveHeaderCb != NULL)
    {
        pReceiveHeaderCb(pHttpRef, pHttpRef->pInpBuf, (uint32_t)strlen(pHttpRef->pInpBuf), pUserData);
    }

    // remove header from input buffer
    pHttpRef->iInpOff = pHttpRef->iHeadSize;
    pHttpRef->iInpCnt = pHttpRef->iInpLen - pHttpRef->iHeadSize;

    // header successfully parsed
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpOnError

    \Description
        Logs the WinHTTP error and puts us in the failure state

    \Input *pHttpRef    - module state
    \Input uError       - the error that occurred
    \Input *pDebugText  - operation debug text for logging

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpOnError(ProtoHttpRefT *pHttpRef, uint32_t uError, const char *pDebugText)
{
    NetPrintf(("protohttpgdk: [%p] %s failed with %s\n", pHttpRef, pDebugText, DirtyErrGetName(uError)));
    _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);

    // put all our async operations in init state as async failure and sync failures both come through this function
    _ProtoHttpAsyncOpInit(&pHttpRef->ReadWriteOp);
    _ProtoHttpAsyncOpInit(&pHttpRef->QueryOp);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpStatusCallback

    \Description
        Callback function registered with WinHTTP which provides information to us

    \Input *pHttpSession    - handle to the session
    \Input *pContext        - pointer to our state
    \Input uStatus          - the current status we are being notified with
    \Input *pStatusInfo     - status specific
    \Input uStatusInfoLen   - status specific

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void CALLBACK _ProtoHttpStatusCallback(HINTERNET pHttpSession, DWORD_PTR pContext, DWORD uStatus, LPVOID pStatusInfo, DWORD uStatusInfoLen)
{
    ProtoHttpRefT *pHttpRef = (ProtoHttpRefT *)pContext;

    // certain status will fire before we have a valid ref
    if (pHttpRef != NULL)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] _ProtoHttpStatusCallback -> %s\n", pHttpRef, PROTOHTTP_GetCallstackStatus(uStatus)));
    }
    else
    {
        // ignore callbacks with any context
        return;
    }

    // if we still have an ongoing abort we want to only process closing events, other events will be ignored
    if (pHttpRef->AbortOp.eState != PROTOHTTP_ASYNC_OP_IDLE)
    {
        if (uStatus == WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING)
        {
            _ProtoHttpAsyncOpComplete(&pHttpRef->AbortOp, 0);
        }
        else
        {
            return;
        }
    }

    // handle the statuses that we are concerned with
    if (uStatus == WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION)
    {
        // set that the connection should be closed
        pHttpRef->bClosed = TRUE;
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE)
    {
        DWORD uBytesAvailable = *(DWORD *)pStatusInfo;
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] %d bytes available\n", pHttpRef, uBytesAvailable));
        // save the bytes available into the result
        _ProtoHttpAsyncOpComplete(&pHttpRef->QueryOp, uBytesAvailable);
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE)
    {
        // set the headers as available and they will be processed on the next update
        pHttpRef->bHeadersAvailable = TRUE;
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_READ_COMPLETE)
    {
        // save the bytes read into the result
        _ProtoHttpAsyncOpComplete(&pHttpRef->ReadWriteOp, uStatusInfoLen);
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR)
    {
        WINHTTP_ASYNC_RESULT *pResult = (WINHTTP_ASYNC_RESULT *)pStatusInfo;
        // save the error for use later and then end the request
        pHttpRef->uProtocolError = pResult->dwError;
        _ProtoHttpOnError(pHttpRef, pResult->dwError, "WINHTTP_CALLBACK_STATUS_REQUEST_ERROR");
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE)
    {
        DWORD uResult = *(DWORD *)pStatusInfo;
        // save the error for use later and then end the request
        pHttpRef->uSecureError = uResult;
        _ProtoHttpOnError(pHttpRef, uResult, "WINHTTP_CALLBACK_STATUS_SECURE_FAILURE");
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE)
    {
        // write data if we have data to write, otherwise start getting the response
        _ProtoHttpSetState(pHttpRef, (pHttpRef->iPostSize > 0) ? PROTOHTTP_ST_SEND : PROTOHTTP_ST_RESP);
    }
    else if (uStatus == WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE)
    {
        DWORD uBytesWritten = *(DWORD *)pStatusInfo;
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] async send %d bytes complete\n", pHttpRef, uBytesWritten));
        // save the bytes written into the result
        _ProtoHttpAsyncOpComplete(&pHttpRef->ReadWriteOp, uBytesWritten);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpParseUri

    \Description
        Parses the uri from the already formatted request headers outputting the
        information to a wide-char buffer required for the WinHttp APIs

    \Input *pHdrBuf - the request headers
    \Input iHdrLen  - length of the request headers
    \Input *pUri    - [out] storage for the parsed uri
    \Input iUriMax  - maximum storage size for the uri

    \Version 02/25/2020 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpParseUri(const char *pHdrBuf, int32_t iHdrLen, wchar_t *pUri, int32_t iUriMax)
{
    const char *pEnd;
    size_t uResult;

    // find the beginning of the uri
    if ((pHdrBuf = strchr(pHdrBuf, '/')) == NULL)
    {
        return;
    }
    // find end of the uri
    if ((pEnd = strchr(pHdrBuf, ' ')) == NULL)
    {
        return;
    }
    // copy the uri
    mbstowcs_s(&uResult, pUri, iUriMax, pHdrBuf, pEnd-pHdrBuf);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSendRequest

    \Description
        Perform the work needed to start the http request

    \Input *pHttpRef    - module state

    \Output
        int32_t         - zero=success, negative=error

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSendRequest(ProtoHttpRefT *pHttpRef)
{
    const char *pHdr;
    int32_t iHdrOffset;
    uint32_t uOption = 0;

    // check if we need to open a new connection
    if (pHttpRef->bNewConnection == TRUE)
    {
        // if we have a previous connection. close it
        if (pHttpRef->pHttpConnection != NULL)
        {
            WinHttpCloseHandle(pHttpRef->pHttpConnection);
            pHttpRef->pHttpConnection = NULL;
        }

        // set the hostname for the new connection and then open the connection
        mbstowcs(pHttpRef->pWInpBuf, pHttpRef->strHost, sizeof(pHttpRef->strHost));
        if ((pHttpRef->pHttpConnection = WinHttpConnect(pHttpRef->pHttpSession, pHttpRef->pWInpBuf, pHttpRef->iPort, 0)) == NULL)
        {
            _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpConnect");
            return(-1);
        }

        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] connect start (tick=%u)\n", pHttpRef, NetTick()));
    }
    else
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] reusing previous connection (keep-alive)\n", pHttpRef));
    }

    // copy the path into our wide buffer and open our request
    _ProtoHttpParseUri(pHttpRef->pInpBuf, pHttpRef->iHdrLen, pHttpRef->pWInpBuf, pHttpRef->iWInpMax / sizeof(*pHttpRef->pWInpBuf));
    if ((pHttpRef->pHttpRequest = WinHttpOpenRequest(pHttpRef->pHttpConnection, _ProtoHttp_wstrRequestType[pHttpRef->eRequestType], pHttpRef->pWInpBuf, NULL,
            NULL, NULL, pHttpRef->bSecure ? WINHTTP_FLAG_SECURE : 0)) == NULL)
    {
        _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpOpenRequest");
        return(-2);
    }

    // set the appropriate options
    if (pHttpRef->iKeepAlive == 0)
    {
        uOption |= WINHTTP_DISABLE_KEEP_ALIVE;
    }
    if (pHttpRef->bAuthenticate == FALSE)
    {
        uOption |= WINHTTP_DISABLE_AUTHENTICATION;
    }

    // disable redirects regardless and then handle them individually
    uOption |= WINHTTP_DISABLE_REDIRECTS;

    // handle disabling the features as necessary
    if (uOption != 0)
    {
        if (!WinHttpSetOption(pHttpRef->pHttpRequest, WINHTTP_OPTION_DISABLE_FEATURE, &uOption, sizeof(uOption)))
        {
            NetPrintf(("protohttpgdk: [%p] WINHTTP_OPTION_DISABLE_FEATURE(0x%08x) failed with %s\n", pHttpRef, uOption, DirtyErrGetName(GetLastError())));
        }
    }

    #if DIRTYCODE_LOGGING
    _ProtoHttpDisplayHeader(pHttpRef);
    #endif

    // skip the request line as WinHttpAddRequestHeaders only expects headers in the key value pair form
    pHdr = strstr(pHttpRef->pInpBuf, "\r\n") + 2;
    iHdrOffset = (int32_t)(pHdr - pHttpRef->pInpBuf);

    // copy our headers into the wide buffer to be added to this request
    mbstowcs(pHttpRef->pWInpBuf, pHttpRef->pInpBuf+iHdrOffset, pHttpRef->iHdrLen-iHdrOffset);
    if (!WinHttpAddRequestHeaders(pHttpRef->pHttpRequest, pHttpRef->pWInpBuf, pHttpRef->iHdrLen-iHdrOffset, WINHTTP_ADDREQ_FLAG_ADD))
    {
        _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpAddRequestHeaders");
        return(-3);
    }

    // update the offset and reset if possible
    pHttpRef->iInpOff += pHttpRef->iHdrLen;
    if (pHttpRef->iInpOff == pHttpRef->iInpLen)
    {
        pHttpRef->iInpLen = pHttpRef->iInpOff = 0;
    }

    /* set the connection state (meaning initial connection and request), do it before the WinHttpSendRequest in case it
       happens before the end of that call (on a different thread). */
    _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_CONN);

    // send the request, we specify NULLs here so that we can make the WinHttpWriteData calls later on (if necessary)
    if (!WinHttpSendRequest(pHttpRef->pHttpRequest, NULL, 0, NULL, 0, pHttpRef->iPostSize <= 0 ? 0 : pHttpRef->iPostSize, (DWORD_PTR)pHttpRef))
    {
        _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpSendRequest");
        return(-4);
    }

    // count the attempt
    pHttpRef->iKeepAlive += 1;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSend

    \Description
        Send data to the server

    \Input *pHttpRef    - module state
    \Input *pData       - data we are sending
    \Input iDataSize    - size of the data

    \Output
        int32_t         - zero=success, negative=error

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSend(ProtoHttpRefT *pHttpRef, const char *pData, int32_t iDataSize)
{
    int32_t iResult = iDataSize;

    // start a new operation if possible (operation must be willing to track a new request)
    if (_ProtoHttpAsyncOpStart(&pHttpRef->ReadWriteOp) == FALSE)
    {
        return(0);
    }

    // make the API call that will complete async with a callback to _ProtoHttpStatusCallback
    if (WinHttpWriteData(pHttpRef->pHttpRequest, pData, iDataSize, NULL))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] sent %d bytes\n", pHttpRef, iDataSize));
        #if DIRTYCODE_LOGGING
        if (pHttpRef->iVerbose > 3)
        {
            NetPrintMem(pData, iDataSize, "http-send");
        }
        #endif
    }
    else
    {
        NetPrintf(("protohttpgdk: failed to send %d bytes\n", iDataSize));
        _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpWriteData");
        iResult = -1;
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatChunk

    \Description
        When using chunked/streaming transfers it formats it for sending in our buffer

    \Input *pHttpRef    - module state
    \Input *pStrBuf     - data we are sending
    \Input iSize        - size of the data

    \Output
        int32_t         - positive=amount of data buffered, negative=error

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatChunk(ProtoHttpRefT *pHttpRef, const char *pStrBuf, int32_t iSize)
{
    char *pInpBuf = pHttpRef->pInpBuf + pHttpRef->iInpLen;
    int32_t iInpMax = pHttpRef->iInpMax - pHttpRef->iInpLen;
    int32_t iSendSize, iSentSize;

    // make sure we have enough room for a max chunk header, chunk data, *and* possible end chunk
    if ((iSendSize = iSize) > 0)
    {
        if (iSendSize > (iInpMax - (6+2+2 + 1+2+2)))
        {
            iSendSize = (iInpMax - (6+2+2 + 1+2+2));
        }
    }
    else
    {
       pHttpRef->iPostSize = 0;
    }

    // if we have room to buffer chunk data, or this is the end chunk, do it
    if ((iSendSize > 0) || (iSize == 0))
    {
        // format chunk into buffer
        iSentSize = ds_snzprintf(pInpBuf, iInpMax, "%x\r\n", iSendSize);
        if (iSendSize > 0)
        {
            ds_memcpy(pInpBuf+iSentSize, pStrBuf, iSendSize);
            iSentSize += iSendSize;
        }
        iSentSize += ds_snzprintf(pInpBuf+iSentSize, iInpMax-iSendSize, "\r\n");

        // add chunk to length
        pHttpRef->iInpLen += iSentSize;
    }
    else
    {
        iSendSize = 0;
    }
    // return size of data buffered to caller
    return(iSendSize);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpBufferData

    \Description
        Buffers data into our buffer as sends need to happen async where the data
        must remain accessible during that time

    \Input *pHttpRef    - module state
    \Input *pData       - data we are sending
    \Input iDataSize    - size of the data

    \Output
        int32_t         - amount of data buffered

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpBufferData(ProtoHttpRefT *pHttpRef, const char *pData, int32_t iDataSize)
{
    // clamp to the available space
    iDataSize = DS_MIN(iDataSize, pHttpRef->iInpMax-pHttpRef->iInpLen);
    // copy data into buffer
    ds_memcpy(pHttpRef->pInpBuf+pHttpRef->iInpLen, pData, iDataSize);
    pHttpRef->iInpLen += iDataSize;
    // return the amount of buffered data
    return(iDataSize);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetAppendHeader

    \Description
        Set given string as append header, allocating memory as required.

    \Input *pHttpRef    - module state
    \Input *pAppendHdr  - append header string

    \Output
        int32_t         - zero=success, else error

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSetAppendHeader(ProtoHttpRefT *pHttpRef, const char *pAppendHdr)
{
    int32_t iAppendBufLen, iAppendStrLen;

    // check for empty append string, in which case we free the buffer
    if ((pAppendHdr == NULL) || (*pAppendHdr == '\0'))
    {
        if (pHttpRef->pAppendHdr != NULL)
        {
            DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
            pHttpRef->pAppendHdr = NULL;
        }
        pHttpRef->iAppendLen = 0;
        return(0);
    }

    // check to see if append header is already set
    if ((pHttpRef->pAppendHdr != NULL) && (!strcmp(pAppendHdr, pHttpRef->pAppendHdr)))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] ignoring set of append header '%s' that is already set\n", pHttpRef, pAppendHdr));
        return(0);
    }

    // get append header length
    iAppendStrLen = (int32_t)strlen(pAppendHdr);
    // append buffer size includes null and space for \r\n if not included by submitter
    iAppendBufLen = iAppendStrLen + 3;

    // see if we need to allocate a new buffer
    if (iAppendBufLen > pHttpRef->iAppendLen)
    {
        if (pHttpRef->pAppendHdr != NULL)
        {
            DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
        }
        if ((pHttpRef->pAppendHdr = (char *)DirtyMemAlloc(iAppendBufLen, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) != NULL)
        {
            pHttpRef->iAppendLen = iAppendBufLen;
        }
        else
        {
            NetPrintf(("protohttpgdk: [%p] could not allocate %d byte buffer for append header\n", pHttpRef, iAppendBufLen));
            pHttpRef->iAppendLen = 0;
            return(-1);
        }
    }

    // copy append header
    ds_strnzcpy(pHttpRef->pAppendHdr, pAppendHdr, iAppendStrLen+1);

    // if append header is not \r\n terminated, do it here
    if (pAppendHdr[iAppendStrLen-2] != '\r' || pAppendHdr[iAppendStrLen-1] != '\n')
    {
        ds_strnzcat(pHttpRef->pAppendHdr, "\r\n", pHttpRef->iAppendLen);
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpResizeBuffer

    \Description
        Resize the buffer

    \Input *pHttpRef    - module state
    \Input iBufMax      - new buffer size

    \Output
        int32_t         - zero=success, else failure

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpResizeBuffer(ProtoHttpRefT *pHttpRef, int32_t iBufMax)
{
    int32_t iCopySize;
    char *pInpBuf;

    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] resizing input buffer from %d to %d bytes\n", pHttpRef, pHttpRef->iInpMax, iBufMax));
    if ((pInpBuf = (char *)DirtyMemAlloc(iBufMax, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpgdk: [%p] could not resize input buffer\n", pHttpRef));
        return(-1);
    }

    // calculate size of data to copy from old buffer to new
    if ((iCopySize = pHttpRef->iInpLen - pHttpRef->iInpOff) > iBufMax)
    {
        NetPrintf(("protohttpgdk: [%p] warning; resize of input buffer is losing %d bytes of data\n", pHttpRef, iCopySize - iBufMax));
        iCopySize = iBufMax;
    }
    // copy valid contents of input buffer, if any, to new buffer
    ds_memcpy(pInpBuf, pHttpRef->pInpBuf+pHttpRef->iInpOff, iCopySize);

    // dispose of input buffer
    DirtyMemFree(pHttpRef->pInpBuf, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);

    // update buffer variables
    pHttpRef->pInpBuf = pInpBuf;
    pHttpRef->iInpOff = 0;
    pHttpRef->iInpLen = iCopySize;
    pHttpRef->iInpMax = iBufMax;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRead

    \Description
        Read data out of the input buffer

    \Input *pHttpRef    - module state
    \Input *pBuffer     - [out] where we copy the read data to
    \Input iBufMin      - minimum data to read
    \Input iBufMax      - maximum data to read

    \Output
        int32_t         - negative=error, zero=no data available, positive=number of bytes read

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRead(ProtoHttpRefT *pHttpRef, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    int32_t iLen;

    // early out for failure result
    if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
    {
        int32_t iResult = PROTOHTTP_RECVFAIL;
        if (pHttpRef->uProtocolError == ERROR_WINHTTP_REDIRECT_FAILED)
        {
            iResult = PROTOHTTP_RECVRDIR;
        }
        else if (pHttpRef->uProtocolError == ERROR_WINHTTP_TIMEOUT)
        {
            iResult = PROTOHTTP_TIMEOUT;
        }
        return(iResult);
    }
    // waiting for data
    if ((pHttpRef->eState != PROTOHTTP_ST_BODY) && (pHttpRef->eState != PROTOHTTP_ST_DONE))
    {
        return(PROTOHTTP_RECVWAIT);
    }

    // if they only wanted head, thats all they get
    if ((pHttpRef->iHeadSize > 0) && (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_HEAD))
    {
        return(PROTOHTTP_RECVHEAD);
    }

    // if they are querying only for done state when no more data is available to be read
    if ((iBufMax == 0) && (pHttpRef->eState == PROTOHTTP_ST_DONE) && (pHttpRef->iBodyRcvd == pHttpRef->iBodySize))
    {
        return(PROTOHTTP_RECVDONE);
    }

    // make sure range is valid
    if (iBufMax < 1)
    {
        return(0);
    }
    // clamp the range
    iBufMin = DS_MAX(1, iBufMin);
    iBufMax = DS_MAX(iBufMin, iBufMax);
    iBufMin = DS_MIN(iBufMin, pHttpRef->iInpMax);
    iBufMax = DS_MIN(iBufMax, pHttpRef->iInpMax);

    // figure out how much data is available
    if ((iLen = (pHttpRef->iInpLen - pHttpRef->iInpOff)) > iBufMax)
    {
        iLen = iBufMax;
    }

    // check for end of data
    if ((iLen == 0) && (pHttpRef->eState == PROTOHTTP_ST_DONE))
    {
        return(PROTOHTTP_RECVDONE);
    }

    // see if there is enough to return
    if (iLen >= iBufMin)
    {
        // return data to caller
        if (pBuffer != NULL)
        {
            ds_memcpy(pBuffer, pHttpRef->pInpBuf+pHttpRef->iInpOff, iLen);

            #if DIRTYCODE_LOGGING
            if (pHttpRef->iVerbose > 3)
            {
                NetPrintf(("protohttpgdk: [%p] read %d bytes\n", pHttpRef, iLen));
            }
            if (pHttpRef->iVerbose > 4)
            {
                NetPrintMem(pBuffer, iLen, "http-read");
            }
            #endif
        }

        pHttpRef->iInpOff += iLen;
        pHttpRef->iBodyRcvd += iLen;

        // return bytes read
        return(iLen);
    }

    // nothing available
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpReset

    \Description
        Reset the state back to idle

    \Input *pHttpRef    - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpReset(ProtoHttpRefT *pHttpRef)
{
    // if there is no pending abort request and we have an ongoing request then start the async abortion
    if ((pHttpRef->pHttpRequest != NULL) && (_ProtoHttpAsyncOpStart(&pHttpRef->AbortOp) == TRUE))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: abort\n", pHttpRef));

        WinHttpCloseHandle(pHttpRef->pHttpRequest);
        pHttpRef->pHttpRequest = NULL;

        WinHttpCloseHandle(pHttpRef->pHttpConnection);
        pHttpRef->pHttpConnection = NULL;

        // put us int he pending abort state
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_ABRT);
    }
    // if we have a pending request then clear its data as we also still have any abort pending
    else if (pHttpRef->bPendingRequest == TRUE)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] aborting the pending request\n", pHttpRef));
        pHttpRef->bPendingRequest = FALSE;
    }

    ds_memclr(pHttpRef->strHdr, sizeof(pHttpRef->strHdr));
    ds_memclr(pHttpRef->strRequestHdr, sizeof(pHttpRef->strRequestHdr));
    pHttpRef->iHdrCode = 0;
    pHttpRef->iHdrDate = 0;
    pHttpRef->iHdrLen = 0;
    pHttpRef->iHeadSize = 0;
    pHttpRef->iBodySize = 0;
    pHttpRef->iBodyRcvd = 0;
    pHttpRef->iRecvSize = 0;
    pHttpRef->iInpOff = 0;
    pHttpRef->iInpLen = 0;
    pHttpRef->iInpCnt = 0;
    pHttpRef->uProtocolError = 0;
    pHttpRef->uSecureError = 0;
    pHttpRef->bCloseHdr = FALSE;
    pHttpRef->bClosed = FALSE;
    pHttpRef->bHeadersAvailable = FALSE;
    DirtyMemFree(pHttpRef->pUrl, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    pHttpRef->pUrl = NULL;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpWriteCbProcess

    \Description
        Process and send calls to the write callback

    \Input *pHttpRef    - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpWriteCbProcess(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpWriteCbInfoT WriteCbInfo;
    int32_t iResult;

    ds_memclr(&WriteCbInfo, sizeof(WriteCbInfo));
    WriteCbInfo.eRequestType = pHttpRef->eRequestType;
    WriteCbInfo.eRequestResponse = (ProtoHttpResponseE)pHttpRef->iHdrCode;

    /* download more data when the following are true: we are in the body state or we are in the done state but we haven't received
       everything. note, the body size is negative for chunked transfers when we haven't processed the end chunk. */
    if ((pHttpRef->eState == PROTOHTTP_ST_BODY) || ((pHttpRef->eState == PROTOHTTP_ST_DONE) && ((pHttpRef->iBodySize < 0) || (pHttpRef->iBodyRcvd < pHttpRef->iBodySize))))
    {
        char strTempRecv[1024];
        while ((iResult = _ProtoHttpRead(pHttpRef, strTempRecv, 1, sizeof(strTempRecv))) > 0)
        {
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, strTempRecv, iResult, pHttpRef->pUserData);
        }
    }

    if (pHttpRef->eState > PROTOHTTP_ST_BODY)
    {
        if (pHttpRef->eState == PROTOHTTP_ST_DONE)
        {
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, "", pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_HEAD ? PROTOHTTP_HEADONLY : PROTOHTTP_RECVDONE, pHttpRef->pUserData);
        }
        if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
        {
            iResult = PROTOHTTP_RECVFAIL;
            if (pHttpRef->uProtocolError == ERROR_WINHTTP_REDIRECT_FAILED)
            {
                iResult = PROTOHTTP_RECVRDIR;
            }
            else if (pHttpRef->uProtocolError == ERROR_WINHTTP_TIMEOUT)
            {
                iResult = PROTOHTTP_TIMEOUT;
            }
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, "", iResult, pHttpRef->pUserData);
        }
        pHttpRef->pWriteCb = NULL;
        pHttpRef->pReqCustomHeaderCb = NULL;
        pHttpRef->pReqReceiveHeaderCb = NULL;
        pHttpRef->pUserData = NULL;
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateInit

    \Description
        Handle the ST_INIT state of initializing the winhttp session when the network
        is ready.

    \Input *pHttpRef    - module state

    \Notes
        This is a requirement for using the WinHTTP APIs otherwise the APIs might
        perform non-deterministically and may crash.
        On completion, this will move the module into the ST_IDLE state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateInit(ProtoHttpRefT *pHttpRef)
{
    char strUserAgent[256] = { 0 };
    wchar_t wstrUserAgent[256] = { 0 };
    DWORD uOption = 0;
    HINTERNET pHttpSession;

    // check if netconn has reached the appropriate state
    if ((NetConnStatus('open', 0, NULL, 0) == 0) || (NetConnStatus('nste', 0, NULL, 0) == NetworkConnectivityLevelHintUnknown))
    {
        return;
    }

    // format the user agent
    ds_snzprintf(strUserAgent, sizeof(strUserAgent), "ProtoHttp %d.%d/DS %d.%d.%d.%d.%d (" DIRTYCODE_PLATNAME ")",
        (PROTOHTTP_VERSION>>8)&0xff, PROTOHTTP_VERSION&0xff, DIRTYSDK_VERSION_YEAR, DIRTYSDK_VERSION_SEASON, DIRTYSDK_VERSION_MAJOR,
        DIRTYSDK_VERSION_MINOR, DIRTYSDK_VERSION_PATCH);
    mbstowcs(wstrUserAgent, strUserAgent, sizeof(wstrUserAgent) / sizeof(wstrUserAgent[0]));

    // startup winhttp
    if ((pHttpSession = WinHttpOpen(wstrUserAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, NULL, NULL, pHttpRef->uWinHttpFlags)) == NULL)
    {
        NetPrintf(("protohttpgdk: [%p] could not open winhttp session, failed with %s\n", pHttpRef, DirtyErrGetName(GetLastError())));
        return;
    }

    // disable http/2 in this module
    if (!WinHttpSetOption(pHttpSession, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &uOption, sizeof(uOption)))
    {
        NetPrintf(("protohttpgdk: [%p] WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL failed with %s\n", pHttpRef, DirtyErrGetName(GetLastError())));
        return;
    }

    // set the max redirects
    if (!WinHttpSetOption(pHttpSession, WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS, &pHttpRef->iMaxRedirect, sizeof(pHttpRef->iMaxRedirect)))
    {
        NetPrintf(("protohttpgdk: [%p] WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS failed with %s\n", pHttpRef, DirtyErrGetName(GetLastError())));
        return;
    }

    // set the max response header size
    if (!WinHttpSetOption(pHttpSession, WINHTTP_OPTION_MAX_RESPONSE_HEADER_SIZE, &pHttpRef->iInpMax, sizeof(pHttpRef->iInpMax)))
    {
        NetPrintf(("protohttpgdk: [%p] WINHTTP_OPTION_MAX_RESPONSE_HEADER_SIZE failed with %s\n", pHttpRef, DirtyErrGetName(GetLastError())));
        return;
    }

    // set status callback
    WinHttpSetStatusCallback(pHttpSession, _ProtoHttpStatusCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0);

    // save the session
    pHttpRef->pHttpSession = pHttpSession;
    // move to the idle state
    _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_IDLE);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateAbrt

    \Description
        Handle the ST_ABRT state of processing the aborting of our request

    \Input *pHttpRef    - module state

    \Notes
        After completion this will move us into the ST_CONN/ST_AUTH (depends on settings)
        or ST_FAIL state based on if we have a pending request.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateAbrt(ProtoHttpRefT *pHttpRef)
{
    int32_t iResult;

    // if there is an operation pending early out
    if (pHttpRef->AbortOp.eState == PROTOHTTP_ASYNC_OP_PENDING)
    {
        return;
    }

    // get the result of the operation, if it is complete
    _ProtoHttpAsyncOpGetResult(&pHttpRef->AbortOp, NULL);

    /* if there is not a pending request then we set the failure state
       otherwise we would just continue with the request we made */
    if (pHttpRef->bPendingRequest == FALSE)
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return;
    }
    pHttpRef->bPendingRequest = FALSE;

    /* if we need to query an auth token we would have already kicked off
       the request. change the state to allow for update to check the request */
    if (pHttpRef->bGetAuthToken == TRUE)
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_AUTH);
        return;
    }

    // otherwise just send the request
    if ((iResult = _ProtoHttpSendRequest(pHttpRef)) < 0)
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateAuth

    \Description
        Handle the ST_AUTH state of processing the token request to add to our
        headers

    \Input *pHttpRef    - module state

    \Notes
        After completion this will move us into the ST_CONN or ST_FAIL state
        based on if the operation was successful or not.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateAuth(ProtoHttpRefT *pHttpRef)
{
    int32_t iResult, iTokenLen;
    char strAuthToken[3*1024];
    const char *pTokenUrl = (pHttpRef->strTokenUrl[0] != '\0') ? pHttpRef->strTokenUrl : pHttpRef->strHost;

    if ((iResult = DirtyAuthCheckToken(pHttpRef->iAuthTokenIdx, pTokenUrl, &iTokenLen, strAuthToken)) == DIRTYAUTH_SUCCESS)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] token acquired (len=%d); transitioning to connect state\n", pHttpRef, iTokenLen));

        // format request header and send the request
        if ((iResult = _ProtoHttpFormatRequestHeader(pHttpRef, pHttpRef->pUrl, pHttpRef->strHost, pHttpRef->iPort, pHttpRef->bSecure, _ProtoHttp_strRequestType[pHttpRef->eRequestType], pHttpRef->iPostSize, strAuthToken)) >= 0)
        {
            iResult = _ProtoHttpSendRequest(pHttpRef);
        }
        DirtyMemFree(pHttpRef->pUrl, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
        pHttpRef->pUrl = NULL;
    }
    else if (iResult == DIRTYAUTH_PENDING)
    {
        // pending clear the result
        iResult = 0;
    }
    else
    {
        NetPrintf(("protohttpgdk: [%p] token request failed with %d\n", pHttpRef, iResult));
        DirtyMemFree(pHttpRef->pUrl, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
        pHttpRef->pUrl = NULL;
    }

    // if any failure occurred then move to the failure state
    if (iResult < 0)
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateSend

    \Description
        Handle the ST_SEND state which sends our buffered data

    \Input *pHttpRef    - module state

    \Notes
        This function will early out if we are currently pending a send
        operation. After completion this will move us to the ST_RESP state.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateSend(ProtoHttpRefT *pHttpRef)
{
    uint32_t uBytesWritten;

    // if there is an operation pending early out
    if (pHttpRef->ReadWriteOp.eState == PROTOHTTP_ASYNC_OP_PENDING)
    {
        return;
    }

    // get the result of the operation is it is complete
    if (_ProtoHttpAsyncOpGetResult(&pHttpRef->ReadWriteOp, &uBytesWritten) == TRUE)
    {
        // update the amount of bytes written
        pHttpRef->iInpCnt += uBytesWritten;
        pHttpRef->iInpOff += uBytesWritten;

    }
    // if we have written all our buffered data, move to the next state
    if (pHttpRef->iInpOff == pHttpRef->iInpLen)
    {
        pHttpRef->iInpLen = pHttpRef->iInpOff = 0;
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_RESP);
    }
    // otherwise send more data
    else if ((pHttpRef->iInpLen - pHttpRef->iInpOff) > 0)
    {
        _ProtoHttpSend(pHttpRef, pHttpRef->pInpBuf+pHttpRef->iInpOff, pHttpRef->iInpLen-pHttpRef->iInpOff);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateResp

    \Description
        Handle the ST_RESP state which sends buffered data provided by the user.

    \Input *pHttpRef    - module state

    \Notes
        This function will early out if we are currently pending a send
        operation. The data has to be buffered due to WinHttp needed the data
        to stay in place until the async operation is complete. After completion
        this move us to the ST_HEAD on success or ST_FAIL on failure.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateResp(ProtoHttpRefT *pHttpRef)
{
    uint32_t uBytesWritten;

    // if there is an operation pending early out
    if (pHttpRef->ReadWriteOp.eState == PROTOHTTP_ASYNC_OP_PENDING)
    {
        return;
    }

    // get the result of the operation is it is complete
    if (_ProtoHttpAsyncOpGetResult(&pHttpRef->ReadWriteOp, &uBytesWritten) == TRUE)
    {
        // update the amount of bytes written
        pHttpRef->iInpCnt += uBytesWritten;
        pHttpRef->iInpOff += uBytesWritten;

    }
    // check if we have written all our data (in the case of chunked transfer), reset our offsets
    if ((pHttpRef->iInpLen > 0) && (pHttpRef->iInpLen == pHttpRef->iInpOff))
    {
        pHttpRef->iInpLen = 0;
        pHttpRef->iInpOff = 0;
    }
    // otherwise send more data
    else if ((pHttpRef->iInpLen - pHttpRef->iInpOff) > 0)
    {
        _ProtoHttpSend(pHttpRef, pHttpRef->pInpBuf+pHttpRef->iInpOff, pHttpRef->iInpLen-pHttpRef->iInpOff);
    }
    // if we sent all our data or we are streaming and wrote the zero sized chunk then receive our response
    else if ((pHttpRef->iPostSize == 0) || (pHttpRef->iPostSize == pHttpRef->iInpCnt))
    {
        // reset the input count as we will be using it to track recv
        pHttpRef->iInpCnt = 0;

        // receive the response to get the headers
        if (WinHttpReceiveResponse(pHttpRef->pHttpRequest, NULL))
        {
            _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_HEAD);
        }
        else
        {
            _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpReceiveResponse");
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpProcessRedirect

    \Description
        Handle redirection header (response code=3xx)

    \Input *pHttpRef    - module state

    \Output
        int32_t         - 0=no redirect, 1=redirected, <0=failure

    \Notes
        A maximum of PROTOHTTP_MAXREDIRECT redirections is allowed.  Any further
        redirection attempts will result in a failure state.  A redirection
        Location url is limited based on the size of the http receive buffer.

        Auto-redirection is implemented as specified by RFC:
        (http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3);
        if auto-redirection is not performed, processing ends and it is the
        responsibility of the application to recognize the 3xx result code
        and handle it accordingly.

        Auto-redirection can be disabled by setting the maximum number of
        redirections allowed to zero.

    \Version 03/06/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpProcessRedirect(ProtoHttpRefT *pHttpRef)
{
    char strHost[sizeof(pHttpRef->strHost)], strKind[PROTOHTTPUTIL_SCHEME_MAX];
    int32_t iPort, iResult, iSecure, iUrlLen;
    char *pUrlBuf;

    // do not auto-redirect multiplechoices or notmodified responses
    if ((pHttpRef->iHdrCode == PROTOHTTP_RESPONSE_MULTIPLECHOICES) || (pHttpRef->iHdrCode == PROTOHTTP_RESPONSE_NOTMODIFIED))
    {
        return(0);
    }
    // do not auto-redirect response that are not head or get requests, and are not a SEEOTHER response
    if ((pHttpRef->eRequestType != PROTOHTTP_REQUESTTYPE_GET) && (pHttpRef->eRequestType != PROTOHTTP_REQUESTTYPE_HEAD))
    {
        /* As per HTTP 1.1 RFC, 303 SEEOTHER POST requests may be auto-redirected to a GET request.  302 FOUND responses
           to a POST request are not supposed to be auto-redirected; however, there is a note in the RFC indicating that
           this is a common client behavior, and it is additionally a common server behavior to use 302 even when automatic
           redirection is intended, as some clients do not support 303 SEEOTHER.  Therefore, we perform auto-redirection
           on 302 FOUND responses to POST requests with a GET request for compatibility with servers that choose this
           behavior */
        if ((pHttpRef->iHdrCode == PROTOHTTP_RESPONSE_FOUND) || (pHttpRef->iHdrCode == PROTOHTTP_RESPONSE_SEEOTHER))
        {
            pHttpRef->eRequestType = PROTOHTTP_REQUESTTYPE_GET;
            pHttpRef->iPostSize = 0;
        }
        else
        {
            return(0);
        }
    }

    // get size of location header
    if ((iUrlLen = ProtoHttpGetLocationHeader(pHttpRef, pHttpRef->pInpBuf, NULL, 0, NULL)) <= 0)
    {
        NetPrintf(("protohttpgdk: [%p] no location included in redirect header\n", pHttpRef));
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return(-1);
    }

    // get url at the end of input buffer
    pUrlBuf = pHttpRef->pInpBuf + pHttpRef->iInpMax - iUrlLen;
    if (ProtoHttpGetLocationHeader(pHttpRef, pHttpRef->pInpBuf, pUrlBuf, iUrlLen, NULL) != 0)
    {
        NetPrintf(("protohttpgdk: [%p] failed to get location header url\n", pHttpRef));
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return(-1);
    }

    // parse url for protocol
    pUrlBuf = (char *)ProtoHttpUrlParse(pUrlBuf, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure);
    // only auto-redirect if http/s protocol
    if ((ds_stricmp(strKind, "https") != 0) && (ds_stricmp(strKind, "http") != 0))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] skipping auto-redirection of non-http protocol '%s'\n", pHttpRef, strKind));
        return(0);
    }

    // process based on max redirections allowed; zero=disabled
    if (pHttpRef->iMaxRedirect == 0)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] auto-redirection disabled\n", pHttpRef));
        return(0);
    }
    else if (++pHttpRef->iNumRedirect > pHttpRef->iMaxRedirect)
    {
        NetPrintf(("protohttpgdk: [%p] maximum number of redirections (%d) exceeded\n", pHttpRef, pHttpRef->iMaxRedirect));
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return(-1);
    }

    // close connection?
    if (pHttpRef->bCloseHdr == TRUE)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: server request\n", pHttpRef));

        WinHttpCloseHandle(pHttpRef->pHttpConnection);
        pHttpRef->pHttpConnection = NULL;
    }

    // format redirection request
    if ((iResult = _ProtoHttpFormatRequestHeader(pHttpRef, pUrlBuf, strHost, iPort, iSecure, _ProtoHttp_strRequestType[pHttpRef->eRequestType], 0, NULL)) < 0)
    {
        NetPrintf(("protohttpgdk: [%p] redirect header format request failed\n", pHttpRef));
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return(-1);
    }
    // send redirection request
    _ProtoHttpSendRequest(pHttpRef);
    return(1);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateHead

    \Description
        Handle the ST_HEAD state which parses the response headers

    \Input *pHttpRef    - module state

    \Notes
        This function waits until to callback has fired that signals that the
        headers are avaiable. After completion this will move us into the ST_BODY
        on success or ST_FAIL on failure.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateHead(ProtoHttpRefT *pHttpRef)
{
    int32_t iResult = 0;
    DWORD uInpMax = pHttpRef->iWInpMax;
    mbstate_t MbState;
    const wchar_t *pToConvert = pHttpRef->pWInpBuf;

    // early out if we haven't yet received the headers
    if (pHttpRef->bHeadersAvailable == FALSE)
    {
        return;
    }

    ds_memclr(pHttpRef->pWInpBuf, uInpMax);

    // pull the headers out and write them into our input buffer
    if (!WinHttpQueryHeaders(pHttpRef->pHttpRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, pHttpRef->pWInpBuf, &uInpMax, NULL))
    {
        NetPrintf(("protohttpgdk: result = %u\n", uInpMax));
        _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpQueryHeaders");
        return;
    }
    wcsrtombs(pHttpRef->pInpBuf, &pToConvert, pHttpRef->iInpMax, &MbState);
    pHttpRef->iInpLen = uInpMax / sizeof(*pHttpRef->pWInpBuf);

    // parse the information out of the header
    if (_ProtoHttpParseHeader(pHttpRef) < 0)
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_FAIL);
        return;
    }
    // we can write new data now that the header has been parsed
    pHttpRef->iInpOff = pHttpRef->iInpLen = 0;

    // handle response code?
    if (PROTOHTTP_GetResponseClass(pHttpRef->iHdrCode) == PROTOHTTP_RESPONSE_REDIRECTION)
    {
        iResult = _ProtoHttpProcessRedirect(pHttpRef);
    }

    // if we are not redirecting then process the body normally. in the case of a redirection or error we should not try to process the body
    if (iResult == 0)
    {
        // set the pending query as it could trigger synchronously (the callback will fire within the same stack)
        _ProtoHttpAsyncOpStart(&pHttpRef->QueryOp);

        // check to see if there is a response
        if (WinHttpQueryDataAvailable(pHttpRef->pHttpRequest, NULL))
        {
            _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_BODY);
        }
        else
        {
            _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpQueryDataAvailable");
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateBody

    \Description
        Handle the ST_BODY state that receives the response body from the server

    \Input *pHttpRef    - module state

    \Notes
        This function will early out if we are receiving data or query for more
        data. On completion this function will move to ST_DONE on success and
        ST_FAIL on failure.

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateBody(ProtoHttpRefT *pHttpRef)
{
    uint32_t uBytesMax, uBytesRead, uBytesAvailable;

    // early out if there is a pending operation
    if ((pHttpRef->ReadWriteOp.eState == PROTOHTTP_ASYNC_OP_PENDING) || (pHttpRef->QueryOp.eState == PROTOHTTP_ASYNC_OP_PENDING))
    {
        return;
    }

    // process the read if it was completed
    if (_ProtoHttpAsyncOpGetResult(&pHttpRef->ReadWriteOp, &uBytesRead) == TRUE)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpgdk: [%p] recv %d bytes\n", pHttpRef, uBytesRead));
        #if DIRTYCODE_LOGGING
        if (pHttpRef->iVerbose > 3)
        {
            NetPrintMem(pHttpRef->pInpBuf+pHttpRef->iInpLen, uBytesRead, "http-recv");
        }
        #endif

        pHttpRef->iInpLen += uBytesRead;
        pHttpRef->iInpCnt += uBytesRead;
        pHttpRef->uBytesAvailable -= uBytesRead;
    }

    // process the query if it was completed
    if (_ProtoHttpAsyncOpGetResult(&pHttpRef->QueryOp, &uBytesAvailable) == TRUE)
    {
        // check if the request is complete (0 bytes available)
        if (uBytesAvailable == 0)
        {
            // update the state and amount read
            _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_DONE);
            pHttpRef->iBodySize = pHttpRef->iInpCnt;
        }
        // otherwise update the available bytes
        else
        {
            pHttpRef->uBytesAvailable = uBytesAvailable;
        }
    }

    /* before check what available space we have in our buffer, check if we have read everything to reset the read points.
       this has to happen here instead of in _ProtoHttpRead to make sure we don't touch the iInpLen while we have a pending
       call to WinHttpReadData. doing otherwise would potentially corrupt what is being downloaded by putting it in the
       wrong location */
    if (pHttpRef->iInpOff == pHttpRef->iInpLen)
    {
        pHttpRef->iInpOff = pHttpRef->iInpLen = 0;
    }

    // if we have available space in our buffer and there is data to read then read it
    if ((uBytesMax = DS_MIN(pHttpRef->uBytesAvailable, (unsigned)(pHttpRef->iInpMax - pHttpRef->iInpLen))) > 0)
    {
        // set the pending read as it could trigger synchronously (the callback will fire within the same stack)
        _ProtoHttpAsyncOpStart(&pHttpRef->ReadWriteOp);

        // kick off the read of data
        if (!WinHttpReadData(pHttpRef->pHttpRequest, pHttpRef->pInpBuf+pHttpRef->iInpLen, uBytesMax, NULL))
        {
            _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpReadData");
        }
    }
    // otherwise query more data if we have not moved into the completion state
    else if ((pHttpRef->uBytesAvailable == 0) && (pHttpRef->eState != PROTOHTTP_ST_DONE))
    {
        // set the pending query as it could trigger synchronously (the callback will fire within the same stack)
        _ProtoHttpAsyncOpStart(&pHttpRef->QueryOp);

        // query for more data to read
        if (!WinHttpQueryDataAvailable(pHttpRef->pHttpRequest, NULL))
        {
            _ProtoHttpOnError(pHttpRef, GetLastError(), "WinHttpQueryDataAvailable");
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpDestroy

    \Description
        Cleans up the module after we are in a safe state to destroy

    \Input *pHttpRef    - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static void _ProtoHttpDestroy(ProtoHttpRefT *pHttpRef)
{
    // close any pending connection
    if (pHttpRef->pHttpConnection != NULL)
    {
        WinHttpCloseHandle(pHttpRef->pHttpConnection);
    }

    // kill the crit
    NetCritKill(&pHttpRef->HttpCrit);

    // unregister the callback and destroy the session
    WinHttpSetStatusCallback(pHttpRef->pHttpSession, NULL, 0, 0);
    WinHttpCloseHandle(pHttpRef->pHttpSession);

    // cleanup any remaining memory
    DirtyMemFree(pHttpRef->pUrl, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    DirtyMemFree(pHttpRef->pWInpBuf, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    DirtyMemFree(pHttpRef->pInpBuf, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    DirtyMemFree(pHttpRef, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpDestroyInternal

    \Description
        Called internally by the netconn external cleanup to process our async
        destroy

    \Input *pUserData   - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpDestroyInternal(void *pUserData)
{
    ProtoHttpRefT *pHttpRef = (ProtoHttpRefT *)pUserData;

    // update the ref to do any processing
    ProtoHttpUpdate(pHttpRef);

    // if the abort is complete and we are in the failure state, call destroy
    if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
    {
        _ProtoHttpDestroy(pHttpRef);
        return(0);
    }
    else
    {
        // try again
        return(-1);
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

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
ProtoHttpRefT *ProtoHttpCreate(int32_t iBufSize)
{
    ProtoHttpRefT *pHttpRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // clamp the buffer size
    if (iBufSize < 4*1024)
    {
        iBufSize = 4*1024;
    }

    // allocate the resources
    if ((pHttpRef = (ProtoHttpRefT *)DirtyMemAlloc(sizeof(*pHttpRef), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpgdk: [%p] unable to allocate module state\n", pHttpRef));
        return(NULL);
    }
    ds_memclr(pHttpRef, sizeof(*pHttpRef));

    // save memgroup (will be used in ProtoHttpDestroy)
    pHttpRef->iMemGroup = iMemGroup;
    pHttpRef->pMemGroupUserData = pMemGroupUserData;

    // allocate the main buffer
    if ((pHttpRef->pInpBuf = (char *)DirtyMemAlloc(iBufSize, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpgdk: [%p] unable to allocate protohttp buffer\n", pHttpRef));
        ProtoHttpDestroy(pHttpRef);
        return(NULL);
    }
    pHttpRef->iInpMax = iBufSize;

    // allocate the wide character buffer
    if ((pHttpRef->pWInpBuf = (wchar_t *)DirtyMemAlloc(iBufSize * sizeof(*pHttpRef->pWInpBuf), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpgdk: [%p] unable to allocate the widechar protohttp buffer\n", pHttpRef));
        ProtoHttpDestroy(pHttpRef);
        return(NULL);
    }
    pHttpRef->iWInpMax = iBufSize * sizeof(*pHttpRef->pWInpBuf);

    // initialize other settings
    pHttpRef->iVerbose = 1;

    // save params & set defaults
    pHttpRef->iMaxRedirect = PROTOHTTP_MAXREDIRECT;
    pHttpRef->eState = PROTOHTTP_ST_INIT;
    pHttpRef->iKeepAlive = pHttpRef->iKeepAliveDflt = 1;
    pHttpRef->bReuseOnPost = TRUE;
    pHttpRef->bAuthenticate = TRUE;
    pHttpRef->uWinHttpFlags = _ProtoHttp_uWinHttpFlags;

    // create the crit for read/write operations
    NetCritInit(&pHttpRef->HttpCrit, "ProtoHttpGDK");

    NetPrintf(("protohttpgdk: [%p] creation successful\n", pHttpRef));

    // call update in an attempt to move from init -> idle if the network is available
    ProtoHttpUpdate(pHttpRef);

    // return the state
    return(pHttpRef);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCallback

    \Description
        Set header callbacks.

    \Input *pHttpRef          - module state
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
        header, as protohttp appends itself.

        The ProtoHttpReceiveHeaderCbT callback is used to view the header
        immediately on reception.  It can be used when the built-in header
        cache (retrieved with ProtoHttpStatus('htxt') is too small to hold
        the entire header received.  It is also possible with this method
        to view redirection response headers that cannot be retrieved
        normally.  This can be important if, for example, the application
        wishes to attach new cookies to a redirection response.  The
        custom response header and custom header callback can be used in
        conjunction to implement this type of functionality.

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
void ProtoHttpCallback(ProtoHttpRefT *pHttpRef, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pCallbackRef)
{
    pHttpRef->pCustomHeaderCb  = pCustomHeaderCb;
    pHttpRef->pReceiveHeaderCb = pReceiveHeaderCb;
    pHttpRef->pCallbackRef     = pCallbackRef;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDestroy

    \Description
        Destroy the module and release its state

    \Input *pHttpRef    - module state

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
void ProtoHttpDestroy(ProtoHttpRefT *pHttpRef)
{
    // use external destruction mechanism of netconn if we need to wait for an abort to complete
    if (pHttpRef->pHttpRequest != NULL)
    {
        ProtoHttpAbort(pHttpRef);
        NetConnControl('recu', 0, 0, (void *)_ProtoHttpDestroyInternal, pHttpRef);
    }
    else
    {
        _ProtoHttpDestroy(pHttpRef);
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGet

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a getting
        data from the appropriate server.

    \Input *pHttpRef    - module state
    \Input *pUrl        - the url to retrieve
    \Input bHeadOnly    - if TRUE only get header

    \Output
        int32_t         - negative=failure, else success

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpGet(ProtoHttpRefT *pHttpRef, const char *pUrl, uint32_t bHeadOnly)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecv

    \Description
        Return the actual url data.

    \Input *pHttpRef    - module state
    \Input *pBuffer     - buffer to store data in
    \Input iBufMin      - minimum number of bytes to return (returns zero if this number is not available)
    \Input iBufMax      - maximum number of bytes to return (buffer size)

    \Output
        int32_t         - negative=error, zero=no data available or bufmax <= 0, positive=number of bytes read

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpRecv(ProtoHttpRefT *pHttpRef, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    int32_t iResult;

    // handle rate throttling, if enabled
    iBufMax = SocketRateThrottle(&pHttpRef->RecvRate, SOCK_STREAM, iBufMax, "recv");

    NetCritEnter(&pHttpRef->HttpCrit);
    iResult = _ProtoHttpRead(pHttpRef, pBuffer, iBufMin, iBufMax);
    NetCritLeave(&pHttpRef->HttpCrit);

    // update data rate estimation
    SocketRateUpdate(&pHttpRef->RecvRate, iResult, "recv");

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecvAll

    \Description
        Return all of the url data.

    \Input *pHttpRef    - module state
    \Input *pBuffer     - buffer to store data in
    \Input iBufSize     - size of buffer

    \Output
        int32_t         - PROTOHTTP_RECV*, or positive=bytes in response

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpRecvAll(ProtoHttpRefT *pHttpRef, char *pBuffer, int32_t iBufSize)
{
    int32_t iRecvMax, iRecvResult;

    // try to read as much data as possible (leave one byte for null termination)
    for (iRecvMax = iBufSize-1; (iRecvResult = ProtoHttpRecv(pHttpRef, pBuffer + pHttpRef->iRecvSize, 1, iRecvMax - pHttpRef->iRecvSize)) > 0; )
    {
        // add to amount received
        pHttpRef->iRecvSize += iRecvResult;
    }

    // check response code
    if (iRecvResult == PROTOHTTP_RECVDONE)
    {
        // null-terminate response & return completion success
        if ((pBuffer != NULL) && (iBufSize > 0))
        {
            pBuffer[pHttpRef->iRecvSize] = '\0';
        }
        iRecvResult = pHttpRef->iRecvSize;
    }
    else if ((iRecvResult < 0) && (iRecvResult != PROTOHTTP_RECVWAIT))
    {
        // an error occurred
        NetPrintf(("protohttpgdk: [%p] error %d receiving response\n", pHttpRef, iRecvResult));
    }
    else if (iRecvResult == 0)
    {
        iRecvResult = (pHttpRef->iRecvSize < iRecvMax) ? PROTOHTTP_RECVWAIT : PROTOHTTP_RECVBUFF;
    }

    // return result to caller
    return(iRecvResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPost

    \Description
        Initiate transfer of data to to the server via a HTTP POST command.

    \Input *pHttpRef    - module state
    \Input *pUrl        - the URL that identifies the POST action.
    \Input *pData       - pointer to URL data (optional, may be NULL)
    \Input iDataSize    - size of data being uploaded (see Notes below)
    \Input bDoPut       - meanless on xboxone

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

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpPost(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pData, int64_t iDataSize, uint32_t bDoPut)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, pData, iDataSize, bDoPut ? PROTOHTTP_REQUESTTYPE_PUT : PROTOHTTP_REQUESTTYPE_POST));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSend

    \Description
        Send data during an ongoing Post transaction.

    \Input *pHttpRef    - module state
    \Input *pData       - pointer to data to send
    \Input iDataSize    - size of data being sent

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpSend(ProtoHttpRefT *pHttpRef, const char *pData, int32_t iDataSize)
{
    int32_t iResult;

    // make sure we are in the sending state
    if (pHttpRef->eState < PROTOHTTP_ST_RESP)
    {
        // not ready to send data yet
        return(0);
    }
    else if (pHttpRef->eState != PROTOHTTP_ST_RESP)
    {
        // if we're past ST_RESP, an error occurred
        return(-1);
    }

    // handle optional data rate throttling
    if ((iDataSize = SocketRateThrottle(&pHttpRef->SendRate, SOCK_STREAM, iDataSize, "send")) == 0)
    {
        return(0);
    }

    // get access to the crit
    NetCritEnter(&pHttpRef->HttpCrit);
    // send the data
    iResult = (pHttpRef->iPostSize < 0) ? _ProtoHttpFormatChunk(pHttpRef, pData, iDataSize) : _ProtoHttpBufferData(pHttpRef, pData, iDataSize);
    // release access to the crit
    NetCritLeave(&pHttpRef->HttpCrit);

    // update data rate estimation
    SocketRateUpdate(&pHttpRef->SendRate, iResult, "send");

    // return result
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDelete

    \Description
        Request deletion of a server-based resource.

    \Input *pHttpRef    - module state
    \Input *pUrl        - Url describing reference to delete

    \Output
        int32_t         - negative=failure, zero=success

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpDelete(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_DELETE));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpOptions

    \Description
        Request options from a server.

    \Input *pHttpRef    - module state
    \Input *pUrl        - Url describing reference to get options on

    \Output
        int32_t         - negative=failure, zero=success

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpOptions(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_OPTIONS));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPatch

    \Description
        Initiate transfer of data to to the server via a HTTP PATCH command.

    \Input *pHttpRef    - module state
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

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpPatch(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pData, int64_t iDataSize)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, pData, iDataSize, PROTOHTTP_REQUESTTYPE_PATCH));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRequestCb2

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a transfer
        from the appropriate server.  Use ProtoHttpRequest() macro wrapper if
        callback is not required.

    \Input *pHttpRef            - module state
    \Input *pUrl                - the url to retrieve
    \Input *pData               - user data to send to server (PUT, POST and PATCH only)
    \Input iDataSize            - size of user data to send to server (PUT, POST and PATCH only)
    \Input eRequestType         - request type to make
    \Input *pWriteCb            - write callback (optional)
    \Input *pCustomHeaderCb     - custom header callback (optional)
    \Input *pReceiveHeaderCb    - receive header callback (optional)
    \Input *pUserData           - callback user data (optional)

    \Output
        int32_t                 - >=0: amount of bytes processed, <0:=failure

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpRequestCb2(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pData, int64_t iDataSize, ProtoHttpRequestTypeE eRequestType, ProtoHttpWriteCbT *pWriteCb, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pUserData)
{
    char strKind[8], strHost[sizeof(pHttpRef->strHost)];
    ProtoHttpStateE eState = pHttpRef->eState;
    int32_t iPort, bSecure, iResult = 0, iUrlLen;
    uint8_t bPortSpecified, bRecvToken = pHttpRef->bGetAuthToken;

    // save callbacks
    pHttpRef->pWriteCb = pWriteCb;
    pHttpRef->pReqCustomHeaderCb = pCustomHeaderCb;
    pHttpRef->pReqReceiveHeaderCb = pReceiveHeaderCb;
    pHttpRef->pUserData = pUserData;

    // remember the size of the request
    pHttpRef->iPostSize = iDataSize;

    // check to make sure that the initialization of winhttp has completed
    if (pHttpRef->eState == PROTOHTTP_ST_INIT)
    {
        NetPrintf(("protohttpgdk: [%p] unable to process request while still in the init state. make sure that protohttpupdate is being called regularly\n", pHttpRef));
        return(PROTOHTTP_EAGAIN);
    }

    NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] %s %s\n", pHttpRef, _ProtoHttp_strRequestType[eRequestType], pUrl));
    pHttpRef->eRequestType = eRequestType;

    // attempt an abort, if there is no request pending or already an abort in progress this is no-op.
    _ProtoHttpReset(pHttpRef);

    // parse the url for kind, host and port
    pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &bSecure, &bPortSpecified);
    iUrlLen = (int32_t)strlen(pUrl) + 1;

    // fill in any missing info (relative url) if available
    if (_ProtoHttpApplyBaseUrl(pHttpRef, strKind, strHost, sizeof(strHost), &iPort, &bSecure, bPortSpecified) != 0)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] %s %s://%s:%d%s\n", pHttpRef, _ProtoHttp_strRequestType[eRequestType], bSecure ? "https" : "http", strHost, iPort, pUrl));
    }

    // assume we don't want a new connection to start with
    pHttpRef->bNewConnection = FALSE;

    // determine if host, port, or security settings have changed since the previous request
    if ((bSecure != pHttpRef->bSecure) || (ds_stricmp(strHost, pHttpRef->strHost) != 0) || (iPort != pHttpRef->iPort))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] requesting new connection -- url change to %s\n", pHttpRef, strHost));

        // reset keep-alive
        pHttpRef->iKeepAlive = pHttpRef->iKeepAliveDflt;

        // save new server/port/security state
        ds_strnzcpy(pHttpRef->strHost, strHost, sizeof(pHttpRef->strHost));
        pHttpRef->iPort = iPort;
        pHttpRef->bSecure = bSecure;

        // make sure we use a new connection
        pHttpRef->bNewConnection = TRUE;
    }

    // check to see if previous connection (if any) is still active
    if ((pHttpRef->bNewConnection == FALSE) && (pHttpRef->pHttpConnection == NULL))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] requesting new connection -- previous connection was closed\n", pHttpRef));
        pHttpRef->bNewConnection = TRUE;
    }

    // check to make sure we are in a known valid state
    if ((pHttpRef->bNewConnection == FALSE) && (eState != PROTOHTTP_ST_IDLE) && (eState != PROTOHTTP_ST_DONE))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] request new connection -- current state %d does not allow connection reuse\n", pHttpRef, eState));
        pHttpRef->bNewConnection = TRUE;
    }

    // if executing put/post, check to see if connection reuse on request is allowed
    if ((pHttpRef->bNewConnection == FALSE) && (pHttpRef->bReuseOnPost == FALSE) && ((eRequestType == PROTOHTTP_REQUESTTYPE_PUT) || (eRequestType == PROTOHTTP_REQUESTTYPE_PATCH) || (eRequestType == PROTOHTTP_REQUESTTYPE_POST)))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] request new connection -- reuse on put/post disabled\n", pHttpRef));
        pHttpRef->bNewConnection = TRUE;
    }

    // get auth token if requested
    if (bRecvToken == TRUE)
    {
        const char *pTokenUrl = (pHttpRef->strTokenUrl[0] != '\0') ? pHttpRef->strTokenUrl : pHttpRef->strHost;

        // make token request
        if ((iResult = DirtyAuthGetToken(pHttpRef->iAuthTokenIdx, pTokenUrl, FALSE)) == DIRTYAUTH_SUCCESS)
        {
            // token is cached and already available, so we proceed directly to connection stage
            bRecvToken = FALSE;
        }
        else if (iResult == DIRTYAUTH_PENDING)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] acquiring token for %s for use in authorization header\n", pHttpRef, pTokenUrl));
        }
        else
        {
            NetPrintf(("protohttpgdk: [%p] DirtyAuthGetToken() returned unexpected result %d\n", pHttpRef, iResult));
            return(-2);
        }
    }

    // format request header and append data if the auth token request is not pending
    if (bRecvToken == FALSE)
    {
        if ((iResult = _ProtoHttpFormatRequestHeader(pHttpRef, pUrl, strHost, iPort, bSecure, _ProtoHttp_strRequestType[eRequestType], iDataSize, NULL)) < 0)
        {
            return(iResult);
        }

        // append data?
        if ((pData != NULL) && (iDataSize > 0))
        {
            if (iDataSize > (pHttpRef->iInpMax - pHttpRef->iInpLen))
            {
                iDataSize = (pHttpRef->iInpMax - pHttpRef->iInpLen);
            }

            // copy data into buffer
            ds_memcpy(pHttpRef->pInpBuf + pHttpRef->iInpLen, pData, (int32_t)iDataSize);
            pHttpRef->iInpLen += iDataSize;
        }
        else if (iDataSize < 0)
        {
            // for a streaming port, return no data written
            iDataSize = 0;
        }
    }
    /* otherwise we need to tell the caller that he must send the data later (if present) because we need to the space for the headers we will be appending
       we could allocate memory to save this off but the complexity of calculating the needed space plus trying to shift the buffer around outways the benefits.
       we allocate the storage for the uri as we need it at the time when we format the request headers */
    else if ((pHttpRef->pUrl = (char *)DirtyMemAlloc(iUrlLen, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) != NULL)
    {
        iDataSize = 0;
        ds_strnzcpy(pHttpRef->pUrl, pUrl, iUrlLen);
    }
    else
    {
        NetPrintf(("protohttpgdk: [%p] failed to allocate temporary storage for our url during auth token requests\n", pHttpRef));
        return(-3);
    }

    // if we are currently aborting then put us in a pending state. the work that was done above is not affected by this operation. the work will continue on after abort is complete
    if (pHttpRef->AbortOp.eState != PROTOHTTP_ASYNC_OP_IDLE)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] queuing the new request as we have an abort pending\n", pHttpRef));
        pHttpRef->bPendingRequest = TRUE;
        return(iDataSize);
    }
    // if the auth token request is not pending then move to the conn state
    else if (bRecvToken == FALSE)
    {
        return(((iResult = _ProtoHttpSendRequest(pHttpRef)) == 0) ? iDataSize : iResult);
    }
    // otherwise wait until the token request is complete
    else
    {
        _ProtoHttpSetState(pHttpRef, PROTOHTTP_ST_AUTH);
        return(iDataSize);
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpAbort

    \Description
        Abort active request, if any.

    \Input *pHttpRef    - module state

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
void ProtoHttpAbort(ProtoHttpRefT *pHttpRef)
{
    NetCritEnter(&pHttpRef->HttpCrit);
    // issue the abort and reset any state needed
    _ProtoHttpReset(pHttpRef);
    NetCritLeave(&pHttpRef->HttpCrit);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetBaseUrl

    \Description
        Set base url that will be used for any relative url references.

    \Input *pHttpRef    - module state
    \Input *pUrl        - base url

    \Version 10/15/2019
*/
/********************************************************************************F*/
void ProtoHttpSetBaseUrl(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    char strHost[sizeof(pHttpRef->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url for kind, host, and port
    ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // set base info
    NetPrintf(("protohttpgdk: [%p] setting base url to %s://%s:%d\n", pHttpRef, iSecure ? "https" : "http", strHost, iPort));
    ds_strnzcpy(pHttpRef->strBaseHost, strHost, sizeof(pHttpRef->strBaseHost));
    pHttpRef->iBasePort = iPort;
    pHttpRef->bBaseSecure = iSecure;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGetLocationHeader

    \Description
        Get location header from the input buffer.  The Location header requires
        some special processing.

    \Input *pHttpRef    - module state
    \Input *pInpBuf     - buffer holding header text
    \Input *pBuffer     - [out] output buffer for parsed location header, null for size request
    \Input iBufSize     - size of output buffer, zero for size request
    \Input **pHdrEnd    - [out] pointer past end of parsed header (optional)

    \Output
        int32_t         - negative=header not received, not found or not enough space, zero=success, positive=size query result

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpGetLocationHeader(ProtoHttpRefT *pHttpRef, const char *pInpBuf, char *pBuffer, int32_t iBufSize, const char **pHdrEnd)
{
    const char *pLocHdr;
    int32_t iLocLen, iLocPreLen=0;

    // get a pointer to header
    if ((pLocHdr = ProtoHttpFindHeaderValue(pHttpRef->strHdr, "location")) == NULL)
    {
        return(-1);
    }

    /* according to RFC location headers should be absolute, but some webservers respond with relative
       URL's.  we assume any url that does not start with "http://" or "https://" is a relative url,
       and if we find one, we assume the request keeps the same security, port, and host as the previous
       request */
    if (ds_strnicmp(pLocHdr, "http://", 7) && ds_strnicmp(pLocHdr, "https://", 8))
    {
        char strTemp[288]; // space for max DNS name (253 chars) plus max http url prefix

        // format http url prefix
        ds_snzprintf(strTemp, sizeof(strTemp), "%s://%s:%d", pHttpRef->bSecure ? "https" : "http", pHttpRef->strHost, pHttpRef->iPort);

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

    \Input *pHttpRef    - module state
    \Input iSelect      - control selector ('time')
    \Input iValue       - selector specific
    \Input iValue2      - selector specific
    \Input *pValue      - selector specific

    \Output
        int32_t         - selector specific

    \Notes
        Selectors are:

        \verbatim
            SELECTOR    DESCRIPTION
            'apnd'      The given buffer will be appended to future headers sent
                        by ProtoHttp.  Note that the User-Agent and Accept lines
                        in the default header will be replaced, so if these lines
                        are desired, they should be supplied in the append header.
            'auth'      Enables/disables authentication (default=enabled)
            'disc'      Close current connection, if any.
            'hver'      Sets header type verification: zero=disabled, else enabled
            'ires'      Resize input buffer
            'keep'      Sets keep-alive; zero=disabled, else enabled
            'maxr'      set max recv rate (bytes/sec; zero=uncapped)
            'maxs'      set max send rate (bytes/sec; zero=uncapped)
            'rmax'      Sets maximum number of redirections (default=3; 0=disable)
            'rput'      Sets connection-reuse on put/post (TRUE=enabled, default TRUE)
            'secu'      Sets the secure flags to init winhttp (for internal use only; if you change this setting you hit an XR violation)
            'spam'      Sets debug output verbosity (0...n)
            'time'      Sets ProtoHttp client timeout in milliseconds (default=WinHttp default)
            'xtok'      Sets XSTS token enable/disable (default=disabled; iValue=
                        enabled true/false, iValue2=user index, pValue=
                        token URN, only required if different from URL)
        \endverbatim

    \Version 10/15/2019 (eesponda)
*/
/*******************************************************************************F*/
int32_t ProtoHttpControl(ProtoHttpRefT *pHttpRef, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iSelect == 'apnd')
    {
        return(_ProtoHttpSetAppendHeader(pHttpRef, (const char *)pValue));
    }
    if (iSelect == 'auth')
    {
        NetPrintf(("protohttpgdk: [%p] %s authentication\n", pHttpRef, iValue ? "enabling" : "disabling"));
        pHttpRef->bAuthenticate = (uint8_t)iValue;
        return(0);
    }
    if (iSelect == 'disc')
    {
        if (pHttpRef->pHttpConnection != NULL)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: user request\n", pHttpRef));

            WinHttpCloseHandle(pHttpRef->pHttpConnection);
            pHttpRef->pHttpConnection = NULL;
        }
        return(0);
    }
    if (iSelect == 'hver')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] header type verification %s\n", pHttpRef, iValue ? "enabled" : "disabled"));
        pHttpRef->bVerifyHdr = iValue;
        return(0);
    }
    if (iSelect == 'ires')
    {
        return(_ProtoHttpResizeBuffer(pHttpRef, iValue));
    }
    if (iSelect == 'keep')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] setting keep-alive to %d\n", pHttpRef, iValue));
        pHttpRef->iKeepAlive = pHttpRef->iKeepAliveDflt = iValue;
        return(0);
    }
    if (iSelect == 'maxr')
    {
        NetPrintf(("protohttpgdk: [%p] setting max recv rate to %d bytes/sec\n", pHttpRef, iValue));
        pHttpRef->RecvRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'maxs')
    {
        NetPrintf(("protohttpgdk: [%p] setting max send rate to %d bytes/sec\n", pHttpRef, iValue));
        pHttpRef->SendRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'rmax')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] setting max redirect to %d\n", pHttpRef, iValue));
        pHttpRef->iMaxRedirect = iValue;

        // set the max redirects
        if (!WinHttpSetOption(pHttpRef->pHttpSession, WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS, &pHttpRef->iMaxRedirect, sizeof(pHttpRef->iMaxRedirect)))
        {
            NetPrintf(("protohttpgdk: [%p] WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS failed with %s\n", pHttpRef, DirtyErrGetName(GetLastError())));
            return(-1);
        }
        return(0);
    }
    if (iSelect == 'rput')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] connection reuse on put/post %s\n", pHttpRef, iValue ? "enabled" : "disabled"));
        pHttpRef->bReuseOnPost = iValue ? TRUE : FALSE;
        return(0);
    }
    #if DIRTYCODE_DEBUG
    if (iSelect == 'secu')
    {
        _ProtoHttp_uWinHttpFlags = iValue ? WINHTTP_FLAG_SECURE_DEFAULTS : WINHTTP_FLAG_ASYNC;
        return(0);
    }
    #endif
    if (iSelect == 'spam')
    {
        pHttpRef->iVerbose = iValue;
        return(0);
    }
    if (iSelect == 'time')
    {
        if (WinHttpSetTimeouts(pHttpRef->pHttpSession, iValue, iValue, iValue, iValue))
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] setting timeout to %d ms\n", pHttpRef, iValue));
            return(0);
        }
        else
        {
            NetPrintf(("protohttpgdk: [%p] setting the timeout on the failed (err=%s)\n", pHttpRef, DirtyErrGetName(GetLastError())));
            return(-1);
        }
    }
    if (iSelect == 'xtok')
    {
        pHttpRef->bGetAuthToken = (uint8_t)iValue;
        pHttpRef->iAuthTokenIdx = (int8_t)iValue2;
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] auth token acquisition %s (user index=%d)\n", pHttpRef, (pHttpRef->bGetAuthToken == TRUE) ? "enabled" : "disabled", iValue2));
        if (pValue != NULL)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpgdk: [%p] setting token urn=%s\n", pHttpRef, pValue));
            ds_strnzcpy(pHttpRef->strTokenUrl, (const char *)pValue, sizeof(pHttpRef->strTokenUrl));
        }
        return(0);
    }
    // unhandled
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpStatus

    \Description
        Return status of current HTTP transfer.  Status type depends on selector.

    \Input *pHttpRef    - module state
    \Input iSelect      - info selector (see Notes)
    \Input *pBuffer     - [out] storage for selector-specific output
    \Input iBufSize     - size of buffer

    \Output
        int32_t         - selector specific

    \Notes
        Selectors are:

    \verbatim
        SELECTOR    RETURN RESULT
        'body'      negative=failed or pending, else size of body (for 64bit size, get via pBuffer)
        'code'      negative=no response, else server response code (ProtoHttpResponseE)
        'data'      negative=failed, zero=pending, positive=amnt of data ready
        'date'      last-modified date, if available
        'done'      negative=failed, zero=pending, positive=done
        'head'      negative=failed or pending, else size of header
        'host'      current host copied to output buffer
        'hres'      return hResult containing either the socket error or ssl error or the http status code
        'htxt'      current received http header text copied to output buffer
        'info'      copies most recent info header received, if any, to output buffer (one time only)
        'imax'      size of input buffer
        'maxr'      return configured max recv rate (bytes/sec; zero=uncapped)
        'maxs'      return configured max send rate (bytes/sec; zero=uncapped)
        'port'      current port
        'ratr'      return current recv rate estimation (bytes/sec)
        'rats'      return current send rate estimation (bytes/sec)
        'rtxt'      most recent http request header text copied to output buffer
        'rmax'      returns currently configured max redirection count
        'time'      TRUE if the client timed out the connection, else FALSE
    \endverbatim

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpStatus(ProtoHttpRefT *pHttpRef, int32_t iSelect, void *pBuffer, int32_t iBufSize)
{
    // return response code
    if (iSelect == 'code')
    {
        return(pHttpRef->iHdrCode);
    }
    // data check: negative=failed, zero=pending, positive=data ready
    if (iSelect == 'data')
    {
        if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
        {
            return(-1);
        }
        if ((pHttpRef->eState == PROTOHTTP_ST_BODY) || (pHttpRef->eState == PROTOHTTP_ST_DONE))
        {
            return(pHttpRef->iInpLen);
        }
        return(0);
    }
    // done check: negative=failed, zero=pending, positive=done
    if (iSelect == 'done')
    {
        if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
        {
            return(-1);
        }
        if (pHttpRef->eState == PROTOHTTP_ST_DONE)
        {
            return(1);
        }
        return(0);
    }
    // return current host
    if (iSelect == 'host')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->strHost, iBufSize);
        return(0);
    }
    // return hResult containing either the windows API hresult or the http status code
    if (iSelect == 'hres')
    {
        if (pHttpRef->iHdrCode > 0)
        {
            return(DirtyErrGetHResult(DIRTYAPI_PROTO_HTTP, pHttpRef->iHdrCode, (pHttpRef->iHdrCode >= PROTOHTTP_RESPONSE_CLIENTERROR) ? TRUE : FALSE));
        }
        else if (pHttpRef->uProtocolError > 0)
        {
            return(HRESULT_FROM_WIN32(pHttpRef->uProtocolError));
        }
        else if (pHttpRef->uSecureError > 0)
        {
            return(HRESULT_FROM_WIN32(pHttpRef->uSecureError));
        }
        else
        {
            return(0);
        }
    }
    // copies info header (if available) to output buffer
    if (iSelect == 'info')
    {
        if (!pHttpRef->bInfoHdr)
        {
            return(0);
        }
        if (pBuffer != NULL)
        {
            ds_strnzcpy((char *)pBuffer, pHttpRef->strHdr, iBufSize);
        }
        pHttpRef->bInfoHdr = FALSE;
        return(pHttpRef->iHdrCode);
    }
    // return size of input buffer
    if (iSelect == 'imax')
    {
        return(pHttpRef->iInpMax);
    }
    // return configured max recv rate
    if (iSelect == 'maxr')
    {
        return(pHttpRef->RecvRate.uMaxRate);
    }
    // return configured max send rate
    if (iSelect == 'maxs')
    {
        return(pHttpRef->SendRate.uMaxRate);
    }
    // return current port
    if (iSelect == 'port')
    {
        return(pHttpRef->iPort);
    }
    // return current recv rate estimation
    if (iSelect == 'ratr')
    {
        return(pHttpRef->RecvRate.uCurRate);
    }
    // return current send rate estimation
    if (iSelect == 'rats')
    {
        return(pHttpRef->SendRate.uCurRate);
    }
    // return current redirection max
    if (iSelect == 'rmax')
    {
        return(pHttpRef->iMaxRedirect);
    }
    // return most recent http request header text
    if (iSelect == 'rtxt')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->strRequestHdr, iBufSize);
        return(0);
    }
    // return timeout indicator
    if (iSelect == 'time')
    {
        return(pHttpRef->uProtocolError == ERROR_WINHTTP_TIMEOUT);
    }

    // the following selectors early-out with errors in failure states
    if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
    {
        return(-1);
    }
    if ((pHttpRef->eState != PROTOHTTP_ST_BODY) && (pHttpRef->eState != PROTOHTTP_ST_DONE))
    {
        return(-2);
    }

    // get the size body
    if (iSelect == 'body')
    {
        if ((pBuffer != NULL) && (iBufSize == sizeof(pHttpRef->iBodySize)))
        {
            ds_memcpy(pBuffer, &pHttpRef->iBodySize, iBufSize);
        }
        return((int32_t)pHttpRef->iBodySize);
    }
    // get the date field out of the header
    if (iSelect == 'date')
    {
        return(pHttpRef->iHdrDate);
    }
    // get the size of the header
    if (iSelect == 'head')
    {
        return(pHttpRef->iHeadSize);
    }
    // copy the received headers
    if (iSelect == 'htxt')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->strHdr, iBufSize);
        return(0);
    }
    // unhandled
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCheckKeepAlive

    \Description
        Checks whether a request for the given Url would be a keep-alive transaction.
        Unlike the 'generic' version, this version is not aware of the current
        connection status, only whether the previous request was made to the same
        host/port/security setting.

    \Input *pHttpRef    - module state
    \Input *pUrl        - Url to check

    \Output
        int32_t         - TRUE if a request to this Url would use keep-alive, else FALSE

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpCheckKeepAlive(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    char strHost[sizeof(pHttpRef->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url
    pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // see if a request for this url would use keep-alive
    if ((pHttpRef->iKeepAlive > 0) && (pHttpRef->iPort == iPort) && (pHttpRef->bSecure == iSecure) && !ds_stricmp(pHttpRef->strHost, strHost))
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

    \Input *pHttpRef    - module state

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
void ProtoHttpUpdate(ProtoHttpRefT *pHttpRef)
{
    // get crit access
    if (!NetCritTry(&pHttpRef->HttpCrit))
    {
        return;
    }

    // handle the appropriate states
    if (pHttpRef->eState == PROTOHTTP_ST_INIT)
    {
        _ProtoHttpUpdateInit(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_ABRT)
    {
        _ProtoHttpUpdateAbrt(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_AUTH)
    {
        _ProtoHttpUpdateAuth(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_SEND)
    {
        _ProtoHttpUpdateSend(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_RESP)
    {
        _ProtoHttpUpdateResp(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_HEAD)
    {
        _ProtoHttpUpdateHead(pHttpRef);
    }
    if (pHttpRef->eState == PROTOHTTP_ST_BODY)
    {
        _ProtoHttpUpdateBody(pHttpRef);
    }

    // cleanup the request if we are in the done or failure state
    if ((pHttpRef->eState == PROTOHTTP_ST_DONE) || (pHttpRef->eState == PROTOHTTP_ST_FAIL))
    {
        if (pHttpRef->pHttpRequest != NULL)
        {
            WinHttpCloseHandle(pHttpRef->pHttpRequest);
            pHttpRef->pHttpRequest = NULL;
        }
    }

    // handle the done state
    if (pHttpRef->eState == PROTOHTTP_ST_DONE)
    {
        // check if we need to close the connection for any reason
        if (pHttpRef->pHttpConnection != NULL)
        {
            if (pHttpRef->bCloseHdr == TRUE)
            {
                NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: server request\n", pHttpRef));

                WinHttpCloseHandle(pHttpRef->pHttpConnection);
                pHttpRef->pHttpConnection = NULL;
            }
            else if (pHttpRef->bClosed == TRUE)
            {
                NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: server close\n", pHttpRef));

                WinHttpCloseHandle(pHttpRef->pHttpConnection);
                pHttpRef->pHttpConnection = NULL;
            }
        }
    }

    // handle the failure state
    if (pHttpRef->eState == PROTOHTTP_ST_FAIL)
    {
        // close the connection
        if (pHttpRef->pHttpConnection != NULL)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpgdk: [%p] closing connection: error\n", pHttpRef));

            WinHttpCloseHandle(pHttpRef->pHttpConnection);
            pHttpRef->pHttpConnection = NULL;
        }
    }

    // call the write callback if necessary
    if (pHttpRef->pWriteCb != NULL)
    {
        _ProtoHttpWriteCbProcess(pHttpRef);
    }

    // release crit access
    NetCritLeave(&pHttpRef->HttpCrit);
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

    \Version 10/15/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert(const uint8_t *pCACert, int32_t iCertSize)
{
    return(0);
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

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert2(const uint8_t *pCACert, int32_t iCertSize)
{
    return(0);
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

    \Version 10/16/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpValidateAllCA(void)
{
    return(0);
}

/*F*************************************************************************/
/*!
    \Function ProtoHttpClrCACerts

    \Description
        Clears all dynamic CA certs from the list.

    \Version 10/16/2019 (eesponda)
*/
/**************************************************************************F*/
void ProtoHttpClrCACerts(void)
{
}
