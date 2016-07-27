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
// FnMatch.h
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
//
// Implements a file path match much like the Unix fnmatch function.
///////////////////////////////////////////////////////////////////////////////


#ifndef	EAIO_FNMATCH_H
#define	EAIO_FNMATCH_H


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>


namespace EA
{
    namespace IO
    {
        enum FnMatchFlags
        {
            kFNMNone       = 0x00,  /// No flags.
            kFNMPathname   = 0x01,  /// Wildcard don't can't see past directory separator chars.
            kFNMNoEscape   = 0x02,  /// Escape sequences (e.g. '\t') are not recognized. Escape sequences are only ever recognized with Unix-style paths and not DOS-style paths that use '\' chars. It's as if this flag is always active when DOS paths are in use.
            kFNMPeriod     = 0x04,  /// A leading '.' is matched only explicitly and not by * or ?.
            kFNMLeadingDir = 0x08,  /// Match a leading directory and allow ignoring after it.
            kFNMPrefixDir  = 0x10,  /// Directory path text that precedes the string can match the pattern.
            kFNMCaseFold   = 0x20,  /// Act case-insensitively.
            kFNMDosPath    = 0x40,  /// Force the path to be interpreted as a DOS path (e.g. '\') instead of native path.
            kFNMUnixPath   = 0x80   /// Force the path to be interpreted as a Unix path (e.g. '/') instead of native path.
        };

        /// FnMatch
        ///
        /// Supports the following special pattern symbols:
        ///     ?   -- arbitrary character
        ///     *   -- arbitrary characters
        ///     []  -- selection of characters
        ///
        /// The kFNMDosPath and kFNMUnixPath flags can be used to override the assumed
        /// path type, which defaults to the native path type of the current platform.
        ///
        /// Example usage:
        ///     bool result = FnMatch("a/b/c/*.?[ab]", "a/b/c/d.qa", kFNMCaseFold);
        ///
        EAIO_API bool FnMatch(const char8_t*  EA_RESTRICT pPattern, const char8_t*  EA_RESTRICT pString, int fnMatchFlags = 0);
        EAIO_API bool FnMatch(const char16_t* EA_RESTRICT pPattern, const char16_t* EA_RESTRICT pString, int fnMatchFlags = 0);

    } // namespace IO

} // namespace EA


#endif // Header include guard








