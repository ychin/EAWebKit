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
// EATextScript.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextScript.h>
#include <stddef.h>
#include <string.h>


namespace EA
{

namespace Text
{


#if (EATEXT_SCRIPT_PROPERTY_SUPPORT > 1)

    // Todo: Complete this table. It is not accurate for all entries.
    ScriptProperties gScriptProperties[] = 
    {
        // mScript                   mbNumericChars               mbComplex                  mbClusterCaret              mbRejectInvalid
        //                                  mbControlChars              mbRTL                       mbCharacterJustify
        //                                         mbAmbiguousChars            mbVertical                  mbUsesInvalidGlyph  
        //                                                mbPrivateChars              mbWordBreak                 mbGrowableClusters
        // -----------------------------------------------------------------------------------------------------------------
        { kScriptCommon,             false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // * (* here means this entry is correct to our knowledge)
        { kScriptInherited,          false, false, false, false, true , false, false, false, true,  false, true,  false, false }, // *
        { kScriptArabic,             false, false, false, false, false, true,  false, false, false, true,  true,  true , false }, // *
        { kScriptArmenian,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptBengali,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptBraille,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptBopomofo,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptBuhid,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptCanadianAboriginal, false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptCherokee,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptCoptic,             false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptCypriot,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptCyrillic,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // *
        { kScriptDeseret,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptDevanagari,         false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptEthiopic,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptGeorgian,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptGothic,             false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptGreek,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // *
        { kScriptGujarati,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptGurmukhi,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptHan,                false, false, false, false, false, false, true,  false, true,  false, true,  false, false }, // *
        { kScriptHangul,             false, false, false, false, false, false, true,  false, true,  false, true,  false, false }, // *
        { kScriptHanunoo,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptHebrew,             false, false, false, false, true,  true,  false, false, false, false, true,  false, false }, // *
        { kScriptHiragana,           false, false, false, false, false, false, true,  false, true,  false, true,  false, false }, // *
        { kScriptKannada,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptKatakana,           false, false, false, false, false, false, true,  false, true,  false, true,  false, false }, // *
        { kScriptKhmer,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptLao,                false, false, false, false, true,  false, false, true,  true,  false, true,  false, true  }, // *
        { kScriptLatin,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // *
        { kScriptLimbu,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptLinearB,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptMalayalam,          false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptMongolian,          false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptMyanmar,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptOgham,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptOldItalic,          false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptOriya,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptOsmanya,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptRunic,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptShavian,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptSinhala,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptSyriac,             false, false, false, false, true,  true,  false, false, true,  false, true,  false, false }, // 
        { kScriptTagalog,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptTagbanwa,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptTaiLe,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptTamil,              false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptTelugu,             false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptThaana,             false, false, false, false, false, true,  false, false, true,  false, true,  false, false }, // 
        { kScriptThai,               false, false, false, false, true,  false, false, true,  true,  false, true,  false, true  }, // *
        { kScriptTibetan,            false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptUgaritic,           false, false, false, false, false, false, false, false, true,  false, true,  false, false }, // 
        { kScriptYi,                 false, false, false, false, false, false, false, false, true,  false, true,  false, false }
    };

#endif // EATEXT_SCRIPT_PROPERTY_SUPPORT


EATEXT_API bool GetScriptProperties(Script script, ScriptProperties* pScriptProperties, bool bForceValid)
{
    #if (EATEXT_SCRIPT_PROPERTY_SUPPORT > 1)
        if((script >= kScriptStart) && (script <= kScriptEnd))
        {
            if(pScriptProperties)
                *pScriptProperties = gScriptProperties[script];
            return true;
        }

        if(bForceValid)
        {
            if(pScriptProperties)
                *pScriptProperties = gScriptProperties[kScriptLatin];
            return true;
        }

        return NULL;

    #else

        bool bReturnValue = true;

        // We start with basic Latin script properties.
        ScriptProperties scriptProperties;
        memset(&scriptProperties, 0, sizeof(scriptProperties));
        scriptProperties.mScript            = script;
        scriptProperties.mbNumericChars     = false;
        scriptProperties.mbControlChars     = false;
        scriptProperties.mbAmbiguousChars   = true;
        scriptProperties.mbPrivateChars     = false;
        scriptProperties.mbComplex          = false;
        scriptProperties.mbRTL              = false;
        scriptProperties.mbVertical         = false;
        scriptProperties.mbWordBreak        = false;
        scriptProperties.mbClusterCaret     = true;     // For Latin we assume that a cluster consisting of a character and its diacritical must put the caret before the character or after the caret.
        scriptProperties.mbCharacterJustify = false;
        scriptProperties.mbUsesInvalidGlyph = true;
        scriptProperties.mbGrowableClusters = false;
        scriptProperties.mbRejectInvalid    = false;

        switch (script)
        {
            case kScriptArabic:{
                scriptProperties.mbComplex = true;
                scriptProperties.mbRTL = true;
                scriptProperties.mbClusterCaret = false;
                scriptProperties.mbCharacterJustify = true;
                scriptProperties.mbGrowableClusters = true;
                break;
            }

            case kScriptDevanagari:{
                scriptProperties.mbComplex = true;
                break;
            }

            case kScriptHebrew:{
                scriptProperties.mbComplex = true;
                scriptProperties.mbRTL = true;
                scriptProperties.mbClusterCaret = false;
                break;
            }

            case kScriptHan:
            case kScriptHangul:
            case kScriptHiragana:
            case kScriptKatakana:
                scriptProperties.mbVertical = true;
                break;

            case kScriptCyrillic:
            case kScriptGreek:
            case kScriptLatin:
            case kScriptTagalog:
                break; // These all use the same default values.

            case kScriptThai:{
                scriptProperties.mbComplex   = true;
                scriptProperties.mbWordBreak = true;
                scriptProperties.mbRejectInvalid = true;
                break;
            }

            case kScriptInherited:
            case kScriptCommon:
            case kScriptUnknown:
            case kScriptYi:
            case kScriptUgaritic:
            case kScriptTibetan:
            case kScriptThaana:
            case kScriptTelugu:
            case kScriptTamil:
            case kScriptTaiLe:
            case kScriptTagbanwa:
            case kScriptSyriac:
            case kScriptSinhala:
            case kScriptShavian:
            case kScriptRunic:
            case kScriptOsmanya:
            case kScriptOriya:
            case kScriptOldItalic:
            case kScriptOgham:
            case kScriptMyanmar:
            case kScriptMongolian:
            case kScriptMalayalam:
            case kScriptLinearB:
            case kScriptLimbu:
            case kScriptLao:
            case kScriptKhmer:
            case kScriptKannada:
            case kScriptHanunoo:
            case kScriptGurmukhi:
            case kScriptGujarati:
            case kScriptGothic:
            case kScriptGeorgian:
            case kScriptEthiopic:
            case kScriptDeseret:
            case kScriptCypriot:
            case kScriptCoptic:
            case kScriptCherokee:
            case kScriptCanadianAboriginal:
            case kScriptBuhid:
            case kScriptBopomofo:
            case kScriptBraille:
            case kScriptBengali:
            case kScriptArmenian:
            default:
                scriptProperties.mScript = kScriptLatin;
                bReturnValue = bForceValid;
                break;
        }

        if(pScriptProperties)
            *pScriptProperties = scriptProperties;
        return bReturnValue;

    #endif // EATEXT_SCRIPT_PROPERTY_SUPPORT
}




///////////////////////////////////////////////////////////////////////////////
// ScriptLocaleEntry
///////////////////////////////////////////////////////////////////////////////

struct ScriptLocaleEntry
{
    char   mLocale[5];  /// The longest this could ever need is 6 (5 chars + trailing zero).
    int8_t mScripts[3];
};


#if (EATEXT_SCRIPT_LOCALE_SUPPORT > 1)

    /// gScriptLocaleEntryArray
    ///
    /// Locales are represented as strings and are of the form: <language>[-<location>], 
    /// where the language is a lower case two or three letter ISO-639 code and the 
    /// location is a two or three character lower case ISO-3166 code which usually 
    /// refers to a country. 
    ///
    /// This table is derived from the Unicode standard. http://www.unicode.org/ucd/
    ///
    /// To consider: This table doesn't need to be very fast, but it could be made faster.
    //  To do:       The table could be compressed somewhat.
    ///
    ScriptLocaleEntry gScriptLocaleEntryArray[] =
    {
        { "aa",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ab",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "af",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "am",     { kScriptEthiopic,            kScriptUnknown,   kScriptUnknown  } },
        { "ar",     { kScriptArabic,              kScriptUnknown,   kScriptUnknown  } },
        { "ast",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ava",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ay",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "az",     { kScriptLatin,               kScriptCyrillic,  kScriptArabic   } }, // Azeri can be written in any of these scripts.
        { "ba",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "bam",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "be",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "bg",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "bh",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "bho",    { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "bi",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "bin",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "bn",     { kScriptBengali,             kScriptUnknown,   kScriptUnknown  } },
        { "bo",     { kScriptTibetan,             kScriptUnknown,   kScriptUnknown  } },
        { "br",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "bs",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "bua",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ca",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ce",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ch",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "chm",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "chr",    { kScriptCherokee,            kScriptUnknown,   kScriptUnknown  } },
        { "co",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "cs",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "cu",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "cv",     { kScriptCyrillic,            kScriptLatin,     kScriptUnknown  } },
        { "cy",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "da",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "de",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "dz",     { kScriptTibetan,             kScriptUnknown,   kScriptUnknown  } },
        { "el",     { kScriptGreek,               kScriptUnknown,   kScriptUnknown  } },
        { "en",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "eo",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "es",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "et",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "eu",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fa",     { kScriptArabic,              kScriptUnknown,   kScriptUnknown  } },
        { "fi",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fj",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fo",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fr",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ful",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fur",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "fy",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ga",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "gd",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "gez",    { kScriptEthiopic,            kScriptUnknown,   kScriptUnknown  } },
        { "gl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "gn",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "gu",     { kScriptGujarati,            kScriptUnknown,   kScriptUnknown  } },
        { "gv",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ha",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "haw",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "he",     { kScriptHebrew,              kScriptUnknown,   kScriptUnknown  } },
        { "hi",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "ho",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "hr",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "hu",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "hy",     { kScriptArmenian,            kScriptUnknown,   kScriptUnknown  } },
        { "ia",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ibo",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "id",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ie",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ik",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "io",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "is",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "it",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "iu",     { kScriptCanadianAboriginal,  kScriptUnknown,   kScriptUnknown  } },
        { "ja",     { kScriptHan,                 kScriptHiragana,  kScriptKatakana } },
        { "ka",     { kScriptGeorgian,            kScriptUnknown,   kScriptUnknown  } },
        { "kaa",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ki",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "kk",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "kl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "km",     { kScriptKhmer,               kScriptUnknown,   kScriptUnknown  } },
        { "kn",     { kScriptKannada,             kScriptUnknown,   kScriptUnknown  } },
        { "ko",     { kScriptHangul,              kScriptHan,       kScriptUnknown  } },
        { "kok",    { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "ks",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "ku",     { kScriptCyrillic,            kScriptLatin,     kScriptUnknown  } },
        { "kum",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "kv",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "kw",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ky",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "la",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "lb",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "lez",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "lo",     { kScriptLao,                 kScriptUnknown,   kScriptUnknown  } },
        { "lt",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "lv",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "mg",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "mh",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "mi",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "mk",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ml",     { kScriptMalayalam,           kScriptUnknown,   kScriptUnknown  } },
        { "mn",     { kScriptMongolian,           kScriptUnknown,   kScriptUnknown  } },
        { "mo",     { kScriptLatin,               kScriptCyrillic,  kScriptUnknown  } },
        { "mr",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "mt",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "my",     { kScriptMyanmar,             kScriptUnknown,   kScriptUnknown  } },
        { "nb",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ne",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "nl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "nn",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "no",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ny",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "oc",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "om",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "or",     { kScriptOriya,               kScriptUnknown,   kScriptUnknown  } },
        { "os",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "pl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "pt",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "rm",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ro",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ru",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "sa",     { kScriptDevanagari,          kScriptUnknown,   kScriptUnknown  } },
        { "sah",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "sco",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "se",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sel",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "sh",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "si",     { kScriptSinhala,             kScriptUnknown,   kScriptUnknown  } },
        { "sk",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sm",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sma",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "smj",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "smn",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sms",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "so",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sq",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sr",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "sv",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "sw",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "syr",    { kScriptSyriac,              kScriptUnknown,   kScriptUnknown  } },
        { "ta",     { kScriptTamil,               kScriptUnknown,   kScriptUnknown  } },
        { "te",     { kScriptTelugu,              kScriptUnknown,   kScriptUnknown  } },
        { "tg",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "th",     { kScriptThai,                kScriptUnknown,   kScriptUnknown  } },
        { "ti",     { kScriptEthiopic,            kScriptUnknown,   kScriptUnknown  } },
        { "tig",    { kScriptEthiopic,            kScriptUnknown,   kScriptUnknown  } },
        { "tk",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "tl",     { kScriptTagalog,             kScriptUnknown,   kScriptUnknown  } },
        { "tn",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "to",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "tr",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "ts",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "tt",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "tw",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "tyv",    { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ug",     { kScriptArabic,              kScriptUnknown,   kScriptUnknown  } },
        { "uk",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ur",     { kScriptArabic,              kScriptUnknown,   kScriptUnknown  } },
        { "uz",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } },
        { "ven",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "vi",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "vo",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "vot",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "wa",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "wen",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "wo",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "xh",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "yap",    { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "yi",     { kScriptHebrew,              kScriptUnknown,   kScriptUnknown  } },
        { "yo",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
        { "zh",     { kScriptHan,                 kScriptUnknown,   kScriptUnknown  } },
        { "zu",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } },
    };


#else // EATEXT_SCRIPT_LOCALE_SUPPORT

    // Here we have the most common languages supported by EA software.
    ScriptLocaleEntry gScriptLocaleEntryArray[] =
    {
        { "ar",     { kScriptArabic,              kScriptUnknown,   kScriptUnknown  } }, // kLanguageArabic
        { "cs",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageCzech
        { "da",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageDanish
        { "de",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageGerman
        { "el",     { kScriptGreek,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageGreek
        { "en",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageEnglish
        { "es",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageSpanish
        { "fi",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageFinnish
        { "fr",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageFrench
        { "it",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageItalian
        { "iw",     { kScriptHebrew,              kScriptUnknown,   kScriptUnknown  } }, // kLanguageHebrew
        { "ja",     { kScriptHan,                 kScriptHiragana,  kScriptKatakana } }, // kLanguageJapanese
        { "ko",     { kScriptHangul,              kScriptHan,       kScriptUnknown  } }, // kLanguageKorean
        { "nl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageDutch
        { "no",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageNorwegian
        { "pl",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguagePolish
        { "pt",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguagePortuguese
        { "ru",     { kScriptCyrillic,            kScriptUnknown,   kScriptUnknown  } }, // kLanguageRussian
        { "sv",     { kScriptLatin,               kScriptUnknown,   kScriptUnknown  } }, // kLanguageSwedish
        { "th",     { kScriptThai,                kScriptUnknown,   kScriptUnknown  } }, // kLanguageThai
        { "zh",     { kScriptHan,                 kScriptUnknown,   kScriptUnknown  } }, // kLanguageChinese
    };

#endif // EATEXT_SCRIPT_LOCALE_SUPPORT


// Possibly allow the user to overide this table.
const ScriptLocaleEntry* gpScriptLocaleEntryArray      = gScriptLocaleEntryArray;
uint32_t                   gnScriptLocaleEntryArrayCount = sizeof(gScriptLocaleEntryArray) / sizeof(gScriptLocaleEntryArray[0]);


EATEXT_API Script GetScriptFromLocale(const Char* pLocale, int index)
{
    if(index < 3)
    {
        // Do quick tests for some very common locales.
        if((pLocale[0] == 'e') && (pLocale[1] == 'n') && ((pLocale[2] == '-') || (pLocale[2] == 0)))
            return (index == 0) ? kScriptLatin : kScriptUnknown;

        // Convert Char to lower case char.
        char pLocaleTemp[3];
        int  i;

        for(i = 0; (i < 2) && pLocale[i]; i++) // Just use the first two letters of the locale.
        {
            pLocaleTemp[i] = (char)pLocale[i];
            if((pLocaleTemp[i] >= 'A') && (pLocaleTemp[i] <= 'Z'))
                pLocaleTemp[i] = (char)(pLocaleTemp[i] + ('a' - 'A'));
        }
        pLocaleTemp[i] = 0;

        // We do a classic binary search of the array.
        int nLower = 0; // This must be a signed type.
        int nUpper = (int)gnScriptLocaleEntryArrayCount;

        while(nLower <= nUpper)
        {
            const int nMid = (nLower + nUpper) / 2;
            const int cmp  = strcmp(gpScriptLocaleEntryArray[nMid].mLocale, pLocaleTemp);

            if(cmp == 0)
                return (Script)gpScriptLocaleEntryArray[nMid].mScripts[index];
            else if(cmp > 0)
                nUpper = nMid - 1;
            else // if(cmp < 0)
                nLower = nMid + 1;
        }
    }

    return kScriptUnknown;
}


EATEXT_API bool LocaleUsesScript(const Char* pLocale, Script script)
{
    // Current implementation is to use GetScriptFromLocale.
    int    i = 0;
    Script scriptTemp;

    while((scriptTemp = GetScriptFromLocale(pLocale, i++)) != kScriptUnknown)
    {
        if(scriptTemp == script)
            return true;
    }   
    return false;
}




///////////////////////////////////////////////////////////////////////////////
// ScriptTableEntry
///////////////////////////////////////////////////////////////////////////////

struct ScriptTableEntry
{
    uint16_t mnBegin;   /// We don't expect to support Chars with values > 65536, even if Char is a 32 bit type.
    uint16_t mnCount;   /// We don't expect to support Chars with values > 65536, even if Char is a 32 bit type.
    int8_t   mnScript;  /// One of enum Script
};


#if (EATEXT_SCRIPT_CHAR_SUPPORT > 1)

    /// gScriptTableEntryArray
    ///
    /// This table is derived from the Unicode standard. http://www.unicode.org/ucd/
    ///
    /// Todo: Convert this structure into one that can be searched faster.
    ///       Also consider making a version of the table that supports only 
    ///       a subset of the scripts (maybe ~5) and can be searched very fast.
    ///
    /// To consider: Have this table be based on EATEXT_XXX_SCRIPT_SUPPORTED.
    ///
    ScriptTableEntry gScriptTableEntryArray[] = 
    { 
        { 0x0041,    26, kScriptLatin },
        { 0x0061,    26, kScriptLatin },
        { 0x00aa,     1, kScriptLatin },
        { 0x00b5,     1, kScriptGreek },
        { 0x00ba,     1, kScriptLatin },
        { 0x00c0,    23, kScriptLatin },
        { 0x00d8,    31, kScriptLatin },
        { 0x00f8,   319, kScriptLatin },
        { 0x0250,   105, kScriptLatin },
        { 0x02e0,     5, kScriptLatin },
        { 0x0300,    80, kScriptInherited },
        { 0x0360,    16, kScriptInherited },
        { 0x037a,     1, kScriptGreek },
        { 0x0386,     1, kScriptGreek },
        { 0x0388,     3, kScriptGreek },
        { 0x038c,     1, kScriptGreek },
        { 0x038e,    20, kScriptGreek },
        { 0x03a3,    44, kScriptGreek },
        { 0x03d0,    38, kScriptGreek },
        { 0x03f7,     5, kScriptGreek },
        { 0x0400,   130, kScriptCyrillic },
        { 0x0483,     4, kScriptCyrillic },
        { 0x0488,     2, kScriptInherited },
        { 0x048a,    69, kScriptCyrillic },
        { 0x04d0,    38, kScriptCyrillic },
        { 0x04f8,     2, kScriptCyrillic },
        { 0x0500,    16, kScriptCyrillic },
        { 0x0531,    38, kScriptArmenian },
        { 0x0559,     1, kScriptArmenian },
        { 0x0561,    39, kScriptArmenian },
        { 0x0591,    17, kScriptInherited },
        { 0x05a3,    23, kScriptInherited },
        { 0x05bb,     3, kScriptInherited },
        { 0x05bf,     1, kScriptInherited },
        { 0x05c1,     2, kScriptInherited },
        { 0x05c4,     1, kScriptInherited },
        { 0x05d0,    27, kScriptHebrew },
        { 0x05f0,     3, kScriptHebrew },
        { 0x0621,    26, kScriptArabic },
        { 0x0641,    10, kScriptArabic },
        { 0x064b,    11, kScriptInherited },
        { 0x066e,     2, kScriptArabic },
        { 0x0670,     1, kScriptInherited },
        { 0x0671,    99, kScriptArabic },
        { 0x06d5,     1, kScriptArabic },
        { 0x06d6,    15, kScriptInherited },
        { 0x06e5,     2, kScriptArabic },
        { 0x06e7,     2, kScriptInherited },
        { 0x06ea,     4, kScriptInherited },
        { 0x06ee,     2, kScriptArabic },
        { 0x06fa,     3, kScriptArabic },
        { 0x06ff,     1, kScriptArabic },
        { 0x0710,    59, kScriptSyriac },
        { 0x074d,     3, kScriptSyriac },
        { 0x0780,    50, kScriptThaana },
        { 0x0901,    57, kScriptDevanagari },
        { 0x093c,    18, kScriptDevanagari },
        { 0x0950,     5, kScriptDevanagari },
        { 0x0958,    12, kScriptDevanagari },
        { 0x0966,    10, kScriptDevanagari },
        { 0x0981,     3, kScriptBengali },
        { 0x0985,     8, kScriptBengali },
        { 0x098f,     2, kScriptBengali },
        { 0x0993,    22, kScriptBengali },
        { 0x09aa,     7, kScriptBengali },
        { 0x09b2,     1, kScriptBengali },
        { 0x09b6,     4, kScriptBengali },
        { 0x09bc,     9, kScriptBengali },
        { 0x09c7,     2, kScriptBengali },
        { 0x09cb,     3, kScriptBengali },
        { 0x09d7,     1, kScriptBengali },
        { 0x09dc,     2, kScriptBengali },
        { 0x09df,     5, kScriptBengali },
        { 0x09e6,    12, kScriptBengali },
        { 0x0a02,     2, kScriptGurmukhi },
        { 0x0a05,     6, kScriptGurmukhi },
        { 0x0a0f,     2, kScriptGurmukhi },
        { 0x0a13,    22, kScriptGurmukhi },
        { 0x0a2a,     7, kScriptGurmukhi },
        { 0x0a32,     2, kScriptGurmukhi },
        { 0x0a35,     2, kScriptGurmukhi },
        { 0x0a38,     2, kScriptGurmukhi },
        { 0x0a3c,     1, kScriptGurmukhi },
        { 0x0a3e,     5, kScriptGurmukhi },
        { 0x0a47,     2, kScriptGurmukhi },
        { 0x0a4b,     3, kScriptGurmukhi },
        { 0x0a59,     4, kScriptGurmukhi },
        { 0x0a5e,     1, kScriptGurmukhi },
        { 0x0a66,    15, kScriptGurmukhi },
        { 0x0a81,     3, kScriptGujarati },
        { 0x0a85,     9, kScriptGujarati },
        { 0x0a8f,     3, kScriptGujarati },
        { 0x0a93,    22, kScriptGujarati },
        { 0x0aaa,     7, kScriptGujarati },
        { 0x0ab2,     2, kScriptGujarati },
        { 0x0ab5,     5, kScriptGujarati },
        { 0x0abc,    10, kScriptGujarati },
        { 0x0ac7,     3, kScriptGujarati },
        { 0x0acb,     3, kScriptGujarati },
        { 0x0ad0,     1, kScriptGujarati },
        { 0x0ae0,     4, kScriptGujarati },
        { 0x0ae6,    10, kScriptGujarati },
        { 0x0b01,     3, kScriptOriya },
        { 0x0b05,     8, kScriptOriya },
        { 0x0b0f,     2, kScriptOriya },
        { 0x0b13,    22, kScriptOriya },
        { 0x0b2a,     7, kScriptOriya },
        { 0x0b32,     2, kScriptOriya },
        { 0x0b35,     5, kScriptOriya },
        { 0x0b3c,     8, kScriptOriya },
        { 0x0b47,     2, kScriptOriya },
        { 0x0b4b,     3, kScriptOriya },
        { 0x0b56,     2, kScriptOriya },
        { 0x0b5c,     2, kScriptOriya },
        { 0x0b5f,     3, kScriptOriya },
        { 0x0b66,    10, kScriptOriya },
        { 0x0b71,     1, kScriptOriya },
        { 0x0b82,     2, kScriptTamil },
        { 0x0b85,     6, kScriptTamil },
        { 0x0b8e,     3, kScriptTamil },
        { 0x0b92,     4, kScriptTamil },
        { 0x0b99,     2, kScriptTamil },
        { 0x0b9c,     1, kScriptTamil },
        { 0x0b9e,     2, kScriptTamil },
        { 0x0ba3,     2, kScriptTamil },
        { 0x0ba8,     3, kScriptTamil },
        { 0x0bae,     8, kScriptTamil },
        { 0x0bb7,     3, kScriptTamil },
        { 0x0bbe,     5, kScriptTamil },
        { 0x0bc6,     3, kScriptTamil },
        { 0x0bca,     4, kScriptTamil },
        { 0x0bd7,     1, kScriptTamil },
        { 0x0be7,    12, kScriptTamil },
        { 0x0c01,     3, kScriptTelugu },
        { 0x0c05,     8, kScriptTelugu },
        { 0x0c0e,     3, kScriptTelugu },
        { 0x0c12,    23, kScriptTelugu },
        { 0x0c2a,    10, kScriptTelugu },
        { 0x0c35,     5, kScriptTelugu },
        { 0x0c3e,     7, kScriptTelugu },
        { 0x0c46,     3, kScriptTelugu },
        { 0x0c4a,     4, kScriptTelugu },
        { 0x0c55,     2, kScriptTelugu },
        { 0x0c60,     2, kScriptTelugu },
        { 0x0c66,    10, kScriptTelugu },
        { 0x0c82,     2, kScriptKannada },
        { 0x0c85,     8, kScriptKannada },
        { 0x0c8e,     3, kScriptKannada },
        { 0x0c92,    23, kScriptKannada },
        { 0x0caa,    10, kScriptKannada },
        { 0x0cb5,     5, kScriptKannada },
        { 0x0cbd,     8, kScriptKannada },
        { 0x0cc6,     3, kScriptKannada },
        { 0x0cca,     4, kScriptKannada },
        { 0x0cd5,     2, kScriptKannada },
        { 0x0cde,     1, kScriptKannada },
        { 0x0ce0,     2, kScriptKannada },
        { 0x0ce6,    10, kScriptKannada },
        { 0x0d02,     2, kScriptMalayalam },
        { 0x0d05,     8, kScriptMalayalam },
        { 0x0d0e,     3, kScriptMalayalam },
        { 0x0d12,    23, kScriptMalayalam },
        { 0x0d2a,    16, kScriptMalayalam },
        { 0x0d3e,     6, kScriptMalayalam },
        { 0x0d46,     3, kScriptMalayalam },
        { 0x0d4a,     4, kScriptMalayalam },
        { 0x0d57,     1, kScriptMalayalam },
        { 0x0d60,     2, kScriptMalayalam },
        { 0x0d66,    10, kScriptMalayalam },
        { 0x0d82,     2, kScriptSinhala },
        { 0x0d85,    18, kScriptSinhala },
        { 0x0d9a,    24, kScriptSinhala },
        { 0x0db3,     9, kScriptSinhala },
        { 0x0dbd,     1, kScriptSinhala },
        { 0x0dc0,     7, kScriptSinhala },
        { 0x0dca,     1, kScriptSinhala },
        { 0x0dcf,     6, kScriptSinhala },
        { 0x0dd6,     1, kScriptSinhala },
        { 0x0dd8,     8, kScriptSinhala },
        { 0x0df2,     2, kScriptSinhala },
      //{ 0x0e01,    58, kScriptThai },
      //{ 0x0e3f,    29, kScriptThai },
        { 0x0e00,   128, kScriptThai }, // This is a simpler version of the above academically precise version.
      //{ 0x0e81,     2, kScriptLao },
      //{ 0x0e84,     1, kScriptLao },
      //{ 0x0e87,     2, kScriptLao },
      //{ 0x0e8a,     1, kScriptLao },
      //{ 0x0e8d,     1, kScriptLao },
      //{ 0x0e94,     4, kScriptLao },
      //{ 0x0e99,     7, kScriptLao },
      //{ 0x0ea1,     3, kScriptLao },
      //{ 0x0ea5,     1, kScriptLao },
      //{ 0x0ea7,     1, kScriptLao },
      //{ 0x0eaa,     2, kScriptLao },
      //{ 0x0ead,    13, kScriptLao },
      //{ 0x0ebb,     3, kScriptLao },
      //{ 0x0ec0,     5, kScriptLao },
      //{ 0x0ec6,     1, kScriptLao },
      //{ 0x0ec8,     6, kScriptLao },
      //{ 0x0ed0,    10, kScriptLao },
      //{ 0x0edc,     2, kScriptLao },
        { 0x0e80,   128, kScriptLao }, // This is a simpler version of the above academically precise version.
        { 0x0f00,     1, kScriptTibetan },
        { 0x0f18,     2, kScriptTibetan },
        { 0x0f20,    20, kScriptTibetan },
        { 0x0f35,     1, kScriptTibetan },
        { 0x0f37,     1, kScriptTibetan },
        { 0x0f39,     1, kScriptTibetan },
        { 0x0f40,     8, kScriptTibetan },
        { 0x0f49,    34, kScriptTibetan },
        { 0x0f71,    20, kScriptTibetan },
        { 0x0f86,     6, kScriptTibetan },
        { 0x0f90,     8, kScriptTibetan },
        { 0x0f99,    36, kScriptTibetan },
        { 0x0fc6,     1, kScriptTibetan },
        { 0x1000,    34, kScriptMyanmar },
        { 0x1023,     5, kScriptMyanmar },
        { 0x1029,     2, kScriptMyanmar },
        { 0x102c,     7, kScriptMyanmar },
        { 0x1036,     4, kScriptMyanmar },
        { 0x1040,    10, kScriptMyanmar },
        { 0x1050,    10, kScriptMyanmar },
        { 0x10a0,    38, kScriptGeorgian },
        { 0x10d0,    41, kScriptGeorgian },
        { 0x1100,    90, kScriptHangul },
        { 0x115f,    68, kScriptHangul },
        { 0x11a8,    82, kScriptHangul },
        { 0x1200,     7, kScriptEthiopic },
        { 0x1208,    63, kScriptEthiopic },
        { 0x1248,     1, kScriptEthiopic },
        { 0x124a,     4, kScriptEthiopic },
        { 0x1250,     7, kScriptEthiopic },
        { 0x1258,     1, kScriptEthiopic },
        { 0x125a,     4, kScriptEthiopic },
        { 0x1260,    39, kScriptEthiopic },
        { 0x1288,     1, kScriptEthiopic },
        { 0x128a,     4, kScriptEthiopic },
        { 0x1290,    31, kScriptEthiopic },
        { 0x12b0,     1, kScriptEthiopic },
        { 0x12b2,     4, kScriptEthiopic },
        { 0x12b8,     7, kScriptEthiopic },
        { 0x12c0,     1, kScriptEthiopic },
        { 0x12c2,     4, kScriptEthiopic },
        { 0x12c8,     7, kScriptEthiopic },
        { 0x12d0,     7, kScriptEthiopic },
        { 0x12d8,    23, kScriptEthiopic },
        { 0x12f0,    31, kScriptEthiopic },
        { 0x1310,     1, kScriptEthiopic },
        { 0x1312,     4, kScriptEthiopic },
        { 0x1318,     7, kScriptEthiopic },
        { 0x1320,    39, kScriptEthiopic },
        { 0x1348,    19, kScriptEthiopic },
        { 0x1369,    20, kScriptEthiopic },
        { 0x13a0,    85, kScriptCherokee },
        { 0x1401,   620, kScriptCanadianAboriginal },
        { 0x166f,     8, kScriptCanadianAboriginal },
        { 0x1681,    26, kScriptOgham },
        { 0x16a0,    75, kScriptRunic },
        { 0x16ee,     3, kScriptRunic },
        { 0x1700,    13, kScriptTagalog },
        { 0x170e,     7, kScriptTagalog },
        { 0x1720,    21, kScriptHanunoo },
        { 0x1740,    20, kScriptBuhid },
        { 0x1760,    13, kScriptTagbanwa },
        { 0x176e,     3, kScriptTagbanwa },
        { 0x1772,     2, kScriptTagbanwa },
        { 0x1780,    84, kScriptKhmer },
        { 0x17e0,    10, kScriptKhmer },
        { 0x180b,     3, kScriptInherited },
        { 0x1810,    10, kScriptMongolian },
        { 0x1820,    88, kScriptMongolian },
        { 0x1880,    42, kScriptMongolian },
        { 0x1900,    29, kScriptLimbu },
        { 0x1920,    12, kScriptLimbu },
        { 0x1930,    12, kScriptLimbu },
        { 0x1946,    10, kScriptLimbu },
        { 0x1950,    30, kScriptTaiLe },
        { 0x1970,     5, kScriptTaiLe },
        { 0x1d00,    38, kScriptLatin },
        { 0x1d26,     5, kScriptGreek },
        { 0x1d2b,     1, kScriptCyrillic },
        { 0x1d2c,    49, kScriptLatin },
        { 0x1d5d,     5, kScriptGreek },
        { 0x1d62,     4, kScriptLatin },
        { 0x1d66,     5, kScriptGreek },
        { 0x1d6b,     1, kScriptLatin },
        { 0x1e00,   156, kScriptLatin },
        { 0x1ea0,    90, kScriptLatin },
        { 0x1f00,    22, kScriptGreek },
        { 0x1f18,     6, kScriptGreek },
        { 0x1f20,    38, kScriptGreek },
        { 0x1f48,     6, kScriptGreek },
        { 0x1f50,     8, kScriptGreek },
        { 0x1f59,     1, kScriptGreek },
        { 0x1f5b,     1, kScriptGreek },
        { 0x1f5d,     1, kScriptGreek },
        { 0x1f5f,    31, kScriptGreek },
        { 0x1f80,    53, kScriptGreek },
        { 0x1fb6,     7, kScriptGreek },
        { 0x1fbe,     1, kScriptGreek },
        { 0x1fc2,     3, kScriptGreek },
        { 0x1fc6,     7, kScriptGreek },
        { 0x1fd0,     4, kScriptGreek },
        { 0x1fd6,     6, kScriptGreek },
        { 0x1fe0,    13, kScriptGreek },
        { 0x1ff2,     3, kScriptGreek },
        { 0x1ff6,     7, kScriptGreek },
        { 0x2071,     1, kScriptLatin },
        { 0x207f,     1, kScriptLatin },
        { 0x20d0,    27, kScriptInherited },
        { 0x2126,     1, kScriptGreek },
        { 0x212a,     2, kScriptLatin },
        { 0x2800,   256, kScriptBraille },
        { 0x2e80,    26, kScriptHan },
        { 0x2e9b,    89, kScriptHan },
        { 0x2f00,   214, kScriptHan },
        { 0x3005,     1, kScriptHan },
        { 0x3007,     1, kScriptHan },
        { 0x3021,     9, kScriptHan },
        { 0x302a,     6, kScriptInherited },
        { 0x3038,     4, kScriptHan },
        { 0x3041,    86, kScriptHiragana },
        { 0x3099,     2, kScriptInherited },
        { 0x309d,     3, kScriptHiragana },
        { 0x30a1,    90, kScriptKatakana },
        { 0x30fd,     3, kScriptKatakana },
        { 0x3105,    40, kScriptBopomofo },
        { 0x3131,    94, kScriptHangul },
        { 0x31a0,    24, kScriptBopomofo },
        { 0x31f0,    16, kScriptKatakana },
        { 0x3400,  6582, kScriptHan },
        { 0x4e00, 20902, kScriptHan },
        { 0xa000,  1165, kScriptYi },
        { 0xa490,    55, kScriptYi },
        { 0xac00, 11172, kScriptHangul },
        { 0xf700,    29, kScriptThai },      // 0xf700 - 0xf71d Private use Thai. This is what Microsoft does with their fonts.
        // Other private use characters here.
        { 0xf900,   302, kScriptHan },
        { 0xfa30,    59, kScriptHan },
        { 0xfb00,     7, kScriptLatin },
        { 0xfb13,     5, kScriptArmenian },
        { 0xfb1d,     1, kScriptHebrew },
        { 0xfb1e,     1, kScriptInherited },
        { 0xfb1f,    10, kScriptHebrew },
        { 0xfb2a,    13, kScriptHebrew },
        { 0xfb38,     5, kScriptHebrew },
        { 0xfb3e,     1, kScriptHebrew },
        { 0xfb40,     2, kScriptHebrew },
        { 0xfb43,     2, kScriptHebrew },
        { 0xfb46,    10, kScriptHebrew },
        { 0xfb50,    98, kScriptArabic },
        { 0xfbd3,   363, kScriptArabic },
        { 0xfd50,    64, kScriptArabic },
        { 0xfd92,    54, kScriptArabic },
        { 0xfdf0,    12, kScriptArabic },
        { 0xfe00,    16, kScriptInherited },
        { 0xfe20,     4, kScriptInherited },
        { 0xfe70,     5, kScriptArabic },
        { 0xfe76,   135, kScriptArabic },
        { 0xff21,    26, kScriptLatin },
        { 0xff41,    26, kScriptLatin },
        { 0xff66,    10, kScriptKatakana },
        { 0xff71,    45, kScriptKatakana },
        { 0xffa0,    31, kScriptHangul },
        { 0xffc2,     6, kScriptHangul },
        { 0xffca,     6, kScriptHangul },
        { 0xffd2,     6, kScriptHangul },
        { 0xffda,     3, kScriptHangul }
    };


#else // EATEXT_SCRIPT_CHAR_SUPPORT

    /// gScriptTableEntryArray
    ///
    /// This is a smaller version of this table. It is missing most of the
    /// more esoteric entries, but should be good enough for basic text.
    ///
    /// Note that this table has been simplified not only by removing 
    /// entries but also changing some entries 
    ///
    ScriptTableEntry gScriptTableEntryArray[] = 
    { 
        { 0x0041,    26, kScriptLatin },        // The Latin alphabet.
        { 0x0061,    26, kScriptLatin },        // The Latin alphabet.
        { 0x00aa,     1, kScriptLatin },
        { 0x00ba,     1, kScriptLatin },
        { 0x00b5,     1, kScriptGreek },
        { 0x00c0,    23, kScriptLatin },
        { 0x00d8,    31, kScriptLatin },
        { 0x00f8,   319, kScriptLatin },
        { 0x0250,   105, kScriptLatin },
        { 0x02e0,     5, kScriptLatin },
        { 0x0300,    80, kScriptInherited },
        { 0x0360,    16, kScriptInherited },
        { 0x037a,     1, kScriptGreek },
        { 0x0386,     1, kScriptGreek },
        { 0x0388,     3, kScriptGreek },
        { 0x038c,     1, kScriptGreek },
        { 0x038e,    20, kScriptGreek },
        { 0x03a3,    44, kScriptGreek },
        { 0x03d0,    38, kScriptGreek },
        { 0x03f7,     5, kScriptGreek },
        { 0x0400,   130, kScriptCyrillic },
        { 0x0483,     4, kScriptCyrillic },
        { 0x0488,     2, kScriptInherited },
        { 0x048a,    69, kScriptCyrillic },
        { 0x04d0,    38, kScriptCyrillic },
        { 0x04f8,     2, kScriptCyrillic },
        { 0x0500,    16, kScriptCyrillic },
        { 0x0591,    17, kScriptInherited },
        { 0x05a3,    23, kScriptInherited },
        { 0x05bb,     3, kScriptInherited },
        { 0x05bf,     1, kScriptInherited },
        { 0x05c1,     2, kScriptInherited },
        { 0x05c4,     1, kScriptInherited },

        #if EATEXT_HEBREW_SCRIPT_SUPPORTED
        { 0x05d0,    27, kScriptHebrew },
        { 0x05f0,     3, kScriptHebrew },
        #endif

        #if EATEXT_ARABIC_SCRIPT_SUPPORTED
        { 0x0621,    26, kScriptArabic },
        { 0x0641,    10, kScriptArabic },
        { 0x064b,    11, kScriptInherited },
        { 0x066e,     2, kScriptArabic },
        { 0x0670,     1, kScriptInherited },
        { 0x0671,    99, kScriptArabic },
        { 0x06d5,     1, kScriptArabic },
        { 0x06d6,    15, kScriptInherited },
        { 0x06e5,     2, kScriptArabic },
        { 0x06e7,     2, kScriptInherited },
        { 0x06ea,     4, kScriptInherited },
        { 0x06ee,     2, kScriptArabic },
        { 0x06fa,     3, kScriptArabic },
        { 0x06ff,     1, kScriptArabic },
        #endif

        #if EATEXT_DEVANAGARI_SCRIPT_SUPPORTED
        { 0x0901,    57, kScriptDevanagari },
        { 0x093c,    18, kScriptDevanagari },
        { 0x0950,     5, kScriptDevanagari },
        { 0x0958,    12, kScriptDevanagari },
        { 0x0966,    10, kScriptDevanagari },
        #endif

        #if EATEXT_THAI_SCRIPT_SUPPORTED
        { 0x0e01,    58, kScriptThai },
        { 0x0e40,    15, kScriptThai },
        { 0x0e50,    10, kScriptThai },
        #endif

        { 0x1100,    90, kScriptHangul },
        { 0x115f,    68, kScriptHangul },
        { 0x11a8,    82, kScriptHangul },
        { 0x2e80,    26, kScriptHan },
        { 0x2e9b,    89, kScriptHan },
        { 0x2f00,   214, kScriptHan },
        { 0x3005,     1, kScriptHan },
        { 0x3007,     1, kScriptHan },
        { 0x3021,     9, kScriptHan },
        { 0x3038,     4, kScriptHan },
        { 0x3041,    86, kScriptHiragana },
        { 0x309d,     3, kScriptHiragana },
        { 0x30a1,    90, kScriptKatakana },
        { 0x30fd,     3, kScriptKatakana },
        { 0x3131,    94, kScriptHangul },
        { 0x31f0,    16, kScriptKatakana },
        { 0x3400,  6582, kScriptHan },
        { 0x4e00, 20902, kScriptHan },
        { 0xac00, 11172, kScriptHangul },
        { 0xf900,   302, kScriptHan },
        { 0xfa30,    59, kScriptHan },
        { 0xfb00,     7, kScriptLatin },        // Latin ligatures (ff, fi, etc.)

        #if EATEXT_HEBREW_SCRIPT_SUPPORTED
        { 0xfb1d,     1, kScriptHebrew },
        { 0xfb1f,    10, kScriptHebrew },
        { 0xfb2a,    13, kScriptHebrew },
        { 0xfb38,     5, kScriptHebrew },
        { 0xfb3e,     1, kScriptHebrew },
        { 0xfb40,     2, kScriptHebrew },
        { 0xfb43,     2, kScriptHebrew },
        { 0xfb46,    10, kScriptHebrew },
        #endif

        { 0xff21,    26, kScriptLatin },        // Full-width Latin alahpabet. Used by Asian scripts.
        { 0xff41,    26, kScriptLatin },        // Full-width Latin alahpabet. Used by Asian scripts.
        { 0xff66,    10, kScriptKatakana },     // Half-width Katakana.
        { 0xff71,    45, kScriptKatakana },     // Half-width Katakana.
        { 0xffa0,    31, kScriptHangul },       // Half-width Hangul.
        { 0xffc2,     6, kScriptHangul },       // Half-width Hangul.
        { 0xffca,     6, kScriptHangul },       // Half-width Hangul.
        { 0xffd2,     6, kScriptHangul },       // Half-width Hangul.
        { 0xffda,     3, kScriptHangul }        // Half-width Hangul.
    };

#endif // EATEXT_SCRIPT_CHAR_SUPPORT


const ScriptTableEntry* gpScriptTableEntryArray     = gScriptTableEntryArray;
const size_t            kScriptTableEntryArrayCount = sizeof(gScriptTableEntryArray) / sizeof(gScriptTableEntryArray[0]);


EATEXT_API Script GetScriptFromChar(Char c)
{
    // We do a classic binary search of the RLE array.
    // Todo: Convert this to a hash table or some other kind of fast structure.
    int nLower = 0; // This must be a signed type.
    int nUpper = (c < 255) ? 8 : (int)((unsigned)kScriptTableEntryArrayCount - 1);

    while(nLower <= nUpper)
    {
        const int nMid = (nLower + nUpper) / 2;

        if(c < gScriptTableEntryArray[nMid].mnBegin)
            nUpper = nMid - 1;
        else if(c >= (gScriptTableEntryArray[nMid].mnBegin + gScriptTableEntryArray[nMid].mnCount))
            nLower = nMid + 1;
        else
            return (Script)gScriptTableEntryArray[nMid].mnScript;
    }

    return kScriptCommon;
}


// GetLayoutScriptFromChar
//
// The purpose of this function is to adjust values returned from GetScriptFromChar 
// as needed for layout. GetScriptFromChar follows the Unicode Standard in characterizing 
// chars, but in practice we sometimes need chars to act differently during layout.
// Typically GetScriptFromChar will say that a char is kScriptCommon, but during layout
// it's not really a neutral common script character.
//
EATEXT_API Script GetLayoutScriptFromChar(Char c)
{
    // Process exception characters. 
    // If this list gets too long, convert it to a table lookup.
    if(c == 0x0640) // This is the Arabic tatweel character, which is a modifier char.
        return kScriptInherited; // Could possibly return kScriptArabic.

    return GetScriptFromChar(c);
}



///////////////////////////////////////////////////////////////////////////////
// LocaleScriptArray
///////////////////////////////////////////////////////////////////////////////


#if (EATEXT_LOCALE_SCRIPT_SUPPORT > 1)

    /// gLocaleScriptArray
    ///
    /// Locales are represented as strings and are of the form: <language>[-<location>], where the 
    /// language is a lower case two or three letter ISO-639 code and the location is a two or 
    /// three character lower case ISO-3166 code which usually refers to a country. 
    ///
    /// This list of languages is not a full set of ISO 639 languages, but is simply a set of 
    /// languages that are primary the set of scripts.
    ///
    const Char gLocaleScriptArray[][4] =
    {
        {   0,   0,   0,  0 },  // kScriptCommon          // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptInherited       // Todo: fix this.
        { 'a', 'r',   0,  0 },  // kScriptArabic
        { 'h', 'y',   0,  0 },  // kScriptArmenian
        { 'b', 'n',   0,  0 },  // kScriptBengali
        { 'e', 'n',   0,  0 },  // kScriptBraille
        { 'z', 'h',   0,  0 },  // kScriptBopomofo
        {   0,   0,   0,  0 },  // kScriptBuhid           // Todo: fix this.
        { 'i', 'u',   0,  0 },  // kScriptCanadianAboriginal
        { 'c', 'h', 'r',  0 },  // kScriptCherokee
        { 'c', 'o', 'p',  0 },  // kScriptCoptic
        {   0,   0,   0,  0 },  // kScriptCypriot         // Todo: fix this.
        { 'r', 'u',   0,  0 },  // kScriptCyrillic
        {   0,   0,   0,  0 },  // kScriptDeseret
        { 'h', 'i',   0,  0 },  // kScriptDevanagari
        { 'a', 'm',   0,  0 },  // kScriptEthiopic
        { 'k', 'a',   0,  0 },  // kScriptGeorgian
        {   0,   0,   0,  0 },  // kScriptGothic          // Todo: fix this.
        { 'e', 'l',   0,  0 },  // kScriptGreek
        { 'g', 'u',   0,  0 },  // kScriptGujarati
        { 'p', 'a',   0,  0 },  // kScriptGurmukhi
        { 'j', 'a',   0,  0 },  // kScriptHan
        { 'k', 'o',   0,  0 },  // kScriptHangul
        {   0,   0,   0,  0 },  // kScriptHanunoo         // Todo: fix this.
        { 'h', 'e',   0,  0 },  // kScriptHebrew
        { 'j', 'a',   0,  0 },  // kScriptHiragana
        { 'k', 'n',   0,  0 },  // kScriptKannada
        { 'j', 'a',   0,  0 },  // kScriptKatakana
        { 'k', 'm',   0,  0 },  // kScriptKhmer
        { 'l', 'o',   0,  0 },  // kScriptLao
        { 'e', 'n',   0,  0 },  // kScriptLatin
        {   0,   0,   0,  0 },  // kScriptLimbu           // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptLinearB         // Todo: fix this.
        { 'm', 'l',   0,  0 },  // kScriptMalayalam
        { 'm', 'n',   0,  0 },  // kScriptMongolian
        { 'm', 'y',   0,  0 },  // kScriptMyanmar
        {   0,   0,   0,  0 },  // kScriptOgham           // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptOldItalic       // Todo: fix this.
        { 'o', 'r',   0,  0 },  // kScriptOriya
        {   0,   0,   0,  0 },  // kScriptOsmanya         // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptRunic           // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptShavian         // Todo: fix this.
        { 's', 'i',   0,  0 },  // kScriptSinhala
        { 's', 'y', 'r',  0 },  // kScriptSyriac
        { 't', 'l',   0,  0 },  // kScriptTagalog
        {   0,   0,   0,  0 },  // kScriptTagbanwa        // Todo: fix this.
        {   0,   0,   0,  0 },  // kScriptTaiLe           // Todo: fix this.
        { 't', 'a',   0,  0 },  // kScriptTamil
        { 't', 'e',   0,  0 },  // kScriptTelugu
        { 'd', 'v',   0,  0 },  // kScriptThaana
        { 't', 'h',   0,  0 },  // kScriptThai
        { 'b', 'o',   0,  0 },  // kScriptTibetan
        { 'u', 'g', 'a',  0 },  // kScriptUgaritic
        {   0,   0,   0,  0 }   // kScriptYi              // Todo: fix this.
    };

#endif // EATEXT_LOCALE_SCRIPT_SUPPORT


EATEXT_API const Char* GetLocaleFromScript(Script script, bool bForceValid)
{
    #if EATEXT_LOCALE_SCRIPT_SUPPORT > 1

        if((script >= kScriptStart) && (script <= kScriptEnd))
        {
            if(*gLocaleScriptArray[script])
                return gLocaleScriptArray[script]; // Possibly consider returning a shareable pointer.
        }

        if(bForceValid)
            return gLocaleScriptArray[kScriptLatin];
        return NULL;

    #else

        static const Char kStrAR[] = { 'a', 'r', 0 };
        static const Char kStrRU[] = { 'r', 'u', 0 };
        static const Char kStrHI[] = { 'h', 'i', 0 };
        static const Char kStrEL[] = { 'e', 'l', 0 };
        static const Char kStrJA[] = { 'j', 'a', 0 };
        static const Char kStrKO[] = { 'k', 'o', 0 };
        static const Char kStrHE[] = { 'h', 'e', 0 };
        static const Char kStrEN[] = { 'e', 'n', 0 };
        static const Char kStrTL[] = { 't', 'l', 0 };
        static const Char kStrTH[] = { 't', 'h', 0 };

        switch (script)
        {
            case kScriptArabic:
                return kStrAR;
            case kScriptCyrillic:
                return kStrRU;
            case kScriptDevanagari:
                return kStrHI;
            case kScriptGreek:
                return kStrEL;
            case kScriptHan:
                return kStrJA;
            case kScriptHangul:
                return kStrKO;
            case kScriptHebrew:
                return kStrHE;
            case kScriptHiragana:
                return kStrJA;
            case kScriptKatakana:
                return kStrJA;
            case kScriptLatin:
                return kStrEN;
            case kScriptTagalog:
                return kStrTL;
            case kScriptThai:
                return kStrTH;

            case kScriptYi:
            case kScriptUgaritic:
            case kScriptTibetan:
            case kScriptThaana:
            case kScriptTelugu:
            case kScriptTamil:
            case kScriptTaiLe:
            case kScriptTagbanwa:
            case kScriptSyriac:
            case kScriptSinhala:
            case kScriptShavian:
            case kScriptRunic:
            case kScriptOsmanya:
            case kScriptOriya:
            case kScriptOldItalic:
            case kScriptOgham:
            case kScriptMyanmar:
            case kScriptMongolian:
            case kScriptMalayalam:
            case kScriptLinearB:
            case kScriptLimbu:
            case kScriptLao:
            case kScriptKhmer:
            case kScriptKannada:
            case kScriptHanunoo:
            case kScriptGurmukhi:
            case kScriptGujarati:
            case kScriptGothic:
            case kScriptGeorgian:
            case kScriptEthiopic:
            case kScriptDeseret:
            case kScriptCypriot:
            case kScriptCoptic:
            case kScriptCherokee:
            case kScriptCanadianAboriginal:
            case kScriptBuhid:
            case kScriptBopomofo:
            case kScriptBraille:
            case kScriptBengali:
            case kScriptArmenian:
            case kScriptInherited:
            case kScriptCommon:
            case kScriptUnknown:
            default:
                break;
        }

        if(bForceValid)
            return kStrEN;
        return NULL;

    #endif // EATEXT_LOCALE_SCRIPT_SUPPORT
}



} // namespace Text

} // namespace EA
















