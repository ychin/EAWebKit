/////////////////////////////////////////////////////////////////////////////
// EAIO_Test.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
/////////////////////////////////////////////////////////////////////////////


#include <EAIOTest/EAIO_Test.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/PathString.h>
#include <EAIO/FnEncode.h>
#include <EAIO/version.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EATest/EATest.h>
#include <EAMain/EAEntryPointMain.inl>
#include <eathread/eathread_thread.h>

//Prevents false positive memory leaks on some platforms
#if defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)
	#define EA_MEMORY_GCC_USE_FINALIZE
#endif
#include <MemoryMan/MemoryMan.inl>

#include <MemoryMan/CoreAllocator.inl>
#include <stdio.h>

#ifdef EA_DLL
	#include <MemoryMan/MemoryManDLL.h>
#endif

#if defined(__APPLE__)
	#include <stdio.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
#endif

char gRootDirectory8[EA::IO::kMaxPathLength];
wchar_t gRootDirectoryW[EA::IO::kMaxPathLength];


#if !EASTL_EASTDC_VSNPRINTF
	// Required by EASTL.
	int Vsnprintf8(char* pDestination, size_t n, const char*  pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	#if (EASTDC_VERSION_N >= 10600)
		int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
		{
			return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
		}
	#endif

	#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
		int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments)
		{
			return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
		}
	#endif
#endif


bool gbVerboseOutput = false;


CoreAllocatorMalloc::CoreAllocatorMalloc()
  : mAllocationCount(0), mFreeCount(0)
{
}

CoreAllocatorMalloc::CoreAllocatorMalloc(const CoreAllocatorMalloc &)
  : mAllocationCount(0), mFreeCount(0)
{
}

CoreAllocatorMalloc::~CoreAllocatorMalloc()
{
}

CoreAllocatorMalloc& CoreAllocatorMalloc::operator=(const CoreAllocatorMalloc&)
{
	return *this;
}


int SetupDirectories()
{
	int nErrorCount = 0;

	// Get app root directory
	#if defined(EA_PLATFORM_MICROSOFT)
		#if (defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_CONSOLE) || defined(EA_PLATFORM_WINDOWS_PHONE)) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			EA::IO::GetTempDirectory(gRootDirectory8, EA::IO::kMaxPathLength);
			EA::IO::GetTempDirectory(gRootDirectoryW, EA::IO::kMaxPathLength);

			EA::IO::Path::EnsureTrailingSeparator(gRootDirectory8, EA::IO::kMaxPathLength);
			EA::StdC::Strlcat(gRootDirectory8, "EAIOTest\\", EA::IO::kMaxPathLength);
			EA::IO::Path::EnsureTrailingSeparator(gRootDirectoryW, EA::IO::kMaxPathLength);
			EA::StdC::Strlcat(gRootDirectoryW, "EAIOTest\\", EA::IO::kMaxPathLength);
		#else
			EA::StdC::Strlcpy(gRootDirectory8,  "C:\\temp\\EAIOTest\\", EA::IO::kMaxPathLength);
			EA::StdC::Strlcpy(gRootDirectoryW, L"C:\\temp\\EAIOTest\\", EA::IO::kMaxPathLength);   
		#endif

	#elif defined(EA_PLATFORM_ANDROID) || defined(EA_PLATFORM_APPLE)
		EA::IO::GetTempDirectory(gRootDirectory8, EA::IO::kMaxPathLength);
		EA::IO::GetTempDirectory(gRootDirectoryW, EA::IO::kMaxPathLength);

		EA::IO::Path::EnsureTrailingSeparator(gRootDirectory8, EA::IO::kMaxPathLength);
		EA::StdC::Strlcat(gRootDirectory8, "EAIOTest/", EA::IO::kMaxPathLength);
		EA::IO::Path::EnsureTrailingSeparator(gRootDirectoryW, EA::IO::kMaxPathLength);
		EA::StdC::Strlcat(gRootDirectoryW, "EAIOTest/", EA::IO::kMaxPathLength);

	#elif defined(EA_PLATFORM_STADIA)
		// These paths are different from the paths below in the unix section (which Stadia would fall into) since
		// the local directory on Stadia is read-only.
		EA::StdC::Strlcpy(gRootDirectory8,  "/tmp/EAIOTest/", EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(gRootDirectoryW, L"/tmp/EAIOTest/", EA::IO::kMaxPathLength);
		
	#elif defined(EA_PLATFORM_UNIX)
		// These paths were changed from "/tmp/..." to "./tmp/..." as creating temporary files 
		// in /tmp causes difficulties if the test harness crashes and these files are not
		// cleaned up. As the permissions are set by the creating user this makes it difficult
		// to run the same tests as another user on the same computer. 
		EA::StdC::Strlcpy(gRootDirectory8,  "./tmp/EAIOTest/", EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(gRootDirectoryW, L"./tmp/EAIOTest/", EA::IO::kMaxPathLength);

	#elif defined(EA_PLATFORM_SONY)
		// /hostapp/ is where the "file serving directory" is set. 
		// /app0/ is the working directory. When launched from the VS integrated debugger, it's whatever the working directory is set to be. 
		// /host/C:\... is a path that maps to a PC drive. It works only when the app is run under the debugger or target manager.
		// /data/ is a directory accessible on the Kettle HD, but is only available on dev kits
		// /temp0/ is a temporary directory accessible on the Kettle HD, which is available on both dev and test kits.
		EA::StdC::Strlcpy(gRootDirectory8,  "/temp0/EAIOTest/", EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(gRootDirectoryW, L"/temp0/EAIOTest/", EA::IO::kMaxPathLength);

		EA::IO::PS4GetTemporaryDataMount(NULL, 0, true);

	#elif defined(EA_PLATFORM_NX)
		EA::IO::GetTempDirectory(gRootDirectory8, EA::IO::kMaxPathLength);
		EA::IO::GetTempDirectory(gRootDirectoryW, EA::IO::kMaxPathLength);

		EA::IO::Path::EnsureTrailingSeparator(gRootDirectory8, EA::IO::kMaxPathLength);
		EA::StdC::Strlcat(gRootDirectory8, "EAIOTest/", EA::IO::kMaxPathLength);
		EA::IO::Path::EnsureTrailingSeparator(gRootDirectoryW, EA::IO::kMaxPathLength);
		EA::StdC::Strlcat(gRootDirectoryW, "EAIOTest/", EA::IO::kMaxPathLength);
	#else
		EA::StdC::Strlcpy(gRootDirectory8,  "/EAIOTest/", EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(gRootDirectoryW, L"/EAIOTest/", EA::IO::kMaxPathLength);

	#endif

	// Ensure that we are working with a clean test directory by removing it and re-creating it.
	EA::IO::Directory::Remove(gRootDirectory8, true);
	bool testDirExists = EA::IO::Directory::Create(gRootDirectory8);
	EATEST_VERIFY(testDirExists);

	// Get app temp directory
	char tempDirectory[EA::IO::kMaxPathLength]; tempDirectory[0] = '\0';
	EA::IO::GetTempDirectory(tempDirectory, EA::IO::kMaxPathLength);
	bool tmpDirExists = EA::IO::Directory::EnsureExists(tempDirectory);
	EATEST_VERIFY(tmpDirExists);

	return nErrorCount;
}

int TeardownDirectories()
{
	int nErrorCount = 0;

	// We failed to remove the directory. This is odd, but it might mean the system hasn't let go of a file handle yet
	// or that perhaps one of our tests hasn't closed a file. To get around the first case where the filesystem is just slow,
	// we will retry removing the directory tree a few times
	int i = 0;
	const int retries = 5;
	bool testDirExists;

	do
	{
		EA::IO::Directory::Remove(gRootDirectory8, true);
		testDirExists = EA::IO::Directory::Exists(gRootDirectory8);
		EA::Thread::ThreadSleep(200);
	}while(i++ < retries && !testDirExists);

	EATEST_VERIFY(!testDirExists);

	return nErrorCount;
}


int InitFileSystem()
{
	int nErrorCount = 0;

	EA::IO::SetAllocator(EA::Allocator::ICoreAllocator::GetDefaultAllocator());
	EA::IO::InitializeFileSystem(true);

	return nErrorCount;
}


int ShutdownFileSystem()
{
	int nErrorCount = 0;
	return nErrorCount;
}


extern int TestInternalAtomic();

int EAMain(int argc, char** argv)
{  
	int nErrorCount(0);

	using namespace EA::UnitTest;

	#if defined(EA_DLL) && defined(EA_MEMORY_ENABLED) && EA_MEMORY_ENABLED
		EA::Allocator::InitSharedDllAllocator();
	#endif

	nErrorCount += InitFileSystem();
	nErrorCount += SetupDirectories();

	if(nErrorCount)
		EA::EAMain::Report("Initialization error.\n");

	EA::EAMain::PlatformStartup();

	// Add the tests
	TestApplication testSuite("EAIO Unit Tests", argc, argv);

	testSuite.AddTest("InternalAtomic",   TestInternalAtomic);
	testSuite.AddTest("Async",			  TestAsync);
	testSuite.AddTest("FileBase",         TestFileBase);
	testSuite.AddTest("IniFile",          TestIniFile);
	testSuite.AddTest("PathString",       TestPathString);
	testSuite.AddTest("FnMatch",          TestFnMatch);
	testSuite.AddTest("EAStreamCpp",      TestEAStreamCpp);
	testSuite.AddTest("Stream",           TestStream);
	testSuite.AddTest("TextEncoding",     TestTextEncoding);
	testSuite.AddTest("File",             TestFile);
	testSuite.AddTest("FileNotification", TestFileNotification);
	testSuite.AddTest("FilePath",         TestFilePath);

	// Parse command line arguments
	EA::EAMain::CommandLine   commandLine(argc, argv);
	const char *sResult;

	if((commandLine.FindSwitch("-verbose", false, &sResult) >= 0) || 
		(commandLine.FindSwitch("-v",       false, &sResult) >= 0))
	{
		gbVerboseOutput = EA::StdC::Strcmp(sResult, "no") != 0;
	}
	
	nErrorCount += testSuite.Run();
	nErrorCount += TeardownDirectories();

	if(ShutdownFileSystem() != 0)
	{
		EA::EAMain::Report("Shutdown error.\n");
		nErrorCount++;
	}

	EA::EAMain::PlatformShutdown(nErrorCount);
 
	return nErrorCount;
}

#if defined(EA_PLATFORM_ANDROID)
extern jint jni_main(JNIEnv* environment, jobject activity);

extern "C" __attribute__ ((visibility("default"))) jint Java_com_ea_EAIOTest_EAIOTest_runEntryPoint(JNIEnv* environment, jobject activity)
{
    return jni_main(environment, activity);
}
#endif
