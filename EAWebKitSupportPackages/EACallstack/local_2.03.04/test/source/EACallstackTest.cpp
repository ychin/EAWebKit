///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

#include <EAMain/EAEntryPointMain.inl>

#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocator.inl>


// Global variables
void*                        gpAddress = (void*)-1;
CustomCoreAllocator          gCustomCoreAllocator;
eastl::list<eastl::wstring>  gTestFileList;
const char*                  gpApplicationPath = NULL;
EA::UnitTest::TestApplication* gpTestApplication = nullptr;


///////////////////////////////////////////////////////////////////////////////
// SetupTestFileList
///////////////////////////////////////////////////////////////////////////////
static void SetupTestFileList(int argc, char** argv)
{
#if defined(EA_PLATFORM_ANDROID)
	return;
#endif
	using namespace EA::IO;

	EA::EAMain::CommandLine commandLine(argc, argv);
	EA::IO::Path::PathStringW dataDirectory;
	const char *sResult;

	// Example: Text.exe -d:"C:\Dir\my data"
	if(commandLine.FindSwitch("-d", false, &sResult) >= 0)
	{
		size_t resultLength = EA::StdC::Strlen(sResult);

		// Convert from char to wchar_t.
		int nRequiredStrlen = EA::StdC::Strlcpy((wchar_t*)NULL, sResult, 0, resultLength);
		dataDirectory.resize((eastl_size_t)nRequiredStrlen + 1);
		EA::StdC::Strlcpy(&dataDirectory[0], sResult, dataDirectory.capacity(), resultLength);
		EA::IO::Path::EnsureTrailingSeparator(dataDirectory);
	}
	else
	{
		#if defined (EA_PLATFORM_IPHONE)
			wchar_t homeDirectory[EA::IO::kMaxDirectoryLength] = { 0 };
			wchar_t appName[EA::IO::kMaxDirectoryLength] = { 0 };
			//EA::IO::appleGetHomeDirectory(homeDirectory, EA::IO::kMaxDirectoryLength); Disabled until we can test this.
			//EA::IO::appleGetAppName(appName, EA::IO::kMaxDirectoryLength);
			dataDirectory.assign(homeDirectory);
			dataDirectory += EA_WCHAR("/");
			dataDirectory += appName;
			dataDirectory += EA_WCHAR(".app/assets/");
		#elif defined (EA_PLATFORM_ANDROID)
			dataDirectory.assign(EA_WCHAR("appbundle:/data/"));
		#elif defined (EA_PLATFORM_NX)
			if (nn::fs::CanMountRomForDebug())
				dataDirectory.assign(EA_WCHAR("rom:/data/"));
		#else
			dataDirectory.resize(256);
			dataDirectory.resize((unsigned)EA::IO::Directory::GetCurrentWorkingDirectory(&dataDirectory[0]));
			#if !defined(EA_PLATFORM_XBOXONE) && !defined(EA_PLATFORM_XBSX)
				EA::IO::Path::Append(dataDirectory,EA_WCHAR("./assets/"));
				EA::IO::Path::Normalize(dataDirectory);
			#endif
		#endif
	}

	if(!dataDirectory.empty())
	{
		DirectoryIterator            di;
		DirectoryIterator::EntryList entryList;

		di.ReadRecursive(dataDirectory.c_str(), entryList, EA_WCHAR("*"), kDirectoryEntryFile, true, true);

		for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
		{
			const DirectoryIterator::Entry& entry = *it;
			gTestFileList.push_back(entry.msName.c_str());
		}
	}

	if(gTestFileList.empty())
	{
		EA::UnitTest::Report("No data files were found.\n");
		EA::UnitTest::Report("You need to run with the working directory set to the full path to EACallstack/dev/test/source/data or\n");
		EA::UnitTest::Report("you need to run with the -d <path> command line argument\n");
		EA::UnitTest::Report("The current data directory is \"%ls\".\n", dataDirectory.c_str());
	}
}


///////////////////////////////////////////////////////////////////////////////
// EAMain
//
int EAMain(int argc, char** argv)
{
	int nErrorCount = 0;

	EA::EAMain::PlatformStartup();

	gpApplicationPath = (argc ? argv[0] : nullptr);


#if defined(EA_DEBUG) && EA_MEMORY_PPMALLOC_ENABLED

	EA::Allocator::gpEAGeneralAllocatorDebug->SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdCallStack);
	EA::Allocator::gpEAGeneralAllocatorDebug->SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdGuard);

#endif

	/* Set Allocator */
	EA::IO::SetAllocator(EA::Allocator::ICoreAllocator::GetDefaultAllocator());
	EA::Callstack::SetAllocator(EA::Allocator::ICoreAllocator::GetDefaultAllocator());

	/* Init Modules */
	bool fsInitSuccess = EA::IO::InitializeFileSystem(true);
	EA_UNUSED(fsInitSuccess);
	EA_ASSERT(fsInitSuccess);
	EA::Callstack::InitCallstack();

	SetupTestFileList(argc, argv);

	// Add the tests
	EA::UnitTest::TestApplication testSuite("EACallstack Unit Tests", argc, argv);
	gpTestApplication = &testSuite;

	::testing::InitGoogleTest(&argc, argv);
	int testResult = RUN_ALL_TESTS();

	EA::Callstack::ShutdownCallstack();

	// Add in the global errors.
	nErrorCount += EA::UnitTest::GetGlobalErrorCount();

	nErrorCount += testResult;

	EATEST_VERIFY(gCustomCoreAllocator.mAllocationCount == 0);
	EATEST_VERIFY(gCustomCoreAllocator.mFreeCount == 0);

	EA::EAMain::PlatformShutdown(nErrorCount);

	return nErrorCount;
}
