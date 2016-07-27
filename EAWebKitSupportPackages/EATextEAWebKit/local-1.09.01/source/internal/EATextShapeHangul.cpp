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
// EATextShapeHangul.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// We are concerned here with the composing of Hangul Jamo into predefined
// Hangul syllables. 
//
// See section 3.11, 4.4, 14.3, and 15.1 of the Unicode Standard version 3.0. 
///////////////////////////////////////////////////////////////////////////////


#include <EAText/internal/EATextScriptHangul.h>
#include <EAText/EATextUnicode.h>
#include <EAText/EATextTypesetter.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{

const size_t kMaxHangulCharClusterSize  = 32; // In reality, valid clusters are only as many as 3 chars.
const size_t kMaxHangulGlyphClusterSize = 32; // In reality, valid clusters are only as many as 3 glyphs unless there is some unusual stuff happening such as ligatures.


enum HangulClusterType
{
    kHangulClusterTypeNone,
    kHangulClusterTypeJamo,
    kHangulClusterTypeTone,
    kHangulClusterTypeUnicode
};


///////////////////////////////////////////////////////////////////////////////
// Macros
//
// In the macros below we use:
//    'c' to denote an arbitrary Unicode character.
//    's' to denote a Hangul syllable.
//    'j' to denote a Jamo syllable.
//    'l' to denote a Leading Jamo.
//    'v' to denote a Vowel Jamo.
//    't' to denote a Trailing Jamo.
// In each case the values are still all Unicode characters and don't have to be
// from the range that their name denotes. The latters merely help us understand
// what the characters are used as in the function.
//
// These macro names and values are taken directly from the Unicode Standard, 
// version 3.0, section 3.11.
//
#define IsS(s)                  (((unsigned)(s) - 0xAC00u) < (0xD7A4u - 0xAC00u))
#define IsJ(j)                  (((unsigned)(j) - 0x1100u) < (0x1200u - 0x1100u))
#define IsL(j)                  (((unsigned)(j) - 0x1100u) < (0x1160u - 0x1100u))
#define IsV(j)                  (((unsigned)(j) - 0x1160u) < (0x11A8u - 0x1160u)) // Note that we include 0x1160 (filler char).
#define IsT(j)                  (((unsigned)(j) - 0x11A8u) < (0x1200u - 0x11A8u))
#define IsTone(c)               (((unsigned)(c) - 0x302Eu) < (0x3030u - 0x302Eu)) // Two characters: 0x302E & 0x302F.
#define IsLSyllable(j)          (((unsigned)(j) - 0x1100u) < (0x1113u - 0x1100u))
#define IsVSyllable(j)          (((unsigned)(j) - 0x1161u) < (0x1176u - 0x1161u))
#define IsTSyllable(j)          (((unsigned)(j) - 0x11A8u) < (0x11C3u - 0x11A8u))
#define HasT(s)                 (((s) - 0xAC00u) % 28)
#define GetSyllableLVT(l, v, t) (0xAC00 + (((l) - 0x1100) * 21 + ((v) - 0x1161)) * 28 + ((t) - 0x11A7))
#define GetSyllableLV(l, v)     (0xAC00 + (((l) - 0x1100) * 21 + ((v) - 0x1161)) * 28)
#define GetL(s)                 (0x1100 + (((s) - 0xAC00u) / 588))
#define GetV(s)                 (0x1161 + (((s) - 0xAC00u) % 588) / 28)
#define GetT(s)                 (0x11A7 + (((s) - 0xAC00u) % 28))


///////////////////////////////////////////////////////////////////////////////
// AppendHangulCharCluster
//
eastl_size_t Typesetter::AppendHangulCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize, int clusterType)
{
    // The 'iCharBegin' argument refers to the position of pCharCluster in mLineLayout.mCharArray. 
    // There is a possibility that pCharCluster doesn't point to mLineLayout.mCharArray[iCharBegin],
    // as we may be doing some kind of implicit substitution. Thus we have pCharCluster 
    // as an explicit parameter.

    const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[iCharBegin];
    GlyphId                   pGlyphIdArray[kMaxHangulGlyphClusterSize];
    eastl_size_t              glyphCount = 0;
    eastl_size_t              charsEaten = 0; // Number of chars read from pCharCluster.
    
    EA_ASSERT(clusterSize && ((iCharBegin + charCount) <= mLineLayout.mCharArray.size()));
    #ifdef EA_DEBUG
        memset(pGlyphIdArray, 0, sizeof(pGlyphIdArray));
    #endif

    switch(clusterType)
    {
        case kHangulClusterTypeJamo:
        {
            const Char cTone       = pCharCluster[clusterSize - 1];
            const bool bAppendTone = IsTone(cTone);
            int        syllableLength;
            Char       c;

            if(bAppendTone)
                --clusterSize; // Pretend it isn't there for now.

            if((clusterSize >= 3) && IsLSyllable(pCharCluster[0]) && IsVSyllable(pCharCluster[1]) && IsTSyllable(pCharCluster[2]))
                syllableLength = 3;
            else if((clusterSize >= 2) && IsLSyllable(pCharCluster[0]) && IsVSyllable(pCharCluster[1]))
                syllableLength = 2;
            else
                syllableLength = 0;

            if(syllableLength)
            {
                if(syllableLength == 3)
                    c = GetSyllableLVT(pCharCluster[0], pCharCluster[1], pCharCluster[2]);
                else
                    c = GetSyllableLV(pCharCluster[0], pCharCluster[1]);

                GetGlyphsForChar(&c, 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);

                charsEaten += syllableLength;
            }

            // If there are any other chars to shape (usually there aren't), just append them.
            for(eastl_size_t i = charsEaten; i < clusterSize; i += charsEaten)
            {
                if((pCharCluster[i] != kJamoLFiller) &&  // Outright ignore filler chars.
                   (pCharCluster[i] != kJamoVFiller))
                {
                    // The font selection in use had better have the desired glyphs or 
                    // else we will be displaying a lot of square boxes on the screen.
                    charsEaten += GetGlyphsForChar(pCharCluster + i, clusterSize - i, pAnalysisInfo, 
                                                    pGlyphIdArray + glyphCount, glyphCount);
                }
                else
                    ++charsEaten;
            }

            EA_ASSERT(glyphCount > 0); // Do we have just a single Jamo filler char?
            if(glyphCount == 0)
            {
                const Char cHangulFiller = 0x3164; // 0x3164 is the Hangul filler char. It's just blank like a space char.

                charsEaten += GetGlyphsForChar(&cHangulFiller, 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount, EATEXT_CHAR("\x25CB _o"), 4);
            }

            if(bAppendTone)
            {
                charsEaten += GetGlyphsForChar(&cTone, 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount, 
                                                 (cTone == kHangulTone1) ? EATEXT_CHAR("\x00b7") : EATEXT_CHAR(":"), 1);
            }

            break;
        }

        case kHangulClusterTypeTone: // Standalone tone with no preceeding base glyph.
        {
            // We prepend a character which is a circle implemented with dotted lines. 
            // It is the Unicode standard for generic base character to be represented
            // by such a dotted circle. It is a defacto standard that we prepend such 
            // a character when none is provided. It makes the text look more sensible.
            const Char cDottedCircle = 0x25CC;

            for(eastl_size_t i = charsEaten; i < clusterSize; i += charsEaten) // clusterSize should usually (always?) be just one.
            {
                // Note that in practice we're going to need to put the tone to the 
                // left of the glyph, though here we stuff it afterwards. What we have
                // here won't do. Also, the tone chars ideally have a zero advance,
                // but the fallbacks definitely do not. 

                // We draw the tone, using fallback characters if the tone char isn't present.
                charsEaten += GetGlyphsForChar(pCharCluster + i, clusterSize - i, pAnalysisInfo, 
                                               pGlyphIdArray + glyphCount, glyphCount, 
                                               (pCharCluster[i] == kHangulTone1) ? EATEXT_CHAR("\x00b7") : EATEXT_CHAR(":"), 1);

                // Append a dotted-circle glyph, with a fallback of some other glyphs.
                GetGlyphsForChar(&cDottedCircle, 1, pAnalysisInfo, 
                                  pGlyphIdArray + glyphCount, glyphCount, EATEXT_CHAR("\x25CB _o"), 4);
            }

            break;
        }

        case kHangulClusterTypeUnicode:
        {
            // We just convert the Unicode Chars to glyphs.
            for(eastl_size_t i = charsEaten; i < clusterSize; i += charsEaten)
            {
                charsEaten += GetGlyphsForChar(pCharCluster + i, clusterSize - i, pAnalysisInfo, 
                                                pGlyphIdArray + glyphCount, glyphCount);
            }
            break;
        }

        default:
            EA_FAIL_MESSAGE("Typesetter::AppendHangulCharCluster: Unknown cluster type.");
            break;
    }

    // Assert that all of the passed in cluster was processed.
    EA_ASSERT(charsEaten == clusterSize);

    // We use the general glyph append function.
    AppendGeneralGlyphCluster(iCharBegin, charCount, pCharCluster, charsEaten, pGlyphIdArray, glyphCount, pAnalysisInfo->mnBidiLevel);

    // We use the general glyph placement function.
    PlaceGeneralGlyphCluster(iCharBegin, charsEaten);

    // As it stands now, all chars must result in at least one glyph, though that 
    // one glyph could be a zero-width space character. Note that the render code
    // has the opportunity to remove such a "no-op" instruction from the display list.
    EA_ASSERT(glyphCount > 0);
    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetHangulCharCluster
//
// The return value is the number of chars eaten; clusterSize is the number of 
// chars generated. In the large majority of cases, they will be equal.
//
eastl_size_t Typesetter::GetHangulCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                              Char* pCharCluster, eastl_size_t& clusterSize, int& clusterType)
{
    EA_ASSERT((i < iCharEnd) && (iCharEnd <= mLineLayout.mCharArray.size()));

    bool         bCompose  = false;  // To do: Make this configurable, perhaps part of LayoutSettings.
    eastl_size_t charCount = 0;

    if(!bCompose) // If we leave the characters as-is and don't convert Jamo to composed syllables...
    {
        clusterType     = kHangulClusterTypeUnicode;
        pCharCluster[0] = mLineLayout.mCharArray[i];
        clusterSize     = 1;
        charCount       = 1;
    }
    else
    {
        clusterSize = 0;
        clusterType = kHangulClusterTypeJamo; // This might change below.

        for(const Char* p = &mLineLayout.mCharArray[i], *pEnd = &mLineLayout.mCharArray[iCharEnd]; 
            (p < pEnd) && (clusterSize < kMaxHangulCharClusterSize); ++p, ++charCount)
        {
            const Char c = *p;
            const bool bIsAnyHangul = (IsS(c) || IsJ(c) || IsTone(c));

            if(clusterSize != 0) // If this is not the first char in the cluster...
            {
                const Char cPrev = p[-1];

                // If the current character cannot combine with the previous character...
                // To consider: This should be a table lookup instead of a bunch of comparisons.
                if(!bIsAnyHangul ||
                ( IsT(cPrev) && IsL(c)) ||
                ( IsV(cPrev) && IsL(c)) ||
                ( IsT(cPrev) && IsV(c)) ||
                (!IsL(cPrev) && IsS(c)) ||
                    IsTone(cPrev))
                {
                    // We have a completed syllable.
                    break;
                }
            }

            if(!bIsAnyHangul) // If not any kind of Hangul (syllable, jamo, tone)...
            {
                // We have non-Hangul Unicode (e.g. Latin or simply a space chars).
                pCharCluster[clusterSize++] = c;
                ++charCount;
                clusterType = kHangulClusterTypeUnicode;
                break;
            }
            else if(IsS(c)) // If the char is a Hangul syllable...
            {
                // We break the Hangul syllable down to its Jamo components. 
                // We'll put them back together again in the next step of 
                // the shaping pipeline.
                pCharCluster[clusterSize++] = GetL(c);
                pCharCluster[clusterSize++] = GetV(c);

                if(HasT(c))
                    pCharCluster[clusterSize++] = GetT(c);
            }
            else if((clusterSize == 0) && IsTone(c)) // If char is an initial Hangul tone...
            {
                // We have a standalone tone.
                pCharCluster[clusterSize++] = c;
                ++charCount;
                clusterType = kHangulClusterTypeTone;
                break;
            }
            else
                pCharCluster[clusterSize++] = c; // Else we are working with individual Jamo symbols...
        }
    }

    EA_ASSERT_MESSAGE(clusterType != kHangulClusterTypeNone, "Typesetter::GetHangulCharCluster: Unknown cluster type.");
    EA_ASSERT((charCount > 0) && (clusterSize > 0));

    return charCount;
}


///////////////////////////////////////////////////////////////////////////////
// ShapeHangul
//
// We convert Hangul Jamo components to Hangul (precomposed) syllables.
// We leave Hangul syllables as-is.
//
void Typesetter::ShapeHangul(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    Char pCharCluster[kMaxHangulCharClusterSize];
    int  clusterType = kHangulClusterTypeNone;

    EA_ASSERT(iCharEnd <= mLineLayout.mCharArray.size());

    for(eastl_size_t i = iCharBegin, clusterSize, charCount; i < iCharEnd; i += charCount)
    {
        #ifdef EA_DEBUG
            memset(pCharCluster, 0, sizeof(pCharCluster));
        #endif

        charCount = GetHangulCharCluster(i, iCharEnd, pCharCluster, clusterSize, clusterType);
        AppendHangulCharCluster(i, charCount, pCharCluster, clusterSize, clusterType);
    }
}






// Undefine defines in the case that this .cpp file is part of an 
// aggregated .cpp file build.
#undef IsS
#undef IsJ
#undef IsL
#undef IsV
#undef IsT
#undef IsTone
#undef IsLSyllable
#undef IsVSyllable
#undef IsTSyllable
#undef HasT
#undef GetSyllableLVT
#undef GetSyllableLV
#undef GetL
#undef GetV
#undef GetT



} // namespace Text

} // namespace EA














