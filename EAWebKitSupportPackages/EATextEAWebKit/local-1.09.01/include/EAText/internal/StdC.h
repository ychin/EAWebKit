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
// StdC.h
// By Paul Pedriana - 2007
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERAL_STDC_H
#define EATEXT_INTERAL_STDC_H


#include <EABase/eabase.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <EAASSERT/eaassert.h>




namespace EA
{
    namespace Text
    {
        size_t Strlen(const char8_t* pString);
        size_t Strlen(const char16_t* pString);

        char8_t*  Strcpy(char8_t* pDestination, const char8_t* pSource);
        char16_t* Strcpy(char16_t* pDestination, const char16_t* pSource);

        char8_t*  Strncpy(char8_t*  pDestination, const char8_t*  pSource, size_t n);
        char16_t* Strncpy(char16_t* pDestination, const char16_t* pSource, size_t n);

        int Strcmp(const char8_t*  pString1, const char8_t*  pString2);
        int Strcmp(const char16_t* pString1, const char16_t* pString2);

        int Strnicmp(const char8_t*  pString1, const char8_t*  pString2, size_t n);
 
        int Stricmp(const char8_t*  pString1, const char8_t*  pString2);
        int Stricmp(const char16_t* pString1, const char16_t* pString2);

        char8_t*  Strcat(char8_t*  pDestination, const char8_t*  pSource);
        char16_t* Strcat(char16_t* pDestination, const char16_t* pSource);

        const char8_t*  Strchr(const char8_t*  pString, int c);
        const char16_t* Strchr(const char16_t* pString, char16_t c);

        size_t Strlcpy(char8_t*  pDestination, const char8_t*  pSource, size_t nDestCapacity);
        size_t Strlcpy(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity);

        int Strlcpy(char16_t* pDestination, const char8_t*  pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);

        char8_t*  Stristr(const char8_t*  pString, const char8_t*  pSubString);

        char16_t* Strlwr(char16_t* pString);

        uint32_t AtoU32(const char8_t*  pString);
        uint32_t AtoU32(const char16_t* pString);

        int32_t AtoI32(const char8_t*  pString);
        int32_t AtoI32(const char16_t* pString);

        uint32_t StrtoU32(const char8_t*  pString, char8_t**  ppStringEnd, int nBase);
        uint32_t StrtoU32(const char16_t* pString, char16_t** ppStringEnd, int nBase);

        int32_t StrtoI32(const char16_t* pString, char16_t** ppStringEnd, int nBase);

        double Strtod(const char8_t* pString, char8_t** ppStringEnd);
        double Strtod(const char16_t*pString, char16_t** ppStringEnd);

        double Atof(const char8_t* pString);
        double Atof(const char16_t* pString);

        int Isspace(char8_t c);
        int Isspace(char16_t c);

        int Isprint(char8_t c);
        int Isprint(char16_t c);

        int Isalnum(char8_t c);
        int Isalnum(char16_t c);

        int Isalpha(char8_t c);
        int Isalpha(char16_t c);

        const char8_t* GetTextLine(const char8_t* pText, const char8_t* pTextEnd, const char8_t** ppNewText);

        bool SplitTokenDelimited(const char16_t* pSource, size_t nSourceLength, char16_t cDelimiter, 
                                       char16_t* pToken,  size_t nTokenLength, const char16_t** ppNewSource = NULL);

        bool SplitTokenSeparated(const char8_t* pSource, size_t nSourceLength, char8_t c, 
                                       char8_t* pToken,  size_t nTokenLength, const char8_t** ppNewSource);

        uint32_t FNV1(const void* pData, size_t nLength, uint32_t nInitialValue = 2166136261U);


    } // namespace Text

} // namespace EA



///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace Text
    {
        inline size_t Strlen(const char8_t* pString)
        {
            return strlen(pString);
        }

        inline size_t Strlen(const char16_t* pString)
        {
            return wcslen(pString);
        }


        inline char8_t* Strcpy(char8_t* pDestination, const char8_t*  pSource)
        {
            return strcpy(pDestination, pSource);
        }

        inline char16_t* Strcpy(char16_t* pDestination, const char16_t* pSource)
        {
            return wcscpy(pDestination, pSource);
        }


        inline char8_t* Strncpy(char8_t*  pDestination, const char8_t*  pSource, size_t n)
        {
            return strncpy(pDestination, pSource, n);
        }

        inline char16_t* Strncpy(char16_t* pDestination, const char16_t* pSource, size_t n)
        {
            return wcsncpy(pDestination, pSource, n);
        }


        inline int Strcmp(const char8_t*  pString1, const char8_t*  pString2)
        {
            return strcmp(pString1, pString2);
        }

        inline int Strcmp(const char16_t* pString1, const char16_t* pString2)
        {
            return wcscmp(pString1, pString2);
        }


        inline char8_t*  Strcat(char8_t*  pDestination, const char8_t*  pSource)
        {
            return strcat(pDestination, pSource);
        }

        inline char16_t* Strcat(char16_t* pDestination, const char16_t* pSource)
        {
            return wcscat(pDestination, pSource);
        }


        inline const char8_t*  Strchr(const char8_t*  pString, int c)
        {
            return strchr(pString, c);
        }

        inline const char16_t* Strchr(const char16_t* pString, char16_t c)
        {
            return wcschr(pString, c);
        }


        inline uint32_t AtoU32(const char8_t*  pString)
        {
            return (uint32_t)strtoul(pString, NULL, 10);
        }

        inline uint32_t AtoU32(const char16_t* pString)
        {
            return StrtoU32(pString, NULL, 10);
        }

        inline int32_t AtoI32(const char8_t*  pString)
        {
            return (int32_t)strtol(pString, NULL, 10);
        }


        inline int32_t AtoI32(const char16_t* pString)
        {

            return StrtoI32(pString, NULL, 10);
        }

        inline uint32_t StrtoU32(const char8_t*  pString, char8_t**  ppStringEnd, int nBase)
        {
            return (uint32_t)strtoul(pString, ppStringEnd, nBase);
        }

        inline double Strtod(const char8_t* pString, char8_t** ppStringEnd)
        {
            return strtod(pString, ppStringEnd);
        }

        inline double Strtod(const char16_t* pString, char16_t** ppStringEnd)
        {
#if (EA_WCHAR_SIZE == 2) && defined(EA_HAVE_WCHAR_IMPL)
            return wcstod(pString, ppStringEnd);
#else
            char8_t  buffer8[64];
            char8_t* p    = buffer8;
            char8_t* pEnd = buffer8 + 63; 

            while(p != pEnd)
            {   //             '+'                  'z'
                if((*pString < 0x2b) || (*pString > 0x7a)) // This includes a '\0' check.
                    break;
                *p++ = (char8_t)(int16_t)(uint16_t)*pString++;
            }
            *p = 0;

            double d = strtod(buffer8, &p);
            if(ppStringEnd)
                *ppStringEnd = (char16_t*)pString + (uintptr_t)(p - buffer8); // This math should be safe in the presence of multi-byte UTF8 strings because the only chars that can be part of a floating point string are always single bytes.

            return d;
#endif

        }


        inline double Atof(const char8_t* pString)
        {
            return strtod(pString, NULL);
        }

        inline double Atof(const char16_t* pString)
        {
            return Strtod(pString, NULL);
        }

        inline int Isspace(char8_t c)
        {
            return isspace((uint8_t)c);
        }

        inline int Isspace(char16_t c)
        {
            if(c < 256) 
                return isspace((uint8_t)c);
            return 0;
        }


        inline int Isprint(char8_t c)
        {
            return isprint(c);
        }

        inline int Isprint(char16_t c)
        {
            if(c < 256) 
                return isprint((uint8_t)c);
            return 0;
        }


        inline int Isalnum(char8_t c)
        {
            return isalnum((uint8_t)c);
        }

        inline int Isalnum(char16_t c)
        {
            if(c < 256) 
                return isalnum((uint8_t)c);
            return 1; // This isn't strictly correct, but for our uses it will be OK unless the user tries to do something unusual.
        }


        inline int Isalpha(char8_t c)
        {
            return isalpha((uint8_t)c);
        }

        inline int Isalpha(char16_t c)
        {
            if(c < 256) 
                return isalpha((uint8_t)c);
            return 1; // This isn't strictly correct, but for our uses it will be OK unless the user tries to do something unusual.
        }


    } // namespace Text

} // namespace EA



#endif // Header include guard






