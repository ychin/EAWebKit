/*
Copyright (C) 2005,2009-2010 Electronic Arts, Inc.  All rights reserved.

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dirtysock.h"


#if DIRTYCODE_DEBUG
int (*_Platform_pLogPrintf)(const char *pFmt, ...) = NetPrintfCode;
#endif

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/

#if DIRTYCODE_DEBUG
void NetPrintfVerboseCode(int32_t iVerbosityLevel, int32_t iCheckLevel, const char *pFormat, ...)
{
    va_list Args;
    char strText[1024];

    va_start(Args, pFormat);
    vsnzprintf(strText, sizeof(strText), pFormat, Args);
    va_end(Args);

    if (iVerbosityLevel > iCheckLevel)
    {
        NetPrintf(("%s", strText));
    }
}
#endif

#if DIRTYCODE_DEBUG
void NetPrintWrapCode(const char *pString, int32_t iWrapCol)
{
    const char *pTemp, *pEnd, *pEqual, *pSpace;
    char strTemp[256] = "   ";
    uint32_t bDone;
    int32_t iLen;

    for (bDone = FALSE; bDone == FALSE; )
    {
        for (pTemp=pString, pEnd=pTemp+iWrapCol, pSpace=NULL, pEqual=NULL; (pTemp < pEnd); pTemp++)
        {
            if ((*pTemp == ' ') || (*pTemp == '\t'))
            {
                pSpace = pTemp;
            }

            if (*pTemp == '=')
            {
                pEqual = pTemp;
            }

            if ((*pTemp == '\n') || (*pTemp == '\0'))
            {
                break;
            }
        }
        
        if (pTemp == pEnd)
        {
            if (pSpace != NULL)
            {
                pTemp = pSpace;
            }
            else if (pEqual != NULL)
            {
                pTemp = pEqual;
            }
        }

        iLen = pTemp - pString + 1;
        strncpy(strTemp + 3, pString, iLen);
        if (*pTemp == '\0')
        {
            strTemp[iLen+2] = '\n';
            strTemp[iLen+3] = '\0';
            bDone = TRUE;
        }
        else if ((*pTemp != '\n') && (*pTemp != '\r'))
        {
            strTemp[iLen+3] = '\n';
            strTemp[iLen+4] = '\0';
        }
        else
        {
            strTemp[iLen+3] = '\0';
        }

        NetPrintf(("%s", strTemp));

        pString += iLen;
    }
}
#endif // #if DIRTYCODE_DEBUG

#if DIRTYCODE_DEBUG
void NetPrintMemCode(const void *pMem, int32_t iSize, const char *pTitle)
{
    static const char _hex[] = "0123456789ABCDEF";
    char strOutput[128];
    int32_t iBytes, iOutput = 2;

    memset(strOutput, ' ', sizeof(strOutput)-1);
    strOutput[sizeof(strOutput)-1] = '\0';

    NetPrintf(("dirtylib: dumping memory for object %s (%d bytes)\n", pTitle, iSize));

    for (iBytes = 0; iBytes < iSize; iBytes++, iOutput += 2)
    {
        unsigned char cByte = ((unsigned char *)pMem)[iBytes];
        strOutput[iOutput]   = _hex[cByte>>4];
        strOutput[iOutput+1] = _hex[cByte&0xf];
        strOutput[(iOutput/2)+40] = isprint(cByte) ? cByte : '.';
        if (iBytes > 0)
        {
            if (((iBytes+1) % 16) == 0)
            {
                strOutput[(iOutput/2)+40+1] = '\0';
                NetPrintf(("%s\n", strOutput));
                memset(strOutput, ' ', sizeof(strOutput)-1);
                strOutput[sizeof(strOutput)-1] = '\0';
                iOutput = 0;
            }
            else if (((iBytes+1) % 4) == 0)
            {
                iOutput++;
            }
        }
    }

    if ((iBytes % 16) != 0)
    {
        strOutput[(iOutput/2)+40+1] = '\0';
        NetPrintf(("%s\n", strOutput));
    }
}
#endif
