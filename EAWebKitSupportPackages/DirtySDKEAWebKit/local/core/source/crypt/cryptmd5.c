/*
Copyright (C) 2002,2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

    \File    cryptmd5.c

*/
/*************************************************************************************************H*/


/*** Include files *********************************************************************/

#include <string.h>
#include "dirtyplatform.h"
#include "cryptmd5.h"

/*** Defines ***************************************************************************/

#define FF(x, y, z) (z ^ ((y ^ z) & x))
#define GG(x, y, z) (y ^ ((x ^ y) & z))
#define HH(x, y, z) (x ^ y ^ z)
#define II(x, y, z) (y ^ (x | ~z))

#define ACC(r, t1, t2, t3, s, x) \
    r += (t1); r += (t2); r+= (t3); r = (r<<s) | (r>>(32-s)); r += x;

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables

static char *_MD5_HexChars = "0123456789abcdef";

// Public variables


/*** Private Functions *****************************************************************/


static void _CryptMD5Transform(CryptMD5T *pContext, const unsigned char *pBlock)
{
    register uint32_t a;
    register uint32_t b;
    register uint32_t c;
    register uint32_t d;
    uint32_t uData[16];
    volatile uint32_t *pVector = (uint32_t *)pBlock;

    if (1)
    {
        pBlock += 64;
        pVector = uData+16;
        while (pVector != uData)
        {
            b = *--pBlock;
            b = (b << 8) + *--pBlock;
            b = (b << 8) + *--pBlock;
            b = (b << 8) + *--pBlock;
            *--pVector = b;
        }
    }

    a = pContext->uRegs[0];
    b = pContext->uRegs[1];
    c = pContext->uRegs[2];
    d = pContext->uRegs[3];

    ACC(a, FF(b, c, d), pVector[ 0],0xd76aa478,  7, b);
    ACC(d, FF(a, b, c), pVector[ 1],0xe8c7b756, 12, a);
    ACC(c, FF(d, a, b), pVector[ 2],0x242070db, 17, d);
    ACC(b, FF(c, d, a), pVector[ 3],0xc1bdceee, 22, c);
    ACC(a, FF(b, c, d), pVector[ 4],0xf57c0faf,  7, b);
    ACC(d, FF(a, b, c), pVector[ 5],0x4787c62a, 12, a);
    ACC(c, FF(d, a, b), pVector[ 6],0xa8304613, 17, d);
    ACC(b, FF(c, d, a), pVector[ 7],0xfd469501, 22, c);
    ACC(a, FF(b, c, d), pVector[ 8],0x698098d8,  7, b);
    ACC(d, FF(a, b, c), pVector[ 9],0x8b44f7af, 12, a);
    ACC(c, FF(d, a, b), pVector[10],0xffff5bb1, 17, d);
    ACC(b, FF(c, d, a), pVector[11],0x895cd7be, 22, c);
    ACC(a, FF(b, c, d), pVector[12],0x6b901122,  7, b);
    ACC(d, FF(a, b, c), pVector[13],0xfd987193, 12, a);
    ACC(c, FF(d, a, b), pVector[14],0xa679438e, 17, d);
    ACC(b, FF(c, d, a), pVector[15],0x49b40821, 22, c);

    ACC(a, GG(b, c, d), pVector[ 1],0xf61e2562,  5, b);
    ACC(d, GG(a, b, c), pVector[ 6],0xc040b340,  9, a);
    ACC(c, GG(d, a, b), pVector[11],0x265e5a51, 14, d);
    ACC(b, GG(c, d, a), pVector[ 0],0xe9b6c7aa, 20, c);
    ACC(a, GG(b, c, d), pVector[ 5],0xd62f105d,  5, b);
    ACC(d, GG(a, b, c), pVector[10],0x02441453,  9, a);
    ACC(c, GG(d, a, b), pVector[15],0xd8a1e681, 14, d);
    ACC(b, GG(c, d, a), pVector[ 4],0xe7d3fbc8, 20, c);
    ACC(a, GG(b, c, d), pVector[ 9],0x21e1cde6,  5, b);
    ACC(d, GG(a, b, c), pVector[14],0xc33707d6,  9, a);
    ACC(c, GG(d, a, b), pVector[ 3],0xf4d50d87, 14, d);
    ACC(b, GG(c, d, a), pVector[ 8],0x455a14ed, 20, c);
    ACC(a, GG(b, c, d), pVector[13],0xa9e3e905,  5, b);
    ACC(d, GG(a, b, c), pVector[ 2],0xfcefa3f8,  9, a);
    ACC(c, GG(d, a, b), pVector[ 7],0x676f02d9, 14, d);
    ACC(b, GG(c, d, a), pVector[12],0x8d2a4c8a, 20, c);

    ACC(a, HH(b, c, d), pVector[ 5],0xfffa3942,  4, b);
    ACC(d, HH(a, b, c), pVector[ 8],0x8771f681, 11, a);
    ACC(c, HH(d, a, b), pVector[11],0x6d9d6122, 16, d);
    ACC(b, HH(c, d, a), pVector[14],0xfde5380c, 23, c);
    ACC(a, HH(b, c, d), pVector[ 1],0xa4beea44,  4, b);
    ACC(d, HH(a, b, c), pVector[ 4],0x4bdecfa9, 11, a);
    ACC(c, HH(d, a, b), pVector[ 7],0xf6bb4b60, 16, d);
    ACC(b, HH(c, d, a), pVector[10],0xbebfbc70, 23, c);
    ACC(a, HH(b, c, d), pVector[13],0x289b7ec6,  4, b);
    ACC(d, HH(a, b, c), pVector[ 0],0xeaa127fa, 11, a);
    ACC(c, HH(d, a, b), pVector[ 3],0xd4ef3085, 16, d);
    ACC(b, HH(c, d, a), pVector[ 6],0x04881d05, 23, c);
    ACC(a, HH(b, c, d), pVector[ 9],0xd9d4d039,  4, b);
    ACC(d, HH(a, b, c), pVector[12],0xe6db99e5, 11, a);
    ACC(c, HH(d, a, b), pVector[15],0x1fa27cf8, 16, d);
    ACC(b, HH(c, d, a), pVector[ 2],0xc4ac5665, 23, c);

    ACC(a, II(b, c, d), pVector[ 0],0xf4292244,  6, b);
    ACC(d, II(a, b, c), pVector[ 7],0x432aff97, 10, a);
    ACC(c, II(d, a, b), pVector[14],0xab9423a7, 15, d);
    ACC(b, II(c, d, a), pVector[ 5],0xfc93a039, 21, c);
    ACC(a, II(b, c, d), pVector[12],0x655b59c3,  6, b);
    ACC(d, II(a, b, c), pVector[ 3],0x8f0ccc92, 10, a);
    ACC(c, II(d, a, b), pVector[10],0xffeff47d, 15, d);
    ACC(b, II(c, d, a), pVector[ 1],0x85845dd1, 21, c);
    ACC(a, II(b, c, d), pVector[ 8],0x6fa87e4f,  6, b);
    ACC(d, II(a, b, c), pVector[15],0xfe2ce6e0, 10, a);
    ACC(c, II(d, a, b), pVector[ 6],0xa3014314, 15, d);
    ACC(b, II(c, d, a), pVector[13],0x4e0811a1, 21, c);
    ACC(a, II(b, c, d), pVector[ 4],0xf7537e82,  6, b);
    ACC(d, II(a, b, c), pVector[11],0xbd3af235, 10, a);
    ACC(c, II(d, a, b), pVector[ 2],0x2ad7d2bb, 15, d);
    ACC(b, II(c, d, a), pVector[ 9],0xeb86d391, 21, c);

    pContext->uRegs[0] += a;
    pContext->uRegs[1] += b;
    pContext->uRegs[2] += c;
    pContext->uRegs[3] += d;
}


/*** Public Functions ******************************************************************/


void CryptMD5Init(CryptMD5T *pContext)
{
    pContext->uCount = 0;

    pContext->uRegs[0] = 0x67452301;  
    pContext->uRegs[1] = 0xefcdab89;  
    pContext->uRegs[2] = 0x98badcfe;  
    pContext->uRegs[3] = 0x10325476;  
}

void CryptMD5Update(CryptMD5T *pContext, const void *_pBuffer, int32_t iLength)
{
    int32_t uAdd;
    uint32_t uCount;
    const unsigned char *pBuffer = _pBuffer;

    if (iLength < 0)
    {
        for (iLength = 0; pBuffer[iLength] != 0; ++iLength)
            ;
    }

    uCount = pContext->uCount&63;
    pContext->uCount += iLength;

    if (uCount > 0)
    {
        uAdd = 64-uCount;

        if (iLength < uAdd)
        {
            memcpy(pContext->strData+uCount, pBuffer, iLength);
            return;
        }

        memcpy(pContext->strData+uCount, pBuffer, uAdd);
        pBuffer += uAdd;
        iLength -= uAdd;
        _CryptMD5Transform(pContext, pContext->strData);
    }

    while (iLength >= 64)
    {
        _CryptMD5Transform(pContext, pBuffer);
        pBuffer += 64;
        iLength -= 64;
    }

    if (iLength > 0)
    {
        memcpy(pContext->strData, pBuffer, iLength);
    }
}

void CryptMD5Final(CryptMD5T *pContext, void *_pBuffer, int32_t iLength)
{
    int32_t uIndex;
    uint32_t uZero;
    uint32_t *pZero;
    uint32_t uData = 0;
    unsigned char *pBuffer = _pBuffer;

    uIndex = pContext->uCount & 63;
    pContext->strData[uIndex++] = 0x80;

    if (uIndex > 56)
    {
        pContext->strData[uIndex+0] = 0;
        pContext->strData[uIndex+1] = 0;
        pContext->strData[uIndex+2] = 0;
        pContext->strData[uIndex+3] = 0;
        pContext->strData[uIndex+4] = 0;
        pContext->strData[uIndex+5] = 0;
        pContext->strData[uIndex+6] = 0;
        pContext->strData[uIndex+7] = 0;

        _CryptMD5Transform(pContext, pContext->strData);
        uIndex = 0;
    }

    pContext->strData[uIndex+0] = 0;
    pContext->strData[uIndex+1] = 0;
    pContext->strData[uIndex+2] = 0;

    uZero = (uIndex+3)>>2;
    pZero = ((uint32_t *)pContext->strData)+uZero;
    do
    {
        *pZero++ = 0;
    }
    while (++uZero < 64/4);

    pContext->strData[56] = (unsigned char)(pContext->uCount<<3);
    pContext->strData[57] = (unsigned char)(pContext->uCount>>5);
    pContext->strData[58] = (unsigned char)(pContext->uCount>>13);
    pContext->strData[59] = (unsigned char)(pContext->uCount>>21);
    pContext->strData[60] = (unsigned char)(pContext->uCount>>29);

    _CryptMD5Transform(pContext, pContext->strData);

#ifdef __linux__
    if (iLength == MD5_BINARY_OUT/2)
    {
        uData = pContext->uRegs[0];
        pBuffer[0] = uData;
        uData >>= 8;
        pBuffer[1] = uData;
        uData >>= 8;
        pBuffer[2] = uData;
        uData >>= 8;
        pBuffer[3] = uData;

        uData = pContext->uRegs[1];
        pBuffer[4] = uData;
        uData >>= 8;
        pBuffer[5] = uData;
        uData >>= 8;
        pBuffer[6] = uData;
        uData >>= 8;
        pBuffer[7] = uData;
        return;
    }

    if (iLength == MD5_BINARY_OUT)
    {
        uData = pContext->uRegs[0];
        pBuffer[0] = uData;
        uData >>= 8;
        pBuffer[1] = uData;
        uData >>= 8;
        pBuffer[2] = uData;
        uData >>= 8;
        pBuffer[3] = uData;

        uData = pContext->uRegs[1];
        pBuffer[4] = uData;
        uData >>= 8;
        pBuffer[5] = uData;
        uData >>= 8;
        pBuffer[6] = uData;
        uData >>= 8;
        pBuffer[7] = uData;

        uData = pContext->uRegs[2];
        pBuffer[8] = uData;
        uData >>= 8;
        pBuffer[9] = uData;
        uData >>= 8;
        pBuffer[10] = uData;
        uData >>= 8;
        pBuffer[11] = uData;

        uData = pContext->uRegs[3];
        pBuffer[12] = uData;
        uData >>= 8;
        pBuffer[13] = uData;
        uData >>= 8;
        pBuffer[14] = uData;
        uData >>= 8;
        pBuffer[15] = uData;
        return;
    }
#endif

    for (uIndex = 0; uIndex < 16; ++uIndex)
    {
        if ((uIndex & 3) == 0)
        {
            uData = pContext->uRegs[uIndex>>2];
        }
        if (iLength >= MD5_STRING_OUT)
        {
            *pBuffer++ = _MD5_HexChars[(uData>>4)&15];
            *pBuffer++ = _MD5_HexChars[(uData>>0)&15];
        }
        else if (uIndex < iLength)
        {
            *pBuffer++ = (unsigned char)uData;
        }
        uData >>= 8;
    }
    
    if (iLength >= MD5_STRING_OUT)
    {
        *pBuffer = 0;
    }
}

