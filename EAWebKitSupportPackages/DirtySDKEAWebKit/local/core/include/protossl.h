/*
Copyright (C) 2002,2009 Electronic Arts, Inc.  All rights reserved.

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

/*H*************************************************************************************************/
/*!

    \File    protossl.h
 
*/
/*************************************************************************************************H*/

#ifndef _protossl_h
#define _protossl_h

/*!
\Module Proto
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct ProtoSSLRefT ProtoSSLRefT;

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

ProtoSSLRefT *ProtoSSLCreate(void);

void ProtoSSLReset(ProtoSSLRefT *pState);

void ProtoSSLDestroy(ProtoSSLRefT *pState);

void ProtoSSLUpdate(ProtoSSLRefT *pState);

ProtoSSLRefT* ProtoSSLAccept(ProtoSSLRefT *pState, int32_t iSecure,
                             struct sockaddr *pAddr, int32_t *pAddrlen);

int32_t ProtoSSLBind(ProtoSSLRefT *pState, const struct sockaddr *pAddr, int32_t pAddrlen);

int32_t ProtoSSLConnect(ProtoSSLRefT *pState, int32_t iSecure, const char *pAddr, uint32_t uAddr, int32_t iPort);

int32_t ProtoSSLDisconnect(ProtoSSLRefT *pState);

int32_t ProtoSSLListen(ProtoSSLRefT *pState, int32_t iBacklog);

int32_t ProtoSSLSend(ProtoSSLRefT *pState, const char *pBuffer, int32_t iLength);

int32_t ProtoSSLRecv(ProtoSSLRefT *pState, char *pBuffer, int32_t iLength);

int32_t ProtoSSLStat(ProtoSSLRefT *pState, int32_t iSelect, void *pBuffer, int32_t iLength);

int32_t ProtoSSLControl(ProtoSSLRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue);

int32_t ProtoSSLSetCACert(const uint8_t *pCACert, int32_t iCertSize);

void ProtoSSLClrCACerts(void);

#ifdef __cplusplus
}
#endif

//@}

#endif // _protossl_h

