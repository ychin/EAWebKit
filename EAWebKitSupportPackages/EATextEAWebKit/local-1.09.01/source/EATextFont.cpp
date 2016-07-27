/*
Copyright (C) 2004, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextFont.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextFont.h>
#include <EAText/internal/EATextCoreAllocatorNew.h>
#include <EAText/internal/StdC.h>
#include <coreallocator/icoreallocator_interface.h>
#include <coreallocator/icoreallocatormacros.h>
#include EA_ASSERT_HEADER
#include <stddef.h>
#include <string.h>
#include <math.h>


namespace EA
{

namespace Text
{


///////////////////////////////////////////////////////////////////////////////
// FontDescription
///////////////////////////////////////////////////////////////////////////////

bool FontDescription::operator==(const FontDescription& fd) const
{
    return  (mfSize   == fd.mfSize) &&
            (mStyle   == fd.mStyle) &&
            (mfWeight == fd.mfWeight) &&
            (mSmooth  == fd.mSmooth) &&
            (Stricmp(mFamily, fd.mFamily) == 0) &&
            (mEffect  == fd.mEffect);

    // We currently leave out the following, which isn't entirely correct, but is
    // more efficient. It turns out that we can leave these out very often because
    // the above entities will already uniquely identify the font in most cases.
    //    float     mfStretch;
    //    Pitch     mPitch;
    //    Variant   mVariant;
    //    float     mfEffectX;
    //    float     mfEffectY;
    //    Color     mEffectBaseColor;
    //    Color     mEffectColor;
    //    Color     mHighLightColor;
}



///////////////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////////////

Font::Font(Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
    mpUserData(NULL),
    mpStream(NULL),
    mReplacementGlyphId(0), // Will be set to something else later.
    mSupportedScriptSet(),
    mRefCount(0)
{
    // Empty 
}


Font::Font(const Font& font)
{
    operator=(font);
}


Font& Font::operator=(const Font&)
{
    EA_FAIL(); // This is not yet supported.
    return *this;
}


Font::~Font()
{
    // Empty
}


void Font::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;
}


int Font::AddRef()
{
    EA_ASSERT(mRefCount < 50000); // Sanity check.
    return ++mRefCount;
}


int Font::Release()
{
    if(mRefCount > 1)
        return --mRefCount;

    if(mpCoreAllocator)
        CORE_DELETE(mpCoreAllocator, this);

    return 0;
}


void Font::SetOption(int32_t /*option*/, int32_t /*value*/)
{
    // Currently we do nothing.
}


void* Font::GetUserData() const
{
    // Intentionally not thread safe.
    return mpUserData;
}


void Font::SetUserData(void* pUserData)
{
    // Intentionally not thread safe.
    mpUserData = pUserData;
}


IO::IStream* Font::GetStream()
{
    return mpStream;
}


void Font::SetupSupportedScriptSet()
{
    mSupportedScriptSet.reset();

    // kScriptCommon includes punctuation, spaces, control characters, symbols, etc.
    // Every font will support at least some common script characters. But almost no
    // font will support all of them. 
    mSupportedScriptSet.set((size_t)kScriptCommon);

    if(IsCharSupported(0x0621, kScriptUnknown) && IsCharSupported(0x0641, kScriptUnknown))
        mSupportedScriptSet.set(kScriptArabic);

    if(IsCharSupported(0x0401, kScriptUnknown) && IsCharSupported(0x0414, kScriptUnknown))
        mSupportedScriptSet.set(kScriptCyrillic);

    if(IsCharSupported(0x0909, kScriptUnknown) && IsCharSupported(0x092A, kScriptUnknown))
        mSupportedScriptSet.set(kScriptDevanagari);

    if(IsCharSupported(0x0391, kScriptUnknown) && IsCharSupported(0x03B8, kScriptUnknown))
        mSupportedScriptSet.set(kScriptGreek);

    if(IsCharSupported(0x70A7, kScriptUnknown) && IsCharSupported(0x7535, kScriptUnknown))   // <-- These test characters need to be revised.
        mSupportedScriptSet.set(kScriptHan);

    if(IsCharSupported(0xC2B9, kScriptUnknown) && IsCharSupported(0xC9D1, kScriptUnknown))
        mSupportedScriptSet.set(kScriptHangul);

    if(IsCharSupported(0x05D0, kScriptUnknown) && IsCharSupported(0x05E2, kScriptUnknown))
        mSupportedScriptSet.set(kScriptHebrew);

    if(IsCharSupported(0x3061, kScriptUnknown) && IsCharSupported(0x3078, kScriptUnknown))
        mSupportedScriptSet.set(kScriptHiragana);

    if(IsCharSupported(0x30A4, kScriptUnknown) && IsCharSupported(0x30EE, kScriptUnknown))
        mSupportedScriptSet.set(kScriptKatakana);

    if(IsCharSupported(0x0041, kScriptUnknown) && IsCharSupported(0x007A, kScriptUnknown))
        mSupportedScriptSet.set(kScriptLatin);

    if(IsCharSupported(0x0E01, kScriptUnknown) && IsCharSupported(0x0E2C, kScriptUnknown))
        mSupportedScriptSet.set(kScriptThai);
}


} // namespace Text

} // namespace EA











