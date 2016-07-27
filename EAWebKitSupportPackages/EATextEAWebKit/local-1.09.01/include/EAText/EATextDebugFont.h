/*
Copyright (C) 2007, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextDebugFont.h
// By Paul Pedriana - 2007
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

















