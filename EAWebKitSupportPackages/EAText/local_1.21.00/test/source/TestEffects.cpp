/////////////////////////////////////////////////////////////////////////////
// TestEffects.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextFont.h>
#include <EAText/EATextCache.h>
#include <EAText/EATextEffects.h>
#include <EAText/EATextFontServer.h>
#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextTypesetter.h>
#include <eathread/eathread_thread.h>
#include <eathread/eathread_sync.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAString.h>
#include <EATest/EATest.h>
#include <MemoryMan/MemoryMan.h>


using namespace EA::Text;




/////////////////////////////////////////////////////////////////////////////
// TestEffectsPlugin
//
class TestEffectsPlugin : public EffectsPlugin
{
public:
	void ProcessEffect(uint32_t id, EffectsProcessor* pEffectsProcessor, EffectsState* pEffectsState, void* pUserData);
};

void TestEffectsPlugin::ProcessEffect(uint32_t /*id*/, EffectsProcessor* /*pEffectsProcessor*/, EffectsState* /*pEffectsState*/, void* /*pUserData*/)
{
	// Possibly do something.
}


/////////////////////////////////////////////////////////////////////////////
// TestEffectsBasic
//
int TestEffectsBasic()
{
	using namespace EA::StdC;
	using namespace EA::Text;

	int  nErrorCount = 0;
	bool bResult;

	// Setup GlyphCache
	EA::Text::GlyphCache_Memory glyphCache;
	const int nResult = glyphCache.Init(1, 1);
	EATEST_VERIFY(nResult == 1);

	// Setup FontServer
	EA::Text::FontServer* const pFontServer = EA::Text::GetFontServer();
	EA_ASSERT(pFontServer);

	// Add some fonts.
	FilePathChar pFontPath[EA::IO::kMaxPathLength];
	const FilePathChar* fontNameArray[3] = { EATEXT_FILE_PATH_CHAR("cour.ttf"), EATEXT_FILE_PATH_CHAR("verdana.ttf"), EATEXT_FILE_PATH_CHAR("tahoma.ttf") };

	for(size_t i = 0; i < EAArrayCount(fontNameArray); i++)
	{
		GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
		Strcat(pFontPath, fontNameArray[i]);
		
		if(EA::IO::File::Exists(pFontPath))
		{
			uint32_t nFaceCount = pFontServer->AddFace(pFontPath);
			EATEST_VERIFY(nFaceCount == 1);
		}
	}

	// Create some effects
	EffectsState es;
	int          nEffectTestCount = 0;
	int32_t      nTextErrorCount = 0;

	{
		// Create a glyph shadow
		es.ClearInstructionList();
		es.ResetParameters();
		es.SetGlyphColor(0xff000000);
		es.SetGlyphSmooth(true);
		es.SetGlyphBrush(EA::Text::kBSRound, 5, 1.f, 1.f);
		es.DrawGlyphSmearOutline();
		es.ApplyBoxBlur(1, 1.0, 2);
		es.Merge();
		es.Clear();
		es.SetGlyphColor(0xfff00000);
		es.DrawGlyph();
		es.Merge();
		pFontServer->AddEffect(0x00001000 + nEffectTestCount++, es.mInstructionList, es.mInstructionListSize);

		// Create an empty instruction list. This is a no-op. It shoudl generate an invisible font.
		es.ClearInstructionList();  // You normally don't need to call ClearInstructionList unless you are re-using an EffectsState.
		es.ResetParameters();       // You normally don't need to call ResetParameters unless you are re-using an EffectsState.
		EATEST_VERIFY(es.mInstructionListSize == 0);
		pFontServer->AddEffect(0x00001000 + nEffectTestCount++, es.mInstructionList, es.mInstructionListSize);

		// Create a simple glyph
		es.ClearInstructionList();
		es.ResetParameters();
		es.SetGlyphColor(0xffffffff);
		es.DrawGlyph();
		es.Merge();
		EATEST_VERIFY(es.mInstructionListSize == 4);
		pFontServer->AddEffect(0x00001000 + nEffectTestCount++, es.mInstructionList, es.mInstructionListSize);

		// Create a glyph shadow
		es.ClearInstructionList();
		es.ResetParameters();
		es.SetGlyphColor(0xffffffff);
		es.SetGlyphOffset(2, 2);
		es.DrawGlyph();
		es.ApplyBoxBlur(2, 1.5, 1);
		es.Merge();
		EATEST_VERIFY(es.mInstructionListSize == 11);
		pFontServer->AddEffect(0x00001000 + nEffectTestCount++, es.mInstructionList, es.mInstructionListSize);

		// Create a glyph outline with solid glyph center.
		CreateInstructionList("SetGlyphColor 0xffffffff\n"
							  "SetGlyphSmooth 1\n"
							  "SetGlyphBrush 0 3 0.5 0.5\n"
							  "DrawGlyphSmearOutline\n"
							  "ApplyCurve 1 1\n"
							  "Merge\n"
							  "Clear\n"
							  "SetGlyphColor 0xffffffff\n"
							  "SetGlyphSmooth 0\n"
							  "DrawGlyph\n"
							  "Merge\n", es, &nTextErrorCount);
		EATEST_VERIFY(es.mInstructionListSize == 21);
		pFontServer->AddEffect(0x00001000 + nEffectTestCount++, es.mInstructionList, es.mInstructionListSize);
	}


	{
		// Acquire font some fonts
		TextStyle       ts;
		FontDescription fd;
		Font*           pFontArray[16];
		int32_t         i;
		index_t         count;
		LineLayout      lineLayout;

		ts.mfSize  = 12;
		Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Tahoma"));

		for(i = 0; i < nEffectTestCount; i++)
		{
			ts.mEffect = 0x00001000 + i;
			pFontArray[i] = pFontServer->GetFont(&ts);
			EA_ASSERT(pFontArray[i]);
			bResult = pFontArray[i]->GetFontDescription(fd);
			EA_ASSERT(bResult && (fd.mEffect == ts.mEffect)); 
			EA_UNUSED(bResult);
		}

		for(i = 0; i < nEffectTestCount; i++)
		{
			count = (index_t)LayoutSimple("o", (uint32_t)strlen("o"), 0, 0, pFontArray[i], lineLayout);
			EATEST_VERIFY(count == strlen("o"));
		}

		for(i = 0; i < nEffectTestCount; i++)
		{
			pFontArray[i]->Release();
		}
	}

	// Shutdown
	glyphCache.Shutdown();

	return nErrorCount;
}



/*
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
*/


/*
static intptr_t FontTestThreadFunction(void* pvFontThreadData)
{
	using namespace EA::Text;

	FontThreadData* const pFontThreadData = (FontThreadData*)pvFontThreadData;

	// EA::Thread::ThreadId threadId = EA::Thread::GetThreadId();
	// EA::UnitTest::ReportVerbosity(2, "Font test function created: 0x%08x\n", (unsigned)(uintptr_t)threadId);

	while(!pFontThreadData->mbShouldQuit)
	{
		const int                kCharCount = 32;
		TextStyle                ts;
		GlyphId                  glyphs[kCharCount];
		Char                     ch[kCharCount];
		GlyphMetrics             gm;
		Font*                    pFont;
		const Font::GlyphBitmap* bm;
		int                      i;

		Strcpy16(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
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
*/


/////////////////////////////////////////////////////////////////////////////
// TestEffectsMultithreaded
//
// We test the following from multiple threads:
//    X font faces.
//    Y font sizes per face.
//    Z characters per font.
//
int TestEffectsMultithreaded()
{
	int nErrorCount = 0;

	/*
	using namespace EA::Thread;
	using namespace EA::Text;

	// Setup GlyphCache
	EA::Text::GlyphCache_Memory glyphCache;
	const int nResult = glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
	EATEST_VERIFY(nResult == 1);


	// Setup FontServer
	EA::Text::FontServer fontServer;
	pFontServer->Init();

	FilePathChar pFontPath[EA::IO::kMaxPathLength];

	GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
	Strcat16(pFontPath, EATEXT_FILE_PATH_CHAR("arial.ttf"));
	uint32_t nFaceCount = pFontServer->AddFace(pFontPath);
	EATEST_VERIFY(nFaceCount == 1);

	GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
	Strcat16(pFontPath, EATEXT_FILE_PATH_CHAR("cour.ttf"));
	nFaceCount = pFontServer->AddFace(pFontPath);
	EATEST_VERIFY(nFaceCount == 1);

	GetApplicationFontDirectory(pFontPath, EA::IO::kMaxPathLength);
	Strcat16(pFontPath, EATEXT_FILE_PATH_CHAR("verdana.ttf"));
	nFaceCount = pFontServer->AddFace(pFontPath);
	EATEST_VERIFY(nFaceCount == 1);


	// Setup FontThreadData
	FontThreadData fontThreadData;

	fontThreadData.mbShouldQuit = false;
	fontThreadData.mpFontServer = &fontServer;
	fontThreadData.mpGlyphCache = &glyphCache;

	fontThreadData.mFaceNameArray.push_back(eastl::string("Arial"));
	fontThreadData.mFaceNameArray.push_back(eastl::string("Courier New"));
	fontThreadData.mFaceNameArray.push_back(eastl::string("Verdana"));


	// Create threads
	const int kThreadCount(16);
	Thread thread[kThreadCount];
	Thread::Status status;
	int i;

	for(i = 0; i < kThreadCount; i++)
		thread[i].Begin(FontTestThreadFunction, &fontThreadData);

	EA::UnitTest::ThreadSleepRandom(10000, 10000);

	fontThreadData.mbShouldQuit = true;

	for(i = 0; i < kThreadCount; i++)
	{
		status = thread[i].WaitForEnd(GetThreadTime() + 30000);
		EATEST_VERIFY(status == Thread::kStatusEnded);
	}

	nErrorCount += (int)fontThreadData.mnErrorCount;

	pFontServer->Shutdown();
	glyphCache.Shutdown();
	*/

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestEffects
//
int TestEffects()
{
	int nErrorCount = 0;

	nErrorCount += TestEffectsBasic();
	nErrorCount += TestEffectsMultithreaded();

	return nErrorCount;
}
















