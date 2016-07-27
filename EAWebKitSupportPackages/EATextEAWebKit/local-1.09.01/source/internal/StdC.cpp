/*
Copyright (C) 2007,2009-2010, 2013 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// StdC.cpp
// By Paul Pedriana - 2007
///////////////////////////////////////////////////////////////////////////////



#include <EAText/internal/StdC.h>
#include <string.h>
#include <errno.h>
#include <math.h>


namespace EA
{
namespace Text
{

size_t Strlcpy(char8_t* pDestination, const char8_t* pSource, size_t nDestCapacity)
{
    const size_t n = Strlen(pSource);

    if(n < nDestCapacity)
        memcpy(pDestination, pSource, (n + 1) * sizeof(*pSource));
    else
    {
        if(nDestCapacity)
        {
            memcpy(pDestination, pSource, (nDestCapacity - 1) * sizeof(*pSource));
            if(nDestCapacity)
                pDestination[nDestCapacity - 1] = 0;
        }
    }

    return n;
}

size_t Strlcpy(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity)
{
    const size_t n = Strlen(pSource);

    if(n < nDestCapacity)
        memcpy(pDestination, pSource, (n + 1) * sizeof(*pSource));
    else
    {
        if(nDestCapacity)
        {
            memcpy(pDestination, pSource, (nDestCapacity - 1) * sizeof(*pSource));
            if(nDestCapacity)
                pDestination[nDestCapacity - 1] = 0;
        }
    }

    return n;
}

int Strlcpy(char16_t* pDest, const char8_t*  pSource, size_t nDestCapacity, size_t nSourceLength)
{
    // Normally this function does a UTF8 to UTF16 conversion, but our version here is dumb.
    // See the EAStdC package for a proper implementation.
    size_t destCount = 0;

    while(nSourceLength-- > 0)
    {
        const uint32_t c = (uint8_t)*pSource++;

        if(c == 0) // Break on NULL char, even if explicit length was set
            break;

        if(pDest && ((destCount + 1) < nDestCapacity))
            *pDest++ = static_cast<char16_t>(c);

        destCount++;
    }

    if(pDest && nDestCapacity)
        *pDest = 0;

    return (int)(unsigned)destCount;
}

inline char8_t Tolower(char8_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char8_t)(c + ('a' - 'A'));
    return c;
}

inline char16_t Tolower(char16_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char16_t)(c + ('a' - 'A'));
    return c;
}

char16_t* Strlwr(char16_t* pString)
{
    char16_t* pStringTemp = pString;

    while(*pStringTemp)
    {
        const char16_t c = *pStringTemp;
        *pStringTemp++ = Tolower(c);
    }

    return pString;
}

int Stricmp(const char8_t* pString1, const char8_t* pString2)
{
    char8_t c1, c2;

    // PowerPC has lhzu and sthu instructions which make pre-increment 
    // updates faster for the PowerPC's load and store architecture.
    #ifdef EA_PROCESSOR_POWERPC
        --pString1;
        --pString2;
        while((c1 = Tolower(*++pString1)) == (c2 = Tolower(*++pString2)))
        {
            if(c1 == 0)
                return 0;
        }
    #else
        while((c1 = Tolower(*pString1++)) == (c2 = Tolower(*pString2++)))
        {
            if(c1 == 0)
                return 0;
        }
    #endif

    return (c1 - c2);
}

int Stricmp(const char16_t* pString1, const char16_t* pString2)
{
    char16_t c1, c2;

    // PowerPC has lhzu and sthu instructions which make pre-increment 
    // updates faster for the PowerPC's load and store architecture.
    #ifdef EA_PROCESSOR_POWERPC
        --pString1;
        --pString2;
        while((c1 = Tolower(*++pString1)) == (c2 = Tolower(*++pString2)))
        {
            if(c1 == 0)
                return 0;
        }
    #else
        while((c1 = Tolower(*pString1++)) == (c2 = Tolower(*pString2++)))
        {
            if(c1 == 0)
                return 0;
        }
    #endif

    return (c1 - c2);
}

int Strnicmp(const char8_t* pString1, const char8_t* pString2, size_t n)
{
    char8_t c1, c2;

    // PowerPC has lhzu and sthu instructions which make pre-increment 
    // updates faster for the PowerPC's load and store architecture.
    #ifdef EA_PROCESSOR_POWERPC
        --pString1;
        --pString2;
        ++n;
        while(--n)
        {
            if((c1 = Tolower(*++pString1)) != (c2 = Tolower(*++pString2)))
                return (c1 - c2);
            else if(c1 == 0)
                break;
        }
    #else
        ++n;
        while(--n)
        {
            if((c1 = Tolower(*pString1++)) != (c2 = Tolower(*pString2++)))
                return (c1 - c2);
            else if(c1 == 0)
                break;
        }
    #endif
        
    return 0;
}

char8_t* Stristr(const char8_t* s1, const char8_t* s2)
{
    const char8_t* cp = s1;

    if(!*s2)
        return (char8_t*)s1;

    while(*cp)
    {
        const char8_t* s = cp;
        const char8_t* t = s2;

        while(*s && *t && (Tolower(*s) == Tolower(*t)))
            ++s, ++t;

        if(*t == 0)
            return (char8_t*)cp;
        ++cp;
    }

    return 0;
}


inline bool Isdigit(char16_t c)
{
    return (((unsigned)c - (unsigned)'0') < 10u);
}

inline char16_t Toupper(char16_t c)
{
    if(((unsigned)c - (unsigned)'a') <= (unsigned)('z' - 'a')) // If the letter is lower case
        c -= (char16_t)('a' - 'A');
    return c;
}

static uint64_t StrtoU64Common(const char16_t* pValue, char16_t** ppEnd, int nBase, bool bUnsigned)
{
    uint64_t        nValue(0);                 // Current value
    const char16_t* p = pValue;                // Current position
    char16_t        c;                         // Temp value
    char16_t        chSign('+');               // One of either '+' or '-'
    bool            bDigitWasRead(false);      // True if any digits were read.
    bool            bOverflowOccurred(false);  // True if integer overflow occurred.

    // Skip leading whitespace
    c = *p++;
    while(Isspace(c))
        c = *p++;

    // Check for sign.
    if((c == '-') || (c == '+')){
        chSign = c;
        c = *p++;
    }

    // Do checks on nBase.
    if((nBase < 0) || (nBase == 1) || (nBase > 36)){
        if(ppEnd)
            *ppEnd = (char16_t*)pValue;
        return 0;
    }
    else if(nBase == 0){
        // Auto detect one of base 8, 10, or 16. 
        if(c != '0')
            nBase = 10;
        else if(*p == 'x' || *p == 'X')
            nBase = 16;
        else
            nBase = 8;
    }
    if(nBase == 16){
        // If there is a leading '0x', then skip past it.
        if((c == '0') && ((*p == 'x') || (*p == 'X'))) {
            ++p;
            c = *p++;
        }
    }

    // If nValue exceeds this, an integer overflow will occur.
    const uint64_t nMaxValue(UINT64_MAX / nBase);
    const uint64_t nModValue(UINT64_MAX % nBase);


    for(unsigned nCurrentDigit; ;){
        if(Isdigit(c))
            nCurrentDigit = (unsigned)(c - '0');
        else if(Isalpha(c))
            nCurrentDigit = (unsigned)(Toupper(c) - 'A' + 10);
        else
            break; // The digit is invalid.

        if(nCurrentDigit >= (unsigned)nBase)
            break; // The digit is invalid.

        bDigitWasRead = true;

        // Check for overflow.
        if((nValue < nMaxValue) || ((nValue == nMaxValue) && ((uint64_t)nCurrentDigit <= nModValue)))
            nValue = (nValue * nBase) + nCurrentDigit;
        else
            bOverflowOccurred = true; // Set the flag, but continue processing.

        c = *p++;
    }

    --p; // Go back to the last character

    if(!bDigitWasRead){
        if(ppEnd)
            p = pValue; // We'll assign 'ppEnd' below.
    }                                                                                  // INT64_MAX + 1 is the same thing as -INT64_MIN with most compilers.
    else if(bOverflowOccurred || (!bUnsigned && (((chSign == '-') && (nValue > ((uint64_t)INT64_MAX + 1))) || ((chSign == '+') && (nValue > (uint64_t)INT64_MAX))))){
        // Integer overflow occurred.
        if(bUnsigned)
            nValue = UINT64_MAX;
        else if(chSign == '-')
            nValue = (uint64_t)INT64_MAX + 1; // INT64_MAX + 1 is the same thing as -INT64_MIN with most compilers.
        else
            nValue = INT64_MAX;
        errno = ERANGE; // The standard specifies that we set this value.
    }

    if(ppEnd)
        *ppEnd = (char16_t*)p;

    if(chSign == '-')
        nValue = (uint64_t)(-(int64_t)nValue);

    return nValue;
}


int32_t StrtoI32(const char16_t* pValue, char16_t** ppEnd, int nBase)
{
    int64_t val = (int64_t) StrtoU64Common(pValue, ppEnd, nBase, false);

    if(val < INT32_MIN)
    {
        return (int32_t)INT32_MIN;
    }

    if(val > INT32_MAX)
    {
        return INT32_MAX;
    }

    return (int32_t) val;
}

uint32_t StrtoU32(const char16_t* pValue, char16_t** ppEnd, int nBase)
{
    uint64_t val = StrtoU64Common(pValue, ppEnd, nBase, true);

    if(val > UINT32_MAX)
    {
        return UINT32_MAX;
    }

    return (uint32_t)val;
}

const char8_t* GetTextLine(const char8_t* pText, const char8_t* pTextEnd, const char8_t** ppNewText)
{
    if(pText < pTextEnd)
    {
        while((pText < pTextEnd) && (*pText != '\r') && (*pText != '\n'))
            ++pText;

        if(ppNewText)
        {
            *ppNewText = pText;

            if(*ppNewText < pTextEnd)
            {
                if((++*ppNewText < pTextEnd) && (**ppNewText ^ *pText) == ('\r' ^ '\n'))
                    ++*ppNewText;
            }
        }
    }
    else if(ppNewText)
        *ppNewText = pTextEnd;

    return pText;
}

bool SplitTokenDelimited(const char16_t* pSource, size_t nSourceLength, char16_t cDelimiter, 
                         char16_t* pToken, size_t nTokenLength, const char16_t** ppNewSource)
{
    // terminate the token ( so it appears empty if we don't find anything )
    if ( pToken && nTokenLength )
        *pToken = 0;

    if ( pSource && nSourceLength && *pSource )
    {
        // look for the delimiter
        for ( size_t i = 0; i < nSourceLength && *pSource; i++ )
        {
            const char16_t cTemp( *pSource );

            // update new source pointer if present
            if ( ppNewSource )
                (*ppNewSource)++;

            if ( cTemp == cDelimiter ) // If there is a delimiter match...
                break; // We are done.
            else
            {
                // keep moving characters into the token until we find the delimiter or reached the end of the token string
                if ( pToken && ( ( i + 1 ) < nTokenLength ) ) // we need an extra character for terminating null
                {
                    *pToken = cTemp;    // add the character
                     pToken++;          // increment the token pointer
                    *pToken = 0;        // insert terminating null character
                }

                pSource++; // increment source pointer
            }
        }

        return true;
    }

    return false;
}

bool SplitTokenSeparated(const char8_t* pSource, size_t nSourceLength, char8_t c, 
                               char8_t* pToken,  size_t nTokenLength, const char8_t** ppNewSource)
{
    if ( pToken && nTokenLength )
        *pToken = '\0';

    if ( pSource )
    {
        size_t nTokenIndex = 0;
        bool   bFoundToken = false;
        bool   bReadToken  = false;

        for ( size_t i = 0; i < nSourceLength; i++ )
        {
            const char8_t cTemp(*pSource);

            if ( cTemp != '\0' )
            {
                if ( cTemp != c )
                {
                    bFoundToken = true;

                    if ( bReadToken )
                        return true;
                    else
                    {
                        if ( pToken && (( nTokenIndex + 1 ) < nTokenLength) )
                        {
                            *pToken++ = cTemp;
                            nTokenIndex++;
                            *pToken = '\0';
                        }
                    }
                }
                else
                {
                    if ( bFoundToken )
                        bReadToken = true;
                }

                if ( ppNewSource )
                    (*ppNewSource)++;

                pSource++;
            }
            else
                break;
        }

        return bFoundToken;
    }

    return false;
}

uint32_t FNV1(const void* pData, size_t nLength, uint32_t nInitialValue)
{
    const uint8_t* pData8 = (const uint8_t*)pData;
    const uint8_t* const pData8End = pData8 + nLength;

    while(pData8 < pData8End)
        nInitialValue = (nInitialValue * 16777619) ^ *pData8++;

    return nInitialValue;
}

} // namespace Text

} // namespace EA







