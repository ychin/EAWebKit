///////////////////////////////////////////////////////////////////////////////
// EATextScriptArabic.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSCRIPTARABIC_H
#define EATEXT_INTERNAL_EATEXTSCRIPTARABIC_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{

namespace Text
{


/// ArabicGlyphTypeFlag
/// 
/// The values here are significant as they refer to OpenType flags.
///
enum ArabicGlyphTypeFlag
{
	kAGTFIsolated = 1,    /// a.k.a. Nominal glyph form.
	kAGTFFinal    = 2,    /// a.k.a. Right-joining glyph form. Both right-joining and dual-joining Chars may employ this form.
	kAGTFMedial   = 4,    /// a.k.a. Dual-joining glyph form. Only dual-joining Chars may employ this form.
	kAGTFInitial  = 8,    /// a.k.a. Left-joining glyph form. Both left-joining and dual-joining Chars may employ this form.
};


/// ArabicGlyphExclusionFlags
///
enum ArabicGlyphExclusionFlags
{
	kAGEFIsolated =                 kAGTFInitial | kAGTFMedial | kAGTFFinal,
	kAGEFInitial  = kAGTFIsolated                | kAGTFMedial | kAGTFFinal,
	kAGEFMedial   = kAGTFIsolated | kAGTFInitial               | kAGTFFinal,
	kAGEFFinal    = kAGTFIsolated | kAGTFInitial | kAGTFMedial,
};




} // namespace Text

} // namespace EA


#endif // Header include guard








