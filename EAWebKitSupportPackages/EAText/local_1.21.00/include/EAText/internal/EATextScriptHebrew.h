///////////////////////////////////////////////////////////////////////////////
// EATextScriptHebrew.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSCRIPTHEBREW_H
#define EATEXT_INTERNAL_EATEXTSCRIPTHEBREW_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{

namespace Text
{



///////////////////////////////////////////////////////////////////////////////
// IsCharHebrew
//
// This is a fast alternative to (GetScriptFromChar(c) == kScriptHebrew).
//
// This comparison checks the entire Hebrew block, though actually some 
// chars in the Hebrew block aren't assigned. Thus this is more of a check
// for the Hebrew block and not Hebrew script. The large majority of the 
// time checking for the block is sufficient; and it's much faster.
//
inline bool IsCharHebrew(Char c)
{
	return ((unsigned)c - 0x0590u) < (0x0600u - 0x0590u); // Faster way of saying (c >= 0x0590 && c < 0x0600)
}



} // namespace Text

} // namespace EA


#endif // Header include guard








