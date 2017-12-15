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
// EATextUnicode.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextUnicode.h>
#include "internal/UnicodeData.inl"
#include <string.h>
#include <stdio.h>
#include EA_ASSERT_HEADER

    #pragma warning(push, 0)
    #include <Windows.h>
    #pragma warning(pop)

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 6385)  // Invalid data: accessing 'variable', the readable size is 'n' bytes, but 'n' bytes might be read.
#endif


namespace EA
{

namespace Text
{


/// upper_bound
///
/// This is similar to eastl::upper_bound and std::upper_bound. The reason we implement it 
/// here like this is that some users of this module want to be independent of EASTL and std STL.
/// The primary difference between this and the std upper_bound is that this requires 
/// RandomAccessIterator instead of just ForwardIterator.
///
template <typename RandomAccessIterator, typename T, typename Compare>
RandomAccessIterator
upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare compare)
{
    ptrdiff_t len = last - first;

    while(len > 0)
    {
        RandomAccessIterator i    = first;
        ptrdiff_t            len2 = len >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'.

        i += len2;

        if(!compare(value, *i))
        {
            first = ++i;
            len -= len2 + 1;
        }
        else
        {
            len = len2;
        }
    }

    return first;
}




///////////////////////////////////////////////////////////////////////////////
// GetSpaceProperties
//
static int GetSpaceProperties(Char c)
{
    switch (c)
    {
        case kCharLF:
        case kCharVT:
        case kCharFF:
        case kCharCR:
            return (kSTBreak | kSTControl);

        case kCharHT:
            return (kSTWidth | kSTBreak | kSTControl);

        case kCharSpace:
        case kCharNQSP:
        case kCharMQSP:
        case kCharENSP:
        case kCharEMSP:
        case kChar3MSP:
        case kChar4MSP:
        case kChar6MSP:
        case kCharPSP:
        case kCharTHSP:
        case kCharHSP:
        case kCharIDSP:
        // Also 0x205F (medium mathematical space)
            return (kSTWidth | kSTBreak);

        case kCharNBSP:
        case kCharFSP:  // Question: Should this char be here or above?
        // Also 0x202F (narrow no-break space)
            return (kSTWidth | kSTNoBreak);

        case kCharZWSP:
        case kCharZWNJ:     
            return (kSTZeroWidth | kSTBreak);

        case kCharZWJ:
        case kCharWJoin:
        case kCharZWNBSP   :
            return (kSTZeroWidth | kSTNoBreak);
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// IsSpace
//
EATEXT_API bool IsSpace(Char c, int spaceTypeFlags, bool bRequireAllFlags)
{
    const int spaceProperties = GetSpaceProperties(c);

    if(bRequireAllFlags)
        return (spaceProperties & spaceTypeFlags) == spaceTypeFlags;
    return (spaceProperties & spaceTypeFlags) != 0;
}



///////////////////////////////////////////////////////////////////////////////
// GetCharName
//
    EATEXT_API uint32_t GetCharName(Char c, Char* pName, uint32_t nNameCapacity)
    {
        // A list of all such characters can (as of this writing) be found at:
        //    http://www.unicode.org/Public/UNIDATA/NamesList.txt
        //
        // Under Win32, we simply use the OS-supplied GetUName.dll to retrieve the 
        // character names. Alternatively, we would go to the Unicode names list 
        // and pack it into some data file and access it there.
        typedef DWORD(__stdcall *GetUNameFunc)(uint16_t nUnicodeChar, Char* pName);

        static bool         bFunctionLinkAttemptedAlready = false;
        static GetUNameFunc pGetUName = NULL;

        if(!bFunctionLinkAttemptedAlready)
        {
            HINSTANCE hInstance = LoadLibraryA("GetUName.dll");
            if(hInstance)
                pGetUName = (GetUNameFunc)(void*)GetProcAddress((HMODULE)hInstance, "GetUName");
            bFunctionLinkAttemptedAlready = true;
        }

        if(pGetUName && (nNameCapacity >= 256))
            return (uint32_t)pGetUName(c, pName);
        return 0;
    }



///////////////////////////////////////////////////////////////////////////////
// GetMirrorChar
//
EATEXT_API Char GetMirrorChar(Char c)
{
    // We do a classic binary search of the RLE array.
    // To consider: Convert this to a hash table. If we don't use a hash table then consider  
    // breaking this into a two phase search where the most likely results are tested first.  
    int nLower = 0; // This must be a signed type.
    int nUpper = (int)gnMirrorPairArrayCount;

    while(nLower <= nUpper)
    {
        const int nMid = (nLower + nUpper) / 2;

        if(c == gpMirrorPairArray[nMid].mFirst)
            return gpMirrorPairArray[nMid].mSecond;
        else if(c < gpMirrorPairArray[nMid].mFirst)
            nUpper = nMid - 1;
        else // if(c > gpMirrorPairArray[nMid].mFirst)
            nLower = nMid + 1;
    }

    return c;
}


///////////////////////////////////////////////////////////////////////////////
// SetMirrorPairArray
//
/*
static void SetMirrorPairArray(MirrorPair* pMirrorPairArray, uint32_t nMirrorPairArrayCount)
{
    gpMirrorPairArray      = pMirrorPairArray;
    gnMirrorPairArrayCount = nMirrorPairArrayCount;

    #if EATEXT_DEBUG
        for(uint32_t i = 1; i < nMirrorPairArrayCount; i++)
            EA_ASSERT(gpMirrorPairArray[i].mFirst > gpMirrorPairArray[i - 1].mFirst);
    #endif
}
*/


///////////////////////////////////////////////////////////////////////////////
// IsCharAlphabetic
//
EATEXT_API bool IsCharAlphabetic(Char c)
{
    // Boolean property Alphabetic. Lu + Ll + Lt + Lm + Lo + Nl + Other_Alphabetic
    
    if(IsCharCategory(c, kCCFlagUppercaseLetter | kCCFlagLowercaseLetter | kCCFlagTitlecaseLetter | kCCFlagModifierLetter | kCCFlagOtherLetter | kCCFlagLetterNumber))
        return true;

    // We need to test for Other_Alphabetic. The Unicode Database PropList.txt file
    // lists them, though there are nearly 500 of them. We pick just the ones of 
    // interest to ourselves here. In particular, we are targeting Thai, Hebrew, 
    // Arabic, Greek, and Devanagari. The implementation here might well be better
    // done with a table, as it is starting to go beyond what works better in code.

    switch (c)
    {
        case 0x0E31:
        case 0x0E4D:
        case 0x05BF:
        case 0x05C7:
        case 0x0670:
        case 0x06ED:
        case 0x0345:
        case 0xFB1E:
            return true;
    }

    const unsigned cu = (unsigned)c;

    if(EA_UNLIKELY(cu >= 0x05B0)) // Most of the time this will evaluate to false.
    {
        if(((cu - 0x0E34u) <= (0x0E3Au - 0x0E34u)) || // Note that this is a faster way of saying (cu >= 0x0E34 && cu <= 0x0E3A), as it involves only a single comparison.
           ((cu - 0x05B0u) <= (0x05B9u - 0x05B0u)) || // However, it's important in such comparisons that we use the trailing 'u' in order to make the expression unsigned.
           ((cu - 0x05BBu) <= (0x05BDu - 0x05BBu)) ||
           ((cu - 0x05C1u) <= (0x05C2u - 0x05C1u)) ||
           ((cu - 0x05C4u) <= (0x05C5u - 0x05C4u)) ||
           ((cu - 0x0610u) <= (0x0615u - 0x0610u)) ||
           ((cu - 0x064Bu) <= (0x0657u - 0x064Bu)) ||
           ((cu - 0x0659u) <= (0x065Eu - 0x0659u)) ||
           ((cu - 0x06D6u) <= (0x06DCu - 0x06D6u)) ||
           ((cu - 0x06E1u) <= (0x06E4u - 0x06E1u)) ||
           ((cu - 0x06E7u) <= (0x06E8u - 0x06E7u)) ||
           ((cu - 0x0901u) <= (0x0903u - 0x0901u)) ||
           ((cu - 0x093Eu) <= (0x0940u - 0x093Eu)) ||
           ((cu - 0x0941u) <= (0x0948u - 0x0941u)) ||
           ((cu - 0x0949u) <= (0x094Cu - 0x0949u)) ||
           ((cu - 0x0962u) <= (0x0963u - 0x0962u)))
        {
            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// IsCharIdeographic
//
EATEXT_API bool IsCharIdeographic(Char c)
{
    if(c > 0x3000) // Most of the time this will evaluate to false. Unless we are working with Asian text, in which case it will usually be true.
    {
        if(((c - 0x3400u) <= (0x4DB5u - 0x3400u)) ||
           ((c - 0x4E00u) <= (0x9FBBu - 0x4E00u)) ||
           ((c - 0xF900u) <= (0xFA2Du - 0xF900u)) ||
           ((c - 0xFA70u) <= (0xFAD9u - 0xFA70u)) ||
           ((c - 0x3006u) <= (0x3007u - 0x3006u)) ||
           ((c - 0x3021u) <= (0x3029u - 0x3021u)) ||
           ((c - 0x3038u) <= (0x303Au - 0x3038u)))
        {
            return true;
        }
    }

    return false;
}
 

///////////////////////////////////////////////////////////////////////////////
// IsCharWhiteSpace
//
EATEXT_API bool IsCharWhiteSpace(Char c)
{
    if(c < 0x2000) // This branch is taken the large majority of the time.
    {
        return (c == 0x0020u) ||                          // SPACE
               ((c - 0x0009u) <= (0x000Du - 0x0009u)) ||  // <control-0009> - <control-000D>
               (c == 0x00A0u);                            // NO-BREAK SPACE
               
    }

    return ((c - 0x2000u) <= (0x200Au - 0x2000u)) ||    // EN QUAD - HAIR SPACE
           (c == 0x0085u) ||                            // <control-0085>
           (c == 0x1680u) ||                            // OGHAM SPACE MARK
           (c == 0x180Eu) ||                            // MONGOLIAN VOWEL SEPARATOR
           (c == 0x2028u) ||                            // LINE SEPARATOR
           (c == 0x2029u) ||                            // PARAGRAPH SEPARATOR
           (c == 0x202Fu) ||                            // NARROW NO-BREAK SPACE
           (c == 0x205Fu) ||                            // MEDIUM MATHEMATICAL SPACE
           (c == 0x3000u);                              // IDEOGRAPHIC SPACE
}


///////////////////////////////////////////////////////////////////////////////
// IsCharDigit
//
// http://www.fileformat.info/info/unicode/category/Nd/list.htm
// We don't support many of the esoteric language digits that the Unicode standard defines.
//
EATEXT_API bool IsCharDigit(Char c, int& digit)
{
    if(((unsigned)c - 0x0030u) <= (0x0039u - 0x0030u))
    {
        digit = ((unsigned)c - 0x0030u);
        return true;
    }

    #if defined(EATEXT_KATAKANA_SCRIPT_SUPPORTED) && EATEXT_KATAKANA_SCRIPT_SUPPORTED
        if(((unsigned)c - 0xFF10u) <= (0xFF19u - 0xFF10u))
        {
            digit = ((unsigned)c - 0xFF10u);
            return true;
        }
    #endif

    #if defined(EATEXT_THAI_SCRIPT_SUPPORTED) && EATEXT_THAI_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x0E50u) <= (0x0E59u - 0x0E50u))
        {
            digit = ((unsigned)c - 0x0E50u);
            return true;
        }
    #endif

    #if defined(EATEXT_ARABIC_SCRIPT_SUPPORTED) && EATEXT_ARABIC_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x0660u) <= (0x0669u - 0x0660u))
        {
            digit = ((unsigned)c - 0x0660u);
            return true;
        }
    #endif
 
    #if defined(EATEXT_DEVANAGARI_SCRIPT_SUPPORTED) && EATEXT_DEVANAGARI_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x0966u) <= (0x096fu - 0x0966u))
        {
            digit = ((unsigned)c - 0x0966u);
            return true;
        }

    #endif

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetBidiClass
//
EATEXT_API BidiClass GetBidiClass(Char c)
{
    // The information for this function is derived from the Unicode Standard
    // "Derived Bidi Class.txt" document and the "Unicode Data.txt" document.
    // The comparisons done here are not in order of Unicode value but are in
    // order from most common to least common.

    if(c < 256)
        return (BidiClass)gBidiClassTable256[(uint16_t)c];

    // Check CJ characters
    #if EATEXT_HAN_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x3400u) <= (0x9FBBu - 0x3400u))
            return kBidiClassL;
    #endif

    // Check Korean characters
    #if EATEXT_HANGUL_SCRIPT_SUPPORTED
        if(((unsigned)c - 0xAC00u) <= (0xD7A3u - 0xAC00u))
            return kBidiClassL;
    #endif

    // Check for asian numerals
    if(((unsigned)c - 0xFF10u) <= (0xFF19u - 0xFF10u)) // full width 0 - 9
        return kBidiClassEN;

    // Other numerals of possible interest.
    //06F0..06F9    ; EN # Nd  [10] EXTENDED ARABIC-INDIC DIGIT ZERO..EXTENDED ARABIC-INDIC DIGIT NINE
    //2070          ; EN # No       SUPERSCRIPT ZERO
    //2074..2079    ; EN # No   [6] SUPERSCRIPT FOUR..SUPERSCRIPT NINE
    //2080..2089    ; EN # No  [10] SUBSCRIPT ZERO..SUBSCRIPT NINE

    // Check for uncommon whitespace
    if(((unsigned)c - 0x2000u) <= (0x200Au - 0x2000u)) // A bunch of explicit space chars.
        return kBidiClassWS;
    if((c == 0x2028) || (c == 0x3000)) // LINE SEPARATOR, IDEOGRAPHIC SPACE
        return kBidiClassWS;

    // Check for non-spacing marks (usually diacriticals)
    if(((unsigned)c - 0x0300u) <= (0x036Fu - 0x0300u)) // Latin diacritics (accents and what-not)
        return kBidiClassNSM;

    // Check for common number separators
    if((c == 0x202F) || (c == 0x060C))
        return kBidiClassCS;

    // Check for European number terminator (currency symbols, actually)
    if(((unsigned)c - 0x20A0u) <= (0x20CFu - 0x20A0u))
        return kBidiClassET;

    // Check for Thai non-spacing marks.
    #if EATEXT_THAI_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x0E34u) <= (0x0E3Au - 0x0E34u))
            return kBidiClassNSM;
        if(((unsigned)c - 0x0E47u) <= (0x0E4Eu - 0x0E47u))
            return kBidiClassNSM;
        if(c == 0x0E31)
            return kBidiClassNSM;
    #endif

    // Check for Hebrew
    #if EATEXT_HEBREW_SCRIPT_SUPPORTED
        // Standard Hebrew alphabet.
        if(EA_UNLIKELY(((unsigned)c - 0x0590u) <= (0x05FFu - 0x0590u))) // Faster way of saying (c >= 0x0590 && c <= 0x05FF)
        {
            if(c >= 0x05C5)
                return kBidiClassR;
            return kBidiClassNSM;
        }

        #if (EATEXT_HEBREW_SCRIPT_SUPPORTED >= 2)
            // Hebrew presentation forms.
            if(EA_UNLIKELY(((unsigned)c - 0xFB1Du) <= (0xFB4Fu - 0xFB1Du)))
            {
                if(EA_UNLIKELY(c == 0xFB1E))
                    return kBidiClassNSM;
                return kBidiClassR;
            }
        #endif
    #endif

    #if EATEXT_ARABIC_SCRIPT_SUPPORTED
        // Arabic characters
        if(((unsigned)c - 0x0600u) <= (0x06FFu - 0x0600u))
            return (BidiClass)gArabicBidiClassTable256[c - 0x0600u];
    #endif

    // Check for boundary-neutrals, which are mostly control chars and zero-width space.
    if(c == 0xFEFF) // ZERO WIDTH NO-BREAK SPACE
        return kBidiClassBN;
    if(((unsigned)c - 0x200Bu) <= (0x200Du - 0x200Bu)) // ZERO WIDTH SPACE..ZERO WIDTH JOINER
        return kBidiClassBN;
    if(((unsigned)c - 0x2060u) <= (0x2063u - 0x2060u)) // WORD JOINER..INVISIBLE SEPARATOR
        return kBidiClassBN;

    // Check for other neutrals
    if(((unsigned)c - 0x2018u) <= (0x201Du - 0x2018u))
        return kBidiClassBN;

    // Check for directional overrides, which we very rarely see.
    if(((unsigned)c - 0x202Au) <= (0x202Eu - 0x202Au)) // Directional overrides.
    {
        switch (c) {
            case 0x202A:
                return kBidiClassLRE;
            case 0x202B:
                return kBidiClassRLE;
            case 0x202C:
                return kBidiClassPDF;
            case 0x202D:
                return kBidiClassLRO;
            case 0x202E:
                return kBidiClassRLO;
        }
    }

    // Check for Lao non-spacing marks.
    #if EATEXT_LAO_SCRIPT_SUPPORTED
        if(((unsigned)c - 0x0EB4u) <= (0x0EB9u - 0x0EB4u))
            return kBidiClassNSM;
        if(((unsigned)c - 0x0EBBu) <= (0x0EBCu - 0x0EBBu))
            return kBidiClassNSM;
        if(((unsigned)c - 0x0EC8u) <= (0x0ECDu - 0x0EC8u))
            return kBidiClassNSM;
        if(c == 0x0EB1)
            return kBidiClassNSM;
    #endif

    return kBidiClassL;
}


///////////////////////////////////////////////////////////////////////////////
// IsCharSTerm
//
EATEXT_API bool IsCharSTerm(Char c)
{
    // See "STerm" in PropList.txt in the Unicode database.
    // To consider: Remove some of the items below, for brevity.
    // To consider: Implement this as an "in-code nibble trie".

    if(c < 0x0500) // This branch is taken the large majority of the time.
        return ((c == 0x0021) || (c == 0x002E) || (c == 0x003F));

    if(c < 0xfe52)
    {
        if(c < 0x1362)
        {
            return ((c == 0x055C) || (c == 0x055E) || (c == 0x0589) || (c == 0x061F) ||
                    (c == 0x06D4) || (c == 0x0700) || (c == 0x0701) || (c == 0x0702) ||
                    (c == 0x0964) || (c == 0x0965) || (c == 0x104A) || (c == 0x104B));
        }

        return ((c == 0x1362) || (c == 0x1367) || (c == 0x1368) || (c == 0x166E) || (c == 0x1803) || 
                (c == 0x1809) || (c == 0x1944) || (c == 0x1945) || (c == 0x203C) || (c == 0x203D) ||
                (c == 0x2047) || (c == 0x2048) || (c == 0x2049) || (c == 0x3002));
    }
    
    return ((c == 0xFE52) || (c == 0xFE56) || (c == 0xFE57) || 
            (c == 0xFF01) || (c == 0xFF0E) || (c == 0xFF1F) || (c == 0xFF61));
}


///////////////////////////////////////////////////////////////////////////////
// IsCharHyphen
//
EATEXT_API bool IsCharHyphen(Char c)
{
    // This information taken from the Unicode database property list.
    // To consider: Support the distinction between breaking and non-breaking
    // hyphens and possibly soft hyphens.

    switch(c)
    {
        case 0x002d: // HYPHEN-MINUS
        case 0x00ad: // SOFT HYPHEN
        case 0x058a: // ARMENIAN HYPHEN
        case 0x1806: // MONGOLIAN TODO SOFT HYPHEN
        case 0x2010: // HYPHEN
        case 0x2011: // NON-BREAKING HYPHEN
        case 0x2e17: // DOUBLE OBLIQUE HYPHEN
        case 0x30fb: // KATAKANA MIDDLE DOT
        case 0xfe63: // SMALL HYPHEN-MINUS
        case 0xff0d: // FULLWIDTH HYPHEN-MINUS
        case 0xff65: // HALFWIDTH KATAKANA MIDDLE DOT
            return true;
    }

    return false;
}



///////////////////////////////////////////////////////////////////////////////
// IsGraphemeExtend
//
EATEXT_API bool IsGraphemeExtend(Char c)
{
    // kCPGraphemeExtend means (Me + Mn + Mc + Other_Grapheme_Extend - Grapheme_Link - CGJ)
    // We can probably ignore Other_Grapheme_Extend, but the others are significant if we 
    // want to support Devanagari (which we need to do).

    // We choose to ignore Other_Grapheme_Extend, but for future use, 
    // here is the list of them:
    // 09BE          ; Other_Grapheme_Extend # Mc       BENGALI VOWEL SIGN AA
    // 09D7          ; Other_Grapheme_Extend # Mc       BENGALI AU LENGTH MARK
    // 0B3E          ; Other_Grapheme_Extend # Mc       ORIYA VOWEL SIGN AA
    // 0B57          ; Other_Grapheme_Extend # Mc       ORIYA AU LENGTH MARK
    // 0BBE          ; Other_Grapheme_Extend # Mc       TAMIL VOWEL SIGN AA
    // 0BD7          ; Other_Grapheme_Extend # Mc       TAMIL AU LENGTH MARK
    // 0CC2          ; Other_Grapheme_Extend # Mc       KANNADA VOWEL SIGN UU
    // 0CD5..0CD6    ; Other_Grapheme_Extend # Mc   [2] KANNADA LENGTH MARK..KANNADA AI LENGTH MARK
    // 0D3E          ; Other_Grapheme_Extend # Mc       MALAYALAM VOWEL SIGN AA
    // 0D57          ; Other_Grapheme_Extend # Mc       MALAYALAM AU LENGTH MARK
    // 0DCF          ; Other_Grapheme_Extend # Mc       SINHALA VOWEL SIGN AELA-PILLA
    // 0DDF          ; Other_Grapheme_Extend # Mc       SINHALA VOWEL SIGN GAYANUKITTA
    // 200C..200D    ; Other_Grapheme_Extend # Cf   [2] ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    // 1D165         ; Other_Grapheme_Extend # Mc       MUSICAL SYMBOL COMBINING STEM
    // 1D16E..1D172  ; Other_Grapheme_Extend # Mc   [5] MUSICAL SYMBOL COMBINING FLAG-1..MUSICAL SYMBOL COMBINING FLAG-5

    const bool bIsExtender = IsCharCategory(c, kCCFlagNonSpacingMark | kCCFlagEnclosingMark | kCCFlagCombiningSpacingMark);

    if(bIsExtender)
    {
        // If not Grapheme_Link and not kCharCGJ (034F), then it is a bonafide extender.
        // Here is the list of all grapheme links. But we only chose to pay attention
        // to a couple of them right now, in order to reduce our code size.
        // 034F          ; Grapheme_Link # Mn       COMBINING GRAPHEME JOINER
        // 094D          ; Grapheme_Link # Mn       DEVANAGARI SIGN VIRAMA
        // 09CD          ; Grapheme_Link # Mn       BENGALI SIGN VIRAMA
        // 0A4D          ; Grapheme_Link # Mn       GURMUKHI SIGN VIRAMA
        // 0ACD          ; Grapheme_Link # Mn       GUJARATI SIGN VIRAMA
        // 0B4D          ; Grapheme_Link # Mn       ORIYA SIGN VIRAMA
        // 0BCD          ; Grapheme_Link # Mn       TAMIL SIGN VIRAMA
        // 0C4D          ; Grapheme_Link # Mn       TELUGU SIGN VIRAMA
        // 0CCD          ; Grapheme_Link # Mn       KANNADA SIGN VIRAMA
        // 0D4D          ; Grapheme_Link # Mn       MALAYALAM SIGN VIRAMA
        // 0DCA          ; Grapheme_Link # Mn       SINHALA SIGN AL-LAKUNA
        // 0E3A          ; Grapheme_Link # Mn       THAI CHARACTER PHINTHU
        // 1039          ; Grapheme_Link # Mn       MYANMAR SIGN VIRAMA
        // 17D2          ; Grapheme_Link # Mn       KHMER SIGN COENG
        // A806          ; Grapheme_Link # Mn       SYLOTI NAGRI SIGN HASANTA
        // 10A3F         ; Grapheme_Link # Mn       KHAROSHTHI VIRAMA

        if((c != 0x094D) && (c != 0x0E3A) && (c != 0x034F))
            return true;
    }

    return false;
}



///////////////////////////////////////////////////////////////////////////////
// GetCharBlock
//
EATEXT_API Block GetCharBlock(Char c)
{
    if(c < 0x0080)
        return kBlockBasicLatin;

    // We do a classic binary search of the RLE array.
    // Todo: Convert this to a hash table or some other kind of fast structure.
    int nLower = 0; // This must be a signed type.
    int nUpper = (int)gnBlockTableEntryArrayCount - 1;

    while(nLower <= nUpper)
    {
        const int nMid = (nLower + nUpper) / 2;

        if(c < gBlockTableEntryArray[nMid].mnBegin)
            nUpper = nMid - 1;
        else if(c >= (gBlockTableEntryArray[nMid].mnBegin + gBlockTableEntryArray[nMid].mnCount))
            nLower = nMid + 1;
        else
            return (Block)gBlockTableEntryArray[nMid].mnBlock;
    }

    return kBlockNone;
}



///////////////////////////////////////////////////////////////////////////////
// CharCategoryLess
//
struct CharCategoryLess
{
    inline bool operator()(const CharCategoryInfo& a, const CharCategoryInfo& b) const
        { return a.mCode < b.mCode; }
};


///////////////////////////////////////////////////////////////////////////////
// GetCharCategory
//
EATEXT_API CharCategory GetCharCategory(Char nChar)
{
    const size_t           kCCTableSize = sizeof(gCharCategoryTable) / sizeof(gCharCategoryTable[0]);
    const CharCategoryInfo cciTemp      = { nChar, 0, 0, 0 }; // We make this so we can use the generic upper_bound function.

    const CharCategoryInfo* const pCC = (EA::Text::upper_bound(gCharCategoryTable, gCharCategoryTable + kCCTableSize, cciTemp, CharCategoryLess()) - 1);
    EA_ASSERT((pCC >= gCharCategoryTable) && (pCC < gCharCategoryTable + kCCTableSize));

    if(pCC->mpBitMask == NULL) 
        return (CharCategory)pCC->mCharClass0;

    const int nBitIndex = nChar - pCC->mCode;
    return (CharCategory)((pCC->mpBitMask[nBitIndex >> 3] & (1 << (nBitIndex & 7))) ? pCC->mCharClass1 : pCC->mCharClass0);
}


///////////////////////////////////////////////////////////////////////////////
// GetHangulSyllableType
//
EATEXT_API HangulSyllableType GetHangulSyllableType(Char c)
{
    //const int kJamoLCount  = 19;
    //const int kJamoVCount  = 21;
      const int kJamoTCount  = 28;
    //const int kHangulCount = kJamoLCount * kJamoVCount * kJamoTCount; // kHangulCount == 11172

    if(c >= 0x1100) // 0x1100 is the first Jamo char in Unicode.
    {
        if(c < 0x1200) // 0x1200 is the end of the Jamo range in Unicode.
        {
            if(c < 0x1160) // 0x1160 is the end of the Jamo consonants (choseong).
                return kHSTLeadingJamo; // Strictly speaking, 0x115a - 0x115e are "reserved" and we might want to discount them.
            else if(c < 0x11a8) // 0x11a8 is the last of the Jamo medial vowels (jungseong). 
                return kHSTVowelJamo;
            else // Else we are dealing with final consonants (jongseong).
                return kHSTTrailingJamo;
        }
        else if((c - 0xac00u) < (0xd7a4u - 0xac00u)) // 0xac00 is the base of all Hangul. 0xd7a4 is the end of Hangul.
            return (((c - 0xac00) % kJamoTCount) == 0) ? kHSTLVSyllable : kHSTLVTSyllable;
    }

    return kHSTNone;
}



///////////////////////////////////////////////////////////////////////////////
// GetLineBreakCategory
//
EATEXT_API LineBreakCategory GetLineBreakCategory(Char c)
{
    // The large majority of the time for Western languages this function will 
    // simply return with the initial table lookup directly below. 
    // For other languages there will be other table lookups or some if/else code.
    // Unfortunately, the line break characteristics of characters are rather
    // disjoint in the Unicode Standard and there is no elegant way to encode 
    // them aside from possibly a single very large (64 Kb) lookup table.
    // The implementation we have here works with a much smaller amount of memory
    // but is not going to be as fast.

    // First 256 characters (mostly Latin). 
    if(c < 0x0100u)
        return (LineBreakCategory)gLineBreakCategoryTable256[c];

    // Hiragana, Katakana
    #if (defined(EATEXT_HIRAGANA_SCRIPT_SUPPORTED) && EATEXT_HIRAGANA_SCRIPT_SUPPORTED) || \
        (defined(EATEXT_KATAKANA_SCRIPT_SUPPORTED) && EATEXT_KATAKANA_SCRIPT_SUPPORTED)

        if((c - 0x3000u) < (0x3100u - 0x3000u)) // Hiragana, Katakana
            return (LineBreakCategory)gLineBreakCategoryTableHK[c - 0x3000u];

        if((c - 0xFF00u) <= (0xFFFFu - 0xFF00u)) // Halfwidth and Fullwidth forms
            return (LineBreakCategory)gLineBreakCategoryTableHF[c - 0xFF00u];
    #endif

    // Han ideographs and Yi syllables
    #if defined(EATEXT_HAN_SCRIPT_SUPPORTED) && EATEXT_HAN_SCRIPT_SUPPORTED
        if((c - 0x4E00u) < (0xA700u - 0x4E00u)) // Actually, 0xA015 is a non-spacing character, but we ignore it because it is rare.
            return kLBCIdeographic;
    #endif

    // Combining Diacritical Marks
    if((c - 0x0300u) < (0x0374u - 0x0300u))
        return kLBCCombiningMark;

    // General Punctuation
    if((c - 0x2000u) < (0x2062u - 0x2000u))
        return (LineBreakCategory)gLineBreakCategoryTableGP[c - 0x2000u];

    #if defined(EATEXT_HANGUL_SCRIPT_SUPPORTED) && EATEXT_HANGUL_SCRIPT_SUPPORTED
        if((c - 0xAC00u) < (0xD7A4u - 0xAC00u))
        {
            if((c - 0xAC00u) % 0x1C)
                return kLBCHangulJamo3;
            return kLBCHangulJamo2;
        }

        if((c - 0x1100u) < (0x1200u - 0x1100u))
        {
            if((c - 0x1100u) < (0x1160u - 0x1100u))
                return kLBCHangulJamoLeading;

            if((c - 0x1160u) < (0x11A8u - 0x1160u))
                return kLBCHangulJamoVowel;

            //else we have (c - 0x11A8u) < (0x1160u - 0x11A8u))
            return kLBCHangulJamoTrailing;
        }

        if((c - 0x302Eu) < (0x3030u - 0x302Eu))
            return kLBCCombiningMark;

        // These are uncommon enough that we might want to ignore them.
        if((c - 0x3131u) < (0x318Fu - 0x3131u))
            return kLBCIdeographic;
    #endif

    // Greek and Coptic, Cyrillic, Armenian
    #if defined(EATEXT_GREEK_SCRIPT_SUPPORTED) && EATEXT_GREEK_SCRIPT_SUPPORTED
        if((c - 0x0374u) < (0x0590u - 0x0374u)) 
        {
            // To consider: Replace this with a lookup table.

            if((c - 0x0374u) < (0x0483u - 0x0374u)) // Greek and Coptic, Cyrillic
            {
                if(c == 0x037E)
                    return kLBCInfixNumeric;
                return kLBCAlphabetic;
            }

            if((c - 0x0483u) < (0x048Au - 0x0483u)) // Cyrillic combining marks
                return kLBCCombiningMark;

            if((c - 0x048Au) < (0x0590u - 0x048Au)) // Cyrillic, Armenian
                return kLBCAlphabetic;
        }
    #endif

    // Hebrew
    #if defined(EATEXT_HEBREW_SCRIPT_SUPPORTED) && EATEXT_HEBREW_SCRIPT_SUPPORTED
        if((c - 0x0590u) < (0x0600u - 0x0590u))
        {
            if((c - 0x0591u) < (0x05D0u - 0x0591u)) // Hebrew combining marks
                return kLBCCombiningMark;

            if((c - 0x05D0u) < (0x064Bu - 0x05D0u)) // Hebrew and Arabic (bug: there are combining marks in here)
                return kLBCAlphabetic;
        }
    #endif

    // Arabic and Syriac
    #if defined(EATEXT_ARABIC_SCRIPT_SUPPORTED) && EATEXT_ARABIC_SCRIPT_SUPPORTED
        if((c - 0x0600u) < (0x0730u - 0x0600u))
        {
            // To consider: Replace this with a lookup table.

            if((c - 0x064Bu) < (0x0660u - 0x064Bu)) // Arabic combining marks
                return kLBCCombiningMark;

            if((c - 0x0660u) < (0x066Du - 0x0660u)) // Arabic numerals
                return kLBCNumeric;

            if((c - 0x066Du) < (0x06D4u - 0x066Du)) // Arabic
                return kLBCAlphabetic;

            if((c - 0x06D4u) < (0x06D5u - 0x06D4u)) // Arabic exclamation
                return kLBCExclamation;

            if((c - 0x06D5u) < (0x06FAu - 0x06D5u)) // Arabic combining marks
                return kLBCCombiningMark;

            if((c - 0x06FAu) < (0x0730u - 0x06FAu)) // Arabic and Syriac
                return kLBCAlphabetic;
        }
    #endif

    // Thai (which you really need a dictionary to do proper line-breaking for)
    #if defined(EATEXT_THAI_SCRIPT_SUPPORTED) && EATEXT_THAI_SCRIPT_SUPPORTED
        if((c - 0x0E00u) < (0x0E5Cu - 0x0E00u))
            return (LineBreakCategory)gLineBreakCategoryTableThai[c - 0x0E00u];
    #endif

    #if defined(EATEXT_DEVANAGARI_SCRIPT_SUPPORTED) && EATEXT_DEVANAGARI_SCRIPT_SUPPORTED
        if((c - 0x0900u) < (0x097eu - 0x0900u))
            return (LineBreakCategory)gLineBreakCategoryTableDevanagari[c - 0x0900u];
    #endif

    // The following is disabled because it is the same thing as the default return value.
    // Latin Extended A, Latin Extended B, IPA Extensions, Spacing Modifier Letters.
    //if((c - 0x0100u) < (0x0300u - 0x0100u))
    //    return kLBCAlphabetic;

    return kLBCAlphabetic;
}


///////////////////////////////////////////////////////////////////////////////
// GetDirection
//
EATEXT_API int GetDirection(const Char* pText, uint32_t nTextLength)
{
    // To do: Implement Unicode TR9, as soon as we have the table set up to 
    //        read this information.
    // TR9: 
    //     P2. In each paragraph, find the first character of type L, AL, or R.
    //         Because paragraph separators delimit text in this algorithm, 
    //         this will generally be the first strong character after a 
    //         paragraph separator or at the very beginning of the text. 
    //         Note that the characters of type LRE, LRO, RLE, RLO are ignored 
    //         in this rule. This is because typically they are used to indicate 
    //         that the embedded text is the opposite direction than the 
    //         paragraph level.
    //     P3. If a character is found in P2 and it is of type AL or R, then 
    //         set the paragraph embedding level to one; otherwise, set it to zero.

    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4062) // enumerate 'kBidiClassON' in switch of enum 'EA::Text::BidiClass' is not handled
    #endif

    for(const Char* pTextEnd = pText + nTextLength; pText != pTextEnd; ++pText)
    {
        const BidiClass bidiClass = GetBidiClass(*pText); // To do: Implement this

        switch(bidiClass)
        {
            case kBidiClassL:
                return kDirectionLTR;

            case kBidiClassR:
            case kBidiClassAL:
                return kDirectionRTL;

            case kBidiClassLRE:
            case kBidiClassLRO:
            case kBidiClassRLE:
            case kBidiClassRLO:
            case kBidiClassPDF:
            case kBidiClassEN:
            case kBidiClassES:
            case kBidiClassET:
            case kBidiClassAN:
            case kBidiClassCS:
            case kBidiClassNSM:
            case kBidiClassBN:
            case kBidiClassB:
            case kBidiClassS:
            case kBidiClassWS:
            case kBidiClassON:
                continue;
        }
    }

    return kDirectionNeutral;
}


///////////////////////////////////////////////////////////////////////////////
// GetCombiningClass
//
// We use the DerivedCombiningClass.txt file to implement this functionality.
//
EATEXT_API int32_t GetCombiningClass(Char c)
{
    // There are no combining marks below 0x0300, and there are no combining
    // marks of interest to us above 0x3100.

    if((c >= 0x0300) && (c < 0x3100))
    {
        // We do a classic binary search of the RLE array.
        int nLower = 0; // This must be a signed type.
        int nUpper = (int)(sizeof(gCombiningClassEntryArray) / sizeof(gCombiningClassEntryArray[0])) - 1;

        while(nLower <= nUpper)
        {
            const int nMid = (nLower + nUpper) / 2;

            if(c < gCombiningClassEntryArray[nMid].mnBegin)
                nUpper = nMid - 1;
            else if(c >= (gCombiningClassEntryArray[nMid].mnBegin + gCombiningClassEntryArray[nMid].mnCount))
                nLower = nMid + 1;
            else
                return gCombiningClassEntryArray[nMid].mnClass;
        }
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetDecompositionType
//
// See http://icu-project.org/docs/papers/optimized_unicode_composition_and_decomposition.html
//
EATEXT_API DecompositionType GetDecompositionType(Char /*c*/)
{
    // Dummy implementation for now.
    // Notify the maintainer of this package when you need an implementation done.
    return kDTNone;
}


///////////////////////////////////////////////////////////////////////////////
// ASCIICompatibleEncoding
//
// See http://icu-project.org/docs/papers/optimized_unicode_composition_and_decomposition.html
//
EATEXT_API uint32_t ASCIICompatibleEncoding(const Char* pTextInput, uint32_t nTextLength, char8_t* pTextOutput, uint32_t nTextOutputCapacity)
{
    // Dummy implementation for now.
    // Notify the maintainer of this package when you need an implementation done.
    EA_ASSERT(pTextInput != NULL);
    uint32_t i, j;

    for(i = 0, j = 0; i != nTextLength; i++)
    {
        if((i + 1) < nTextOutputCapacity)
            pTextOutput[j++] = (char8_t)(uint8_t)pTextInput[i]; // This is completely wrong.
    }

    if(j < nTextOutputCapacity)
        pTextOutput[j] = 0;

    return i;
}




} // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


