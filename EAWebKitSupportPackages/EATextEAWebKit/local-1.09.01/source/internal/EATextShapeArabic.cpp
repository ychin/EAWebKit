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
// EATextShapeArabic.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextUnicode.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/internal/EATextScriptArabic.h>
#include <EAText/internal/EATextOpenType.h>
#include EA_ASSERT_HEADER


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


namespace EA
{
namespace Text
{

const size_t kMaxArabicCharClusterSize  = 32; // In reality, valid clusters are only as many as 3 chars.
const size_t kMaxArabicGlyphClusterSize = 32; // In reality, valid clusters are only as many as 3 glyphs unless there is some unusual stuff happening such as ligatures.


///////////////////////////////////////////////////////////////////////////////
// We follow the Unicode standard section 8.2, rules R1-R7.
//
// R1: Transparent characters do not affect the joining behaviour or base 
//     (spacing) characters.
// R2: A right-joining character X that has a right join-causing character on 
//     the right will adopt the form Xr.
// R3: A left-joining character X that has a left join-causing character on 
//     the left will adope the form Xl.
// R4: A dual-joining character X that has a right join-causing character on 
//     the right and a left join-causing character on the left will adopt the 
//     form Xm.
// R5: A dual-joining character X that has a right join-causing character on 
//     the right and no left join-causing character on the left will adopt
//     the form Xr.
// R6: A dual-joining character X that has a left join-causing character on 
//     the left and non right join-causing character on the right will adopt 
//     the form Xl.
// R7: If none of the above ruls applies to a character X, then it will adopt 
//     the nominal form Xn.
///////////////////////////////////////////////////////////////////////////////


#define U kAJCNonJoining   // The standard uses 'U' and not 'N'.
#define C kAJCJoinCausing
#define D kAJCDualJoining
#define L kAJCLeftJoining
#define R kAJCRightJoining
#define T kAJCTransparent


/// gAJCArray
///
/// Defines the joining class for Arabic chars.
///
const ArabicJoiningClass gAJCArray[] =
{
    U, U, R, R, R, R, D, R, D, R, D, D, D, D, D, R,     // 0620
    R, R, R, D, D, D, D, D, D, D, D, U, U, U, U, U,     // 0630
    C, D, D, D, D, D, D, D, R, D, D, T, T, T, T, T,     // 0640
    T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, U,     // 0650
    U, U, U, U, U, U, U, U, U, U, U, U, U, U, D, D,     // 0660
    T, R, R, R, U, R, R, R, D, D, D, D, D, D, D, D,     // 0670
    D, D, D, D, D, D, D, D, R, R, R, R, R, R, R, R,     // 0680
    R, R, R, R, R, R, R, R, R, R, D, D, D, D, D, D,     // 0690
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,     // 06A0
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,     // 06B0
    R, D, R, R, R, R, R, R, R, R, R, R, D, R, D, R,     // 06C0
    D, D, R, R, U, R, U, T, T, T, T, T, T, T, T, T,     // 06D0
    T, T, T, T, T, U, U, T, T, U, T, T, T, T, R, R,     // 06E0
    U, U, U, U, U, U, U, U, U, U, D, D, D, U, U, D      // 06F0
};


/// gAJCArray2
///
/// Defines the joining class for supplemental Arabic chars.
///
const ArabicJoiningClass gAJCArray2[] =
{
    D, D, D, D, D, D, D, D, D, R, R, R, D, D, D, D,     // 0750
    D, D, D, D, D, D, D, D, D, D, D, R, R, D, U, U,     // 0760
    U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U      // 0770
};



/// GetArabicJoiningClass
///
/// The direction argument (-1, 0, +1) refers to whether we want the previous 
/// non-transparent Arabic char, the current Arabic char, or the next non-transparent 
/// Arabic char. 
///
static ArabicJoiningClass GetArabicJoiningClass(const Char* pText, eastl_size_t nTextLength, eastl_size_t pos, int direction)
{
    for(;;)
    {
        if((pos == 0) && (direction < 0))
            return U;

        pos += direction;

        if(pos >= nTextLength)
            return U;

        ArabicJoiningClass jc;
        const unsigned c = pText[pos];

        if((c - 0x0620u) < (0x0700u - 0x0620u))
            jc = gAJCArray[c - 0x0620];
        else if((c - 0x0750u) < (0x0780u - 0x0750u))
            jc = gAJCArray2[c - 0x0750];
        else if(c == kCharZWJ)
            return C;
        else
            return U;

        if((direction == 0) || (jc != T))
            return jc;
    }
}


/// AssignArabicCharProperties
///
/// The direction argument (-1, 0, +1) refers to whether we want the previous 
/// non-transparent Arabic char, the current Arabic char, or the next non-transparent 
/// Arabic char. 
///
void AssignArabicCharProperties(const Char* pText, eastl_size_t nTextLength, GlyphInfo* pGlyphInfo)
{
    EA_ASSERT(pText && (nTextLength > 0) && pGlyphInfo);

    for(eastl_size_t i = 0; i < nTextLength; i++)
    {
        const ArabicJoiningClass jcPrev = GetArabicJoiningClass(pText, nTextLength, i, -1);
        const ArabicJoiningClass jcCur  = GetArabicJoiningClass(pText, nTextLength, i,  0);
        const ArabicJoiningClass jcNext = GetArabicJoiningClass(pText, nTextLength, i, +1);

        // R1
        if(jcCur == kAJCTransparent)
        {
            pGlyphInfo[i].mScriptFlags = kAGEFIsolated;
            continue;
        }

        // R2
        if(jcPrev == kAJCJoinCausing ||
           jcPrev == kAJCLeftJoining ||
           jcPrev == kAJCDualJoining)
        {
            if(jcCur == kAJCRightJoining)
            {
                pGlyphInfo[i].mScriptFlags = kAGEFFinal;
                continue;
            }
        }

        // R3
        if(jcCur == kAJCLeftJoining)
        {
            if(jcNext == kAJCJoinCausing  ||
               jcNext == kAJCRightJoining ||
               jcNext == kAJCDualJoining)
            {
                pGlyphInfo[i].mScriptFlags = kAGEFInitial;
                continue;
            }
        }

        // R4
        if(jcPrev == kAJCJoinCausing ||
           jcPrev == kAJCLeftJoining ||
           jcPrev == kAJCDualJoining)
        {
            if(jcCur == kAJCDualJoining)
            {
                if(jcNext == kAJCJoinCausing  ||
                   jcNext == kAJCRightJoining ||
                   jcNext == kAJCDualJoining)
                {
                    pGlyphInfo[i].mScriptFlags = kAGEFMedial;
                    continue;
                }
            }
        }

        // R5
        if(jcPrev == kAJCJoinCausing ||
           jcPrev == kAJCLeftJoining ||
           jcPrev == kAJCDualJoining)
        {
            if(jcCur == kAJCDualJoining)
            {
                if(!(jcNext == kAJCJoinCausing  ||
                     jcNext == kAJCRightJoining ||
                     jcNext == kAJCDualJoining))
                {
                    pGlyphInfo[i].mScriptFlags = kAGEFFinal;
                    continue;
                }
            }
        }

        // R6
        if(!(jcPrev == kAJCJoinCausing ||
             jcPrev == kAJCLeftJoining ||
             jcPrev == kAJCDualJoining))
        {
            if(jcCur == kAJCDualJoining)
            {
                if(jcNext == kAJCJoinCausing  ||
                   jcNext == kAJCRightJoining ||
                   jcNext == kAJCDualJoining)
                {
                    pGlyphInfo[i].mScriptFlags = kAGEFInitial;
                    continue;
                }
            }
        }

        // R7
        pGlyphInfo[i].mScriptFlags = kAGEFIsolated;
    }
}


///////////////////////////////////////////////////////////////////////////////
// OTFLookupFlagMatch
//
inline bool OTFLookupFlagMatch(uint32_t glyphClass, uint32_t lookupFlag)
{
    // To do: We need to consider kOTFLookupFlagMarkAttachmentType as well.
    return ((lookupFlag & (1 << glyphClass)) == 0);
}


///////////////////////////////////////////////////////////////////////////////
// CompareGlyphIds
//
// This function compares a string of glyphs in the LineLayout to an input
// pGlyphIdArray. It needs to be able to skip over 'ignored' glyphs in 
// the LineLayout (based on lookupFlags). A typical case of ignoring glyphs
// would be if there was a marking character in the LineLayout which should
// be considered non-existant for the purposes of the comparison.
//
int32_t CompareGlyphIds(const LineLayout& lineLayout, eastl_size_t iGlyph, 
                        const GlyphId* pGlyphIdArray, eastl_size_t glyphIdCount, uint32_t lookupFlags)
{
    const GlyphId*       pBegin = &lineLayout.mGlyphArray[iGlyph];
    const GlyphId* const pEnd   = lineLayout.mGlyphArray.end();
    const GlyphId*       pCur   = pBegin;
    const GlyphInfo*     pGICur = &lineLayout.mGlyphInfoArray[iGlyph];
    eastl_size_t         i;

    for(i = 0; (i < glyphIdCount) && (pCur < pEnd); ++pCur, ++pGICur)
    {
        if(OTFLookupFlagMatch(pGICur->mOpenTypeLookupFlags, lookupFlags))
        {
            // Note that in this function we increment i only if the lineLayout glyph is used (not ignored).
            if(*pCur != pGlyphIdArray[i++])
                return -1; // There was a comparison mismatch. In practice, most of the time this function will exit this way.
        }
    }

    // If the lineLayout ran out of glyphs before pGlyphIdArray, return -1.
    if(i < glyphIdCount)
        return -1;

    // Return the number of glyphs in lineLayout that matched pGlyphIdArray/glyphIdCount.
    // Note that this result could be greater than glyphIdCount if there were ignored 
    // glyphs in the lineLayout.
    return (int32_t)(intptr_t)(pCur - pBegin);
}


///////////////////////////////////////////////////////////////////////////////
// SubstituteGlyphs1
//
// Replaces a single glyph in the lineLayout with one or more glyphs from pGlyphIdArray.
// Such a substitution is a "decomposition" and is not common but not unheard of either
// in complex scripts.
//
void SubstituteGlyphs1(LineLayout& lineLayout, eastl_size_t iGlyph, 
                        const GlyphId* pGlyphIdArray, eastl_size_t glyphIdCount)
{
    // Most commonly, glyphIdCount will be 2.
    EA_ASSERT(glyphIdCount > 0);

    if(glyphIdCount > 1)
    {
        // Need to save this because the insertion operation may resize the glyph info array.
        const GlyphInfo glyphInfoSaved = lineLayout.mGlyphInfoArray[iGlyph];

        lineLayout.mGlyphArray    .insert(lineLayout.mGlyphArray    .begin() + (iGlyph + 1), glyphIdCount - 1, GlyphId());
        lineLayout.mGlyphInfoArray.insert(lineLayout.mGlyphInfoArray.begin() + (iGlyph + 1), glyphIdCount - 1, glyphInfoSaved);

        for(eastl_size_t i = 0; i < glyphIdCount; i++)
        {
            lineLayout.mGlyphArray[i] = pGlyphIdArray[i];

            lineLayout.mGlyphInfoArray[i].mClusterSize      = (unsigned)glyphIdCount; // The new multiple glyphs form a glyph cluster of size 'glyphIdCount'.
            lineLayout.mGlyphInfoArray[i].mClusterPosition  = (unsigned)i;            // The position of this glyph within the cluster is 'i'.
            lineLayout.mGlyphInfoArray[i].mCharCount        = 1;                      // The new multiple glyphs refer to a single initial char/glyph.
            lineLayout.mGlyphInfoArray[i].mClusterBreakable = 0;                      // Since a single glyph is being represented by multiple, the set is indivisible.
        }
    }
    else
    {
        // Else there is a simple 1:1 glyph substitution. This usually won't happen, as such 
        // 1:1 substitutions are usually done directly via Gsub lookup type 1 and never call
        // a heavier function like this one. However, a font artist may in fact have set up 
        // a 1:1 substitution via Gsub lookup types other than type 1.
        lineLayout.mGlyphArray[iGlyph] = *pGlyphIdArray;
    }
}
    

///////////////////////////////////////////////////////////////////////////////
// SubstituteGlyphs2
//
// Replaces multiple glyphs in the lineLayout with one glyphId.
// Such a substitution is a "composition" and it is usually a ligature formation.
// Some languages (e.g. Arabic) have required ligatures, whereby you must implement
// the ligature and it is not optional or decorative like in Western languages.
//
void SubstituteGlyphs2(LineLayout& lineLayout, eastl_size_t iGlyph, eastl_size_t iGlyphCount, 
                        GlyphId glyphId, uint32_t /*lookupFlags*/)
{
    // To do: Need to skip glyphs in lineLayout that are ignored. Such glyphs
    // need to be preserved by copying them to the end of the replaced sequence.
    // This kind of ignoring operation makes writing a text editor maddening.

    // Most commonly, iGlyphCount will be 2.
    EA_ASSERT(iGlyphCount > 0);

    // Erase glyphs after iGlyph, leaving just a single glyph at position iGlyph.
    lineLayout.mGlyphArray    .erase(lineLayout.mGlyphArray    .begin() + (iGlyph + 1), lineLayout.mGlyphArray    .begin() + (iGlyph + iGlyphCount));
    lineLayout.mGlyphInfoArray.erase(lineLayout.mGlyphInfoArray.begin() + (iGlyph + 1), lineLayout.mGlyphInfoArray.begin() + (iGlyph + iGlyphCount));

    // Set the replacement glyph. This glyph replaces the set that was just erased.
    lineLayout.mGlyphArray[iGlyph] = glyphId;

    // Set the new glyph info.
    lineLayout.mGlyphInfoArray[iGlyph].mClusterSize      = 1;                       // This one glyph per multiple chars. In a way, mClusterSize might be thought of as being a fractional value. But we use 1.
    lineLayout.mGlyphInfoArray[iGlyph].mClusterPosition  = 0;                       // There is only one glyph here, so it must be position 0.
    lineLayout.mGlyphInfoArray[iGlyph].mCharCount        = (unsigned)iGlyphCount;   // This is the number of chars this glyph refers to.
    lineLayout.mGlyphInfoArray[iGlyph].mClusterBreakable = 1;                       // Since there are multiple chars that generated this glyph, we se it to be breakable.
}



///////////////////////////////////////////////////////////////////////////////
// DoGlyphSubstitution
//
// Fonts implement a substitution feature by not just having a single table 
// for all substitutions but by having multiple tables, with some of them 
// being in different formats. These tables are created directly by the 
// font artist and unfortunately are often more optimized for the artist's
// convenience than for processing efficiency. See the FontLab OpenType 
// window, for example, to see how artists create OpenType substitutions.
//
eastl_size_t DoGlyphSubstitution(LineLayout& lineLayout, eastl_size_t iGlyph, 
                                 const FeatureLookup& featureLookup, const OTFLookup& otfLookup, const OTF* /*pOTF*/)
{
    const eastl_size_t iGlyphSaved = iGlyph;
    const uint32_t     lookupFlags = otfLookup.mLookupFlags & (kOTFLookupFlagIgnoreBaseGlyphs | kOTFLookupFlagIgnoreLigatures | kOTFLookupFlagIgnoreMarks | kOTFLookupFlagMarkAttachmentType);
    GlyphInfo&         glyphInfo   = lineLayout.mGlyphInfoArray[iGlyph];

    if(((featureLookup.mGlyphFlags & glyphInfo.mScriptFlags) == 0) &&       // This is an Arabic glyph property check.
        OTFLookupFlagMatch(glyphInfo.mOpenTypeLookupFlags, lookupFlags))    // This is an OpenType glyph class check. This is separate because we use OTFLookupFlagMatch elsewhere. 
    {
        const uint32_t lookupType = otfLookup.mLookupType;

        // For each sub-table... (at most one should handle the given glyph)
        for(eastl_size_t i = 0; i < otfLookup.mSubTableOffsetCount; i++)
        {
            const OTFLookupSubTableGsub& gsub            = otfLookup.mSubTable.mpGsubArray[i];
            const GlyphId                glyphIdOriginal = lineLayout.mGlyphArray[iGlyph];
            const int32_t                iCoverage       = gsub.mCoverage.GetCoverageIndex(glyphIdOriginal);

            if(iCoverage >= 0) // If this sub-table handles the glyph...
            {
                switch (lookupType)
                {
                    case 1: // (single glyph converted to another glyph)
                    {
                        if(gsub.mFormat == 2)
                            lineLayout.mGlyphArray[iGlyph] = gsub.mLookup.mSingle2.mpSubstitutionArray[iCoverage];
                        else
                            lineLayout.mGlyphArray[iGlyph] = (GlyphId)(lineLayout.mGlyphArray[iGlyph] + gsub.mLookup.mSingle1.mDeltaGlyphId);
                        iGlyph++;
                        break;
                    }

                    case 2: // (single glyph expanded to multiple glyphs)
                    {
                        EA_ASSERT(gsub.mFormat == 1);

                        if(gsub.mFormat == 1)
                        {
                            const OTFSequence& s = gsub.mLookup.mMultiple1.mpSequenceArray[iCoverage];

                            // We insert (s.mSubstitutionCount) chars from (s.mpSubstitutionArray) into lineLayout at iGlyph.
                            SubstituteGlyphs1(lineLayout, iGlyph, s.mpSubstitutionArray, s.mSubstitutionCount);
                            iGlyph += s.mSubstitutionCount;
                        }
                        break;
                    }

                    case 4: // (multiple glyphs condensed to a single glyph)
                    {
                        EA_ASSERT(gsub.mFormat == 1);

                        if((gsub.mFormat == 1) && ((iGlyph + 1) < lineLayout.mGlyphArray.size()))
                        {
                            const OTFGsubLigature1& ligature1   = gsub.mLookup.mLigature1;
                            const OTFLigatureSet&   ligatureSet = ligature1.mpLigatureSetArray[iCoverage];

                            for(uint32_t j = 0; j < ligatureSet.mLigatureCount; j++)
                            {
                                // If the current glyph array matches ligature.mpComponentArray, 
                                // replace the glyphs in the glyph array with ligature.mGlyph.
                                const OTFLigature& ligature   = ligatureSet.mpLigatureArray[j];
                                const int32_t      matchCount = CompareGlyphIds(lineLayout, iGlyph + 1, ligature.mpComponentArray, ligature.mComponentCount - 1, lookupFlags);

                                if(matchCount >= 0)
                                {
                                    SubstituteGlyphs2(lineLayout, iGlyph, matchCount + 1, ligature.mGlyph, lookupFlags);
                                    iGlyph += 1;
                                    break;
                                }
                            }
                        }
                        break;
                    }

                    case 5: // (substitution of glyphs based on their context)
                    {
                        // Currently we handle only format 2. We'll do the others as-needed.
                        EA_ASSERT(gsub.mFormat == 2);

                        if(gsub.mFormat == 2)
                        {
                            // To do
                        }
                        break;
                    }

                    case 6: // (substitution of glyphs based on their context)
                    {
                        // Currently we handle only format 2. We'll do the others as-needed.
                        EA_ASSERT(gsub.mFormat == 2);

                        if(gsub.mFormat == 2)
                        {
                            // To do
                        }
                        break;
                    }

                    default:
                        continue;
                }

                break;
            }
        }
    }

    // If the glyph was not handled by the above logic, just skip the glyph.
    if(iGlyph == iGlyphSaved)
        ++iGlyph;

    return iGlyph;
}


///////////////////////////////////////////////////////////////////////////////
// DoGlyphSubstitution
//
void DoGlyphSubstitution(LineLayout& lineLayout, eastl_size_t iGlyphBegin, const FeatureLookupArray& featureLookupArray, const OTF* pOTF)
{
    // Glyph substitution is necessarily an O(n^3) operation. There is no way
    // around this and glyph substitution is mandatory for scripts such 
    // as Arabic and Devanagari. The only saving grace is that the n values
    // tend to be low (< 5).

    // For each feature...
    for(eastl_size_t f = 0, fEnd = featureLookupArray.size(); f < fEnd; f++) // 'f' refers to "feature".
    {
        const FeatureLookup& featureLookup = featureLookupArray[f];

        // For each feature's lookup...
        if(!featureLookup.mLookupPtrArray.empty()) // If the feature is implemented...
        {
            for(eastl_size_t i = 0, iEnd = featureLookup.mLookupPtrArray.size(); i < iEnd; i++)
            {
                const OTFLookup& otfLookup = *featureLookup.mLookupPtrArray[i];

                // For each glyph...
                for(eastl_size_t iGlyph = iGlyphBegin; iGlyph < lineLayout.mGlyphArray.size(); ) // Intentionally re-evaluate size every time through the loop, as it might change.
                    iGlyph = DoGlyphSubstitution(lineLayout, iGlyph, featureLookup, otfLookup, pOTF);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// CompleteLineLayoutArrays
//
void Typesetter::CompleteLineLayoutArrays(eastl_size_t iCharBegin, eastl_size_t iCharEnd, eastl_size_t iGlyphBegin)
{
    // Upon entry into this function, our mCharArray and mAnalysisInfoArray should
    // be in sync with each other. And mGlyphArray and mGlyphInfoArray should be in
    // sync with each other. But the other arrays haven't been updated yet.

    EA_ASSERT(mLineLayout.mAnalysisInfoArray.size() == mLineLayout.mCharArray.size());
    EA_ASSERT(mLineLayout.mGlyphArray.size()        == mLineLayout.mGlyphInfoArray.size());

    mLineLayout.mGlyphIndexArray.resize(iCharEnd);
    mLineLayout.mCharIndexArray.resize(mLineLayout.mGlyphInfoArray.size());
    mLineLayout.mGlyphLayoutInfoArray.resize(mLineLayout.mGlyphInfoArray.size());

    eastl_size_t iChar     = iCharBegin;
    eastl_size_t iGlyph    = iGlyphBegin;
    eastl_size_t iGlyphEnd = mLineLayout.mGlyphInfoArray.size();
    eastl_size_t i;

    while(iGlyph < iGlyphEnd)
    {
        GlyphInfo&       gi  = mLineLayout.mGlyphInfoArray[iGlyph];
        GlyphLayoutInfo& gli = mLineLayout.mGlyphLayoutInfoArray[iGlyph];
  
        // gi.mCharCount     // Multiple chars per glyph.
        // gi.mClusterSize   // Multiple glyphs per char.

        for(i = 0; i < gi.mCharCount; i++)
        {
            mLineLayout.mGlyphIndexArray[iChar + i] = (uint32_t)iGlyph;

            if(EA_LIKELY(mLineLayout.mCharArray[iChar + i] != kCharOBJ))
            {
                gli.mpFont         = mLineLayout.mAnalysisInfoArray[iChar + i].mpFont; // Note that gli.mpFont is a union of mpFont and mpObject.
                gi.mbGlyphIsObject = 0;
            }
            else
            {
                const Item* const pItem = GetScheduleItemFromCharIndex(iChar + i, true); // iChar is line-relative.
                gli.mpObject       = pItem->mObject.mpObjectPtr; // Note that gli.mpFont is a union of mpFont and mpObject.
                gi.mbGlyphIsObject = 1;
            }
        }

        for(i = 0; i < gi.mClusterSize; i++)
            mLineLayout.mCharIndexArray[iGlyph + i] = (uint32_t)iChar;

        iGlyph += gi.mClusterSize;
        iChar  += gi.mCharCount;
    }

    // The GlyphInfo data should match the char and glyph string lengths.
    EA_ASSERT((iChar == iCharEnd) && (iGlyph == iGlyphEnd));

    // Verify that the above code put the arrays in their expected sizes.
    // We maintain mCharArray and mGlyphIndexArray in parallel.
    // We maintain mGlyphArray, mGlyphInfoArray, mGlyphLayoutInfoArray, and mCharIndexArray in parallel.
    EA_ASSERT(mLineLayout.mGlyphIndexArray.size() == iCharEnd);
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphInfoArray.size());
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mGlyphLayoutInfoArray.size());
    EA_ASSERT(mLineLayout.mGlyphArray.size() == mLineLayout.mCharIndexArray.size());
}



///////////////////////////////////////////////////////////////////////////////
// SetupArabicGsubLookup
//
// To do: Find a way to cache this for a given OTF.
//
void SetupArabicGsubLookup(FeatureLookupArray& featureLookupArray, const OTF* pOTF)
{
    featureLookupArray.resize(10);

    featureLookupArray[0].mFeatureTag = kOTFTag_ccmp;
    featureLookupArray[0].mGlyphFlags = 0xffffffff;

    featureLookupArray[1].mFeatureTag = kOTFTag_isol;
    featureLookupArray[1].mGlyphFlags = kAGTFIsolated;

    featureLookupArray[2].mFeatureTag = kOTFTag_fina;
    featureLookupArray[2].mGlyphFlags = kAGTFFinal;

    featureLookupArray[3].mFeatureTag = kOTFTag_medi;
    featureLookupArray[3].mGlyphFlags = kAGTFMedial;

    featureLookupArray[4].mFeatureTag = kOTFTag_init;
    featureLookupArray[4].mGlyphFlags = kAGTFInitial;

    featureLookupArray[5].mFeatureTag = kOTFTag_rlig;
    featureLookupArray[5].mGlyphFlags = 0x00000000;

    featureLookupArray[6].mFeatureTag = kOTFTag_calt;
    featureLookupArray[6].mGlyphFlags = 0xffffffff;

    featureLookupArray[7].mFeatureTag = kOTFTag_liga;
    featureLookupArray[7].mGlyphFlags = 0x00000000;

    featureLookupArray[8].mFeatureTag = kOTFTag_cswh;
    featureLookupArray[8].mGlyphFlags = 0xffffffff;

    featureLookupArray[9].mFeatureTag = kOTFTag_mset;
    featureLookupArray[9].mGlyphFlags = 0xffffffff;

    pOTF->mGsub.BuildFeatureLookup(kOTFTag_arab, kOTFTag_0000, featureLookupArray);
}


// AppendArabicGlyphCluster
//
// This function differs from other versions of AppendXXXCluster in Typesetter 
// because this version updates only the GlyphArray and the GlyphInfoArray.
// It does this because it needs to perform substitutions on the glyphs before
// laying them out and making mappings between glyphs and the source Unicode chars.
//
void Typesetter::AppendArabicGlyphCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                          const Char* pCharCluster, eastl_size_t charClusterSize, 
                                          const GlyphId* pGlyphCluster, eastl_size_t glyphClusterSize, 
                                          int embeddingLevel, const OTF* pOTF)
{
    (void)charClusterSize; // Prevent compiler warnings.
    (void)iCharBegin;
    EA_ASSERT(charCount && charClusterSize && glyphClusterSize);

    for(eastl_size_t g = 0; g < glyphClusterSize; g++)
    {
        const GlyphId glyphId = pGlyphCluster[g];

        // Add a GlyphId entry. This will refer to the glyph provided by the Font.
        mLineLayout.mGlyphArray.push_back(glyphId);

        // Add a GlyphInfo entry. This will store logical information about the glyph.
        mLineLayout.mGlyphInfoArray.push_back();
        GlyphInfo& gi = mLineLayout.mGlyphInfoArray.back();

        gi.mGJC                 = kGJCNone;                          // We set mGJC to something else later if and when we do justification.
        gi.mClusterPosition     = (unsigned)g;                       // If we were to have multiple glyphs that are together visually, 
        gi.mClusterSize         = (unsigned)glyphClusterSize;        // we would revise mClusterPosition and mClusterSize. We would need to
        gi.mClusterBreakable    = 0;                                 // do this revision later, when we have all glyphs shaped for a line or paragraph.
        gi.mCharCount           = (unsigned)charCount;               // gi.mCharCount currently has only two bits of data.
        gi.mDirection           = embeddingLevel;                    // 
        gi.mbGlyphIsObject      = *pCharCluster != kCharOBJ;         //
        gi.mOpenTypeLookupFlags = (uint8_t)pOTF->mGdef.mGlyphClassDef.GetGlyphClass(glyphId);
    }
}



// ShapeArabic
//
void Typesetter::ShapeArabic(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    EA_ASSERT(iCharEnd <= mLineLayout.mCharArray.size());

    const OTF* const pOTF = mLineLayout.mAnalysisInfoArray[0].mpFont->GetOTF();

    // We more or less need to have OpenType font information in order to correctly display Arabic.
    if(pOTF && pOTF->IsScriptSupported("arab"))
    {
        const eastl_size_t iGlyphBegin = mLineLayout.GetGlyphIndexFromCharIndex(iCharBegin);

        for(eastl_size_t i = iCharBegin, charCount = 0; i < iCharEnd; i += charCount)
        {
            const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[i];

            Char         pCharCluster[kMaxArabicCharClusterSize];
            eastl_size_t charClusterSize = 0;

            charCount = GetGeneralCharCluster(i, iCharEnd, pCharCluster, charClusterSize);

            for(eastl_size_t c = 0, charsEaten = 0, glyphCount = 0, glyphCountPrev = 0; c < charClusterSize; c += charsEaten)
            {
                GlyphId pGlyphIdArray[kMaxArabicGlyphClusterSize];

                charsEaten = GetGlyphsForChar(pCharCluster + c, charClusterSize - c, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);

                AppendArabicGlyphCluster(iCharBegin, charCount, pCharCluster + c, charsEaten, 
                                        pGlyphIdArray + glyphCountPrev, glyphCount - glyphCountPrev, 
                                        pAnalysisInfo->mnBidiLevel, pOTF);

                glyphCountPrev = glyphCount;
            }
        }

        // Do OpenType substitutions.
        FeatureLookupArray featureLookupArray;

        SetupArabicGsubLookup(featureLookupArray, pOTF);
        AssignArabicCharProperties(&mLineLayout.mCharArray[iCharBegin], iCharEnd - iCharBegin, &mLineLayout.mGlyphInfoArray[iGlyphBegin]);
        DoGlyphSubstitution(mLineLayout, iGlyphBegin, featureLookupArray, pOTF);
        CompleteLineLayoutArrays(iCharBegin, iCharEnd, iGlyphBegin);
        PlaceGeneralGlyphCluster(iCharBegin, iCharEnd - iCharBegin);
    }
    else
    {
        // Generic fallback. Arabic text will look wrong, but at least something will be displayed.
        ShapeGeneral(iCharBegin, iCharEnd);
    }
}



#undef U
#undef C
#undef D
#undef L
#undef R
#undef T


} // namespace Text

} // namespace EA


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

