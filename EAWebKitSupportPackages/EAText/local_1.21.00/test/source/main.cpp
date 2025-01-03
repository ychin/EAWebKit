/////////////////////////////////////////////////////////////////////////////
// main.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include "EATextTest.h"
#include <EATest/EATest.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextFontServer.h>
#include <EASTL/string.h>
#include <EAIO/Allocator.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileUtil.h>
#include <EAStdC/EAString.h>
#include <MemoryMan/MemoryMan.h>
#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocator.inl>
#include <coreallocator/icoreallocator_interface.h>
#include <time.h>

#include "EAMain/EAEntryPointMain.inl"


///////////////////////////////////////////////////////////////////////////////
// External functions
//
extern int TestIterator();
extern int TestEffects();
extern int TestFreeType();
extern int TestStyle();
extern int TestStyleReader();
extern int TestScript();
extern int TestUnicode();
extern int TestBidi();
extern int TestCollation();
extern int TestTypesetter();
extern int TestBaseline();
extern int TestBreak();
extern int TestFont();
extern int TestBitmapFont();
extern int TestPolygonFont();
extern int TestCache();
extern int TestRender();
extern int TestOpenType();
extern int TestSpeed();


///////////////////////////////////////////////////////////////////////////////
// CustomCoreAllocator
//
class CustomCoreAllocator : public EA::Allocator::ICoreAllocator 
{
public:
	CustomCoreAllocator()
	  : mAllocationCount(0), mFreeCount(0)
	{ }

	~CustomCoreAllocator()
	{ }

	void* Alloc(size_t size, const char* name, unsigned flags)
	{
		mAllocationCount++;
		#if defined(EA_DEBUG)
			void* pMemory = EA::Allocator::gpEAGeneralAllocatorDebug->MallocAlignedDebug(size, 16, 0, (int)flags, 0, name, __FILE__, __LINE__);
		#else
			EA_UNUSED(name); EA_UNUSED(flags);
			void* pMemory = EA::Allocator::gpEAGeneralAllocator->MallocAligned(size, 16);
		#endif
		return pMemory;
	}

	void* Alloc(size_t size, const char* name, unsigned flags, unsigned align, unsigned alignOffset)
	{
		mAllocationCount++;
		#if defined(EA_DEBUG)
			void* pMemory = EA::Allocator::gpEAGeneralAllocatorDebug->MallocAlignedDebug(size, align, alignOffset, (int)flags, 0, name, __FILE__, __LINE__);
		#else
			EA_UNUSED(name); EA_UNUSED(flags);
			void* pMemory = EA::Allocator::gpEAGeneralAllocator->MallocAligned(size, align, alignOffset);
		#endif
		return pMemory;
	}

	void Free(void* block, size_t /*size*/)
	{
		mFreeCount++;
		#if defined(EA_DEBUG)
			EA::Allocator::gpEAGeneralAllocatorDebug->Free(block);
		#else
			EA::Allocator::gpEAGeneralAllocator->Free(block);
		#endif
	}

public:
	int32_t mAllocationCount;
	int32_t mFreeCount;
};

CustomCoreAllocator gCustomCoreAllocator;



/////////////////////////////////////////////////////////////////////////////
// InitFontServer
//
int InitFontServer()
{
	int nErrorCount = 0;

	// Set up a font server
	EA::Text::FontServer* const pFontServer = EA::Text::GetFontServer();
	EATEST_VERIFY(pFontServer != NULL);
	if(pFontServer)
	{
		EA::Text::FilePathChar pFontDirectory[EA::IO::kMaxPathLength];
		GetTestDataDirectory(pFontDirectory, EA::IO::kMaxPathLength);

		EA::Text::FilePathChar pFontPath[EA::IO::kMaxPathLength];
		EA::StdC::Strcpy(pFontPath, pFontDirectory);
		EA::StdC::Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("arial.ttf"));

		// Enable OpenType.
		pFontServer->SetOption(EA::Text::FontServer::kOptionOpenTypeFeatures, 1);
		pFontServer->AddFace(pFontPath);

		// On some systems that don't have system font directories,
		// the following code will have not affect. Our tests won't
		// be affected by this.
		// pFontServer->AddDirectory(pFontDirectory); Disabled because it's too much of an invitation to failure due to something funky about the files in the directory which are beyond our control.
	}

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// ShutdownFontServer
//
int ShutdownFontServer()
{
	int nErrorCount = 0;

	EA::Text::FontServer* const pFontServer = EA::Text::GetFontServer();
	EATEST_VERIFY(pFontServer != NULL);

	if(pFontServer)
	{
		pFontServer->FreeUnusedFonts();
		pFontServer->RemoveFace(EATEXT_CHAR("Arial"));
		pFontServer->Shutdown();
	}

	EA::Text::SetFontServer(NULL);

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// EAMain
//
int EAMain(int argc, char** argv)
{
	bool                    bPrintHelp(false);
	EA::EAMain::CommandLine commandLine(argc, argv);
	const char*             pResult;

	#if (EA_MEMORY_DEBUG_ENABLED == 1)
		EA::Allocator::gpEAGeneralAllocatorDebug->SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdCallStack);
	#endif

	#if defined(EA_PLATFORM_DESKTOP)
		EA::Allocator::gpEAGeneralAllocator->SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull, 8);
	#else
		EA::Allocator::gpEAGeneralAllocator->SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull, 64);
	#endif

	gpAppCommandLine = &commandLine;
	EA::EAMain::PlatformStartup();
	EA::IO::InitializeFileSystem(true);

	// Add all tests
	EA::UnitTest::TestApplication testSuite("EAText Unit Tests", argc, argv);

	testSuite.AddTest("Bidi",           TestBidi);
	testSuite.AddTest("Unicode",        TestUnicode);
	testSuite.AddTest("Collation",      TestCollation);
	testSuite.AddTest("Iterator",       TestIterator);
	testSuite.AddTest("Break",          TestBreak);
	testSuite.AddTest("Style",          TestStyle);
	testSuite.AddTest("StyleReader",    TestStyleReader);
	testSuite.AddTest("Effects",        TestEffects);
	testSuite.AddTest("FreeType",       TestFreeType);
	testSuite.AddTest("OpenType",       TestOpenType);
	testSuite.AddTest("Script",         TestScript);
	testSuite.AddTest("Typesetter",     TestTypesetter);
	testSuite.AddTest("Baseline",       TestBaseline);
	testSuite.AddTest("Font",           TestFont);
	testSuite.AddTest("BitmapFont",     TestBitmapFont);
	testSuite.AddTest("PolygonFont",    TestPolygonFont);
	testSuite.AddTest("Cache",          TestCache);
	testSuite.AddTest("Render",         TestRender);
	testSuite.AddTest("Speed",          TestSpeed);

	if((commandLine.FindSwitch("-s", false, &pResult) >= 0))
		EA::UnitTest::SetRandSeed(EA::StdC::AtoU32(pResult));

	if(commandLine.FindSwitch("-?", false, NULL) >= 0)
		bPrintHelp = true;

	if(bPrintHelp)
	{
		EA::UnitTest::Report("Options\n");
		EA::UnitTest::Report("   -w:<yes|no>   Wait at end. Default is no.\n");
		EA::UnitTest::Report("   -v:<yes|no>   Trace output that is unrelated to results. Default is no.\n");
		EA::UnitTest::Report("   -l:N          Test level in range of [1, 10]. 10 means max. Default is 5.\n");
		EA::UnitTest::Report("   -s:N          Specify a randomization seed. Default is 0 (use clock).\n");
		EA::UnitTest::Report("   -?            Show help.\n");
	}

	// Set up debug parameters.
	#ifdef EA_DEBUG
		EA::Allocator::gpEAGeneralAllocatorDebug->SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdGuard);
	#endif

	EA::Text::SetAllocator(&gCustomCoreAllocator);
	EA::IO::SetAllocator(&gCustomCoreAllocator);

	gRandom.SetSeed(EA::UnitTest::GetRandSeed());

	// Run tests
	EA::Text::Init();
	InitFontServer();
	const int testResult = testSuite.Run();
	ShutdownFontServer();
	EA::Text::Shutdown();

	EA::Allocator::gpEAGeneralAllocator->ValidateHeap(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull);

	EA::EAMain::PlatformShutdown(testResult);

	return testResult;
}



















