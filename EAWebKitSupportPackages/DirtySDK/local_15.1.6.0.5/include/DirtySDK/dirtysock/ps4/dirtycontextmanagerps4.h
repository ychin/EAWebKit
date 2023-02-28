/*H*************************************************************************************/
/*!
    \File dirtycontextmanagerps4.h

    \Description
        DirtyContextManagerPs4 handles thread synchornization for creating and freeing
        sceHttp, sceSsl and sceNetPool context creation and deletion. 
        This functionality is necessary since the Sony library does not offer a thread 
        safe implementation.

    \Copyright
        Copyright (c) Electronic Arts 2016

    \Version 1.0 07/22/2016 (amakoukji)  First Version
*/
/*************************************************************************************H*/

#ifndef _dirtycontextmanagerps4_h
#define _dirtycontextmanagerps4_h

/*!
\Moduledef DirtyContextManagerPs4 DirtyContextManagerPs4
\Modulemember DirtySock
*/
//@{

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

/*** Defines ***************************************************************************/
#define DIRTYCM_POOLSIZE                            (16*1024)
#define DIRTYCM_SSL_POOLSIZE                        (304 * 1024)
#define DIRTYCM_MAX_NET_CONTEXT_SHARES_DEFAULT      (10)
#define DIRTYCM_ERROR_ALREADY_INITIALIZED           (-2)
#define DIRTYCM_ERROR_OUT_OF_MEMORY                 (-3)
#define DIRTYCM_ERROR_MAX_CONTEXTS_REACHED          (-4)
#define DIRTYCM_ERROR_UNKNOWN_CONTEXT               (-5)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Functions *************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int32_t DirtyContextManagerInit(void);
void DirtyContextManagerShutdown(void);

// create netpool context 
int32_t DirtyContextManagerCreateNetPoolContext(const char *strName, int32_t iNetPoolSize);

// create ssl context 
int32_t DirtyContextManagerCreateSslContext(int32_t iSslBufferSize);

// create http context 
int32_t DirtyContextManagerCreateHttpContext(int32_t iSceNetContextId, int32_t iSslContextId, int32_t iHttpBufferSize);

// release net context
int32_t DirtyContextManagerFreeNetPoolContext(int32_t iSceNetContextId);

// release http context
int32_t DirtyContextManagerFreeSslContext(int32_t iSslContextId);

// release http context 
int32_t DirtyContextManagerFreeHttpContext(int32_t iHttpContextId);

#ifdef DIRTYCODE_PS5
// create http2 context
int32_t DirtyContextManagerCreateHttp2Context(int32_t iSceNetContextId, int32_t iSslContextId, uint32_t iConcurrentRequest);

// release http2 context
int32_t DirtyContextManagerFreeHttp2Context(int32_t iHttp2ContextId);
#endif

// control selectors
int32_t DirtyContextManagerControl(int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue);

// status selectors
int32_t DirtyContextManagerStatus(int32_t iSelect, int32_t iValue, void *pBuffer, int32_t iBufSize);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtycontextmanagerps4_h

