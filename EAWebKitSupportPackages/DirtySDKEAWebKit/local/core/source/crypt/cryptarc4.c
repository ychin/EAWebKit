/*
Copyright (C) 2000-2005,2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
    \File cryptarc4.c

*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirtyplatform.h"
#include "cryptarc4.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

// Private variables

// Public variables


/*** Private Functions ************************************************************/

/*** Public Functions *************************************************************/


void CryptArc4Init(CryptArc4T *pState, const unsigned char *pKeyBuf, int32_t iKeyLen, int32_t iIter)
{
    uint32_t uWalk;
    unsigned char uSwap;
    unsigned char uTemp0;
    unsigned char uTemp1;

    if (iIter < 1)
    {
        iIter = 1;
    }

    pState->walk = 0;
    pState->swap = 0;

    for (uWalk = 0; uWalk < 256; ++uWalk)
    {
        pState->state[uWalk] = (unsigned char)uWalk;
    }

    if (iKeyLen > 0)
    {
        for (uSwap = 0; iIter > 0; --iIter)
        {
            for (uWalk = 0; uWalk < 256; ++uWalk)
            {
                uSwap += pState->state[uWalk] + pKeyBuf[uWalk % iKeyLen];
                uTemp0 = pState->state[uWalk];
                uTemp1 = pState->state[uSwap];
                pState->state[uWalk] = uTemp1;
                pState->state[uSwap] = uTemp0;
            }
        }
    }
}

void CryptArc4Apply(CryptArc4T *pState, unsigned char *pBuffer, int32_t iLength)
{
    uint32_t uTemp0;
    uint32_t uTemp1;
    uint32_t uWalk = pState->walk;
    uint32_t uSwap = pState->swap;

    for (; iLength > 0; --iLength)
    {
        uWalk = (uWalk+1)&255;
        uSwap = (uSwap+pState->state[uWalk])&255;

        uTemp0 = pState->state[uWalk];
        uTemp1 = pState->state[uSwap];
        pState->state[uWalk] = uTemp1;
        pState->state[uSwap] = uTemp0;

        *pBuffer++ ^= pState->state[(uTemp0+uTemp1)&255];
    }

    pState->walk = uWalk;
    pState->swap = uSwap;
}

void CryptArc4Advance(CryptArc4T *pState, int32_t iLength)
{
    uint32_t uTemp0;
    uint32_t uTemp1;
    uint32_t uWalk = pState->walk;
    uint32_t uSwap = pState->swap;

    for (; iLength > 0; --iLength)
    {
        uWalk = (uWalk+1)&255;
        uSwap = (uSwap+pState->state[uWalk])&255;

        uTemp0 = pState->state[uWalk];
        uTemp1 = pState->state[uSwap];
        pState->state[uWalk] = uTemp1;
        pState->state[uSwap] = uTemp0;
    }

    pState->walk = uWalk;
    pState->swap = uSwap;
}

