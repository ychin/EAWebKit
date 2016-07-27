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

/////////////////////////////////////////////////////////////////////////////
// EAText/EATextStyleReader.cpp
//
// Written by Talin. Maintained by Paul Pedriana
//
// Parser for CSS-like styles.
/////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextStyleReader.h>

#if EATEXT_STYLE_MANAGER_ENABLED
#include <EAText/internal/StdC.h>
#include <EASTL/fixed_vector.h>
#include EA_ASSERT_HEADER


    namespace EA
    {
        namespace StdC
        {
            namespace Internal
            {
                template <typename Dest, typename DestCharType, typename Source>
                struct Strlcat6Class
                {
                    static bool Strlcat6Impl(Dest& d, const Source* pSource, size_t sourceLength)
                    {
                        const int additionalLength = EA::Text::Strlcpy(&d[0], pSource, 0, sourceLength);

                        if(additionalLength >= 0) // If there wasn't an encoding error in the source string...
                        {
                            const typename Dest::size_type originalLength = d.length();
                            d.resize(originalLength + (typename Dest::size_type)additionalLength);
                            EA::Text::Strlcpy(&d[originalLength], pSource, d.length() + 1, sourceLength);

                            return true;
                        }

                        return false;
                    }
                };

                template <typename Dest, typename CharType> // Specialization for the case that source and dest are equal.
                struct Strlcat6Class<Dest, CharType, CharType>
                {
                    static bool Strlcat6Impl(Dest& d, const CharType* pSource, size_t sourceLength)
                        { d.append(pSource, (typename Dest::size_type)sourceLength); return true; }
                };
            }

            template <typename Dest, typename Source>
            inline bool Strlcat(Dest& d, const Source* pSource, size_t sourceLength = (size_t)~0)
            {
                if(sourceLength == (size_t)~0)
                    sourceLength = strlen(pSource);

                return Internal::Strlcat6Class<Dest, typename Dest::value_type, Source>::Strlcat6Impl(d, pSource, sourceLength);
            }
        }
    }

namespace EA {
namespace Text {

static const float kDPI = 72.0f;                /// "Dots per inch"
static const float kCmToInches = 0.3937f;       /// Metric conversion factor
static const float kMmToInches = 0.03937f;      /// Metric conversion factor
static const float kMediumFontSize = 12.0f;     /// Default font size is 12
static const float kFontSizeStep = 1.15f;       /// Each larger size is 16% bigger


//////////////////////////////////////////////////////////////////////////
/// Clamp a floating point value between two values.
static inline float fclamp(float fVal, float fMin, float fMax)
{
   if (fVal < fMin)
        return fMin;
   if (fVal > fMax)
        return fMax;
   return fVal;
}


//////////////////////////////////////////////////////////////////////////
/// Pack values A, R, G and B into a 32-bit color
inline uint32_t PackARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

//////////////////////////////////////////////////////////////////////////
/// Pack values A, R, G and B into a 32-bit color.
inline uint32_t PackARGB(int32_t a, int32_t r, int32_t g, int32_t b)
{
    return ((a & 0x00ff) << 24) | ((r & 0x00ff) << 16) | ((g & 0x00ff) << 8) | (b & 0x00ff);
}

// Stylesheet Properties
enum TextStylePropertyID {
    Property_None,
    Property_BackgroundColor,
    Property_Color,
    Property_DigitSubstitution,
    Property_Font,
    Property_FontEffect,
    Property_FontEmphasisPosition,
    Property_FontEmphasisStyle,
    Property_FontFamily,
    Property_FontPitch,
    Property_FontSize,
    Property_FontSmooth,
    Property_FontStretch,
    Property_FontStyle,
    Property_FontVariant,
    Property_FontWeight,
    Property_LetterSpacing,
    Property_LineSpacing,
    Property_PasswordMode,
    Property_TextAlign,
    Property_TextDecoration,
    Property_TextJustify,
    Property_TextOverflowMode,
    Property_VerticalAlign,
    Property_WordSpacing,
    Property_WrapOption,
    Property_WritingMode
};


// Master property table (pre-sorted).
EA::Text::Lexer::IStaticDefinition<const char8_t, TextStylePropertyID> propertyTable[] = {
   // No entry for Property_None, because that's just a placeholder 
   { "background-color",        Property_BackgroundColor, },
   { "color",                   Property_Color, },
   { "digit-substitution",      Property_DigitSubstitution, },
   { "font",                    Property_Font, },
   { "font-effect",             Property_FontEffect, },
   { "font-emphasis-position",  Property_FontEmphasisPosition, },
   { "font-emphasis-style",     Property_FontEmphasisStyle, },
   { "font-family",             Property_FontFamily, },
   { "font-size",               Property_FontSize, },
   { "font-smooth",             Property_FontSmooth, },
   { "font-stretch",            Property_FontStretch, },
   { "font-style",              Property_FontStyle, },
   { "font-variant",            Property_FontVariant, },
   { "font-weight",             Property_FontWeight, },
   { "letter-spacing",          Property_LetterSpacing, },
   { "line-spacing",            Property_LineSpacing, },
   { "password-mode",           Property_PasswordMode, },
   { "text-align",              Property_TextAlign, },
   { "text-decoration",         Property_TextDecoration, },
   { "text-justify",            Property_TextJustify, },
   { "text-overflow-mode",      Property_TextOverflowMode, },
   { "text-valign",             Property_VerticalAlign, },
   { "vertical-align",          Property_VerticalAlign, },
   { "whitespace",              Property_WrapOption, },
   { "word-spacing",            Property_WordSpacing, },
   { "wrap-option",             Property_WrapOption, },
   { "writing-mode",            Property_WritingMode, }
};

// Units table (pre-sorted).
EA::Text::Lexer::IStaticDefinition<const char8_t, float> unitsTable[] = {
    { "cm",              kDPI * kCmToInches, },
    { "in",              kDPI, },
    { "mm",              kDPI * kMmToInches, },
    { "pc",              12.0f, },
    { "pt",              1.0f, },
    { "px",              1.0f, }
};

// Text style keyword table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::Style> fontStyleTable[] = {
    { "italic",          EA::Text::kStyleItalic, },
    { "normal",          EA::Text::kStyleNormal, },
    { "oblique",         EA::Text::kStyleOblique, }
};

// Font weight keyword table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, float> fontWeightTable[] = {
    { "bold",            (float)EA::Text::kWeightBold, },
    { "bolder",          (float)EA::Text::kWeightHeaviest, },
    { "lighter",         (float)EA::Text::kWeightLightest, },
    { "normal",          (float)EA::Text::kWeightNormal, }
};

// Font variant (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::Variant> fontVariantTable[] = {
    { "normal",          EA::Text::kVariantNormal, },
    { "small-caps",      EA::Text::kVariantSmallCaps, }
};

// Font pitch (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::Pitch> fontPitchTable[] = {
    { "fixed",           EA::Text::kPitchVariable, },
    { "variable",        EA::Text::kPitchFixed, }
};

// Font size keyword table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, float> fontSizeTable[] = {
    { "large",           kMediumFontSize * kFontSizeStep, },
    { "medium",          kMediumFontSize, },
    { "small",           kMediumFontSize / kFontSizeStep, },
    { "x-large",         kMediumFontSize * kFontSizeStep * kFontSizeStep, },    // Note that these * and / operations are in place
    { "x-small",         kMediumFontSize / kFontSizeStep / kFontSizeStep, },    // to make the numbers come out a certain way.
    { "xx-large",        kMediumFontSize * kFontSizeStep * kFontSizeStep * kFontSizeStep, },
    { "xx-small",        kMediumFontSize / kFontSizeStep / kFontSizeStep / kFontSizeStep, }
};

// Text decoration table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t,uint16_t> textDecorationTable[] = {
    { "line-through",    EA::Text::kDFOverline, },
    { "none",            EA::Text::kDFNone, },
    { "overline",        EA::Text::kDFLineThrough, },
    { "underline",       EA::Text::kDFUnderline, }
};

// Vertical alignment table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t,EA::Text::VAlignment> vAlignmentTable[] = {
    { "bottom",          EA::Text::kVABottom, },
    { "middle",          EA::Text::kVACenter, },
    { "top",             EA::Text::kVATop, }
};

// Horizontal alignment table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t,EA::Text::HAlignment> hAlignmentTable[] = {
    { "center",          EA::Text::kHACenter, },
    { "justify",         EA::Text::kHAJustify, },
    { "left",            EA::Text::kHALeft, },
    { "right",           EA::Text::kHARight, }
};

// Text wrap table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::TextWrap> wrapOptionTable[] = {
    { "emergency",       EA::Text::kTWEmergency, },
    { "hard-wrap",       EA::Text::kTWHard, },
    { "no-wrap",         EA::Text::kTWNone, },
    { "normal",          EA::Text::kTWWrap, },
    { "nowrap",          EA::Text::kTWNone, },
    { "soft-wrap",       EA::Text::kTWSoft, },
    { "wrap",            EA::Text::kTWWrap, }
};

// Font smoothing table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::Smooth> fontSmoothTable[] = {
    { "always",          EA::Text::kSmoothEnabled, },
    { "auto",            EA::Text::kSmoothDefault, },
    { "never",           EA::Text::kSmoothNone, }
};

// Font effect table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, uint32_t> fontEffectTable[] = {
    { "depressed",      EA::Text::kEffectDepressed, },
    { "none",           EA::Text::kEffectNone, },
    { "outline",        EA::Text::kEffectOutline, },
    { "raised",         EA::Text::kEffectRaised, },
    { "shadow",         EA::Text::kEffectShadow, }
};

// Text justification table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::Justify> textJustifyTable[] = {
    { "distribute",      EA::Text::kJustifyDistribute, },
    { "inter-cluster",   EA::Text::kJustifyInterCluster, },
    { "inter-ideograph", EA::Text::kJustifyInterIdeograph, },
    { "inter-word",      EA::Text::kJustifyInterWord, },
    { "kashida",         EA::Text::kJustifyKashida, },
    { "newspaper",       EA::Text::kJustifyNewspaper, }
};

// text overflow table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t ,EA::Text::TextOverflow> textOverflowTable[] = {
    { "clip",            EA::Text::kTOClip, },
    { "ellipsis",        EA::Text::kTOEllipsis, },
    { "none",            EA::Text::kTONone, }
};

// emphasis style table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::EmphasisStyle> emphasisStyleTable[] = {
    { "accent",          EA::Text::kESAccent, },
    { "circle",          EA::Text::kESCircle, },
    { "disc",            EA::Text::kESDisc, },
    { "dot",             EA::Text::kESDot, },
    { "none",            EA::Text::kESNone, }
};

// emphasis position table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::EmphasisPosition> emphasisPositionTable[] = {
    { "after",           EA::Text::kEPAfter, },
    { "before",          EA::Text::kEPBefore, }
};

// digit substitution table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::DigitSubstitution> digitSubstitutionTable[] = {
    { "context",         EA::Text::kDSContext, },
    { "national",        EA::Text::kDSNational, },
    { "none",            EA::Text::kDSNone, },
    { "traditional",     EA::Text::kDSTraditional, },
    { "western",         EA::Text::kDSWestern, }
};

// password mode table (pre-sorted).
static EA::Text::Lexer::IStaticDefinition<const char8_t, EA::Text::PasswordMode> passwordModeTable[] = {
    { "none",            EA::Text::kPMNone, },
    { "password",        EA::Text::kPMPassword, }
};




TextStyleReader::TextStyleReader(StyleManager* pManager)
    : reader_type()
    , mpManager(pManager)
    , mToken(NoToken)
    , mTokenLine(0)
    , mTokenLineStart(0)
    , mTokenPos(0)
    , mTokenValue(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "TextStyleReader")))
    , mpCallback(NULL)
    , mpCallbackContext(NULL)
    , mpErrorCallback(NULL)
    , mpErrorContext(NULL)
    , mResult(kResultSuccess)
    , mWarningCount(0)
{
}


TextStyleReader::TextStyleReader(StyleManager* pManager, const char8_t* pBegin)
    : reader_type(pBegin)
    , mpManager(pManager)
    , mToken(NoToken)
    , mTokenLine(0)
    , mTokenLineStart(0)
    , mTokenPos(0)
    , mTokenValue(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "TextStyleReader")))
    , mpCallback(NULL)
    , mpCallbackContext(NULL)
    , mpErrorCallback(NULL)
    , mpErrorContext(NULL)
    , mResult(kResultSuccess)
    , mWarningCount(0)
{
    Advance();
}


TextStyleReader::TextStyleReader(StyleManager* pManager, const char8_t* pBegin, const char8_t* pEnd)
    : reader_type(pBegin, pEnd)
    , mpManager(pManager)
    , mToken(NoToken)
    , mTokenLine(0)
    , mTokenLineStart(0)
    , mTokenPos(0)
    , mTokenValue(EASTLAllocatorType(EASTL_NAME_VAL(EATEXT_ALLOC_PREFIX "TextStyleReader")))
    , mpCallback(NULL)
    , mpCallbackContext(NULL)
    , mpErrorCallback(NULL)
    , mpErrorContext(NULL)
    , mResult(kResultSuccess)
    , mWarningCount(0)
{
    Advance();
}


bool TextStyleReader::MatchString(char_type endQuote)
{
    using namespace EA::Text::Lexer;

    mMatchText.clear();

    for (;;)
    {
        if (Skip(endQuote))
            return true;
        if (!Match(Any))
            return false;
    }
}


void TextStyleReader::Report(TextStyleResultCode nCode)
{
    if (mpErrorCallback)
        (*mpErrorCallback)(nCode, mTokenLine, mpErrorContext);
    else
    {
        #if defined(EA_DEBUG)
            static const char* sErrorNames[] =
            {
                "ErrorBase",
                "InvalidChar",
                "InvalidToken",
                "UnterminatedString",
                "BadColorFormat",
                "BadFontID",
                "BaseFontNotFound",
                "SemicolonExpected",
                "UnknownProperty",
                "InvalidOrMissingPropertyValue"
            };

            if (nCode >= kErrorBase && nCode <= kWarningInvalidOrMissingPropertyValue)
            {
                EA_FAIL_FORMATTED(("Error '%s' on line %d", sErrorNames[nCode - kErrorBase], mTokenLine));
                return;
            }
        #endif

        EA_FAIL_FORMATTED(("Error %x on line %d", nCode, mTokenLine));
    }
    mResult = nCode;
}


void TextStyleReader::Warn(TextStyleResultCode nWarning)
{
    mWarningCount++;
    Report(nWarning);
}


void TextStyleReader::Fatal(TextStyleResultCode nWarning)
{
    Report(nWarning);
}


bool TextStyleReader::Advance()
{
    using namespace EA::Text::Lexer;

    static MatchChar<'-'>      Dash;
    static MatchChar<'_'>      Underscore;
    static MatchAnyOf<'-','+'> Sign;
    static MatchAnyOf<'x','X'> LetterX;
    static MatchRange<'a','f'> HexLower;
    static MatchRange<'A','F'> HexUpper;

    mMatchText.clear();
    mTokenPos = Position();
    mTokenLine = Line();
    mTokenLineStart = LineStart();

    while (!AtEoF())
    {
        // Whitespace
        if (SkipWS() || ParseCPPComment(*this))
        {
            while (SkipWS() || ParseCPPComment(*this)) 
                { }
            continue;
        }

        // Identifier

        if (Match(LowerCase | UpperCase | Underscore))
        {
            while (Match(LowerCase | UpperCase | Underscore | Digit | Dash))
                { }
            mToken = TokenIdent;
            return true;
        }

        // Number
        //float f;
        bool bSign = Match(Sign);

        if (Match(Digit))
        {
            mToken = TokenInteger;

            if (mMatchText[0] == '0' && Match(LetterX))
            {
                mMatchText.clear();
                while (Match(HexLower | HexUpper | Digit))
                    { }
                mToken = mMatchText.size() > 0 ? TokenHexInteger : TokenError;
                return true;
            }

            MatchWhile(Digit);        // Grab all digits
            if (Match('.'))           // Check for decimal point
            {
                MatchWhile(Digit);    // Push digits after decimal point
                mToken = TokenFloat;
            }
            return true;
        }
        else if (bSign)
            mToken = TokenError;

        // Quoted string
        if (Skip('\"'))
        {
            if (MatchString('\"'))
            {
                mToken = TokenString;
                return true;
            }

            Fatal(kErrorUnterminatedString);
            mToken = TokenError;
        }

        if (Skip('\''))
        {
            if (MatchString('\''))
            {
                mToken = TokenString;
                return true;
            }

            Fatal(kErrorUnterminatedString);
            mToken = TokenError;
        }

        // Must be an operator or something
        // For now, just return the single char as a token
        char_type c = Read();

        if (Isprint(c))
        {
            switch (c) {
                case '#': mToken = TokenPound; break;
                case '@': mToken = TokenAt; break;
                case ':': mToken = TokenColon; break;
                case ';': mToken = TokenSemicolon; break;
                case '(': mToken = TokenOpenParen; break;
                case ')': mToken = TokenCloseParen; break;
                case '{': mToken = TokenOpenBrace; break;
                case '}': mToken = TokenCloseBrace; break;
                case ',': mToken = TokenComma; break;
                case '+': mToken = TokenPlus; break;
                case '%': mToken = TokenPercent; break;
                default:
                    Fatal(kErrorInvalidChar);
                    break;
            }

            Append(c);
            return true;
        }

        Fatal(kErrorInvalidChar);
        mToken = TokenError;
        return false;
    }

    mToken = NoToken;
    return false;
}


bool TextStyleReader::SkipSpace()
{
    bool bResult = false;

    while (mToken == TokenWS)
    {
        mTokenValue.clear();
        Advance();
        bResult = true;
    }
    return bResult;
}


// Used for skipping past errors
void TextStyleReader::SkipDeclaration()
{
    static EA::Text::Lexer::MatchAnyOf<';','}'> DeclEnd;

    if (mToken != TokenSemicolon)
    {
        SkipUntil(DeclEnd);
        Skip(';');
    }
    Advance();
}


bool TextStyleReader::MatchToken(TextStyleToken token)
{
    if (mToken == token)
    {
        mTokenValue.swap(MatchText());
        Advance();
        return true;
    }
    return false;
}


template<typename T, int N>
bool TextStyleReader::MatchSymbol(const T (&Table)[N], typename T::value_type& result)
{
    if (mToken == TokenIdent)
    {
        if (Lookup(Table, MatchText().c_str(), result))
        {
            Advance();
            return true;
        }
    }
    return false;
}


bool TextStyleReader::MatchSymbol(const char8_t* pSymbol)
{
    if ((mToken == TokenIdent) && (Strcmp(pSymbol, MatchText().c_str()) == 0))
    {
        Advance();
        return true;
    }
    return false;
}


bool TextStyleReader::MatchFloat(float& result)
{
    if ((mToken == TokenFloat) || (mToken == TokenInteger))
    {
        result = float(Strtod(MatchText().c_str(), NULL));
        Advance();
        return true;
    }
    return false;
}


bool TextStyleReader::MatchInteger(uint32_t& result)
{
    if (mToken == TokenInteger)
    {
        result = StrtoU32(MatchText().c_str(), NULL, 10);
        Advance();
        return true;
    }

    if (mToken == TokenHexInteger)
    {
        result = StrtoU32(MatchText().c_str(), NULL, 16);
        Advance();
        return true;
    }
    return false;
}


bool TextStyleReader::MatchLength(float& length, float normal)
{
    if (MatchFloat(length))
    {
        float fscale = 0.f;
        if (MatchSymbol(unitsTable, fscale))
            length *= fscale;
        return true;
    } 
    else if (MatchSymbol("normal"))
    {
        length = normal;
        return true;
    }
    return false;
}


bool TextStyleReader::MatchColor(Color& color)
{
    using namespace EA::Text::Lexer;

    eastl::string8 &matchText = MatchText();

    if (mToken == TokenPound)
    {
        // This first case has to be handle carefully because of token pipelining.
        // Because we're dealing with the lexer directly, we're working with the
        // 'next' token to be parsed, not the current token.
        static MatchRange<'a','f'>  HexLower;
        static MatchRange<'A','F'>  HexUpper;

        // Grab all the hex digits
        matchText.clear();

        for (int i = 0; i < 8; i++)
        {
            if (!Match(Digit | HexLower | HexUpper)) 
                break;
        }

        uint32_t n = StrtoU32(matchText.c_str(), NULL, 16);

        if (matchText.length() == 8)
            color = n;
        else if (matchText.length() == 6)
            color = n | 0xff000000;
        else if (matchText.length() == 4)
        {
            color = ((n & 0xf000) << 12) | ((n & 0x0f00) << 8) | ((n & 0x00f0) << 4) | ((n & 0x000f) << 0);
            color = color | (color << 4);
        }
        else if (matchText.length() == 3)
        {
            color = ((n & 0x0f00) << 8) | ((n & 0x00f0) << 4) | ((n & 0x000f) << 0);
            color = 0xff000000 | color | (color << 4);
        }
        else
        {
            Warn(kErrorBadColorFormat);
            Advance();
            return false;
        }
        Advance();
        return true;
    }

    if (MatchSymbol("rgb"))
    {
        float channels[3];

        if (MatchToken(TokenOpenParen))
        {
            int numChannels = 0;

            while (numChannels < 3)
            {
                float n = 0.f;

                if (MatchFloat(n))
                {
                    if (MatchToken(TokenPercent))
                        n = n * 255.0f / 100.0f;

                    channels[numChannels++] = n;

                    if (!MatchToken(TokenComma)) 
                        break;
                }
            }

            if (numChannels == 3 && MatchToken(TokenCloseParen))
            {
                uint8_t r, g, b;

                r = (uint8_t)fclamp(channels[0], 0.0f, 255.0f);
                g = (uint8_t)fclamp(channels[1], 0.0f, 255.0f);
                b = (uint8_t)fclamp(channels[2], 0.0f, 255.0f);
                color = PackARGB((uint8_t)255, r, g, b);

                return true;
            }
        }

        Warn(kErrorBadColorFormat);
        return false;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////
// font-family: [[<family-name> | <generic-family>] [, <family-name>| <generic-family>]*]
//////////////////////////////////////////////////////////////////////////
bool TextStyleReader::ParseFontFamilies(TextStyle& style)
{
    using namespace EA::Text;

    uint32_t numFonts = 0;

    while (MatchToken(TokenIdent) || MatchToken(TokenString))
    {
        if (numFonts < kFamilyNameArrayCapacity)
        {
            Strlcpy(&style.mFamilyNameArray[numFonts][0], mTokenValue.c_str(), kFamilyNameCapacity);
            numFonts++;
        }

        if (!MatchToken(TokenComma)) 
            break;
    }

    // Null-terminate the rest of the fonts
    while (numFonts < kFamilyNameArrayCapacity)
        style.mFamilyNameArray[numFonts++][0] = 0;

    return numFonts > 0;
}


//////////////////////////////////////////////////////////////////////////
// font-size: [xx-small | x-small | small | medium | large | x-large | xx-large]
// font-size: [larger | smaller]
// font-size: [length | percentage]
//////////////////////////////////////////////////////////////////////////
bool TextStyleReader::ParseFontSize(TextStyle& style)
{
    if (MatchLength(style.mfSize, 12.0f) || MatchSymbol(fontSizeTable, style.mfSize))
        return true;
    else if (MatchSymbol("larger"))
    {
        style.mfSize *= kFontSizeStep;
        return true;
    }
    else if (MatchSymbol("smaller"))
    {
        style.mfSize /= kFontSizeStep;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
// font-effect: none | effectID (<hex guid id> | <name string which is hashed to guid by FNV1>) | effectName <string> | [[outline | shadow | raised | depressed] xval yval base_color background_color [highlight_color]]
//////////////////////////////////////////////////////////////////////////
bool TextStyleReader::ParseFontEffect(TextStyle& style) 
{
    if (MatchSymbol("none"))
        return true;
    else if (MatchSymbol("effectID")) // Look for a hex GUID.
    {
        uint32_t effectID = 0;

        if (MatchToken(TokenString))
        {
            style.mEffect = FNV1(mTokenValue.c_str(), mTokenValue.length());
            return true;
        }
        else if (MatchInteger(effectID))
        {
            style.mEffect = effectID;
            return true;
        }
    }
    else if (MatchSymbol("effectName")) // Look for a string.
    {
        if (MatchToken(TokenString))
        {
            style.mEffect = FNV1(mTokenValue.c_str(), mTokenValue.length());
            return true;
        }
    }
    else if (MatchSymbol(fontEffectTable, style.mEffect) && 
             MatchFloat(style.mfEffectX) && 
             MatchFloat(style.mfEffectY) && 
             MatchColor(style.mEffectBaseColor) && 
             MatchColor(style.mEffectColor)) 
    {
        // For kEffectRaised and kEffectDepressed there is a third color: highlight
        if((style.mEffect == kEffectRaised || style.mEffect == kEffectDepressed) && !MatchColor(style.mHighLightColor))
            return false;

        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////
// Parses a single property, up to but not including the semicolon or
// !important flag.
//////////////////////////////////////////////////////////////////////////
bool TextStyleReader::ParseProperty(TextStyle& style)
{
    TextStylePropertyID propID = Property_None;

    if (!MatchSymbol(propertyTable, propID))
    {
        if (mToken == TokenIdent)
        {
            Warn(kWarningUnknownProperty);
            return false;
        }

        Warn(kWarningUnknownProperty);
        return false;
    }

    if (!MatchToken(TokenColon))
    {
        // Silently ignore malformed property as per CSS 2.1.
        return true;
    }

    switch (propID)
    {
        default:
            Warn(kWarningUnknownProperty);
            return false;

        //////////////////////////////////////////////////////////////////////////
        // font-family: [[<family-name> | <generic-family>] [, <family-name>| <generic-family>]*]
        //////////////////////////////////////////////////////////////////////////
        case Property_FontFamily:
            return ParseFontFamilies(style);

        //////////////////////////////////////////////////////////////////////////
        // font-style: normal | italic | oblique
        //////////////////////////////////////////////////////////////////////////
        case Property_FontStyle:
            return MatchSymbol(fontStyleTable, style.mStyle);

        //////////////////////////////////////////////////////////////////////////
        // font-weight: normal | bold | bolder | lighter | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900
        //////////////////////////////////////////////////////////////////////////
        case Property_FontWeight:
            return MatchFloat(style.mfWeight) || MatchSymbol(fontWeightTable, style.mfWeight);

        //////////////////////////////////////////////////////////////////////////
        // font-variant: normal | small-caps
        //////////////////////////////////////////////////////////////////////////
        case Property_FontVariant:
            return MatchSymbol(fontVariantTable, style.mVariant);

        //////////////////////////////////////////////////////////////////////////
        // font-pitch: fixed | variable
        //////////////////////////////////////////////////////////////////////////
        case Property_FontPitch:
            return MatchSymbol(fontPitchTable, style.mPitch);

        //////////////////////////////////////////////////////////////////////////
        // font-smooth: auto | always | never
        //////////////////////////////////////////////////////////////////////////
        case Property_FontSmooth:
            return MatchSymbol(fontSmoothTable, style.mSmooth);

        //////////////////////////////////////////////////////////////////////////
        // font-effect: none | effectID (<hex guid id> | <name string which is hashed to guid by FNV1>) | effectName <string> | [[outline | shadow | raised | depressed] xval yval base_color background_color [highlight_color]]
        //////////////////////////////////////////////////////////////////////////
        case Property_FontEffect:
                return ParseFontEffect(style);

        //////////////////////////////////////////////////////////////////////////
        // font-stretch: <length> | normal
        //////////////////////////////////////////////////////////////////////////
        case Property_FontStretch:
            return MatchLength(style.mfStretch, 1.0f);

        //////////////////////////////////////////////////////////////////////////
        // font-emphasis-style: none | accent | dot | circle | disc
        //////////////////////////////////////////////////////////////////////////
        case Property_FontEmphasisStyle:
            return MatchSymbol(emphasisStyleTable, style.mEmphasisStyle);

        //////////////////////////////////////////////////////////////////////////
        // font-emphasis-position: before | after
        //////////////////////////////////////////////////////////////////////////
        case Property_FontEmphasisPosition:
            return MatchSymbol(emphasisPositionTable, style.mEmphasisPosition);

        //////////////////////////////////////////////////////////////////////////
        // font-size: [xx-small | x-small | small | medium | large | x-large | xx-large]
        // font-size: [larger | smaller]
        // font-size: [length | percentage]
        //////////////////////////////////////////////////////////////////////////
        case Property_FontSize:
            return ParseFontSize(style);

        //////////////////////////////////////////////////////////////////////////
        // font: [<font-style> || <font-variant> || <font-weight>]? <font-size> <font-family>
        //////////////////////////////////////////////////////////////////////////
        case Property_Font:
        {
            while (MatchSymbol(fontStyleTable, style.mStyle)
                || MatchSymbol(fontVariantTable, style.mVariant)
                || MatchSymbol(fontWeightTable, style.mfWeight))
            {}

            return ParseFontSize(style) && ParseFontFamilies(style);
        }

        //////////////////////////////////////////////////////////////////////////
        // color: <color>
        //////////////////////////////////////////////////////////////////////////
        case Property_Color:
            return MatchColor(style.mColor);

        //////////////////////////////////////////////////////////////////////////
        // background-color: <color> | transparent
        //////////////////////////////////////////////////////////////////////////
        case Property_BackgroundColor:
            return MatchColor(style.mColorBackground);

        //////////////////////////////////////////////////////////////////////////
        // text-decoration: none | [underline || overline || line-through || blink]
        //////////////////////////////////////////////////////////////////////////
        case Property_TextDecoration:
        {
            uint16_t flag = 0;

            while (MatchSymbol(textDecorationTable, flag))
            {
                if (flag)
                    style.mnDecorationFlags |= flag;
                else 
                    style.mnDecorationFlags = 0;
            }
            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        // vertical-align: top | middle | bottom 
        //////////////////////////////////////////////////////////////////////////
        case Property_VerticalAlign:
            return MatchSymbol(vAlignmentTable, style.mVAlignment);

        //////////////////////////////////////////////////////////////////////////
        // text-justify: inter-word | inter-ideograph | distribute | newspaper | inter-cluster | kashida
        //////////////////////////////////////////////////////////////////////////
        case Property_TextJustify:
            return MatchSymbol(textJustifyTable, style.mJustify);

        //////////////////////////////////////////////////////////////////////////
        // text-align: left | right | center | justify
        //////////////////////////////////////////////////////////////////////////
        case Property_TextAlign:
            return MatchSymbol(hAlignmentTable, style.mHAlignment);

        //////////////////////////////////////////////////////////////////////////
        // wrap-option: wrap | no-wrap | soft-wrap | hard-wrap | emergency
        // whitespace:  normal | pre
        //////////////////////////////////////////////////////////////////////////
        case Property_WrapOption:
            if (MatchSymbol(wrapOptionTable, style.mTextWrap))
                return true;
            break;

        //////////////////////////////////////////////////////////////////////////
        // text-overflow-mode: none | clip | ellipsis
        //////////////////////////////////////////////////////////////////////////
        case Property_TextOverflowMode:
            return MatchSymbol(textOverflowTable, style.mTextOverflow);

        //////////////////////////////////////////////////////////////////////////
        // line-spacing: <length> | normal
        //////////////////////////////////////////////////////////////////////////
        case Property_LineSpacing:
            return MatchLength(style.mfLineSpace, 0.0f);

        //////////////////////////////////////////////////////////////////////////
        // word-spacing: <length> | normal
        //////////////////////////////////////////////////////////////////////////
        case Property_WordSpacing:
            return MatchLength(style.mfWordSpacing, 0.0f);

        //////////////////////////////////////////////////////////////////////////
        // letter-spacing: <length> | normal
        //////////////////////////////////////////////////////////////////////////
        case Property_LetterSpacing:
            return MatchLength(style.mfLetterSpacing, 0.0f);

        //////////////////////////////////////////////////////////////////////////
        // digit-substitution: <length>
        //////////////////////////////////////////////////////////////////////////
        case Property_DigitSubstitution:
            return MatchSymbol(digitSubstitutionTable, style.mDigitSubstitution);

        //////////////////////////////////////////////////////////////////////////
        // password-mode: password | normal
        //////////////////////////////////////////////////////////////////////////
        case Property_PasswordMode:
            return MatchSymbol(passwordModeTable, style.mPasswordMode);

        //////////////////////////////////////////////////////////////////////////
        // writing-mode: lr-tb | rl-tb | tb-lr | tb-rl
        //////////////////////////////////////////////////////////////////////////
        case Property_WritingMode:
            return false;       // Not implemented
    }

    return false;
}


void TextStyleReader::AddStyleAlias(const char8_t* pStyleNameAlias, uint32_t nStyleID)
{
    mStyleNameIdMap[pStyleNameAlias] = nStyleID;
}


bool TextStyleReader::ReadStyle(char8_t* pStyleName, int32_t nStyleNameCapacity, uint32_t& nStyleID, TextStyle& style) 
{
    while (MatchToken(TokenAt)) 
    {
        eastl::fixed_string<Char, 256, true>        buffer;       // buffer look like this: "abc\0123\defghi\0"
        eastl::fixed_vector<eastl_size_t, 16, true> argPosArray;

        while (!MatchToken(TokenSemicolon)) 
        {
            if (MatchToken(TokenIdent) || MatchToken(TokenString))
            {
                argPosArray.push_back(buffer.size()); // We save positions here and not Char* pointers because buffer could be resized/reallocated during this loop.
                EA::StdC::Strlcat(buffer, mTokenValue.c_str(), mTokenValue.length());
                buffer += (Char)0;
            } 
            else 
            {
                Fatal(kErrorSemicolonExpected);
                return false;
            }
        }

        if (mpCallback)
        {
            eastl::fixed_vector<const Char*, 16, true> argArray;

            for(eastl_size_t i = 0; i < argPosArray.size(); i++)
            {
                const Char* pArg = &buffer[argPosArray[i]];
                argArray.push_back(pArg);
            }

            (*mpCallback)((int32_t)argArray.size(), argArray.data(), mpCallbackContext);
        }
    }

    pStyleName[0] = 0;

    if (MatchToken(TokenIdent) || MatchToken(TokenString))
    {
        Strlcpy(pStyleName, mTokenValue.c_str(), nStyleNameCapacity);
        Strlcpy(style.mStyleName, mTokenValue.c_str(), EAArrayCount(style.mStyleName));

        if (MatchToken(TokenOpenParen))
        {
            if (!MatchInteger(nStyleID))
            {
                Fatal(kErrorBadFontID);
                return false;
            }

            if (!MatchToken(TokenCloseParen))
            {
                Fatal(kErrorBadFontID);
                return false;
            }
        }
    }

    bool bFoundParent = false;

    if (MatchToken(TokenColon)) // If the user is specifying a parent style to inherit from...
    {
        uint32_t nParentID = 0;

        if (MatchToken(TokenIdent) || MatchToken(TokenString))
        {
            const NameString sName(mTokenValue.data(), mTokenValue.length());
            tStyleNameMap::const_iterator it = mStyleNameIdMap.find(sName);

            if (it != mStyleNameIdMap.end() && mpManager->GetStyle(it->second, &style) != NULL) {
                style.mParentId = it->second;
                bFoundParent = true;
            }
            else
            {
                Fatal(kErrorBaseFontNotFound);
                return false;
            }
        } 
        else if (MatchInteger(nParentID))
        {
            if (mpManager->GetStyle(nParentID, &style) != NULL)
            {
                style.mParentId = nParentID;
                bFoundParent = true;
            }
            else
            {
                Fatal(kErrorBaseFontNotFound);
                return false;
            }
        }
    }

    if (!bFoundParent)    // If no parent style was found...
        style = TextStyle();

    if (MatchToken(TokenOpenBrace))
    {
        while (!AtEoF())
        {
            if (MatchToken(TokenCloseBrace))
            {
                Strlcpy(style.mStyleName, pStyleName, EAArrayCount(style.mStyleName));
                return true;
            }

            if (!ParseProperty(style))
            {
                Warn(kWarningInvalidOrMissingPropertyValue);
                SkipDeclaration();
                Warn(kWarningInvalidOrMissingPropertyValue);
                continue;
            }
            else if (MatchToken(TokenSemicolon))
                continue;
            else if (mToken != TokenCloseBrace)
            {
                Warn(kWarningInvalidOrMissingPropertyValue);
                SkipDeclaration();
                continue;
            }
        }

        Fatal(kWarningInvalidOrMissingPropertyValue);
        return false;

    }
    else if (mToken == NoToken)
        return false;
    else
        Fatal(kErrorInvalidToken);

    return false;
}


bool TextStyleReader::ReadStyles()
{
    TextStyle     style;
    uint32_t      styleId = kStyleIdUnknown;
    const int32_t kStyleNameCapacity = 32;
    char8_t       styleName[kStyleNameCapacity];

    while(!AtEoF())
    {
        if(!ReadStyle(styleName, kStyleNameCapacity, styleId, style))
            return false;

        styleId = mpManager->AddStyle(styleId, style);

        if(Strlen(styleName) > 0)
            mStyleNameIdMap[styleName] = styleId;
    }

    return true;
}


} // namespace Text

} // namespace EA

#endif // EATEXT_STYLE_MANAGER_ENABLED

