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
// EATextLayout.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// *** This file deprecated and no longer supported. ***
//
// It is completely superceded by EATextTypesetter.
///////////////////////////////////////////////////////////////////////////////

#include <EAText/EATextLayout.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextBreak.h>
#include <EAText/EATextUnicode.h>
#include EA_ASSERT_HEADER
#include <math.h>


#if EATEXT_LAYOUT_ENABLED


#ifdef _MSC_VER // Enable highest warning for this file alone under MSVC.
    #pragma warning(push, 4)
    #pragma warning(disable: 4100) // unreferenced formal parameter. To do: Remove this asap.
    #pragma warning(disable: 4127) // conditional expression is constant.
#endif


namespace EA
{

namespace Text
{


/// LayoutScratchpad
///
/// This is a temporary scratchpad for use by layout functions or the user.
//
/// To do: Allow for dynamic resizing of the contents, at least for development.
///
/// To consider: Allow for per-thread scratchpads. This would obviate the need
/// for mutex locking.
///
/// To do: Allow the user to pass in their own scratchpad to the Layout functions.
///
struct LayoutScratchpad
{
    static const uint32_t kBufferSize = 1024;

    uint32_t          mGlyphIndexArray[kBufferSize];
    uint32_t          mTextIndexArray[kBufferSize];
    GlyphAttributes   mGlyphAttributesArray[kBufferSize];
    GlyphDisplayEntry mGlyphDisplayEntryArray[kBufferSize];

    LayoutScratchpad() { }

protected:
    LayoutScratchpad(const LayoutScratchpad&);
    LayoutScratchpad& operator=(const LayoutScratchpad&);
};

LayoutScratchpad gLayoutScratchpad;




///////////////////////////////////////////////////////////////////////////////
// (misc)
///////////////////////////////////////////////////////////////////////////////



/// GetBaselineAndDescent
///
/// Calculates the max baseline and descent values for an array of glyphs which
/// possible use differening fonts, each of which would likely have differening
/// font metrics.
///
/// To consider: Expose this function for external use.
///
void GetBaselineAndDescent(GlyphAttributes* pGA, uint32_t nGASize, LayoutMetrics* pLayoutMetrics)
{
    Font*       pFontCurrent = NULL;
    FontMetrics fontMetrics;

    pLayoutMetrics->mfBaseline = 0;
    pLayoutMetrics->mfDescent  = 0;

    for(const GlyphAttributes* const pGAEnd = pGA + nGASize; pGA < pGAEnd; ++pGA)
    {
        if(!pFontCurrent || (pFontCurrent != pGA->mpFont)) // If we need to re-evaluate the FontMetrics.
        {
            pFontCurrent = pGA->mpFont;

            if(pFontCurrent)
                pFontCurrent->GetFontMetrics(fontMetrics);
            else
                memset(&fontMetrics, 0, sizeof(FontMetrics));
        }

        if(pLayoutMetrics->mfBaseline < fontMetrics.mfBaseline)
           pLayoutMetrics->mfBaseline = fontMetrics.mfBaseline;

        if(pLayoutMetrics->mfDescent > fontMetrics.mfDescent)
           pLayoutMetrics->mfDescent = fontMetrics.mfDescent;
    }
}



///////////////////////////////////////////////////////////////////////////////
// Layout
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Layout
//
Layout::Layout()
  : mLayoutSettings()
{
    // Empty
}


///////////////////////////////////////////////////////////////////////////////
// ~Layout
//
Layout::~Layout()
{
    // Empty
}


///////////////////////////////////////////////////////////////////////////////
// SetLayoutSettings
//
void Layout::SetLayoutSettings(const LayoutSettings* pLayoutSettings)
{
    if(pLayoutSettings)
        mLayoutSettings = *pLayoutSettings;
    else
        mLayoutSettings = LayoutSettings(); // Construct a default layout settings and assign it to mLayoutSettings.

    // To do: Set mLayoutSettings.mScript if mScript < kScriptBegin
    // To do: Set mLayoutSettings.mDirection if mDirection != LTR or RTL.
}


///////////////////////////////////////////////////////////////////////////////
// SetLayoutFonts
//
void Layout::SetLayoutFonts(const TextStyle* pTextStyle, LayoutSettings* pLayoutSettings, FontServer* pFontServer)
{
    if(!pLayoutSettings)
        pLayoutSettings = &mLayoutSettings;

    if(!pTextStyle)
        pTextStyle = &pLayoutSettings->mTextStyleDefault;

    if(!pFontServer)
    {
        EA_ASSERT(GetFontServer(false)); // Assert that there is a font server.
        pFontServer = GetFontServer(true); // Get the font server, creating it if necessary.
    }

    // Free existing fonts.
    pLayoutSettings->mFontSelection.clear();
    pFontServer->GetFont(pTextStyle, pLayoutSettings->mFontSelection, kScriptLatin, true);
}


///////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
uint32_t Layout::LayoutSimple(const Char* pText, uint32_t nTextSize, float x, float y, Font* pFont, 
                            GlyphDisplayEntry* pGlyphDisplayEntryArray)
{
    if(!pFont && !mLayoutSettings.mFontSelection.empty())
        pFont = *mLayoutSettings.mFontSelection.begin();

    EA_ASSERT(pFont);

    if(mLayoutSettings.mbCellTopPosition)
    {
        FontMetrics fontMetrics;
        pFont->GetFontMetrics(fontMetrics);
        y += (fontMetrics.mfBaseline * mLayoutSettings.mfYScale);
    }

    GlyphMetrics glyphMetrics;

    pFont->GetGlyphIds(pText, nTextSize, &pGlyphDisplayEntryArray->mGlyphId, true, sizeof(GlyphDisplayEntry));

    for(const Char* const pTextEnd = pText + nTextSize; pText < pTextEnd; pText++, pGlyphDisplayEntryArray++, x += glyphMetrics.mfHAdvanceX)
    {
        pGlyphDisplayEntryArray->mpFont = pFont;

        pFont->GetGlyphMetrics(pGlyphDisplayEntryArray->mGlyphId, glyphMetrics);

        pGlyphDisplayEntryArray->mfPenX    = x;
        pGlyphDisplayEntryArray->mfPenY    = y;
        pGlyphDisplayEntryArray->mfX1      = x +  glyphMetrics.mfHBearingX;
        pGlyphDisplayEntryArray->mfY1      = y - (glyphMetrics.mfHBearingY * mLayoutSettings.mfYScale);
        pGlyphDisplayEntryArray->mfX2      = pGlyphDisplayEntryArray->mfX1 + glyphMetrics.mfSizeX;
        pGlyphDisplayEntryArray->mfY2      = pGlyphDisplayEntryArray->mfY1 + (glyphMetrics.mfSizeY * mLayoutSettings.mfYScale);
        pGlyphDisplayEntryArray->mfAdvance = glyphMetrics.mfHAdvanceX;
        memset(&pGlyphDisplayEntryArray->mGLA, 0, sizeof(pGlyphDisplayEntryArray->mGLA)); // This is just a couple bytes.
    }

    return nTextSize;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// LayoutSimple
//
// This is the same as the other LayoutSimple, except that the input is 8 bit text.
//
uint32_t Layout::LayoutSimple(const char* pText, uint32_t nTextSize, float x, float y, Font* pFont, 
                            GlyphDisplayEntry* pGlyphDisplayEntryArray)
{
    if(!pFont && !mLayoutSettings.mFontSelection.empty())
        pFont = *mLayoutSettings.mFontSelection.begin();

    EA_ASSERT(pFont);

    if(mLayoutSettings.mbCellTopPosition)
    {
        FontMetrics fontMetrics;
        pFont->GetFontMetrics(fontMetrics);
        y += (fontMetrics.mfBaseline * mLayoutSettings.mfYScale);
    }

    GlyphMetrics glyphMetrics;

    for(const char* const pTextEnd = pText + nTextSize; pText < pTextEnd; pText++, pGlyphDisplayEntryArray++, x += glyphMetrics.mfHAdvanceX)
    {
        const Char c = (uint8_t)*pText;
        pFont->GetGlyphIds(&c, 1, &pGlyphDisplayEntryArray->mGlyphId, true);

        pGlyphDisplayEntryArray->mpFont = pFont;

        pFont->GetGlyphMetrics(pGlyphDisplayEntryArray->mGlyphId, glyphMetrics);

        pGlyphDisplayEntryArray->mfPenX    = x;
        pGlyphDisplayEntryArray->mfPenY    = y;
        pGlyphDisplayEntryArray->mfX1      = x +  glyphMetrics.mfHBearingX;
        pGlyphDisplayEntryArray->mfY1      = y - (glyphMetrics.mfHBearingY * mLayoutSettings.mfYScale);
        pGlyphDisplayEntryArray->mfX2      = pGlyphDisplayEntryArray->mfX1 + glyphMetrics.mfSizeX;
        pGlyphDisplayEntryArray->mfY2      = pGlyphDisplayEntryArray->mfY1 + (glyphMetrics.mfSizeY * mLayoutSettings.mfYScale);
        pGlyphDisplayEntryArray->mfAdvance = glyphMetrics.mfHAdvanceX;
        memset(&pGlyphDisplayEntryArray->mGLA, 0, sizeof(pGlyphDisplayEntryArray->mGLA)); // This is just a couple bytes.
    }

    return nTextSize;
}



///////////////////////////////////////////////////////////////////////////////
// GetLayoutLineAnalysisArray
//
AnalysisArray& GetLayoutLineAnalysisArray()
{
    static AnalysisArray sLayoutTextLineAnalysysArray(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "Layout"))); // this is protected by the mutex inside

    return sLayoutTextLineAnalysysArray;
}


///////////////////////////////////////////////////////////////////////////////
// LayoutTextLine
//
uint32_t Layout::LayoutTextLine(const Char* pText, uint32_t nTextSize, 
                                float x, float y, 
                                const TextStyle* pTextStyle,
                                GlyphDisplayEntry* pGlyphDisplayEntryArray, uint32_t nGlyphDisplayEntryArrayCapacity,
                                LayoutMetrics* pLayoutMetrics)
{
    uint32_t        nGlyphCountTotal = 0;
    LayoutMetrics   layoutMetrics;

    if(!pTextStyle)
        pTextStyle = &mLayoutSettings.mTextStyleDefault;

    if(nTextSize)
    {
        // EA_ASSERT(mLayoutSettings.mFontSelection.mpFontArray[0]); Possibly enable this assert.
        if(mLayoutSettings.mFontSelection.empty())
            SetLayoutFonts(pTextStyle);

        EA_ASSERT_MESSAGE(!mLayoutSettings.mFontSelection.empty(), "Layout::LayoutTextLine: No fonts to work with.");

        const TextRun textRun(pText, (uint32_t)nTextSize);
        AnalysisArray& analysisArray = GetLayoutLineAnalysisArray();
        analysisArray.clear();

        // We generally must analyze the entire paragraph, as reordering could move glyphs positions around.
        AnalyzeText(&textRun, 1, analysisArray, NULL);
        EA_ASSERT(!analysisArray.empty());

        if(nGlyphDisplayEntryArrayCapacity > LayoutScratchpad::kBufferSize)  // We are using own own buffer for shaping, at it has a max size.
            nGlyphDisplayEntryArrayCapacity = LayoutScratchpad::kBufferSize; // So even if the user passes in more, we are stuck with this max size.

        // Shape all glyphs on the line.
        const uint32_t nGlyphCount = ShapeGlyphs(&textRun, 1, gLayoutScratchpad.mGlyphIndexArray, &analysisArray.front(), (uint32_t)analysisArray.size(),
                                               gLayoutScratchpad.mGlyphAttributesArray, nGlyphDisplayEntryArrayCapacity, NULL);

        GetBaselineAndDescent(gLayoutScratchpad.mGlyphAttributesArray, eastl::min_alt(nGlyphCount, nGlyphDisplayEntryArrayCapacity), &layoutMetrics);

        if(nGlyphCount <= nGlyphDisplayEntryArrayCapacity)
        {
            if(mLayoutSettings.mbCellTopPosition)
                y += (layoutMetrics.mfBaseline * mLayoutSettings.mfYScale);

            // If there are more than one analysis, do ordering. Actually, ordering is needed only if there are differing directions.
            if(analysisArray.size() > 1)
                OrderText(&analysisArray.front(), (uint32_t)analysisArray.size(), &analysisArray.front());

            // Create a high level display list.
            nGlyphCountTotal = PlaceGlyphs(&analysisArray.front(), (uint32_t)analysisArray.size(), gLayoutScratchpad.mGlyphIndexArray,
                                           gLayoutScratchpad.mGlyphAttributesArray, LayoutScratchpad::kBufferSize, x, y, pGlyphDisplayEntryArray);
        }
    }
    else
    {
        layoutMetrics.mfBaseline = 1; // We use a small number instead of zero in order to help prevent divide by zero errors a user might forget to deal with.
        layoutMetrics.mfDescent  = 0;
    }

    if(pLayoutMetrics)
    {
        pLayoutMetrics->mfBaseline  = layoutMetrics.mfBaseline;
        pLayoutMetrics->mfDescent   = layoutMetrics.mfDescent;
        pLayoutMetrics->mnSize      = nTextSize;
        pLayoutMetrics->mnLineCount = 1;

        // To do: This supports horizontal text only. Fix this to deal with vertical as well.
        if(nGlyphCountTotal)
            pLayoutMetrics->mfSpace = ((pGlyphDisplayEntryArray[nGlyphCountTotal - 1].mfPenX + gLayoutScratchpad.mGlyphAttributesArray[nGlyphCountTotal - 1].mfAdvance)) - x;
        else
            pLayoutMetrics->mfSpace = 0;

        // We make an assumption that the glyph generation above is consistent with our space identification here.
        uint32_t nVisibleSize = nTextSize;

        while(nVisibleSize > 0)
        {
            if(!IsSpace(pText[nVisibleSize - 1], kSTAll, false))
                break;
            --nVisibleSize;
        }

        pLayoutMetrics->mnVisibleSize  = nVisibleSize;
        pLayoutMetrics->mfVisibleSpace = 0;

        // What we do here is convert the text index to a glyph index, and then get the width up to the point of that glyph.
        const uint32_t nGlyphCountVisible = nVisibleSize ? gLayoutScratchpad.mGlyphIndexArray[nVisibleSize - 1] + 1 : 0;

        if(nGlyphCountVisible > 0)
        {
            #ifdef EATEXT_VERTICAL_ENABLED
                if(pTextStyle->mOrientation == kOrientationHorizontal)
                    pLayoutMetrics->mfVisibleSpace = ((pGlyphDisplayEntryArray[nGlyphCountVisible - 1].mfPenX + gLayoutScratchpad.mGlyphAttributesArray[nGlyphCountVisible - 1].mfAdvance)) - x;
                else
                    pLayoutMetrics->mfVisibleSpace = ((pGlyphDisplayEntryArray[nGlyphCountVisible - 1].mfPenY + gLayoutScratchpad.mGlyphAttributesArray[nGlyphCountVisible - 1].mfAdvance)) - y;
            #else
                pLayoutMetrics->mfVisibleSpace = ((pGlyphDisplayEntryArray[nGlyphCountVisible - 1].mfPenX + gLayoutScratchpad.mGlyphAttributesArray[nGlyphCountVisible - 1].mfAdvance)) - x;
            #endif
        }
    }

    return nGlyphCountTotal;
}



///////////////////////////////////////////////////////////////////////////////
// GetParagraphAnalysisArray
//
AnalysisArray& GetParagraphAnalysisArray()
{
    static AnalysisArray sLayoutTextParagraphAnalysisArray(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "Layout")));  // this is protected by the mutex inside

    return sLayoutTextParagraphAnalysisArray;
}


///////////////////////////////////////////////////////////////////////////////
// LayoutTextParagraph
//
uint32_t Layout::LayoutTextParagraph(const Char* pText, uint32_t nTextSize, 
                                   float fRectLeft, float fRectTop, float fRectRight, float fRectBottom, 
                                   const TextStyle* pTextStyle,
                                   GlyphDisplayEntry* pGlyphDisplayEntryArray, uint32_t nGlyphDisplayEntryArrayCapacity,
                                   LayoutMetrics* pLayoutMetrics)
{
    uint32_t        nGlyphCountTotal = 0;
    LayoutMetrics   layoutMetrics;
    uint32_t        nLineCount;

    if(!pTextStyle)
        pTextStyle = &mLayoutSettings.mTextStyleDefault;

    nLineCount = 0;
    layoutMetrics.mfBaseline = 1; // We use a small number instead of zero in order to help prevent divide by zero errors a user might forget to deal with.
    layoutMetrics.mfDescent  = 0;

    if(nTextSize)
    {
        // EA_ASSERT(mLayoutSettings.mFontSelection.mpFontArray[0]); Possibly enable this assert.
        if(mLayoutSettings.mFontSelection.empty())
            SetLayoutFonts(pTextStyle);

        EA_ASSERT_MESSAGE(!mLayoutSettings.mFontSelection.empty(), "Layout::LayoutTextParagraph: No fonts to work with.");

        const TextRun textRun(pText, (uint32_t)nTextSize);
        AnalysisArray& analysisArray = GetParagraphAnalysisArray();
        analysisArray.clear();

        // We generally must analyze the entire paragraph, as reordering could move glyphs positions around.
        AnalyzeText(&textRun, 1, analysisArray, NULL);
        EA_ASSERT(!analysisArray.empty());

        if(nGlyphDisplayEntryArrayCapacity > LayoutScratchpad::kBufferSize)  // We are using own own buffer for shaping, at it has a max size.
            nGlyphDisplayEntryArrayCapacity = LayoutScratchpad::kBufferSize; // So even if the user passes in more, we are stuck with this max size.

        // We shape all glyphs in the paragraph, even if they don't fit within the paragraph rectangle.
        // It may be feasible to do some guessing about what text will fit within the area and only 
        // shape those glyphs. On the other hand, most of the time the entire paragraph fits.
        uint32_t nGlyphCount = ShapeGlyphs(&textRun, 1, gLayoutScratchpad.mGlyphIndexArray, &analysisArray.front(), (uint32_t)analysisArray.size(),
                                            gLayoutScratchpad.mGlyphAttributesArray, nGlyphDisplayEntryArrayCapacity, gLayoutScratchpad.mTextIndexArray);

        if(nGlyphCount <= nGlyphDisplayEntryArrayCapacity)
        {
            // Note that we calculate the baseline and descent here for the entire paragraph instead of 
            // on a line by line basis. This results in all paragraph lines being the same distance apart.
            // For some uses this is undesirable. For the time being the only resolution is to write your
            // own layout. An HTML display would want per-line metrics, but an HTML display would always
            // write its own layout.
            GetBaselineAndDescent(gLayoutScratchpad.mGlyphAttributesArray, nGlyphCount, &layoutMetrics); // To do: Have ShapeGlyphs tell us where the last different font is, so GetBaselineAndDescent can be fast.
            const float fLineHeight    = (layoutMetrics.mfBaseline - layoutMetrics.mfDescent); // This is always going to be a positive value.
            const float fLineIncrement = fLineHeight * mLayoutSettings.mfYScale; // Could be a positive or negative value.

            // Calculate all line breaks. This will tell us how many lines there are so we can know 
            // how to position the paragraph vertically. 
            uint32_t pTextLineEndArray[256];
            float  pTextLineSpaceArray[256];

            nLineCount = LineBreakText(&textRun, 1, 0, nTextSize, gLayoutScratchpad.mGlyphIndexArray, &analysisArray, gLayoutScratchpad.mGlyphAttributesArray, gLayoutScratchpad.mTextIndexArray,
                                        fRectRight - fRectLeft, pTextLineEndArray, 256, pTextLineSpaceArray);

            // Calculate the paragraph vertical position.
            float yBegin;

            switch(pTextStyle->mVAlignment)
            {
                case kVABottom:
                    yBegin = fRectBottom - ((nLineCount * fLineHeight) * mLayoutSettings.mfYScale);
                    break;
                case kVACenter:{ // mLayoutSettings.mfYScale
                    const float fRectangleHeight = fRectBottom - fRectTop;
                    const float fParagraphHeight = (int)nLineCount * fLineHeight;
                    yBegin = fRectTop + (((fRectangleHeight - fParagraphHeight) * 0.5f) * mLayoutSettings.mfYScale);
                    break;
                }
                case kVATop:
                case kVAInherit:
                default:
                    yBegin = fRectTop;
                    break;
            }

            yBegin += (layoutMetrics.mfBaseline * mLayoutSettings.mfYScale); // Move layout position from top of cell to baseline.

            // Place the glyphs for each line.
            uint32_t nAnalysisBegin = 0;
            uint32_t nAnalysisEnd = 0;
            uint32_t nAnalysisSize = (uint32_t)analysisArray.size();
            float    y = yBegin;
            float    x;

            for(uint32_t i = 0; i < nLineCount; i++)
            {
                // Find the span of analyses that are within this line. The analyses should fit perfectly.
                while((nAnalysisEnd < nAnalysisSize) && ((analysisArray[nAnalysisEnd].mnPosition + analysisArray[nAnalysisEnd].mnLength) <= pTextLineEndArray[i]))
                    nAnalysisEnd++;

                // If there is more than one analysis, do ordering. Actually, ordering is needed only if there are differing directions.
                if((nAnalysisEnd - nAnalysisBegin) > 1)
                    OrderText(&analysisArray[nAnalysisBegin], nAnalysisEnd - nAnalysisBegin, &analysisArray[nAnalysisBegin]);

                // Create a high level display list.
                switch(pTextStyle->mHAlignment){
                    case kHARight:
                        // Note: we don't currently deal with whitespace at the end of the line. It would be proper to do so.
                        // We can solve this by finding trailing whitespace here or we can solve it by adding an extra parameter
                        // to the LineBreakText function which returns visible line space as well as line space.
                        x = fRectRight - pTextLineSpaceArray[i];
                        break;
                    case kHACenter:
                        x = fRectLeft + (((fRectRight - fRectLeft) - pTextLineSpaceArray[i]) * 0.5f);
                        break;
                    case kHAJustify:
                    case kHALeft:
                    case kHAInherit:
                    default:
                        x = fRectLeft;
                        break;
                }

                if(pTextStyle->mHAlignment == kHAJustify) // If justifying text...
                {
                    const uint32_t nTextIndexBegin = analysisArray[nAnalysisBegin].mnPosition;
                    const uint32_t nTextIndexEnd   = analysisArray[nAnalysisEnd - 1].mnPosition + analysisArray[nAnalysisEnd - 1].mnLength;

                    JustifyGlyphs(&textRun, 1, nTextIndexBegin, nTextIndexEnd - nTextIndexBegin, gLayoutScratchpad.mGlyphIndexArray,
                                   gLayoutScratchpad.mGlyphAttributesArray, nGlyphDisplayEntryArrayCapacity, 
                                   gLayoutScratchpad.mTextIndexArray, ((fRectRight - fRectLeft) - pTextLineSpaceArray[i]), false);
                }

                nGlyphCount = PlaceGlyphs(&analysisArray[nAnalysisBegin], nAnalysisEnd - nAnalysisBegin, gLayoutScratchpad.mGlyphIndexArray,
                                          gLayoutScratchpad.mGlyphAttributesArray, nGlyphDisplayEntryArrayCapacity, 
                                          x, y, pGlyphDisplayEntryArray + nGlyphCountTotal);

                // Reset data for the next line.
                nGlyphCountTotal += nGlyphCount;
                nAnalysisBegin    = nAnalysisEnd;
                y                += fLineIncrement;
            }
        }
    }

    if(pLayoutMetrics)
    {
        pLayoutMetrics->mnSize         = nTextSize; // To do: Fix this, as using nTextSize here is incorrect when space ran out.
        pLayoutMetrics->mfSpace        = (int)nLineCount * (layoutMetrics.mfBaseline - layoutMetrics.mfDescent);
        pLayoutMetrics->mnVisibleSize  = nTextSize; // To do: Fix this, as it is not taking into account trailing space. For paragraph layout, you usually shouldn't care about this parameter.
        pLayoutMetrics->mfVisibleSpace = pLayoutMetrics->mfSpace;
        pLayoutMetrics->mfBaseline     = layoutMetrics.mfBaseline;
        pLayoutMetrics->mfDescent      = layoutMetrics.mfDescent;
        pLayoutMetrics->mnLineCount    = nLineCount;
    }

    return nGlyphCountTotal;
}



///////////////////////////////////////////////////////////////////////////////
// GetTextLayoutMetrics
//
float Layout::GetTextLayoutMetrics(const Char* pText, uint32_t nTextSize, 
                                    const TextStyle* pTextStyle, 
                                    LayoutMetrics* pLayoutMetrics)
{
    LayoutTextLine(pText, nTextSize, 0, 0, pTextStyle, gLayoutScratchpad.mGlyphDisplayEntryArray, LayoutScratchpad::kBufferSize, pLayoutMetrics);

    return pLayoutMetrics->mfSpace;
}


///////////////////////////////////////////////////////////////////////////////
// GetNominalBaseline
//
float Layout::GetNominalBaseline(const TextStyle* pTextStyle) const
{
    float                 fResult = 0;
    LayoutSettings        layoutSettings;
    const LayoutSettings* pSettings;

    if(pTextStyle)
    {
        pSettings = &layoutSettings;

        // Since we're passing in the layout settings to use, we're not really
        // modifying any instance vars.
        const_cast<Layout*>(this)->SetLayoutFonts(pTextStyle, &layoutSettings);
    }
    else
        pSettings = &mLayoutSettings;

    for(FontSelection::iterator it = pSettings->mFontSelection.begin(), itEnd = pSettings->mFontSelection.end(); it != itEnd; ++it)
    {
        Font* const pFont = *it;

        if(pFont)
        {
            FontMetrics fontMetrics;

            pFont->GetFontMetrics(fontMetrics);
            fResult = eastl::max_alt(fResult, fontMetrics.mfBaseline);
        }
    }

    return fResult;
}


///////////////////////////////////////////////////////////////////////////////
// GetNominalDescent
//
float Layout::GetNominalDescent(const TextStyle* pTextStyle) const
{
    float                 fResult = 0;
    LayoutSettings        layoutSettings;
    const LayoutSettings* pSettings;

    if(pTextStyle)
    {
        pSettings = &layoutSettings;

        // Since we're passing in the layout settings to use, we're not really
        // modifying any instance vars.
        const_cast<Layout*>(this)->SetLayoutFonts(pTextStyle, &layoutSettings);
    }
    else
        pSettings = &mLayoutSettings;

    for(FontSelection::iterator it = pSettings->mFontSelection.begin(), itEnd = pSettings->mFontSelection.end(); it != itEnd; ++it)
    {
        Font* const pFont = *it;

        if(pFont)
        {
            FontMetrics fontMetrics;
            pFont->GetFontMetrics(fontMetrics);
            fResult = eastl::min_alt(fResult, static_cast<float>(floor(fontMetrics.mfDescent)));
        }
    }

    return fResult;
}



///////////////////////////////////////////////////////////////////////////////
// GetLineBreakAnalysysArray
//
AnalysisArray& GetLineBreakAnalysysArray()
{
    static AnalysisArray sCalculateLineBreakAnalysysArray(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "Layout"))); // this is protected by the mutex inside

    return sCalculateLineBreakAnalysysArray;
}


///////////////////////////////////////////////////////////////////////////////
// CalculateLineBreak
//
uint32_t Layout::CalculateLineBreak(const Char* pText, uint32_t nTextSize, float fSpace, 
                                  const TextStyle* pTextStyle, 
                                  LayoutMetrics* pLayoutMetrics)
{
    TextRun         textRun(pText, (uint32_t)nTextSize);
    AnalysisArray&  analysisArray = GetLineBreakAnalysysArray();
    analysisArray.clear();
    uint32_t        nLineSize;
    float           fLineSpace;

    // EA_ASSERT(!mLayoutSettings.mFontSelection.empty()); Possibly enable this assert.
    if(mLayoutSettings.mFontSelection.empty())
        SetLayoutFonts(pTextStyle);

    EA_ASSERT_MESSAGE(!mLayoutSettings.mFontSelection.empty(), "Layout::CalculateLineBreak: No fonts to work with.");

    const uint32_t nAnalysisCount =    AnalyzeText(&textRun, 1, analysisArray, NULL);
  /*const uint32_t nGlyphCount    = */ ShapeGlyphs(&textRun, 1, gLayoutScratchpad.mGlyphIndexArray, &analysisArray.front(), nAnalysisCount, gLayoutScratchpad.mGlyphAttributesArray, LayoutScratchpad::kBufferSize, gLayoutScratchpad.mTextIndexArray);
  /*const uint32_t nLineCount     = */ LineBreakText(&textRun, 1, 0, nTextSize, gLayoutScratchpad.mGlyphIndexArray, &analysisArray, gLayoutScratchpad.mGlyphAttributesArray, gLayoutScratchpad.mTextIndexArray, fSpace, &nLineSize, 1, &fLineSpace);

    if(pLayoutMetrics)
    {
        pLayoutMetrics->mnSize      = nLineSize;
        pLayoutMetrics->mfSpace     = fLineSpace;
        pLayoutMetrics->mnLineCount = 1;

        // We make an assumption that the glyph generation above is consistent with our space identification here.
        // We want to convert a glyph count to a text count.
        uint32_t nVisibleSize = nLineSize;

        // Walk from the end of the text towards the beginning, looking for the first non-space char. 
        // Most of the time, this loop runs only one or two times.
        while(nVisibleSize > 0)
        {
            if(!IsSpace(pText[nVisibleSize - 1], kSTAll, false))
                break;
            --nVisibleSize;
        }

        pLayoutMetrics->mnVisibleSize  = nVisibleSize;
        pLayoutMetrics->mfVisibleSpace = fLineSpace;

        if(nVisibleSize)
        {
            const uint32_t nVisibleGlyphCount = gLayoutScratchpad.mGlyphIndexArray[nVisibleSize - 1] + 1;
            const uint32_t nLineGlyphCount    = gLayoutScratchpad.mGlyphIndexArray[nLineSize    - 1] + 1;

            for(const GlyphAttributes *pGlyphAttributes    = gLayoutScratchpad.mGlyphAttributesArray + nVisibleGlyphCount, 
                                      *pGlyphAttributesEnd = gLayoutScratchpad.mGlyphAttributesArray + nLineGlyphCount; 
                pGlyphAttributes < pGlyphAttributesEnd; ++pGlyphAttributes)
            {
                pLayoutMetrics->mfVisibleSpace -= pGlyphAttributes->mfAdvance;
            }
        }
    }

    return nLineSize;
}



///////////////////////////////////////////////////////////////////////////////
// GetParagraphBoundary
//
uint32_t Layout::GetParagraphBoundary(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                                    uint32_t* pParagraphDelimiterIndex, uint32_t* pParagraphEndIndex)
{
    TextRunIterator it(pTextRunArray, (uint32_t)nTextRunArraySize);
    uint32_t          d(0), e(0);

    if(!it.Empty())
    {
        do{
            ++e;
            const Char c = *it;         // We look for any of the following sequences:
                                        //    \r
            if(c == '\r')               //    \r\n
            {                           //    \n
                ++it;                   //    \x2029 (Unicode paragraph separator)
                if(*it == '\n')
                    ++e;
                break;
            }
            else if(c == '\n' || c == 0x2029)
                break;

            ++d;
            ++it;
        } while(*it != kCharInvalid);
    }

    if(pParagraphDelimiterIndex)
        *pParagraphDelimiterIndex = d;

    if(pParagraphEndIndex)
        *pParagraphEndIndex = e;

    return e;
}



///////////////////////////////////////////////////////////////////////////////
// GetTextProperties
//
void Layout::GetTextProperties(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, TextProperties& textProperties)
{
    memset(&textProperties, 0, sizeof(textProperties)); // An optimizing compiler with intrinsics enabled will optimize this.

    TextRunIterator  it(pTextRunArray, (uint32_t)nTextRunArraySize);
    ScriptProperties sp;

    if(!it.Empty())
    {
        do{
            // This is not an ideal mechanism. To properly implement this function, we need 
            // to look at individual character properties and not the properties of the scripts
            // each character belongs to. The code below is thus merely guessing what the 
            // properties are, though the guesses aren't likely to be terrible. 
            // To do: Improve this function when our EATextUnicode functionality is capable.
            const Char   c = *it;
            const Script s = GetScriptFromChar(c);

            GetScriptProperties(s, &sp, true);

            if(sp.mbNumericChars)
                textProperties.mbNumericChars = true;
            if(sp.mbControlChars)
                textProperties.mbControlChars = true;
            if(sp.mbAmbiguousChars)
                textProperties.mbAmbiguousChars = true;
            if(sp.mbPrivateChars)
                textProperties.mbPrivateChars = true;
            if(sp.mbComplex)
                textProperties.mbComplex = true;
            if(sp.mbRTL)
                textProperties.mbRTLChars = true;
            else
            {
                //textProperties.mbNeutralChars = true;
                textProperties.mbLTRChars = true;
            }

            if(sp.mbComplex)
                textProperties.mbClusters = true;

            ++it;
        } while(*it != kCharInvalid);
    }
}



///////////////////////////////////////////////////////////////////////////////
// AnalyzeText
//
uint32_t Layout::AnalyzeText(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                            AnalysisArray& analysisArray, const TextState* /*pTextState*/)
{
    // For the time being, we support only Latin script and assume all text is Latin or equivalent.
    // Once we get up and running we will fill out this function to be proper.
    // To do: Provide support for a config option called EATEXT_LATIN_ONLY or similar which
    //        always does simple analysis.
    analysisArray.resize(analysisArray.size() + 1);
    Analysis& analysis = analysisArray.back();

    for(const TextRun *pTextRunCurrent = pTextRunArray, *pTextRunEnd = pTextRunArray + nTextRunArraySize; pTextRunCurrent < pTextRunEnd; ++pTextRunCurrent)
        analysis.mnLength += pTextRunArray->mnTextSize;

    analysis.mnPosition = 0;
    analysis.mTextState.mnLevel = 0;
    analysis.mTextState.mbForcedDirection = 0;
    analysis.mTextState.mbSwapInhibited = 0;
    analysis.mTextState.mbDisableNationalDigits = 0;
    analysis.mScript = kScriptLatin;
    analysis.mnParity = 0;
    analysis.mnParentParity = 0;
    analysis.mbJoinBefore = 0;
    analysis.mbJoinAfter = 0;

    return 1;
}



///////////////////////////////////////////////////////////////////////////////
// ShapeGlyphs
//
uint32_t Layout::ShapeGlyphs(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t* pGlyphIndexArray,
                            const Analysis* /*pAnalysisArray*/, uint32_t /*nAnalysisArraySize*/, 
                            GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphDisplayEntryArrayCapacity, 
                            uint32_t* pTextIndexArray)
{
    // There always must be at least one because we need space at the end of even an empty layout for an extra pGlyphIndexArray and pTextIndexArray entry.
    EA_ASSERT(nGlyphDisplayEntryArrayCapacity > 0);

    // For the time being, we support only Latin script and assume all text is Latin or equivalent.
    // Once we get up and running we will fill out this function to be proper.
    uint32_t          nGlyphCount = 0;
    TextRunIterator it(pTextRunArray, (uint32_t)nTextRunArraySize);

    if(!it.Empty())
    {
        Font*        pFont;
        GlyphMetrics glyphMetrics;

        // For now we just always pick one font for all glyphs and assume it works.
        // To do: Have an EATEXT_ config option that enables per-glyph font selection.
        //        Possibly have it be a runtime option instead of compile-time. 
        //        This allows fully mixed-language text to be drawn properly.
        EA_ASSERT(!mLayoutSettings.mFontSelection.empty());
        pFont = *mLayoutSettings.mFontSelection.begin();
        EA_ASSERT(pFont != NULL);

        do{
            Char c = *it;

            // Write the glyph attributes.
            if(nGlyphCount < (nGlyphDisplayEntryArrayCapacity - 1)) // -1 because we need space at the end for an extra pGlyphIndexArray and pTextIndexArray entry.
            {
                GlyphAttributes& ga = pGlyphAttributesArray[nGlyphCount];

                // Possibly substitute password mode.
                // To consider: Provide an EATEXT_ config option that enables/disables password mode.
                if(mLayoutSettings.mTextStyleDefault.mPasswordMode == kPMPassword)
                    c = kCharPASS; // Normally, the password char is Unicode 0x25CF (9679)

                // To do: Check the Unicode value that is generating this glyph and if it is a 
                //        zero-width glyph then give the glyph an empty entry. We probably don't 
                //        want to skip the glyph altogether. Take into consideration mLayoutSettings.mbDisplayZWG.
                //        const Char kCharZWSP    = 0x200B;    /// 8203       Zero-Width Space
                //        const Char kCharZWNJ    = 0x200C;    /// 8204       Zero-Width Non-Joiner
                //        const Char kCharZWJ     = 0x200D;    /// 8205       Zero-Width Joiner
                //        const Char kCharZWNBSP  = 0xFEFF;    /// 65279      Zero-Width No-Break Space. Same as kCharBOM. Use kCharWJoin instead.
                //        const Char kCharWJoin   = 0x2060;    /// 8288       Word Joiner, acts the same as Zero-Width No-Break Space

                pFont->GetGlyphIds(&c, 1, &ga.mGlyphId, true); // 'true' here means to always return a valid glyph, possibly the replacement glyph.
                pFont->GetGlyphMetrics(ga.mGlyphId, glyphMetrics);

                ga.mfPenX = 0;
                ga.mfPenY = 0;
                ga.mfX1   =  glyphMetrics.mfHBearingX;
                ga.mfY1   =  glyphMetrics.mfHBearingY * -mLayoutSettings.mfYScale;

                #if EATEXT_VERTICAL_ENABLED
                    if(mLayoutSettings.mTextStyleDefault.mOrientation == kOrientationHorizontal)
                    {
                        ga.mfX2      =  glyphMetrics.mfHBearingX + glyphMetrics.mfSizeX;
                        ga.mfY2      = (glyphMetrics.mfHBearingY - glyphMetrics.mfSizeY) * -mLayoutSettings.mfYScale;
                        ga.mfAdvance =  glyphMetrics.mfHAdvanceX;
                    }
                    else // kOrientationVertical
                    {
                        ga.mfX2      =  glyphMetrics.mfVBearingX + glyphMetrics.mfSizeX;
                        ga.mfY2      = (glyphMetrics.mfVBearingY + glyphMetrics.mfSizeY) * -mLayoutSettings.mfYScale;
                        ga.mfAdvance =  glyphMetrics.mfVAdvanceY;
                    }
                #else
                    // Treat as kOrientationHorizontal.
                    ga.mfX2      =  glyphMetrics.mfHBearingX + glyphMetrics.mfSizeX;
                    ga.mfY2      = (glyphMetrics.mfHBearingY - glyphMetrics.mfSizeY) * -mLayoutSettings.mfYScale;
                    ga.mfAdvance =  glyphMetrics.mfHAdvanceX;
                #endif

                ga.mpFont = pFont;
                ga.mGLA.mGJC = kGJCNone;          // To do: fix this.
                ga.mGLA.mClusterPosition = 0;     // To do: fix this.
                ga.mGLA.mClusterSize = 1;         // To do: fix this.
                ga.mGLA.mbDiacritic = 0;          // To do: fix this.
                ga.mGLA.mbInvisible = (c == ' '); // To do: fix this.
                ga.mGLA.mbZeroWidth = 0;          // To do: fix this.

                *pGlyphIndexArray++ = nGlyphCount;
                if(pTextIndexArray)
                    *pTextIndexArray++  = nGlyphCount;
            }

            ++nGlyphCount;
            ++it;
        } while(*it != kCharInvalid);

        #if (EATEXT_KERNING_SUPPORT > 0)
            Kerning kerning;

            // Apply kerning. It seems that this is best done as a post-processing pass, 
            // due to the volatile nature of glyph selection as we shape above.
            // To do: Convert this to (slightly faster) pointer iterator instead of array index iterator.
            uint32_t i = 0, iEnd = eastl::min_alt(nGlyphCount, nGlyphDisplayEntryArrayCapacity) - 1;

            while(i < iEnd)
            {
                if(pFont->GetKerning(pGlyphAttributesArray[i].mGlyphId, pGlyphAttributesArray[i + 1].mGlyphId, kerning, 0, true))
                    pGlyphAttributesArray[i].mfAdvance += kerning.mfKernX;
                i++;
            }
        #endif
    }

    // Write a final entry for each index array which tells you how to match their ends.
    *pGlyphIndexArray = nGlyphCount;
    if(pTextIndexArray && (nGlyphCount < nGlyphDisplayEntryArrayCapacity))
        *pTextIndexArray = nGlyphCount;

    return nGlyphCount;
}



uint32_t GetWord(TextRunIterator& it, Char& cWordEnd)
{
    if(!it.AtEnd()) // If there is anything to process...
    {
        LineBreakIterator lbi(it);
        const uint32_t    startPosition(it.GetPosition());
        const uint32_t    breakPosition(lbi.GetNextLineBreak());

        cWordEnd = it[breakPosition - 1];
        it.SetPosition(breakPosition);

        return (breakPosition - startPosition);
    }

    cWordEnd = 0;
    return 0;
}

inline bool IsCharNewline(Char c)
{
    return (c == '\n') || (c == 0x2029); // We ignore '\r' because it always precedes '\n' in conventional text.
}


///////////////////////////////////////////////////////////////////////////////
// LineBreakText
//
uint32_t Layout::LineBreakText(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t nTextStartIndex, uint32_t nTextSize, const uint32_t* pGlyphIndexArray,
                             AnalysisArray* pAnalysisArray, 
                             const GlyphAttributes* pGlyphArray, const uint32_t* pTextIndexArray,
                             float fSpace, uint32_t* pTextLineEndArray, uint32_t nTextLineEndArraySize, float* pTextLineSpaceArray)
{
    uint32_t          nLineCount = 0;
    uint32_t          nTextIndex = nTextStartIndex;
    TextRunIterator it(pTextRunArray, (uint32_t)nTextRunArraySize, (uint32_t)nTextStartIndex);
    TextRunIterator itPrev(it);

    if(fSpace < 0.1f)
        fSpace = 0.1f; // Set the space to at least some tiny amount.

    // Make it so that nTextSize is not the size of the block of text to line break but instead is now the end index in that block.
    nTextSize = nTextStartIndex + nTextSize;

    EA_ASSERT(pTextLineEndArray && nTextLineEndArraySize);

    // Run a loop, once for each line.
    while((nTextIndex < nTextSize) && (nLineCount < nTextLineEndArraySize)) // While input isn't exhausted and output isn't exhausted...
    {
        // 'it' points to the current text, which should be the same as the text at position pTextIndexArray[nGlyphIndex].
        // EA_ASSERT(it == 'text at position pTextIndexArray[nGlyphIndex]');

        float  fCurrentSpace = 0.f;
        float  fNewSpace = 0.f;
        uint32_t nCurrentWordTextSize = 0;
        uint32_t nCurrentWordTextEndIndex = 0;
        uint32_t nCurrentWordGlyphBeginIndex = 0;
        uint32_t nCurrentWordGlyphEndIndex = 0;
        Char   cWordEnd = 0;

        EA_ASSERT(it.GetPosition() == nTextIndex);

        while((nTextIndex < nTextSize) && (fCurrentSpace <= fSpace) && !IsCharNewline(cWordEnd)) //  While input isn't exhausted and there is still space on the first line and we haven't hit a foreced newline...
        {
            itPrev = it; // Save the position of the beginning of the word, as the iterator will be incremented on the next loop.

            // At this point we have a pointer to text and a pointer to corresponding glyphs.
            // We need to calculate break opportunities based on text but need to calculate space based on glyphs.
            // We find one word of text and find the corresponding word in terms of glyphs.
            nCurrentWordTextSize        = GetWord(it, cWordEnd);
            nCurrentWordTextEndIndex    = nTextIndex + nCurrentWordTextSize;
            nCurrentWordGlyphBeginIndex = pGlyphIndexArray[nTextIndex];
            nCurrentWordGlyphEndIndex   = pGlyphIndexArray[nCurrentWordTextEndIndex];

            fNewSpace = fCurrentSpace;
            for(uint32_t i = nCurrentWordGlyphBeginIndex; i < nCurrentWordGlyphEndIndex; i++)
            {
                const float fAdvance = pGlyphArray[i].mfAdvance;

                if(fAdvance > 0.f) // Ignore characters that go backwards, as they would screw up the advance calculation.
                {
                    fNewSpace += fAdvance;

                    if(fNewSpace > fSpace) // If the new word doesn't fit...
                        goto SpaceEnd; // C++ doesn't have a way to exit multiple loops, so it's most efficient for us to use goto.
                }
            }

            fCurrentSpace = fNewSpace;
            nTextIndex    = nCurrentWordTextEndIndex;
            EA_ASSERT(it.GetPosition() == nTextIndex);
        }

        SpaceEnd:
        // At this point 'itPrev' points to the previous word begin and 'it' points to the next word begin.
        // For empty input text, 'itPrev' and 'it' will be equal. But this shouldn't happen because we only execute this code if the text is non-empty.
        EA_ASSERT((nTextIndex == nTextStartIndex) || !(it == itPrev));

        if(nTextIndex == nTextStartIndex) // If nothing fit in the space...
        {
            switch(mLayoutSettings.mTextStyleDefault.mTextWrap)
            {
                case kTWSoft:
                case kTWWrap:
                case kTWNone:
                default: // Fall through to kTWHard, as that's the safest mode.
                case kTWHard:
                case kTWInherit:
                {
                    // What we do here is just place the first word as if it was able to fit.
                    // Leave 'it' where it is.
                    fCurrentSpace += fNewSpace;
                    nTextIndex     = nCurrentWordTextEndIndex;
                    EA_ASSERT(it.GetPosition() == nTextIndex);
                    break;
                }

                case kTWEmergency:  // Since nLength > 0 and nCurrentGlyphCount == 0, there must be a long initial word or a small nWidth.
                {
                    it = itPrev; // Go back to the beginning.
                    EA_ASSERT(it.GetPosition() == nTextIndex);

                    // Here we simply start grabbing chars until we hit the right side of the space.
                    fCurrentSpace = 0.f;

                    for(uint32_t i = pGlyphIndexArray[nTextIndex]; (nTextIndex < nTextSize) && (fCurrentSpace < fSpace); i++)
                    {
                        const float fAdvance = pGlyphArray[i].mfAdvance;

                        if(fAdvance > 0.f) // Ignore characters that go backwards, as they would screw up the advance calculation.
                        {
                            fCurrentSpace += fAdvance;

                            if(fCurrentSpace < fSpace)
                            {
                                while(nTextIndex <= pTextIndexArray[i]) // There can be 0, 1, or more iterations of this loop.
                                {
                                    ++nTextIndex;
                                    ++it;
                                }
                            }
                        }
                    }
                    EA_ASSERT(it.GetPosition() == nTextIndex);
                    break;
                }
            }
        }     // Note: We need to be very careful about this fNewSpace > fSpace expression.
        else if((nTextIndex < nTextSize) && ((fNewSpace > fSpace) || !IsCharNewline(cWordEnd))) // Else the line has glyphs on it but we ran out of space before all glyphs could fit on the line.
        {
            it = itPrev; // Go to the beginning of the previous word, which should be the first word for the next line.
            EA_ASSERT(it.GetPosition() == nTextIndex);
        }

        // Any space characters after the end of the width are invisible and should 
        // be included as part of the fitting characters. But we only allow at most
        // one of them. 
        const Char cCurrent = *it;
        if((cCurrent == ' ') && (cWordEnd != ' ') && (nTextIndex < nTextSize)) // To do: fix this to check for whitespace and also possibly make the text index check redundant so it can be removed.
        {
            ++it;          // We know that 'it' points to a single standalone (i.e. non-clustered) Char, so a simple increment is OK here.
            ++nTextIndex;
        }

        EA_ASSERT(fCurrentSpace > 0.f);
        if(fCurrentSpace < 0.1f)
            fCurrentSpace = 0.1f;

        if(nTextIndex > nTextStartIndex) // If any text was fitted into the space...
        {
            if(pTextLineSpaceArray)
                pTextLineSpaceArray[nLineCount] = fCurrentSpace;
            pTextLineEndArray[nLineCount] = nTextStartIndex = nTextIndex; // Prepare to fit a new line of text.
            nLineCount++;

            // Possibly split an AnalysisArray entry here if an entry is found to straddle a line.
            // The reason we would want to split the AnalysisArray is that reordering of text must
            // be done via the Analysis information and must be done independently for each line.
            // Thus, an Analysis must not cross two lines. So if we find such an Analysis, we split 
            // it so that the first part of it is on one line and the second is on the next line.
            if(pAnalysisArray)
            {
                for(AnalysisArray::iterator itAA = pAnalysisArray->begin(); itAA != pAnalysisArray->end(); ++itAA) // This is a linear search, but in practice there are rarely more three Analysis items, so this will not be slow.
                {
                    Analysis& analysis = *itAA;

                    if((nTextStartIndex >= analysis.mnPosition) && (nTextStartIndex < (analysis.mnPosition + analysis.mnLength))) // If the line start is in this Analysis.
                    {
                        if(nTextStartIndex > analysis.mnPosition) // If the line start is somewhere in *the middle* of the Analysis...
                        {
                            // We split the analysis 
                            Analysis analysisNew(analysis);
                            analysisNew.mnLength = nTextStartIndex - analysis.mnPosition;
                            analysis.mnPosition  = nTextStartIndex;
                            analysis.mnLength   -= analysisNew.mnLength;
                            pAnalysisArray->insert(itAA, analysisNew);
                        }

                        break;
                    }
                }
            }
        }
        else
            break; // The first time any text doesn't fit, then no more text can ever fit.
    }

    return nLineCount;
}



///////////////////////////////////////////////////////////////////////////////
// OrderText
//
void Layout::OrderText(const Analysis* /*pAnalysisArrayInput*/, uint32_t /*nAnalysisArrayInputSize*/, Analysis* /*pAnalysisArrayOutput*/)
{
    // We don't yet support reordering of text (which means we don't yet support bidirectional text).
    // But we will support it eventually. In the meantime, this function is a no-op.
}



///////////////////////////////////////////////////////////////////////////////
// JustifyGlyphs
//
void Layout::JustifyGlyphs(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t nTextStartIndex, uint32_t nTextSize, uint32_t* pGlyphIndexArray,
                            GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphDisplayEntryArrayCapacity, uint32_t* /*pTextIndexArray*/,
                            float fSizeChange, bool bSizeChangeIsRelative)
{
    // We don't yet support the insertion of new glyphs in order to implement justification.
    // Such insertion would be used in Arabic script to implement inter-glyph extenders called kashidas.
    // Such insertion would also result in inserting entries into pGlyphAttributesArray and modifying
    // pGlyphIndexArray and pTextIndexArray.

    // Find out if this script allows stretching of text lines by increasing 
    // inter-character space in addition to inter-word space. We are interested
    // in the ScriptProperties::mbCharacterJustify value.
    ScriptProperties scriptProperties;
    GetScriptProperties(mLayoutSettings.mScript, &scriptProperties, true);

    // Move the text iterator to the position of the glyph start index.
    TextRunIterator it(pTextRunArray, (uint32_t)nTextRunArraySize, (uint32_t)nTextStartIndex);
    EA_ASSERT(*it != kCharInvalid);

    // If we want to stretch the text out by a percentage, then we need to know 
    // what the space usage is of the given text. Then we calculate the absolute 
    // space usage increase. Then we distribute the space increase amongst the eligible
    // glyphs in an appropriate way, such as by increasing their advance widths.
    const uint32_t kEligibleTextIndexArrayCapacity = 256;
    uint32_t       pEligibleTextIndexArray[kEligibleTextIndexArrayCapacity];
    uint32_t       nEligibleTextIndexArraySize = 0;

    // Walk down the text and find eligible stretching candidates.
    // For now we just do a dumb pick which doesn't take into account character context.
    // Character context is important because, for example, we don't want to stretch
    // space characters (which ought to be no-break space) that are between numerical digits. 
    for(uint32_t i = nTextStartIndex, iEnd = nTextStartIndex + nTextSize; i < iEnd; ++i)
    {
        const Char c = *it;
        ++it;

        if(c == ' ') // To do: Fix this to use a proper set of Unicode space characters (and not tabs). 
        {
            if(nEligibleTextIndexArraySize < kEligibleTextIndexArrayCapacity)
                pEligibleTextIndexArray[nEligibleTextIndexArraySize++] = i;
        }
    }

    // Calculate the absolute size change if not already absolute.
    if(bSizeChangeIsRelative) // If the size change is a relative value and not an absolute value...
    {
        // Now find the space usage of the input glyphs.
        float fSpace = 0.f;
        const uint32_t nGlyphStartIndex = pGlyphIndexArray[nTextStartIndex];
        const uint32_t nGlyphEndIndex   = pGlyphIndexArray[nTextStartIndex + nTextSize - 1] + 1; // We do the '-1/+1' here because otherwise we could run off the array bounds.

        (void)nGlyphDisplayEntryArrayCapacity;
        EA_ASSERT(nGlyphEndIndex <= nGlyphDisplayEntryArrayCapacity);

        for(uint32_t nGlyphIndex = nGlyphStartIndex; nGlyphIndex < nGlyphEndIndex; ++nGlyphIndex)
        {
            const float fAdvance = pGlyphAttributesArray[nGlyphIndex].mfAdvance;
            if(fAdvance > 0)
                fSpace += fAdvance;
        }

        fSizeChange = (fSpace * fSizeChange) - fSpace; // If input fSizeChange is 1.0 (relative), fSizeChange will become 0 (absolute).
    }

    // Now distribute the size change across the eligible glyphs.
    const float fSizeChangePerGlyph = fSizeChange / nEligibleTextIndexArraySize;

    for(uint32_t j = 0; j < nEligibleTextIndexArraySize; ++j)
    {
        const uint32_t nTextIndex  = pEligibleTextIndexArray[j];
        const uint32_t nGlyphIndex = pGlyphIndexArray[nTextIndex];
        pGlyphAttributesArray[nGlyphIndex].mfAdvance += fSizeChangePerGlyph;
    }
}



///////////////////////////////////////////////////////////////////////////////
// PlaceGlyphs
//
uint32_t Layout::PlaceGlyphs(const Analysis* pAnalysisArray, uint32_t nAnalysisArraySize, const uint32_t* pGlyphIndexArray,
                            const GlyphAttributes* pGlyphAttributesArray, uint32_t /*nGlyphAttributesArraySize*/, 
                            float x, float y, GlyphDisplayEntry* pGlyphDisplayEntryArray)
{
    GlyphDisplayEntry* pGlyphDisplayEntryCurrent = pGlyphDisplayEntryArray;

    float xCurrent = x;

    // Right now we assume that all text is left to right. So we just place glyphs sequentially.
    // To do this properly, we need to place the RTL analyses backwards. Also, we support horizontal
    // output only for the time being. We will rectify this once the system is up and running.
    for(uint32_t i = 0; i < nAnalysisArraySize; i++)
    {
        const Analysis& analysis = pAnalysisArray[i];

        const uint32_t nGlyphIndexBegin = pGlyphIndexArray[analysis.mnPosition];
        const uint32_t nGlyphIndexEnd   = pGlyphIndexArray[analysis.mnPosition + analysis.mnLength];

        const GlyphAttributes* pGlyphAttributesBegin   = pGlyphAttributesArray + nGlyphIndexBegin;
        const GlyphAttributes* pGlyphAttributesCurrent = pGlyphAttributesBegin;
        const GlyphAttributes* pGlyphAttributesEnd     = pGlyphAttributesArray + nGlyphIndexEnd;

        while(pGlyphAttributesCurrent < pGlyphAttributesEnd)
        {
            pGlyphDisplayEntryCurrent->mpFont    = pGlyphAttributesCurrent->mpFont;
            pGlyphDisplayEntryCurrent->mfPenX    = xCurrent;
            pGlyphDisplayEntryCurrent->mfPenY    = y;
            pGlyphDisplayEntryCurrent->mfX1      = xCurrent + pGlyphAttributesCurrent->mfX1;
            pGlyphDisplayEntryCurrent->mfY1      = y        + pGlyphAttributesCurrent->mfY1; // Don't need to adjust for yscale because glyph attributes already have that built in.
            pGlyphDisplayEntryCurrent->mfX2      = xCurrent + pGlyphAttributesCurrent->mfX2;
            pGlyphDisplayEntryCurrent->mfY2      = y        + pGlyphAttributesCurrent->mfY2;
            pGlyphDisplayEntryCurrent->mfAdvance = pGlyphAttributesCurrent->mfAdvance;
            pGlyphDisplayEntryCurrent->mGLA      = pGlyphAttributesCurrent->mGLA;
            pGlyphDisplayEntryCurrent->mGlyphId  = pGlyphAttributesCurrent->mGlyphId;

            xCurrent += pGlyphAttributesCurrent->mfAdvance;
            pGlyphAttributesCurrent++;
            pGlyphDisplayEntryCurrent++;
        }
    }

    return (uint32_t)(pGlyphDisplayEntryCurrent - pGlyphDisplayEntryArray);
}



///////////////////////////////////////////////////////////////////////////////
// TextPositionToDisplayPosition
//
bool Layout::TextPositionToDisplayPosition(uint32_t nTextPosition, bool bTrueIfLeadingFalseIfTrailing, 
                                            const uint32_t* pGlyphIndexArray, uint32_t nGlyphIndexArraySize, 
                                            const GlyphAttributes* pGlyphAttributesArray, uint32_t /*nGlyphAttributesArraySize*/, 
                                            Point& ptDisplayPosition)
{
    if(bTrueIfLeadingFalseIfTrailing)
        nTextPosition++;

    if(nTextPosition < nGlyphIndexArraySize)
    {
        // To do: We need to check for intra-cluster positioning with some complex scripts.
        const uint32_t nGlyphIndex = pGlyphIndexArray[nTextPosition];
        ptDisplayPosition.mX     = pGlyphAttributesArray[nGlyphIndex].mfPenX;
        ptDisplayPosition.mY     = pGlyphAttributesArray[nGlyphIndex].mfPenY;
    }
    else
    {
        const uint32_t nGlyphIndex = pGlyphIndexArray[nGlyphIndexArraySize - 1];
        ptDisplayPosition.mX     = pGlyphAttributesArray[nGlyphIndex].mfPenX + pGlyphAttributesArray[nGlyphIndex].mfAdvance;
        ptDisplayPosition.mY     = pGlyphAttributesArray[nGlyphIndex].mfPenY;
    }

    return (nTextPosition < nGlyphIndexArraySize);
}



///////////////////////////////////////////////////////////////////////////////
// DisplayPositionToTextPosition
//
bool Layout::DisplayPositionToTextPosition(const Point& ptDisplayPosition, const uint32_t* pTextIndexArray, uint32_t /*nTextIndexArraySize*/, 
                                            GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArraySize, 
                                            uint32_t& nTextPosition, bool& bTrueIfLeadingFalseIfTrailing)
{
    // We walk the string linearly from beginning to end. Usually these strings are only 10 to 50 characters in length.
    // To do: We need to check for intra-cluster positioning with some complex scripts.
    for(uint32_t i = 0; i < nGlyphAttributesArraySize; i++)
    {
        #ifdef EATEXT_VERTICAL_ENABLED
            if(mLayoutSettings.mTextStyleDefault.mOrientation == kOrientationHorizontal)
            {
        #endif
                const float fDistance1 = (pGlyphAttributesArray[i].mfPenX - ptDisplayPosition.mX);

                if(fDistance1 > 0) // If we've passed the display position...
                {
                    if(i == 0) // If the input position is prior to all glyphs.
                    {
                        bTrueIfLeadingFalseIfTrailing = true;
                        nTextPosition = 0;
                    }
                    else
                    {
                        const float fDistance0 = (ptDisplayPosition.mX - pGlyphAttributesArray[i - 1].mfPenX);

                        if(fDistance0 > fDistance1)
                        {
                            bTrueIfLeadingFalseIfTrailing = true;
                            nTextPosition = pTextIndexArray[i];
                        }
                        else
                        {
                            bTrueIfLeadingFalseIfTrailing = false;
                            nTextPosition = pTextIndexArray[i];
                        }
                    }
                }
        #ifdef EATEXT_VERTICAL_ENABLED
            }
            else
            {
                // This section of code is the same as the horizontal code above but using Y instead of X. 
                // To consider: Find a way to fold the two sections into one.
                const float fDistance1 = (pGlyphAttributesArray[i].mfPenY - ptDisplayPosition.mY);

                if(fDistance1 > 0) // If we've passed the display position...
                {
                    if(i == 0) // If the input position is prior to all glyphs.
                    {
                        bTrueIfLeadingFalseIfTrailing = true;
                        nTextPosition = 0;
                    }
                    else
                    {
                        const float fDistance0 = (ptDisplayPosition.mY - pGlyphAttributesArray[i - 1].mfPenY);

                        if(fDistance0 > fDistance1)
                        {
                            bTrueIfLeadingFalseIfTrailing = true;
                            nTextPosition = pTextIndexArray[i];
                        }
                        else
                        {
                            bTrueIfLeadingFalseIfTrailing = false;
                            nTextPosition = pTextIndexArray[i];
                        }
                    }
                }
            }
        #endif
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ShutdownLayout
//
EATEXT_API void ShutdownLayout()
{
    AnalysisArray& lineAnalysisArray = GetLayoutLineAnalysisArray();
    lineAnalysisArray.set_capacity(0);

    AnalysisArray& breakAnalysisArray = GetLineBreakAnalysysArray();
    breakAnalysisArray.set_capacity(0);

    AnalysisArray& paragraphAnalysisArray = GetParagraphAnalysisArray();
    paragraphAnalysisArray.set_capacity(0);
}



} // namespace Text

} // namespace EA


#endif // EATEXT_LAYOUT_ENABLED



