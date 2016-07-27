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

    \File    cryptmd5.h
*/
/*************************************************************************************************H*/


#ifndef _cryptmd5_h
#define _cryptmd5_h

/*!
\Module Crypt
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

#define MD5_BINARY_OUT 16       
#define MD5_STRING_OUT 33       

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct CryptMD5T CryptMD5T;

struct CryptMD5T
{
    unsigned char strData[64+8];
    uint32_t uCount;        
    uint32_t uRegs[4];      
};

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CryptMD5Init(CryptMD5T *pContext);

void CryptMD5Update(CryptMD5T *pContext, const void *pBuffer, int32_t iLength);

void CryptMD5Final(CryptMD5T *pContext, void *pBuffer, int32_t iLength);

#ifdef __cplusplus
}
#endif

//@}

#endif // _cryptmd5_h


