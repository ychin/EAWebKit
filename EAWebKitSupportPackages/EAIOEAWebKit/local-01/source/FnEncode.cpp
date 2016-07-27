/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// FnEncode.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Alex Liberman and Talin
//
// Character transcoding function for filenames.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include EA_ASSERT_HEADER

#include <EAIO/FnEncode.h>


namespace EA
{
    namespace IO
    {

        // static const int32_t kLeadingSurrogateStart  = 0x0000d800;
        // static const int32_t kLeadingSurrogateEnd    = 0x0000dbff;
        // static const int32_t kTrailingSurrogateStart = 0x0000dc00;
        // static const int32_t kTrailingSurrogateEnd   = 0x0000dfff;
        // static const int32_t kSurrogateOffset        = 0x00010000 - (kLeadingSurrogateStart << 10) - kTrailingSurrogateStart;

        //////////////////////////////////////////////////////////////////////////
        static uint8_t utf8lengthTable[ 256 ] = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };

        //////////////////////////////////////////////////////////////////////////
        /// Used to subtract out the control bits in multi-byte sequence.
        static const uint32_t utf8DecodingOffsetTable[ 5 ] = {
            0,                                                  // 0x00000000
            0,                                                  // 0x00000000
            (0xC0 << 6) + 0x80,                                 // 0x00003080
            (0xE0 << 12) + (0x80 << 6) + 0x80,                  // 0x000e2080
            (0xF0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80    // 0x03c82080
        };

        //////////////////////////////////////////////////////////////////////////
        // The minimum value that can be encoded in a particular number
        // of bytes. Used to disallow non-canonical encoded sequences.
        // It turns out that this is not a fully proper way to check for 
        // valid sequences. See the Unicode Standard http://unicode.org/versions/corrigendum1.html
        static const uint32_t utf8MinimumValueTable[ 5 ] = {
            0x00000000,     // This slot is unused
            0x00000000,     // 1 byte per char
            0x00000080,     // 2 bytes per char
            0x00000800,     // 3 bytes per char
            0x00010000      // 4 bytes per char
        };

        //////////////////////////////////////////////////////////////////////////
        // One past the maximum value that can be encoded in a particular number
        // of bytes. Used to disallow non-canonical encoded sequences.
        static const uint32_t utf8MaximumValueTable[ 5 ] = {
            0x00000000,     // This slot is unused
            0x00000080,     // 1 byte per char
            0x00000800,     // 2 bytes per char
            0x00010000,     // 3 bytes per char
            0x00110000      // 4 bytes per char
        };


        ///////////////////////////////////////////////////////////////////////////////
        // StrlcpyUTF16toUTF8
        //
        EAIO_API size_t StrlcpyUTF16ToUTF8(char8_t* pDest, size_t nDestLength, const char16_t* pSrc, size_t nSrcLength)
        {
            size_t destCount = 0;

            while (nSrcLength-- > 0)
            {
                uint32_t c = *pSrc++;   // Deal with surrogate characters

                // Encode as UTF-8
                if (c < 0x00000080u)
                {
                    if (c == 0) // Break on NULL char, even if explicit length was set
                        break;

                    if (pDest && destCount + 1 < nDestLength)
                    {
                        *pDest++ = static_cast<char8_t>(c);
                    }
                    destCount += 1;
                }
                else if (c < 0x00000800u)
                {
                    if (pDest && destCount + 2 < nDestLength)
                    {
                        *pDest++ = static_cast<char8_t>((c >> 6) | 0xc0);
                        *pDest++ = static_cast<char8_t>((c | 0x80) & 0xbf);
                    }
                    destCount += 2;
                }
                else if (c < 0x00010000u)
                {
                    if (pDest && destCount + 3 < nDestLength)
                    {
                        *pDest++ = static_cast<char8_t>((c >> 12) | 0xe0);
                        *pDest++ = static_cast<char8_t>(((c >>  6) | 0x80) & 0xbf);
                        *pDest++ = static_cast<char8_t>((c | 0x80) & 0xbf);
                    }
                    destCount += 3;
                }
                else if (c < 0x00200000u)
                {
                    if (pDest && destCount + 4 < nDestLength)
                    {
                        *pDest++ = static_cast<char8_t>((c >> 18) | 0xf0);
                        *pDest++ = static_cast<char8_t>(((c >> 12) | 0x80) & 0xbf);
                        *pDest++ = static_cast<char8_t>(((c >>  6) | 0x80) & 0xbf);
                        *pDest++ = static_cast<char8_t>((c | 0x80) & 0xbf);
                    }
                    destCount += 4;
                }
                else
                {
                    const uint32_t kIllegalUnicodeChar = 0x0000fffd;
                    if (pDest && destCount + 3 < nDestLength)
                    {
                        *pDest++ = static_cast<char8_t>((kIllegalUnicodeChar >> 12) | 0xe0);
                        *pDest++ = static_cast<char8_t>(((kIllegalUnicodeChar >>  6) | 0x80) & 0xbf);
                        *pDest++ = static_cast<char8_t>((kIllegalUnicodeChar | 0x80) & 0xbf);
                    }
                    destCount += 3;
                }
            }

            if (pDest && nDestLength != 0)
                *pDest = 0;

            return destCount;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // StrlcpyUTF8toUTF16
        //
        EAIO_API size_t StrlcpyUTF8ToUTF16(char16_t* pDest, size_t nDestLength, const char8_t* pSrc, size_t nSrcLength)
        {
            size_t destCount = 0;

            while (nSrcLength-- > 0)
            {
                uint32_t c = (uint8_t)*pSrc++; // Important that we cast to uint8_t, because char8_t is signed.

                if (c < 128)
                {
                    if (c == 0) // Break on NULL char, even if explicit length was set
                        break;

                    if (pDest && destCount + 1 < nDestLength)
                        *pDest++ = static_cast<char16_t>(c);

                    destCount += 1;
                }
                else
                {
                    uint32_t nLength = utf8lengthTable[c]; // nLength may be zero, in which case we'll return 'IncorrectEncoding'.

                    // Do we have an incomplete string?
                    if ((nLength > (nSrcLength + 1)) || (nLength == 0)) {
                        EA_FAIL_MSG("Incomplete Unicode character in buffer");
                        break;
                    }

                    // Now decode the remaining ("following") bytes.
                    for (uint32_t i = 0; i < nLength - 1; ++i) 
                    {
                        uint8_t nByte = *pSrc++;

                        if ((nByte < 0x80u) || (nByte > 0xbfu))   // Syntax check
                        {
                            EA_FAIL_MSG("Invalid following byte");
                            return destCount;
                        }

                        c = (c << 6) + nByte;   // Preserve control bits (don't OR)
                    }

                    nSrcLength -= (nLength - 1);                // We've just processed all remaining bytes for this multi-byte character
                    c -= utf8DecodingOffsetTable[ nLength ];    // Subtract accumulated control bits just once

                    // Check for canonical encoding.
                    if ((c >= utf8MinimumValueTable[nLength]) && (c < utf8MaximumValueTable[nLength]))
                    {
                        if (pDest && destCount + 1 < nDestLength)
                            *pDest++ = static_cast<char16_t>(c);

                        destCount += 1;
                    }
                    else
                        break;
                }
            }

            if (pDest && nDestLength != 0)
                *pDest = 0;

            return destCount;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // ConvertPathUTF8ToUTF16
        //
        EAIO_API uint32_t ConvertPathUTF8ToUTF16(Path::PathString16& dstPath16, const char8_t* pSrcPath8)
        {
            uint32_t nCharsNeeded = (uint32_t)StrlcpyUTF8ToUTF16(NULL, 0, pSrcPath8);
            dstPath16.resize(nCharsNeeded);
            // (EASTL factors-in a hidden + 1 for NULL terminator)
            return (uint32_t)StrlcpyUTF8ToUTF16(&dstPath16[0], nCharsNeeded + 1, pSrcPath8);
        }


        ///////////////////////////////////////////////////////////////////////////////
        // ConvertPathUTF16ToUTF8
        //
        EAIO_API uint32_t ConvertPathUTF16ToUTF8(Path::PathString8& dstPath8, const char16_t* pSrcPath16)
        {
            uint32_t nCharsNeeded = (uint32_t)StrlcpyUTF16ToUTF8(NULL, 0, pSrcPath16);
            dstPath8.resize(nCharsNeeded);
            // (EASTL factors-in a hidden + 1 for NULL terminator)
            return (uint32_t)StrlcpyUTF16ToUTF8(&dstPath8[0], nCharsNeeded + 1, pSrcPath16);
        }


        ///////////////////////////////////////////////////////////////////////////////
        // StrEq16
        //
        EAIO_API bool StrEq16(const char16_t* str1, const char16_t* str2)
        {
            while (*str1 == *str2)
            {
                if (*str1 == 0)
                    return true;
                str1++;
                str2++;
            }
            return false;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // EAIOStrlen8
        //
        EAIO_API size_t EAIOStrlen8(const char8_t* str)
        {
            size_t result = 0;
            while (*str++)
                result++;
            return result;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // EAIOStrlen16
        //
        EAIO_API size_t EAIOStrlen16(const char16_t* str)
        {
            size_t result = 0;
            while (*str++)
                result++;
            return result;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // EAIOStrlcpy8
        //
        EAIO_API size_t EAIOStrlcpy8(char8_t* pDestination, const char8_t* pSource, size_t nDestCapacity)
        {
            const size_t n = strlen(pSource);

            if(n < nDestCapacity)
                memcpy(pDestination, pSource, (n + 1) * sizeof(*pSource));
            else
            {
                if(nDestCapacity)
                {
                    memcpy(pDestination, pSource, (nDestCapacity - 1) * sizeof(*pSource));
                    if(nDestCapacity)
                        pDestination[nDestCapacity - 1] = 0;
                }
            }

            return n;
        }


        ///////////////////////////////////////////////////////////////////////////////
        // EAIOStrlcpy16
        //
        EAIO_API size_t EAIOStrlcpy16(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity)
        {
            const size_t n = EAIOStrlen16(pSource);

            if(n < nDestCapacity)
                memcpy(pDestination, pSource, (n + 1) * sizeof(*pSource));
            else
            {
                if(nDestCapacity)
                {
                    memcpy(pDestination, pSource, (nDestCapacity - 1) * sizeof(*pSource));
                    if(nDestCapacity)
                        pDestination[nDestCapacity - 1] = 0;
                }
            }

            return n;
        }

    }
}
