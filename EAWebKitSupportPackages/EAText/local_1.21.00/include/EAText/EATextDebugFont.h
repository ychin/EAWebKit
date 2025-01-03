///////////////////////////////////////////////////////////////////////////////
// EATextDebugFont.h
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTDEBUGFONT_H
#define EATEXT_EATEXTDEBUGFONT_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextOutlineFont.h>
#include <string.h>


namespace EA
{
	namespace Text
	{
		/// DebugFont
		///
		/// Example usage:
		///    DebugFont* pFont = new DebugFont;
		///    if(pFont->Open(pStream)){
		///       pFont->SetTransform(16);
		///       GlyphBitmap* pGlyphBitmap;
		///       if(pFont->RenderGlyphBitmap(&pGlyphBitmap, 'a', kRFUnicodeId)){
		///          // Do something with the bitmap.
		///          pFont->DoneGlyphBitmap(pGlyphBitmap);
		///       }
		///       pFont->Close();
		///    }
		///    delete pFont;
		///
		class EATEXT_API DebugFont : public OutlineFont
		{
		public:
			enum {
				kCharMin =  32,
				kCharMax = 126,
				kCharEnd = kCharMax + 1
			};

			DebugFont(Allocator::ICoreAllocator* pCoreAllocator = NULL);

			bool SetTransform(float fSize, float fAngle = 0.f, float fSkew = 0.f);

			bool SetTransform(float f00, float f01, float f10, float f11)
				{ return OutlineFont::SetTransform(f00, f01, f10, f11); }

			bool GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics)
			{
				if((glyphId - (unsigned)kCharMin) < ((unsigned)kCharEnd - (unsigned)kCharMin))
				{
					glyphMetrics = mGlyphMetricsArray[glyphId - kCharMin];
					return true;
				}

				glyphMetrics = mGlyphMetricsArray[0];
				return false;
			}

			GlyphId GetGlyphId(Char c)
			{
				return (GlyphId)c; // We do no error testing, for speed purposes.
			}

			uint32_t GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
									 bool /*bUseReplacementGlyph*/ = true, const uint32_t /*nGlyphIdStride*/ = sizeof(GlyphId), bool /*bWriteInvalidGlyphs*/ = false)
			{
				memcpy(pGlyphIdArray, pCharArray, nCharArrayCount * sizeof(Char));
				return nCharArrayCount;
			}

			uint32_t GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
									 bool /*bUseReplacementGlyph*/ = true, const uint32_t /*nGlyphIdStride*/ = sizeof(GlyphId), bool /*bWriteInvalidGlyphs*/ = false)
			{
				for(uint32_t i = 0; i < nCharArrayCount; i++)
					*pGlyphIdArray++ = (uint8_t)pCharArray[i];
				return nCharArrayCount;
			}

			bool IsCharSupported(Char c, Script)
			{
				return ((c - (unsigned)kCharMin) < ((unsigned)kCharEnd - (unsigned)kCharMin)); // We support ascii chars 32 - 126 inclusive.
			}

			bool GetKerning(GlyphId /*g1*/, GlyphId /*g2*/, Kerning& /*kerning*/, int /*direction*/, bool /*bHorizontalLayout*/ = true)
			{
				return false;
			}

			// bool RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId g, uint32_t renderFlags = kRFDefault, 
			//                          float fXFraction = 0, float fYFraction = 0);
			// 
			// void DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap);

		protected:
			void SetupGlyphMetrics();

		protected:
			GlyphMetrics mGlyphMetricsArray[kCharEnd - kCharMin];  // 95 because we support ascii chars 32 - 126 inclusive.

		}; // class DebugFont

	} // namespace Text

} // namespace EA



#endif // EATEXT_EATEXTDEBUGFONT_H

















