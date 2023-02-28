///////////////////////////////////////////////////////////////////////////////
// DemoUnicode.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextUnicode.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////
// MainDemoUnicode
//
// This function demonstrates some basic usage of the Unicode module within
// EAText. Most of the Unicode module is stuff that's used internally by EAText
// and the EAText Typesetter class in particular. But some functions in the 
// Unicode module may be useful nevertheless.
//
int MainDemoUnicode(int, char**)
{
	using namespace EA::Text;

	{
		// Example usage of Unicode byte order marks.
		const char text[32] = "blah blah file data";

		if(strncmp(text, kBOM_UTF8, strlen(kBOM_UTF8)) == 0) // There is also kBOM_UTF16_BE, kBOM_UTF16_LE, kBOM_UTF32_BE, kBOM_UTF32_LE.
		{
			// pText is UTF8 encoded.
		}
	}


	{
		// Example usage of IsSpace

		bool bNonBreakingSpace       = IsSpace(' ', kSTNoBreak);
		bool bZeroWidthAndBreakable  = IsSpace(' ', kSTZeroWidth | kSTBreak);
		bool bIsCRuntimeLibrarySpace = IsSpace(' ', kSTWidth | kSTControl);
		bool bIsAnySpace             = IsSpace(' ', kSTAll, false);

		(void)bNonBreakingSpace; (void)bZeroWidthAndBreakable; (void)bIsCRuntimeLibrarySpace; (void)bIsAnySpace;
	}


	{
		// Example usage of GetCharBlock

		Block block = GetCharBlock((Char)0x3045);

		if(block == kBlockHiragana)
		{
			// The char is Japanese Hiragana.
		}

		(void)block;
	}


	{
		// Example usage of GetMirrorChar

		Char c1 = GetMirrorChar('[');   // Returns ']'
		Char c2 = GetMirrorChar('a');   // Returns 'a'
		Char c3 = GetMirrorChar('$');   // Returns '$'

		(void)c1; (void)c2; (void)c3;
	}


	{
		// Example usage of IsCharUppercase

		bool b1 = IsCharUppercase('A'); // Returns true
		bool b2 = IsCharUppercase('a'); // Returns false
		bool b3 = IsCharUppercase('$'); // Returns false

		(void)b1; (void)b2; (void)b3;
	}


	{
		// Example of IsCharSTerm (sentence termination)

		bool b1 = IsCharSTerm('!');     // Returns true
		bool b2 = IsCharSTerm(' ');     // Returns false
		bool b3 = IsCharSTerm('.');     // Returns true
		bool b4 = IsCharSTerm('a');     // Returns false
		bool b5 = IsCharSTerm(0xFF61);  // Returns true (Japanese period).

		(void)b1; (void)b2; (void)b3; (void)b4; (void)b5;
	}

	return 0;
}


