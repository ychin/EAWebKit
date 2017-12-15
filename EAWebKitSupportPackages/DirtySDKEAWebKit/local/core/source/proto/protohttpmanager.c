/*
Copyright (C) 2009-2010, 2013 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*H********************************************************************************/
/*!
    \File protohttpmanager.c

*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirtysock.h"
#include "dirtymem.h"
#include "protohttpmanager.h"

/*** Defines **********************************************************************/

#define HTTPMANAGER_COPYURL     (FALSE)

#define HTTPMANAGER_MAXREFS     (64)

#define HTTPMANAGER_MAXCMDS     (256)

#define HTTPMANAGER_FINALDEBUG  ((!DIRTYCODE_DEBUG) && FALSE)

#if HTTPMANAGER_FINALDEBUG
#include <windows.h> 
#endif

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

typedef enum HttpManagerHttpCmdStateE
{
    HTTPMANAGER_CMDSTATE_IDLE = 0,      
    HTTPMANAGER_CMDSTATE_WAIT,          
    HTTPMANAGER_CMDSTATE_ACTV          
} HttpManagerHttpCmdStateE;

typedef enum HttpManagerHttpRefStateE
{
    HTTPMANAGER_REFSTATE_NONE = 0,     
    HTTPMANAGER_REFSTATE_IDLE,        
    HTTPMANAGER_REFSTATE_BUSY,         
} HttpManagerHttpRefStateE;

typedef struct HttpManagerHttpRefT
{
    ProtoHttpRefT *pProtoHttp;        
    uint8_t uHttpState;
    int8_t iTransactions;
    uint8_t _pad[2];
} HttpManagerHttpRefT;

typedef struct HttpManagerHttpCmdT
{
    int32_t iHttpHandle;                
    HttpManagerHttpRefT *pHttpRef;      
    void *pCallbackRef;                 
    const char *pUrl;                   
    int32_t iQueueTick;                 
    uint8_t uRequestType;              
    uint8_t uState;                    
    uint8_t _pad[2];
} HttpManagerHttpCmdT;

struct HttpManagerRefT
{
    int32_t iMemGroup;      
    void *pMemGroupUserData;
    
    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;  
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;    

    int32_t iHttpHandle;

    int32_t iVerbose;

    int32_t iNumActiveTransactions;        
    int32_t iMaxActiveTransactions;         
    int32_t iNumQueuedTransactions;         
    int32_t iMaxQueuedTransactions;         
    uint32_t uNumTransactions;              
    uint32_t uNumKeepAliveTransactions;     
    uint32_t uSumQueueLatency;              
    uint32_t uMaxQueueLatency;              

    int32_t iHttpNumRefs;

    HttpManagerHttpRefT HttpRefs[HTTPMANAGER_MAXREFS];
    
    HttpManagerHttpCmdT HttpCmds[HTTPMANAGER_MAXCMDS];
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

// Private variables

#if HTTPMANAGER_FINALDEBUG
int32_t _HttpManager_iStartTick = 0;
#endif

// Public variables


/*** Private Functions ************************************************************/


#if HTTPMANAGER_FINALDEBUG
#undef NetPrintf
#define NetPrintf(_x) _HttpManagerPrintfCode _x
static int32_t _HttpManagerPrintfCode(const char *pFormat, ...)
{
    va_list pFmtArgs;
    char strText[4096];
    char strTick[16];
    const char *pText = strText;

	va_start(pFmtArgs, pFormat);
    if ((pFormat[0] == '%') && (pFormat[1] == 's') && (pFormat[2] == 0))
    {
        pText = va_arg(pFmtArgs, const char *);
    }
    else
    {
        vsprintf(strText, pFormat, pFmtArgs);
    }
    va_end(pFmtArgs);

    snzprintf(strTick, sizeof(strTick), "[%d] ", NetTick()-_HttpManager_iStartTick);
    OutputDebugString(strTick);
    OutputDebugString(pText);
    return(0);
}
#endif

#if HTTPMANAGER_FINALDEBUG
#undef NetPrintfVerbose
#define NetPrintfVerbose(_x) _HttpManagerPrintfVerboseCode _x
static int32_t _HttpManagerPrintfVerboseCode(int32_t iVerbosityLevel, int32_t iCheckLevel, const char *pFormat, ...)
{
    va_list pFmtArgs;
    char strText[4096];
    char strTick[16];
    const char *pText = strText;

	va_start(pFmtArgs, pFormat);
    if ((pFormat[0] == '%') && (pFormat[1] == 's') && (pFormat[2] == 0))
    {
        pText = va_arg(pFmtArgs, const char *);
    }
    else
    {
        vsprintf(strText, pFormat, pFmtArgs);
    }
    va_end(pFmtArgs);

    if (iCheckLevel < iVerbosityLevel)
    {
        snzprintf(strTick, sizeof(strTick), "[%d] ", NetTick()-_HttpManager_iStartTick);
        OutputDebugString(strTick);
        OutputDebugString(pText);
    }
    return(0);
}
#endif

static int32_t _HttpManagerAllocHandle(HttpManagerRefT *pHttpManager)
{
    int32_t iHttpHandle = pHttpManager->iHttpHandle;
    pHttpManager->iHttpHandle = (pHttpManager->iHttpHandle + 1) & 0x7fffffff;
    return(iHttpHandle);
}

static HttpManagerHttpCmdT *_HttpManagerAllocCmd(HttpManagerRefT *pHttpManager)
{
    HttpManagerHttpCmdT *pHttpCmd;
    int32_t iHttpCmd;
    
    for (iHttpCmd = 0, pHttpCmd = NULL; iHttpCmd < HTTPMANAGER_MAXCMDS; iHttpCmd += 1)
    {
        if (pHttpManager->HttpCmds[iHttpCmd].iHttpHandle == 0)
        {
            pHttpCmd = &pHttpManager->HttpCmds[iHttpCmd];
            memset(pHttpCmd, 0, sizeof(*pHttpCmd));
            pHttpCmd->iHttpHandle = _HttpManagerAllocHandle(pHttpManager);
            break;
        }
    }
    return(pHttpCmd);
}

static HttpManagerHttpRefT *_HttpManagerAllocRef(HttpManagerRefT *pHttpManager, const char *pUrl)
{
    HttpManagerHttpRefT *pHttpRef, *pAllocRef;
    uint8_t aKeepAliveState[HTTPMANAGER_MAXREFS];
    int32_t iHttpRef, iQueueDepth;
    
    memset(aKeepAliveState, 0, sizeof(aKeepAliveState));
    for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
    {
        pHttpRef = &pHttpManager->HttpRefs[iHttpRef];
        if ((pHttpRef->uHttpState != HTTPMANAGER_REFSTATE_NONE) && ProtoHttpCheckKeepAlive(pHttpRef->pProtoHttp, pUrl))
        {
            aKeepAliveState[iHttpRef] = TRUE;
        }
    }        
    
    for (iQueueDepth = 0, pAllocRef = NULL; pAllocRef == NULL; iQueueDepth += 1)
    {
        for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
        {
            pHttpRef = &pHttpManager->HttpRefs[iHttpRef];
            if ((pHttpRef->iTransactions == iQueueDepth) && (aKeepAliveState[iHttpRef] == TRUE))
            {
                pAllocRef = pHttpRef;
                pAllocRef->iTransactions += 1;
                pHttpManager->uNumKeepAliveTransactions += 1;
                NetPrintfVerbose((pHttpManager->iVerbose, 0, "httpmanager: queued ref=%2d count=%d keep=TRUE  url=%s\n", iHttpRef, pAllocRef->iTransactions, pUrl));
                break;
            }
        }
        if (pAllocRef != NULL)
        {
            break;
        }

        for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
        {
            pHttpRef = &pHttpManager->HttpRefs[iHttpRef];
            if (pHttpRef->iTransactions == iQueueDepth)
            {
                pAllocRef = pHttpRef;
                pAllocRef->iTransactions += 1;
                NetPrintfVerbose((pHttpManager->iVerbose, 0, "httpmanager: queued ref=%2d count=%d keep=FALSE url=%s\n", iHttpRef, pAllocRef->iTransactions, pUrl));
                break;
            }
        }
    }

    if (pAllocRef != NULL)
    {
        pHttpManager->iNumQueuedTransactions += 1;
        if (pHttpManager->iMaxQueuedTransactions < pHttpManager->iNumQueuedTransactions)
        {
            pHttpManager->iMaxQueuedTransactions = pHttpManager->iNumQueuedTransactions;
        }
    }

    return(pAllocRef);
}

static HttpManagerHttpCmdT *_HttpManagerGetCmd(HttpManagerRefT *pHttpManager, int32_t iHandle)
{
    HttpManagerHttpCmdT *pHttpCmd;
    int32_t iHttpCmd;
    
    for (iHttpCmd = 0, pHttpCmd = NULL; iHttpCmd < HTTPMANAGER_MAXCMDS; iHttpCmd += 1)
    {
        if (pHttpManager->HttpCmds[iHttpCmd].iHttpHandle == iHandle)
        {
            pHttpCmd = &pHttpManager->HttpCmds[iHttpCmd];
            break;
        }
    }
    return(pHttpCmd);
}

static void _HttpManagerUpdateTransactionStats(HttpManagerRefT *pHttpManager, HttpManagerHttpCmdT *pHttpCmd)
{
    uint32_t uLatency;

    pHttpManager->iNumQueuedTransactions -= 1;
    pHttpManager->iNumActiveTransactions += 1;
    if (pHttpManager->iMaxActiveTransactions < pHttpManager->iNumActiveTransactions)
    {
        pHttpManager->iMaxActiveTransactions = pHttpManager->iNumActiveTransactions;
    }
    uLatency = NetTick() - pHttpCmd->iQueueTick;
    if (pHttpManager->uMaxQueueLatency < uLatency)
    {
        pHttpManager->uMaxQueueLatency = uLatency;
    }
    pHttpManager->uSumQueueLatency += uLatency;
    pHttpManager->uNumTransactions += 1;
}

static int32_t _HttpManagerRequestStart(HttpManagerRefT *pHttpManager, HttpManagerHttpCmdT *pHttpCmd, const char *pData, int32_t iDataSize)
{
    #if DIRTYCODE_DEBUG
    if (pHttpCmd->pHttpRef->uHttpState != HTTPMANAGER_REFSTATE_IDLE)
    {
        NetPrintf(("httpmanager: error; get request issued on non-idle ref\n"));
    }
    #endif

    _HttpManagerUpdateTransactionStats(pHttpManager, pHttpCmd);

    pHttpCmd->pHttpRef->uHttpState = HTTPMANAGER_REFSTATE_BUSY;
    pHttpCmd->uState = HTTPMANAGER_CMDSTATE_ACTV;

    ProtoHttpCallback2(pHttpCmd->pHttpRef->pProtoHttp, pHttpManager->pCustomHeaderCb, pHttpManager->pReceiveHeaderCb, pHttpCmd->pCallbackRef);

    return(ProtoHttpRequest(pHttpCmd->pHttpRef->pProtoHttp, pHttpCmd->pUrl, pData, iDataSize, pHttpCmd->uRequestType));
}

static int32_t _HttpManagerRequest(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, const char *pData, int32_t iDataSize, ProtoHttpRequestTypeE eRequestType)
{
    HttpManagerHttpCmdT *pHttpCmd;
    
    if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
    {
        NetPrintf(("httpmanager: unrecognized transaction %d in request\n", iHandle));
        return(-1);
    }
    pHttpCmd->iQueueTick = NetTick();
    pHttpCmd->pUrl = pUrl;
    pHttpCmd->uRequestType = (uint8_t)eRequestType;
    
    if ((pHttpCmd->pHttpRef = _HttpManagerAllocRef(pHttpManager, pUrl)) == NULL)
    {
        NetPrintf(("httpmanager: unable to allocate ref for transaction %d\n", iHandle));
        return(-2);
    }
    
    if (pHttpCmd->pHttpRef->iTransactions == 1)
    {
        return(_HttpManagerRequestStart(pHttpManager, pHttpCmd, pData, iDataSize));
    }
    else
    {
        #if HTTPMANAGER_COPYURL
        pHttpCmd->pUrl = DirtyMemAlloc(strlen(pUrl)+1, HTTPMGR_MEMID, pHttpManager->iMemGroup, pHttpManager->pMemGroupUserData);
        strnzcpy((char *)pHttpCmd->pUrl, pUrl, strlen(pUrl)+1);
        #else
        pHttpCmd->pUrl = pUrl;
        #endif
        pHttpCmd->uState = HTTPMANAGER_CMDSTATE_WAIT;
        return(0);
    }
}


/*** Public Functions *************************************************************/


HttpManagerRefT *HttpManagerCreate(int32_t iHttpBufSize, int32_t iHttpNumRefs)
{
    HttpManagerRefT *pHttpManager;
    void *pMemGroupUserData;
    int32_t iMemGroup;
    int32_t iHttpRef;
    
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
    
    if (iHttpNumRefs > HTTPMANAGER_MAXREFS)
    {
        NetPrintf(("httpmanager: %d requested refs exceeds max of %d; clamping\n", iHttpNumRefs, HTTPMANAGER_MAXREFS));
        iHttpNumRefs = HTTPMANAGER_MAXREFS;
    }
    
    if ((pHttpManager = DirtyMemAlloc(sizeof(*pHttpManager), HTTPMGR_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("httpmanager: unable to allocate module state\n"));
        return(NULL);
    }
    memset(pHttpManager, 0, sizeof(*pHttpManager));

    pHttpManager->iMemGroup = iMemGroup;
    pHttpManager->pMemGroupUserData = pMemGroupUserData;
    pHttpManager->iHttpNumRefs = iHttpNumRefs;
    pHttpManager->iHttpHandle = 1;  
    
    for (iHttpRef = 0; iHttpRef < iHttpNumRefs; iHttpRef += 1)
    {
        if ((pHttpManager->HttpRefs[iHttpRef].pProtoHttp = ProtoHttpCreate(iHttpBufSize)) == NULL)
        {
            NetPrintf(("httpmanager: could not allocate http ref %d\n", iHttpRef));
            HttpManagerDestroy(pHttpManager);
            return(NULL);
        }
        ProtoHttpControl(pHttpManager->HttpRefs[iHttpRef].pProtoHttp, 'keep', 1, 0, NULL);
        pHttpManager->HttpRefs[iHttpRef].uHttpState = HTTPMANAGER_REFSTATE_IDLE;
    }

    #if HTTPMANAGER_FINALDEBUG
    _HttpManager_iStartTick = NetTick();
    #endif

    return(pHttpManager);
}

void HttpManagerCallback(HttpManagerRefT *pHttpManager, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pCallbackRef)
{
    pHttpManager->pCustomHeaderCb = pCustomHeaderCb;
    pHttpManager->pReceiveHeaderCb = pReceiveHeaderCb;
}

void HttpManagerDestroy(HttpManagerRefT *pHttpManager)
{
    int32_t iHttpRef;

    NetPrintf(("httpmanager: transactions: %d\n", pHttpManager->uNumTransactions));
    if (pHttpManager->uNumTransactions > 0)
    {
        NetPrintf(("httpmanager: keep-alive transactions: %d\n", pHttpManager->uNumKeepAliveTransactions));
        NetPrintf(("httpmanager: max active transactions: %d\n", pHttpManager->iMaxActiveTransactions));
        NetPrintf(("httpmanager: max queued transactions: %d\n", pHttpManager->iMaxQueuedTransactions));
        NetPrintf(("httpmanager: avg queue wait time: %dms\n", pHttpManager->uSumQueueLatency/pHttpManager->uNumTransactions));
        NetPrintf(("httpmanager: max queue wait time: %dms\n", pHttpManager->uMaxQueueLatency));
    }
    
    for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
    {
        if (pHttpManager->HttpRefs[iHttpRef].pProtoHttp != NULL)
        {
            ProtoHttpDestroy(pHttpManager->HttpRefs[iHttpRef].pProtoHttp);
        }
    }
    DirtyMemFree(pHttpManager, HTTPMGR_MEMID, pHttpManager->iMemGroup, pHttpManager->pMemGroupUserData);
}

int32_t HttpManagerAlloc(HttpManagerRefT *pHttpManager)
{
    HttpManagerHttpCmdT *pHttpCmd;
    
    if ((pHttpCmd = _HttpManagerAllocCmd(pHttpManager)) == NULL)
    {
        NetPrintf(("httpmanager: could not allocate new http transaction for HttpManagerGet() request\n"));
        return(-1);
    }
    NetPrintfVerbose((pHttpManager->iVerbose, 1, "httpmanager: allocated handle %d\n", pHttpCmd->iHttpHandle));
    return(pHttpCmd->iHttpHandle);
}

void HttpManagerFree(HttpManagerRefT *pHttpManager, int32_t iHandle)
{
    HttpManagerHttpCmdT *pHttpCmd;
    
    if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
    {
        NetPrintf(("httpmanager: unrecognized transaction %d in HttpManagerFree()\n", iHandle));
        return;
    }
    NetPrintfVerbose((pHttpManager->iVerbose, 1, "httpmanager: releasing handle %d\n", iHandle));
    if (pHttpCmd->pHttpRef != NULL)
    {
        if (pHttpCmd->pHttpRef->uHttpState == HTTPMANAGER_REFSTATE_BUSY)
        {
            pHttpManager->iNumActiveTransactions -= 1;
            pHttpCmd->pHttpRef->uHttpState = HTTPMANAGER_REFSTATE_IDLE;
        }
        else
        {
            NetPrintf(("httpmanager: why are we releasing a non-busy handle?\n"));
        }
        pHttpCmd->pHttpRef->iTransactions -= 1;
    }
    #if HTTPMANAGER_COPYURL
    if (pHttpCmd->pUrl != NULL)
    {
        DirtyMemFree((void *)pHttpCmd->pUrl, HTTPMGR_MEMID, pHttpManager->iMemGroup, pHttpManager->pMemGroupUserData);
    }
    #endif
    memset(pHttpCmd, 0, sizeof(*pHttpCmd));
}

int32_t HttpManagerGet(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, uint32_t bHeadOnly)
{
    return(_HttpManagerRequest(pHttpManager, iHandle, pUrl, NULL, 0, bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET));
}

int32_t HttpManagerRecv(HttpManagerRefT *pHttpManager, int32_t iHandle, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    HttpManagerHttpCmdT *pHttpCmd;

    if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
    {
        NetPrintf(("httpmanager: unrecognized transaction %d in HttpManagerRecv()\n", iHandle));
        return(-1);
    }

    if (pHttpCmd->uState != HTTPMANAGER_CMDSTATE_ACTV)
    {
        return(0);
    }
    
    return(ProtoHttpRecv(pHttpCmd->pHttpRef->pProtoHttp, pBuffer, iBufMin, iBufMax));
}

int32_t HttpManagerRecvAll(HttpManagerRefT *pHttpManager, int32_t iHandle, char *pBuffer, int32_t iBufSize)
{
    HttpManagerHttpCmdT *pHttpCmd;

    if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
    {
        NetPrintf(("httpmanager: unrecognized transaction %d in HttpManagerRecv()\n", iHandle));
        return(-1);
    }

    if (pHttpCmd->uState != HTTPMANAGER_CMDSTATE_ACTV)
    {
        return(0);
    }
    
    return(ProtoHttpRecvAll(pHttpCmd->pHttpRef->pProtoHttp, pBuffer, iBufSize));
}

int32_t HttpManagerPost(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, const char *pData, int32_t iDataSize, uint32_t bDoPut)
{
    return(_HttpManagerRequest(pHttpManager, iHandle, pUrl, pData, iDataSize, bDoPut ? PROTOHTTP_REQUESTTYPE_PUT : PROTOHTTP_REQUESTTYPE_POST));
}

int32_t HttpManagerSend(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pData, int32_t iDataSize)
{
    HttpManagerHttpCmdT *pHttpCmd;

    if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
    {
        NetPrintf(("httpmanager: unrecognized transaction %d in HttpManagerRecv()\n", iHandle));
        return(-1);
    }

    if (pHttpCmd->uState != HTTPMANAGER_CMDSTATE_ACTV)
    {
        return(0);
    }
    
    return(ProtoHttpSend(pHttpCmd->pHttpRef->pProtoHttp, pData, iDataSize));
}

int32_t HttpManagerRequest(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, const char *pData, int32_t iDataSize, ProtoHttpRequestTypeE eRequestType)
{
    return(_HttpManagerRequest(pHttpManager, iHandle, pUrl, pData, iDataSize, eRequestType));
}

int32_t HttpManagerControl(HttpManagerRefT *pHttpManager, int32_t iHandle, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    int32_t iHttpRef;
    
    if (iHandle > 0)
    {
        HttpManagerHttpCmdT *pHttpCmd;
        if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
        {
            NetPrintf(("httpmanager: handle %d unrecognized in call to HttpManagerControl()\n", iHandle));
            return(-1);
        }
        if (iSelect == 'cbup')
        {
            NetPrintfVerbose((pHttpManager->iVerbose, 1, "httpmanager: setting callback for handle %d to 0x%08x\n", iHandle, pValue));
            pHttpCmd->pCallbackRef = pValue;
            return(0);
        }
    }
    else
    {
        if (iSelect == 'spam')
        {
            pHttpManager->iVerbose = iValue;
        }
        for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
        {
            if (pHttpManager->HttpRefs[iHttpRef].pProtoHttp != NULL)
            {
                ProtoHttpControl(pHttpManager->HttpRefs[iHttpRef].pProtoHttp, iSelect, iValue, iValue2, pValue);
            }
        }
        return(0);
    }
    return(-1);    
}

int32_t HttpManagerStatus(HttpManagerRefT *pHttpManager, int32_t iHandle, int32_t iSelect, void *pBuffer, int32_t iBufSize)
{
    if (iHandle > 0)
    {
        HttpManagerHttpCmdT *pHttpCmd;
        
        if ((pHttpCmd = _HttpManagerGetCmd(pHttpManager, iHandle)) == NULL)
        {
            NetPrintf(("httpmanager: unrecognized transaction %d in HttpManagerGet()\n", iHandle));
            return(-1);
        }

        if (pHttpCmd->uState != HTTPMANAGER_CMDSTATE_ACTV)
        {
            NetPrintf(("httpmanager: warning -- attempt to get status of handle %d when that transaction is not active\n"));
            return(-1);
        }

        return(ProtoHttpStatus(pHttpCmd->pHttpRef->pProtoHttp, iSelect, pBuffer, iBufSize));
    }
    return(-1);    
}

void HttpManagerUpdate(HttpManagerRefT *pHttpManager)
{
    HttpManagerHttpRefT *pHttpRef;
    int32_t iHttpRef;

    for (iHttpRef = 0; iHttpRef < pHttpManager->iHttpNumRefs; iHttpRef += 1)
    {
        pHttpRef = &pHttpManager->HttpRefs[iHttpRef];

        if ((pHttpRef->uHttpState == HTTPMANAGER_REFSTATE_IDLE) && (pHttpRef->iTransactions > 0))
        {
            HttpManagerHttpCmdT *pHttpCmd;
            int32_t iHttpCmd;

            for (iHttpCmd = 0; iHttpCmd < HTTPMANAGER_MAXCMDS; iHttpCmd += 1)
            {
                pHttpCmd = &pHttpManager->HttpCmds[iHttpCmd];
                if (pHttpCmd->pHttpRef == pHttpRef)
                {
                    NetPrintfVerbose((pHttpManager->iVerbose, 0, "httpmanager: starting queued url %s for ref %d\n", pHttpCmd->pUrl, pHttpRef-pHttpManager->HttpRefs));
                    _HttpManagerRequestStart(pHttpManager, pHttpCmd, NULL, 0);
                    break;
                }
            }
        }
        if (pHttpRef->uHttpState == HTTPMANAGER_REFSTATE_BUSY)
        {
            ProtoHttpUpdate(pHttpRef->pProtoHttp);
        }
    }
}
