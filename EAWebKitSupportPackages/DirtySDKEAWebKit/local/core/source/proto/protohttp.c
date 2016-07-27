/*
Copyright (C) 2000-2004,2009-2010 Electronic Arts, Inc.  All rights reserved.

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
    \File protohttp.c
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirtysock.h"
#include "dirtymem.h"
#include "dirtyvers.h"
#include "protossl.h"
#include "protohttp.h"

/*** Defines **********************************************************************/

#define PROTOHTTP_TIMEOUT       (30*1000)

#define PROTOHTTP_MAXREDIRECT   (3)

#define PROTOHTTP_HDRCACHESIZE  (1024)

#define PROTOHTTP_HDRAPPENDSIZE (1024)

#define PROTOHTTP_VERSION (0x0103)          

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

struct ProtoHttpRefT
{
    ProtoSSLRefT *pSsl;     

    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;  
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;    
    void *pCallbackRef;    

   
    int32_t iMemGroup;      
    void *pMemGroupUserData;

    int32_t iPort;          
    int32_t iSecure;       
    
    enum
    {
        ST_IDLE,            
        ST_CONN,           
        ST_SEND,            
        ST_RESP,           
        ST_HEAD,            
        ST_BODY,            
        ST_DONE,            
        ST_FAIL            
    } eState;              

    int32_t iHdrCode;       
    int32_t iHdrDate;       

    int32_t iPostSize;      
    int32_t iHeadSize;      
    int32_t iBodySize;      
    int32_t iRecvSize;      

    char *pInpBuf;          
    int32_t iInpMax;        
    int32_t iInpOff;        
    int32_t iInpLen;        
    int32_t iInpCnt;        
    int32_t iChkLen;        

    int32_t iNumRedirect;   
    int32_t iMaxRedirect;  

    uint32_t uTimeout;      
    uint32_t uTimer;        
    int32_t iKeepAlive;     
    int32_t iKeepAliveDflt; 

    char strHdr[PROTOHTTP_HDRCACHESIZE]; 
    char strRequestHdr[PROTOHTTP_HDRCACHESIZE]; 
    char strAppendHdr[PROTOHTTP_HDRAPPENDSIZE]; 
    char strHost[256];      
    
    uint8_t bTimeout;       
    uint8_t bChunked;       
    uint8_t bHeadOnly;      
    uint8_t bCloseHdr;      
    uint8_t iVerbose;       
    uint8_t bVerifyHdr;     
    uint8_t bHttp1_0;       
    uint8_t bCompactRecv;   
    uint8_t bInfoHdr;       
    uint8_t bNewConnection; 
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

// Private variables

static const char _ProtoHttp_strRequestNames[PROTOHTTP_NUMREQUESTTYPES][16] =
{
    "HEAD", "GET", "POST", "PUT", "DELETE"
};


// Public variables


/*** Private Functions ************************************************************/


static const char *_ProtoHttpParseUrl(const char *pUrl, char *pKind, int32_t iKindSize, char *pHost, int32_t iHostSize, int32_t *pPort, int32_t *pSecure)
{
    const char *s;
    int32_t i, iPort;
    
    while ((*pUrl != 0) && (*pUrl <= ' '))
    {
        pUrl++;
    }

    *pKind = '\0';
    for (s = pUrl; (*s >= 'a') && (*s <= 'z'); s++)
    {
    }
    if ((*s == ':') && ((s-pUrl) < iKindSize))
    {
        for (i = 0; pUrl[i] != ':'; i++)
        {
            pKind[i] = pUrl[i];
        }
        pUrl += i+1;
        pKind[i] = '\0';
    }
    
    *pSecure = (stricmp(pKind, "https") == 0);

    while ((*pUrl != 0) && (*pUrl <= ' '))
    {
        pUrl++;
    }

    if ((pUrl[0] == '/') && (pUrl[1] == '/'))
    {
        pUrl += 2;
    }

    for (i = 0; i < (iHostSize-1); i++)
    {
        if ((*pUrl <= ' ') || (*pUrl == '/') || (*pUrl == '?') || (*pUrl == ':'))
        {
            break;
        }
        pHost[i] = *pUrl++;
    }
    pHost[i] = '\0';

    iPort = 0;
    if (*pUrl == ':')
    {
        for (pUrl++, iPort = 0; (*pUrl >= '0') && (*pUrl <= '9'); pUrl++)
        {
            iPort = (iPort * 10) + (*pUrl & 15);
        }
    }

    if (iPort == 0)
    {
        iPort = *pSecure ? 443 : 80;
    }
    *pPort = iPort;
    
    while ((*pUrl != 0) && (*pUrl <= ' '))
    {
        pUrl++;
    }
    
    return(pUrl);
}

static void _ProtoHttpClose(ProtoHttpRefT *pState, const char *pReason)
{
    if (ProtoSSLStat(pState->pSsl, 'stat', NULL, 0) < 0)
    {
        return;
    }
    
    NetPrintf(("protohttp: closing connection: %s\n", pReason));
    ProtoSSLDisconnect(pState->pSsl);
    pState->bCloseHdr = FALSE;
}

static void _ProtoHttpReset(ProtoHttpRefT *pState)
{
    memset(pState->strHdr, 0, sizeof(pState->strHdr));
    memset(pState->strRequestHdr, 0, sizeof(pState->strRequestHdr));
    pState->eState = ST_IDLE;
    pState->iHdrCode = -1;
    pState->iHdrDate = 0;
    pState->iHeadSize = pState->iBodySize = 0;
    pState->iRecvSize = 0;
    pState->iInpOff = 0;
    pState->iChkLen = 0;
    pState->bTimeout = FALSE;
    pState->bChunked = FALSE;
    pState->bHeadOnly = FALSE;
}

static void _ProtoHttpFormatRequestHeader(ProtoHttpRefT *pState, const char *pUrl, const char *pHost, int32_t iPort, const char *pRequest, const char *pData, int32_t iDataLen) 
{
    int32_t iQueryPos, iOffset = 0;
    
    if (*pUrl == '\0')
    {
        pUrl = "/";
    }

    iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "%s %s HTTP/1.1\r\n", pRequest, pUrl);
    iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "Host: %s:%d\r\n", pHost, iPort);
    if (iDataLen != -1)
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "Content-Length: %d\r\n", iDataLen);
    }
    else
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "Transfer-Encoding: chunked\r\n");
    }
    if (pState->iKeepAlive == 0)
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "Connection: Close\r\n");
    }
    if (!stristr(pState->strAppendHdr, "User-Agent:"))
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "User-Agent: ProtoHttp %d.%d/DS %d.%d.%d.%d\r\n",
            (PROTOHTTP_VERSION>>8)&0xff, PROTOHTTP_VERSION&0xff,
            (DIRTYVERS>>24)&0xff, (DIRTYVERS>>16)&0xff,
            (DIRTYVERS>>8)&0xff, DIRTYVERS&0xff);
    }
    if (pState->strAppendHdr[0] == '\0')
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "Accept: */*\r\n");
    }
    else
    {
        iOffset += snzprintf(pState->pInpBuf+iOffset, pState->iInpMax-iOffset, "%s", pState->strAppendHdr);
    }

    if (pState->pCustomHeaderCb != NULL)
    {
        pState->pCustomHeaderCb(pState, pState->pInpBuf, pState->iInpMax, pData, iDataLen, pState->pCallbackRef);
    }
    
    iQueryPos = strnzcat(pState->pInpBuf, "\r\n", pState->iInpMax);

    strnzcpy(pState->strRequestHdr, pState->pInpBuf, sizeof(pState->strRequestHdr));

    pState->iInpLen = iQueryPos;
}

static int32_t _ProtoHttpFormatRequest(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int32_t iDataLen, ProtoHttpRequestTypeE eRequestType)
{
    char strHost[sizeof(pState->strHost)], strKind[8];
    int32_t iPort = 0, iSecure;
    int32_t eState = pState->eState;

    NetPrintf(("protohttp: %s %s\n", _ProtoHttp_strRequestNames[eRequestType], pUrl));

    pState->bNewConnection = FALSE;

    _ProtoHttpReset(pState);

    pUrl = _ProtoHttpParseUrl(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure);

    if ((iSecure != pState->iSecure) || (stricmp(strHost, pState->strHost) != 0) || (iPort != pState->iPort))
    {
        NetPrintf(("protohttp: requesting new connection -- url change to %s\n", strHost));
        
        pState->iKeepAlive = pState->iKeepAliveDflt;
        
        strnzcpy(pState->strHost, strHost, sizeof(pState->strHost));
        pState->iPort = iPort;
        pState->iSecure = iSecure;
    
        pState->bNewConnection = TRUE;
    }

    if ((pState->bNewConnection == FALSE) && (ProtoSSLStat(pState->pSsl, 'stat', NULL, 0) < 0))
    {
        NetPrintf(("protohttp: requesting new connection -- previous connection was closed\n"));
        pState->bNewConnection = TRUE;
    }

	if ((pState->bNewConnection == FALSE) && (eState != ST_IDLE) && (eState != ST_DONE))
	{
        NetPrintf(("protohttp: requesting new connection -- current state of %d does not allow connection reuse\n", eState));
        pState->bNewConnection = TRUE;
    }

    _ProtoHttpFormatRequestHeader(pState, pUrl, strHost, iPort, _ProtoHttp_strRequestNames[eRequestType], pData, iDataLen);   

    if ((pData != NULL) && (iDataLen != 0))
    {
        if (iDataLen > (pState->iInpMax - pState->iInpLen))
        {
            iDataLen = (pState->iInpMax - pState->iInpLen);
        }

        memcpy(pState->pInpBuf + pState->iInpLen, pData, iDataLen);
        pState->iInpLen += iDataLen;
    }

    pState->bHeadOnly = (eRequestType == PROTOHTTP_REQUESTTYPE_HEAD) ? TRUE : FALSE;
    return(iDataLen);
}

static void _ProtoHttpSendRequest(ProtoHttpRefT *pState)
{
    int32_t iResult;
    uint8_t uTest;

    if (pState->bNewConnection == FALSE)
    {
        if ((iResult = ProtoSSLRecv(pState->pSsl, &uTest, sizeof(uTest))) > 0)
        {
            NetPrintf(("protohttp: requesting new connection -- receive on previous connection returned data (0x%02x)\n", uTest));
            pState->bNewConnection = TRUE;
        }
        else if (iResult < 0)
        {
            NetPrintf(("protohttp: requesting new connection -- received %d error response from previous connection\n", iResult));
            pState->bNewConnection = TRUE;
        }
    }

    pState->uTimer = NetTick() + pState->uTimeout;

    if (pState->bNewConnection == TRUE)
    {
        _ProtoHttpClose(pState, "new connection");
    
        ProtoSSLConnect(pState->pSsl, pState->iSecure, pState->strHost, 0, pState->iPort);
        pState->eState = ST_CONN;
    }
    else
    {
        NetPrintf(("protohttp: reusing previous connection (keep-alive)\n"));
        pState->eState = ST_SEND;
    }

    if (pState->iKeepAlive == 0)
    {
        pState->bCloseHdr = TRUE;
    }   

    pState->iKeepAlive += 1;

    ProtoHttpUpdate(pState);
}

static const char *_ProtoHttpFindHeaderFieldValue(const char *pInpBuf, const char *pHeaderText)
{
    char strSearchText[64];
    const char *pFoundText;
    
    snzprintf(strSearchText, sizeof(strSearchText), "\n%s:", pHeaderText);
    if ((pFoundText = stristr(pInpBuf, strSearchText)) != NULL)
    {
        pFoundText += strlen(strSearchText);
        
        while ((*pFoundText != '\0') && (*pFoundText <= ' '))
        {
            pFoundText += 1;
        }
    }
    
    return(pFoundText);
}

static int32_t _ProtoHttpGetHeaderFieldName(const char *pInpBuf, char *pOutBuf, int32_t iOutLen)
{
    int32_t iOutSize;

    for (iOutSize = 0; (iOutSize < iOutLen) && (pInpBuf[iOutSize] != ':') && (pInpBuf[iOutSize] != '\0'); iOutSize += 1)
    {
        pOutBuf[iOutSize] = pInpBuf[iOutSize];
    }
    if (iOutSize == iOutLen)
    {
        return(-1);
    }
    pOutBuf[iOutSize] = '\0';
    return(iOutSize);
}

static int32_t _ProtoHttpGetHeaderFieldValue(const char *pInpBuf, char *pOutBuf, int32_t iOutLen, const char **ppHdrEnd)
{
    int32_t iValLen;

    if (pInpBuf == NULL)
    {
        return(-1);
    }
    
    for (iValLen = 0; *pInpBuf != '\0' ; pInpBuf += 1, iValLen += 1)
    {
        if ((pInpBuf[0] == '\r') && (pInpBuf[1] == '\n'))
        {
            if ((pInpBuf[2] == ' ') || (pInpBuf[2] == '\t'))
            {
                for (pInpBuf += 3; (*pInpBuf == ' ') || (*pInpBuf == '\t'); pInpBuf += 1)
                    ;
            }
            else 
            {
                break;
            }
        }

        if (pOutBuf != NULL)
        {
            pOutBuf[iValLen] = *pInpBuf;
            if ((iValLen+1) >= iOutLen)
            {
                *pOutBuf = '\0';
                return(-1);
            }
        }
    }
    if (ppHdrEnd != NULL)
    {
        *ppHdrEnd = pInpBuf;
    }
    if (pOutBuf != NULL)
    {
        pOutBuf[iValLen] = '\0';
        return(0);
    }
    return(iValLen+1);
}

static int32_t _ProtoHttpGetLocationHeader(ProtoHttpRefT *pState, const char *pInpBuf, char *pBuffer, int32_t iBufSize, const char **pHdrEnd)
{
    const char *pLocHdr;
    int32_t iLocLen, iLocPreLen=0;

    if ((pLocHdr = _ProtoHttpFindHeaderFieldValue(pState->pInpBuf, "location")) == NULL)
    {
        return(-1);
    }
    
    if (*pLocHdr == '/')
    {
        char strTemp[128];

        snzprintf(strTemp, sizeof(strTemp), "%s://%s:%d", pState->iSecure ? "https" : "http", pState->strHost, pState->iPort); 
        iLocPreLen = (int32_t)strlen(strTemp);

        if (pBuffer != NULL)
        {
            strnzcpy(pBuffer, strTemp, iBufSize);
            pBuffer = (void *)((uint8_t *)pBuffer + iLocPreLen);
            iBufSize -= iLocPreLen;
        }
    }

    iLocLen = _ProtoHttpGetHeaderFieldValue(pLocHdr, pBuffer, iBufSize, pHdrEnd);
    if ((pBuffer == NULL) && (iBufSize == 0))
    {
        iLocLen += iLocPreLen;
    }

    return(iLocLen);
}

static int32_t _ProtoHttpParseHeader(ProtoHttpRefT *pState)
{
    char *s = pState->pInpBuf;
    char *t = pState->pInpBuf+pState->iInpLen-3;
    char strTemp[128];
    
    while ((s != t) && ((s[0] != '\r') || (s[1] != '\n') || (s[2] != '\r') || (s[3] != '\n')))
    {
        s++;
    }
    if (s == t)
    {
        return(-1);
    }

    pState->iHeadSize = s+4-pState->pInpBuf;
    s[2] = s[3] = 0;

    if (pState->bVerifyHdr)
    {
        if (strncmp(pState->pInpBuf, "HTTP", 4))
        {
            NetPrintf(("protohttp: invalid result type\n"));
            pState->eState = ST_FAIL;
            return(-2);
        }
    }

    pState->bHttp1_0 = !strncmp(pState->pInpBuf, "HTTP/1.0", 8);

    #if DIRTYCODE_DEBUG
    if (pState->iVerbose > 0)
    {
        NetPrintf(("protohttp: received response header:\n"));
        NetPrintWrap(pState->pInpBuf, 80);
    }
    #endif

    s = pState->pInpBuf;
    while ((*s != 0) && (*s > ' '))
    {
        s++;
    }
    while ((*s != 0) && (*s <= ' '))
    {
        s++;
    }
    for (pState->iHdrCode = 0; (*s >= '0') && (*s <= '9'); ++s)
    {
        pState->iHdrCode = (pState->iHdrCode * 10) + (*s & 15);
    }

    if ((_ProtoHttpGetHeaderFieldValue(_ProtoHttpFindHeaderFieldValue(pState->pInpBuf, "content-length"), strTemp, sizeof(strTemp), NULL)) != -1)
    {
        pState->iBodySize = strtol(strTemp, NULL, 10);
    }
    else
    {
        pState->iBodySize = -1;
    }

    if ((_ProtoHttpGetHeaderFieldValue(_ProtoHttpFindHeaderFieldValue(pState->pInpBuf, "last-modified"), strTemp, sizeof(strTemp), NULL)) != -1)
    {
        pState->iHdrDate = strtotime(strTemp);
    }
    else
    {
        pState->iHdrDate = 0;
    }

    if ((_ProtoHttpGetHeaderFieldValue(_ProtoHttpFindHeaderFieldValue(pState->pInpBuf, "transfer-encoding"), strTemp, sizeof(strTemp), NULL)) != -1)
    {
        pState->bChunked = !stricmp(strTemp, "chunked");
    }

    if (pState->bCloseHdr == FALSE)
    {
        _ProtoHttpGetHeaderFieldValue(_ProtoHttpFindHeaderFieldValue(pState->pInpBuf, "connection"), strTemp, sizeof(strTemp), NULL);
        pState->bCloseHdr = !stricmp(strTemp, "close");
    }

    pState->bInfoHdr = PROTOHTTP_GetResponseClass(pState->iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL;

    strnzcpy(pState->strHdr, pState->pInpBuf, sizeof(pState->strHdr));

    if (pState->pReceiveHeaderCb != NULL)
    {
        pState->pReceiveHeaderCb(pState, pState->pInpBuf, (uint32_t)strlen(pState->pInpBuf), pState->pCallbackRef);
    }

    pState->iInpOff = pState->iHeadSize;
    pState->iInpCnt = pState->iInpLen - pState->iHeadSize;

    return(0);
}

static void _ProtoHttpProcessInfoHeader(ProtoHttpRefT *pState)
{
    NetPrintf(("protohttp: ignoring %d response from server\n", pState->iHdrCode));

    pState->iInpLen = 0;
    pState->iInpOff = 0;
    pState->eState = ST_HEAD;
}

static void _ProtoHttpRedirect(ProtoHttpRefT *pState)
{
    char *pUrlBuf;
    int32_t iUrlLen;
    
    if (++pState->iNumRedirect > pState->iMaxRedirect)
    {
        NetPrintf(("protohttp: maximum number of redirections (%d) exceeded\n", pState->iMaxRedirect));
        pState->eState = ST_FAIL;
        return;
    }

    if ((iUrlLen = _ProtoHttpGetLocationHeader(pState, pState->pInpBuf, NULL, 0, NULL)) <= 0)
    {
        NetPrintf(("protohttp: no location included in redirect header\n"));
        pState->eState = ST_FAIL;
        return;
    }

    pUrlBuf = pState->pInpBuf + pState->iInpMax - iUrlLen;
    if (_ProtoHttpGetLocationHeader(pState, pState->pInpBuf, pUrlBuf, iUrlLen, NULL) != 0)
    {
        NetPrintf(("protohttp: failed to get location header url"));
        pState->eState = ST_FAIL;
        return;
    }
    
    if (pState->bCloseHdr)
    {
        _ProtoHttpClose(pState, "server request");
    }

    _ProtoHttpFormatRequest(pState, pUrlBuf, NULL, 0, pState->bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET);
    _ProtoHttpSendRequest(pState);
}

static int32_t _ProtoHttpChunkProcess(ProtoHttpRefT *pState, int32_t iBufMax)
{
    int32_t iChkSize, iLen;

    if (pState->iInpLen == pState->iInpOff)
    {
        return(0);
    }
    
    if (pState->iChkLen == 0)
    {
        char *s = pState->pInpBuf+pState->iInpOff, *s2;
        char *t = pState->pInpBuf+pState->iInpLen-1;

        for (s2=s; (s2 < t) && ((s2[0] != '\r') || (s2[1] != '\n')); s2++)
            ;
        if (s2 == t)
        {
            if (pState->iInpLen == pState->iInpMax)
            {
                pState->bCompactRecv = TRUE;
            }
            return(0);
        }

        if ((pState->iChkLen = strtol(s, NULL, 16)) == 0)
        {
            NetPrintf(("protohttp: parsed end chunk\n"));
            pState->iInpOff = pState->iInpLen;
            pState->eState = ST_DONE;
            
            return(0);
        }
        else
        {
            NetPrintfVerbose((pState->iVerbose, 1, "protohttp: parsed chunk size=%d\n", pState->iChkLen));
        }

        pState->iInpOff += s2-s+2;
    }

    if ((iLen = pState->iInpLen - pState->iInpOff) > iBufMax)
    {
        iLen = iBufMax;
    }

    if (iLen >= pState->iChkLen)
    {
        if (iLen >= (pState->iChkLen+2))
        {
            iChkSize = pState->iChkLen;
            pState->iChkLen = 0;
        }
        else if (pState->iInpLen == pState->iInpMax)
        {
            iChkSize = iLen/2;
            pState->iChkLen -= iChkSize;
            pState->bCompactRecv = TRUE;
        }
        else 
        {
            iChkSize = 0;
        }
    }
    else
    {
        iChkSize = iLen;
        pState->iChkLen -= iChkSize;
    }

    return(iChkSize);
}

static int32_t _ProtoHttpSend(ProtoHttpRefT *pState, const char *pStrBuf, int32_t iSize)
{
    int32_t iResult;

    if ((iResult = ProtoSSLSend(pState->pSsl, pStrBuf, iSize)) > 0)
    {
        #if DIRTYCODE_DEBUG
        if (pState->iVerbose > 2)
        {
            NetPrintMem(pStrBuf, iResult, "http-send");
        }
        #endif
    
        pState->uTimer = NetTick() + pState->uTimeout;
    }
    else if (iResult < 0)
    {
        NetPrintf(("protohttp: error %d sending %d bytes\n", iResult, iSize));
        pState->eState = ST_FAIL;
    }
    return(iResult);
}

static int32_t _ProtoHttpSendChunk(ProtoHttpRefT *pState, const char *pStrBuf, int32_t iSize)
{
    char *pInpBuf = pState->pInpBuf + pState->iInpLen;
    int32_t iInpMax = pState->iInpMax - pState->iInpLen;
    int32_t iSendSize, iSentSize;

    iInpMax = pState->iInpMax - pState->iInpLen;

    if ((iSendSize = iSize) > 0)
    {
        if (iSendSize > (iInpMax - (6+2+2 + 1+2+2)))
        {
            iSendSize = (iInpMax - (6+2+2 + 1+2+2));
        }
        if (iSendSize <= 0)
        {
            NetPrintf(("protohttp: compacting send buffer\n"));
            memmove(pState->pInpBuf, pState->pInpBuf+pState->iInpOff, pState->iInpLen-pState->iInpOff);
            pState->iInpLen -= pState->iInpOff;
            pState->iInpOff = 0;
            return(_ProtoHttpSendChunk(pState, pStrBuf, iSize));
        }
    }
    else
    {
        pState->iPostSize = 0;
    }
    
    iSentSize = snzprintf(pInpBuf, iInpMax, "%x\r\n", iSendSize);
    if (iSendSize > 0)
    {
        memcpy(pInpBuf+iSentSize, pStrBuf, iSendSize);
        iSentSize += iSendSize;
    }
    iSentSize += snzprintf(pInpBuf+iSentSize, iInpMax, "\r\n");

    pState->iInpLen += iSentSize;

    return(iSendSize);
}

static int32_t _ProtoHttpRecv(ProtoHttpRefT *pState, char *pStrBuf, int32_t iSize)
{
    int32_t iResult;

    if ((iResult = ProtoSSLRecv(pState->pSsl, pStrBuf, iSize)) > 0)
    {
        #if DIRTYCODE_DEBUG
        if (pState->iVerbose > 2)
        {
            NetPrintMem(pStrBuf, iResult, "http-recv");
        }
        #endif
        
        pState->uTimer = NetTick() + pState->uTimeout;
    }
    return(iResult);
}


/*** Public Functions *************************************************************/


ProtoHttpRefT *ProtoHttpCreate(int32_t iBufSize)
{
    ProtoHttpRefT *pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    if (iBufSize < 4096)
    {
        iBufSize = 4096;
    }

    if ((pState = DirtyMemAlloc(sizeof(*pState), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttp: unable to allocate module state\n"));
        return(NULL);
    }
    memset(pState, 0, sizeof(*pState));
    if ((pState->pSsl = ProtoSSLCreate()) == NULL)
    {
        NetPrintf(("protohttp: unable to allocate ssl module\n"));
        ProtoHttpDestroy(pState);
        return(NULL);
    }
    if ((pState->pInpBuf = DirtyMemAlloc(iBufSize, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttp: unable to allocate protohttp buffer\n"));
        ProtoHttpDestroy(pState);
        return(NULL);
    }

    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->eState = ST_IDLE;
    pState->iInpMax = iBufSize;
    pState->uTimeout = PROTOHTTP_TIMEOUT;
    pState->bVerifyHdr = TRUE;
    pState->iVerbose = 1;
    pState->iMaxRedirect = PROTOHTTP_MAXREDIRECT;

    return(pState);
}

void ProtoHttpCallback(ProtoHttpRefT *pState, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, void *pCallbackRef)
{
    pState->pCustomHeaderCb = pCustomHeaderCb;
    pState->pCallbackRef = pCallbackRef;
}

void ProtoHttpCallback2(ProtoHttpRefT *pState, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pCallbackRef)
{
    pState->pCustomHeaderCb = pCustomHeaderCb;
    pState->pReceiveHeaderCb = pReceiveHeaderCb;
    pState->pCallbackRef = pCallbackRef;
}

void ProtoHttpDestroy(ProtoHttpRefT *pState)
{
    if (pState->pSsl != NULL)
    {
        ProtoSSLDestroy(pState->pSsl);
    }
    if (pState->pInpBuf != NULL)
    {
        DirtyMemFree(pState->pInpBuf, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }
    DirtyMemFree(pState, PROTOHTTP_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

int32_t ProtoHttpGet(ProtoHttpRefT *pState, const char *pUrl, uint32_t bHeadOnly)
{
    pState->iNumRedirect = 0;

    _ProtoHttpFormatRequest(pState, pUrl, NULL, 0, bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET);
    _ProtoHttpSendRequest(pState);
    return(0);
}

int32_t ProtoHttpRecv(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    int32_t iLen;

    if (pState->eState == ST_FAIL)
        return(PROTOHTTP_RECVFAIL);
    if ((pState->eState != ST_BODY) && (pState->eState != ST_DONE))
        return(PROTOHTTP_RECVWAIT);

    if (pState->bHeadOnly == TRUE)
        return(PROTOHTTP_RECVHEAD);

    if (iBufMax < 1)
        return(0);

    if (iBufMin < 1)
        iBufMin = 1;
    if (iBufMax < iBufMin)
        iBufMax = iBufMin;
    if (iBufMin > pState->iInpMax)
        iBufMin = pState->iInpMax;
    if (iBufMax > pState->iInpMax)
        iBufMax = pState->iInpMax;

    if ((iBufMin > pState->iInpMax-pState->iInpOff) || (pState->bCompactRecv == TRUE))
    {
        memmove(pState->pInpBuf, pState->pInpBuf+pState->iInpOff, pState->iInpLen-pState->iInpOff);
        pState->iInpLen -= pState->iInpOff;
        pState->iInpOff = 0;
        pState->bCompactRecv = FALSE;
        ProtoHttpUpdate(pState);
    }
    
    if (pState->bChunked == TRUE)
    {
        iLen = _ProtoHttpChunkProcess(pState, iBufMax);
    }
    else if ((iLen = (pState->iInpLen - pState->iInpOff)) > iBufMax)
    {
        iLen = iBufMax;
    }

    if ((iLen == 0) && (pState->eState == ST_DONE))
    {
        return(PROTOHTTP_RECVDONE);
    }

    if ((iLen >= iBufMin) || (pState->iInpCnt == pState->iBodySize))
    {
        if (pBuffer != NULL)
        {
            memcpy(pBuffer, pState->pInpBuf+pState->iInpOff, iLen);
            
            #if DIRTYCODE_DEBUG
            if (pState->iVerbose > 3)
            {
                NetPrintMem(pBuffer, iLen, "http-read");
            }
            #endif
        }
        pState->iInpOff += iLen;

        if ((pState->bChunked == TRUE) && (pState->iChkLen == 0))
        {
            pState->iInpOff += 2;
        }

        return(iLen);
    }

    return(0);
}

int32_t ProtoHttpRecvAll(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufSize)
{
    int32_t iRecvMax, iRecvResult;

    for (iRecvMax = iBufSize-1; (iRecvResult = ProtoHttpRecv(pState, pBuffer + pState->iRecvSize, 1, iRecvMax - pState->iRecvSize)) > 0; )
    {
        pState->iRecvSize += iRecvResult;

        if (pState->iRecvSize == iRecvMax)
        {
            iRecvResult = PROTOHTTP_RECVBUFF;
            break;
        }
    }
    
    if (iRecvResult == PROTOHTTP_RECVDONE)
    {
        pBuffer[pState->iRecvSize] = '\0';
        return(pState->iRecvSize);
    }
    else if ((iRecvResult < 0) && (iRecvResult != PROTOHTTP_RECVWAIT))
    {
        NetPrintf(("protohttp: error %d receiving response\n", iRecvResult));
        return(iRecvResult);
    }
    else 
    {
        return(0);
    }
}


int32_t ProtoHttpPost(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int32_t iDataSize, uint32_t bDoPut)
{
    int32_t iDataSent;
    if ((pData != NULL) && (iDataSize <= 0))
    {
        iDataSize = (int32_t)strlen(pData);
    }
    pState->iPostSize = iDataSize;
    iDataSent = _ProtoHttpFormatRequest(pState, pUrl, pData, iDataSize, bDoPut ? PROTOHTTP_REQUESTTYPE_PUT : PROTOHTTP_REQUESTTYPE_POST);
    _ProtoHttpSendRequest(pState);
    return(iDataSent);
}

int32_t ProtoHttpSend(ProtoHttpRefT *pState, const char *pData, int32_t iDataSize)
{
    if ((pState->iPostSize < 0) && (pState->eState == ST_SEND))
    {
    }
    else if (pState->eState < ST_RESP)
    {
        return(0);
    }
    else if (pState->eState != ST_RESP)
    {
        return(-1);
    }

    if (iDataSize > pState->iInpMax)
    {
        iDataSize = pState->iInpMax;
    }

    if (pState->iPostSize < 0)
    {
        return(_ProtoHttpSendChunk(pState, pData, iDataSize));
    }
    else
    {
        return(_ProtoHttpSend(pState, pData, iDataSize));
    }
}

int32_t ProtoHttpDelete(ProtoHttpRefT *pState, const char *pUrl)
{
    pState->iNumRedirect = 0;

    _ProtoHttpFormatRequest(pState, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_DELETE);
    _ProtoHttpSendRequest(pState);
    
    return(0);
}

int32_t ProtoHttpRequest(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int32_t iDataSize, ProtoHttpRequestTypeE eRequestType)
{
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
    else
    {
        NetPrintf(("protohttp: unrecognized request type %d\n", eRequestType));
        return(-1);
    }
}

void ProtoHttpAbort(ProtoHttpRefT *pState)
{
    _ProtoHttpClose(pState, "abort");

    _ProtoHttpReset(pState);
}

int32_t ProtoHttpGetHeaderValue(ProtoHttpRefT *pState, const char *pHdrBuf, const char *pName, char *pBuffer, int32_t iBufSize, const char **pHdrEnd)
{
    int32_t iResult;
    
    if ((pHdrBuf = _ProtoHttpFindHeaderFieldValue(pHdrBuf, pName)) == NULL)
    {
        return(-1);
    }
 
    if ((pState != NULL) && (!stricmp(pName, "location")))
    {
        iResult = _ProtoHttpGetLocationHeader(pState, pHdrBuf, pBuffer, iBufSize, pHdrEnd);
    }
    else 
    {
        iResult = _ProtoHttpGetHeaderFieldValue(pHdrBuf, pBuffer, iBufSize, pHdrEnd);
    }
    
    return(iResult);
}

int32_t ProtoHttpGetNextHeader(ProtoHttpRefT *pState, const char *pHdrBuf, char *pName, int32_t iNameSize, char *pValue, int32_t iValSize, const char **pHdrEnd)
{
    int32_t iNameLen, iResult;
    
    if (!strncmp(pHdrBuf, "HTTP", 4))
    {
        for ( ; *pHdrBuf != '\r' && *pHdrBuf != '\0'; pHdrBuf += 1)
            ;
        if (*pHdrBuf == '\0')
        {
            return(-1);
        }
    }

    pHdrBuf += 2;
    
    if ((iNameLen = _ProtoHttpGetHeaderFieldName(pHdrBuf, pName, iNameSize)) <= 0)
    {
        return(-1);
    }
    for (pHdrBuf += iNameLen + 1; (*pHdrBuf != '\0') && (*pHdrBuf <= ' '); pHdrBuf += 1)
        ;
    
    iResult = _ProtoHttpGetHeaderFieldValue(pHdrBuf, pValue, iValSize, pHdrEnd);
    return(iResult);
}


int32_t ProtoHttpControl(ProtoHttpRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iSelect == 'apnd')
    {
        strnzcpy(pState->strAppendHdr, pValue, sizeof(pState->strAppendHdr));
        return(0);
    }
    if (iSelect == 'disc')
    {
        _ProtoHttpClose(pState, "user request");
        return(0);
    }
    if (iSelect == 'hver')
    {
        NetPrintf(("protohttp: header type verification %s\n", iValue ? "enabled" : "disabled"));
        pState->bVerifyHdr = iValue;
    }
    if (iSelect == 'keep')
    {
        NetPrintf(("protohttp: setting keep-alive to %d\n", iValue));
        pState->iKeepAlive = pState->iKeepAliveDflt = iValue;
        return(0);
    }
    if (iSelect == 'rmax')
    {
        pState->iMaxRedirect = iValue;
        return(0);
    }
    if (iSelect == 'spam')
    {
        pState->iVerbose = iValue;
        return(0);
    }
    if (iSelect == 'time')
    {
        NetPrintf(("protohttp: setting timeout to %d ms\n", iValue));
        pState->uTimeout = (unsigned)iValue;
        return(0);
    }
    return(ProtoSSLControl(pState->pSsl, iSelect, iValue, iValue2, pValue));
}


int32_t ProtoHttpStatus(ProtoHttpRefT *pState, int32_t iSelect, void *pBuffer, int32_t iBufSize)
{
    if (iSelect == 'sque')
    {
        if (pState->pSsl != NULL)
        {
            return(ProtoSSLStat(pState->pSsl, 'sque', NULL, 0));
        }
        return(-1);
    }

    if (iSelect == 'host')
    {
        strnzcpy(pBuffer, pState->strHost, iBufSize);
        return(0);
    }

    if (iSelect == 'rtxt')
    {
        strnzcpy(pBuffer, pState->strRequestHdr, iBufSize);
        return(0);
    }
    
    if (iSelect == 'done')
    {
        if (pState->eState == ST_FAIL)
            return(-1);
        if (pState->eState == ST_DONE)
            return(1);
        return(0);
    }

    if (iSelect == 'data')
    {
        if (pState->eState == ST_FAIL)
            return(-1);
        if ((pState->eState == ST_BODY) || (pState->eState == ST_DONE))
            return(pState->iInpLen);
        return(0);
    }

    if (iSelect == 'code')
        return(pState->iHdrCode);
    
    if (iSelect == 'time')
        return(pState->bTimeout);

    if (iSelect == 'info')
    {
        if (pState->bInfoHdr)
        {
            if (pBuffer != NULL)
            {
                strnzcpy(pBuffer, pState->strHdr, iBufSize);
            }
            pState->bInfoHdr = FALSE;
            return(pState->iHdrCode);
        }
        return(0);
    }
    if (iSelect == 'locn')
    {
        return(_ProtoHttpGetLocationHeader(pState, pState->pInpBuf, pBuffer, iBufSize, NULL));
    }

    if (pState->eState == ST_FAIL)
        return(-1);
    if ((pState->eState != ST_BODY) && (pState->eState != ST_DONE))
        return(-2);

    if (iSelect == 'head')
        return(pState->iHeadSize);
    if (iSelect == 'body')
        return(pState->iBodySize);
    if (iSelect == 'date')
        return(pState->iHdrDate);
    if (iSelect == 'htxt')
    {
        strnzcpy(pBuffer, pState->strHdr, iBufSize);
        return(0);
    }
    return(-1);
}

int32_t ProtoHttpCheckKeepAlive(ProtoHttpRefT *pState, const char *pUrl)
{
    char strHost[sizeof(pState->strHost)], strKind[6];
    int32_t iPort, iSecure;
    
    pUrl = _ProtoHttpParseUrl(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure);
    
    if ((pState->iKeepAlive > 0) && (pState->iPort == iPort) && (pState->iSecure == iSecure) && !stricmp(pState->strHost, strHost))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

void ProtoHttpUpdate(ProtoHttpRefT *pState)
{
    int32_t iResult;
    char strData[256];

    ProtoSSLUpdate(pState->pSsl);

    if ((pState->eState != ST_IDLE) && (pState->eState != ST_DONE) && (pState->eState != ST_FAIL))
    {
        if (NetTickDiff(NetTick(), pState->uTimer) >= 0) 
        {
            NetPrintf(("protohttp: server timeout (state=%d)\n", pState->eState));
            pState->eState = ST_FAIL;
            pState->bTimeout = TRUE;
        }
    }

    if (pState->eState == ST_CONN)
    {
        iResult = ProtoSSLStat(pState->pSsl, 'stat', NULL, 0);
        if (iResult > 0)
        {
            pState->uTimer = NetTick() + pState->uTimeout;
            pState->eState = ST_SEND;
        }
        if (iResult < 0)
        {
            NetPrintf(("protohttp: ST_CONN got ST_FAIL (err=%d)\n", iResult));
            pState->eState = ST_FAIL;
        }
    }

    if (pState->eState == ST_SEND)
    {
        iResult = _ProtoHttpSend(pState, pState->pInpBuf+pState->iInpOff, pState->iInpLen);
        if (iResult > 0)
        {
            pState->iInpOff += iResult;
            pState->iInpLen -= iResult;
            if (pState->iInpLen == 0)
            {
                #if DIRTYCODE_DEBUG
                if (pState->iVerbose > 0)
                {
                    int32_t iRequestType;
                    
                    for (iRequestType = 0; iRequestType < PROTOHTTP_NUMREQUESTTYPES; iRequestType += 1)
                    {
                        if (!strncmp(pState->pInpBuf, _ProtoHttp_strRequestNames[iRequestType], strlen(_ProtoHttp_strRequestNames[iRequestType])))
                        {
                            char *pHdrEnd = strstr(pState->pInpBuf, "\r\n\r\n"), cHdrChr;
                            if (pHdrEnd != NULL)
                            {
                                cHdrChr = *pHdrEnd;
                                *pHdrEnd = '\0';
                                NetPrintf(("protohttp: sent request:\n"));
                                NetPrintWrap(pState->pInpBuf, 80);
                                *pHdrEnd = cHdrChr;
                            }
                            break;
                        }
                    }
                }
                #endif
                pState->iInpOff = 0;
            }
        }
        else if (iResult < 0)
        {
            NetPrintf(("protohttp: failed to send request (err=%d)\n", iResult));
            pState->iInpLen = 0;
            pState->eState = ST_FAIL;
        }
        if ((pState->iInpLen == 0) && (pState->iPostSize != -1))
        {
            pState->eState = ST_RESP;
        }
    }
    
    if (pState->eState == ST_RESP)
    {
        if ((iResult = _ProtoHttpRecv(pState, strData, 1)) > 0)
        {
            pState->pInpBuf[0] = strData[0];
            pState->iInpLen = 1;
            pState->iInpOff = 0;
            pState->eState = ST_HEAD;
        }
        if (iResult < 0)
        {
            if (pState->iKeepAlive > 1)
            {
                NetPrintf(("protohttp: request failure on keep-alive connection; retrying\n"));
                _ProtoHttpClose(pState, "retry");
                
                pState->iInpLen = pState->iInpOff;
                pState->iInpOff = 0;
                
                pState->iKeepAlive = 1;
                
                ProtoSSLConnect(pState->pSsl, pState->iSecure, pState->strHost, 0, pState->iPort);
                pState->eState = ST_CONN;
            }
            else
            {        
                NetPrintf(("protohttp: ST_HEAD byte0 got ST_FAIL (err=%d)\n", iResult));
                pState->iInpLen = 0;
                pState->eState = ST_FAIL;
            }
        }
    }

    if (pState->eState == ST_HEAD)
    {
        if ((iResult = _ProtoHttpRecv(pState, pState->pInpBuf+pState->iInpLen, pState->iInpMax - pState->iInpLen)) > 0)
        {
            pState->iInpLen += iResult;
        }
        if (iResult < 0)
        {
            NetPrintf(("protohttp: ST_HEAD append got ST_FAIL (err=%d, len=%d)\n", iResult, pState->iInpLen));
            pState->eState = ST_FAIL;
        }
    }

    if ((pState->eState == ST_HEAD) && (pState->iInpLen > 4))
    {
        if (_ProtoHttpParseHeader(pState) < 0)
        {
            return;
        }

        if ((pState->bHttp1_0 == TRUE) && (pState->iBodySize == 0) && (pState->iInpCnt > 0))
        {
            pState->iBodySize = -1;
        }
        
        if (pState->bHeadOnly == TRUE)
        {
            pState->eState = ST_DONE;
        }
        else if ((pState->iBodySize >= 0) && (pState->iInpCnt >= pState->iBodySize))
        {
            pState->eState = ST_DONE;
        }
        else
        {
            pState->eState = ST_BODY;
        }

        if (PROTOHTTP_GetResponseClass(pState->iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL)
        {
            _ProtoHttpProcessInfoHeader(pState);
        }
        else if (PROTOHTTP_GetResponseClass(pState->iHdrCode) == PROTOHTTP_RESPONSE_REDIRECTION)
        {
            _ProtoHttpRedirect(pState);
        }
    }

    while (pState->eState == ST_BODY)
    {
        if ((pState->iInpLen > 0) && (pState->iInpOff == pState->iInpLen))
        {
            pState->iInpOff = pState->iInpLen = 0;
        }

        iResult = pState->iInpMax - pState->iInpLen;
        if (iResult <= 0)
        {
            iResult = 0;
        }
        else
        {
            iResult = _ProtoHttpRecv(pState, pState->pInpBuf+pState->iInpLen, iResult);
        }
        if (iResult == 0)
        {
            break;
        }

        if ((iResult == SOCKERR_CLOSED) && ((pState->iBodySize == -1) || (pState->iBodySize == pState->iInpCnt)))
        {
            pState->iBodySize = pState->iInpCnt;
            pState->bCloseHdr = TRUE;
            pState->eState = ST_DONE;
            break;
        }

        if (iResult < 0)
        {
            NetPrintf(("protohttp: ST_FAIL (err=%d)\n", iResult));
            pState->eState = ST_FAIL;
            break;
        }

        pState->iInpLen += iResult;
        pState->iInpCnt += iResult;

        if ((pState->iBodySize >= 0) && (pState->iInpCnt >= pState->iBodySize))
        {
            pState->eState = ST_DONE;
            break;
        }
    }

    if (pState->eState == ST_FAIL)
    {
        _ProtoHttpClose(pState, "error");
    }
    
    if (pState->eState == ST_DONE)
    {
        if (pState->bCloseHdr)
        {
            _ProtoHttpClose(pState, "server request");
        }
    }
}
    
int32_t ProtoHttpUrlEncodeIntParm(char *pBuffer, int32_t iLength, const char *pParm, int32_t iValue)
{
    char strValue[32];
    char *pData;

    snzprintf(strValue, sizeof(strValue), "%d", iValue);
    pData = strValue;

    iLength -= 1;

    for (; (*pBuffer != 0) && (iLength > 0); iLength--)
    {
        pBuffer++;
    }

    for (; (*pParm != 0) && (iLength > 0); iLength--)
    {
        *pBuffer++ = *pParm++;
    }

    for (; (*pData != 0) && (iLength > 0); iLength--)
    {
        *pBuffer++ = *pData++;
    }

    *pBuffer = '\0';

    return(0);
}

int32_t ProtoHttpUrlEncodeStrParm(char *pBuffer, int32_t iLength, const char *pParm, const char *pData)
{
    static char _strSafe[256] =
        "0000000000000000" "0000000000000000"   
        "0000000000000000" "@ABCDEFGHI000000"   
        "0JKLMNO111111111" "1111111111100000"   
        "0JKLMNO111111111" "1111111111100000"   
        "0000000000000000" "0000000000000000"   
        "0000000000000000" "0000000000000000"   
        "0000000000000000" "0000000000000000"   
        "0000000000000000" "0000000000000000";  

    
    static char _strHex[16] = "0123456789ABCDEF";

    iLength -= 1;

    for (; (*pBuffer != 0) && (iLength > 0); iLength--)
    {
        pBuffer++;
    }

    for (; (*pParm != 0) && (iLength > 0); iLength--)
    {
        *pBuffer++ = *pParm++;
    }

    for (; (*pData != 0) && (iLength > 2); )
    {
        if (_strSafe[*(uint8_t *)pData] == '0')
        {
            uint8_t ch = *pData++;
            *pBuffer++ = '%';
            *pBuffer++ = _strHex[ch>>4];
            *pBuffer++ = _strHex[ch&15];
            iLength -= 3;    
        }
        else
        {
            *pBuffer++ = *pData++;
            iLength--;
        }
    }

    for (; (*pData != 0) && (iLength > 0) && (_strSafe[*(uint8_t *)pData] != '0'); iLength--)
    {
        *pBuffer++ = *pData++;
    }

    *pBuffer = '\0';

    return(0);
}

int32_t ProtoHttpSetCACert(const uint8_t *pCACert, int32_t iCertSize)
{
    return(ProtoSSLSetCACert(pCACert, iCertSize));
}

void ProtoHttpClrCACerts(void)
{
    ProtoSSLClrCACerts();
}
