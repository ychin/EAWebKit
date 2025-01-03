/////////////////////////////////////////////////////////////////////////////
// TestBaseline.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EATest/EATest.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextBaseline.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextCache.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_string.h>
#include <EAStdC/EAString.h>
#include <MemoryMan/MemoryMan.h>

// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const uint8_t pPalormSmallCapsTTF[] = { 0x23, 0xa2, ... }; 
#include "data/ParolmSmallCaps.ttf.inl"

using namespace EA::Text;

#ifndef MIN
	#define MIN(a, b) ((a < b) ? a : b)
	#define EATEXT_TEST_MIN_DEFINED
#endif

int FollowBaseLineTest(Font* const pFont, const EATEXT_CHAR_TYPE* text, const size_t textLen, Bezier2DIterator& bezierIt, TextStyle& textStyle)
{
	int nErrorCount = 0;

	Typesetter ts;
	LineLayout lineLayout;
	ts.SetDefaultTextStyle(&textStyle);

	size_t numGlyphs = ts.LayoutParagraph(text, textLen, 0, 0, 1000, 1000, &textStyle, lineLayout, EA::Text::LineLayout::kComponentAll);

	// Curve the text.
	// Specifically we will curve the text so that the text will be a left-to-right arc from about 10 o'clock to 2 o'clock
	BaselineLayout    baselineLayout;
	eastl::vector<GlyphLayoutInfoEx> gliArray;
	gliArray.resize(numGlyphs); // explicitly resize, not reserve since we will be referencing the end of the array by value

	baselineLayout.FollowBaseline(&lineLayout, gliArray.data(), &bezierIt, 0.f, 0.f, kHACenter, kDirectionLTR, textStyle.mfLetterSpacing);

	// Let's look at the first and last glyphs which we expect to be roughly the same y position
	GlyphLayoutInfoEx beginGlyph = gliArray[0];
	GlyphLayoutInfoEx endGlyph = gliArray[textLen - 1];

	float avgGlyphHeight = 0.f;
	for (eastl_size_t i = 0, iEnd = lineLayout.mGlyphLayoutInfoArray.size(); i < iEnd; i++)
	{
		const GlyphLayoutInfo& gli = lineLayout.mGlyphLayoutInfoArray[i];

		avgGlyphHeight += fabsf(gli.mfY1 - gli.mfPenY);
	}
	avgGlyphHeight = avgGlyphHeight / textLen;

	// Verify the end glyph's y position is within one avg glyph height of the begin glyph's y position
	float err = avgGlyphHeight;
	float beginY = MIN(beginGlyph.mfY3, beginGlyph.mfY4);
	float endY = MIN(endGlyph.mfY3, endGlyph.mfY4);
	bool isBalanced = fabs(beginY - endY) < err;
	EATEST_VERIFY_F(isBalanced, "Curving Text '%ls' failed: (%0.02f - %0.02f) < %0.02f && (%0.02f + %0.02f) > %0.02f)", text, beginY, err, endY, beginY, err, endY);


	// Useful print to display the glyphs in a web browser to help visualize what this test is trying to do. 
	// Dump the output into an html file or here http://www.w3schools.com/graphics/svg_intro.asp via "Try it yourself" button
#if 0
	if (!isBalanced)
	{
		///             [x2,y2]
		/// [x1,y1]    *
		///       *    **
		///        ** *  *  
		///         *     * [x3,y3]
		///          *
		///        [x4,y4]
		///
		float xOffset = 450.f;
		float yOffset = 500.f;
		size_t i = 0;

		EA::EAMain::Report("\n%ls:\n<!DOCTYPE html><html><body><svg height=\"1000\" width=\"1000\">", text);
		for (auto layoutInfo : gliArray)
		{
			EA::EAMain::Report("<polygon points=\"");
			EA::EAMain::Report("%f,%f %f,%f %f,%f %f,%f",
				layoutInfo.mfX4 + xOffset, layoutInfo.mfY4 + yOffset, layoutInfo.mfX3 + xOffset, layoutInfo.mfY3 + yOffset,
				layoutInfo.mfX2 + xOffset, layoutInfo.mfY2 + yOffset, layoutInfo.mfX1 + xOffset, layoutInfo.mfY1 + yOffset);
			EA::EAMain::Report("\" style=\"fill:%s;stroke:purple;stroke-width:1\" />\n", (i == 0 || i == textLen - 1) ? "none" : "none");
			i++;
		}
		EA::EAMain::Report("</svg></body></html>");
	}
#endif

	return nErrorCount;
}

/////////////////////////////////////////////////////////////////////////////
// TestBaseline
//
int TestBaseline()
{
	int nErrorCount = 0;

	// Layout a curved string of text and ensure that the text is approximately centered.
	{
		GlyphCache_Memory glyphCache;
		FontServer        fontServer;

		glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
		fontServer.Init();
		{
			EA::IO::MemoryStream* pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
			fontServer.AddFace(pMemoryStream, kFontTypeOutline);

			TextStyle textStyle;
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Parolm"));
			Font* const pFont = fontServer.GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			pFont->SetTransform(30);

			Bezier2DIterator bezierIt;
			bezierIt.MakeSemicircle(Point(0, 0), 200, 3.1415f, false);
			// Layout some text.
			// This text is specifically chosen as it is known that upon placing it's glyphs along
			// the semi-circle baseline, some glyphs will overlap and need to be shifted.
			//
			// We defined a char-array because c++ is silly and doesn't have a widely 
			// supported standard for computing string length at compile time
			const EATEXT_CHAR_TYPE kTextString[] = { 'T', 'h', 'e', ' ', 'R', 'e', 'a', 'd', 'i', 'n', 'g', ' ',
													 'R', 'a', 'i', 'n', 'b', 'o', 'o', 'o', 'o', 'o', 'w'};
			const size_t kTextStringLen = EAArrayCount(kTextString);
			nErrorCount += FollowBaseLineTest(pFont, kTextString, kTextStringLen, bezierIt, textStyle);

			// Release font from GetFont() call
			pFont->Release();
		}

		fontServer.Shutdown();
		glyphCache.Shutdown();
	}
	
	// Regression Test for an issue found in Tunguska.
	// Tunguska isn't sure which player name was to blame, but one of the names when adjusted to follow
	// a curved baseline would infinitely recurse due to the FollowBaseLine loop exit condition never being met. They recursion was
	// not something they could produce on any platform other than PS4 which seems odd and slightly unrelated, however they did
	// highlight that our follow baseline code cannot cope with explicit letter spacing text styles. We test that here and also
	// attempt to catch infinite recursion issues should they arise again.
	{
		GlyphCache_Memory glyphCache;
		FontServer        fontServer;

		glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
		fontServer.Init();
		{
			FilePathChar pFontPath[EA::IO::kMaxPathLength];
			GetTestDataDirectory(pFontPath, EA::IO::kMaxPathLength);
			EA::StdC::Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("TunguskaFuturaStdMedium.ttf"));
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

			TextStyle textStyle;
			textStyle.mfSize = 12.f;
			textStyle.mfLetterSpacing = 6.0f;

			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("futura std medium"));
			Font* const pFont = fontServer.GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			eastl::vector< eastl::fixed_string<EATEXT_CHAR_TYPE, 32> > playerNameList =
			{	EATEXT_CHAR("Kranium4000"), EATEXT_CHAR("q6a83d86ec1 - SEen"), EATEXT_CHAR("ekblom"),EATEXT_CHAR("Bailando"),
				EATEXT_CHAR("DICE_dhansson"), EATEXT_CHAR("TobyBlack"),	EATEXT_CHAR("SirFluffalot"), EATEXT_CHAR("DevRopowell"),
				EATEXT_CHAR("Muttley_"), EATEXT_CHAR("LordSeeJay"),	EATEXT_CHAR("alexirae"), EATEXT_CHAR("royale - dice"),
				EATEXT_CHAR("oscarjohansson"), EATEXT_CHAR("Ciddhartha"), EATEXT_CHAR("Dash_Spacebar"), EATEXT_CHAR("DDixon"),
				EATEXT_CHAR("KalleWiren"), EATEXT_CHAR("B - Frew"),	EATEXT_CHAR("MiXiE"), EATEXT_CHAR("ili0s"),
				EATEXT_CHAR("CaptainCasual"), EATEXT_CHAR("frals"), EATEXT_CHAR("davidolofsson"), EATEXT_CHAR("Holgmeister"),
				EATEXT_CHAR("DICEDH"), EATEXT_CHAR("maakuss"), EATEXT_CHAR("Destructinations"), EATEXT_CHAR("Swagner1"),
				EATEXT_CHAR("Silent - _ - Ted"), EATEXT_CHAR("JDab"), EATEXT_CHAR("SDouggs"), EATEXT_CHAR("valliDICE"),
				EATEXT_CHAR("Ludsor"), EATEXT_CHAR("kongharald"), EATEXT_CHAR("LiquidVision"), EATEXT_CHAR("vimalm"),
				EATEXT_CHAR("call_me_mr_BIG"), EATEXT_CHAR("MJ_AdVentura"),	EATEXT_CHAR("mman"), EATEXT_CHAR("ponyrille"),
				EATEXT_CHAR("Kattungen"), EATEXT_CHAR("davidolofsson"),	EATEXT_CHAR("Fisk"), EATEXT_CHAR("Trix") 
			};
			
			Bezier2DIterator bezierIt;
			bezierIt.MakeSemicircle(Point(0, 0), 77, 3.141592f, false);

			for (auto& playerName : playerNameList)
			{
				nErrorCount += FollowBaseLineTest(pFont, playerName.c_str(), playerName.length(), bezierIt, textStyle);
			}

			// Release font from GetFont() call
			pFont->Release();
		}

		fontServer.Shutdown();
		glyphCache.Shutdown();
	}
	return nErrorCount;
}

#ifdef EATEXT_MIN_DEFINED
	#undef EATEXT_TEST_MIN_DEFINED
	#undef MIN
#endif




