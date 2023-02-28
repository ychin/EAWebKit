/////////////////////////////////////////////////////////////////////////////
// TestPolygonFont.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextPolygonFont.h>
#include <EAText/EATextFontServer.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileUtil.h>
#include <EATest/EATest.h>


using namespace EA::Text;


// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const unsigned char pTestPolygonFont[] = { 0x23, 0xa2, ... }; 
// #include "data/Test.polygonFont.inl"


int TestPolygonFont()
{
	int nErrorCount = 0;

	{ // FontServer
		FontServer fontServer;

		// Temporary code which needs to be replaced with generic code:
		const FilePathChar* const pFontPath = EATEXT_FILE_PATH_CHAR("F:\\Projects\\Helios\\CTG_EARS\\DL\\UTFTools\\PolygonFontGenerator\\project\\verdana.polygonFont");

		if(EA::IO::File::Exists(pFontPath))
		{
			uint32_t nFaceCount = fontServer.AddFace(pFontPath);
			EATEST_VERIFY(nFaceCount == 1);

			TextStyle ssCSS;
			EA::StdC::Strcpy(ssCSS.mFamilyNameArray[0], EATEXT_CHAR("Verdana 1"));
			ssCSS.mfSize = 1;

			Font* const pFont = fontServer.GetFont(&ssCSS);
			EATEST_VERIFY(pFont != NULL);

			if(pFont)
			{
				Kerning      kerning;
				GlyphMetrics glyphMetrics;
				Char         charArray[2] = { 'A', 'V' };
				GlyphId      glyphArray[2] = { 0, 0 };
				bool         bResult;
				uint32_t     nGlyphCount;

				nGlyphCount = pFont->GetGlyphIds(charArray, 2, glyphArray, false);
				EATEST_VERIFY(nGlyphCount == 2);

				pFont->GetKerning(glyphArray[0], glyphArray[1], kerning, 0, false);
				// There is nothing in particular to test about the kerning value at this time.

				bResult = pFont->GetGlyphMetrics(glyphArray[0], glyphMetrics);
				EATEST_VERIFY(bResult);

				pFont->Release();
			}
		}
	}

	return nErrorCount;
}



















