///////////////////////////////////////////////////////////////////////////////
// EATextFont.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextFont.h>
#include <EAText/internal/EATextCoreAllocatorNew.h>
#include <EAStdC/EAString.h>
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
	return  (mfSize				== fd.mfSize) &&
			(mStyle				== fd.mStyle) &&
			(mfWeight			== fd.mfWeight) &&
			(mfStretch			== fd.mfStretch) &&
			(mPitch				== fd.mPitch) &&
			(mVariant			== fd.mVariant) &&
			(mSmooth			== fd.mSmooth) &&
			(mEffect			== fd.mEffect) &&
			(mfEffectX			== fd.mfEffectX) &&
			(mfEffectY			== fd.mfEffectY) &&
			(mEffectBaseColor	== fd.mEffectBaseColor) &&
			(mEffectColor		== fd.mEffectColor) &&
			(mHighLightColor	== fd.mHighLightColor) &&
			(EA::StdC::Stricmp(mFamily, fd.mFamily) == 0);
}



///////////////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////////////

#if EATEXT_THREAD_SAFETY_ENABLED
	EA::Thread::AtomicUint32 Font::sCreationCounter;
#else
	uint32_t Font::sCreationCounter;
#endif

Font::Font(Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
	mpUserData(NULL),
	mpStream(NULL),
	mSupportedScriptSet(),
	mCreationCount(sCreationCounter++),
	mReplacementGlyphId(0), // Will be set to something else later.
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
	// In the event this font is freed but not written over
	// we will ensure that any stale access to this font will fail
	// any comparison checks in the glyph cache.
	mCreationCount = (uint32_t) -1;
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
	#if EATEXT_THREAD_SAFETY_ENABLED
		int32_t rc = mRefCount.Decrement();
		if(rc)
			return rc;
	#else
		if(mRefCount > 1)
			return --mRefCount;
	#endif

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

uint32_t  Font::GetCreationCount()
{
	return mCreationCount;
}


} // namespace Text

} // namespace EA











