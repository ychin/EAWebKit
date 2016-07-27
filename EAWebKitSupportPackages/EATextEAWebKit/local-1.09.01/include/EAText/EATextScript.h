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
// EATextScript.h
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTSCRIPT_H
#define EATEXT_EATEXTSCRIPT_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{
    namespace Text
    {
        /// Script
        ///
        /// We use the ISO 15924 script identification standard as a list of theoretically supported scripts.
        /// A script is a writing system, usually closely related to a spoken language. Written English is 
        /// denoted by the script called Latin, as are a number of other Western languages, such as Italian,
        /// Spanish, and even simply Latin.
        ///
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


        /// ScriptProperties
        ///
        /// Defines text classification and layout properties of scripts.
        ///
        struct ScriptProperties
        {
            Script   mScript            : 7;    /// The script associated with these properties.
            bool     mbNumericChars     : 1;    /// If true, the script contains only digits and the other characters used in writing numbers by the rules of the Unicode bidirectional algorithm. For example, currency symbols, the thousands separator, and the decimal point are classified as numeric when adjacent to or between digits.
            bool     mbControlChars     : 1;    /// If true, the script contains only control characters.
            bool     mbAmbiguousChars   : 1;    /// If set, the script contains characters that are supported by more than one charset.
            bool     mbPrivateChars     : 1;    /// If true, the script uses characters in the private Unicode range (E000 - F8FF).  
            bool     mbComplex          : 1;    /// If true, the script for the language requires special shaping or layout. If fComplex is false, the script contains no combining characters and requires no contextual shaping or reordering.
            bool     mbRTL              : 1;    /// If true, the script is right-to-left instead of left-to-right.
            bool     mbVertical         : 1;    /// If true, the script -can- be written vertically in addition to horizontally. No modern script uses only vertical orientation, as they have all adapted to horizontal orientation.
            bool     mbWordBreak        : 1;    /// If true, the the script requires non-trivial word breaking analysis. Trival means an algorithm can be made to do it. Thai is an example where an algorithm doesn't work.
            bool     mbClusterCaret     : 1;    /// If true, this language restricts caret placement to cluster boundaries, for example, Thai and Indian. Arabic and Hebrew allow caret to be in the middle of a cluster.
            bool     mbCharacterJustify : 1;    /// If true, justification for the script is achieved by increasing the space between all letters, not only between words. 
            bool     mbUsesInvalidGlyph : 1;    /// If true, this script displays a special character (e.g. a square) when a glyph cannot be displayed (e.g. because the font lacks it).
            bool     mbGrowableClusters : 1;    /// If true, then shaping (usually contextual) may cause the glyph set to increase in count. We assume the increase is not due to some weakness in the font's glyph support but due to a true need to increase the glyph count.
            bool     mbRejectInvalid    : 1;    /// If ture, then invalid char sequences are possible and applications should act upon such characters appropriately. Thai is a script with this property.
        };

        /// GetScriptProperties
        ///
        /// This is a generic script property retrieval function.
        /// The return value is true if the given script is supported.
        /// If the output pScriptProperties is NULL, no attempt is made to write 
        /// to it, but the return value is still valid.
        /// If bForceValid is true, then the return value is always true and the 
        /// pScriptProperties data will always be written to with a best match.
        ///
        EATEXT_API bool GetScriptProperties(Script script, ScriptProperties* pScriptProperties, bool bForceValid);


        /// GetScriptFromLocale
        ///
        /// Converts a locale string to a Script normally used by that locale.
        /// For example, the "en-us" locale converts to kScriptLatin.
        /// If the input pLocale is NULL, the return value is kScriptUnknown.
        /// A given locale may use multiple scripts. The input 'index' defines 
        /// which of these scripts you would like to retrieve. A return value
        /// of kScriptUnknown means that no known script exists for the given 
        /// locale and index pair. There is never any more than three scripts
        /// associated with a locale, as defined by this library.
        /// The input locale is case-insensitive.
        ///
        EATEXT_API Script GetScriptFromLocale(const Char* pLocale, int index = 0); 


        /// LocaleUsesScript
        ///
        /// Returns true if the given locale uses the given script.
        /// If the input pLocale is NULL, the return value is false.
        /// The input locale is case-insensitive.
        ///
        EATEXT_API bool LocaleUsesScript(const Char* pLocale, Script script);
          

        /// GetScriptFromChar
        ///
        /// Given a Unicode character, this function returns what script that character normally belongs to.
        /// Returns kScriptCommon if the character is not associated with any specific script.
        /// This function will not return kScriptUnknown under any condition.
        /// The resulting values are specified as such by the Unicode standard.
        /// Strictly speaking, this function is Unicode character property querying function
        /// and possibly could be grouped with the generic set of Unicode character property functions.
        ///
        EATEXT_API Script GetScriptFromChar(Char c);


        /// GetLayoutScriptFromChar
        ///
        /// This is like GetScriptFromChar except for some characters it returns a different value.
        /// The reason is that for layout purposes some characters shouldn't be treated the same
        /// as the Unicode Standard labels the character. For example, U+0640 (Arabic Tatweel) is 
        /// labeled as Common script by the Unicode Standard, but it is actually an Arabic modifier
        /// character.
        ///
        EATEXT_API Script GetLayoutScriptFromChar(Char c);


        /// GetLocaleFromScript
        ///
        /// Returns the locale which is most commonly associated with the given script.
        /// If input bForceValid is true, then a valid locale will be returned, regardless of the 
        /// value for script. Otherwise, NULL will be returned for invalid scripts or scripts that
        /// have no associated locale.
        /// The returned locale will be lower-case.
        ///
        EATEXT_API const Char* GetLocaleFromScript(Script script, bool bForceValid);


    } // namespace Text

} // namespace EA


#endif // EATEXT_EATEXTSCRIPT_H






