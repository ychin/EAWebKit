/*
Copyright (C) 2009 Electronic Arts, Inc.  All rights reserved.

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
    \File protohttpmanager.h

*/
/********************************************************************************H*/

#ifndef _protohttpmanager_h
#define _protohttpmanager_h

/*!
\Module Proto
*/
//@{

/*** Include files ****************************************************************/

#include "protohttp.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

typedef struct HttpManagerRefT HttpManagerRefT;

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

HttpManagerRefT *HttpManagerCreate(int32_t iHttpBufSize, int32_t iHttpNumRefs);

void HttpManagerCallback(HttpManagerRefT *pHttpManager, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pUserRef);

void HttpManagerDestroy(HttpManagerRefT *pHttpManager);

int32_t HttpManagerAlloc(HttpManagerRefT *pHttpManager);

void HttpManagerFree(HttpManagerRefT *pHttpManager, int32_t iHandle);

int32_t HttpManagerGet(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, uint32_t bHeadOnly);

int32_t HttpManagerRecv(HttpManagerRefT *pHttpManager, int32_t iHandle, char *pBuffer, int32_t iBufMin, int32_t iBufMax);

int32_t HttpManagerRecvAll(HttpManagerRefT *pHttpManager, int32_t iHandle, char *pBuffer, int32_t iBufSize);

int32_t HttpManagerPost(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, const char *pData, int32_t iDataSize, uint32_t bDoPut);

int32_t HttpManagerSend(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pData, int32_t iDataSize);

int32_t HttpManagerRequest(HttpManagerRefT *pHttpManager, int32_t iHandle, const char *pUrl, const char *pData, int32_t iDataSize, ProtoHttpRequestTypeE eRequestType);

int32_t HttpManagerControl(HttpManagerRefT *pHttpManager, int32_t iHandle, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue);

int32_t HttpManagerStatus(HttpManagerRefT *pHttpManager, int32_t iHandle, int32_t iSelect, void *pBuffer, int32_t iBufSize);

void HttpManagerUpdate(HttpManagerRefT *pHttpManager);

#ifdef __cplusplus
}
#endif

//@}

#endif // _protohttpmanager_h

