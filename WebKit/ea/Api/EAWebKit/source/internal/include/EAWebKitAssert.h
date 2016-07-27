/*
Copyright (C) 2009, 2011, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitAssert.h
//
// By Arpit Baldeva (Isolated Paul Pedriana's code in EAWebkit.h)
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITASSERT_H
#define EAWEBKIT_EAWEBKITASSERT_H


///////////////////////////////////////////////////////////////////////
// This file isolates the EAWebkit Assert functionality from the EAWebkit.h. The reason for it is to avoid the dependency of core Webkit code on
// EAWebkit.h. The EAWebkit assert functionality is something that should be internal to the library and not exposed to the main app code.
///////////////////////////////////////////////////////////////////////

#include <EAWebKit/EAWebKitConfig.h>

///////////////////////////////////////////////////////////////////////
// Assertion failures
///////////////////////////////////////////////////////////////////////

#define EA_LOG_LEVEL (EA::WebKit::GetParameters().mEAWebkitLogLevel)

// These functions will call EAWebKitClient::DebugLog and 
// are thus global convenience functions.
extern "C" void WTFReportAssertionFailure(const char* file, int line, const char* function, const char* assertion);
extern "C" void WTFReportAssertionFailureWithMessage(const char* file, int line, const char* function, const char* assertion, const char* format, ...);
extern "C" void EAWriteLogMessage(const char* format, ...);

#define EAW_LOG(loglevel, fmt, ...)  if (loglevel <= EA_LOG_LEVEL) EAWriteLogMessage(fmt, __VA_ARGS__); else ((void)0)

#if EAWEBKIT_ASSERT_ENABLED
#define EAW_ASSERT(expr)                     if(!(expr)) WTFReportAssertionFailure(__FILE__, __LINE__, "", #expr); else ((void)0)
#define EAW_ASSERT_MSG(expr, msg)            if(!(expr)) WTFReportAssertionFailure(__FILE__, __LINE__, "", msg); else ((void)0)
#define EAW_ASSERT_FORMATTED(expr, fmt, ...) if(!(expr)) WTFReportAssertionFailureWithMessage(__FILE__, __LINE__, "", #expr, fmt, __VA_ARGS__); else ((void)0)
#define EAW_FAIL_MSG(msg)                                WTFReportAssertionFailure(__FILE__, __LINE__, "", msg)
#define EAW_FAIL_FORMATTED(fmt, ...)                     WTFReportAssertionFailureWithMessage(__FILE__, __LINE__, "", "fail", fmt, __VA_ARGS__)
#else
#define EAW_ASSERT(expr)                     ((void)0)
#define EAW_ASSERT_MSG(expr, msg)            ((void)0)
#define EAW_ASSERT_FORMATTED(expr, ...)      ((void)0)
#define EAW_FAIL_MSG(msg)                    ((void)0)
#define EAW_FAIL_FORMATTED(fmt, ...)         ((void)0)
#endif

#define EAW_CRASH()  *(volatile int*)(0) = 0

#endif // Header include guard
