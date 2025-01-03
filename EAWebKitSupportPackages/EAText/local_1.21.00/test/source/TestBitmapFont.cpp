/////////////////////////////////////////////////////////////////////////////
// TestBitmapFont.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include "Bitmap.h"
#include <EAText/EATextBmpFont.h>
#include <EAText/EATextTypesetter.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <MemoryMan/MemoryMan.h>
#include <EATest/EATest.h>
#include EA_ASSERT_HEADER



/////////////////////////////////////////////////////////////////////////////
// TestBmpFont file
//
// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const uint8_t pTestBmpFontData[] = { 0x23, 0xa2, ... }; 
// The font is "TestBmpFont 16" (family = TestBmpFont, size = 16, style = normal).
//
#include "data/Test.bmpFont.inl"



// This is a RAM image of the texture we want to use. It is static and 
// so we can only ever have one of these at a time in our tests.
EA::Font::Bitmap gTestBmpFontBitmap;


/////////////////////////////////////////////////////////////////////////////
// TestBmpFont
//
// We create a subclass just for the purpose of this test.
// The sole purpose of this class is to expose the font's GlyphMetricsMap
// so that we can read it and create fake texture data from it. 
//
class TestBmpFont : public EA::Text::BmpFont
{
public:
	static const uint32_t kBmpTextureFakeFormat = 0x12345678;

	typedef EA::Text::BmpFont::BmpGlyphMetricsMap MetricsMap;

	MetricsMap& GetMetricsMap()
		{ return mBmpGlyphMetricsMap; }

	void CreateBogusBitmap();
};




void TestBmpFont::CreateBogusBitmap()
{
	// What we do here is create an ARGB texture that has a bunch of opaque rectangles on it. 
	// We go through the GlyphMetricsMap and for each glyph we draw an opaque rectangle where 
	// it is supposed to be. This is simpler than attempting to load a platform-specific 
	// texture format that BitmapFontEditor created.

	if(!mTextureInfoArray.empty())
	{
		if(gTestBmpFontBitmap.Init((int)mTextureInfoArray[0]->mnSize, (int)mTextureInfoArray[0]->mnSize))
		{
			for(BmpGlyphMetricsMap::iterator it = mBmpGlyphMetricsMap.begin(); it != mBmpGlyphMetricsMap.end(); ++it)
			{
				BmpGlyphMetricsMap::value_type value = *it;

			  //EA::Text::GlyphId          glyphId = value.first;
				EA::Text::BmpGlyphMetrics& metrics = value.second;

				if(metrics.mnTextureIndex == 0) // Actually for our test there should only be one texture and this should always be true.
				{
					gTestBmpFontBitmap.FillRect(0xffffffff, metrics.mnPositionX, metrics.mnPositionY, (int)metrics.mfSizeX, (int)metrics.mfSizeY);
				}
			}
		}
	}
}



static int TestBitmapFontTypesetting(EA::Text::GlyphCache* pGlyphCache, TestBmpFont* pFont)
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{
		TestString  s16(EATEXT_CHAR("Hello World"));
		Typesetter  ts;
		LineLayout  lineLayout;
		bool        bResult;

		EATextTest_CheckMemory();

		index_t count = LayoutSimple(s16.c_str(), s16.length(), 20, 20, pFont, lineLayout);
		EATEST_VERIFY(count == s16.length());

		LineLayout::GlyphArray::iterator itG = lineLayout.mGlyphArray.begin();

		for(LineLayout::GlyphLayoutInfoArray::iterator it = lineLayout.mGlyphLayoutInfoArray.begin(); 
			it != lineLayout.mGlyphLayoutInfoArray.end(); ++it, ++itG)
		{
			GlyphLayoutInfo& gli = *it;
			EATEST_VERIFY(gli.mpFont == pFont);

			GlyphId g = *itG;
			EATEST_VERIFY(g != kGlyphIdInvalid);

			GlyphTextureInfo gti;
			bResult = pGlyphCache->GetGlyphTextureInfo(gli.mpFont, g, gti);
			EATEST_VERIFY(bResult);

			EATEST_VERIFY(gti.mpTextureInfo != NULL);
			EATEST_VERIFY(gti.mpTextureInfo->mTexture == (uintptr_t)&gTestBmpFontBitmap);
			EATEST_VERIFY(gti.mpTextureInfo->mFormat  == TestBmpFont::kBmpTextureFakeFormat);
		}

		EATextTest_CheckMemory();
	}

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestBitmapFont
//
int TestBitmapFont()
{
	using namespace EA::Text;
	//using namespace EA::StdC;

	int nErrorCount = 0;

	EA::IO::MemoryStream* const pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pTestBmpFontData, sizeof(pTestBmpFontData), true, false);
	pMemoryStream->AddRef();

	{
		// We we do here is add the BmpFont to the FontServer via a memory image of its file.
		// The FontServer doesn't try to interpret the texture (bitmap) image data of the 
		// font, so we need to add it to the BmpFont manually. We can do this addition 
		// at any time before the font is used. It may even be useful for an application
		// to delay texture loading until the font is needed.
		FontServer        fontServer;
		GlyphCache_Memory glyphCache;

		glyphCache.Init(1, 1);
		fontServer.Init();

		// You need to set a GlyphCache with the FontServer or else you need to manually 
		// set it with all BmpFonts.
		fontServer.SetDefaultGlyphCache(&glyphCache);

		uint32_t count = fontServer.AddFace(pMemoryStream, kFontTypeBitmap);
		EATEST_VERIFY(count == 1);

		// The AddFace call above doesn't tell us what Font* was created. This is so because
		// multiple Fonts (e.g. different sizes and styles) can be created from a Face, 
		// though with bitmapped fonts it happens to be that usually you get only one Font for the Face.
		// So we get the associated font from the server ourselves.
		TextStyle textStyle;
		EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("TestBmpFont")); // We can dispense with the size because we have registered only one size with FontServer, so that one will always be returned.
		textStyle.mfSize = 16;

		Font* pFont = fontServer.GetFont(&textStyle);
		EATEST_VERIFY(pFont != NULL);

		if(pFont)
		{
			// This cast is something of a C++ hack that assumes BmpFont can be treated as TestBmpFont.
			// In theory this can't be relied upon, but in practice it can.
			TestBmpFont* const pTestBmpFont = static_cast<TestBmpFont*>(pFont);

			FontDescription fd;
			bool bResult = pTestBmpFont->GetFontDescription(fd);
			EATEST_VERIFY(bResult);

			// Make sure the family matches the expected value.
			EATEST_VERIFY(EA::StdC::Strcmp(fd.mFamily, EATEXT_CHAR("TestBmpFont")) == 0);
			EATEST_VERIFY(fd.mfSize == 16);

			// Make sure the GlyphCache was passed on from the FontServer to the Font when the FontServer created the font.
			GlyphCache* const pGlyphCache = pTestBmpFont->GetGlyphCache();
			EATEST_VERIFY(pGlyphCache == &glyphCache);

			// What we do here is manually create our own custom texture format that nobody 
			// knows about (nor needs to know about) except us.
			pTestBmpFont->CreateBogusBitmap();
			pTestBmpFont->SetBmpTexture(0, (uintptr_t)&gTestBmpFontBitmap, TestBmpFont::kBmpTextureFakeFormat);
 
			// Call a test function which will verify that things are working as expected with this custom texture.
			nErrorCount += TestBitmapFontTypesetting(&glyphCache, pTestBmpFont);

			pFont->Release();
		}

		fontServer.Shutdown();
		glyphCache.Shutdown();
	}

	pMemoryStream->Release();

	return nErrorCount;
}



















