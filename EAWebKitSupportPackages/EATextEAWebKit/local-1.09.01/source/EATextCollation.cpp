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
// EATextCollation.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextCollation.h>
#include <EAText/internal/EATextScriptHangul.h>
#include <EASTL/bonus/sort_extra.h>
#include <EASTL/fixed_string.h>
#include <EAText/internal/StdC.h>
#include "internal/CollationData.inl"
#include EA_ASSERT_HEADER

    #include <Windows.h>



///////////////////////////////////////////////////////////////////////////////
// EATEXT_OS_COLLATION_ENABLED
// 
// Defined as 0 or 1. 1 is default.
// If enabled then any OS-provided collation functionality is used instead of
// our collation functionality. The OS collation functionality is likely to 
// be more correct than ours, since such functionality is usually from a major
// OS like Windows or Unix. However, you might want to disable it if you want
// it to act the same across all platforms.
#ifndef EATEXT_OS_COLLATION_ENABLED
    #define EATEXT_OS_COLLATION_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_OS_COLLATION_AVAILABLE
// 
// Defined as 0 or 1. The value depends on whether the OS has its own collation
// functionality and if EATEXT_OS_COLLATION_ENABLED is enabled.
// This config option is not user-settable. It's informationational and is based
// off of other config options.
//
#if EATEXT_OS_COLLATION_ENABLED
        #define EATEXT_OS_COLLATION_AVAILABLE 1
#else
    #define EATEXT_OS_COLLATION_AVAILABLE 0
#endif




namespace EA
{

namespace Text
{

typedef eastl::fixed_string<Char, 128, true> FixedBufferString;


struct SortByCombiningClass // : public eastl::binary_function<Char, Char, bool>
{
    bool operator()(Char a, Char b) const {
        return (GetCombiningClass(a) < GetCombiningClass(b));
    }
};



    // This table is borrowed from the EALocale package.
    const char8_t* sOSLocaleTable[] = 
    {
        // "ISO^Win32LCID
        "en-us\0" "0400",      // Process or User Default Language     
        "sq-al\0" "041c",      // Albanian     
        "ar-sa\0" "0401",      // Arabic (Saudi Arabia)     
        "zh-tw\0" "0404",      // Chinese (Taiwan)     
        "zh-cn\0" "0804",      // Chinese (PRC)     
        "zh-hk\0" "0c04",      // Chinese (Hong Kong SAR, PRC)     
        "zh-sg\0" "1004",      // Chinese (Singapore)     
        "cs-cz\0" "0405",      // Czech     
        "da-dk\0" "0406",      // Danish     
        "nl-nl\0" "0413",      // Dutch (Netherlands)     
        "nl-be\0" "0813",      // Dutch (Belgium)     
        "en-us\0" "0409",      // English United States    
        "en-gb\0" "0809",      // English Great Britain (UK)    
        "fi-fi\0" "040b",      // Finnish     
        "fr-fr\0" "040c",      // French (Standard)     
        "fr-be\0" "080c",      // French (Belgian)     
        "fr-ca\0" "0c0c",      // French (Canadian)     
        "de-de\0" "0407",      // German (Standard)     
        "de-ch\0" "0807",      // German (Switzerland)     
        "el-gr\0" "0408",      // Greek     
        "iw-il\0" "040d",      // Hebrew     
        "hi-in\0" "0439",      // Hindi   
        "hu-hu\0" "040e",      // Hungarian     
        "is-is\0" "040f",      // Icelandic     
        "it-it\0" "0410",      // Italian (Standard)     
        "it-ch\0" "0810",      // Italian (Switzerland)     
        "ja-jp\0" "0411",      // Japanese     
        "ko-kp\0" "0412",      // Korean (North)    
        "ko-kr\0" "0412",      // Korean (South)    
        "no-no\0" "0414",      // Norwegian (Bokmal)     
        "pl-pl\0" "0415",      // Polish     
        "pt-br\0" "0416",      // Portuguese (Brazil)     
        "pt-pt\0" "0816",      // Portuguese (Portugal)     
        "ro-ro\0" "0418",      // Romanian     
        "ru-ru\0" "0419",      // Russian     
        "sk-sk\0" "041b",      // Slovak     
        "es-es\0" "040a",      // Spanish (Spain, Traditional Sort)     
        "es-mx\0" "080a",      // Spanish (Mexican)     
        "es-es\0" "0c0a",      // Spanish (Spain, Modern Sort)     
        "sv-se\0" "041d",      // Swedish     
        "sv-fi\0" "081d",      // Swedish (Finland)
        "th-th\0" "041e",      // Thai     
        "tr-tr\0" "041f"       // Turkish     
    };



///////////////////////////////////////////////////////////////////////////////
// GetCanonicalDecomposition
//
static const Char* GetCanonicalDecomposition(Char& c, uint32_t& count)
{
    // There are no decomposable chars below 0x00C0, and there are none
    // of interest to us above 0x30FE.

    if((c >= 0x00C0) && (c <= 0x30FE))
    {
        // To consider: Replace this with eastl::lower_bound.

        int nLower = 0;
        int nUpper = (int)(sizeof(gNFDEntryArray) / sizeof(gNFDEntryArray[0]));

        while(nLower <= nUpper)
        {
            const int nMid = (nLower + nUpper) / 2;

            if(c == gNFDEntryArray[nMid].mChar)
            {
                count = 2;
                return &gNFDEntryArray[nMid].mD0; // We should change mD0 to be an array of size 2 instead of being two successive chars.
            }
            else if(c < gNFDEntryArray[nMid].mChar)
                nUpper = nMid - 1;
            else
                nLower = nMid + 1;
        }
    }

    count = 1;
    return &c;
}


///////////////////////////////////////////////////////////////////////////////
// IsNormalized
//
EATEXT_API bool IsNormalized(const Char* pText, uint32_t nTextLength, NormalizationType nt, int* pNormalizationResultFlags)
{
    // There is a potentially faster way to go about this, which is to use the "quick check" 
    // approach described in UAX #15 and via the DerivedNormalizationProps.txt file published
    // with the Unicode Standard data. 

    int flags;

    Normalize(pText, nTextLength, NULL, 0, nt, &flags);

    if(pNormalizationResultFlags)
        *pNormalizationResultFlags = flags;

    return (flags & kNRFUnchanged) != 0;
}


///////////////////////////////////////////////////////////////////////////////
// Normalize
//
// Implement no normalization form. Simply copy the text and return the flags
// associated with it. This function is essentially a fallback that has little
// practical purpose beyond giving the caller the resulting flag information.
//
static uint32_t NormalizeNone(const Char* pTextInput,  uint32_t nTextInputLength,
                              Char* pTextOutput, uint32_t nTextOutputCapacity, 
                              NormalizationType /*nt*/, int* pNormalizationResultFlags)
{
    int flags = (kNRFAscii | kNRFUnchanged);

    for(uint32_t i = 0; i < nTextInputLength; ++i, ++pTextInput)
    {
        if(*pTextInput >= 0x0080)
            flags &= ~kNRFAscii;

        if(i < nTextOutputCapacity)
            *pTextOutput++ = *pTextInput;
    }

    if(pNormalizationResultFlags)
       *pNormalizationResultFlags = flags;

    return nTextInputLength;
}


///////////////////////////////////////////////////////////////////////////////
// NormalizeD
//
// Implement normalization NFD and NFKD (decomposition forms).
//
// As with much of the rest of this Unicode library, we don't support the 
// entire Unicode character space, though we support the large majority that
// we are going to be interested in. There are a large number of characters
// in the Unicode Standard which have non-trivial decompositions but also 
// are unlikely to be seen by our users. Instead of making a huge table of
// decomposition data, we use the Expands_On_NFD section of DerivedNormalizationProps.txt
// to decide what to decompose and write manual decompositions.
//
// If you need a more full treatment of the Unicode standard, please contact
// the maintainer of this package.
//
static uint32_t NormalizeD(const Char* pTextInput,  uint32_t nTextInputLength,
                           Char* pTextOutput, uint32_t nTextOutputCapacity, 
                           NormalizationType nt, int* pNormalizationResultFlags)
{
    uint32_t nOutputLength = 0;
    int      flags = (kNRFAscii | kNRFUnchanged); // The text is ASCII and denormalized until proven otherwise.

    for(uint32_t i = 0; i < nTextInputLength; ++i, ++pTextInput)
    {
        Char c = *pTextInput;

        if(c >= 0x0080)
            flags &= ~kNRFAscii;

        // Hangul (Korean) decomposition. With Hangul, NFD and NFKD are the same,
        // so both normalization forms can share the same code here.
        if((c - 0xAC00u) <= (0xD7A4u - 0xAC00u)) // If c is composed Hangul...
        {
            c -= kHangulBegin;

            const Char cL = kJamoLBase + (c / kJamoNCount);
            const Char cV = kJamoVBase + (c % kJamoNCount) / kJamoTCount;
            const Char cT = kJamoTBase + (c % kJamoTCount);

            if(nOutputLength++ < nTextOutputCapacity)
                *pTextOutput++ = cL;

            if(nOutputLength++ < nTextOutputCapacity)
                *pTextOutput++ = cV;

            if(cT != kJamoTBase)
            {
                if(nOutputLength++ < nTextOutputCapacity)
                    *pTextOutput++ = cT;
            }
        }
        else
        {
            // We need to recursively apply character decomposition. By recursively, we mean that
            // char c may decompose into char c1 and c2, but either of c1 or c2 may itself decompose.
            // In practice such recursive decomposition is rarely required.

            typedef eastl::fixed_string<Char, 16> DString;

            DString      ds(&c, 1);
            uint32_t     count;
            eastl_size_t j = 0;
            eastl_size_t jEnd;

            EA_UNUSED(nt);             // Avoid unused var warning.
            EA_ASSERT(nt != kNT_NFKD); // We don't yet have support for compatibility decomposition.

            while(j < ds.size()) // Size can change so we re-evaluate it every loop iteration.
            {
                const Char* const p = GetCanonicalDecomposition(ds[j], count);

                if(count > 1) // If there is a decomposition...
                    ds.replace(j, 1, p, count);
                else
                    ++j;
            }

            // We need to sort the ds entries by combining class.
            if(ds.size() > 1)
                eastl::bubble_sort(ds.begin(), ds.end(), SortByCombiningClass());

            for(j = 0, jEnd = ds.size(); j < jEnd; j++)
            {
                if(nOutputLength++ < nTextOutputCapacity)
                    *pTextOutput++ = ds[j];
            }
        }
    }

    if(pNormalizationResultFlags)
       *pNormalizationResultFlags = flags;

    return nOutputLength;
}


///////////////////////////////////////////////////////////////////////////////
// NormalizeC
//
// Implement normalization NFC and NFKC (composition forms).
//
// As with much of the rest of this Unicode library, we don't support the 
// entire Unicode character space, though we support the large majority that
// we are going to be interested in. There are a large number of characters
// in the Unicode Standard which have non-trivial compositions but also 
// are unlikely to be seen by our users. 
//
// If you need a more full treatment of the Unicode standard, please contact
// the maintainer of this package.
//
static uint32_t NormalizeC(const Char* pTextInput,  uint32_t nTextInputLength,
                           Char* pTextOutput, uint32_t nTextOutputCapacity, 
                           NormalizationType nt, int* pNormalizationResultFlags)
{
    // To do: Implement this. It currently just does a no-op copy.

    return NormalizeNone(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags);
}


///////////////////////////////////////////////////////////////////////////////
// Normalize
//
// This is the main entrypoint to Normalize.
//
EATEXT_API uint32_t Normalize(const Char* pTextInput,  uint32_t nTextInputLength,
                              Char* pTextOutput, uint32_t nTextOutputCapacity, 
                              NormalizationType nt, int* pNormalizationResultFlags)
{
    if(nTextInputLength == 0xffffffff)
        nTextInputLength = (uint32_t)Strlen(pTextInput);

    if((nt == kNT_NFD) || (nt == kNT_NFKD)) // Decomposition forms
        return NormalizeD(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags);

    if((nt == kNT_NFC) || (nt == kNT_NFKC)) // Composition forms
        return NormalizeC(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags);

    return NormalizeNone(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags);
}


/*
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <stdio.h>

static void GenerateCaseTableData()
{
    EA::IO::FileStream file(EA_CHAR16("C:\\temp\\UnicodeCase.txt"));
    file.Open();

    char buffer[512];
    EA::IO::size_type size;

    while((size = EA::IO::ReadLine(&file, buffer, 512)) < EA::IO::kSizeTypeDone) // While there there were more lines...
    {
        eastl::fixed_string<char, 512> s(buffer, size);
        eastl_size_t pos = 0;
        unsigned n;

        //const int kLowerCase = 12;
        const int kUpperCase = 13;
        //const int kTitleCase = 14;

        for(int j = 0; j < kUpperCase; j++)
        {
            pos = s.find(';', pos + 1);
            EA_ASSERT(pos < s.length());
        }

        int wasRead = sscanf(s.c_str() + pos + 1, "%X", &n);
        if(!wasRead)
            n = 0;
        s.sprintf("    0x%04x,\n", n);
        OutputDebugString(s.c_str());
    }

    file.Close();
}
*/

///////////////////////////////////////////////////////////////////////////////
// UpperCaseTable
//
const Char gUpperCaseTable[0x0590] = 
{
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0041,
    0x0042,
    0x0043,
    0x0044,
    0x0045,
    0x0046,
    0x0047,
    0x0048,
    0x0049,
    0x004a,
    0x004b,
    0x004c,
    0x004d,
    0x004e,
    0x004f,
    0x0050,
    0x0051,
    0x0052,
    0x0053,
    0x0054,
    0x0055,
    0x0056,
    0x0057,
    0x0058,
    0x0059,
    0x005a,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x039c,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x00c0,
    0x00c1,
    0x00c2,
    0x00c3,
    0x00c4,
    0x00c5,
    0x00c6,
    0x00c7,
    0x00c8,
    0x00c9,
    0x00ca,
    0x00cb,
    0x00cc,
    0x00cd,
    0x00ce,
    0x00cf,
    0x00d0,
    0x00d1,
    0x00d2,
    0x00d3,
    0x00d4,
    0x00d5,
    0x00d6,
    0x0000,
    0x00d8,
    0x00d9,
    0x00da,
    0x00db,
    0x00dc,
    0x00dd,
    0x00de,
    0x0178,
    0x0000,
    0x0100,
    0x0000,
    0x0102,
    0x0000,
    0x0104,
    0x0000,
    0x0106,
    0x0000,
    0x0108,
    0x0000,
    0x010a,
    0x0000,
    0x010c,
    0x0000,
    0x010e,
    0x0000,
    0x0110,
    0x0000,
    0x0112,
    0x0000,
    0x0114,
    0x0000,
    0x0116,
    0x0000,
    0x0118,
    0x0000,
    0x011a,
    0x0000,
    0x011c,
    0x0000,
    0x011e,
    0x0000,
    0x0120,
    0x0000,
    0x0122,
    0x0000,
    0x0124,
    0x0000,
    0x0126,
    0x0000,
    0x0128,
    0x0000,
    0x012a,
    0x0000,
    0x012c,
    0x0000,
    0x012e,
    0x0000,
    0x0049,
    0x0000,
    0x0132,
    0x0000,
    0x0134,
    0x0000,
    0x0136,
    0x0000,
    0x0000,
    0x0139,
    0x0000,
    0x013b,
    0x0000,
    0x013d,
    0x0000,
    0x013f,
    0x0000,
    0x0141,
    0x0000,
    0x0143,
    0x0000,
    0x0145,
    0x0000,
    0x0147,
    0x0000,
    0x0000,
    0x014a,
    0x0000,
    0x014c,
    0x0000,
    0x014e,
    0x0000,
    0x0150,
    0x0000,
    0x0152,
    0x0000,
    0x0154,
    0x0000,
    0x0156,
    0x0000,
    0x0158,
    0x0000,
    0x015a,
    0x0000,
    0x015c,
    0x0000,
    0x015e,
    0x0000,
    0x0160,
    0x0000,
    0x0162,
    0x0000,
    0x0164,
    0x0000,
    0x0166,
    0x0000,
    0x0168,
    0x0000,
    0x016a,
    0x0000,
    0x016c,
    0x0000,
    0x016e,
    0x0000,
    0x0170,
    0x0000,
    0x0172,
    0x0000,
    0x0174,
    0x0000,
    0x0176,
    0x0000,
    0x0000,
    0x0179,
    0x0000,
    0x017b,
    0x0000,
    0x017d,
    0x0053,
    0x0243,
    0x0000,
    0x0000,
    0x0182,
    0x0000,
    0x0184,
    0x0000,
    0x0000,
    0x0187,
    0x0000,
    0x0000,
    0x0000,
    0x018b,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0191,
    0x0000,
    0x0000,
    0x01f6,
    0x0000,
    0x0000,
    0x0000,
    0x0198,
    0x023d,
    0x0000,
    0x0000,
    0x0000,
    0x0220,
    0x0000,
    0x0000,
    0x01a0,
    0x0000,
    0x01a2,
    0x0000,
    0x01a4,
    0x0000,
    0x0000,
    0x01a7,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x01ac,
    0x0000,
    0x0000,
    0x01af,
    0x0000,
    0x0000,
    0x0000,
    0x01b3,
    0x0000,
    0x01b5,
    0x0000,
    0x0000,
    0x01b8,
    0x0000,
    0x0000,
    0x0000,
    0x01bc,
    0x0000,
    0x01f7,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x01c4,
    0x01c4,
    0x0000,
    0x01c7,
    0x01c7,
    0x0000,
    0x01ca,
    0x01ca,
    0x0000,
    0x01cd,
    0x0000,
    0x01cf,
    0x0000,
    0x01d1,
    0x0000,
    0x01d3,
    0x0000,
    0x01d5,
    0x0000,
    0x01d7,
    0x0000,
    0x01d9,
    0x0000,
    0x01db,
    0x018e,
    0x0000,
    0x01de,
    0x0000,
    0x01e0,
    0x0000,
    0x01e2,
    0x0000,
    0x01e4,
    0x0000,
    0x01e6,
    0x0000,
    0x01e8,
    0x0000,
    0x01ea,
    0x0000,
    0x01ec,
    0x0000,
    0x01ee,
    0x0000,
    0x0000,
    0x01f1,
    0x01f1,
    0x0000,
    0x01f4,
    0x0000,
    0x0000,
    0x0000,
    0x01f8,
    0x0000,
    0x01fa,
    0x0000,
    0x01fc,
    0x0000,
    0x01fe,
    0x0000,
    0x0200,
    0x0000,
    0x0202,
    0x0000,
    0x0204,
    0x0000,
    0x0206,
    0x0000,
    0x0208,
    0x0000,
    0x020a,
    0x0000,
    0x020c,
    0x0000,
    0x020e,
    0x0000,
    0x0210,
    0x0000,
    0x0212,
    0x0000,
    0x0214,
    0x0000,
    0x0216,
    0x0000,
    0x0218,
    0x0000,
    0x021a,
    0x0000,
    0x021c,
    0x0000,
    0x021e,
    0x0000,
    0x0000,
    0x0000,
    0x0222,
    0x0000,
    0x0224,
    0x0000,
    0x0226,
    0x0000,
    0x0228,
    0x0000,
    0x022a,
    0x0000,
    0x022c,
    0x0000,
    0x022e,
    0x0000,
    0x0230,
    0x0000,
    0x0232,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x023b,
    0x0000,
    0x0000,
    0x2c7e,
    0x2c7f,
    0x0000,
    0x0241,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0246,
    0x0000,
    0x0248,
    0x0000,
    0x024a,
    0x0000,
    0x024c,
    0x0000,
    0x024e,
    0x2c6f,
    0x2c6d,
    0x2c70,
    0x0181,
    0x0186,
    0x0000,
    0x0189,
    0x018a,
    0x0000,
    0x018f,
    0x0000,
    0x0190,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0193,
    0x0000,
    0x0000,
    0x0194,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0197,
    0x0196,
    0x0000,
    0x2c62,
    0x0000,
    0x0000,
    0x0000,
    0x019c,
    0x0000,
    0x2c6e,
    0x019d,
    0x0000,
    0x0000,
    0x019f,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x2c64,
    0x0000,
    0x0000,
    0x01a6,
    0x0000,
    0x0000,
    0x01a9,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x01ae,
    0x0244,
    0x01b1,
    0x01b2,
    0x0245,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x01b7,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0399,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0370,
    0x0000,
    0x0372,
    0x0000,
    0x0000,
    0x0000,
    0x0376,
    0x0000,
    0x03fd,
    0x03fe,
    0x03ff,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0386,
    0x0388,
    0x0389,
    0x038a,
    0x0000,
    0x0391,
    0x0392,
    0x0393,
    0x0394,
    0x0395,
    0x0396,
    0x0397,
    0x0398,
    0x0399,
    0x039a,
    0x039b,
    0x039c,
    0x039d,
    0x039e,
    0x039f,
    0x03a0,
    0x03a1,
    0x03a3,
    0x03a3,
    0x03a4,
    0x03a5,
    0x03a6,
    0x03a7,
    0x03a8,
    0x03a9,
    0x03aa,
    0x03ab,
    0x038c,
    0x038e,
    0x038f,
    0x0000,
    0x0392,
    0x0398,
    0x0000,
    0x0000,
    0x0000,
    0x03a6,
    0x03a0,
    0x03cf,
    0x0000,
    0x03d8,
    0x0000,
    0x03da,
    0x0000,
    0x03dc,
    0x0000,
    0x03de,
    0x0000,
    0x03e0,
    0x0000,
    0x03e2,
    0x0000,
    0x03e4,
    0x0000,
    0x03e6,
    0x0000,
    0x03e8,
    0x0000,
    0x03ea,
    0x0000,
    0x03ec,
    0x0000,
    0x03ee,
    0x039a,
    0x03a1,
    0x03f9,
    0x0000,
    0x0000,
    0x0395,
    0x0000,
    0x0000,
    0x03f7,
    0x0000,
    0x0000,
    0x03fa,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0410,
    0x0411,
    0x0412,
    0x0413,
    0x0414,
    0x0415,
    0x0416,
    0x0417,
    0x0418,
    0x0419,
    0x041a,
    0x041b,
    0x041c,
    0x041d,
    0x041e,
    0x041f,
    0x0420,
    0x0421,
    0x0422,
    0x0423,
    0x0424,
    0x0425,
    0x0426,
    0x0427,
    0x0428,
    0x0429,
    0x042a,
    0x042b,
    0x042c,
    0x042d,
    0x042e,
    0x042f,
    0x0400,
    0x0401,
    0x0402,
    0x0403,
    0x0404,
    0x0405,
    0x0406,
    0x0407,
    0x0408,
    0x0409,
    0x040a,
    0x040b,
    0x040c,
    0x040d,
    0x040e,
    0x040f,
    0x0000,
    0x0460,
    0x0000,
    0x0462,
    0x0000,
    0x0464,
    0x0000,
    0x0466,
    0x0000,
    0x0468,
    0x0000,
    0x046a,
    0x0000,
    0x046c,
    0x0000,
    0x046e,
    0x0000,
    0x0470,
    0x0000,
    0x0472,
    0x0000,
    0x0474,
    0x0000,
    0x0476,
    0x0000,
    0x0478,
    0x0000,
    0x047a,
    0x0000,
    0x047c,
    0x0000,
    0x047e,
    0x0000,
    0x0480,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x048a,
    0x0000,
    0x048c,
    0x0000,
    0x048e,
    0x0000,
    0x0490,
    0x0000,
    0x0492,
    0x0000,
    0x0494,
    0x0000,
    0x0496,
    0x0000,
    0x0498,
    0x0000,
    0x049a,
    0x0000,
    0x049c,
    0x0000,
    0x049e,
    0x0000,
    0x04a0,
    0x0000,
    0x04a2,
    0x0000,
    0x04a4,
    0x0000,
    0x04a6,
    0x0000,
    0x04a8,
    0x0000,
    0x04aa,
    0x0000,
    0x04ac,
    0x0000,
    0x04ae,
    0x0000,
    0x04b0,
    0x0000,
    0x04b2,
    0x0000,
    0x04b4,
    0x0000,
    0x04b6,
    0x0000,
    0x04b8,
    0x0000,
    0x04ba,
    0x0000,
    0x04bc,
    0x0000,
    0x04be,
    0x0000,
    0x0000,
    0x04c1,
    0x0000,
    0x04c3,
    0x0000,
    0x04c5,
    0x0000,
    0x04c7,
    0x0000,
    0x04c9,
    0x0000,
    0x04cb,
    0x0000,
    0x04cd,
    0x04c0,
    0x0000,
    0x04d0,
    0x0000,
    0x04d2,
    0x0000,
    0x04d4,
    0x0000,
    0x04d6,
    0x0000,
    0x04d8,
    0x0000,
    0x04da,
    0x0000,
    0x04dc,
    0x0000,
    0x04de,
    0x0000,
    0x04e0,
    0x0000,
    0x04e2,
    0x0000,
    0x04e4,
    0x0000,
    0x04e6,
    0x0000,
    0x04e8,
    0x0000,
    0x04ea,
    0x0000,
    0x04ec,
    0x0000,
    0x04ee,
    0x0000,
    0x04f0,
    0x0000,
    0x04f2,
    0x0000,
    0x04f4,
    0x0000,
    0x04f6,
    0x0000,
    0x04f8,
    0x0000,
    0x04fa,
    0x0000,
    0x04fc,
    0x0000,
    0x04fe,
    0x0000,
    0x0500,
    0x0000,
    0x0502,
    0x0000,
    0x0504,
    0x0000,
    0x0506,
    0x0000,
    0x0508,
    0x0000,
    0x050a,
    0x0000,
    0x050c,
    0x0000,
    0x050e,
    0x0000,
    0x0510,
    0x0000,
    0x0512,
    0x0000,
    0x0514,
    0x0000,
    0x0516,
    0x0000,
    0x0518,
    0x0000,
    0x051a,
    0x0000,
    0x051c,
    0x0000,
    0x051e,
    0x0000,
    0x0520,
    0x0000,
    0x0522,
    0x0000,
    0x0524,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0531,
    0x0532,
    0x0533,
    0x0534,
    0x0535,
    0x0536,
    0x0537,
    0x0538,
    0x0539,
    0x053a,
    0x053b,
    0x053c,
    0x053d,
    0x053e,
    0x053f,
    0x0540,
    0x0541,
    0x0542,
    0x0543,
    0x0544,
    0x0545,
    0x0546,
    0x0547,
    0x0548,
    0x0549,
    0x054a,
    0x054b,
    0x054c,
    0x054d,
    0x054e,
    0x054f,
    0x0550,
    0x0551,
    0x0552,
    0x0553,
    0x0554,
    0x0555,
    0x0556,
    0x0000,
    0x0000
};


///////////////////////////////////////////////////////////////////////////////
// LowerCaseTable
//
const Char gLowerCaseTable[0x0590] = 
{
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0061,
    0x0062,
    0x0063,
    0x0064,
    0x0065,
    0x0066,
    0x0067,
    0x0068,
    0x0069,
    0x006a,
    0x006b,
    0x006c,
    0x006d,
    0x006e,
    0x006f,
    0x0070,
    0x0071,
    0x0072,
    0x0073,
    0x0074,
    0x0075,
    0x0076,
    0x0077,
    0x0078,
    0x0079,
    0x007a,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x00e0,
    0x00e1,
    0x00e2,
    0x00e3,
    0x00e4,
    0x00e5,
    0x00e6,
    0x00e7,
    0x00e8,
    0x00e9,
    0x00ea,
    0x00eb,
    0x00ec,
    0x00ed,
    0x00ee,
    0x00ef,
    0x00f0,
    0x00f1,
    0x00f2,
    0x00f3,
    0x00f4,
    0x00f5,
    0x00f6,
    0x0000,
    0x00f8,
    0x00f9,
    0x00fa,
    0x00fb,
    0x00fc,
    0x00fd,
    0x00fe,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0101,
    0x0000,
    0x0103,
    0x0000,
    0x0105,
    0x0000,
    0x0107,
    0x0000,
    0x0109,
    0x0000,
    0x010b,
    0x0000,
    0x010d,
    0x0000,
    0x010f,
    0x0000,
    0x0111,
    0x0000,
    0x0113,
    0x0000,
    0x0115,
    0x0000,
    0x0117,
    0x0000,
    0x0119,
    0x0000,
    0x011b,
    0x0000,
    0x011d,
    0x0000,
    0x011f,
    0x0000,
    0x0121,
    0x0000,
    0x0123,
    0x0000,
    0x0125,
    0x0000,
    0x0127,
    0x0000,
    0x0129,
    0x0000,
    0x012b,
    0x0000,
    0x012d,
    0x0000,
    0x012f,
    0x0000,
    0x0069,
    0x0000,
    0x0133,
    0x0000,
    0x0135,
    0x0000,
    0x0137,
    0x0000,
    0x0000,
    0x013a,
    0x0000,
    0x013c,
    0x0000,
    0x013e,
    0x0000,
    0x0140,
    0x0000,
    0x0142,
    0x0000,
    0x0144,
    0x0000,
    0x0146,
    0x0000,
    0x0148,
    0x0000,
    0x0000,
    0x014b,
    0x0000,
    0x014d,
    0x0000,
    0x014f,
    0x0000,
    0x0151,
    0x0000,
    0x0153,
    0x0000,
    0x0155,
    0x0000,
    0x0157,
    0x0000,
    0x0159,
    0x0000,
    0x015b,
    0x0000,
    0x015d,
    0x0000,
    0x015f,
    0x0000,
    0x0161,
    0x0000,
    0x0163,
    0x0000,
    0x0165,
    0x0000,
    0x0167,
    0x0000,
    0x0169,
    0x0000,
    0x016b,
    0x0000,
    0x016d,
    0x0000,
    0x016f,
    0x0000,
    0x0171,
    0x0000,
    0x0173,
    0x0000,
    0x0175,
    0x0000,
    0x0177,
    0x0000,
    0x00ff,
    0x017a,
    0x0000,
    0x017c,
    0x0000,
    0x017e,
    0x0000,
    0x0000,
    0x0000,
    0x0253,
    0x0183,
    0x0000,
    0x0185,
    0x0000,
    0x0254,
    0x0188,
    0x0000,
    0x0256,
    0x0257,
    0x018c,
    0x0000,
    0x0000,
    0x01dd,
    0x0259,
    0x025b,
    0x0192,
    0x0000,
    0x0260,
    0x0263,
    0x0000,
    0x0269,
    0x0268,
    0x0199,
    0x0000,
    0x0000,
    0x0000,
    0x026f,
    0x0272,
    0x0000,
    0x0275,
    0x01a1,
    0x0000,
    0x01a3,
    0x0000,
    0x01a5,
    0x0000,
    0x0280,
    0x01a8,
    0x0000,
    0x0283,
    0x0000,
    0x0000,
    0x01ad,
    0x0000,
    0x0288,
    0x01b0,
    0x0000,
    0x028a,
    0x028b,
    0x01b4,
    0x0000,
    0x01b6,
    0x0000,
    0x0292,
    0x01b9,
    0x0000,
    0x0000,
    0x0000,
    0x01bd,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x01c6,
    0x01c6,
    0x0000,
    0x01c9,
    0x01c9,
    0x0000,
    0x01cc,
    0x01cc,
    0x0000,
    0x01ce,
    0x0000,
    0x01d0,
    0x0000,
    0x01d2,
    0x0000,
    0x01d4,
    0x0000,
    0x01d6,
    0x0000,
    0x01d8,
    0x0000,
    0x01da,
    0x0000,
    0x01dc,
    0x0000,
    0x0000,
    0x01df,
    0x0000,
    0x01e1,
    0x0000,
    0x01e3,
    0x0000,
    0x01e5,
    0x0000,
    0x01e7,
    0x0000,
    0x01e9,
    0x0000,
    0x01eb,
    0x0000,
    0x01ed,
    0x0000,
    0x01ef,
    0x0000,
    0x0000,
    0x01f3,
    0x01f3,
    0x0000,
    0x01f5,
    0x0000,
    0x0195,
    0x01bf,
    0x01f9,
    0x0000,
    0x01fb,
    0x0000,
    0x01fd,
    0x0000,
    0x01ff,
    0x0000,
    0x0201,
    0x0000,
    0x0203,
    0x0000,
    0x0205,
    0x0000,
    0x0207,
    0x0000,
    0x0209,
    0x0000,
    0x020b,
    0x0000,
    0x020d,
    0x0000,
    0x020f,
    0x0000,
    0x0211,
    0x0000,
    0x0213,
    0x0000,
    0x0215,
    0x0000,
    0x0217,
    0x0000,
    0x0219,
    0x0000,
    0x021b,
    0x0000,
    0x021d,
    0x0000,
    0x021f,
    0x0000,
    0x019e,
    0x0000,
    0x0223,
    0x0000,
    0x0225,
    0x0000,
    0x0227,
    0x0000,
    0x0229,
    0x0000,
    0x022b,
    0x0000,
    0x022d,
    0x0000,
    0x022f,
    0x0000,
    0x0231,
    0x0000,
    0x0233,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x2c65,
    0x023c,
    0x0000,
    0x019a,
    0x2c66,
    0x0000,
    0x0000,
    0x0242,
    0x0000,
    0x0180,
    0x0289,
    0x028c,
    0x0247,
    0x0000,
    0x0249,
    0x0000,
    0x024b,
    0x0000,
    0x024d,
    0x0000,
    0x024f,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0371,
    0x0000,
    0x0373,
    0x0000,
    0x0000,
    0x0000,
    0x0377,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x03ac,
    0x0000,
    0x03ad,
    0x03ae,
    0x03af,
    0x03cc,
    0x03cd,
    0x03ce,
    0x0000,
    0x03b1,
    0x03b2,
    0x03b3,
    0x03b4,
    0x03b5,
    0x03b6,
    0x03b7,
    0x03b8,
    0x03b9,
    0x03ba,
    0x03bb,
    0x03bc,
    0x03bd,
    0x03be,
    0x03bf,
    0x03c0,
    0x03c1,
    0x03c3,
    0x03c4,
    0x03c5,
    0x03c6,
    0x03c7,
    0x03c8,
    0x03c9,
    0x03ca,
    0x03cb,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x03d7,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x03d9,
    0x0000,
    0x03db,
    0x0000,
    0x03dd,
    0x0000,
    0x03df,
    0x0000,
    0x03e1,
    0x0000,
    0x03e3,
    0x0000,
    0x03e5,
    0x0000,
    0x03e7,
    0x0000,
    0x03e9,
    0x0000,
    0x03eb,
    0x0000,
    0x03ed,
    0x0000,
    0x03ef,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x03b8,
    0x0000,
    0x0000,
    0x03f8,
    0x0000,
    0x03f2,
    0x03fb,
    0x0000,
    0x0000,
    0x037b,
    0x037c,
    0x037d,
    0x0450,
    0x0451,
    0x0452,
    0x0453,
    0x0454,
    0x0455,
    0x0456,
    0x0457,
    0x0458,
    0x0459,
    0x045a,
    0x045b,
    0x045c,
    0x045d,
    0x045e,
    0x045f,
    0x0430,
    0x0431,
    0x0432,
    0x0433,
    0x0434,
    0x0435,
    0x0436,
    0x0437,
    0x0438,
    0x0439,
    0x043a,
    0x043b,
    0x043c,
    0x043d,
    0x043e,
    0x043f,
    0x0440,
    0x0441,
    0x0442,
    0x0443,
    0x0444,
    0x0445,
    0x0446,
    0x0447,
    0x0448,
    0x0449,
    0x044a,
    0x044b,
    0x044c,
    0x044d,
    0x044e,
    0x044f,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0461,
    0x0000,
    0x0463,
    0x0000,
    0x0465,
    0x0000,
    0x0467,
    0x0000,
    0x0469,
    0x0000,
    0x046b,
    0x0000,
    0x046d,
    0x0000,
    0x046f,
    0x0000,
    0x0471,
    0x0000,
    0x0473,
    0x0000,
    0x0475,
    0x0000,
    0x0477,
    0x0000,
    0x0479,
    0x0000,
    0x047b,
    0x0000,
    0x047d,
    0x0000,
    0x047f,
    0x0000,
    0x0481,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x048b,
    0x0000,
    0x048d,
    0x0000,
    0x048f,
    0x0000,
    0x0491,
    0x0000,
    0x0493,
    0x0000,
    0x0495,
    0x0000,
    0x0497,
    0x0000,
    0x0499,
    0x0000,
    0x049b,
    0x0000,
    0x049d,
    0x0000,
    0x049f,
    0x0000,
    0x04a1,
    0x0000,
    0x04a3,
    0x0000,
    0x04a5,
    0x0000,
    0x04a7,
    0x0000,
    0x04a9,
    0x0000,
    0x04ab,
    0x0000,
    0x04ad,
    0x0000,
    0x04af,
    0x0000,
    0x04b1,
    0x0000,
    0x04b3,
    0x0000,
    0x04b5,
    0x0000,
    0x04b7,
    0x0000,
    0x04b9,
    0x0000,
    0x04bb,
    0x0000,
    0x04bd,
    0x0000,
    0x04bf,
    0x0000,
    0x04cf,
    0x04c2,
    0x0000,
    0x04c4,
    0x0000,
    0x04c6,
    0x0000,
    0x04c8,
    0x0000,
    0x04ca,
    0x0000,
    0x04cc,
    0x0000,
    0x04ce,
    0x0000,
    0x0000,
    0x04d1,
    0x0000,
    0x04d3,
    0x0000,
    0x04d5,
    0x0000,
    0x04d7,
    0x0000,
    0x04d9,
    0x0000,
    0x04db,
    0x0000,
    0x04dd,
    0x0000,
    0x04df,
    0x0000,
    0x04e1,
    0x0000,
    0x04e3,
    0x0000,
    0x04e5,
    0x0000,
    0x04e7,
    0x0000,
    0x04e9,
    0x0000,
    0x04eb,
    0x0000,
    0x04ed,
    0x0000,
    0x04ef,
    0x0000,
    0x04f1,
    0x0000,
    0x04f3,
    0x0000,
    0x04f5,
    0x0000,
    0x04f7,
    0x0000,
    0x04f9,
    0x0000,
    0x04fb,
    0x0000,
    0x04fd,
    0x0000,
    0x04ff,
    0x0000,
    0x0501,
    0x0000,
    0x0503,
    0x0000,
    0x0505,
    0x0000,
    0x0507,
    0x0000,
    0x0509,
    0x0000,
    0x050b,
    0x0000,
    0x050d,
    0x0000,
    0x050f,
    0x0000,
    0x0511,
    0x0000,
    0x0513,
    0x0000,
    0x0515,
    0x0000,
    0x0517,
    0x0000,
    0x0519,
    0x0000,
    0x051b,
    0x0000,
    0x051d,
    0x0000,
    0x051f,
    0x0000,
    0x0521,
    0x0000,
    0x0523,
    0x0000,
    0x0525,
    0x0000,
    0x0561,
    0x0562,
    0x0563,
    0x0564,
    0x0565,
    0x0566,
    0x0567,
    0x0568,
    0x0569,
    0x056a,
    0x056b,
    0x056c,
    0x056d,
    0x056e,
    0x056f,
    0x0570,
    0x0571,
    0x0572,
    0x0573,
    0x0574,
    0x0575,
    0x0576,
    0x0577,
    0x0578,
    0x0579,
    0x057a,
    0x057b,
    0x057c,
    0x057d,
    0x057e,
    0x057f,
    0x0580,
    0x0581,
    0x0582,
    0x0583,
    0x0584,
    0x0585,
    0x0586,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
};


///////////////////////////////////////////////////////////////////////////////
// GetCase
//
EATEXT_API CaseType GetCase(Char c)
{
    if(c < 0x0590) // Chars above this have no case that we recognize
    {
        const Char lc = gLowerCaseTable[c];
        if(lc)  // If there is a lower case version of this character...
            return kCaseTypeUpper;

        const Char uc = gUpperCaseTable[c];
        if(uc) // If there is an upper case version of this character...
            return kCaseTypeLower;
    }

    return kCaseTypeNone;
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseLower
//
static uint32_t ConvertCaseLower(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    EA_ASSERT(pTextInput != NULL);
    uint32_t i, j;

    for(i = 0, j = 0; i != nTextLength; i++)
    {
        Char c = pTextInput[i];

        // Write hard-coded logic for the most common case.
        // We have this logic here in order to avoid a possible memory cache hit to the table.
        if(c < 0x005b)
        {
            if(c > 0x0040)
                c += 0x0020;
            // else no change.
        }
        else if(c < 0x0590) // chars above this have no case that we recognize
        {
            if(gLowerCaseTable[c])
                c = gLowerCaseTable[c];
        }
        // else no change.

        if((i + 1) < nTextOutputCapacity)
            pTextOutput[j++] = c;
    }

    if(j < nTextOutputCapacity)
        pTextOutput[j] = 0;

    return i;
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseUpper
//
static uint32_t ConvertCaseUpper(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    EA_ASSERT(pTextInput != NULL);
    uint32_t i, j;

    for(i = 0, j = 0; i != nTextLength; i++)
    {
        Char c = pTextInput[i];

        // Write hard-coded logic for the most common case.
        // We have this logic here in order to avoid a possible memory cache hit to the table.
        if(c > 0x0060)
        {
            if(c < 0x007b)
                c -= 0x0020;
            else if(c < 0x0590) // chars above this have no case that we recognize
            {
                if(gUpperCaseTable[c])
                    c = gUpperCaseTable[c];
            }
            // else no change.
        }

        if((i + 1) < nTextOutputCapacity)
            pTextOutput[j++] = c;
    }

    if(j < nTextOutputCapacity)
        pTextOutput[j] = 0;

    return i;
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseTitle
//
static uint32_t ConvertCaseTitle(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    EA_ASSERT(pTextInput != NULL);
    uint32_t i, j;
    bool     prevIsSpace = true;
    bool     currentIsSpace = true;

    for(i = 0, j = 0; i != nTextLength; i++)
    {
        prevIsSpace    = currentIsSpace;
        currentIsSpace = IsSpace(pTextInput[i], kSTAll, false);

        if((i + 1) < nTextOutputCapacity)
        {
            if(prevIsSpace && !currentIsSpace)
                pTextOutput[j++] = ToUpper(pTextInput[i]);
            else
                pTextOutput[j++] = ToLower(pTextInput[i]);
        }
    }

    if(j < nTextOutputCapacity)
        pTextOutput[j] = 0;

    return i;
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseReverse
//
static uint32_t ConvertCaseReverse(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    EA_ASSERT(pTextInput != NULL);
    uint32_t i, j;

    for(i = 0, j = 0; i != nTextLength; i++)
    {
        if((i + 1) < nTextOutputCapacity)
        {
            CaseType caseType = GetCase(pTextInput[i]);

            if(caseType == kCaseTypeLower)
                pTextOutput[j++] = ToUpper(pTextInput[i]);
            else if(caseType == kCaseTypeUpper)
                pTextOutput[j++] = ToLower(pTextInput[i]);
        }
    }

    if(j < nTextOutputCapacity)
        pTextOutput[j] = 0;

    return i;
}


EATEXT_API uint32_t ConvertCase(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity, CaseType caseType)
{
    switch (caseType)
    {
        case kCaseTypeLower:
            return ConvertCaseLower(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity);

        case kCaseTypeUpper:
            return ConvertCaseUpper(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity);

        case kCaseTypeTitle:
            return ConvertCaseTitle(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity);

        case kCaseTypeReverse:
            return ConvertCaseReverse(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity);

        case kCaseTypeNone:
        default:
        {
            uint32_t i;

            for(i = 0; i != nTextLength; i++, pTextInput++)
            {
                if((i + 1) < nTextOutputCapacity)
                    *pTextOutput++ = *pTextInput;
            }

            if(nTextOutputCapacity > 0)
                *pTextOutput = 0;

            return i;
        }
    }
}


EATEXT_API uint32_t ConvertCase(const Char* pTextInput, Char* pTextOutput, uint32_t nTextOutputCapacity, CaseType caseType)
{
    const uint32_t nTextLength = (uint32_t)Strlen(pTextInput);

    return ConvertCase(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, caseType);
}


EATEXT_API Char ToLower(Char c)
{
    Char cResult[4]; // Use [4] in case ConvertCase somehow results in more than one char (couldn't possibly need to be bigger than 4).
    ConvertCase(&c, 1, &cResult[0], 4, kCaseTypeLower);
    return cResult[0]; 
}


EATEXT_API Char ToUpper(Char c)
{
    Char cResult[4]; // Use [4] in case ConvertCase somehow results in more than one char (couldn't possibly need to be bigger than 4).
    ConvertCase(&c, 1, &cResult[0], 4, kCaseTypeUpper);
    return cResult[0]; 
}





///////////////////////////////////////////////////////////////////////////////
// Collator
///////////////////////////////////////////////////////////////////////////////

Collator::Collator(bool /*bEnableCache*/, Allocator::ICoreAllocator* /*pAllocator*/)
  : mSpecialCharOption(kSCONone)
  , mAccentSortOption(kASONone)
  , mbIgnoreCase(false)
  , mbLocaleFirst(false)
  , mbDigitsAsNumbers(false)
  , mLCID(LOCALE_USER_DEFAULT)
{
    mLocale[0] = 0;
}


void Collator::SetLocale(const char* pLocale)
{
        //using namespace EA::StdC;

        mLCID = LOCALE_USER_DEFAULT;

        const size_t kTableSize = sizeof(sOSLocaleTable) / sizeof(sOSLocaleTable[0]);
        size_t i = 0;

        for(i = 0; i < kTableSize; ++i)
        {
            if(Stricmp(pLocale, sOSLocaleTable[i]) == 0)
            {
                mLCID = StrtoU32(sOSLocaleTable[i] + 6, NULL, 16);
                break;
            }
        }

        if(i == kTableSize)
        {
            for(i = 0; i < kTableSize; ++i)
            {
                if(Strnicmp(pLocale, sOSLocaleTable[i], 2) == 0)
                {
                    mLCID = StrtoU32(sOSLocaleTable[i] + 6, NULL, 16);
                    break;
                }
            }
        }
}


void Collator::EnableCache(bool /*bEnableCache*/, Allocator::ICoreAllocator* /*pAllocator*/, uint32_t /*maxMemoryUsage*/)
{

}


void Collator::SetSortOption(SortOption option, int value)
{
    switch(option)
    {
        case kSONone:
            break;

        case kSOSpecialChar:
            mSpecialCharOption = (SpecialCharOption)value;
            break;

        case kSOAccentSort:
            mAccentSortOption = (AccentSortOption)value;
            break;

        case kSOIgnoreCase:
            mbIgnoreCase = (value != 0);
            break;

        case kSOLocaleFirst:
            mbLocaleFirst = (value != 0);
            break;

        case kSODigitsAsNumbers:
            mbDigitsAsNumbers = (value != 0);
            break;
    }
}


namespace Internal
{
    int SortKeyCompare(const uint8_t* pString1, const uint8_t* pString2)
    {
        uint8_t c1, c2;

        while((c1 = *pString1++) == (c2 = *pString2++))
        {
            if(c1 == 0)
                return 0;
        }

        return (c1 - c2);
    }

} // namespace



///////////////////////////////////////////////////////////////////////////////
// BuildSortKey
//
// This function expects the string argument to be a decomposed string of 
// normalization type NFD (canonical decomposition). 
//
void Collator::BuildSortKey(const FixedSortString& /*s*/, FixedSortKey& /*k*/)
{
    // To do.
}


///////////////////////////////////////////////////////////////////////////////
// Compare
//
// Implement Unicode Technical Standard #10, section 4 (Main Algorithm)
//
// If the two strings are both entirely ASCII and mSpecialCharOption == kSCONonIgnorable, 
// then we can use a simple strcmp comparison. But we need to mind the kSOCapitalsFirst option.
//
// If caching is enabled and either string has a cached sort key, use the key.
//
int Collator::Compare(const Char* p1, const Char* p2)
{
    size_t p1Length = Strlen(p1);
    size_t p2Length = Strlen(p2);

    return Compare(p1, (uint32_t)p1Length, p2, (uint32_t)p2Length);
}


#if !EATEXT_OS_COLLATION_AVAILABLE

// To do a basic Unicode-style compare, we don't just subtract the character integer values like strcmp does.
// The character values for ASCII are ordered like this:
//    ABC...XYZ[\]^_`abc...xyz...(accented euro chars follow) AaAaBbCcDdDd...
// but we need to compare as if the characters are ordered like this:
//   ...[\]^_`aAbBcC...xXyYzZ... (accented euro chars follow) aAaAbBcCdDdD...
// The Unicode Standard provides a better and more complete algorithm for this, though it requires
// heavier table lookups. We are going to see if the following simplified code suffices until we
// write the better algorithm, assuming the better algorithm isn't too heavy.
static int CompareBasicIgnoreCase(const Char* p1, uint32_t p1Length, const Char* p2, uint32_t p2Length)
{
    while(p1Length && p2Length)
    {
        Char c1        = *p1++;
        Char c2        = *p2++;
        int c1Lower    = static_cast<int>(ToLower(c1));
        int c2Lower    = static_cast<int>(ToLower(c2));

        if(c1Lower != c2Lower) // If the lower case version of them is different (e.g. c1 == 'c' and c2 == 'a')
        {
            // Major problem with this: It causes accented 'a' to sort higher than unaccented 'b'. 
            // All characters in the 'a' family should sort before all characters in the 'b' family.
            return (c1Lower - c2Lower);
        }
        // else the characters are equal

        --p1Length;
        --p2Length;
    }

    if(p1Length)  // If the strings were equal up to the end of pString2, but pString1 had additional characters (and thus is longer)...
        return  1; // string 1 is greater than string2.
    else if (p2Length)
        return -1; // string 2 is greater than string1.

    return 0;
}


// Note that we are doing a (simplified) Unicode Standard compare, which is not the same as strcmp.
// See above for a detailed discussion of this.
static int CompareBasic(const Char* p1, uint32_t p1Length, const Char* p2, uint32_t p2Length)
{
    while(p1Length && p2Length)
    {
        Char c1        = *p1++;
        Char c2        = *p2++;
        bool c1IsUpper = (GetCase(c1) == kCaseTypeUpper);
        bool c2IsUpper = (GetCase(c2) == kCaseTypeUpper);
        int c1Lower    = static_cast<int>(ToLower(c1));
        int c2Lower    = static_cast<int>(ToLower(c2));

        if(c1Lower != c2Lower) // If the lower case version of them is different (e.g. c1 == 'c' and c2 == 'a')
        {
            // Major problem with this: It causes accented 'A' to sort higher than unaccented 'b'. 
            // All characters in the 'A' family should sort before all characters in the 'B' family.
            return (c1Lower - c2Lower);
        }
        else if(c1Lower == c2Lower) // If the lower case version of them is equal (e.g. c1 == 'a' and c2 == 'a')
        {
            if(c1IsUpper != c2IsUpper) // If they were originally of different case... (e.g. c1 == 'A' and c2 == 'a')
                return (c1IsUpper ? 1 : -1);
        }
        // else the characters are equal

        --p1Length;
        --p2Length;
    }

    if(p1Length)  // If the strings were equal up to the end of pString2, but pString1 had additional characters (and thus is longer)...
        return  1; // string 1 is greater than string2.
    else if (p2Length)
        return -1; // string 2 is greater than string1.

    return 0;
}

#endif // EATEXT_OS_COLLATION_AVAILABLE


int Collator::Compare(const Char* p1, uint32_t p1Length, const Char* p2, uint32_t p2Length)
{
    // Until we completely finish the platform-independent code, we use the Windows CompareString API under Windows.
    #if EATEXT_OS_COLLATION_AVAILABLE && defined(EA_PLATFORM_WINDOWS)
        // Note: it seems that Windows wants upperc-case versions of the locale country (e.g. "en-US" instead of "en-us").
        DWORD dwFlags = 0;

        if(mbIgnoreCase)
            dwFlags |= NORM_IGNORECASE; // LINGUISTIC_IGNORECASE seems better than NORM_IGNORECASE, but is available only on Vista+. For most uses they supposedly act the same.

        #if (WINVER >= 0x0601)
            if(mbDigitsAsNumbers) // You need the Windows 7 SDK in order to support this.
                dwFlags |= SORT_DIGITSASNUMBERS;
        #endif

        // CompareStringExW exists but only on Vista and later.
        const int result = CompareStringW(mLCID, dwFlags, p1, (int)p1Length, p2, (int)p2Length);

        return (result - 2); // See the Windows documentation for an explanation of this.

    #else
        // This is an unlocalized implementation, but works most of the time with Western text anyway.
        if(mbIgnoreCase)
            return CompareBasicIgnoreCase(p1, p1Length, p2, p2Length);
        else
            return CompareBasic(p1, p1Length, p2, p2Length);

        /* This is unfinished:
        using namespace Internal;

        int             result = 0;
        const Char*     p[2] = { p1, p2 };
        FixedSortString s[2];
        FixedSortKey    k[2];
        int             resultFlags[2];
        size_t          i;

        for(i = 0; i < 2; i++)
        {
            s[i].resize(kFixedLength - 1, Char(0)); // To do: Make it so this resize faster, as it currently fills the string with zeroes.

            const uint32_t requiredLength = Normalize(p[i], 0xffffffff, &s[i][0], s[i].size(), kNT_NFD, &resultFlags[i]);

            s[i].resize(requiredLength);

            if(requiredLength > s[i].size()) // If there wasn't initially enough room...
                Normalize(p[i], 0xffffffff, &s[i][0], s[i].size(), kNT_NFD, &resultFlags[i]);
        }

        // If we can do a simple strcmp...
        if((mSpecialCharOption == kSCONonIgnorable) && (resultFlags[0] & kNRFAscii) && (resultFlags[1] & kNRFAscii))
            result = Strcmp(s[0].c_str(), s[1].c_str());
        else
        {
            BuildSortKey(s[0], k[0]);
            BuildSortKey(s[1], k[1]);

            result = SortKeyCompare(k[0].c_str(), k[1].c_str());

            // If caching is enabled, cache the sort keys.
            //   (to do)
        }

        return result;
        */
    #endif
}


} // namespace Text

} // namespace EA


