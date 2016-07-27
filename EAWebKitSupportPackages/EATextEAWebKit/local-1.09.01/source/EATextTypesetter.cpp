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

#include <EAText/EATextTypesetter.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextBreak.h>
#include <EAText/internal/EATextBidi.h>
#include <EASTL/fixed_hash_set.h>
#include <EASTL/fixed_list.h>
#include <EAText/internal/StdC.h>
#include EA_ASSERT_HEADER
#include <float.h>
#include <limits.h>
#include <math.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


#ifndef EATEXT_DEBUG
    #undef  Validate
    #define Validate()
#endif

// Useful during unit testing:
// extern int EATextTest_CheckMemory_Imp(const char* pFile, int nLine);


namespace EA
{

namespace Text
{


/// gDefaultLayoutSettings
///
/// Defines a default text LayoutSettings for the system.
///
EATEXT_API LayoutSettings& GetDefaultLayoutSettings()
{
    static LayoutSettings gDefaultLayoutSettings;

    return gDefaultLayoutSettings;
}


namespace
{
    /// kMaxAnalysisSize
    ///
    /// For now we cap the max analysis size. Otherwise the user could kill us 
    /// with a giant block of data. What we want to do in the long run is try 
    /// to use this max analysis size but if the current line space is very 
    /// long then we can dynamically increase the analysis size as we need for
    /// a given line. We can alternatively declare that a given line of text
    /// has a maximum Unicode character length.
    ///
    const eastl_size_t kMaxAnalysisSize = 16384;


    /// SpanList
    ///
    /// Used to store the visual extents of a span of glyphs.
    ///
    typedef eastl::fixed_list<Span, 16, true> SpanList;


    inline bool SpansOverlap(const Span& a, const Span& b)
    {
        return ((a.mfEnd + 0.2f) > b.mfBegin) && // The 0.2f is floating point slop.
               ((b.mfEnd + 0.2f) > a.mfBegin);
    }


    inline void MakeSpanUnion(Span& a, const Span& b)
    {
        a.mfBegin = eastl::min_alt(a.mfBegin, b.mfBegin);
        a.mfEnd   = eastl::max_alt(a.mfEnd,   b.mfEnd);
    }


    void AddSpanToSpanList(const Span& span, SpanList& spanList)
    {
        // This implementation may appear to be algorithm of order (n^2) (i.e. slow),
        // but in practice it isn't slow because there are usually only no more than 
        // about 1 or 2 spans in the spanList. In pathological cases that don't appear
        // in this kind of typography, this kind of function might be less than optimal.

        for(SpanList::iterator it = spanList.begin(), itEnd = spanList.end(); it != itEnd; ++it)
        {
            Span spanCurrent = *it;

            if(SpansOverlap(spanCurrent, span))
            {
                MakeSpanUnion(spanCurrent, span);
                spanList.erase(it);
                return AddSpanToSpanList(spanCurrent, spanList);
            }
        }

        // There was no overlap, so add the span to the list.
        spanList.push_back(span);
    }


    void AddRectangleToRectangleSet(const Rectangle& rect, RectangleSet& rectangleSet)
    {
        // Similar to AddSpanToSpanList except spans aren't union-ed unless they 
        // have the same top/bottom values (not just vertically overlap).
        Rectangle unionRect(rect);

        // Search for and combine all spans that overlap with the new span
        for(RectangleSet::iterator it = rectangleSet.begin(); it != rectangleSet.end(); )
        {
            if((it->mTop == unionRect.mTop) && (it->mBottom == unionRect.mBottom))
            {
                if((it->mLeft <= unionRect.mRight) && (it->mRight >= unionRect.mLeft))
                {
                    if(it->mLeft < unionRect.mLeft)
                        unionRect.mLeft = it->mLeft;

                    if(it->mRight > unionRect.mRight)
                        unionRect.mRight = it->mRight;

                    it = rectangleSet.erase(it);
                    continue;
                }
            }

            ++it;
        }

        // Re-insert the merged span.
        rectangleSet.insert(unionRect);
    }


    const GlyphLayoutInfo* GetNearestTextGlyphLayoutInfo(const GlyphInfo* pGI, const GlyphLayoutInfo* pGLIBegin, 
                                                                const GlyphLayoutInfo* pGLI, const GlyphLayoutInfo* pGLIEnd)
    {
        const GlyphLayoutInfo* pGLICurrent;
        const GlyphInfo*       pGICurrent;

        // Check backwards
        for(pGLICurrent = pGLI, pGICurrent = pGI; pGLICurrent >= pGLIBegin; --pGLICurrent, --pGICurrent)
        {
            if(!pGICurrent->mbGlyphIsObject)
                return pGLICurrent;
        }

        // Check forwards
        for(pGLICurrent = pGLI, pGICurrent = pGI; pGLICurrent < pGLIEnd; ++pGLICurrent, ++pGICurrent)
        {
            if(!pGICurrent->mbGlyphIsObject)
                return pGLICurrent;
        }

        return NULL; // Either the text is empty or the text is all objects and no text.
    }


} // namespace



///////////////////////////////////////////////////////////////////////////////
// LayoutSettings
///////////////////////////////////////////////////////////////////////////////

LayoutSettings::LayoutSettings()
  : mScript(kScriptLatin),
    mDirection(kDirectionDefault),
    mbDynamicFontSelection(true),
    mTextStyleDefault(),
    mbEnableAutoHyphenation(false), 
    mbCellTopPosition(true),
    mfYScale(+1.f),
    mbEnableLigatures(false), 
    mbHideControlCharacters(false),
    mbDisplayZWG(false)
{
    memset(mLocale, 0, sizeof(mLocale));
}


LayoutSettings::LayoutSettings(const LayoutSettings& x)
{
    operator=(x);
}


LayoutSettings& LayoutSettings::operator=(const LayoutSettings& x)
{
    memcpy(mLocale, x.mLocale, sizeof(mLocale));

    mDirection              = x.mDirection;
    mScript                 = x.mScript;
    mFontSelection          = x.mFontSelection;
    mbDynamicFontSelection  = x.mbDynamicFontSelection;
    mTextStyleDefault       = x.mTextStyleDefault;
    mbEnableAutoHyphenation = x.mbEnableAutoHyphenation;
    mbCellTopPosition       = x.mbCellTopPosition;
    mfYScale                = x.mfYScale;
    mbEnableLigatures       = x.mbEnableLigatures;
    mbHideControlCharacters = x.mbHideControlCharacters;
    mbDisplayZWG            = x.mbDisplayZWG;

    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// LineLayout
///////////////////////////////////////////////////////////////////////////////

LineLayout::LineLayout(Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
    mCharArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/LineLayout", mpCoreAllocator)),
    mAnalysisInfoArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/AnalysisInfoArray", mpCoreAllocator)),
    mGlyphArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/GlyphArray", mpCoreAllocator)),
    mGlyphInfoArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/GlyphInfoArray", mpCoreAllocator)),
    mGlyphLayoutInfoArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/GlyphLayoutInfoArray", mpCoreAllocator)),
    mGlyphIndexArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/GlyphIndexArray", mpCoreAllocator)),
    mCharIndexArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "Typesetter/CharIndexArray", mpCoreAllocator)),
    mLineMetrics(),
    mLineCount(),
    mCharScheduleIndex(0),
    mbSimpleLayout(false)
{
    memset(&mLineMetrics, 0, sizeof(mLineMetrics));
}


LineLayout::LineLayout(const LineLayout& x)
{
    // We do not copy mpCoreAllocator; we copy only the contents.
    mCharArray              = x.mCharArray;
    mAnalysisInfoArray      = x.mAnalysisInfoArray;
    mGlyphArray             = x.mGlyphArray;
    mGlyphInfoArray         = x.mGlyphInfoArray;
    mGlyphLayoutInfoArray   = x.mGlyphLayoutInfoArray;
    mGlyphIndexArray        = x.mGlyphIndexArray;
    mCharIndexArray         = x.mCharIndexArray;
    mLineMetrics            = x.mLineMetrics;
    mLineCount              = x.mLineCount;
    mCharScheduleIndex      = x.mCharScheduleIndex;
    mbSimpleLayout          = x.mbSimpleLayout;
}


void LineLayout::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;

    mCharArray           .get_allocator().set_allocator(pCoreAllocator);
    mAnalysisInfoArray   .get_allocator().set_allocator(pCoreAllocator);
    mGlyphArray          .get_allocator().set_allocator(pCoreAllocator);
    mGlyphInfoArray      .get_allocator().set_allocator(pCoreAllocator);
    mGlyphLayoutInfoArray.get_allocator().set_allocator(pCoreAllocator);
    mGlyphIndexArray     .get_allocator().set_allocator(pCoreAllocator);
    mCharIndexArray      .get_allocator().set_allocator(pCoreAllocator);
}


void LineLayout::Clear(bool /*bThisArgumentIsDeprecated*/)
{
    mCharScheduleIndex = 0;
    mLineCount = 0;

    NewLine();
}


void LineLayout::NewLine()
{
    mCharArray.clear();
    mAnalysisInfoArray.clear();

    NewGlyphs();
}


void LineLayout::NewGlyphs()
{
    mGlyphArray.clear();
    mGlyphInfoArray.clear();
    mGlyphLayoutInfoArray.clear();
    mGlyphIndexArray.clear();
    mCharIndexArray.clear();
    memset(&mLineMetrics, 0, sizeof(mLineMetrics));
    mbSimpleLayout = false;
}


///////////////////////////////////////////////////////////////////////////////
// Assign
//
void LineLayout::Assign(const LineLayout& lineLayout, int componentFlags)
{
    mpCoreAllocator = lineLayout.mpCoreAllocator;

    if(componentFlags & kComponentCharArray)
        mCharArray = lineLayout.mCharArray;
    else
        mCharArray.clear();

    if(componentFlags & kComponentCharAnalysis)
        mAnalysisInfoArray = lineLayout.mAnalysisInfoArray;
    else
        mAnalysisInfoArray.clear();

    if(componentFlags & kComponentGlyphArray)
        mGlyphArray = lineLayout.mGlyphArray;
    else
        mGlyphArray.clear();

    if(componentFlags & kComponentGlyphInfoArray)
        mGlyphInfoArray = lineLayout.mGlyphInfoArray;
    else
        mGlyphInfoArray.clear();

    if(componentFlags & kComponentGlyphLayoutInfoArray)
        mGlyphLayoutInfoArray = lineLayout.mGlyphLayoutInfoArray;
    else
        mGlyphLayoutInfoArray.clear();

    if(componentFlags & kComponentGlyphIndexArray)
        mGlyphIndexArray = lineLayout.mGlyphIndexArray;
    else
        mGlyphIndexArray.clear();

    if(componentFlags & kComponentCharIndexArray)
        mCharIndexArray = lineLayout.mCharIndexArray;
    else
        mCharIndexArray.clear();

    mLineMetrics       = lineLayout.mLineMetrics;
    mLineCount         = lineLayout.mLineCount;
    mCharScheduleIndex = lineLayout.mCharScheduleIndex;    
}


void LineLayout::Append(const LineLayout& lineLayout, int componentFlags)
{
    // This function does not increment the mLineCount member variable,
    // as it isn't necessarily so that the appended layout represents an 
    // independent line.

    // LineMetrics
    mLineMetrics.mnLineLength        += lineLayout.mLineMetrics.mnLineLength;
    mLineMetrics.mnVisibleLineLength  = 0; // This number is nearly meaningless across multiple lines.
    mLineMetrics.mfSpace              = eastl::max_alt(mLineMetrics.mfSpace,        lineLayout.mLineMetrics.mfSpace);
    mLineMetrics.mfVisibleSpace       = eastl::max_alt(mLineMetrics.mfVisibleSpace, lineLayout.mLineMetrics.mfVisibleSpace);
    mLineMetrics.mfBaseline           = eastl::max_alt(mLineMetrics.mfBaseline,     lineLayout.mLineMetrics.mfBaseline);
    mLineMetrics.mfDescent            = eastl::min_alt(mLineMetrics.mfDescent,      lineLayout.mLineMetrics.mfDescent);

    // Arrays
    // Do kComponentGlyphIndexArray and kComponentCharIndexArray before 
    // other flags, as we'll change the array size.

    if((componentFlags & kComponentGlyphIndexArray) &&
       (componentFlags & kComponentCharArray)       && 
       (componentFlags & kComponentGlyphArray))
    {
        // Append to mGlyphIndexArray.
        const eastl_size_t nGlyphArraySize      = mGlyphArray.size();
        const eastl_size_t nGlyphIndexArraySize = mGlyphIndexArray.size();

        mGlyphIndexArray.insert(mGlyphIndexArray.end(), lineLayout.mGlyphIndexArray.begin(), lineLayout.mGlyphIndexArray.end());

        for(index_t* pCurrent = mGlyphIndexArray.data() + nGlyphIndexArraySize, *pEnd = pCurrent + lineLayout.mGlyphIndexArray.size(); pCurrent < pEnd; pCurrent++)
            *pCurrent += nGlyphArraySize;
    }

    if((componentFlags & kComponentCharIndexArray) &&
       (componentFlags & kComponentCharArray)      && 
       (componentFlags & kComponentGlyphArray))
    {
        // Append to mCharIndexArray.
        const eastl_size_t nCharArraySize      = mCharArray.size();
        const eastl_size_t nCharIndexArraySize = mCharIndexArray.size();

        mCharIndexArray.insert(mCharIndexArray.end(), lineLayout.mCharIndexArray.begin(), lineLayout.mCharIndexArray.end());

        for(index_t* pCurrent = mCharIndexArray.data() + nCharIndexArraySize, *pEnd = pCurrent + lineLayout.mCharIndexArray.size(); pCurrent < pEnd; pCurrent++)
            *pCurrent += nCharArraySize;
    }

    if(componentFlags & kComponentCharArray)
        mCharArray += lineLayout.mCharArray;

    if(componentFlags & kComponentCharAnalysis)
        mAnalysisInfoArray.insert(mAnalysisInfoArray.end(), lineLayout.mAnalysisInfoArray.begin(), lineLayout.mAnalysisInfoArray.end());

    if(componentFlags & kComponentGlyphArray)
        mGlyphArray.insert(mGlyphArray.end(), lineLayout.mGlyphArray.begin(), lineLayout.mGlyphArray.end());

    if(componentFlags & kComponentGlyphInfoArray)
        mGlyphInfoArray.insert(mGlyphInfoArray.end(), lineLayout.mGlyphInfoArray.begin(), lineLayout.mGlyphInfoArray.end());

    if(componentFlags & kComponentGlyphLayoutInfoArray)
        mGlyphLayoutInfoArray.insert(mGlyphLayoutInfoArray.end(), lineLayout.mGlyphLayoutInfoArray.begin(), lineLayout.mGlyphLayoutInfoArray.end());
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphIndexFromCharIndex
//
index_t LineLayout::GetGlyphIndexFromCharIndex(index_t iChar) const
{
    if(iChar < mGlyphIndexArray.size())
    {
        EA_ASSERT(mGlyphIndexArray[iChar] < mGlyphArray.size());
        return mGlyphIndexArray[iChar];
    }

    // May want to assert that iChar <= mGlyphIndexArray.size().
    // Is it bad if the user references a char that is two indexes beyond the end?
    return mGlyphArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetCharIndexFromGlyphIndex
//
index_t LineLayout::GetCharIndexFromGlyphIndex(index_t iGlyph) const
{
    if(iGlyph < mCharIndexArray.size())
    {
        EA_ASSERT(mCharIndexArray[iGlyph] < mCharArray.size());
        return mCharIndexArray[iGlyph];
    }

    // May want to assert that iGlyph <= mCharIndexArray.size().
    // Is it bad if the user references a glyph that is two indexes beyond the end?
    return mCharArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphRangeFromCharRange
//
void LineLayout::GetGlyphRangeFromCharRange(index_t  iCharBegin,  index_t  iCharEnd, 
                                            index_t& iGlyphBegin, index_t& iGlyphEnd) const
{
    iGlyphBegin = GetGlyphIndexFromCharIndex(iCharBegin);
    iGlyphEnd   = GetGlyphIndexFromCharIndex(iCharEnd);
}


///////////////////////////////////////////////////////////////////////////////
// GetCharRangeFromGlyphRange
//
void LineLayout::GetCharRangeFromGlyphRange(index_t  iGlyphBegin, index_t  iGlyphEnd, 
                                            index_t& iCharBegin,  index_t& iCharEnd) const
{
    iCharBegin = GetCharIndexFromGlyphIndex(iGlyphBegin);
    iCharEnd   = GetCharIndexFromGlyphIndex(iGlyphEnd);
}


///////////////////////////////////////////////////////////////////////////////
// OffsetSegment
//
void LineLayout::OffsetSegment(float fDeltaX, float fDeltaY, index_t iGlyphBegin, index_t iGlyphEnd)
{
    if((fabs(fDeltaX) > 0.1f) || // We add some slop because a line can get moved around
       (fabs(fDeltaY) > 0.1f))   // and some precision could be lost in the process.
    {
        GlyphLayoutInfo* pGLI    = mGlyphLayoutInfoArray.begin() + iGlyphBegin;
        GlyphLayoutInfo* pGLIEnd = mGlyphLayoutInfoArray.begin() + iGlyphEnd;

        while(pGLI < pGLIEnd)
        {
            pGLI->mfPenX += fDeltaX;
            pGLI->mfPenY += fDeltaY;
            pGLI->mfX1   += fDeltaX;
            pGLI->mfY1   += fDeltaY;
            pGLI->mfX2   += fDeltaX;
            pGLI->mfY2   += fDeltaY;

            ++pGLI;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// CalculateVisibleSpace
//
// This function calculates:
//     mLineMetrics.mnVisibleLineLength
//     mLineMetrics.mfVisibleSpace
//
void LineLayout::CalculateVisibleSpace()
{
    eastl_size_t iEnd; 

    // Calculate which character is the last space character (i.e. mnVisibleLineLength).
    // If this assertion fails, it's usually because mCharArray is empty. Were you using 
    // the LayoutSimple function and didn't specify kComponentCharArray?
    EA_ASSERT(mLineMetrics.mnLineLength <= mCharArray.size());

    for(iEnd = mLineMetrics.mnLineLength; iEnd > 0; --iEnd)
    {
        const Char c = mCharArray[iEnd - 1];

        if((c != 0x0020) && (c != 0x00A0) && (c != 0x3000)) // 0x00A0 -> kCharNBSP; 0x3000 -> kCharIDSP
            break;
    }

    mLineMetrics.mnVisibleLineLength = iEnd;

    // Calculate what the extent of the preceding glyph is (i.e. mfVisibleSpace).
    const eastl_size_t iGlyphBegin  = 0;
    const eastl_size_t iGlyphEnd    = GetGlyphIndexFromCharIndex(iEnd);

    if(iGlyphEnd)
    {
        // This isn't correct, though it will work most of the time. We need to go back and find the base char of clusters.
        GlyphLayoutInfo& gliFirst   = mGlyphLayoutInfoArray[iGlyphBegin];
        GlyphLayoutInfo& gliLast    = mGlyphLayoutInfoArray[iGlyphEnd - 1];
        mLineMetrics.mfVisibleSpace = ((gliLast.mfAdvance > 0) ? (gliLast.mfPenX + gliLast.mfAdvance) : gliLast.mfPenX) - gliFirst.mfPenX;
    }
    else
        mLineMetrics.mfVisibleSpace = 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetTextPositionFromDisplayPosition
//
index_t LineLayout::GetTextPositionFromDisplayPosition(float xPosition, float /*yPosition*/, 
                                                       bool bRequireGlyphHit, PickType pickType, 
                                                       GlyphSide& glyphSide, Direction layoutDirection) const
{
    // We walk the string linearly from beginning to end. 
    // Usually these strings are only 10 to 50 characters in length.
    // We need to deal with RTL text.
    // We need to deal with clusters that are breakable, such as ligatures and some complex script glyphs.

    index_t iChar = kIndexInvalid;

    // These variables serve to help us deduce an answer when the 
    // input xPosition is outside the range of the glyphs in the lineLayout.
    Span span = { FLT_MAX, -FLT_MAX };

    // Test each glyph in turn.
    for(index_t iGlyph = 0, iGlyphEnd = mGlyphLayoutInfoArray.size(); iGlyph < iGlyphEnd; ++iGlyph)
    {
        // To do: We need to deal with grapheme clusters. If fRelativeLogicalPosition > 0.5 then
        // we want to use the end side of the last glyph in the grapheme cluster. I think the easiest
        // way to deal with this is to get the grapheme cluster here and make a union of its individual
        // glyphs' bounding boxes and test against that.

        const GlyphLayoutInfo* const pGlyphLayoutInfo = &mGlyphLayoutInfoArray[iGlyph];

        float       fRelativeLogicalPosition = -1.f;
        const float fPositionBegin           = pGlyphLayoutInfo->mfPenX;
        const float fPositionEnd             = pGlyphLayoutInfo->mfPenX + pGlyphLayoutInfo->mfAdvance;

        if(fPositionBegin < fPositionEnd) // If the glyph is LTR...
        {
            //bTextIsLTR = true;

            if(fPositionBegin < span.mfBegin)
                span.mfBegin = fPositionBegin;

            if(fPositionEnd > span.mfEnd)
                span.mfEnd = fPositionEnd;

            if((xPosition >= fPositionBegin) && (xPosition < fPositionEnd))
            {
                fRelativeLogicalPosition = ((xPosition - fPositionBegin) / (fPositionEnd - fPositionBegin));
                glyphSide = ((fRelativeLogicalPosition < 0.5f) ? kGlyphSideBefore : kGlyphSideAfter);
            }
        }
        else if(fPositionBegin > fPositionEnd) // If the glyph is RTL...
        {
            //bTextIsLTR = false;

            if(fPositionEnd < span.mfBegin)
                span.mfBegin = fPositionEnd;

            if(fPositionBegin > span.mfEnd)
                span.mfEnd = fPositionBegin;

            if((xPosition >= fPositionEnd) && (xPosition < fPositionBegin))
            {
                // We subtract 1.f from this because we want the logical position of the text as opposed to
                // the visual position of the text. Thus if xPosition is at the right side of the glyph
                // in RTL text, we want fRelativeLogicalPosition to be 0.0 for that glyph and not 1.0.
                fRelativeLogicalPosition = (1.f - ((xPosition - fPositionEnd) / (fPositionBegin - fPositionEnd)));
                glyphSide = ((fRelativeLogicalPosition < 0.5f) ? kGlyphSideBefore : kGlyphSideAfter);
            }
        }

        if(fRelativeLogicalPosition != -1.f) // If a hit was achieved above...
        {
            // To do: We need to deal with the possibility that the char is a ligature.
            // If so, then we want to use fRelativeLogicalPosition to tell which char we 
            // want to return to the user.

            if((pickType == kPickTypeInsertion) && (glyphSide == kGlyphSideAfter))
            {
                glyphSide = kGlyphSideBefore;
                ++iGlyph;
                EA_ASSERT(iGlyph <= mGlyphLayoutInfoArray.size());
            }

            iChar = GetCharIndexFromGlyphIndex(iGlyph);
            return iChar;
        }
    }

    if(!bRequireGlyphHit) // If the user 
    {
        // In this case either our glyph array was empty or xPosition was to the left or to the right of
        // our glyphs. So we have a little logic to figure out what we should return. What we return is
        // going to be based on xPosition and on the base paragraph directionality (LTR or RTL). If the 
        // base direction is RTL, then an xPosition to the right of the line refers to the beginning of 
        // the line's text, and thus we return a char index of zero.

        if(xPosition < span.mfBegin) // If sPosition is to the left of the glyphs...
        {
            if(layoutDirection == kDirectionLTR)
            {
                glyphSide = kGlyphSideBefore;
                iChar     = 0;
            }
            else
            {
                glyphSide = kGlyphSideAfter;
                iChar     = mCharArray.size();
            } 
        }
        else // xPosition > fXMax
        {
            EA_ASSERT((xPosition + 1.f) > span.mfEnd); // +1 to deal with roundoff errors.

            if(layoutDirection == kDirectionLTR)
            {
                glyphSide = kGlyphSideAfter;
                iChar     = mCharArray.empty() ? 0 : mCharArray.size() - 1;
            }
            else
            {
                glyphSide = kGlyphSideBefore;
                iChar     = 0;
            } 
        }
    }

    return iChar;
}


///////////////////////////////////////////////////////////////////////////////
// GetDisplayPositionFromTextPosition
//
void LineLayout::GetDisplayPositionFromTextPosition(index_t nTextPosition, GlyphSide glyphSide,
                                                    Point& ptDisplayPosition, Direction /*layoutDirection*/) const
{
    ptDisplayPosition.mX = 0;
    ptDisplayPosition.mY = 0;

    const eastl_size_t charCount  = mCharArray.size();
    const eastl_size_t glyphCount = mGlyphLayoutInfoArray.size();

    if(glyphCount)
    {
        index_t iGlyph = GetGlyphIndexFromCharIndex(nTextPosition);

        // Do a clipping test.
        if(iGlyph >= glyphCount)
        {
            iGlyph        = (glyphCount - 1);
            nTextPosition = charCount;
            glyphSide     = kGlyphSideAfter;
        }

        const GlyphLayoutInfo* const pGlyphLayoutInfoInitial = &mGlyphLayoutInfoArray[iGlyph];
        const GlyphLayoutInfo*       pGlyphLayoutInfo        = pGlyphLayoutInfoInitial;

        const GlyphInfo* const       pGlyphInfoInitial       = &mGlyphInfoArray[iGlyph];
        const GlyphInfo*             pGlyphInfo              = pGlyphInfoInitial;

        if(glyphSide == kGlyphSideAfter)
        {
            // We need to move to the next insertion position. Often this position is at 
            // the next grapheme cluster, but in the case of ligatures, you need to split 
            // a glyph into two or three parts.

            const GlyphLayoutInfo* const pGlyphLayoutInfoEnd = mGlyphLayoutInfoArray.begin() + glyphCount;

            for(++pGlyphLayoutInfo, ++pGlyphInfo; pGlyphLayoutInfo < pGlyphLayoutInfoEnd; ++pGlyphLayoutInfo, ++pGlyphInfo)
            {
                if(pGlyphInfo->mClusterPosition == 0) // If this glyph is the beginning of a new cluster...
                    break;
            }

            --pGlyphLayoutInfo; // Move to the previous glyph, as we will be using its (mfPenX + mfAdvance).
            --pGlyphInfo;
        }

        ptDisplayPosition.mX = pGlyphLayoutInfo->mfPenX;
        ptDisplayPosition.mY = pGlyphLayoutInfo->mfPenY;

        if(glyphSide == kGlyphSideAfter)
        {
            float fAdvance = pGlyphLayoutInfoInitial->mfAdvance;

            if(pGlyphInfoInitial->mClusterSize && pGlyphInfoInitial->mClusterBreakable)
            {
                // We need to set the position to graphically be in the middle of the cluster.
                fAdvance /= pGlyphInfoInitial->mClusterSize; // To consider: Make this a multiply instead of a divide.
            }

            ptDisplayPosition.mX += fAdvance;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetDisplayPositionFromTextPosition
//
float LineLayout::GetDisplayPositionFromTextPosition(index_t nTextPosition, GlyphSide glyphSide, Direction layoutDirection) const
{
    Point pos;
    GetDisplayPositionFromTextPosition(nTextPosition, glyphSide, pos, layoutDirection);

    return pos.mX;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphSelection
//
eastl_size_t LineLayout::GetGlyphSelection(index_t iCharBegin, index_t iCharEnd, RectangleSet& rectangleSet, 
                                LayoutSettings* pLayoutSettings, float fVerticalOffset) const
{
    SpanList spanList;
    index_t  iGlyphBegin;
    index_t  iGlyphEnd;

    GetGlyphRangeFromCharRange(iCharBegin, iCharEnd, iGlyphBegin, iGlyphEnd);

    const GlyphLayoutInfo*       pGLI    = mGlyphLayoutInfoArray.begin() + iGlyphBegin;
    const GlyphLayoutInfo* const pGLIEnd = mGlyphLayoutInfoArray.begin() + iGlyphEnd;
    const GlyphInfo*             pGI     = mGlyphInfoArray.begin()       + iGlyphBegin;

    while(pGLI < pGLIEnd)
    {
        EA_ASSERT(pGI->mCharCount > 0);
        const float fAdvance = (pGLI->mfAdvance / pGI->mCharCount); // mCharCount will be > 1 when we have ligatures.

        const Span span = {
            eastl::min_alt(pGLI->mfPenX, pGLI->mfPenX + fAdvance),
            eastl::max_alt(pGLI->mfPenX, pGLI->mfPenX + fAdvance)
        };

        AddSpanToSpanList(span, spanList);

        ++pGLI;
        ++pGI;
    }

    // The span generation loop below uses a text-baseline-equals-y-coordinate-origin assumption.
    // If the user is using the top of the text line as the coordinate origin then we compensate.
    if(pLayoutSettings && pLayoutSettings->mbCellTopPosition)
        fVerticalOffset -= mLineMetrics.mfBaseline;

    for(SpanList::iterator it = spanList.begin(), itEnd = spanList.end(); it != itEnd; ++it)
    {
        const Span&     span(*it);  // Problem: We need to take into account LayoutSettings::mfYScale here.
        const Rectangle rectangle(span.mfBegin, mLineMetrics.mfBaseline + fVerticalOffset, span.mfEnd, mLineMetrics.mfDescent + fVerticalOffset);

        rectangleSet.insert(rectangle);
    }

    return rectangleSet.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphDecoration
//
// For decoration selections (e.g. underline, strikethrough), we need to consider  
// that different fonts often result in different vertical positioning of underlining and linethrough. 
// As a result, we need to have different rectangles for each span and font combination.
eastl_size_t LineLayout::GetGlyphDecoration(DecorationFlags decoration, index_t iCharBegin, index_t iCharEnd, RectangleSet& rectangleSet, LayoutSettings* /*pLayoutSettings*/, float fVerticalOffset) const
{
    if(decoration != kDFNone)
    {
        index_t iGlyphBegin;
        index_t iGlyphEnd;

        GetGlyphRangeFromCharRange(iCharBegin, iCharEnd, iGlyphBegin, iGlyphEnd);

        const GlyphLayoutInfo*       pGLIFirst = mGlyphLayoutInfoArray.data();
        const GlyphLayoutInfo*       pGLI      = pGLIFirst + iGlyphBegin;
        const GlyphLayoutInfo* const pGLIEnd   = pGLIFirst + iGlyphEnd;
        const GlyphInfo*             pGI       = mGlyphInfoArray.data() + iGlyphBegin;

        FontMetrics fontMetrics;
        Rectangle   rect;
        
        // We need an independant adjustement that will not overwrite the fVerticalOffset 
        // to support both Baseline Y-Origin and Top-of-the-text Y-Origin coordinates systems.
        float fYCoordinateAdjustment = 0;
      
        // The span generation loop below uses a text-baseline-equals-y-coordinate-origin assumption.
        while(pGLI < pGLIEnd)
        {
            EA_ASSERT(pGI->mCharCount > 0);
            const float fAdvance = (pGLI->mfAdvance / pGI->mCharCount); // mCharCount will be > 1 when we have ligatures.

            rect.mLeft  = eastl::min_alt(pGLI->mfPenX, pGLI->mfPenX + fAdvance);
            rect.mRight = eastl::max_alt(pGLI->mfPenX, pGLI->mfPenX + fAdvance);

            const GlyphLayoutInfo* pGLIForMetrics = pGLI; 

            if(EA_UNLIKELY(pGI->mbGlyphIsObject)) // If the glyph refers to an object (which is rarely the case)...
            {
                // To make underlining of objects better, the object should use the underlining metrics of the text right before it. 
                // That way an object that is in the middle of a text string will have a smooth underline that is contiguous with the text string.
                pGLIForMetrics = GetNearestTextGlyphLayoutInfo(pGI, pGLIFirst, pGLI, mGlyphLayoutInfoArray.data() + mGlyphLayoutInfoArray.size());
            }

            if(pGLIForMetrics)
            {
                EA_ASSERT(pGLIForMetrics->mpFont);
                pGLIForMetrics->mpFont->GetFontMetrics(fontMetrics);

                switch (decoration) // TODO: We need to take into account LayoutSettings::mfYScale.
                {
                    case kDFNone: 
						break; // kDFNone is filtered above, but it is needed here to avoid compiler warnings.

                    case kDFOverline:
                        fYCoordinateAdjustment = ceilf(-fontMetrics.mfDescent);
                        rect.mTop       = floorf(-fontMetrics.mfOverlinePosition - (fontMetrics.mfOverlineThickness * 0.5f));
                        rect.mBottom    = floorf(-fontMetrics.mfOverlinePosition + (fontMetrics.mfOverlineThickness * 0.5f));
                        break;

                    case kDFLineThrough:
                        fYCoordinateAdjustment = ceilf(-fontMetrics.mfDescent);
                        rect.mTop       = floorf(-fontMetrics.mfLinethroughPosition - (fontMetrics.mfLinethroughThickness * 0.5f));
                        rect.mBottom    = floorf(-fontMetrics.mfLinethroughPosition + (fontMetrics.mfLinethroughThickness * 0.5f));
                        break;
                    
                    case kDFUnderline:
                        //Underline does not need any adjustment
                        rect.mTop    = ceilf(-fontMetrics.mfUnderlinePosition - (fontMetrics.mfUnderlineThickness * 0.5f));
                        rect.mBottom = ceilf(-fontMetrics.mfUnderlinePosition + (fontMetrics.mfUnderlineThickness * 0.5f));
                        break;
                }

                rect.mTop    += pGLIForMetrics->mfPenY + fVerticalOffset + fYCoordinateAdjustment;
                rect.mBottom += pGLIForMetrics->mfPenY + fVerticalOffset + fYCoordinateAdjustment;

                AddRectangleToRectangleSet(rect, rectangleSet);
            }

            ++pGLI;
            ++pGI;
        }
    }

    return rectangleSet.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetFontSelection
//
void LineLayout::GetFontSelection(FontSelection& fontSelection) const
{
    for(GlyphLayoutInfoArray::const_iterator it(mGlyphLayoutInfoArray.begin()), itEnd(mGlyphLayoutInfoArray.end()); it != itEnd; ++it)
    {
        const GlyphLayoutInfo* const pGLI = &(*it);

        fontSelection.insert(pGLI->mpFont);
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetLineHeight
//
float LineLayout::GetLineHeight() const
{
    // Previous code which is incorrect for fractional metrics:
    // return floorf(mLineMetrics.mfBaseline) - ceilf(mLineMetrics.mfDescent);

    return ceilf(mLineMetrics.mfBaseline) - floorf(mLineMetrics.mfDescent);
}



///////////////////////////////////////////////////////////////////////////////
// GetBoundingBox
//
void LineLayout::GetBoundingBox(float& w, float& h, bool bVisible)
{
    if(bVisible)
        w = mLineMetrics.mfSpace;
    else
    {
        if(mLineMetrics.mfVisibleSpace == 0) // If the visible space hasn't been calculated yet...
            CalculateVisibleSpace();
        w = mLineMetrics.mfVisibleSpace;
    }

    if(!mGlyphLayoutInfoArray.empty())
    {
        // This implementation assumes that the text flows top to bottom and that the line(s)
        // in the LineLayout are in order from top to bottom, like a single paragraph.
        const GlyphLayoutInfo& gliFirst    = mGlyphLayoutInfoArray.front();
        const GlyphLayoutInfo& gliLast     = mGlyphLayoutInfoArray.back();
        const float            fLineHeight = GetLineHeight();

        h = fabsf(gliFirst.mfPenY - gliLast.mfPenY) + fLineHeight;

        // Implementation which is arbitrary but significantly slower:
        // float yMin = FLT_MAX;
        // float yMax = FLT_MIN;
        // 
        // for(GlyphLayoutInfoArray::const_iterator it = mGlyphLayoutInfoArray.begin(), itEnd = mGlyphLayoutInfoArray.end(); it != itEnd; ++it)
        // {
        //     const GlyphLayoutInfo& gli = *it;
        //  
        //     if(yMin > gli.mfPenY)
        //         yMin = gli.mfPenY;
        //     if(yMax < gli.mfPenY)
        //         yMax = gli.mfPenY;
        // }
        // 
        // h = (yMax - yMin) + fLineHeight;
    }
    else
        h = 0;
}


///////////////////////////////////////////////////////////////////////////////
// Validate
//
// This function is linked away when not used, as would be the case in a release build.
//
bool LineLayout::Validate(bool bValidateSync)
{
    bool bReturnValue = true;

    if(mbSimpleLayout)
        bValidateSync = false;

    // We maintain mCharArray, mAnalysisInfoArray, and mGlyphIndexArray in parallel.
    EA_ASSERT(mCharArray.size() == mAnalysisInfoArray.size());
    if(mCharArray.size() != mAnalysisInfoArray.size())
        bReturnValue = false;

    EA_ASSERT(mCharArray.size() == mGlyphIndexArray.size());
    if(mCharArray.size() != mGlyphIndexArray.size())
        bReturnValue = false;
    if(bValidateSync)
    {
        // Verify that every entry in mGlyphIndexArray points refers to a valid mGlyphArray entry.
        for(eastl_size_t i = 0, iEnd = mGlyphIndexArray.size(), gSize = mGlyphArray.size(); i < iEnd; i++)
        {
            EA_ASSERT(mGlyphIndexArray[i] < gSize);
            if(mGlyphIndexArray[i] >= gSize)
                bReturnValue = false;
        }
    }

    // We maintain mGlyphArray, mGlyphInfoArray, mGlyphLayoutInfoArray, and mCharIndexArray in parallel.
    if(bValidateSync)
    {
        EA_ASSERT_MSG(mGlyphArray.size() == mGlyphInfoArray.size(), "LineLayout::Validate failure: Did you forget to call lineLayout.Clear before layout?");
        if(mGlyphArray.size() != mGlyphInfoArray.size())
            bReturnValue = false;
    }

    // These should always be of equal size; if they aren't synchronized then they should both be empty.
    EA_ASSERT(mGlyphArray.size() == mGlyphLayoutInfoArray.size());
    if(mGlyphArray.size() != mGlyphLayoutInfoArray.size())
        bReturnValue = false;

    if(bValidateSync)
    {
        EA_ASSERT(mGlyphArray.size() == mCharIndexArray.size());
        if(mGlyphArray.size() != mCharIndexArray.size())
            bReturnValue = false;

        // Verify that every entry in mCharIndexArray points refers to a valid mCharArray entry.
        for(eastl_size_t i = 0, iEnd = mCharIndexArray.size(), cSize = mCharArray.size(); i < iEnd; i++)
        {
            EA_ASSERT(mCharIndexArray[i] < cSize);
            if(mCharIndexArray[i] >= cSize)
                bReturnValue = false;
        }
    }

    // Verify that information in mGlyphLayoutInfoArray is sane.
    for(eastl_size_t i = 0, iEnd = mGlyphLayoutInfoArray.size(); i < iEnd; ++i)
    {
        const GlyphLayoutInfo& gli = mGlyphLayoutInfoArray[i];

        EA_ASSERT((gli.mpFont != NULL) || (mGlyphInfoArray[i].mbGlyphIsObject != 0)); // All text entries must have a Font.
        if((gli.mpFont == NULL) && (mGlyphInfoArray[i].mbGlyphIsObject == 0))
            bReturnValue = false;

        EA_ASSERT(fabs(gli.mfAdvance) < 10000);         // Sanity check.
        if(!(fabs(gli.mfAdvance) < 10000))
            bReturnValue = false;

        EA_ASSERT(fabs(gli.mfX2 - gli.mfX1) < 10000);   // Sanity check.
        if(!(fabs(gli.mfX2 - gli.mfX1) < 10000))
            bReturnValue = false;

        EA_ASSERT(fabs(gli.mfY2 - gli.mfY1) < 10000);   // Sanity check.
        if(!(fabs(gli.mfY2 - gli.mfY1) < 10000))
            bReturnValue = false;
    }

    return bReturnValue;
}





///////////////////////////////////////////////////////////////////////////////
// LineLayoutIterator
///////////////////////////////////////////////////////////////////////////////

LineLayoutIterator::LineLayoutIterator(const EA::Text::LineLayout* pLineLayout)
    : mpLineLayout(pLineLayout),
      mNextIndex(0)
{
}


void LineLayoutIterator::Reset(const EA::Text::LineLayout* pLineLayout)
{
    if(pLineLayout)
        mpLineLayout = pLineLayout;
    mNextIndex = 0;
}


bool LineLayoutIterator::GetNextTextStyleSegment(eastl_size_t& nSegmentBegin, eastl_size_t& nSegmentEnd)
{
    // We have to move past any objects that are in the layout at the current position.
    // Then we find a contiguous segment of a given TextStyle.
    // To do: This can be made faster by working with pointers instead of array indexes.

    eastl_size_t i    = mNextIndex;
    eastl_size_t iEnd = mpLineLayout->mGlyphInfoArray.size();

    for( ; i < iEnd; ++i)
    {
        if(!mpLineLayout->mGlyphInfoArray[i].mbGlyphIsObject)
            break;
    }

    nSegmentBegin = i;
    nSegmentEnd   = i;

    if(i < iEnd) // If we aren't at the end...
    {
        const index_t iChar = mpLineLayout->GetCharIndexFromGlyphIndex(nSegmentBegin);
        const EA::Text::TextStyle* const pTextStyle = mpLineLayout->mAnalysisInfoArray[iChar].mpTextStyle;

        while((i < iEnd)
            && (mpLineLayout->mAnalysisInfoArray[mpLineLayout->GetCharIndexFromGlyphIndex(i)].mpTextStyle == pTextStyle)
            && !mpLineLayout->mGlyphInfoArray[i].mbGlyphIsObject)
        {
            ++nSegmentEnd;
            ++i;
        }

        mNextIndex = i;
        return true;
    }

    mNextIndex = iEnd;
    return false;
}


bool LineLayoutIterator::GetNextObject(eastl_size_t& nObjectPosition)
{
    eastl_size_t i    = mNextIndex;
    eastl_size_t iEnd = mpLineLayout->mGlyphInfoArray.size();

    for( ; i < iEnd; i++)
    {
        if(mpLineLayout->mGlyphInfoArray[i].mbGlyphIsObject)
        {
            nObjectPosition = i;
            mNextIndex = i + 1;
            return true;
        }
    }

    mNextIndex = iEnd;
    return false;
}





///////////////////////////////////////////////////////////////////////////////
// ScheduleLocation
///////////////////////////////////////////////////////////////////////////////

Typesetter::ScheduleLocation::ScheduleLocation()
  : mnScheduleIndex(0),
    mnCharBase(0),
    mnCharOffset(0)
{
    // Empty
}


bool Typesetter::ScheduleLocation::operator==(const EA::Text::Typesetter::ScheduleLocation& x)
{
    return ((mnScheduleIndex == x.mnScheduleIndex) && // The mScheduleIndex comparison here can probably be removed.
            (mnCharBase      == x.mnCharBase)      &&
            (mnCharOffset    == x.mnCharOffset));
}


index_t Typesetter::ScheduleLocation::GetCharIndex() const
{
    return mnCharBase + mnCharOffset;
}




///////////////////////////////////////////////////////////////////////////////
// Global functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GetMaxFontMetrics
//
EATEXT_API void GetMaxFontMetrics(const FontSelection& fontSelection, float& fMaxBaseline, float& fMaxDescent)
{
    FontMetrics fontMetrics;

    for(FontSelection::const_iterator it = fontSelection.begin(), itEnd = fontSelection.end(); it != itEnd; ++it)
    {
        Font* const pFont = *it;

        EA_ASSERT(pFont);
        pFont->GetFontMetrics(fontMetrics);

        fMaxBaseline = eastl::max_alt(fMaxBaseline, fontMetrics.mfBaseline);
        fMaxDescent  = eastl::min_alt(fMaxDescent,  fontMetrics.mfDescent);
    }
}



///////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
EATEXT_API index_t LayoutSimple(const Char* pText, index_t nTextSize, float x, float y,
                                Font* pFont, GlyphId* pGlyphIdArray, GlyphLayoutInfo* pGlyphLayoutInfo,
                                LayoutSettings* pLayoutSettings, LineMetrics* pLineMetrics, index_t* pLineCount)
{
    index_t     nLineCount = 1;
    const float xBegin     = x;
    float       xMax       = x;

    if(!pLayoutSettings)
        pLayoutSettings = &GetDefaultLayoutSettings();

    if(!pFont && !pLayoutSettings->mFontSelection.empty())
        pFont = *pLayoutSettings->mFontSelection.begin();

    EA_ASSERT(pFont); // If this fails then the user didn't supply a font nor were there any specified in the LayoutSettings.
    if(pFont)
    {
        float fLineHeight = 0; // Zero until we need to calculate it.

        if(pLayoutSettings->mbCellTopPosition)
        {
            FontMetrics fontMetrics;

            pFont->GetFontMetrics(fontMetrics);
            y += floorf(fontMetrics.mfBaseline * pLayoutSettings->mfYScale);
        }

        GlyphMetrics glyphMetrics;

        pFont->GetGlyphIds(pText, (uint32_t)nTextSize, pGlyphIdArray, true);

        for(const Char* const pTextEnd(pText + nTextSize); pText < pTextEnd; pText++)
        {
            const Char c = *pText;

            if(x > xMax)
                xMax = x;

            pFont->GetGlyphMetrics(*pGlyphIdArray, glyphMetrics);

            pGlyphLayoutInfo->mpFont    = pFont;
            pGlyphLayoutInfo->mfPenX    = x;
            pGlyphLayoutInfo->mfPenY    = y;
            pGlyphLayoutInfo->mfX1      = x +  glyphMetrics.mfHBearingX;
            pGlyphLayoutInfo->mfY1      = y - (glyphMetrics.mfHBearingY * pLayoutSettings->mfYScale);
            pGlyphLayoutInfo->mfAdvance = glyphMetrics.mfHAdvanceX;

            if (c != L'\n')
            {
                pGlyphLayoutInfo->mfX2 = pGlyphLayoutInfo->mfX1 + glyphMetrics.mfSizeX;
                pGlyphLayoutInfo->mfY2 = pGlyphLayoutInfo->mfY1 + (glyphMetrics.mfSizeY * pLayoutSettings->mfYScale);

                x += glyphMetrics.mfHAdvanceX;
            }
            else
            {
                // We set this glyph to be empty. This is the fastest solution, as otherwise 
                // we would have to synchronize pGlyphIdArray and pGlyphLayoutInfoArray.
                pGlyphLayoutInfo->mfX2 = pGlyphLayoutInfo->mfX1;

                if(fLineHeight == 0)
                {
                    FontMetrics fontMetrics;
                    pFont->GetFontMetrics(fontMetrics);
                    fLineHeight = floorf(fontMetrics.mfLineHeight + 0.5f);
                }

                x  = xBegin;
                y += fLineHeight * pLayoutSettings->mfYScale;
                ++nLineCount;
            }

            ++pGlyphIdArray;
            ++pGlyphLayoutInfo;
        }
    }

    if(pLineMetrics)
    {
        FontMetrics fontMetrics;
        pFont->GetFontMetrics(fontMetrics);

        pLineMetrics->mfBaseline   = fontMetrics.mfBaseline;
        pLineMetrics->mfDescent    = fontMetrics.mfDescent;
        pLineMetrics->mnLineLength = nTextSize;                       // This is sensible only for the case of a single line.
        pLineMetrics->mfSpace      = nTextSize ? (xMax - xBegin) : 0;
    }

    if(pLineCount)
        *pLineCount = nLineCount;

    return nTextSize;
}


///////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
EATEXT_API index_t LayoutSimple(const Char* pText, index_t nTextSize, float x, float y, 
                                Font* pFont, LineLayout& lineLayout, 
                                LayoutSettings* pLayoutSettings, 
                                uint32_t componentFlags)
{
    lineLayout.mbSimpleLayout = true;
    lineLayout.mGlyphArray.resize(nTextSize);
    lineLayout.mGlyphLayoutInfoArray.resize(nTextSize);

    const index_t count = LayoutSimple(pText, nTextSize, x, y, pFont, 
                                        const_cast<GlyphId*>(lineLayout.mGlyphArray.data()), 
                                        const_cast<GlyphLayoutInfo*>(lineLayout.mGlyphLayoutInfoArray.data()), 
                                        pLayoutSettings, &lineLayout.mLineMetrics, &lineLayout.mLineCount);

    if(componentFlags & LineLayout::kComponentCharArray)
        lineLayout.mCharArray.assign(pText, nTextSize);

    // To consider: Support kComponentCharAnalysis, kComponentGlyphIndexArray, kComponentCharIndexArray

    // Not necessary, as currently count always == nTextSize.
    // lineLayout.mGlyphArray.resize(count);
    // lineLayout.mGlyphLayoutInfoArray.resize(count);

    return count;
}


///////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
EATEXT_API index_t LayoutSimple(const char8_t* pText, index_t nTextSize, float x, float y, 
                                Font* pFont, GlyphId* pGlyphIdArray, GlyphLayoutInfo* pGlyphLayoutInfo, 
                                LayoutSettings* pLayoutSettings, LineMetrics* pLineMetrics, index_t* pLineCount)
{
    index_t     nLineCount = 1;
    const float xBegin     = x;
    float       xMax       = x;

    if(!pLayoutSettings)
        pLayoutSettings = &GetDefaultLayoutSettings();

    if(!pFont && !pLayoutSettings->mFontSelection.empty())
        pFont = *pLayoutSettings->mFontSelection.begin();

    EA_ASSERT(pFont); // If this fails then the user didn't supply a font nor were there any specified in the LayoutSettings.
    if(pFont)
    {
        float fLineHeight = 0; // Zero until we need to calculate it.

        if(pLayoutSettings->mbCellTopPosition)
        {
            FontMetrics fontMetrics;

            pFont->GetFontMetrics(fontMetrics);
            y += floorf(fontMetrics.mfBaseline * pLayoutSettings->mfYScale);
        }

        GlyphMetrics glyphMetrics;

        pFont->GetGlyphIds(pText, (uint32_t)nTextSize, pGlyphIdArray, true);

        for(const char8_t* const pTextEnd(pText + nTextSize); pText < pTextEnd; pText++)
        {
            const char8_t c = (char8_t)(uint8_t)*pText;

            if (x > xMax)
                xMax = x;

            pFont->GetGlyphMetrics(*pGlyphIdArray, glyphMetrics);

            pGlyphLayoutInfo->mpFont    = pFont;
            pGlyphLayoutInfo->mfPenX    = x;
            pGlyphLayoutInfo->mfPenY    = y;
            pGlyphLayoutInfo->mfX1      = x +  glyphMetrics.mfHBearingX;
            pGlyphLayoutInfo->mfY1      = y - (glyphMetrics.mfHBearingY * pLayoutSettings->mfYScale);
            pGlyphLayoutInfo->mfAdvance = glyphMetrics.mfHAdvanceX;

            if (c != L'\n')
            {
                pGlyphLayoutInfo->mfX2 = pGlyphLayoutInfo->mfX1 + glyphMetrics.mfSizeX;
                pGlyphLayoutInfo->mfY2 = pGlyphLayoutInfo->mfY1 + (glyphMetrics.mfSizeY * pLayoutSettings->mfYScale);

                x += glyphMetrics.mfHAdvanceX;
            }
            else
            {
                // We set this glyph to be empty. This is the fastest solution, as otherwise 
                // we would have to synchronize pGlyphIdArray and pGlyphLayoutInfoArray.
                pGlyphLayoutInfo->mfX2 = pGlyphLayoutInfo->mfX1;

                if(fLineHeight == 0)
                {
                    FontMetrics fontMetrics;
                    pFont->GetFontMetrics(fontMetrics);
                    fLineHeight = floorf(fontMetrics.mfLineHeight + 0.5f);
                }

                x  = xBegin;
                y += fLineHeight * pLayoutSettings->mfYScale;
                ++nLineCount;
            }

            ++pGlyphIdArray;
            ++pGlyphLayoutInfo;
        }
    }

    if(pLineMetrics)
    {
        FontMetrics fontMetrics;
        pFont->GetFontMetrics(fontMetrics);

        pLineMetrics->mfBaseline   = fontMetrics.mfBaseline;
        pLineMetrics->mfDescent    = fontMetrics.mfDescent;
        pLineMetrics->mnLineLength = nTextSize;                       // This is sensible only for the case of a single line.
        pLineMetrics->mfSpace      = nTextSize ? (xMax - xBegin) : 0;
   }

    if(pLineCount)
        *pLineCount = nLineCount;

    return nTextSize;
}




///////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
EATEXT_API index_t LayoutSimple(const char8_t* pText, index_t nTextSize, float x, float y, 
                                Font* pFont, LineLayout& lineLayout, 
                                LayoutSettings* pLayoutSettings,
                                uint32_t componentFlags)
{
    lineLayout.mbSimpleLayout = true;
    lineLayout.mGlyphArray.resize(nTextSize);
    lineLayout.mGlyphLayoutInfoArray.resize(nTextSize);

    const index_t count = LayoutSimple(pText, nTextSize, x, y, pFont, 
                                        const_cast<GlyphId*>(lineLayout.mGlyphArray.data()), 
                                        const_cast<GlyphLayoutInfo*>(lineLayout.mGlyphLayoutInfoArray.data()), 
                                        pLayoutSettings, &lineLayout.mLineMetrics, &lineLayout.mLineCount);

    if(componentFlags & LineLayout::kComponentCharArray)
    {
        lineLayout.mCharArray.resize(nTextSize);
        for(index_t i = 0; i < nTextSize; i++)
            lineLayout.mCharArray[i] = (uint8_t)pText[i];
    }

    // To consider: Support kComponentCharAnalysis, kComponentGlyphIndexArray, kComponentCharIndexArray

    // Not necessary, as currently count always == nTextSize.
    // lineLayout.mGlyphArray.resize(count);
    // lineLayout.mGlyphLayoutInfoArray.resize(count);

    return count;
}



///////////////////////////////////////////////////////////////////////////////
// Typesetter
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Typesetter
//
Typesetter::Typesetter(Allocator::ICoreAllocator* pCoreAllocator)
 :  mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
    mLayoutSettings(GetDefaultLayoutSettings()),
    mpFontServer(NULL),

    mTextStyleArray(1), // Set to an initial size of 1, with the first item being the default style.
    mpCurrentTextStyle(NULL),
    mfLayoutSpace(0),
    mfLayoutPenX(0),
    mfLayoutPenY(0),
    mbEllipsize(false),
    mPasswordChar(0x25CF),

    mSchedule(),
    mLineBegin(),
    mLineEnd(),
    mAnalysisEnd(),
    mScheduleEnd(),

    mLineState(kLineStateComplete),
    mLineLayout(),
    mnLastLineEndIndex(0),
    mBidiRunInfoArray(),
    mbNonGeneralScriptPresent(false),
    mfPenX(0),
    mfPenXMax(0)
{
    SetDefaultTextStyle(&mLayoutSettings.mTextStyleDefault);
}


///////////////////////////////////////////////////////////////////////////////
// ~Typesetter
//
Typesetter::~Typesetter()
{
    // Empty 
}


void Typesetter::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;
    
    // We pass on the allocator to our LineLayout. 
    // To consider: Allow the user to control this assignment.
    mLineLayout.SetAllocator(pCoreAllocator);

    mSchedule.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// GetLayoutSettings
//
LayoutSettings& Typesetter::GetLayoutSettings()
{
    return mLayoutSettings;
}


///////////////////////////////////////////////////////////////////////////////
// GetLayoutSettings
//
const LayoutSettings& Typesetter::GetLayoutSettings() const
{
    return mLayoutSettings;
}


///////////////////////////////////////////////////////////////////////////////
// SetLayoutSettings
//
void Typesetter::SetLayoutSettings(LayoutSettings& layoutSettings)
{
    Validate();

    mLayoutSettings = layoutSettings;

    // Make sure mLocale is valid.
    if(!mLayoutSettings.mLocale[0])
        Strcpy(mLayoutSettings.mLocale, EATEXT_DEFAULT_LOCALE);

    // Make sure mScript is valid.
    if(mLayoutSettings.mScript < kScriptStart) // If the script is not set...
        mLayoutSettings.mScript = GetScriptFromLocale(mLayoutSettings.mLocale); 

    // Make sure mDirection is valid.
    if((mLayoutSettings.mDirection != kDirectionLTR) &&  // If the direction is not specific...
       (mLayoutSettings.mDirection != kDirectionRTL))
    {
        ScriptProperties scriptProperties;

        GetScriptProperties(mLayoutSettings.mScript, &scriptProperties, true);
        mLayoutSettings.mDirection = (scriptProperties.mbRTL ? kDirectionRTL : kDirectionLTR);
    }

    // The user should supply a font selection or the user 
    // supply a FontServer so we can get our own fonts.
    // We don't assert that condition here, but will do so
    // when the user tries to do layout.

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// GetLayoutSpace
//
void Typesetter::GetLayoutSpace(float& fSpace, float& fPenX, float& fPenY) const
{
    fSpace = mfLayoutSpace;
    fPenX  = mfLayoutPenX;
    fPenY  = mfLayoutPenY;
}


///////////////////////////////////////////////////////////////////////////////
// SetLayoutSpace
//
void Typesetter::SetLayoutSpace(float fSpace, float fPenX, float fPenY)
{
    mfLayoutSpace = fSpace;
    mfLayoutPenX  = fPenX;
    mfLayoutPenY  = fPenY;
}

///////////////////////////////////////////////////////////////////////////////
// SetFontServer
//
void Typesetter::SetFontServer(FontServer* pFontServer)
{
    mpFontServer = pFontServer;
}


///////////////////////////////////////////////////////////////////////////////
// SetDefaultTextStyle
//
void Typesetter::SetDefaultTextStyle(const TextStyle* pDefaultTextStyle)
{
    if(pDefaultTextStyle)
    {
        mLayoutSettings.mTextStyleDefault = *pDefaultTextStyle;
        mTextStyleArray[0] = &mLayoutSettings.mTextStyleDefault;

        // Setting mpCurrentStyle is something we need to do but it should only 
        // be done between paragraph layouts. We assert so here.
        EA_ASSERT(mLineState == kLineStateComplete);
        mpCurrentTextStyle = mTextStyleArray[0];
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetDefaultTextStyle
//
const TextStyle& Typesetter::GetDefaultTextStyle() const
{
    return mLayoutSettings.mTextStyleDefault;
}


///////////////////////////////////////////////////////////////////////////////
// Reset
//
void Typesetter::Reset(bool bClearSchedule)
{
    Validate();

    if(bClearSchedule)
    {
        // Clear the item list
        mSchedule.clear();
        mScheduleEnd.mnScheduleIndex = 0;
        mScheduleEnd.mnCharOffset = 0;
        mScheduleEnd.mnCharBase = 0;

        // We're starting from the beginning of the schedule.
        mLineBegin   = mScheduleEnd;
        mAnalysisEnd = mScheduleEnd;

        mnLastLineEndIndex = 0; // Go back to the the paragraph beginning.
    } 
    // Else reset the current item to the start of the current line,
    // as we're going to re-use the items that are already in the queue.

    mLineEnd     = mLineBegin;
    mAnalysisEnd = mLineBegin;

    // Reset our style information. Preserve the first entry.
    mTextStyleArray.resize(1);
    mpCurrentTextStyle = &mLayoutSettings.mTextStyleDefault;

    // This is currently a paragraph-level setting, so we reset it here 
    // as opposed to setting it in NextLine(). There would be optimization 
    // opportunities if it were line-based, but on the other hand text
    // of a paragraph tends to use all the same script.
    mbNonGeneralScriptPresent = false;

    // Force the line state to complete, regardless of what it might actually
    // be at this time. This way, NextLine won't think we're calling it while
    // in the middle of processing the previous line.
    mLineState = kLineStateComplete;
    NextLine(false); // This will clear the current LineLayout data.
    mLineLayout.mCharScheduleIndex = 0; // Reset the paragraph layout as well.

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// GetLineState
//
Typesetter::LineState Typesetter::GetLineState() const
{
    return mLineState;
}


///////////////////////////////////////////////////////////////////////////////
// ProcessLine
//
// ProcessLine determines if there is enough accumulated text to form a 
// complete wrapped line. The answer is either kLineFull (meaning 'definitely yes') 
// or kLinePartial (meaning 'definitely no or unsure').
//
Typesetter::LineState Typesetter::ProcessLine()
{
    Validate();

    if(mLineState != kLineStateFull) // If we haven't completed filling the line...
    {
        mfPenX = 0;
        mfPenXMax = 0;
        mBidiRunInfoArray.clear();

        // We need to have a check that tests if the analysis text is not 
        // long enough to find the end of a line and increase the amount
        // of analysis we do. Currently we unilaterally stop at kMaxAnalysisSize,
        // whereas there are pathological cases whereby this might not be
        // enough. 
        UpdateAnalysis();

        // What we do in the short run is always re-shape the line from the beginning. 
        // We don't yet attempt to do iterative shaping. Once we have some confidence 
        // in our system then we can attempt iterative shaping. Recall that shaping
        // is the conversion of Unicode characters to glyphs, including substitutions
        // and rearrangements of glyphs as needed.
        ShapeText();

        // Once the text is shaped (converted to glyphs), we can proceed to line breaking.
        // We do this by measuring the glyphs and finding the last line breakable point
        // that fits within our margins.
        UpdateLineState();
    }

    Validate();

    return mLineState;
}


///////////////////////////////////////////////////////////////////////////////
// IsScheduleEmpty
//
bool Typesetter::IsScheduleEmpty() const
{
    return (mAnalysisEnd.mnScheduleIndex == mScheduleEnd.mnScheduleIndex);
}


///////////////////////////////////////////////////////////////////////////////
// GetScheduleLocationFromCharIndex
//
void Typesetter::GetScheduleLocationFromCharIndex(index_t charIndex, ScheduleLocation& scheduleLocation, 
                                                    bool bCharIndexIsLineRelative) const
{
    // We have little choice but to iterate mSchedule. Since mSchedule is ordered,
    // it should be feasible to do a binary search of it instead of a linear search.
    // We'll implement a linear search for now and consider making it binary if it 
    // turns out we are working with large schedule sizes.

    if(bCharIndexIsLineRelative)
        charIndex += mLineLayout.mCharScheduleIndex;

    index_t charSum = 0;

    for(eastl_size_t i = 0, iEnd = mSchedule.size(); i < iEnd; ++i)
    {
        const Item& item = mSchedule[i];

        if(charIndex < (charSum + item.mCount)) // If the charIndex refers to this schedule Item...
        {
            scheduleLocation.mnScheduleIndex = i;
            scheduleLocation.mnCharBase      = charSum;
            scheduleLocation.mnCharOffset    = charIndex - charSum;
            return;
        }

        charSum += item.mCount;
    }

    EA_ASSERT((mScheduleEnd.mnScheduleIndex == mSchedule.size()) && 
              (mScheduleEnd.mnCharBase      == charSum)          && 
              (mScheduleEnd.mnCharOffset    == 0));

    scheduleLocation = mScheduleEnd;
}


///////////////////////////////////////////////////////////////////////////////
// GetScheduleItemFromCharIndex
//
Typesetter::Item* Typesetter::GetScheduleItemFromCharIndex(index_t charIndex, bool bCharIndexIsLineRelative)
{
    ScheduleLocation scheduleLocation;

    GetScheduleLocationFromCharIndex(charIndex, scheduleLocation, bCharIndexIsLineRelative);

    EA_ASSERT(scheduleLocation.mnScheduleIndex < mSchedule.size());
    return &mSchedule[scheduleLocation.mnScheduleIndex];
}


///////////////////////////////////////////////////////////////////////////////
// AddTextStyle
//
const TextStyle* Typesetter::AddTextStyle(const TextStyle * textStyle)
{
    // The mTextStyleArray member is an array of all unique styles that are
    // in effect for the current layout. The Reset function clears all but
    // the first mTextStyleArray entry. When the user calls this AddTextStyle
    // function, we see if there is an existing match and simply return it.
    // Otherwise we append the textStyle to mTextStyleArray. There is no need
    // for reference counting of the usage of these text styles, as the 
    // lifetime of these elements is clearly defined and controlled internally
    // to this class.

    for(eastl_size_t i = 0, iEnd = mTextStyleArray.size(); i < iEnd; ++i)
    {
        if(mTextStyleArray[i] && *textStyle == *mTextStyleArray[i])
            return mTextStyleArray[i];
    }

    mTextStyleArray.push_back(textStyle);
    return mTextStyleArray.back();
}


///////////////////////////////////////////////////////////////////////////////
// AddTextRun
//
void Typesetter::AddTextRun(const Char* pText, index_t nTextLength)
{
    Validate();
    EA_ASSERT(mpCurrentTextStyle);

    // Add the text run to the item list.
    mSchedule.push_back();
    Item& item = mSchedule.back();

    item.mType   = kItemTypeText;
    item.mIndex  = mScheduleEnd.mnCharBase;
    item.mCount  = nTextLength;
    item.mpStyle = mpCurrentTextStyle;

    EA_ASSERT(pText); // A nTextLength of zero should be OK.
    item.mText.mpCharData = pText;

    // Set the end location
    mScheduleEnd.mnScheduleIndex += 1;
    mScheduleEnd.mnCharBase      += item.mCount;

    if(mLineState == kLineStateComplete)
        mLineState = kLineStatePartial;

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// AddObject
//
void Typesetter::AddObject(void* pObject, GlyphMetrics& glyphMetrics, bool bBindPrev, bool bBindNext)
{
    Validate();
    EA_ASSERT(mpCurrentTextStyle && (glyphMetrics.mfSizeX < 100000) && (glyphMetrics.mfSizeY < 100000) && (glyphMetrics.mfHBearingY < 100000));

    // Add the object to the item list.
    mSchedule.push_back();
    Item& item = mSchedule.back();

    item.mType   = kItemTypeObject;
    item.mIndex  = mScheduleEnd.mnCharBase;
    item.mCount  = 1 + (bBindPrev ? 1 : 0) + (bBindNext ? 1 : 0);
    item.mpStyle = mpCurrentTextStyle;

    item.mObject.mpObjectPtr   = pObject;
    item.mObject.mGlyphMetrics = glyphMetrics;
    item.mObject.mbBindPrev    = bBindPrev;
    item.mObject.mbBindNext    = bBindNext;

    // Set the end location.
    mScheduleEnd.mnScheduleIndex += 1;
    mScheduleEnd.mnCharBase      += item.mCount;

    if(mLineState == kLineStateComplete)
        mLineState = kLineStatePartial;

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// AddLineBreak
//
void Typesetter::AddLineBreak()
{
    Validate();
    EA_ASSERT(mpCurrentTextStyle);

    // Add the line break to the item list.
    mSchedule.push_back();
    Item& item = mSchedule.back();

    item.mType   = kItemTypeLineBreak;
    item.mIndex  = mScheduleEnd.mnCharBase;
    item.mCount  = 1;
    item.mpStyle = mpCurrentTextStyle;

    // Set the end location. Line breaks have a size of 0.
    mScheduleEnd.mnScheduleIndex += 1;
    mScheduleEnd.mnCharBase      += 1;

    if(mLineState == kLineStateComplete)
        mLineState = kLineStatePartial;

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// AddParagraphBreak
//
void Typesetter::AddParagraphBreak()
{
    Validate();
    EA_ASSERT(mpCurrentTextStyle);

    // Add the style change callback to the end of the item list.
    mSchedule.push_back();
    Item& item = mSchedule.back();

    item.mType   = kItemTypeParagraphBreak;
    item.mIndex  = mScheduleEnd.mnCharBase;
    item.mCount  = 1;
    item.mpStyle = mpCurrentTextStyle;

    // Set the end location. Paragraph breaks have a size of 0.
    mScheduleEnd.mnScheduleIndex += 1;
    mScheduleEnd.mnCharBase      += item.mCount;

    if(mLineState == kLineStateComplete)
        mLineState = kLineStatePartial;

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// AddStyleChange
//
void Typesetter::AddStyleChange(const TextStyle* pStyle)
{
    mpCurrentTextStyle = AddTextStyle(pStyle);
}


///////////////////////////////////////////////////////////////////////////////
// EndCharIndex
//
index_t Typesetter::GetEndCharIndex() const
{
    return mScheduleEnd.mnCharBase;
}


///////////////////////////////////////////////////////////////////////////////
// FinalizeLine
//
void Typesetter::FinalizeLine()
{
    ////////////////////////////////
    // Cement the line definition //
    ////////////////////////////////

    Validate();

    if((mLineState == kLineStatePartial) && !IsScheduleEmpty()) // If we are not done processing and there is anything left to process...
    {
        // Trigger processing of remaining items.
        // ProcessLine will either set mLineState to kLineStateFull or it will 
        // have exhausted all characters that we have to lay out.
        ProcessLine();

        // If this assertion fails, contact Paul Pedriana, as it can in 
        // fact fail as of this writing in the pathological case of a 
        // layout box being able to hold a very large number of characters.
        EA_ASSERT((mLineState == kLineStateFull) || IsScheduleEmpty());

        // We'll set mLineState to kLineStateFull below.
    }

    // We leave most of the LineMetrics as they are. The last value for these as 
    // of that last ProcessLine should be accurate, even if the line wasn't full.
    //     - mnLineLength           : Already set
    //     - mnVisibleLineLength    : Need to calculate
    //     - mfSpace                : Already set
    //     - mfVisibleSpace         : Need to calculate
    //     - mfBaseline             : Already set
    //     - mfDescent              : Already set

    // Calculate the amount of visible space the line uses. This means ignoring trailing space.
    mLineLayout.CalculateVisibleSpace();

    // We chop off the analysis info that refers to chars past the end of this line.
    // We cannot preserve this analysis info because it can change upon re-analysis 
    // due to characters later in the text that affect previous text.
    const eastl_size_t lineLength = mLineLayout.mLineMetrics.mnLineLength;

    if(mLineLayout.mCharArray.size() != lineLength) // If we don't appear to already be of the correct size..
    {
        // Calculate iGlyphEnd before doing the char array resize below. 
        // Otherwise the GetGlyphIndexFromCharIndex would yield incorrect 
        // values, because it's dependent on those sizes.
        const eastl_size_t iGlyphEnd = mLineLayout.GetGlyphIndexFromCharIndex(mLineLayout.mLineMetrics.mnLineLength);

        // Chop off glyph info that refers to glyphs past the end of this line.
        mLineLayout.mGlyphArray.resize(iGlyphEnd);
        mLineLayout.mGlyphInfoArray.resize(iGlyphEnd);
        mLineLayout.mGlyphLayoutInfoArray.resize(iGlyphEnd);
        mLineLayout.mCharIndexArray.resize(iGlyphEnd);

        mLineLayout.mCharArray.resize(lineLength);
        mLineLayout.mAnalysisInfoArray.resize(lineLength);
        mLineLayout.mGlyphIndexArray.resize(lineLength);

        // Chop off mBidiRunInfoArray at hte end fo the line.
        for(eastl_size_t r = 0, rEnd = mBidiRunInfoArray.size(); r < rEnd; ++r)
        {
            RunInfo& runInfo = mBidiRunInfoArray[r];

            if(runInfo.mnCharEnd >= lineLength)
            {
                runInfo.mnCharEnd = lineLength;
                mBidiRunInfoArray.resize(r + 1);
                break;
            }
        }
    }

    mLineLayout.mLineCount = 1;

    // Set mLineEnd to be at mLineBegin + mnLineLength.
    const index_t charIndex = mLineBegin.GetCharIndex(); // charIndex is paragraph-relative.
    GetScheduleLocationFromCharIndex(charIndex + mLineLayout.mLineMetrics.mnLineLength, mLineEnd, false);
    mAnalysisEnd = mLineEnd;

    // Set our mnLastLineEndIndex
    mnLastLineEndIndex += mLineLayout.mLineMetrics.mnLineLength; 

    Validate();

    ///////////////////////
    // Final processing  //
    ///////////////////////

    // The code from here on could be broken out into a separate function, and doing so would
    // allow the user to more easily intervene in the layout process.

    // Here is where we would do ellipsis substitution. 
    // The line breaking algorithm would have set the last line to overflow the layout space,
    // and here we need to remove all glyphs off the back such that ellipsis (0x2026) can fit.
    if(mbEllipsize && (mLineLayout.mLineMetrics.mfSpace > mfLayoutSpace))
        SubstituteEllipsis();

    // Unicode Annex #9 (Bidi), section 3.4 L1 specifies that whitespace at the end of a line
    // or paragraph gets converted to the base paragraph embedding level, if it isn't 
    // so already.
    AdjustWhitespaceEmbedding();

    // Here we move the line to be at the user-specified coordinate origin and 
    // be of the user-specified alignment or justification.
    AdjustPositioning();

    // We implement Unicode Annex #9 (Bidi), section 3.4 L2, which describes how
    // bidirectional text must be re-ordered. 
    OrderGlyphs();

    // This function by its nature always sets the line state to full.
    mLineState = kLineStateFull;

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// NextLine
//
void Typesetter::NextLine(bool bAdvanceYPosition)
{
    Validate();

    // Assert that the user isn't trying to call NextLine without first calling
    // FinalizeLine. Calling NextLine without first calling FinalizeLine will 
    // result in a line that ends too early or will result in lost text after
    // the current line. We cannot call FinalizeLine for the user here because 
    // we don't know what the user's intentions are.
    EA_ASSERT(mLineState != kLineStatePartial);

    mLineLayout.mCharScheduleIndex = mLineEnd.GetCharIndex();

    // Clear the line state.
    mLineState   = IsScheduleEmpty() ? kLineStateComplete : kLineStatePartial;
    mLineBegin   = mLineEnd; // This probably isn't necessary, as FinalizeLine already does this.
    mAnalysisEnd = mLineEnd; // This probably isn't necessary, as FinalizeLine already does this.

    // Always clear mbEllipsize when beginning a new line. The user optionally
    // enables it manually after calling this function.
    mbEllipsize = false;

    mBidiRunInfoArray.clear();
    mfPenX    = 0;
    mfPenXMax = 0;

    if(bAdvanceYPosition)
    {
        if(mLayoutSettings.mTextStyleDefault.mfLineSpace > 0)
            mfLayoutPenY += mLayoutSettings.mTextStyleDefault.mfLineSpace * mLayoutSettings.mfYScale;
        else
          //mfLayoutPenY += (floorf(mLineLayout.mLineMetrics.mfBaseline) - ceilf(mLineLayout.mLineMetrics.mfDescent)) * mLayoutSettings.mfYScale;
            mfLayoutPenY += (ceilf(mLineLayout.mLineMetrics.mfBaseline) - floorf(mLineLayout.mLineMetrics.mfDescent)) * mLayoutSettings.mfYScale;
    }

    // Clear the line layout completely.
    mLineLayout.NewLine();

    Validate();
}


///////////////////////////////////////////////////////////////////////////////
// EllipsizeLine
//
void Typesetter::EllipsizeLine(bool bEnable)
{
    mbEllipsize = bEnable;
}


///////////////////////////////////////////////////////////////////////////////
// GetLineLayout
//
LineLayout& Typesetter::GetLineLayout()
{
    return mLineLayout;
}


///////////////////////////////////////////////////////////////////////////////
// UpdateAnalysis
//
void Typesetter::UpdateAnalysis()
{
    const eastl_size_t charArraySizeOriginal = mLineLayout.mCharArray.size();
    eastl_size_t       analysisInfoArraySize;

    // We append additional characters to mLineLayout.mCharArray and mLineLayout.mAnalysisInfoArray.
    while(((analysisInfoArraySize = mLineLayout.mAnalysisInfoArray.size()) < kMaxAnalysisSize) && // While analysis size < max analysis size and
          (mAnalysisEnd.mnScheduleIndex < mScheduleEnd.mnScheduleIndex))                          // while there are any items left to analyze...
    {
        EA_ASSERT(mAnalysisEnd.mnScheduleIndex < mSchedule.size());

        Item& item = mSchedule[mAnalysisEnd.mnScheduleIndex];
        bool  bItemComplete = true;

        switch (item.mType)
        {
            case kItemTypeLineBreak:
            case kItemTypeParagraphBreak:
            {
                // Append 0x2028 (kCharLSEP) or 0x2029 (kCharPSEP) to the analysis text.
                // There is no need to append anything else to the analysis text, as line breaks always terminate lines.
                const Char c = ((item.mType == kItemTypeLineBreak) ? kCharLSEP : kCharPSEP);

                AddChar(c, item.mpStyle, false);
                break;
            }

            case kItemTypeObject:
            {
                // Possibly append ZWNBSP to the analysis text.
                // Append 0xFFFC (kCharOBJ) to the analysis text.
                // Possibly append ZWNBSP to the analysis text.

                if(item.mObject.mbBindPrev)
                    AddChar(kCharZWNBSP, item.mpStyle, false);

                AddChar(kCharOBJ, item.mpStyle, false); // Specially use 'false' here.

                // We have the following to still deal with. The shaping engine will need to 
                // know what the GlyphMetrics are for the object and the user will want to know
                // what the given object pointer is for the object.
                // One way to deal with this is to make a vector of mpObjectPtr/mGlyphMetrics
                // and have some extra bits/bytes in the AnalysisInfo for the character which 
                // refer to the mpObjectPtr/mGlyphMetrics. Or we can store a pointer to this 
                // schedule item in the AnalysisInfo.
                //     void*        mpObjectPtr;
                //     GlyphMetrics mGlyphMetrics;

                if(item.mObject.mbBindNext)
                    AddChar(kCharZWNBSP, item.mpStyle, false);

                break;
            }

            case kItemTypeText:
            {
                // Append some or all of the text to the analysis text.
                // Append only some of it if the text is overly long.
                EA_ASSERT(mAnalysisEnd.mnCharOffset <= item.mCount);
                EA_ASSERT(analysisInfoArraySize < kMaxAnalysisSize);

                const index_t nMaxCount = (kMaxAnalysisSize - analysisInfoArraySize);
                const Char*   pCurrent  = item.mText.mpCharData + mAnalysisEnd.mnCharOffset;
                const Char*   pEnd      = item.mText.mpCharData + item.mCount;      // Also: pEnd is equal to (pCurrent + nCount)
                const index_t nCount    = (item.mCount - mAnalysisEnd.mnCharOffset);

                EA_ASSERT(pCurrent <= pEnd);

                if(nCount > nMaxCount) // If this string is long and causes mLineLayout.mAnalysisInfoArray to go over kMaxAnalysisSize...
                {
                    // We have a problem here. We probably want to set pEnd to be 
                    // on a character cluster boundary, lest we leave trailing text
                    // that is an invalid cluster. We can use CharacterBreakIterator
                    // to walk the text on cluster boundaries. Let's try that.

                    const TextRun tr(pCurrent, (uint32_t)nCount);
                    CharacterBreakIterator cbi(&tr, 1);
                    uint32_t b = 0;

                    while(b < nMaxCount)
                        b = cbi.GetNextCharBreak();

                    pEnd = pCurrent + b;
                    EA_ASSERT((pCurrent <= pEnd) && (pEnd <= (item.mText.mpCharData + item.mCount)));
                }

                // OK, currently we apply all the text in this segment, even if it goes over kMaxAnalysisSize. 
                while(pCurrent < pEnd)
                {
                    AddChar(*pCurrent, item.mpStyle, true);

                    ++mAnalysisEnd.mnCharOffset;
                    ++pCurrent;
                }

                if(mAnalysisEnd.mnCharOffset != item.mCount) // If we still have text to process in this Item...
                    bItemComplete = false;

                break;
            } // switch
        } // while

        if(bItemComplete)
        {
            mAnalysisEnd.mnScheduleIndex++;
            mAnalysisEnd.mnCharBase   += mAnalysisEnd.mnCharOffset;
            mAnalysisEnd.mnCharOffset = 0;
        }
    }

    const eastl_size_t charArraySize = mLineLayout.mCharArray.size();
    if(charArraySize > charArraySizeOriginal) // If anything was added above...
    {
        // We apply Unicode Standard Annex #9 (a.k.a. TR9) Bidi algorithm.
        // If EATEXT_BIDI_SUPPORTED is disabled, then Bidi sets the analysis to LTR values.
        Bidi(mLayoutSettings.mDirection, &mLineLayout.mCharArray[0], &mLineLayout.mAnalysisInfoArray[0], charArraySize);
    }
}


///////////////////////////////////////////////////////////////////////////////
// AddChar
//
void Typesetter::AddChar(Char c, const TextStyle * pStyle, bool bTestForInvalidChars)
{
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4061 4062) // enumerate in switch of enum is not explicitly handled
    #endif

    // What we do here is add the Char to mCharArray and add a corresponding
    // entry to mAnalysisInfoArray. We fill out some of the mAnalysisInfoArray
    // data, but not all of it.

    EA_ASSERT(mLineLayout.mCharArray.size() == mLineLayout.mAnalysisInfoArray.size());

    // We reserve kCharObj for laying out of inline objects. If the incoming 
    // Unicode text happens to have one of these chars, then we don't want it
    // to be used, as it would be confused with our reserved usage of such chars.
    if(bTestForInvalidChars && (c == kCharOBJ))
        c = kCharREPL;

    const eastl_size_t initialSize = mLineLayout.mCharArray.size();

    mLineLayout.mCharArray.push_back(c);
    mLineLayout.mAnalysisInfoArray.push_back();

    AnalysisInfo& ai      = mLineLayout.mAnalysisInfoArray.back();
    AnalysisInfo* pAIPrev = (initialSize ? &mLineLayout.mAnalysisInfoArray[initialSize - 1] : NULL);

    EA_ASSERT(pStyle);
    ai.mpTextStyle = pStyle;

    // GetScriptFromChar returns the script associated with the Unicode character.
    // However, it returns kScriptCommon for characters that aren't associated 
    // with any script, such as symbol characters like '#' and '8'. The question
    // is: Do we do something about kScriptCommon values here or do we postpone
    // their consideration until the shaping phase?
    ai.mScript = GetScriptFromChar(c);

    // We check for any scripts that trigger non-general shaping. Doing so allows
    // us to do quicker processing by eliminating character processing pathways
    // that would have no effect due to all text being general. We include all 
    // scripts that we might conceivably support in the future, because if we 
    // didn't then when those were added then the new script programmer wouldn't
    // necessarily know to come edit this section.
    switch(ai.mScript)
    {
        case kScriptArabic:
        case kScriptBengali:
        case kScriptBopomofo:
        case kScriptDevanagari:
        case kScriptHangul:
        case kScriptHebrew:
        case kScriptLao:
        case kScriptThai:
            mbNonGeneralScriptPresent = true;
            break;

        // The following are known to work with the general shaper.
        // case kScriptCyrillic:
        // case kScriptGreek:
        // case kScriptHan:
        // case kScriptHiragana:
        // case kScriptKatakana:
        default:
            break;
    }

    if(pAIPrev)
    {
        ai.mbJoinBefore      = IsCharJoinControl(mLineLayout.mCharArray[initialSize - 1]) ? 1 : 0;
        pAIPrev->mbJoinAfter = IsCharJoinControl(c); 
    }
    else
        ai.mbJoinBefore = 0;

    ai.mbJoinAfter = 0; // We can't answer this until the next character.

    // UpdateAnalysis will update the rest of the AnalysisInfo.

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFontSelection
//
void Typesetter::UpdateFontSelection(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    EA_ASSERT(iCharEnd > iCharBegin);

    if(mLayoutSettings.mbDynamicFontSelection)
    {
        if(!mpFontServer)
            mpFontServer = GetFontServer(true);
        EA_ASSERT(mpFontServer);

        if(mLayoutSettings.mTextStyleDefault.mPasswordMode == kPMPassword) // If the text is password text...
        {
            // Get a Font for password text. Password text ideally consists 
            // of Unicode 0x25CF (9679), but if that isn't present then we 
            // can fall back to using '*'. For now we just get the first 
            // font we can find for the given script and default TextStyle.

            Font* const pFont = mpFontServer->GetFont(mTextStyleArray[0], NULL, 0, mPasswordChar, kScriptUnknown, true);
            GlyphId     glyphId;

            if(!pFont->GetGlyphIds(&mPasswordChar, 1, &glyphId, false))
                mPasswordChar = '*'; // We make the assumption here that pFont at least has a '*' char. But really you want to just make sure a true password bullet char is in your font set.

            for(eastl_size_t i = iCharBegin; i < iCharEnd; i++)
                mLineLayout.mAnalysisInfoArray[i].mpFont = pFont;

            pFont->Release();
        }
        else
        {
            Font*            pSavedFont        = NULL;
            Font*            pCurrentFont      = NULL;
            const TextStyle* pCurrentTextStyle = NULL;

            for(eastl_size_t i = iCharBegin; i < iCharEnd; i++)
            {
                const Char   c      = mLineLayout.mCharArray[i];
                const Script script = mLineLayout.mAnalysisInfoArray[i].mScript;

                if(pCurrentTextStyle == mLineLayout.mAnalysisInfoArray[i].mpTextStyle)  // If using the same style as the last char...
                {
                    if(pCurrentFont) // In practice this should always be so due to the logic here, but we check anyway.
                    {
                        if(!pCurrentFont->IsCharSupported(c, script)) // If the font doesn't support this char, do a new lookup.
                        {
                            pSavedFont   = pCurrentFont; // Save the current font so that we can use a different font just for this char.
                            pCurrentFont = mpFontServer->GetFont(pCurrentTextStyle, NULL, 0, c, script, true);
                            pCurrentFont->Release();

                            // We are losing our refcount on this font here due to the Release and if 
                            // the user removes the font from the server then our reference will be stale. 
                            // An alternative would be to store the font in a member mFontSelection. 
                            // But that would add yet more overhead to layout font maintenance.
                        }
                    }
                }
                else // Else the style has changed.
                {
                    pCurrentTextStyle = mLineLayout.mAnalysisInfoArray[i].mpTextStyle;

                    pCurrentFont = mpFontServer->GetFont(pCurrentTextStyle, NULL, 0, c, script, true);
                    pCurrentFont->Release();
                }
 
                EA_ASSERT(pCurrentFont != NULL);
                mLineLayout.mAnalysisInfoArray[i].mpFont = pCurrentFont;

                if(pSavedFont)
                {
                    pCurrentFont = pSavedFont;
                    pSavedFont = NULL;
                }
            }
        }
    }
    else // Else the user has specifically chosen the font set for us to use.
    {
        EA_ASSERT(!mLayoutSettings.mFontSelection.empty());

        if(!mLayoutSettings.mFontSelection.empty())
        {
            // We need to go through the user-supplied Fonts and find which ones
            // are best for each character. This is a significant issue because the
            // text may have characters that aren't supported by any given font.

            for(eastl_size_t i = iCharBegin; i < iCharEnd; i++)
            {
                const Char c = mLineLayout.mCharArray[i];

                if((c < 0x0080) || (mLayoutSettings.mFontSelection.size() == 1)) // We assume all fonts support ASCII/English (c <= 0x0080).
                    mLineLayout.mAnalysisInfoArray[i].mpFont = *mLayoutSettings.mFontSelection.begin();
                else
                {
                    FontSelection::iterator it, itEnd;

                    for(it = mLayoutSettings.mFontSelection.begin(), itEnd = mLayoutSettings.mFontSelection.end(); it != itEnd; ++it)
                    {
                        Font* const pFont = *it;

                        if(pFont->IsCharSupported(c, mLineLayout.mAnalysisInfoArray[i].mScript))
                            break;
                    }

                    if(it != itEnd)
                        mLineLayout.mAnalysisInfoArray[i].mpFont = *it;
                    else
                        mLineLayout.mAnalysisInfoArray[i].mpFont = *mLayoutSettings.mFontSelection.begin();
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// ShapeText
//
void Typesetter::ShapeText()
{
    // We shape all the text that's currently in mCharArray / mAnalysisInfoArray.
    // We do this by shaping contiguous segments of the text that are of the
    // same script.

    // To consider: The following implementation is index-based, whereas we can 
    // make it a little more efficient if it was iterator-based, though it 
    // would be harder to read.

    if(!mLineLayout.mCharArray.empty()) // If there is anything to shape...
    {
        // Clear the glyph portion of the line layout.
        mLineLayout.NewGlyphs();

        // Update the font selections.
        eastl_size_t     nIndexCurrentStyleRun = 0;
        const TextStyle* pTextStyleCurrent     = mLineLayout.mAnalysisInfoArray[0].mpTextStyle;

        for(eastl_size_t i = 0, iEnd = mLineLayout.mCharArray.size(); i < iEnd; ++i)  // The code below relies on (i < iEnd) as opposed to (i != iEnd)
        {
            const TextStyle* const pTextStyle = mLineLayout.mAnalysisInfoArray[i].mpTextStyle;
            const bool             bTextEnd   = ((i + 1) == iEnd);

            if(bTextEnd)
                i++;

            EA_ASSERT(pTextStyle);
            if((pTextStyleCurrent != pTextStyle) || bTextEnd)
            {
                UpdateFontSelection(nIndexCurrentStyleRun, i);

                nIndexCurrentStyleRun = i;
                pTextStyleCurrent     = pTextStyle;
            }
        }

        EA_ASSERT(pTextStyleCurrent);

        if(!mbNonGeneralScriptPresent) // Shortcut: If all text is Latin or one of the other generally shaped scripts...
        {
            const eastl_size_t size = mLineLayout.mCharArray.size();

            // Question: Should we use kScriptLatin here or should we use mLayoutSettings.mScript?
            mBidiRunInfoArray.push_back(RunInfo(kScriptLatin, 0, 0, size));
            ShapeText(kScriptLatin, 0, size);
        }
        else
        {
            mBidiRunInfoArray.push_back();

            // Shape the text, run by run.
            eastl_size_t nIndexCurrentRun  = 0;
            Script       scriptCurrentRun  = mLineLayout.mAnalysisInfoArray[0].mScript;
            int          levelCurrentRun   = mLineLayout.mAnalysisInfoArray[0].mnBidiLevel;
            RunInfo*     pRunInfo          = &mBidiRunInfoArray[0];

            for(eastl_size_t i = 0, iEnd = mLineLayout.mCharArray.size(); i <= iEnd; ++i) // Note that we do the unusual thing of comparing (i <= iEnd) instead of (i < iEnd).
            {
                // kScriptCommon refers to characters (e.g. ' ' char) that don't belong to
                // any particular script, or put another way, they can be used with any script.

                const Script script            = (i < iEnd) ? mLineLayout.mAnalysisInfoArray[i].mScript     : kScriptUnknown;
                const int    bidiLevel         = (i < iEnd) ? mLineLayout.mAnalysisInfoArray[i].mnBidiLevel : -1;
                const bool   bScriptChanged    = (script != scriptCurrentRun) && (script != kScriptInherited);
                const bool   bBidiLevelChanged = (bidiLevel != levelCurrentRun);
                const bool   bChangeOccurred   = (i == iEnd) || bScriptChanged || bBidiLevelChanged;

                if(EASTL_UNLIKELY(bChangeOccurred)) // If we are at the final element, have a change in script, or have a change in bidi direction...
                {
                    pRunInfo->mScript     = scriptCurrentRun;
                    pRunInfo->mnBidiLevel = levelCurrentRun;
                    pRunInfo->mnCharEnd   = i; // mnCharBegin will be set already and we don't want to set it here. It is not necessarily equal to nIndexCurrentRun.

                    EA_ASSERT(i > nIndexCurrentRun); // Assert that there is something to shape. The math above should guarantee this.
                    ShapeText(scriptCurrentRun, nIndexCurrentRun, i);

                    scriptCurrentRun = script;
                    levelCurrentRun  = bidiLevel;
                    nIndexCurrentRun = i;

                    if(bBidiLevelChanged && (i != iEnd)) // If we are about to begin a new bidi level...
                    {
                        mBidiRunInfoArray.push_back(RunInfo(scriptCurrentRun, levelCurrentRun, nIndexCurrentRun, nIndexCurrentRun));
                        pRunInfo = &mBidiRunInfoArray.back();
                    }

                    EA_ASSERT(mLineLayout.mGlyphIndexArray.size() == i);
                }
            }
        }

        EA_ASSERT(mLineLayout.mGlyphIndexArray.size() == mLineLayout.mCharArray.size());
    }

    EA_ASSERT(mBidiRunInfoArray.empty() == mLineLayout.mCharArray.empty());
}


///////////////////////////////////////////////////////////////////////////////
// ShapeText
//
void Typesetter::ShapeText(Script script, eastl_size_t iBegin, eastl_size_t iCharEnd)
{
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4061 4602) // enumerate in switch of enum is not explicitly handled by a case label
    #endif

    switch (script)
    {
        case kScriptArabic:
            ShapeArabic(iBegin, iCharEnd);
            break;

        case kScriptDevanagari: // Hindi
        case kScriptBengali:
        case kScriptGujarati:
        case kScriptGurmukhi:
        case kScriptMalayalam:
        case kScriptKannada:
        case kScriptOriya:
        case kScriptTamil:
        case kScriptSinhala:
        case kScriptTibetan:
        case kScriptTelugu:
        case kScriptLimbu:
            ShapeIndic(iBegin, iCharEnd);
            break;

        case kScriptHangul:
            ShapeHangul(iBegin, iCharEnd);
            break;

        case kScriptHebrew:
            ShapeHebrew(iBegin, iCharEnd);
            break;

        case kScriptThai:
            ShapeThai(iBegin, iCharEnd);
            break;

        // The following scripts should be supported by ShapeGeneral to a usable 
        // degree. Some of these scripts are useless for the kind of software we
        // write, but it's useful to document that the general shaper can handle them.
        //    Armenian Bopomofo Cherokee Coptic Cyrillic Deseret Ethiopic Georgian 
        //    Gothic Greek Han Hiragana Katakana Latin Ogham OldItalic Runic Yi Braille
        //    Cypriot Limbu Osmanya Shavian LinearB Ugaritic

        default:
            ShapeGeneral(iBegin, iCharEnd);
            break;
    }

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphsForChar
//
// This is a basic function that converts chars to glyphs without respect to 
// shaping or script-specific knowledge. It can nevertheless be used by 
// shaping engines to convert characters to glyphs.
// Increments the glyphIdCount parameter by the number of generated glyphs,
// which will be equal to the 'count' input value.
// Returns the number of Chars eaten.
//
eastl_size_t Typesetter::GetGlyphsForChar(const Char* pChar, eastl_size_t count, 
                                          const AnalysisInfo* pAnalysisInfo, GlyphId* pGlyphIdArray, eastl_size_t& glyphIdCount,
                                          const Char* pFallbackOptions, eastl_size_t fallbackOptionCount)
{
    (void)count; // Prevent compiler warnings.
    EA_ASSERT(count > 0);

    if(EA_LIKELY(*pChar != kCharOBJ))
    {        
        uint32_t glyphCount = pAnalysisInfo->mpFont->GetGlyphIds(pChar, 1, pGlyphIdArray, fallbackOptionCount == 0);

        for(eastl_size_t i = 0; !glyphCount && (i < fallbackOptionCount); ++i)
            glyphCount = pAnalysisInfo->mpFont->GetGlyphIds(pFallbackOptions + i, 1, pGlyphIdArray, (i + 1) < fallbackOptionCount); // For the last try, set the last argument to true (always return valid glyph).
    }
    else
        pGlyphIdArray[0] = kGlyphIdInvalid;

    // As it stands now, we only get one glyph per Char.
    glyphIdCount++;

    return 1;
}


///////////////////////////////////////////////////////////////////////////////
// SetGlyphLayoutInfo
//
void Typesetter::SetGlyphLayoutInfo(GlyphLayoutInfo& glyphLayoutInfo, float fPenX, const GlyphMetrics& glyphMetrics)
{
    // To consider: We could possibly implement TextStype::mfStretch here by multiplying the resulting
    // metrics below by mfStretch. However, such an approach would not look as good as if the font
    // were originally measured and rasterized with stretch. In particular, the texture would likely
    // be set to do point sampling and thus a stretched glyph would look blocky instead of smooth.
    // So if the user wants to implement font stretch well, it's better if the user use the Font::SetTransform
    // function to control this. 

    glyphLayoutInfo.mfPenX    = fPenX;
    glyphLayoutInfo.mfPenY    = 0; // We apply mfLayoutPenY in the final positioning step.
    glyphLayoutInfo.mfX1      = glyphLayoutInfo.mfPenX +  glyphMetrics.mfHBearingX;
    glyphLayoutInfo.mfY1      = glyphLayoutInfo.mfPenY + (glyphMetrics.mfHBearingY * -mLayoutSettings.mfYScale);

    // For vertical text, we would do different processing here.
    glyphLayoutInfo.mfX2      = glyphLayoutInfo.mfPenX +  (glyphMetrics.mfHBearingX + glyphMetrics.mfSizeX);
    glyphLayoutInfo.mfY2      = glyphLayoutInfo.mfPenY + ((glyphMetrics.mfHBearingY - glyphMetrics.mfSizeY) * -mLayoutSettings.mfYScale);
    glyphLayoutInfo.mfAdvance = glyphMetrics.mfHAdvanceX;
}



#if (EATEXT_LIGATURE_SUPPORT >= 1) // If we have basic ligature support...

    namespace
    {
        struct LigatureMapEntry
        {
            Char mChars[4];
            Char mChar;
        };

        const LigatureMapEntry lmeArray[9] =
        { 
            { EA_CHAR16("ffl"), 0xFB04 }, // Note that we place ffl before ff, so as to catch it first.
            { EA_CHAR16("ffi"), 0xFB03 }, 
            { EA_CHAR16("fl"),  0xFB02 }, 
            { EA_CHAR16("fi"),  0xFB01 }, 
            { EA_CHAR16("ff"),  0xFB00 }, 
            { EA_CHAR16("oe"),  0x0153 }, 
            { EA_CHAR16("OE"),  0x0152 }, 
            { EA_CHAR16("ij"),  0x0133 }, 
            { EA_CHAR16("IJ"),  0x0132 },
        };

        eastl_size_t LigatureMatch(const Char* pLigature, const Char* pSource, const Char* pSourceEnd)
        {
            // We could also use the EASTL 'identical' algorithm here, though what we do here
            // is a little more staightforward for the case that pLigature is 0-terminated.
            eastl_size_t i = 0;

            while((pSource < pSourceEnd) && (*pSource == *pLigature))
            {
                ++pSource;
                ++pLigature;
                ++i;
            }    

            return *pLigature ? 0 : i;
        }

    } // namespace

#endif


///////////////////////////////////////////////////////////////////////////////
// GetUnicodeLigatureGlyph
//
eastl_size_t Typesetter::GetUnicodeLigatureGlyph(const Char* pChar, eastl_size_t count, Font* pFont, GlyphId& glyphId)
{
    // We don't implement EATEXT_LIGATURE_SUPPORT >= 2) here because that level
    // of ligature support is at the glyph substitution level as opposed to the
    // Unicode character substitution level.

    #if (EATEXT_LIGATURE_SUPPORT >= 1) // If we have basic ligature support...
        const size_t kTableCount = sizeof(lmeArray) / sizeof(lmeArray[0]);

        for(size_t i = 0; i < kTableCount; i++)
        {
            EA_ASSERT(Strlen8(lmeArray[i].mChars) <= kMaxLigatureLength);

            const eastl_size_t charCount = LigatureMatch(lmeArray[i].mChars, pChar, pChar + count);

            if(charCount) // If we have a match...
            {
                const Char cLigature = lmeArray[i].mChar;

                if(pFont->GetGlyphIds(&cLigature, 1, &glyphId, true))
                    return charCount;
            }
        }
    #else
        (void)count; // Prevent compiler warnings.
    #endif

    pFont->GetGlyphIds(pChar, 1, &glyphId, true);
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
// GetObjectMetrics
//
void Typesetter::GetObjectMetrics(index_t iGlyph, GlyphMetrics& glyphMetrics)
{
    const index_t     iChar = mLineLayout.GetCharIndexFromGlyphIndex(iGlyph); // iChar is line-relative.
    const Item* const pItem = GetScheduleItemFromCharIndex(iChar, true);

    EA_ASSERT(pItem && (pItem->mType == kItemTypeObject));
    glyphMetrics = pItem->mObject.mGlyphMetrics;
}


///////////////////////////////////////////////////////////////////////////////
// UpdateLineState
//
void Typesetter::UpdateLineState()
{
    // This function attempts to measure the currently shaped text against
    // the current layout space. If the text is more than enough to fill
    // the space, the line state is set to kLineStateFull. Otherwise it is
    // left as kStatePartial and the user needs to add more text in order
    // to achieve kLineStateFull.

    index_t       nCurrentBreakIndex(0);
    const index_t nTextLength(mLineLayout.mCharArray.size());

    if(nCurrentBreakIndex < nTextLength)
    {
        const int         kBreakFlags(kLineBreakTypePossible | kLineBreakTypeMandatory | kLineBreakTypeHyphen);
        LineBreakType     lineBreakType(kLineBreakTypePossible);
        const TextRun     textRun(mLineLayout.mCharArray.data(), (uint32_t)nTextLength);
        LineBreakIterator lineBreakIterator(&textRun, 1);
        index_t           nNextBreakIndex(0);
        const float       fLayoutWidth(mfLayoutSpace);
        float             fCurrentBreakWidth(0.f);
        float             fNextBreakWidth(0.f);
        TextWrap          textWrap;

        if(mbEllipsize)
            textWrap = kTWNone;
        else if(mLayoutSettings.mTextStyleDefault.mTextWrap == kTWWrap) // In practice kTWWrap seems to result in potential hangs with certain input. 
            textWrap = kTWEmergency;                                    // It's not really this function's fault that the hangs occur, but it's easiest
        else                                                            // if this function just disables kTWWrap anyway.
            textWrap = mLayoutSettings.mTextStyleDefault.mTextWrap;

        if(mLayoutSettings.mTextStyleDefault.mHAlignment == kHAJustify) // The Unicode Standard Annex #14, section 3.1, states: "Korean makes use of both styles of line break."
            lineBreakIterator.SetUseWesternBreaksWithKorean(false);     //    "When Korean text is justified, the second style is commonly used, even for interspersed Latin letters. But when ragged margins are used, the Western style (relying on spaces) is commonly used instead, even for ideographs."

        while(nCurrentBreakIndex < nTextLength) // While we still have more text to examine...
        {
            nNextBreakIndex = (index_t)lineBreakIterator.GetNextLineBreak(kBreakFlags);
            fNextBreakWidth = GetTextWidth(nNextBreakIndex);
            lineBreakType   = lineBreakIterator.GetLineBreakType();

            if((fNextBreakWidth > fLayoutWidth) && (textWrap != kTWNone)) // If this last break goes over the alotted space...
            {
                if(textWrap == kTWHard) // If we are to allow words to overhang over the right side of the margin...
                {
                    mLineLayout.mLineMetrics.mnLineLength = nNextBreakIndex;
                    mLineLayout.mLineMetrics.mfSpace      = fNextBreakWidth;
                }
                else if((textWrap == kTWSoft) || ((textWrap == kTWEmergency) && (nCurrentBreakIndex == 0))) // If we are to break on grapheme cluster boundaries instead of word boundaries...
                {
                    // Find the last grapheme cluster that fits on the line.
                    // Start with nCurrentBreakIndex / fCurrentBreakWidth and   
                    // add the widths of each cluster until we don't fit.
                    index_t nSavedBreakIndex(nNextBreakIndex);
                    float   fSavedBreakWidth(fNextBreakWidth);

                    CharacterBreakIterator charBreakIterator(&textRun, 1);
                    charBreakIterator.SetPosition((uint32_t)nCurrentBreakIndex);

                    while(nCurrentBreakIndex < nTextLength)
                    {
                        nNextBreakIndex = (index_t)charBreakIterator.GetNextCharBreak();
                        fNextBreakWidth = GetTextWidth(nNextBreakIndex);

                        if(fNextBreakWidth > fLayoutWidth)  // If this break position results in text that's over the edge...
                        {
                            mLineLayout.mLineMetrics.mnLineLength = nSavedBreakIndex;  // Go back to the previous break.
                            mLineLayout.mLineMetrics.mfSpace      = fSavedBreakWidth;
                            break;
                        }

                        nSavedBreakIndex = nNextBreakIndex;
                        fSavedBreakWidth = fNextBreakWidth;
                    }
                }
                else // (textWrap == kTWWrap) || ((textWrap == kTWEmergency) && (nCurrentBreakIndex > 0))
                {
                    if(nCurrentBreakIndex > 0) // If there were any chars that fit...
                    {
                        mLineLayout.mLineMetrics.mnLineLength = nCurrentBreakIndex;
                        mLineLayout.mLineMetrics.mfSpace      = fCurrentBreakWidth;
                    }
                    else // Else we had one long word or fLayoutWidth was very small to start with.
                    {
                        // This is a dangerous situation, as it could result in infinite loops trying to fit text.
                        mLineLayout.mLineMetrics.mnLineLength = 0;
                        mLineLayout.mLineMetrics.mfSpace      = 0;
                        mLineState = kLineStatePartial;
                        return;
                    }
                }

                mLineState = kLineStateFull;
                return;
            }
            else if(nNextBreakIndex == nTextLength)
            {
                EA_ASSERT(lineBreakIterator.AtEnd());
                // Fall through. We'll break out of the loop naturally.
            }
            else if(lineBreakType == kLineBreakTypeMandatory) // If we are at a \n char or equivalent... we always break.
            {
                mLineLayout.mLineMetrics.mnLineLength = nNextBreakIndex;
                mLineLayout.mLineMetrics.mfSpace      = fNextBreakWidth;
                mLineState = kLineStateFull;
                return;
            }

            nCurrentBreakIndex = nNextBreakIndex;
            fCurrentBreakWidth = fNextBreakWidth;
        }

        // At this point, we ran out of text without filling our space.
        // We'll update our layout stats to be what the were as of the end of the text.
        mLineLayout.mLineMetrics.mnLineLength = nCurrentBreakIndex;
        mLineLayout.mLineMetrics.mfSpace      = fCurrentBreakWidth;
        mLineState = kLineStatePartial;
    }

    // Assert that if the line state is full, then line length and line space are sane values.
    EA_ASSERT((mLineState != kLineStateFull) || 
              ((mLineLayout.mLineMetrics.mnLineLength < 100000.f) && 
               (mLineLayout.mLineMetrics.mfSpace      >= 0.f)     &&
               (mLineLayout.mLineMetrics.mfSpace      < 1000000.f)));
}


///////////////////////////////////////////////////////////////////////////////
// GetTextWidth
//
float Typesetter::GetTextWidth(eastl_size_t iCharEnd)
{
    // The calculation here assumes that the entire layed out text is layed
    // out with the starting pen at position zero, and with any RTL text 
    // being layed out up till this point as LTR. Bidirectional text is 
    // reversed later in the text layout pipeline.

    if(iCharEnd)
    {
        // Note: When you have a glyph cluster (e.g. character plus diacritic), 
        // usually one of them has an advance width of some value while the other(s) 
        // have a zero value. The code here assumes that the last glyph in the 
        // cluster is the one with the non-zero advance width. 
        //
        // Get the last glyph. Note that we use (GetGlyphIndexFromCharIndex(iCharEnd) - 1) 
        // and not (GetGlyphIndexFromCharIndex(iCharEnd - 1)).
        eastl_size_t iGlyphLast = mLineLayout.GetGlyphIndexFromCharIndex(iCharEnd);

        if(iGlyphLast > 0)
            --iGlyphLast;

        EA_ASSERT(iGlyphLast < 100000); // Make sure it isn't some (unsigned) negative value.

        return mLineLayout.mGlyphLayoutInfoArray[iGlyphLast].mfPenX + 
               mLineLayout.mGlyphLayoutInfoArray[iGlyphLast].mfAdvance;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SubstituteEllipsis
//
// What we need to do here is modify the glyph info and glyph layout info
// for the line. This is a little tricky because we need to make sure we 
// stay on cluster boundaries and need to rebuild the glyph and char index
// arrays appropriately. The ellipsis retains the embedding level of the 
// last glyph which fit on the line.
//
// Algorithm:
//     1. Find the ellipsis glyph and get its metrics.
//     2. Find the last glyph on the line that begins a cluster and whose prev char xpen + advance + ellipsis advance < layout width.
//     3. Replace this last glyph with the ellipsis glyph and its info and metrics. Leave GlyphInfo::mDirection as-is, though.
//     4. Chop GlyphArray, GlyphInfoArray, GlyphLayoutInfo, and CharIndexArray array to the position of the ellipsis glyph.
//     5. Set all the replaced entries of GlyphIndexArray to refer to the ellipsis glyph index.
//
void Typesetter::SubstituteEllipsis()
{
    // EA_ASSERT(mLineState == kLineStateFull); Disabled, as the calling function hasn't set kLineStateFull yet when calling us.

    if(mLineLayout.mLineMetrics.mfSpace > mfLayoutSpace)
    {
        eastl_size_t i, size;

        // 0. Assert pre-conditions.
        EA_ASSERT(!mLineLayout.mAnalysisInfoArray.empty());
        Validate();

        // 1. Find the ellipsis glyph and get its metrics.
        Char            c = kCharEllipsis;
        GlyphId         glyphIdArray[4]; // We should need only one, but we are putting some future-proof into this.
        eastl_size_t    glyphIdCount = 0;
        GlyphMetrics    glyphMetrics;
        GlyphInfo       glyphInfo;
        GlyphLayoutInfo glyphLayoutInfo;
        AnalysisInfo*   pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[0]; // We currently use the font used for the first glyph in our line.

        glyphLayoutInfo.mpFont = pAnalysisInfo->mpFont;
        GetGlyphsForChar(&c, 1, pAnalysisInfo, glyphIdArray, glyphIdCount, EATEXT_CHAR("_"), 1); // An alternative would be the multi-glyph sequence of "...". However, a large majority of modern fonts have true ellipsis. Also, "..." is "bad typography" and we want to discourage it.
        glyphLayoutInfo.mpFont->GetGlyphMetrics(glyphIdArray[0], glyphMetrics);
        SetGlyphLayoutInfo(glyphLayoutInfo, 0, glyphMetrics);
        glyphInfo.mGJC              = kGJCNone;
        glyphInfo.mbGlyphIsObject   = 0;
        glyphInfo.mClusterPosition  = 0;
        glyphInfo.mClusterSize      = 1;
        glyphInfo.mClusterBreakable = 0;
        glyphInfo.mCharCount        = 1;
        glyphInfo.mDirection        = kDirectionLTR; // This value may change below.
        glyphInfo.mOpenTypeLookupFlags = 0;
        glyphInfo.mScriptFlags = 0;

        // 2. Find the last glyph on the line that begins a cluster and whose prev char xpen + advance + ellipsis advance < layout width.
        eastl_size_t ellipsisGlyphIndex = 0;
        float fEllipsisPenX = 0;
        const float fPenMin = mfLayoutSpace - glyphLayoutInfo.mfAdvance;

        for(i = mLineLayout.mGlyphLayoutInfoArray.size() - 1, size = mLineLayout.mGlyphLayoutInfoArray.size(); i < size; i--) // eastl__size_t is unsigned and doesn't go negative.
        {
            if((mLineLayout.mGlyphInfoArray[i].mClusterPosition == 0) &&
               (mLineLayout.mGlyphLayoutInfoArray[i].mfPenX <= fPenMin)) // If we are on a cluster boundary...
            {
                ellipsisGlyphIndex = i;
                fEllipsisPenX      = mLineLayout.mGlyphLayoutInfoArray[i].mfPenX;
                break;
            }
        }

        // 3. Replace this last glyph with the ellipsis glyph and its info and metrics. Leave GlyphInfo::mDirection as-is, though.
        glyphInfo.mDirection = mLineLayout.mGlyphInfoArray[ellipsisGlyphIndex].mDirection;
        glyphLayoutInfo.mfPenX += fEllipsisPenX;
        glyphLayoutInfo.mfX1   += fEllipsisPenX;
        glyphLayoutInfo.mfX2   += fEllipsisPenX;
        mLineLayout.mGlyphArray[ellipsisGlyphIndex]           = glyphIdArray[0];
        mLineLayout.mGlyphInfoArray[ellipsisGlyphIndex]       = glyphInfo;
        mLineLayout.mGlyphLayoutInfoArray[ellipsisGlyphIndex] = glyphLayoutInfo;

        // 4. Chop GlyphArray, GlyphInfoArray, GlyphLayoutInfo, and CharIndexArray array to the position of the ellipsis glyph.
        mLineLayout.mGlyphArray          .resize(ellipsisGlyphIndex + 1);
        mLineLayout.mGlyphInfoArray      .resize(ellipsisGlyphIndex + 1);
        mLineLayout.mGlyphLayoutInfoArray.resize(ellipsisGlyphIndex + 1);
        mLineLayout.mCharIndexArray      .resize(ellipsisGlyphIndex + 1);

        // 5. Set all the replaced entries of GlyphIndexArray to refer to the ellipsis glyph index.
        for(i = mLineLayout.mGlyphIndexArray.size() - 1, size = mLineLayout.mGlyphIndexArray.size(); i < size; i--) // eastl__size_t is unsigned and doesn't go negative.
        {
            if(mLineLayout.mGlyphIndexArray[i] > (uint32_t)ellipsisGlyphIndex)
               mLineLayout.mGlyphIndexArray[i] = (uint32_t)ellipsisGlyphIndex;
            else
                break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// AdjustWhitespaceEmbedding
//
// Unicode Annex #9 (Bidi), section 3.4 L1 specifies that whitespace at the 
// end of a line or paragraph gets converted to the base paragraph embedding 
// level, if it isn't so already.
//
void Typesetter::AdjustWhitespaceEmbedding()
{
    #if EATEXT_BIDI_SUPPORTED
        // As of this writing, we don't implement tabs as per section 3.4 L1.
        // We treat tabs just like whitespace and thus only convert trailing
        // tabs along with trailing whitespace. Implementing tabs as per L1
        // would be somewhat painful and not very useful to the large majority
        // of end-users.

        if(!mBidiRunInfoArray.empty())
        {
            RunInfo& runInfo = mBidiRunInfoArray.back();

            EA_COMPILETIME_ASSERT((kDirectionLTR == 0) && (kDirectionRTL == 1)); // These are 0 and 1 by design, we assert they are so just to make sure.

            if((runInfo.mnBidiLevel % 2) != mLayoutSettings.mDirection) // If the last embedding level != the base embedding level...
            {
                // We need to take any trailing whitespace and newlines and 
                // make a new RunInfo for them whereby the embedding level 
                // is the base paragraph level.

                eastl_size_t iEnd = mLineLayout.mCharArray.find_last_not_of(EATEXT_CHAR(" \r\n\t")); // We probably want to check for Unicode newlines as well.

                if(iEnd != (mLineLayout.mCharArray.size() - 1)) // If there are any such trailing chars...
                {
                    // If the entire line is space characters, then it should already be so
                    // that the characters match the base embedding level of the paragraph.
                    // Thus the following ought to always evaluate as false.
                    if(iEnd == eastl::basic_string<Char>::npos) // If the entire line was space characters...
                    {
                        mBidiRunInfoArray.pop_back();
                        iEnd = 0;
                    }
                    else
                        runInfo.mnCharEnd = ++iEnd;

                    const RunInfo runInfoNew(kScriptUnknown, mLayoutSettings.mDirection, iEnd, mLineLayout.mCharArray.size());
                    mBidiRunInfoArray.push_back(runInfoNew);
                }
            }
    }
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// AdjustPositioning
//
void Typesetter::AdjustPositioning()
{
    /////////////
    // Metrics //
    /////////////

    for(eastl_size_t i = 0, iEnd = mLineLayout.mGlyphInfoArray.size(); i < iEnd; ++i)
    {
        GlyphLayoutInfo& gli = mLineLayout.mGlyphLayoutInfoArray[i];
        FontMetrics      fontMetrics;

        GlyphInfo& gi = mLineLayout.mGlyphInfoArray[i];

        if(gi.mbGlyphIsObject == 0) // If this is a regular character...
        {
            // Update the baseline and descent values for the line. 
            // If a line of characters is represented by multiple fonts, we want the 
            // line to know the max descent and max ascent found in these fonts.
            //
            // To consider: We are calling GetFontMetrics here for every character,
            // whereas we really only need to call it once for every newly represented
            // Font on the current line. There thus may be a way to optimize this better.
            gli.mpFont->GetFontMetrics(fontMetrics);

            if(fontMetrics.mfBaseline < -gli.mfY1)
               fontMetrics.mfBaseline = -gli.mfY1;

            if(fontMetrics.mfDescent > -gli.mfY2)
               fontMetrics.mfDescent = -gli.mfY2;
        }
        else
        {
            // Note that for our uses here, baseline is the same thing as the object's ascent, 
            // and for a simple rectangular object, the ascent is the same as the y-bearing.
            fontMetrics.mfBaseline = -gli.mfY1;
            fontMetrics.mfDescent  = -gli.mfY2;
        }

        // To consider: The font baseline and descent could be a fractional value, which it 
        // often is with some fonts. Thus the following mfBaseline and mfDescent could be fractional.
        if(mLineLayout.mLineMetrics.mfBaseline < fontMetrics.mfBaseline)
           mLineLayout.mLineMetrics.mfBaseline = fontMetrics.mfBaseline;

        if(mLineLayout.mLineMetrics.mfDescent > fontMetrics.mfDescent)
           mLineLayout.mLineMetrics.mfDescent = fontMetrics.mfDescent;
    }

    ///////////////
    // Alignment //
    ///////////////

    float fXOffset = mfLayoutPenX;
    float fYOffset = mfLayoutPenY;

    // It turns out that common practice is that when a line is the last line of
    // a paragraph, it isn't justified, but rather is left-aligned. The CSS standard
    // does not describe this; it appears to be a de-facto standard.
    if(mLayoutSettings.mTextStyleDefault.mHAlignment == kHAJustify)
    {
        // We check that the last char of the line is a paragraph separator, though it
        // might be more explicit if we provide some kind of SetLastLine() function
        // for the user.
        if(!mLineLayout.mCharArray.empty())
        {
            const Char c = mLineLayout.mCharArray.back();

            if((c == 0x000A) || // LINE FEED (LF)               kCharLF
               (c == 0x2029) || // PARAGRAPH SEPARATOR (PS)     kCharPSEP
               (c == 0x0085) || // NEXT LINE (NEL)              kCharNEL
               (c == 0x2028))   // LINE SEPARATOR (LS)          kCharLSEP
            {
                mLayoutSettings.mTextStyleDefault.mHAlignment = kHALeft;
            }
        }
    }

    // Question:
    // Do we use mLayoutSettings.mTextStyle.mDirection or mLayoutSettings.mDirection?
    // It seems to me that TextStyle::mDirection should be removed from TextStyle, 
    // as directionality is a paragraph style and not a text style.

    switch (mLayoutSettings.mTextStyleDefault.mHAlignment)
    {
        case kHAInherit:
        case kHALeft:
            if(mLayoutSettings.mDirection == kDirectionRTL)
                fXOffset += (mfLayoutSpace - mLineLayout.mLineMetrics.mfSpace);
            // Else we do nothing because in RTL text, left and right are reversed.
            break;

        case kHARight:
            if(mLayoutSettings.mDirection == kDirectionLTR)
                fXOffset += (mfLayoutSpace - mLineLayout.mLineMetrics.mfSpace);
            // Else we do nothing because in RTL text, left and right are reversed.
            break;

        case kHACenter:
            fXOffset += floorf((mfLayoutSpace - mLineLayout.mLineMetrics.mfSpace) / 2);
            break;

        case kHAJustify:
            Justify();
            break;
    }

    // To do (?): Deal with vertical alignment. This function (AdjustPositioning) is about 
    // the positioning of a single line. Vertical alignment might be best dealt with here
    // and maybe should be dealt with at a higher level with functions like LayoutParagraph.
    //switch (mLayoutSettings.mTextStyleDefault.mVAlignment)
    //{
    //    // kVATop, kVACenter, kVABottom
    //}
    
    if(mLayoutSettings.mbCellTopPosition) // If we want [0,0] to refer to the top of the line box instead of the baseline...
        fYOffset += floorf((mLineLayout.mLineMetrics.mfBaseline + 0.5f) * mLayoutSettings.mfYScale); // Round it.

    mLineLayout.Offset(fXOffset, fYOffset);
}


///////////////////////////////////////////////////////////////////////////////
// Justify
//
// We implement justification, which is the spreading out of text across the 
// available layout space. 
//
// Justification is discussed briefly in the Unicode Standard 4.0 section 5.13,
// but little information is given about the topic there. A discussion of space
// characters is given in section 6.2 and provides some information. Lastly, 
// section 15.2 discusses some of the space characters in more detail.
//
// * Latin *
// An ideal typesetting system might attempt to shrink the space between
// words instead of increase it if it turns out that such shrinking will allow
// one more word to fit on the line instead of being pushed to the next line.
// If we want to support such functionality, we would need to do it as part
// of the line breaking routine, then the final rearrangement would be done here.
// 
// The set of space characters that are eligible for expansion are: 0020, 00A0,
// 202F, 205F, and 3000. Other space chars such as 2002 ("EN SPACE") are 
// specifically fixed in width. Other space chars such as 200B ("ZERO WIDTH SPACE")
// have no width by design (and are thus also fixed in width). However, Thai
// uses zero width space to indicate breaks and so you can expand 200B if it
// is between Thai chars.
//
// We have mLayoutSettings.mTextStyleDefault.mJustify to tell us what kind of 
// justification to do based on the CSS 3 standard, though it would seem that
// we should be able to do this contextually instead of via a directive.
//
// We should probably not increase inter-character spacing in German, as such 
// spacing indicates emphasis in German and thus could confuse the user.
//
// * Chinese / Japanese *
// Ideagraphic scripts allow adjusting of distance between symbols where there 
// are no space characters between symbols of which to expand. However, Mozilla
// and Internet Explorer don't seem to employ this but instead expand only 
// space characters.
//
// * Thai *
// Normally, Unicode char 0x200B is of zero width and is not expanded.
// However, Thai uses zero width space to indicate breaks and so you can expand 
// 200B if it is between Thai chars.
//
// * Arabic *
// Arabic justification is done by extending glyphs via kashidas, which are 
// horizontal lines between glyphs. Arabic is a cursive script, which means it's 
// what we call "hand writing" as opposed to "printing". So if you want to make 
// your handwriting of a word stretch out as you write it, you make the connectors 
// between your letters longer. That's what kashidas are; they are little horizontal 
// line glyphs that implement this stretching. We can possibly have a variable such
// as mLayoutSettings.mfKashidaToTextRatio to tell us how much to stretch spaces 
// vs. stretching kashidas.
// 
// Note: We don't yet have support for Arabic justification, because as of this 
// writing we don't yet have support for Arabic shaping. 
//
// * Hyphenation *
// Justification via hyphenation is another option, but it doesn't appear that 
// web browsers emply automatic hyphenation. To support justification via 
// hyphenation, we would need to have a hyphenation dictionary so we can know
// where to safely put hyphenations, and we would need to modify the line
// breaking function to try hyphenation. Our LineBreakIterator already recognizes
// hyphenation but doesn't implement it. We likely will not implement automatic
// hyphenation unless it is asked for by somebody. 
//
void Typesetter::Justify()
{
    // To consider: Do something with ScriptProperties::mbCharacterJustify.
    // This may not be useful because we could be dealing with a paragraph
    // of arbitrary mixed scripts.
    //    ScriptProperties scriptProperties;
    //    GetScriptProperties(mLayoutSettings.mScript, &scriptProperties, true);

    // fExtraSpace is how much space we need to add to the line.
    const float fExtraSpace = (mfLayoutSpace - mLineLayout.mLineMetrics.mfVisibleSpace);

    // spaceIndexArray is a list of all characters that are candidates for inter-word spacing extension.
    eastl::fixed_vector<eastl_size_t, 64, true> spaceIndexArray; // To consider: Make this a member variable.

    // The full Unicode 4.0 set of space chars is: 0020, 00A0, 200B (Thai only), 202F, 205F, and 3000.
    // However, we ignore a couple of them because they are rare.
    for(eastl_size_t i = 0, iEnd = mLineLayout.mLineMetrics.mnVisibleLineLength; i < iEnd; i++)
    {
        const Char c = mLineLayout.mCharArray[i];

        if((c == 0x0020) || (c == 0x00A0) || (c == 0x3000)
           #if EATEXT_THAI_SCRIPT_SUPPORTED || EATEXT_LAO_SCRIPT_SUPPORTED
                // The line below checks to see if 200B is preceeded by a Thai char.
            || ((c == 0x200B) && (i > 0) && ((unsigned)mLineLayout.mCharArray[i - 1] - 0x0E00u) < (0x0E80u - 0x0E00u))
           #endif
           )
        {
            spaceIndexArray.push_back(i);
        }
    }

    if(!spaceIndexArray.empty())
    {
        // We go through the line's GlyphLayoutInfo array and expand the 
        // characters that were identified above. This is done by moving 
        // the glyphs after each space char rightward. Note that this works
        // for RTL text because we haven't re-ordered RTL text yet; it is 
        // still LTR.
        const float fPerSpaceIncrease = (fExtraSpace / spaceIndexArray.size());

        for(eastl_size_t i = 0, iEnd = spaceIndexArray.size(); i < iEnd; i++)
        {
            const eastl_size_t charIndex  = spaceIndexArray[i] + 1; // +1 because we want to move glyphs that are after the space.
            const eastl_size_t glyphIndex = mLineLayout.GetGlyphIndexFromCharIndex(charIndex);

            mLineLayout.OffsetSegment(fPerSpaceIncrease, 0, glyphIndex, mLineLayout.mGlyphLayoutInfoArray.size());
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// ReorderBidiRunInfoArray
//
// The Unicode Standard section 3.4 L2 says:
//    From the highest level found in the text to the lowest odd 
//    level on each line, including intermediate levels not actually 
//    present in the text, reverse any contiguous sequence of 
//    characters that are at that level or higher.
//
// We implement this algorithm, but do it on runs instead of 
// doing it on glyphs of runs. This is faster because we don't spend
// a lot of CPU cycles rearranging glyph positions repeatedly.
// And we can start with the highest level - 1 since we don't 
// need to reverse chars within a run.
//
void Typesetter::ReorderBidiRunInfoArray(RunInfoArray& runInfoArray)
{
    #if EATEXT_BIDI_SUPPORTED
        EA_ASSERT(!mBidiRunInfoArray.empty());

        // If we have just a single LTR run, we don't need to do anything complicated.
        if((mBidiRunInfoArray.size() == 1) && ((mBidiRunInfoArray[0].mnBidiLevel % 2) == 0)) // If there is one run and it is LTR...
            runInfoArray = mBidiRunInfoArray;
        else
        {
            typedef eastl::fixed_vector<eastl_size_t, 16, true> IndexArray;

            IndexArray indexArray; // Indexes into mBidiRunInfoArray.
            int        highestLevel   = INT_MIN;
            int        lowestOddLevel = INT_MAX;

            // Setup
            for(eastl_size_t i = 0, iEnd = mBidiRunInfoArray.size(); i < iEnd; i++)
            {
                const int level = mBidiRunInfoArray[i].mnBidiLevel;

                indexArray.push_back(i);

                if(level > highestLevel)
                    highestLevel = level;

                if((level % 2) && (level < lowestOddLevel))
                    lowestOddLevel = level;
            }

            if(highestLevel == INT_MIN)   // This shouldn't happen, because we wouldn't have bidi unless there
                highestLevel = 0;         // are both directions. That's what the bi in bidi means, of course.
            if(lowestOddLevel == INT_MAX)
                lowestOddLevel = 1;

            // "From the highest level found in the text to the lowest odd level" ...
            for(int e = (highestLevel - 1); e >= lowestOddLevel; e--)
            {
                // "reverse any contiguous sequence of characters that are at that level or higher"
                // Note that the vector iterators here are just pointers to indexes.
                for(IndexArray::iterator it = indexArray.begin(), itEnd = indexArray.end(), itContigEnd; it != itEnd; it = itContigEnd)
                {
                    // Find the first RunInfo that is >= e.
                    while((it != itEnd) && (mBidiRunInfoArray[*it].mnBidiLevel < e))
                        ++it;

                    if(it != itEnd)
                    {
                        itContigEnd = it;

                        // Find the next RunInfo that is < e.
                        while((itContigEnd != itEnd) && (mBidiRunInfoArray[*itContigEnd].mnBidiLevel >= e))
                            ++itContigEnd;

                        eastl::reverse(it, itContigEnd);
                    }
                    else
                        itContigEnd = itEnd;
                }
            }

            // Generate runInfoArray, based on the reordering in indexArray.
            runInfoArray.clear();

            for(eastl_size_t b = 0, bEnd = indexArray.size(); b < bEnd; b++)
            {
                const eastl_size_t index   = indexArray[b];
                const RunInfo&     runInfo = mBidiRunInfoArray[index];

                runInfoArray.push_back(runInfo);
            }
        }
    #else
        runInfoArray = mBidiRunInfoArray;
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// ReverseGlyphs
//
// We want to reorder the clusters within the run so that instead of the run
// being from fRunStartPos to fRunEndPos, it goes from fRunEndPos to fRunStartPos.
// All horizontal glyph position values should simply be mirrored.
// We leave the order of our GlyphLayoutInfo array as it is, we merely adjust the 
// positions of the glyphs. So the result is that mGlyphLayoutInfoArray lists
// glyphs in logical (memory) order, but the glyphs are merely non-contiguously
// drawn on the screen. In some cases, the temporal order of glyph drawing within
// a cluster matters due to the way the font is designed and things look a little
// different when the order is reversed.
//
void Typesetter::ReverseGlyphs(GlyphLayoutInfo* pGLIBegin, GlyphLayoutInfo* pGLIEnd, 
                               float fRunStartPos, float fRunEndPos)
{
    #if EATEXT_BIDI_SUPPORTED
        EA_ASSERT(pGLIBegin < pGLIEnd);

        // fOriginMirror is a point to the right of fRunEndPos that is the same distance
        // to the right of fRunEndPos that the coordinate system origin (i.e. zero) is 
        // to the left of fRunStartPos.
        const float fOriginMirror = (fRunEndPos + fRunStartPos);

        while(pGLIBegin < pGLIEnd)
        {
            const float fPenXNew      = fOriginMirror - pGLIBegin->mfPenX;
            const float fXTranslation = fPenXNew - pGLIBegin->mfPenX - pGLIBegin->mfAdvance;

            pGLIBegin->mfX1      += fXTranslation;
            pGLIBegin->mfX2      += fXTranslation;
            pGLIBegin->mfPenX     = fPenXNew;
            pGLIBegin->mfAdvance  = -pGLIBegin->mfAdvance;

            ++pGLIBegin;
        }
    #else
        (void)pGLIBegin; (void)pGLIEnd; (void)fRunStartPos; (void)fRunEndPos;
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// OrderGlyphs
//
// We implement Unicode Annex #9 (Bidi), section 3.4 L2.
//
void Typesetter::OrderGlyphs()
{
    #if EATEXT_BIDI_SUPPORTED
        // The Unicode standard suggests an multi-pass algorithm in section 3.4 L2  
        // which involves repeated scans over the glyphs which reverse segments of them,
        // including the glyphs within the segments. We do things a little differently.
        // You very much want to read the examples in section L2 to understand what
        // it is that bidi reordering needs to do.

        if(mbNonGeneralScriptPresent) // We can't have RTL text unless we have non-general script, so this is a quick check that for all non-RTL locales will almost always evaluate to false.
        {
            // We dynamically calculate bidi presence. We can alternatively cache this value
            // when we do shaping.
            bool bBidiPresent = false;

            for(eastl_size_t i = 0, iEnd = mBidiRunInfoArray.size(); i < iEnd; ++i) // Usually iEnd is a value <= 3.
            {
                if(mBidiRunInfoArray[i].mnBidiLevel % 2)
                {
                    bBidiPresent = true;
                    break;
                }
            }

            if(bBidiPresent)
            {
                // Steps:
                //    - Re-order the runs. 
                //    - Move the glyphs referred to by the run to visually where the run is.
                //    - Reverse the glyphs in runs with an even (RTL) embedding level.

                RunInfoArray bidiRunInfoArrayNew; 
                ReorderBidiRunInfoArray(bidiRunInfoArrayNew);

                float fRunStartPos = mLineLayout.mGlyphLayoutInfoArray[0].mfPenX;

                for(eastl_size_t i = 0, iEnd = bidiRunInfoArrayNew.size(); i < iEnd; i++)
                {
                    const RunInfo& runInfo = bidiRunInfoArrayNew[i];

                    if(runInfo.mnCharEnd > runInfo.mnCharBegin) // If the run is non-empty...
                    {
                        const eastl_size_t  iGlyphBegin = mLineLayout.GetGlyphIndexFromCharIndex(runInfo.mnCharBegin);
                        GlyphLayoutInfo&    gliBegin    = mLineLayout.mGlyphLayoutInfoArray[iGlyphBegin];
                        const float         fGlyphMin   = (gliBegin.mfAdvance > 0) ? gliBegin.mfPenX : (gliBegin.mfPenX + gliBegin.mfAdvance);
                        const float         fOffset     = fRunStartPos - gliBegin.mfPenX;

                        // What's the best way to get the width of the run? Last glyph advance or next run pen X?
                        // It turns out that they ought to always be equal, since run changes will not be linked
                        // by kerned glyphs. However, 'next run pen x' would be the more correct solution.
                        const eastl_size_t iGlyphEnd  = mLineLayout.GetGlyphIndexFromCharIndex(runInfo.mnCharEnd);
                        GlyphLayoutInfo&   gliLast    = mLineLayout.mGlyphLayoutInfoArray[iGlyphEnd - 1];    // This isn't correct, though it will work most of the time. We need to go back and find the base char of clusters.
                        const float        fGlyphMax  = (gliLast.mfAdvance > 0) ? (gliLast.mfPenX + gliLast.mfAdvance) : gliLast.mfPenX;
                        const float        fRunWidth  = (fGlyphMax - fGlyphMin);

                        // EA_ASSERT(gliLast.mfAdvance != 0.f); // This will fail, and we need to fix the problem. See a couple lines above.

                        mLineLayout.OffsetSegment(fOffset, 0, iGlyphBegin, iGlyphEnd);

                        // Reverse the glyphs in RTL runs.
                        if(runInfo.mnBidiLevel % 2) // If the run is RTL...
                            ReverseGlyphs(&gliBegin, &gliLast + 1, fRunStartPos, fRunStartPos + fRunWidth);

                        fRunStartPos += fRunWidth;
                    }
                }
            }
        }
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// LayoutLine
//
index_t Typesetter::LayoutLine(const Char* pText, index_t nTextSize, float x, float y, const TextStyle* pTextStyle)
{
    const float kRightBoundary = 100000.f;

    Reset();
    SetLayoutSpace(kRightBoundary, x, y);
    SetDefaultTextStyle(pTextStyle);
    mLayoutSettings.mTextStyleDefault.mHAlignment = kHALeft; // By definition, this function lays out left-aligned text.
    AddTextRun(pText, nTextSize);
    FinalizeLine();

    // Possibly offset the line, based on the resulting font/line metrics.
    // To do: If the Font for the line is fixed, then we'll know the line
    // metrics before doing the layout above and can apply the adjustment
    // before layout instead of afterwards.
    float xOffset = 0.f;

    //if(mLayoutSettings.mbCellTopPosition) // If the user wants their input y position to refer to the top of the text box instead of the baseline...
    //{
    //    //bool bFitPixelGrid = true; // To do: Make this pixel fitting optional. Should it be an argument or a LayoutSettings member?
    //    //if(bFitPixelGrid) 
    //    //   mLineLayout.mLineMetrics.mfBaseline = (float)ceilf(mLineLayout.mLineMetrics.mfBaseline);
    //}

    if(mLayoutSettings.mDirection % 1) // If the base paragraph direction is RTL...
        xOffset = mLineLayout.mLineMetrics.mfSpace - kRightBoundary;

    mLineLayout.Offset(xOffset, 0);

    return mLineLayout.mGlyphArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// LayoutParagraph
//
index_t Typesetter::LayoutParagraph(const Char* pText, index_t nTextSize, 
                                    float fRectLeft, float fRectTop, float fRectRight, float fRectBottom, 
                                    const TextStyle* pTextStyle, LineLayout& lineLayout, int componentFlags)
{
    // To do: Provide an option for the user to specify that the layout is done to a pixel grid.
    // To consider: Provide an option that causes all lines to be the same height. 
    // Currently the lines can be variable, as with an HTML display. To do this would require
    // laying out all lines and finding the tallest line and using that height for all lines.

    float       fLineHeightSum        = 0;       // Sum of text line heights.
    bool        bFitPixelGrid         = true;
    const float fRectWidth            = fRectRight - fRectLeft;
    const float fRectHeight           = fRectBottom - fRectTop;
    const float fPenX                 = fRectLeft; // This works OK for RTL text as it will be shifted to the right side of the rect appropriately.
    LineLayout  lineLayoutTemp(lineLayout.mpCoreAllocator);
    LineLayout& lineLayoutWorking     = (&lineLayout == &mLineLayout) ? lineLayoutTemp : lineLayout;
    bool        bCellTopPositionSaved = mLayoutSettings.mbCellTopPosition;

    mLayoutSettings.mbCellTopPosition = true; // For paragraph layouts, we always want to use cell top positioning so our vertical alignment within the bounds works.
    mLineLayout.Clear(); // The Reset call will actually do this same clear, but due to the order of execution, a bogus assertion failure might fire unless we pre-emptively clear the LineLayout here.
    Reset();
    SetLayoutSpace(fRectWidth, fPenX, 0); // Initially set the pen to [fPenX, 0]. We'll adjust the final position of glyphs if needed later.
    SetDefaultTextStyle(pTextStyle);
    AddTextRun(pText, nTextSize);

    // If the user has alignment set to justify, we make sure the paragraph is seen
    // as such by the Typesetter by making sure it has a trailing '\n'. This is 
    // necessary because by convention that last line of a justified paragraph is
    // not justified. But Typesetter can't know what the last line of a paragraph 
    // is without the user's help.
    if((pTextStyle->mHAlignment == kHAJustify) && (nTextSize && (pText[nTextSize - 1] != '\n'))) // Actually, the line could end with kCharPSEP, kCharNEL, or kCharLSEP as well as '\n'.
        AddParagraphBreak();

    LineState lineState = GetLineState();

    while(lineState != Typesetter::kLineStateComplete)
    {
        // To do: Figure out how to decide that this is the last line and implement it.
        //        Strictly speaking, the only way to be 100% sure in the presence of 
        //        variable height lines is to layout the last line twice: once to recognize 
        //        that it really is the last line, and once with ellipsizing enabled.
        // if((pTextStyle->mTextOverflow == kTOEllipsis) && (this is the last line))
        //    EllipsizeLine(true);

        FinalizeLine(); // This will take care of alignment or justification.

        const float fLineHeight = (float)ceilf(mLineLayout.mLineMetrics.mfBaseline - mLineLayout.mLineMetrics.mfDescent);

        if(bFitPixelGrid)
        {
            // We have the case where the baseline and descent values may be fractional even  
            // though the line height (baseline - descent) may be integral.
            mLineLayout.mLineMetrics.mfBaseline = (float)floorf(mLineLayout.mLineMetrics.mfBaseline + 0.5f); // Round it.
            mLineLayout.mLineMetrics.mfDescent  = mLineLayout.mLineMetrics.mfBaseline - fLineHeight;
        }

        // We add the line's height to the sum. At the end we'll possibly offset the 
        // entire layed out paragraph based on this value and the paragraph vertical alignment.
        fLineHeightSum += fLineHeight;
        SetLayoutSpace(fRectWidth, fPenX, fLineHeightSum * mLayoutSettings.mfYScale); // Move the pen Y position vertically by the height of the line.

        lineLayoutWorking.Append(mLineLayout, componentFlags);
        lineLayoutWorking.mLineCount++;
        NextLine(false);
        lineState = GetLineState();
    }

    // At this point we know the line count and the sum height of all lines.
    // Also, all lines are layed out relative to an initial y position of 0.
    // So we want to revise the glyph positions to account for final line height
    // and alignment values.

    float fYOffset = fRectTop; // Move layout position from top of cell to baseline.

    switch(pTextStyle->mVAlignment)
    {
        case kVABottom:
            fYOffset += floorf((fRectHeight - fLineHeightSum) * mLayoutSettings.mfYScale);
            break;

        case kVACenter:
        {
            fYOffset += floorf(((fRectHeight - fLineHeightSum) * 0.5f) * mLayoutSettings.mfYScale);
            break;
        }

        case kVATop:
        case kVAInherit:
        default:
            break;
    }

    lineLayoutWorking.Offset(0, fYOffset);

    if(&lineLayout == &mLineLayout)
    {
        mLineLayout.Clear();
        lineLayoutTemp.Append(mLineLayout);
    }

    mLayoutSettings.mbCellTopPosition = bCellTopPositionSaved; // Restore the saved value.

    return lineLayout.mGlyphArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetParagraphSize
//
EA::Text::Point Typesetter::GetParagraphSize(const Char* pText, index_t nTextSize, 
                                             const TextStyle* pTextStyle, float fLayoutWidth)
{
    Point       result(0, 0);
    LineLayout& layout = GetLineLayout();

    Reset();
    SetLayoutSpace((fLayoutWidth > 0) ? fLayoutWidth : 10000, 0, 0);
    SetDefaultTextStyle(pTextStyle);
    AddTextRun(pText, nTextSize);

    while(GetLineState() != Typesetter::kLineStateComplete)
    {
        FinalizeLine(); // This will take care of alignment or justification.
        result.mY += layout.GetLineHeight();
        result.mX = eastl::max_alt(result.mX, layout.mLineMetrics.mfVisibleSpace);
        NextLine(false);
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////
// Validate
//
// This function is linked away when not used, as would be the case in a release build.
//
bool Typesetter::Validate()
{
    bool bReturnValue = true;

    // Useful during unit testing:
    // EATextTest_CheckMemory_Imp(__FILE__, __LINE__);

    if(!mLineLayout.Validate())
        bReturnValue = false;

    if(mLineState == kLineStateComplete)
    {
        // Verify that if we have any glyphs, then mLineCount is non-zero.
        EA_ASSERT(mLineLayout.mGlyphArray.empty() || (mLineLayout.mLineCount != 0));
        if(!mLineLayout.mGlyphArray.empty() && (mLineLayout.mLineCount == 0))
            bReturnValue = false;
    }

    // The schedule end value should be equal to the schedule size.
    EA_ASSERT(mScheduleEnd.mnScheduleIndex == (uint32_t)mSchedule.size());
    if(mScheduleEnd.mnScheduleIndex != (uint32_t)mSchedule.size())
        bReturnValue = false;

    // The schedule end is not used, so its subindex is not used and should always be zero.
    EA_ASSERT(mScheduleEnd.mnCharOffset == 0);
    if(mScheduleEnd.mnCharOffset != 0)
        bReturnValue = false;

    // To do: Verify that the following have sane values relative to each other:
    //    Schedule         mSchedule;
    //    ScheduleLocation mLineBegin;
    //    ScheduleLocation mLineEnd;
    //    ScheduleLocation mAnalysisEnd;
    //    ScheduleLocation mScheduleEnd;

    return bReturnValue;
}



} // namespace Text

} // namespace EA


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

