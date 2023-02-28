/////////////////////////////////////////////////////////////////////////////
// TestFont.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextFont.h>
#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextBmpFont.h>
#include <EAText/EATextDebugFont.h>
#include <EAText/EATextCache.h>
#include <EAText/EATextFontServer.h>
#include <EAText/EATextTypesetter.h>
#include <eathread/eathread_thread.h>
#include <eathread/eathread_sync.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAStreamMemory.h>
#include <EAStdC/EAStopwatch.h>
#include <MemoryMan/MemoryMan.h>
#include <EATest/EATest.h>



// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const uint8_t pPalormSmallCapsTTF[] = { 0x23, 0xa2, ... }; 
#include "data/ParolmSmallCaps.ttf.inl"

// Here we #include an embedded TrueType font which is located inside 
// a .inl file and which is declared as a C char array like this:
//   const uint8_t pHelvRoundedObliqueTTF[] = { 0x23, 0xa2, ... }; 
// This data comes from the Adobe collection HelveticaRoundedLTStd-Bd.ttf file.
#include "data/HelvRoundedOblique.ttf.inl"


#define NUM_TESTFONT_ITERATIONS 16


/////////////////////////////////////////////////////////////////////////////
// TestFontBasic
//
static int TestFontBasic()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{ // Misc
		FilePathChar pFontDirectory[256];
		uint32_t     nn, n0, n1, n256;

		nn   = GetSystemFontDirectory(NULL, 0);
		n0   = GetSystemFontDirectory(pFontDirectory, 0);
		n1   = GetSystemFontDirectory(pFontDirectory, 1);
		n256 = GetSystemFontDirectory(pFontDirectory, 256);
		
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			EATEST_VERIFY(nn   > 0);
			EATEST_VERIFY(n0   > 0);
			EATEST_VERIFY(n1   > 0);
			EATEST_VERIFY(n256 > 0);
		#else
			// Test other platforms if this becomes possible.
			EATEST_VERIFY(nn   < 10000);
			EATEST_VERIFY(n0   < 10000);
			EATEST_VERIFY(n1   < 10000);
			EATEST_VERIFY(n256 < 10000);
		#endif
	}


	{ // Microsoft msgothic.ttc test. This is a regression of a Font Fusion-related bug.
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			FontServer            fontServer;
			uint32_t              nFaceCount;
			EA::IO::MemoryStream* pMemoryStream;
			Font*                 pFont;
			bool                  fontExists;

			// Add "Cambria", for the purpose of making sure that we can load MS Gothic while it and some other fonts are present.
			FilePathChar pFontPath[EA::IO::kMaxPathLength];
			GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
			EA::StdC::Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("cambria.ttc"));
			fontExists = EA::IO::File::Exists(pFontPath);
			if(fontExists)
			{
				nFaceCount = fontServer.AddFace(pFontPath);
				EATEST_VERIFY(nFaceCount > 0);
			}
			else
				EA::UnitTest::ReportVerbosity(1, "Font not found: \nThis unit test is designed to use this" EATEXT_FILE_PATH_CHAR_FORMAT, pFontPath);

			// Add "Parolm" (small caps)
			pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
			pMemoryStream->AddRef();
			nFaceCount = fontServer.AddFace(pMemoryStream, kFontTypeOutline);
			pMemoryStream->Release();
			EATEST_VERIFY(nFaceCount > 0);

			// Add "HelveticaRounded LT Std Bd" (oblique; has italics flag set)
			pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pHelvRoundedObliqueTTF, sizeof(pHelvRoundedObliqueTTF), true, false, GetAllocator());
			pMemoryStream->AddRef();
			nFaceCount = fontServer.AddFace(pMemoryStream, kFontTypeOutline);
			pMemoryStream->Release();
			EATEST_VERIFY(nFaceCount > 0);


			// Get "MS Gothic"
			TextStyle textStyle;
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("MS Gothic"));
			textStyle.mfSize = 12;
			pFont = fontServer.GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			if(pFont)
			{
				// Verify that many fonts can be created of different sizes from the same face.
				EA::Text::LineLayout    lineLayout;
				Font*                   pFontArray[NUM_TESTFONT_ITERATIONS];
				TestString              s16(EATEXT_CHAR("Hello World"));

				memset(pFontArray, 0, sizeof(pFontArray));

				for(int i = 0; i < NUM_TESTFONT_ITERATIONS; i++)
				{
					textStyle.mfSize = (float)i + 6;
					pFontArray[i] = fontServer.GetFont(&textStyle);
					EATEST_VERIFY(pFontArray[i] != NULL);

					if(pFontArray[i])
					{
						const index_t count = EA::Text::LayoutSimple(s16.c_str(), s16.length(), 20, 20, pFontArray[i], lineLayout);
						EATEST_VERIFY(count == s16.length());
					}
				}

				for(int j = 0; j < NUM_TESTFONT_ITERATIONS; j++)
				{
					if(pFontArray[j])
						pFontArray[j]->Release();
				}

				pFont->Release();
			}

			// Get "HelveticaRounded LT Std Bd"
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("HelveticaRounded LT Std Bd"));
			textStyle.mfSize   = 12;
			textStyle.mStyle   = kStyleItalic;
			textStyle.mfWeight = kWeightBold;
			pFont = fontServer.GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			if(pFont)
			{
				EA::Text::FontDescription fd;
				pFont->GetFontDescription(fd);

				// There was a bug whereby the font style for this font wasn't 
				// being correctly read. This verifies that it is being read correctly.
				EATEST_VERIFY(fd.mStyle == EA::Text::kStyleItalic);

				pFont->Release();
			}

		#endif
	}


	return nErrorCount;
}



/////////////////////////////////////////////////////////////////////////////
// TestFontServer
//
static int TestFontServer1()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{ // FontServer
		GlyphCache_Memory glyphCache;
		FontServer        fontServer;

		glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
		fontServer.Init();

		#if defined(EA_PLATFORM_WINDOWS)
			FilePathChar        pFontPath[EA::IO::kMaxPathLength];
			const FilePathChar* fontNameArray[] = { EATEXT_FILE_PATH_CHAR("arial.ttf"), EATEXT_FILE_PATH_CHAR("ariali.ttf") /* Arial Italic*/ };

			for(int i = 0; i < EAArrayCount(fontNameArray); ++i)
			{
				GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
				EA::StdC::Strcat(pFontPath, fontNameArray[i]);
				uint32_t nFaceCount = fontServer.AddFace(pFontPath);

				if(nFaceCount < 1) // If failed... // Fonts may in theory have more than one face in a file.
				{
					if(EA::IO::File::Exists(pFontPath))
					{
						EATEST_VERIFY_F(nFaceCount < 1, "FontServer::AddFace failed with " EATEXT_FILE_PATH_CHAR_FORMAT ".", pFontPath); 
					}
					else
					{
						EA::UnitTest::Report("Font not found at path: " EATEXT_FILE_PATH_CHAR_FORMAT "\n", pFontPath);
					}
				}
			}
		#endif


		// For console or embedded platforms, we can't rely on specific 
		// fonts being in a system font directory. For those we'll have
		// to do our own embedded font for this test.

		EA::IO::MemoryStream* pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
		fontServer.AddFace(pMemoryStream, kFontTypeOutline);
		TextStyle textStyle;

		{
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Parolm"));
			Font* const pFont = fontServer.GetFont(&textStyle);
			EATEST_VERIFY(pFont != NULL);

			if (pFont)
			{
				Kerning      kerning;
				GlyphMetrics glyphMetrics;
				Char         charArray[2] = { 'A', 'V' };
				GlyphId      glyphArray[2] = { 0, 0 };
				bool         bResult;
				uint32_t     nGlyphCount;

				nGlyphCount = pFont->GetGlyphIds(charArray, 2, glyphArray, false);
				EATEST_VERIFY(nGlyphCount == 2);

				bResult = pFont->GetKerning(glyphArray[0], glyphArray[1], kerning, 0, false);
				EATEST_VERIFY(bResult);

				bResult = pFont->GetGlyphMetrics(glyphArray[0], glyphMetrics);
				EATEST_VERIFY(bResult);

				pFont->Release();
			}
		}

		// EnumerateFonts(FontDescription* pFontDescriptionArray, uint32_t nCount)
		{
			FontDescription fontDescriptionArray[NUM_TESTFONT_ITERATIONS];
			uint32_t count;

			memset(fontDescriptionArray, 0, sizeof(fontDescriptionArray));
			count = fontServer.EnumerateFonts(fontDescriptionArray, 0);
			EATEST_VERIFY(fontDescriptionArray[0].mFamily[0] == 0);
			EATEST_VERIFY(count > 0);

			memset(fontDescriptionArray, 0, sizeof(fontDescriptionArray));
			count = fontServer.EnumerateFonts(fontDescriptionArray, 1);
			EATEST_VERIFY(fontDescriptionArray[0].mFamily[0] != 0);
			EATEST_VERIFY(fontDescriptionArray[1].mFamily[0] == 0);
			EATEST_VERIFY(count > 0);
		}

		{
			// Verify that many fonts can be created of different sizes from the same face.
			EA::Text::LineLayout    lineLayout;
			Font*                   pFontArray[NUM_TESTFONT_ITERATIONS];
			TestString              s16(EATEXT_CHAR("Hello World"));

			memset(pFontArray, 0, sizeof(pFontArray));

			for(int i = 0; i < NUM_TESTFONT_ITERATIONS; i++)
			{
				textStyle.mfSize = (float)i + 6;
				pFontArray[i] = fontServer.GetFont(&textStyle);
				EATEST_VERIFY(pFontArray[i] != NULL);

				if(pFontArray[i])
				{
					const index_t count = EA::Text::LayoutSimple(s16.c_str(), s16.length(), 20, 20, pFontArray[i], lineLayout);
					EATEST_VERIFY(count == s16.length());
				}
			}

			for(int j = 0; j < NUM_TESTFONT_ITERATIONS; j++)
			{
				if(pFontArray[j])
					pFontArray[j]->Release();
			}
		}

		// We can only be sure the Arial Italic font will be available on Windows Machines as we request to load it explicitly above
		#if defined(EA_PLATFORM_WINDOWS)
			{
				// Verify that our font selection algorithm is correct - SimCity Regression Check
				// Provide two valid font family names, however ones with different inherent stylings. If our primary family font doesn't
				// match our requested style we must choose a font which does match the styling (in this case the second font must be chosen)
				FontDescription fontDescriptionArray[NUM_TESTFONT_ITERATIONS];
				uint32_t count;

				memset(fontDescriptionArray, 0, sizeof(fontDescriptionArray));
				count = fontServer.EnumerateFonts(fontDescriptionArray, 3);
				EATEST_VERIFY(count > 1);

				// The primary font is unstyled whereas Arial Italic is inherently italic and should be preferred, regardless of weighting
				TextStyle ts = TextStyle(EATEXT_CHAR("Parolm;Arial;"), 0.0f, kStyleItalic, float(kWeightBold));

				FontSelection fontSelection;
				fontServer.GetFont(&ts, fontSelection);

				Font* f = *(fontSelection.begin());
				FontDescription desc;
				f->GetFontDescription(desc);
				EATEST_VERIFY(EA::StdC::Strncmp(EATEXT_CHAR("Arial"), desc.mFamily, EA::StdC::Strlen(EATEXT_CHAR("Arial"))) == 0);
			}
		#endif

		{ // uint32_t FreeUnusedFonts();
			uint32_t count = fontServer.FreeUnusedFonts();
			EATEST_VERIFY(count > 0);
		}

		{
			fontServer.AddFace(pMemoryStream, kFontTypeOutline);
			// Verify that RemoveFace works.
			fontServer.SetDefaultGlyphCache(&glyphCache); // Normally you'd call this once on app startup.

			TextStyle ts;
			EA::StdC::Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Parolm"));
			ts.mfSize = 1.5;    // Some new value we haven't used yet.

			Font* pFont = fontServer.GetFont(&ts, NULL, 0, EA::Text::kCharInvalid, EA::Text::kScriptUnknown, true);
			EATEST_VERIFY(pFont != NULL);

			if(pFont)
			{
				uint32_t         fakeBitmap;
				GlyphTextureInfo gti;

				bool result = glyphCache.AddGlyphTexture(pFont, 17, &fakeBitmap, 1, 1, 1, kTextureFormatARGB, gti);
				EATEST_VERIFY(result);
				result = glyphCache.AddGlyphTexture(pFont, 18, &fakeBitmap, 1, 1, 1, kTextureFormatARGB, gti);
				EATEST_VERIFY(result);

				fontServer.RemoveFace(EATEXT_CHAR("Parolm"));

				result = glyphCache.GetGlyphTextureInfo(pFont, 17, gti);
				EATEST_VERIFY(!result); // It should be absent.
				result = glyphCache.GetGlyphTextureInfo(pFont, 18, gti);
				EATEST_VERIFY(!result); // It should be absent.

				pFont->Release();

				// We re-add the face so other tests may use it since it was removed above
				pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
				fontServer.AddFace(pMemoryStream, kFontTypeOutline);
			}
		}


		{
			// Verify that kOptionRequireFontFamilyMatch works.
			fontServer.AddFace(pMemoryStream, kFontTypeOutline);
			int32_t optionSaved = fontServer.GetOption(FontServer::kOptionRequireFontFamilyMatch);
			fontServer.SetOption(FontServer::kOptionRequireFontFamilyMatch, 2); // With this setting FontServer should fail if the font doesn't match.

			TextStyle ts;
			EA::StdC::Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Nonexistant"));
			Font* const pFont = fontServer.GetFont(&ts);
			EATEST_VERIFY(pFont == NULL);

			fontServer.SetOption(FontServer::kOptionRequireFontFamilyMatch, optionSaved);
		}

		{
			fontServer.FreeUnusedFonts();
			EA::IO::MemoryStream* pNewMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
			fontServer.AddFace(pNewMemoryStream, kFontTypeOutline);
			TextStyle ts;
			EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Parolm"));

			ts.mfWeight = 400.0f; // normal weighting
			Font* pFontNormalWeight = fontServer.GetFont(&ts);
			EATEST_VERIFY(pFontNormalWeight != NULL);

			// Now fetch a text style for a font we don't have but is in the same family
			ts.mfWeight = 700.0f; // bold weighting
			Font* pFontBoldWeight1 = fontServer.GetFont(&ts);
			EATEST_VERIFY(pFontBoldWeight1 != NULL);
			EATEST_VERIFY(pFontNormalWeight != pFontBoldWeight1);

			// Fetch the same missing font again and double check we didn't create a new approximate font for the user
			Font* pFontBoldWeight2 = fontServer.GetFont(&ts);
			EATEST_VERIFY(pFontBoldWeight2 != NULL);
			EATEST_VERIFY(pFontBoldWeight1 == pFontBoldWeight2);

			pFontBoldWeight1->Release();	// For GetFont() call
			pFontBoldWeight2->Release();	// For GetFont() call
			fontServer.FreeUnusedFonts();

			Font* pFontBoldWeight3 = fontServer.GetFont(&ts);
			EATEST_VERIFY(pFontBoldWeight3 != NULL);
			EATEST_VERIFY(pFontNormalWeight != pFontBoldWeight3);

			pFontBoldWeight3->Release();	// For GetFont() call
			pFontNormalWeight->Release();	// For GetFont() call
		}

		fontServer.Shutdown();
		glyphCache.Shutdown();
	}

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestDebugFont
//
static int TestDebugFont()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{   // layout performance.

		eastl::vector<eastl::string8> stringArray;

		// Setup a random set of debug strings.
		for(eastl_size_t i = 0; i < NUM_TESTFONT_ITERATIONS; i++)
		{
			stringArray.push_back();

			for(int j = 0; j < 40; j++)
			{
				const char8_t c = (char8_t)gRandom.RandRange(DebugFont::kCharMin, DebugFont::kCharMax);

				stringArray[i].push_back(c);
			}
		}

		EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCycles);
		EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCycles);

		EA::IO::MemoryStream* const pMemoryStream = new EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());

		DebugFont*   const pDebugFont   = new DebugFont;
		OutlineFont* const pOutlineFont = new OutlineFont;

		if(pDebugFont->Open(pMemoryStream) && pOutlineFont->Open(pMemoryStream))
		{
			LineLayout lineLayout;

			pDebugFont->SetTransform(12);
			pOutlineFont->SetTransform(12);

			int pass;

			for(pass = 0; pass < 2; ++pass)
			{
				stopwatch1.Restart();

				for(int n = 0; n < 400; n++)
				{
					for(eastl_size_t i = 0, iEnd = stringArray.size(); i < iEnd; i++)
					{
						const eastl::string8& s8 = stringArray[i];
						EA::Text::LayoutSimple(s8.c_str(), s8.size(), 0, 0, pDebugFont, lineLayout);
					}                
				}

				stopwatch1.Stop();
			}

			for(pass = 0; pass < 2; ++pass)
			{
				stopwatch2.Restart();

				for(int n = 0; n < 400; n++)
				{
					for(eastl_size_t i = 0, iEnd = stringArray.size(); i < iEnd; i++)
					{
						const eastl::string8& s8 = stringArray[i];
						EA::Text::LayoutSimple(s8.c_str(), s8.size(), 0, 0, pOutlineFont, lineLayout);
					}                
				}

				stopwatch2.Stop();
			}

			EA::UnitTest::ReportVerbosity(1, "TestDebugFont: time 1: %I64u time 2: %I64u\n", (uint64_t)stopwatch1.GetElapsedTime(), (uint64_t)stopwatch2.GetElapsedTime());

			pDebugFont->Close();
			pOutlineFont->Close();
		}

		delete pDebugFont;
		delete pOutlineFont;
	}

	return nErrorCount;
}



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
		#if EATHREAD_VERSION_N >= 11702
			EA::UnitTest::ReportVerbosity(2, "Font test function created: %s\n", EAThreadThreadIdToString(threadId));
		#else
			EA::UnitTest::ReportVerbosity(2, "Font test function created: 0x%08x\n", (unsigned)(uintptr_t)threadId);
		#endif

		EA::UnitTest::ReportVerbosity(2, "Font test function created: %s\n", EAThreadThreadIdToString(threadId));

		while(!pFontThreadData->mbShouldQuit)
		{
			const int                kCharCount = NUM_TESTFONT_ITERATIONS;
			TextStyle                ts;
			GlyphId                  glyphs[kCharCount];
			Char                     ch[kCharCount];
			GlyphMetrics             gm;
			Font*                    pFont;
			const Font::GlyphBitmap* bm;
			int                      i;

			EA::StdC::Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
			ts.mfSize = 10.0f + (gRandom.RandLimit(250) / 10.0f);

			pFont = pFontThreadData->mpFontServer->GetFont(&ts, NULL, 0, EA::Text::kCharInvalid, EA::Text::kScriptUnknown, false);

			if(pFont)
			{
				// Make a random string
				for(i = 0; i < kCharCount; ++i)
					ch[i] = (Char)(size_t)gRandom.RandRange(0x21, 0xff);

				pFont->GetGlyphIds(ch, kCharCount, glyphs);

				for(i = 0; i < kCharCount; ++i)
				{
					pFont->GetGlyphMetrics(glyphs[i], gm); 

					if(pFont->RenderGlyphBitmap(&bm, glyphs[i]))
						pFont->DoneGlyphBitmap(bm);
				}

				pFont->Release();
			}

			EAReadWriteBarrier();
		}

		return 0;
	}
#endif


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



/////////////////////////////////////////////////////////////////////////////
// TestFontMultithreaded
//
// We test the following from multiple threads:
//    X font faces.
//    Y font sizes per face.
//    Z characters per font.
//
static int TestFontMultithreaded()
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

		const FilePathChar* kFontFileNames[3] = { EATEXT_FILE_PATH_CHAR("arial.ttf"), EATEXT_FILE_PATH_CHAR("cour.ttf"), EATEXT_FILE_PATH_CHAR("verdana.ttf") };

		for(size_t i = 0; i < EAArrayCount(kFontFileNames); i++)
		{
			FilePathChar pFontPath[EA::IO::kMaxPathLength];
			GetTestDataDirectory(pFontPath, EA::IO::kMaxPathLength);
			EA::StdC::Strcat(pFontPath, kFontFileNames[i]);
			uint32_t nFaceCount = fontServer.AddFace(pFontPath);

			if(nFaceCount < 1) // If failed... // Fonts may in theory have more than one face in a file.
			{
				if(EA::IO::File::Exists(pFontPath))
					EATEST_VERIFY_F(nFaceCount < 1, "FontServer::AddFace failed with " EATEXT_FILE_PATH_CHAR_FORMAT ".", pFontPath); 
				else
					EA::UnitTest::Report("Font not found at path: " EATEXT_FILE_PATH_CHAR_FORMAT "\n", pFontPath);
			}
		}

		// Setup FontThreadData
		FontThreadData fontThreadData;

		fontThreadData.mbShouldQuit = false;
		fontThreadData.mpFontServer = &fontServer;
		fontThreadData.mpGlyphCache = &glyphCache;

		fontThreadData.mFaceNameArray.push_back(eastl::string("Arial"));
		fontThreadData.mFaceNameArray.push_back(eastl::string("Courier New"));
		fontThreadData.mFaceNameArray.push_back(eastl::string("Verdana"));

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

		EA::UnitTest::ThreadSleepRandom(2000, 2000);

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
// TestFontAttributes
//
// We test the ability of the Font class to recognize TrueType font attributes.
// It turns out that Font Fusion and FreeType have different means of parsing
// font information and present it in ways that are different from each other.
// This caused problems for us when we switched from Font Fusion to FreeType.
//
static int TestFontAttributes()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{
		// Look for -TestFontAttributesDir:<path>
		const char* pResult;
		int         result = GetAppCommandLine().FindSwitch("TestFontAttributesDir", false, &pResult);

		if((result >= 0) && *pResult && EA::IO::Directory::Exists(pResult))
		{
			// The directory is typically a collection of related fonts which we want to make sure are all 
			// seen as unique by the Font and FontServer. So:
			// We collect a list of all the .ttf fonts in the directory or all the .otf fonts in the directory.
			// We assume these fonts are each of a unique style. We then load each of them and they should
			// all load and none should be considered duplicates of another.
			wchar_t dirW[EA::IO::kMaxDirectoryLength]; 
			EA::StdC::Strlcpy(dirW, pResult, EAArrayCount(dirW));
			EA::IO::Path::EnsureTrailingSeparator(dirW, EAArrayCount(dirW));

			EA::IO::DirectoryIterator di;
			EA::IO::DirectoryIterator::EntryList entryList;

			size_t readResult = di.Read(dirW, entryList, L"*.ttf", EA::IO::kDirectoryEntryFile, 512, false);

			if(readResult == 0)
				readResult = di.Read(dirW, entryList, L"*.otf", EA::IO::kDirectoryEntryFile, 512, false);

			if(readResult > 0)
			{
				FontServer fontServer;

				fontServer.Init();

				for(EA::IO::DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
				{
					EA::IO::DirectoryIterator::Entry& entry = *it;

					entry.msName.insert(0, dirW);
					eastl::basic_string<FilePathChar> facePath;
					EA::StdC::Strlcpy(facePath, entry.msName.c_str());
					uint32_t nFaceCount = fontServer.AddFace(facePath.c_str());

					if(nFaceCount < 1) // If failed...
					{
						EA::UnitTest::Report("TestFontAttributes: FontServer::AddFace failed for %ls\n", entry.msName.c_str());

						EA::IO::FileStream fileStream(entry.msName.c_str());

						if(fileStream.Open())
						{
							EA::Text::OutlineFont outlineFont;

							if(outlineFont.Open(&fileStream))
							{
								EA::UnitTest::Report("    because the font attributes collided with another font.\n");
								// To consider: print attributes here.
							}
							else
								EA::UnitTest::Report("    because the font contents weren't recognized as valid.\n");
						}
						else
							EA::UnitTest::Report("    because the font file couldn't be opened.\n");
					}
				}

				fontServer.Shutdown();
			}
		}
	}

	return nErrorCount;
}

static int TestFontServer2()
{
	using namespace EA::Text;

	int nErrorCount = 0;

    // Code to test and debug problems with Server::GetFont. 
	// This was prompted by a user reporting that a font Server with Helvetica Lt Std fonts registered wasn't able
	// to pick the right font that the app requested.
	{
		// Look for -TestFontSelectionDir:"<path>" -TestFontSelectionName:"<family name>"
		// We register all fonts present in the directory <path> and issue a GetFont with the given family name.
		// The font retured should have a matching family name. 

		eastl::basic_string<Char> sFontSelectionDir;
		eastl::basic_string<Char> sFontFamilyName;
		const char*               pResult;

		int result = GetAppCommandLine().FindSwitch("TestFontSelectionDir", false, &pResult);
		if(result >= 0)
			sFontSelectionDir.assign_convert(pResult);

		result = GetAppCommandLine().FindSwitch("TestFontSelectionName", false, &pResult);
		if(result >= 0)
			sFontFamilyName.assign_convert(pResult);

		if(EA::IO::Directory::Exists(sFontSelectionDir.c_str()))
		{
			// The directory is typically a collection of related fonts which we want to make sure are all 
			// seen as unique by the Font and FontServer. So:
			// We collect a list of all the .ttf fonts in the directory or all the .otf fonts in the directory.
			// We assume these fonts are each of a unique style. We then load each of them and they should
			// all load and none should be considered duplicates of another.
			wchar_t dirW[EA::IO::kMaxDirectoryLength]; 
			EA::StdC::Strlcpy(dirW, sFontSelectionDir.c_str(), EAArrayCount(dirW));
			EA::IO::Path::EnsureTrailingSeparator(dirW, EAArrayCount(dirW));

			EA::IO::DirectoryIterator di;
			EA::IO::DirectoryIterator::EntryList entryList;

			size_t readResult = di.Read(dirW, entryList, L"*.ttf", EA::IO::kDirectoryEntryFile, 512, false);

			if(readResult == 0)
				readResult = di.Read(dirW, entryList, L"*.otf", EA::IO::kDirectoryEntryFile, 512, false);

			if(readResult > 0)
			{
				FontServer fontServer;
				fontServer.Init();                

				// Register all the fonts in the directory. While doing so, we collect the family name of each one for later use.
				FontDescription fontDescription;
				eastl::vector< eastl::basic_string<Char> > fontFamilyNameArray;

				for(EA::IO::DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
				{
					EA::IO::DirectoryIterator::Entry& entry = *it;

					entry.msName.insert(0, dirW);
					eastl::basic_string<FilePathChar> facePath;
					EA::StdC::Strlcpy(facePath, entry.msName.c_str());
					uint32_t nFaceCount = fontServer.AddFace(facePath.c_str(), kFontTypeUnknown, &fontDescription);
					EATEST_VERIFY(nFaceCount > 0);
					if(nFaceCount > 0) // Save the family name. Well check it below.
						fontFamilyNameArray.push_back(eastl::basic_string<Char>(fontDescription.mFamily));
				}

				if(!sFontFamilyName.empty()) // If a family name to test was given on the command line...
					fontFamilyNameArray.push_back(sFontFamilyName);

				// Now verify that every family name in fontFamilyNameArray can be passed to GetFont and return a font of that family.
				for(eastl_size_t f = 0; f < fontFamilyNameArray.size(); f++)
				{
					TextStyle ts;

					size_t requiredStrlen = EA::StdC::Strlcpy(ts.mFamilyNameArray[0], fontFamilyNameArray[f].c_str(), EAArrayCount(ts.mFamilyNameArray[0]));
					EATEST_VERIFY(requiredStrlen < EAArrayCount(ts.mFamilyNameArray[0]));

					ts.mfSize = 16;
				
					Font* pFont = fontServer.GetFont(&ts, NULL, 0, EA::Text::kCharInvalid, EA::Text::kScriptUnknown, true);
					EATEST_VERIFY(pFont != NULL);

					if(pFont)
					{
						pFont->GetFontDescription(fontDescription);

						EATEST_VERIFY_F(fontFamilyNameArray[f] == fontDescription.mFamily, "TestFontServer GetFont failure for dir \"%ls\", expected family: %I16s, actual fmaily: %I16s", dirW, fontFamilyNameArray[f].c_str(), fontDescription.mFamily);

						pFont->Release();
					}
				}

				fontServer.Shutdown();
			}
		}
	}
    
	return nErrorCount;
}

/////////////////////////////////////////////////////////////////////////////
// TestFont
//
int TestFont()
{
	int nErrorCount = 0;

	nErrorCount += TestFontAttributes();
	nErrorCount += TestDebugFont();
	nErrorCount += TestFontBasic();
	nErrorCount += TestFontServer1();
    nErrorCount += TestFontServer2();
	nErrorCount += TestFontMultithreaded();
	
	return nErrorCount;
}
















