/////////////////////////////////////////////////////////////////////////////
// TestSpeed.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextTypesetter.h>
#include <eathread/eathread.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EATest/EATest.h>


using namespace EA::Text;


namespace
{
	const uint32_t kTextSize = 500;

	// Just over 1300 characters
	const Char* const pSpeedTestLatin = EATEXT_CHAR("I am the First Accused.\nI hold a Bachelor's Degree in Arts and practised as an attorney in Johannesburg for a number of years in partnership with Oliver Tambo. I am a convicted prisoner serving five years for leaving the country without a permit and for inciting people to go on strike at the end of May 1961.\nAt the outset, I want to say that the suggestion made by the State in its opening that the struggle in South Africa is under the influence of foreigners or communists is wholly incorrect. I have done whatever I did, both as an individual and as a leader of my people, because of my experience in South Africa and my own proudly felt African background, and not because of what any outsider might have said.\nIn my youth in the Transkei I listened to the elders of my tribe telling stories of the old days. Amongst the tales they related to me were those of wars fought by our ancestors in defence of the fatherland. The names of Dingane and Bambata, Hintsa and Makana, Squngthi and Dalasile, Moshoeshoe and Sekhukhuni, were praised as the glory of the entire African nation. I hoped then that life might offer me the opportunity to serve my people and make my own humble contribution to their freedom struggle. This is what has motivated me in all that I have done in relation to the charges made against me in this case." );

} // namespace



/////////////////////////////////////////////////////////////////////////////
// TestSpeedLayoutSimple
//
void TestSpeedLayoutSimple()
{
	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsMicroseconds);

	LayoutSettings layoutSettings;

	TextStyle textStyle;
	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	FontServer* const pFontServer = GetFontServer(true);
	pFontServer->GetFont(&textStyle, layoutSettings.mFontSelection, kScriptLatin, true);

	LineLayout lineLayout;
	lineLayout.mGlyphArray.resize(2048);
	lineLayout.mGlyphLayoutInfoArray.resize(2048);

	for(int i = 0; i < 2; i++)
	{
		stopwatch.Restart();
		LayoutSimple(pSpeedTestLatin, kTextSize, 0, 0, NULL, &lineLayout.mGlyphArray[0], 
					 &lineLayout.mGlyphLayoutInfoArray[0], &layoutSettings);
		stopwatch.Stop();

		if(i == 1)
			EA::UnitTest::ReportVerbosity(1, "TestSpeedLayoutSimple: %u microseconds\n", stopwatch.GetElapsedTime());
	}
}



/////////////////////////////////////////////////////////////////////////////
// TestSpeedLayoutLine
//
void TestSpeedLayoutLine()
{
	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsMicroseconds);

	LayoutSettings layoutSettings;
	Typesetter     typesetter;

	TextStyle textStyle;
	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	FontServer* const pFontServer = GetFontServer(true);
	pFontServer->GetFont(&textStyle, layoutSettings.mFontSelection, kScriptLatin, true);
	layoutSettings.mbDynamicFontSelection = false; // Just always use the Font in mFontSelection.

	typesetter.SetLayoutSettings(layoutSettings);
	typesetter.LayoutLine(pSpeedTestLatin, kTextSize, 0, 0, &textStyle);

	for(int i = 0; i < 2; i++)
	{
		stopwatch.Restart();
		typesetter.LayoutLine(pSpeedTestLatin, kTextSize, 0, 0, &textStyle);
		stopwatch.Stop();

		if(i == 1)
			EA::UnitTest::ReportVerbosity(1, "TestSpeedLayoutLine: %u microseconds\n", stopwatch.GetElapsedTime());
	}
}



/////////////////////////////////////////////////////////////////////////////
// TestSpeedLayoutParagraph
//
void TestSpeedLayoutParagraph()
{
	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsMicroseconds);

	LayoutSettings layoutSettings;
	Typesetter     typesetter;
	LineLayout     lineLayout;

	TextStyle textStyle;
	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	FontServer* const pFontServer = GetFontServer(true);
	pFontServer->GetFont(&textStyle, layoutSettings.mFontSelection, kScriptLatin, true);
	layoutSettings.mbDynamicFontSelection = false; // Just always use the Font in mFontSelection.
	typesetter.SetLayoutSettings(layoutSettings);

	for(int i = 0; i < 2; i++)
	{
		stopwatch.Restart();
		typesetter.LayoutParagraph(pSpeedTestLatin, kTextSize, 0, 0, 400, 10000, &textStyle, lineLayout, 
								   LineLayout::kComponentGlyphArray | LineLayout::kComponentGlyphLayoutInfoArray);
		stopwatch.Stop();

		if(i == 1)
			EA::UnitTest::ReportVerbosity(1, "TestSpeedLayoutParagraph: %u microseconds\n", stopwatch.GetElapsedTime());
	}
}



/////////////////////////////////////////////////////////////////////////////
// TestSpeedLayoutParagraphManual
//
void TestSpeedLayoutParagraphManual()
{
	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsMicroseconds);

	LayoutSettings        layoutSettings;
	Typesetter            typesetter;
	LineLayout            lineLayout;
	Typesetter::LineState lineState;

	TextStyle textStyle;
	textStyle.mfSize = 12;
	EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));

	FontServer* const pFontServer = GetFontServer(true);
	pFontServer->GetFont(&textStyle, layoutSettings.mFontSelection, kScriptLatin, true);
	layoutSettings.mbDynamicFontSelection = false; // Just always use the Font in mFontSelection.

	typesetter.SetLayoutSettings(layoutSettings);
	typesetter.SetLayoutSpace(400, 0, 0);
	typesetter.SetDefaultTextStyle(&textStyle);

	for(int i = 0; i < 2; i++)
	{
		typesetter.Reset();

		stopwatch.Restart();

		typesetter.AddTextRun(pSpeedTestLatin, kTextSize);

		for(lineState  = typesetter.GetLineState(); 
			lineState != Typesetter::kLineStateComplete; 
			lineState  = typesetter.GetLineState())
		{
			typesetter.FinalizeLine();
			typesetter.NextLine();
		}

		stopwatch.Stop();

		 if(i == 1)
			EA::UnitTest::ReportVerbosity(1, "TestSpeedLayoutParagraphManual: %u microseconds\n", stopwatch.GetElapsedTime());
	}
}



/////////////////////////////////////////////////////////////////////////////
// TestSpeed
//
int TestSpeed()
{
	#ifndef EA_DEBUG
		EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault + 3);
	#endif

	TestSpeedLayoutSimple();
	TestSpeedLayoutLine();
	TestSpeedLayoutParagraph();
	TestSpeedLayoutParagraphManual();
	//TestSpeedLayoutParagraphHebrew();
	//TestSpeedLayoutParagraphArabic();

	#ifndef EA_DEBUG
		EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault);
	#endif

	return 0;
}





















