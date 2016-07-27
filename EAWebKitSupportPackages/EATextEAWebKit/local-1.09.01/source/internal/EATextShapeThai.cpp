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
// EATextScriptThai.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The implementation here is strongly influenced by the document:
//     Thai Input and Output Methods
//     Translated from "Computer and Thai Language"
//     Translated by Chamrat Atsawaprecha
//     Feburary 12, 1992
//
// Modifications to accomodate the Unicode Standard 4.0 and modern layout
// issues have been made. Additionally, the TACTIS (Thai API Consortium/Thai
// Industrial Standard) is an influence, though mostly this influence is 
// referenced by the above-mentioned document.
//
// The variable names used here and the algorithms are taken more or less
// directly from the above-mentioned document.
//
///////////////////////////////////////////////////////////////////////////////

#include <EAText/EATextUnicode.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/EATextScript.h>
#include <EAText/internal/EATextScriptThai.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{

const size_t kMaxThaiCharClusterSize  = 32; // In reality, valid clusters are only as many as 3 chars.
const size_t kMaxThaiGlyphClusterSize = 32; // In reality, valid clusters are only as many as 3 glyphs unless there is some unusual stuff happening such as ligatures.


// We make some abbreviations so that the table below is more legible.
const uint8_t tcCTRL    = kThaiCharClassCTRL;
const uint8_t tcNON     = kThaiCharClassNON;
const uint8_t tcCONS    = kThaiCharClassCONS;
const uint8_t tcLV      = kThaiCharClassLV;
const uint8_t tcFV1     = kThaiCharClassFV1;
const uint8_t tcFV2     = kThaiCharClassFV2;
const uint8_t tcFV3     = kThaiCharClassFV3;
const uint8_t tcAM      = kThaiCharClassAM;
const uint8_t tcBV1     = kThaiCharClassBV1;
const uint8_t tcBV2     = kThaiCharClassBV2;
const uint8_t tcBD      = kThaiCharClassBD;
const uint8_t tcTONE    = kThaiCharClassTONE;
const uint8_t tcAD1     = kThaiCharClassAD1;
const uint8_t tcAD2     = kThaiCharClassAD2;
const uint8_t tcAD3     = kThaiCharClassAD3;
const uint8_t tcAD4     = kThaiCharClassAD4;
const uint8_t tcAV1     = kThaiCharClassAV1;
const uint8_t tcAV2     = kThaiCharClassAV2;
const uint8_t tcAV3     = kThaiCharClassAV3;
const uint8_t tcBCON    = kThaiCharClassBCON;


///////////////////////////////////////////////////////////////////////////////
// gThaiCharClassTable
//
// Defines character classes for characters in Thai and Lao scripts.
// This is implemented as uint8_t instead of ThaiCharClass in order to 
// save memory used in case the compiler devotes more than 8 bits per entry.
//
const uint8_t gThaiCharClassTable[256] = 
{
    // Thai
     tcNON, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS,  // 0x0E00
    tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS,  // 0x0E10
    tcCONS, tcCONS, tcCONS, tcCONS,  tcFV3, tcCONS,  tcFV3, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS,  tcNON,  // 0x0E20
     tcFV1,  tcAV2,  tcFV1,   tcAM,  tcAV1,  tcAV3,  tcAV2,  tcAV3,  tcBV1,  tcBV2,   tcBD,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  // 0x0E30
      tcLV,   tcLV,   tcLV,   tcLV,   tcLV,  tcFV2,  tcNON,  tcAD2, tcTONE, tcTONE, tcTONE, tcTONE,  tcAD1,  tcAD4,  tcAD3,  tcNON,  // 0x0E40
     tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON, tcCTRL,  // 0x0E50
    tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL,  // 0x0E60
    tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL,  // 0x0E70
    
    // Lao
     tcNON, tcCONS, tcCONS,  tcNON, tcCONS,  tcNON,  tcNON, tcCONS, tcCONS,  tcNON, tcCONS,  tcNON,  tcNON, tcCONS,  tcNON,  tcNON,  // 0x0E80
     tcNON,  tcNON,  tcNON,  tcNON, tcCONS, tcCONS, tcCONS, tcCONS,  tcNON, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS, tcCONS,  // 0x0E90
     tcNON, tcCONS, tcCONS, tcCONS,  tcNON, tcCONS,  tcNON, tcCONS,  tcNON,  tcNON, tcCONS, tcCONS,  tcNON, tcCONS, tcCONS,  tcNON,  // 0x0EA0
     tcFV1,  tcAV2,  tcFV1,   tcAM,  tcAV1,  tcAV3,  tcAV2,  tcAV3,  tcBV1,  tcBV2,  tcNON,  tcAV2, tcBCON,  tcFV3,  tcNON,  tcNON,  // 0x0EB0
      tcLV,   tcLV,   tcLV,   tcLV,   tcLV,  tcNON,  tcNON,  tcNON, tcTONE, tcTONE, tcTONE, tcTONE,  tcAD1,  tcAD4,  tcNON,  tcNON,  // 0x0EC0
     tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON,  tcNON, tcCONS, tcCONS,  tcNON, tcCTRL,  // 0x0ED0
    tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL,  // 0x0EE0
    tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL, tcCTRL   // 0x0EF0
};



///////////////////////////////////////////////////////////////////////////////
// ThaiCharLayoutFlags
//
// These are used to categorize characters for layout.
//
// See Section 4 of "Thai Input and Output Methods" for descriptions of 
// these classes.
//
enum ThaiCharLayoutFlag
{
    kThaiCharLayoutFlagNone = 0x0000,
    kThaiCharLayoutFlagNC   = 0x0001, // No-tail consonant
    kThaiCharLayoutFlagUC   = 0x0002, // Up-tail consonant
    kThaiCharLayoutFlagBC   = 0x0004, // Bot-tail consonant
    kThaiCharLayoutFlagSC   = 0x0008, // Split-tail consonant
    kThaiCharLayoutFlagAV   = 0x0010, // Above vowel
    kThaiCharLayoutFlagBV   = 0x0020, // Below vowel
    kThaiCharLayoutFlagTN   = 0x0040, // tfTN
    kThaiCharLayoutFlagAD   = 0x0080, // Above diacritic
    kThaiCharLayoutFlagBD   = 0x0100, // Below diacritic
    kThaiCharLayoutFlagAM   = 0x0200  // Sara Am
};


// We make some abbreviations so that the table below is more legible.
const uint16_t tfNo = kThaiCharLayoutFlagNone;
const uint16_t tfNC = kThaiCharLayoutFlagNC;
const uint16_t tfUC = kThaiCharLayoutFlagUC;
const uint16_t tfBC = kThaiCharLayoutFlagBC;
const uint16_t tfSC = kThaiCharLayoutFlagSC;
const uint16_t tfAV = kThaiCharLayoutFlagAV;
const uint16_t tfBV = kThaiCharLayoutFlagBV;
const uint16_t tfTN = kThaiCharLayoutFlagTN;
const uint16_t tfAD = kThaiCharLayoutFlagAD;
const uint16_t tfBD = kThaiCharLayoutFlagBD;
const uint16_t tfAM = kThaiCharLayoutFlagAM;



///////////////////////////////////////////////////////////////////////////////
// gThaiCharLayoutFlagsTable
//
// This is implemented as uint16_t instead of ThaiCharLayoutFlags in order to 
// save memory used in case the compiler devotes more than 16 bits per entry.
//
const uint16_t gThaiCharLayoutFlagsTable[256] = 
{
    // Thai
    tfNo, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfSC, tfBC, tfBC,  // 0x0E00
    tfSC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfNC, tfUC, tfNC, tfUC, tfNC, tfUC,  // 0x0E10
    tfNC, tfNC, tfNC, tfNC, tfNo, tfNC, tfNo, tfNC, tfNC, tfNC, tfNC, tfNC, tfUC, tfNC, tfNC, tfNo,  // 0x0E20
    tfNo, tfAV, tfNo, tfAM, tfAV, tfAV, tfAV, tfAV, tfBV, tfBV, tfBD, tfNo, tfNo, tfNo, tfNo, tfNo,  // 0x0E30
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfAD, tfTN, tfTN, tfTN, tfTN, tfAD, tfAD, tfAD, tfNo,  // 0x0E40
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo,  // 0x0E50
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo,  // 0x0E60
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo,  // 0x0E70
                                                                                                   
    // Lao                                                                                         
    tfNo, tfNC, tfNC, tfNo, tfNC, tfNo, tfNo, tfNC, tfNC, tfNo, tfNC, tfNo, tfNo, tfNC, tfNo, tfNo,  // 0x0E80
    tfNo, tfNo, tfNo, tfNo, tfNC, tfNC, tfNC, tfNC, tfNo, tfNC, tfNC, tfUC, tfNC, tfUC, tfNC, tfUC,  // 0x0E90
    tfNo, tfNC, tfUC, tfNC, tfNo, tfNC, tfNo, tfNC, tfNo, tfNo, tfNC, tfNC, tfNo, tfNC, tfNC, tfNo,  // 0x0EA0
    tfNo, tfAV, tfNo, tfAM, tfAV, tfAV, tfAV, tfAV, tfBV, tfBV, tfNo, tfAV, tfBD, tfNC, tfNo, tfNo,  // 0x0EB0
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfTN, tfTN, tfTN, tfTN, tfAD, tfAD, tfNo, tfNo,  // 0x0EC0
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNC, tfNC, tfNo, tfNo,  // 0x0ED0
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo,  // 0x0EE0
    tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo, tfNo   // 0x0EF0
};


///////////////////////////////////////////////////////////////////////////////
// gThaiCharPairingTable
//
// This is a table of ThaiCharClass/ThaiCharClass pairings and what their
// pairing means with respect to text editing and layout. The table refers
// to the pairing of char 1 with a succeeding char 2. Char 1 is represented
// by rows below, and char 2 is represented by columns.
//
// See Section 5.2 of "Thai Input and Output Methods" for descriptions of 
// these pairings and their meanings. Meanings are briefly described here:
//
//     ----------------------------------------------------------------------------------------------------
//     Value  Meaning                    Accept/Reject          Where to display if accepted
//                                  mode 0  mode 1  mode 2
//     ----------------------------------------------------------------------------------------------------
//     A      accept                accept  accept  accept      next display cell
//     C      accept (compose)      accept  accept  accept      same display cell as the leading character
//     S      strict mode reject    accept  accept  reject      next display cell
//     R      reject                accept  reject  reject      next display cell
//     X      accept (non-display)  accept  accept  accept      non-display
//
const char gThaiCharPairingTable[20][20] = 
{
   // CTRL NON  CONS LV   FV1  FV2  FV3  AM   BV1  BV2  BD   TONE AD1  AD2  AD3  AD4  AV1  AV2  AV3  BCON
    { 'X', 'A', 'A', 'A', 'A', 'A', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // CTRL
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // NON 
    { 'X', 'A', 'A', 'A', 'A', 'S', 'A', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, // CONS
    { 'X', 'S', 'A', 'S', 'S', 'S', 'S', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // LV  
    { 'X', 'A', 'A', 'A', 'A', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // FV1 
    { 'X', 'A', 'A', 'A', 'A', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // FV2 
    { 'X', 'A', 'A', 'A', 'S', 'A', 'S', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // FV3 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AM  
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'C', 'R', 'R', 'C', 'R', 'R', 'R', 'R' }, // BV1 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // BV2 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // BD  
    { 'X', 'A', 'A', 'A', 'A', 'A', 'A', 'C', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // TONE
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AD1 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AD2 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AD3 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AD4 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'C', 'R', 'R', 'C', 'R', 'R', 'R', 'R' }, // AV1 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' }, // AV2 
    { 'X', 'A', 'A', 'A', 'S', 'S', 'A', 'R', 'R', 'R', 'R', 'C', 'R', 'C', 'R', 'R', 'R', 'R', 'R', 'R' }, // AV3 
    { 'X', 'A', 'A', 'A', 'A', 'S', 'A', 'C', 'C', 'C', 'R', 'C', 'R', 'R', 'R', 'C', 'C', 'C', 'C', 'R' }  // BCON
};



///////////////////////////////////////////////////////////////////////////////
// gThaiCharTable
//
// Thai is unusual in that its decoration characters have multiple representations
// in TrueType fonts. The Unicode standard specifies that there is a single 
// Mai Ek (0x0E48) character. However, a TrueType font will have multiple 
// representations of that character, with the extra representations residing
// in the 'private use' space around 0xF700. 
//
// Our ThaiCharAdjustment table is used to specify the alternative characters
// in this table.
//
// We stuff the alternate characters (0xF7__) into the unused upper 32 entries 
// of the Thai block. Lao doesn't need the same thing.
//
const Char gThaiCharTable[256] = 
{
    // Thai
    0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,  // 0x0E00
    0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17, 0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,  // 0x0E10
    0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27, 0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,  // 0x0E20
    0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37, 0x0E38, 0x0E39, 0x0E3A,      0,      0,      0,      0, 0x0E3F,  // 0x0E30
    0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,  // 0x0E40
    0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57, 0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0xF718, 0xF719, 0xF71A,      0,  // 0x0E50
    0xF700, 0xF701, 0xF702, 0xF703, 0xF704, 0x2026, 0xF705, 0xF706, 0xF707, 0xF708, 0xF709, 0xF70A, 0xF70B, 0xF70C, 0xF70D, 0xF70E,  // 0x0E60
    0xF70F, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0xF710, 0xF711, 0xF712, 0xF713, 0xF714, 0xF715, 0xF716, 0xF717,  // 0x0E70
    
    // Lao
    0x0020, 0x0E81, 0x0E82,      0, 0x0E84,      0,      0, 0x0E87, 0x0E88,      0, 0x0E8A,      0,      0, 0x0E8D,      0,      0,  // 0x0E80
         0,      0,      0,      0, 0x0E94, 0x0E95, 0x0E96, 0x0E97,      0, 0x0E99, 0x0E9A, 0x0E9B, 0x0E9C, 0x0E9D, 0x0E9E, 0x0E9F,  // 0x0E90
         0, 0x0EA1, 0x0EA2, 0x0EA3,      0, 0x0EA5,      0, 0x0EA7,      0,      0, 0x0EAA, 0x0EAB,      0, 0x0EAD, 0x0EAE, 0x0EAF,  // 0x0EA0
    0x0EB0, 0x0EB1, 0x0EB2, 0x0EB3, 0x0EB4, 0x0EB5, 0x0EB6, 0x0EB7, 0x0EB8, 0x0EB9,      0, 0x0EBB, 0x0EBC, 0x0EBD,      0,      0,  // 0x0EB0
    0x0EC0, 0x0EC1, 0x0EC2, 0x0EC3, 0x0EC4,      0, 0x0EC6,      0, 0x0EC8, 0x0EC9, 0x0ECA, 0x0ECB, 0x0ECC, 0x0ECD,      0,      0,  // 0x0EC0
    0x0ED0, 0x0ED1, 0x0ED2, 0x0ED3, 0x0ED4, 0x0ED5, 0x0ED6, 0x0ED7, 0x0ED8, 0x0ED9,      0,      0, 0x0EDC, 0x0EDD,      0,      0   // 0x0ED0
};


///////////////////////////////////////////////////////////////////////////////
// ThaiCharAdjustment
//
// Thai is unusual in that its decoration characters have multiple representations
// in TrueType fonts. The Unicode standard specifies that there is a single 
// Mai Ek (0x0E48) character. However, a TrueType font will have multiple 
// representations of that character, with the extra representations residing
// in the 'private use' space around 0xF700. 
//
// ThaiCharAdjustment table is used to specify such alternative characters via 
// indexes into gThaiCharTable.
//
struct ThaiCharAdjustment
{
public:
    uint8_t mStart_TONE_AD;
    uint8_t mStart_AV;
    uint8_t mStart_BV_BD;
    uint8_t mStart_TailCutCons;

    uint8_t mShiftDown_TONE_AD[8];
    uint8_t mShiftDownLeft_TONE_AD[8];
    uint8_t mShiftLeft_TONE_AD[8];
    uint8_t mShiftLeft_AV[7];
    uint8_t mShiftDown_BV_BD[3];
    uint8_t mTailCutCons[4];

    uint8_t mAmComp[2];

public:
    uint8_t shiftdown_tone_ad(eastl_size_t i)     const { return mShiftDown_TONE_AD    [i - mStart_TONE_AD];     } 
    uint8_t shiftdownleft_tone_ad(eastl_size_t i) const { return mShiftDownLeft_TONE_AD[i - mStart_TONE_AD];     }
    uint8_t shiftleft_tone_ad(eastl_size_t i)     const { return mShiftLeft_TONE_AD    [i - mStart_TONE_AD];     }
    uint8_t shiftleft_av(eastl_size_t i)          const { return mShiftLeft_AV         [i - mStart_AV];          }
    uint8_t shiftdown_bv_bd(eastl_size_t i)       const { return mShiftDown_BV_BD      [i - mStart_BV_BD];       }
    uint8_t tailcutcons(eastl_size_t i)           const { return mTailCutCons          [i - mStart_TailCutCons]; }
};


static const ThaiCharAdjustment gThaiCharAdjustment = 
{
    0x47, 0x31, 0x38, 0x0D,
    { 0x47, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x4D, 0x4E },
    { 0x7A, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x79, 0x4E },
    { 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x79, 0x4E },
    { 0x78, 0xA0, 0xA0, 0x61, 0x62, 0x63, 0x64 },
    { 0x5C, 0x5D, 0x5E },
    { 0x70, 0xA0, 0xA0, 0x60 },
    { 0x4D, 0x32 }
};


static const ThaiCharAdjustment gLaoCharAdjustment = 
{
    0xC7, 0xB1, 0xB8, 0x8D,
    { 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE },
    { 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE },
    { 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE },
    { 0xB1, 0xA0, 0xA0, 0xB4, 0xB5, 0xB6, 0xB7 },
    { 0xB8, 0xB9, 0xA0 },
    { 0x8D, 0xA0, 0xA0, 0x90 },
    { 0xCD, 0xB2 }
};


inline eastl_size_t GetThaiTableIndex(Char c)
{
    return (c - 0x0E00);
}


inline bool IsCharThaiLao(Char c)
{
    return ((unsigned)c - 0x0E00u) < (0x0F00u - 0x0E00u); // Faster way of saying (c >= 0x0E00 && c < 0x0F00)
}


inline ThaiCharClass GetThaiCharClass(Char c)
{
    return (IsCharThaiLao(c) ? (ThaiCharClass)gThaiCharClassTable[c - 0x0E00] : kThaiCharClassNON);
}


inline ThaiCharLayoutFlag GetThaiCharLayoutFlag(Char c)
{
    // To consider: We can probably remove the IsCharThaiLao check if we can guarantee 
    // the input char is always Thai/Lao.
    // return (IsCharThaiLao(c) ? (ThaiCharClass)gThaiCharLayoutFlagsTable[c - 0x0E00] : kThaiCharLayoutFlagNone);

    EA_ASSERT(IsCharThaiLao(c));
    return (ThaiCharLayoutFlag)gThaiCharLayoutFlagsTable[c - 0x0E00];
}


inline bool IsThaiCharLayoutFlag(Char c, unsigned flags)
{
    // To consider: We can probably remove the IsCharThaiLao check if we can guarantee 
    // the input char is always Thai/Lao.
    // return (IsCharThaiLao(c) ? ((gThaiCharLayoutFlagsTable[c - 0x0E00] & flags) != 0) : false);

    EA_ASSERT(IsCharThaiLao(c));
    return ((gThaiCharLayoutFlagsTable[c - 0x0E00] & flags) != 0);
}


inline char GetThaiCharPairingResult(Char c0, Char c1)
{
    return gThaiCharPairingTable[GetThaiCharClass(c0)][GetThaiCharClass(c1)];
}


///////////////////////////////////////////////////////////////////////////////
// GetThaiGlyphs
//
eastl_size_t Typesetter::GetThaiGlyphs(eastl_size_t i, const Char* pChar, eastl_size_t clusterSize, GlyphId* pGlyphIdArray)
{
    // The 'i' argument refers to the position of pChar in mLineLayout.mCharArray. 
    // There is a possibility that pChar doesn't point to mLineLayout.mCharArray[i],
    // as we may be doing some kind of implicit substitution. Thus we have pChar 
    // as an explicit parameter.

    eastl_size_t glyphCount = 0;
    const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[i];
    const ThaiCharAdjustment* const pTable = IsCharThai(*pChar) ? &gThaiCharAdjustment : &gLaoCharAdjustment;

    switch (clusterSize)
    {
        case 1:
        {
            // Any kind of decoration character must be preceded by a base character.
            if(IsThaiCharLayoutFlag(pChar[0], tfAV | tfBV | tfTN | tfAD | tfBD | tfAM))
            {
                // We prepend a character which is a circle implemented with dotted lines. 
                // It is the Unicode standard for generic base character to be represented
                // by such a dotted circle. It is a defacto standard that we prepend such 
                // a character when none is provided. It makes the text look more sensible.
                const Char cDottedCircle = 0x25CC; // If we can't use 0x25CC, we ought to try a space char.
                GetGlyphsForChar(&cDottedCircle, 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount, EATEXT_CHAR("\x25CB _o"), 4);
            }

            GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);

            break;
        }

        case 2:
        {
            if(IsThaiCharLayoutFlag(pChar[0], tfNC | tfBC | tfSC) &&
               IsThaiCharLayoutFlag(pChar[1], tfAM))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[0]],          1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[1]],          1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAM))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                   1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_tone_ad(pTable->mAmComp[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[1]],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfNC | tfBC | tfSC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAV))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfNC | tfBC | tfSC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_tone_ad(GetThaiTableIndex(pChar[1]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAV))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                       1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_av(GetThaiTableIndex(pChar[1]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                                1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdownleft_tone_ad(GetThaiTableIndex(pChar[1]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfNC | tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV | tfBD))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfBC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV | tfBD))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                          1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_bv_bd(GetThaiTableIndex(pChar[1]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfSC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV | tfBD))
            {
                GetGlyphsForChar(&gThaiCharTable[pTable->tailcutcons(GetThaiTableIndex(pChar[0]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])],                      1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else
            {
                // See comments above regarding char 0x25CC. 
                const Char cDottedCircle = 0x25CC; // If we can't use 0x25CC, we ought to try a space char.
                GetGlyphsForChar(&cDottedCircle,                               1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount, EATEXT_CHAR("\x25CB _o"), 4);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }

            break;
        }

        case 3:
        {
            if(IsThaiCharLayoutFlag(pChar[0], tfNC | tfBC | tfSC) &&
               IsThaiCharLayoutFlag(pChar[1], tfTN) &&
               IsThaiCharLayoutFlag(pChar[2], tfAM))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],  1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[0]],           1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])],  1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[1]],           1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfTN) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAM))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_tone_ad(pTable->mAmComp[0])],          1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_tone_ad(GetThaiTableIndex(pChar[1]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->mAmComp[1]],                                     1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfAV) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_av(GetThaiTableIndex(pChar[1]))],      1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftleft_tone_ad(GetThaiTableIndex(pChar[2]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfUC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                                1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])],                                1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdownleft_tone_ad(GetThaiTableIndex(pChar[2]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfNC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_tone_ad(GetThaiTableIndex(pChar[2]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfSC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[pTable->tailcutcons(GetThaiTableIndex(pChar[0]))],       1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[1])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_tone_ad(GetThaiTableIndex(pChar[2]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else if(IsThaiCharLayoutFlag(pChar[0], tfBC) &&
                    IsThaiCharLayoutFlag(pChar[1], tfBV) &&
                    IsThaiCharLayoutFlag(pChar[2], tfAD | tfTN))
            {
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[0])],                            1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_bv_bd(GetThaiTableIndex(pChar[1]))],   1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
                GetGlyphsForChar(&gThaiCharTable[pTable->shiftdown_tone_ad(GetThaiTableIndex(pChar[2]))], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
            else
            {
                for(eastl_size_t j = 0; j < 3; j++)
                    GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[j])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);
            }
        }
        break;

        case 4:
        default:
        {
            // Call ourselves recursively for the first three glyphs, then manually add on
            // any trailing glyphs without any selection/placement intelligence.
            glyphCount = GetThaiGlyphs(i, pChar, 3, pGlyphIdArray);

            for(eastl_size_t j = 3; j < clusterSize; j++)
                GetGlyphsForChar(&gThaiCharTable[GetThaiTableIndex(pChar[j])], 1, pAnalysisInfo, pGlyphIdArray + glyphCount, glyphCount);

            break;
        }
    }

    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// AppendThaiCharCluster
//
eastl_size_t Typesetter::AppendThaiCharCluster(eastl_size_t iCharBegin, eastl_size_t charCount, 
                                                 const Char* pCharCluster, eastl_size_t clusterSize)
{
    // The 'iCharBegin' argument refers to the position of pCharCluster in mLineLayout.mCharArray. 
    // There is a possibility that pCharCluster doesn't point to mLineLayout.mCharArray[iCharBegin],
    // as we may be doing some kind of implicit substitution. Thus we have pCharCluster 
    // as an explicit parameter.

    const AnalysisInfo* const pAnalysisInfo = &mLineLayout.mAnalysisInfoArray[iCharBegin];
    GlyphId                   pGlyphIdArray[kMaxThaiGlyphClusterSize];
    eastl_size_t              glyphCount = 0;

    EA_ASSERT(clusterSize && ((iCharBegin + clusterSize) <= mLineLayout.mCharArray.size()));

    if(IsCharThaiLao(*pCharCluster)) // If the cluster is using Thai or Lao script...
        glyphCount = GetThaiGlyphs(iCharBegin, pCharCluster, clusterSize, pGlyphIdArray);
    else
    {
        // In this case we choose glyphs as if doing general glyph shaping. 
        // We just convert the Unicode Chars to glyphs.
        for(eastl_size_t i = 0; i < clusterSize; )
            i += GetGlyphsForChar(pCharCluster + i, clusterSize - i, &mLineLayout.mAnalysisInfoArray[i], pGlyphIdArray + glyphCount, glyphCount);
    }

    AppendGeneralGlyphCluster(iCharBegin, charCount, pCharCluster, charCount, 
                                pGlyphIdArray, glyphCount, pAnalysisInfo->mnBidiLevel);

    PlaceGeneralGlyphCluster(iCharBegin, charCount);

    // As it stands now, all chars must result in at least one glyph, though that 
    // one glyph could be a zero-width space character. Note that the render code
    // has the opportunity to remove such a "no-op" instruction from the display list.
    EA_ASSERT(glyphCount > 0);
    return glyphCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetThaiCharCluster
//
// The return value is the number of chars eaten; clusterSize is the number of 
// chars generated. In the large majority of cases, they will be equal.
//
eastl_size_t Typesetter::GetThaiCharCluster(eastl_size_t i, eastl_size_t iCharEnd, 
                                              Char* pCharCluster, eastl_size_t& clusterSize)
{
    EA_ASSERT((i < iCharEnd) && (iCharEnd <= mLineLayout.mCharArray.size()));

    Script clusterScript = kScriptUnknown;
    Char   cPrev = 0;

    clusterSize = 0;

    for(const Char* p = &mLineLayout.mCharArray[i], *pEnd = &mLineLayout.mCharArray[iCharEnd]; 
            (p < pEnd) && (clusterSize < kMaxThaiCharClusterSize); ++p)
    {
        const Char   c      = *p;
        const Script script = GetScriptFromChar(c);

        EA_ASSERT(script != kScriptUnknown);
        if(clusterScript == kScriptUnknown)
           clusterScript = script;

        if((script == clusterScript) && ((clusterSize == 0) || (GetThaiCharPairingResult(cPrev, c) == 'C'))) // If this is the first char of the cluster or is a composed char with the previous char(s)...
        {
            pCharCluster[clusterSize++] = c;
            cPrev = c;
        }
        else
            break;
    }

    EA_ASSERT(clusterSize > 0);
    return clusterSize;
}


///////////////////////////////////////////////////////////////////////////////
// ShapeThai
//
// Thai is one of the most complicated of the scripts to shape. It involves
// a number of glyph substitution scenarios that can be dealt only by 
// table-based approaches.
//
void Typesetter::ShapeThai(eastl_size_t iCharBegin, eastl_size_t iCharEnd)
{
    EA_ASSERT(iCharEnd <= mLineLayout.mCharArray.size());

    Char         pCharCluster[kMaxThaiCharClusterSize];
    eastl_size_t clusterSize;
    eastl_size_t charCount;

    for(eastl_size_t i = iCharBegin; i < iCharEnd; i += charCount)
    {
        #ifdef EA_DEBUG
            memset(pCharCluster, 0, sizeof(pCharCluster));
        #endif

        charCount = GetThaiCharCluster(i, iCharEnd, pCharCluster, clusterSize);
        AppendThaiCharCluster(i, charCount, pCharCluster, clusterSize);
    }
}


} // namespace Text

} // namespace EA





















