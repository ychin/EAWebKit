/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// FnMatch.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Maintained by Paul Pedriana.
//
// Implements a file path match much like the Unix fnmatch function.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/FnMatch.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>


namespace EA
{

namespace IO
{

namespace // anonymous
{


///////////////////////////////////////////////////////////////////////////////
// RangeMatch
//
// Helper function for FnMatch.
//
const char8_t* RangeMatch(const char8_t* pPattern, char8_t cSeparator, char8_t test, int fnMatchFlags)
{
    int     ok;
    char8_t c, c2;
    int     negate = (*pPattern == '!' || *pPattern == '^');

    if(negate)
        ++pPattern;

    if(fnMatchFlags & kFNMCaseFold)
        test = (char8_t)tolower((unsigned char)test);

    for(ok = 0; (c = *pPattern++) != ']'; ) 
    {
        if((c == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
            c = *pPattern++;

        if(c == '\0')
            return NULL;

        if(fnMatchFlags & kFNMCaseFold)
            c = (char8_t)tolower((unsigned char)c);

        if((*pPattern == '-') && ((c2 = pPattern[1]) != '\0') && (c2 != ']')) 
        {
            pPattern += 2;

            if((c2 == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
                c2 = *pPattern++;

            if(c2 == '\0')
                return NULL;

            if(fnMatchFlags & kFNMCaseFold)
                c2 = (char8_t)tolower((unsigned char)c2);

            if((unsigned char)c    <= (unsigned char)test &&
               (unsigned char)test <= (unsigned char)c2)
            {
                ok = 1;
            }
        } 
        else if(c == test)
            ok = 1;
    }

    return (ok == negate) ? NULL : pPattern;
}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// FnMatch
//
EAIO_API bool FnMatch(const char8_t* EA_RESTRICT pPattern, const char8_t* EA_RESTRICT pString, int fnMatchFlags)
{
    #if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS)
        if((fnMatchFlags & kFNMUnixPath) == 0)
            fnMatchFlags |= kFNMDosPath;
    #else
        if((fnMatchFlags & kFNMDosPath) == 0)
            fnMatchFlags |= kFNMUnixPath;
    #endif

    const char8_t* const pStringBegin = pString;
    const char8_t        cSeparator = (fnMatchFlags & kFNMDosPath) ? '\\' : '/';
    char8_t              test;

    for(;;)
    {
        char8_t c = *pPattern++;

        switch (c)
        {
            case '\0':
                if((fnMatchFlags & kFNMLeadingDir) && (*pString == cSeparator))
                    return true;

                return (*pString == '\0') ? true : false;

            case '?':
                if(*pString == '\0')
                    return false;

                if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
                    return false;

                if((*pString == '.') && 
                    (fnMatchFlags & kFNMPeriod) &&
                    ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
                {
                    return false;
                }

                ++pString;
                break;

            case '*':
                c = *pPattern;

                while(c == '*')
                    c = *++pPattern;

                if((*pString == '.') && 
                   (fnMatchFlags & kFNMPeriod) &&
                   ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
                {
                    return false;
                }

                if(c == '\0')
                {
                    if(fnMatchFlags & kFNMPathname)
                        return (fnMatchFlags & kFNMLeadingDir) || !strchr(pString, cSeparator) ? true : false;
                    else
                        return true;
                }

                else if((c == cSeparator) && (fnMatchFlags & kFNMPathname))
                {
                    pString = strchr(pString, cSeparator);

                    if(pString == NULL)
                        return false;
                    break;
                }

                while((test = *pString) != '\0') 
                {
                    if(FnMatch(pPattern, pString, (fnMatchFlags & ~kFNMPeriod)))
                        return true;

                    if((test == cSeparator) && (fnMatchFlags & kFNMPathname))
                        break;

                    ++pString;
                }

                return false;

            case '[':
                if(*pString == '\0')
                    return false;

                if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
                    return false;

                if((pPattern = RangeMatch(pPattern, cSeparator, *pString, fnMatchFlags)) == NULL)
                    return false;

                ++pString;
                break;

            case '\\':
                if(cSeparator != '\\')  // '\' is always treated as a file path separator when using DOS paths.
                {
                    if((fnMatchFlags & kFNMNoEscape) == 0)
                    {
                        if((c = *pPattern++) == '\0')
                        {
                            c = '\\';
                            --pPattern;
                        }
                    }
                }
                // Fall through

            default:
                if(c == *pString)
                {
                    // Empty
                }
                else if((fnMatchFlags & kFNMCaseFold) && (tolower((unsigned char)c) == tolower((unsigned char)*pString)))
                {
                    // Empty
                }
                else if((fnMatchFlags & kFNMPrefixDir) && 
                         (*pString == '\0') &&
                         (((c == cSeparator) && (pString != pStringBegin)) || ((pString == (pStringBegin + 1)) && (*pStringBegin == cSeparator))))
                {
                    return true;
                }
                else
                    return false;

                pString++;
                break;

        } // switch()

    } // for()

} // FnMatch




namespace // anonymous
{

// We make our our Strchr because some platforms don't implement it, some implement
// it in 32 bit wchar_t only (Linux), and this function is very simple.
char16_t* Strchr(const char16_t* pString, char16_t c)
{
    do {
        if(*pString == c)
            return (char16_t*)pString;
    } while (*pString++);

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// RangeMatch
//
// Helper function for FnMatch.
//
const char16_t* RangeMatch(const char16_t* pPattern, char16_t cSeparator, char16_t test, int fnMatchFlags)
{
    int      ok;
    char16_t c, c2;
    int      negate = (*pPattern == '!' || *pPattern == '^');

    if(negate)
        ++pPattern;

    if(fnMatchFlags & kFNMCaseFold)
        test = (char16_t)tolower((char16_t)test);

    for(ok = 0; (c = *pPattern++) != ']'; ) 
    {
        if((c == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
            c = *pPattern++;

        if(c == '\0')
            return NULL;

        if(fnMatchFlags & kFNMCaseFold)
            c = (char16_t)tolower((char16_t)c);

        if((*pPattern == '-') && ((c2 = pPattern[1]) != '\0') && (c2 != ']')) 
        {
            pPattern += 2;

            if((c2 == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
                c2 = *pPattern++;

            if(c2 == '\0')
                return NULL;

            if(fnMatchFlags & kFNMCaseFold)
                c2 = (char16_t)tolower((char16_t)c2);

            if((char16_t)c    <= (char16_t)test &&
               (char16_t)test <= (char16_t)c2)
            {
                ok = 1;
            }
        } 
        else if(c == test)
            ok = 1;
    }

    return (ok == negate) ? NULL : pPattern;
}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// FnMatch
//
EAIO_API bool FnMatch(const char16_t* EA_RESTRICT pPattern, const char16_t* EA_RESTRICT pString, int fnMatchFlags)
{
    #if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS)
        if((fnMatchFlags & kFNMUnixPath) == 0)
            fnMatchFlags |= kFNMDosPath;
    #else
        if((fnMatchFlags & kFNMDosPath) == 0)
            fnMatchFlags |= kFNMUnixPath;
    #endif

    const char16_t* const pStringBegin = pString;
    const char16_t        cSeparator = (fnMatchFlags & kFNMDosPath) ? '\\' : '/';
    char16_t              test;

    for(;;)
    {
        char16_t c = *pPattern++;

        switch (c)
        {
            case '\0':
                if((fnMatchFlags & kFNMLeadingDir) && (*pString == cSeparator))
                    return true;

                return (*pString == '\0') ? true : false;

            case '?':
                if(*pString == '\0')
                    return false;

                if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
                    return false;

                if((*pString == '.') && 
                    (fnMatchFlags & kFNMPeriod) &&
                    ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
                {
                    return false;
                }

                ++pString;
                break;

            case '*':
                c = *pPattern;

                while(c == '*')
                    c = *++pPattern;

                if((*pString == '.') && 
                   (fnMatchFlags & kFNMPeriod) &&
                   ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
                {
                    return false;
                }

                if(c == '\0')
                {
                    if(fnMatchFlags & kFNMPathname)
                        return (fnMatchFlags & kFNMLeadingDir) || !Strchr(pString, cSeparator) ? true : false;
                    else
                        return true;
                }

                else if((c == cSeparator) && (fnMatchFlags & kFNMPathname))
                {
                    pString = Strchr(pString, cSeparator);

                    if(pString == NULL)
                        return false;
                    break;
                }

                while((test = *pString) != '\0') 
                {
                    if(FnMatch(pPattern, pString, (fnMatchFlags & ~kFNMPeriod)))
                        return true;

                    if((test == cSeparator) && (fnMatchFlags & kFNMPathname))
                        break;

                    ++pString;
                }

                return false;

            case '[':
                if(*pString == '\0')
                    return false;

                if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
                    return false;

                if((pPattern = RangeMatch(pPattern, cSeparator, *pString, fnMatchFlags)) == NULL)
                    return false;

                ++pString;
                break;

            case '\\':
                if(cSeparator != '\\')  // '\' is always treated as a file path separator when using DOS paths.
                {
                    if((fnMatchFlags & kFNMNoEscape) == 0)
                    {
                        if((c = *pPattern++) == '\0')
                        {
                            c = '\\';
                            --pPattern;
                        }
                    }
                }
                // Fall through

            default:
                if(c == *pString)
                {
                    // Empty
                }
                else if((fnMatchFlags & kFNMCaseFold) && (tolower((char16_t)c) == tolower((char16_t)*pString)))
                {
                    // Empty
                }
                else if((fnMatchFlags & kFNMPrefixDir) && 
                         (*pString == '\0') &&
                         (((c == cSeparator) && (pString != pStringBegin)) || ((pString == (pStringBegin + 1)) && (*pStringBegin == cSeparator))))
                {
                    return true;
                }
                else
                    return false;

                pString++;
                break;

        } // switch()

    } // for()

} // FnMatch



} // namespace IO

} // namespace EA













