/*
Copyright (C) 2002,2009-2010 Electronic Arts, Inc.  All rights reserved.

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

    \File    cryptrsa.c

*/
/*************************************************************************************************H*/


/*** Include files *********************************************************************/

#include <string.h>             
#include "dirtysock.h"
#include "cryptrsa.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables


// Public variables


/*** Private functions *******************************************************/


static int32_t _Addition(uint16_t *pResult, int32_t iWidth, uint16_t *pAdd1, uint16_t *pAdd2)
{
    register uint32_t uAccum;

    pAdd1 += iWidth;
    pAdd2 += iWidth;
    pResult += iWidth;

    for (uAccum = 0; iWidth > 0; --iWidth)
    {
        uAccum += (*--pAdd1 + *--pAdd2);
        *--pResult = (uint16_t)uAccum;
        uAccum >>= 16;
    }

    return(uAccum);
}

static int32_t _Subtract(uint16_t *pResult, int32_t iWidth, uint16_t *pSub1, uint16_t *pSub2)
{
    register uint32_t uAccum;

    pSub1 += iWidth;
    pSub2 += iWidth;
    pResult += iWidth;

    for (uAccum = 0; iWidth > 0; --iWidth)
    {
        uAccum = (*--pSub1 - *--pSub2) - uAccum;
        *--pResult = (uint16_t)uAccum;
        uAccum = (uAccum >> 16) & 1;
    }

    return(uAccum);
}

static int32_t _BitTest(uint16_t *pValue, int32_t iWidth, int32_t iBit)
{
    int32_t iStep = sizeof(*pValue) * 8;
    int32_t iWeight = (iBit % iStep);
    int32_t iOffset = (iStep * iWidth - iBit - 1) / iStep;
    uint16_t uResult;

    uResult = (uint16_t)(pValue[iOffset] & (1 << iWeight));
    return(uResult);
}

static void _Multiply(uint16_t *pResult, int32_t iWidth, uint16_t *pMul1, uint16_t *pMul2, uint16_t *pMod)
{
    int32_t iCount;
    int32_t iCarry;
    uint16_t Temp1[4096/16];
    uint16_t Temp2[4096/16];
    uint16_t *pCur = Temp1;
    uint16_t *pAlt = Temp2;

    memset(pCur, 0, iWidth * sizeof(*pResult));

    for (iCount = iWidth*sizeof(*pResult)*8-1; iCount >= 0; --iCount)
    {
        iCarry = _Addition(pCur, iWidth, pCur, pCur); 
        if (!_Subtract(pAlt, iWidth, pCur, pMod) || iCarry)
        {
            uint16_t *pSwap = pCur;
            pCur = pAlt;
            pAlt = pSwap;
        }

        if (_BitTest(pMul1, iWidth, iCount))
        {
            iCarry = _Addition(pCur, iWidth, pCur, pMul2);
            if (!_Subtract(pAlt, iWidth, pCur, pMod) || iCarry)
            {
                uint16_t *pSwap = pCur;
                pCur = pAlt;
                pAlt = pSwap;
            }
        }
    }

    memcpy(pResult, pCur, iWidth * sizeof(*pResult));
}

static int32_t _ToWords(uint16_t *pResult, int32_t iWidth, const uint8_t *pSource, int32_t iLength)
{
    int32_t iFullWordCount = iLength/2;
    int32_t iWordCount = (iLength+1)/2;

    if (iWidth < 0)
    {
        iWidth = iWordCount;
    }

    for (; iWidth > iWordCount; --iWidth)
    {
        *pResult++ = 0;
    }
    if (iFullWordCount != iWordCount)
    {
        *pResult++ = pSource[0];
        pSource += 1;
        iWidth -= 1;
    }
    for (; iWidth > 0; --iWidth)
    {
        *pResult++ = (pSource[0]<<8)|(pSource[1]<<0);
        pSource += 2;
    }
    return(iWordCount);
}

static void _FromWords(const uint16_t *pSource, int32_t iWidth, uint8_t *pResult, int32_t iLength)
{
    iLength /= 2;

    pSource += (iWidth - iLength);

    for (; iLength > 0; --iLength)
    {
        *pResult++ = (uint8_t)(*pSource>>8);
        *pResult++ = (uint8_t)(*pSource>>0);
        pSource += 1;
    }
}

static void _Exponentiate(uint8_t *pResult, uint8_t *pPowerof, const uint8_t *pModulus, int32_t iModSize,  const uint8_t *pExponent, int32_t iExpSize)
{
    int32_t iIndex;
    int32_t iWidth;
    uint32_t uExponent;
    uint16_t Modulus[4096/16];
    uint16_t Powerof[4096/16];
    uint16_t Accumul[4096/16];

    iWidth = _ToWords(Modulus, -1, pModulus, iModSize);
    
    _ToWords(Powerof, -1, pPowerof, iModSize);

    uExponent = 0;
    for (iIndex = 0; iIndex < iExpSize; ++iIndex)
    {
        uExponent = (uExponent << 8) | pExponent[iIndex];
    }

    if (uExponent == 3)
    {
        _Multiply(Accumul, iWidth, Powerof, Powerof, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Powerof, Modulus);  
    }
    else if (uExponent == 17)
    {
        _Multiply(Accumul, iWidth, Powerof, Powerof, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Powerof, Modulus);  
    }
    else if (uExponent == 65537)
    {
        _Multiply(Accumul, iWidth, Powerof, Powerof, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Accumul, Modulus);  
        _Multiply(Accumul, iWidth, Accumul, Powerof, Modulus);  
    }
    else
    {
        NetPrintf(("cryptrsa: warning; executing general case code, which is likely to be slow\n"));
        
        memset(Accumul, 0, sizeof(Accumul));
        Accumul[iWidth-1] = 1;

        for (; uExponent != 0; uExponent >>= 1)
        {
            if (uExponent & 1)
            {
                _Multiply(Accumul, iWidth, Accumul, Powerof, Modulus);
            }
            _Multiply(Powerof, iWidth, Powerof, Powerof, Modulus);
        }
    }

    _FromWords(Accumul, iWidth, pResult, iModSize);
}


/*** Public functions ********************************************************/


void CryptRSAInit(CryptRSAT *pState, const uint8_t *pModulus, int32_t iModSize, const uint8_t *pExponent, int32_t iExpSize)
{
    const int32_t iMaxModSize = sizeof(pState->KeyModData)-1;
    const int32_t iMaxExpSize = sizeof(pState->KeyExpData)-1;

    if (iModSize > iMaxModSize)
    {
        NetPrintf(("cryptrsa: requested iModSize of %d is too large; truncating to %d\n", iModSize, iMaxModSize));
        iModSize = iMaxModSize;
    }
    if (iExpSize > iMaxExpSize)
    {
        NetPrintf(("cryptrsa: requested iExpSize of %d is too large; truncating to %d\n", iExpSize, iMaxExpSize));
        iExpSize = iMaxExpSize;
    }

    pState->iKeyModSize = iModSize;
    pState->iKeyExpSize = iExpSize;
    memcpy(pState->KeyModData, pModulus, iModSize);
    memcpy(pState->KeyExpData, pExponent, iExpSize);
}

void CryptRSAInitMaster(CryptRSAT *pState, const uint8_t *pMaster, int32_t iMasterLen)
{
    int32_t iIndex;
    uint32_t uRandom = NetTick();

    for (iIndex = 0; iIndex < pState->iKeyModSize; ++iIndex)
    {
        pState->EncryptBlock[iIndex] = ((uRandom & (1<<(iIndex&31))) != 0);
    }

    for (iIndex = 0; iIndex < pState->iKeyModSize; ++iIndex)
    {
        do
        {
            uRandom = (uRandom * 69069) + 69069;
            pState->EncryptBlock[iIndex] ^= (char)uRandom;
        }
        while (pState->EncryptBlock[iIndex] == 0);
    }

    pState->EncryptBlock[0] = 0;        
    pState->EncryptBlock[1] = 2;        
    iIndex = pState->iKeyModSize - iMasterLen;
    pState->EncryptBlock[iIndex-1] = 0; 
    memcpy(pState->EncryptBlock+iIndex, pMaster, iMasterLen);
}

void CryptRSAInitSignature(CryptRSAT *pState, const uint8_t *pSig, int32_t iSigLen)
{
    memcpy(pState->EncryptBlock, pSig, iSigLen);
}

void CryptRSAEncrypt(CryptRSAT *pState)
{
    _Exponentiate(pState->EncryptBlock, pState->EncryptBlock,
                  pState->KeyModData, pState->iKeyModSize,
                  pState->KeyExpData, pState->iKeyExpSize);
}


