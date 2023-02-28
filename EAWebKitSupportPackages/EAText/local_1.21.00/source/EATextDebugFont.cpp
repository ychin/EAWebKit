///////////////////////////////////////////////////////////////////////////////
// EATextDebugFont.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextDebugFont.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{




///////////////////////////////////////////////////////////////////////////////
// DebugFont
///////////////////////////////////////////////////////////////////////////////

DebugFont::DebugFont(Allocator::ICoreAllocator* pCoreAllocator)
	: OutlineFont(pCoreAllocator)
{
	memset(mGlyphMetricsArray, 0, sizeof(mGlyphMetricsArray));
}


// bool DebugFont::Close()
// {
//     bool bReturnValue = OutlineFont::Close();
// 
//     // Do something here?
// 
//     return bReturnValue;
// }


bool DebugFont::SetTransform(float fSize, float fAngle, float fSkew)
{
	// We intercept SetTransform here and call SetupGlyphMetrics. 
	// We do this here because it is after SetTransform that the 
	// font is first usable.

	if(OutlineFont::SetTransform(fSize, fAngle, fSkew))
	{
		SetupGlyphMetrics();
		return true;
	}

	return false;
}


// bool DebugFont::RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId glyphId, uint32_t renderFlags, float fXFraction, float fYFraction)
// {
//     bool bReturnValue = OutlineFont::RenderGlyphBitmap(pGlyphBitmap, glyphId, renderFlags, fXFraction, fYFraction);
// 
//     // Do something here?
// 
//     return bReturnValue;
// }


// void DebugFont::DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap)
// {
//     OutlineFont::DoneGlyphBitmap(pGlyphBitmap);
// }


void DebugFont::SetupGlyphMetrics()
{
	for(Char c = kCharMin; c < kCharEnd; ++c)
	{
		GlyphId g = 0;
		OutlineFont::GetGlyphIds(&c, 1, &g, true);
		OutlineFont::GetGlyphMetrics(g, mGlyphMetricsArray[c - kCharMin]);
	}
}


} // namespace Text

} // namespace EA











