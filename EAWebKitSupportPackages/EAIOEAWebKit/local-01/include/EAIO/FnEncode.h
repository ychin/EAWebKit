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
// FnEncode.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Alex Liberman and Talin.
//
// Character transcoding functions for filenames
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_FNENCODE_H
#define EAIO_FNENCODE_H


#ifndef INCLUDED_eabase_H
    #include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#ifndef EAIO_EASTREAM_H
    #include <EAIO/EAStream.h>      // for kLengthNull
#endif
#ifndef EAIO_PATHSTRING_H
    #include <EAIO/PathString.h>    // for ConvertPathUTF{8,16}ToUTF{8,16}
#endif


namespace EA
{
    namespace IO
    {
        /// StrlcpyUTF16ToUTF8
        /// Copies a UTF16 string to a UTF8 string, but otherwise acts similar to strlcpy except for the 
        /// return value.
        /// Returns the strlen of the destination string. If destination pointer is NULL, returns the 
        /// strlen of the would-be string.
        /// Specifying a source length of kLengthNull copies from the source string up to the first NULL 
        /// character.
        EAIO_API size_t StrlcpyUTF16ToUTF8(char8_t* pDest, size_t nDestLength, const char16_t* pSrc, size_t nSrcLength = kLengthNull);
        
        /// StrlcpyUTF8ToUTF16
        /// Copies a UTF8 string to a UTF16 string, but otherwise acts similar to strlcpy except for the 
        /// return value.
        /// Returns the strlen of the destination string. If destination pointer is NULL, returns the 
        /// strlen of the would-be string.
        /// Specifying a source length of kLengthNull copies from the source string up to the first NULL 
        /// character.
        EAIO_API size_t StrlcpyUTF8ToUTF16(char16_t* pDest, size_t nDestLength, const char8_t* pSrc, size_t nSrcLength = kLengthNull);

        ///////////////////////////////////////////////////////////////////////////////
        /// Convenient conversion functions used by EAFileUtil and EAFileNotification
        ///////////////////////////////////////////////////////////////////////////////

        /// ConvertPathUTF8ToUTF16
        /// Expands the destination to the desired size and then performs a Strlcpy 
        /// with UTF8->UTF16 conversion.
        /// Returns the number of characters written.
        EAIO_API uint32_t ConvertPathUTF8ToUTF16(Path::PathString16& dstPath16, const char8_t* pSrcPath8);

        /// ConvertPathUTF16ToUTF8
        /// Expands the destination to the desired size and then performs a Strlcpy with 
        /// UTF16->UTF8 conversion.
        /// Returns the number of characters written.
        EAIO_API uint32_t ConvertPathUTF16ToUTF8(Path::PathString8& dstPath8, const char16_t* pSrcPath16);

        ///////////////////////////////////////////////////////////////////////////////
        // String comparison, strlen, and strlcpy for this module, since we don't have
        // access to EACRT.
        ///////////////////////////////////////////////////////////////////////////////

        EAIO_API bool StrEq16(const char16_t* str1, const char16_t* str2);

        EAIO_API size_t    EAIOStrlen8(const char8_t* str);
        EAIO_API size_t    EAIOStrlen16(const char16_t* str);
        EAIO_API size_t    EAIOStrlcpy8(char8_t* pDestination, const char8_t* pSource, size_t nDestCapacity);
        EAIO_API size_t    EAIOStrlcpy16(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity);

    } // namespace IO

} // namespace EA

#endif // EAIO_FNENCODE_H

