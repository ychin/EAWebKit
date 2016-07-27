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
// EATextTypesetter.h
//
// Maintained by Paul Pedriana and Talin.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Typesetter is the primary layout engine of EAText. 
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTTYPESETTER_H
#define EATEXT_EATEXTTYPESETTER_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextFontServer.h>
#include <EAText/EATextScript.h>
#include <EASTL/fixed_set.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>


namespace EA
{
    namespace Text
    {
        // Forward declarations
        class FontServer;
        class OTF;          // OpenType Font info


        /// index_t is the type used for all indices of characters and glyphs.
        typedef eastl_size_t index_t;

        /// Defines an index which is invalid or refers to nothing.
        const index_t kIndexInvalid = (index_t)-1;


        /// RectangleSet
        ///
        /// Defines a set of Rectangles.
        /// Used for describing glyph areas.
        ///
        typedef eastl::fixed_set<Rectangle, 4, true> RectangleSet;


        /// Span
        ///
        /// Defines a horizontal or vertical spatial span.
        ///
        struct Span
        {
            float mfBegin;
            float mfEnd;
        };


        /// LayoutSettings
        ///
        /// During layout of text, we need to have contextual information to tell us how to lay out 
        /// the text. For example, different languages/locales have different word-wrapping policies.
        /// 
        struct EATEXT_API LayoutSettings
        {
            // Environment
            Char              mLocale[6];               /// Example: "en-us". Default value is EATEXT_DEFAULT_LOCALE. An empty string sets the locale to be the default value (EATEXT_DEFAULT_LOCALE). 
            Script            mScript;                  /// The primary script of the document or paragraph. Set to kScriptUnknown if you want the script to be based on the locale. Default value is kScriptUnknown.
            Direction         mDirection;               /// The base direction for text layout. Set to kDirectionDefault if you want the direction to be based on the Script. Default value is kDirectionDefault.
            FontSelection     mFontSelection;           /// An explicit set of fonts to be used for the layout (typesetting). The Typesetter class can use this if it is non-empty or will use the FontServer to dynamically choose fonts.
            bool              mbDynamicFontSelection;   /// True if the font selection is dynamic and the layout engine should use any font to select glyphs. False if the only fonts used should come from mFontSelection. Default value is true.

            // Style directives
            TextStyle         mTextStyleDefault;        /// **This field is deprecated; use TypeSetter::GetDefaultTextStyle instead.** Default style to use if one is not specified.

            // Layout/Shaping directives not already represented in mTextStyleDefault.
            // To consider: Move these directives to TextStyle.
            bool              mbEnableAutoHyphenation;  /// Use automatic hyphenation (if such functionality is available). Default is false.
            bool              mbCellTopPosition;        /// If true, then the y coordinate origin is assumed to be at the top of the text line box instead of at the baseline of the text. Default is true.
            float             mfYScale;                 /// Equal to +1 or -1. +1 is for coordinate systems with positive y being downward, with -1 being coordinate systems being y upward. Default is +1 (y-down).
            bool              mbEnableLigatures;        /// If true, then ligatures are chosen when available. Default is false.
            bool              mbHideControlCharacters;  /// If true, we treat control characters as zero-width non-breaking space. Default is false.
            bool              mbDisplayZWG;             /// True if zero width characters should be shaped to a visible glyph with width. Default is false.

            LayoutSettings();
            LayoutSettings(const LayoutSettings& x);
            LayoutSettings& operator=(const LayoutSettings& x);
        };

        /// GetDefaultLayoutSettings
        ///
        /// Gets a reference to the global default layout settings.
        ///
        EATEXT_API LayoutSettings& GetDefaultLayoutSettings();


        /// AnalysisInfo
        ///
        /// Describes pertinent information about each character in laid out text.
        ///
        struct EATEXT_API AnalysisInfo // 8 bytes
        {
            const TextStyle* mpTextStyle;             /// The style used by the current character.
            Font*            mpFont;                  /// The Font that is targeted towards this Char/TextStyle/Script combination.
            Script           mScript           : 7;   /// One of enum Script. For runs that have multiple scripts present, this is the primary script.
            BidiClass        mBidiClass        : 5;   /// One of enum BidiClass.
            int              mnBidiLevel       : 4;   /// Bidirectional embedding level. Range of [0, 15]. Odd numbers are left to right, even are right to left.
          //unsigned         mnParity          : 1;   /// mnBidiLevel level parity. 0 for LTR, 1 for RTL.
          //unsigned         mnParentParity    : 1;   /// mnBidiLevel level parity. 0 for LTR, 1 for RTL. Parent partity is not necessarily the opposite of parity, due to, for example, RLE being forcibly embedded within already RLE text.
          //unsigned         mbForcedDirection : 1;   /// True if this char's direction was set by an explicit direction override (LRO or RLO).
            unsigned         mbJoinBefore      : 1;   /// There is a joiner before this item (run) such as kCharZWNJ (zero-width nonjoiner) or kCharZWJ.
            unsigned         mbJoinAfter       : 1;   /// There is a joiner after this item (run) such as kCharZWNJ or kCharZWJ.
        };


        /// GlyphSide
        ///
        /// Defines which side or half of a glyph to refer to in functions
        /// that use this information. For example, if you hit-text a glyph
        /// you may want to know which side of the glyph you hit so that you
        /// know where to place a text editor caret.
        ///
        enum GlyphSide
        {
            kGlyphSideBefore, /// For LTR text, this is to the left of the character. For RTL text it is to the right.
            kGlyphSideAfter   /// For LTR text, this is to the right of the character. For RTL text it is to the left.
        };


        /// PickType
        ///
        /// Used by the GetTextPositionFromDisplayPosition function. 
        ///
        enum PickType
        {
            kPickTypeInsertion,  /// Used for doing character range highlighting and insertion. When you pick the right side of a glyph, the next glyph is selected.
            kPickTypeCharacter   /// Used for hit-testing individual characters. Anywhere you pick on a glyph, that glyph is selected.
        };


        /// GlyphInfo
        ///
        /// Glyph information which is layout-independent but is used to lay out the glyph.
        ///
        struct EATEXT_API GlyphInfo  // 4 bytes
        {
            GlyphJustificationClass mGJC              : 4;   /// Justification classification
            unsigned                mbGlyphIsObject   : 1;   /// True if the glyph is a user-supplied inline object instead of a font glyph.
            unsigned                mClusterPosition  : 3;   /// Zero-based position of this glyph within its cluster. Standalone glyphs have a value of 0.
            unsigned                mClusterSize      : 3;   /// Size of the cluster this glyph is in. Standalone glyphs have a value of 1.
            unsigned                mClusterBreakable : 1;   /// True if this cluster (sequence of glyphs) can be broken up in a text editor. Else deleting one component of the cluster results in deleting the others.
            unsigned                mCharCount        : 2;   /// Number of Chars that this glyph maps to, in the case of ligatures and composed glyphs. This value is >= 1, and as of this writing, if this value > 1 then (mClusterSize == 1) and (mClusterPosition == 0).
            unsigned                mDirection        : 1;   /// Same as enum Direction. Either kDirectionLTR or kDirectionRTL.
          //unsigned                mbDiacritic       : 1;   /// True if this character is a diacritic of any type.
          //unsigned                mbInvisible       : 1;   /// True if this character has nothing to it (e.g. space, tab, zero-width).
          //unsigned                mbZeroWidth       : 1;   /// ZWSP, ZWNBSP, ZWJ, ZWNJ, or anything with no width (see IsCharZeroWidth()). If this is true, then mbInvisible will be true also.
            uint8_t                 mScriptFlags;            /// Flags used by some shaping engines, values depend on the script/engine. For example, see ArabicGlyphTypeFlag. May be unused.
            uint8_t                 mOpenTypeLookupFlags;    /// Flags assigned by OpenType to glyphs and used for Gsub and Gpos. See OTFLookupFlagBit. May be unused.
        };


        /// GlyphLayoutInfo
        ///
        /// Provides information for an individual glyph that is laid out or in a display list.
        ///
        /// To consider: Find a way to compress this data, perhaps by reducing the font pointer 
        /// to a 16 bit font id and using some form of compression on the floating point
        /// coordinate values, perhaps to 12.4 fixed point. This struct could be packed
        /// down to 20 bytes or less. Note that it is not terrible that this struct be
        /// so big, as it is usually a temporary intermediate struct and there usually
        /// aren't any more than a hundred or so of them in an array.
        ///
        struct EATEXT_API GlyphLayoutInfo  // 32 bytes
        {
            union {
                Font*   mpFont;         /// Font. If this is stored on disk as a persisted object, it holds the numerical unique font id. Ideally this member would be of type uintptr_t, but that would make it less readable.
                void*   mpObject;       /// Object info; applicable to when the user supplies a generic object to layout instead of a Unicode character.
            };

            float       mfPenX;         /// Pen position, used for display lists of laid out text. When GlyphAttributes applies to an individual glyph, this is zero. This field is more or less required for fonts which are not based on bitmaps, such as polygonal fonts or vector fonts. The pen position is the local coordinate origin for the glyph.
            float       mfPenY;         /// When GlyphLayoutInfo applies to an individual glyph, this is zero.
            float       mfAdvance;      /// Distance along x (horizontal text) or y (vertical text) to next pen position. This value does not include kerning that might be applied which alters the actual next pen position used.
            float       mfX1;           /// Same as x bearing: x position of top-left corner. For individual glyphs the position is relative to the pen position; for laid out display lists it is in absolute coordinates.
            float       mfY1;           /// Same as y bearing: y position of top-left corner.
            float       mfX2;           /// X position of bottom-right corner. We use x2 instead of width  because the former maps to what is done with the value better.
            float       mfY2;           /// Y position of bottom-right corner. We use y2 instead of height because the former maps to what is done with the value better.
        };


        /// LineMetrics
        ///
        /// Defines metrics of laid out text. These are useful for knowing the space used by 
        /// laid out text. These layout metrics are coordinate-system-orientation-independent.
        /// Thus, the mfBaseline value for a char like 'A' is always positive for both y-down
        /// and y-up coordinate systems, and mfDescent for a char like 'g' is always negative.
        /// The height of a line is (mfBaseline - mfDescent)
        ///
        struct EATEXT_API LineMetrics
        {
            index_t  mnLineLength;          /// Length of a given line in Unicode Chars.
            index_t  mnVisibleLineLength;   /// Length of a given line in Unicode Chars, discounting trailing spaces (as defined by IsSpace(c, kSTAll)).
            float    mfSpace;               /// Space used by laid out Unicode characters, defined by final pen position. For line layout, this is the end of the line; for paragraph layout, this is the height of the paragraph for horizontal layout or the width of the paragraph for vertical layout.
            float    mfVisibleSpace;        /// Space used by the mnVisibleSize characters, defined by pen position of first trailing invisible character. For paragraph layout, this will be equal to mfSpace.
            float    mfBaseline;            /// Equal to max baseline value for all fonts represented on the line. The entire line height is the baseline minus (because descent is negative) the descent.
            float    mfDescent;             /// Equal to min descent for all fonts represented on the line. Recall that lower descents mean lower values.
        };


        /// LineLayout
        ///
        /// Contains layout data for the current line. 
        /// The Typesetter class lays out lines and writes their data into an 
        /// instance of LineLayout. LineLayout is the fundamental layout unit.
        ///
        class EATEXT_API LineLayout
        {
        public:
            enum Component
            {
                kComponentCharArray            =   1,
                kComponentCharAnalysis         =   2,
                kComponentGlyphArray           =   4,
                kComponentGlyphInfoArray       =   8,
                kComponentGlyphLayoutInfoArray =  16,
                kComponentGlyphIndexArray      =  32,
                kComponentCharIndexArray       =  64,
                kComponentAll                  = 127
            };

            LineLayout(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            LineLayout(const LineLayout&);

            /// SetAllocator
            ///
            /// Sets the memory allocator used by this class. By default the EAText allocator
            /// is used. You can use this function to set a specific allocator used just
            /// by this class. This function must be called before this class is used.
            ///
            void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

            /// Resets the LineLayout to a newly initialized state.
            /// Used to draw a new paragraph. To consider: rename this NewParagraph.
            void Clear(bool bThisArgumentIsDeprecated = true);

            /// Rests the LineLayout to a state ready for a new line (but not the
            /// new paragraph).
            void NewLine();

            /// Resets the LineLayout to a state ready to draw new glyphs on the 
            /// current line (but not to draw a new line or new paragraph).
            void NewGlyphs();

            /// Assigns a LineLayout to this LineLayout.
            void Assign(const LineLayout& lineLayout, int componentFlags = kComponentAll);

            /// Appends a LineLayout to this LineLayout.
            /// This function does not increment the mLineCount member variable,
            /// as it isn't necessarily so that the appended layout represents an 
            /// independent line.
            void Append(const LineLayout& lineLayout, int componentFlags = kComponentAll);

            /// Converts a Char index to its corresponding Glyph index.
            /// This is useful for telling what glyphs correspond to what Chars,
            /// as there isn't a 1:1 correspondence between the two in many circumstances.
            index_t GetGlyphIndexFromCharIndex(index_t iChar) const;

            /// Converts a Glyph index to its corresponding Char index.
            index_t GetCharIndexFromGlyphIndex(index_t iGlyph) const;

            /// Converts a Char range in the current LineLayout to its corresponding GlyphId range.
            void GetGlyphRangeFromCharRange(index_t  iCharBegin,  index_t  iCharEnd, 
                                            index_t& iGlyphBegin, index_t& iGlyphEnd) const;

            /// Converts a GlyphId range in the current LineLayout to its corresponding Char range.
            void GetCharRangeFromGlyphRange(index_t  iGlyphBegin, index_t  iGlyphEnd, 
                                            index_t& iCharBegin,  index_t& iCharEnd) const;

            /// Offsets a segment of glyphs. See Offset for more info.
            void OffsetSegment(float fDeltaX, float fDeltaY, index_t iGlyphBegin, index_t iGlyphEnd);

            /// Offsets the layout by the given x and y delta.
            /// All coordinates are added to x and y. This is useful for aligning a layout
            /// to the final destination after the metrics of the layout are calculated.
            /// You may not need this function if you can set the pen position to the desired
            /// location before layout.
            void Offset(float fDeltaX, float fDeltaY)
                { OffsetSegment(fDeltaX, fDeltaY, 0, mGlyphLayoutInfoArray.size()); }

            /// Calculates LineMetrics::mnVisibleSpace and LineMetrics::mnVisibleLineLength
            /// if not calculated already. Normally these are calculated automatically at
            /// the end of a line layout.
            /// If the LineLayout was generated via LayoutSimple then it will need to have
            /// be called with the kComponentCharArray flag in addition to the default flags.
            /// Otherwise CalculateVisibleSpace may fail to work properly.
            void CalculateVisibleSpace();

            /// Given a coordinate in the glyph output display, this function tells 
            /// what original text index corresponds to that position. This function is 
            /// useful in doing hit-testing with a cursor over an area of displayed text 
            /// on a visual output device. This function applies to the currently laid
            /// out line of text or the input pLineLayout and pAnalysisInfo if these
            /// are non-NULL. 
            /// If bRequireGlyphHit is true, then kIndexInvalid will be returned when the 
            /// position is not within the visible glyph range (e.g. it was to the left or
            /// right of the line). bRequireGlyphHit does not require the input position to
            /// be on solid glyph pixels, but merely requires it to be within the glyph box.
            /// This function does not work for a LineLayout with multiple lines,
            /// for efficiency reasons.
            index_t GetTextPositionFromDisplayPosition(float xPosition, float yPosition, 
                                                       bool bRequireGlyphHit, PickType pickType, 
                                                       GlyphSide& glyphSide, Direction layoutDirection) const;

            /// Given a text position, this function tells what position on the glyph 
            /// output display corresponds to it. This function is useful in implementing 
            /// a caret display in a graphical text editing application. This function 
            /// applies to the currently laid out line of text or the input pLineLayout 
            /// and pAnalysisInfo if these are non-NULL.
            /// This function does not work for a LineLayout with multiple lines,
            /// for efficiency reasons.
            void GetDisplayPositionFromTextPosition(index_t nTextPosition, GlyphSide glyphSide,
                                                    Point& ptDisplayPosition, Direction layoutDirection) const;

            /// Given a text position, this function tells what position on the glyph 
            /// output display corresponds to it. This function is useful in implementing 
            /// a caret display in a graphical text editing application. This function 
            /// applies to the currently laid out line of text or the input pLineLayout 
            /// and pAnalysisInfo if these are non-NULL.
            /// This function does not work for a LineLayout with multiple lines,
            /// for efficiency reasons.
            /// (This is the same as the above function, except it only returns an X coordinate.)
            float GetDisplayPositionFromTextPosition(index_t nTextPosition, GlyphSide glyphSide, Direction layoutDirection) const;

            /// Yields one or more rectangles corresponding to a selected segment
            /// of characters in a line. This function is useful for implementing the
            /// highlighting of segments of text in a text editor or display. Note that 
            /// the output consists of possibly more than one rectangle. This is due to 
            /// the possibility of bidirectional text causing visual discontinuities
            /// in selected ranges of text.
            /// The returned RectangleSet returns y values (rect.mTop, rect.mBottom) which
            /// are normalized to relative to a y=0 baseline, even if the glyph y positions
            /// are nowhere near zero. If pLayoutSettings is specified and pLayoutSettings->mbCellTopPosition
            /// is true, then the values are normalized to a y=0 text top instead.
            /// The user can pass in a vertical offset value which is applied to all rectangles.
            /// This vertical offset is useful for yielding rectangles that are in the right 
            /// y space for your display. 
            /// This function does not work for a LineLayout with multiple lines,
            /// for efficiency reasons.
            eastl_size_t GetGlyphSelection(index_t iCharBegin, index_t iCharEnd, RectangleSet& rectangleSet, 
                            LayoutSettings* pLayoutSettings = NULL, float fVerticalOffset = 0.f) const;

            /// Gets underline, strike-through, overline information for the given
            /// selected range of text. Note that only a single DecorationFlag type
            /// may be used per call, for efficiency reasons. You can always call this
            /// function multiple types with different DecorationFlags values.
            eastl_size_t GetGlyphDecoration(DecorationFlags decoration, index_t iCharBegin, index_t iCharEnd, 
                            RectangleSet& rectangleSet, LayoutSettings* pLayoutSettings = NULL, float fVerticalOffset = 0.f) const;

            /// Retrieves a unique set of Fonts used by this LineLayout.
            /// This FontSelection could be used with the GetMaxFontMetrics function
            /// to tell you the min/max height values for the line. 
            /// This function adds fonts to the supplied FontSelection; it does not 
            /// clear the FontSelection before adding any fonts.
            void GetFontSelection(FontSelection& fontSelection) const;

            /// Validates that our data structures are sane.
            /// This function is linked away when not used, as would be the case in a release build.
            bool Validate(bool bValidateSync = true);

            /// Compute the height of the current line (not including leading). Put here because the
            /// calculation should be done in a standard way, since it is done so frequently.
            float GetLineHeight() const;

            /// Gets the width and height of the LineLayout. 
            /// If bVisible is true then the horizontal extents are determined by including
            /// only visible glyphs and not whitespace or otherwise invisible glyphs.
            void GetBoundingBox(float& w, float& h, bool bVisible);

        public:
            /// Currently we are using simple eastl::vectors. We'll revise this in
            /// the future to be more friendly to controlled memory situations.
            /// Switching these to eastl::deque containers would allow for dynamic
            /// memory allocation from a user-supplied pool to be friendly.
            typedef eastl::basic_string<Char,            EA::Allocator::EASTLICoreAllocator>  CharArray;
            typedef eastl::vector      <AnalysisInfo,    EA::Allocator::EASTLICoreAllocator>  AnalysisInfoArray;
            typedef eastl::vector      <GlyphId,         EA::Allocator::EASTLICoreAllocator>  GlyphArray;
            typedef eastl::vector      <GlyphInfo,       EA::Allocator::EASTLICoreAllocator>  GlyphInfoArray;
            typedef eastl::vector      <GlyphLayoutInfo, EA::Allocator::EASTLICoreAllocator>  GlyphLayoutInfoArray;
            typedef eastl::vector      <index_t,         EA::Allocator::EASTLICoreAllocator>  IndexArray;


            Allocator::ICoreAllocator*  mpCoreAllocator;        /// Memory allocator. Defaults to global EAText allocator.
            CharArray                   mCharArray;             /// Specifies the Chars that are laid out.
            AnalysisInfoArray           mAnalysisInfoArray;     /// Specifies information about the Chars in mCharArray. mAnalysisInfoArray.size() == mCharArray.size().
            GlyphArray                  mGlyphArray;            /// Specifies the GlyphIds of all glyphs that are laid out, based on the mCharArray values and the Font.
            GlyphInfoArray              mGlyphInfoArray;        /// Specifies information about a glyph that is unrelated to the drawing of the glyph.
            GlyphLayoutInfoArray        mGlyphLayoutInfoArray;  /// Specifies information about how each glyph is drawn.
            IndexArray                  mGlyphIndexArray;       /// Specifies the glyph index corresponding to a given char index. mGlyphIndexArray.size() == mCharArray.size().
            IndexArray                  mCharIndexArray;        /// Specifies the char index corresponding to a given glyph index. mCharIndexArray.size() === mGlyphArray.size().
            LineMetrics                 mLineMetrics;           /// The LineMetrics for the line.
            eastl_size_t                mLineCount;             /// Count of the number of lines in this layout. Usually the value is 1, but it can be more if a higher level paragraph layout function is used.
            index_t                     mCharScheduleIndex;     /// Which char in the schedule does this layout begin with. This is useful for telling where in the paragraph the current line is.
            bool                        mbSimpleLayout;         /// If true, then the layout represents simple layout whereby only GlyphArray and GlyphLayoutInfoArray are valid and the other data is not used and thus not sync'd with it.

        }; // class LineLayout




        /// LineLayoutIterator
        ///
        /// Iterates a LineLayout object and returns segments of contiguous styles to 
        /// the user. If you are rendering lines of text of varying style or with embedded
        /// objects, this iterator will allow you to render all contiguous spans at once.
        ///
        class EATEXT_API LineLayoutIterator
        {
        public:
            LineLayoutIterator(const EA::Text::LineLayout* pLineLayout);

            void Reset(const EA::Text::LineLayout* pLineLayout = NULL);
            bool GetNextTextStyleSegment(eastl_size_t& nSegmentBegin, eastl_size_t& nSegmentEnd);
            bool GetNextObject(eastl_size_t& nObjectPosition);

        protected:
            const EA::Text::LineLayout* mpLineLayout;
            eastl_size_t                mNextIndex;
        };



        /// GetMaxFontMetrics
        ///
        /// Given a selection of Fonts, this function tells what the maximum baseline
        /// value is and the minimum descent value is for the font selection. 
        /// The height of the layout text box would simply be fMaxBaseline - fMaxDescent.
        /// Recall that 'baseline' is defined as the distance from the top of the layout 
        /// box to the baseline of laid out text; it is like (max ascent + leading). 
        /// Recall that descent values are negative as the drop below the baseline.
        /// 
        /// Note that if you are fitting layout to a pixel grid, you probably want to convert
        /// the resulting fMaxBaseline to ceil(fMaxBaseline) and fMaxDescent to floor(fMaxDescent)
        /// and calculate the line height as ceil - floor.
        ///
        EATEXT_API void GetMaxFontMetrics(const FontSelection& fontSelection, float& fMaxBaseline, float& fMaxDescent);



        /// LayoutSimple
        ///
        /// Does basic but fast layout of a string of text with a single predetermined font.
        /// Layout is done on a single line. No complex functionality such as shaping
        /// and bidirectionality is supported. Each Unicode input character is simply
        /// directly converted to its associated glyph and placed in the output list
        /// along with appropriate pen advance information. The only special character 
        /// is the '\n' character which does what you would expect. 
        /// Input text can be any Unicode text.
        /// Returns the resulting glyph count, which will usually be equal to nTextSize,
        /// though will be less if '\n' characters are present.
        ///
        /// Example usage:
        ///    GlyphId         glyphIdArray[32];
        ///    GlyphLayoutInfo glyphLayoutInfoArray[32];
        ///
        ///    index_t glyphCount = LayoutSimple(L"Hello World", strlen("Hello World"), 0, 0, 
        ///                                       pSomeFont, glyphIdArray, pGlyphLayoutInfo);
        ///
        EATEXT_API index_t LayoutSimple(const Char* pText, index_t nTextSize, float x, float y, 
                                         Font* pFont, GlyphId* pGlyphIdArray, GlyphLayoutInfo* pGlyphLayoutInfo, 
                                         LayoutSettings* pLayoutSettings = NULL, LineMetrics* pLineMetrics = NULL, index_t* pLineCount = NULL);

        /// In this version of LayoutSimple, the text is layed out to the given lineLayout.
        /// The user can supply a pLayoutSetting to use or NULL for default.
        /// The componentFlags specifies the extent of the information written to
        /// the lineLayout. By default only the final glyph information is written.
        /// However, the user can also specify that the char array information is 
        /// written via the kComponentCharArray flag, or that additionally the 
        /// glyph/char mapping is written via the kComponentGlyphIndexArray flag.
        /// If you will be calling metrics functions such as CalculateVisibleSpace
        /// on the results of this function then you will need to at least specify
        /// the kComponentCharArray flag in addition to the default.
        EATEXT_API index_t LayoutSimple(const Char* pText, index_t nTextSize, float x, float y, 
                                         Font* pFont, LineLayout& lineLayout, 
                                         LayoutSettings* pLayoutSettings = NULL, 
                                         uint32_t componentFlags = LineLayout::kComponentGlyphArray | LineLayout::kComponentGlyphLayoutInfoArray);

        /// LayoutSimple
        ///
        /// This is an 8 bit per character version of LayoutSimple. Characters are still
        /// interpreted as Unicode, but only Unicode values from 0-255 are recognized.
        /// Note that such Unicode values are the Latin-1 character set (a.k.a. ISO 8559-1).
        /// The text is not treated as UTF8 Unicode; the 16 bit version of this function
        /// can be used to display Unicode text.
        ///
        EATEXT_API index_t LayoutSimple(const char8_t* pText, index_t nTextSize, float x, float y, 
                                         Font* pFont, GlyphId* pGlyphIdArray, GlyphLayoutInfo* pGlyphLayoutInfo, 
                                         LayoutSettings* pLayoutSettings = NULL, LineMetrics* pLineMetrics = NULL, index_t* pLineCount = NULL);

        /// See the 16 bit version for detailed documentation.
        EATEXT_API index_t LayoutSimple(const char8_t* pText, index_t nTextSize, float x, float y, 
                                         Font* pFont, LineLayout& lineLayout, 
                                         LayoutSettings* pLayoutSettings = NULL, 
                                         uint32_t componentFlags = LineLayout::kComponentGlyphArray | LineLayout::kComponentGlyphLayoutInfoArray);


        /// Typesetter
        ///
        /// This is the primary text layout interface for EAText.
        /// It supercedes the previous layout system present in EATextLayout.
        ///
        /// Typesetter works by taking a list of text items and fitting them into 
        ///
        /// Example usage:
        ///    Typesetter ts;
        ///    Typesetter::LineState lineState;
        ///    
        ///    ts.SetLayoutSpace(0, 0, 0);
        ///    ts.AddTextRun(pString, nStringLength);
        ///    
        ///    for(lineState  = ts.GetLineState(); 
        ///        lineState != Typesetter::kLineStateComplete; 
        ///        lineState  = ts.GetLineState())
        ///    {
        ///        // You can call ts.AddTextRun anywhere within this body to add more text.
        ///        // You can change the layout box anwhere within this body, usually after calling FinalizeLine.
        ///        ts.FinalizeLine();
        ///        // Do something with ts.GetLineLayout() here.
        ///        ts.NextLine();
        ///    }
        ///
        class EATEXT_API Typesetter
        {
        public:
            /// Default constructor
            Typesetter(Allocator::ICoreAllocator* pCoreAllocator = NULL);

            /// Destructor.
            ~Typesetter();

            /// SetAllocator
            ///
            /// Sets the memory allocator used by this class. By default the EAText allocator
            /// is used. You can use this function to set a specific allocator used just
            /// by this class. This function must be called before this class is used.
            ///
            void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

        public:
            ///////////////////////////////////////////////////////////////////
            /// High level layout functionality
            ///////////////////////////////////////////////////////////////////

            /// LayoutLine
            ///
            /// Draws a string of left-aligned text on the specified destination 
            /// at the specified location. The (x, y) location position refers 
            /// to the pen position and not the top-left or bottom-left of the 
            /// character cell. 
            ///
            /// Returns the number of glyphs generated.
            ///
            /// This function is roughly the equivalent of the following:
            ///     ts.SetLayoutSpace(100000000.f, 0, 0);
            ///     ts.SetDefaultTextStyle(pTextStyle);
            ///     ts.AddTextRun(pText, nTextSize);
            ///     ts.FinalizeLine();
            ///
            /// Example usage:
            ///    Typesetter ts;
            ///
            ///    ts.LayoutTextLine(pString, nStringLength, 100, 0, pSomeTextStyle);
            ///    // Do something with ts.GetLineLayout() here.
            ///
            index_t LayoutLine(const Char* pText, index_t nTextSize, float x, float y, const TextStyle* pTextStyle);


            /// LayoutParagraph
            ///
            /// Draws a potentially multi-line paragraph of a single style within the given rectangle. 
            /// The text is drawn -- with wrapping -- within the rectangle until the first character 
            /// which is completely outside the rectangle. The horizontal and vertical alignment directives 
            /// of the Typesetter LayoutSettings are adhered to.
            /// 
            /// If the input rectangle coordinates have a height <= 1.0 (for horizontal layout)
            /// then this function acts much the same as the DrawTextLine function, as it will 
            /// always run out of room at the end of the first line.
            ///
            /// This function appends all the lines to the user-supplied LineLayout object.
            /// Normally Typesetter writes lines one by one and expects the user to process the
            /// individual lines as they are finalized. This function internally takes those
            /// individual lines and appends them to the user-supplied LineLayout object.
            ///
            /// Returns the number of glyphs generated.
            ///
            /// See LayoutTextLine for more information about the parameters and functionality.
            ///
            /// Example usage:
            ///    Typesetter ts;
            ///    LineLayout lineLayout;
            ///
            ///    ts.LayoutParagraph(pString, nStringLength, 0, 0, 100, 100, pSomeTextStyle, lineLayout);
            ///    // Do something with lineLayout here.
            ///
            index_t LayoutParagraph(const Char* pText, index_t nTextSize, 
                                    float fRectLeft, float fRectTop, float fRectRight, float fRectBottom, 
                                    const TextStyle* pTextStyle, LineLayout& lineLayout, int componentFlags = LineLayout::kComponentAll);

            /// GetParagraphSize
            ///
            /// Calculates the width and height of a single-line or multi-line paragraph, using the
            /// supplied text run and text style.
            ///
            /// The optional fLayoutWidth parameter is used to select between line-layout and
            /// paragraph-layout. A value <= 0 indicates that the text should be laid out on a
            /// single line; otherwise the text will be word-wrapped using fLayoutWidth as the
            /// width between the margins.
            ///
            /// Example usage:
            ///    Typesetter ts;
            ///    Point2DFloat ts.GetParagraphSize(pString, nStringLength, 100, pSomeTextStyle);
            ///
            Point GetParagraphSize(const Char* pText, index_t nTextSize, const TextStyle* pTextStyle, float fLayoutWidth = 0);

        public:
            ///////////////////////////////////////////////////////////////////
            /// Low level layout functionality
            ///////////////////////////////////////////////////////////////////

            /// Returns the current LayoutSettings.
            LayoutSettings& GetLayoutSettings();

            /// Returns the current LayoutSettings (const version).
            const LayoutSettings& GetLayoutSettings() const;

            /// Sets the current LayoutSettings.
            /// LayoutSettings should stay constant at least for each paragraph and 
            /// in most cases for an entirely contiguous document. A web page may 
            /// consist of different 'documents' (be they frames or table cells).
            /// It's important that you set LayoutSettings' mLocale member as per 
            /// the current application locale, in particular if you want correct
            /// bidirectional layout behaviour.
            void SetLayoutSettings(LayoutSettings& layoutSettings);

            /// The space is the horizontal space used to fit the current line.
            /// For vertical text, it refers to vertical space.
            /// This function takes effect immediately for line layout that is 
            /// kLineStatePartial, but otherwise takes effect upon laying out the
            /// next line.
            /// For a paragraph with a base direction of RTL (right to left), you may
            /// want to set fPenX to the right side of the space you are using.
            /// The default initial values for space, pen x, and pen y are 0, 0, 0.
            void SetLayoutSpace(float fSpace, float fPenX, float fPenY);
            void GetLayoutSpace(float& fSpace, float& fPenX, float& fPenY) const;

            /// Sets the font server to use for font style lookups.
            /// If no font server is set, the default global font server will be 
            /// used for font lookups.
            void SetFontServer(FontServer* pFontServer);

            /// Sets the default TextStyle to use for future layout. 
            /// This setting takes effect immediately.
            void SetDefaultTextStyle(const TextStyle* pDefaultTextStyle);

            /// Returns the default TextStyle.
            const TextStyle& GetDefaultTextStyle() const;

            /// Reset the Typesetter. Called at the beginning of each
            /// paragraph. You should call this before laying out a new paragraph.
            /// the bClearSchedule argument specifies whether you want to start
            /// a new paragraph (bClearSchedule = true) or if you simply want to
            /// reset the schedule for the existing line in the current paragraph
            /// (bClearSchedule = false). You shouldn't need to call Reset(false)
            /// unless you need to change the schedule for the current line.
            void Reset(bool bClearSchedule = true);

            /// Add a text run to the layout with the current style.
            void AddTextRun(const Char* pText, index_t nTextLength);

            /// Add a non-text object to the layout. 
            /// For Unicode purposes, the object is treated as the Unicode
            /// object replacement character (0xFFFC). However, we use the 
            /// GlyphMetrics arguments of this function for the object as opposed
            /// to GlyphMetrics for character 0xFFFC present in the text font.
            ///
            /// The bBindPrev and bBindNext arguments specify whether the object
            /// should be bound to the previous and next Unicode characters for 
            /// the purpose of word and line breaking. A value of true results in
            /// the same behaviour as if the object had a ZWNBSP (zero-width, non-
            /// breaking space) between it and the previous/next character.
            void AddObject(void* pObject, GlyphMetrics& glyphMetrics, bool bBindPrev, bool bBindNext);

            // Possible function to add explicit embedded direction instructions 
            // as per the CSS 'direction' and 'unicode-bidi' style elements. 
            // This can also be accomplished by adding one of the kCharLRE,
            // kCharRLE, kCharLRO, kCharRLO, or kCharPDF Unicode chars
            // void AddBidiDirection(Direction direction, bool bEmbed, bool bAdd);

            /// Add an explicit line break.
            /// Adding a paragraph break has the same action as adding the 
            /// Unicode line separator character (0x2028) as text.
            void AddLineBreak();

            /// Tell the typesetter we have come to the end of the paragraph.
            /// Note that this does not mean the end of the layout process, 
            /// as there may be unprocessed items remaining in the schedule.
            /// Adding a paragraph break has the same action as adding the 
            /// Unicode paragraph separator character (0x2029) as text.
            void AddParagraphBreak();

            /// Add a style change to the schedule.
            void AddStyleChange(const TextStyle* pStyle);

            /// Returns the current character index of the end of the last schedule
            /// item.
            index_t GetEndCharIndex() const;

            /// Defines the state of the current line layout.
            enum LineState
            {
                kLineStatePartial = 0,  /// The text doesn't have enough chars to fill the line.
                kLineStateFull,         /// The text has enough chars to fill the line.
                kLineStateComplete      /// There is no more text.
            };

            /// Returns the current line layout state.
            LineState GetLineState() const;

            /// ProcessLine the current line layout state, based on the current layout schedule.
            LineState ProcessLine();

            /// Does final positioning of the characters/glyphs determined to be 
            /// on the current line. This includes but isn't necessarily limited to
            /// bidirectional re-ordering and justification.
            /// Note that this can be called on a line that isn't full and in fact needs
            /// to be called on a line that isn't full when the user has no more text
            /// to supply.
            /// This function always sets the line state to kLineStateFull.
            void FinalizeLine();

            /// Tells the typesetter to finish the start laying out the next line.
            /// The existing LineLayout and LineState are cleared by this function.
            /// This function sets the LineState to kLineStatePartial if there is any more
            /// text in the schedule and kLineStateComplete if there is nothing else
            /// in the schedule.
            /// If the bAdvanceYPosition parameter is set to 'true', then the current
            /// line position will be advanced by the height of the current line 
            /// (i.e. the line that was just previously laid out.)
            /// If a custom TextStyle was set via SetDefaultTextStyle and its mfLineSpace
            /// is greater than zero, that line height will be used instead and no regard
            /// for the height of the characters will be made.
            void NextLine(bool bAdvanceYPosition = false);

            /// Tells the typesetter to let the current line overflow the layout boundaries
            /// and replace the trailing parts with an ellipsis. If the current line
            /// entirely fits within the layout space, no ellipsis is added.
            /// The user would call this function if the TextStyle is set to kTOEllipsis
            /// and the layout of the current paragraph is about to run out of space.
            /// This function applies only to the current line, and must be called
            /// before calling ProcessLine or FinalizeLine for the current line. 
            void EllipsizeLine(bool bEnable);

            /// Retrieves a reference to the current LineLayout.
            /// This LineLayout is usually only usable if FinalizeLine was called.
            /// This object can be saved for later use.
            LineLayout& GetLineLayout();

        protected:
            enum ItemType 
            {
                kItemTypeText = 0,          /// A literal text item.
                kItemTypeObject,            /// A non-text object.
                kItemTypeLineBreak,         /// A line break.
                kItemTypeParagraphBreak,    /// A paragraph break.
            };

            struct Text
            {
                const Char* mpCharData; /// Character data (text node only).
            };

            struct Object
            {
                void*        mpObjectPtr;     /// Object to render (object node only)
                GlyphMetrics mGlyphMetrics;   /// User-supplied glygh metrics.
                bool         mbBindPrev;      /// Object binds to the previous Unicode character.
                bool         mbBindNext;      /// Object binds to the next Unicode character.
            };

            struct LineBreak
            {
                // Empty
            };

            struct StyleChangeData
            {
                TextStyle* mpStyle;         /// Text style pointer
            };

            /// Item is a POD struct which stores information about a text run, 
            /// style change, or non-text object in the paragraph.
            struct Item 
            {
                ItemType mType;      /// Item type
                index_t  mIndex;     /// Index within paragraph of the Char this Item refers to.
                index_t  mCount;     /// Count if Chars this Item refers to.
                const TextStyle* mpStyle;  /// Text style pointer

                union 
                {
                    Text              mText;
                    Object            mObject;
                    LineBreak         mLineBreak;
                };
            };

            /// Array of items for the paragraph.
            /// Currently we are using a simple eastl::vector. We'll revise this in
            /// the future to be more friendly to controlled memory situations.
            typedef eastl::vector<Item, EA::Allocator::EASTLICoreAllocator> Schedule;


            /// Represents a schedule location. ScheduleLocation is an iterator
            /// into a container of schedule Items. For a vector of Items (i.e. a Schedule), 
            /// a ScheduleLocation refers to a specific location within that Schedule,
            /// and it may refer to a location in the middle a given Item as opposed
            /// to the beginning of the Item.
            struct ScheduleLocation
            {
                eastl_size_t mnScheduleIndex;   /// Schedule item index, relative to schedule.
                index_t      mnCharBase;        /// Char index of the beginning of the Schedule entry (the Item). The absolute char index referred to by a ScheduleLocation is mnCharBase + mnCharOffset.
                index_t      mnCharOffset;      /// Char index within the Schedule entry (the Item). Will be 0 or 1 for all types except kItemTypeText, as all other types have only one element.

                ScheduleLocation();
                bool operator==(const EA::Text::Typesetter::ScheduleLocation& x);

                index_t GetCharIndex() const;   /// Simply returns mnCharBase + mnCharOffset.
            };

            /// Describes a run of shaped text.
            /// In our layout algorithm we divide a paragraph into runs of 
            /// the same Bidi level and shape them individually. We store
            /// the list of runs in RunInfo so that we can re-order the glyphs
            /// appriately for display.
            struct RunInfo
            {
                Script  mScript;
                int     mnBidiLevel;
                index_t mnCharBegin;
                index_t mnCharEnd;

                RunInfo(Script script = kScriptUnknown, int bidiLevel = 0, index_t iCharBegin = 0, index_t iCharEnd = 0)
                  : mScript(script), mnBidiLevel(bidiLevel), mnCharBegin(iCharBegin), mnCharEnd(iCharEnd) {}
            };

            typedef eastl::fixed_vector<RunInfo, 4, true> RunInfoArray;

        protected:
            /// Tells if there are any more items to process in the schedule.
            bool IsScheduleEmpty() const;

            /// Converts a char index to a ScheduleLocation. 
            /// If bCharIndexIsLineRelative is true, the char index is line-relative,
            /// else it is paragraph-relative.
            void GetScheduleLocationFromCharIndex(index_t charIndex, ScheduleLocation& scheduleLocation, bool bCharIndexIsLineRelative) const;

            /// Converts a char index to a schedule Item. 
            /// If bCharIndexIsLineRelative is true, the char index is line-relative,
            /// else it is paragraph-relative.
            Item* GetScheduleItemFromCharIndex(index_t charIndex, bool bCharIndexIsLineRelative);

            /// Adds a new text style to the set of styles used for characters in the 
            /// current paragraph. The Reset function will clear all text styles except
            /// the default text style. Returns a pointer to the internal storage for
            /// this style.
            const TextStyle* AddTextStyle(const TextStyle* textStyle);

            /// Processes mSchedule items that haven't been analyzed. This results in 
            /// entries being appended to mLineLayout.mAnalysisInfoArray. We don't 
            /// Process all mSchedule items, but only up to the number of items we 
            /// believe we'll need for the current line.
            void UpdateAnalysis();

            /// Adds a Unicode char to the current LineLayout. This char will be considered
            /// during the next round of analysis. 
            /// If bFilterReservedChars is true, then this function will check to see if the 
            /// incoming char is a special char that we reserve for our internal uses.
            /// If the char is such a special char, then this function replaces it with
            /// another replacement char of some sort.
            void AddChar(Char c, const TextStyle * pStyle, bool bFilterReservedChars);

            /// Sets the fonts used by the characters in the given range.
            void UpdateFontSelection(eastl_size_t iCharBegin, eastl_size_t iCharEnd);

            /// For a given char cluster of size count (usually just one), this function converts
            /// it into the appropriate glyph(s) in pGlyphIdArray. Returns the number of input
            /// Chars that were eaten. Sets the number of written GlyphIds in glyphIdCount.
            eastl_size_t GetGlyphsForChar(const Char* pChar, eastl_size_t count, const AnalysisInfo* pAnalysisInfo, 
                                           GlyphId* pGlyphIdArray, eastl_size_t& glyphIdCount, 
                                           const Char* pFallbackOptions = NULL, eastl_size_t fallbackOptionCount = 0);

            /// Writes the appropriate values into GlyphLayoutInfo for the given glyphMetrics.
            /// This involves the math of figuring out advances, x and y bearings, etc.
            void SetGlyphLayoutInfo(GlyphLayoutInfo& glyphLayoutInfo, float fPenX, const GlyphMetrics& glyphMetrics);

            /// Tells if there is a ligature available for the given sequence of Chars.
            /// Returns the count of Chars eaten, or zero if not ligature is available.
            /// This function is for Unicode substitution and not glyph substitution.
            eastl_size_t GetUnicodeLigatureGlyph(const Char* pChar, eastl_size_t count, Font* pFont, GlyphId& glyphId);

            /// Given a glyph index to a user object (from AddObject()), return its metrics.
            void GetObjectMetrics(index_t iGlyph, GlyphMetrics& glyphMetrics);

            /// Measures the glyphs and finds the last line-breakable point
            /// that fits within our margins.
            void UpdateLineState();

            /// Given a run of Unicode characters, this function tells what the sum of 
            /// the pen advances is for the corresponding glyph sequence. The return value
            /// will always be >= 0.f.
            float GetTextWidth(eastl_size_t iCharBegin);

            /// Replaces trailing text with ellipsis if there is non-fitting text.
            void SubstituteEllipsis();

            /// Implements Unicode Annex #9 (Bidi), section 3.4 L1.
            void AdjustWhitespaceEmbedding();

            /// Move the line to be at the user-specified coordinate origin and be of the 
            /// user-specified alignment or justification.
            void AdjustPositioning();

            /// Implements justification, which is the spreading out of text across the 
            /// available layout space. 
            void Justify();

            /// Reorders mBidiRunInfoArray from logical order (i.e. memory order) to display order.
            /// This function re-orders the array entries, but not the glyphs that they
            /// correspond to. Such a thing would be a successive step.
            void ReorderBidiRunInfoArray(RunInfoArray& runInfoArray);

            /// Reverses the order of the glyphs referred to by the given runInfoArray.
            void ReverseGlyphs(GlyphLayoutInfo* pGLIBegin, GlyphLayoutInfo* pGLILast, 
                               float fRunStartPos, float fRunEndPos);

            /// Orders text based on bidirectional embedding level. This is different from 
            /// any script-specific reordering that might occur between glyphs during shaping.
            /// For text that has no bidirectional glyphs, this function will do nothing.
            void OrderGlyphs();

            /// Validates that our data structures are sane.
            /// This function is linked away when not used, as would be the case in a release build.
            bool Validate();

        protected:
            /// Shapes all the current text into glyphs. 
            void ShapeText();

            /// Shape the given run of text (of the same script) into glyphs.
            void ShapeText(Script script, eastl_size_t iCharBegin, eastl_size_t iCharEnd);

            /// General shaping functions1
            void         PlaceGeneralGlyphCluster(eastl_size_t i, eastl_size_t charClusterSize);
            void         AppendGeneralGlyphCluster(eastl_size_t i, eastl_size_t charCount, 
                                                   const Char* pCharCluster, eastl_size_t charClusterSize, 
                                                   const GlyphId* pGlyphCluster, eastl_size_t glyphClusterSize, 
                                                   int embeddingLevel);
            eastl_size_t AppendGeneralCharCluster(eastl_size_t i, eastl_size_t charCount, 
                                                  const Char* charCluster, eastl_size_t clusterSize);
            eastl_size_t GetGeneralCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                               Char* pCharCluster, eastl_size_t& clusterSize);
            void         ShapeGeneral(eastl_size_t iCharBegin, eastl_size_t iCharEnd);
            void         CompleteLineLayoutArrays(eastl_size_t iCharBegin, eastl_size_t iCharEnd, eastl_size_t iGlyphBegin);

            // Thai shaping functions
            eastl_size_t GetThaiGlyphs(eastl_size_t i, const Char* pCharCluster, eastl_size_t clusterSize, GlyphId* pGlyphIdArray);
            eastl_size_t AppendThaiCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize);
            eastl_size_t GetThaiCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                                Char* pCharCluster, eastl_size_t& clusterSize);
            void         ShapeThai(eastl_size_t iCharBegin, eastl_size_t iCharEnd);

            // Hebrew shaping functions.
            eastl_size_t GetHebrewGlyphsForChars(const Char* pCharCluster, eastl_size_t clusterSize, const AnalysisInfo* pAnalysisInfo, 
                                                  GlyphId* pGlyphIdArray, eastl_size_t& glyphCount);
            eastl_size_t AppendHebrewCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize);
            eastl_size_t GetHebrewCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                                Char* pCharCluster, eastl_size_t& clusterSize);
            void         ShapeHebrew(eastl_size_t iCharBegin, eastl_size_t iCharEnd);

            // Hangul shaping functions.
            eastl_size_t AppendHangulCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize, int clusterType);
            eastl_size_t GetHangulCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                               Char* pCharCluster, eastl_size_t& clusterSize, int& clusterType);
            void         ShapeHangul(eastl_size_t iCharBegin, eastl_size_t iCharEnd);

            // Arabic shaping functions.
            void         ShapeArabic(eastl_size_t iCharBegin, eastl_size_t iCharEnd);
            void         AppendArabicGlyphCluster(eastl_size_t i, eastl_size_t charCount, 
                                                   const Char* pCharCluster, eastl_size_t charClusterSize, 
                                                   const GlyphId* pGlyphCluster, eastl_size_t glyphClusterSize, 
                                                   int embeddingLevel, const OTF* pOTF);
            // Other shaping functions.
            void ShapeIndic(eastl_size_t iCharBegin, eastl_size_t iCharEnd);

        protected:
            typedef eastl::fixed_vector<const TextStyle*, 8> TextStyleArray;

            static const int kMaxLigatureLength = 3;    /// Maximum number of characters a ligature can have. See GlyphInfo::mCharCount.

            // Invariants
            Allocator::ICoreAllocator* mpCoreAllocator;
            LayoutSettings             mLayoutSettings;        /// User-supplied layout settings. The settings are set by the user and are immutable during layout.
            FontServer*                mpFontServer;           /// The font server to use for font lookups.

            // Semi-variant
            TextStyleArray             mTextStyleArray;        /// List of all text styles in current use. These have no specific order; values are appended as new styles are set by the user.
            const TextStyle*           mpCurrentTextStyle;     /// Pointer into mTextStyleArray, indicating the last style that was set.
            float                      mfLayoutSpace;          /// The space the text line is laid out into.
            float                      mfLayoutPenX;           /// The initial pen x position for layout.
            float                      mfLayoutPenY;           /// The initial pen y position for layout.
            bool                       mbEllipsize;            /// If true, we ellipsize the line; we let it overflow and replace the end of it with an ellipsis.
            Char                       mPasswordChar;          /// The char used for displaying password text. Usually Unicode 0x25CF (9679), same as EA::Text::kCharPass.

            // Schedule
            Schedule                   mSchedule;              /// List of layout items to process.
            ScheduleLocation           mLineBegin;             /// Location of start of line.
            ScheduleLocation           mLineEnd;               /// Location of end of line.
            ScheduleLocation           mAnalysisEnd;           /// Location of the last schedule item we analyzed.
            ScheduleLocation           mScheduleEnd;           /// Location of end of schedule. (mScheduleEnd.mItemIndex == mSchedule.size())

            // Layout data
            LineState                  mLineState;                 /// The current line state. kLinePartial, kLineFull, etc.
            LineLayout                 mLineLayout;                /// The current line layout. Analysis array, glyph array, etc.
            index_t                    mnLastLineEndIndex;         /// The end Char index of the last line laid out (since last call to FinalizeLine) in the paragraph.
            RunInfoArray               mBidiRunInfoArray;          /// A list of the runs of equivalent bidi level.
            bool                       mbNonGeneralScriptPresent;  /// True if there is script present in the paragraph that requires a shaper other than the general shaper.
            float                      mfPenX;                     /// mfPenX/mfPenMaxX are temporary variables that aren't needed for the final placement step.
            float                      mfPenXMax;                  /// We track max pen position because it allows us to do better kerning.

        }; // class Typesetter

    } // namespace Text

} // namespace EA




#endif // Header include guard













