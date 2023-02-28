/*H********************************************************************************/
/*!
    \File dirtycontextmanagerps4.c

   \Description
        dirtycontextManagerPs4 handles thread synchornization for creating and freeing
        sceHttp, sceSsl and sceNetPool context creation and deletion. 
        This functionality is necessary since the Sony library does not offer a thread 
        safe implementation.

    \Copyright
        Copyright (c) Electronic Arts 2016

    \Version 1.0 07/22/2016 (amakoukji)  First Version
*/
/********************************************************************************H*/


/*** Include files *********************************************************************/
#include <net.h>
#include <libhttp.h>
#include <string.h>

#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/ps4/dirtycontextmanagerps4.h"

#ifdef DIRTYCODE_PS5
#include <libhttp2.h>
#endif

/*** Defines ***************************************************************************/

#define DIRTYCM_MAX_CONTEXTS           (200)
#define DIRTYCM_MAX_NET_CONTEXTS       (20)
#define DIRTYCM_EXTERNAL_NETPOOL       (-1)
#define DIRTYCM_MIN_NETHEAP_SIZE       (4096)
#define DIRTYCM_POOLSIZE_FACTOR        (16384)

#ifdef DIRTYCODE_PS5
#define DIRTYCM_MIN_HTTP2_POOLSIZE     (256 * 1024) // 256KiB is good for 3 simultaneous request
#endif

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct NetContextRefT
{
    int32_t iContextId;
    int32_t iContextSize;
    int32_t iRefCount;   
} NetContextRefT;

typedef struct DirtyContextManagerRefT
{
    int32_t iMemGroup;
    void *pMemGroupUserData;

    NetCritT crit;

    uint32_t uMaxNetContextShares;
    int32_t aHttpContextId[DIRTYCM_MAX_CONTEXTS];
    int32_t aSslContextId[DIRTYCM_MAX_CONTEXTS];
    NetContextRefT aNetContexts[DIRTYCM_MAX_NET_CONTEXTS];

    #ifdef DIRTYCODE_PS5
    int32_t aHttp2ContextId[DIRTYCM_MAX_CONTEXTS];
    #endif

} DirtyContextManagerRefT;

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/
static DirtyContextManagerRefT *_DirtyContextManagerRef = NULL;


/*** Functions *************************************************************************/

#ifdef DIRTYCODE_PS5
/*F********************************************************************************/
/*!
    \Function   _DirtyContextManagerHttp2PoolSize

    \Description
        Calculates the Http2 pool size needed given the number of concurrent request

    \Input iConncurrentRequest - number of concurrent request

    \Output
        uint32_t               - minimum http2 pool size needed 

    \Version 10/30/2019 (tcho)
*/
/********************************************************************************F*/
static uint32_t _DirtyContextManagerHttp2PoolSize(int32_t iConncurrentRequest)
{
    uint32_t iHttp2PoolSize = ((iConncurrentRequest - 1) / 3 + 1) * DIRTYCM_MIN_HTTP2_POOLSIZE;
    return(iHttp2PoolSize);
}
#endif

static int32_t _DirtyContextManagerShareNetPool(int32_t iNetPoolSize)
{
    int32_t iSlot = 0;
    int32_t iLowerRefCountSlot = 0;
    int32_t iLowerRefCount = _DirtyContextManagerRef->aNetContexts[iLowerRefCountSlot].iRefCount;
    
    for (iSlot = 0; iSlot < DIRTYCM_MAX_NET_CONTEXTS; ++iSlot)
    {
        // search for the lowest ref count which also has at least the same or larger netpool size
        if ((_DirtyContextManagerRef->aNetContexts[iSlot].iContextSize >= iNetPoolSize) 
            && (_DirtyContextManagerRef->aNetContexts[iSlot].iRefCount < iLowerRefCount))
        {
            iLowerRefCountSlot = iSlot;
            iLowerRefCount = _DirtyContextManagerRef->aNetContexts[iSlot].iRefCount;
        }
    }
        
    if (_DirtyContextManagerRef->aNetContexts[iLowerRefCountSlot].iRefCount >= (int32_t)_DirtyContextManagerRef->uMaxNetContextShares || iLowerRefCountSlot < 0)
    {
        // do not allow infinite sharing or http requests will start to fail in flight or in creating http context
        // also do not pick a netPool with a size which is too small.
        return(DIRTYCM_ERROR_MAX_CONTEXTS_REACHED);
    }
    else
    {
        // increment ref count for slot
        ++(_DirtyContextManagerRef->aNetContexts[iLowerRefCountSlot].iRefCount);
    }
    
    // return the context id
    return(_DirtyContextManagerRef->aNetContexts[iLowerRefCountSlot].iContextId);
}

// public functions
int32_t DirtyContextManagerInit()
{
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    if (_DirtyContextManagerRef != NULL)
    {
        // early out, already created
        return(DIRTYCM_ERROR_ALREADY_INITIALIZED);
    }

    _DirtyContextManagerRef = (DirtyContextManagerRefT*)DirtyMemAlloc((int32_t)sizeof(DirtyContextManagerRefT), DIRTYCM_MEMID, iMemGroup, pMemGroupUserData);
    if (_DirtyContextManagerRef == NULL)
    {
        // early out, no more memeory
        return(DIRTYCM_ERROR_OUT_OF_MEMORY);
    }

    // setup the context of the DirtyContextManagerRefT
    ds_memclr(_DirtyContextManagerRef, sizeof(DirtyContextManagerRefT));
    _DirtyContextManagerRef->iMemGroup = iMemGroup;
    _DirtyContextManagerRef->pMemGroupUserData = pMemGroupUserData;
    _DirtyContextManagerRef->uMaxNetContextShares = DIRTYCM_MAX_NET_CONTEXT_SHARES_DEFAULT;
    NetCritInit(&_DirtyContextManagerRef->crit, "DirtyHttpCM");

    return(0);
}

void DirtyContextManagerShutdown()
{
    int i = 0;

    if (_DirtyContextManagerRef == NULL)
    {
        return;
    }

    NetCritEnter(&_DirtyContextManagerRef->crit);

#ifdef DIRTYCODE_PS5

    // loop through http2 contexts and free any unfreed context groups
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        // free unfreed contexts and warn 
        if (_DirtyContextManagerRef->aHttp2ContextId[i] > 0)
        {
            NetPrintf(("dirtycontextmanagerps4: warning, http2 context %d was not freed\n", _DirtyContextManagerRef->aHttpContextId[i]));
            sceHttp2Term(_DirtyContextManagerRef->aHttp2ContextId[i]);
            _DirtyContextManagerRef->aHttp2ContextId[i] = 0;
        }
    }

#endif

    // loop through http contexts and free any unfreed context groups
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        // free unfreed contexts and warn 
        if (_DirtyContextManagerRef->aHttpContextId[i] > 0)
        {
            NetPrintf(("dirtycontextmanagerps4: warning, http context %d was not freed\n", _DirtyContextManagerRef->aHttpContextId[i]));
            sceHttpTerm(_DirtyContextManagerRef->aHttpContextId[i]);
            _DirtyContextManagerRef->aHttpContextId[i] = 0;
        }
    }

    // loop through ssl contexts and free any unfreed context groups
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        // free unfreed contexts and warn 
        if (_DirtyContextManagerRef->aSslContextId[i] > 0)
        {
            NetPrintf(("dirtycontextmanagerps4: warning, ssl context %d was not freed\n", _DirtyContextManagerRef->aSslContextId[i]));
            sceHttpTerm(_DirtyContextManagerRef->aSslContextId[i]);
            _DirtyContextManagerRef->aSslContextId[i] = 0;
        }
    }

    // loop through and free net context
    for (i = 0; i < DIRTYCM_MAX_NET_CONTEXTS; ++i)
    {
         // free unfreed contexts and warn 
        if (_DirtyContextManagerRef->aNetContexts[i].iContextId > 0)
        {
            NetPrintf(("dirtycontextmanagerps4: warning, net pool %d was not freed\n", _DirtyContextManagerRef->aNetContexts[i]));
            sceNetPoolDestroy(_DirtyContextManagerRef->aNetContexts[i].iContextId);
            ds_memclr(&_DirtyContextManagerRef->aNetContexts[i], sizeof(NetContextRefT));
        }
    }

    NetCritLeave(&_DirtyContextManagerRef->crit);

    // kill the critical section
    NetCritKill(&_DirtyContextManagerRef->crit);

    // free the module memory
    DirtyMemFree(_DirtyContextManagerRef, DIRTYCM_MEMID, _DirtyContextManagerRef->iMemGroup, _DirtyContextManagerRef->pMemGroupUserData);
    _DirtyContextManagerRef = NULL;
}

int32_t DirtyContextManagerCreateNetPoolContext(const char *strName, int32_t iNetPoolSize)
{
    int32_t iReturn = 0;
    int32_t i = 0;
    int32_t iSlotId = 0;
    int32_t iFinalSize;
    char netPoolName[64];

    NetCritEnter(&_DirtyContextManagerRef->crit);

    // the net heap size has a 4KiB minimum
    iFinalSize = iNetPoolSize;
    if (iFinalSize < DIRTYCM_MIN_NETHEAP_SIZE)
    {
        iFinalSize = DIRTYCM_MIN_NETHEAP_SIZE;
    }

    // find an empty slot
    for (i = 0; i < DIRTYCM_MAX_NET_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aNetContexts[i].iContextId == 0)
        {
            // found a slot
            iSlotId = i;
            break;
        }
    }

    // only allow ourselves to create a maximum of DIRTYCM_MAX_NET_CONTEXTS contexts
    // these are shared by the entire system, not just the application, and only 32 can exist
    if (i == DIRTYCM_MAX_NET_CONTEXTS)
    {
        // cannot create a new pool, reached self imposed limit, find one to share
        NetPrintf(("dirtycontextmanagerps4: finding a netPool to share.\n"));
        iReturn = _DirtyContextManagerShareNetPool(iNetPoolSize);
        NetCritLeave(&_DirtyContextManagerRef->crit);
        return((iReturn < 0) ? DIRTYCM_ERROR_MAX_CONTEXTS_REACHED : iReturn);
    }

    // auto-generate a pool name if necessary
    if (strName == NULL || strName[0] == '\0' || ds_strnlen(strName, (int32_t)sizeof(netPoolName)) >= (int32_t)sizeof(netPoolName))
    {
        ds_snzprintf(netPoolName, (int32_t)sizeof(netPoolName), "DirtyCM_netpool");
    }

    iReturn = sceNetPoolCreate(strName, iFinalSize, 0);
    if (iReturn < 0)
    {
        // cannot create a new pool, reached system imposed limit, find one to share
        NetPrintf(("dirtycontextmanagerps4: failed to create netPool, error 0x%08x, finding a netPool to share.\n", iReturn));
        iReturn = _DirtyContextManagerShareNetPool(iNetPoolSize);
        NetCritLeave(&_DirtyContextManagerRef->crit);
        return((iReturn < 0) ? DIRTYCM_ERROR_MAX_CONTEXTS_REACHED : iReturn);
    }
    else
    {
        _DirtyContextManagerRef->aNetContexts[iSlotId].iContextId = iReturn;
        _DirtyContextManagerRef->aNetContexts[iSlotId].iContextSize = iFinalSize;
        _DirtyContextManagerRef->aNetContexts[iSlotId].iRefCount = 1;
    }

    NetCritLeave(&_DirtyContextManagerRef->crit);
    return(iReturn);
}

int32_t DirtyContextManagerCreateSslContext(int32_t iSslBufferSize)
{
    int32_t iReturn = 0;
    int32_t i = 0;
    int32_t iSlotId = 0;

    NetCritEnter(&_DirtyContextManagerRef->crit);

    // there are no special size considerations for the ssl buffer

    // find an empty slot
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aSslContextId[i] == 0)
        {
            // found a slot
            iSlotId = i;
            break;
        }
    }

    if (i == DIRTYCM_MAX_CONTEXTS)
    {
        return(DIRTYCM_ERROR_MAX_CONTEXTS_REACHED);
    }

    iReturn = sceSslInit(iSslBufferSize);
    if (iReturn < 0)
    {
        NetPrintf(("dirtycontextmanagerps4: failed to create ssl context, error 0x%08x\n", iReturn));
    }
    else
    {
        _DirtyContextManagerRef->aSslContextId[iSlotId] = iReturn;
    }

    NetCritLeave(&_DirtyContextManagerRef->crit);
    return(iReturn);
}

int32_t DirtyContextManagerCreateHttpContext(int32_t iSceNetContextId, int32_t iSslContextId, int32_t iHttpBufferSize)
{
    int32_t iReturn = 0;
    int32_t i = 0;
    int32_t iSlotId = 0;
    int32_t iFinalSize;

    NetCritEnter(&_DirtyContextManagerRef->crit);

    // http buffer size needs to be a multiple of 16KiB, round up
    iFinalSize = iHttpBufferSize;
    if ((iFinalSize % DIRTYCM_POOLSIZE_FACTOR) != 0)
    {
        int32_t iNewBufSize = ((iFinalSize / DIRTYCM_POOLSIZE_FACTOR) + 1) * DIRTYCM_POOLSIZE_FACTOR;
        NetPrintf(("dirtycontextmanagerps4: the memory pool %d needs to be a multiple of 16k, using next largest multiple %d\n", iFinalSize, iNewBufSize));
        iFinalSize = iNewBufSize;
    }

    // find an empty slot
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aHttpContextId[i] == 0)
        {
            // found a slot
            iSlotId = i;
            break;
        }
    }

    if (i == DIRTYCM_MAX_CONTEXTS)
    {
        NetCritLeave(&_DirtyContextManagerRef->crit);
        return(DIRTYCM_ERROR_MAX_CONTEXTS_REACHED);
    }

    iReturn = sceHttpInit(iSceNetContextId, iSslContextId, iFinalSize);
    if (iReturn < 0)
    {
        NetPrintf(("dirtycontextmanagerps4: failed to create http context, error 0x%08x\n", iReturn));
    }
    else
    {
        _DirtyContextManagerRef->aHttpContextId[iSlotId] = iReturn;
    }

    NetCritLeave(&_DirtyContextManagerRef->crit);
    return(iReturn);
}

#ifdef DIRTYCODE_PS5
/*F********************************************************************************/
/*!
    \Function   DirtyContextManagerCreateHttp2Context

    \Description
        Create Http2 context

    \Input iSceNetContextId  - net pool context
    \Input iSslContextId     - ssl context
    \Input iConcurentRequest - max number of concurrent request

    \Output
        int32_t              - http2 context id

    \Version 10/30/2019 (tcho)
*/
/********************************************************************************F*/
int32_t DirtyContextManagerCreateHttp2Context(int32_t iSceNetContextId, int32_t iSslContextId, uint32_t uConcurrentRequest)
{
    int32_t i = 0;
    int32_t iSlotId = 0;
    int32_t iReturn = 0;

    NetCritEnter(&_DirtyContextManagerRef->crit);

    for (i = 0; i < DIRTYCM_MAX_NET_CONTEXTS; ++i) 
    {
        if (_DirtyContextManagerRef->aHttp2ContextId[i] == 0)
        {
            // found a slot
            iSlotId = i;
            break;
        }
    }

    if (i == DIRTYCM_MAX_CONTEXTS)
    {
        NetCritLeave(&_DirtyContextManagerRef->crit);
        return(DIRTYCM_ERROR_MAX_CONTEXTS_REACHED);
    }

    if ((iReturn = sceHttp2Init(iSceNetContextId, iSslContextId, _DirtyContextManagerHttp2PoolSize(uConcurrentRequest), uConcurrentRequest)) < 0)
    {
        NetPrintf(("dirtycontextmanagerps4: failed to create http2 context, error 0x%08x\n", iReturn));
    }
    else
    {
        _DirtyContextManagerRef->aHttp2ContextId[iSlotId] = iReturn;
    }
    
    NetCritLeave(&_DirtyContextManagerRef->crit);

    return(iReturn);
}

/*F********************************************************************************/
/*!
    \Function   DirtyContextManagerFreeHttp2Context

    \Description
        Free Http2 context

    \Input iHttp2ContextId  - iHttp2ContextId

    \Output
        int32_t             - negative=error, zero=success

    \Version 10/30/2019 (tcho)
*/
/********************************************************************************F*/
int32_t DirtyContextManagerFreeHttp2Context(int32_t iHttp2ContextId)
{
    int32_t i = 0;
    int32_t iReturn = DIRTYCM_ERROR_UNKNOWN_CONTEXT;

    // loop through and find the context
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aHttp2ContextId[i] == iHttp2ContextId)
        {
            NetCritEnter(&_DirtyContextManagerRef->crit);
            sceHttp2Term(_DirtyContextManagerRef->aHttp2ContextId[i]);
            _DirtyContextManagerRef->aHttp2ContextId[i] = 0;
            iReturn = 0;
            NetCritLeave(&_DirtyContextManagerRef->crit);
            break;
        }
    }

    return(iReturn);
}
#endif

int32_t DirtyContextManagerFreeNetPoolContext(int32_t iSceNetContextId)
{
    int32_t i = 0;
    int32_t iReturn = DIRTYCM_ERROR_UNKNOWN_CONTEXT;

    // loop through and find the context
    for (i = 0; i < DIRTYCM_MAX_NET_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aNetContexts[i].iContextId ==  iSceNetContextId)
        {
            NetCritEnter(&_DirtyContextManagerRef->crit);
            // decrement the ref count 
            --(_DirtyContextManagerRef->aNetContexts[i].iRefCount);

            // delete if necessary
            if (_DirtyContextManagerRef->aNetContexts[i].iRefCount <= 0)
            {
                sceNetPoolDestroy(_DirtyContextManagerRef->aNetContexts[i].iContextId);
                _DirtyContextManagerRef->aNetContexts[i].iContextId = 0;
                _DirtyContextManagerRef->aNetContexts[i].iContextSize = 0;
                _DirtyContextManagerRef->aNetContexts[i].iRefCount = 0;
            }
            iReturn = 0;
            NetCritLeave(&_DirtyContextManagerRef->crit);
            break;
        }
    }

    return(iReturn);
}

int32_t DirtyContextManagerFreeSslContext(int32_t iSslContextId)
{
    int32_t i = 0;
    int32_t iReturn = DIRTYCM_ERROR_UNKNOWN_CONTEXT;

    // loop through and find the context
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aSslContextId[i] ==  iSslContextId)
        {
            NetCritEnter(&_DirtyContextManagerRef->crit);
            sceSslTerm(_DirtyContextManagerRef->aSslContextId[i]);
            _DirtyContextManagerRef->aSslContextId[i] = 0;
            iReturn = 0;
            NetCritLeave(&_DirtyContextManagerRef->crit);
            break;
        }
    }

    return(iReturn);
}

int32_t DirtyContextManagerFreeHttpContext(int32_t iHttpContextId)
{
    int32_t i = 0;
    int32_t iReturn = DIRTYCM_ERROR_UNKNOWN_CONTEXT;

    // loop through and find the context
    for (i = 0; i < DIRTYCM_MAX_CONTEXTS; ++i)
    {
        if (_DirtyContextManagerRef->aHttpContextId[i] ==  iHttpContextId)
        {
            NetCritEnter(&_DirtyContextManagerRef->crit);
            sceHttpTerm(_DirtyContextManagerRef->aHttpContextId[i]);
            _DirtyContextManagerRef->aHttpContextId[i] = 0;
            iReturn = 0;
            NetCritLeave(&_DirtyContextManagerRef->crit);
            break;
        }
    }

    return(iReturn);
}

int32_t DirtyContextManagerControl(int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iSelect == 'mncs')
    {
        if (iValue > 0)
        {
            NetPrintf(("dirtycontextmanagerps4: setting max net context shares to %d from %u\n", iValue, _DirtyContextManagerRef->uMaxNetContextShares));
            _DirtyContextManagerRef->uMaxNetContextShares = (uint32_t)iValue;
            return(0);
        }
        else
        {
            NetPrintf(("dirtycontextmanagerps4: invalid value for max net context shares (%d)\n", iValue));
            return(-1);
        }
    }
    return(-1);
}

int32_t DirtyContextManagerStatus(int32_t iSelect, int32_t iValue, void *pBuffer, int32_t iBufSize)
{
    if (iSelect == 'mncs')
    {
        return((int32_t)_DirtyContextManagerRef->uMaxNetContextShares);
    }
    return(-1);
}





