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
// EATextScriptThai.h
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSCRIPTTHAI_H
#define EATEXT_INTERNAL_EATEXTSCRIPTTHAI_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{

namespace Text
{


///////////////////////////////////////////////////////////////////////////////
// ThaiCharClass
//
// We use the standard TACTIS (Thai API Consortium/Thai Industrial Standard) 
// character classifications. We do not use the TACTIS character set, however.
// We use the Unicode character set, which largely maps to the TACTIS set.
//
// See Section 2 and 3 of "Thai Input and Output Methods" for descriptions of 
// these classes.
//
enum ThaiCharClass
{
    kThaiCharClassCTRL, // Control characters, not actually part of Unicode Thai characters
    kThaiCharClassNON,  // Non-composables
    kThaiCharClassCONS, // Consonant
    kThaiCharClassLV,   // Leading vowel
    kThaiCharClassFV1,  // Following vowel
    kThaiCharClassFV2,
    kThaiCharClassFV3,
    kThaiCharClassAM,   // Sara Am, special character which is a special kind of follow vowel
    kThaiCharClassBV1,  // Below vowel
    kThaiCharClassBV2,
    kThaiCharClassBD,   // Below diacritic
    kThaiCharClassTONE, // Tone
    kThaiCharClassAD1,  // Above diacritic
    kThaiCharClassAD2,
    kThaiCharClassAD3,
    kThaiCharClassAD4,
    kThaiCharClassAV1,  // Above vowel
    kThaiCharClassAV2,
    kThaiCharClassAV3,
    kThaiCharClassBCON  // Bot Tail Consonant, a Lao character.
};


///////////////////////////////////////////////////////////////////////////////
// IsCharThai
//
// This is a fast alternative to (GetScriptFromChar(c) == kScriptThai).
//
// This comparison checks the entire Thai block, though actually some 
// chars in the Thai block aren't assigned. Thus this is more of a check
// for the Thai block and not Thai script. The large majority of the time
// checking for the block is sufficient; and it's much faster.
//
inline bool IsCharThai(Char c)
{
    return ((unsigned)c - 0x0E00u) < (0x0E80 - 0x0E00u); // Faster way of saying (c >= 0x0E00 && c < 0x0E80)
}


///////////////////////////////////////////////////////////////////////////////
// IsCharLao
//
// This is a fast alternative to (GetScriptFromChar(c) == kScriptLao).
//
inline bool IsCharLao(Char c)
{
    // This comparison checks the entire Lao block, though actually some 
    // chars in the Thai block aren't assigned.
    return ((unsigned)c - 0x0E80u) < (0x0F00u - 0x0E80u); // Faster way of saying (c >= 0x0E80 && c < 0x0F00)
}


///////////////////////////////////////////////////////////////////////////////
// GetThaiCharClass
//
// Gets the ThaiCharClass for the given char. If the char is not a Thai or 
// Lao char, kThaiCharClassNON is returned.
//
ThaiCharClass GetThaiCharClass(Char c);




} // namespace Text

} // namespace EA


#endif // Header include guard






