///////////////////////////////////////////////////////////////////////////////
// DemoTextBreak.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextBreak.h>
#include <EAStdC/EAString.h>


int MainDemoTextBreak(int, char**)
{
	using namespace EA::Text;
	using namespace EA::StdC;

	{
		// Demo line breaking
		// Iterates all the breakable opportunities as well as forced breaks in the line.

		const Char       *pTest = EATEXT_CHAR("a\x0300 b\x0301 c\x0302 d\x0303 efghijk");
		TextRun           tr(pTest, (uint32_t)Strlen(pTest));
		LineBreakIterator bi(&tr, 1);
		uint32_t          breakPos;

		do{
			breakPos = bi.GetNextLineBreak(kLineBreakTypeEmergency);
			// Do something with breakPos
		}while(breakPos != tr.mnTextSize);
	}

	{
		// Demo character breaking
		// Iterates all the word divisions.

		const Char            *pTest = EATEXT_CHAR("First line\nSecond line\nThird line");
		TextRun                tr(pTest, (uint32_t)Strlen(pTest));
		CharacterBreakIterator bi(&tr, 1);
		uint32_t               breakPos;

		do{
			breakPos = bi.GetNextCharBreak();
			// Do something with breakPos
		}while(breakPos != tr.mnTextSize);
	}

	{
		// Demo word breaking
		// Iterates all the word divisions.

		const Char        *pTest = EATEXT_CHAR("First line\nSecond line\nThird line");
		TextRun           tr(pTest, (uint32_t)Strlen(pTest));
		WordBreakIterator bi(&tr, 1);
		uint32_t          breakPos;

		do{
			breakPos = bi.GetNextWordBreak();
			// Do something with breakPos
		}while(breakPos != tr.mnTextSize);
	}

	{
		// Demo sentence breaking
		// Iterates all the sentence divisions.

		const Char            *pTest = EATEXT_CHAR("First sentence. Second sentence.\nThird sentence.");
		TextRun               tr(pTest, (uint32_t)Strlen(pTest));
		SentenceBreakIterator bi(&tr, 1);
		uint32_t              breakPos;

		do{
			breakPos = bi.GetNextSentenceBreak();
			// Do something with breakPos
		}while(breakPos != tr.mnTextSize);
	}

	{   
		// Demo sentence breaking
		// Iterates all the paragraph divisions.

		const Char            *pTest = EATEXT_CHAR("First sentence. Second sentence.\nThird sentence.");
		TextRun               tr(pTest, (uint32_t)Strlen(pTest));
		SentenceBreakIterator bi(&tr, 1);
		uint32_t              breakPos;

		do{
			breakPos = bi.GetNextSentenceBreak();
			// Do something with breakPos
		}while(breakPos != tr.mnTextSize);
	}

	return 0;
}


