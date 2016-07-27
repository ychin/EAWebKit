/*
Copyright (C) 2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

/*H*************************************************************************************/
/*!
    \File cryptmd2.c

*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <string.h>
#include "dirtyplatform.h"
#include "dirtylib.h"
#include "cryptmd2.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

static uint8_t _CryptMD2_aPiSubst[256] =
{
     41,  46,  67, 201, 162, 216, 124,   1,  61,  54,  84, 161, 236, 240,   6,  19,
     98, 167,   5, 243, 192, 199, 115, 140, 152, 147,  43, 217, 188,  76, 130, 202,
     30, 155,  87,  60, 253, 212, 224,  22, 103,  66, 111, 24,  138,  23, 229,  18,
    190,  78, 196, 214, 218, 158, 222,  73, 160, 251, 245, 142, 187,  47, 238, 122,
    169, 104, 121, 145,  21, 178,   7,  63, 148, 194,  16, 137,  11,  34,  95,  33,
    128, 127,  93, 154,  90, 144,  50,  39,  53,  62, 204, 231, 191, 247, 151,   3,
    255,  25,  48, 179,  72, 165, 181, 209, 215,  94, 146,  42, 172,  86, 170, 198,
     79, 184,  56, 210, 150, 164, 125, 182, 118, 252, 107, 226, 156, 116,   4, 241,
     69, 157, 112,  89, 100, 113, 135,  32, 134,  91, 207, 101, 230,  45, 168,   2,
     27,  96,  37, 173, 174, 176, 185, 246,  28,  70,  97, 105,  52,  64, 126,  15,
     85,  71, 163,  35, 221,  81, 175,  58, 195,  92, 249, 206, 186, 197, 234,  38,
     44,  83,  13, 110, 133,  40, 132,   9, 211, 223, 205, 244, 65,  129,  77,  82,
    106, 220,  55, 200, 108, 193, 171, 250,  36, 225, 123,   8,  12, 189, 177,  74,
    120, 136, 149, 139, 227,  99, 232, 109, 233, 203, 213, 254,  59,   0,  29,  57,
    242, 239, 183,  14, 102,  88, 208, 228, 166, 119, 114, 248, 235, 117,  75,  10,
     49,  68,  80, 180, 143, 237,  31,  26, 219, 153, 141,  51, 159,  17, 131,  20
};

static uint8_t *_CryptMD2_aPadding[] =
{
    "",
    "\001",
    "\002\002",
    "\003\003\003",
    "\004\004\004\004",
    "\005\005\005\005\005",
    "\006\006\006\006\006\006",
    "\007\007\007\007\007\007\007",
    "\010\010\010\010\010\010\010\010",
    "\011\011\011\011\011\011\011\011\011",
    "\012\012\012\012\012\012\012\012\012\012",
    "\013\013\013\013\013\013\013\013\013\013\013",
    "\014\014\014\014\014\014\014\014\014\014\014\014",
    "\015\015\015\015\015\015\015\015\015\015\015\015\015",
    "\016\016\016\016\016\016\016\016\016\016\016\016\016\016",
    "\017\017\017\017\017\017\017\017\017\017\017\017\017\017\017",
    "\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020"
};




/*** Private Functions *****************************************************************/


static void _CryptMD2Transform(CryptMD2T *pContext, const uint8_t *pInput)
{
    uint8_t aCryptBlock[48];
    int32_t iBlock, iIter, iTemp;

    memcpy(&aCryptBlock[0 ], pContext->aState, 16);
    memcpy(&aCryptBlock[16], pInput, 16);
    for (iBlock = 0; iBlock < 16; iBlock += 1)
    {
        aCryptBlock[iBlock+32] = pContext->aState[iBlock] ^ pInput[iBlock];
    }

    for (iIter = 0, iTemp = 0; iIter < 18; iIter += 1)
    {
        for (iBlock = 0; iBlock < 48; iBlock += 1)
        {
            aCryptBlock[iBlock] ^= _CryptMD2_aPiSubst[iTemp];
            iTemp = aCryptBlock[iBlock];
        }
        iTemp = (iTemp + iIter) & 0xff;
    }

    memcpy(pContext->aState, aCryptBlock, sizeof(pContext->aState));

    for (iBlock = 0, iTemp = pContext->aChecksum[15]; iBlock < 16; iBlock += 1)
    {
        iTemp = pContext->aChecksum[iBlock] ^= _CryptMD2_aPiSubst[pInput[iBlock] ^ iTemp];
    }
    
    memset(aCryptBlock, 0, sizeof(aCryptBlock));
}

/*** Public Functions ******************************************************************/


void CryptMD2Init(CryptMD2T *pContext)
{
    memset(pContext, 0, sizeof(*pContext));
}

void CryptMD2Update(CryptMD2T *pContext, const void *_pBuffer, int32_t iLength)
{
    const uint8_t *pBuffer = _pBuffer;
    int32_t iCount, iIndex, iPartLen;
    
    if (iLength < 0)
    {
        for (iLength = 0; pBuffer[iLength] != 0; ++iLength)
            ;
    }
    
    iCount = pContext->iCount;
    pContext->iCount = (iCount + iLength) & 0xf;
    iPartLen = 16 - iCount;
    
    if (iLength >= iPartLen)
    {
        memcpy(&pContext->aBuffer[iCount], pBuffer, iPartLen);
        _CryptMD2Transform(pContext, pContext->aBuffer);
        for (iIndex = iPartLen; iIndex + 15 < iLength; iIndex += 16)
        {
            _CryptMD2Transform(pContext, pBuffer + iIndex);
        }
        iCount = 0;
    }
    else
    {
        iIndex = 0;
    }
    
    memcpy(&pContext->aBuffer[iCount], &pBuffer[iIndex], iLength - iIndex);
}

void CryptMD2Final(CryptMD2T *pContext, void *_pBuffer, int32_t iLength)
{
    uint8_t *pBuffer = _pBuffer;
    int32_t iPadLen;
    
    if (iLength < MD2_BINARY_OUT)
    {
        NetPrintf(("cryptmd2: insufficient buffer for digest output\n"));
        return;
    }
    
    iPadLen = 16 - pContext->iCount;
    CryptMD2Update(pContext, _CryptMD2_aPadding[iPadLen], iPadLen);
    
    CryptMD2Update(pContext, pContext->aChecksum, 16);
    
    memcpy(pBuffer, pContext->aState, MD2_BINARY_OUT);
    
    memset(pContext, 0, sizeof(pContext));
}
