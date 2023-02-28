///////////////////////////////////////////////////////////////////////////////
// TestSmallObjectAllocator
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EASmallObjectAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/internal/shared.h>
#include <string.h>


using namespace EA::Allocator;
using namespace EA::UnitTest;


namespace EA
{
	namespace Allocator
	{
		extern PPM_API GeneralAllocator* gpEAGeneralAllocator;
	}
}


enum Operation
{
	kOperationMalloc,
	kOperationMallocAligned,
	kOperationRealloc,
	kOperationCount
};


namespace TestSOA
{
	uint8_t gFill[kOperationCount]      = { 0xe0, 0xe1, 0xe2 };

	size_t  kDefaultAllocSizeMin        =     0;
	size_t  kDefaultAllocSizeMax        =     0;  // 0 means user allocator max.
	size_t  kDefaultAllocSlotCount      = 10000;
	size_t  kDefaultAllocIterationCount = 20000;
	size_t  kDefaultAllocCycleCount     =     2;

	size_t  kCoreBlockPageSize          = 4096;   // Used for page size optimizations.
}

using namespace TestSOA;


// Helper functions
bool   SOInit(SmallObjectAllocator* pSOA, SmallObjectAllocator::Parameters* pParameters = NULL);
void   SOAssertionFailureInfoFunction(SmallObjectAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
void*  SOCoreAllocationFunction(SmallObjectAllocator* pAllocator, size_t nSize, size_t nAlignment, size_t nAlignmentOffset, void* pContext);
void*  SOCoreAllocationFunctionPaged(SmallObjectAllocator* pAllocator, size_t nSize, size_t nAlignment, size_t nAlignmentOffset, void* pContext);
void   SOCoreFreeFunction(SmallObjectAllocator* pAllocator, void* pCoreBlock, void* pContext);
int    SOTestAllocationGeneric(SmallObjectAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount);
int    SOTestAllocationPathological(SmallObjectAllocator* pAllocator);

// High level test functions
int    SOTestConstruction();
int    SOTestValidateAddress();
int    SOTestAllocation();
int    SOTestPathological();
int    SOTestReportHeap();



bool SOInit(SmallObjectAllocator* pSOA, SmallObjectAllocator::Parameters* pParameters)
{
	bool bReturnValue = true;

	pSOA->SetCoreAllocationFunction(SOCoreAllocationFunction, SOCoreFreeFunction, gpEAGeneralAllocator);
	pSOA->SetAssertionFailureFunction(SOAssertionFailureInfoFunction, pSOA);

	if(pParameters)
		pSOA->Init(*pParameters);
	else
		pSOA->Init(SmallObjectAllocator::Parameters());

	return bReturnValue;
}


void SOAssertionFailureInfoFunction(SmallObjectAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault("SOAssertionFailureInfoFunction:\n        ", pContext);
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);

	if(EA::UnitTest::GetInteractive())
		 EATEST_DEBUG_BREAK();
}


void* SOCoreAllocationFunction(SmallObjectAllocator* /*pAllocator*/, size_t nSize, 
								size_t nAlignment, size_t nAlignmentOffset, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	return pGeneralAllocator->MallocAligned(nSize, nAlignment, nAlignmentOffset);
}


void* SOCoreAllocationFunctionPaged(SmallObjectAllocator* pAllocator, size_t nSize, 
									size_t nAlignment, size_t nAlignmentOffset, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;

	nAlignment = PPM_AlignUp(nSize, 4096);

	// We intentionally ignore alignment offset. The allocator will deal with it if the alignment offset turns out to mismatch nAlignmentOffset.
	return pGeneralAllocator->MallocAligned(nSize, nAlignment, 0);
}


void SOCoreFreeFunction(SmallObjectAllocator* /*pAllocator*/, void* pCoreBlock, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	pGeneralAllocator->Free(pCoreBlock);
}


int SOTestAllocationGeneric(SmallObjectAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, 
							 size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount)
{
	size_t i;

	int nErrorCount = 0;

	if(nAllocSizeMax == 0)
	{
		SmallObjectAllocator::Parameters parameters;
		pAllocator->GetParameters(parameters);
		nAllocSizeMax = parameters.mnMaxMallocSize + 1; // +1 because nAllocSizeMax refers to one-past the largest malloc size.
	}

	MallocInfo* pMallocInfoArray = new MallocInfo[nAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * nAllocSlotCount);

	// Run tests
	for(size_t c(0); c < nAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < nAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, nAllocIterationCount, c, nAllocCycleCount);

			i = gRand.RandRange(0, (int)nAllocSlotCount);

			const size_t n = gRand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);

			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
			else
			{
				pMallocInfoArray[i].mSize = n;
				pMallocInfoArray[i].mFill = gFill[kOperationMalloc];
				pMallocInfoArray[i].mpData = pAllocator->Malloc(n);

				if(pMallocInfoArray[i].mpData)
					memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
				else
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}

			EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		}

		EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		for(i = 0; i < nAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}


int SOTestAllocationPathological(SmallObjectAllocator* pAllocator)
{
	size_t       i;
	int          nErrorCount = 0;
	const size_t kAllocSlotCount      = 2500;
	const size_t kAllocIterationCount = 5000;
	const size_t kAllocCycleCount     = 3;
	const int    kAllocSizeMin        = 0;
	const int    kAllocSizeMax        = 125;

	MallocInfo* const pMallocInfoArray = new MallocInfo[kAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * kAllocSlotCount);

	// Allow for unusual size and alignment situations.
	pAllocator->SetOption(SmallObjectAllocator::kOptionMaxChunkWaste, 128);

	// Run tests
	for(size_t c(0); c < kAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < kAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)kAllocIterationCount, (unsigned)c, (unsigned)kAllocCycleCount);

			i = gRand.RandRange(0, (int)kAllocSlotCount);
			void*& pDest = pMallocInfoArray[i].mpData;

			if(pDest)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
			else
			{
				const size_t x = gRand.RandRange(0, 10);

				if(x < 8) // 80% of the time, do some standard allocation.
				{
					EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
					const size_t n = gRand.RandRange(kAllocSizeMin, kAllocSizeMax);
					pDest = pAllocator->Malloc(n);
					memset(pDest, gFill[kOperationMalloc], n);
					EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
				}
				else
				{
					// Do some pathological operations.
					size_t y = gRand.RandRange(0, 8);
					size_t a = 0;

					switch (y)
					{
						case 0:
						{
							// Small or zero sized allocation.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 1:
						{
							// Small or zero sized allocation with large alignment.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							a = 16 << gRand.RandRange(0, 3);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, a);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 2:
						{
							// Realloc NULL to zero or small size.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationRealloc];
							pMallocInfoArray[i].mpData = pAllocator->Realloc(NULL, pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}


						case 3:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 4:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x00100000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 5:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 6:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}

						case 7:
						{   
							// This should fail
							pMallocInfoArray[i].mSize  = 4096 + gRand.RandRange(0, 128);
							a = 32 << gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pDest = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, a);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
							break;
						}
					}
				}

				if(pMallocInfoArray[i].mpData)
					memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
				else
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}

		EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		for(i = 0; i < kAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
				EATEST_VERIFY(pAllocator->ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}



int SOTestConstruction()
{
	EA::UnitTest::Report("    SOTestConstruction\n");

	int nErrorCount = 0;

	{
		// Test default construction and destruction.
		SmallObjectAllocator soa;

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	{
		// Test default construction.
		SmallObjectAllocator soa;

		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		soa.Shutdown();

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	{
		// Test construction with parameters specified by Init.
		SmallObjectAllocator soa;

		SOInit(&soa);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 0); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SOTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);

		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		soa.Shutdown();

		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	{
		// Test SmallObjectAllocatorN<>
		SmallObjectAllocator::Parameters param;
		param.mnPoolCount = 5;

		SmallObjectAllocatorN<5> soa;

		soa.SetCoreAllocationFunction(SOCoreAllocationFunction, SOCoreFreeFunction, gpEAGeneralAllocator);
		soa.SetAssertionFailureFunction(SOAssertionFailureInfoFunction, &soa);

		soa.Init(param);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		nErrorCount += SOTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);

		soa.Shutdown();
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	{
		// Test page-sized allocations with kCoreBlockPageSize
		SmallObjectAllocator soa;

		SOInit(&soa);
		soa.SetCoreAllocationFunction(SOCoreAllocationFunctionPaged, SOCoreFreeFunction, gpEAGeneralAllocator);
		soa.SetOption(SmallObjectAllocator::kOptionPagedCoreBlockSize, (int)kCoreBlockPageSize);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 1); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SOTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);
	}

	return nErrorCount;
}


int SOTestValidateAddress()
{
	EA::UnitTest::Report("    SOTestValidateAddress\n");

	int nErrorCount = 0;

	SmallObjectAllocator soa;
	SOInit(&soa);


	// Test the allocated pointer itself.
	void* pAlloc = soa.Malloc(100);

	const void* pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == pAlloc);

	// Test the allocated pointer + n, which is still valid, but isn't the allocated ptr itself.
	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallObjectAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallObjectAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallObjectAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == (char*)pAlloc + 40);

	// Test the allocated pointer - n, which is not valid.
	pTest = soa.ValidateAddress((char*)pAlloc - 4, SmallObjectAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress((char*)pAlloc - 4, SmallObjectAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress(NULL, SmallObjectAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == NULL);


	// Test the freed pointer.
	soa.Free(pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == NULL);

	if(soa.GetOption(SmallObjectAllocator::kOptionAlwaysFreeCoreBlocks) == 0)
	{
		pTest = soa.ValidateAddress(pAlloc, SmallObjectAllocator::kAddressTypeOwned);
		EATEST_VERIFY(pTest != NULL);
	}

	return nErrorCount;
}


int SOTestAllocation()
{
	EA::UnitTest::Report("    SOTestAllocation\n");

	int nErrorCount = 0;

	SmallObjectAllocator soa;
	SOInit(&soa);

	gRand.SetSeed(EA::UnitTest::GetRandSeed() + 2);

	// Test AllocationFlags
	MallocInfo* const pMallocInfoArray = new MallocInfo[kDefaultAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * kDefaultAllocSlotCount);

	SmallObjectAllocator::Parameters parameters;
	soa.GetParameters(parameters);
	const size_t nAllocSizeMax = parameters.mnMaxMallocSize + 1; // +1 because nAllocSizeMax refers to one-past the largest malloc size.

	#if PPM_NULL_POINTER_FREE_ENABLED
		soa.Free(NULL); // Make sure Free of NULL pointer works
	#endif

	// Run tests
	for(size_t c(0); c < kDefaultAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < kDefaultAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, kDefaultAllocIterationCount, c, kDefaultAllocCycleCount);

			i = gRand.RandRange(0, (int)kDefaultAllocSlotCount);

			const size_t nOperation = gRand.RandRange(0, kOperationCount);         // 0, 1, 2, or 3.
				  size_t nSize      = gRand.RandRange((int)0, (int)nAllocSizeMax);
			const size_t nAlignment = SmallObjectAllocator::kMinAlignment * gRand.RandRange(0, 2);  // 0, 16, or 32.

			if(pMallocInfoArray[i].mpData)
			{
				// Test freeing of memory
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
				soa.Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
			else
			{
				// Test allocating memory
				switch (nOperation)
				{ 
					case kOperationMalloc:
					{
						pMallocInfoArray[i].mSize  = nSize;
						pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
						pMallocInfoArray[i].mpData = soa.Malloc(pMallocInfoArray[i].mSize);
						EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SOTestAllocation error: Failed to allocate memory.\n");

						if(pMallocInfoArray[i].mpData)
							memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
						else
							memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

						break;
					}

					case kOperationRealloc:
					{
						pMallocInfoArray[i].mSize  = nSize;
						pMallocInfoArray[i].mFill  = gFill[kOperationRealloc];
						pMallocInfoArray[i].mpData = soa.Realloc(pMallocInfoArray[i].mpData, nSize);
						EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SOTestAllocation error: Failed to allocate memory.\n");

						if(pMallocInfoArray[i].mpData)
							memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
						else
							memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

						break;
					}

					default: // kOperationMallocAligned
					{
						pMallocInfoArray[i].mSize  = nSize;
						pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
						pMallocInfoArray[i].mpData = soa.MallocAligned(pMallocInfoArray[i].mSize, nAlignment);
						EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SOTestAllocation error: Failed to allocate memory.\n");

						if(pMallocInfoArray[i].mpData)
							memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
						else
							memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

						break;
					}
				}
			}
		}

		for(i = 0; i < kDefaultAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
				soa.Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}


int SOTestPathological()
{
	EA::UnitTest::Report("    SOTestPathological\n");

	int nErrorCount = 0;

	{
		SmallObjectAllocator soa;

		SOInit(&soa);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 3); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SOTestAllocationPathological(&soa);

		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		soa.Shutdown();

		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	return nErrorCount;
}



static bool SODescribeBlock(const SmallObjectAllocator::BlockInfo* pBlockInfo, void* /*pContext*/)
{
	const char* blockType = "Unknown";

	switch(pBlockInfo->mBlockType)
	{
		case SmallObjectAllocator::kBlockTypeNone:
			blockType = "None";
			break;
		case SmallObjectAllocator::kBlockTypeInvalid:
			blockType = "Invalid";
			break;
		case SmallObjectAllocator::kBlockTypeAllocated:
			blockType = "Allocated";
			break;
		case SmallObjectAllocator::kBlockTypeFree:
			blockType = "Free";
			break;
		case SmallObjectAllocator::kBlockTypeCore:
			blockType = "Core";
			break;
	}

	EA::UnitTest::ReportVerbosity(1, "Block type: %12.12s core: 0x%08x, core size: %8d, data addr: 0x%08x, data size: %8d\n", 
					 blockType, (unsigned)(uintptr_t)pBlockInfo->mpCore, (unsigned)pBlockInfo->mnCoreSize, 
					 (unsigned)(uintptr_t)pBlockInfo->mpData, (unsigned)pBlockInfo->mnDataSize);
	return true;
}


int SOTestReportHeap()
{
	EA::UnitTest::Report("    SOTestReportHeap\n");

	int nErrorCount = 0;

	SmallObjectAllocator soa;
	SOInit(&soa);

	// Allocate some blocks
	const size_t kArraySize = 32;
	void* pArray[kArraySize];
	memset(pArray, 0, sizeof(pArray));

	for(size_t i = 0; i < kArraySize; i++)
	{
		pArray[i] = soa.Malloc(i * 3);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
	}

	// Free a few blocks.
	soa.Free(pArray[3]); pArray[3] = NULL;
	EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

	soa.Free(pArray[5]); pArray[5] = NULL;
	EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

	soa.Free(pArray[6]); pArray[6] = NULL;
	EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

	// Do an allocated memory report.
	if(EA::UnitTest::GetVerbosity() >= 1)
		soa.TraceAllocatedMemory();

	// Test ReportBegin, ReportNext, ReportEnd.
	SmallObjectAllocator::ReportContext reportContext;

	if(soa.ReportBegin(&reportContext, 0xff))
	{
		for(const SmallObjectAllocator::BlockInfo* pBlockInfo = soa.ReportNext(&reportContext); pBlockInfo; pBlockInfo = soa.ReportNext(&reportContext))
			SODescribeBlock(pBlockInfo, NULL);

		soa.ReportEnd(&reportContext);
	}

	// Free the rest of the blocks.
	for(size_t i = 0; i < kArraySize; i++)
	{
		if(pArray[i])
		{
			soa.Free(pArray[i]);
			EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		}
	}

	return nErrorCount;
}


int SOTestRegression()
{
	EA::UnitTest::Report("    SOTestRegression\n");

	int nErrorCount = 0;

	{   // Test user report that freeing up memory doesn't result in freed core blocks.
		SmallObjectAllocator soa;
		SOInit(&soa);

		// Allocate some blocks
		const size_t kArraySize = 260;
		void* pArray[kArraySize];
		memset(pArray, 0, sizeof(pArray));

		for(int i = 0; i < 256; ++i)
		{
			pArray[i] = soa.Malloc(8);
			EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		}

		for(int j = 0; j < 256; ++j)
		{
			soa.Free(pArray[j]);
			pArray[j] = NULL;
			EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));
		}

		pArray[0] = soa.Malloc(80);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		pArray[1] = soa.Malloc(80);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		pArray[2] = soa.Malloc(80);
		EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));

		// Free the rest of the blocks.
		for(size_t j = 0; j < kArraySize; j++)
		{
			if(pArray[j])
			{
				soa.Free(pArray[j]);
				EATEST_VERIFY(soa.ValidateHeap(SmallObjectAllocator::kHeapValidationLevelFull));  
			}
		}
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestSmallObjectAllocator
//
// This is the main entry function for the SmallObjectAllocator unit test.
//
int TestSmallObjectAllocator()
{
	int nErrorCount = 0;

	// Set some parameters, based on the global settings.
	kDefaultAllocSlotCount      = gnAllocSlotCount * 2;
	kDefaultAllocIterationCount = gnAllocIterationCount * 2;
	kDefaultAllocCycleCount     = gnAllocCycleCount;

	nErrorCount += SOTestRegression();
	nErrorCount += SOTestConstruction();
	nErrorCount += SOTestValidateAddress();
	nErrorCount += SOTestAllocation();
	nErrorCount += SOTestPathological();
	nErrorCount += SOTestReportHeap();

	return nErrorCount;
}






