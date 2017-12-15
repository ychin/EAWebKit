/*
Copyright (C) 2009, 2013 Electronic Arts, Inc.  All rights reserved.

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

#include "EAAssert/eaassert.h"

#ifndef EA_ASSERT_HAVE_OWN_HEADER

#include <stdio.h>

    #if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400)
        #undef  _WIN32_WINNT
        #define _WIN32_WINNT 0x0400
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #pragma warning(push,0)
    #include <Windows.h>        // ::IsDebuggerPresent
    #pragma warning(pop)


///////////////////////////////////////////////////////////////////////////////
// EA_PLATFORM_MICROSOFT
//
// Defined as 1 or undefined.
// Implements support for the definition of EA_PLATFORM_MICROSOFT for the case
// of using EABase versions prior to the addition of its EA_PLATFORM_MICROSOFT support.
//


///////////////////////////////////////////////////////////////////////////////
// EA_PLATFORM_MOBILE
//
// Defined as 1 or undefined.
// Implements support for the definition of EA_PLATFORM_MOBILE for the case
// of using EABase versions prior to the addition of its EA_PLATFORM_MOBILE support.
//
#if (EABASE_VERSION_N < 20022) && !defined(CS_UNDEFINED_STRING)
    #if defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || \
        defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(EA_PLATFORM_WINCE)
        #define CS_UNDEFINED_STRING 1
    #endif
#endif


#if !defined(EA_ASSERT_VSNPRINTF)
        #define EA_ASSERT_VSNPRINTF _vsnprintf
        #define EA_ASSERT_SNPRINTF   _snprintf
#endif


namespace EA { 
    namespace Assert { 
    namespace Detail {
    namespace {

        bool DefaultFailureCallback(const char* expr, const char* filename, int line, const char* function, const char* msg, va_list args)
        {
            const int largeEnough = 2048;
            char output[largeEnough + 1] = {};
            char fmtMsg[largeEnough + 1] = {};

            int len = EA_ASSERT_VSNPRINTF(fmtMsg, largeEnough, msg, args);

            // different platforms return different values for the error, but in both
            // cases it'll be out of bounds, so clamp the return value to largeEnough.
            if (len < 0 || len > largeEnough)
                len = largeEnough;

            fmtMsg[len] = '\0';

            len = EA_ASSERT_SNPRINTF(output, largeEnough,
                "%s(%d) : EA_ASSERT failed: '%s' in function: %s\n, message: %s",
                filename, line, expr, function, fmtMsg);
            if (len < 0 || len > largeEnough)
                len = largeEnough;

            output[len] = '\0';

            if (IsDebuggerPresent())
            {
                ::OutputDebugStringA(output);
            }
            else
                puts(output);


            return true;
        }
        
        FailureCallback gFailureCallback = &DefaultFailureCallback; 
    }}

    void SetFailureCallback(FailureCallback failureCallback)
    {
        Detail::gFailureCallback = failureCallback;
    }

    FailureCallback GetFailureCallback()
    {
        return Detail::gFailureCallback;
    }


    bool Detail::VCall(const char *expr, const char *filename, int line, const char *function, const char *msg, ...)
    {
        va_list args;
        va_start(args, msg);
        bool ret = (*GetFailureCallback())(expr, filename, line, function, msg, args);
        va_end(args);
        return ret;
    }

    bool Detail::Call(const char *expr, const char *filename, int line, const char *function)
    {
        return VCall(expr, filename, line, function, "none");
    }

    bool Detail::Call(const char *expr, const char *filename, int line, const char *function, const char* msg)
    {
        return VCall(expr, filename, line, function, msg);

    }

    }}

#endif
