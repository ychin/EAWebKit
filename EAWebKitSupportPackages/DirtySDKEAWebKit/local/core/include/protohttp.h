/*
Copyright (C) 2000-2007,2009 Electronic Arts, Inc.  All rights reserved.

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
    \File    protohttp.h

*/
/********************************************************************************H*/

#ifndef _protohttp_h
#define _protohttp_h

/*!
\Module Proto
*/
//@{

/*** Include files ****************************************************************/

/*** Defines **********************************************************************/

#define PROTOHTTP_POST      (0)     
#define PROTOHTTP_PUT       (1)     

#define PROTOHTTP_STREAM_BEGIN (-1) 

#define PROTOHTTP_STREAM_END   (0)  

#define PROTOHTTP_HEADBODY  (0)     
#define PROTOHTTP_HEADONLY  (1)     

#define PROTOHTTP_RECVDONE  (-1)    
#define PROTOHTTP_RECVFAIL  (-2)   
#define PROTOHTTP_RECVWAIT  (-3)    
#define PROTOHTTP_RECVHEAD  (-4)    
#define PROTOHTTP_RECVBUFF  (-5)    

/*** Macros ***********************************************************************/

#define PROTOHTTP_GetResponseClass(_eError)                 (((_eError)/100) * 100)

/*** Type Definitions *************************************************************/

typedef enum ProtoHttpRequestTypeE
{
    PROTOHTTP_REQUESTTYPE_HEAD = 0,     
    PROTOHTTP_REQUESTTYPE_GET,          
    PROTOHTTP_REQUESTTYPE_POST,         
    PROTOHTTP_REQUESTTYPE_PUT,          
    PROTOHTTP_REQUESTTYPE_DELETE,       
    
    PROTOHTTP_NUMREQUESTTYPES
} ProtoHttpRequestTypeE;

typedef enum ProtoHttpResponseE
{
    PROTOHTTP_RESPONSE_INFORMATIONAL        = 100,  
    PROTOHTTP_RESPONSE_CONTINUE             = 100,  
    PROTOHTTP_RESPONSE_SWITCHPROTO,                 

    PROTOHTTP_RESPONSE_SUCCESSFUL           = 200,  
    PROTOHTTP_RESPONSE_OK                   = 200,  
    PROTOHTTP_RESPONSE_CREATED,                     
    PROTOHTTP_RESPONSE_ACCEPTED,                    
    PROTOHTTP_RESPONSE_NONAUTH,                     
    PROTOHTTP_RESPONSE_NOCONTENT,                   
    PROTOHTTP_RESPONSE_RESETCONTENT,                
    PROTOHTTP_RESPONSE_PARTIALCONTENT,              
                                            
    PROTOHTTP_RESPONSE_REDIRECTION          = 300,  
    PROTOHTTP_RESPONSE_MULTIPLECHOICES      = 300,  
    PROTOHTTP_RESPONSE_MOVEDPERMANENTLY,            
    PROTOHTTP_RESPONSE_FOUND,                       
    PROTOHTTP_RESPONSE_SEEOTHER,                    
    PROTOHTTP_RESPONSE_NOTMODIFIED,                 
    PROTOHTTP_RESPONSE_USEPROXY,                    
    PROTOHTTP_RESPONSE_RESERVED,                    
    PROTOHTTP_RESPONSE_TEMPREDIRECT,                
                                            
    PROTOHTTP_RESPONSE_CLIENTERROR          = 400,  
    PROTOHTTP_RESPONSE_BADREQUEST           = 400,  
    PROTOHTTP_RESPONSE_UNAUTHORIZED,                
    PROTOHTTP_RESPONSE_PAYMENTREQUIRED,             
    PROTOHTTP_RESPONSE_FORBIDDEN,                   
    PROTOHTTP_RESPONSE_NOTFOUND,                   
    PROTOHTTP_RESPONSE_METHODNOTALLOWED,            
    PROTOHTTP_RESPONSE_NOTACCEPTABLE,               
    PROTOHTTP_RESPONSE_PROXYAUTHREQ,                
    PROTOHTTP_RESPONSE_REQUESTTIMEOUT,              
    PROTOHTTP_RESPONSE_CONFLICT,                    
    PROTOHTTP_RESPONSE_GONE,                        
    PROTOHTTP_RESPONSE_LENGTHREQUIRED,              
    PROTOHTTP_RESPONSE_PRECONFAILED,                
    PROTOHTTP_RESPONSE_REQENTITYTOOLARGE,           
    PROTOHTTP_RESPONSE_REQURITOOLONG,               
    PROTOHTTP_RESPONSE_UNSUPPORTEDMEDIA,            
    PROTOHTTP_RESPONSE_REQUESTRANGE,                
    PROTOHTTP_RESPONSE_EXPECTATIONFAILED,           
                                            
    PROTOHTTP_RESPONSE_SERVERERROR          = 500,  
    PROTOHTTP_RESPONSE_INTERNALSERVERERROR  = 500,  
    PROTOHTTP_RESPONSE_NOTIMPLEMENTED,              
    PROTOHTTP_RESPONSE_BADGATEWAY,                  
    PROTOHTTP_RESPONSE_SERVICEUNAVAILABLE,          
    PROTOHTTP_RESPONSE_GATEWAYTIMEOUT,              
    PROTOHTTP_RESPONSE_HTTPVERSUNSUPPORTED          
} ProtoHttpResponseE;                       

typedef struct ProtoHttpRefT ProtoHttpRefT;

typedef void (ProtoHttpCustomHeaderCbT)(ProtoHttpRefT *pState, char *pHeader, uint32_t uHeaderSize, const char *pData, uint32_t uDataLen, void *pUserRef);

typedef void (ProtoHttpReceiveHeaderCbT)(ProtoHttpRefT *pState, const char *pHeader, uint32_t uHeaderSize, void *pUserRef);

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

ProtoHttpRefT *ProtoHttpCreate(int32_t iRcvBuf);

void ProtoHttpCallback(ProtoHttpRefT *pState, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, void *pUserRef);

void ProtoHttpCallback2(ProtoHttpRefT *pState, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pUserRef);

void ProtoHttpDestroy(ProtoHttpRefT *pState);

int32_t ProtoHttpGet(ProtoHttpRefT *pState, const char *pUrl, uint32_t bHeadOnly);

int32_t ProtoHttpRecv(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufMin, int32_t iBufMax);

int32_t ProtoHttpRecvAll(ProtoHttpRefT *pState, char *pBuffer, int32_t iBufSize);

int32_t ProtoHttpPost(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int32_t iDataSize, uint32_t bDoPut);

int32_t ProtoHttpSend(ProtoHttpRefT *pState, const char *pData, int32_t iDataSize);

int32_t ProtoHttpDelete(ProtoHttpRefT *pState, const char *pUrl);

int32_t ProtoHttpRequest(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int32_t iDataSize, ProtoHttpRequestTypeE eRequestType);

void ProtoHttpAbort(ProtoHttpRefT *pState); 

int32_t ProtoHttpGetHeaderValue(ProtoHttpRefT *pState, const char *pHdrBuf, const char *pName, char *pBuffer, int32_t iBufSize, const char **pHdrEnd);

int32_t ProtoHttpGetNextHeader(ProtoHttpRefT *pState, const char *pHdrBuf, char *pName, int32_t iNameSize, char *pValue, int32_t iValSize, const char **pHdrEnd);

int32_t ProtoHttpControl(ProtoHttpRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue);

int32_t ProtoHttpStatus(ProtoHttpRefT *pState, int32_t iSelect, void *pBuffer, int32_t iBufSize);

int32_t ProtoHttpCheckKeepAlive(ProtoHttpRefT *pState, const char *pUrl);

void ProtoHttpUpdate(ProtoHttpRefT *pState);

int32_t ProtoHttpUrlEncodeIntParm(char *pBuffer, int32_t iLength, const char *pParm, int32_t iValue);

int32_t ProtoHttpUrlEncodeStrParm(char *pBuffer, int32_t iLength, const char *pParm, const char *pData);

int32_t ProtoHttpSetCACert(const uint8_t *pCACert, int32_t iCertSize);

void ProtoHttpClrCACerts(void);


#ifdef __cplusplus
}
#endif

//@}        

#endif // _protohttp_h

