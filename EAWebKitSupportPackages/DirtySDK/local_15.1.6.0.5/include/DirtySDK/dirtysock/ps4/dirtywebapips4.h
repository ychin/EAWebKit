/*H*************************************************************************************/
/*!
    \File dirtywebapips4.h

    \Description
        Provides a wrapper for the SceNpWebApi on ps4. Manages a queue of requests
        and a dedicated thread to handle a number of blocking SceNpWebApi functions.

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
typedef void (DirtyWebApiPushEventCallbackT)(int32_t userCtxId, int32_t callbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApiPushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg);

typedef struct DirtyWebApiCreateParamsT
{
    size_t netHeapSize;     //!< in bytes. If 0, DIRTY_WEBAPI_NET_HEAP_SIZE is used.
    size_t sslHeapSize;     //!< in bytes. If 0, DIRTY_WEBAPI_SSL_HEAP_SIZE is used.
    size_t httpHeapSize;    //!< in bytes. If 0, DIRTY_WEBAPI_HTTP_HEAP_SIZE is used.
    size_t webApiHeapSize;  //!< in bytes. If 0, DIRTY_WEBAPI_WEBAPI_HEAP_SIZE is used.
    size_t readBufferSize;  //!< in bytes. If 0, DIRTY_WEBAPI_READ_BUFFER_SIZE is used.
} DirtyWebApiCreateParamsT;

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
DirtyWebApiRefT *DirtyWebApiCreate(const DirtyWebApiCreateParamsT* pCreateParams);

void DirtyWebApiDestroy(DirtyWebApiRefT *pRef);

void DirtyWebApiUpdate(DirtyWebApiRefT *pRef);

int32_t DirtyWebApiRequest(DirtyWebApiRefT *pRef, int32_t iUserIndex, const char *pApiGroup, SceNpWebApiHttpMethod eHttpMethod, const char *pPath, const uint8_t *pContent, int32_t iContentLength, const char *pContentType, DirtyWebApiCallbackT *pCallback, void *pUserData);

int32_t DirtyWebApiRequestEx(DirtyWebApiRefT *pRef, int32_t iUserIndex, const char *pApiGroup, SceNpWebApiHttpMethod eHttpMethod, const char *pPath, const uint8_t *pContent, int32_t iContentLength, const char *pContentType, const char *pReqHeaders, DirtyWebApiCallbackT *pCallback, void *pUserData);

int32_t DirtyWebApiAbortRequests(DirtyWebApiRefT *pRef, int32_t iUserIndex);

int32_t DirtyWebApiAddPushEventListenerEx(DirtyWebApiRefT *pRef, const char * eventType, const char * pNpServiceName, SceNpServiceLabel npServiceLabel, DirtyWebApiPushEventCallbackT *callback, void *pUserData);

int32_t DirtyWebApiAddPushEventListener(DirtyWebApiRefT *pRef, const char * eventType, DirtyWebApiPushEventCallbackT *callback, void *pUserData);

int32_t DirtyWebApiRemovePushEventListener(DirtyWebApiRefT *pRef, int32_t iRequestId, DirtyWebApiPushEventCallbackT *callback);

int32_t DirtyWebApiControl(DirtyWebApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue);

int32_t DirtyWebApiStatus(DirtyWebApiRefT *pRef, int32_t iStatus, int32_t iValue, void* pBuf, int32_t iBufSize);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtywebapips4_h
