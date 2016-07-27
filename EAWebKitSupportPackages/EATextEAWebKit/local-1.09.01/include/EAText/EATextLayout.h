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
// EATextLayout.h
//
// By Paul Pedriana - 2004
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// *** This file deprecated and no longer supported. ***
//
// It is completely superceded by EATextTypesetter.
///////////////////////////////////////////////////////////////////////////////




























#ifndef EATEXT_EATEXTLAYOUT_H
#define EATEXT_EATEXTLAYOUT_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextScript.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/EATextIterator.h>
#include <EAText/EATextUnicode.h>
#include <EASTL/vector.h>
#include <stddef.h>
#include <string.h>


#if EATEXT_LAYOUT_ENABLED

namespace EA
{
    namespace Text
    {
        // Forward declarations
        class Font;
        class FontServer;



        /// TextProperties
        ///
        /// Defines properties of a given string of text. This is useful for telling the engine what kind
        /// of text it has to work on and how it might be able to take shortcuts by eliminating steps for
        /// simple text. TextProperties are similar to ScriptProperties. The distinction is that TextProperties
        /// are simply the sum of the ScriptProperties found for a piece of text that possibly has multiple
        /// scripts present. Only those ScriptProperties which affect text properties are represented here.
        ///
        struct EATEXT_API TextProperties
        {
            bool mbNumericChars   : 1;  /// True if string has characters that are only digits and the other characters used in writing numbers by the rules of the Unicode bidirectional algorithm. For example, currency symbols, the thousands separator, and the decimal point are classified as numeric when adjacent to or between digits.
            bool mbControlChars   : 1;  /// True if string has characters that are only control characters.
            bool mbAmbiguousChars : 1;  /// True if string has characters that are supported by more than one charset.
            bool mbPrivateChars   : 1;  /// True if string has characters from the private Unicode range (E000 - F8FF). 
            bool mbComplex        : 1;  /// True if string has characters that would require complex layout.
            bool mbLTRChars       : 1;  /// True if string has characters that are categorized as left to right.
            bool mbRTLChars       : 1;  /// True if string has characters that are categorized as right to left.
            bool mbNeutralChars   : 1;  /// True if string has characters that are categorized as directionally neutral.
            bool mbClusters       : 1;  /// True if string has characters that are always clustered (e.g. composed Thai).

            TextProperties()
              : mbNumericChars(false), mbControlChars(false), mbAmbiguousChars(false), mbPrivateChars(false),
                mbComplex(false), mbLTRChars(false), mbRTLChars(false), mbNeutralChars(false), mbClusters(false) { }
        };


        /// TextState
        ///
        /// Defines the state of text before analysis (e.g. at the beginning of a paragraph) 
        /// and for each item (run) in the paragraph. To understand what these values mean, 
        /// you really need to understand the Unicode standard related to bidirectionality. 
        /// See http://www.unicode.org/reports/tr9/. This struct is similar to the Uniscribe
        /// SCRIPT_STATE struct.
        ///
        struct EATEXT_API TextState
        {
            unsigned mnLevel                 : 4;   /// Bidirectional embedding level. 0, 1, 2, 3, ...
            unsigned mbForcedDirection       : 1;   /// True if this run's direction was set by an explicit direction override (LRO or RLO).
            unsigned mbSwapInhibited         : 1;   /// True if character mirroring has been explicitly disabled (by ISS). ISS and ASS are deprecated by the Unicode standard (http://www.unicode.org/reports/tr20/#Deprecated), but some text editors generate these codes and so we support them.
            unsigned mbDisableNationalDigits : 1;   /// True if the use of locale-specific digits is disabled (NaDS, NoDS). NaDS and NoDS are deprecated by the Unicode standard (http://www.unicode.org/reports/tr20/#Deprecated), but some text editors generate these codes and so we support them.
        };


        /// Analysis
        ///
        /// An analysis defines a segment of text of a single direction and script.
        /// The layout pipeline takes a string of text and produces one or more Analysis
        /// objects for it. Plain English text would only ever have one such Analysis, 
        /// but the use of mixed languages or the use of numbers within RTL languages
        /// can produce more than one Analysis objects for the text. An Analysis is 
        /// particularly important in enabling proper reordering of bidirectional text.
        ///
        struct EATEXT_API Analysis
        {
            uint32_t  mnPosition;           /// The position in a string of text this analysis applies to.
            uint32_t  mnLength;             /// The length in a string of text that this analysis applies to.
            TextState mTextState;           /// This is analysis info that applies not just to a text run, but also to the layout state at the beginning of the paragraph before the first run.
            Script    mScript        : 7;   /// One of enum Script. For runs that have multiple scripts present, this is the primary script.
            unsigned  mnParity       : 1;   /// Embedding level 'parity'. 0 for LTR, 1 for RTL.
            unsigned  mnParentParity : 1;   /// Embedding level 'parity'. 0 for LTR, 1 for RTL. Parent partity is not necessarily the opposite of parity.
            unsigned  mbJoinBefore   : 1;   /// There is a joiner before this item (run) such as a zero-width joiner.
            unsigned  mbJoinAfter    : 1;   /// There is a joiner after this item (run) such as a zero-width joiner.
        };

        /// AnalysisArray
        ///
        /// To do: Make this use our custom allocator. 
        /// To consider: Convert this to an intrusive linked list. That is, add mpPrev and mpNext 
        ///              pointers to the Analysis struct. This may not be a benefit as it uses more
        ///              memory and usually results in more allocations.
        ///
        typedef eastl::vector<Analysis> AnalysisArray; 



        /// GlyphLayoutAttributes
        ///
        struct EATEXT_API GlyphLayoutAttributes // 2 bytes
        {
            GlyphJustificationClass mGJC             : 4;   /// Justification classification
            unsigned                mClusterPosition : 3;   /// Zero-based position of this glyph within its cluster. Standalone glyphs have a value of 0.
            unsigned                mClusterSize     : 3;   /// Size of the cluster this glyph is in. Standalone glyphs have a value of 1.
            unsigned                mbDiacritic      : 1;   /// True if this character is a diacritic of any type.
            unsigned                mbInvisible      : 1;   /// True if this character has nothing to it (e.g. space, tab, zero-width).
            unsigned                mbZeroWidth      : 1;   /// ZWSP, ZWNBSP, ZWJ, ZWNJ, or anything with no width. If this is true, then mbInvisible will be true also.
        };


        /// GlyphAttributes
        ///
        /// Provides information for an individual glyph that is layed out or in a display list.
        ///
        /// To do: Find a way to compress this data, perhaps by reducing the font pointer to 
        ///        a 16 bit font id and using some form of compression on the floating point
        ///        coordinate values, perhaps to 12.4 fixed point. This struct could be packed
        ///        down to 20 bytes or less. Note that it is not terrible that this struct be
        ///        so big, as it is usually a temporary intermediate struct and there usually
        ///        aren't any more than a hundred or so of them in an array.
        ///
        struct EATEXT_API GlyphAttributes // 36 bytes
        {
            Font*                   mpFont;         /// Font. If this is stored on disk as a persisted object, it holds the numerical unique font id. Ideally this member would be of type uintptr_t, but that would make it less readable.
            float                   mfPenX;         /// Pen position, used for display lists of layed out text. When GlyphAttributes applies to an individual glyph, this is zero. This field is more or less required for fonts which are not based on bitmaps, such as polygonal fonts or vector fonts. The pen position is the local coordinate origin for the glyph.
            float                   mfPenY;         /// When GlyphAttributes applies to an individual glyph, this is zero.
            float                   mfX1;           /// Same as x bearing: x position of top-left corner. For individual glyphs the position is relative to the pen position; for layed out display lists it is in absolute coordinates.
            float                   mfY1;           /// Same as y bearing: y position of top-left corner.
            float                   mfX2;           /// X position of bottom-right corner. We use x2 instead of width  because the former maps to what is done with the value better.
            float                   mfY2;           /// Y position of bottom-right corner. We use y2 instead of height because the former maps to what is done with the value better.
            float                   mfAdvance;      /// Distance along x (horizontal text) or y (vertical text) to next glyph.
            GlyphLayoutAttributes   mGLA;           /// Glyph layout attributes. Defines basic properties of glyphs needed for layout.
            GlyphId                 mGlyphId;       /// Maps to a glyph id in the font. Not usually the same as Unicode value.
        };

        struct GlyphDisplayEntry : public GlyphAttributes { };


        /// LayoutMetrics
        ///
        /// Defines metrics of layed out text. These are useful for knowing the space used by 
        /// layed out text. These layout metrics are coordinate-system-orientation-independent.
        /// Thus, the mfBaseline value for a char like 'A' is always positive for both y-down
        /// and y-up coordinate systems, and mfDescent for a char like 'g' is always negative.
        ///
        struct EATEXT_API LayoutMetrics
        {
            uint32_t mnSize;          /// Number of layed out Unicode characters.
            float    mfSpace;         /// Space used by layed out Unicode characters, defined by final pen position. For line layout, this is the end of the line; for paragraph layout, this is the height of the paragraph for horizontal layout or the width of the paragraph for vertical layout.
            uint32_t mnVisibleSize;   /// Number of layout out Unicode characters to the last visible one. Trailing invisible characters are not included.
            float    mfVisibleSpace;  /// Space used by the mnVisibleSize characters, defined by pen position of first trailing invisible character. For paragraph layout, this will be equal to mfSpace.
            float    mfBaseline;      /// Equal to max of FontMetrics::mfBaseline for all fonts represented on the line. The entire line height is the baseline minus (because descent is negative) the descent.
            float    mfDescent;       /// Equal to min of FontMetrics::mfDescent for all fonts represented on the line. Recall that lower descents mean lower values.
            uint32_t mnLineCount;     /// Number of lines resulting from the layout.
        };



        ///////////////////////////////////////////////////////////////////////
        /// class Layout
        ///
        /// Implements text layout.
        ///       
        class EATEXT_API Layout
        {
        public:
            Layout();
           ~Layout();


            /// GetLayoutSettings
            ///
            LayoutSettings* GetLayoutSettings()
                { return &mLayoutSettings; }

            
            /// SetLayoutSettings
            ///
            /// Sets the default layout settings. The default layout settings are used whenever a 
            /// specific layout setting is not supplied by the user in function calls which use
            /// a LayoutSettings object. The pLayoutSettings data is copied to an internal version
            /// and the user can dispose of the pLayoutSettings at any time thereafter.
            ///
            /// If pLayoutSettings is NULL, then the default layout settings are set to their 
            /// default values.
            ///
            void SetLayoutSettings(const LayoutSettings* pLayoutSettings);


            /// SetLayoutFonts
            ///
            /// Sets up the fonts to use for layout, given a TextStyle. The fonts set in the
            /// pLayoutSettings; if pLayoutSettings is NULL, then the member LayoutSettings of
            /// this Layout class is used.
            ///
            void SetLayoutFonts(const TextStyle* pTextStyle = NULL, LayoutSettings* pLayoutSettings = NULL, FontServer* pFontServer = NULL);


            ///////////////////////////////////////////////////////////////////
            // Higher level functionality
            ///////////////////////////////////////////////////////////////////

            /// LayoutSimple
            ///
            /// Does very basic layout of a string of text with a single predetermined font.
            /// Layout is done on a single line. No complex functionality such as shaping
            /// and bidirectionality is supported. Each Unicode input characater is simply
            /// directly converted to its associated glyph and placed in the output list
            /// along with appropriate pen advance information. Control characters such 
            /// as '\n' have no special effect on the output and are treated just like 
            /// conventional displayed glyphs. Input text can be any Unicode text.
            ///
            /// Example usage:
            ///    GlyphDisplayEntry pGDE[32];
            ///    uint32_t glyphCount = layout.LayoutSimple("Hello World", strlen("Hello World"), 0, 0, pSomeFont, Font::kRFGrayscale, pGDE);
            ///
            uint32_t LayoutSimple(const Char* pText, uint32_t nTextSize, float x, float y, Font* pFont, 
                                 GlyphDisplayEntry* pGlyphDisplayEntryArray);

            /// LayoutSimple
            ///
            /// This is an 8 bit per charcter version of LayoutSimple. Characters are still
            /// interpreted as Unicode, but only Unicode values from 0-255 are recognized.
            /// Note that such Unicode values are the Latin-1 character set, a.k.a. ISO 8559-1.
            ///
            /// Example usage:
            ///    GlyphDisplayEntry pGDE[32];
            ///    uint32_t glyphCount = layout.LayoutSimple("Hello World", strlen("Hello World"), 0, 0, pSomeFont, Font::kRFGrayscale, pGDE);
            ///
            uint32_t LayoutSimple(const char* pText, uint32_t nTextSize, float x, float y, Font* pFont, 
                                 GlyphDisplayEntry* pGlyphDisplayEntryArray);


            /// LayoutTextLine
            ///
            /// Draws a string of text on the specified destination at the specified 
            /// location. The x,y location position normally refers to the pen position 
            /// and not the top-left or bottom-left of the character cell. 
            ///
            /// The TextStyle specifies the basic look of the text, such as 
            /// font family, font weight, color, smoothing.
            ///
            /// The LayoutSettings is optional and specifies advanced layout properties,
            /// such as base direction, word wrapping settings, digit support, etc.
            /// Note that LayoutSettings in some respects is extending the TextStyle.
            ///
            /// If the input ppDisplayList is non-null, the instructions to draw the 
            /// text are stored in the returned ppDisplayList. This display list must later
            /// be destroyed with DestroyDisplayList.
            ///
            /// This function is primarily for drawing 2D text onto a 2D raster destination.
            /// However, if you pass pDestination as NULL and x,y as 0, then text is drawn
            /// as a 3D object which uses the current model and view transformations to specify
            /// where and how it is drawn in 3D space.
            ///
            uint32_t LayoutTextLine(const Char* pText, uint32_t nTextSize, 
                                    float x, float y, 
                                    const TextStyle* pTextStyle,
                                    GlyphDisplayEntry* pGlyphDisplayEntryArray, uint32_t nGlyphDisplayEntryArrayCapacity,
                                    LayoutMetrics* pLayoutMetrics = NULL);


            /// LayoutTextParagraph
            ///
            /// Draws a potentially multi-line paragraph of a single style within the given rectangle. 
            /// The text is drawn -- with wrapping -- within the rectangle until the first character 
            /// which is completely outside the rectangle.
            /// The horizontal and vertical alignment directives of LayoutSettings are adhered to.
            /// 
            /// If the input rectangle coordinates have a height <= 1.0 (for horizontal layout)
            /// then this function acts much the same as the DrawTextLine function, as it will 
            /// always run out of room at the end of the first line.
            ///
            /// See LayoutTextLine for more information about the parameters and functionality.
            ///
            uint32_t LayoutTextParagraph(const Char* pText, uint32_t nTextSize, 
                                        float fRectLeft, float fRectTop, float fRectRight, float fRectBottom, 
                                        const TextStyle* pTextStyle,
                                        GlyphDisplayEntry* pGlyphDisplayEntryArray, uint32_t nGlyphDisplayEntryArrayCapacity,
                                        LayoutMetrics* pLayoutMetrics = NULL);


            ///////////////////////////////////////////////////////////////////
            // Higher level metrics
            ///////////////////////////////////////////////////////////////////

            /// GetTextLayoutMetrics
            ///
            /// Returns the (linear) pixel usage of the input string. By this we simply 
            /// mean how many pixels wide the string is when rendered.
            /// If the layout is horizontal (specified in the layout settings), then the
            /// return value is the horizontal pixel width. Else for the case of vertical
            /// text it is the height.
            ///
            /// Returns the space used by the input string. This space is simply the 
            /// distance from the starting pen position to the ending pen position at
            /// the end of the string, regardless of the fact that the string may be 
            /// all space characters or otherwise invisible.
            ///
            float GetTextLayoutMetrics(const Char* pText, uint32_t nTextSize, 
                                        const TextStyle* pTextStyle, 
                                        LayoutMetrics* pLayoutMetrics);


            /// GetNominalBaseline
            ///
            /// Returns the estimated baseline distance for the specified text style
            /// (or the default style if none is specified.) The result will be the
            /// baseline with the greatest magnitude selected from the list of fonts
            /// that are referenced by this style.
            ///
            float GetNominalBaseline( const TextStyle *pTextStyle = NULL ) const;

            /// GetNominalDescent
            ///
            /// Returns the estimated descent distance for the specified text style
            /// (or the default style if none is specified.) The result will be the
            /// descent with the greatest magnitude selected from the list of fonts
            /// that are referenced by this style.
            ///
            float GetNominalDescent( const TextStyle *pTextStyle = NULL ) const;

            /// CalculateLineBreak
            ///
            /// This is a one-shot high level paragraph line breaking function, though it returns
            /// only one break at a time. It is useful for implementing basic multi-line text displays. 
            ///
            /// The return value is the length of text which fits within the space. Note that 
            /// it is possible that invisible space after the text (as part of the last glyph's
            /// area or as individual space characters) may extend beyond the space.
            ///
            /// The nSpace parameter specifies how much pixel space there is for the text.
            /// 
            /// The pSpace parameter allows the user to retrive the pixel length of the fitted text.
            /// The pVisibleLength parameter returns the string length to the last character with 
            /// visible pixels. Trailing spaces would not be included.
            /// The pVisibleSpaceParameter allows the user to retrieve the amount of visible pixels 
            /// used by the fitted string.
            ///
            uint32_t CalculateLineBreak(const Char* pText, uint32_t nTextSize, float fSpace, 
                                      const TextStyle* pTextStyle, 
                                      LayoutMetrics* pLayoutMetrics);





            ///////////////////////////////////////////////////////////////////
            // Lower level functionality
            //
            // These functions are listed in the order you would normally call 
            // them in sequence.
            ///////////////////////////////////////////////////////////////////


            /// GetParagraphBoundary
            ///
            /// *** This function is deprecated. Use EATextBreak instead. ***
            ///
            /// Given an arbitrary block of text, this function finds paragraph
            /// boundaries suitable for doing layout with. If you don't already have
            /// text separated into paragraphs or equivalently standalone blocks,
            /// then this function is the first step in the text layout pipeline.
            ///
            /// This function detects paragraphs that end by "\n', "\r", "\r\n", 
            /// the Unicode standard paragraph separator character, or simply the 
            /// end of the text itself. This function detects only a paragraph at a time.
            /// To consider: Consider having this work on multiple paragraphs at a time.
            ///
            /// The input pTextRunArray must point to valid text if nTextRunArrayLength is > 0.
            /// The text of pTextRunArray need not be zero-terminated.
            /// The output pParagraphDelimiterIndex is the index of the first delimiter.
            /// The output pParagrphEndIndex is the index of one-past the last delimiter
            /// and this refers to the start of the next paragraph. If the text has no
            /// paragraph delimiters, both values will be equal to the size of the text
            /// and thus refer to one-past the last character in the text.
            /// Both of these output parameters may be NULL, in which case they will not
            /// be written to. 
            /// The return value is the same as what is written to pParagraphEndIndex.
            ///
            uint32_t GetParagraphBoundary(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                                        uint32_t* pParagraphDelimiterIndex = NULL, uint32_t* pParagraphEndIndex = NULL);


            /// GetTextProperties
            ///
            /// Tells what kind of characters are in the input text.
            /// The pLayoutSettings parameter provides information that affects how decisions about
            /// complex chars are made. For example, digits might be set to use script-specific digits.
            /// If pLayoutSettings is NULL, the default layout settings are applied.
            ///
            void GetTextProperties(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                                   TextProperties& textProperties);

            /// AnalyzeText
            ///
            /// Produces one or more Analysis objects for a string of text.
            ///
            /// The input pTextRunArray must point to valid text if nTextRunArrayLength is > 0.
            /// The text of pTextRunArray need not be zero-terminated.
            /// The return value is the number of Analysis entries the input text produces.
            /// The return value is thus the same as the number of entries added to analysisArray.
            ///
            /// Analysis should be done on a paragraph (or similar) of text at a time. The reason for
            /// this is that otherwise bidirectionality and shaping can otherwise fail due to tricky
            /// issues related to text reordering and glyph substitution. Thus the input pTextRunArray 
            /// should contain an entire paragraph of text. It will often happen that you have the 
            /// paragraph as a single continuous string; if so then simply pass nTextRunArrayLength
            /// as 1. But if you are doing styled text (e.g. mixed fonts) then you may very well have
            /// your text broken into runs of similar style. In this case you will want to use a 
            /// nTextRunArrayLength which is >= 1.
            ///
            /// Note that the resulting analysis array doesn't necessarily match the text run array
            /// one for one. It would likely only be so when there is only one text run and it
            /// has text from only one script. But if nTextRunArrayLength > 0 then it is unlikely
            /// that the resulting pAnalysisArray size will be the same, as analysis runs and text
            /// runs aren't the same kind of thing.
            ///
            /// To consider: Possibly use a linked list of Analysis items instead of an array.
            ///
            uint32_t AnalyzeText(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                               AnalysisArray& analysisArray, const TextState* pTextState = NULL);


            /// ShapeGlyphs
            ///
            /// Glyph shaping is the process of converting Unicode code points (characters) into font glyphs. 
            /// Shaping allows for implementation of ligatures, the application of Thai tones, the coalescing
            /// of base characters and diactricals into individual glyphs, the selection of proper Arabic 
            /// glyphs depending on position, etc.
            ///
            /// The input pTextRun must point to valid text if nTextRunLength is > 0.
            /// The text of input pTextRun need not be zero-terminated.
            /// The input pTextRunArray must refer to the entire paragraph of analyzed text, 
            /// even if the pAnalysisArray input is refers to a subsegment of that paragraph.
            /// The output pGlyphIndexArray must NULL or be at least as large as nTextRunLength + 1.
            /// The output pTextIndexArray must be NULL or be at least as large as nGlyphAttributesArrayCapacity + 1.
            /// The return value is the number of GlyphAttributes that the input text generates.
            /// The number of entries written to pTextIndexArray will be equal to the return value + 1.
            /// Note that the return value may be greater than nGlyphAttributesArraySize, though 
            /// no more than nGlyphAttributesArraySize entries will be written to pGlyphAttributesArray.
            ///
            /// Upon return, pGlyphIndexArray, if non-NULL, will hold an array of indexes into pGlyphArray  
            /// indicating which glyph each character in pTextRun refers to plus one-past the last character
            /// in the text run. If a character in pTextRun refers to more than one glyph (i.e. a cluster), 
            /// then the value refers to the first glyph in the cluster.
            /// It's possible that multiple input characters can refer to the same cluster. This information is
            /// useful for the implementation of cursor hit-testing and related functionality.
            ///
            /// Since shaping text often can expand text, you will normally want the pGlyphAttributesArray 
            /// to be at least two times the size of the text run referred to by the input pAnalysisArray,
            /// plus another fifteen or so entries. 
            ///
            /// Normally, shaping is done on text analyzed by AnalyzeText; that's why this function requires
            /// as input the pAnalysisArray generated by AnalyzeText. However, you can use this function on 
            /// an analysis by analysis basis. In other words, you can pass in a single Analysis generated by
            /// the AnalyzeText function and it will shape just that pTextRunArray substring into pGlyphAttributesArray.
            /// This kind of processing is useful (possibly required) for the shaping of styled text whereby
            /// one segment of text uses a different font than another.
            ///
            /// To consider: Possibly GlyphAttributes should have the GlyphId member removed and ShapeGlyphs
            ///              should take an array of GlyphId and an array of GlyphAttributes. If we provide
            ///              a stride argument for both arrays, the user could still pass these as a single
            ///              array if desired. The question here is one of how important it is to break up
            ///              the GlyphAttributes structure. Maybe it's not important at all.
            ///
            uint32_t ShapeGlyphs(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t* pGlyphIndexArray,
                               const Analysis* pAnalysisArray, uint32_t nAnalysisArraySize, 
                               GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArrayCapacity, 
                               uint32_t* pTextIndexArray = NULL);


            /// LineBreakText
            /// 
            /// Fits one or more lines of text into the given space. The size of the text is based on the
            /// glyph advance information, while the break locations are determined by standardized line
            /// breaking algorithms. Note that line breaks can occur in the middle of runs.
            /// Line breaking must be done on shaped text, as it is dependent on the displayed glyphs and
            /// not simply on the input Unicode code points (i.e. keyboard presses).
            ///
            /// The input pGlyphAttributesArray is the array of glyph information generated by ShapeGlyphs. 
            /// The input fSpace is the linear space the glyphs are to fit in. For horizontal text, 
            /// fSpace refers to the the pixel width of the line.
            /// The output pGlyphLineStartArray stores the position of the glyphs that start each line,
            /// with the first entry always being the same value as nGlyphStartIndex.
            /// The input nGlyphLineStartArraySize indicates the size of pGlyphLineStartArray and thus
            /// indicates the number of line breaks the user would like to calculate. This value is
            /// often specified as simply one, as the user often wants to break just a line at a time.
            /// The output pGlyphLineSpaceArray, if non-NULL, will contain the amount of space used by
            /// each line returned in pGlyphLineStartArray. If pGlyphLineSpaceArray is non-NULL, then it must
            /// be able to hold at least nGlyphLineStartArraySize entries.
            ///
            /// Returns the number of line breaks written to pGlyphLineStartArray. Note that this is different
            /// from some other functions in this module which return the total number of something regardless 
            /// of the user-provided buffer size. The reason for this is that the user usually cares only 
            /// about finding about the next N breaks and doesn't care if there are more breaks in the text. 
            ///
            uint32_t LineBreakText(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t nTextStartIndex, uint32_t nTextSize, const uint32_t* pGlyphIndexArray,
                                AnalysisArray* pAnalysisArray, 
                                const GlyphAttributes* pGlyphArray, const uint32_t* pTextIndexArray,
                                float fSpace, uint32_t* pTextLineEndArray, uint32_t nTextLineEndArraySize, float* pTextLineSpaceArray);


            /// OrderGlyphs
            ///
            /// Orders text based on bidirectional embedding level.
            ///
            /// Reordering is done on a line-by-line basis, and so one must pass in analysis
            /// items from just a single line. Otherwise, the ordering could be done incorrectly.
            /// The output analysis can be the same memory space as the input analysis.
            /// Note that to order text, only analysis information is needed; text and glyph
            /// information is not needed.
            /// 
            void OrderText(const Analysis* pAnalysisArrayInput, uint32_t nAnalysisArrayInputSize, Analysis* pAnalysisArrayOutput);


            /// JustifyGlyphs
            ///
            /// Modifies glyphs for justification. If the LayoutSettings specifies vertical text display instead
            /// of horizontal display, the justification occurs vertically instead of horizontally. The glyphs
            /// are modified via changes in advance widths and the addition of kashidas for arabic-related text.
            /// Note that justifying a string of glyphs is not the same as simply stretching a string of glyphs.
            /// For information on fKashidaToTextRatio, see http://www.w3.org/TR/css3-text/#text-kashida-space.
            /// Justification is done via kashida glyphs, extra spacing between words, and extra spacing between
            /// characters. Glyphs are not stretched; such stretching must be specifed as part of the text style
            /// specification.
            ///
            /// In practice, the user of this function may need to give a little helping hand in deciding 
            /// what text range should be considered for justification and in deciding how much space to add
            /// to each text run, especially if the user is working with styled text.
            ///
            void JustifyGlyphs(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, uint32_t nTextStartIndex, uint32_t nTextSize, uint32_t* pGlyphIndexArray,
                            GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArrayCapacity, uint32_t* pTextIndexArray,
                            float fSizeChange, bool bSizeChangeIsRelative);


            /// PlaceGlyphs
            ///
            /// Generates a graphics-system independent (i.e. "high level") display list for a run of glyphs.
            /// The glyphs are place in a straight line, either horizontally. The order of the glyphs on the 
            /// line is based on the input Analysis array and the bidirectional level of the individual analysis runs.
            ///
            uint32_t PlaceGlyphs(const Analysis* pAnalysisArray, uint32_t nAnalysisArraySize, const uint32_t* pGlyphIndexArray,
                               const GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArraySize, 
                               float x, float y, GlyphDisplayEntry* pGlyphDisplayEntryArray);



            ///////////////////////////////////////////////////////////////////
            // Low level metrics
            ///////////////////////////////////////////////////////////////////

            /// TextPositionToDisplayPosition
            ///
            /// Given a text position, this function tells what position on the glyph output display corresponds
            /// to it. This function is useful in implementing a caret display in a graphical text editing application.
            /// This function works on a single linear run of text. If you have multiple lines of text, you will
            /// need to break them into individual runs and test the appropriate run with this function. 
            ///
            bool TextPositionToDisplayPosition(uint32_t nTextPosition, bool bTrueIfLeadingFalseIfTrailing, 
                                                const uint32_t* pGlyphIndexArray, uint32_t nGlyphIndexArraySize, 
                                                const GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArraySize, 
                                                Point& ptDisplayPosition);

            /// DisplayPositionToTextPosition
            ///
            /// Given a coordinate in the glyph output display, this function tells what glyph index and what
            /// original text index corresponds to that position. This function is useful in doing hit-testing
            /// with a cursor over an area of displayed text on a visual output device.
            /// This function works on a single linear run of text. If you have multiple lines of text, you will
            /// need to break them into individual runs and test the appropriate run with this function. 
            ///
            bool DisplayPositionToTextPosition(const Point& ptDisplayPosition, const uint32_t* pTextIndexArray, uint32_t nTextIndexArraySize, 
                                                GlyphAttributes* pGlyphAttributesArray, uint32_t nGlyphAttributesArraySize, 
                                                uint32_t& nTextPosition, bool& bTrueIfLeadingFalseIfTrailing);


        protected:
            LayoutSettings mLayoutSettings;

        }; // class Layout


        // Frees any statically allocated Layout memory.
        EATEXT_API void ShutdownLayout();


    } // namespace Text


} // namespace EA


#endif // EATEXT_LAYOUT_ENABLED

#endif // EATEXT_EATEXTLAYOUT_H





