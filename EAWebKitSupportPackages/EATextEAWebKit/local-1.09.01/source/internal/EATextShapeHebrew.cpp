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
// EATextShapeHebrew.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This implementation is based on the Unicode Standard, plus some wisdon 
// regarding practical issues. If you want to understand Hebrew script, 
// there are plenty of sources on the Internet to get you started, such as 
// http://www.jewfaq.org/alephbet.htm. Hebrew is not very difficult to shape,
// though properly positioning glyphs without OpenType support can be 
// potentially problematic.
//
///////////////////////////////////////////////////////////////////////////////

#include <EAText/internal/EATextScriptHebrew.h>
#include <EAText/EATextUnicode.h>
#include <EAText/EATextTypesetter.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{

const size_t kMaxHebrewCharClusterSize  = 32; // In reality, valid clusters are only as many as 3 chars.
const size_t kMaxHebrewGlyphClusterSize = 32; // In reality, valid clusters are only as many as 3 glyphs unless there is some unusual stuff happening such as ligatures.

enum HebrewCharClass
{
    kHCCNone       = 0,  /// Unassigned Unicode code point (e.g. 0590).
    kHCCSpacing    = 1,  /// Regular Hebrew 'alefbetical' character (e.g. 05D0).
    kHCCNonSpacing = 2,  /// Diacritic character (e.g. 05C1).
    kHCCDagesh     = 3   /// Dagesh character (05BC), which is a special kind of non-spacing diacritic character. http://en.wikipedia.org/wiki/Dagesh
};

enum HebrewCharClassFlags
{
    kHCCFNone       = 0x01,  // 1 << kHCCNone
    kHCCFSpacing    = 0x02,  // 1 << kHCCSpacing
    kHCCFNonSpacing = 0x04,  // 1 << kHCCNonSpacing
    kHCCFDagesh     = 0x08   // 1 << kHCCDagesh
};


// We make some abbreviations so that the table below is more legible.
const uint8_t hcNo = kHCCNone;
const uint8_t hcSp = kHCCSpacing;
const uint8_t hcNS = kHCCNonSpacing;
const uint8_t hcDa = kHCCDagesh;


///////////////////////////////////////////////////////////////////////////////
// gHebrewCharClassTable
//
const uint8_t gHebrewCharClass[112] = 
{
    hcNo, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, // 0x0590
    hcNS, hcNS, hcNo, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, // 0x05A0
    hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNS, hcNo, hcNS, hcDa, hcNS, hcSp, hcNS, // 0x05B0
    hcSp, hcNS, hcNS, hcSp, hcNS, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, // 0x05C0
    hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, // 0x05D0
    hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcSp, hcNo, hcNo, hcNo, hcNo, hcNo, // 0x05E0
    hcSp, hcSp, hcSp, hcSp, hcSp, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo, hcNo  // 0x05F0
};


///////////////////////////////////////////////////////////////////////////////
// gHebrewClusterTable
//
// Defines which classes of Hebrew characters can be part of a cluster.
// This is a map of HebrewCharClass to HebrewCharClass. The table refers
// to the pairing of char 1 with a succeeding char 2. Char 1 is represented
// by rows below, and char 2 is represented by columns.
// See the Unicode Standard, section 8.1.
//
const unsigned gHebrewClusterTable[4][4] = 
{
    { 0, 0, 0, 0 },
    { 0, 0, 1, 1 },  // kHCCSpacing / kHCCNonSpacing, kHCCSpacing / kHCCDagesh
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 }   // kHCCDagesh / kHCCSpacing
};


///////////////////////////////////////////////////////////////////////////////
// GetHebrewGlyphsForChars
//
// This is the Hebrew version of the general GetGlyphsForChar function, though
// it works on multiple chars. It merely does some possible filtering and then 
// calls GetGlyphsForChar.
//
eastl_size_t Typesetter::GetHebrewGlyphsForChars(const Char* pChar, eastl_size_t clusterSize, const AnalysisInfo* pAnalysisInfo, 
                                                  GlyphId* pGlyphIdArray, eastl_size_t& glyphCount)
{
    Char          charTemp[kMaxHebrewCharClusterSize];
    eastl_size_t  j, k;

    glyphCount = 0;

    // We don't convert non-final and final forms of letters based on the context.
    // We expect that the user has done that and if they want to use a non-final
    // character at the end of a word then we let them do so and we just display
    // that character. This refers to characters such as Kaf, Mem, Nun, and Pe.

    for(j = 0, k = 0; j < clusterSize; ++j, ++k)
    {
        const Char c = pChar[j];

        EA_ASSERT(IsCharHebrew(c));
        if((j == 0) && (gHebrewCharClass[c - 0x0590] == kHCCNonSpacing)) // If the first char is a decoration char...
        {
            // We prepend a character which is a circle implemented with dotted lines. 
            // It is the Unicode standard for generic base character to be represented
            // by such a dotted circle. It is a defacto standard that we add such a
            // character when none is provided. It makes the text look more sensible.
            charTemp[k++] = c;
            charTemp[k]   = 0x25CC; // If we can't use 0x25CC, we ought to try a space char.
        }
        else
            charTemp[k] = c;
    }

    for(j = 0; j < k; ) // Now 'k' is the new cluster size, though 99% of the time it will be equal to clusterSize.
    {
        j += GetGlyphsForChar(charTemp + j, k - j, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount, EATEXT_CHAR("\x25CB _o"), 4);
    }

    EA_ASSERT(glyphCount == clusterSize);
    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// AppendHebrewCharCluster
//
eastl_size_t Typesetter::AppendHebrewCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize)
{
    // The 'iCharBegin' argument refers to the position of pCharCluster in mLineLayout.mCharArray. 
    // There is a possibility that pCharCluster doesn't point to mLineLayout.mCharArray[iCharBegin],
    // as we may be doing some kind of implicit substitution. Thus we have pCharCluster 
    // as an explicit parameter.

    const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[iCharBegin];
    GlyphId                   pGlyphIdArray[kMaxHebrewGlyphClusterSize];
    eastl_size_t              glyphCount = 0;
    eastl_size_t              glyphCountPrev = 0;
    eastl_size_t              charsEaten = 0;

    EA_ASSERT(clusterSize && ((iCharBegin + charCount) <= mLineLayout.mCharArray.size()));
    #ifdef EA_DEBUG
        memset(pGlyphIdArray, 0, sizeof(pGlyphIdArray));
    #endif

    for(eastl_size_t c = 0; c < clusterSize; c += charsEaten)
    {
        if(IsCharHebrew(*pCharCluster))
            charsEaten = GetHebrewGlyphsForChars(pCharCluster + c, clusterSize - c, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
        else
            charsEaten = GetGlyphsForChar(pCharCluster + c, clusterSize - c, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);

        AppendGeneralGlyphCluster(iCharBegin, charCount, pCharCluster + c, charsEaten, 
                                  pGlyphIdArray + glyphCountPrev, glyphCount - glyphCountPrev, 
                                  pAnalysisInfo->mnBidiLevel);

        // Hebrew fonts have decorations that may need to be positionally adjusted.
        // We could do such adjustments with OpenType or with heuristics. The former
        // is more involved and slower but results in the best output. The latter
        // has the opposite characteristics. Right now we do general placement, but 
        // we ought to do some adjustments, such as the case whereby we have lone
        // diacritics that we added a 0x25CC char to.
        PlaceGeneralGlyphCluster(iCharBegin + c, charsEaten);

        glyphCountPrev = glyphCount;
    }

    // As it stands now, all chars must result in at least one glyph, though that 
    // one glyph could be a zero-width space character. Note that the render code
    // has the opportunity to remove such a "no-op" instruction from the display list.
    EA_ASSERT(glyphCount > 0);
    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetHebrewCharCluster
//
// We read a single character cluster. A cluster is defined by a set of 
// table-based combining rules whereby each character in the Hebrew block
// is given a category of 'none', 'spacing', 'non-spacing', or 'dagesh'.
//
// The return value is the number of chars eaten; clusterSize is the number of 
// chars generated. In the large majority of cases, they will be equal.
//
eastl_size_t Typesetter::GetHebrewCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                              Char* pCharCluster, eastl_size_t& clusterSize)
{
    EA_ASSERT((i < iCharEnd) && (iCharEnd <= mLineLayout.mCharArray.size()));

    clusterSize = 0;

    for(const Char* p = &mLineLayout.mCharArray[i], *pEnd = &mLineLayout.mCharArray[iCharEnd]; 
         (p < pEnd) && (clusterSize < kMaxHebrewCharClusterSize); ++p)
    {
        const Char c = *p;

        // If the character is non-Hebrew, it can't compose with Hebrew.
        // If the character is the first and it is a diacritic, it can't compose with succeeding Hebrew.
        const bool bCharIsHebrew = IsCharHebrew(c);

        if(!bCharIsHebrew || ((clusterSize == 0) && ((1 << gHebrewCharClass[c - 0x0590]) & (kHCCFNonSpacing | kHCCFDagesh))))
        {
            if(clusterSize == 0) // If this is the first char...
            {
                if(bCharIsHebrew)
                    pCharCluster[clusterSize++] = c;
                else
                    return GetGeneralCharCluster(i, iCharEnd, pCharCluster, clusterSize);
            }
            break;
        }
        else
        {
            if(clusterSize == 0)
                pCharCluster[clusterSize++] = c;
  
            EA_ASSERT(IsCharHebrew(*pCharCluster) && IsCharHebrew(c));

            const unsigned prevCharClass = gHebrewCharClass[*pCharCluster - 0x0590];
            const unsigned curCharClass  = gHebrewCharClass[c - 0x0590];

            EA_ASSERT((prevCharClass < 4) && (curCharClass < 4));

            if((clusterSize == 0) || gHebrewClusterTable[prevCharClass][curCharClass]) // If the char is the first char or can pair with the first char...
                pCharCluster[clusterSize++] = c;
            else
            {
                EA_ASSERT(clusterSize > 0);
                break;
            }
        }
    }

    EA_ASSERT(clusterSize > 0);
    return clusterSize; // We always have a 1:1 relationship between chars in the source and chars in the generated cluster.
}


///////////////////////////////////////////////////////////////////////////////
// ShapeHebrew
//
void Typesetter::ShapeHebrew(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    EA_ASSERT(iCharEnd <= mLineLayout.mCharArray.size());

    Char         pCharCluster[kMaxHebrewCharClusterSize];
    eastl_size_t clusterSize;
    eastl_size_t charCount;

    for(eastl_size_t i = iCharBegin; i < iCharEnd; i += charCount)
    {
        #ifdef EA_DEBUG
            memset(pCharCluster, 0, sizeof(pCharCluster));
        #endif

        charCount = GetHebrewCharCluster(i, iCharEnd, pCharCluster, clusterSize);
        AppendHebrewCharCluster(i, charCount, pCharCluster, clusterSize);
    }
}


} // namespace Text

} // namespace EA









