/*
Copyright (C) 2005, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EALexer.h
// Created by Talin - 2005
//
// A simple, stream-based lexical analyser.
/////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EALEXER_H
#define EATEXT_INTERNAL_EALEXER_H


#include <EABase/eabase.h>
#include <EASTL/string.h>


#ifndef EALEXER_FORCE_INLINE
    #ifdef __GNUC__
        // Must disable this under GCC because GCC errors when not able to implement it.
        #define EALEXER_FORCE_INLINE inline
    #else
        #define EALEXER_FORCE_INLINE EA_FORCE_INLINE
    #endif
#endif


namespace EA
{
    namespace Text
    {
        namespace Lexer {

            //////////////////////////////////////////////////////////////////////////
            /// Equality test expression
            ///
            template<int Value>
            struct Character {
                Character() {}
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return ch == Value;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Match a range of characters.
            ///
            template<int Low, int High>
            struct Range {
                Range() {}

                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return ch >= Low && ch <= High;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Set membership test (size 2)
            ///
            template<int A, int B>
            struct Character2 {
                Character2() {}
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return ch == A || ch == B;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Set membership test (size 3)
            ///
            template<int A, int B, int C>
            struct Character3 {
                Character3() {}
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return ch == A || ch == B || ch == C;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Set membership test (size 4)
            ///
            template<int A, int B, int C, int D>
            struct Character4 {
                Character4() {}
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return ch == A || ch == B || ch == C || ch == D;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Match either of two alternatives
            ///
            template<typename First, typename Second>
            struct Alternative {
                Alternative() {}

                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return First::match(ch) || Second::match(ch);
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Match both conditions
            ///
            template<typename First, typename Second>
            struct Both {
                Both() {}

                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return First::match(ch) && Second::match(ch);
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// The logical inverse of a match
            ///
            template<typename Predicate>
            struct LogicalNot {
                LogicalNot() {}

                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return !Predicate::match(ch);
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// For whitespace, we use EACRT, since the definition of whitespace is'
            /// kind of complicated.
            ///
            struct IsSpace {
                IsSpace();
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return isspace((char16_t)ch) != 0;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Match anything.
            ///
            struct IsAny {
                IsAny();
                static EALEXER_FORCE_INLINE bool match(int /*ch*/) {
                    return true;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Match any printable character.
            ///
            struct IsPrint {
                IsPrint();
                static EALEXER_FORCE_INLINE bool match(int ch) {
                    return isprint((char16_t)ch) != 0;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Generic wrapper around an match expression so that we don't have to
            /// overload everything.
            ///
            template<typename Predicate>
            struct MatchExpression {
                typedef Predicate pred;
                MatchExpression() {}
            };

            //////////////////////////////////////////////////////////////////////////
            /// Similar to a match expression, except that it doesn't record anything.
            ///
            template<typename Predicate>
            struct SkipExpression {
                typedef Predicate pred;
                SkipExpression() {}
            };

            //////////////////////////////////////////////////////////////////////////
            /// Wrapper around the equality test.
            ///
            template <int Value>
            struct MatchChar : public MatchExpression< Character< Value > > {};

            //////////////////////////////////////////////////////////////////////////
            /// Wrapper around the range test
            ///
            template <int Low, int High>
            struct MatchRange : public MatchExpression< Range< Low, High > > {};

            //////////////////////////////////////////////////////////////////////////
            /// Wrapper around the character set test
            ///
            template <int A, int B, int C=0, int D=0>
            struct MatchAnyOf : public MatchExpression< Character4< A, B, C, D > > {};

            template <int A, int B, int C>
            struct MatchAnyOf<A,B,C,0> : public MatchExpression< Character3< A, B, C > > {};

            template <int A, int B>
            struct MatchAnyOf<A,B,0,0> : public MatchExpression< Character2< A, B > > {};

            //////////////////////////////////////////////////////////////////////////
            /// Wrapper around an alternative (same as Alternative, but unwraps args)
            ///
            template <typename First, typename Second >
            struct MatchEither : public MatchExpression< Alternative< typename First::pred, typename Second::pred > > {};

            //////////////////////////////////////////////////////////////////////////
            /// Overload of '|' operator to combine alternatives
            ///
            template<typename First, typename Second>
            MatchExpression< Alternative< First, Second > > operator|(
                const MatchExpression<First> & /*first*/,
                const MatchExpression<Second> & /*second*/)
            {
                return MatchExpression< Alternative< First, Second > >();
            }

            //////////////////////////////////////////////////////////////////////////
            /// Overload of '&' operator 
            ///
            template<typename First, typename Second>
            MatchExpression< Both< First, Second > > operator&(
                const MatchExpression<First> & /*first*/,
                const MatchExpression<Second> & /*second*/)
            {
                return MatchExpression< Both< First, Second > >();
            }

            //////////////////////////////////////////////////////////////////////////
            /// Overload of '!' operator
            ///
            template<typename Predicate>
            MatchExpression< LogicalNot< Predicate > > operator!(const MatchExpression<Predicate> & /*pred*/) {
                return MatchExpression< LogicalNot< Predicate > >();
            }

            //////////////////////////////////////////////////////////////////////////
            /// Wraps an expression, indicating we want to skip it...
            ///
            template<typename Predicate>
            SkipExpression< Predicate > Skip(const MatchExpression<Predicate> &pred) {
                return SkipExpression< Predicate >();
            }

            //////////////////////////////////////////////////////////////////////////
            /// Some commonly-used predicates. Note that these should, in general, produce
            /// no instance data and no run-time initialization.
            ///
            static MatchRange<'a','z'>      LowerCase;
            static MatchRange<'A','Z'>      UpperCase;
            static MatchRange<'0','9'>      Digit;
            static MatchChar<' '>           Space;
            static MatchChar<'\n'>          Newline;
            static MatchChar<'\r'>          Return;
            static MatchExpression< IsSpace > Whitespace;
            static MatchExpression< IsAny > Any;
            static MatchExpression< IsPrint > Printable;

            //////////////////////////////////////////////////////////////////////////
            /// Generic text stream reader class.
            ///
            template<typename Char>
            class CharStream {
            public:
                typedef Char                                    char_type;
                typedef eastl::basic_string<Char>               string_type;
                typedef typename string_type::iterator          iterator;
                typedef typename string_type::const_iterator    const_iterator;
                typedef uintptr_t                               position_type;

                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                CharStream()
                    : mScanPos(NULL)
                    , mScanEnd(NULL)
                {}

                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                CharStream(const_iterator pBegin) {
                    const_iterator pEnd = pBegin;
                    while (*pEnd) 
                        ++pEnd;
                    SetInput(pBegin, pEnd);
                }
    
                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                CharStream(const_iterator pBegin, const_iterator pEnd) {
                    SetInput(pBegin, pEnd);
                }

                //////////////////////////////////////////////////////////////////////////
                /// Set the address of the input buffer
                ///
                void SetInput(const_iterator pBegin, const_iterator pEnd) {
                    mScanPos = pBegin;
                    mScanEnd = pEnd;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Current scan position in the input stream.
                ///
                position_type Position() const {
                    return (position_type)mScanPos;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Read function. Reads the current character and returns it.
                /// Returns 0 if we are at the end of the stream.
                ///
                char_type Current() const {
                    return *mScanPos;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Lookahead function. Given a lookahead index, returns either the Nth
                /// character after the current read position, or 0 if such a character
                /// would be past the end of file.
                ///
                char_type LA(int nIndex) const {
                    return ((mScanPos + nIndex) < mScanEnd) ? mScanPos[nIndex] : 0;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Advance to the next character.
                ///
                void Advance() {
                    ++mScanPos;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Advance N characters ahead
                ///
                void Advance(int count) {
                    mScanPos += count;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Read function. Reads the current character and returns it.
                /// Returns 0 if we are at the end of the stream.
                ///
                char_type Read() {
                    return (mScanPos < mScanEnd) ? *mScanPos++ : 0;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Return true if we're at the end of the stream.
                ///
                bool AtEoF() const {
                    return (mScanPos >= mScanEnd);
                }

                //////////////////////////////////////////////////////////////////////////
                /// Return true if there's more characters to read.
                ///
                bool More() const {
                    return (mScanPos < mScanEnd);
                }

                //////////////////////////////////////////////////////////////////////////
                /// Returns the line starting at the specified position
                ///
                bool GetLine(position_type line_start, char_type* pBuffer, size_t nBufferLength)
                {
                    if (line_start != 0)
                    {
                        char_type* linePtr = (char_type*)line_start;
                        size_t i;
                        for (i = 0; i+1 < nBufferLength; i++)
                        {
                            char_type c = linePtr[i];
                            if (c == '\r' || c == '\n')
                                break;
                            pBuffer[i] = c;
                        }
                        pBuffer[i] = 0;
                        return true;
                    }
                    return false;
                }

            protected:
                const_iterator mScanPos;           /// Current scan position
                const_iterator mScanEnd;           /// End of input text
            };


            //////////////////////////////////////////////////////////////////////////
            /// Base class for lexical analyzers. For an example of how to use,
            /// See the SimpleLexer class defined below.
            ///
            template<typename Char, typename Stream = CharStream<Char> >
            class TokenReader {
            public:
                typedef Char                            char_type;
                typedef eastl::basic_string<Char>       string_type;
                typedef typename Stream::iterator       iterator;
                typedef typename Stream::const_iterator const_iterator;
                typedef uintptr_t                       position_type;

                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                TokenReader()
                    : mStream()
                    , mLineBegin(0)
                    , mCurrentLine(1)     // Line number starts with 1 for historical reasons
                {}

                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                TokenReader(const_iterator pBegin)
                    : mStream(pBegin)
                    , mLineBegin(mStream.Position())
                    , mCurrentLine(1)     // Line number starts with 1 for historical reasons
                {
                }
    
                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                TokenReader(const_iterator pBegin, const_iterator pEnd)
                    : mStream(pBegin, pEnd)
                    , mLineBegin(mStream.Position())
                    , mCurrentLine(1)     // Line number starts with 1 for historical reasons
                {
                }

                //////////////////////////////////////////////////////////////////////////
                /// Set the address of the input buffer
                ///
                void SetInput(const_iterator pBegin, const_iterator pEnd) {
                    mStream.SetInput(pBegin, pEnd);
                    mLineBegin = (position_type)pBegin;
                    mCurrentLine = 1;     // Line number starts with 1 for historical reasons
                }

                //////////////////////////////////////////////////////////////////////////
                /// Current scan position in the input stream.
                ///
                position_type Position() const 
                    { return mStream.Position(); }

                //////////////////////////////////////////////////////////////////////////
                /// Read function. Reads the current character and returns it.
                /// Returns 0 if we are at the end of the stream.
                ///
                char_type Read() 
                    { return mStream.Read(); }

                //////////////////////////////////////////////////////////////////////////
                /// Return true if we're at the end of the stream.
                ///
                bool AtEoF() const 
                {
                    return mStream.AtEoF();
                }

                //////////////////////////////////////////////////////////////////////////
                /// Called by match functions to increment the current line number.
                ///
                void LineBreak() 
                {
                    mLineBegin = mStream.Position();
                    mCurrentLine++;
                }

                //////////////////////////////////////////////////////////////////////////
                /// The line number of the current read position. Note that this may be
                /// different than the line number of the current token.
                ///
                int32_t Line() const 
                    { return mCurrentLine; }

                //////////////////////////////////////////////////////////////////////////
                /// The read position of the start of the currently parsed line.
                ///
                position_type LineStart() const 
                    { return mLineBegin; }

                //////////////////////////////////////////////////////////////////////////
                /// Append the character to the matched text value.
                ///
                void Append(Char c) 
                    { mMatchText.push_back(c); }

                //////////////////////////////////////////////////////////////////////////
                /// Simple matching function. If the character at the current read position
                /// is equal to 'c', then advance the read position and return true.
                /// The matched character is also appended to the token value string.
                ///
                bool Match(uint32_t c) {
                    if (mStream.More() && (c == (uint32_t)mStream.Current())) {
                        Append(mStream.Current());
                        mStream.Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Predicate matching function. If the character at the current read
                /// position satisfies the specified predicate, then then advance the
                /// read position and return true. The matched character is also
                /// appended to the token value string.
                ///
                template<class Predicate>
                bool Match(const MatchExpression<Predicate> & /*test*/) 
                {
                    if (mStream.More() && Predicate::match(mStream.Current())) 
                    {
                        Append(mStream.Current());
                        mStream.Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Takes a sequence of characters as a string. Match if the next characters
                /// in the stream are equal to the sequence.
                ///
                template<class C>
                bool Match(C *pMatchStr) 
                {
                    int nIndex;
                    for (nIndex = 0; pMatchStr[nIndex] != '\0'; nIndex++) {
                        if (pMatchStr[nIndex] != mStream.LA(nIndex))
                            return false;
                    }
                    while (nIndex--) {
                        Append(mStream.Current());
                        mStream.Advance();
                    }
                    return true;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Match all characters that satisfy the specified predicate, and
                /// append them to the current token.
                ///
                template<class Predicate>
                void MatchWhile(const MatchExpression<Predicate> & /*test*/) 
                {
                    while (mStream.More() && Predicate::match(mStream.Current())) {
                        Append(mStream.Current());
                        mStream.Advance();
                    }
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip over the current character unconditionally.
                ///
                bool Skip() 
                {
                    if (mStream.More()) {
                        mStream.Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip over the character at the current read position if it is 'c'.
                /// (Skipping is the same as matching, except that the character is not
                /// recorded in the match string.)
                ///
                bool Skip(uint32_t c) 
                {
                    if (mStream.More() && (c == (uint32_t)mStream.Current())) {
                        mStream.Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip over the specific character sequence.
                ///
                template<class C>
                bool Skip(C *pMatchStr) 
                {
                    int nIndex;
                    for (nIndex = 0; pMatchStr[nIndex] != '\0'; nIndex++) {
                        if (pMatchStr[nIndex] != mStream.LA(nIndex))
                            return false;
                    }
                    mStream.Advance(nIndex);
                    return true;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip until the specified character sequence is encountered, or end
                /// of file is reached.
                ///
                template<class C>
                bool SkipUntil(C *pMatchStr)
                {
                    do {
                        if (Skip(pMatchStr)) 
                            return true;
                    } while (SkipLineBreak() || Skip()) ;

                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip until the specified predicate is matched, or end
                /// of file is reached.
                ///
                template<class Predicate>
                bool SkipUntil(const MatchExpression<Predicate>& /*test*/) 
                {
                    do {
                        if (Predicate::match(mStream.Current()))
                            return true;
                    } while (SkipLineBreak() || Skip()) ;

                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// If the character at the current read position satisfies the specified
                /// predicate, then then advance the read position and return true.
                ///
                template<class Predicate>
                bool Skip(const MatchExpression<Predicate>& /*test*/) 
                {
                    if (mStream.More() && Predicate::match(mStream.Current())) {
                        mStream.Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip all characters that satisfy the specified predicate.
                ///
                template<class Predicate>
                bool SkipWhile(const MatchExpression<Predicate> & /*test*/) 
                {
                    bool bResult = false;
                    while (mStream.More() && Predicate::match(mStream.Current())) {
                        bResult = true;
                        mStream.Advance();
                    }
                    return bResult;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip over a linebreak. Provided here because it is used fairly commonly.
                /// This updates the current line count.
                ///
                bool SkipLineBreak() {
                    if (Skip(Return)) {
                        Skip(Newline);
                        LineBreak();
                        return true;
                    }
                    else if (Skip(Newline)) {
                        LineBreak();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip to the end of the line
                ///
                void SkipToEoL() {
                    while (!SkipLineBreak() && Skip()) 
                    { }
                }

                //////////////////////////////////////////////////////////////////////////
                /// Skip whitespace
                bool SkipWS()
                {
                    bool bResult = false;
                    while (SkipLineBreak() || Skip(Whitespace))
                        bResult = true;
                    return bResult;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Returns the string value of the current match.
                ///
                string_type &MatchText() {
                    return mMatchText;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Return the input stream
                Stream & InputStream()
                    { return mStream; }

            protected:
                Stream              mStream;            /// Text input stream
                position_type       mLineBegin;         /// Start of the current line
                int32_t             mCurrentLine;       /// Current line number
                string_type         mMatchText;         /// Current token value (text)
            };

            //////////////////////////////////////////////////////////////////////////
            /// Utility function to parse C++-style escape sequences
            ///
            /// Todo: Add support for hex, octal character sequences
            /// Todo: Add support for unicode chars
            template<typename TokenReader>
            static EALEXER_FORCE_INLINE bool ParseEscape(TokenReader& l)
            {
                if (l.Skip('\\')) 
                {
                    typename TokenReader::char_type c = l.Read();
                    switch (c) 
                    {
                        case 'b': l.Append('\b'); break;
                        case 't': l.Append('\r'); break;
                        case 'r': l.Append('\r'); break;
                        case 'f': l.Append('\f'); break;
                        case 'n': l.Append('\n'); break;
                        case 'v': l.Append('\v'); break;
                        case '0': l.Append('\0'); break;
                        case '\\':l.Append('\\'); break;
                        default:
                            l.Append('\\');
                            l.Append(c);
                            break;
                    }
                    return true;
                }
                return false;
            }

            //////////////////////////////////////////////////////////////////////////
            /// Parse escaped string. Consumes characters until the closing delimeter
            /// has been read.
            ///
            template<typename TokenReader>
            bool ParseEscapedString(TokenReader& reader, typename TokenReader::char_type endQuote) 
            {
                for (;;) {
                    if (reader.Skip((uint32_t)endQuote))
                        return true;
                    if (ParseEscape(reader))
                        continue;
                    if (!reader.Match(Any))
                        return false;
                }
            }

            //////////////////////////////////////////////////////////////////////////
            /// Utility function to parse C++-style single-line comment
            ///
            template<typename TokenReader>
            static EALEXER_FORCE_INLINE bool ParseCPPComment(TokenReader& l) 
            {
                // Single-line comment
                if (l.Skip("//")) {
                    l.SkipToEoL();
                    return true;
                }

                // Multi-line comment
                if (l.Skip("/*")) {
                    if (!l.SkipUntil("*/")) {
                        // Error unterminated comment
                    }
                    return true;
                }
                return false;
            }

            //////////////////////////////////////////////////////////////////////////
            /// A lexer for C-style languages
            ///
            template<typename Char>
            class SimpleLexer 
            {
            public:

                enum Token
                {
                    TokenEnd,
                    TokenIdent,
                    TokenInteger,
                    TokenFloat,
                    TokenStringLiteral,
                    TokenCharLiteral,

                    // Tokens for punctuation are the same integer value as
                    // the character that was matched.
                    TokenOpenBrace = '{',
                    TokenCloseBrace = '}',

                    // Error tokens
                    TokenErrorBase = -1000,
                    TokenErrorInvalidChar,
                    TokenErrorUnterminatedString,
                };

                typedef TokenReader<Char> reader_type;
                typedef typename reader_type::const_iterator const_iterator;
                typedef typename reader_type::char_type char_type;
                typedef typename reader_type::string_type string_type;
                typedef typename reader_type::position_type position_type;

                //////////////////////////////////////////////////////////////////////////
                /// Constructor
                ///
                SimpleLexer(const_iterator pBegin, const_iterator pEnd)
                    : mReader(pBegin, pEnd)
                {
                    Advance();
                }

                //////////////////////////////////////////////////////////////////////////
                /// Returns the type of the current token
                ///
                Token TokenType() const {
                    return mToken;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Returns the value of the token that was just matched.
                ///
                const string_type &TokenValue() const {
                    return mTokenValue;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Returns the line number of the most recently matched token
                ///
                int32_t TokenLine() const 
                    { return mTokenLine; }

                //////////////////////////////////////////////////////////////////////////
                /// Indicate that we have completely parsed a token. This corresponds to
                /// an "acceptor" state in a DFA.
                ///
                void Accept(Token token) 
                    { mToken = token; }

                //////////////////////////////////////////////////////////////////////////
                /// Match vs. the current token type
                ///
                bool Match(Token token) 
                {
                    if (mToken == token) {
                        // Store the matched text value into the current token value.
                        swap(mReader.MatchText(), mTokenValue);
                        Advance();
                        return true;
                    }
                    return false;
                }

                //////////////////////////////////////////////////////////////////////////
                /// Advance to the next token
                ///
                bool Advance() 
                {
                    static MatchRange<'a','f'>  HexLower;
                    static MatchRange<'A','F'>  HexUpper;
                    static MatchChar<'_'>       Underscore;
                    static MatchAnyOf<'+','-'>  Sign;
                    static MatchAnyOf<'E','e'>  Exponent;

                    mReader.MatchText().clear();
                    mTokenPos = mReader.Position();
                    mTokenLine = mReader.Line();
                    mTokenLineStart = mReader.LineStart();

                    while (!mReader.AtEoF()) 
                    {
                        // Whitespace
                        if (mReader.SkipWS()) 
                            continue;

                        if (ParseCPPComment(mReader))
                            continue;

                        // Identifier
                        if (mReader.Match(LowerCase | UpperCase | Underscore)) {
                            mReader.MatchWhile(LowerCase | UpperCase | Digit | Underscore);
                            Accept(TokenIdent);
                            return true;
                        }

                        // Hex numbers
                        if (mReader.Match("0x") || mReader.Match("0X")) {
                            mReader.MatchWhile(Digit | HexLower | HexUpper);
                            Accept(TokenInteger);
                            return true;
                        }

                        // Number
                        if (mReader.Match(Digit)) {
                            mReader.MatchWhile(Digit);        // Grab all digits
                            Accept(TokenInteger);

                            // Check for decimal point
                            if (mReader.Match('.')) {
                                mReader.MatchWhile(Digit);    // Push digits after decimal point
                                Accept(TokenFloat);
                            }

                            // Check for exponent
                            if (mReader.Match(Exponent)) {
                                mReader.Match(Sign);          // Check for exponent sign
                                mReader.MatchWhile(Digit);    // Push digits after exponent
                                Accept(TokenFloat);
                            }
                            return true;
                        }

                        // Quoted string
                        if (mReader.Skip('\"')) {
                            if (ParseEscapedString(mReader, '\"')) {
                                Accept(TokenStringLiteral);
                                return true;
                            }
                            Accept(TokenErrorUnterminatedString);
                        }

                        if (mReader.Skip('\'')) {
                            if (ParseEscapedString(mReader, '\'')) {
                                Accept(TokenCharLiteral);
                                return true;
                            }
                            Accept(TokenErrorUnterminatedString);
                        }

                        // Must be an operator or something
                        // For now, just return the single char as a token
                        char_type c = mReader.Read();
                        if (Isprint(c)) {
                            mToken = (Token)c;
                            mReader.Append(c);
                            return true;
                        }

                        Accept(TokenErrorInvalidChar);
                        return false;
                    }

                    return TokenEnd;
                }

            private:
                reader_type   mReader;
                Token         mToken;             /// Current token type
                int32_t       mTokenLine;         /// Line number at start of token
                position_type mTokenLineStart;    /// Start of line which token is on
                position_type mTokenPos;          /// Start of the current token
                string_type   mTokenValue;        /// Current token value (text)
            };


            //////////////////////////////////////////////////////////////////////////
            /// Another utility class that's useful for lexer creation.
            /// This one stores a name/value pair that can be looked up
            /// quickly via bsearch.
            ///
            template<typename Char, typename Value>
            struct StaticDefinition
            {
                typedef Char                char_type;
                typedef Value               value_type;
                typedef StaticDefinition    this_type;

                const char_type* mName;
                value_type       mValue;

                friend bool operator<(const this_type& sd, const char_type* b)
                {
                    const char_type* a = sd.mName;

                    while (*a && *a == *b) 
                        { a++, b++; }
                    return (*a < *b) ? true : false;
                }

                friend bool operator<(const char_type* a, const this_type& sd)
                {
                    const char_type* b = sd.mName;

                    while (*a && *a == *b) 
                        { a++, b++; }
                    return (*a < *b) ? true : false;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Similar to above, but uses case-insensitive search.
            ///
            template<typename Char, typename Value>
            struct IStaticDefinition {
                typedef Char                char_type;
                typedef Value               value_type;
                typedef IStaticDefinition   this_type;

                const char_type* mName;
                value_type       mValue;

                friend bool operator<(const this_type& sd, const char_type* b)
                {
                    using namespace eastl;
                    const char_type* a = sd.mName;

                    while (*a && CharToLower(*a) == CharToLower(*b)) 
                        { a++, b++; }
                    return (*a < *b) ? true : false;
                }

                friend bool operator<(const char_type* a, const this_type& sd)
                {
                    using namespace eastl;
                    const char_type *b = sd.mName;
                    while (*a && CharToLower(*a) == CharToLower(*b)) 
                        { a++, b++; }
                    return (*a < *b) ? true : false;
                }
            };

            //////////////////////////////////////////////////////////////////////////
            /// Does a bsearch on a sorted array of names, to find the associated
            /// value.
            ///
            template<typename T, int N>
            bool Lookup(
                const T (&Table)[N],
                const typename T::char_type *pName,
                typename T::value_type &result)
            {
                eastl::pair< const T*, const T* > found =
                    eastl::equal_range(&Table[0], &Table[N], pName);

                if (found.first + 1 == found.second) {
                    result = found.first->mValue;
                    return true;
                } 

                return false;
            }

            //////////////////////////////////////////////////////////////////////////
            /// Does a bsearch on a sorted array of names, to find the associated
            /// value.
            ///
            template<typename T, int N>
            const typename T::value_type &LookupOrFallback(
                const T (&Table)[N],
                const typename T::char_type*  pName,
                const typename T::value_type& failResult)
            {
                eastl::pair< const T*, const T* > result = eastl::equal_range(&Table[0], &Table[N], pName);

                if (result.first + 1 == result.second)
                    return result.first->mValue;

                return failResult;
            }

        }
    }
}

#endif





