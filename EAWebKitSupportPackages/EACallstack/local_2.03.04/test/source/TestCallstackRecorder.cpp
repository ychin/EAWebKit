///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif


///////////////////////////////////////////////////////////////////////////////
// TestCallstackRecorder
//
EA::Callstack::CallstackRecorder* gpCallstackRecorder;

struct TCRTest
{
	int mRefCount;

	TCRTest() : mRefCount(0){ }

	int AddRef();
	int Release();
};

int TCRTest::AddRef()
{
	RecordCurrentCallstack(*gpCallstackRecorder, "SomeClass::AddRef", (uintptr_t)this);

	return ++mRefCount;
}

int TCRTest::Release()
{
	RecordCurrentCallstack(*gpCallstackRecorder, "SomeClass::Release", (uintptr_t)this);

	if(mRefCount > 1)
		return --mRefCount;
	delete this;
	return 0;
}

void TCR1(TCRTest* p);
void TCR1(TCRTest* p)
{
	p->AddRef();
	p->Release();
}

void TCR2(TCRTest* p);
void TCR2(TCRTest* p)
{
	TCR1(p);
}

void TCR3(TCRTest* p);
void TCR3(TCRTest* p)
{
	TCR2(p);
}

EA_OPTIMIZE_OFF();

static const int MAX_CALLSTACK_TABLE = 64;
volatile int gHackToPreventUnroll = 2;
volatile int gHackZeroForTailRecursionSuppression = 0; // we include this in the returned expression so the compiler cannot perform tail call optimization

// Nintendo's nn::diag::GetBacktrace has some issues with this test when these variables
// are local to the function. So as per their recommendation on the forums, we make these
// global here.
//
// https://developer.nintendo.com/group/development/g1kr9vj6/forums/english/-/gts_message_boards/thread/291232381#latest
void*  pCallstack0[MAX_CALLSTACK_TABLE];
void*  pCallstack1[MAX_CALLSTACK_TABLE];
void*  pCallstackRecursive[MAX_CALLSTACK_TABLE];

EA_NO_INLINE EA::Callstack::callstack_hash_t  GetHashUnique_Function0(int recursionDepthRemaining)
{
	if(recursionDepthRemaining == 0)
	{
		size_t entryCount = EA::Callstack::GetCallstack(pCallstack0, MAX_CALLSTACK_TABLE, NULL);
		return EA::Callstack::GetCallstackHash( pCallstack0, entryCount);
	}
	else
	{
		return GetHashUnique_Function0(recursionDepthRemaining - 1);
	}
}

 EA_NO_INLINE EA::Callstack::callstack_hash_t  GetHashUnique_Function1(int recursionDepthRemaining)
{
	if(recursionDepthRemaining == 0)
	{
		size_t entryCount = EA::Callstack::GetCallstack(pCallstack1, MAX_CALLSTACK_TABLE, NULL);
		return EA::Callstack::GetCallstackHash( pCallstack1, entryCount);
	}
	else
	{
		return GetHashUnique_Function0(recursionDepthRemaining - 1);
	}
}

EA_NO_INLINE EA::Callstack::callstack_hash_t  RecursiveCallToCheckUniqueness(int recursionDepthRemaining)
{
	if (recursionDepthRemaining == 0)
	{
		size_t entryCount = EA::Callstack::GetCallstack(pCallstackRecursive, MAX_CALLSTACK_TABLE+32, NULL);
		return EA::Callstack::GetCallstackHash( pCallstackRecursive, entryCount) | gHackZeroForTailRecursionSuppression;
	}
	else
	{
		return RecursiveCallToCheckUniqueness(recursionDepthRemaining-1) | gHackZeroForTailRecursionSuppression;
	}
}

EA_NO_INLINE int TestCallstackHashFunctionality()
{
	int nErrorCount = 0;
	for (int i = 0; i < MAX_CALLSTACK_TABLE; i++)
	{
		for (int j = 0; j < MAX_CALLSTACK_TABLE; j++)
		{
			EA::Callstack::callstack_hash_t checkValArray[2];
			for (int k = 0; k < gHackToPreventUnroll; k++)
			{
				checkValArray[k] = RecursiveCallToCheckUniqueness( (k ==0) ? i : j); // clever trick to have the same callsite for both callstacks
			}

			bool bSame = (checkValArray[0] == checkValArray[1]);
			if (i != j)
			{
				EATEST_VERIFY_MSG(!bSame, "FAILED: GetCallstackHash reports hash collision for unique recursive function calls (expected different hashes).\n");
			}
			else
			{
				EATEST_VERIFY_MSG(bSame, "FAILED: GetCallstackHash reports hash collision for unique recursive function calls (expected same hashes).\n");
			}
		}
	}


	EATEST_VERIFY_MSG(GetHashUnique_Function0(gHackToPreventUnroll) != GetHashUnique_Function1(gHackToPreventUnroll), "FAILED: GetCallstackHash reports hash collision for unique functions\n");


	return nErrorCount;
}

EA_OPTIMIZE_ON();

int TestCallstackRecorder()
{
	using namespace EA::Callstack;

	EA::UnitTest::ReportVerbosity(1, "\nTestCallstackRecorder\n");

	int nErrorCount = 0;

	nErrorCount += TestCallstackHashFunctionality();

	gpCallstackRecorder = CORE_NEW(EA::Allocator::ICoreAllocator::GetDefaultAllocator(), "EACallstack/CallstackRecorder", 0) EA::Callstack::CallstackRecorder;

	TCRTest* const p = new TCRTest;

	p->AddRef();

	for(int i = 0; i < 2; i++)
	{
		TCR1(p);
		TCR2(p);
		TCR3(p);
	}

	p->Release();

	// Set up an AddressRepCache. Normally in a real application you
	// wouldn't save and restore these; you'd just create one for the 
	// application.
	AddressRepCache arc;
	AddressRepCache* const pARCSaved = SetAddressRepCache(&arc);

	// Set up the debug info settings.
	wchar_t dbPath[EA::IO::kMaxPathLength] = { 0 };

	if(gpApplicationPath)
		EA::StdC::Strlcpy(dbPath, gpApplicationPath, EA::IO::kMaxPathLength);
	else
		EA::StdC::GetCurrentProcessPath(dbPath);


	if (!dbPath[0])
	{
		// What to do?
	}

	// On Microsoft platforms we use the .pdb or .map file associated 
	// with the executable. On GCC-based platforms, the executable is 
	// an ELF file which has the debug information built into it.
	#if defined(_MSC_VER)
		EA::StdC::Strcpy(EA::IO::Path::GetFileExtension(dbPath), L".map");
		arc.AddDatabaseFile(dbPath);

		EA::StdC::Strcpy(EA::IO::Path::GetFileExtension(dbPath), L".pdb");
		arc.AddDatabaseFile(dbPath);
	#else
		arc.AddDatabaseFile(dbPath);
	#endif

	// Write out the results.
	gpCallstackRecorder->SetName("Callstack Test");

	// Need to use a debug output stream instead of a file stream.
	// Need a way to verify results, as the code here just prints them for inspecting.
	if(EA::UnitTest::GetVerbosity() >= 1)
	{
		EA::IO::StreamLog fs1;
		fs1.AddRef();
		EA::Callstack::TraceCallstackRecording(*gpCallstackRecorder, &fs1, true);

		EA::IO::StreamLog fs2;
		fs2.AddRef();
		EA::Callstack::TraceCallstackRecording(*gpCallstackRecorder, &fs2, false);
	}

	// Restore old AddressRepCache.
	SetAddressRepCache(pARCSaved);

	CORE_FREE(EA::Allocator::ICoreAllocator::GetDefaultAllocator(), gpCallstackRecorder);

	return nErrorCount;
}

// TODO(mwinkler): This test is really slow and times out on debug builds. Figure out what is slow
#if !defined(EA_DEBUG)

TEST(EACallstackTest, TestCallstackRecorder)
{
	int nErrorCount = TestCallstackRecorder();

	EXPECT_EQ(nErrorCount, 0);
}

#endif

EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();


