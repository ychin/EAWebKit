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
// EATextStyleReader.h
//
// Written by Talin. Maintained by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// EATextStyleReader parses a CSS font style to one or more TextStyle entries.
// 
// Style format:
//    See http://www.w3.org/TR/2002/WD-css3-fonts-20020802/
//       and
//    See http://www.w3.org/TR/2003/CR-css3-text-20030514/
//
//    We support standard CSS3 syntax, with two extensions: 
//       - A hex number can be supplied to set an numerical id for the style. 
//         If it is not present, an id will be generated from the hash of 
//         the style name.
//       - Inheritance is supported whereby a given style can inherit and 
//         override from a parent.
//
//    See the example below for the syntax of these extensions.
//
//    Otherwise, we support the conventional CSS style spec to a decent degree, 
//    but not rigorously in all cases, for a number of reasons. We don't try 
//    too hard to be extremely flexible with the syntax, as that would increase 
//    the code size of the implementation. For example, the opening
//    brace '{' after the style name must be on the same line as the style name.
//    Shorthand font specification (e.g. font: italic small-caps bold 12px arial) 
//    is not supported, as there are too many ambiguities to deal with to make 
//    it worth our effort. 
//
// Supported tags (items marked by '*' are not standard CSS tags):
//    font                          // font: [ <font-style> || <font-variant> || <font-weight> ]? <font-size> <font-family>
//    font-family                   // font-family: [[ <family-name> | <generic-family> ] [, <family-name>| <generic-family>]* ]
//    font-size                     // font-size: [ xx-small | x-small | small | medium | large | x-large | xx-large ] 
//                                  // font-size: [ larger | smaller ]
//                                  // font-size: [ length | percentage ]
//    font-style                    // font-style: normal | italic | oblique
//    font-weight                   // font-weight: normal | bold | bolder | lighter | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900
//    font-variant                  // font-variant: normal | small-caps
//    font-pitch                    // font-pitch: fixed | variable
//    font-smooth                   // font-smooth: auto | always | never
//    font-effect                   // font-effect: none | effectID <hex guid id> | effectName <string> | [[ outline | shadow | raised | depressed ] xval yval (#<AARRGGBB> | #<RRGGBB>) [(#<AARRGGBB> | #<RRGGBB>)]]
//    font-stretch                  // font-stretch: <length> | normal
//    color                         // color: #<AARRGGBB> | #<RRGGBB>
//    background-color              // background-color: #<AARRGGBB> | #<RRGGBB> | transparent
//    text-decoration               // text-decoration: none | [ underline || overline || line-through || blink ]
//    writing-mode                  // writing-mode: [lr-tb] | [rl-tb] | [tb-lr] | [tb-rl]
//    text-align                    // text-align: left | right | center | justify
//    vertical-align                // vertical-align: top | middle | bottom 
//    text-justify                  // text-justify: inter-word | inter-ideograph | distribute | newspaper | inter-cluster | kashida
//    wrap-option                   // wrap-option: wrap | no-wrap | soft-wrap | hard-wrap | emergency
//    whitespace                    // whitespace:  normal | pre
//    text-overflow-mode            // text-overflow-mode: none | clip | ellipsis
//    font-emphasize-style          // font-emphasis-style: none | accent | dot | circle | disc
//    font-emphasize-position       // font-emphasis-position: before | after
//   *digit-substitution            // digit-substitution: <length>
//   *password-mode                 // password-mode: password | normal
//    letter-spacing                // letter-spacing: <length> | normal
//    word-spacing                  // word-spacing: <length> | normal
//    line-spacing                  // line-spacing: <length> | normal
//
// Also, there is a mechanism whereby the user can embed custom information into the 
// file via the '@' statement. Any line which begins with '@' is considered to contain
// information special to the user and the user can act on it as he chooses.
// The primary use for this is so that the user can specify font files to load which 
// happen to support the given styles specified in the file.
//
// Example style specification:
//    // Base font specification.
//    MainFont (0x134ad5f){
//        font-family: "Helvetica Neue", arial;
//        font-size:   10px;
//    }
//    
//    // Inherits from MainFont
//    DialogTitle (0xacbf4564) : MainFont{
//        font-size:   14px;
//        font-style:  italic;
//        color:       #ff00ff;
//        font-effect: outline 2 2 #0000ff #000000;     // or font-effect: effectID 0x3a4a54ac;
//    }
//
// Example usage:
//    TextStyleReader tsr;
//    char8_t         styleName[32];
//    uint32_t        styleId;
//    TextStyle       style;
//
//    while(tsr.ReadStyle(styleName, 32, styleId, style))
//    {
//        // Do something with styleName, styleId, style.
//    }
//
///////////////////////////////////////////////////////////////////////////////



#ifndef EATEXT_EATEXTSTYLEREADER_H
#define EATEXT_EATEXTSTYLEREADER_H

#include <EAText/EATextConfig.h>

#if EATEXT_STYLE_MANAGER_ENABLED

#include <EAText/EATextStyle.h>
#include <EAText/internal/EALexer.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_string.h>


#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Text
    {
        // Stylesheet token definitions
        enum TextStyleToken
        {
            NoToken = 0,                 // Returned when no keyword is encountered
            TokenError,                  // An error token which matches nothing

            // standard terminals
            TokenWS,                     // Whitespace
            TokenIdent,                  // An identifier
            TokenInteger,                // An integer
            TokenHexInteger,             // An integer
            TokenFloat,                  // A number
            TokenString,                 // A quoted string

            // delimiters and punc
            TokenPound,                  // Pound sign
            TokenAt,                     // At-sign
            TokenColon,                  // Colon
            TokenSemicolon,              // Semicolon
            TokenOpenParen,              // (
            TokenCloseParen,             //)
            TokenOpenBrace,              // {
            TokenCloseBrace,             // }
            TokenComma,                  // ,
            TokenPlus,                   // +
            TokenPercent                 // %
        };


        enum TextStyleResultCode
        {
            kResultSuccess = 0,

            kErrorBase                  = 0x02600000,
            kErrorInvalidChar,
            kErrorInvalidToken,
            kErrorUnterminatedString,
            kErrorBadColorFormat,
            kErrorBadFontID,
            kErrorBaseFontNotFound,
            kErrorSemicolonExpected,
            kWarningUnknownProperty,
            kWarningInvalidOrMissingPropertyValue
        };


        //////////////////////////////////////////////////////////////////////////
        /// TextStyleReader
        ///
        /// Reads CSS text style information from a string of file data.
        ///
        class EATEXT_API TextStyleReader : public EA::Text::Lexer::TokenReader<char8_t>
        {
        public:
            typedef Lexer::TokenReader<char8_t>  reader_type;
            typedef reader_type::const_iterator  const_iterator;
            typedef reader_type::char_type       char_type;
            typedef reader_type::string_type     string_type;
            typedef reader_type::position_type   position_type;

            // Defines a type for the callback function that is called when you register a custom
            // '@' command with SetCommandCallback. See SetCommandCallback for example usage.
            typedef void (*tCommandCallback)(int32_t argc, const Char** argv, void* pContext);

            /// The error callback is called for each warning or error encountered.
            typedef void (*tErrorCallback)(TextStyleResultCode cpde, uint32_t line, void* pContext);

            /// Constructors
            TextStyleReader(StyleManager* pManager);
            TextStyleReader(StyleManager* pManager, const_iterator pBegin);
            TextStyleReader(StyleManager* pManager, const_iterator pBegin, const_iterator pEnd);

            /// Error result code.
            TextStyleResultCode GetResult() const;

            /// Read the next style
            bool ReadStyle(char8_t* pStyleName, int32_t nStyleNameCapacity, uint32_t& nStyleID, TextStyle& style);

            /// Read all styles until EoF
            bool ReadStyles();

            /// Define an alternate name for a specific style
            void AddStyleAlias(const char8_t* pStyleNameAlias, uint32_t nStyleID);

            int32_t       CurrentLine() const;
            int32_t       CurrentColumn() const;
            position_type CurrentLinePtr() const;
            uint32_t      GetWarningCount() const { return mWarningCount; }

            // Specifies a type for the callback function that is called when you register a custom
            // '@' command with SetCommandCallback. See SetCommandCallback for example usage.
            //
            // Example usage:
            //    TextStyle file:
            //        @font arial*.ttf;
            //        @effect "SuperShadow" "SetGlyphColor 0xff061FB5\tSetGlyphSmooth 1 ...";
            //        
            //        MainStyle(0x0134ad5f){
            //            font-family : courier, arial;
            //            font-size: 32px;
            //        }
            //        
            //    void CommandCallback(int32_t argc, const Char** argv, void* pContext) {
            //        // argv[0] is the command name, such as "font"
            //    }
            //
            //    mTextStyleReader.SetCommandCallback(CommandCallback, this);
            //
            void SetCommandCallback(tCommandCallback pCallback, void* pContext);

            // Specifies a callback function which is called when there is a parsing error.
            void SetErrorCallback(tErrorCallback pCallback, void* pContext);

        protected:
            void Warn(TextStyleResultCode nWarning);
            void Fatal(TextStyleResultCode nError);
            void Report(TextStyleResultCode nCode);
            bool Advance();
            void SkipDeclaration();
            bool SkipSpace();
            bool MatchInteger(uint32_t& result);
            bool MatchFloat(float& result);
            bool MatchString(char_type endQuote);
            bool MatchToken(TextStyleToken token);
            bool MatchLength(float& result, float normal);
            template<typename T, int N>
            bool MatchSymbol(const T (&Table)[N], typename T::value_type& result);
            bool MatchSymbol(const char8_t* pSymbol);
            bool MatchColor(Color& color);

            // Parsing functions for specific style properties
            bool ParseFontFamilies(TextStyle& style);
            bool ParseFontSize(TextStyle& style);
            bool ParseProperty(TextStyle& style);
            bool ParseFontEffect(TextStyle& style);

            /// NameString
            typedef eastl::fixed_string<char, 12, true> NameString;

            typedef eastl::fixed_hash_map< NameString, uint32_t, 15, 16, true, eastl::string_hash<NameString> > tStyleNameMap;

        protected:
            StyleManager*       mpManager;          /// Style manager
            TextStyleToken      mToken;             /// Current token type
            int32_t             mTokenLine;         /// Line number at start of token
            position_type       mTokenLineStart;    /// The read position of the start of the currently parsed line.
            position_type       mTokenPos;          /// Thre read position of the current token of the currently parsed line.
            string_type         mTokenValue;        /// Current token value (text)
            tStyleNameMap       mStyleNameIdMap;    /// Records stylename/id pairs.
            tCommandCallback    mpCallback;         /// Command callback
            void*               mpCallbackContext;  /// Command callback context
            tErrorCallback      mpErrorCallback;    /// Error callback
            void*               mpErrorContext;     /// Error callback context
            TextStyleResultCode mResult;
            uint32_t            mWarningCount;      /// Number of warnings

        }; // class TextStyleReader

    } // namespace Text

} // namespace EA




///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace Text
    {

        inline TextStyleResultCode TextStyleReader::GetResult() const
        {
            return mResult;
        }

        inline int32_t TextStyleReader::CurrentLine() const
        {
            return mTokenLine;
        }

        inline int32_t TextStyleReader::CurrentColumn() const
        {
            return (int32_t)(mTokenPos - mTokenLineStart);
        }

        inline TextStyleReader::position_type TextStyleReader::CurrentLinePtr() const
        {
            return mTokenLineStart;
        }

        inline void TextStyleReader::SetCommandCallback(tCommandCallback pCallback, void* pContext)
        {
            mpCallback        = pCallback;
            mpCallbackContext = pContext;
        }

        inline void TextStyleReader::SetErrorCallback(tErrorCallback pCallback, void* pContext)
        {
            mpErrorCallback = pCallback;
            mpErrorContext  = pContext;
        }

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_STYLE_MANAGER_ENABLED

#endif // Header sentry

