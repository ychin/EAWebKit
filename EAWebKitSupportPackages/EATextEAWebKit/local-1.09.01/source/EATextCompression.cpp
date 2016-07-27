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
// EATextCompression.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextCompression.h>
#include EA_ASSERT_HEADER


namespace EA
{
namespace Text
{


// SCSU command values
enum SCSUCommand
{
    SQ0 = 0x01, // Quote from window pair 0
    SQU = 0x0e, // Quote a single Unicode character
    SCU = 0x0f, // Change to Unicode mode
    SC0 = 0x10, // Select window 0
    UC0 = 0xe0, // Select window 0
    UQU = 0xf0  // Quote a single Unicode character
};


// Constant kSCSUWindowOffsets for the 8 static and 8 dynamic windows 
static const Char kSCSUWindowOffsets[16] =
{
    // Initial kSCSUWindowOffsets for the 8 dynamic (sliding) windows
    0x0080, // Latin-1
    0x00c0, // Latin Extended A
    0x0400, // Cyrillic
    0x0600, // Arabic
    0x0900, // Devanagari
    0x3040, // Hiragana
    0x30a0, // Katakana
    0xff00, // Fullwidth ASCII

    // Offsets for the 8 static windows
    0x0000, // ASCII for quoted tags
    0x0080, // Latin - 1 Supplement (for access to punctuation)
    0x0100, // Latin Extended-A
    0x0300, // Combining Diacritical Marks
    0x2000, // General Punctuation
    0x2080, // Currency Symbols 
    0x2100, // Letterlike Symbols and Number Forms
    0x3000  // CJK Symbols and punctuation
};


static bool IsWithinWindow(Char c, int32_t offset)
{
    return ((uint32_t)c - (uint32_t)offset) < 0x80u;
}


static int GetWindow(Char c)
{
    // To consider: Implement this as a binary search of some sort.
    const int kCount = sizeof(kSCSUWindowOffsets) / sizeof(kSCSUWindowOffsets[0]);

    for(int i = 0; i < kCount; ++i)
    {
        if(IsWithinWindow(c, kSCSUWindowOffsets[i]))
            return i;
    }

    return -1;
}


///////////////////////////////////////////////////////////////////////////////
// CompressSCSU
//
// For SCSU, we use a variation of the Unicode standard sample code defined 
// in http://www.unicode.org/reports/tr6/
//
EATEXT_API uint32_t CompressSCSU(const Char* pTextInput, uint32_t nTextInputLength, 
                                       Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    // State variables
    bool bUnicodeMode = false;
    int  nWindow = 0;   // Dynamic window 0..7

    // Loop variables
    const Char* const pTextInputEnd  = pTextInput + nTextInputLength;
    const Char* const pTextOutputEnd = pTextOutput + nTextOutputCapacity;
    Char* pTextOutputCurrent = pTextOutput;
    int   w; // Result of GetWindow(), -1..7

    while(pTextInput < pTextInputEnd)
    {
        const Char c = *pTextInput++;

        if(bUnicodeMode)
        {
            if(c <= 0x7f)
            {
                // US-ASCII: switch to single-byte mode with the previous dynamic window
                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = (Char)(UC0 + nWindow);
                pTextOutputCurrent++;
                bUnicodeMode = false;
                pTextInput--; // Back up and go again in non-Unicode mode.
            }
            else if(((w = GetWindow(c)) >= 0) && (w <= 7)) 
            {
                // Switch to single-byte mode with a matching dynamic window
                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = (Char)(UC0 + w);
                pTextOutputCurrent++;
                nWindow = w;
                bUnicodeMode = false;
                pTextInput--; // Back up and go again in non-Unicode mode.
            }
            else
            {
                if((c - 0xe000u) <= (0xf2ffu - 0xe000u)) // Equivalent to: if((0xe000 <= c) && (c <= 0xf2ff))
                {
                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = UQU;
                    pTextOutputCurrent++;
                }

                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = (Char)(c >> 8);
                pTextOutputCurrent++;

                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = c;
                pTextOutputCurrent++;
            }
        }
        else
        {
            // Single-byte mode
            if(c <= 0x007f)
            {
                if(c < 0x0020)
                {
                    // Encode C0 control code:
                    // Check the code point against the bit mask 0010 0110 0000 0001
                    // which contains 1-bits at the bit positions corresponding to
                    // code points 0D 0A 09 00 (CR LF TAB NUL) which are encoded directly.
                    // All other C0 control codes are quoted with SQ0.
                    if((c < 0x0010) && (((1 << c) & 0x2601) == 0))
                    {
                        if(pTextOutputCurrent < pTextOutputEnd)
                            *pTextOutputCurrent = SQ0;
                        pTextOutputCurrent++;
                    }
                }

                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = c;
                pTextOutputCurrent++;
            }
            else if(IsWithinWindow(c, kSCSUWindowOffsets[nWindow]))
            {
                // Use the current dynamic window
                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = (Char)(0x80 + (c - kSCSUWindowOffsets[nWindow]));
                pTextOutputCurrent++;
            }
            else if((w = GetWindow(c)) >= 0)
            {
                if(w <= 7)
                {
                    // Switch to a dynamic window
                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)(SC0 + w);
                    pTextOutputCurrent++;

                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)(0x80 + (c - kSCSUWindowOffsets[w]));
                    pTextOutputCurrent++;

                    nWindow = w;
                }
                else
                {
                    // Quote from a static window
                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)(SQ0 + (w - 8));
                    pTextOutputCurrent++;

                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)(c - kSCSUWindowOffsets[w]);
                    pTextOutputCurrent++;
                }
            }
            else if(c == 0xfeff)
            {
                // Encode the signature character U+FEFF with SQU
                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = SQU;
                pTextOutputCurrent++;

                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = 0xfe;
                pTextOutputCurrent++;

                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = 0xff;
                pTextOutputCurrent++;
            }
            else
            {
                // Switch to Unicode mode
                if(pTextOutputCurrent < pTextOutputEnd)
                    *pTextOutputCurrent = SCU;
                pTextOutputCurrent++;

                bUnicodeMode = true;
                pTextInput--; // Back up and go again in Unicode mode.
            }
        }
    }

    // Return the number of characters required to do the compression.
    return (uint32_t)(uintptr_t)(pTextOutputCurrent - pTextOutput);
}


/// kSlideValues
///
/// This table is intentionally non-const, as it is potenially 
/// modified by the DecompressSCSU function.
///
static Char kSlideValues[8] = 
{
    0x0080,
    0x00C0,
    0x0400,
    0x0600,
    0x0900,
    0x3040,
    0x30A0,
    0xFF00
};

const static Char kWindow[256]=
{
    0x0000, 0x0080, 0x0100, 0x0180, 0x0200, 0x0280, 0x0300, 0x0380,
    0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780,
    0x0800, 0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80,
    0x0C00, 0x0C80, 0x0D00, 0x0D80, 0x0E00, 0x0E80, 0x0F00, 0x0F80,
    0x1000, 0x1080, 0x1100, 0x1180, 0x1200, 0x1280, 0x1300, 0x1380,
    0x1400, 0x1480, 0x1500, 0x1580, 0x1600, 0x1680, 0x1700, 0x1780,
    0x1800, 0x1880, 0x1900, 0x1980, 0x1A00, 0x1A80, 0x1B00, 0x1B80,
    0x1C00, 0x1C80, 0x1D00, 0x1D80, 0x1E00, 0x1E80, 0x1F00, 0x1F80,
    0x2000, 0x2080, 0x2100, 0x2180, 0x2200, 0x2280, 0x2300, 0x2380,
    0x2400, 0x2480, 0x2500, 0x2580, 0x2600, 0x2680, 0x2700, 0x2780,
    0x2800, 0x2880, 0x2900, 0x2980, 0x2A00, 0x2A80, 0x2B00, 0x2B80,
    0x2C00, 0x2C80, 0x2D00, 0x2D80, 0x2E00, 0x2E80, 0x2F00, 0x2F80,
    0x3000, 0x3080, 0x3100, 0x3180, 0x3200, 0x3280, 0x3300, 0x3800,
    0xE000, 0xE080, 0xE100, 0xE180, 0xE200, 0xE280, 0xE300, 0xE380,
    0xE400, 0xE480, 0xE500, 0xE580, 0xE600, 0xE680, 0xE700, 0xE780,
    0xE800, 0xE880, 0xE900, 0xE980, 0xEA00, 0xEA80, 0xEB00, 0xEB80,
    0xEC00, 0xEC80, 0xED00, 0xED80, 0xEE00, 0xEE80, 0xEF00, 0xEF80,
    0xF000, 0xF080, 0xF100, 0xF180, 0xF200, 0xF280, 0xF300, 0xF380,
    0xF400, 0xF480, 0xF500, 0xF580, 0xF600, 0xF680, 0xF700, 0xF780,
    0xF800, 0xF880, 0xF900, 0xF980, 0xFA00, 0xFA80, 0xFB00, 0xFB80,
    0xFC00, 0xFC80, 0xFD00, 0xFD80, 0xFE00, 0xFE80, 0xFF00, 0xFF80,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x00C0, 0x0250, 0x0370, 0x0530, 0x3040, 0x30A0, 0xFF60
};



///////////////////////////////////////////////////////////////////////////////
// DecompressSCSU
//
EATEXT_API uint32_t DecompressSCSU(const Char* pTextInput, uint32_t nTextInputLength, 
                                         Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    // State variables
    bool bUnicodeMode = false;
    int  nWindow = 0;   // Dynamic window 0..7

    // Loop variables
    const Char* const pTextInputEnd  = pTextInput + nTextInputLength;
    const Char* const pTextOutputEnd = pTextOutput + nTextOutputCapacity;
    Char* pTextOutputCurrent = pTextOutput;

    while(pTextInput < pTextInputEnd)
    {
        Char c = *pTextInput++;

        if((c - 0x0020u) < (0x0080u - 0x0020u)) // This is a faster way of saying: if((c >= 0x0020) && (c < 0x0080))
        {
            if(pTextOutputCurrent < pTextOutputEnd)
                *pTextOutputCurrent = c;
            pTextOutputCurrent++;
        }
        else if(c >= 0x0080)
        {
            if(pTextOutputCurrent < pTextOutputEnd)
                *pTextOutputCurrent = (Char)((c - 0x80) + kSlideValues[nWindow]);
            pTextOutputCurrent++;
        }
        else if(((1 << c) & 0x3601) == 0) // If c is one of (0x000D 0x000C 0x000A 0x0009 0x0000) ...
        {
            if(pTextOutputCurrent < pTextOutputEnd)
                *pTextOutputCurrent = c;
            pTextOutputCurrent++;
        }
        else if((c - 0x0001u) <= (0x0008u - 0x0001u)) // SQn   // This is a faster way of saying: if((c >= 0x0008) && (c <= 0x0001))
        {
            // Single quote
            const Char d = *pTextInput++;

            if(pTextOutputCurrent < pTextOutputEnd)
                *pTextOutputCurrent = (Char)(d < 0x0080 ? d + (kSCSUWindowOffsets + 8)[c - 0x0001] : d - 0x0080 + kSlideValues[c - 0x0001]);
            pTextOutputCurrent++;
        }
        else if((c - 0x0010u) <= (0x0017u - 0x0010u)) // SCn   // This is a faster way of saying: if((c >= 0x0010) && (c <= 0x0017))
        {
            // Change window
            nWindow = c - 0x0010;
        }
        else if((c >= 0x0018) && (c <= 0x001F)) // SDn
        {
            // Define window
            nWindow = c - 0x0018;
            kSlideValues[nWindow] = kWindow[*pTextInput++];
        }
        else if(c == 0x000B) // SDX
        {
            const Char d = *pTextInput++;
            const Char e = *pTextInput++;

            nWindow = (d >> 5);
            kSlideValues[nWindow] = (Char)(0x10000 + ((((d & 0x001F) << 8) | e) << 7));
        }
        else if(c == 0x000E) // SQU
        {
            c = *pTextInput++;

            if(pTextOutputCurrent < pTextOutputEnd)
                *pTextOutputCurrent = (Char)((c << 8) | (*pTextInput++));
            pTextOutputCurrent++;
        }
        else if(c == 0x000F) // SCU
        {
            // Change to Unicode mode 
            bUnicodeMode = true;

            while(bUnicodeMode)
            {
                c = *pTextInput++;

                if((c <= 0x00DF) || (c >= 0x00F3))
                {
                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)((c << 8) | (*pTextInput++));
                    pTextOutputCurrent++;
                }
                else if(c == 0x00F0) // UQU
                {
                    c = *pTextInput++;

                    if(pTextOutputCurrent < pTextOutputEnd)
                        *pTextOutputCurrent = (Char)((c << 8) | (*pTextInput++));
                    pTextOutputCurrent++;
                }
                else if((c - 0x00E0u) <= (0x00E7u - 0x00E0u)) // UCn        // This is a faster way of saying: if((c >= 0x00E0) && (c <= 0x00E7))
                {
                    nWindow = c - 0x00E0; 
                    bUnicodeMode = false;
                }
                else if((c - 0x00E8u) <= (0x00EF - 0x00E8u)) // UDn         // This is a faster way of saying: if((c >= 0x00E8) && (c <= 0x00EF))
                {
                    nWindow = (c - 0x00E8);
                    kSlideValues[nWindow] = kWindow[*pTextInput++]; 
                    bUnicodeMode = false;
                }
                else if(c == 0x00F1) // UDX
                {
                    const Char d = *pTextInput++;
                    const Char e = *pTextInput++;

                    nWindow = (d >> 5);
                    kSlideValues[nWindow] = (Char)(0x10000 + ((((d & 0x001F) << 8) | e) << 7));
                    bUnicodeMode = false;
                }
            }
        }
    }

    // Return the number of characters required to do the compression.
    return (uint32_t)(uintptr_t)(pTextOutputCurrent - pTextOutput);
}




///////////////////////////////////////////////////////////////////////////////
// CompressBOCU
//
EATEXT_API uint32_t CompressBOCU(const Char* pTextInput, uint32_t nTextInputLength, 
                                       Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    const Char* const pTextInputEnd  = pTextInput + nTextInputLength;
    const Char* const pTextOutputEnd = pTextOutput + nTextOutputCapacity;
    Char* pTextOutputCurrent = pTextOutput;

    while(pTextInput < pTextInputEnd)
    {
        Char c = *pTextInput++;

        // This implementation is incomplete. Currently all we do is copy 
        // the bytes. We will implement the compression in the future.
        // Bug Paul Pedriana if and when you need this.
        if(pTextOutputCurrent < pTextOutputEnd)
            *pTextOutputCurrent = c;
        pTextOutputCurrent++;
    }

    // Return the number of characters required to do the compression.
    return (uint32_t)(uintptr_t)(pTextOutputCurrent - pTextOutput);
}



///////////////////////////////////////////////////////////////////////////////
// DecompressBOCU
//
EATEXT_API uint32_t DecompressBOCU(const Char* pTextInput, uint32_t nTextInputLength, 
                                         Char* pTextOutput, uint32_t nTextOutputCapacity)
{
    const Char* const pTextInputEnd  = pTextInput + nTextInputLength;
    const Char* const pTextOutputEnd = pTextOutput + nTextOutputCapacity;
    Char* pTextOutputCurrent = pTextOutput;

    while(pTextInput < pTextInputEnd)
    {
        Char c = *pTextInput++;

        // This implementation is incomplete. Currently all we do is copy 
        // the bytes. We will implement the decompression in the future.
        if(pTextOutputCurrent < pTextOutputEnd)
            *pTextOutputCurrent = c;
        pTextOutputCurrent++;
    }

    // Return the number of characters required to do the compression.
    return (uint32_t)(uintptr_t)(pTextOutputCurrent - pTextOutput);
}



} // namespace Text

} // namespace EA









