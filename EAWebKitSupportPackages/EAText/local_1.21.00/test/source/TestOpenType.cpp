/////////////////////////////////////////////////////////////////////////////
// TestOpenType.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/internal/EATextOpenType.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextFontServer.h>
#include <EAIO/EAStream.h>
#include <EAStdC/EAString.h>
#include <EATest/EATest.h>


using namespace EA::Text;


///////////////////////////////////////////////////////////////////////////////
// TestOpenTypeTableTrace
//
int TestOpenTypeTableTrace(Font* pFont)
{
	int nErrorCount = 0;

	EA::IO::IStream* pStream = pFont->GetStream();
	EATEST_VERIFY(pStream != NULL);

	OTF otf;
	bool result = otf.Load(pStream);
	EATEST_VERIFY(result);

	// To do.

	result = otf.Unload();
	EATEST_VERIFY(result);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestOpenType
//
int TestOpenType()
{
	int nErrorCount = 0;

	// We read Microsoft OpenType fonts for this test and so until we can set up
	// some OpenType fonts on other platforms, we just test under Windows.
	#ifdef EA_PLATFORM_WINDOWS
		FontServer* pFontServer = GetFontServer();
		EATEST_VERIFY(pFontServer != NULL);

		if(pFontServer)
		{
			FilePathChar fontPath[256];

			EA::Text::GetSystemFontDirectory(fontPath, 256);
		  //EA::StdC::Strcat(fontPath, EATEXT_FILE_PATH_CHAR("micross.ttf"));   // This font supports Arabic.
			EA::StdC::Strcat(fontPath, EATEXT_FILE_PATH_CHAR("mangal.ttf"));    // This font supports Devanagari (Hindi).
		  //EA::StdC::Strcat(fontPath, EATEXT_FILE_PATH_CHAR("arialuni.ttf"));  // This font supports all scripts and languages of significance.

			pFontServer->AddFace(fontPath, kFontTypeOutline);

			TextStyle textStyle;
		  //EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Microsoft Sans Serif"));
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Mangal"));
		  //EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial Unicode MS"));
			textStyle.mfSize = 16;

			Font* const pFont = pFontServer->GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			if(pFont)
			{
				nErrorCount += TestOpenTypeTableTrace(pFont);

				pFont->Release();
			}
		}
	#endif

	return nErrorCount;
}
















