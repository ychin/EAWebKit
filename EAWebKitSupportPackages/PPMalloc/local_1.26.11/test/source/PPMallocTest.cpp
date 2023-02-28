/////////////////////////////////////////////////////////////////////////////
// PPMallocTest.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
	#pragma warning(disable: 4530 4548 4509)
	#pragma warning(disable : 4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif

#include <EABase/eabase.h>
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <EATest/EATest.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EAMain/EAMainInitFini.inl>
#include <EAMain/EAEntryPointMain.inl>
#include <EATest/EASTLVsnprintf.inl>
#include <eathread/eathread.h>
#include <coreallocator/icoreallocator_interface.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdlib.h>
#include <new>

#if defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT) && defined(_DEBUG)
	#include <crtdbg.h>
#endif

#if defined(EA_PLATFORM_WINDOWS)
	#pragma warning(push, 0)
	#include <Windows.h>
	#pragma warning(pop)
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifndef _Ret_maybenull_
#define _Ret_maybenull_
#endif

#ifndef _Post_writable_byte_size_
#define _Post_writable_byte_size_(x)
#endif

#ifndef _Ret_notnull_
#define _Ret_notnull_
#endif


///////////////////////////////////////////////////////////////////////////////
// gTestLengthSeconds
//
unsigned int gTestLengthSeconds = 10;

///////////////////////////////////////////////////////////////////////////////
// PPMallocTest_GetGeneralAllocator()
//
namespace EA
{
	namespace Allocator
	{
		static auto& PPMallocTest_GetGeneralAllocator()
		{
			static GeneralAllocatorDebug sEAGeneralAllocator;
			return sEAGeneralAllocator;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// Globals
//
size_t             gnAllocSizeMin(4);
size_t             gnAllocSizeMax(1000);
size_t             gnAllocSlotCount(5000);
size_t             gnAllocIterationCount(gnAllocSlotCount * 2);
size_t             gnAllocCycleCount(1);
size_t             gnNewCoreSize(0);
size_t             gnThreadCount(6);
int                gnAutoHeapValidationLevel(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull);
size_t             gnAutoHeapValidationFrequency(256);
bool               gbExtraHeapValidation(false);
EA::UnitTest::Rand gRand(1000);



///////////////////////////////////////////////////////////////////////////////
// ClampRange
//
void ClampRange(size_t& nValue, size_t nMin, size_t nMax)
{
   if(nValue  < nMin)
	  nValue  = nMin;
   if(nValue >= nMax)
	  nValue  = nMax;
}


///////////////////////////////////////////////////////////////////////////////
// PPMallocTest_CheckMemory_Imp
//
int PPMallocTest_CheckMemory_Imp(const char* pFile, int nLine)
{
	int  nErrorCount(0);
	bool bMemoryOK(true);

	if(gbExtraHeapValidation)
	{
		#if defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT) && defined(_DEBUG)
			if(!_CrtCheckMemory())
				bMemoryOK = false;
		#endif
		
		#ifdef EA_DEBUG
			if(!EA::Allocator::PPMallocTest_GetGeneralAllocator().ValidateHeap(EA::Allocator::GeneralAllocator::kHeapValidationLevelBasic))
				bMemoryOK = false;
		#endif
	}

	EATEST_VERIFY_MSG(bMemoryOK, "Memory check failure");

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// SetupParameters
//
void SetupParameters(int argc, char* argv[])
{
	using namespace EA::StdC;

	// Scale options default values based on platform.
	const float systemSpeed = GetSystemSpeed(EA::UnitTest::kSpeedTypeCPU);

	gnAllocSlotCount      = (size_t)((gnAllocSlotCount * systemSpeed) + 0.5f);
	gnAllocIterationCount = (gnAllocSlotCount * 2);
	gnThreadCount         = (size_t)((((gnThreadCount * systemSpeed) + 0.5f) >= 2) ? ((gnThreadCount * systemSpeed) + 0.5f) : 2);

	// Get working parameters.
	for(int a(1); a < argc; a++)
	{
		if(!Stricmp(argv[a], "-Min") && (a < (argc - 1)))
			gnAllocSizeMin = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-Max") && (a < (argc - 1)))
			gnAllocSizeMax = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-SlotCount") && (a < (argc - 1)))
			gnAllocSlotCount = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-IterationCount") && (a < (argc - 1)))
			gnAllocIterationCount = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-CycleCount") && (a < (argc - 1)))
			gnAllocCycleCount = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-NewCoreSize") && (a < (argc - 1)))
			gnNewCoreSize = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-ThreadCount") && (a < (argc - 1)))
			gnThreadCount = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-ValidationFrequency") && (a < (argc - 1)))
			gnAutoHeapValidationFrequency = (size_t)atoi(argv[a + 1]);

		if(!Stricmp(argv[a], "-ExtraHeapValidation"))
			gbExtraHeapValidation = true;
	}

	// Possibly fix any parameters.
	ClampRange(gnAllocSizeMin, 0, 100000000);
	ClampRange(gnAllocSizeMax, 0, 100000000);
	ClampRange(gnAllocSizeMin, 0, gnAllocSizeMax - 1);
	ClampRange(gnAllocSlotCount, 1, 100000000);
	ClampRange(gnAllocIterationCount, 1, 100000000);
	ClampRange(gnAllocCycleCount, 1, 100000000);
	ClampRange(gnThreadCount, 1, 32);

	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	// Display working parameters.
	EA::UnitTest::ReportVerbosity(1, "\n");
	EA::UnitTest::ReportVerbosity(1, "Alloc min size: %u\n",        (unsigned)gnAllocSizeMin);
	EA::UnitTest::ReportVerbosity(1, "Alloc max size: %u\n",        (unsigned)gnAllocSizeMax);
	EA::UnitTest::ReportVerbosity(1, "Alloc slot count: %u\n",      (unsigned)gnAllocSlotCount);
	EA::UnitTest::ReportVerbosity(1, "Cycle count: %u\n",           (unsigned)gnAllocCycleCount);
	EA::UnitTest::ReportVerbosity(1, "Alloc iteration count: %u\n", (unsigned)gnAllocIterationCount);
	EA::UnitTest::ReportVerbosity(1, "Thread count: %u\n",          (unsigned)gnThreadCount);
	EA::UnitTest::ReportVerbosity(1, "Interactive: %s\n",           EA::UnitTest::GetInteractive() ? "yes" : "no");
	EA::UnitTest::ReportVerbosity(1, "Rand seed: %u\n",             (unsigned)gRand.GetSeed());
	EA::UnitTest::ReportVerbosity(1, "Extra heap validation: %s\n", gbExtraHeapValidation ? "yes" : "no");
}


#ifdef EA_DLL
	// We create a bogus TestSmallBlockAllocator function here in DLL builds.
	// The reason we do this is that the regular TestSmallBlockAllocator function
	// is currently in the extras directory and doesn't support building in 
	// DLL mode. So we just make a no-op version here to satisfy the linker.
	int TestSmallBlockAllocator()
	{
		return 0;
	}
#endif



///////////////////////////////////////////////////////////////////////////////
// operator new/delete helper functions to deal with static-init order issues
///////////////////////////////////////////////////////////////////////////////
namespace Detail
{
	static void* Malloc(size_t size, int flags)
	{
		void* mem = EA::Allocator::PPMallocTest_GetGeneralAllocator().Malloc(size, flags);

		if(mem == nullptr)
			mem = malloc(size);

		return mem;
	};

	static void* MallocAligned(size_t size, size_t alignment, size_t alignmentOffset, int flags)
	{
		void* mem = EA::Allocator::PPMallocTest_GetGeneralAllocator().MallocAligned(size, alignment, alignmentOffset, flags);

		if(mem == nullptr)		
		{
			// perform an aligned allocation via malloc
        	const size_t newSize = (size + (alignment - 1));
			size_t address = (size_t)(malloc(newSize));
			mem = (void*)(address & ~(alignment - 1));
		}

		return mem;
	};

	static void Free(void* p)
	{
		if (p)
		{
			if (EA::Allocator::PPMallocTest_GetGeneralAllocator().ValidateAddress(p, EA::Allocator::GeneralAllocator::kAddressTypeOwned) == p)
				return EA::Allocator::PPMallocTest_GetGeneralAllocator().Free(p);
			else
				return free(p);
		}
	}
} // namespace Detail




///////////////////////////////////////////////////////////////////////////////
// operator new overrides
//
// EASTL requires some of the following new operators to be defined.
// Some platform linked-in libraries also require some of the following.
///////////////////////////////////////////////////////////////////////////////
void* operator new[](size_t size, const char* /*pName*/, int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	return Detail::Malloc(size, flags);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* /*pName*/, int flags, 
						unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	return Detail::MallocAligned(size, alignment, alignmentOffset, flags);
}

void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE()
{
	return Detail::Free(p);
}

_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new[](size_t size)
{
	void* pMemory = Detail::Malloc(size, 0);

#if !defined(EA_COMPILER_NO_EXCEPTIONS)
	if (pMemory == NULL)
	{
		throw std::bad_alloc();
	}
#endif

	return pMemory;
}

_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new[](size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
{
	return operator new[](size);
}

void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
{
	return operator delete[](p);
}

_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new(size_t size)
{
	return operator new[](size);
}

_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new(size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
{
	return operator new[](size);
}

namespace
{
	// ICAWrapper is a simple wrapper to provide an ICoreAllocator interface on top of a GeneralAllocatorDebug.
	//
	// Note: MemoryMan also provides a wrapper for this purpose, but PPMalloc tests intentionally provide custom
	// new operators which could cause problems if MemoryMan is also linked in.
	// Therefore, to avoid conflicts we have implemented this wrapper instead of using MemoryMan.
	class ICAWrapper : public EA::Allocator::ICoreAllocator
	{
	public:
		ICAWrapper(EA::Allocator::GeneralAllocatorDebug *generalAllocatorDebug)
			:mpGeneralAllocatorDebug(generalAllocatorDebug)
		{
		}

		virtual ~ICAWrapper()
		{
			mpGeneralAllocatorDebug = NULL;
		}

		virtual void* Alloc(size_t size, const char* name, unsigned int flags)
		{
			return mpGeneralAllocatorDebug->MallocDebug(size, (int)flags, /*nDebugDataFlags*/ 0, name, /*pFile*/ NULL, /*nLine*/ 0);
		}

		virtual void* Alloc(size_t size, const char* name, unsigned int flags, 
				unsigned int align, unsigned int alignOffset)
		{
			return mpGeneralAllocatorDebug->MallocAlignedDebug(size, (size_t)align, (size_t)alignOffset, (int)flags, /*nDebugDataFlags*/ 0, name, /*pFile*/ NULL, /*nLine*/ 0);
		}

		virtual void Free(void* pData, size_t /*size*/)
		{
			return mpGeneralAllocatorDebug->Free(pData);
		}

	private:
		EA::Allocator::GeneralAllocatorDebug *mpGeneralAllocatorDebug;
	};
}

EA::Allocator::ICoreAllocator* EA::Allocator::ICoreAllocator::GetDefaultAllocator()
{
	static ICAWrapper sICAWrapper(&PPMallocTest_GetGeneralAllocator());

	return &sICAWrapper;
}

void EAMainInit()
{
	using namespace EA::Allocator;

	EA::UnitTest::SetLowProcessPriority();

	// This allocator is just our general purpose allocator which can be used for arbitrary test memory.
	EA::Allocator::SetGeneralAllocator(&PPMallocTest_GetGeneralAllocator());
	PPMallocTest_GetGeneralAllocator().SetOption(EA::Allocator::GeneralAllocatorDebug::kOptionEnablePtrValidation, 0);
	PPMallocTest_GetGeneralAllocator().SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelBasic, 64);
	PPMallocTest_GetGeneralAllocator().SetGuardSize(3.f, 128, 1024);
	PPMallocTest_GetGeneralAllocator().SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdGuard);
}

void EAMainFini()
{
	// The EAMainInit/EAMainFini functionality requires both an EAMainInit
	// and EAMainFini function to be defined. However we have no work to do
	// in EAMainFini, so this function definition is left empty on purpose.
}

///////////////////////////////////////////////////////////////////////////////
// EAMain
//
int EAMain(int argc, char** argv)
{
	using namespace EA::Allocator;

	gTestLengthSeconds = (unsigned int)(10.f * EA::UnitTest::GetSystemSpeed());
	if (gTestLengthSeconds == 0)
		gTestLengthSeconds = 5;

	EA::UnitTest::TestApplication testSuite("PPMalloc Unit Tests", argc, argv);

	testSuite.AddTest("Speed",                  TestSpeed);
	testSuite.AddTest("StackAllocator",         TestStackAllocator);
	testSuite.AddTest("FixedAllocator",         TestFixedAllocator);
	testSuite.AddTest("NewDelete",              TestNewDelete);
	testSuite.AddTest("Recorder",               TestRecorder);
	testSuite.AddTest("GeneralAllocator",       TestGeneralAllocator);
	testSuite.AddTest("SmallObjectAllocator",   TestSmallObjectAllocator);
	testSuite.AddTest("NonLocalAllocator",      TestNonLocalAllocator);
	testSuite.AddTest("SmallBlockAllocator",    TestSmallBlockAllocator);
	testSuite.AddTest("ScratchpadAllocator",    TestScratchpadAllocator);

	SetupParameters(argc, argv); // Need to call this after testSuite is constructed, because testSuite sets up EATest config values that SetupParameters uses.

	ICAWrapper icaWrapper(&PPMallocTest_GetGeneralAllocator());
	EA::Thread::SetAllocator(&icaWrapper);

	int testResult = testSuite.Run();

	if(EA::EAMain::GetVerbosity() > 0)
		PPMallocTest_GetGeneralAllocator().TraceAllocatedMemory();

	return testResult;
}
