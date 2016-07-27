/*
Copyright (C) 2003,2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
    \File lobbybase64.c

*/
/*******************************************************************H*/

/*** Include files ***************************************************/

#include "dirtyplatform.h"
#include "lobbybase64.h"

/*** Defines *********************************************************/

/*** Type Definitions ************************************************/

/*** Variables *******************************************************/

static const char _LobbyBase64_strEncode[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const signed char _LobbyBase64_strDecode[] =
{
    62,                                                 // +
    -1, -1, -1,                                         // ,-.
    63,                                                 // /
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,             // 0-9
    -1, -1, -1, -1, -1, -1, -1,                         // :;<=>?@
    0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, // A-
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, //  -Z
    -1, -1, -1, -1, -1, -1,                             // [\]^_`
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // a-
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51  //  -z
};

/*** Public functions ************************************************/

void LobbyBase64Encode(int32_t iInputLen, const char *pInput, char *pOutput)
{
    int32_t o = 0;
    int32_t i = 0;
    const char *encode = _LobbyBase64_strEncode;

    while (iInputLen >= 3)
    {
        pOutput[o+0] = encode[(pInput[i]>>2)&0x3F];
        pOutput[o+1] = encode[((pInput[i]&0x3)<<4)|((pInput[i+1]>>4)&0x0F)];
        pOutput[o+2] = encode[((pInput[i+1]&0xF)<<2)|((pInput[i+2]>>6)&0x03)];
        pOutput[o+3] = encode[(pInput[i+2]&0x3F)];
        o += 4;
        i += 3;
        iInputLen -= 3;
    }
    switch (iInputLen)
    {
    case 1:
        pOutput[o+0] = encode[(pInput[i]>>2)&0x3F];
        pOutput[o+1] = encode[((pInput[i]&0x3)<<4)];
        pOutput[o+2] = '=';
        pOutput[o+3] = '=';
        o += 4;
        break;
    case 2:
        pOutput[o+0] = encode[(pInput[i]>>2)&0x3F];
        pOutput[o+1] = encode[((pInput[i]&0x3)<<4)|((pInput[i+1]>>4)&0x0F)];
        pOutput[o+2] = encode[((pInput[i+1]&0xF)<<2)];
        pOutput[o+3] = '=';
        o += 4;
        break;
    }
    pOutput[o] = '\0';
}


int32_t LobbyBase64Decode(int32_t iInputLen, const char *pInput, char *pOutput)
{
    char ci[4];
    char co0, co1, co2, co3;
    int32_t iInputCnt, iInputOff, iOutputOff;
    const signed char *decode = _LobbyBase64_strDecode;

    for (iInputOff = 0, iOutputOff = 0; iInputOff < iInputLen; )
    {
        for (iInputCnt = 0; (iInputCnt < 4) && (iInputOff < iInputLen) && (pInput[iInputOff] != '\0'); iInputOff += 1)
        {
            if ((pInput[iInputOff] == ' ') || (pInput[iInputOff] == '\t') || (pInput[iInputOff] == '\r') || (pInput[iInputOff] == '\n'))
            {
                continue;
            }
            else if ((pInput[iInputOff] < '+') || (pInput[iInputOff] > 'z'))
            {
                return(0);
            }
            else
            {                
                ci[iInputCnt++] = pInput[iInputOff];
            }
        }
        if (iInputCnt == 0)
        {
            break;
        }
        if (iInputCnt < 4)
        {
            return(0);
        }
        co0 = decode[(int32_t)ci[0]-'+'];
        co1 = decode[(int32_t)ci[1]-'+'];
        co2 = decode[(int32_t)ci[2]-'+'];
        co3 = decode[(int32_t)ci[3]-'+'];
        
        if ((co0 >= 0) && (co1 >= 0))
        {
            if ((co2 >= 0) && (co3 >= 0))
            {
                if (pOutput != NULL)
                {
                    pOutput[iOutputOff+0] = (co0<<2)|((co1>>4)&0x3);
                    pOutput[iOutputOff+1] = (co1&0x3f)<<4|((co2>>2)&0x3F);
                    pOutput[iOutputOff+2] = ((co2&0x3)<<6)|co3;
                }
                iOutputOff += 3;
            }
            else if ((co2 >= 0) && (ci[3] == '='))
            {
                if (pOutput != NULL)
                {
                    pOutput[iOutputOff+0] = (co0<<2)|((co1>>4)&0x3);
                    pOutput[iOutputOff+1] = (co1&0x3f)<<4|((co2>>2)&0x3F);
                }
                iOutputOff += 2;
                iInputOff = iInputLen;
            }
            else if ((ci[2] == '=') && (ci[3] == '='))
            {
                if (pOutput != NULL)
                {
                    pOutput[iOutputOff+0] = (co0<<2)|((co1>>4)&0x3);
                }
                iOutputOff += 1;
                iInputOff = iInputLen;
            }
            else
            {
                return(0);
            }
        }
        else
        {
            return(0);
        }
    }
    if (pOutput == NULL)
    {
        return((iInputOff == iInputLen) ? iOutputOff : 0);
    }
    else
    {
        return(iInputOff == iInputLen);
    }
}
