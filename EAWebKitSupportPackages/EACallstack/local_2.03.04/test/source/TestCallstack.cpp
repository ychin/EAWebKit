///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif


#if EA_CALLSTACK_GETCALLSTACK_SUPPORTED

intptr_t Abs(intptr_t x)
{
	return (x < 0 ? -x : x);
}

struct CallstackEntryInfo
{
	void*       mpAddress;          //
	InfoString  msFunction;         //
	int         mnFunctionOffset;   // A value of -1 means we don't know.
	InfoString  msFile;             //
	int         mnFileLine;         //
	InfoString  msSource;           //

	CallstackEntryInfo()
		: mpAddress(nullptr)
		, msFunction()
		, mnFunctionOffset(-1)
		, msFile()
		, mnFileLine(-1)
		, msSource()
	{
	}

	CallstackEntryInfo(void* pAddress, const char* pFunction, int nFunctionOffset, const char* pFile, int fileLine, const char* pSource)
		: mpAddress(pAddress)
		, msFunction(pFunction ? pFunction : "")
		, mnFunctionOffset(nFunctionOffset)
		, msFile(pFile ? pFile : "")
		, mnFileLine(fileLine)
		, msSource(pSource ? pSource : "")
	{
	}

	void Clear()
	{
		mpAddress = nullptr;
		msFunction.clear();
		mnFunctionOffset = -1;
		msFile.clear();
		mnFileLine = -1;
		msSource.clear();
	}
};

typedef eastl::vector<CallstackEntryInfo> CallstackEntryInfoArray;


EA_NO_INLINE void TestCallstack01(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries);
EA_NO_INLINE void TestCallstack02(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries);
EA_NO_INLINE void TestCallstack03(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries);
EA_NO_INLINE void TestCallstack04(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries);
EA_NO_INLINE void TestCallstack05(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries);

///////////////////////////////////////////////////////////////////////////////
// VerifyCallstack
//
EA_NO_INLINE void VerifyCallstack(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	using namespace EA::Callstack;

	void* pCallstack[32];
	void* verifyCallstackIP;

	EA::Callstack::GetInstructionPointer(verifyCallstackIP);
	size_t nCallstackDepth = EA::Callstack::GetCallstack(pCallstack, 32, nullptr); int nFirstCallstackLineNumber = __LINE__;

	expectedEntries.insert(expectedEntries.begin(),
						   CallstackEntryInfo(verifyCallstackIP, __FUNCTION__, -1, __FILE__, nFirstCallstackLineNumber, nullptr));

	/**
	 * Lookup the Return Addresses via Debug Symbols and make sure the information matches the expectedEntries that was populated.
	 */
	CallstackEntryInfoArray reportedEntries;

	for (eastl_size_t i = 0; (i < nCallstackDepth) && (i < expectedEntries.size()); ++i)
	{
		const char* pStrResults[kARTCount];
		int         pIntResults[kARTCount];
		int         nAddressRepFlags     = (kARTFlagFunctionOffset | kARTFlagFileLine | kARTFlagSource | kARTFlagAddress);
		int         nAddressRepReturnVal = arc.GetAddressRep(nAddressRepFlags, reinterpret_cast<uint64_t>(pCallstack[i]),
															 pStrResults, pIntResults);

		ASSERT_TRUE(nAddressRepReturnVal != 0);

		CallstackEntryInfo ei;

		ei.mpAddress = pCallstack[i];

		if(nAddressRepReturnVal & kARTFlagFunctionOffset)
		{
			ei.msFunction       = pStrResults[kARTFunctionOffset];
			ei.mnFunctionOffset = pIntResults[kARTFunctionOffset];
		}

		if(nAddressRepReturnVal & kARTFlagFileLine)
		{
			ei.msFile     = pStrResults[kARTFileLine];
			ei.mnFileLine = pIntResults[kARTFileLine];
		}

		// Verify that the source result is at least non-NULL. This will fail if this build doesn't have debug symbols available.
		if (nAddressRepReturnVal & kARTFlagSource)
		{
			ASSERT_TRUE(pStrResults[kARTSource] != nullptr);
		}

		reportedEntries.push_back(ei);
	}

	/**
	 * Verify each entry in the reference Callstack against the Callstack return by EA::Callstack
	 * Compare expectedEntries with reportedEntries
	 */
	for (eastl_size_t i = 0; (i < reportedEntries.size()); ++i)
	{
		const CallstackEntryInfo& entryInfoReported = reportedEntries[i];
		const CallstackEntryInfo& entryInfoExpected = expectedEntries[i];

		/* Test the Return IPs */
		{
			const intptr_t addressExpected = (intptr_t)entryInfoExpected.mpAddress;
			const intptr_t addressReported = (intptr_t)entryInfoReported.mpAddress;
			const intptr_t addressDiff = Abs(addressExpected - addressReported);
			const intptr_t kAddressDiffMax = 1024;

			EXPECT_TRUE((addressDiff < kAddressDiffMax) ||
						((entryInfoExpected.mnFileLine > 1) && (abs(entryInfoExpected.mnFileLine - entryInfoReported.mnFileLine) < 5)));
		}

		/* Test the function name. */
#if EACALLSTACK_NATIVE_FUNCTION_LOOKUP_AVAILABLE

		{
			const char* funcExpected = entryInfoExpected.msFunction.c_str();
			const char* funcReported = entryInfoReported.msFunction.c_str();

			EXPECT_TRUE(EA::StdC::Strstr(funcReported, funcExpected) != nullptr);
		}

#endif

		/* Test the file/line file name. */
#if EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE

		{
			const char* fileExpected = EA::IO::Path::GetFileName(entryInfoExpected.msFile.c_str());
			const char* fileReported = EA::IO::Path::GetFileName(entryInfoReported.msFile.c_str());

			EXPECT_TRUE(EA::StdC::Stricmp(fileExpected, fileReported) == 0);
		}

#endif

		/* Test the file/line line. */
#if EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE

		{
			const int lineExpected = entryInfoExpected.mnFileLine;
			const int lineReported = entryInfoReported.mnFileLine;

			EXPECT_LE(abs(lineExpected - lineReported), 15);
		}

#endif

	}

	expectedEntries.erase(expectedEntries.begin());
}


EA_NO_INLINE void TestCallstack05(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	void* pAddress;
	EA::Callstack::GetInstructionPointer(pAddress);
	CallstackEntryInfo ei(pAddress, __FUNCTION__, -1, __FILE__, __LINE__, NULL);
	expectedEntries.insert(expectedEntries.begin(), ei);
	VerifyCallstack(arc, expectedEntries);
}

EA_NO_INLINE void TestCallstack04(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	void* pAddress;
	EA::Callstack::GetInstructionPointer(pAddress);
	CallstackEntryInfo ei(pAddress, __FUNCTION__, -1, __FILE__, __LINE__, NULL);
	expectedEntries.insert(expectedEntries.begin(), ei);
	VerifyCallstack(arc, expectedEntries);
	TestCallstack05(arc, expectedEntries);
}

EA_NO_INLINE void TestCallstack03(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	void* pAddress;
	EA::Callstack::GetInstructionPointer(pAddress);
	CallstackEntryInfo ei(pAddress, __FUNCTION__, -1, __FILE__, __LINE__, NULL);
	expectedEntries.insert(expectedEntries.begin(), ei);
	VerifyCallstack(arc, expectedEntries);
	TestCallstack04(arc, expectedEntries);
}

EA_NO_INLINE void TestCallstack02(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	void* pAddress;
	EA::Callstack::GetInstructionPointer(pAddress);
	CallstackEntryInfo ei(pAddress, __FUNCTION__, -1, __FILE__, __LINE__, NULL);
	expectedEntries.insert(expectedEntries.begin(), ei);
	VerifyCallstack(arc, expectedEntries);
	TestCallstack03(arc, expectedEntries);
}

EA_NO_INLINE void TestCallstack01(EA::Callstack::AddressRepCache& arc, CallstackEntryInfoArray& expectedEntries)
{
	void* pAddress;
	EA::Callstack::GetInstructionPointer(pAddress);
	CallstackEntryInfo ei(pAddress, __FUNCTION__, -1, __FILE__, __LINE__, NULL);
	expectedEntries.insert(expectedEntries.begin(), ei);
	VerifyCallstack(arc, expectedEntries);
	TestCallstack02(arc, expectedEntries);
}

int SetUpARC(EA::Allocator::ICoreAllocator* pCoreAllocator, EA::Callstack::AddressRepCache& arc, EA::Callstack::AddressRepCache** oldArc)
{
	/* Get the path to the symbol database for the executing module. */
	wchar_t dbDirectory[EA::IO::kMaxPathLength] = { 0 };
	wchar_t dbPath[EA::IO::kMaxPathLength] = { 0 };

	if (gpApplicationPath)
	{
		EA::StdC::Strlcpy(dbPath, gpApplicationPath, EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(dbDirectory, gpApplicationPath, EA::IO::kMaxPathLength);
		wchar_t* p = EA::IO::Path::GetFileName(dbDirectory);
		if (p)
		{
			*p = 0;
		}
	}
	else
	{
		EA::StdC::GetCurrentProcessPath(dbPath);
		EA::StdC::GetCurrentProcessDirectory(dbDirectory);
	}

	/* Initialize an address rep cache object with the symbol database. */
	arc.SetAllocator(pCoreAllocator);

	EA::Callstack::AddressRepCache* const pARCSaved = EA::Callstack::SetAddressRepCache(&arc);

	// On Microsoft platforms we use the .pdb or .map file associated
	// with the executable. On GCC-based platforms, the executable is
	// an ELF file which has the debug information built into it.
	EA::IO::DirectoryIterator dir;
	EA::IO::DirectoryIterator::EntryList entryList;

	#if defined(EA_COMPILER_MSVC) && EACALLSTACK_MS_PDB_READER_ENABLED
	{   // Add .pdb files.
		dir.ReadRecursive(dbDirectory, entryList, EA_WCHAR("*.pdb"), EA::IO::kDirectoryEntryFile, true, true);

		for (EA::IO::DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
		{
			EA::IO::DirectoryIterator::Entry& entry  = *it;
			arc.AddDatabaseFile(entry.msName.c_str());
		}

		entryList.clear();
	}
	#elif !defined(EA_PLATFORM_APPLE) && !defined(EA_PLATFORM_NX) // We aren't able to read Apple DWARF info.
	{   // Add ELF (DWARF) files
		if	(EA::IO::File::Exists(dbPath))  // Add the ELF file. Not every compiler uses ELF+DWARF.
		{
			arc.AddDatabaseFile(dbPath);
		}
	}
	#endif

#if !defined(EA_PLATFORM_NX)
	{   // Add .map files.
		#if defined(EA_PLATFORM_APPLE)
			const wchar_t* kMapFileExtension = EA_WCHAR("*.txt");
		#else
			const wchar_t* kMapFileExtension = EA_WCHAR("*.map");
		#endif

		dir.ReadRecursive(dbDirectory, entryList, kMapFileExtension, EA::IO::kDirectoryEntryFile, true, true);

		for (EA::IO::DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
		{
			EA::IO::DirectoryIterator::Entry& entry  = *it;
			arc.AddDatabaseFile(entry.msName.c_str());
		}

		entryList.clear();
	}

	// Add source file directories for source code lookups.
	wchar_t sourceFileDir[EA::IO::kMaxPathLength];
	EA::StdC::Strlcpy(sourceFileDir, __FILE__, EA::IO::kMaxPathLength);
	*EA::IO::Path::GetFileName(sourceFileDir) = 0;

	/* Add the directory of this source file. */
	arc.AddSourceCodeDirectory(sourceFileDir);
	arc.AddSourceCodeDirectory(EA_WCHAR("C:\\"));       // For Win32
#endif

	EXPECT_TRUE(arc.GetDatabaseFileCount() > 0) << "Debug Symbol Files Unable To Be Loaded!";
	if (arc.GetDatabaseFileCount() == 0)
	{
		return -1;
	}

	*oldArc = pARCSaved;

	return 0;
}

/********************* TEST CASES ************************/

// TODO(mwinkler): DB lookup and deploying works on other platforms
#if defined(EA_PLATFORM_MICROSOFT)

TEST(EACallstackTest, TestCallstackCurrentContextWithLookup05)
{
	gCustomCoreAllocator.ResetCounters();

	EA::Allocator::ICoreAllocator* oldAllocator = EA::Callstack::GetAllocator();
	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	{
		EA::Callstack::AddressRepCache arc;
		EA::Callstack::AddressRepCache* oldArc;
		if (SetUpARC(&gCustomCoreAllocator, arc, &oldArc) < 0)
		{
			return;
		}

		CallstackEntryInfoArray expectedEntries;

		TestCallstack05(arc, expectedEntries);

		SetAddressRepCache(oldArc);
	}

	EXPECT_NE(gCustomCoreAllocator.mAllocationCount, 0);
	EXPECT_EQ(gCustomCoreAllocator.mAllocationCount, gCustomCoreAllocator.mFreeCount);
	EXPECT_FALSE(gCustomCoreAllocator.AnyLeaks());

	EA::Callstack::SetAllocator(oldAllocator);
}

TEST(EACallstackTest, TestCallstackCurrentContextWithLookup04)
{
	gCustomCoreAllocator.ResetCounters();

	EA::Allocator::ICoreAllocator* oldAllocator = EA::Callstack::GetAllocator();
	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	{
		EA::Callstack::AddressRepCache arc;
		EA::Callstack::AddressRepCache* oldArc;
		if (SetUpARC(&gCustomCoreAllocator, arc, &oldArc) < 0)
		{
			return;
		}

		CallstackEntryInfoArray expectedEntries;

		TestCallstack04(arc, expectedEntries);

		SetAddressRepCache(oldArc);
	}

	EXPECT_NE(gCustomCoreAllocator.mAllocationCount, 0);
	EXPECT_EQ(gCustomCoreAllocator.mAllocationCount, gCustomCoreAllocator.mFreeCount);
	EXPECT_FALSE(gCustomCoreAllocator.AnyLeaks());

	EA::Callstack::SetAllocator(oldAllocator);
}

TEST(EACallstackTest, TestCallstackCurrentContextWithLookup03)
{
	gCustomCoreAllocator.ResetCounters();

	EA::Allocator::ICoreAllocator* oldAllocator = EA::Callstack::GetAllocator();
	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	{
		EA::Callstack::AddressRepCache arc;
		EA::Callstack::AddressRepCache* oldArc;
		if (SetUpARC(&gCustomCoreAllocator, arc, &oldArc) < 0)
		{
			return;
		}

		CallstackEntryInfoArray expectedEntries;

		TestCallstack03(arc, expectedEntries);

		SetAddressRepCache(oldArc);
	}

	EXPECT_NE(gCustomCoreAllocator.mAllocationCount, 0);
	EXPECT_EQ(gCustomCoreAllocator.mAllocationCount, gCustomCoreAllocator.mFreeCount);
	EXPECT_FALSE(gCustomCoreAllocator.AnyLeaks());

	EA::Callstack::SetAllocator(oldAllocator);
}

TEST(EACallstackTest, TestCallstackCurrentContextWithLookup02)
{
	gCustomCoreAllocator.ResetCounters();

	EA::Allocator::ICoreAllocator* oldAllocator = EA::Callstack::GetAllocator();
	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	{
		EA::Callstack::AddressRepCache arc;
		EA::Callstack::AddressRepCache* oldArc;
		if (SetUpARC(&gCustomCoreAllocator, arc, &oldArc) < 0)
		{
			return;
		}

		CallstackEntryInfoArray expectedEntries;

		TestCallstack02(arc, expectedEntries);

		SetAddressRepCache(oldArc);
	}

	EXPECT_NE(gCustomCoreAllocator.mAllocationCount, 0);
	EXPECT_EQ(gCustomCoreAllocator.mAllocationCount, gCustomCoreAllocator.mFreeCount);
	EXPECT_FALSE(gCustomCoreAllocator.AnyLeaks());

	EA::Callstack::SetAllocator(oldAllocator);
}

TEST(EACallstackTest, TestCallstackCurrentContextWithLookup01)
{
	gCustomCoreAllocator.ResetCounters();

	EA::Allocator::ICoreAllocator* oldAllocator = EA::Callstack::GetAllocator();
	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	{
		EA::Callstack::AddressRepCache arc;
		EA::Callstack::AddressRepCache* oldArc;
		if (SetUpARC(&gCustomCoreAllocator, arc, &oldArc) < 0)
		{
			return;
		}

		CallstackEntryInfoArray expectedEntries;

		TestCallstack01(arc, expectedEntries);

		SetAddressRepCache(oldArc);
	}

	EXPECT_NE(gCustomCoreAllocator.mAllocationCount, 0);
	EXPECT_EQ(gCustomCoreAllocator.mAllocationCount, gCustomCoreAllocator.mFreeCount);
	EXPECT_FALSE(gCustomCoreAllocator.AnyLeaks());

	EA::Callstack::SetAllocator(oldAllocator);
}

#endif

TEST(EACallstackTest, TestGetCallstackCurrentContext)
{
	void* beforeIP;
	void* afterIP;

	{
		void* callstack[5];
		EA::Callstack::GetInstructionPointer(beforeIP);
		size_t ret = EA::Callstack::GetCallstack(callstack, 5, nullptr);
		EA::Callstack::GetInstructionPointer(afterIP);

		EXPECT_GE(ret, 1);

		EXPECT_GE(callstack[0], beforeIP);
		EXPECT_LE(callstack[0], afterIP);
	}
	{
		void* callstack[256];
		EA::Callstack::GetInstructionPointer(beforeIP);
		size_t ret = EA::Callstack::GetCallstack(callstack, 256, nullptr);
		EA::Callstack::GetInstructionPointer(afterIP);

		EXPECT_GE(ret, 1);

		EXPECT_GE(callstack[0], beforeIP);
		EXPECT_LE(callstack[0], afterIP);
	}
}

TEST(EACallstackTest, TestGetCallstackNULLTerminated)
{
	{
		void* callstack[1];
		size_t ret = EA::Callstack::GetCallstack(callstack, 1, nullptr);

		EXPECT_EQ(callstack[ret], nullptr);
	}
	{
		void* callstack[2];
		size_t ret = EA::Callstack::GetCallstack(callstack, 2, nullptr);

		EXPECT_EQ(callstack[ret], nullptr);
	}
	{
		void* callstack[5];
		size_t ret = EA::Callstack::GetCallstack(callstack, 5, nullptr);

		EXPECT_EQ(callstack[ret], nullptr);
	}
}

TEST(EACallstackTest, TestGetCallstackContext)
{
	EA::Callstack::CallstackContext context;
	bool result = EA::Callstack::GetCallstackContext(context, (intptr_t)EA::Thread::GetThreadId());

#if EA_CALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED

	EXPECT_TRUE(result);

#else

	EA_UNUSED(result);

#endif
}

EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();

#endif // EA_CALLSTACK_GETCALLSTACK_SUPPORTED
