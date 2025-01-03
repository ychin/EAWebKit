///////////////////////////////////////////////////////////////////////////////
// TestGeneralAllocator
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAAlignment.h>
#include <EASTL/map.h>
#include <EASTL/fixed_set.h>
#include <EASTL/vector.h>
#include <eathread/eathread_atomic.h>
#include <eathread/eathread_thread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace EA::Allocator;


enum Operation
{
	kOperationMalloc,
	kOperationCalloc,
	kOperationRealloc,
	kOperationMallocAligned,
	kOperationRealloc2,
	kOperationCount
};

const int gFill[kOperationCount] = { 0xe0, 0xe1, 0xe2, 0xe3, 0xe4 };


// Function declarations
size_t SubHeapCoreFreeFunction(GeneralAllocator* pGeneralAllocatorChild, void* pCore, size_t nSize, void* pContext);
size_t SimpleMallocFreeFunction(GeneralAllocator* pGeneralAllocatorChild, void* pCore, size_t nSize, void* pContext);
bool   MallocFailureFunction(GeneralAllocator* pGeneralAllocatorChild, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
bool   DescribeBlock(const GeneralAllocator::BlockInfo* pBlockInfo, void* pContext);

int    TestConstruction();
int    TestAsInterface();
int    TestDisableSystemAlloc();
int    TestValidateAddress();
int    TestInitNotification();
int    TestSubHeap();
int    TestAllocation();
int    Test64Bit();
int    TestLargeAllocations();
int    TestLargePages();
int    TestThreading();
int    TestAllocationFlags();
int    TestBestFit();
int    TestDebugNaming();
int    TestDebugGroupId();
int    TestReportHeap();
int    TestDelayedFree();
int    TestExtendCore();
int    TestDebugDataLocationSeparate();
int    TestAllocationGeneric(GeneralAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, 
							 size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount, EA::UnitTest::Rand& rand = gRand);
int    TestAllocationAligned(GeneralAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, 
							 size_t alignment, size_t alignmentOffset,
							 size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount, EA::UnitTest::Rand& rand = gRand);


#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

	#include <Windows.h>

	bool MemoryOverlapExists(const void* pChunk, size_t /*nChunkSize*/, void* pAddressArray[], size_t nAddressCount, uint32_t nPageSize)
	{
		for(size_t i = 0; i < nAddressCount; ++i)
		{
			if((pChunk >= pAddressArray[i] && ((char*)pChunk < (char*)pAddressArray[i] + nPageSize))) // If pChunk begins within this page...
				return true;

			// We shouldn't need this check.
			//if((((char*)pChunk + nChunkSize) >= pAddressArray[i] && (((char*)pChunk + nChunkSize) < (char*)pAddressArray[i] + nPageSize))) // If pChunk ends within this page...
			//    return true;
		}

		return false;
	}

	// This function uses the Windows GetWriteWatch in combination with MEM_WRITE_WATCH to help us
	// do a quick heap validation by calling ValidateChunk only on chunks within memory pages that
	// have been written to since the last time we called this function.
	// To consider: Put the following functionality into GeneralAllocator.
	bool ValidateHeapFast(GeneralAllocator* pAllocator, GeneralAllocator::HeapValidationLevel 
							heapValidationLevel = GeneralAllocator::kHeapValidationLevelBasic)
	{
		bool bReturnValue = true;

		GeneralAllocator::SnapshotImage reportBuffer;

		const void* const pContext = pAllocator->ReportBegin(NULL, GeneralAllocator::kBlockTypeAll, 
											false, &reportBuffer, sizeof(GeneralAllocator::SnapshotImage));

		if(pContext)
		{
			const size_t kAddressCapacity = ((256 * 1024 * 1024) / 4096); // Enough pointers to hold 256MB of 4K pages.
			void**       pAddressArray    = (void**)VirtualAlloc(NULL, kAddressCapacity * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ULONG_PTR    nAddressCount    = 0;
			ULONG        nPageSize        = 0;

			if(pAddressArray)
			{
				for(const GeneralAllocator::BlockInfo* pBlockInfo = pAllocator->ReportNext(pContext); 
					pBlockInfo && bReturnValue; pBlockInfo = pAllocator->ReportNext(pContext))
				{
					// If we are beginning to iterate a new core block (a new region of mapped memory), get the changed pages for it.
					if((pBlockInfo->mBlockType == GeneralAllocator::kBlockTypeCore) || pBlockInfo->mbMemoryMapped)
					{
						nAddressCount = kAddressCapacity;
						nPageSize     = 0;

						// GetWriteWatch is a Windows function which works on memory blocks VirtualAllocated with MEM_WRITE_WATCH.
						// We can use this to tell if the user wrote outside VirtualAllocated space and into the reserved space 
						// beyond it. This is not a general test for out-of-bounds writes, but it's a useful quick test for certain
						// kinds of overwrites. Here we use it to tell which blocks in our entire heap were written to and thus allow
						// us to call ValidateChunk on just the blocks in the space that was written to. This is faster than doing a
						// ValidateChunk call on every block in the entire heap.
						// The core block must have been allocated with the MEM_WRITE_WATCH (0x00200000) flag for GetWriteWatch to succeed.
						const UINT err = GetWriteWatch(WRITE_WATCH_FLAG_RESET, (void*)pBlockInfo->mpCore, pBlockInfo->mnBlockSize,
														pAddressArray, &nAddressCount, &nPageSize);
						if(err)
							nAddressCount = 0;
					}

					if(nAddressCount && (pBlockInfo->mBlockType != GeneralAllocator::kBlockTypeCore)) // If there are any changed pages in this core block and if this is a memory allocation and not a core block...
					{
						if(MemoryOverlapExists(pBlockInfo->mpBlock, pBlockInfo->mnBlockSize, pAddressArray, nAddressCount, nPageSize))
							bReturnValue = pAllocator->ValidateChunk(pBlockInfo->mpBlock, heapValidationLevel);
					}
				}

				VirtualFree(pAddressArray, 0, MEM_RELEASE);
			}

			pAllocator->ReportEnd(pContext);
		}

		return bReturnValue;
	}

#else

#if defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
	#include <Windows.h>

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
		#include <xmem.h>
	#endif
#endif

	bool ValidateHeapFast(GeneralAllocator*, GeneralAllocator::HeapValidationLevel)
	{
		return true;
	}

#endif


///////////////////////////////////////////////////////////////////////////////
// GetLargestFreeBlockManual
//
// Manually walks the heap looking for the largest free block.
//
static size_t GetLargestFreeBlockManual(EA::Allocator::GeneralAllocator* pAllocator)
{
	using namespace EA::Allocator;

	GeneralAllocator::SnapshotImage snapshot;
	size_t                          nLargestBlock = 0;
	const void* const               pContext = pAllocator->ReportBegin(NULL, GeneralAllocator::kBlockTypeFree, false, &snapshot, sizeof(snapshot));

	for(const GeneralAllocator::BlockInfo* pBlockInfo = pAllocator->ReportNext(pContext); 
		pBlockInfo; 
		pBlockInfo = pAllocator->ReportNext(pContext))
	{
		// ASSERT(pBlockInfo->mBlockType == GeneralAllocator::kBlockTypeFree, "Unexpected block type.");

		if(pBlockInfo->mnBlockSize > nLargestBlock)
			nLargestBlock = pBlockInfo->mnBlockSize;
	}

	pAllocator->ReportEnd(pContext);

	return nLargestBlock;
}


///////////////////////////////////////////////////////////////////////////////
// TestAllocationGeneric
//
// Generic testing of an allocator.
//
int TestAllocationGeneric(GeneralAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, 
						   size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount, EA::UnitTest::Rand& rand)
{
	size_t i;

	int nErrorCount = 0;

	EATEST_VERIFY(nAllocSizeMin < nAllocSizeMax);

	void** pDataArray = new void*[nAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * nAllocSlotCount);

	int reallocCount = 0;

	// Run tests
	for(size_t c(0); c < nAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < nAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)nAllocIterationCount, (unsigned)c, (unsigned)nAllocCycleCount);

			i = rand.RandRange(0, (int)nAllocSlotCount);

			const bool   bRealloc = (rand.RandRange(0, 10) == 0);
			const size_t n        = rand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);

			if(bRealloc)
			{
				if(pDataArray[i])
					reallocCount++;
				pDataArray[i] = pAllocator->Realloc(pDataArray[i], n, 0); // To consider: modify the allocation flags.
				EATEST_VERIFY(pDataArray[i] != nullptr);
			}
			else if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				pDataArray[i] = pAllocator->Malloc(n);
				EATEST_VERIFY(pDataArray[i] != nullptr);
				// Verify the ptr exists, even though we test above, if we get a null we want the test 
				// to fail (instead of crash) so we can continue to run other tests
				if(pDataArray[i])
					memset(pDataArray[i], gFill[kOperationMalloc], n);
			}

			EATEST_VERIFY(ValidateHeapFast(pAllocator, GeneralAllocator::kHeapValidationLevelBasic));
		}

		for(i = 0; i < nAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}

	delete[] pDataArray;

	return nErrorCount;
}
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// TestAllocationAligned
//
int TestAllocationAligned(GeneralAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, 
						   size_t alignment, size_t alignmentOffset,
						   size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount, EA::UnitTest::Rand& rand)
{
	size_t i;

	int nErrorCount = 0;

	EATEST_VERIFY(nAllocSizeMin < nAllocSizeMax);

	void** pDataArray = new void*[nAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * nAllocSlotCount);

	// Run tests
	for(size_t c(0); c < nAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < nAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)nAllocIterationCount, (unsigned)c, (unsigned)nAllocCycleCount);

			i = rand.RandRange(0, (int)nAllocSlotCount);
			const size_t n = rand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);

			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				pDataArray[i] = pAllocator->MallocAligned(n, alignment, alignmentOffset);
				memset(pDataArray[i], gFill[kOperationMallocAligned], n);
			}

			// EATEST_VERIFY(ValidateHeapFast(pAllocator, GeneralAllocator::kHeapValidationLevelBasic));
		}

		for(i = 0; i < nAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}

	delete[] pDataArray;

	return nErrorCount;
}
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// TestShared
//
int TestShared()
{
	EA::UnitTest::Report("    TestShared\n");

	int nErrorCount = 0;

	{
		// #define PPM_MIN(a, b) ((a) < (b) ? (a) : (b))
		// #define PPM_MAX(a, b) ((a) > (b) ? (a) : (b))

		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		for(int i = 0; i < 100000; i++)
		{ 
			uint32_t n0   = gRand.RandValue();
			uint32_t n1   = gRand.RandValue();

			uint32_t nMin = PPM_MIN(n0, n1);
			EATEST_VERIFY(nMin == n0 || nMin == n1);
			EATEST_VERIFY(nMin  < n0 || nMin == n0);

			uint32_t nMax = PPM_MAX(n0, n1);
			EATEST_VERIFY(nMax == n0 || nMax == n1);
			EATEST_VERIFY(nMax  > n0 || nMax == n0);
		}
	}

	{
		// unsigned GetPPMallocDebugTime();
		// uint64_t GetPPMallocDebugTimeUs();

		// We verify merely that time moves foward.
		unsigned nMS = GetPPMallocDebugTime();
		uint64_t nUS = GetPPMallocDebugTimeUs();

		for(int i = 0; i < 100; i++)
		{
			unsigned nMSNow = GetPPMallocDebugTime();
			EATEST_VERIFY((nMSNow >= nMS) || ((signed)nMS < 0 && (signed)nMSNow >= 0));

			uint64_t nUSNow = GetPPMallocDebugTimeUs();
			EATEST_VERIFY((nUSNow >= nUS) || ((int64_t)nUS < 0 && (int64_t)nUSNow >= 0));
		}
	}


	{
		// bool MemoryFillCheck(const unsigned char* pData, size_t nDataSize, unsigned char c);

		const size_t kSize = 32768;
		uint8_t* const pMemory = new uint8_t[kSize];
		bool bResult;

		memset(pMemory, 0xd3, kSize);
		bResult = MemoryFillCheck(pMemory, 0, 0xd3);
		EATEST_VERIFY(bResult);
		bResult = MemoryFillCheck(pMemory, 1, 0xd3);
		EATEST_VERIFY(bResult);
		bResult = MemoryFillCheck(pMemory, kSize, 0xd3);
		EATEST_VERIFY(bResult);

		pMemory[0] = 0xd2;
		bResult = MemoryFillCheck(pMemory, 0, 0xd3);
		EATEST_VERIFY(bResult);
		bResult = MemoryFillCheck(pMemory, 1, 0xd3);
		EATEST_VERIFY(!bResult);
		bResult = MemoryFillCheck(pMemory, kSize, 0xd3);
		EATEST_VERIFY(!bResult);

		pMemory[0] = 0xd3;
		pMemory[kSize - 1] = 0xd2;
		bResult = MemoryFillCheck(pMemory, 0, 0xd3);
		EATEST_VERIFY(bResult);
		bResult = MemoryFillCheck(pMemory, 1, 0xd3);
		EATEST_VERIFY(bResult);
		bResult = MemoryFillCheck(pMemory, kSize, 0xd3);
		EATEST_VERIFY(!bResult);

		delete[] pMemory;
	}


	{
		// DataPreviewType GetDataPreview(const void* pData, size_t nDataSize, char* pBuffer, wchar_t* pBufferW, size_t nBufferLength);
		// To do.
	}

	{
		// void TraceFunctionDefault(const char* pTraceText, void* pContext);
		// To do.
	}

	{
		// unsigned GetPageSize();

		const unsigned nPageSize = EA::Allocator::GetPageSize();
		EATEST_VERIFY(nPageSize >= 512);

		const bool bIsPowerOf2 = PPM_IsPowerOf2(nPageSize);
		EATEST_VERIFY(bIsPowerOf2);
	}

	{
		// uint32_t PPM_Log2(uint32_t x);
		// uint64_t PPM_Log2(uint64_t x);
		// bool     PPM_IsPowerOf2(size_t nValue);

		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		for(uint32_t i = 0; i < 100000; i++)
		{ 
			uint32_t n     = gRand.RandValue();
			uint32_t nLog2 = PPM_Log2(n);
			uint32_t nPow2 = (uint32_t)(1 << nLog2);

			EATEST_VERIFY((nPow2 <= n) && (nPow2 > (n / 2)));
		}

		for(uint32_t i = 0; i < 100000; i++)
		{ 
			uint64_t n     = ((uint64_t)gRand.RandValue() << 32) | gRand.RandValue();
			uint64_t nLog2 = PPM_Log2(n);
			uint64_t nPow2 = ((uint64_t)1 << nLog2);

			EATEST_VERIFY((nPow2 <= n) && (nPow2 > (n / 2)));
		}

		for(size_t j = 1; j < (1 << 20); j++)
		{
			const bool b = PPM_IsPowerOf2(j);

			switch (j)
			{
				case     1: case     2: case      4: case      8: case     16:
				case    32: case    64: case    128: case    256: case    512:
				case  1024: case  2048: case   4096: case   8192: case  16384:
				case 32768: case 65536: case 131072: case 262144: case 524288:
					EATEST_VERIFY(b);
					break;

				default:
					EATEST_VERIFY(!b);
					break;
			}
		}
	}

	{
		// bool   PPM_IsAligned(const void* p, size_t nAlignment);
		// bool   PPM_IsAligned(size_t nValue, size_t nAlignment);
		// char*  PPM_AlignUp(const void* p, size_t nAlignment);
		// char*  PPM_AlignUp(const char* p, size_t nAlignment);
		// size_t PPM_AlignUp(size_t nValue, size_t nAlignment);

		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		for(uint32_t i = 0; i < 100000; i++)
		{ 
			uint64_t    n = (uint64_t)gRand.RandValue() | ((uint64_t)gRand.RandValue() << 32);
			const char* p = (const char*)(uintptr_t)n;
			bool        bIsAligned;
			char*       pUp;

			for(size_t a = 8; a <= 1024; a *= 2)
			{
				bIsAligned = PPM_IsAligned(p, a);
				EATEST_VERIFY(bIsAligned == ((n % a) == 0));

				pUp = PPM_AlignUp(p, a);
				EATEST_VERIFY((pUp >= p) && ((pUp < (p + a))) && PPM_IsAligned(pUp, a));
			}
		}
	}

#if PPM_DEBUG_CALLSTACK_AVAILABLE
	{
		// void   InitCallstack();
		// void   ShutdownCallstack();
		// size_t GetCallStack(void* pReturnAddressArray[], size_t nReturnAddressArrayLength);
		// size_t DescribeCallStack(const void* pReturnAddressArray[], size_t nReturnAddressArrayLength, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase* addressRepLookup=nullptr);

		const size_t kAddressCount = 32;
		void*        pReturnAddressArray[kAddressCount];
		size_t       n;

		InitCallstack();

		// Verify that the limit of '1' is honored.
		memset(pReturnAddressArray, 0x11, sizeof(pReturnAddressArray));
		n = GetCallStack(pReturnAddressArray, 1);
		EATEST_VERIFY(n <= 1); // Zero is possible on non-supported platforms.
		EATEST_VERIFY((uint32_t)(uintptr_t)pReturnAddressArray[1] == 0x11111111);

		// This time try to read the entire call stack.
		n = GetCallStack(pReturnAddressArray, kAddressCount);
		EATEST_VERIFY(n <= kAddressCount); // Zero is possible on non-supported platforms.

		if(n > 0)
		{
			const size_t kBufferSize = 256;
			char buffer[kBufferSize];

			// Verify that the limit of '1' is honored.
			memset(buffer, 0x11, sizeof(buffer));
			n = DescribeCallStack((const void**)pReturnAddressArray, n, buffer, 1);
			EATEST_VERIFY(n <= 1);
			EATEST_VERIFY(buffer[1] == 0x11);

			n = DescribeCallStack((const void**)pReturnAddressArray, n, buffer, kBufferSize);
			EATEST_VERIFY(n <= kBufferSize);
		}

		ShutdownCallstack();
	}
#endif

	return nErrorCount;
}

static bool sSimpleMallocFreeFunctionCalled = false;

///////////////////////////////////////////////////////////////////////////////
// TestConstruction
//
int TestConstruction()
{
	EA::UnitTest::Report("    TestConstruction\n");

	int nErrorCount = 0;

	const size_t kCoreSize = (1 * 1024 * 1024);
	const size_t kCoreSizeLarge = kCoreSize*8;

	{
		// Test default construction.
		GeneralAllocatorDebug generalAllocator; // Intentionally use our own values below as opposed to global values.
		nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 1000, gnAllocSlotCount / 4, gnAllocIterationCount / 4, gnAllocCycleCount); // Max concurrent core use should be ~1,250,000 bytes
		generalAllocator.Shutdown();
	}

	{
		// Test construction with core.
		void* pCore = malloc(kCoreSize);
		GeneralAllocatorDebug generalAllocator(pCore, kCoreSize, false); // Intentionally use our own values below as opposed to global values.
		nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 1000, gnAllocSlotCount / 4, gnAllocIterationCount / 4, gnAllocCycleCount); // Max concurrent core use should be ~1,250,000 bytes
		generalAllocator.Shutdown();
		free(pCore);
	}

	{
		// Test minimal size construction with core.
		void* pCore        = malloc(4096 + 16);  //Add some so that we can align to 16\n" byte bounds.
		void* pCoreAligned = PPM_AlignUp(pCore, 16);
		GeneralAllocatorDebug generalAllocator(pCoreAligned, 4096, false);
		nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 20, 100, 100, 1);
		generalAllocator.Shutdown();
		free(pCore);
	}

	{
		// Test Init, use, Shutdown, Init, use, Shutdown, etc.
		const size_t          kISISCoreSize = (EA::UnitTest::GetSystemMemoryMB() >= 2048) ? (256 * 1024 * 1024) : (4 * 1024 * 1024);
		GeneralAllocatorDebug generalAllocator;

		generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)kISISCoreSize);

		for(int i = 0; i < 10; i++)
		{
			bool bResult = generalAllocator.Init();
			EATEST_VERIFY(bResult);

			// We pick the sizes given here because they should match the core block size about right.
			nErrorCount += TestAllocationGeneric(&generalAllocator, 0 * (kISISCoreSize / kCoreSize), 1000 * (kISISCoreSize / kCoreSize), gnAllocSlotCount / 4, gnAllocIterationCount / 4, gnAllocCycleCount);

			bResult = generalAllocator.Shutdown();
			EATEST_VERIFY(bResult);
		}
	}
	
	{
		// Test construction with core added via Init.
		void* pCore = malloc(kCoreSize);
		GeneralAllocatorDebug generalAllocator;
		generalAllocator.Init(pCore, kCoreSize, false); // Intentionally use our own values below as opposed to global values.
		nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 1000, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount); // Max concurrent core use should be ~1,250,000 bytes
		generalAllocator.Shutdown();
		free(pCore);
	}

	{
		// Test construction with core added via AddCore.
		void* pCore1 = malloc(kCoreSize);
		void* pCore2 = malloc(kCoreSize);
		GeneralAllocatorDebug generalAllocator;
		generalAllocator.AddCore(pCore1, kCoreSize, false);
		generalAllocator.AddCore(pCore2, kCoreSize, false); // Intentionally use our own values below as opposed to global values.
		nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 1000, gnAllocSlotCount, gnAllocIterationCount * 2, gnAllocCycleCount * 2);
		generalAllocator.Shutdown();
		free(pCore1);
		free(pCore2);
	}

	{
		// Test core freeing with NON virtual alloc memory  ShouldFreeCore = true
		void* pCore1 = malloc(kCoreSizeLarge);
		const bool bShouldFreeCore = true;
		GeneralAllocatorDebug generalAllocator;
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0); 
		generalAllocator.AddCore(pCore1, kCoreSizeLarge, bShouldFreeCore, true, SimpleMallocFreeFunction, NULL);
		sSimpleMallocFreeFunctionCalled = false;
		void* pCore2 = generalAllocator.Malloc(kCoreSize);
		void* pCore3 = generalAllocator.Malloc(kCoreSize);
		generalAllocator.Free(pCore2);
		generalAllocator.Free(pCore3); // core free should happen here
		void* pCore4ShouldBeNull = generalAllocator.Malloc(kCoreSize);
		EATEST_VERIFY_MSG( pCore4ShouldBeNull == NULL, "We should have got null because the only core memory we gave the allocator should have been freed.");
		EATEST_VERIFY_MSG(sSimpleMallocFreeFunctionCalled, "Expecting our free function to be called.");
		sSimpleMallocFreeFunctionCalled = false;
		generalAllocator.Shutdown();
		EATEST_VERIFY_MSG(!sSimpleMallocFreeFunctionCalled, "Shutdown should not have called the free function.");
	}


	{
		// Test core freeing NON virtual alloc memory  ShouldFreeCore = false
		void* pCore1 = malloc(kCoreSizeLarge);
		GeneralAllocatorDebug generalAllocator;
		const bool bShouldFreeCore = false;
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0); 
		generalAllocator.AddCore(pCore1, kCoreSizeLarge, bShouldFreeCore, true, SimpleMallocFreeFunction, NULL);
		sSimpleMallocFreeFunctionCalled = false;
		void* pCore2 = generalAllocator.Malloc(kCoreSize);
		void* pCore3 = generalAllocator.Malloc(kCoreSize);
		generalAllocator.Free(pCore2);
		generalAllocator.Free(pCore3); 
		EATEST_VERIFY_MSG(!sSimpleMallocFreeFunctionCalled, "Expecting our free function to be not called.");
		void* pCore4ShouldNotBeNull = generalAllocator.Malloc(kCoreSize);
		EATEST_VERIFY_MSG(pCore4ShouldNotBeNull != NULL, "We should still have core memory and therefore the pointer should not be null.");
		generalAllocator.Free(pCore4ShouldNotBeNull); 
		generalAllocator.Shutdown();
		EATEST_VERIFY_MSG(!sSimpleMallocFreeFunctionCalled, "Shutdown should not have called the free function.");
		free(pCore1);
	}

	// testing Virtual Allocate
	#if defined(EA_PLATFORM_WINDOWS) && !PPM_MALLOC_AS_COREALLOC
	{
		// Test using virtual allocated memory as core with kCoreTypeMMap setting. Test with ShouldFreeCore=true
		void* pCore1 =  VirtualAlloc(0,kCoreSizeLarge,MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		const bool bShouldFreeCore = true;
		GeneralAllocatorDebug generalAllocator;
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0);
		generalAllocator.AddCoreEx(pCore1, kCoreSizeLarge, GeneralAllocator::kCoreTypeMMap, 0, bShouldFreeCore, true, NULL,NULL,NULL,NULL);
		void* pCore2 = generalAllocator.Malloc(kCoreSize);
		void* pCore3 = generalAllocator.Malloc(kCoreSize);
		generalAllocator.Free(pCore2);
		generalAllocator.Free(pCore3); // core free should happen here
		void* pCore4ShouldBeNull = generalAllocator.Malloc(kCoreSize);
		EATEST_VERIFY_MSG( pCore4ShouldBeNull == NULL, "We should have got null because the only core memory we gave the allocator should have been freed.");
		generalAllocator.Shutdown();
	}

	{
		// Test using virtual allocated memory as core with kCoreTypeMMap setting. Test with ShouldFreeCore = false
		void* pCore1 =  VirtualAlloc(0, kCoreSizeLarge, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		const bool bShouldFreeCore = false;
		GeneralAllocatorDebug generalAllocator;
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0);
		generalAllocator.AddCoreEx(pCore1, kCoreSizeLarge, GeneralAllocator::kCoreTypeMMap, 0, bShouldFreeCore, false, NULL,NULL,NULL,NULL);
		void* pCore2 = generalAllocator.Malloc(kCoreSize);
		void* pCore3 = generalAllocator.Malloc(kCoreSize);
		generalAllocator.Free(pCore2);
		generalAllocator.Free(pCore3); 
		void* pCore4ShouldNotBeNull = generalAllocator.Malloc(kCoreSize);
		EATEST_VERIFY_MSG(pCore4ShouldNotBeNull != NULL, "We should still have core memory and therefore the pointer should not be null.");
		generalAllocator.Shutdown();
		VirtualFree(pCore1, kCoreSizeLarge, MEM_DECOMMIT | MEM_RELEASE);
	}

	{

		// Trim Test
		// Test using virtual allocated memory as core with kCoreTypeMMap setting. Test with ShouldFreeCore = false and mbShouldTrim = true
		void* pCore1 =  VirtualAlloc(0, kCoreSizeLarge, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		GeneralAllocatorDebug generalAllocator;
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0);
		const bool bShouldFreeCore = true;
		const bool bShouldTrimCore = true;
		generalAllocator.AddCoreEx(pCore1, kCoreSizeLarge, GeneralAllocator::kCoreTypeMMap, 0, bShouldFreeCore, bShouldTrimCore, NULL,NULL,NULL,NULL);
		void* pCore2 = generalAllocator.Malloc(kCoreSize);
		void* pCore3 = generalAllocator.Malloc(kCoreSize);
		generalAllocator.Free(pCore2);
		size_t amountTrimmed  = generalAllocator.TrimCore(0);
		EATEST_VERIFY_MSG(amountTrimmed > kCoreSize  , "We are expecting to have trimmed the core.");

		void* pCore4ShouldBeNull = generalAllocator.Malloc(kCoreSizeLarge - amountTrimmed);
		EATEST_VERIFY_MSG(pCore4ShouldBeNull == NULL, "We should have got null because the trimming of the core there will not be enough memory remaining.");

		generalAllocator.Free(pCore3);
		pCore4ShouldBeNull = generalAllocator.Malloc(kCoreSize);
		EATEST_VERIFY_MSG(pCore4ShouldBeNull == NULL, "We should have got null because the only core memory we gave the allocator should have been freed.");
		generalAllocator.Shutdown();
	}
	#endif

	{
		// Test small initial core specified by the constructor.
		const size_t kBlockSize = 65536; // This should be a value significantly smaller than PPM_NEW_CORE_SIZE_DEFAULT, because we are testing that the actual core block is not rounded up to PPM_NEW_CORE_SIZE_DEFAULT.
		GeneralAllocatorDebug generalAllocator(NULL, kBlockSize);

		// Verify that there is exactly one core block of kBlockSize bytes.
		const void* pReport = generalAllocator.ReportBegin(NULL, GeneralAllocator::kBlockTypeCore);
		EATEST_VERIFY(pReport != NULL);

		if(pReport)
		{
			const GeneralAllocator::BlockInfo* pBlockInfo = generalAllocator.ReportNext(pReport, GeneralAllocator::kBlockTypeCore);
			EATEST_VERIFY(pBlockInfo != NULL);

			if(pBlockInfo)
			{
				EATEST_VERIFY_F(pBlockInfo->mnBlockSize == kBlockSize, "    Unexpected block size. Expected: %u, actual: %u", (unsigned)kBlockSize, (unsigned)pBlockInfo->mnBlockSize);

				pBlockInfo = generalAllocator.ReportNext(pReport, GeneralAllocator::kBlockTypeCore);
				EATEST_VERIFY((pBlockInfo == NULL) || (pBlockInfo->mpCore == NULL)); // Verify there are no more allocated core blocks.
			}

			generalAllocator.ReportEnd(pReport);
		}
	}

	return nErrorCount;
}
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// TestAsInterface
//
int TestAsInterface()
{
	EA::UnitTest::Report("    TestAsInterface\n");

	int nErrorCount = 0;

	GeneralAllocator       ga;
	GeneralAllocatorDebug  gad;
	GeneralAllocator*      pGA;
	GeneralAllocatorDebug* pGAD;

	pGA = (GeneralAllocator*)ga.AsInterface(GeneralAllocator::kTypeId);
	EATEST_VERIFY_MSG(pGA == &ga, "TestAsInterface failure.\n");

	pGAD = (GeneralAllocatorDebug*)ga.AsInterface(GeneralAllocatorDebug::kTypeId);
	EATEST_VERIFY_MSG(pGAD == NULL, "TestAsInterface failure.\n");

	pGA = (GeneralAllocator*)gad.AsInterface(GeneralAllocator::kTypeId);
	EATEST_VERIFY_MSG(pGA == &gad, "TestAsInterface failure.\n");

	pGAD = (GeneralAllocatorDebug*)gad.AsInterface(GeneralAllocatorDebug::kTypeId);
	EATEST_VERIFY_MSG(pGAD == &gad, "TestAsInterface failure.\n");

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TDSAMallocFailureFunction
//
struct TDSData
{
	TDSData() : 
		mMallocFailureCount(0),
		mMinExpectedAllocatedVolume(0),
		mAllocatedVolume(0),
		mNewCoreSize(0),
		mbUnexpectedMallocFailure(false)
	{ }

	size_t mMallocFailureCount;
	size_t mMinExpectedAllocatedVolume;
	size_t mAllocatedVolume;
	size_t mNewCoreSize;
	bool   mbUnexpectedMallocFailure;
};


bool TDSAMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, 
							   size_t nAllocatorRequestedSize, void* pContext)
{
	TDSData* pTDSData = (TDSData*)pContext;

	pTDSData->mMallocFailureCount++;
	
	// Add 32K to account for various overhead. If this fails at some point in the future, 
	// this 32K value may need to be increased. 
	//if((pTDSData->mAllocatedVolume + nMallocRequestedSize + 32768) < pTDSData->mMinExpectedAllocatedVolume)
	//    pTDSData->mbUnexpectedMallocFailure = true;

	return pGeneralAllocator->AddCore(0, pTDSData->mNewCoreSize);
}


///////////////////////////////////////////////////////////////////////////////
// TestDisableSystemAlloc
//
int TestDisableSystemAlloc()
{
	EA::UnitTest::Report("    TestDisableSystemAlloc\n");

	int nErrorCount = 0;

  //const float    systemSpeed  = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeCPU);
	const uint64_t systemMemory = EA::UnitTest::GetSystemMemoryMB();
	const size_t   fCount       = (systemMemory < 256) ? 2 : 4;

	for(size_t f = 0; f < fCount; f++)
	{
		GeneralAllocator ga;
		TDSData          tdsData;

		const size_t kCoreBlockSize              = (systemMemory < 256) ? (2 * 1024 * 1024) : (16 * 1024 * 1024);
		const size_t kExpectedMallocFailureCount = f;
		const size_t kAllocVolumeSize            = ((kExpectedMallocFailureCount + 1) * kCoreBlockSize) - (kCoreBlockSize * 20 / 100);

		ga.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0);
		ga.SetOption(GeneralAllocator::kOptionNewCoreSize,       (int)kCoreBlockSize);
		ga.SetOption(GeneralAllocator::kOptionCoreIncrementSize, (int)kCoreBlockSize);
		ga.SetMallocFailureFunction(TDSAMallocFailureFunction, &tdsData);
		ga.AddCore(0, kCoreBlockSize);

		tdsData.mMinExpectedAllocatedVolume = kCoreBlockSize;
		tdsData.mNewCoreSize = kCoreBlockSize;

		gRand.SetSeed(EA::UnitTest::GetRandSeed());
		
		while(tdsData.mAllocatedVolume < kAllocVolumeSize)
		{
			const size_t n = (size_t)(uint32_t)gRand.RandRange((int)kCoreBlockSize / 32, (int)kCoreBlockSize / 16);
			void* const  p = ga.Malloc(n);

			EATEST_VERIFY_MSG(p != NULL, "TestDisableSystemAlloc failure.\n");

			tdsData.mAllocatedVolume += n;
		}

		EATEST_VERIFY_F(tdsData.mbUnexpectedMallocFailure == false, "TestDisableSystemAlloc failure (f = %u).\n", (unsigned)f);
		EATEST_VERIFY_F(tdsData.mMallocFailureCount == kExpectedMallocFailureCount, "TestDisableSystemAlloc failure (f = %u, expected = %u).\n", (unsigned)tdsData.mMallocFailureCount, (unsigned)f);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestValidateAddress
//
int TestValidateAddress()
{
	EA::UnitTest::Report("    TestValidateAddress\n");

	int nErrorCount = 0;

	GeneralAllocator       ga;
	GeneralAllocatorDebug  gad;
	void*                  pAlloc;

	// Try four combinations: 
	//     GeneralAllocator    / GeneralAllocatorDebug
	//     kAllocationFlagNone / kAllocationFlagMMap
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			GeneralAllocator* const pGA = (i == 0) ? &ga : &gad;

			if(j == 0)
				pAlloc = pGA->Malloc(100, GeneralAllocator::kAllocationFlagNone);
			else
				pAlloc = pGA->Malloc(100, GeneralAllocator::kAllocationFlagMMap);

			// Test the allocated pointer itself.
			const void* pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeSpecific);
			EATEST_VERIFY_MSG(pTest == pAlloc, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeAllocated);
			EATEST_VERIFY_MSG(pTest == pAlloc, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeOwned);
			EATEST_VERIFY_MSG(pTest == pAlloc, "ValidateAddress failure.\n");

			// Test the allocated pointer + n, which is still valid, but isn't the allocated ptr itself.
			pTest = pGA->ValidateAddress((char*)pAlloc + 40, GeneralAllocator::kAddressTypeSpecific);
			EATEST_VERIFY_MSG(pTest == NULL, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress((char*)pAlloc + 40, GeneralAllocator::kAddressTypeAllocated);
			EATEST_VERIFY_MSG(pTest == pAlloc, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress((char*)pAlloc + 40, GeneralAllocator::kAddressTypeOwned);
			EATEST_VERIFY_MSG(pTest == (char*)pAlloc + 40, "ValidateAddress failure.\n");

			// Test the allocated pointer - n, which is not valid.
			pTest = pGA->ValidateAddress((char*)pAlloc - 4, GeneralAllocator::kAddressTypeSpecific);
			EATEST_VERIFY_MSG(pTest == NULL, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress((char*)pAlloc - 4, GeneralAllocator::kAddressTypeAllocated);
			EATEST_VERIFY_MSG(pTest == NULL, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress((char*)pAlloc - 4, GeneralAllocator::kAddressTypeOwned);
			#if PPM_MMAP_SUPPORTED
				EATEST_VERIFY_MSG(pTest == ((j == 0) ? ((char*)pAlloc - 4) : (const void*)NULL), "ValidateAddress failure.\n");
			#endif

			pGA->Free(pAlloc);


			// Test the freed pointer.
			pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeSpecific);
			EATEST_VERIFY_MSG(pTest == NULL, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeAllocated);
			EATEST_VERIFY_MSG(pTest == NULL, "ValidateAddress failure.\n");

			pTest = pGA->ValidateAddress(pAlloc, GeneralAllocator::kAddressTypeOwned);
			#if PPM_MMAP_SUPPORTED
				EATEST_VERIFY_MSG(pTest == ((j == 0) ? pAlloc : (const void*)NULL), "ValidateAddress failure.\n");
			#endif
		}
	}

	return nErrorCount;
}




///////////////////////////////////////////////////////////////////////////////
// Listener
//
// Class used for TestInitNotification.
//
struct Listener
{
	GeneralAllocator::InitCallbackNode mICN;
	GeneralAllocator* mGeneralAllocatorArray[16];
	int mnArraySize;

	Listener()
	{
		mICN.mpInitCallbackFunction = NULL;
		mICN.mpContext = this;
		mnArraySize = 0;
	}

	~Listener()
	{
		Shutdown();
	}

	void Init()
	{
		if(!mICN.mpInitCallbackFunction)
		{
			mICN.mpInitCallbackFunction = &Callback;
			GeneralAllocator::SetInitCallback(&mICN, true);
		}
	}

	void Shutdown()
	{
		if(mICN.mpInitCallbackFunction)
		{
			GeneralAllocator::SetInitCallback(&mICN, false);
			mICN.mpInitCallbackFunction = NULL;
		}
	}

	static void Callback(GeneralAllocator* /*pGeneralAllocator*/, bool bInit, void* pContext)
	{
		Listener* pListener = (Listener*)pContext;
		if(bInit)
			pListener->mnArraySize++;
		else
			pListener->mnArraySize--;
		// To do: Maintain the mGeneralAllocatorArray list here.
	}
};


///////////////////////////////////////////////////////////////////////////////
// TestInitNotification
//
int TestInitNotification()
{
	EA::UnitTest::Report("    TestInitNotification\n");

	int nErrorCount = 0;

	Listener listenerArray[8];
	int      i;

	for(i = 0; i < 4; i++)
		listenerArray[i].Init();

	for(i = 0; i < 4; i++)
		listenerArray[i].Shutdown();

	for(i = 0; i < 4; i++)
		listenerArray[i].Init();

	GeneralAllocator       ga1;
	GeneralAllocatorDebug  gad1;

	for(i = 4; i < 8; i++)
		listenerArray[i].Init();

	GeneralAllocator       ga2;
	GeneralAllocatorDebug  gad2;

	for(i = 4; i < 8; i++)
		listenerArray[i].Shutdown();

	GeneralAllocator       ga3;
	GeneralAllocatorDebug  gad3;

	// Let the allocators and listeners shut themselves down.

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestSubHeap
//
// Test the ability to create a GeneralAllocator heap that uses a slice from 
// a larger GeneralAllocator as its source of core.
//
const size_t kSubHeapCoreSize = 100000;

size_t SubHeapCoreFreeFunction(GeneralAllocator* /*pGeneralAllocatorChild*/, void* pCore, size_t nSize, void* pContext)
{
	GeneralAllocator* const pGeneralAllocatorParent = (GeneralAllocator*)pContext;

	pGeneralAllocatorParent->Free(pCore);

	return nSize;
}



size_t SimpleMallocFreeFunction(GeneralAllocator* /*pGeneralAllocatorChild*/, void* pCore, size_t nSize, void* pContext)
{
	sSimpleMallocFreeFunctionCalled = true;
	free(pCore);
	return nSize;
}

bool MallocFailureFunction(GeneralAllocator* pGeneralAllocatorChild, size_t /*nMallocRequestedSize*/, size_t nAllocatorRequestedSize, void* pContext)
{
	GeneralAllocator* const pGeneralAllocatorParent = (GeneralAllocator*)pContext;

	if(nAllocatorRequestedSize < kSubHeapCoreSize) // Allocate at least kSubHeapCoreSize bytes for new core.
		nAllocatorRequestedSize = kSubHeapCoreSize;

	void* const pNewCore = pGeneralAllocatorParent->Malloc(nAllocatorRequestedSize);

	if(pNewCore)
		pGeneralAllocatorChild->AddCore(pNewCore, nAllocatorRequestedSize, true, false, SubHeapCoreFreeFunction, pGeneralAllocatorParent);

	return (pNewCore != NULL);
}

int TestSubHeap()
{
	EA::UnitTest::Report("    TestSubHeap\n");

	int nErrorCount = 0;

	// Create a main heap.
	GeneralAllocator generalAllocatorParent;
	generalAllocatorParent.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	generalAllocatorParent.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);

	// Create a sub heap from the main heap.
	GeneralAllocator generalAllocatorChild(generalAllocatorParent.Malloc(kSubHeapCoreSize), kSubHeapCoreSize, false, false, SubHeapCoreFreeFunction, &generalAllocatorParent);
	generalAllocatorChild.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	generalAllocatorChild.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
	generalAllocatorChild.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0); // Disable system allocation, so that all allocations go to our MallocFailureFunction.
	generalAllocatorChild.SetMallocFailureFunction(MallocFailureFunction, &generalAllocatorParent);

	// Test the allocator in such a way as to trigger a request for more core by the sub allocator.
	gRand.SetSeed(EA::UnitTest::GetRandSeed()); // Intentionally use our own values below as opposed to global values.
	nErrorCount += TestAllocationGeneric(&generalAllocatorChild, 0, 1000, gnAllocSlotCount / 4, gnAllocIterationCount / 4, gnAllocCycleCount);

	return nErrorCount;
}
///////////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////////
// TestAllocation
//
// This is test that's useful for testing things that the other tests don't cover.
//
int TestAllocation()
{
	EA::UnitTest::Report("    TestAllocation\n");

	int nErrorCount = 0;

	// Test GeneralAllocatorDebug
	{
		GeneralAllocatorDebug generalAllocator;

		generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
		generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);
		generalAllocator.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

		gRand.SetSeed(EA::UnitTest::GetRandSeed());
		nErrorCount += TestAllocationGeneric(&generalAllocator, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount);
	}

	// Test GeneralAllocator
	{
		GeneralAllocator generalAllocator;

		generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
		generalAllocator.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

		gRand.SetSeed(EA::UnitTest::GetRandSeed());
		nErrorCount += TestAllocationGeneric(&generalAllocator, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// Test64Bit
//
int Test64Bit()
{
	int nErrorCount = 0;

	// This test it currently disabled on Capilano because on December SDK the 
	// title memory is temporary restricted to 4GB
	// TODO: enable this test on Capilano on the next SDK
	#if ((EA_PLATFORM_PTR_SIZE >= 8) || defined(_WIN64)) && !defined(EA_PLATFORM_XBOXONE)

		EA::UnitTest::Report("    Test64Bit\n");

		uint64_t systemMemoryMB = EA::UnitTest::GetSystemMemoryMB();

		if(systemMemoryMB > 8000) // If 8 GB or more...
		{
			const size_t k4GB  = (uint64_t)1024 * 1024 * 1024 * 4;
			const size_t k5GB  = (uint64_t)1024 * 1024 * 1024 * 5;
			const size_t k16MB = (uint64_t)1024 * 1024 * 16;
 
			for(int i = 0; i < 2; i++)
			{
				GeneralAllocatorDebug generalAllocator;
				const bool bEnableMappedMemory = (i == 0);

				generalAllocator.SetOption(GeneralAllocator::kOptionMMapMaxAllowed, bEnableMappedMemory ? 1000000 : 0);
				generalAllocator.SetDelayedFreePolicy(GeneralAllocatorDebug::kDelayedFreePolicyNone, 0);    // Disable delayed frees so that we don't use huge amounts of memory, and so that the delayed free mechanism doesn't think we are crazy.

				const size_t reportedMaxCoreSize = generalAllocator.GetMaxCoreSize();
				EATEST_VERIFY(reportedMaxCoreSize > k4GB);

				const bool bAddResult = generalAllocator.AddCore(NULL, k5GB, false);
				EATEST_VERIFY(bAddResult);

				if(bAddResult)
				{
					void* pHuge = generalAllocator.Malloc(k5GB - k16MB);
					EATEST_VERIFY(pHuge != NULL);

					if(pHuge)
					{
						//if(!bEnableMappedMemory)
						//    EATEST_VERIFY(?); // Currently the only way to verify memory is not mapped is to see that it is not in a core block.

						generalAllocator.Free(pHuge);
					}

					// We have 5GB of core memory, so we need to do big Mallocs from it.
					nErrorCount += TestAllocationGeneric(&generalAllocator, 0, 10000000, 400, 1500, 2);
					generalAllocator.Shutdown();
				}
			}
		}
		else
			EA::UnitTest::ReportVerbosity(1, "Test64Bit: 64 bit tests are disabled for this platform (" EA_PLATFORM_DESCRIPTION ") due to lack of sufficient system memory.\n");

	#endif

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestLargeAllocations
//
// This is test that's useful for testing things that the other tests don't cover.
//
int TestLargeAllocations()
{
	EA::UnitTest::Report("    TestLargeAllocations\n");

	int nErrorCount = 0;

	uint64_t systemMemoryMB = EA::UnitTest::GetSystemMemoryMB();

	if(systemMemoryMB < 256)
		EA::UnitTest::ReportVerbosity(1, "Large allocation tests are disabled for this platform (" EA_PLATFORM_DESCRIPTION ").\n");
	else
	{
		// Test GeneralAllocatorDebug
		{
			GeneralAllocatorDebug generalAllocator;

			generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
			generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);
			generalAllocator.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

			gRand.SetSeed(EA::UnitTest::GetRandSeed());
			nErrorCount += TestAllocationGeneric(&generalAllocator, 1024, 2 * 1024 * 1024, 50, gnAllocIterationCount, gnAllocCycleCount);
			nErrorCount += TestAllocationGeneric(&generalAllocator, 64000, 120000, 20, 50, 2);
		}

		// Test GeneralAllocator
		{
			GeneralAllocator generalAllocator;

			generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
			generalAllocator.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

			gRand.SetSeed(EA::UnitTest::GetRandSeed());
			nErrorCount += TestAllocationGeneric(&generalAllocator, 1024, 2 * 1024 * 1024, 50, gnAllocIterationCount, gnAllocCycleCount);
			nErrorCount += TestAllocationGeneric(&generalAllocator, 64000, 120000, 20, 50, 2);
		}
	}
	return nErrorCount;
}


#if defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
struct LargePageCoreContext
{
	int& nErrorCount;
	size_t initialReservedSize;
	size_t numberOfCommits;

	void VerifyCommits(size_t expectedCommits)
	{
		EATEST_VERIFY_F(numberOfCommits == expectedCommits, "Expected that %u commit operations would be done at this point, but found there were %zu\n", expectedCommits, numberOfCommits);
	}
};
struct LargePageReportContext
{
	int& nErrorCount;
	size_t expectedReservedSize;
	size_t expectedCommitSize;
};

static bool LargePageHeapReportValidator(const GeneralAllocator::BlockInfo* pBlockInfo, void* pContext)
{
	LargePageReportContext& reportContext = *(LargePageReportContext *)pContext;
	int& nErrorCount = reportContext.nErrorCount;
	EATEST_VERIFY_F(pBlockInfo->mCoreReservedSize == reportContext.expectedReservedSize, "Expected %zu bytes to be reserved, found that %zu were actually reserved\n", reportContext.expectedReservedSize, pBlockInfo->mCoreReservedSize);
	EATEST_VERIFY_F(pBlockInfo->mnBlockSize == reportContext.expectedCommitSize, "Expected %zu bytes to be committed, found that %zu were actually committed\n", reportContext.expectedCommitSize, pBlockInfo->mnBlockSize);
	return false;
}

static size_t LargePageFreeCore(GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, void* pContext)
{
	::VirtualFree(pCore, 0, MEM_RELEASE);
	return nSize;
}

static size_t LargePageExtendCore(GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, size_t nMinExtendedSize, size_t nReservedSize, void* pContext)
{
	LargePageCoreContext& coreContext = *(LargePageCoreContext *)pContext;
	int& nErrorCount = coreContext.nErrorCount;

	size_t commitSize = PPM_AlignUp(nMinExtendedSize, EA_MEBIBYTE(4));
	void* addressToExtendFrom = (char*)pCore + nSize;
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
		void* virtualAllocResult = XMemVirtualAlloc(addressToExtendFrom, commitSize, MEM_COMMIT | MEM_2MB_PAGES, XMEM_CPU, PAGE_READWRITE);
	#else
		void* virtualAllocResult = ::VirtualAlloc(addressToExtendFrom, commitSize, MEM_COMMIT | MEM_4MB_PAGES, PAGE_READWRITE);
	#endif
	++coreContext.numberOfCommits;
	EATEST_VERIFY_F(virtualAllocResult != nullptr, "Failed to get memory when trying to extend %zu bytes from the system\n", commitSize);
	return commitSize;
}

static void* LargePageAddCore(size_t* pMinSize, size_t* pReservedSize,
								GeneralAllocator::CoreFreeFunction* ppCoreFreeFunction, void** ppCoreFreeFunctionContext,
								GeneralAllocator::CoreExtendFunction* ppCoreExtendFunction, void** ppCoreExtendFunctionContext,
								uint32_t& coreType, void* pContext)
{
	LargePageCoreContext& coreContext = *(LargePageCoreContext *)pContext;
	int& nErrorCount = coreContext.nErrorCount;
	size_t reservationSize = PPM_AlignUp(coreContext.initialReservedSize, EA_MEBIBYTE(4));
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
		void* pCore = XMemVirtualAlloc(NULL, reservationSize, MEM_RESERVE | MEM_2MB_PAGES, XMEM_CPU, PAGE_READWRITE);
	#else
		void* pCore = ::VirtualAlloc(0, reservationSize, MEM_RESERVE | MEM_4MB_PAGES, PAGE_READWRITE);
	#endif

	EATEST_VERIFY_F(pCore != nullptr, "Failed to get memory when trying to reserve %zu bytes from the system\n", reservationSize);

	*pReservedSize = reservationSize;
	*ppCoreFreeFunction = LargePageFreeCore;
	*ppCoreFreeFunctionContext = pContext;
	*ppCoreExtendFunction = LargePageExtendCore;
	*ppCoreExtendFunctionContext = pContext;

	size_t commitSize = PPM_AlignUp(*pMinSize, EA_MEBIBYTE(4));
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
		pCore = XMemVirtualAlloc(pCore, commitSize, MEM_COMMIT | MEM_2MB_PAGES, XMEM_CPU, PAGE_READWRITE);
	#else
		pCore = ::VirtualAlloc(pCore, commitSize, MEM_COMMIT | MEM_4MB_PAGES, PAGE_READWRITE);
	#endif
	*pMinSize = commitSize;
	++coreContext.numberOfCommits;

	EATEST_VERIFY_F(pCore != nullptr, "Failed to get memory when trying to commit %zu bytes from the system\n", commitSize);

	return pCore;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// TestLargePages
//
int TestLargePages()
{
	EA::UnitTest::Report("    TestLargePages\n");

#if defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
	int nErrorCount = 0;

	// Test GeneralAllocator
	{
		const size_t expectedInitialReservationSize = 3 * EA_MEBIBYTE(4);
		const size_t expectedInitialCommitSize = EA_MEBIBYTE(4);

		GeneralAllocator generalAllocator;
		generalAllocator.SetOption(GeneralAllocator::kOptionMMapThreshold, EA_GIBIBYTE(1)); // Keep the allocator from mapping its own memory pages from the system

		LargePageCoreContext coreContext { nErrorCount, expectedInitialReservationSize, 0 };

		generalAllocator.SetAddCoreFunction(LargePageAddCore, &coreContext);

		GeneralAllocator::Parameters params;
		params.mnReservedSize = size_t(2.5*EA_MEBIBYTE(4)); // Reserve 2.5 of the 4MB pages - this should be rounded up to 3 of the 4MB pages by the allocator.
		params.mnCoreSize = EA_KIBIBYTE(1); // Commit 1k of core - this should be rounded up to a 4MB page by the allocator.
		params.mpCoreExtendContext = &nErrorCount;
		params.mpCoreExtendFunction = LargePageExtendCore;
		params.mpCoreFreeContext = &nErrorCount;
		params.mpCoreFreeFunction = LargePageFreeCore;
		generalAllocator.Init(params);

		//Confirm that we have reserved 12MB and committed 4MB
		LargePageReportContext reportContext { nErrorCount, expectedInitialReservationSize, EA_MEBIBYTE(4)};
		generalAllocator.ReportHeap(LargePageHeapReportValidator, &reportContext, GeneralAllocator::kBlockTypeCore);
		coreContext.VerifyCommits(1);

		// Request 2MB.  If the initial commit was done for a 4MB page, this should not require any additional extension/commits.
		generalAllocator.Malloc(EA_MEBIBYTE(2));
		coreContext.VerifyCommits(1);

		// Request another 1MB.  If the initial commit was done for a 4MB page, this should not require any additional extension/commits.
		generalAllocator.Malloc(EA_MEBIBYTE(1));
		coreContext.VerifyCommits(1);

		// Request another 2MB.  This should necessitate another extension/commit as it will cross the 4MB page boundary.
		generalAllocator.Malloc(EA_MEBIBYTE(2));
		coreContext.VerifyCommits(2);

		//Confirm that we have reserved 12MB and committed 8MB
		reportContext.expectedCommitSize = EA_MEBIBYTE(8);
		generalAllocator.ReportHeap(LargePageHeapReportValidator, &reportContext, GeneralAllocator::kBlockTypeCore);
	}

	return nErrorCount;
#else
	EA::UnitTest::ReportVerbosity(1, "Large page tests are disabled for this platform (" EA_PLATFORM_DESCRIPTION ").\n");
	return 0;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// TestThreading
//
	#if PPM_TEST_THREADING_ENABLED
	struct ThreadTestSetup
	{
		bool mUseDebugAllocator;
	#if PPM_HOOKS_SUPPORTED
		bool mUseHooks;
		bool mUseLocksDuringHooks;
	#endif
	};

	struct GlobalThreadTestInfo
	{
		ThreadTestSetup*		mpThreadTestSetup = NULL;
		GeneralAllocator*       mpAllocator = NULL;
		EA::Thread::AtomicInt32 mnThreadCount = 0;
		EA::Thread::AtomicInt32 mnErrorCount = 0;
		EA::Thread::AtomicInt32 mnHookInUseCount = 0;
		EA::Thread::AtomicInt32 mnAllocatorCallInUseCount = 0;
		EA::Thread::AtomicInt32 mnAllocatorAllocationCount = 0;

		GlobalThreadTestInfo(ThreadTestSetup *pTestSetup, int32_t nThreadCount = 0) :
			mpThreadTestSetup(pTestSetup),
			mnThreadCount(nThreadCount)
		{
			if (pTestSetup->mUseDebugAllocator)
			{
				mpAllocator = new EA::Allocator::GeneralAllocatorDebug();
			}
			else
			{
				mpAllocator = new EA::Allocator::GeneralAllocator();
			}
		}

		~GlobalThreadTestInfo()
		{
			delete mpAllocator;
		}
	};

	struct ThreadTestInfo
	{
		GlobalThreadTestInfo* mpGlobalInfo = NULL;
		EA::UnitTest::Rand mRand = EA::UnitTest::Rand(0);
		EA::Thread::Thread mThread;
	};

	#if PPM_HOOKS_SUPPORTED
		static void Hook(const EA::Allocator::GeneralAllocator::HookInfo* pHookInfo, void* pContext)
		{
			int nErrorCount = 0;

			GlobalThreadTestInfo* const pGlobalThreadTestInfo = (GlobalThreadTestInfo*)pContext;
			uint32_t newHookCount = pGlobalThreadTestInfo->mnHookInUseCount.Increment();
			// If Locks are not used during Hooks, the Hook function may be called concurrently from multiple threads.
			EATEST_VERIFY(!pGlobalThreadTestInfo->mpThreadTestSetup->mUseLocksDuringHooks || (newHookCount == 1));

			if (pHookInfo->mHookType == EA::Allocator::GeneralAllocator::kHookTypeMalloc)
			{
				// for Malloc functions Hook should be called twice, once on entry and once on exit.
				if (pHookInfo->mbEntry == true)
				{
					pGlobalThreadTestInfo->mnAllocatorCallInUseCount.Increment();
				}
				else
				{
					pGlobalThreadTestInfo->mnAllocatorCallInUseCount.Decrement();
				}

				if((pHookInfo->mHookSubType == EA::Allocator::GeneralAllocator::kHookSubTypeMalloc) || (pHookInfo->mHookSubType == EA::Allocator::GeneralAllocator::kHookSubTypeMallocAligned))
				{
					pGlobalThreadTestInfo->mnAllocatorAllocationCount.Increment();
				}
				else if (pHookInfo->mHookSubType == EA::Allocator::GeneralAllocator::kHookSubTypeRealloc)
				{
					// Realloc with NULL pData is effectively Malloc.
					if (!pHookInfo->mpDataInput)
					{
						pGlobalThreadTestInfo->mnAllocatorAllocationCount.Increment();
					}
				}
				else
				{
					EATEST_VERIFY_MSG(false, "Unknown HookType used!");
				}
			}

			if (pHookInfo->mHookType == EA::Allocator::GeneralAllocator::kHookTypeFree)
			{
				// for Free functions Hook should be called twice, once on entry and once on exit.
				if (pHookInfo->mbEntry == true)
				{
					pGlobalThreadTestInfo->mnAllocatorCallInUseCount.Increment();
				}
				else
				{
					pGlobalThreadTestInfo->mnAllocatorCallInUseCount.Decrement();
				}

				// No need to check subtypes as Malloc, MallocAligned and Realloc(with NULL) all map to Free.
				pGlobalThreadTestInfo->mnAllocatorAllocationCount.Decrement();
			}

			pGlobalThreadTestInfo->mnHookInUseCount.Decrement();
			pGlobalThreadTestInfo->mnErrorCount += nErrorCount;
		}
	#endif

	static intptr_t TestThreadingThreadFunction(void* pContext)
	{
		const int32_t AlignmentPow2Limit = 10;
		const int32_t offsetAlignment = 16;
		ThreadTestInfo* const pThreadTestInfo = (ThreadTestInfo*)pContext;
		auto* const pGlobalInfo = pThreadTestInfo->mpGlobalInfo;

		pGlobalInfo->mnErrorCount += TestAllocationGeneric(pGlobalInfo->mpAllocator, gnAllocSizeMin, gnAllocSizeMax,
															   gnAllocSlotCount / gnThreadCount, gnAllocIterationCount / gnThreadCount, 
															   gnAllocCycleCount, pThreadTestInfo->mRand);

		int32_t alignment = 1 << pThreadTestInfo->mRand.RandLimit(AlignmentPow2Limit);
		int32_t offset = pThreadTestInfo->mRand.RandRange(0, alignment / offsetAlignment) * offsetAlignment;
		EA::UnitTest::ReportVerbosity(1, "TestThreadingThreadFunction using aligment %d and offset %d\n", alignment, offset);
		pGlobalInfo->mnErrorCount += TestAllocationAligned(pGlobalInfo->mpAllocator, gnAllocSizeMin, gnAllocSizeMax,
															   alignment, offset,
															   gnAllocSlotCount / gnThreadCount, gnAllocIterationCount / gnThreadCount,
															   gnAllocCycleCount, pThreadTestInfo->mRand);

		pGlobalInfo->mpAllocator->Lock(true); // Just to exercise this function.
		pGlobalInfo->mpAllocator->Lock(false);

		pGlobalInfo->mnThreadCount--;

		return 0;
	}
#endif

int TestThreading()
{
	EA::UnitTest::Report("\tTestThreading\n");
	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	int nErrorCount = 0;

	#if PPM_TEST_THREADING_ENABLED
		ThreadTestSetup tests[] =
		{
			#if PPM_HOOKS_SUPPORTED
				// { mUseDebugAllocator, mUseHooks, mUseLocksDuringHooks }
				{false, false, false},
				// {false, false, true}, If mUseHooks is false then mUseLocksDuringHooks is meaningless.
				{false, true, false},
				{false, true, true},
				{true, false, false},
				// {false, false, true}, If mUseHooks is false then mUseLocksDuringHooks is meaningless.
				{true, true, false},
				{true, true, true}
			#else
				// { mUseDebugAllocator }
				{false},
				{true}
			#endif		
		};

		const int			  numThreads = 4;
		EA::Thread::Thread    thread;
		for (auto &test : tests)
		{
			#if PPM_HOOKS_SUPPORTED
				EA::EAMain::Report("\t\t mUseDebugAllocator: %s, mUseHooks: %s, mUseLocksDuringHooks: %s,\n",
					test.mUseDebugAllocator ? "true" : "false",
					test.mUseHooks ? "true" : "false",
					test.mUseLocksDuringHooks ? "true" : "false");
			#else
				EA::EAMain::Report("\t\t mUseDebugAllocator: %s\n",
					test.mUseDebugAllocator ? "true" : "false");
			#endif

			GlobalThreadTestInfo *pGlobalThreadTestInfo = new GlobalThreadTestInfo(&test, numThreads);

			#if PPM_HOOKS_SUPPORTED
				if (test.mUseHooks)
				{
					pGlobalThreadTestInfo->mpAllocator->SetHookFunction(&Hook, pGlobalThreadTestInfo);
				}

				if (test.mUseLocksDuringHooks)
				{
					pGlobalThreadTestInfo->mpAllocator->SetOption(GeneralAllocator::kOptionLockDuringHookCalls, 1);
				}
			#endif

			pGlobalThreadTestInfo->mpAllocator->SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
			pGlobalThreadTestInfo->mpAllocator->SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

			ThreadTestInfo* threadTestInfo = new ThreadTestInfo[numThreads];
			for (int i(0); i < numThreads; i++)
			{
				threadTestInfo[i].mpGlobalInfo = pGlobalThreadTestInfo;
				threadTestInfo[i].mRand.SetSeed(gRand.RandValue());
				threadTestInfo[i].mThread.Begin(TestThreadingThreadFunction, &threadTestInfo[i]);
			}

			auto endTime = EA::Thread::GetThreadTime() + (gTestLengthSeconds * 1000);
			for (int i(0); i < numThreads; i++)
			{
				threadTestInfo[i].mThread.WaitForEnd(endTime);
			}

			nErrorCount += pGlobalThreadTestInfo->mnErrorCount;
			EATEST_VERIFY_MSG(pGlobalThreadTestInfo->mnThreadCount == 0, "Concurrency failure: Thread(s) didn't end.");
			EATEST_VERIFY_MSG(pGlobalThreadTestInfo->mnHookInUseCount == 0, "Concurrency failure: Mismatched number of Hook start/end.");
			EATEST_VERIFY_MSG(pGlobalThreadTestInfo->mnAllocatorCallInUseCount == 0, "Concurrency failure: Mismatched number of calls to Hook in Allocator functions (Malloc/Free/Realloc/etc.");
			EATEST_VERIFY_MSG(pGlobalThreadTestInfo->mnAllocatorAllocationCount == 0, "Concurrency failure: Mismatched number of calls to Malloc/Free.");

			// If not all the threads have terminated, leak the allocator as it is probably in a bad state and will deadlock on Shutdown(), also leak the TestInfos as they are potentially still being written to.
			if (pGlobalThreadTestInfo->mnThreadCount == 0)
			{
				delete[] threadTestInfo;
				delete pGlobalThreadTestInfo;
			}
			threadTestInfo = NULL;
			pGlobalThreadTestInfo = NULL;
		}
	#else
		EA::UnitTest::ReportVerbosity(1, "Threading test not currently done on this platform.\n");
		// For platforms that aren't pre-emptive, we'll need to have the test call
		// a yield function in order to properly test multiple threads.
	#endif        

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestAllocationFlags
//
int TestAllocationFlags()
{
	EA::UnitTest::Report("    TestAllocationFlags\n");

	int nErrorCount = 0;

	GeneralAllocatorDebug generalAllocator;
	GeneralAllocator* pAllocator = &generalAllocator;

	pAllocator->SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	pAllocator->SetOption(GeneralAllocator::kOptionEnableHighAllocation, 1);
	pAllocator->SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);

	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	//Test AllocationFlags
	void** pDataArray = new void*[gnAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * gnAllocSlotCount);

	int nFlagOptions[] = 
	{ 
		GeneralAllocator::kAllocationFlagNone,
		GeneralAllocator::kAllocationFlagHigh,
		GeneralAllocator::kAllocationFlagEndFit,
		GeneralAllocator::kAllocationFlagBestFit,
		GeneralAllocator::kAllocationFlagHigh   | GeneralAllocator::kAllocationFlagEndFit,
		GeneralAllocator::kAllocationFlagHigh   | GeneralAllocator::kAllocationFlagBestFit,
		GeneralAllocator::kAllocationFlagEndFit | GeneralAllocator::kAllocationFlagBestFit,
		GeneralAllocator::kAllocationFlagMMap
		// To consider: Add MMap plus other flags, just to make sure there isn't some silly error going on there.
	};

	// Run tests
	for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < gnAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)gnAllocIterationCount, (unsigned)c, (unsigned)gnAllocCycleCount);

			i = gRand.RandRange(0, (int)gnAllocSlotCount);

			size_t nSize = 0;

			if(j == 0)
				nSize = gRand.RandRange(4, 1000);           // Random number between 4 and 1000
			else if(j == 1)
				nSize = 1000 + (gRand.RandLimit(4) * 500);  // 1000, 1500, 2000, or 2500
			else if(j == 2)
				nSize = gRand.RandRange(1000, 4000);        // Random number between 1000 and 4000

			const size_t nOperation = gRand.RandRange(0, kOperationCount);      // 0, 1, 2, 3, or 4.
			const int    nFlags     = nFlagOptions[gRand.RandRange(0, sizeof(nFlagOptions)/sizeof(nFlagOptions[0]))];
			const size_t nCount     = gRand.RandRange(1, 3);                    // 1, or 2.
			const size_t nAlignment = 16 * gRand.RandRange(0, 2);               // 0, 16\n", or 32.
			const size_t nOffset    = PPM_MIN_ALIGN * gRand.RandRange(0, 2);    // 0,  PPM_MIN_ALIGN, or PPM_MIN_ALIGN * 2.

			if(nOperation == kOperationRealloc2)
			{
				// Test reallocating memory.
				pDataArray[i] = pAllocator->Realloc(pDataArray[i], nSize, nFlags);
				if(pDataArray[i])
					memset(pDataArray[i], gFill[nOperation], nSize);
				else
					EATEST_VERIFY_F(pDataArray[i] != NULL, "TestAllocationFlags error: Failed to allocate memory. Operation %d, size %u, flags 0x%08x\n", (int)nOperation, (unsigned)nSize, (unsigned)nFlags);

				// Test freeing of null.
				#if PPM_NULL_POINTER_FREE_ENABLED
					pAllocator->Free(NULL); // Make sure Free of NULL pointer works
				#endif
			}
			else if(pDataArray[i])
			{
				// Test freeing of memory
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				// Test allocating memory
				switch (nOperation)
				{ 
					case kOperationMalloc:
						pDataArray[i] = pAllocator->Malloc(nSize, nFlags);
						break;
					case kOperationCalloc:
						pDataArray[i] = pAllocator->Calloc(nCount, nSize, nFlags);
						nSize *= nCount; // Do this so that we can use just nSize below.
						break;
					case kOperationRealloc:
						pDataArray[i] = pAllocator->Realloc(pDataArray[i], nSize, nFlags);
						break;
					default: // kOperationMallocAligned
						pDataArray[i] = pAllocator->MallocAligned(nSize, nAlignment, nOffset, nFlags);
						break;
				}

				if(pDataArray[i])
				{
					if(nOperation == kOperationCalloc) // If calloc...
					{
						void* pData = pDataArray[i];

						for(size_t k = 0; k < nSize; k++)
						{
							if(((char*)pData)[k] != 0)
							{
								EATEST_VERIFY_MSG((((char*)pData)[k]) != 0, "TestAllocationFlags error: Calloc didn't return zeroed memory.\n");
								break;
							}
						}
					}
					memset(pDataArray[i], gFill[nOperation], nSize);
				}
				else
					EATEST_VERIFY_F(pDataArray[i] != NULL, "TestAllocationFlags error: Failed to allocate memory. Operation %d, size %u, flags 0x%08x\n", (int)nOperation, (unsigned)nSize, (unsigned)nFlags);
			}
		}

		for(i = 0; i < gnAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}

	delete[] pDataArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestBestFit
//
int TestBestFit()
{
	EA::UnitTest::Report("    TestBestFit\n");

	int nErrorCount = 0;

	GeneralAllocatorDebug generalAllocator;
	GeneralAllocator* pAllocator = &generalAllocator;

	pAllocator->SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	pAllocator->SetOption(GeneralAllocator::kOptionEnableHighAllocation, 1);
	pAllocator->SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
	pAllocator->SetOption(GeneralAllocator::kOptionMaxFastBinRequestSize, 0);

	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	//Test AllocationFlags
	void** pDataArray = new void*[gnAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * gnAllocSlotCount);

	int nFlagOptions[] = 
	{ 
		GeneralAllocator::kAllocationFlagNone,
		GeneralAllocator::kAllocationFlagBestFit,
		GeneralAllocator::kAllocationFlagHigh,
		GeneralAllocator::kAllocationFlagHigh   | GeneralAllocator::kAllocationFlagBestFit,
		GeneralAllocator::kAllocationFlagEndFit | GeneralAllocator::kAllocationFlagBestFit
	};

	// Run tests
	for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < gnAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)gnAllocIterationCount, (unsigned)c, (unsigned)gnAllocCycleCount);

			i = gRand.RandRange(0, (int)gnAllocSlotCount);

			size_t nSize = 0;

			if(j == 0)
				nSize = gRand.RandRange(4, 1000);           // Random number between 4 and 1000
			else if(j == 1)
				nSize = 1000 + (gRand.RandLimit(4) * 500);  // 1000, 1500, 2000, or 2500
			else if(j == 2)
				nSize = gRand.RandRange(1000, 4000);        // Random number between 1000 and 4000

			const size_t nOperation = gRand.RandRange(0, kOperationCount);                      // 0, 1, 2, 3, or 4.
			const int    nFlags     = nFlagOptions[gRand.RandRange(0, sizeof(nFlagOptions)/sizeof(nFlagOptions[0]))];
			const size_t nCount     = gRand.RandRange(1, 3);                                    // 1, or 2.
			const size_t nAlignment = 16 * gRand.RandRange(0, 2);                               // 0, 16\n", or 32.
			const size_t nOffset    = GeneralAllocator::kAlignmentMin * gRand.RandRange(0, 2);  // 0,  kAlignmentMin, or kAlignmentMin * 2.

			if(nOperation == kOperationRealloc2)
			{
				// Test reallocating memory.
				pDataArray[i] = pAllocator->Realloc(pDataArray[i], nSize, nFlags);
				if(pDataArray[i])
					memset(pDataArray[i], gFill[nOperation], nSize);
				else
					EATEST_VERIFY_MSG(pDataArray[i] != NULL, "TestBestFit error: Failed to allocate memory.\n");
			}
			else if(pDataArray[i])
			{
				// Test freeing of memory
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				// Test allocating memory
				switch (nOperation)
				{ 
					case kOperationMalloc:
						pDataArray[i] = pAllocator->Malloc(nSize, nFlags);
						break;
					case kOperationCalloc:
						pDataArray[i] = pAllocator->Calloc(nCount, nSize, nFlags);
						nSize *= nCount; // Do this so that we can use just nSize below.
						break;
					case kOperationRealloc:
						pDataArray[i] = pAllocator->Realloc(pDataArray[i], nSize, nFlags);
						break;
					default: // kOperationMallocAligned
						pDataArray[i] = pAllocator->MallocAligned(nSize, nAlignment, nOffset, nFlags);
						break;
				}

				if(pDataArray[i])
				{
					if(nOperation == kOperationCalloc) // If calloc...
					{
						for(size_t k = 0; k < nSize; k++)
						{
							void* pData = pDataArray[i];

							if(((char*)pData)[k] != 0)
							{
								EATEST_VERIFY_MSG(((char*)pData)[k] != 0, "TestBestFit error: Calloc didn't return zeroed memory.\n");
								break;
							}
						}
					}
					memset(pDataArray[i], gFill[nOperation], nSize);
				}
				else
					EATEST_VERIFY_MSG(pDataArray[i] != NULL, "TestBestFit error: Failed to allocate memory.\n");
			}
		}

		for(i = 0; i < gnAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}


	// Run special regression of user-reported bug.
	for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < gnAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)gnAllocIterationCount, (unsigned)c, (unsigned)gnAllocCycleCount);

			i = gRand.RandRange(0, (int)gnAllocSlotCount);

			size_t nSize = 0;

			if(j == 0)
				nSize = gRand.RandRange(4, 1000);           // Random number between 4 and 1000
			else if(j == 1)
				nSize = 1000 + (gRand.RandLimit(4) * 500);  // 1000, 1500, 2000, or 2500
			else if(j == 2)
				nSize = gRand.RandRange(1000, 4000);        // Random number between 1000 and 4000

			const int    nFlags     = GeneralAllocator::kAllocationFlagHigh | GeneralAllocator::kAllocationFlagEndFit | GeneralAllocator::kAllocationFlagBestFit | GeneralAllocator::kAllocationFlagAlwaysAlign;
			const size_t nAlignment = 16 * gRand.RandRange(0, 2);               // 0, 16\n", or 32.
			const size_t nOffset    = PPM_MIN_ALIGN * gRand.RandRange(0, 2);    // 0,  PPM_MIN_ALIGN, or PPM_MIN_ALIGN * 2.

			#if defined(EA_PLATFORM_DESKTOP)
				EATEST_VERIFY(pAllocator->ValidateHeap(GeneralAllocator::kHeapValidationLevelFull));
			#endif // Else skip it because it takes a while and we probably don't really need to do this.

			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				pDataArray[i] = pAllocator->MallocAligned(nSize, nAlignment, nOffset, nFlags);

				if(pDataArray[i])
					memset(pDataArray[i], gFill[kOperationMallocAligned], nSize);
				else
					EATEST_VERIFY_MSG(pDataArray[i] != NULL, "TestBestFit error: Failed to allocate memory.\n");
			}
		}

		for(i = 0; i < gnAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				pAllocator->Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}


	delete[] pDataArray;

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestDebugNaming
//
// Tests named allocations
//
int TestDebugNaming()
{
	EA::UnitTest::Report("    TestDebugNaming\n");

	int nErrorCount = 0;

	GeneralAllocatorDebug generalAllocator;
	generalAllocator.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	generalAllocator.SetOption(GeneralAllocator::kOptionEnableHighAllocation, 1);
	generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	//Test AllocationFlags
	void** pDataArray = new void*[gnAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * gnAllocSlotCount);

	int nFlagOptions[4] = 
	{ 
		GeneralAllocator::kAllocationFlagNone,
		GeneralAllocator::kAllocationFlagHigh,
		GeneralAllocator::kAllocationFlagEndFit,
		GeneralAllocator::kAllocationFlagHigh | GeneralAllocator::kAllocationFlagEndFit
	};

	// Run tests
	for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < gnAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)gnAllocIterationCount, (unsigned)c, (unsigned)gnAllocCycleCount);

			i = gRand.RandRange(0, (int)gnAllocSlotCount);

			if(pDataArray[i])
			{
				// Verify name is as intended
				size_t nRequestedSize = 0xffffffff;
				char    pStoredName[256] = { 0 };
				char    pExpectedName[256] = { 0 };
				generalAllocator.GetDebugData(pDataArray[i], GeneralAllocatorDebug::kDebugDataIdSize, &nRequestedSize, sizeof(nRequestedSize));
				generalAllocator.GetDebugData(pDataArray[i], GeneralAllocatorDebug::kDebugDataIdName, &pStoredName, sizeof(pStoredName)/sizeof(pStoredName[0]));
				EA::StdC::Sprintf(pExpectedName, "%u", (unsigned)nRequestedSize);
				EATEST_VERIFY_MSG(strcmp(pExpectedName, pStoredName) == 0, "TestDebugNaming error.\n");

				// Free the space.
				generalAllocator.Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
			else
			{
				const size_t   nOperation  = (gRand.RandRange(0, 2) == 0) ? kOperationMalloc : kOperationMallocAligned;  // 0, 1.
				const size_t   nSize       = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
				const int      nFlags      = nFlagOptions[gRand.RandRange(0, 4)];
				const size_t   nAlignment  = gRand.RandLimit(2) ? (size_t)0 : (size_t)(8 << gRand.RandRange(0, 3)); // 0, 8, 16\n", 32, 64.
				const size_t   nOffset     = PPM_MIN_ALIGN * gRand.RandRange(0, 5);      // 0, 8, 16\n", 24, 32. (or 0, 16\n", 32, 48, 64 for PPM_MIN_ALIGN = 16\n")
				const size_t   bFileLine   = gRand.RandRange(0, 2);          // 0, 1.
				const unsigned nDebugFlags = (1 << GeneralAllocatorDebug::kDebugDataIdSize) | (1 << GeneralAllocatorDebug::kDebugDataIdFlags);

				char pName[256];
				EA::StdC::Sprintf(pName, "%u", (unsigned)nSize);

				switch (nOperation)
				{
					case kOperationMalloc:
						if(bFileLine)
							pDataArray[i] = generalAllocator.MallocDebug(nSize, nFlags, nDebugFlags, pName, __FILE__, __LINE__);
						else
							pDataArray[i] = generalAllocator.MallocDebug(nSize, nFlags, nDebugFlags, pName);
						break;

					default:
						if(bFileLine)
							pDataArray[i] = generalAllocator.MallocAlignedDebug(nSize, nAlignment, nOffset, nFlags, nDebugFlags, pName, __FILE__, __LINE__);
						else
							pDataArray[i] = generalAllocator.MallocAlignedDebug(nSize, nAlignment, nOffset, nFlags, nDebugFlags, pName);
						break;
				}

				if(pDataArray[i])
					memset(pDataArray[i], gFill[nOperation], nSize);
				else
					EATEST_VERIFY_MSG(!pDataArray[i], "TestDebugNaming error\n");
			}
		}

		for(i = 0; i < gnAllocSlotCount; i++)
		{
			if(pDataArray[i])
			{
				generalAllocator.Free(pDataArray[i]);
				pDataArray[i] = NULL;
			}
		}
	}

	delete[] pDataArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDebugGroupId
//
// Tests GeneralAllocatorDebug::kDebugDataIdGroup.
//
int TestDebugGroupId()
{
	EA::UnitTest::Report("    TestDebugGroupId\n");

	int nErrorCount = 0;

	size_t i;

	GeneralAllocatorDebug generalAllocatorDebug;
	generalAllocatorDebug.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	generalAllocatorDebug.SetDefaultDebugDataFlag(GeneralAllocatorDebug::kDebugDataIdGroup);
	generalAllocatorDebug.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
	
	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	void** pDataArray = new void*[gnAllocSlotCount];
	memset(pDataArray, 0, sizeof(void*) * gnAllocSlotCount);

	// Allocate memory, assigning different groups.
	EA::UnitTest::ReportVerbosity(1, "doing allocations...");
	for(i = 0; i < gnAllocSlotCount; i++)
	{
		const size_t nSize           = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
		const bool   bDisableGroupId = ((i % 20) == 0);

		if(EA::UnitTest::GetVerbosity() >= 1)
		{
			if((i % 100) == 0)
				 putchar('.');
		}

		generalAllocatorDebug.SetCurrentGroupId((int)(i % 2)); // Every other allocation belongs to the same group.

		if(bDisableGroupId)
			generalAllocatorDebug.SetDefaultDebugDataFlags(generalAllocatorDebug.GetDefaultDebugDataFlags() & ~(1 << GeneralAllocatorDebug::kDebugDataIdGroup));

		pDataArray[i] = generalAllocatorDebug.Malloc(nSize);

		if(bDisableGroupId)
			generalAllocatorDebug.SetDefaultDebugDataFlag(GeneralAllocatorDebug::kDebugDataIdGroup);
	}
	if(EA::UnitTest::GetVerbosity() >= 1)
		putchar('\n');

	// Report existing memory
	const void* const pContext = generalAllocatorDebug.ReportBegin(NULL, GeneralAllocator::kBlockTypeAllocated, true); // 'true' here means to iterate the heap in place (dynamically) instead of taking a snapshot of it.

	EA::UnitTest::ReportVerbosity(1, "validation checks...");
	size_t loop = 0;

	for(const GeneralAllocator::BlockInfo* pBlockInfo = generalAllocatorDebug.ReportNext(pContext); 
		 pBlockInfo; 
		 pBlockInfo = generalAllocatorDebug.ReportNext(pContext))
	{
		int    nGroupId;                // Actual found group id for allocation.
		size_t nIndex(0);               // Original index for allocation.
		bool   bDisableGroupId = false; // Original value for disabling.

		if(EA::UnitTest::GetVerbosity() >= 1)
		{
			if((loop % 1000) == 0)
				 putchar('.');
		}

		loop++;

		for(i = 0; i < gnAllocSlotCount; i++) // Find out which index this block is.
		{
			if(pDataArray[i] == pBlockInfo->mpData)
			{
				nIndex = i;
				bDisableGroupId = ((i % 20) == 0);
				break;
			}            
		}

		if(i < 100) // If the block was one of ours...
		{
			if(generalAllocatorDebug.GetDebugData(pBlockInfo->mpData, GeneralAllocatorDebug::kDebugDataIdGroup, &nGroupId, sizeof(nGroupId)))
			{
				if(bDisableGroupId)
					EATEST_VERIFY_MSG(!bDisableGroupId, "TestDebugGroupId: Group id was found but shouldn't have been.\n");
				else if(((int)nIndex % 2) != nGroupId)
					EATEST_VERIFY_MSG(((int)nIndex % 2) == nGroupId, "TestDebugGroupId: Group id was incorrect.\n");
			}
			else
				EATEST_VERIFY_MSG(bDisableGroupId, "TestDebugGroupId: Group id wasn't found but should have been.\n");
		}
		else
		{
			// Verify that GetDebugData works OK (regardless of return value) for any block, including those we may not have allocated.
			generalAllocatorDebug.GetDebugData(pBlockInfo->mpData, GeneralAllocatorDebug::kDebugDataIdGroup, &nGroupId, sizeof(nGroupId));
		}
	}
	generalAllocatorDebug.ReportEnd(pContext);
	if(EA::UnitTest::GetVerbosity() >= 1)
		putchar('\n');


	// Free all memory
	EA::UnitTest::ReportVerbosity(1, "freeing memory...");
	for(i = 0; i < gnAllocSlotCount; i++)
	{
		if(EA::UnitTest::GetVerbosity() >= 1)
		{
			if((i % 100) == 0)
				 putchar('.');
		}

		if(pDataArray[i])
		{
			generalAllocatorDebug.Free(pDataArray[i]);
			pDataArray[i] = NULL;
		}
	}
	if(EA::UnitTest::GetVerbosity() >= 1)
		putchar('\n');

	delete[] pDataArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestReportHeap
//
bool DescribeBlock(const GeneralAllocator::BlockInfo* pBlockInfo, void* /*pContext*/)
{
	//GeneralAllocator* pGeneralAllocator = reinterpret_cast<GeneralAllocator*>(pContext);
	const char* blockType = "Unknown";

	switch(pBlockInfo->mBlockType)
	{
		case GeneralAllocator::kBlockTypeNone:
			blockType = "None";
			break;
		case GeneralAllocator::kBlockTypeInvalid:
			blockType = "Invalid";
			break;
		case GeneralAllocator::kBlockTypeAllocated:
			blockType = "Allocated";
			break;
		case GeneralAllocator::kBlockTypeFree:
			blockType = "Free";
			break;
		case GeneralAllocator::kBlockTypeCore:
			blockType = "Core";
			break;
	}

	EA::UnitTest::ReportVerbosity(1, "Block type: %12.12s addr: 0x%08x, size: %8d, data addr: 0x%08x, data size: %8d\n", blockType, (unsigned)(uintptr_t)pBlockInfo->mpBlock, (unsigned)pBlockInfo->mnBlockSize, (unsigned)(uintptr_t)pBlockInfo->mpData, (unsigned)pBlockInfo->mnDataSize);
	return true;
}


int TestReportHeap()
{
	EA::UnitTest::Report("    TestReportHeap\n");

	int nErrorCount = 0;

	GeneralAllocatorDebug generalAllocator;
	generalAllocator.SetDefaultDebugDataFlag(GeneralAllocatorDebug::kDebugDataIdCallStack);
	generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);  // A gnNewCoreSize of zero means to use PPM_NEW_CORE_SIZE_DEFAULT.

	void* p0 = generalAllocator.MallocDebug(128, 0, 0, NULL, __FILE__, __LINE__);
	void* p1 = generalAllocator.Malloc(256);
	void* p2 = generalAllocator.Malloc(64);
	void* p3 = generalAllocator.MallocDebug(73, 0, 1 << GeneralAllocatorDebug::kDebugDataIdAllocationTime, "Test", __FILE__, __LINE__);
	void* p4 = generalAllocator.Malloc(512);
	void* p5 = generalAllocator.Malloc(514);
	void* p6 = generalAllocator.Malloc(88888);
	void* p7 = generalAllocator.Malloc(4);
	void* p8 = generalAllocator.MallocDebug(644664, 0, (1 << GeneralAllocatorDebug::kDebugDataIdInternalCount) | (1 << GeneralAllocatorDebug::kDebugDataIdOverhead), NULL, __FILE__, __LINE__);
	void* p9 = generalAllocator.Malloc(12);

	// Free a few blocks.
	generalAllocator.Free(p3);
	generalAllocator.Free(p5);
	generalAllocator.Free(p6);

	// Do a report.
	if(EA::UnitTest::GetVerbosity() >= 1)
	{
		// We print some statements that we are tracing so that a user doesn't see the output and think it's due to some memory leak.
		EA::UnitTest::ReportVerbosity(1, "Trace of current memory allocations in TestReportHeap:\n");

		void* pContext;
		GeneralAllocator::TraceFunction pTraceFunction = generalAllocator.GetTraceFunction(&pContext);
		pTraceFunction("Trace of current memory allocations in TestReportHeap:\n", pContext);

		generalAllocator.TraceAllocatedMemory();
		generalAllocator.ReportHeap(DescribeBlock, static_cast<GeneralAllocator*>(&generalAllocator), 0xff, false);
		generalAllocator.ReportHeap(DescribeBlock, static_cast<GeneralAllocator*>(&generalAllocator), 0xff, true);

		EA::UnitTest::ReportVerbosity(1, "End of trace of current memory allocations in TestReportHeap.\n");
		pTraceFunction("End of trace of current memory allocations in TestReportHeap.\n", pContext);
	}

	// Test GetLargestFreeBlock.
	// Michael Schwartz of EALA reported that GeneralAllocator::GetLargestFreeBlock was not 
	// reporting the largest block and that a manual implementation was working better.
	generalAllocator.ClearCache();
	const size_t nLargestSize       = generalAllocator.GetLargestFreeBlock();
	const size_t nLargestSizeManual = GetLargestFreeBlockManual(&generalAllocator);
	EATEST_VERIFY(nLargestSize == nLargestSizeManual);

	// Free the rest of the blocks.
	generalAllocator.Free(p0);
	generalAllocator.Free(p1);
	generalAllocator.Free(p2);
	generalAllocator.Free(p4);
	generalAllocator.Free(p7);
	generalAllocator.Free(p8);
	generalAllocator.Free(p9);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDelayedFree
//
int TestDelayedFree()
{
	EA::UnitTest::Report("    TestDelayedFree\n");

	int nErrorCount = 0;

	const size_t kCoreSize = 10000000;
	void* const pCore = malloc(kCoreSize);
	GeneralAllocatorDebug generalAllocator(pCore, kCoreSize, false);
	generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0); // Disable system allocation, so that all allocations go to our MallocFailureFunction.
	//generalAllocator.SetDefaultDebugDataFlags(0); Possibly enable this some of the time.

	for(int j = 0; j < 4; j++)
	{
		size_t i;

		for(i = 0; i < 32; i++)
		{
			const size_t n = (size_t)1 << i;

			if(n <= generalAllocator.GetMaxMallocSize())
			{
				void* const pData = generalAllocator.Malloc(n);

				if(pData)
					generalAllocator.Free(pData);
			}
		}
	 
		for(i = 0; i < 32; i++)
		{
			const size_t n = (size_t)1 << i;

			if(n <= generalAllocator.GetMaxMallocSize())
			{
				void* const pData = generalAllocator.MallocAligned(n, 128, 0, GeneralAllocator::kAllocationFlagBestFit);

				if(pData)
					generalAllocator.Free(pData);
			}
		}
	 
		generalAllocator.ClearDelayedFreeList();
	}

	generalAllocator.Shutdown();
	free(pCore);

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestExtendCore
//
#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	static size_t kReservedSize     = 1024 * 1024;              // 1024 Kib
	static size_t kCommitSize       = kReservedSize / 4;        //  256 Kib
	static size_t kAllocationSize   = kCommitSize / 5;          //  ~60 Kib
	static size_t kAllocationVolume = kReservedSize * 3 / 4;    //  768 Kib

	static size_t nFreeCallCount   = 0;
	static size_t nExtendCallCount = 0;
	static size_t nMallocCallCount = 0;

	static size_t TestCoreFreeFunction(GeneralAllocator*, void* pCore, size_t nSize, void* /*pContext*/)
	{
		nFreeCallCount++;
		VirtualFree(pCore, 0, MEM_RELEASE);

		return nSize;
	}

	static size_t TestCoreExtendFunction(GeneralAllocator*, void* pCore, size_t nSize, size_t nMinExtendedSize, size_t nReservedSize, void* /*pContext*/)
	{
		int nErrorCount = 0;

		EATEST_VERIFY(nReservedSize == kReservedSize);             // We initially called AddCoreEx with kReservedSize as the specified reserved size, so GeneralAllocator should hand that same value back to us.
		EATEST_VERIFY((nSize + nMinExtendedSize) < kReservedSize); // We rigged the test code below so that this should be so. In practice it might well be that the user can't extend the memory any more. Perhaps we should test that too.

		nExtendCallCount++;
		nMinExtendedSize = PPM_AlignUp(nMinExtendedSize, kCommitSize);
		pCore            = VirtualAlloc((char*)pCore + nSize, nMinExtendedSize, MEM_COMMIT, PAGE_READWRITE);

		EATEST_VERIFY(pCore != NULL);
		EA::UnitTest::IncrementGlobalErrorCount(nErrorCount);

		return pCore ? nMinExtendedSize : 0;
	}

	static bool TestMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* /*pContext*/)
	{
		bool bReturnValue = false;

		nMallocCallCount++;

		// We assert that we rigged this test sanely. The TestExtendCore function below should be calling
		// Malloc for memory blocks taht are of sizes smaller than kInitialCommitSize. This is only because 
		// it allows us to make the logic of this unit test simpler.
		if((nMallocRequestedSize < kCommitSize) && (nAllocatorRequestedSize < kCommitSize))
		{
			void* pCore = VirtualAlloc(NULL, kReservedSize, MEM_RESERVE, PAGE_READWRITE);

			if(pCore)
			{
				pCore = VirtualAlloc(pCore, kCommitSize, MEM_COMMIT, PAGE_READWRITE);

				if(pCore)
				{
					bReturnValue = pGeneralAllocator->AddCoreEx(pCore, kCommitSize, GeneralAllocator::kCoreTypeUserDefined, kReservedSize, true, false, TestCoreFreeFunction, NULL, TestCoreExtendFunction, NULL);

					// Call our callbacks from now on to extend memory. Currently GeneralAllocator uses 
					// kOptionEnableSystemAlloc to control both the allocation of core and the extension 
					// of core memory. We probably should change that so they are controlled independently.
					pGeneralAllocator->SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 1);

					// Disable mapped memory. We want all allocations to come from core memory.
					pGeneralAllocator->SetOption(GeneralAllocator::kOptionMMapThreshold, 0xffffffff);
				}
			}
		}

		return bReturnValue;
	}
#endif

int TestExtendCore()
{
	EA::UnitTest::Report("    TestExtendCore\n");

	int nErrorCount = 0;
	
#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	{
		// We test the ExtendCoreCallback via VirtualAlloc.
		{
			GeneralAllocator generalAllocator(NULL, 0, false, false, TestCoreFreeFunction, NULL, TestCoreExtendFunction, NULL);
			generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0); // Make it so our TestMallocFailureFunction is always called when out of core memory.
			generalAllocator.SetMallocFailureFunction(TestMallocFailureFunction, NULL);

			for(size_t n = 0; n < kAllocationVolume; n += kAllocationSize)
			{
				void* p = generalAllocator.Malloc(kAllocationSize);
				EATEST_VERIFY(p != NULL);
			}

			// Verify that there is exactly one core block of kBlockSize bytes.
			const void* pReport = generalAllocator.ReportBegin(NULL, GeneralAllocator::kBlockTypeCore);
			EATEST_VERIFY(pReport != NULL);

			if(pReport)
			{
				const GeneralAllocator::BlockInfo* pBlockInfo = generalAllocator.ReportNext(pReport, GeneralAllocator::kBlockTypeCore);
				EATEST_VERIFY(pBlockInfo != NULL);

				if(pBlockInfo)
				{
					pBlockInfo = generalAllocator.ReportNext(pReport, GeneralAllocator::kBlockTypeCore);
					EATEST_VERIFY((pBlockInfo == NULL) || (pBlockInfo->mpCore == NULL)); // Verify there are no more allocated core blocks.
				}

				generalAllocator.ReportEnd(pReport);
			}

			// We just let the allocator 'leak' the memory. It will be cleaned up in our TestCoreFreeFunction.
		}

		// This it outside of the above block because we want ~GeneralAllocator to have executed and free the core.
		EATEST_VERIFY((nFreeCallCount == 1) && (nExtendCallCount > 0) && (nMallocCallCount == 1));
		nFreeCallCount   = 0;
		nExtendCallCount = 0;
		nMallocCallCount = 0;
	}
#endif

	// Basic test to ensure when a core is added that the allocation requested is aligned to the system's Page Size
	{
		static int sExtendCoreTestNErrorCount = 0;
		auto LocalMallocFailureFunction = [](GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* /*pContext*/)
		{

			unsigned int pageSize = GetPageSize();
			if (nAllocatorRequestedSize % pageSize)
			{
				// The allocator will always need a core size in PageSize increments
				sExtendCoreTestNErrorCount++;
			}

			return false;
		};

		// Create an allocator with no core memory so allocating anything will force a core to be added in our failure function
		GeneralAllocator generalAllocator(nullptr, 0, false, false, nullptr, nullptr, nullptr, nullptr);
		generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0); // Make it so our TestMallocFailureFunction is always called when out of core memory.
		generalAllocator.SetMallocFailureFunction(LocalMallocFailureFunction, NULL);

		void* p = generalAllocator.Malloc(1);
		EATEST_VERIFY(p == nullptr);
		EATEST_VERIFY(sExtendCoreTestNErrorCount == 0);
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestDebugDataLocationSeparate
//
int TestDebugDataLocationSeparate()
{
	EA::UnitTest::Report("    TestDebugDataLocationSeparate\n");

	int nErrorCount = 0;

	// We create a Debug allocator to hold DataLocationSeparate info from the regular allocator.
	GeneralAllocatorDebug gaDebug; // Declare this first so it gets destroyed last.
	GeneralAllocatorDebug ga1;
	GeneralAllocatorDebug ga2;

	ga1.SetAllocator(&gaDebug);
	ga2.SetAllocator(&gaDebug);

	ga1.SetDefaultDebugDataLocation(GeneralAllocatorDebug::kDebugDataLocationSeparate);
	ga2.SetDefaultDebugDataLocation(GeneralAllocatorDebug::kDebugDataLocationSeparate);

	ga1.SetOption(GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);
	ga2.SetOption(GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);

	ga1.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);
	ga2.SetOption(GeneralAllocator::kOptionNewCoreSize, (int)gnNewCoreSize);

	ga1.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	ga2.SetAutoHeapValidation((GeneralAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

	gRand.SetSeed(EA::UnitTest::GetRandSeed());

	nErrorCount += TestAllocationGeneric(&ga1, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount);
	nErrorCount += TestAllocationGeneric(&ga2, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount);

	return nErrorCount;
}



#include <EASTL/map.h>
#include <stdio.h>
struct Allocation { void* ptr; unsigned size; };

const char* pAllocationRecording = 
{
	"A 173539376 4424 16 \n"        // The spaces before \n chars are significant, as they help us parse below.
	"A 193527824 1048576 16 \n"
	"A 173543824 20 16 \n"
	"A 173543856 4424 16 \n"
	"A 173548304 4424 16 \n"
	"A 173552752 6312 16 \n"
	"A 173559088 2376 16 \n"
	"A 173561488 2376 16 \n"
	"A 173563888 2376 16 \n"
	"A 173566288 2376 16 \n"
	"A 198705168 65864 16 \n"
	"A 173568688 1352 16 \n"
	"A 173570064 1352 16 \n"
	"A 173571440 1352 16 \n"
	"A 173572816 1352 16 \n"
	"A 173574192 4424 16 \n"
	"A 173578640 4424 16 \n"
	"A 173583088 4424 16 \n"
	"A 173587536 4424 16 \n"
	"A 173591984 8520 16 \n"
	"A 175374384 8520 16 \n"
	"A 175382928 8520 16 \n"
	"A 175391472 8520 16 \n"
	"A 175400016 12616 16 \n"
	"A 175412656 12616 16 \n"
	"A 175425296 6504 16 \n"
	"A 175431824 6504 16 \n"
	"A 175439920 33096 16 \n"
	"A 175438352 60 16 \n"
	"A 175438432 20 16 \n"
	"A 175438464 60 16 \n"
	"A 175438544 20 16 \n"
	"A 175473040 16744 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 175489808 13304 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 726794288 14296 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 726808608 24632 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 726833264 25368 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 726990896 25016 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727015936 15896 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727031856 15448 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727056432 15720 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727072176 27912 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727100112 10616 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 731840560 13464 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727110752 9272 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 736296976 78920 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 731906096 62712 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 731854048 11832 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 731865904 15416 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 731881344 15528 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 736165936 25368 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 727047328 8520 16 \n"
	"A 731896896 8520 16 \n"
	"A 736191328 8520 16 \n"
	"A 736199872 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 736208416 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673251376 49848 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673316912 50552 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673382448 51016 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673447984 59784 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673513520 60408 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673579056 59128 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673433488 12840 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 736216960 12920 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673367488 12872 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673644592 21160 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673665776 21624 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673687424 21432 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673710128 28968 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673739120 29176 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673775664 28696 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673301248 13880 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673804384 14008 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673818416 14104 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673841200 29592 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673870816 29976 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673906736 29864 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673832544 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673936624 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673945168 23288 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673972272 23016 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673995312 22760 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674496560 63496 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674693136 66552 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674562096 65176 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674627632 25688 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674653344 25880 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674824240 25496 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674849760 34488 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674679248 9624 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674037808 25848 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674063680 26344 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674103344 26104 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674129472 31352 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674090048 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674018096 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674026640 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674168880 15720 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674184624 9528 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674194176 9608 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674098592 4424 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673573952 4424 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674884272 4424 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 673507792 4424 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674203808 10424 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674214256 10776 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674234416 10040 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674244480 19368 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674299952 52696 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674263872 21112 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674285008 14136 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674225056 8616 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674365488 27176 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674392688 19400 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674412112 17784 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679084048 86040 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674352672 8520 16 \n"
	"A 674431024 8520 16 \n"
	"A 674439568 8520 16 \n"
	"A 674448112 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674456656 21928 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 678953008 25112 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 678978144 24920 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679018544 25256 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679003088 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679346192 83432 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674478608 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674487152 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679043824 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679052368 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679060912 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679069456 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679215152 42616 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679257792 18152 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679280688 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679289232 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679297776 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679306320 13960 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679608336 89880 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679739408 67448 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679870480 114488 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680001552 88888 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680132624 68920 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674889776 53832 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674955312 50728 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 674943632 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675020848 31064 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675006064 8520 16 \n"
	"A 679320304 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679328848 10808 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675051936 14712 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675066672 9912 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679477296 15864 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679493184 12744 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675076608 9432 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680394768 68744 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680263728 38408 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680787984 89048 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680919056 115864 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679505952 9880 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679515856 10824 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680329264 31096 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680525872 36216 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680302160 18152 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680591408 35464 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680562112 17336 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680360384 18792 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680626896 26248 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681246736 171992 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681050160 44696 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680320336 8520 16 \n"
	"A 680579472 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680379200 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681115696 24680 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681181232 48712 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681443376 42376 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679526704 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681229968 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681094880 16024 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681140400 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681148944 10888 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681159856 11304 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681171184 9480 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681485776 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681771024 200328 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681639952 76872 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682295312 232664 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681508912 23992 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681532928 23960 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681494320 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681574448 31096 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681605568 30952 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682033200 31272 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682064496 30680 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 681556912 13560 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682098736 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682107280 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682115824 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682164272 60680 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682124368 8840 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682133232 27496 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682754064 167384 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675217424 70472 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675086384 18888 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682950672 73768 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675105296 11272 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675116592 8520 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675125136 9304 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675151920 61176 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679542832 18328 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679561184 21992 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 683474960 91608 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680722480 54792 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 696778768 456648 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 697237520 367624 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 697630736 337624 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 683606032 110648 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 698023952 232408 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682229808 65464 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 675134464 13032 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 698286096 337848 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 696320016 168680 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 698679312 210280 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 696516624 87736 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 679583200 18232 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682229808 20504 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 680777296 8824 16 \n"
	"F 175438544 \n"
	"A 175438544 20 16 \n"
	"A 682250336 9032 16 \n"
	"EOF"
};



///////////////////////////////////////////////////////////////////////////////
// TestRegressions
//
int TestRegressions()
{
	EA::UnitTest::Report("    TestRegressions\n");

	int nErrorCount = 0;
	int nRegressionTest = 0;

	#if (EATEST_VERSION_N > 20400)
		if(EA::UnitTest::GetTestLevel())
			return 0;
	#elif (PPM_DEBUG < 2)
		if(pAllocationRecording[0] == 123) // This is an arbitrary expression which is always true in practice. Used to avoid compiler warnings that would occur if we just returned 0 here unilaterally.
			return 0;
	#endif

	{
		// Bioware/Mass Effect report that Unix64 skip lists can be trashed during TrimCore.
		// In particular, when core fails to free, the skip list was getting trashed because
		// the a block was removed from it but not properly replaced upon a failed trim.
		// Our solution is to not do the remove then replace on failure, but rather to not
		// remove the block until it already has been sucessfully freed.
		// The following test won't really be exercizing the regression on Windows because 
		// GeneralAllocator on Windows does core extending and shrinking (because it can),
		// whereas other platforms just add new core or completely free existing core.
		// And this latter behavior is what we need to test.

		EA::UnitTest::Report("        Regression Test %d\n", ++nRegressionTest);
		EA::Allocator::GeneralAllocator allocator;

		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 131072);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionTrimThreshold, 131072 / 2);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapThreshold, 131072 * 64);  //Something high, so we don't alloc mmap blocks instead of regular blocks.        
		allocator.Init(0, 131072 - 2048, false);  //This will translate to a 131072 initial core block

		eastl::vector<void*> blockArray;

		for(int n = 0; n < 16; n++)
		{
			size_t nSize = (size_t)gRand.RandRange(16384-512, 16384+512);
			blockArray.push_back(allocator.Malloc(nSize));
		}

		for(size_t i = 0; i < 1000000; i++)
		{
			for(int n = 0; n < 12; n++)
			{
				uint32_t index = gRand.RandLimit((uint32_t)blockArray.size());
				allocator.Free(blockArray[index]);
				blockArray.erase(blockArray.begin() + index);
			}

			for(int n = 0; n < 12; n++)
			{
				size_t nSize = (size_t)gRand.RandRange(16384-512, 16384+512);
				blockArray.push_back(allocator.Malloc(nSize));
			}

			if (((i + 1) % 100000) == 0)
				EA::UnitTest::ReportVerbosity(1, "            Iteration %d complete\n", i + 1);
		}
	}

	{
		EA::UnitTest::Report("        Regression Test %d\n", ++nRegressionTest);
		EA::Allocator::GeneralAllocatorDebug allocator;

		allocator.SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull, 0);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapThreshold, 65536);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 65536);
		allocator.SetOption(EA::Allocator::GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);
		allocator.SetGuardSize(0, 0, 0); // Disable guard fills.
		allocator.Init(0, size_t(0), true);

		eastl::string sRecording(pAllocationRecording);
		eastl_size_t pos = 0;

		typedef eastl::map<unsigned, Allocation> AddressToAllocation;
		AddressToAllocation addressToAllocation;

		for(int i = 0; ; i++) // Provide the counter in order do aid debugging.
		{
			char     type;
			unsigned address;

			if(sscanf(sRecording.data() + pos, "%c %u", &type, &address) != 2)
				break;
			pos = sRecording.find(" ", pos) + 1;
			pos = sRecording.find(" ", pos) + 1;

			if(i == 611) // This was useful for debugging.
			{
				if(EA::UnitTest::GetVerbosity() >= 1)
					allocator.TraceAllocatedMemory();
			}

			if(type == 'A')
			{
				unsigned int size;
				unsigned int alignment;
				sscanf(sRecording.data() + pos, "%u %u", &size, &alignment);
				//EA::UnitTest::ReportVerbosity(1, "A %u %u %u\n", address, size, alignment);

				void* ptr = allocator.MallocAligned(size, alignment);
				if((intptr_t)ptr == 0x02300030)
					type++;
				memset(ptr, 0x11, size);
				Allocation allocation = { ptr, size };
				addressToAllocation[address] = allocation;

				if(sscanf(sRecording.data() + pos, "\n") != 0)
					break;
				pos = sRecording.find("\n", pos) + 1;
			}
			else
			{
				//EA::UnitTest::ReportVerbosity(1, "F %u\n", address);
				AddressToAllocation::iterator it = addressToAllocation.find(address);
				allocator.Free(it->second.ptr);
				addressToAllocation.erase(it);
				if(sscanf(sRecording.data() + pos, "\n") != 0)
					break;
				pos = sRecording.find("\n", pos) + 1;
			}
		}

		/* For debugging:
		for(AddressToAllocation::iterator i = addressToAllocation.begin(), e = addressToAllocation.end(); i != e; ++i)
		{
			unsigned int   size = i->second.size;
			unsigned char* ptr  = (unsigned char*)i->second.ptr;

			for(unsigned int j = 0; j < size; ++j)
			{
				if(ptr[j])
					EA::UnitTest::ReportVerbosity(1, "%s", "");
			}
		}
		*/
	}


	{
		EA::UnitTest::Report("        Regression Test %d\n", ++nRegressionTest);
		// Try to programmatically replicate the problem DICE has been having.
		for(int n = 0; n < 3 /* or higher value */; n++)
		{
			EA::Allocator::GeneralAllocator allocator;

			allocator.SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelFull, 0);
			allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapThreshold, 65536);
			allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 65536);
			//allocator.SetOption(EA::Allocator::GeneralAllocatorDebug::kOptionEnablePtrValidation, 1);
			//allocator.SetGuardSize(0, 0, 0); // Disable guard fills.
			allocator.Init(0, size_t(0), true);

			nErrorCount += TestAllocationGeneric(&allocator, 256, 65536 + 1024, 100/* or higher value */, 400/* or higher value */, 2);
		}
	}

	{
		EA::UnitTest::Report("        Regression Test %d\n", ++nRegressionTest);
		// "When we allocate with the size of 65280 and align 128 we get an assert."
		if(EA::UnitTest::GetSystemMemoryMB() >= 2048) // If the system has a lot of memory...
		{
			EA::Allocator::GeneralAllocator allocator;

			allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapThreshold, 65536);
			allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 65536);
			allocator.Init(0, 0, true);

			nErrorCount += TestAllocationAligned(&allocator, 65276, 65284, 128, 0, 100, 200, 2);
		}
	}

	{
		EA::UnitTest::Report("        Regression Test %d\n", ++nRegressionTest);
		// DICE/Battlefield report of XBox 360 GeneralAllocator crash with the following data.
		#if defined(EA_PLATFORM_WINDOWS)
				FILE* input = fopen("C:\\projects\\EAOS\\UTF\\DL\\UTFKernel\\PPMalloc\\dev\\test\\source\\data\\Recording1.txt", "r");

			if(input)
			{
				EA::Allocator::GeneralAllocator allocator;

				allocator.Init(0, 32*1024*1024, false);
				allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapThreshold, 1024*1024);
				allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionMMapPageSize, 64*1024);
				allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 64*1024);

				typedef eastl::map<uint32_t, void*> State;
				State state;
				char  buffer[256];
				int   i = 1;

				while(fgets(buffer, sizeof buffer, input))
				{
					uint32_t addr, size, align;

					switch (buffer[0])
					{
						case 'A':
						{
							sscanf(buffer + 2, "%u %u %08x", &size, &align, &addr);

							EATEST_VERIFY(state.find(addr) == state.end());
							state[addr] = allocator.MallocAligned(size, align);
							break;
						}

						case 'F':
						{
							sscanf(buffer + 2, "%08x", &addr);

							State::iterator it = state.find(addr);
							EATEST_VERIFY(it != state.end());
							if(it != state.end())
							{
								allocator.Free(it->second);
								state.erase(it);
							}
							break;
						}

						default:
							// Ignore the character.
							break;
					}

					if((i % 1024) || (i >= 27200)) // The crash was happening when i was 27205. Just the right block size to trigger a miscalculation crash.
						EATEST_VERIFY_F(allocator.ValidateHeap(GeneralAllocator::kHeapValidationLevelFull), "Heap validation failed for recording line %d: %s", i, buffer);

					i++;
				}

				fclose(input);
			}
		#endif 
	}

	return nErrorCount;
}



struct CoreAddFunctionTestInfo
{
	int                          mCoreAddCount;  // Number of times CoreAddFunction was called.
	int                          mCoreFreeCount; // Number of times CoreFreeFunction was called.
	eastl::fixed_set<void*, 128> mCoreMemorySet; // All pointers returned by CoreAddFunction. Each should be freed by CoreFreeFunction when done.
	int                          mErrorCount;

	CoreAddFunctionTestInfo() : mCoreAddCount(0), mCoreFreeCount(0), mCoreMemorySet(), mErrorCount(0) {}
};


static size_t CoreFreeFunction(GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, void* pContext)
{
	CoreAddFunctionTestInfo* pCoreAddFunctionTestInfo = (CoreAddFunctionTestInfo*)pContext;

	int& nErrorCount = pCoreAddFunctionTestInfo->mErrorCount; // Declared so that EATEST_VERIFY can reference 'nErrorCount'.

	if(pCoreAddFunctionTestInfo->mCoreMemorySet.find(pCore) != pCoreAddFunctionTestInfo->mCoreMemorySet.end())
	{
		pCoreAddFunctionTestInfo->mCoreFreeCount++;
		free(pCore);
		return nSize;
	}
	else
	{
		// Force an error. We test for pCore == NULL instead of passing false, as some compilers might warn about that.
		EATEST_VERIFY_MSG(pCore == NULL, "TestSetAddCoreFunction/CoreFreeFunction: Invalid pCore."); // pCore should never
	}

	return 0;
}


static void* CoreAddFunction(size_t* pMinSize, size_t* pReservedSize, EA::Allocator::GeneralAllocator::CoreFreeFunction* ppCoreFreeFunction, void** ppCoreFreeFunctionContext, 
							 EA::Allocator::GeneralAllocator::CoreExtendFunction* ppCoreExtendFunction, void** ppCoreExtendFunctionContext, uint32_t& coreType, void* pContext)
{
	void* pMemory = malloc(*pMinSize);

	if(pMemory)
	{
		CoreAddFunctionTestInfo* pCoreAddFunctionTestInfo = (CoreAddFunctionTestInfo*)pContext;
		pCoreAddFunctionTestInfo->mCoreAddCount++;
		pCoreAddFunctionTestInfo->mCoreMemorySet.insert(pMemory);

		*pReservedSize = *pMinSize;

		*ppCoreFreeFunction        = CoreFreeFunction;
		*ppCoreFreeFunctionContext = pContext;

		*ppCoreExtendFunction        = NULL;
		*ppCoreExtendFunctionContext = NULL;

		coreType = EA::Allocator::GeneralAllocator::kCoreTypeUserDefined + 1234; // We don't use kCoreTypeMalloc because we are testing this user-defined case.
	}

	return pMemory;
}


static int TestSetAddCoreFunction()
{
	int nErrorCount = 0;

	EA::UnitTest::Report("    TestSetAddCoreFunction\n");

	{
		EA::Allocator::GeneralAllocator allocator;

		allocator.Init(0, size_t(0), true);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 1024*1024);

		CoreAddFunctionTestInfo info;
		allocator.SetAddCoreFunction(CoreAddFunction, &info);

		nErrorCount += TestAllocationGeneric(&allocator, 256, 65536 + 1024, 500, 4000, 1); // If core size is 1024*1024, then this results in at least 10 core blocks needed.

		allocator.Shutdown();

		EATEST_VERIFY(info.mCoreAddCount == info.mCoreAddCount);
		nErrorCount += info.mErrorCount;
	}

	return nErrorCount;    
}


int TestFootprints()
{
	EA::UnitTest::Report("    TestFootprints\n");

	int nErrorCount = 0;

	// Helps with printing and testing for the code below
	auto VerifyFootprintsHelper = [&nErrorCount](int line, const GeneralAllocator& ga, size_t expectedPhysicalFootprint, size_t expectedVirtualFootprint, size_t expectedNumCores)
	{
		const size_t physicalFootprint = ga.PhysicalFootprint();
		const size_t virtualFootprint = ga.VirtualFootprint();
		const size_t numCores = ga.NumCores();

		EA::UnitTest::TestInternal::EATEST_VERIFY_F_IMP(expectedPhysicalFootprint == physicalFootprint, nErrorCount, __FILE__, line, "Physical Footprint Mismatch: Expected(%" PRIu64 ") == Actual(%" PRIu64 ")", expectedPhysicalFootprint, physicalFootprint);
		EA::UnitTest::TestInternal::EATEST_VERIFY_F_IMP(expectedVirtualFootprint == virtualFootprint, nErrorCount, __FILE__, line, "Virtual Footprint Mismatch: Expected(%" PRIu64 ") == Actual(%" PRIu64 ")", expectedVirtualFootprint, virtualFootprint);
		EA::UnitTest::TestInternal::EATEST_VERIFY_F_IMP(expectedNumCores == numCores, nErrorCount, __FILE__, line, "Num Cores Mismatch: Expected(%" PRIu64 ") == Actual(%" PRIu64 ")", expectedNumCores, numCores);
	};
	#define VerifyFootprints(ga, expectedPhysicalFootprint, expectedVirtualFootprint, expectedNumCores) VerifyFootprintsHelper(__LINE__, ga, expectedPhysicalFootprint, expectedVirtualFootprint, expectedNumCores)


	// Sanity check / Base Case: An allocator with no core and no allocations are made
	{
		// Create an allocator with no core memory
		GeneralAllocator generalAllocator(nullptr, 0, false, false, nullptr, nullptr, nullptr, nullptr);
		VerifyFootprints(generalAllocator, 0, 0, 0);
	}

	// Test allocating memory
	{
		const size_t kCoreSize = (1 * 1024 * 1024);
		const size_t kAllocSize = 37;
		void* pCore = malloc(kCoreSize);

		// Scoping is so that the general allocator will have shutdown before we free the core
		{
			GeneralAllocator generalAllocator(pCore, kCoreSize, false);
			generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 0);
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);

			void* p0 = generalAllocator.Malloc(kAllocSize);
			EATEST_VERIFY(p0 != nullptr);
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);

			generalAllocator.Free(p0);
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);
		}

		free(pCore);
	}

	// Test trimming cores and verifying the physical footprint metric is updated.
	{
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		{
			const size_t kCoreSize = (1 * 1024 * 1024);
			const size_t kCoreSizeLarge = kCoreSize * 8;
			const bool bShouldFreeCore = true;
			const bool bShouldTrimCore = true;

			void* pCore1 = VirtualAlloc(0, kCoreSizeLarge, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			GeneralAllocatorDebug generalAllocator;
			generalAllocator.SetOption(GeneralAllocatorDebug::kOptionEnableSystemAlloc, 0);
			generalAllocator.AddCoreEx(pCore1, kCoreSizeLarge, GeneralAllocator::kCoreTypeMMap, 0, bShouldFreeCore, bShouldTrimCore, NULL, NULL, NULL, NULL);
			void* pCore2 = generalAllocator.Malloc(kCoreSize);
			void* pCore3 = generalAllocator.Malloc(kCoreSize);
			EA_UNUSED(pCore3);
			generalAllocator.Free(pCore2);

			const size_t physical_footprint1 = generalAllocator.PhysicalFootprint();
			{
				size_t amountTrimmed = generalAllocator.TrimCore(0);
				EATEST_VERIFY_MSG(amountTrimmed > kCoreSize, "We are expecting to have trimmed the core.");
			}
			const size_t physical_footprint2 = generalAllocator.PhysicalFootprint();
			EATEST_VERIFY_MSG(physical_footprint1 > physical_footprint2, "We trimmed the core so the physical footprint should have decreased.");

			generalAllocator.Shutdown();
		}
		#endif

		// Test trim empty cores works correctly - an allocator should be able to have 0 cores once trimmed with the option set
		{
			const size_t kCoreSize = (4 * 1024 * 1024);
			const size_t kAllocSize = (64 * 1024);

			GeneralAllocator generalAllocator(nullptr, 0, false);
			generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 1);
			generalAllocator.SetOption(GeneralAllocator::kOptionMMapMaxAllowed, 0);
			generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, kCoreSize);
			generalAllocator.SetOption(GeneralAllocator::kOptionEnableTrimToZero, 1);
			VerifyFootprints(generalAllocator, 0, 0, 0);

			// Allocate some memory which will force the system to allocate a new core
			void* p0 = generalAllocator.Malloc(kAllocSize);
			EATEST_VERIFY(p0 != nullptr);
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);

			// free our physical allocation. Normally this core would linger around, but we have explicitly set all empty cores
			// to be freed.
			generalAllocator.Free(p0);
			generalAllocator.TrimCore(0);
			VerifyFootprints(generalAllocator, 0, 0, 0);
		}
	}

	
	// Test adding/freeing a cores
	{
		// Helper class to get us the size of protected constants and members in the general allocator
		// so we can avoid magic numbers in some of the test below
		class AllocatorInspector : public GeneralAllocator
		{
		public:
			static size_t GetAllocationOverheadSize() { return sizeof(CoreBlock) + kDoubleFenceChunkSize; }
		};

		struct TestContext
		{
			GeneralAllocator::CoreFreeFunction pFreeCoreFunction;
			GeneralAllocator::CoreAddFunction pAddCoreFunction;
		};

		auto LocalFreeCoreFunction = [](GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, void* pContext)
		{
			free(pCore);
			return nSize;
		};

		auto LocalAddCoreFunction = [](size_t* pMinSize, size_t* pReservedSize,
			GeneralAllocator::CoreFreeFunction* ppCoreFreeFunction, void** ppCoreFreeFunctionContext,
			GeneralAllocator::CoreExtendFunction* ppCoreExtendFunction, void** ppCoreExtendFunctionContext,
			uint32_t& coreType, void* pContext)
		{
			TestContext* pFuncs = (TestContext*)pContext;
			*ppCoreFreeFunction = pFuncs->pFreeCoreFunction;

			return malloc(*pReservedSize);
		};

		TestContext testContext;
		testContext.pAddCoreFunction = LocalAddCoreFunction;
		testContext.pFreeCoreFunction = LocalFreeCoreFunction;

		const size_t kCoreSize = (1 * 1024 * 1024);

		// Scoping is so that the general allocator will have shutdown before we free the core
		{
			GeneralAllocator generalAllocator(nullptr, 0, false);
			generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, kCoreSize);
			generalAllocator.SetOption(GeneralAllocator::kOptionMMapMaxAllowed, 0);
			generalAllocator.SetAddCoreFunction(LocalAddCoreFunction, &testContext);

			VerifyFootprints(generalAllocator, 0, 0, 0);

			// required for the next two VerifyFootprints() calls nested below 
			EATEST_VERIFY(kCoreSize > 128*1024);
			{
				// We expect that since there is no core memory already reserved, any malloc
				// will force a new core to be added thus increasing the physical footprint
				void* p0 = generalAllocator.Malloc(64*1024);
				EATEST_VERIFY(p0 != nullptr);
				VerifyFootprints(generalAllocator, kCoreSize, 0, 1);

				// Since we have added a core above that covers both of these small allocs
				// we don't expect any change in our physical footprint
				void* p1 = generalAllocator.Malloc(64 * 1024);
				EATEST_VERIFY(p1 != nullptr);
				VerifyFootprints(generalAllocator, kCoreSize, 0, 1);

				generalAllocator.Free(p1);
				generalAllocator.Free(p0);

				// We have freed all the alloc'd memory but one core is kept so 
				// nothing should have changed here either
				VerifyFootprints(generalAllocator, kCoreSize, 0, 1);
			}

			// We should have no memory allocated currently so the following should be true.
			// We need to be certain of this otherwise the next test will fail since it will 
			// allocate too many cores.
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);
			{
				// Allocate enough memory to force two additional cores to be added
				// We do some arithmetic to ensure that any internal tracking overhead 
				// is accounted for so we don't accidentally allocate /more/ than two cores
				// which would throw off this test
				size_t pageSize = GetPageSize(); 
				size_t largeAllocSize = EA::StdC::AlignDown<size_t>(kCoreSize*2 - AllocatorInspector::GetAllocationOverheadSize(), pageSize);

				void* p = generalAllocator.Malloc(largeAllocSize);
				EATEST_VERIFY(p != nullptr);
				VerifyFootprints(generalAllocator, kCoreSize * 3, 0, 2);

				generalAllocator.Free(p);
				VerifyFootprints(generalAllocator, kCoreSize, 0, 1);
			}
		}
	}

	// Test adding/freeing Virtual Allocations
	{
		const size_t kCoreSize = (1 * 1024 * 1024);
		const size_t kMMapPageSize = GetPageSize();
		const size_t kAllocSize = 64;

		// Scoping is so that the general allocator will have shutdown before we free the core
		{
			GeneralAllocator generalAllocator(nullptr, 0, false);
			generalAllocator.SetOption(GeneralAllocator::kOptionEnableSystemAlloc, 1);
			generalAllocator.SetOption(GeneralAllocator::kOptionMMapMaxAllowed, 1);
			generalAllocator.SetOption(GeneralAllocator::kOptionMMapThreshold, kMMapPageSize);
			generalAllocator.SetOption(GeneralAllocator::kOptionMMapPageSize, kMMapPageSize);
			generalAllocator.SetOption(GeneralAllocator::kOptionNewCoreSize, kCoreSize);
			VerifyFootprints(generalAllocator, 0, 0, 0);

			// We have no core allocated 
			void* p0 = generalAllocator.Malloc(kAllocSize, GeneralAllocator::kAllocationFlagMMap);
			EATEST_VERIFY(p0 != nullptr);
			VerifyFootprints(generalAllocator, 0, kMMapPageSize, 0);

			// We are only allowed one virtual allocation (from the settings above)
			// so the next allocation should add a core via system calls and not affect the
			// virtual footprint
			void* p1 = generalAllocator.Malloc(kAllocSize);
			EATEST_VERIFY(p1 != nullptr);
			VerifyFootprints(generalAllocator, kCoreSize, kMMapPageSize, 1);

			// free our physical allocation (this is our only core so the core will remain)
			generalAllocator.Free(p1);
			VerifyFootprints(generalAllocator, kCoreSize, kMMapPageSize, 1);

			// free virtual memory
			generalAllocator.Free(p0);
			VerifyFootprints(generalAllocator, kCoreSize, 0, 1);
		}
	}

	return nErrorCount;
}


static int TestSetMemoryRangeCallbackFunction()
{
	int nErrorCount = 0;

	EA::UnitTest::Report("    SetMemoryRangeCallbackFunction\n");

	struct CallbackUserData
	{
		eastl::map<void*, size_t> addressMap;
		bool IsValid() { return addressMap.size() == 0; }

	} callbackUserData;

	auto memoryRangeCallback = [](void* userData, void* address, size_t size, bool allocated)
	{
		auto* pCallbackUserData = (CallbackUserData*)userData;
		auto& addressMap = pCallbackUserData->addressMap;

		if (allocated)
		{
			addressMap[address] = size;
		}
		else
		{
			if (addressMap.find(address) != addressMap.end())
				addressMap.erase(address);
		}
	};

	{
		EA::Allocator::GeneralAllocator allocator;

		allocator.Init(0, size_t(0), true);
		allocator.SetOption(EA::Allocator::GeneralAllocator::kOptionNewCoreSize, 1024 * 1024);

		allocator.SetMemoryRangeCallbackFunction(memoryRangeCallback, &callbackUserData);

		nErrorCount += TestAllocationGeneric(&allocator, 256, 65536 + 1024, 500, 4000, 1); // If core size is 1024*1024, then this results in at least 10 core blocks needed.

		allocator.Shutdown();
	}

	EATEST_VERIFY(callbackUserData.IsValid());

	return nErrorCount;
}





#ifdef EA_PLATFORM_SONY
size_t sceLibcHeapSize = 64*1024*1024; 
#endif

// TestGeneralAllocator
//
// Useful command line arguments relevant to this test:
//    To test core blocks: -Min 5000 -Max 20000 -SlotCount 10000 -IterationCount 30000 -CycleCount 10 -NewCoreSize 65536
//
int TestGeneralAllocator()
{
	int nErrorCount = 0;

	nErrorCount += TestSetMemoryRangeCallbackFunction();
	nErrorCount += TestSetAddCoreFunction();
	nErrorCount += TestDisableSystemAlloc();
	nErrorCount += Test64Bit();
	nErrorCount += TestLargeAllocations();
	nErrorCount += TestLargePages();
	nErrorCount += TestRegressions();
	nErrorCount += TestConstruction();
	nErrorCount += TestBestFit();
	nErrorCount += TestShared();
	nErrorCount += TestAsInterface();
	nErrorCount += TestValidateAddress();
	nErrorCount += TestInitNotification();
	nErrorCount += TestReportHeap();
	nErrorCount += TestSubHeap();
	nErrorCount += TestDebugDataLocationSeparate();
	nErrorCount += TestAllocation();
	nErrorCount += TestAllocationFlags();
	nErrorCount += TestThreading();
	nErrorCount += TestDebugNaming();
	nErrorCount += TestDebugGroupId();
	nErrorCount += TestDelayedFree();
	nErrorCount += TestExtendCore();
	nErrorCount += TestFootprints();

	return nErrorCount;
}


