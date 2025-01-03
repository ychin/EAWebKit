///////////////////////////////////////////////////////////////////////////////
// EATextScriptHangul.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSCRIPTHANGUL_H
#define EATEXT_INTERNAL_EATEXTSCRIPTHANGUL_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{

namespace Text
{

const unsigned kHangulBegin     = 0xAC00;   // Hangul range is from 0xAC00 - 0xD7A4
const unsigned kHangulCharCount = 11172;    // 0x2BA4

const unsigned kJamoBegin     = 0x1100;     // Jamo range is from 0x1100 to 0x1200
const unsigned kJamoCharCount = 256;        // 0x0100

const unsigned kJamoLBase = 0x1100;   // L == "leading consonant" , officially known as "choseong".
const unsigned kJamoVBase = 0x1161;   // V == "vowel"             , officially known as "jungseong".
const unsigned kJamoTBase = 0x11A7;   // T == "trailing consonant", officially known as "jongseong". Note that this char is one before the T set.

const unsigned kJamoLFiller = 0x115F;
const unsigned kJamoVFiller = 0x1160;

const unsigned kJamoLCount = 19;
const unsigned kJamoVCount = 21;
const unsigned kJamoTCount = 28;
const unsigned kJamoNCount = (kJamoVCount * kJamoTCount); // 588

const unsigned kHangulTone1 = 0x302E;   // Single tone. This char is a single dot to the left of a glyph.
const unsigned kHangulTone2 = 0x302F;   // Double tone. This char is a double dot (like a colon) to the left of a glyph.


inline bool IsCharHangulSyllable(Char c)
{
	return ((unsigned)c - 0xAC00u) < (0xD7A4u - 0xAC00u); // Faster way of saying (c >= 0xAC00 && c < 0xD7A4)
}


inline bool IsCharJamo(Char c)
{
	return ((unsigned)c - 0x1100u) < (0x1200u - 0x1100u); // Faster way of saying (c >= 0x1100 && c < 0x1200)
}





} // namespace Text

} // namespace EA


#endif // Header include guard








