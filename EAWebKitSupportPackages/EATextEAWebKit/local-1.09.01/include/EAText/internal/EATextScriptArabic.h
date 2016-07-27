/*
Copyright (C) 2006, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextScriptArabic.h
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSCRIPTARABIC_H
#define EATEXT_INTERNAL_EATEXTSCRIPTARABIC_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{

namespace Text
{


/// ArabicGlyphTypeFlag
/// 
/// The values here are significant as they refer to OpenType flags.
///
enum ArabicGlyphTypeFlag
{
    kAGTFIsolated = 1,    /// a.k.a. Nominal glyph form.
    kAGTFFinal    = 2,    /// a.k.a. Right-joining glyph form. Both right-joining and dual-joining Chars may employ this form.
    kAGTFMedial   = 4,    /// a.k.a. Dual-joining glyph form. Only dual-joining Chars may employ this form.
    kAGTFInitial  = 8,    /// a.k.a. Left-joining glyph form. Both left-joining and dual-joining Chars may employ this form.
};


/// ArabicGlyphExclusionFlags
///
enum ArabicGlyphExclusionFlags
{
    kAGEFIsolated =                 kAGTFInitial | kAGTFMedial | kAGTFFinal,
    kAGEFInitial  = kAGTFIsolated                | kAGTFMedial | kAGTFFinal,
    kAGEFMedial   = kAGTFIsolated | kAGTFInitial               | kAGTFFinal,
    kAGEFFinal    = kAGTFIsolated | kAGTFInitial | kAGTFMedial,
};




} // namespace Text

} // namespace EA


#endif // Header include guard








