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
    \File    cryptrsa.h
*/
/*************************************************************************************************H*/

#ifndef _cryptrsa_h
#define _cryptrsa_h

/*!
\Module Crypt
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct CryptRSARandomT CryptRSARandomT;

struct CryptRSARandomT
{
    uint32_t u0;
    uint32_t u1;
};


typedef struct CryptRSAT CryptRSAT;

struct CryptRSAT
{
    int32_t iKeyModSize;            //!< size of public key modulus
    int32_t iKeyExpSize;            //!< size of public key exponent
    uint8_t EncryptBlock[1024];
    uint8_t KeyModData[256+1];      //!< public key modulus
    uint8_t KeyExpData[256+1];      //!< public key exponent
    CryptRSARandomT Random;
};

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CryptRSAInit(CryptRSAT *pState, const unsigned char *pModulus, int32_t iModSize,
                  const unsigned char *pExponent, int32_t iExpSize);

void CryptRSAInitMaster(CryptRSAT *pState, const unsigned char *pMaster, int32_t iMasterLen);

void CryptRSAInitSignature(CryptRSAT *pState, const unsigned char *pSig, int32_t iSigLen);


void CryptRSAEncrypt(CryptRSAT *pState);

#ifdef __cplusplus
}
#endif

//@}

#endif // _cryptrsa_h

