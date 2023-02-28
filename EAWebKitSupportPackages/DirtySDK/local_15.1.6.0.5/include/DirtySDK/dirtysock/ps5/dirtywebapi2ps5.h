/*H*************************************************************************************/
/*!
    \File dirtywebapi2ps5.h

    \Description
        Provides a wrapper for the SceNpWebApi2 on ps4 and ps5. Manages a queue of requests
        and a dedicated thread to handle a number of blocking SceNpWebApi2 functions.

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 2.3 04/26/2013 (mcorcoran) Initial implementation
*/
/*************************************************************************************H*/

#ifndef _dirtywebapips4_h
#define _dirtywebapips4_h

/*!
\Moduledef DirtyWebApiPS4 DirtyWebApiPS4
\Modulemember DirtySock
*/
//@{

/*** Include files *********************************************************************/

#include <np.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock/dirtyaddr.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct DirtyWebApiRefT DirtyWebApiRefT;
typedef void (DirtyWebApiCallbackT)(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData);
typedef void (DirtyWebApiPushEventCallbackT)(int32_t iUserCtxId, int32_t iCallbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApi2PushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg);
typedef void (DirtyWebApiPushEventConextCallbackT)(int32_t iUserCtxId, int32_t iCallbackId, const SceNpWebApi2PushEventPushContextId *pPushCtxId, SceNpWebApi2PushEventPushContextCallbackType cbType, const char *pNpServiceName, SceNpServiceLabel npServieLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApi2PushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg);

typedef struct DirtyWebApi2CreateParamsT
{
    size_t netHeapSize;             //!< in bytes. If 0, DIRTY_WEBAPI2_NET_HEAP_SIZE is used.
    size_t sslHeapSize;             //!< in bytes. If 0, DIRTY_WEBAPI2_SSL_HEAP_SIZE is used.
    size_t webApiHeapSize;          //!< in bytes. If 0, DIRTY_WEBAPI2_WEBAPI_HEAP_SIZE is used.
    size_t readBufferSize;          //!< in bytes. If 0, DIRTY_WEBAPI2_READ_BUFFER_SIZE is used.
    uint32_t uMaxConcurrentRequest; //!< max number of concurrent Http2 request
    uint32_t uThreadCpuAffinity;    //!< this is the default thread affinity setting if we cannot query NetConn for it
} DirtyWebApi2CreateParamsT;

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
DirtyWebApiRefT *DirtyWebApiCreate(const DirtyWebApi2CreateParamsT* pCreateParams);

void DirtyWebApiDestroy(DirtyWebApiRefT *pRef);

void DirtyWebApiUpdate(DirtyWebApiRefT *pRef);

int64_t DirtyWebApiRequest(DirtyWebApiRefT *pRef, int32_t iUserIndex, const char *pApiGroup, SceNpWebApi2HttpMethod eHttpMethod, const char *pPath, const uint8_t *pContent, int32_t iContentLength, const char *pContentType, DirtyWebApiCallbackT *pCallback, void *pUserData);

int64_t DirtyWebApiRequestEx(DirtyWebApiRefT *pRef, int32_t iUserIndex, const char *pApiGroup, SceNpWebApi2HttpMethod eHttpMethod, const char *pPath, const uint8_t *pContent, int32_t iContentLength, const char *pContentType, const char *pReqHeaders, DirtyWebApiCallbackT *pCallback, void *pUserData);

int32_t DirtyWebApiAbortRequestById(DirtyWebApiRefT *pRef, int32_t iUserIndex, int64_t iWebRequestId);

int32_t DirtyWebApiAbortRequests(DirtyWebApiRefT *pRef, int32_t iUserIndex);

int32_t DirtyWebApiAddPushEventListenerEx(DirtyWebApiRefT *pRef, const char * eventType, const char * pNpServiceName, SceNpServiceLabel npServiceLabel, DirtyWebApiPushEventCallbackT *callback, void *pUserData);

int32_t DirtyWebApiAddPushEventListener(DirtyWebApiRefT *pRef, const char * eventType, DirtyWebApiPushEventCallbackT *callback, void *pUserData);

int32_t DirtyWebApiCreatePushContext(DirtyWebApiRefT *pRef, int32_t iUserIndex, SceNpWebApi2PushEventPushContextId *pPushContextId);

int32_t DirtyWebApiDeletePushContext(DirtyWebApiRefT *pRef, int32_t iUserIndex, SceNpWebApi2PushEventPushContextId *pPushContextId);

int32_t DirtyWebApiRemovePushEventListener(DirtyWebApiRefT *pRef, int32_t iRequestId, DirtyWebApiPushEventCallbackT *callback);

int32_t DirtyWebApiAddPushEventContextListener(DirtyWebApiRefT *pRef, const char * eventType, DirtyWebApiPushEventConextCallbackT *callback, void *pUserData);

int32_t DirtyWebApiAddPushEventContextListenerEx(DirtyWebApiRefT *pRef, const char * eventType, const char * pNpServiceName, SceNpServiceLabel npServiceLabel, DirtyWebApiPushEventConextCallbackT *callback, void *pUserData);

int32_t DirtyWebApiRemovePushEventContextListener(DirtyWebApiRefT *pRef, int32_t iRequestId, DirtyWebApiPushEventConextCallbackT *callback);

int32_t DirtyWebApiControl(DirtyWebApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue);

int32_t DirtyWebApiStatus(DirtyWebApiRefT *pRef, int32_t iStatus, int32_t iValue, void* pBuf, int32_t iBufSize);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtywebapi2ps4_h
