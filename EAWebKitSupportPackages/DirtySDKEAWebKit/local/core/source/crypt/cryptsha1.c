/*
Copyright (C) 2004,2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

/*H*******************************************************************/
/*!
    \File cryptsha1.c
*/
/*******************************************************************H*/

/*** Include files ***************************************************/

#include <string.h>            

#include "dirtyplatform.h"
#include "cryptsha1.h"

/*** Defines *********************************************************/

#define CRYPTSHA1_rol(n, x) (((x)<<(n))|((x)>>(32-(n))))

/*** Type Definitions ************************************************/

/*** Variables *******************************************************/

/*** Private functions ***********************************************/

static void _CryptSha1CopyHash(CryptSha1T *pSha1, void *pBuffer, uint32_t uLength)
{
    unsigned char *pOutput = pBuffer;
    uint32_t i;

    if (uLength > CRYPTSHA1_HASHSIZE)
    {
        uLength = CRYPTSHA1_HASHSIZE;
    }
    for (i = 0; i != uLength; i += 1)
    {
        pOutput[i] = pSha1->H[(i/4)]>>((3-(i%4))*8);
    }
}

static void _CryptSha1ProcessBlock(CryptSha1T *pSha1, const unsigned char *M)
{
    uint32_t i;
    uint32_t t;
    unsigned A, B, C, D, E;
    uint32_t W[80];

    for (i = 0; i != 16; i += 1)
    {
        W[i] = (M[i*4]<<24)|(M[i*4+1]<<16)|(M[i*4+2]<<8)|M[i*4+3];
    }

    for (t = 16; t != 80; t += 1)
    {
        W[t] = CRYPTSHA1_rol(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = pSha1->H[0];
    B = pSha1->H[1];
    C = pSha1->H[2];
    D = pSha1->H[3];
    E = pSha1->H[4];

    for (t = 0; t != 20; t += 1)
    {
        uint32_t TEMP = CRYPTSHA1_rol(5, A) + ((B&C)|((~B)&D)) + E + W[t] + 0x5A827999;
        E = D; D = C; C = CRYPTSHA1_rol(30, B); B = A; A = TEMP;
    }

    for (t = 20; t != 40; t += 1)
    {
        uint32_t TEMP = CRYPTSHA1_rol(5, A) + (B^C^D) + E + W[t] + 0x6ED9EBA1;
        E = D; D = C; C = CRYPTSHA1_rol(30, B); B = A; A = TEMP;
    }

    for (t = 40; t != 60; t += 1)
    {
        uint32_t TEMP = CRYPTSHA1_rol(5, A) + ((B&C)|(B&D)|(C&D)) + E + W[t] + 0x8F1BBCDC;
        E = D; D = C; C = CRYPTSHA1_rol(30, B); B = A; A = TEMP;
    }

    for (t = 60; t != 80; t += 1)
    {
        uint32_t TEMP = CRYPTSHA1_rol(5, A) + (B^C^D) + E + W[t] + 0xCA62C1D6;
        E = D; D = C; C = CRYPTSHA1_rol(30, B); B = A; A = TEMP;
    }

    pSha1->H[0] += A;
    pSha1->H[1] += B;
    pSha1->H[2] += C;
    pSha1->H[3] += D;
    pSha1->H[4] += E;
}


/*** Public functions ************************************************/

void CryptSha1Init(CryptSha1T *pSha1)
{
    pSha1->uCount = 0;
    pSha1->uPartialCount = 0;

    pSha1->H[0] = 0x67452301;
    pSha1->H[1] = 0xEFCDAB89;
    pSha1->H[2] = 0x98BADCFE;
    pSha1->H[3] = 0x10325476;
    pSha1->H[4] = 0xC3D2E1F0;
}


void CryptSha1Update(CryptSha1T *pSha1, const unsigned char *pInput, uint32_t uInputLen)
{
    if (pSha1->uPartialCount != 0)
    {
        uint32_t uWant = sizeof(pSha1->strData) - pSha1->uPartialCount;
        uint32_t uHave = uWant > uInputLen ? uInputLen : uWant;
        memcpy(&pSha1->strData[pSha1->uPartialCount], pInput, uHave);
        pInput += uHave;
        uInputLen -= uHave;
        if (uHave == uWant)
        {
            _CryptSha1ProcessBlock(pSha1, pSha1->strData);
            pSha1->uCount += sizeof(pSha1->strData);
            pSha1->uPartialCount = 0;
        }
        else
        {
            pSha1->uPartialCount += uHave;
        }
    }
    while (uInputLen >= sizeof(pSha1->strData))
    {
        _CryptSha1ProcessBlock(pSha1, pInput);
        pSha1->uCount += sizeof(pSha1->strData);
        uInputLen -= sizeof(pSha1->strData);
        pInput += sizeof(pSha1->strData);
    }
    if (uInputLen != 0)
    {
        memcpy(&pSha1->strData[pSha1->uPartialCount], pInput, uInputLen);
        pSha1->uPartialCount += uInputLen;
    }
}


void CryptSha1Final(CryptSha1T *pSha1, void *pBuffer, uint32_t uLength)
{
    uint32_t i;
    uint8_t uPad = 0x80;
    uint32_t uSpace = sizeof(pSha1->strData) - pSha1->uPartialCount;

    pSha1->uCount += pSha1->uPartialCount;
    if (uSpace < 9)
    {
        pSha1->strData[pSha1->uPartialCount] = uPad;
        for (i = pSha1->uPartialCount+1; i < sizeof(pSha1->strData); i += 1)
        {
            pSha1->strData[i] = 0x0;
        }
        _CryptSha1ProcessBlock(pSha1, pSha1->strData);
        uPad = 0x0;
        pSha1->uPartialCount = 0;
    }
    pSha1->strData[pSha1->uPartialCount] = uPad;
    for (i = pSha1->uPartialCount+1; i < sizeof(pSha1->strData)-8; i += 1)
    {
        pSha1->strData[i] = 0x0;
    }
    
    pSha1->strData[56] = 0;
    pSha1->strData[57] = 0;
    pSha1->strData[58] = 0;
    pSha1->strData[59] = (pSha1->uCount>>(32-3))&0xFF;
    pSha1->strData[60] = (pSha1->uCount>>(24-3))&0xFF;
    pSha1->strData[61] = (pSha1->uCount>>(16-3))&0xFF;
    pSha1->strData[62] = (pSha1->uCount>>(8-3))&0xFF;
    pSha1->strData[63] = (pSha1->uCount<<3)&0xFF;
    _CryptSha1ProcessBlock(pSha1, pSha1->strData);

    _CryptSha1CopyHash(pSha1, pBuffer, uLength);
}
