/*
Copyright (C) 2009, 2011 Electronic Arts, Inc.  All rights reserved.

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

/////////////////////////////////////////////////////////////////////////////
// DateEA.h
// Written by Paul Pedriana
//
// Implements IETF RFC 2616 date conversions.
/////////////////////////////////////////////////////////////////////////////

#ifndef DateEA_h
#define DateEA_h


#include <time.h>


namespace EA
{
    namespace WebKit
    {
        // Reads RFC 822 dates, RFC 1123 dates, RFC 850 dates, RFC 1036 dates, C asctime dates.
        // Web servers out there have a habit of using these inconsistently with the HTTP standard,
        // which asks for RFC 1123 dates.
        // The output time will be in local time, as per the C time() function.
        time_t ReadDateString(const char* pDateString);

        // Writes an RFC 1123 string using the time expressed as GMT.
        // Input time is expected to be local time, as per the C time() function.
        // The dateStringCapacity should be at least 40.
        // Returns the strlen of the written string. Returns -1 if there wasn't 
        // enough capacity in the string, which won't occur as long as you 
        // always specify a capacity >= 40.
        // Example output:
        //      "Tue, 15 Nov 1994 12:45:26 GMT"
        int WriteDateString(time_t time, char* pDateString, size_t dateStringCapacity);
    }
}


#endif // Header include guard



