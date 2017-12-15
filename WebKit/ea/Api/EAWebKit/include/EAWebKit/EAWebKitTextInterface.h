/*
Copyright (C) 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

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

#ifndef EAWEBKIT_TEXT_INTERFACE_H
#define EAWEBKIT_TEXT_INTERFACE_H

#include "EAWebKitConfig.h"
#include <string.h> //For memset

namespace EA
{
namespace WebKit
{

typedef uint16_t GlyphId;
typedef char16_t Char;          // Needs to stay in sync with JSTextInferface.h
typedef uint32_t Color;

const uint32_t	kFamilyNameCapacity      = 32;
const uint32_t	kFamilyNameArrayCapacity = 8;
const Char		kCharInvalid = 0xFFFF;  
const GlyphId	kGlyphIdInvalid = 0xffff;
const int		TextBreakDone = -1;

enum BitmapFormat
{
    kBFNone       =  0,     /// No format in particular.
    kBFMonochrome =  1,     /// 1 bit per pixel, no compression aside from the bits packed in the bytes.
    kBFGrayscale  =  8,     /// 8 bits per pixel, no compression. Each pixel represents opacity.
    kBFARGB       = 32,     /// 32 bits per pixel, no compression. Format is 32 bit 0xaarrggbb.
    kBFRGBA       = 33,     /// 32 bits per pixel, no compression. Format is 32 bit 0xrrggbbaa.
    kBFDXT3       =  3,     /// DXT3 encoding
    kBFARGB_FLIP  = 64      /// This is really an RGBA but needs to be flipped to ARGB 
};

enum Pitch
{
    kPitchVariable, /// Variable width of characters (e.g. Arial).
    kPitchFixed,    /// Fixed width of characters (e.g. Courier).
    kPitchDefault = kPitchVariable
};

enum Script
{                                 // ISO 15924 code (four letter codes; see http://www.unicode.org/reports/tr24/)
    kScriptUnknown = -1,          //
    kScriptCommon = 0,            // Zyyy  // Special reserved code for characters that are used in multiple scripts.
    kScriptInherited,             // Qaai  // Special reserved code for script inherited by the environment.

    kScriptStart,                 //
    kScriptArabic = kScriptStart, // Arab
    kScriptArmenian,              // Armn
    kScriptBengali,               // Beng
    kScriptBraille,               // Brai
    kScriptBopomofo,              // Bopo                           // Mandarin Chinese
    kScriptBuhid,                 // Buhd
    kScriptCanadianAboriginal,    // Cans
    kScriptCherokee,              // Cher
    kScriptCoptic,                // Qaac
    kScriptCypriot,               // Cprt
    kScriptCyrillic,              // Cyrl (Cyrs)
    kScriptDeseret,               // Dsrt
    kScriptDevanagari,            // Deva
    kScriptEthiopic,              // Ethi
    kScriptGeorgian,              // Geor (Geon, Geoa)
    kScriptGothic,                // Goth
    kScriptGreek,                 // Grek
    kScriptGujarati,              // Gujr
    kScriptGurmukhi,              // Guru
    kScriptHan,                   // Hani                           // Chinese and Japanese symbols (Kanji)
    kScriptHangul,                // Hang                           // Korean symbols
    kScriptHanunoo,               // Hano
    kScriptHebrew,                // Hebr
    kScriptHiragana,              // Hira                           // Japanese syllabary
    kScriptKannada,               // Knda
    kScriptKatakana,              // Kana
    kScriptKhmer,                 // Khmr
    kScriptLao,                   // Laoo
    kScriptLatin,                 // Latn (Latf, Latg)              // English, Spanish, German French, etc.
    kScriptLimbu,                 // Limb
    kScriptLinearB,               // Linb
    kScriptMalayalam,             // Mlym
    kScriptMongolian,             // Mong
    kScriptMyanmar,               // Mymr
    kScriptOgham,                 // Ogam
    kScriptOldItalic,             // Ital
    kScriptOriya,                 // Orya
    kScriptOsmanya,               // Osma
    kScriptRunic,                 // Runr
    kScriptShavian,               // Shaw
    kScriptSinhala,               // Sinh
    kScriptSyriac,                // Syrc (Syrj, Syrn, Syre)
    kScriptTagalog,               // Tglg
    kScriptTagbanwa,              // Tagb
    kScriptTaiLe,                 // Tale
    kScriptTamil,                 // Taml
    kScriptTelugu,                // Telu
    kScriptThaana,                // Thaa
    kScriptThai,                  // Thai
    kScriptTibetan,               // Tibt
    kScriptUgaritic,              // Ugar
    kScriptYi,                    // Yiii
    kScriptEnd = kScriptYi
};

enum Weight
{
    kWeightLightest = 100,
    kWeightNormal   = 400,  /// The CSS standard specifies 400 as normal.
    kWeightBold     = 700,  /// The CSS standard specifies 700 as bold.
    kWeightHeaviest = 900,
    kWeightDefault  = kWeightNormal
};


struct GlyphDrawInfo
{
    uint8_t*     mpData;         // Base address of glyph texture page
    uint32_t     mSize;
    intptr_t     mStride;
    BitmapFormat mFormat;
    float        mV0;           /// Position of glyph on texture. These are UV positions and so are between 0 and 1.
    float        mU0;           /// Position of glyph on texture.
    float        mV1;           /// Position of glyph on texture.
    float        mU1;           /// Position of glyph on texture.
    float x1;     // Destination glyph box x left
    float x2;     // Destination glyph box x right
    float y1;     // Destination glyph box y top
    float y2;     // Destination glyph box y bottom
    GlyphDrawInfo() :  mpData(0) {}  // We really just need a clean data ptr
};	

struct GlyphMetrics
{
    float mfSizeX;            /// The width of the glyph.
    float mfSizeY;            /// The height of the glyph.
    float mfHBearingX;        /// The distance from the origin ("pen position") to the left of the glyph. This usually is a value >= 0. May be negative for glyphs that are diacritics which modify the character before them.
    float mfHBearingY;        /// The distance from the origin to the top of the glyph. This is usually a value >= 0. May be negative for glyphs that are entirely below the baseline (horizontal text).
    float mfHAdvanceX;        /// The distance from the origin to the origin of the next glyph. This is usually a value > 0.
};

struct Kerning
{
    float mfKernX;
    float mfKernY;
};

enum Style
{
    kStyleNormal,  /// Normal characters, no slanting.
    kStyleOblique, /// Normal characters that are merely slanted. 
    kStyleItalic,  /// Modified characters (e.g. calligraphically) with slanting.
    kStyleDefault = kStyleNormal
};

enum Variant
{
    kVariantNormal,
    kVariantSmallCaps,
    kVariantDefault = kVariantNormal
};
enum Smooth
{
    kSmoothNone,                    /// Monochrome font with no anti-aliasing. Works fine for well-hinted fonts.
    kSmoothEnabled,                 /// Full anti-aliasing. This is slower because it requires destination reads and concomitant alpha blends.
    kSmoothDefault = kSmoothNone    /// Whatever the default is.
};

enum FontType
{
    kFontTypeUnknown,   /// The font type is not known.
    kFontTypeBitmap,    /// The glyphs are derived from pregenerated bitmaps.
    kFontTypeOutline,   /// The glyphs are derived from outlines (such as Bezier). TrueType, PostScript, and OpenType fonts are outline fonts, though they allow for embedding of bitmaps. 
    kFontTypeStroke,    /// The glyphs are derived from strokes. This is common for East Asian kanji.
    kFontTypePolygon   /// The glyphs are derived from polygons.
};

enum FontFormat
{
	kTrueType,
	kOpenType
};
// This needs to stay in sync with the enum in JSTextInterface.h 
enum CharCategory
{
    kCCUnassigned             =  0, /// Non-category for unassigned and non-character code points.
    kCCGeneralOtherTypes      =  0, /// Cn other, not assigned. Same as kCCUnassigned. Note that no characters in UnicodeData.txt have this property. 
    kCCControlChar            =  1, /// Cc
    kCCFormatChar             =  2, /// Cf
    kCCPrivateUseChar         =  3, /// Co
    kCCSurrogate              =  4, /// Cs
    kCCUppercaseLetter        =  5, /// Lu
    kCCLowercaseLetter        =  6, /// Ll
    kCCTitlecaseLetter        =  7, /// Lt
    kCCModifierLetter         =  8, /// Lm
    kCCOtherLetter            =  9, /// Lo
    kCCNonSpacingMark         = 10, /// Mn
    kCCEnclosingMark          = 11, /// Me
    kCCCombiningSpacingMark   = 12, /// Mc
    kCCDecimalDigitNumber     = 13, /// Nd
    kCCLetterNumber           = 14, /// Nl
    kCCOtherNumber            = 15, /// No
    kCCSpaceSeparator         = 16, /// Zs
    kCCLineSeparator          = 17, /// Zl
    kCCParagraphSeparator     = 18, /// Zp
    kCCDashPunctuation        = 19, /// Pd
    kCCStartPunctuation       = 20, /// Ps
    kCCEndPunctuation         = 21, /// Pe
    kCCConnectorPunctuation   = 22, /// Pc
    kCCOtherPunctuation       = 23, /// Po
    kCCInitialPunctuation     = 24, /// Pi
    kCCFinalPunctuation       = 25, /// Pf
    kCCMathSymbol			  = 26, /// Sm
    kCCCurrencySymbol         = 27, /// Sc
    kCCModifierSymbol         = 28, /// Sk
    kCCOtherSymbol            = 29  /// So
};
 
enum BidiClass
{
    kBidiClassON = 0,   /// Other Neutrals. Includes all other characters, including OBJECT REPLACEMENT CHARACTER.
    kBidiClassL,        /// Left-to-Right. Includes LRM, Most alphabetic, syllabic, Han ideographic characters, digits that are neither European nor Arabic, ...
    kBidiClassR,        /// Right-to-Left. Includes RLM, Hebrew alphabet, most punctuation specific to that script, ...
    kBidiClassAN,       /// Arabic Number. Includes Arabic-Indic digits, Arabic decimal & thousands separators, ...
    kBidiClassEN,       /// European Number. Includes European digits, Eastern Arabic-Indic digits, ...
    kBidiClassAL,       /// Right-to-Left Arabic. Includes Arabic, Thaana, and Syriac alphabets, most punctuation specific to those scripts, ...
    kBidiClassNSM,      /// Non-Spacing Mark. Includes characters marked Mn (Non-Spacing Mark) and Me (Enclosing Mark) in the Unicode Character Database.
    kBidiClassCS,       /// Common Number Separator. Includes colon, comma, full stop (period), non-breaking space, ...
    kBidiClassES,       /// European Number Separator. Includes plus sign, minus sign.
    kBidiClassET,       /// European Number Terminator. Includes degree, currency symbols, ...
    kBidiClassBN,       /// Boundary Neutral. Includes most formatting and control characters, other than those explicitly given types above.
    kBidiClassS,        /// Segment Separator. Includes tab.
    kBidiClassWS,       /// Whitespace. Includes space, figure space, line separator, form feed, general punctuation spaces, ...
    kBidiClassB,        /// Paragraph Separator. Includes paragraph separator, appropriate newline functions, higher-protocol paragraph determination.
    kBidiClassRLO,      /// Right-to-Left Override.
    kBidiClassRLE,      /// Right-to-Left Embedding.
    kBidiClassLRO,      /// Left-to-Right Override.
    kBidiClassLRE,      /// Left-to-Right Embedding.
    kBidiClassPDF,      /// Pop Directional Format.
};

enum CaseType
{
    kCaseTypeNone,      /// No case.
    kCaseTypeLower,     /// Lower case.
    kCaseTypeUpper,     /// Upper case.
    kCaseTypeTitle,     /// Title case, which refers to chars in some language which combine an upper and lower character, such as Dz.
    kCaseTypeReverse    /// The reverse of the current case. Useful for flipping string sort order between upper and lower case strings.
};

enum DecompositionType
{
	kDTNone,            /// none
	kDTCanonical,       /// can
	kDTCompatible,      /// com
	kDTCircle,          /// enc
	kDTFinal,           /// fin
	kDTFont,            /// font
	kDTFraction,        /// fra
	kDTInitial,         /// init
	kDTIsolated,        /// iso
	kDTMedial,          /// med
	kDTNarrow,          /// nar
	kDTNoBreak,         /// nb
	kDTSmall,           /// sml
	kDTSquare,          /// sqr
	kDTSubscript,       /// sub
	kDTSuperscript,     /// sup
	kDTVertical,        /// vert
	kDTWide             /// wide
};

struct FontMetrics
{
    float mfSize;                   /// The classic font size metric; what you see in the text editor 'size' box.
    Pitch mPitch;                   /// Specifies if the font is of variable or fixed pitch (i.e. monospaced).
    float mfHAdvanceXMax;           /// For fixed pitch fonts, this is the advance for all non-zero-advance glyphs.
    float mfVAdvanceYMax;           /// For fixed pitch fonts, this is the advance for all non-zero-advance glyphs. May be equal to mfLineHeight for fonts without vertical metrics.
    float mfAscent;                 /// Max ascent value for the font. This will normally be >= 0. Covers the distance from the baseline to the top of the highest lower-case glyph (usually 'b').
    float mfDescent;                /// Min descent value for the font. This will normally be <= 0. Covers the distance from the baseline to the bottom of the lowest glyph. Bottoms of characters such as j, p, and q would be in this area.
    float mfLeading;                /// The extra space between the bottom of the descent and the top of the next line of text below the current one. Leading is often zero.
    float mfBaseline;               /// The distance from the top of a line down to the baseline. Equal to mfAscent + mfExternalLeading.
    float mfLineHeight;             /// The height of a line of text. Equal to mfDescent + mfBaseline. Also equal to mfDescent + mfAscent + mfExternalLeading.
    float mfXHeight;                /// The height of lower-case Latin glyphs (usually 'x').
    float mfCapsHeight;             /// The height of top of upper-case Latin glyphs (usually 'H').
    float mfUnderlinePosition;      /// The position is relative to baseline; negative values mean below the baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
    float mfUnderlineThickness;     /// Thickness of the line. 
    float mfLinethroughPosition;    /// The position is relative to baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
    float mfLinethroughThickness;   /// Thickness of the line. 
    float mfOverlinePosition;       /// The position is relative to baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
    float mfOverlineThickness;      /// Thickness of the line.

    FontMetrics() { memset(this, 0, sizeof(*this)); }
};


struct TextStyle
{
    //Font Style related properties
	Char     mFamilyNameArray[kFamilyNameArrayCapacity][kFamilyNameCapacity]; /// A list of families to use as options for characters. The list is terminated by an empty string.
    float    mfSize;                    /// Size is in pixels.
    Style    mStyle;                    /// e.g. italic. CSS calls italic and oblique "styles".
    float    mfWeight;                  /// [100 - 900]. Higher means bolder.
    Variant  mVariant;                  /// e.g. small caps.
    Pitch    mPitch;                    /// Pitch type.
    Smooth   mSmooth;                   /// 

    // Text style properties
    float    mfLetterSpacing;			/// Specifies additional spacing between letters within a word, in pixels. Negative values are accepted.
    float    mfWordSpacing;				/// Specifies additional spacing between words, in pixels. Word spacing is applied in addition to letter spacing. Negative values are accepted.

	TextStyle()
		: mfSize(0.f)
		, mStyle(EA::WebKit::kStyleNormal), mfWeight(EA::WebKit::kWeightNormal)
		, mVariant(EA::WebKit::kVariantNormal),	mPitch(EA::WebKit::kPitchVariable), mSmooth(EA::WebKit::kSmoothNone)
		, mfLetterSpacing(0.0f), mfWordSpacing(0.0f)
	{
		mFamilyNameArray[0][0] = 0;
	}
};

struct TextDrawInfo
{
    const EA::WebKit::GlyphDrawInfo* mpGDI; /// Returned array of draw info. 
    int mGDICount;                      /// Returned draw info array count.
    float mW;                           /// Bounding width of glyphs.
    float mH;                           /// Bounding height of text glyphs.
    float mXMin;                        /// Min x glyph position
    float mYMin;                        /// Min y glyph position.
};

class IFont
{
public:
    virtual ~IFont() {}
    
    virtual int     AddRef(void) = 0;
    virtual int     Release(void) = 0;
    virtual bool	IsItalic(void) = 0;
    virtual bool	IsSmallCaps(void) = 0;
	virtual float	GetSize(void) const = 0;  // Font Size
	virtual bool	SetSize(float fSize) = 0;
    virtual bool	GetFontMetrics(FontMetrics& fontMetrics) = 0;
    virtual bool	GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics) = 0;
    
    virtual uint32_t GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
                                        bool bUseReplacementGlyph = true, const uint32_t nGlyphIdStride = sizeof(GlyphId), bool bWriteInvalidGlyphs = false) = 0;
	
    virtual bool	GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool bHorizontalLayout = true) = 0;
	virtual bool	IsCharSupported(Char c, Script script = kScriptUnknown) = 0;
    virtual void    SetSmoothing(EA::WebKit::Smooth type) = 0;   
    virtual void    BeginDraw(void) = 0;
    virtual void    EndDraw(void) = 0;
}; 

class ITextSystem
{
public:
    virtual ~ITextSystem() {}
    
	virtual bool Init(void) = 0;
	virtual bool Shutdown(void) = 0;

	// Font Support
    virtual bool                GetGlyphs(IFont* pFont, const GlyphId* gIdArray, int glyphCount, GlyphDrawInfo* outGDI) = 0;
    virtual EA::WebKit::IFont*	GetFont(const EA::WebKit::TextStyle& textStyle, Char c = kCharInvalid) = 0;
    virtual EA::WebKit::IFont*	CreateNewFont(EA::WebKit::FontType fontType, const void* pSourceData, uint32_t nSourceSize, int nFaceIndex = 0) = 0;
    virtual uint32_t			AddFace(void* data, size_t dataSize) = 0;
    virtual bool				AddSubstitution(const char16_t* pFamily, const char16_t* pFamilySubstitution) = 0;
	virtual bool				SupportsFormat(EA::WebKit::FontFormat format) = 0;
    virtual bool                GetDrawInfoForComplexText(EA::WebKit::IFont* pFont, const EA::WebKit::Char* textRun, int textRunCount, float clipLeft, float clipRight, EA::WebKit::TextDrawInfo& outDrawInfo) = 0;
    virtual float               GetWidthForComplexText(EA::WebKit::IFont* pFont, const EA::WebKit::Char* run, int runCount) = 0;

    // Text Layout Support
    virtual void*	CharacterBreakIterator(Char* pText, int length) = 0;
	virtual void*	CursorBreakIterator(Char* pText, int length) = 0;
	virtual void*	WordBreakIterator(Char* pText, int length) = 0;
    virtual void*	SentenceBreakIterator(Char* pText, int length) = 0;
    virtual int		TextBreakFirst(void* pIterator) = 0;
	virtual int		TextBreakLast(void* pIterator) = 0;

    virtual int		TextBreakNext(void* pIterator) = 0;
    virtual int		TextBreakCurrent(void* pIterator) = 0;
    virtual int		TextBreakPrevious(void* pIterator) = 0;
    virtual int		TextBreakPreceding(void* pIterator, int position) = 0;
    virtual int		TextBreakFollowing(void* pIterator, int position) = 0;
    virtual bool	IsTextBreak(void* pIterator, int position) = 0;
    virtual int		FindNextWordFromIndex(Char* chars, int len, int position, bool forward) = 0;
    virtual void	FindWordBoundary(Char* chars, int len, int position, int* start, int* end) = 0;

    virtual void*   AcquireLineBreakIterator(Char* pText, int length) = 0;
    virtual void    ReleaseLineBreakIterator(void *pIterator) = 0;

    // Unicode Support
    virtual int32_t			GetCombiningClass(Char c) = 0;
    virtual Char			GetMirrorChar(Char c) = 0;
    virtual CharCategory	GetCharCategory(Char c) = 0;
    virtual BidiClass		GetBidiClass(Char c) = 0;
    virtual uint32_t        ConvertCase(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity, CaseType caseType, const char* locale) = 0;
	virtual DecompositionType DecompositionType(Char c) = 0;
    //  Array capacity
    virtual uint32_t		GetFamilyNameArrayCapacity(void) = 0; 
};

} // Namespace WebKit
} // Namespace EA
#endif // Header include guard


