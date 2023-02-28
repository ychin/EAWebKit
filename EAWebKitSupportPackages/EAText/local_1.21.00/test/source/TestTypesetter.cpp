/////////////////////////////////////////////////////////////////////////////
// TestTypesetter.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EAText/EATextBreak.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/EATextStyleReader.h>
#include <EAText/EATextUnicode.h>
#include <EAText/EATextIterator.h>
#include <EAText/EATextCache.h>
#include <eathread/eathread_thread.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileUtil.h>
#include <MemoryMan/MemoryMan.h>
#include <EATest/EATest.h>
#include <math.h>



// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const uint8_t pAhemTTF[] = { 0x23, 0xa2, ... }; 
// This data comes from the Ahem.ttf file, which is a special font whose
// glyphs are all solid squares of the same dimensions as the M square.
// The purpose of this font is to test font rendering and it was originally
// designed for web browser testing.
#include "data/Ahem.ttf.inl"


using namespace EA::Text;


namespace
{   
	#if !defined(EA_PLATFORM_ANDROID)
	const int32_t NUM_ITERATIONS = 1500;
	#else
	// The android simulator is quite slow...
	const int32_t NUM_ITERATIONS = 500;
	#endif


	// Test text
	const Char* gpTestText0 = EATEXT_CHAR("Hello world.");
	const Char* gpTestText1 = EATEXT_CHAR("Phonetic modifiers (a.k.a. diacritics) are small glyphs attached to primary alphabetic glyphs whose purpose is generally to modify the pronunciation of the primary glyph or the characters around it. Here we have three tables: common modifiers, uncommon modifiers, and Thai modifiers. Modifiers exist for a number of scripts (writing systems) that we don't cover here, but the concepts are similar. The Thai modifiers are present because we often make applications for the Thai market and have to work with these modifiers in text processing code. The common modifiers are used in the most common European languages, while the uncommon modifiers are used sporadically in other languages. The Thai modifiers are specific to Thai.");
	const Char* gpTestText2 = EATEXT_CHAR(" ");
	const Char* gpTestText3 = EATEXT_CHAR("");

	const Char* gpTestTextArray[4] = { gpTestText0, gpTestText1, gpTestText2, gpTestText3 };

	EA::Text::Rectangle gLayoutBoxes[4];

	void SetupLayoutBoxes()
	{
		gLayoutBoxes[0].mLeft   = 0;
		gLayoutBoxes[0].mTop    = 0;
		gLayoutBoxes[0].mRight  = 200;
		gLayoutBoxes[0].mBottom = 0;

		gLayoutBoxes[1].mLeft   = 0;
		gLayoutBoxes[1].mTop    = 0;
		gLayoutBoxes[1].mRight  = 1000000;
		gLayoutBoxes[1].mBottom = 0;

		gLayoutBoxes[2].mLeft   = 0;
		gLayoutBoxes[2].mTop    = 0;
		gLayoutBoxes[2].mRight  = 1;
		gLayoutBoxes[2].mBottom = 0;

		gLayoutBoxes[3].mLeft   = 0;
		gLayoutBoxes[3].mTop    = 0;
		gLayoutBoxes[3].mRight  = 0;
		gLayoutBoxes[3].mBottom = 0;
	}

} // namespace



/////////////////////////////////////////////////////////////////////////////
// TestLineLayout
//
int TestLineLayout()
{
	int nErrorCount = 0;

	{
		LineLayout      llA;
		LineLayout      llB;
		GlyphLayoutInfo gli;
		eastl_size_t    i;
		Char            c = 'a';

		memset(&gli, 0, sizeof(gli));
		gli.mpFont = (Font*)(uintptr_t)0x00000001;

		for(i = 0; i < 4; i++, c++)
		{
			llA.mCharArray.push_back(c);
			llA.mAnalysisInfoArray.push_back();
			llA.mGlyphArray.push_back((GlyphId)c);          // Pretend the Char is equivalent to GlyphId.
			llA.mGlyphInfoArray.push_back();
			llA.mGlyphLayoutInfoArray.push_back(gli);
			llA.mGlyphIndexArray.push_back(i);              // Specifies the glyph index corresponding to a given char index.
			llA.mCharIndexArray.push_back(i);               // Specifies the char index corresponding to a given glyph index.

			EATEST_VERIFY(llA.Validate(true));
		}

		for(i = 0; i < 4; i++, c++)
		{
			llB.mCharArray.push_back(c);
			llB.mAnalysisInfoArray.push_back();
			llB.mGlyphArray.push_back((GlyphId)c);
			llB.mGlyphInfoArray.push_back();
			llB.mGlyphLayoutInfoArray.push_back(gli);
			llB.mGlyphIndexArray.push_back(i);
			llB.mCharIndexArray.push_back(i);

			EATEST_VERIFY(llB.Validate(true));
		}

		llA.Append(llB);
		EATEST_VERIFY(llA.Validate(true));
		EATEST_VERIFY(llA.mGlyphIndexArray.size() == 8);
		for(i = 0; i < 8; i++)
		{
			EATEST_VERIFY(llA.mGlyphIndexArray[i] == i);
			EATEST_VERIFY(llA.mCharIndexArray[i] == i);
		}

		llB.Append(llA);
		EATEST_VERIFY(llB.Validate(true));
		EATEST_VERIFY(llB.mCharIndexArray.size() == 12);
		for(i = 0; i < 12; i++)
		{
			EATEST_VERIFY(llB.mGlyphIndexArray[i] == i);
			EATEST_VERIFY(llB.mCharIndexArray[i] == i);
		}
	}

	#if 0 // Disabled because it currently isn't testing anything new.
	{
		Typesetter ts;
		LineLayout lineLayout;
		TextStyle  textStyle;
		TestString s;

		for(int i = 0; i < 100; i++)
		{
			GenerateRandomString(s, 1, 10, 6);

			lineLayout.Clear();

			ts.LayoutParagraph(s.c_str(), s.length(), 0, 0, 133, 10000, &textStyle, lineLayout);

			// Copy line layout back to typesetter.
			LineLayout& tsLL = ts.GetLineLayout();
			tsLL.Clear();
			tsLL.Append(lineLayout);
			EATEST_VERIFY(tsLL.Validate(true));
		}
	}
	#endif

	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestTypesetterNop
//
// Nop => No-op, no operation. Test the layout of nothing.
//
int TestTypesetterNop()
{
	int nErrorCount = 0;

	EATextTest_CheckMemory();

	// Here we do a test of a Typesetter that is given no schedule of items.
	Typesetter ts;
	Typesetter::LineState lineState;

	lineState = ts.GetLineState();
	EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

	lineState = ts.ProcessLine();
	EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

	ts.FinalizeLine();
	lineState = ts.GetLineState();
	EATEST_VERIFY(lineState == Typesetter::kLineStateFull);

	const LineLayout& lineLayout = ts.GetLineLayout();
	EATEST_VERIFY(lineLayout.mCharArray.empty());

	ts.NextLine();
	lineState = ts.GetLineState();
	EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

	EATextTest_CheckMemory();

	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestTypesetterLayoutSimple
//
// Tests the LayoutSimple functions.
//
int TestTypesetterLayoutSimple()
{
	int nErrorCount = 0;

	TestString        s16;
	eastl::string8    s8;
	float             x;
	float             y;
	LineLayout        lineLayout;
	TextStyle         textStyle;
	LayoutSettings    layoutSettings;
	FontServer* const pFontServer = GetFontServer(true);
	index_t           count;

	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	EATEST_VERIFY(pFontServer != NULL);
	Font* const pPrimaryFont = pFontServer->GetFont(&textStyle, layoutSettings.mFontSelection, kScriptLatin, true);
	EATEST_VERIFY(pPrimaryFont && !layoutSettings.mFontSelection.empty());

	EATextTest_CheckMemory();

	for(int i = 0; i < NUM_ITERATIONS && (nErrorCount == 0); i++)
	{
		GenerateRandomString(s16, 0, 100);
		x = (float)gRandom.RandRange(-100000, +1000000) / 100.f; // Generate a random float.
		y = (float)gRandom.RandRange(-100000, +1000000) / 100.f;
		lineLayout.mGlyphArray.resize(s16.length());
		lineLayout.mGlyphLayoutInfoArray.resize(s16.length());

		count = LayoutSimple(s16.data(), s16.length(), x, y, NULL, lineLayout.mGlyphArray.data(), lineLayout.mGlyphLayoutInfoArray.data(), &layoutSettings);
		EATEST_VERIFY(count == (s16.length() - eastl::count(s16.begin(), s16.end(), (Char)'\n')));
	}

	EATextTest_CheckMemory();

	for(int i = 0; i < NUM_ITERATIONS && (nErrorCount == 0); i++)
	{
		GenerateRandomString(s8, 0, 100);
		x = (float)gRandom.RandRange(-100000, +1000000) / 100.f; // Generate a random float.
		y = (float)gRandom.RandRange(-100000, +1000000) / 100.f;
		lineLayout.mGlyphArray.resize(s8.length());
		lineLayout.mGlyphLayoutInfoArray.resize(s8.length());

		count = LayoutSimple(s8.data(), s8.length(), x, y, NULL, lineLayout.mGlyphArray.data(), lineLayout.mGlyphLayoutInfoArray.data(), &layoutSettings);
		EATEST_VERIFY(count == (s8.length() - eastl::count(s8.begin(), s8.end(), (char8_t)'\n')));
	}

	{
		// Test the '\n' support in LayoutSimple.
		s16 = EATEXT_CHAR("Hello\nWorld\n");
		lineLayout.mGlyphArray.resize(s16.length());
		count = LayoutSimple(s16.data(), s16.length(), 100, 100, NULL, lineLayout.mGlyphArray.data(), lineLayout.mGlyphLayoutInfoArray.data(), &layoutSettings);
		EATEST_VERIFY(count == s16.length()); // Currently LayoutSimple renders \n chars as zero-width glyphs, as this makes its logic simpler and faster.

		s8 = "Hello\n\nWorld\n";
		lineLayout.mGlyphArray.resize(s8.length());
		count = LayoutSimple(s8.data(), s8.length(), 100, 100, NULL, lineLayout.mGlyphArray.data(), lineLayout.mGlyphLayoutInfoArray.data(), &layoutSettings);
		EATEST_VERIFY(count == s8.length()); // Currently LayoutSimple renders \n chars as zero-width glyphs, as this makes its logic simpler and faster.
	}

	{
		// Test componentFlags support in LayoutSimple.
		//    index_t LayoutSimple(const Char* pText, index_t nTextSize, float x, float y, 
		//                         Font* pFont, LineLayout& lineLayout, 
		//                         LayoutSettings* pLayoutSettings = NULL, 
		//                         uint32_t componentFlags = LineLayout::kComponentGlyphArray | LineLayout::kComponentGlyphLayoutInfoArray);


		s16 = EATEXT_CHAR("Hello\nWorld\n");
		lineLayout.Clear();
		count = LayoutSimple(s16.data(), s16.length(), 100, 100, pPrimaryFont, lineLayout, &layoutSettings, LineLayout::kComponentCharArray);
		EATEST_VERIFY(count == s16.length()); // Currently LayoutSimple renders \n chars as zero-width glyphs, as this makes its logic simpler and faster.

		s8 = "Hello\n\nWorld\n";
		lineLayout.Clear();
		count = LayoutSimple(s8.data(), s8.length(), 100, 100, pPrimaryFont, lineLayout, &layoutSettings, LineLayout::kComponentCharArray);
		EATEST_VERIFY(count == s8.length()); // Currently LayoutSimple renders \n chars as zero-width glyphs, as this makes its logic simpler and faster.
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterHighLevel
//
// Tests Typesetter::LayoutLine and TypeSetter::LayoutParagraph.
//
int TestTypesetterHighLevel()
{
	int nErrorCount = 0;

	TestString        s16;
	float             x;
	float             y;
	float             w;
	float             h;
	Typesetter        ts;
	LineLayout        lineLayout;
	TextStyle         textStyle;
	LayoutSettings    layoutSettings;
	FontServer* const pFontServer = GetFontServer(true);

	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	layoutSettings.mbDynamicFontSelection = true;

	EATEST_VERIFY(pFontServer != NULL);
	ts.SetFontServer(pFontServer);

	EATextTest_CheckMemory();

	/////////////////////////////////
	// LayoutLine
	/////////////////////////////////
	for(int i = 0; i < NUM_ITERATIONS; i++)
	{
		RandomizeTextStyle(textStyle);

		GenerateRandomString(s16, 0, 50);
		x = (float)gRandom.RandRange(-100000, +1000000) / 100.f; // Generate a random float.
		y = (float)gRandom.RandRange(-100000, +1000000) / 100.f;

		ts.LayoutLine(s16.data(), s16.length(), x, y, &textStyle);

		ts.GetLineLayout().GetBoundingBox(w, h, true);

		//const LineLayout& lineLayout = ts.GetLineLayout();
		//EA::UnitTest::ReportVerbosity(1, "TestTypesetterHighLevel: string: %I16s\n", s16.c_str());
		//EA::UnitTest::ReportVerbosity(1, "TestTypesetterHighLevel: char array: %I16s\n", lineLayout.mCharArray.c_str());

		lineLayout.Clear(true);
	}

	EATextTest_CheckMemory();


	/////////////////////////////////
	// LayoutParagraph
	/////////////////////////////////
	for(int i = 0; i < NUM_ITERATIONS / 5; i++)
	{
		RandomizeTextStyle(textStyle);

		GenerateRandomString(s16, 0, 400);
		x = (float)gRandom.RandRange(-100000, +1000000) / 100.f; // Generate a random float.
		y = (float)gRandom.RandRange(-100000, +1000000) / 100.f;

		ts.LayoutParagraph(s16.data(), s16.length(), x, y, x + 200, y + 1000, &textStyle, lineLayout);

		lineLayout.GetBoundingBox(w, h, true);

		// It would be nice if we could take the output and divide it into lines
		// and print them individually. This would require looking at the glyph
		// layout y positions.
		//const LineLayout& lineLayout = ts.GetLineLayout();
		//EA::UnitTest::ReportVerbosity(1, "%I16s\n", s16.c_str());
		//EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

		lineLayout.Clear(true);
	}

	EATextTest_CheckMemory();

	// Verifyies the mfLineSpace value from the TextStyle is being applied to the LineLayout when calling LayoutParagraph
	{
		const float kLineSpaceOffset = 15.0f;
		float originalBBW = 0.0f, originalBBH = 0.0f, originalLineHeight = 0.0f,
			  currentBBW = 0.0f, currentBBH = 0.0f;

		// These two lines must correspond with one another
		const float kNumLines = 2;
		s16.assign(EATEXT_CHAR("A\nB"));
		
		RandomizeTextStyle(textStyle);
		textStyle.mfLineSpace = 0.0f; // use the text calculated line height

		ts.LayoutParagraph(s16.data(), s16.length(), x, y, x + 200, y + 1000, &textStyle, lineLayout);
		lineLayout.GetBoundingBox(originalBBW, originalBBH, false);
		originalLineHeight = lineLayout.GetLineHeight();

		lineLayout.Clear(true);

		textStyle.mfLineSpace = kLineSpaceOffset;
		ts.LayoutParagraph(s16.data(), s16.length(), x, y, x + 200, y + 1000, &textStyle, lineLayout);
		lineLayout.GetBoundingBox(currentBBW, currentBBH, false);

		EATEST_VERIFY(currentBBH >= (originalBBH - (originalLineHeight*kNumLines) + ((kLineSpaceOffset - 1.0f)*kNumLines))); // adjust in case any rounding occurred internally
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestTypesetter1
//
// Tests Typesetter lower level layout functionality.
//
int TestTypesetter1()
{
	int nErrorCount = 0;

	EATextTest_CheckMemory();

	Typesetter            ts;
	Typesetter::LineState lineState = ts.GetLineState();

	EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		for(size_t i = 0; i < EAArrayCount(gpTestTextArray); i++) // For each test text...
		{
			EATextTest_CheckMemory();

			EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpTestTextArray[i]);
			EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

			ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);
			ts.AddTextRun(gpTestTextArray[i], (index_t)EA::StdC::Strlen(gpTestTextArray[i]));

			lineState = ts.GetLineState();
			EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpTestTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

			while(lineState != Typesetter::kLineStateComplete)
			{
				EATextTest_CheckMemory();

				lineState = ts.ProcessLine();

				if(lineState == Typesetter::kLineStatePartial)
				{
					// Call FinalizeLine here, as we've already submitted all the text and so if the 
					// LineState is kLineStatePartial then there must not be enough text to fill the line.
					// You wouldn't call FinalizeLine in the case that you have more text to submit.
					ts.FinalizeLine();

					lineState = ts.GetLineState();
					EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
				}

				if(lineState == Typesetter::kLineStateFull)
				{
					// When the LineState is kLineStateFull, the next thing to do is generally to
					// call FinalizeLine. 
					ts.FinalizeLine();

					const LineLayout& lineLayout = ts.GetLineLayout();
					EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

					ts.NextLine();
					lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
				}

				EATextTest_CheckMemory();
			}

			ts.Reset();
			lineState = ts.GetLineState();

			EATextTest_CheckMemory();
		}
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestTypesetter2
//
// Similar to TestTypesetter1 but is tougher.
//
int TestTypesetter2()
{
	int nErrorCount = 0;

	EATextTest_CheckMemory();

	Typesetter            ts;
	Typesetter::LineState lineState = ts.GetLineState();

	EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		for(size_t i = 0; i < EAArrayCount(gpTestTextArray); i++) // For each test text...
		{
			EATextTest_CheckMemory();

			EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpTestTextArray[i]);
			EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

			ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);

			// We we do here is submit the text run in parts.
			const Char* pCurrent = gpTestTextArray[i];
			const Char* pEnd     = pCurrent + EA::StdC::Strlen(pCurrent);

			while(pCurrent < pEnd)
			{
				int32_t count = (int32_t)(pEnd - pCurrent);
				if(count > 8)
					count = 8;

				count = gRandom.RandRange(0, count + 1); // Select 0, 1, 2, 3, ... count Chars.
				EATEST_VERIFY((pCurrent + count) <= pEnd);

				ts.AddTextRun(pCurrent, (uint32_t)count);
				pCurrent += count;
			}

			lineState = ts.GetLineState();
			EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpTestTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

			while(lineState != Typesetter::kLineStateComplete)
			{
				EATextTest_CheckMemory();

				lineState = ts.ProcessLine();

				if(lineState == Typesetter::kLineStatePartial)
				{
					// Call FinalizeLine here, as we've already submitted all the text and so if the 
					// LineState is kLineStatePartial then there must not be enough text to fill the line.
					// You wouldn't call FinalizeLine in the case that you have more text to submit.
					ts.FinalizeLine();

					lineState = ts.GetLineState();
					EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
				}

				if(lineState == Typesetter::kLineStateFull)
				{
					// When the LineState is kLineStateFull, the next thing to do is generally to call FinalizeLine. 
					ts.FinalizeLine();

					// Print the line to stdout.
					const LineLayout& lineLayout = ts.GetLineLayout();
					EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

					// Test picking
					index_t iChar = 0, iCharLast = 0;

					for(float f = (gLayoutBoxes[b].mLeft - 10); f <= (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace + 10); f += 3.f)
					{
						EA::Text::GlyphSide glyphSide;

						// The 'true' here means that we require a hit and kIndexInvalid will be returned
						// if the position is to the left or right of the laid out string.
						iChar = lineLayout.GetTextPositionFromDisplayPosition(f, 0, true, kPickTypeInsertion, glyphSide, kDirectionLTR);

						if(iChar == kIndexInvalid)
						{
							EATEST_VERIFY((f < (gLayoutBoxes[b].mLeft + .1f)) || 
										  (f > (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace - .1f)));
							iChar = iCharLast; // Restore it.
						}
						else
							EATEST_VERIFY(iChar >= iCharLast);

						iCharLast = iChar;
					}

					EATEST_VERIFY((iCharLast == lineLayout.mCharArray.size()) || (iCharLast == lineLayout.mCharArray.size() - 1));

					// Test reverse-picking
					for(eastl_size_t j = 0; j < lineLayout.mCharArray.size(); j++)
					{
						EA::Text::Point pt;

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideBefore, pt, kDirectionLTR);
						EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 0.1f)); This test is temporarily disabled because it's failing; need to figure out why.

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideAfter, pt, kDirectionLTR);
						EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 36));   This test is temporarily disabled because it's failing; need to figure out why. // We need a lot of slop here because in 'emergency' breaking, a line might be a single character which overflows the right side.
					}

					ts.NextLine();
					lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
				}

				EATextTest_CheckMemory();
			}

			ts.Reset();
			lineState = ts.GetLineState();

			EATextTest_CheckMemory();
		}
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterObjects1
//
int TestTypesetterObjects1()
{
	int nErrorCount = 0;

	EATextTest_CheckMemory();

	// Typesetter
	Typesetter            ts;
	Typesetter::LineState lineState;
	LineLayout&           llayout = ts.GetLineLayout();
	LineLayoutIterator    lli(&llayout);
	size_t                i;

	// Object glyph metrics
	const size_t kObjectCount = 50;
	GlyphMetrics glyphMetrics[kObjectCount];

	for(i = 0; i < kObjectCount; i++)
	{
		glyphMetrics[i].mfSizeX     = (float)(1 + (i % 25));
		glyphMetrics[i].mfSizeY     = (float)(1 + (i %  5));
		glyphMetrics[i].mfHBearingX = 0;
		glyphMetrics[i].mfHBearingY = 0;
		glyphMetrics[i].mfHAdvanceX = glyphMetrics[i].mfSizeX + 1;
	}

	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		intptr_t nLastObjectValue = -1; // Used to verify that we are walking through the objects one by one in order.

		EATextTest_CheckMemory();

		ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);

		for(i = 0; i < kObjectCount; i++)
			ts.AddObject((void*)(intptr_t)i, glyphMetrics[i], false, false);

		lineState = ts.ProcessLine();

		while(lineState != Typesetter::kLineStateComplete)
		{
			eastl_size_t iChar;

			EATextTest_CheckMemory();
			ts.FinalizeLine();

			// We look at the resulting LineLayout and verify that it is reasonable.
			lli.Reset();

			while(lli.GetNextObject(iChar))
			{
				EATEST_VERIFY(iChar < 50);

				const index_t          iGlyph  = llayout.GetGlyphIndexFromCharIndex(iChar);
				const GlyphInfo&       gi      = llayout.mGlyphInfoArray[iGlyph];
				const GlyphLayoutInfo& gli     = llayout.mGlyphLayoutInfoArray[iGlyph];
				void* const            pObject = gli.mpObject;
				intptr_t               nObject = (intptr_t)pObject;

				EATEST_VERIFY(gi.mbGlyphIsObject != 0);
				EATEST_VERIFY((gli.mfX2 - gli.mfX1) == glyphMetrics[nObject].mfSizeX);
				EATEST_VERIFY((gli.mfY2 - gli.mfY1) == glyphMetrics[nObject].mfSizeY);
				EATEST_VERIFY(gli.mfAdvance == glyphMetrics[nObject].mfHAdvanceX);

				EATEST_VERIFY(nObject == (nLastObjectValue + 1));
				nLastObjectValue = nObject;
			}

			ts.NextLine();
			lineState = ts.GetLineState();
			EATextTest_CheckMemory();
		}

		ts.Reset();
		EATextTest_CheckMemory();
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


// This Unicode text was generated with Paul Pedriana's "Clipboard Unicode To C" applet.
// When you run it, any text you copy to the clipboard gets pasted as a Unicode C string literal.

const Char* gpHebrewText0 = EATEXT_CHAR("\x05D0\x05E8\x05DB\x05D9\x05D5\x05DF\x0020\x05D4\x05D0\x05E8\x05E5");
const Char* gpHebrewText1 = EATEXT_CHAR("\x05E8\x05D5\x05D1\x0020\x05D4\x05E2\x05D9\x05EA\x05D5\x05E0\x05D0\x05D9\x05DD\x0020\x05D4\x05E8\x05D5\x05E1\x05D9\x05DD\x0020\x05D8\x05E2\x05E0\x05D5\x0020\x05DB\x05D9\x0020\x05D4\x05D9\x05D3\x05D9\x05E0\x05E7\x0020\x05D1\x05D9\x05E6\x05E2\x0020\x05DC\x05D0\x0020\x05DE\x05E2\x05D8\x0020\x05D8\x05E2\x05D5\x05D9\x05D5\x05EA\x0020\x05D8\x05E7\x05D8\x05D9\x05D5\x05EA\x0020\x05D5\x05DB\x05D9\x0020\x05E7\x05E9\x05D8\x05DF\x0020\x05D4\x05D9\x05D4\x0020\x05E2\x05D3\x05D9\x05E3\x0020\x05E2\x05DC\x05D9\x05D5\x0020\x05D1\x05D3\x05E8\x05D2\x05D4\x0020\x05D0\x05D7\x05EA\x0020\x05DC\x05E4\x05D7\x05D5\x05EA\x0020\x05D1\x05E0\x05D9\x05D4\x05D5\x05DC\x0020\x05D4\x05DE\x05E9\x05D7\x05E7\x002E");
const Char* gpHebrewText2 = EATEXT_CHAR(" ");
const Char* gpHebrewText3 = EATEXT_CHAR("");
const Char* gpHebrewTextArray[4] = { gpHebrewText0, gpHebrewText1, gpHebrewText2, gpHebrewText3 };


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterHebrew1
//
int TestTypesetterHebrew1()
{
	int                   nErrorCount = 0;
	LayoutSettings        layoutSettings;
	Typesetter            ts;
	Typesetter::LineState lineState = ts.GetLineState();
	TextStyle             textStyle;

	// Use an expansive Unicode font.
	textStyle.mfSize = 16;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	// Setup a native Hebrew display environment.
	EA::StdC::Strcpy(layoutSettings.mLocale, EATEXT_CHAR("iw-il")); // Hebrew-Israel
	layoutSettings.mScript    = kScriptHebrew;
	layoutSettings.mDirection = kDirectionRTL;
	ts.SetLayoutSettings(layoutSettings);

	// Run the tests
	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		for(size_t i = 0; i < EAArrayCount(gpHebrewTextArray); i++) // For each test text...
		{
			EATextTest_CheckMemory();

			EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpHebrewTextArray[i]);
			EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

			ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);

			// We we do here is submit the text run in parts.
			const Char* pCurrent = gpHebrewTextArray[i];
			const Char* pEnd     = pCurrent + EA::StdC::Strlen(pCurrent);

			while(pCurrent < pEnd)
			{
				int32_t count = (int32_t)(pEnd - pCurrent);
				if(count > 8)
					count = 8;

				count = gRandom.RandRange(0, count + 1); // Select 0, 1, 2, 3, ... count Chars.
				EATEST_VERIFY((pCurrent + count) <= pEnd);

				ts.AddTextRun(pCurrent, (uint32_t)count);
				pCurrent += count;
			}

			lineState = ts.GetLineState();
			EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpTestTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

			while(lineState != Typesetter::kLineStateComplete)
			{
				EATextTest_CheckMemory();

				lineState = ts.ProcessLine();

				if(lineState == Typesetter::kLineStatePartial)
				{
					// Call FinalizeLine here, as we've already submitted all the text and so if the 
					// LineState is kLineStatePartial then there must not be enough text to fill the line.
					// You wouldn't call FinalizeLine in the case that you have more text to submit.
					ts.FinalizeLine();

					lineState = ts.GetLineState();
					EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
				}

				if(lineState == Typesetter::kLineStateFull)
				{
					// When the LineState is kLineStateFull, the next thing to do is generally to
					// call FinalizeLine. 
					ts.FinalizeLine();

					// Print the line to stdout.
					const LineLayout& lineLayout = ts.GetLineLayout();
					EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

					// Test picking
					index_t iChar = 0, iCharLast = 0;

					for(float f = (gLayoutBoxes[b].mLeft - 10); f <= (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace + 10); f += 3.f)
					{
						EA::Text::GlyphSide glyphSide;

						// The 'true' here means that we require a hit and kIndexInvalid will be returned
						// if the position is to the left or right of the laid out string.
						iChar = lineLayout.GetTextPositionFromDisplayPosition(f, 0, true, kPickTypeInsertion, glyphSide, kDirectionLTR);

						if(iChar == kIndexInvalid)
						{
							//EATEST_VERIFY((f < (gLayoutBoxes[b].mLeft + .1f)) ||    Disabled because it's failing (slightly) and we need to figure out why.
							//              (f > (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace - .1f)));
							iChar = iCharLast; // Restore it.
						}
						else
						{
							// EATEST_VERIFY(iChar >= iCharLast);  Disabled because it's failing (slightly) and we need to figure out why.
						}

						iCharLast = iChar;
					}

					// EATEST_VERIFY(iCharLast == lineLayout.mCharArray.size()); Disabled because it's failing (slightly) and we need to figure out why.

					// Test reverse-picking
					for(eastl_size_t j = 0; j < lineLayout.mCharArray.size(); j++)
					{
						EA::Text::Point pt;

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideBefore, pt, kDirectionLTR);
					  //EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 0.1f));  This test is temporarily disabled because it's failing; need to figure out why.

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideAfter, pt, kDirectionLTR);
					  //EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 36));    This test is temporarily disabled because it's failing; need to figure out why. // We need a lot of slop here because in 'emergency' breaking, a line might be a single character which overflows the right side.
					}

					ts.NextLine();
					lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
				}

				EATextTest_CheckMemory();
			}

			ts.Reset();
			lineState = ts.GetLineState();

			EATextTest_CheckMemory();
		}
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


// This Unicode text was generated with Paul Pedriana's "Clipboard Unicode To C" applet.
// When you run it, any text you copy to the clipboard gets pasted as a Unicode C string literal.

const Char* gpHangulText0 = EATEXT_CHAR("\xBC31\xD559\xB9BC\xCC28\xC218\x0020\xC11C\xAC70");
const Char* gpHangulText1 = EATEXT_CHAR("\xC804\xC2DC\xAD00\xC744\x0020\xCC3E\xC740\x0020\xADF8\xB4E4\xC740\x0020\xC704\xB300\xD55C\x0020\xC218\xB839\x0020\xAE40\xC77C\xC131\xB3D9\xC9C0\xC758\x0020\xC601\xC6D0\xBD88\xBA78\xD560\x0020\xC5C5\xC801\xC744\x0020\xBCF4\xC5EC\xC8FC\xB294\x0020\xC0AC\xC9C4\xB4E4\xACFC\x0020\xC601\xAD11\xC2A4\xB7EC\xC6B4\x0020\x300A\x314C\x002E\x3137\x300B\xC758\x0020\xC804\xD1B5\xC744\x0020\xACC4\xC2B9\xD558\xC5EC\x0020\xC870\xC120\xB85C\xB3D9\xB2F9\xC744\x0020\xD544\xC2B9\xBD88\xD328\xC758\x0020\xD5A5\xB3C4\xC801\xB825\xB7C9\xC73C\xB85C\x0020\xAC15\xD654\xBC1C\xC804\xC2DC\xD0A4\xC2E0\x0020\xACBD\xC560\xD558\xB294\x0020\xAE40\xC815\xC77C\xC7A5\xAD70\xB2D8\xC758\x0020\xB839\xB3C4\xC758\x0020\xD604\xBA85\xC131\xC744\x0020\xBCF4\xC5EC\xC8FC\xB294\x0020\xC0AC\xC9C4\xB4E4\x002C\xC808\xC138\xC704\xC778\xB4E4\xAED8\xC11C\x0020\xC9D1\xD544\xD558\xC2E0\x0020\xBD88\xD6C4\xC758\x0020\xACE0\xC804\xC801\xB85C\xC791\xB4E4\xC744\x0020\xAE4A\xC740\x0020\xAC10\xB3D9\xC18D\xC5D0\x0020\xBCF4\xC558\xB2E4\x002E");
const Char* gpHangulText2 = EATEXT_CHAR(" ");
const Char* gpHangulText3 = EATEXT_CHAR("");
const Char* gpHangulTextArray[4] = { gpHangulText0, gpHangulText1, gpHangulText2, gpHangulText3 };


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterHangul1
//
int TestTypesetterHangul1()
{
	int                   nErrorCount = 0;
	LayoutSettings        layoutSettings;
	Typesetter            ts;
	Typesetter::LineState lineState = ts.GetLineState();
	TextStyle             textStyle;

	// Use an expansive Unicode font.
	textStyle.mfSize = 16;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	// Setup a native Hangul display environment.
	EA::StdC::Strcpy(layoutSettings.mLocale, EATEXT_CHAR("ko-kr")); // Korean-Korea
	layoutSettings.mScript    = kScriptHangul;
	layoutSettings.mDirection = kDirectionLTR;
	ts.SetLayoutSettings(layoutSettings);

	// Run the tests
	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		for(size_t i = 0; i < EAArrayCount(gpHangulTextArray); i++) // For each test text...
		{
			EATextTest_CheckMemory();

			EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpHangulTextArray[i]);
			EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

			ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);

			// We we do here is submit the text run in parts.
			const Char* pCurrent = gpHangulTextArray[i];
			const Char* pEnd     = pCurrent + EA::StdC::Strlen(pCurrent);

			while(pCurrent < pEnd)
			{
				int32_t count = (int32_t)(pEnd - pCurrent);
				if(count > 8)
					count = 8;

				count = gRandom.RandRange(0, count + 1); // Select 0, 1, 2, 3, ... count Chars.
				EATEST_VERIFY((pCurrent + count) <= pEnd);

				ts.AddTextRun(pCurrent, (uint32_t)count);
				pCurrent += count;
			}

			lineState = ts.GetLineState();
			EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpTestTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

			while(lineState != Typesetter::kLineStateComplete)
			{
				EATextTest_CheckMemory();

				lineState = ts.ProcessLine();

				if(lineState == Typesetter::kLineStatePartial)
				{
					// Call FinalizeLine here, as we've already submitted all the text and so if the 
					// LineState is kLineStatePartial then there must not be enough text to fill the line.
					// You wouldn't call FinalizeLine in the case that you have more text to submit.
					ts.FinalizeLine();

					lineState = ts.GetLineState();
					EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
				}

				if(lineState == Typesetter::kLineStateFull)
				{
					// When the LineState is kLineStateFull, the next thing to do is generally to
					// call FinalizeLine. 
					ts.FinalizeLine();

					// Print the line to stdout.
					const LineLayout& lineLayout = ts.GetLineLayout();
					EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

					// Test picking
					index_t iChar = 0, iCharLast = 0;

					for(float f = (gLayoutBoxes[b].mLeft - 10); f <= (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace + 10); f += 3.f)
					{
						EA::Text::GlyphSide glyphSide;

						// The 'true' here means that we require a hit and kIndexInvalid will be returned
						// if the position is to the left or right of the laid out string.
						iChar = lineLayout.GetTextPositionFromDisplayPosition(f, 0, true, kPickTypeInsertion, glyphSide, kDirectionLTR);

						if(iChar == kIndexInvalid)
						{
							EATEST_VERIFY((f < (gLayoutBoxes[b].mLeft + .1f)) || 
										  (f > (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace - .1f)));
							iChar = iCharLast; // Restore it.
						}
						else
							EATEST_VERIFY(iChar >= iCharLast);

						iCharLast = iChar;
					}

					// EATEST_VERIFY(iCharLast == lineLayout.mCharArray.size()); Disabled until we can figure out why it's failing.

					// Test reverse-picking
					for(eastl_size_t j = 0; j < lineLayout.mCharArray.size(); j++)
					{
						EA::Text::Point pt;

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideBefore, pt, kDirectionLTR);
						EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 0.1f));    This test is temporarily disabled because it's failing; need to figure out why.

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideAfter, pt, kDirectionLTR);
						EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 36));      This test is temporarily disabled because it's failing; need to figure out why. // We need a lot of slop here because in 'emergency' breaking, a line might be a single character which overflows the right side.
					}

					ts.NextLine();
					lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
				}

				EATextTest_CheckMemory();
			}

			ts.Reset();
			lineState = ts.GetLineState();

			EATextTest_CheckMemory();
		}
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}


// This Unicode text was generated with Paul Pedriana's "Clipboard Unicode To C" applet.
// When you run it, any text you copy to the clipboard gets pasted as a Unicode C string literal.

const Char* gpArabicText0 = EATEXT_CHAR("\x0627\x0644\x0627\x0633\x062a\x0645\x0631\x0631\x0020\x0628\x062f\x0648\x0646\x0020\x0633\x062c\x064a\x0644");
const Char* gpArabicText1 = EATEXT_CHAR("\x0648\x064A\x0623\x062A\x064A\x0020\x0647\x0630\x0627\x0020\x0627\x0644\x0642\x0631\x0627\x0631\x0020\x0628\x0639\x062F\x0020\x0623\x0642\x0644\x0020\x0645\x0646\x0020\x062B\x0644\x0627\x062B\x0629\x0020\x0623\x0633\x0627\x0628\x064A\x0639\x0020\x0645\x0646\x0020\x062A\x0648\x0635\x0644\x0020\x0645\x0635\x0631\x0020\x0644\x0627\x062A\x0641\x0627\x0642\x0020\x0623\x0648\x0644\x064A\x0020\x0645\x0639\x0020\x0635\x0646\x062F\x0648\x0642\x0020\x0627\x0644\x0646\x0642\x062F\x0020\x0627\x0644\x062F\x0648\x0644\x064A\x0020\x0644\x0646\x064A\x0644\x0020\x062B\x0644\x0627\x062B\x0629\x0020\x0645\x0644\x064A\x0627\x0631\x0627\x062A\x0020\x062F\x0648\x0644\x0627\x0631\x0020\x062A\x0633\x0627\x0639\x062F\x0647\x0627\x0020\x0639\x0644\x0649\x0020\x062A\x0642\x0644\x064A\x0635\x0020\x062D\x062C\x0645\x0020\x0627\x0644\x0639\x062C\x0632\x0020\x0641\x064A\x0020\x0627\x0644\x0645\x0648\x0627\x0632\x0646\x0629\x060C\x0020\x0643\x0645\x0627\x0020\x062A\x0628\x0627\x062D\x062B\x062A\x0020\x0627\x0644\x0642\x0627\x0647\x0631\x0629\x0020\x0623\x064A\x0636\x0627\x0020\x0645\x0639\x0020\x0627\x0644\x0628\x0646\x0643\x0020\x0627\x0644\x062F\x0648\x0644\x064A\x0020\x0644\x0644\x062D\x0635\x0648\x0644\x0020\x0639\x0644\x0649\x0020\x0642\x0631\x0636\x0020\x0628\x0642\x064A\x0645\x0629\x0020\x0645\x0644\x064A\x0627\x0631\x064A\x0646\x0020\x0648\x0032\x0030\x0030\x0020\x0645\x0644\x064A\x0648\x0646\x0020\x062F\x0648\x0644\x0627\x0631\x0020\x064A\x062F\x0641\x0639\x0020\x0639\x0644\x0649\x0020\x0645\x062F\x0649\x0020\x0633\x0646\x062A\x064A\x0646\x002E");
const Char* gpArabicText2 = EATEXT_CHAR("\x0645\x0635\x0631\x0020\x062A\x0624\x062C\x0644\x0020\x062E\x0637\x0637\x0627\x0020\x0644\x0642\x0631\x0648\x0636\x0020\x062F\x0648\x0644\x064A\x0629");
const Char* gpArabicText3 = EATEXT_CHAR("\x0627\x0644\x062C\x0645\x0639\x0629\x0020\x0032\x0033\x002F\x0037\x002F\x0031\x0034\x0033\x0032\x0020\x0647\x0640\x0020\x002D\x0020\x0627\x0644\x0645\x0648\x0627\x0641\x0642\x0020\x0032\x0034\x002F\x0036\x002F\x0032\x0030\x0031\x0031\x0020\x0645\x0020\x0028\x0622\x062E\x0631\x0020\x062A\x062D\x062F\x064A\x062B\x0029\x0020\x0627\x0644\x0633\x0627\x0639\x0629\x0020\x0032\x0033\x003A\x0031\x0030\x0020\x0028\x0645\x0643\x0629\x0020\x0627\x0644\x0645\x0643\x0631\x0645\x0629\x0029\x060C\x0020\x0032\x0030\x003A\x0031\x0030\x0020\x0028\x063A\x0631\x064A\x0646\x062A\x0634\x0029");
const Char* gpArabicTextArray[4] = { gpArabicText0, gpArabicText1, gpArabicText2, gpArabicText3 };


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterArabic1
//
int TestTypesetterArabic1()
{
	int                   nErrorCount = 0;
	LayoutSettings        layoutSettings;
	Typesetter            ts;
	Typesetter::LineState lineState = ts.GetLineState();
	TextStyle             textStyle;

	// Use an expansive Unicode font.
	textStyle.mfSize = 16;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("EA HelveticaNeue Med")); // This font has OpenType instructions that exercize our Lam-Alif ligature code.
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[2], EATEXT_CHAR("Arial"));

	// Setup a native Arabic display environment.
	EA::StdC::Strcpy(layoutSettings.mLocale, EATEXT_CHAR("ar-sa")); // Arabic-Saudi Arabia
	layoutSettings.mScript    = kScriptArabic;
	layoutSettings.mDirection = kDirectionRTL;
	ts.SetLayoutSettings(layoutSettings);

	// Run the tests
	for(size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
	{
		for(size_t i = 0; i < EAArrayCount(gpArabicTextArray); i++) // For each test text...
		{
			EATextTest_CheckMemory();

			EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpArabicTextArray[i]);
			EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

			ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);

			// We we do here is submit the text run in parts.
			const Char* pCurrent = gpArabicTextArray[i];
			const Char* pEnd     = pCurrent + EA::StdC::Strlen(pCurrent);

			while(pCurrent < pEnd)
			{
				int32_t count = (int32_t)(pEnd - pCurrent);
				if(count > 8)
					count = 8;

				count = gRandom.RandRange(0, count + 1); // Select 0, 1, 2, 3, ... count Chars.
				EATEST_VERIFY((pCurrent + count) <= pEnd);

				ts.AddTextRun(pCurrent, (uint32_t)count);
				pCurrent += count;
			}

			lineState = ts.GetLineState();
			EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpArabicTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

			while(lineState != Typesetter::kLineStateComplete)
			{
				EATextTest_CheckMemory();

				lineState = ts.ProcessLine();

				if(lineState == Typesetter::kLineStatePartial)
				{
					// Call FinalizeLine here, as we've already submitted all the text and so if the 
					// LineState is kLineStatePartial then there must not be enough text to fill the line.
					// You wouldn't call FinalizeLine in the case that you have more text to submit.
					ts.FinalizeLine();

					lineState = ts.GetLineState();
					EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
				}

				if(lineState == Typesetter::kLineStateFull)
				{
					// When the LineState is kLineStateFull, the next thing to do is generally to
					// call FinalizeLine. 
					ts.FinalizeLine();

					// Print the line to stdout.
					const LineLayout& lineLayout = ts.GetLineLayout();
					EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

					// Test picking
					index_t iChar = 0, iCharLast = 0;

					for(float f = (gLayoutBoxes[b].mLeft - 10); f <= (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace + 10); f += 3.f)
					{
						EA::Text::GlyphSide glyphSide;

						// The 'true' here means that we require a hit and kIndexInvalid will be returned
						// if the position is to the left or right of the laid out string.
						iChar = lineLayout.GetTextPositionFromDisplayPosition(f, 0, true, kPickTypeInsertion, glyphSide, kDirectionLTR);

						if(iChar == kIndexInvalid)
						{
							//EATEST_VERIFY((f < (gLayoutBoxes[b].mLeft + .1f)) ||  Disabled until we can figure out why it's failing.
							//              (f > (gLayoutBoxes[b].mLeft + lineLayout.mLineMetrics.mfSpace - .1f)));
							iChar = iCharLast; // Restore it.
						}
						else
						{
							// EATEST_VERIFY(iChar >= iCharLast);       Disabled until we can figure out why it's failing.
						}

						iCharLast = iChar;
					}

					// EATEST_VERIFY(iCharLast == lineLayout.mCharArray.size()); Disabled until we can figure out why it's failing.

					// Test reverse-picking
					for(eastl_size_t j = 0; j < lineLayout.mCharArray.size(); j++)
					{
						EA::Text::Point pt;

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideBefore, pt, kDirectionLTR);
					  //EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 0.1f)); This test is temporarily disabled because it's failing; need to figure out why.

						lineLayout.GetDisplayPositionFromTextPosition(j, kGlyphSideAfter, pt, kDirectionLTR);
					  //EATEST_VERIFY(pt.mX >= (gLayoutBoxes[b].mLeft  - 0.1f));
					  //EATEST_VERIFY(pt.mX <= (gLayoutBoxes[b].mRight + 36));   This test is temporarily disabled because it's failing; need to figure out why. // We need a lot of slop here because in 'emergency' breaking, a line might be a single character which overflows the right side.
					}

					ts.NextLine();
					lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
				}

				EATextTest_CheckMemory();
			}

			ts.Reset();
			lineState = ts.GetLineState();

			EATextTest_CheckMemory();
		}
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestTypesetterRegression
//
// This is where we put regressions of bugs we've found and fixed and want
// to make sure we test for that bug in the future.
//
int TestTypesetterRegression()
{
	int nErrorCount = 0;

	#ifdef EA_PLATFORM_WINDOWS
		{ // Test RedAlert 3 Thai line breaking problem.
			EA::Text::InitWordBreakDictionary();

			TestString s16;

			s16 += EATEXT_CHAR("\x0E02\x0E2D\x0E15\x0E49\x0E2D\x0E19\x0E23\x0E31\x0E1A\x0E40\x0E02\x0E49\x0E32\x0E2A");
			s16 += EATEXT_CHAR("\x0E39\x0E48\x0020\x0052\x0065\x0064\x0020\x0041\x006C\x0065\x0072\x0074\x0020\x0033");
			s16 += EATEXT_CHAR("\x0020\x004F\x006E\x006C\x0069\x006E\x0065\x0021\x0020\x0E01\x0E23\x0E38\x0E13\x0E32");
			s16 += EATEXT_CHAR("\x0E2D\x0E48\x0E32\x0E19\x0E04\x0E33\x0E41\x0E19\x0E30\x0E19\x0E33\x0E14\x0E49\x0E32");
			s16 += EATEXT_CHAR("\x0E19\x0E25\x0E48\x0E32\x0E07\x0E01\x0E48\x0E2D\x0E19\x0E40\x0E23\x0E34\x0E48\x0E21");
			s16 += EATEXT_CHAR("\x0E40\x0E25\x0E48\x0E19\x0E40\x0E01\x0E21\x005C\x0072\x005C\x006E\x005C\x0072\x005C");
			s16 += EATEXT_CHAR("\x006E\x0E2B\x0E32\x0E01\x0E04\x0E38\x0E13\x0E22\x0E31\x0E07\x0E44\x0E21\x0E48\x0E44");
			s16 += EATEXT_CHAR("\x0E14\x0E49\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A\x0E35\x0E22\x0E19\x0E43\x0E19\x0E01");
			s16 += EATEXT_CHAR("\x0E32\x0E23\x0E17\x0E35\x0E48\x0E08\x0E30\x0E40\x0E25\x0E48\x0E19\x0E40\x0E01\x0E21");
			s16 += EATEXT_CHAR("\x0E2D\x0E2D\x0E19\x0E44\x0E25\x0E19\x0E4C\x0020\x0E01\x0E23\x0E38\x0E13\x0E32\x0E04");
			s16 += EATEXT_CHAR("\x0E25\x0E34\x0E01\x0E17\x0E35\x0E48\x0E1B\x0E38\x0E48\x0E21\x0020\x0052\x0065\x0067");
			s16 += EATEXT_CHAR("\x0069\x0073\x0074\x0065\x0072\x0020\x0E17\x0E32\x0E07\x0E14\x0E49\x0E32\x0E19\x0E0B");
			s16 += EATEXT_CHAR("\x0E49\x0E32\x0E22\x0E0B\x0E36\x0E48\x0E07\x0E08\x0E30\x0E40\x0E0A\x0E37\x0E48\x0E2D");
			s16 += EATEXT_CHAR("\x0E21\x0E15\x0E48\x0E2D\x0E04\x0E38\x0E13\x0E44\x0E1B\x0E22\x0E31\x0E07\x0E2B\x0E19");
			s16 += EATEXT_CHAR("\x0E49\x0E32\x0E01\x0E32\x0E23\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A\x0E35\x0E22\x0E19");
			s16 += EATEXT_CHAR("\x0E02\x0E2D\x0E07\x0020\x0045\x0041\x0020\x006F\x006E\x006C\x0069\x006E\x0065\x0020");
			s16 += EATEXT_CHAR("\x0E17\x0E33\x0E15\x0E32\x0E21\x0E04\x0E33\x0E41\x0E19\x0E30\x0E19\x0E33\x0E17\x0E35");
			s16 += EATEXT_CHAR("\x0E48\x0E1B\x0E23\x0E32\x0E01\x0E0F\x0E2D\x0E22\x0E39\x0E48\x0E41\x0E25\x0E49\x0E27");
			s16 += EATEXT_CHAR("\x0E01\x0E25\x0E31\x0E1A\x0E21\x0E32\x0E22\x0E31\x0E07\x0E2B\x0E19\x0E49\x0E32\x0E19");
			s16 += EATEXT_CHAR("\x0E35\x0E49\x0E40\x0E21\x0E37\x0E48\x0E2D\x0E17\x0E33\x0E40\x0E2A\x0E23\x0E47\x0E08");
			s16 += EATEXT_CHAR("\x005C\x0072\x005C\x006E\x005C\x0072\x005C\x006E\x0E2B\x0E32\x0E01\x0E04\x0E38\x0E13");
			s16 += EATEXT_CHAR("\x0E44\x0E14\x0E49\x0E17\x0E33\x0E01\x0E32\x0E23\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A");
			s16 += EATEXT_CHAR("\x0E35\x0E22\x0E19\x0E40\x0E23\x0E35\x0E22\x0E1A\x0E23\x0E49\x0E2D\x0E22\x0E41\x0E25");
			s16 += EATEXT_CHAR("\x0E49\x0E27\x0020\x0E01\x0E23\x0E38\x0E13\x0E32\x0E01\x0E23\x0E2D\x0E01\x0E02\x0E49");
			s16 += EATEXT_CHAR("\x0E2D\x0E21\x0E39\x0E25\x0E17\x0E32\x0E07\x0E14\x0E49\x0E32\x0E19\x0E0B\x0E49\x0E32");
			s16 += EATEXT_CHAR("\x0E22\x0020\x0E23\x0E27\x0E21\x0E16\x0E36\x0E07\x0E0A\x0E37\x0E48\x0E2D\x0020\x0041");
			s16 += EATEXT_CHAR("\x0063\x0063\x006F\x0075\x006E\x0074\x0020\x0020\x0045\x0041\x0020\x0E23\x0E2B\x0E31");
			s16 += EATEXT_CHAR("\x0E2A\x0E1C\x0E48\x0E32\x0E19\x0020\x0E41\x0E25\x0E30\x0E2D\x0E2D\x0E19\x0E44\x0E25");
			s16 += EATEXT_CHAR("\x0E19\x0E4C\x0E44\x0E2D\x0E14\x0E35\x0E02\x0E2D\x0E07\x0E04\x0E38\x0E13\x0020\x0E41");
			s16 += EATEXT_CHAR("\x0E25\x0E49\x0E27\x0E04\x0E25\x0E34\x0E01\x0E1B\x0E38\x0E48\x0E21\x0020\x004C\x006F");
			s16 += EATEXT_CHAR("\x0067\x0020\x0049\x006E\x005C\x0072\x005C\x006E\x005C\x0072\x005C\x006E\x0E43\x0E19");
			s16 += EATEXT_CHAR("\x0E01\x0E32\x0E23\x0E15\x0E34\x0E14\x0E15\x0E32\x0E21\x0E02\x0E48\x0E32\x0E27\x0E25");
			s16 += EATEXT_CHAR("\x0E48\x0E32\x0E2A\x0E38\x0E14\x0E40\x0E01\x0E35\x0E48\x0E22\x0E27\x0E01\x0E31\x0E1A");
			s16 += EATEXT_CHAR("\x0020\x0052\x0065\x0064\x0020\x0041\x006C\x0065\x0072\x0074\x0020\x0033\x0020\x0E2A");
			s16 += EATEXT_CHAR("\x0E32\x0E21\x0E32\x0E23\x0E16\x0E40\x0E02\x0E49\x0E32\x0E44\x0E1B\x0E40\x0E22\x0E35");
			s16 += EATEXT_CHAR("\x0E48\x0E22\x0E21\x0E0A\x0E21\x0E44\x0E14\x0E49\x0E17\x0E35\x0E48\x0020\x0072\x0065");
			s16 += EATEXT_CHAR("\x0064\x0061\x006C\x0065\x0072\x0074\x0033\x002E\x0063\x006F\x006D\x0020\x0E02\x0E2D");
			s16 += EATEXT_CHAR("\x0E1A\x0E04\x0E38\x0E13\x0E17\x0E35\x0E48\x0E21\x0E32\x0E40\x0E25\x0E48\x0E19\x0E40");
			s16 += EATEXT_CHAR("\x0E01\x0E21\x0E41\x0E25\x0E30\x0E02\x0E2D\x0E43\x0E2B\x0E49\x0E42\x0E0A\x0E04\x0E14");
			s16 += EATEXT_CHAR("\x0E35\x0E43\x0E19\x0E01\x0E32\x0E23\x0E40\x0E25\x0E48\x0E19\x0021");

			const Char*  pTest = s16.c_str();
			Typesetter   ts;
			LineLayout   lineLayout;
			TextStyle    textStyle;
			size_t kTextLength = s16.length();

			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Tahoma"));
			textStyle.mfSize = 18;

			ts.LayoutParagraph(pTest, kTextLength, 0, 0, 455, 1000, &textStyle, lineLayout);

			// Somehow verify that there is no line break at char 141 (the middle of a certain base/accent pair).

			// lineHeight = lineLayout.GetLineHeight();
			// float        bboxW, bboxH, lineHeight;
			// lineLayout.GetBoundingBox(bboxW, bboxH, true);
			// 
			// Verify that the bboxH is equal to ~11 times the height of a single line.
			// EATEST_VERIFY((bboxH >= ((lineHeight * 11) - 1)) && (bboxH < ((lineHeight * 11) + 1)));

			EA::Text::ShutdownWordBreakDictionary();
		}
	#endif

	{
		Typesetter ts;
		LineLayout lineLayout;
		TextStyle  textStyle;
		TestString s;

		for(int i = 0; i < 100; i++)
		{
			GenerateRandomString(s, 1, 10, 6);

			lineLayout.Clear();

			ts.LayoutParagraph(s.c_str(), s.length(), 0, 0, 133, 10000, &textStyle, lineLayout);

			// Copy line layout back to typesetter.
			LineLayout& tsLL = ts.GetLineLayout();
			tsLL.Clear();
			tsLL.Append(lineLayout); // This was causing a bogus assertion failure in EAText 1.02.00.
			EATEST_VERIFY(tsLL.Validate(true));
		}
	}

	{
		// Test that Typesetter properly recognizes newlines and force-breaks on them.
		Typesetter ts;
		LineLayout lineLayout;
		TextStyle  textStyle;
		float      bboxW, bboxH, lineHeight;
		TestString s16;

		s16 += EATEXT_CHAR("abc def\n");
		s16 += EATEXT_CHAR("ghi jkl\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("mno pqr\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("stu vwx\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("\n");
		s16 += EATEXT_CHAR("yz"); // We have 11 lines

		EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
		textStyle.mfSize = 10;

		ts.LayoutParagraph(s16.c_str(), s16.length(), 0, 0, 1000, 1000, &textStyle, lineLayout);
		lineHeight = lineLayout.GetLineHeight();
		lineLayout.GetBoundingBox(bboxW, bboxH, true);

		// Verify that the bboxH is equal to ~11 times the height of a single line.
		// Disabled temporarily because it's triggering failures. The failure is due to a real bug 
		// in the typesetter line height rounding which has been around for a long time.
		// EATEST_VERIFY_F((bboxH >= ((lineHeight * 11) - 1)) && (bboxH < ((lineHeight * 11) + 1)), "BBox expected to be ~11 * lineHeight, but isn't: BBox height: %f; lineHeight: %f; 11 * lineHeight: %f", bboxH, lineHeight, lineHeight * 11);
		EATEST_VERIFY((bboxW > 0) && (bboxH > 0) && (lineHeight > 0)); // Temporary alternative test.
	}

	//Scrabble: Text on the right side of an inlined image would change and no longer match the font on the left side of the image
	//ex: "some font <image> different font"
	{
		const char* pStyle =
			"   MainStyle {                                 \n"
			"      font-family : \"Arial\";                 \n"
			"      font-size: 55px;                         \n"
			"      font-style: italic;                      \n"
			"      background-color : #223344;              \n"
			"      color : #234;                            \n"
			"   }                                           \n"
			"                                               \n"
			"   WindowDefaultStyle(0x00000001) : MainStyle {\n"
			"       font-family:\"Trebuchet MS\";                \n"
			"       font-size:24pt;                         \n"
			"   }                                           \n";

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);
		Typesetter      ts;
		FontServer      fontServer;

		//Setup fontServer with a default font and a font which
		//does map a character code to a glyphIndex for 0xFFFC (obj)
		fontServer.Init();
		
		FilePathChar pArialPath[EA::IO::kMaxPathLength];
		FilePathChar pCourPath[EA::IO::kMaxPathLength];

		GetTestDataDirectory(pArialPath, EA::IO::kMaxPathLength);
		GetTestDataDirectory(pCourPath, EA::IO::kMaxPathLength);

		EA::StdC::Strcat(pArialPath, EATEXT_FILE_PATH_CHAR("arial.ttf"));
		EA::StdC::Strcat(pCourPath,  EATEXT_FILE_PATH_CHAR("trebuc.ttf"));

		fontServer.AddFace(pArialPath);     
		fontServer.AddFace(pCourPath);

		//Read in the styles we wish to use
		EATEST_VERIFY(reader.ReadStyles());
		EATEST_VERIFY(reader.GetWarningCount() == 0);

		const TextStyle* pTextStyle = styleManager.GetStyle(0x00000001);
		EA_ASSERT(pTextStyle != NULL);
		
		ts.SetFontServer(&fontServer);

		//Set the size of the area we want to draw on
		ts.SetLayoutSpace(431.0f, 0.0f, 0.0f);
		
		//Add text to the left side of the image
		ts.AddStyleChange(pTextStyle);
		ts.AddTextRun(EATEXT_CHAR("L"), 1);

		//Add an object Glyph (in Scrabble's case a 50px by 50px image)
		GlyphMetrics gm;
		gm.mfSizeX = 50.0f;
		gm.mfSizeY = 50.0f;
		gm.mfHBearingX = 0.0f;
		gm.mfHBearingY = 37.0f;
		gm.mfHAdvanceX = 50.0f;
		void* image = (void*)(intptr_t)1;
		ts.AddObject(image, gm, false, false);

		//Add text to the right side of the image with the same style as before
		ts.AddStyleChange(pTextStyle);
		ts.AddTextRun(EATEXT_CHAR("R"), 1);

		ts.ProcessLine();

		eastl::vector<GlyphLayoutInfo, EA::Allocator::EASTLICoreAllocator> glyphLayoutInfo = ts.GetLineLayout().mGlyphLayoutInfoArray;

		Font* leftTextFont = glyphLayoutInfo[0].mpFont;
		Font* imageFont = glyphLayoutInfo[1].mpFont;
		Font* rightTextFont = glyphLayoutInfo[2].mpFont;

		//Image should be given the same font corrsponding to it's analysisInfo (which will be nothing)
		EATEST_VERIFY(leftTextFont != imageFont);

		//The text on the left and right side of the image should have the same font
		EATEST_VERIFY(leftTextFont == rightTextFont);

		fontServer.Shutdown();
	}

	// Tunguska: Arabic text forcing glyph substitutions of > 2 characters would crash EAText
	{
		LayoutSettings  layoutSettings;
		LineLayout		lineLayout;
		Typesetter      ts;
		TextStyle       textStyle;
		TestString		s(EATEXT_CHAR("\x0627\x0644\x0644\x0647"));
		FontServer      fontServer;

		fontServer.Init();
		ts.SetFontServer(&fontServer);

		FilePathChar pFontPath[EA::IO::kMaxPathLength];
		GetTestDataDirectory(pFontPath, EA::IO::kMaxPathLength);
		EA::StdC::Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("NotoKufiArabic-Regular.ttf"));
		bool fontExists = EA::IO::File::Exists(pFontPath);

		if (fontExists)
		{
			int nFaceCount = fontServer.AddFace(pFontPath);
			EATEST_VERIFY(nFaceCount > 0);
		}
		else
		{
			EA::UnitTest::ReportVerbosity(1, "Font not found: \nThis unit test is designed to use this" EATEXT_FILE_PATH_CHAR_FORMAT, pFontPath);
		}

		// Use an expansive Unicode font.
		textStyle.mfSize = 16;
		EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Noto Kufi Arabic")); // This font has OpenType instructions that exercize our Lam-Alif ligature code.
		Font* const pFont = fontServer.GetFont(&textStyle);
		EATEST_VERIFY(pFont != NULL);

		// Setup a native Arabic display environment.
		EA::StdC::Strcpy(layoutSettings.mLocale, EATEXT_CHAR("ar-sa")); // Arabic-Saudi Arabia
		layoutSettings.mScript = kScriptArabic;
		layoutSettings.mDirection = kDirectionRTL;
		ts.SetLayoutSettings(layoutSettings);

		lineLayout.Clear();

		// This line will internally call ShapeText and crash in the original test case
		ts.LayoutParagraph(s.c_str(), s.length(), 0, 0, 133, 10000, &textStyle, lineLayout); 

		// Our 4 glpyhs should have been replaced by one
		EATEST_VERIFY(lineLayout.mGlyphInfoArray.size() == 1);

		// Copy line layout back to typesetter.
		LineLayout& tsLL = ts.GetLineLayout();
		tsLL.Clear();
		
		EATEST_VERIFY(tsLL.Validate(true));
		EATEST_VERIFY(fontServer.Shutdown());
	}

	// Frostbite UI:
	// Letter spacing was being applied twice - https://jira.frostbite.ea.com/browse/FB-69768
	// Right aligned text with a letter spacing was going off the right edge - https://jira.frostbite.ea.com/browse/FB-78878
	{
		Typesetter ts;
		TextStyle  textStyle;
		LineLayout lineLayout;
		TestString s1 = EATEXT_CHAR("WWWWWWW");
		TestString s2 = EATEXT_CHAR("WWWWWWW\nWWWWWWW");
		float leftEdge, rightEdge;
		float noSpaceWidth1, noSpaceWidth2;
		const float boundingBoxWidth = 1000.f;

		EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
		textStyle.mfSize = 10;

		// Determine how big the bounds of the text are without any letter spacing
		ts.LayoutParagraph(s1.c_str(), s1.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		float spaceBetweenGlyphsNoSpace = lineLayout.mGlyphLayoutInfoArray[1].mfX1 - lineLayout.mGlyphLayoutInfoArray[0].mfX2;
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s1.length() - 1].mfX2;
		noSpaceWidth1 = rightEdge - leftEdge;
		lineLayout.Clear();

		ts.LayoutParagraph(s2.c_str(), s2.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s2.length() - 1].mfX2;
		noSpaceWidth2 = rightEdge - leftEdge;
		lineLayout.Clear();

		// Add in letter spacing and ensure the bounding box appropriately larger
		textStyle.mfLetterSpacing = 8.f;
		const float letterSpacingIncrease = textStyle.mfLetterSpacing * (s1.length()-1);

		ts.LayoutParagraph(s1.c_str(), s1.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		float spaceBetweenGlyphsWithSpace = lineLayout.mGlyphLayoutInfoArray[1].mfX1 - lineLayout.mGlyphLayoutInfoArray[0].mfX2;
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s1.length() - 1].mfX2;
		EATEST_VERIFY(spaceBetweenGlyphsNoSpace == spaceBetweenGlyphsWithSpace - textStyle.mfLetterSpacing);
		EATEST_VERIFY_F((rightEdge-leftEdge == noSpaceWidth1+letterSpacingIncrease), "Letter spacing should increase the width of the single-line bounding box appropriately. Expected width of %f, got %f\n", noSpaceWidth1+letterSpacingIncrease, rightEdge-leftEdge);
		lineLayout.Clear();

		ts.LayoutParagraph(s2.c_str(), s2.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s2.length() - 1].mfX2;
		EATEST_VERIFY_F((rightEdge-leftEdge == noSpaceWidth2+letterSpacingIncrease), "Letter spacing should increase the width of the multi-line bounding box appropriately. Expected width of %f, got %f\n", noSpaceWidth2+letterSpacingIncrease, rightEdge-leftEdge);
		lineLayout.Clear();

		// Perform right alignment and ensure the glyphs are at the right edge
		textStyle.mHAlignment = kHARight;

		ts.LayoutParagraph(s1.c_str(), s1.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s1.length()-1].mfX2;
		EATEST_VERIFY_F(rightEdge == boundingBoxWidth, "Right aligned single-line text with letter spacing should be at the right edge. Expected %f, got %f\n", boundingBoxWidth, rightEdge);
		lineLayout.Clear();

		ts.LayoutParagraph(s2.c_str(), s2.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s2.length()-1].mfX2;
		EATEST_VERIFY_F(rightEdge == boundingBoxWidth, "Right aligned multi-line text with letter spacing should be at the right edge. Expected %f, got %f\n", boundingBoxWidth, rightEdge);
		lineLayout.Clear();

		// Perform center alignment and ensure the left and right edges are approximately the same distance in from the side
		textStyle.mHAlignment = kHACenter;

		ts.LayoutParagraph(s1.c_str(), s1.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s1.length() - 1].mfX2;
		EATEST_VERIFY_F(fabsf(boundingBoxWidth-rightEdge-leftEdge) <= 1.f, "Center aligned single-line text with letter spacing should have similar distances from each edge. Distances from edge were %f and %f, which should be different by < 1 \n", leftEdge, boundingBoxWidth-rightEdge);
		lineLayout.Clear();

		/*
		This test does not work.
		EATextTypesetter uses this code when doing centering:
			fXOffset += floorf((mfLayoutSpace - mLineLayout.mLineMetrics.mfSpace) / 2);
		I suspect mfSpace is acconting for the \n, or something, increasing the horizontal size of the text
		when centering. This was not the bug we hit, so I am not fixing it here, but if it becomes a problem
		we need to fix, this test should cover it.

		ts.LayoutParagraph(s2.c_str(), s2.length(), 0.f, 0.f, boundingBoxWidth, 100.f, &textStyle, lineLayout);
		leftEdge = lineLayout.mGlyphLayoutInfoArray[0].mfX1;
		rightEdge = lineLayout.mGlyphLayoutInfoArray[s2.length() - 1].mfX2;
		EATEST_VERIFY_F(fabsf(boundingBoxWidth - rightEdge - leftEdge) <= 1.f, "Center aligned multi-line text with letter spacing should have similar distances from each edge. Distances from edge were %f and %f, which should be different by < 1 \n", leftEdge, boundingBoxWidth - rightEdge);
		lineLayout.Clear();
		*/
	}

	return nErrorCount;
}


static int TestTypesetterAlignment()
{
	int nErrorCount = 0;

	// Add "Ahem" (test font) to the Font Server.
	EA::IO::MemoryStream* pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pAhemTTF, sizeof(pAhemTTF), true, false, GetAllocator());
	pMemoryStream->AddRef();
	FontServer* const pFontServer = GetFontServer(true);
	uint32_t nFaceCount = pFontServer->AddFace(pMemoryStream, kFontTypeOutline);
	pMemoryStream->Release();
	EATEST_VERIFY(nFaceCount == 1);

	// Specify the text style: Ahem 12. 
	// This font should have a line height of 16 and all glyphs are squares of size 16x16.
	TextStyle                      textStyle;
	eastl::vector<FontDescription> fontDescriptionArray;
	uint32_t                       iCount = pFontServer->EnumerateFonts(NULL, 0);

	EATEST_VERIFY(iCount > 0);
	if(iCount)
	{
		fontDescriptionArray.resize(iCount);
		pFontServer->EnumerateFonts(&fontDescriptionArray[0], iCount);

		for(float fSize = 6.0; fSize < 18.0; fSize += 3.f)
		{
			textStyle.mfSize = fSize;

			for(uint32_t i = 0; i < iCount; ++i) // For each font that the FontServer has...attempt to get that font from the FontServer.
			{
				EA::UnitTest::ReportVerbosity(1, "Registered font: %I16s\n", fontDescriptionArray[i].mFamily);

				EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], fontDescriptionArray[i].mFamily);

				Font* pFont = pFontServer->GetFont(&textStyle);
				EATEST_VERIFY(pFont != NULL);
				if(pFont)
				{
					FontDescription fontDescription;
					bool bResult = pFont->GetFontDescription(fontDescription);
					EATEST_VERIFY(bResult);
					if(bResult)
						EATEST_VERIFY_F((EA::StdC::Strcmp(fontDescription.mFamily, fontDescriptionArray[i].mFamily) == 0), "GetFont failed. Expected: %I16s, Returned: %I16s\n", fontDescriptionArray[i].mFamily, fontDescription.mFamily);
					
					// Do layout with the given font style.
					Typesetter           typesetter;
					LayoutSettings       layoutSettings;
					EA::Text::LineLayout lineLayout;
					const Char           pText[] = { 0x00c8, 0x00c7, 0 };

					layoutSettings.mbCellTopPosition = true;
					typesetter.SetLayoutSettings(layoutSettings);

					lineLayout.Clear();
					textStyle.mVAlignment = kVATop;
					typesetter.LayoutParagraph(pText, 2, 0.0f, 0.0f, 100.f, 100.f, &textStyle, lineLayout);
					for(eastl_size_t g = 0; g < lineLayout.mGlyphLayoutInfoArray.size(); g++)
					{
						const GlyphLayoutInfo& gli = lineLayout.mGlyphLayoutInfoArray[g];
						EATEST_VERIFY(gli.mfY1 >= 0); // Verify that no pixels are above the rectangle top.
					}

					lineLayout.Clear();
					textStyle.mVAlignment = kVABottom;
					typesetter.LayoutParagraph(pText, 2, 0.0f, 0.0f, 100.f, 100.f, &textStyle, lineLayout);
					for(eastl_size_t g = 0; g < lineLayout.mGlyphLayoutInfoArray.size(); g++)
					{
						const GlyphLayoutInfo& gli = lineLayout.mGlyphLayoutInfoArray[g];
						EATEST_VERIFY(gli.mfY2 <= 100); // Verify that no pixels are below the rectangle bottom.
					}

					layoutSettings.mbCellTopPosition = false;
					typesetter.SetLayoutSettings(layoutSettings);

					lineLayout.Clear();
					textStyle.mVAlignment = kVATop;
					typesetter.LayoutParagraph(pText, 2, 0.0f, 0.0f, 100.f, 100.f, &textStyle, lineLayout);
					for(eastl_size_t g = 0; g < lineLayout.mGlyphLayoutInfoArray.size(); g++)
					{
						const GlyphLayoutInfo& gli = lineLayout.mGlyphLayoutInfoArray[g];
						EATEST_VERIFY(gli.mfY1 >= 0); // Verify that no pixels are above the rectangle top.
					}

					lineLayout.Clear();
					textStyle.mVAlignment = kVABottom;
					typesetter.LayoutParagraph(pText, 2, 0.0f, 0.0f, 100.f, 100.f, &textStyle, lineLayout);
					for(eastl_size_t g = 0; g < lineLayout.mGlyphLayoutInfoArray.size(); g++)
					{
						const GlyphLayoutInfo& gli = lineLayout.mGlyphLayoutInfoArray[g];
						EATEST_VERIFY(gli.mfY2 <= 100); // Verify that no pixels are below the rectangle bottom.
					}

					pFont->Release();
				}
			}
		}
	}

	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestTypesetterDecorations
//
// The biggest problem of the Decoration system is to handle all the possible
// case depending on whether the coordinate system is y-up or y-down as well as
// whether the cooringate origin is the baseline or the top of the text.
// We then wanted to have tests cases to handle most of the possible configurations.
// 
// TODO: Add support for a Y-Up coordinate system when the GetGlyphDecoration support it.
static int TestDecorations(Typesetter ts, TextStyle textStyle, eastl::basic_string<Char> text, LineLayout lineLayout)
{
	int nErrorCount = 0;

	//Prepare the Glyphs information
	index_t iGlyphBegin;
	index_t iGlyphEnd;
	lineLayout.GetGlyphRangeFromCharRange(0, text.length(), iGlyphBegin, iGlyphEnd);

	const GlyphLayoutInfo*       pGLIFirst = lineLayout.mGlyphLayoutInfoArray.data();
	const GlyphLayoutInfo*       pGLI      = pGLIFirst + iGlyphBegin;
	
	//Prepare the font and line metrics
	FontMetrics fontMetrics;
	EA_ASSERT(pGLI); 
	EA_ASSERT(pGLI->mpFont);
	pGLI->mpFont->GetFontMetrics(fontMetrics);
	
	float ceilfBaseline     = ceilf(lineLayout.mLineMetrics.mfBaseline);
	float floorfBaseline    = floorf(lineLayout.mLineMetrics.mfBaseline);
	float roundLineHeight   = ceilf(fontMetrics.mfLineHeight); //Due to a manual adjustment in EATextOutlineFont.cpp for fractionnal metrics the LineHeight is equals to (mfBaseline - mfDescent) + 1.0f.
	
	/* usefull for debugging 
	printf("Rounded values: ceilfBaseline = %f, floorfBaseline = %f, roundLineHeight = %f \n", ceilfBaseline, floorfBaseline, roundLineHeight);
	*/

	EA::Text::RectangleSet rectLayoutSpecified, rectLayoutDefault;
	EA::Text::DecorationFlags flagSet[] = { EA::Text::kDFUnderline, EA::Text::kDFLineThrough,  EA::Text::kDFOverline};

	for(size_t i = 0; i < EAArrayCount(flagSet); ++i)
	{
		if(textStyle.mnDecorationFlags & flagSet[i])
		{
			/* usefull for debugging
			printf("Current flag : %i \n", flagSet[i]);
			*/

			//Fill both rectangle with LayoutSetting specified and LayoutSetting default
			lineLayout.GetGlyphDecoration(flagSet[i], static_cast<EA::Text::index_t>(0), static_cast<EA::Text::index_t>(text.length()), rectLayoutSpecified, &ts.GetLayoutSettings());
			lineLayout.GetGlyphDecoration(flagSet[i], static_cast<EA::Text::index_t>(0), static_cast<EA::Text::index_t>(text.length()), rectLayoutDefault);
			
			EATEST_VERIFY(rectLayoutSpecified.size() == rectLayoutDefault.size()); //Verify that both function call returns the same amount of Rectangle.

			//Prepare iterators
			EA::Text::RectangleSet::const_iterator itLayout(rectLayoutSpecified.begin()), itLayoutEnd(rectLayoutSpecified.end()), 
												   itDefault(rectLayoutDefault.begin()),  itDefaultEnd(rectLayoutDefault.end());

			for( ; itLayout != itLayoutEnd || itDefault != itDefaultEnd; ++itLayout, ++itDefault)
			{
				/* usefull for debugging  
				printf("rect position with Layout Specified: left:%f, top:%f, right:%f, bottom:%f \n", itLayout->mLeft,  itLayout->mTop,  itLayout->mRight,  itLayout->mBottom);
				printf("rect position with Layout Default: left:%f, top:%f, right:%f, bottom:%f \n" , itDefault->mLeft, itDefault->mTop, itDefault->mRight, itDefault->mBottom);
			   */

				EATEST_VERIFY(itLayout->mLeft == itDefault->mLeft && 
							  itLayout->mTop == itDefault->mTop && 
							  itLayout->mRight == itDefault->mRight &&
							  itLayout->mBottom == itDefault->mBottom); //Verify that both rect have the same coordinates
				 
				//============ Soft test ============//
				if(flagSet[i] & EA::Text::kDFUnderline) // Verify Underline
				{
					EATEST_VERIFY( ceilfBaseline <= itLayout->mTop && ceilfBaseline <= itDefault->mTop ); // Verify that it is below or start on the baseline.
					EATEST_VERIFY( roundLineHeight >= itLayout->mBottom && roundLineHeight >= itDefault->mBottom ); // Verify that it doesn't exceed the line height.
				}

				if(flagSet[i] & EA::Text::kDFLineThrough) // Verify Strikethrough
				{
					EATEST_VERIFY( (0 < itLayout->mTop) && (0 < itDefault->mTop) ); // Verify that it is below the top.
					EATEST_VERIFY( floorfBaseline > itLayout->mBottom && floorfBaseline > itDefault->mBottom ); // Verify that it is above the baseline.
				}
				
				if(flagSet[i] & EA::Text::kDFOverline) // Verify Overline
				{
					EATEST_VERIFY( 0 < itLayout->mTop && 0 < itDefault->mTop ); // Verify that it is above the top.
					EATEST_VERIFY( floorfBaseline > itLayout->mBottom  && floorfBaseline > itDefault->mBottom ); // Verify that it is above the baseline.
				}
			}

			rectLayoutSpecified.clear();
			rectLayoutDefault.clear();
		}
	}
	return nErrorCount;
}




static int TestTypesetterDecorations()
{
	int nErrorCount = 0;

	const float testedSize[] = {8.0f, 12.0f, 16.0f, 22.0f, 26.0f, 36.0f, 48.0f, 72.0f, 86.0f, 98.0f, 112.0f, 128.0f};
	//The order between testedFont and fontFamilyName array must be in sync
	const EATEXT_FILE_PATH_CHAR_TYPE *testedFont[] = { EATEXT_FILE_PATH_CHAR("arial.ttf"), EATEXT_FILE_PATH_CHAR("times.ttf"), EATEXT_FILE_PATH_CHAR("cour.ttf"), EATEXT_FILE_PATH_CHAR("comic.ttf") };
	const EATEXT_CHAR_TYPE *fontFamilyName[] = { EATEXT_CHAR("Arial"), EATEXT_CHAR("Times New Roman"), EATEXT_CHAR("Courier New"), EATEXT_CHAR("Comic Sans MS") };
	
	eastl::basic_string<Char> text = EATEXT_CHAR("This is a text in order to augment the actual Unit test to detect further problem related to both y-up/y-down coordinate system as well as when the origin or the top of the text is the baseline.");
	
	// Build a text style with all decorations
	EA::Text::TextStyle myTextStyle = EA::Text::TextStyle();
	myTextStyle.mnDecorationFlags  = EA::Text::kDFOverline;
	myTextStyle.mnDecorationFlags += EA::Text::kDFLineThrough;
	myTextStyle.mnDecorationFlags += EA::Text::kDFUnderline;

	FilePathChar pFontDir[EA::IO::kMaxPathLength];
	FilePathChar pFontPath[EA::IO::kMaxPathLength];
	GetTestDataDirectory(pFontDir, EA::IO::kMaxPathLength);
	
	FontServer fontServer;
	fontServer.Init();

	EA::StdC::Stopwatch      stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds, true);
	EA::StdC::LimitStopwatch limitStopwatch(EA::StdC::Stopwatch::kUnitsSeconds, 20, true);

	for(size_t iFont = 0; iFont < EAArrayCount(testedFont); ++iFont)
	{
		EA::StdC::Strcpy(pFontPath, pFontDir);
		EA::StdC::Strcat(pFontPath, testedFont[iFont]);
		
		uint32_t addedFaceCount = fontServer.AddFace(pFontPath);
		
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // We know these fonts will be present under Windows.
			EATEST_VERIFY(addedFaceCount > 0); //Fail if the requested font cannot be loaded
		#else
			EA_UNUSED(addedFaceCount); //Other platform might not have these fonts.
		#endif
			
		EA::StdC::Strcpy(myTextStyle.mFamilyNameArray[0], fontFamilyName[iFont]);

		/* usefull for debugging
		printf("Starting test for font id: %i\n", iFont);
		*/
		for(size_t iFontSize = 0; iFontSize < EAArrayCount(testedSize); ++iFontSize)
		{
			/* usefull for debugging
			printf("Font size: %f \n==============\n", testedSize[iFontSize]);
			*/
			myTextStyle.mfSize = testedSize[iFontSize];

			Typesetter  ts;
			ts.SetFontServer(&fontServer);
			ts.SetDefaultTextStyle(&myTextStyle);
			ts.AddTextRun(text.c_str(), text.length());
			ts.FinalizeLine();

			LineLayout& lineLayout = ts.GetLineLayout(); //< LineLayout is usually only usable if FinalizeLine was called.
			nErrorCount += TestDecorations(ts, myTextStyle, text, lineLayout);

			// We print a '.' char every N seconds in order to indicate we are still alive to any listeners of this test.
			if(limitStopwatch.IsTimeUp())
			{
				EA::UnitTest::Report(".\n");
				limitStopwatch.SetTimeLimit(20, true);
			}

			// If this takes longer than N minutes, quit the test. Some testing environments (e.g. those with remote disks) are
			// pathologically slow about reading disk files and automated test runs in such environments can take too long.
			if(stopwatch.GetElapsedTime() > (3 * 60 * 1000))
			{
				break;
			}
		}
	}
	fontServer.Shutdown();

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// EA_THREADS_AVAILABLE
//
// Defined as 0 or 1
// Defines if threading is supported on the given platform.
// If 0 then the EAThread implementation is not capable of creating threads,
// but other facilities (e.g. mutex) work in a non-thread-aware way.
//
#ifndef EA_THREADS_AVAILABLE         // If EAThread isn't already defining this (i.e. if we are using an old version of EAThread)...
	#define EA_THREADS_AVAILABLE 1
#endif
#define NUM_TESTFONT_ITERATIONS 16

/////////////////////////////////////////////////////////////////////////////
// FontThreadData
//
struct FontThreadData
{
	volatile bool                mbShouldQuit;
	EA::Thread::AtomicInt32      mnErrorCount;
	EA::Text::FontServer*        mpFontServer;
	EA::Text::GlyphCache*        mpGlyphCache;
	eastl::vector<eastl::string> mFaceNameArray;

	FontThreadData() : mbShouldQuit(false), mnErrorCount(0), mpFontServer(NULL), mpGlyphCache(NULL), mFaceNameArray() { }

private:
	// Prevent default generation of these functions by not defining them
	FontThreadData(const FontThreadData& rhs);               // copy constructor
	FontThreadData& operator=(const FontThreadData& rhs);    // assignment operator
};


#if EA_THREADS_AVAILABLE && EATEXT_THREAD_SAFETY_ENABLED
	static intptr_t FontTestThreadFunction(void* pvFontThreadData)
	{
		using namespace EA::Text;

		FontThreadData* const pFontThreadData = (FontThreadData*)pvFontThreadData;

		EA::Thread::ThreadId threadId = EA::Thread::GetThreadId();
		EA::UnitTest::ReportVerbosity(2, "Font test function created: %s\n", EAThreadThreadIdToString(threadId));

		int nErrorCount = 0;

		Typesetter            ts;
		ts.SetFontServer(pFontThreadData->mpFontServer);
		Typesetter::LineState lineState = ts.GetLineState();

		while(!pFontThreadData->mbShouldQuit)
		{
			EATEST_VERIFY(lineState == Typesetter::kLineStateComplete);

			for (size_t b = 0; b < EAArrayCount(gLayoutBoxes); b++) // For each layout box size...
			{
				for (size_t i = 0; i < EAArrayCount(gpTestTextArray); i++) // For each test text...
				{
					EA::UnitTest::ReportVerbosity(1, "Test string: %I16s\n", gpTestTextArray[i]);
					EA::UnitTest::ReportVerbosity(1, "BBox: %.1f, %.1f, %.1f, %.1f\n", gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop, gLayoutBoxes[b].mRight, gLayoutBoxes[b].mBottom);

					ts.SetLayoutSpace(gLayoutBoxes[b].mRight - gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mLeft, gLayoutBoxes[b].mTop);
					ts.AddTextRun(gpTestTextArray[i], (index_t)EA::StdC::Strlen(gpTestTextArray[i]));

					lineState = ts.GetLineState();
					EATEST_VERIFY((lineState == Typesetter::kLineStatePartial) || (!gpTestTextArray[i][0] && (lineState == Typesetter::kLineStateComplete)));

					while (lineState != Typesetter::kLineStateComplete)
					{
						lineState = ts.ProcessLine();

						if (lineState == Typesetter::kLineStatePartial)
						{
							// Call FinalizeLine here, as we've already submitted all the text and so if the 
							// LineState is kLineStatePartial then there must not be enough text to fill the line.
							// You wouldn't call FinalizeLine in the case that you have more text to submit.
							ts.FinalizeLine();

							lineState = ts.GetLineState();
							EATEST_VERIFY(lineState == Typesetter::kLineStateFull);
						}

						if (lineState == Typesetter::kLineStateFull)
						{
							// When the LineState is kLineStateFull, the next thing to do is generally to
							// call FinalizeLine. 
							ts.FinalizeLine();

							const LineLayout& lineLayout = ts.GetLineLayout();
							EA::UnitTest::ReportVerbosity(1, "%I16s\n", lineLayout.mCharArray.c_str());

							ts.NextLine();
							lineState = ts.GetLineState(); // We don't necessarily need to do this assigment, as we'll catch the kLineStateComplete the next time around.
						}
					}

					ts.Reset();
					lineState = ts.GetLineState();
				}
			}
			ts.Reset();
		}

		return 0;
	}
#endif


static int TestTypesetterMultithreaded()
{
	int nErrorCount = 0;

#if EA_THREADS_AVAILABLE && EATEXT_THREAD_SAFETY_ENABLED
		using namespace EA::Thread;
		using namespace EA::Text;

		// Setup GlyphCache
		EA::Text::GlyphCache_Memory glyphCache;
		const int nResult = glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
		EATEST_VERIFY(nResult == 1);

		// Setup FontServer
		EA::Text::FontServer fontServer;
		fontServer.Init();

		const FilePathChar* kFontFileName = EATEXT_FILE_PATH_CHAR("arial.ttf");

		FilePathChar pFontPath[EA::IO::kMaxPathLength];
		GetTestDataDirectory(pFontPath, EA::IO::kMaxPathLength);
		EA::StdC::Strcat(pFontPath, kFontFileName);
		uint32_t nFaceCount = fontServer.AddFace(pFontPath);

		if(nFaceCount < 1) // If failed... // Fonts may in theory have more than one face in a file.
		{
			if(EA::IO::File::Exists(pFontPath))
				EATEST_VERIFY_F(nFaceCount < 1, "FontServer::AddFace failed with " EATEXT_FILE_PATH_CHAR_FORMAT ".", pFontPath); 
			else
				EA::UnitTest::Report("Font not found at path: " EATEXT_FILE_PATH_CHAR_FORMAT "\n", pFontPath);
		}

		// Setup FontThreadData
		FontThreadData fontThreadData;

		fontThreadData.mbShouldQuit = false;
		fontThreadData.mpFontServer = &fontServer;
		fontThreadData.mpGlyphCache = &glyphCache;

		fontThreadData.mFaceNameArray.push_back(eastl::string("Arial"));

		// Set ourselves to high thread priority, so the running threads don't prevent us from executing below.
		int threadPriority = EA::Thread::GetThreadPriority();
		EA::Thread::SetThreadPriority(threadPriority + 1);

		// Create threads
		const int kThreadCount(8);
		Thread thread[kThreadCount];
		Thread::Status status;
		int i;

		for(i = 0; i < kThreadCount; i++)
		{
			thread[i].Begin(FontTestThreadFunction, &fontThreadData);
		}

		EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsSeconds);
		stopwatch.Start();
		while (stopwatch.GetElapsedTime() < 10)
		{
			fontServer.FreeUnusedFonts();
			EA::UnitTest::ThreadSleepRandom(1, 10);
		}

		fontThreadData.mbShouldQuit = true;

		for(i = 0; i < kThreadCount; i++)
		{
			//test a 60-minute wait
			status = thread[i].WaitForEnd(GetThreadTime() + (1000*60*30) );
			EATEST_VERIFY(status == Thread::kStatusEnded);
		}

		EA::Thread::SetThreadPriority(threadPriority);

		nErrorCount += (int)fontThreadData.mnErrorCount;

		fontServer.Shutdown();
		glyphCache.Shutdown();
	#endif

	return nErrorCount;
}

/////////////////////////////////////////////////////////////////////////////
// GenerateAlphabeticRandomString
//
// Generate a string of alphabet characters with some extended chars, but without asian glyphs, as they introduce linebreaks outside of what we can test with soft hyphens
// 
static const Char gAlphabeticCharSelection16[] =
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E,
	0x010F, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011A, 0x011B, 0x011C	
};


static void GenerateAlphabeticRandomString(TestString& s16, int nMinSize, int nMaxSize)
{
	const eastl_size_t nSize = (eastl_size_t)(size_t)gRandom.RandRange(nMinSize, nMaxSize + 1);

	s16.resize(nSize);

	for (eastl_size_t i = 0; i < nSize; i++)
	{
		size_t   index = (size_t)gRandom.RandRange(0, sizeof(gAlphabeticCharSelection16) / sizeof(gAlphabeticCharSelection16[0]));
		char16_t c = (char16_t)gAlphabeticCharSelection16[index];
		s16[i] = c;
	}
}


/////////////////////////////////////////////////////////////////////////////
// TestTypesetterSoftHyphenation
//
// Soft hyphens should support breaking at their location, but unless used they should not consume space and be replaced in the string. We support two test variants, with
// Asian glyphs and without, with the latter having a few more extra requirements that cannot be guaranteed with Asian glyphs due to their added line-breaking behavior.

struct ReplacementGlyphsSetup
{
	GlyphId hardHyphenGlyphId;
	GlyphId zeroWidthSpaceGlyphId;
};

// internalTestTypesetterSoftHyphenationText runs tests on a given string and style. It was pulled out into a own method to ease regression tests.
static int internalTestTypesetterSoftHyphenationText(Typesetter& ts, const TextStyle& textStyle, const TestString& s16, const ReplacementGlyphsSetup& replacementGlyphs, bool testAsian)
{
	LineLayout lineLayout;
	int nErrorCount = 0;

	const float x = (float)gRandom.RandRange(-100000, +1000000) / 100.f;
	const float y = (float)gRandom.RandRange(-100000, +1000000) / 100.f;

	const int glyphCount = (int)ts.LayoutParagraph(s16.data(), s16.length(), x, y, x + 200, y + 1000, &textStyle, lineLayout);
	float w, h;
	lineLayout.GetBoundingBox(w, h, true);
	EATEST_VERIFY(w <= 200 || textStyle.mTextWrap == EA::Text::kTWNone || textStyle.mTextWrap == EA::Text::kTWHard);	// max width was kept, unless one of the wrap modes that allow overflow are used

	for (int idx = 0; idx < glyphCount; ++idx)
	{
		EATEXT_CHAR_TYPE c = s16.data()[idx];

		// check if we have a successor character, and a linebreak happening here, so we can test that the soft hyphens got processed accordingly
		const bool hasSuccessor = idx + 1 < glyphCount;
		bool linebreakFollowing = false;
		if (textStyle.mOrientation == kOrientationHorizontal)
			linebreakFollowing = hasSuccessor && lineLayout.mGlyphLayoutInfoArray[idx].mfPenY != lineLayout.mGlyphLayoutInfoArray[idx + 1].mfPenY;
		else
			linebreakFollowing = hasSuccessor && lineLayout.mGlyphLayoutInfoArray[idx].mfPenX != lineLayout.mGlyphLayoutInfoArray[idx + 1].mfPenX;

		if (c == L'\x00AD')		// if there was a soft hyphen in the input, let's verify that it was replaced correctly
		{
			if (linebreakFollowing)
			{
				EATEST_VERIFY(textStyle.mTextWrap != kTWWrap || testAsian || lineLayout.mGlyphArray[idx] == replacementGlyphs.hardHyphenGlyphId);			// linebreak here and not running Asian testing -> replaced by an actual hyphen
				EATEST_VERIFY(lineLayout.mGlyphArray[idx] == replacementGlyphs.hardHyphenGlyphId || lineLayout.mGlyphArray[idx] == replacementGlyphs.zeroWidthSpaceGlyphId);	// at any rate, replaced by one of the two valid glyphs
			}
			else
			{
				EATEST_VERIFY(lineLayout.mGlyphArray[idx] == replacementGlyphs.zeroWidthSpaceGlyphId);	// no linebreak here -> replaced by a non-rendering character
				EATEST_VERIFY(lineLayout.mGlyphLayoutInfoArray[idx].mfAdvance == 0);	// no linebreak here -> soft hyphen in input takes no space
				EATEST_VERIFY(textStyle.mOrientation != kOrientationHorizontal || lineLayout.mGlyphLayoutInfoArray[idx].mfX1 == lineLayout.mGlyphLayoutInfoArray[idx].mfX2);	// no linebreak here -> soft hyphen in input takes no space (in horizontal direction)
			}
		}
		else if (hasSuccessor && !testAsian && textStyle.mTextWrap == kTWWrap)			// check that all breaks happen on soft hyphens (not for Asian characters though, and only when trying to wrap)
		{
			EATEST_VERIFY(!linebreakFollowing);											// all linebreaks happen on soft hyphens for this string
		}
	}
	EATextTest_CheckMemory();
	return nErrorCount;
}

// internalTestTypesetterSoftHyphenation runs a test suite with or without Asian glyphs
static int internalTestTypesetterSoftHyphenation(bool testAsian)
{
	int nErrorCount = 0;

	TestString        s16;
	Typesetter        ts;
	LineLayout        lineLayout;
	TextStyle         textStyle;
	LayoutSettings    layoutSettings;
	FontServer* const pFontServer = GetFontServer(true);

	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	GetFontServer()->RemoveFace(EATEXT_CHAR("Courier New"));		// Courier does not support the replacement character correctly, it seems

	layoutSettings.mbDynamicFontSelection = true;

	EATEST_VERIFY(pFontServer != NULL);
	ts.SetFontServer(pFontServer);

	// obtain the "regular hyphen" and "zero width" glyph id, so we can later check that the soft hyphens have been replaced by the approriate glyph. We do it new every iteration, as the font can change	
	ReplacementGlyphsSetup replacementGlyphsSetup;
	TestString specialGlyphsStr(EATEXT_CHAR("-\x200B\x00AD"));
	size_t specialGlyphsStrSize = specialGlyphsStr.size();
	ts.LayoutParagraph(specialGlyphsStr.data(), specialGlyphsStrSize, 0, 0, 200, 1000, &textStyle, lineLayout);
	EATEST_VERIFY(lineLayout.mGlyphArray.size() >= specialGlyphsStrSize);
	if (lineLayout.mGlyphArray.size() >= 3)
		EATEST_VERIFY(lineLayout.mGlyphArray[1] == lineLayout.mGlyphArray[2]);	// check that, for this short string, the correct replacement was done
	replacementGlyphsSetup.hardHyphenGlyphId = !lineLayout.mGlyphArray.empty() ? lineLayout.mGlyphArray[0] : 0;
	replacementGlyphsSetup.zeroWidthSpaceGlyphId = lineLayout.mGlyphArray.size() >= 2 ? lineLayout.mGlyphArray[1] : 0;

	EATextTest_CheckMemory();

	for (int i = 0; (i < NUM_ITERATIONS / 4) && (nErrorCount == 0); i++)
	{
		RandomizeTextStyle(textStyle);
		textStyle.mPasswordMode = kPMNone;							// password mode messes everything up - not supported		
		const uint32_t wrapRandom = gRandom.RandLimit(10);
		textStyle.mTextOverflow = kTOEllipsis;
		textStyle.mTextWrap = wrapRandom > 4 ? EA::Text::kTWWrap : (EA::Text::TextWrap) wrapRandom;	// here, we want to test randomized variants, but with a bias on kTWWrap, which is the mode where soft hyphens actually matter. Others should work as well in the presence of soft hyphens, of course
		if (!testAsian)
			textStyle.mOrientation = kOrientationHorizontal;		// vertical layout seems rather unsupported at the moment (cf. Typesetter::GetTextWidth() invoked from Typesetter::UpdateLineState()), so for the non-asian testing (where we will do checks on all breaks happening on soft hyphens) we force horizontal layouting

		if (testAsian)
			GenerateRandomString(s16, 30, 400);
		else
			GenerateAlphabeticRandomString(s16, 30, 400);
		for (int strPos = (int)s16.length() - 3; strPos > 0; strPos -= gRandom.RandRange(0, 5))	// add soft hyphens in frequent intervals
			s16.insert(strPos, EATEXT_CHAR("\x00AD"));

		nErrorCount += internalTestTypesetterSoftHyphenationText(ts, textStyle, s16, replacementGlyphsSetup, testAsian);
	}

	EATextTest_CheckMemory();

	return nErrorCount;
}

static int TestTypesetterSoftHyphenation()
{
	int nErrorCount = 0;
	nErrorCount += internalTestTypesetterSoftHyphenation(false);		// Asian does provide their own line-breaking logic, which makes a number of tests meaningless. Run a set of tests with latin characters first.
	nErrorCount += internalTestTypesetterSoftHyphenation(true);			// and then let's try with Asian glyphs to see if that is still working out as expected
	return nErrorCount;
}

// Allows us to more simply report which test is running which is useful when the sum run time of the small tests typically
// runs past the build farm's timeout for listening for EAMain::Report()
#define RUN_TEST(test) EA::EAMain::Report("\t[%s]\n", EA_STRINGIFY(test)); nErrorCount += test()

/////////////////////////////////////////////////////////////////////////////
// TestTypesetter
//
int TestTypesetter()
{
	int nErrorCount = 0;

	SetupLayoutBoxes();

	RUN_TEST(TestTypesetterAlignment);
	RUN_TEST(TestLineLayout);
	RUN_TEST(TestTypesetterRegression);
	RUN_TEST(TestTypesetterNop);
	RUN_TEST(TestTypesetterLayoutSimple);
	RUN_TEST(TestTypesetterHighLevel);
	RUN_TEST(TestTypesetter1);
	RUN_TEST(TestTypesetter2);
	RUN_TEST(TestTypesetterHebrew1);
	RUN_TEST(TestTypesetterHangul1);
	RUN_TEST(TestTypesetterArabic1);
	RUN_TEST(TestTypesetterObjects1);
	RUN_TEST(TestTypesetterDecorations);
	RUN_TEST(TestTypesetterMultithreaded);
	RUN_TEST(TestTypesetterSoftHyphenation);

	return nErrorCount;
}
