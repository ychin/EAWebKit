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

/*H*************************************************************************************************/
/*!
    \File cryptmd2.h
*/
/*************************************************************************************************H*/

#ifndef _cryptmd3_h
#define _cryptmd3_h

/*!
\Module Crypt
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

#define MD2_BINARY_OUT 16       
#define MD2_STRING_OUT 33       

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct CryptMD2T CryptMD2T;

struct CryptMD2T
{
    int32_t iCount;         
    uint8_t aState[16];     
    uint8_t aChecksum[16];  
    uint8_t aBuffer[16];    
};

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CryptMD2Init(CryptMD2T *pContext);

void CryptMD2Update(CryptMD2T *pContext, const void *pBuffer, int32_t iLength);

void CryptMD2Final(CryptMD2T *pContext, void *pBuffer, int32_t iLength);

#ifdef __cplusplus
}
#endif

//@}

#endif // _cryptmd2_h


