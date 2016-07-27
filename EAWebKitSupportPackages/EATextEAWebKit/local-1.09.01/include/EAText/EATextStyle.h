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
// EATextStyle.h
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTSTYLE_H
#define EATEXT_EATEXTSTYLE_H



#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextUnicode.h>
#include <EASTL/fixed_hash_map.h>


#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }

    namespace Text
    {
        // Forward declarations
        class StyleManager;


        /////////////////////////////////////////////////////////////////////////
        // CSS
        //
        // We use HTML CSS3 standard naming for font properties.
        /////////////////////////////////////////////////////////////////////////

        /// Family
        ///
        /// A font family is the name of a specific font and other fonts related to it. 
        /// However, in our system we usually reference fonts by name string rather than  
        /// some enumeration. Thus we only define the generic family names such as 
        /// serif and sans-serif here. All true font familes are defined via strings 
        /// such as Arial, Garamond, Times, though a generic family such as "serif" can
        /// be specified as a fallback.
        ///
        enum Family
        {
            kFamilySansSerif,   /// Fonts without serifs.
            kFamilySerif,       /// Fonts with serifs (minor horizontal components at top and bottom of characters)
            kFamilyCursive,     /// Cursive or decorative fonts.
            kFamilyFixedPitch,  /// Fonts that are monospaced, regardless of other properties.
            kFamilyDefault = kFamilySansSerif
        };


        /// Size
        ///
        /// Specifies the size of a font. We usually specify the size of a font in pixels,
        /// but it is useful for some purposes (e.g. CSS descriptors) to provide shorthand
        /// enumerations for common relative sizes. The values in this enumeration don't 
        /// specify any particular pixel size but rather specify relative sizes. The actual
        /// sizes associated with each size here can usually be defined at compile time or 
        /// at run time.
        ///
        enum Size
        {
            kSizeXXSmall,
            kSizeXSmall,
            kSizeSmall,
            kSizeMedium,      /// Medium will usually correspond to something like 12 or 14 pixel text.
            kSizeLarge,
            kSizeXLarge,
            kSizeXXLarge,
            kSizeDefault = kSizeMedium
        };


        /// Style
        ///
        /// Note that the term Style here is used as per the CSS standard, but it is nevertheless
        /// confusing because it refers only to the oblique or italic nature of text and not to 
        /// the full style as defined by CSS (cascading *style* sheets).
        ///
        /// The CSS standard specifies that style consists of normal, oblique, and italic. 
        /// Note that other 'styles' such as bolding are part of the font weight and not the style.
        /// The difference between oblique and italic is that oblique characters are the same shapes
        /// as the normal characters but merely slanted. Italic characters on the other hand are 
        /// both modified (e.g. an f will get a curved tail) and slanted. Note that this Style is 
        /// not the same thing as the broader topic of the "Style" part of "Cascading Style Sheet".
        ///
        enum Style
        {
            kStyleNormal,  /// Normal characters, no slanting.
            kStyleOblique, /// Normal characters that are merely slanted. 
            kStyleItalic,  /// Modified characters (e.g. calligraphically) with slanting.
            kStyleDefault = kStyleNormal
        };


        /// Weight
        ///
        /// The CSS standard for weight consists of numerical values from 100 to 900.
        /// Note that the concept of weight encompasses the concept of font boldness.
        ///
        enum Weight
        {
            kWeightLightest = 100,
            kWeightNormal   = 400,  /// The CSS standard specifies 400 as normal.
            kWeightBold     = 700,  /// The CSS standard specifies 700 as bold.
            kWeightHeaviest = 900,
            kWeightDefault  = kWeightNormal
        };


        /// Stretch
        ///
        /// The CSS standard specifies the following stetch enumerations. However, in our 
        /// system we additionally provide a more basic way of specifying stretch via a
        /// floating point multiplier. 
        ///
        enum Stretch
        {
            kStretchUltraCondensed,
            kStretchExtraCondensed,
            kStretchCondensed,
            kStretchSemiCondensed,
            kStretchNormal,
            kStretchSemiExpanded,
            kStretchExpanded,
            kStretchExtraExpanded,
            kStretchUltraExpanded,
            kStretchDefault = kStretchNormal
        };


        /// Variant
        ///
        /// Defines a variation of a font. Note that small capitals is a variant type, but 
        /// superscript and subscript are not. This is because small capitals is implemented 
        /// via an entirely different font, whereas superscript and subscript are merely small
        /// and repositioned implementations of an existing font. 
        ///
        /// As of CSS3, the only variant type is small caps (font with small but capitalized letters).
        ///
        enum Variant
        {
            kVariantNormal,
            kVariantSmallCaps,
            kVariantDefault = kVariantNormal
        };


        /// Pitch
        /// 
        /// Pitch isn't so much a font style as it is a font property. But nevertheless in 
        /// practice it sometimes happens that the user would like to request a font of a specific pitch.
        /// The Pitch attribute tends to follow a font family. Thus all fonts in the same
        /// family have similar Pitch. That is, they are all variable pitch or monospaced pitch.
        ///
        enum Pitch
        {
            kPitchVariable, /// Variable width of characters (e.g. Arial).
            kPitchFixed,    /// Fixed width of characters (e.g. Courier).
            kPitchDefault = kPitchVariable
        };


        /// Smooth (a.k.a. antialiasing)
        ///
        /// Smoothing is not part of the CSS standard, at least as of CSS3. However, smoothing is 
        /// an important text display attribute under some circumstances. 
        ///
        enum Smooth
        {
            kSmoothNone,                    /// Monochrome font with no anti-aliasing. Works fine for well-hinted fonts.
            kSmoothEnabled,                 /// Full anti-aliasing. This is slower because it requires destination reads and concomitant alpha blends.
            kSmoothDefault = kSmoothNone    /// Whatever the default is.
        };


        /// Effect
        ///
        /// An effect in CSS refers to a graphical variation of a font which changes its appearance.
        /// We define the same effect types as defined by the FCC for closed captioned TV. These happen
        /// to also be effects which are frequently seen in game software. Look up "EIA-708-B" on the 
        /// Internet to read more about this standard.
        ///
        /// It should be noted that the full set of effects that can be applied to font glyphs is 
        /// infinite and indeterminate. In practice, some of these effects can be done by modern 
        /// pixel and vertex shading hardware or software. Other effects are less amenable to such 
        /// processing, due to the design of the hardware and the APIs. Technically speaking, smoothing
        /// (see enum Smooth) is also an effect, but it is prevalent enough that it is identified
        /// independently.
        ///
        enum Effect
        {
            kEffectNone,
            kEffectOutline,
            kEffectShadow,
            kEffectRaised,
            kEffectDepressed,
            kEffectDefault = kEffectNone,
            kEffectUser = 256
        };


        /// DecorationFlags
        ///
        /// A decoration in CSS refers to generic embellishments applied to displayed text which 
        /// are unrelated to the given script.
        ///
        /// To consider: Possibly we should support double-underlines and/or thick underlines?
        ///
        enum DecorationFlags
        {
            kDFNone        = 0,
            kDFUnderline   = 1, 
            kDFLineThrough = 2, /// The CSS standard calls this "line through" and not "strikethrough"
            kDFOverline    = 4,
            kDFDefault     = kDFNone
        };


        /// HAlignment
        ///
        /// Note that the word "justified" refers to text that is stretched from left to right. 
        /// There thus can be no such thing as "left justified"; there is only "left aligned".
        ///
        enum HAlignment
        {
            kHALeft,        /// Text is aligned to the left side of an area.
            kHACenter,      /// Text is horizontally centered within an area.
            kHARight,       /// Text is aligned to the right side of an area.
            kHAJustify,     /// Text is stretched between the two sides of an area.
            kHAInherit,     /// Means inherit from parent style
            kHADefault = kHALeft,
        };


        /// VAlignment
        ///
        enum VAlignment
        {
            kVATop,         /// Text is aligned to the top side of an area.
            kVACenter,      /// Text is vertically centered within an area.
            kVABottom,      /// Text is aligned to the bottom side of an area.
            kVAInherit,     /// Means inherit from parent style
            kVADefault = kVATop,
        };


        /// Justify
        ///
        /// Specifies how text is justified. Recall that Justification is the process whereby
        /// text is layed out to fit completely from the left side of its area to the right
        /// side of the area. This is often done by adjusting the spacings between characters
        /// and/or words in the text. Different writing scripts (e.g. Japanese) do it in 
        /// different ways.
        ///
        enum Justify
        {
            kJustifyInterWord,
            kJustifyInterIdeograph,
            kJustifyDistribute,
            kJustifyNewspaper,
            kJustifyInterCluster,
            kJustifyKashida,
            kJustifyDefault = kJustifyInterWord
        };


        /// TextWrap
        ///
        /// Specifies how word wrapping is done, as per the CSS3 specification.
        /// Normally text is layed out using kTWWrap. kTWEmergency is possibly better
        /// because it is the same as kTWWrap except that it allows for breaking long
        /// words up in pathological cases.
        ///
        enum TextWrap
        {
            /// The text is only wrapped where explicitly specified by preserved line feed characters. 
            /// In the case when lines are longer than the available block width, the overflow will 
            /// be treated in accordance with the 'overflow' property specified in the element. 
            kTWNone,

            /// The text is wrapped at the best line-breaking opportunity (if required) within the 
            /// available block inline-progression dimension (block width in horizontal text flow). 
            /// The best line-breaking opportunity is determined in priority by the existence of 
            /// preserved line feed characters, or by the line-breaking algorithm controlled by 
            /// the 'line-break' and word-break' CSS properties. 
            kTWWrap,

            /// The text is wrapped after the last character which can fit before the ending-edge 
            /// of the line and where explicitly specified by preserved line feed characters. 
            /// No line-breaking algorithm is invoked. The intended usage is the rendering of a 
            /// character terminal emulation. 
            kTWSoft,

            /// Like kTWEmergency except that the first word is not broken into multiple lines 
            /// but instead proceeds to go outside the bounds.
            kTWHard,

            /// The text is wrapped like for the kTWWrap case, except that the line-breaking 
            /// algorithm will allow as a last resort option a text wrap after the last character 
            /// which can fit before the ending edge of the line box (i.e. act like kTWSoft), 
            /// independently of 'line-break', 'word-break-cjk' and 'word-break-inside' properties. 
            /// For example, this addresses the situation of very long words constrained in a 
            /// fixed-width container with no scrolling allowed.
            kTWEmergency,

            /// Means inherit from parent style
            kTWInherit,

            kTWDefault = kTWEmergency,
        };


        /// TextOverflow
        ///
        /// Specifies what is done when text overflows its bounds.
        ///
        enum TextOverflow
        {
            kTONone,      /// Text is not partially rendered and ellipsis are not rendered.
            kTOClip,      /// Clip text as appropriate for the text content. Glyphs representation of the text may be only partially rendered. 
            kTOEllipsis,  /// A visual hint is inserted at each box boundary where text overflow occurs. The 'text-overflow-ellipsis' property determines the content of the hint. The insertions take place after the the last letter that entirely fits on the line. 
            kTODefault = kTONone
        };


        /// EmphasisStyle
        ///
        /// This property sets the style for the emphasis formatting applied to text. 
        /// East Asian documents use the following symbols on top of each glyph to emphasize a 
        /// run of text: an 'accent' symbol, a 'dot', a hollow 'circle', or a solid 'disc'.
        /// Note, that unlike Decoration, this property can affect the line height. 
        /// Furthermore the emphasis style should be distinguished from the Decoration which is 
        /// another method to 'emphasize' text content.
        ///
        enum EmphasisStyle
        {
            kESNone,
            kESAccent,
            kESDot,
            kESCircle,
            kESDisc,
            kESDefault = kESNone
        };


        /// EmphasisPosition
        ///
        /// Refers to the location of where Asian emphasis symbols go relative to each character.
        ///
        enum EmphasisPosition
        {
            kEPBefore,  /// For Asian text (vertically oriented or not), goes above each emphasized character.
            kEPAfter,   /// For Asian text (vertically oriented or not), goes below each emphasized character.
            kEPDefault = kEPBefore
        };


        /// DigitSubstitution
        ///
        /// Specifies how digits are selected or substituted.
        ///
        enum DigitSubstitution
        {
            kDSNone,            /// Don't do digit changes. The text is used as-is.
            kDSContext,         /// Pick digits based on the context of the text.
            kDSWestern,         /// Use Western digits unilaterally.
            kDSNational,        /// Use digits which are the national standard for the locale associated with the text.
            kDSTraditional,     /// Use digits which are traditional for the locale associated with the text. These may be different than national standard digits.
            kDSDefault = kDSNone
        };


        /// PasswordMode
        ///
        /// Specifies how digits are selected or substituted.
        ///
        enum PasswordMode
        {
            kPMNone,
            kPMPassword,
            kPMDefault = kPMNone
        };

        /// FontStyle
        ///
        /// Holds basic style specification information required for requesting a font from 
        /// the FontServer. Doesn't include aspects of fonts that are related to layout or
        /// decoration (e.g. underline). 
        ///
        const uint32_t kFamilyNameArrayCapacity = EATEXT_FAMILY_NAME_ARRAY_CAPACITY;
        const uint32_t kMaxFamilyNameSize       = 31;
        const uint32_t kFamilyNameCapacity      = kMaxFamilyNameSize + 1;

        struct NoInit{ };

        struct EATEXT_API FontStyle
        {
            Char     mFamilyNameArray[kFamilyNameArrayCapacity][kFamilyNameCapacity]; /// A list of families to use as options for characters. The list is terminated by an empty string.
            float    mfSize;                /// Size is in pixels.
            Style    mStyle;                /// e.g. italic. CSS calls italic and oblique "styles".
            float    mfWeight;              /// e.g. kWeightBold (700).
            Variant  mVariant;              /// e.g. small caps.
            Pitch    mPitch;                /// Pitch type.
            Smooth   mSmooth;               /// e.g. kSmoothToBackground. Refers to antialiased output.
            uint32_t mEffect;               /// e.g. kEffectOutline
            float    mfEffectX;             /// If mEffect is kEffectShadow, mfEffectX/mfEffectY controls the x/y offset.
            float    mfEffectY;             /// If mEffect is kEffectOutline, this mfEffectX/mfEffectY x/y thickness.

            FontStyle();
            FontStyle(NoInit) { }
            FontStyle(const FontStyle& fs);
            FontStyle& operator=(const FontStyle& fs);
        };

        EATEXT_API bool operator==(const EA::Text::FontStyle& a, const EA::Text::FontStyle& b);


        /// TextStyle
        ///
        /// Holds a computed CSS3 style specification to the extent that we support CSS3.
        /// By 'computed', we mean that the user specification has been converted from text 
        /// and relative terms (e.g. size of kSizeSmall) to absolute terms (e.g. size of 10).
        /// See http://www.w3.org/TR/2002/WD-css3-fonts-20020802/ (or look for CSS3 font)
        ///
        struct EATEXT_API TextStyle : public FontStyle
        {
            Char              mStyleName[32];       /// User-defined optional name for the style.
            uint32_t          mParentId;            /// StyleId of parent text style, 0 if there's no parent
            Color             mColor;               /// e.g. ARGB of (255, 23, 34, 124). The color of the text itself. If the font is not colorable then the rgb portion is ignored.
            Color             mColorBackground;     /// e.g. ARGB of (255, 23, 34, 124). An alpha of zero means that no background is specified. Background is a rectangular area directly behind the character. It is not the inverse of the character but is simply a rectangle.
            Color             mEffectBaseColor;     /// 
            Color             mEffectColor;         /// If Effect is outline, mEffectColor is outline color, if effect is shadow, mEffectColor is the shadow color.
            Color             mHighLightColor;      /// used for rasied and depressed effects
            uint32_t          mnDecorationFlags;    /// Underline, linethrough (a.k.a. strikethrough), and/or overline.
            Direction         mDirection;           /// See enum.
            Orientation       mOrientation;         /// See enum.
            HAlignment        mHAlignment;          /// See enum.
            VAlignment        mVAlignment;          /// See enum.
            Justify           mJustify;             /// See enum.
            TextWrap          mTextWrap;            /// See enum.
            TextOverflow      mTextOverflow;        /// See enum.
            EmphasisStyle     mEmphasisStyle;       /// See enum.
            EmphasisPosition  mEmphasisPosition;    /// See enum.
            DigitSubstitution mDigitSubstitution;   /// See enum.
            PasswordMode      mPasswordMode;        /// See enum.
            float             mfStretch;            /// Horizontal stretch; 1.0 means no stretch, 0.5 means compressed to half width.
            float             mfLetterSpacing;      /// Specifies additional spacing between letters within a word, in pixels. Negative values are accepted.
            float             mfWordSpacing;        /// Specifies additional spacing between words, in pixels. Word spacing is applied in addition to letter spacing. Negative values are accepted.
            float             mfLineSpace;          /// Height of a text line with the font, in pixels. 0 means default. The position of the baseline remains constant relative to the bottom of the line, and changes relative to the top.

            TextStyle();
            TextStyle(NoInit) { }
            TextStyle(const TextStyle& ts);
            TextStyle& operator=(const TextStyle& ts);
        }; 

        EATEXT_API bool operator==(const TextStyle& a, const TextStyle& b);

        struct EATEXT_API TextStyleHash {
            size_t operator()( const TextStyle& style ) const;
        };


        /// kStyleIdAll
        /// Refers to all styles. See individual StyleManager functions for applicability.
        const uint32_t kStyleIdAll = 0xffffffff; 

        /// kStyleIdUnknown
        /// Refers to an unknown style. See individual StyleManager functions for applicability.
        const uint32_t kStyleIdUnknown = 0xfffffffe; 



        #if EATEXT_STYLE_MANAGER_ENABLED

        /// GetStyleId
        /// Returns an id based on a style name. This function simply returns a hash of the
        /// input string. The input string can be a 8 bit char string or a 16 bit wide char
        /// string. An empty or NULL input string results in an id of zero. Due to the 
        /// nature of the hashing algorithm, it is entirely possible for a non-empty string
        /// to result in an id that is zero. 
        /// 
        /// Example usage:
        ///    uint32_t id = GetStyleId("Main Menu");
        ///
        EATEXT_API uint32_t GetStyleId(const char* pStyleName);




        /// StyleManager
        ///
        /// Manages style sets. The nice thing about style sets is that you can specify
        /// a hierarchical set of text styles that an application uses and you can change
        /// the fonts in usage across the app by changing a single setting. 
        ///
        ///
        /// To consider: Move this class to another location, possibly outside EAText altogether.
        ///
        class EATEXT_API StyleManager
        {
        public:
            /// StyleManager
            StyleManager(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            StyleManager(const StyleManager&);
            StyleManager& operator=(const StyleManager&);

            /// ~StyleManager
            EATEXT_VIRTUAL ~StyleManager();

            /// SetAllocator
            /// Sets the allocator to use for any memory allocation. This class only allocates
            /// memory when its style list is initialized. It does not allocate memory as it 
            /// runs and serves styles.
            /// This function must be called before this class is used.
            void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

            /// GetStyle
            /// Returns the style information for the given style Id.
            /// If the pTS argument is non-null, the style info is written to the passed in data
            /// and the passed in pointer is returned right back to the user. If pTS is NULL, 
            /// a pointer to internal style information is returned. Note that in this latter case
            /// that performance is faster but the returned pointer is not safe for use if the 
            /// given style is removed from the StyleManager in the meantime. Note that other changes
            /// to the StyleManager do not affect the validity of this returned pointer.
            /// The return value will be NULL if the style doesn't exist.
            const TextStyle* GetStyle(uint32_t nStyleId, TextStyle* pTS = NULL) const;

            /// AddStyle
            /// Adds a style to the style manager. If a style for the given nStyleId already exists,
            /// it is overwritten with the new style. The input style is copied to an internal 
            /// representation. It's best if the user sets styles once on startup and doesn't 
            /// change them; that way the style map doesn't change at runtime.
            /// If the input nStyleId is kStyleUnknown, the StyleManager selects a style id 
            /// dynamically.
            /// All calls to AddStyle should be eventually matched by calls to RemoveStyle.
            /// The return value is the style id.
            uint32_t AddStyle(uint32_t nStyleId, const TextStyle& ts);

            /// RemoveStyle
            /// Removes a style from the style manager, based on its id. If the id is kStyleAll,
            /// then all styles are cleared. Clearing all styles is useful as part of a process
            /// of resetting styles.
            void RemoveStyle(uint32_t nStyleId);

            /// EnumerateStyles
            /// Retrieves all existing style Ids, up to the capacity specified.
            /// Return value is the total number of existing style Ids, regardless of the
            /// specified capacity.
            uint32_t EnumerateStyles(uint32_t* pStyleIdArray, uint32_t nStyleIdArrayCapacity) const;

        protected:
            struct TextStyleEx : public TextStyle
            {
                int32_t mnRefCount;

                TextStyleEx() : mnRefCount(0) { }
                TextStyleEx(const TextStyle& ts) : mnRefCount(0) { TextStyle& rThis = *this; rThis = ts; }
                TextStyleEx(const TextStyleEx& tse) : TextStyle(tse), mnRefCount(0) { }
                TextStyleEx& operator=(const TextStyle& ts) { TextStyle& rThis = *this; rThis = ts; return *this; }
                TextStyleEx& operator=(const TextStyleEx& tse) { TextStyle& rThis = *this; rThis = tse; return *this; }
            };

            uint32_t GetUniqueStyleId();

        protected:
            typedef eastl::fixed_hash_map<uint32_t, TextStyleEx*, 8> StyleMap;

            Allocator::ICoreAllocator* mpCoreAllocator;
            StyleMap                   mStyleMap;         // To consider: Make this be a fixed-size data structure such as an array and do lookups via a binary search.
            uint32_t                   mnNextStyleId;

        }; // class StyleManager


        #endif // EATEXT_STYLE_MANAGER_ENABLED


    } // namespace Text

} // namespace EA




///////////////////////////////////////////////////////////////////////////////
// Deprecated functions. Superceded by TextStyleReader.
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace Text
    {
        #if EATEXT_STYLE_MANAGER_ENABLED

            /// ParseStyleText
            /// Parses a CSS font style to one or more TextStyle entries.
            /// Return value is the number of styles present in the input.
            /// If pStyleManager is non-NULL, if any style text references a parent that isn't in the
            /// text, the style manager is consulted to see if it has such a style. 
            EATEXT_API uint32_t ParseStyleText(const char* pCSSText, char pStyleName[][32], uint32_t pIdArray[], 
                                                TextStyle tsArray[], uint32_t nArrayCapacity, 
                                                const StyleManager* pStyleManager = NULL); 
            /// GetStyleFunc
            /// Callback used by ParseStyle function. This function is intended
            /// to retrieve the parent style for calculating style inheritance.
            typedef bool (*GetStyleFunc)(uint32_t parentStyleId, TextStyle& tsResult, void* pContext);

            /// AtCommandCallback
            /// Callback used by ParseStyle function. Called whenever an '@'-command is
            /// found at the beginning of a line. The pCommandText parameter will point
            /// to the text after the '@' char and will be 0-terminated.
            /// The primary use for this is so that the user can specify font files to 
            /// load which happen to support the given styles specified in the file.
            typedef void (*AtCommandCallback)(const char* pCommandText, void* pContext);

            /// ParseStyleText
            /// A different version of the above function, which parses a single style and
            /// returns. The result is false if the end of file has been reached.
            /// A callback function is used to lookup the parent style.
            EATEXT_API bool ParseStyleText(const char*& pCSSText, char* pStyleName, uint32_t& pStyleId, 
                                            TextStyle& tsResult, GetStyleFunc pFunc, void* pContext,
                                            AtCommandCallback pAtCallback = NULL, void* pAtContext = NULL);

        #endif // EATEXT_STYLE_MANAGER_ENABLED

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_EATEXTSTYLE_H
