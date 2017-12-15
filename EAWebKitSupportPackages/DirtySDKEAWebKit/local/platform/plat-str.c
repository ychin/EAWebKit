/*
Copyright (C) 2005,2009-2010, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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


/*** Include files ****************************************************************/

#include <stdarg.h>     
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      

#undef DS_PLATFORM
#include "dirtyplatform.h"

/*** Defines **********************************************************************/

#define snprintf _snprintf
#define vsnprintf _vsnprintf

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Public functions *************************************************************/

int32_t ds_vsnzprintf(char *pBuffer, int32_t iLength, const char *pFormat, va_list Args)
{
    int32_t iResult;

    if (iLength <= 0) return(0);

    iResult = vsnprintf(pBuffer, iLength, pFormat, Args);
    if (iResult >= iLength)
    {
        iResult = 0;
    }
    else if (iResult < 0)
    {
        iResult = 0;
    }
    pBuffer[iResult] = '\0';
    return(iResult);
}

int32_t ds_snzprintf(char *pBuffer, int32_t iLength, const char *pFormat, ...)
{
    int32_t iResult;
    va_list args;

    if (iLength <= 0) return(0);
    
    va_start(args, pFormat);
    iResult = vsnprintf(pBuffer, iLength, pFormat, args);
    va_end(args);
    if (iResult >= iLength)
    {
        iResult = 0;
    }
    else if (iResult < 0)
    {
        iResult = 0;
    }
    pBuffer[iResult] = '\0';
    return(iResult);
}

char *ds_stristr(const char *pHaystack, const char *pNeedle)
{
    int32_t iFirst;
    int32_t iIndex;

    if ((pHaystack != NULL) && (*pHaystack != 0) && (pNeedle != NULL) && (*pNeedle != 0))
    {
        iFirst = tolower((unsigned char)*pNeedle);
        for (; *pHaystack != 0; ++pHaystack)
        {
            if (tolower((unsigned char)*pHaystack) == iFirst)
            {
                for (iIndex = 1;; ++iIndex)
                {
                    if (pNeedle[iIndex] == 0)
                    {
                        return((char *)pHaystack);
                    }
                    if (pHaystack[iIndex] == 0)
                    {
                        break;
                    }
                    if (tolower((unsigned char)pHaystack[iIndex]) != tolower((unsigned char)pNeedle[iIndex]))
                    {
                        break;
                    }
                }
            }
        }
    }
    return(NULL);
}

int32_t ds_stricmp(const char *pString1, const char *pString2)
{
    int32_t r;
    char c1, c2;

    do {
        c1 = *pString1++;
        if ((c1 >= 'A') && (c1 <= 'Z'))
            c1 ^= 32;
        c2 = *pString2++;
        if ((c2 >= 'A') && (c2 <= 'Z'))
            c2 ^= 32;
        r = c1-c2;
    } while ((c1 != 0) && (r == 0));

    return(r);
}

int32_t ds_strnicmp(const char *pString1, const char *pString2, uint32_t uCount)
{
    int32_t r;
    char c1, c2;
    uint32_t uPos;

    if (uCount == 0)
        return (0);

    uPos = 0;
    do {
        c1 = *pString1++;
        if ((c1 >= 'A') && (c1 <= 'Z'))
            c1 ^= 32;
        c2 = *pString2++;
        if ((c2 >= 'A') && (c2 <= 'Z'))
            c2 ^= 32;
        r = c1-c2;
        uPos++;
    } while ((c1 != 0) && (r == 0) && (uPos < uCount));

    return(r);
}

char *ds_strnzcpy(char *pDest, const char *pSource, int32_t iCount)
{
    strncpy(pDest, pSource, iCount);
    pDest[iCount-1] = 0;
    return(pDest);
}

int32_t ds_strsubzcpy(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen)
{
    int32_t iIndex;
    
    if (--iDstLen < 0)
    {
        return(0);
    }
    
    for (iIndex = 0; (iIndex < iSrcLen) && (iIndex < iDstLen) && (pSrc[iIndex] != '\0'); iIndex++)
    {
        pDst[iIndex] = pSrc[iIndex];
    }
    
    pDst[iIndex] = '\0';
    return(iIndex);
}

int32_t ds_strnzcat(char *pDst, const char *pSrc, int32_t iDstLen)
{
    int32_t iDst, iSrc;

    if (--iDstLen < 0)
    {
        return(0);
    }
    
    for (iDst = 0; (iDst < iDstLen) && (pDst[iDst] != '\0'); iDst++)
        ;
        
    for (iSrc = 0; (iDst < iDstLen) && (pSrc[iSrc] != '\0'); iSrc++, iDst++)
    {
        pDst[iDst] = pSrc[iSrc];
    }
    
    pDst[iDst] = '\0';
    return(iDst);
}

int32_t ds_strsubzcat(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen)
{
    int32_t iDst, iSrc;

    if (--iDstLen < 0)
    {
        return(0);
    }
    
    for (iDst = 0; (iDst < iDstLen) && (pDst[iDst] != '\0'); iDst++)
        ;
        
    for (iSrc = 0; (iDst < iDstLen) && (iSrc < iSrcLen) && (pSrc[iSrc] != '\0'); iSrc++, iDst++)
    {
        pDst[iDst] = pSrc[iSrc];
    }
    
    pDst[iDst] = '\0';
    return(iDst);

}
