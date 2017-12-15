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
// EATextBreak.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextBreak.h>
#include <EAText/EATextScript.h>
#include <coreallocator/icoreallocator_interface.h>
#include <coreallocator/icoreallocatormacros.h>
#include <EASTL/core_allocator_adapter.h>
#include <EASTL/string.h>
#include <EASTL/list.h>
#include EA_ASSERT_HEADER


///////////////////////////////////////////////////////////////////////////////
// EATEXT_UNISCRIBE_ENABLED
//
// Defined as 0 or 1, default is 1 for Windows, 0 for others.
// We use Uniscribe on Windows to assist in some things such as Thai line
// breaking functionaliyt.
//
#ifndef EATEXT_UNISCRIBE_ENABLED
    // Under Windows, we use Uniscribe (usp10.dll) for Thai word/line breaking services.
        #define EATEXT_UNISCRIBE_ENABLED 1
        #include <usp10.h>
        #pragma comment(lib, "usp10")
#endif



namespace EA
{

namespace Text
{



///////////////////////////////////////////////////////////////////////////////
// WordBreakDictionary
///////////////////////////////////////////////////////////////////////////////

#if EATEXT_UNISCRIBE_ENABLED
    typedef eastl::basic_string<Char, EA::Allocator::EASTLICoreAllocator> String;
    typedef eastl::list<String, EA::Allocator::EASTLICoreAllocator>       StringList;

    StringList* gpBreakDictionaryAdditionList = NULL;
#endif


EATEXT_API void InitWordBreakDictionary()
{
    // It turns out we don't need to do anything here. 
    // Possibly we want to set the allocator here instead of 
    // setting it in the AddWordBreakDictionaryEntry function.

    #if EATEXT_UNISCRIBE_ENABLED
        gpBreakDictionaryAdditionList = CORE_NEW(EA::Text::GetAllocator(), "gpBreakDictionaryAdditionList", EA::Allocator::MEM_PERM) StringList;
    #endif
}


EATEXT_API void ShutdownWordBreakDictionary()
{
    #if EATEXT_UNISCRIBE_ENABLED
        // This will free the memory allocated for the entries.
        if(gpBreakDictionaryAdditionList)
        {
            gpBreakDictionaryAdditionList->clear();
            CORE_DELETE(EA::Text::GetAllocator(), gpBreakDictionaryAdditionList);
        }
    #endif
}


EATEXT_API void AddWordBreakDictionaryEntry(const Char* pWord)
{
    #if EATEXT_UNISCRIBE_ENABLED
        EA_ASSERT(gpBreakDictionaryAdditionList);

        if(gpBreakDictionaryAdditionList)
        {
            if(gpBreakDictionaryAdditionList->empty())
                gpBreakDictionaryAdditionList->get_allocator().set_allocator(EA::Text::GetAllocator());

            gpBreakDictionaryAdditionList->push_back();
            String& sEntry = gpBreakDictionaryAdditionList->back();
            sEntry.get_allocator().set_allocator(EA::Text::GetAllocator());
            sEntry = pWord;
        }
    #else
        (void)pWord;
    #endif
}




///////////////////////////////////////////////////////////////////////////////
// BreakIteratorBase
///////////////////////////////////////////////////////////////////////////////

uint32_t BreakIteratorBase::SetPosition(uint32_t absolutePosition)
{
    // EA_ASSERT(absolutePosition <= mIterator.GetSize());
    mIterator.SetPosition(absolutePosition);
    return mIterator.GetPosition();
}


uint32_t BreakIteratorBase::SetRelativePosition(int32_t relativePosition)
{
    const int32_t absolutePosition = (int32_t)mIterator.GetPosition() + relativePosition;

    mIterator.SetPosition((uint32_t)absolutePosition);

    return mIterator.GetPosition();
}



///////////////////////////////////////////////////////////////////////////////
// CharacterBreakIterator
///////////////////////////////////////////////////////////////////////////////

static inline bool IsBreakableControl(Char c)
{
    // kCharZWNJ and kCharZWJ are of type Cf (kCCFlagFormatChar), 
    // but we must exclude them as known-breakable. However, other break 
    // logic might allow a break next to them anyway.

    return IsCharCategory(c, kCCFlagControlChar | kCCFlagFormatChar | kCCFlagLineSeparator | kCCFlagParagraphSeparator) && (c != kCharZWNJ) && (c != kCharZWJ);
}


uint32_t CharacterBreakIterator::GetPrevCharBreak()
{
    // See The Unicode Standard v3.0, Chapter 3, section 5 (page 43).
    // See The Unicode Standard v4.0, Chapter 3, section 6 (page 70).
    // See also the Unicode technical report #29.
    // We need to walk to the beginning of the previous cluster.

    // Currently we do the not-highly-efficient technique of starting
    // at the front and iterating forwards, as opposed to the technique
    // of iterating backwards. The latter would require logic similar to
    // but not identical to iterating fowards. For the time being,
    // we do it this way in order to save a few hours' coding time.
    // If you need this to be faster, bug the maintainer of this library.
    const uint32_t nOriginalPosition = mIterator.GetPosition();

    uint32_t nCurrentPosition = 0;
    mIterator.SetPosition(0);

    while(!mIterator.AtEnd())
    {
        const uint32_t nNewPosition = GetNextCharBreak();

        if(nNewPosition >= nOriginalPosition)
        {
            mIterator.SetPosition(nCurrentPosition);
            break;
        }

        nCurrentPosition = nNewPosition;
    }

    return mIterator.GetPosition();
}


uint32_t CharacterBreakIterator::GetNextCharBreak()
{
    // See The Unicode Standard v3.0, Chapter 3, section 5 (page 43).
    // See The Unicode Standard v4.0, Chapter 3, section 6 (page 70).
    // See also the Unicode technical report #29.
    // We need to walk to the end of the current cluster.

    if(!mIterator.AtEnd())
    {
        Char cCurrent = *mIterator;
        ++mIterator;
        Char cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid in the case that mIterator hit the end.

        while(!mIterator.AtEnd())
        {
            if(cCurrent == kCharCR)
            {
                if(cNext != kCharLF)
                    break; // We can break here.
                // Else we can never break between CR and LF.
            }
            else if((IsBreakableControl(cCurrent)) || (cCurrent == kCharLF) || 
                    (IsBreakableControl(cNext))    || (cNext    == kCharLF) || (cNext == kCharCR))
            {
                break; // We can break here.
            }
            else if(!IsGraphemeExtend(cNext))
            {
                const HangulSyllableType hCurrent = GetHangulSyllableType(cCurrent);

                if(hCurrent != kHSTNone) // If it's a Hangul syllable type...
                {
                    const HangulSyllableType hNext = GetHangulSyllableType(cNext);

                    if(hNext != kHSTNone) // If we have two Hangul syllables in a row...
                    {
                        switch (hCurrent)
                        {
                            case kHSTLeadingJamo:
                                if(hNext != kHSTTrailingJamo)
                                    goto Continue; // We cannot break;
                                break;

                            case kHSTVowelJamo:
                            case kHSTLVSyllable:
                                if((hNext == kHSTVowelJamo) || (hNext == kHSTTrailingJamo))
                                    goto Continue; // We cannot break;
                                break;

                            case kHSTTrailingJamo:
                            case kHSTLVTSyllable:
                                if(hNext == kHSTTrailingJamo)
                                    goto Continue; // We cannot break;
                                break;

                            case kHSTNone:
                                break;
                        }
                    }
                }

                break; // We can break here, as there isn't an unbreakable Hangul sequence.
            }

            Continue:
            cCurrent = cNext;
            ++mIterator;
            cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.
        }
    }

    return mIterator.GetPosition();
}



///////////////////////////////////////////////////////////////////////////////
// WordBreakIterator
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GetPrevChar
//
Char GetPrevChar(const CharacterBreakIterator& cbi)
{
    CharacterBreakIterator cbiPrev(cbi);
    cbiPrev.GetPrevCharBreak(); // Go back to cCurrent. Then we'll go back to cCurrent - 1.

    if(!cbiPrev.AtBegin())
    {
        cbiPrev.GetPrevCharBreak();
        return *cbiPrev;
    }

    return kCharInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetNextChar
//
Char GetNextChar(const CharacterBreakIterator& cbi)
{
    CharacterBreakIterator cbiNext(cbi);
    cbiNext.GetNextCharBreak(); // It's OK if we are at the end, the iterator will just stay there.

    return *cbiNext;
}


///////////////////////////////////////////////////////////////////////////////
// IsKatakana
//
static bool IsKatakana(Char c)
{
    // I'm not finding a clear definition of what is Katakana. Some Unicode
    // writings are saying that [0x3031 - 0x3035] are Katakana.

    if(((c - 0x30A0u) <= 0x30FFu - 0x30A0u) || // kBlockKatakana
       ((c - 0x3031u) <= 0x3035u - 0x3031u) || // Kana marks
       ((c - 0x31F0u) <= 0x31FFu - 0x31F0u))   // kBlockKatakanaPhoneticExtensions
    {
        return true;
    }

    return false;
}



///////////////////////////////////////////////////////////////////////////////
// IsALetter
// 
// These are use for identifying symbols that are alphabetic letters that 
// are part of a word.
//
static bool IsALetter(Char c)
{
    if(IsCharAlphabetic(c) || (c == kCharNBSP) || (c == 0x05F3))
    {
        if(!IsCharIdeographic(c) && !IsGraphemeExtend(c) && !IsKatakana(c))
        {
            const Script s = GetScriptFromChar(c);

            if((s != kScriptThai) && (s != kScriptLao) && (s != kScriptHiragana))
                return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// IsMidLetter
// 
// These are use for identifying symbols that are between regular 
// alphabetic letters and make up part of a word. Example: "don't"
//
static bool IsMidLetter(Char c)
{
    return ((c == 0x0027) ||  // APOSTROPHE
            (c == 0x00B7) ||  // MIDDLE DOT
            (c == 0x05F4) ||  // HEBREW PUNCTUATION GERSHAYIM
            (c == 0x2019) ||  // RIGHT SINGLE QUOTATION MARK (curly apostrophe)
            (c == 0x2027) ||  // HYPHENATION POINT
            (c == 0x003A));   // COLON (used in Swedish)
}


///////////////////////////////////////////////////////////////////////////////
// IsMidNum
// 
// These are use for identifying symbols within numbers, such as ',' and '.'
//
static bool IsMidNum(Char c)
{
    return ((c == 0x002C) ||  // COMMA
            (c == 0x002E) ||  // FULL STOP
            (c == 0x003B) ||  // SEMICOLON
            (c == 0x037E) ||  // GREEK QUESTION MARK
            (c == 0x0589) ||  // ARMENIAN FULL STOP
            (c == 0x060D) ||  // ARABIC DATE SEPARATOR
            (c == 0x2044) ||  // FRACTION SLASH
            (c == 0xFE10) ||  // PRESENTATION FORM FOR VERTICAL COMMA
            (c == 0xFE13) ||  // PRESENTATION FORM FOR VERTICAL COLON
            (c == 0xFE14));   // PRESENTATION FORM FOR VERTICAL SEMICOLON
}


///////////////////////////////////////////////////////////////////////////////
// IsLineBreakNumeric
// 
// Returns true if the line break property of the character is Numeric.
// This means digit characters. Note that Chinese has a symbol for one, two,
// three, etc, but these are not digit characters but are instead words.
//
static bool IsLineBreakNumeric(Char c)
{
    // We choose to support a subset of the full Unicode script set.
    // For a full list of all LineBreak Numerics, see the LineBreak.txt 
    // file from the Unicode database and look for chars of type NU.

    // Alternative mechanism which is table-based, but supports all scripts:
    //    return (GetCharCategory(c) == kCCDecimalDigitNumber);

    // Alternative mechanism which is like below but more general but possibly slower:
    //    return (GetLineBreakCategory(c) == kLBCNumeric);

    if((c - 0x0030u) < 10u) // Western digits ('0', '1', etc.)
        return true;

    if((c - 0x0660u) < 10u) // Arabic-indic digits
        return true;

    if((c - 0x06F0u) < 10u) // Extended Arabic-indic digits
        return true;

    if((c - 0x0966u) < 10u) // Devanagari (Hindi) digits
        return true;

    if((c - 0x0E50u) < 10u) // Thai digits
        return true;

    if((c == 0x066B) || (c == 0x066C)) // Arabic decimal and thousands separators.
        return true;                   // I'm not sure if we want to go through the trouble of checking this.

    // Disabled because we save space by not supporting the 
    // following scripts, though we can possibly have an 
    // enabling #define to do so.
    //
    //if((c - 0x09E6u) < 10u) // Bengali digits
    //    return true;
    //
    //if((c - 0x0A66u) < 10u) // Gurmukhi digits
    //    return true;
    //
    //if((c - 0x0AE6u) < 10u) // Gujarati digits
    //    return true;
    //
    //if((c - 0x0B66u) < 10u) // Oriya digits
    //    return true;
    //
    //if((c - 0x0BE6u) < 10u) // Tamil digits
    //    return true;
    //
    //if((c - 0x0C66u) < 10u) // Telugu digits
    //    return true;
    //
    //if((c - 0x0CE6u) < 10u) // Kannada digits
    //    return true;
    //
    //if((c - 0x0D66u) < 10u) // Malayalam digits
    //    return true;
    //
    //if((c - 0x0ED0u) < 10u) // Lao digits
    //    return true;
    //
    //if((c - 0x0F20u) < 10u) // Tibetan digits
    //    return true;
    //
    //if((c - 0x1040u) < 10u) // Myanmar digits
    //    return true;
    //
    //if((c - 0x17E0u) < 10u) // Khmer digits
    //    return true;
    //
    //if((c - 0x1810u) < 10u) // Mongolian digits
    //    return true;
    //
    //if((c - 0x1946u) < 10u) // Limbu digits
    //    return true;
    //
    //if((c - 0x19D0u) < 10u) // New Tai Lue digits
    //    return true;

    return false;
}


uint32_t WordBreakIterator::GetPrevWordBreak(int /*wordBreakType*/)
{
    // Currently we do the not-highly-efficient technique of starting
    // at the front and iterating forwards, as opposed to the technique
    // of iterating backwards. The latter would require logic similar to
    // but not identical to iterating fowards. For the time being,
    // we do it this way in order to save a few hours' coding time.
    const uint32_t nOriginalPosition = mIterator.GetPosition();

    uint32_t nCurrentPosition = 0;
    mIterator.SetPosition(0);

    while(!mIterator.AtEnd())
    {
        const uint32_t nNewPosition = GetNextWordBreak();

        if(nNewPosition >= nOriginalPosition)
        {
            mIterator.SetPosition(nCurrentPosition);
            break;
        }

        nCurrentPosition = nNewPosition;
    }

    return mIterator.GetPosition();
}


uint32_t WordBreakIterator::GetNextWordBreak(int)
{
    // See Unicode technical report #29.
    // We need to iterate by grapheme clusters, and thus will use our CharBreakIterator
    // to do the base navigation instead of directly using our mIterator.

    if(!mIterator.AtEnd())
    {
        Char cCurrent = *mIterator; // As per rule 3, the first char of the cluster is the char used for the word break determination.
        GetNextCharBreak();         // Go to the beginning of the next character cluster.
        Char cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.

        // What this loop does is find any reason that the current character *isn't* at a word break.
        while(!mIterator.AtEnd())
        {
            // Here we create a scope here for the code below, due to our declaration of variables within the code.
            {
                // As per rule #4, if we have a format character, we ignore it and use whatever character was before it.
                // Note: We need to do the same thing forward if cNext is a format character. 
                if((cCurrent != kCharZWNJ) && (cCurrent != kCharZWJ) && (GetCharCategory(cCurrent) == kCCFormatChar))
                {
                    CharacterBreakIterator cbiPrev(*this);
                    cbiPrev.GetPrevCharBreak(); // Go back to cCurrent. Then we'll go back to cCurrent - 1.
                
                    while(!cbiPrev.AtBegin())
                    {
                        cbiPrev.GetPrevCharBreak();
                        cCurrent = *cbiPrev;
                
                        if((cCurrent == kCharZWNJ) || (cCurrent == kCharZWJ) || (GetCharCategory(cCurrent) != kCCFormatChar))
                            break;
                    }
                }

                if((cNext != kCharZWNJ) && (cNext != kCharZWJ) && (GetCharCategory(cNext) == kCCFormatChar))
                    goto Continue; // Rule #4. // Format characters can't be word break boundaries.

                // Alphabetic checks. 
                const bool bCurrentIsALetter = IsALetter(cCurrent);
                const bool bNextIsALetter    = IsALetter(cNext);

                if(bCurrentIsALetter && bNextIsALetter)
                    goto Continue; // Rule #5. Don't break between two alphabetic characters.

                // To consider: Check for apostrophes next to vowels in some European languages (rule #5a).

                if(bCurrentIsALetter && IsMidLetter(cNext)) // If we have a case like "don't" or "abc:def" or "abc.def" (where . is not period but a Unicode middle dot).
                {
                    // We need to peek at the letter after cNext. We are looking for a sequence such as "n't" in the word "can't".
                    const Char cNextNext = GetNextChar(*this);

                    if(IsALetter(cNextNext))
                        goto Continue; // Rule #6. Don't break on an intra-word 'mid-letter'.
                    // Else we have a case like "don' " or "don'5" and so the mid letter doesn't count as binding two parts of a word.
                }

                if(IsMidLetter(cCurrent) && bNextIsALetter) // If we have a case like "?'t" or "?:def" (where ? means unknown and we need to go back and look at it)...
                {
                    // We need to peek at the letter before cCurrent. We are looking for a sequence such as "n't" in the word "can't".
                    const Char cPrev = GetPrevChar(*this);

                    if(IsALetter(cPrev))
                        goto Continue; // Rule #7. Don't break on an intra-word 'mid-letter'.
                    // Else we have a case like " 't" or "5:def" and so the mid letter doesn't count as binding two parts of a word.
                }

                // To consider: Do checks here for words in scripts that can't be broken by character
                // rules. Usually these involve dictionary approaches. Thai is such an example.

                // Numeric checks
                const bool bNextIsNumeric = IsLineBreakNumeric(cNext); // IsLineBreakNumeric is basically the same as isdigit() but more locale-aware.

                if(bCurrentIsALetter && bNextIsNumeric) // If we have a case like "abc5"...
                    goto Continue; // Rule #9. A letter followed by a digit doesn't cause a word break.

                const bool bCurrentIsNumeric = IsLineBreakNumeric(cCurrent);

                if(bCurrentIsNumeric)
                {
                    if(bNextIsNumeric || bNextIsALetter) // If we have a case like "56" or "5a"...
                        goto Continue; // Rule #8 & #10. Letters and digits next to each other don't cause word breaks.

                    if(IsMidNum(cNext)) // If cNext is '.' or ',' or some other chars that can appear within a number.
                    {
                        // We need to peek at the letter after cNext. We are looking for a sequence such as "3.7" in the word "163.765".
                        const Char cNextNext = GetNextChar(*this);

                        if(IsLineBreakNumeric(cNextNext)) // If the character after the (e.g.) decimal point is a digit...
                            goto Continue; // Rule #12. Don't break within a number.
                    }
                }

                if(IsMidNum(cCurrent) && bNextIsNumeric) // If we have a case like ".7"...
                {
                    // We need to peek at the letter before cCurrent. We are looking for a sequence such as "3.7" in the word "163.765".
                    const Char cPrev = GetPrevChar(*this);

                    if(IsLineBreakNumeric(cPrev)) // If the character before the (e.g.) decimal point is a digit...
                        goto Continue; // Rule #11. Don't break within a number.
                }

                const bool bCurrentIsKatakana = IsKatakana(cCurrent);
                const bool bNextIsKatakana    = IsKatakana(cNext);

                if(bCurrentIsKatakana && bNextIsKatakana)
                    goto Continue; // Rule #13. Don't break within Katakana.

                // To consider: Include '.' to count towards bNextIsExtendNumLet.
                const bool bNextIsExtendNumLet = IsCharCategory(cNext, kCCFlagConnectorPunctuation); // These are '_' chars and are similar connector chars that Japanese uses.

                if(bNextIsExtendNumLet) // if cNext is '_' or a similar connector char...
                {
                    if(bCurrentIsALetter || bCurrentIsNumeric || bCurrentIsKatakana)
                        goto Continue; // Rule #13a. Don't break between alphanumerics and connector chars.
                }

                const bool bCurrentIsExtendNumLet = IsCharCategory(cCurrent, kCCFlagConnectorPunctuation);

                if(bCurrentIsExtendNumLet)
                {
                    if(bNextIsALetter || bNextIsNumeric || bNextIsKatakana || bNextIsExtendNumLet)
                        goto Continue; // Rule #13b. Don't break between alphanumerics and connector chars.
                }
            }

            // We are at a word break. However, the user may have set mWordBreakType to filter 
            // some types of word breaks (e.g. user wants just beginning-of-word) word breaks.
            if((mWordBreakType & kWordBreakTypeAll) != kWordBreakTypeAll) // If the user specified anything but all... 
            {
                // Note: This is not a fully correct solution, as words can break 
                // when there is something other than space between them. A proper 
                // solution probably involves addition of some state maintenance above.

                // For this code, let's assume we have the string "abc  def".
                //                                                    ^^^
                //                                                    eib
                // The three positions marked above refer to word end, word inter, and word begin break positions.

                const bool bCurrentIsSpace = IsSpace(cCurrent, kSTAll, false);
                const bool bNextIsSpace    = IsSpace(cNext,    kSTAll, false);

                if(mWordBreakType & kWordBreakTypeBegin)
                {
                    if(bCurrentIsSpace && !bNextIsSpace)
                        break;
                }

                if(mWordBreakType & kWordBreakTypeEnd)
                {
                    if(!bCurrentIsSpace && bNextIsSpace)
                        break;
                }

                if(mWordBreakType & kWordBreakTypeInter)
                {
                    if(bCurrentIsSpace && bNextIsSpace)
                        break;
                }
            }
            else
                break; // The user wants to break at all WordBreakTypes

            Continue:
            cCurrent = cNext;
            GetNextCharBreak();     // Go to the beginning of the next character cluster.
            cNext    = *mIterator;  // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.
        }
    }

    return mIterator.GetPosition();
}



///////////////////////////////////////////////////////////////////////////////
// SentenceBreakIterator
///////////////////////////////////////////////////////////////////////////////

static bool IsBreakUppercase(Char c)
{
    return IsCharCategory(c, kCCFlagUppercaseLetter | kCCFlagTitlecaseLetter);

    // Also, strictly speaking, 0x2160 - 0x216F (Roman numerals) and
    // 0x24B6 - 0x24CF (Latin letters with circles around them) are upper case. 
    // We'll ignore them for now.
}


/*
static bool IsLineBreakQuotation(Char c) // This branch is taken the large majority of the time.
{
    if(c < 0x0218) // This branch is taken the large majority of the time.
        return ((c == 0x0022) || (c == 0x0027) || (c == 0x00AB) || (c == 0x00BB));

    if(c < 0x021E)
    {
        // To consider: we can possibly dispose of these checks, as these are uncommon chars.
        if((c - 0x2E00u) <= (0x2E0Du - 0x2E00u))
            return true;

        switch (c)
        {
            case 0x0218:
            case 0x2019:
            case 0x201B:
            case 0x201C:
            case 0x201D:
            case 0x201F:
            case 0x2039:
            case 0x203A:
            case 0x23B6:
            case 0x275B:
            case 0x275C:
            case 0x275D:
            case 0x275E:
            case 0x2E1C:
            case 0x2E1D:
                return true;
        }
    }

    return false;
}

static bool IsSep(Char c)
{
    return ((c == 0x000A) || // LINE FEED (LF)
            (c == 0x000D) || // CARRIAGE RETURN (CR)
            (c == 0x0085) || // NEXT LINE (NEL)
            (c == 0x2028) || // LINE SEPARATOR (LS)
            (c == 0x2029));  // PARAGRAPH SEPARATOR (PS)
}

static bool IsSentenceSpace(Char c)
{
    return IsCharWhiteSpace(c) && !IsSep(c) && (c != 0x00A0);
}

static bool IsSentenceClose(Char c)
{
    if(IsCharCategory(c, kCCFlagStartPunctuation | kCCFlagEndPunctuation) || IsLineBreakQuotation(c))
    {
        if((c != 0x002E) && !IsCharSTerm(c) && (c != 0x05F3))
            return true;
    }

    return false;
}
*/


uint32_t SentenceBreakIterator::GetPrevSentenceBreak()
{
    // Currently we do the not-highly-efficient technique of starting
    // at the front and iterating forwards, as opposed to the technique
    // of iterating backwards. The latter would require logic similar to
    // but not identical to iterating fowards. 
    const uint32_t nOriginalPosition = mIterator.GetPosition();

    uint32_t nCurrentPosition = 0;
    mIterator.SetPosition(0);

    while(!mIterator.AtEnd())
    {
        const uint32_t nNewPosition = GetNextSentenceBreak();

        if(nNewPosition >= nOriginalPosition)
        {
            mIterator.SetPosition(nCurrentPosition);
            break;
        }

        nCurrentPosition = nNewPosition;
    }

    return mIterator.GetPosition();
}



uint32_t SentenceBreakIterator::GetNextSentenceBreak()
{
    // See Unicode technical report #29.
    // We need to iterate by grapheme clusters, and thus will use our CharBreakIterator
    // to do the base navigation instead of directly using our mIterator.
    // We interpret the rules in TR 29 in a fairly direct way. Our best alternative
    // may be to employ a regular expression evaluator.

    if(!mIterator.AtEnd())
    {
        int nRelativePosition(0);

        Char cCurrent = *mIterator; // As per rule 4, the first char of the cluster is the char used for the word break determination.
        GetNextCharBreak();
        Char cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.

        while(!mIterator.AtEnd())
        {
            if((cCurrent == kCharLF) || (cCurrent == kCharCR) || (cCurrent == kCharNEL) || (cCurrent == kCharLSEP) || (cCurrent == kCharPSEP))
                break; // As per rule 3, break after paragraph separators.

            // As per rule #5, if we have a format character, we ignore it and use whatever character was before it.
            // Note: We need to do the same thing forward if cNext is a format character.
            if((cCurrent != kCharZWNJ) && (cCurrent != kCharZWJ) && (GetCharCategory(cCurrent) == kCCFormatChar))
            {
                CharacterBreakIterator cbiPrev(*this);
                cbiPrev.GetPrevCharBreak(); // Go back to cCurrent. Then we'll go back to cCurrent - 1.
            
                while(!cbiPrev.AtBegin())
                {
                    cbiPrev.GetPrevCharBreak();
                    cCurrent = *cbiPrev;
            
                    if((cCurrent == kCharZWNJ) || (cCurrent == kCharZWJ) || (GetCharCategory(cCurrent) != kCCFormatChar))
                        break;
                }
            }

            if((cNext != kCharZWNJ) && (cNext != kCharZWJ) && (GetCharCategory(cNext) == kCCFormatChar))
                goto Continue; // Rule #5.

            // What we do here is look for character sequences that involve periods but which 
            // don't terminate sentences. This kind of pattern matching is better done with 
            // conventional pattern matching such as regular expressions. However, it would be
            // fairly expensive computation-wise to do such a thing, so we attempt to hard-code
            // a reasonable implementation here. These are rules #6 - #11.

            {
                const bool bCurrentIsATerm = (cCurrent == 0x002E); // 002E is FULL STOP (i.e. period)

                if(bCurrentIsATerm)
                {
                    const bool bNextIsNumeric = IsLineBreakNumeric(cNext);

                    if(bNextIsNumeric)
                        goto Continue; // Rule #6.

                    const bool bNextIsUpper = IsBreakUppercase(cNext);

                    if(bNextIsUpper)
                    {
                        // If prev is uppercase, we have an acronym like U.S. and we  
                        // don't want to consider the period to be a sentence ender.
                        const Char cPrev = GetPrevChar(*this);
                        const bool bPrevIsUpper = IsBreakUppercase(cPrev);

                        if(bPrevIsUpper)
                            goto Continue; // Rule #7
                    }
                }
            }

            // To do: Need to implement rules 8-11. These rules require more complicated 
            // wildcard-like matching, but aren't too hard to implement directly.

            // Check for that which can end a sentence. This is a poor man's version of rule #11.
            if(IsCharSTerm(cCurrent))
                break;

            Continue:
            ++nRelativePosition;
            cCurrent = cNext;
            GetNextCharBreak();
            cNext    = *mIterator;  // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.
        }
    }

    return mIterator.GetPosition();
}


///////////////////////////////////////////////////////////////////////////////
// ParagraphBreakIterator
///////////////////////////////////////////////////////////////////////////////

uint32_t ParagraphBreakIterator::GetPrevParagraphBreak()
{
    // Currently we do the not-highly-efficient technique of starting
    // at the front and iterating forwards, as opposed to the technique
    // of iterating backwards. The latter would require logic similar to
    // but not identical to iterating fowards. 
    const uint32_t nOriginalPosition = mIterator.GetPosition();

    uint32_t nCurrentPosition = 0;
    mIterator.SetPosition(0);

    while(!mIterator.AtEnd())
    {
        const uint32_t nNewPosition = GetNextParagraphBreak();

        if(nNewPosition >= nOriginalPosition)
        {
            mIterator.SetPosition(nCurrentPosition);
            break;
        }

        nCurrentPosition = nNewPosition;
    }

    return mIterator.GetPosition();
}


uint32_t ParagraphBreakIterator::GetNextParagraphBreak()
{
    if(!mIterator.AtEnd())
    {
        Char cCurrent = *mIterator;
        ++mIterator;
        Char cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid in the case that mIterator hit the end.

        while(!mIterator.AtEnd())
        {
            if(cCurrent == kCharCR)
            {
                if(cNext != kCharLF)
                    break; // We can break here.
                // Else we can never break between CR and LF.
            }
            else if(cCurrent == kCharLF)
            {
                break; // We can break here (as \n\r counts as two line breaks.)
            }
            else if(cCurrent  == kCharPSEP)
                break;

            cCurrent = cNext;
            ++mIterator;
            cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.
        }
    }

    return mIterator.GetPosition();
}



///////////////////////////////////////////////////////////////////////////////
// LineBreakIterator
///////////////////////////////////////////////////////////////////////////////

enum BreakAction
{
    kBADirect,              // A line break opportunity exists between two adjacent characters of the given line breaking classes. e.g. "a b"
    kBAIndirect,            // A line break opportunity exists between two characters of the given line breaking classes only if they are separated by one or more spaces. e.g. "a \"b"
    kBACombiningIndirect,   // 
    kBACombiningProhibited, // 
    kBAProhibited,          // No line break opportunity exists between two characters of the given line breaking classes, even if they are separated by one or more space characters.
    kBAExplicit,            // \n, paragraph separator, etc.
    kBAHangulSpace          // 
};


// Define some short-cuts for the table
#define oo kBADirect
#define SS kBAIndirect
#define cc kBACombiningIndirect
#define CC kBACombiningProhibited 
#define XX kBAProhibited


// Break table
//
// The Southeast Asian (Thai/Lao) column is the same as AL for this table, 
// as we use this row of the table only when we are analyzing a transition from non-Thai to Thai.
//
static const char kBreakTable[][kLBCSoutheastAsian + 1] =
{   //                  ---     'after'  class  ------
    //        1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21   22  23  24  25  26  27
    //       OP, CL, QU, GL, NS, EX, SY, IS, PR, PO, NU, AL, ID, IN, HY, BA, BB, B2, ZW, CM, WJ,  H2, H3, JL, JV, JT, SA = after class
    /*OP*/ { XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, CC, XX,  XX, XX, XX, XX, XX, XX, }, // OP open
    /*CL*/ { oo, XX, SS, SS, XX, XX, XX, XX, oo, SS, oo, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // CL close
    /*QU*/ { XX, XX, SS, SS, SS, XX, XX, XX, SS, SS, SS, SS, SS, SS, SS, SS, SS, SS, XX, cc, XX,  SS, SS, SS, SS, SS, SS, }, // QU quotation
    /*GL*/ { SS, XX, SS, SS, SS, XX, XX, XX, SS, SS, SS, SS, SS, SS, SS, SS, SS, SS, XX, cc, XX,  SS, SS, SS, SS, SS, SS, }, // GL glue
    /*NS*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // NS no-start
    /*EX*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // EX exclamation/interrogation
    /*SY*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // SY Syntax (slash)
    /*IS*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, SS, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, SS, }, // IS infix (numeric) separator
    /*PR*/ { SS, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, SS, SS, oo, SS, SS, oo, oo, XX, cc, XX,  SS, SS, SS, SS, SS, SS, }, // PR prefix
    /*PO*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // NU numeric
    /*NU*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, SS, SS, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, SS, }, // AL alphabetic
    /*AL*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, SS, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, SS, }, // AL alphabetic
    /*ID*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // ID ideograph (atomic)
    /*IN*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // IN inseparable
    /*HY*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // HY hyphens and spaces
    /*BA*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, oo, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // BA break after 
    /*BB*/ { SS, XX, SS, SS, SS, XX, XX, XX, SS, SS, SS, SS, SS, SS, SS, SS, SS, SS, XX, cc, XX,  SS, SS, SS, SS, SS, SS, }, // BB break before 
    /*B2*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, oo, oo, oo, oo, SS, SS, oo, XX, XX, cc, XX,  oo, oo, oo, oo, oo, oo, }, // B2 break either side, but not pair
    /*ZW*/ { oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, oo, XX, oo, oo,  oo, oo, oo, oo, oo, oo, }, // ZW zero width space
    /*CM*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, oo, SS, SS, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, oo, SS, }, // CM combining mark
    /*WJ*/ { SS, XX, SS, SS, SS, XX, XX, XX, SS, SS, SS, SS, SS, SS, SS, SS, SS, SS, XX, cc, XX,  SS, SS, SS, SS, SS, SS, }, // WJ word joiner
                                                                                                                        
    /*H2*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, SS, SS, oo, }, // Hangul 2 Jamo syllable
    /*H3*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, SS, oo, }, // Hangul 3 Jamo syllable
    /*JL*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  SS, SS, SS, SS, oo, oo, }, // Jamo Leading Consonant
    /*JV*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, SS, SS, oo, }, // Jamo Vowel
    /*JT*/ { oo, XX, SS, SS, SS, XX, XX, XX, oo, SS, oo, oo, oo, SS, SS, SS, oo, oo, XX, cc, XX,  oo, oo, oo, oo, SS, oo, }, // Jamo Trailing Consonant 
};


#undef oo
#undef SS
#undef cc
#undef CC
#undef XX



uint32_t LineBreakIterator::GetPrevLineBreak(int lineBreakType)
{
    // Currently we do the not-highly-efficient technique of starting
    // at the front and iterating forwards, as opposed to the technique
    // of iterating backwards. The latter would require logic similar to
    // but not identical to iterating fowards.
    const uint32_t nOriginalPosition = mIterator.GetPosition();

    uint32_t nCurrentPosition = 0;
    mIterator.SetPosition(0);

    while(!mIterator.AtEnd())
    {
        const uint32_t nNewPosition = GetNextLineBreak(lineBreakType);

        if(nNewPosition >= nOriginalPosition)
        {
            mIterator.SetPosition(nCurrentPosition);
            break;
        }

        nCurrentPosition = nNewPosition;
    }

    return mIterator.GetPosition();
}


///////////////////////////////////////////////////////////////////////////////
// GetLineBreakCategoryFiltered
//
// This is a filtered version of GetLineBreakCategory which modifes some of 
// the results for our LineBreakIterator's purposes.
//
static LineBreakCategory GetLineBreakCategoryFiltered(Char c)
{
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4061 4062) // enumerate in switch of enum is not explicitly handled by a case label
    #endif

    LineBreakCategory lbc = GetLineBreakCategory(c);

    switch(lbc)
    {
        case kLBCContingentBreak:
        case kLBCSurrogate:
        case kLBCAmbiguous:
        case kLBCUnknown:
            return kLBCAlphabetic;

        case kLBCParagraphSeparator:
        case kLBCNextLine:
            return kLBCMandatoryBreak;

        default:
            break;
    }

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif

    return lbc;
}


///////////////////////////////////////////////////////////////////////////////
// GetNextSoutheastAsianLineBreak
//
// This function returns kLineBreakTypeNone or kLineBreakTypePossible, 
// based on a lookup for Thai (or similar) line breaks. If this function returns 
// kLineBreakTypePossible, cCurrent is set to the last char before the break,
// cNext is set to the first char after the break, lbcCurrent and lbcBase 
// equal to the line break category of cCurrent, lbcNext equal to the line 
// break category of cNext.
//
LineBreakType LineBreakIterator::GetNextSoutheastAsianLineBreak(Char& cCurrent, Char& cNext, LineBreakCategory& lbcBase, 
                                                                LineBreakCategory& lbcCurrent, LineBreakCategory& lbcNext)
{
    #if EATEXT_UNISCRIBE_ENABLED
        TextRunIterator triTemp(mIterator);
        const int       kTextCapacity = 512;
        WCHAR           text[kTextCapacity];
        int             textCount = 0;
        const int       kScriptItemCapacity = 32;
        SCRIPT_ITEM*    pScriptItemArray;
        SCRIPT_ITEM     scriptItemArrayLocal[kScriptItemCapacity];
        SCRIPT_ITEM*    pScriptItemArrayAllocated = NULL;
        int             scriptItemCount = 0;
        HRESULT         hr;
        LineBreakType   lbtResult = kLineBreakTypeNone;

        #ifdef EA_DEBUG
            memset(text, 0, sizeof(text));
            memset(scriptItemArrayLocal, 0, sizeof(scriptItemArrayLocal));
        #endif

        // Copy our iterated text into text/textCount.
        text[textCount++] = cCurrent;
        while(!triTemp.AtEnd() && (textCount < kTextCapacity))
            text[textCount++] = *triTemp++;

        // Check gBreakDictionaryAdditionList for additional dictionary items.
        // This is a linear check. If it turns out that we end up having more 
        // than about 50 dictionary entries, we may need to use a different 
        // data structure and search method.
        if(gpBreakDictionaryAdditionList)
        {
            for(StringList::const_iterator it = gpBreakDictionaryAdditionList->begin(), itEnd = gpBreakDictionaryAdditionList->end(); it != itEnd; ++it)
            {
                const String& sWord = *it;
                const Char*   pW    = sWord.data();
                const Char*   pT    = text;

                while(*pW && (*pW == *pT))
                {
                    ++pW;
                    ++pT;
                }

                if(*pW == 0) // If there is a match... return the entire word.
                {
                    for(eastl_size_t i = 1, iEnd = sWord.size(); i != iEnd; ++i, ++mIterator)
                        cCurrent = *mIterator;

                    cNext      = *mIterator;
                    lbcCurrent = GetLineBreakCategoryFiltered(cCurrent);
                    lbcNext    = GetLineBreakCategoryFiltered(cNext);

                    return kLineBreakTypePossible;
                }
            }
        }

        // First try to use our local buffer. Usually this will be enough.
        pScriptItemArray = scriptItemArrayLocal;
        hr = ScriptItemize(text, textCount, kScriptItemCapacity - 1, NULL, NULL, pScriptItemArray, &scriptItemCount);

        if(FAILED(hr) && (hr == E_OUTOFMEMORY))
        {
            // Try again with a larger amount. There is a minor weakness here in that a
            // long string can result in this GetNextSoutheastAsianLineBreak function being
            // called repeatedly as break positions are found and result in a number of 
            // Alloc/Free pairs for each call. It would be better to have a growable buffer
            // which sticks around until library shutdown. 
            const size_t kAllocationSize = sizeof(SCRIPT_ITEM) * (kTextCapacity + 1);
            pScriptItemArray = pScriptItemArrayAllocated = (SCRIPT_ITEM*)EA::Text::GetAllocator()->Alloc(kAllocationSize, EATEXT_ALLOC_PREFIX, 0);

            if(pScriptItemArray)
            {
                #ifdef EA_DEBUG
                    memset(pScriptItemArray, 0, kAllocationSize);
                #endif
                hr = ScriptItemize(text, textCount, kTextCapacity, NULL, NULL, pScriptItemArray, &scriptItemCount);
            }
        }

        if(SUCCEEDED(hr))
        {
            // To do: Write a loop here which continues until it finds the next break.
            // This is better than using just the first itemization because it may turn 
            // out that there's a break right after the Thai code but which is somehow 
            // in a separate itemization. We don't do this now because this code is new
            // as of this writing and we'll see how things go in practice before worrying
            // about this. 
            SCRIPT_LOGATTR scriptLogAttrArray[kTextCapacity];
          //const int      nItemLength = (scriptItemCount > 1) ? pScriptItemArray[1].iCharPos : textCount;  // Disabled because it has the problem of stopping at the end of the current script, whereas the Thai might be followed by Western punctuation (for example) that it cannot break before.
            const int      nItemLength = textCount;

            memset(scriptLogAttrArray, 0, sizeof(scriptLogAttrArray));

            hr = ScriptBreak(text, nItemLength, &pScriptItemArray[0].a, scriptLogAttrArray);

            if(SUCCEEDED(hr))
            {
                const SCRIPT_LOGATTR* pLogAttr    = scriptLogAttrArray + 1;
                const SCRIPT_LOGATTR* pLogAttrEnd = scriptLogAttrArray + nItemLength;

                for(; pLogAttr < pLogAttrEnd; ++pLogAttr)
                {
                    if(pLogAttr->fSoftBreak && pLogAttr->fCharStop)
                    {
                        lbtResult = kLineBreakTypePossible;
                        break;
                    }

                    cCurrent   = cNext;
                    cNext      = *++mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.
                    lbcBase    = lbcNext;
                    lbcCurrent = lbcNext;
                    lbcNext    = GetLineBreakCategoryFiltered(cNext);
                }
            }
        }

        if(pScriptItemArrayAllocated)
            EA::Text::GetAllocator()->Free(pScriptItemArrayAllocated);

        return lbtResult;
    #else
        (void)cCurrent;
        (void)cNext;
        (void)lbcBase;
        (void)lbcCurrent;
        (void)lbcNext;

        return kLineBreakTypeNone;
    #endif
}


uint32_t LineBreakIterator::GetNextLineBreak(int lineBreakType)
{
    // We implement Unicode Standard Annex #14 (a.k.a. Unicode TR14)

    if(!mIterator.AtEnd())
    {
        Char cCurrent = *mIterator;
        ++mIterator;
        Char cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.

        LineBreakCategory lbcBase    = GetLineBreakCategoryFiltered(cCurrent);
        LineBreakCategory lbcCurrent = lbcBase;
        LineBreakCategory lbcNext;

        bool bLoopEndedDueToLoopBreak = false; // True if a 'break' statement ended the loop instead of the loop condition being false.

        if(lbcBase != kLBCLineFeed)
        {
            // If we're starting with space, we don't 
            if(lbcBase == kLBCSpace)
                lbcBase = kLBCAlphabetic;

            lbcNext = GetLineBreakCategoryFiltered(cNext);

            if(mbUseWesternBreaksWithKorean)
            {
                // Korean text sometimes works like other Asian scripts and can break lines between ideographs,
                // but other times it acts like Western script and breaks only between sentences (e.g. between spaces).
                // The Unicode Standard says that the former is used for justified alignment while the latter is used
                // for ragged (i.e. left, right, center alignment). 
                EA_COMPILETIME_ASSERT((kLBCHangulJamo2 + 4) == kLBCHangulJamoTrailing);
                if((lbcBase >= kLBCHangulJamo2) && (lbcBase <= kLBCHangulJamoTrailing))
                    lbcBase = lbcCurrent = kLBCAlphabetic;
                if((lbcNext >= kLBCHangulJamo2) && (lbcNext <= kLBCHangulJamoTrailing))
                    lbcNext = kLBCAlphabetic;
            }

            while(!mIterator.AtEnd() && 
                  (lbcBase != kLBCMandatoryBreak) && 
                  ((lbcBase != kLBCCarriageReturn) || (lbcNext == kLBCLineFeed)))
            {
                bLoopEndedDueToLoopBreak = true;

                // Handle rule #4 (spaces) outside of the table lookup.
                if(lbcNext == kLBCSpace)
                    goto ContinueWithoutBaseUpdate;

                // Implement rules #3a - #3c.
                if((lbcNext == kLBCMandatoryBreak) || (lbcNext == kLBCLineFeed))
                {
                    lbcBase = kLBCMandatoryBreak;
                    goto ContinueWithoutBaseUpdate;
                }

                // Implement rules #3a - #3c.
                if(lbcNext == kLBCCarriageReturn)
                {
                    lbcBase = kLBCCarriageReturn;
                    goto ContinueWithoutBaseUpdate;
                }

                // To do: Handle hyphenation break opportunities here.
                // if(lineBreakType & kLineBreakTypeHyphen)
                // {
                //     // We need to step back and get the current word with a 
                //     // WordBreakIterator and do a hyphenation dictionary lookup.
                //     // If there is a hyphenation opportunity and it is here,
                //     // then break out with kLineBreakTypeHyphen here.
                // }

                // Handle rule-less scripts.
                if((lbcBase == kLBCSoutheastAsian) || (lbcNext == kLBCSoutheastAsian)) // If we are dealing with a script that has rule-less breaks (such as Thai, Lao, Myanmar, Khmer)...
                {
                    #if defined(EATEXT_THAI_SCRIPT_SUPPORTED) && EATEXT_THAI_SCRIPT_SUPPORTED
                        // The code here is a little weird. We have a separate routine to do Thai (and other Southeast Asian) breaking and we 
                        // call it while in the middle of our loop. But our state needs to be consistent, so we try to maintain the state 
                        // appropriately during the call to our separate routine. 
                        if(lineBreakType & kLineBreakTypePossible)
                        {
                            if(lbcBase == kLBCSoutheastAsian)
                            {
                                // At this point, we are in the middle or end of a Thai word. Call our Thai word break function.
                                const LineBreakType lbt = GetNextSoutheastAsianLineBreak(cCurrent, cNext, lbcBase, lbcCurrent, lbcNext);
    
                                if(lbt == kLineBreakTypeNone)
                                    mLineBreakType = mIterator.AtEnd() ? kLineBreakTypeMandatory : kLineBreakTypePossible;
                                else
                                    mLineBreakType = lbt;
                                break; // We can't tell from here what rule the GetNextSoutheastAsianLineBreak function applied, but the break usually occurred because the beginning of a new Thai word was found. 
                            }
                            // Else lbcBase is non-Thai or is a special Thai char and lbcNext refers to Thai. In this case we 
                            // fall through and let the regular logic handle this case.
                        }
                        else
                        {
                            if(lineBreakType & kLineBreakTypeEmergency)
                                goto ContinueWithEmergencyCheck;    // This isn't exactly right. We need to use this goto only if we are on a breakable character boundary.
                            else
                                goto ContinueWithoutBaseUpdate;
                        }
                    #else
                        if(lbcBase == kLBCSoutheastAsian)
                            lbcBase = kLBCAlphabetic;
                        if(lbcNext == kLBCSoutheastAsian)
                            lbcNext = kLBCAlphabetic;

                        if(lineBreakType & kLineBreakTypeEmergency)
                            goto ContinueWithEmergencyCheck;    // This isn't exactly right. We need to use this goto only if we are on a breakable character boundary.
                        else
                            goto ContinueWithoutBaseUpdate;
                    #endif
                }

                {
                    // We will be using the pairing table from here on, and so we want to
                    // make sure cCurrent and cNext come from within that table's bounds.
                    EA_ASSERT((lbcBase < kLBCSoutheastAsian) && (lbcNext <= kLBCSoutheastAsian));

                    const BreakAction ba = (BreakAction)kBreakTable[lbcBase][lbcNext];

                    if(ba == kBAIndirect)
                    {
                        if((lineBreakType & kLineBreakTypePossible) && (lbcCurrent == kLBCSpace)) // If the context is (A SP * B) ...
                        {
                            mLineBreakType = kLineBreakTypePossible;
                            break; // Rule #12. Break after spaces.
                        }
                    }
                    else if((ba == kBADirect) || (ba == kBAHangulSpace))
                    {
                        if(lineBreakType & kLineBreakTypePossible)
                        {
                            mLineBreakType = kLineBreakTypePossible;
                            break;
                        }
                    }
                    else if(ba == kBACombiningProhibited) // This is the case (OP SP* CM)
                    {
                        // We are dealing with rule #7b here.
                        if(lbcCurrent != kLBCSpace)
                            goto ContinueWithoutBaseUpdate;
                    }
                    else if(ba == kBACombiningIndirect) // If dealing with a combining mark break...
                    {
                        // We never break before a combining mark, unless it is a space.
                        if(lbcCurrent == kLBCSpace)
                        {
                            if(lineBreakType & kLineBreakTypePossible)
                            {
                                mLineBreakType = kLineBreakTypePossible;
                                break; // Rule #12. Break after spaces.
                            }
                        }
                        else
                            goto ContinueWithoutBaseUpdate;
                    }
                    else if(ba == kBAExplicit)
                    {
                        // If (ba == kBAExplicit), then regardless  of the user's input 'lineBreakType' value, 
                        // we break here. Note also that if lineBreakType is kLineBreakTypeEmergency then we 
                        // will always be breaking here for that break type.
                        mLineBreakType = kLineBreakTypeMandatory;
                        break;
                    }
                }

                ContinueWithEmergencyCheck:
                // Handle emergency break opportunities here.
                // In emergency breaking, we simply break on every grapheme cluster. 
                // This is the same thing as if the user simply used the CharacterBreakIterator.
                if(lineBreakType & kLineBreakTypeEmergency)
                {
                    mLineBreakType = kLineBreakTypeEmergency;
                    break;
                }

                // At this point, we will be moving onto the next character (without a break) and 
                // falling through to the next iteration.
                lbcBase = lbcNext;

                ContinueWithoutBaseUpdate:
                cCurrent = cNext;
                ++mIterator;
                cNext    = *mIterator; // Note that cNext might possibly be kCharInvalid, in the case that mIterator hit the end.

                lbcCurrent = lbcNext;
                lbcNext    = GetLineBreakCategoryFiltered(cNext);

                if(mbUseWesternBreaksWithKorean)
                {
                    if((lbcNext >= kLBCHangulJamo2) && (lbcNext <= kLBCHangulJamoTrailing))
                        lbcNext = kLBCAlphabetic;
                }

                bLoopEndedDueToLoopBreak = false;
            }
        }

        // If we exited the loop above due to the loop condition, 
        // then we call it a mandatory break;
        if(!bLoopEndedDueToLoopBreak)
            mLineBreakType = kLineBreakTypeMandatory;
    }

    return mIterator.GetPosition();
}





} // namespace Text

} // namespace EA








