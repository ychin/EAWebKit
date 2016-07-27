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
// EATextShapeGeneral.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextUnicode.h>
#include <EAText/EATextTypesetter.h>
#include <EASTL/algorithm.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{


const size_t kMaxGeneralCharClusterSize  = 32;
const size_t kMaxGeneralGlyphClusterSize = 32;


///////////////////////////////////////////////////////////////////////////////
// PlaceGeneralGlyphCluster
//
// Takes the char/glyph info in mLineLayout and writes the glyphLayoutInfo and
// updates the pen position.
//
void Typesetter::PlaceGeneralGlyphCluster(eastl_size_t iCharBegin, eastl_size_t charClusterSize)
{
    eastl_size_t iGlyphBegin;
    eastl_size_t iGlyphEnd;

    EA_ASSERT(iCharBegin < mLineLayout.mAnalysisInfoArray.size());
    const EA::Text::TextStyle* const pTextStyle = mLineLayout.mAnalysisInfoArray[iCharBegin].mpTextStyle;
    EA_ASSERT(pTextStyle);

    const float fLetterSpacing = pTextStyle->mfLetterSpacing;

    // To consider: As of this writing we don't have support for TextStyle::mfWordSpacing.
    // We aren't going to support it until users need it, but the support could be implemented
    // here or possibly in the ShapeText function. To implement mfWordSpacing requires that we
    // identify word break locations ahead of time with our WordBreakIterator. 

    mLineLayout.GetGlyphRangeFromCharRange(iCharBegin, iCharBegin + charClusterSize, iGlyphBegin, iGlyphEnd);

    #if (EATEXT_KERNING_SUPPORT > 0)
        GlyphId glyphIdPrev = 0;
    #endif

    for(eastl_size_t g = iGlyphBegin; g < iGlyphEnd; ++g)
    {
        const GlyphId    glyphId         = mLineLayout.mGlyphArray[g];
        GlyphInfo&       glyphInfo       = mLineLayout.mGlyphInfoArray[g];
        GlyphLayoutInfo& glyphLayoutInfo = mLineLayout.mGlyphLayoutInfoArray[g];
        GlyphMetrics     glyphMetrics;

        // Possibly do a kerning adjustment.
        #if (EATEXT_KERNING_SUPPORT > 0)
            if((g != iGlyphBegin) && (glyphInfo.mbGlyphIsObject == 0)) // If we are past the first char and we are working with a character and not an object...
            {
                Kerning kerning;

                if(glyphLayoutInfo.mpFont->GetKerning(glyphIdPrev, glyphId, kerning, glyphInfo.mDirection, true))
                    mfPenX += kerning.mfKernX; // Typically mfKernX will be a small (~1 pixel for a 10 pixel wide character) negative (move second char closer to first char) value.
            }
        #endif

        // If the character is a non-spacing mark (e.g. diacritical), then possibly 
        // we would modify the metrics of the base + mark to position the mark more 
        // appropriately. See the Unicode Standard 4.0, Section 3.6.D16.

        if(glyphInfo.mbGlyphIsObject == 0)
            glyphLayoutInfo.mpFont->GetGlyphMetrics(glyphId, glyphMetrics);
        else
            GetObjectMetrics(g, glyphMetrics);

        SetGlyphLayoutInfo(glyphLayoutInfo, mfPenX, glyphMetrics);

        if(glyphInfo.mClusterPosition == 0) // We want to implement letter spacing adjustments only for the base char of clusters.
        {
            glyphMetrics.mfHAdvanceX += fLetterSpacing;
            // To do: If this is a word break location, then apply TextStyle::mfWordSpacing here.
        }

        // We track max pen position because it allows us to do better kerning.
        mfPenX += glyphMetrics.mfHAdvanceX;
        if(mfPenX > mfPenXMax)
            mfPenXMax = mfPenX;
        mfPenX = mfPenXMax;

        #if (EATEXT_KERNING_SUPPORT > 0)
            glyphIdPrev = glyphId;
        #endif
    }
}


///////////////////////////////////////////////////////////////////////////////
// AppendGeneralGlyphCluster
//
// Appends a single cluster of chars, which is represented visually by a 
// single cluster of glyphs.
//
void Typesetter::AppendGeneralGlyphCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                            const Char* pCharCluster, eastl_size_t charClusterSize, 
                                            const GlyphId* pGlyphCluster, eastl_size_t glyphClusterSize, 
                                            int embeddingLevel)
{
    (void)charClusterSize;
    EA_ASSERT(charCount && charClusterSize && glyphClusterSize);

    // We maintain mCharArray and mGlyphIndexArray in parallel.
    // We maintain mGlyphArray, mGlyphInfoArray, mGlyphLayoutInfoArray, and mCharIndexArray in parallel.
    EA_ASSERT(mLineLayout.mGlyphIndexArray.size() == iCharBegin);
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphInfoArray.size());
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphLayoutInfoArray.size());
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mCharIndexArray.size());

    // We append the chars to our LineLayout information. It is assumed that the 
    // given N chars corresponds to the given N glyphs. Usually you will have a 
    // 1:1 correspondence between chars and glyphs and (charClusterSize == glyphClusterSize).
    // Even more often you will have (charClusterSize == glyphClusterSize == 1).
    const eastl_size_t glyphArrayIndexEnd = mLineLayout.mGlyphArray.size();

    // Map each char to the first glyph.
    while(mLineLayout.mGlyphIndexArray.size() < (iCharBegin + charCount))
        mLineLayout.mGlyphIndexArray.push_back(glyphArrayIndexEnd);

    for(eastl_size_t g = 0; g < glyphClusterSize; g++)
    {
        const GlyphId glyphId = pGlyphCluster[g];
        Font* const   pFont   = mLineLayout.mAnalysisInfoArray[iCharBegin].mpFont;

        // Add a GlyphId entry. This will refer to the glyph provided by the Font.
        mLineLayout.mGlyphArray.push_back(glyphId);

        // Add a GlyphInfo entry. This will store logical information about the glyph.
        mLineLayout.mGlyphInfoArray.push_back();
        GlyphInfo& gi = mLineLayout.mGlyphInfoArray.back();

        // Add a GlyphLayoutInfo entry. This will store rendering information about the glyph.
        mLineLayout.mGlyphLayoutInfoArray.push_back();
        GlyphLayoutInfo& gli = mLineLayout.mGlyphLayoutInfoArray.back();

        gi.mGJC              = kGJCNone;                          // We set mGJC to something else later if and when we do justification.
        gi.mClusterPosition  = (unsigned)g;                       // If we were to have multiple glyphs that are together visually, 
        gi.mClusterSize      = (unsigned)glyphClusterSize;        // we would revise mClusterPosition and mClusterSize. We would need to
        gi.mClusterBreakable = 0;                                 // do this revision later, when we have all glyphs shaped for a line or paragraph.
        gi.mCharCount        = (unsigned)charCount;               // gi.mCharCount currently has only two bits of data.
        gi.mDirection        = embeddingLevel;                    // 

        if(EA_LIKELY(*pCharCluster != kCharOBJ))
        {
            gli.mpFont = pFont; // Note that gli.mpFont is a union of mpFont and mpObject.
            gi.mbGlyphIsObject = 0;
        }
        else
        {
            const Item* const pItem = GetScheduleItemFromCharIndex(iCharBegin, true); // iCharBegin is line-relative.
            gli.mpObject = pItem->mObject.mpObjectPtr; // Note that gli.mpFont is a union of mpFont and mpObject.
            gi.mbGlyphIsObject = 1;
        }

        // Map each glyph to the first char - that identified by 'iCharBegin'.
        mLineLayout.mCharIndexArray.push_back(iCharBegin);

        // We maintain mCharArray and mGlyphIndexArray in parallel.
        // We maintain mGlyphArray, mGlyphInfoArray, mGlyphLayoutInfoArray, and mCharIndexArray in parallel.
        EA_ASSERT(mLineLayout.mGlyphIndexArray.size() == (iCharBegin + charCount));
        EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphInfoArray.size());
        EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphLayoutInfoArray.size());
        EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mCharIndexArray.size());
    }
}


///////////////////////////////////////////////////////////////////////////////
// AppendGeneralCharCluster
//
// charCount is the number of chars referred to in mLineLayout.mCharArray.
// clusterSize is the number of chars in pCharCluster, which itself was 
// generated from mLineLayout.mCharArray and 98% of the time is equivalent.
// The strings will be unequal in the case of character-level substitutions
// such as mirroring, password chars, control chars, etc.
//
// The return value is the number of glyphs generated from pCharCluster/clusterSize.
//
eastl_size_t Typesetter::AppendGeneralCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                  const Char* pCharCluster, eastl_size_t clusterSize)
{
    // The 'iCharBegin' argument refers to the position of pCharCluster in mLineLayout.mCharArray. 
    // There is a possibility that pCharCluster doesn't point to mLineLayout.mCharArray[i],
    // as we may be doing some kind of implicit substitution. Thus we have pCharCluster 
    // as an explicit parameter.

    const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[iCharBegin];
    GlyphId                   pGlyphIdArray[kMaxGeneralGlyphClusterSize];
    eastl_size_t              glyphCount = 0;
    eastl_size_t              glyphCountPrev = 0;
    eastl_size_t              charsEaten = 0;

    EA_ASSERT(clusterSize && ((iCharBegin + charCount) <= mLineLayout.mCharArray.size()));
    #ifdef EA_DEBUG
        memset(pGlyphIdArray, 0, sizeof(pGlyphIdArray));
    #endif

    for(eastl_size_t i = 0; i < clusterSize; i += charsEaten)
    {
        // GetGlyphsForChar does a simple char -> glyph 1:1 conversion, through pAnalysisInfo->mpFont (mLineLayout.mAnalysisInfoArray[iCharBegin]). 
        charsEaten = GetGlyphsForChar(pCharCluster + i, clusterSize - i, pAnalysisInfo, 
                                        pGlyphIdArray + glyphCount, glyphCount);
        EA_ASSERT(charsEaten > 0);

        AppendGeneralGlyphCluster(iCharBegin, charCount, pCharCluster + i, charsEaten, 
                                  pGlyphIdArray + glyphCountPrev, glyphCount - glyphCountPrev, 
                                  pAnalysisInfo->mnBidiLevel);

        PlaceGeneralGlyphCluster(iCharBegin + i, charsEaten);

        glyphCountPrev = glyphCount;
    }

    // As it stands now, all chars must result in at least one glyph, though that 
    // one glyph could be a zero-width space character. Note that the render code
    // has the opportunity to remove such a "no-op" instruction from the display list.
    EA_ASSERT(glyphCount > 0);
    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetGeneralCharCluster
//
// We read just a single character cluster. For now in this general shaper we make 
// all clusters just a single char. Decoration chars are not yet recognized.
// However, when we do read decoration chars, charCount and cluster count would
// be incremented appropriately. It turns out that standalone Latin decoration 
// chars (e.g. 0x0300 Combining Grave Accent) are set to have negative x offsets
// and zero advance, so they don't need to be put into a char cluster in order 
// to work.
//
// The return value is the number of chars eaten; clusterSize is the number of 
// chars generated. In the large majority of cases, they will be equal.
//
eastl_size_t Typesetter::GetGeneralCharCluster(eastl_size_t iCharBegin, eastl_size_t iCharEnd, 
                                                Char* pCharCluster, eastl_size_t& clusterSize)
{
    (void)iCharEnd; // Prevent compiler warnings.
    EA_ASSERT((iCharBegin < iCharEnd) && (iCharEnd <= mLineLayout.mCharArray.size()));

    Char       c   = mLineLayout.mCharArray[iCharBegin];
    const bool bZW = IsCharZeroWidth(c);

    if(EA_UNLIKELY(mLayoutSettings.mTextStyleDefault.mPasswordMode == kPMPassword)) // If the text is password text...
    {
        // Possibly we should eat the current cluster instead of doing a
        // conversion of each Unicode code point to a password char.
        c = mPasswordChar;
    }
    else if(EA_UNLIKELY(c == kCharNBSP)) // If we have a non-breaking space character...
        c = kCharSpace;
    else if(EA_UNLIKELY(bZW && mLayoutSettings.mbDisplayZWG)) // If we have a zero-width glyph but we are supposed to display such glyphs...
        c = '_'; // Replace the ZW char with an actual char.
    else if(EA_UNLIKELY((c == kCharLF) || (c == kCharCR) || (c == kCharPSEP) || (c == kCharNEL) || (c == kCharLSEP))) // We always substitute newlines for ZWSpace.
        c = kCharZWSP;
    else if(EA_UNLIKELY(mLayoutSettings.mbHideControlCharacters && IsCharCategory(c, kCCFlagControlChar)))
        c = kCharZWNBSP;
    else if(EA_UNLIKELY(mLineLayout.mAnalysisInfoArray[iCharBegin].mnBidiLevel % 2)) // If we have a RTL bidi level...
        c = GetMirrorChar(c); // Most often, this will just return c.

    // As it stands now, we have a 1:1 mapping of input to output chars.
    clusterSize     = 1;
    pCharCluster[0] = c;

    return 1;
}


///////////////////////////////////////////////////////////////////////////////
// ShapeGeneral
//
// This function shapes general LTR (left to right) text. It should support
// a number of scripts reasonably well, including all modern Western scripts.
// This function expects text to be entirely LTR and will not work correctly
// if there is any RTL-ordered text in it.
//
// Runs are positioned in their own coordinate system. If multiple runs will
// be strung together on the same line, the runs will have to be offset.
//
// If you have any questions about this function, as Paul Pedriana. Some of the
// details are subtle and may not be immediately obvious.
//
void Typesetter::ShapeGeneral(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    Char pCharCluster[kMaxGeneralCharClusterSize];

    EA_ASSERT(iCharEnd <= mLineLayout.mCharArray.size());

    for(eastl_size_t i = iCharBegin, clusterSize, charCount; i < iCharEnd; i += charCount)
    {
        #ifdef EA_DEBUG
            memset(pCharCluster, 0, sizeof(pCharCluster));
        #endif

        charCount = GetGeneralCharCluster(i, iCharEnd, pCharCluster, clusterSize);
        AppendGeneralCharCluster(i, charCount, pCharCluster, clusterSize);
    }
}



} // namespace Text

} // namespace EA
















