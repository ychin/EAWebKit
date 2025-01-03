///////////////////////////////////////////////////////////////////////////////
// TestSmallBlockAllocator
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "EASmallBlockAllocator.h"
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/internal/shared.h>
#include <EAStdC/EASprintf.h>

using namespace EA::Allocator;
using namespace EA::UnitTest;
using namespace EA::EAMain;


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


namespace TestSBA
{
	uint8_t gFill[kOperationCount]      = { 0xe0, 0xe1, 0xe2 };

	size_t  kDefaultAllocSizeMin        =     0;
	size_t  kDefaultAllocSizeMax        =     0;  // 0 means user allocator max.
	size_t  kDefaultAllocSlotCount      = 10000;
	size_t  kDefaultAllocIterationCount = 20000;
	size_t  kDefaultAllocCycleCount     =     2;

	const size_t kDefaultAuditTestCount = 24;
}

using namespace TestSBA;


// Helper functions
bool   SBInit(SmallBlockAllocator* pSOA, bool testPagedSizedCoreBlocks = false, SmallBlockAllocator::Parameters* pParameters = NULL);
void   SBAssertionFailureInfoFunction(SmallBlockAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
void*  SBCoreAllocationFunction(SmallBlockAllocator* pAllocator, size_t nSize, size_t nAlignment, size_t nAlignmentOffset, void* pContext);
void   SBCoreFreeFunction(SmallBlockAllocator* pAllocator, void* pCore, void* pContext);
int    SBTestAllocationGeneric(SmallBlockAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount);
int    SBTestAllocationPathological(SmallBlockAllocator* pAllocator);

// High level test functions
int    SBTestConstruction();
int    SBTestValidateAddress();
int    SBTestAllocation(bool testPagedSizedCoreBlocks);
int    SBTestPathological();
int    SBTestCoreBlockRecycling();
int    SBTestReportHeap();
int    SBTestExtensions();


bool SBInit(SmallBlockAllocator* pSOA, bool testPagedSizedCoreBlocks, SmallBlockAllocator::Parameters* pParameters)
{
	bool bReturnValue = true;

	//It's valid to call this before init
	pSOA->SetAssertionFailureFunction(SBAssertionFailureInfoFunction, pSOA);

	if(pParameters)
		pSOA->Init(pParameters, SBCoreAllocationFunction, SBCoreFreeFunction, gpEAGeneralAllocator, gpEAGeneralAllocator);
	else
	{
		SmallBlockAllocator::Parameters defaultParams;

		if(testPagedSizedCoreBlocks)
			defaultParams.mnMaxAlignment = defaultParams.mnCoreBlockSize;

		pSOA->Init(&defaultParams, SBCoreAllocationFunction, SBCoreFreeFunction, gpEAGeneralAllocator, gpEAGeneralAllocator);
	}

	// Allow for maximum waste
	pSOA->SetOption(SmallBlockAllocator::kOptionMaxAllocWaste, (int)pSOA->GetMaxChunkSize());

	return bReturnValue;
}


void SBAssertionFailureInfoFunction(SmallBlockAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault("SBAssertionFailureInfoFunction:\n        ", pContext);
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);

	if(EA::UnitTest::GetInteractive())
		 EATEST_DEBUG_BREAK();
}


void* SBCoreAllocationFunction(SmallBlockAllocator* /*pAllocator*/, size_t nSize,
								size_t nAlignment, size_t nAlignmentOffset, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	return pGeneralAllocator->MallocAligned(nSize, nAlignment, nAlignmentOffset);
}

void SBCoreFreeFunction(SmallBlockAllocator* /*pAllocator*/, void* pCore, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	pGeneralAllocator->Free(pCore);
}


int SBTestAllocationGeneric(SmallBlockAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax,
							 size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount)
{
	size_t i;

	int nErrorCount = 0;

	if(nAllocSizeMax == 0)
	{
		nAllocSizeMax = pAllocator->GetMaxChunkSize() + 1; // +1 because nAllocSizeMax refers to one-past the largest malloc size.
	}

	MallocInfo* pMallocInfoArray = new MallocInfo[nAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * nAllocSlotCount);

	// Run tests
	for(size_t c(0); c < nAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < nAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::EAMain::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, nAllocIterationCount, c, nAllocCycleCount);

			i = gRand.RandRange(0, (int)nAllocSlotCount);

			const size_t n = gRand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);

			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				if(s > 1)
				{
					size_t halfS = pAllocator->GetUsableSize( reinterpret_cast<uint8_t*>(pMallocInfoArray[i].mpData) + (s/2));
					EATEST_VERIFY(s/2 == halfS);
				}

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
		}

		EATEST_VERIFY(pAllocator->Validate());

		for(i = 0; i < nAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				if(s > 1)
				{
					size_t halfS = pAllocator->GetUsableSize( reinterpret_cast<uint8_t*>(pMallocInfoArray[i].mpData) + (s/2));
					EATEST_VERIFY(s/2 == halfS);
				}

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}

int SBTestAllocationPathological(SmallBlockAllocator* pAllocator)
{
	size_t       i;
	int          nErrorCount = 0;
	const size_t kAllocSlotCount      = 2500;
	const size_t kAllocIterationCount = 5000;
	const size_t kAllocCycleCount     = 3;
	const int    kAllocSizeMin        = 0;
	const int    kAllocSizeMax        = 125;

	MallocInfo* pMallocInfoArray = new MallocInfo[kAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * kAllocSlotCount);

	// Run tests
	for(size_t c(0); c < kAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); j < kAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::EAMain::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)kAllocIterationCount, (unsigned)c, (unsigned)kAllocCycleCount);

			i = gRand.RandRange(0, (int)kAllocSlotCount);
			void*& pDest = pMallocInfoArray[i].mpData;

			if(pDest)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				if(s > 1)
				{
					size_t halfS = pAllocator->GetUsableSize( reinterpret_cast<uint8_t*>(pMallocInfoArray[i].mpData) + (s/2));
					EATEST_VERIFY(s/2 == halfS);
				}

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
			else
			{
				const size_t x = gRand.RandRange(0, 10);

				if(x < 8) // 80% of the time, do some standard allocation.
				{
					const size_t n = gRand.RandRange(kAllocSizeMin, kAllocSizeMax);
					pDest = pAllocator->Malloc(n);
					memset(pDest, gFill[kOperationMalloc], n);
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
							break;
						}

						case 1:
						{
							// Small or zero sized allocation with large alignment.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							a = 16 << gRand.RandRange(0, 3);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize, a);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 2:
						{
							// Realloc NULL to zero or small size.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationRealloc];
							pMallocInfoArray[i].mpData = pAllocator->Realloc(NULL, pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}


						case 3:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							break;
						}

						case 4:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize, 0x00100000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							break;
						}

						case 5:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							break;
						}

						case 6:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							break;
						}

						case 7:
						{
							// This should fail
							pMallocInfoArray[i].mSize  = 4096 + gRand.RandRange(0, 128);
							a = 32 << gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pDest = pAllocator->Malloc(pMallocInfoArray[i].mSize, a);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
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

		EATEST_VERIFY(pAllocator->Validate());

		for(i = 0; i < kAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				if(s > 1)
				{
					size_t halfS = pAllocator->GetUsableSize( reinterpret_cast<uint8_t*>(pMallocInfoArray[i].mpData) + (s/2));
					EATEST_VERIFY(s/2 == halfS);
				}

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}

int SBTestAllocationCoreBlockRecycling(SmallBlockAllocator* pAllocator, SmallBlockAllocator::Parameters* pParameters)
{
	int          nErrorCount = 0;
	const size_t kSimultaneousAllocCountMax = 2048;

	void* pAllocArray[kSimultaneousAllocCountMax];
	memset(pAllocArray, 0, sizeof(pAllocArray));

	//this test assumes a single page with no growing allowed.
	//the goal is to fill up the page with allocations of one size, 
	//then empty it and repeat for each allocation size to show the page getting recycled
	EATEST_VERIFY(pParameters->mnInitCoreBlockCount == 1);
	EATEST_VERIFY(pParameters->mnGrowCoreBlockCount == 0);
	EATEST_VERIFY(pParameters->mnMaxAllocSize > 0); //this test requires explicit max malloc size so we can calculate max allocs per page

	for (size_t allocationSize(pParameters->mnMinAlignment) ; allocationSize <= pParameters->mnMaxAllocSize; allocationSize += pParameters->mnMinAlignment )
	{
		//see how many allocation of this size it takes to fill up a page
		const size_t allocsPerPage = ((pParameters->mnCoreBlockSize - SmallBlockAllocator::CoreBlockOverhead()) / allocationSize);
		EATEST_VERIFY(allocsPerPage < kSimultaneousAllocCountMax);

		//fill up a page
		for (size_t i(0) ; i < allocsPerPage ; i++ )
		{
			pAllocArray[i] = pAllocator->Malloc(allocationSize);
			EATEST_VERIFY(NULL != pAllocArray[i]); //this allocation should succeed
		}

		//try to allocate one more, it should fail since we only have one page, it is full, and we disabled growing
		void* pTemp = pAllocator->Malloc(allocationSize);
		EATEST_VERIFY(NULL == pTemp);

		//empty the page
		for (size_t i(0) ; i < allocsPerPage ; i++ )
		{
			pAllocator->Free(pAllocArray[i]);
		}
	}
	
	EATEST_VERIFY(pAllocator->Validate());

	return nErrorCount;
}

int SBTestConstruction()
{
	EA::EAMain::Report("    SBTestConstruction\n");

	int nErrorCount = 0;

	{
		// Test default construction and destruction.
		SmallBlockAllocator soa;

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(soa.Validate());
	}

	{
		// Test default construction.
		SmallBlockAllocator soa;

		EATEST_VERIFY(soa.Validate());
		soa.Shutdown();

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(soa.Validate());
	}

	{
		// Test construction with parameters specified by Init.
		SmallBlockAllocator soa;

		SBInit(&soa);
		EATEST_VERIFY(soa.Validate());

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 0);
		nErrorCount += SBTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);

		EATEST_VERIFY(soa.Validate());
		soa.Shutdown();

		EATEST_VERIFY(soa.Validate());
	}

	{
		// Test construction with parameters specified by Init including custom pool sizes
		uint16_t customPoolSizes[] = {8,16,32,64,128,136,160,256,0};
		SmallBlockAllocator::Parameters params(256*4+static_cast<uint16_t>(SmallBlockAllocator::CoreBlockOverhead()),7,6);
		params.mnCustomPoolSize = customPoolSizes;

		SmallBlockAllocator soa;

		SBInit(&soa,false,&params);
		EATEST_VERIFY(soa.Validate());

		EATEST_VERIFY(soa.GetMaxChunkSize() == 256);

		nErrorCount += SBTestAllocationGeneric(&soa, kDefaultAllocSizeMin, soa.GetMaxChunkSize(), kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);

		EATEST_VERIFY(soa.Validate());
		soa.Shutdown();

		EATEST_VERIFY(soa.Validate());
	}

	#if 0 //$$todo
	{
		// Test SmallBlockAllocatorN<>
		SmallBlockAllocator::Parameters param;
		param.mnPoolCount = 5;

		SmallBlockAllocatorN<5> soa;
		soa.Init(param);
		EATEST_VERIFY(soa.Validate());

		soa.SetCoreAllocationFunction(SBCoreAllocationFunction, SBCoreFreeFunction, gpEAGeneralAllocator);
		soa.SetAssertionFailureFunction(SBAssertionFailureInfoFunction, &soa);

		nErrorCount += SBTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);

		soa.Shutdown();
		EATEST_VERIFY(soa.Validate());
	}
	#endif

	{
		// Test with page-sized/aligned CoreBlocks
		SmallBlockAllocator soa;

		SBInit(&soa,true);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 2); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SBTestAllocationGeneric(&soa, kDefaultAllocSizeMin, kDefaultAllocSizeMax, kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);
	}

	{
		// Test with non-standard init params
		SmallBlockAllocator soa;

		SmallBlockAllocator::Parameters params(16*2+static_cast<uint16_t>(SmallBlockAllocator::CoreBlockOverhead()),1,1,4,8,sizeof(void*));

		SBInit(&soa, false, &params);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 3); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SBTestAllocationGeneric(&soa, kDefaultAllocSizeMin, soa.GetMaxChunkSize(), kDefaultAllocSlotCount, kDefaultAllocIterationCount, kDefaultAllocCycleCount);
	}

	return nErrorCount;
}


int SBTestValidateAddress()
{
	EA::EAMain::Report("    SBTestValidateAddress\n");

	int nErrorCount = 0;

	SmallBlockAllocator soa;
	SBInit(&soa);


	// Test the allocated pointer itself.
	void* pAlloc = soa.Malloc(100);

	const void* pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == pAlloc);

	// Test the allocated pointer + n, which is still valid, but isn't the allocated ptr itself.
	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallBlockAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallBlockAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = soa.ValidateAddress((char*)pAlloc + 40, SmallBlockAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == (char*)pAlloc + 40);

	// Test the allocated pointer - n, which is not valid.
	pTest = soa.ValidateAddress((char*)pAlloc - 4, SmallBlockAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	//$$todo: can this test work? -4 may put us into another Chunk which may or may not be allocated
	//pTest = soa.ValidateAddress((char*)pAlloc - 4, SmallBlockAllocator::kAddressTypeAllocated);
	//EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress(NULL, SmallBlockAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == NULL);


	// Test the freed pointer.
	soa.Free(pAlloc);

	pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == NULL);

	pTest = soa.ValidateAddress(pAlloc, SmallBlockAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest != NULL);

	return nErrorCount;
}


int SBTestAllocation(bool testPagedSizedCoreBlocks)
{
	EA::EAMain::Report("    SBTestAllocation\n");

	int nErrorCount = 0;

	SmallBlockAllocator soa;
	SBInit(&soa,testPagedSizedCoreBlocks);

	gRand.SetSeed(EA::UnitTest::GetRandSeed() + 2);

	// Test AllocationFlags
	MallocInfo* pMallocInfoArray = new MallocInfo[kDefaultAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * kDefaultAllocSlotCount);

	const size_t nAllocSizeMax = soa.GetMaxChunkSize() + 1; // +1 because nAllocSizeMax refers to one-past the largest malloc size.

	//test safe free .. this should do nothing with pMallocInfoArray
	bool freeSafeResult = soa.FreeSafe(pMallocInfoArray);
	EATEST_VERIFY( false == freeSafeResult );
	EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray, sizeof(MallocInfo) * kDefaultAllocSlotCount, 0));

	#if PPM_NULL_POINTER_FREE_ENABLED
		soa.Free(NULL); // Make sure Free of NULL pointer works
		soa.FreeSafe(NULL); // Make sure Free of NULL pointer works
	#endif

	// Run tests
	for(size_t c(0); c < kDefaultAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); j < kDefaultAllocIterationCount; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::EAMain::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, kDefaultAllocIterationCount, c, kDefaultAllocCycleCount);

			i = gRand.RandRange(0, (int)kDefaultAllocSlotCount);

			const size_t nOperation = gRand.RandRange(0, kOperationCount);         // 0, 1, 2, or 3.
				  size_t nSize      = gRand.RandRange((int)0, (int)nAllocSizeMax);
			const size_t nAlignment = 16 * gRand.RandRange(0, 2);  // 0, 16, or 32.

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
						EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SBTestAllocation error: Failed to allocate memory.\n");

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
						EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SBTestAllocation error: Failed to allocate memory.\n");

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

						// Note by Paul Pedriana: I have seen this MallocAligned fail due to there being simply no more 
						// matching Chunks within pools under 64 bit pointers. SmallObjectAllocator doesn't have a means to 
						// Grow the pool upon the case of there being free blocks but none of them are of the desired alignment.
						#if (EA_PLATFORM_PTR_SIZE < 8)
							EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "SBTestAllocation error: Failed to allocate memory.\n");
						#endif

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


int SBTestPathological()
{
	EA::EAMain::Report("    SBTestPathological\n");

	int nErrorCount = 0;

	{
		SmallBlockAllocator soa;

		SBInit(&soa);
		EATEST_VERIFY(soa.Validate());

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 3); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SBTestAllocationPathological(&soa);

		EATEST_VERIFY(soa.Validate());
		soa.Shutdown();

		EATEST_VERIFY(soa.Validate());
	}

	return nErrorCount;
}

int SBTestCoreBlockRecycling()
{
	EA::EAMain::Report("    SBTestCoreBlockRecycling\n");

	int nErrorCount = 0;

	{
		//Only allow one core block and no growth. Also explicitly state the max alloc size so the test code can read it.
		SmallBlockAllocator::Parameters params(4096,1,0,4,SmallBlockAllocator::kDefaultMaxAlignment,1344);
		SmallBlockAllocator soa;

		SBInit(&soa,false,&params);
		EATEST_VERIFY(soa.Validate());

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 4); // Intentionally use our own values below as opposed to global values.
		nErrorCount += SBTestAllocationCoreBlockRecycling(&soa,&params);

		EATEST_VERIFY(soa.Validate());
		soa.Shutdown();

		EATEST_VERIFY(soa.Validate());
	}

	return nErrorCount;
}

static bool SODescribeNode(const SmallBlockAllocator::ReportNode* pNodeInfo, void* /*pContext*/)
{
	const char* nodeType = "Unknown";
	const SmallBlockAllocator::ChunkInfo* pChunkInfo = NULL;

	switch(pNodeInfo->mNodeType)
	{
		case SmallBlockAllocator::kNodeTypeInvalid:
			nodeType = "Invalid";
			break;
		case SmallBlockAllocator::kNodeTypeChunkAllocated:
			nodeType = "Allocated";
			pChunkInfo = &pNodeInfo->mChunkInfo;
			break;
		case SmallBlockAllocator::kNodeTypeChunkFree:
			nodeType = "Free";
			pChunkInfo = &pNodeInfo->mChunkInfo;
			break;
		case SmallBlockAllocator::kNodeTypeCoreBlock:
			nodeType = "CoreBlock";
			break;
	}

	if ( pChunkInfo )
	{
		EA::EAMain::ReportVerbosity(1, "Node type: %12.12s core: 0x%08x, core size: %8d, data addr: 0x%08x, data size: %8d\n",
									 nodeType, (unsigned)(uintptr_t)pChunkInfo->mCoreBlock, (unsigned)pChunkInfo->mnCoreBlockSize,
									 (unsigned)(uintptr_t)pChunkInfo->mChunk, (unsigned)pChunkInfo->mnChunkSize);
	}

	return true;
}


int SBTestReportHeap()
{
	EA::EAMain::Report("    SBTestReportHeap\n");

	int nErrorCount = 0;

	SmallBlockAllocator soa;
	SBInit(&soa);

	// Allocate some Chunks
	const size_t kArraySize = 32;
	void* pArray[kArraySize];
	memset(pArray, 0, sizeof(pArray));

	for(size_t i = 0; i < kArraySize; i++)
		pArray[i] = soa.Malloc(i * 3);

	// Free a few Chunks.
	soa.Free(pArray[3]); pArray[3] = NULL;
	soa.Free(pArray[5]); pArray[5] = NULL;
	soa.Free(pArray[6]); pArray[6] = NULL;

	// Do an allocated memory report.
	if(EA::EAMain::GetVerbosity() >= 1)
		soa.DumpPools();

	// Test ReportBegin, ReportNext, ReportEnd.
	SmallBlockAllocator::ReportContext reportContext;

	if(soa.ReportBegin(&reportContext, SmallBlockAllocator::kNodeTypeAll))
	{
		for(const SmallBlockAllocator::ReportNode* pNodeInfo = soa.ReportNext(&reportContext); pNodeInfo; pNodeInfo = soa.ReportNext(&reportContext))
		{
			SODescribeNode(pNodeInfo, NULL);
		}
		soa.ReportEnd(&reportContext);
	}

	// Free the rest of the Chunks.
	for(size_t i = 0; i < kArraySize; i++)
	{
		if(pArray[i])
			soa.Free(pArray[i]);
	}

	return nErrorCount;
}

struct SOAuditTestChunkInfo
{
	void*             mpChunk;
	const char*       mpFileName;
	char              mpAllocName[256];
	int32_t           mnGroupId;
	uint32_t          mnRequestedSize;
};

struct SOAuditTestContext
{
	int                    nErrorCount;
	uint32_t               mnChunkCount;
	SOAuditTestChunkInfo   mpChunkInfo[kDefaultAuditTestCount];
};

void SOAuditFunction(const EA::Allocator::SmallBlockAllocator::ChunkInfo* pChunk, void* pContext)
{
	int nErrorCount = 0;
	EATEST_VERIFY(NULL != pContext);

	SOAuditTestContext* pAuditContext = reinterpret_cast<SOAuditTestContext*>(pContext);

	pAuditContext->mnChunkCount++;

	SOAuditTestChunkInfo* pChunkInfo = NULL;
	for ( size_t nChunkIndex = 0; nChunkIndex < kDefaultAuditTestCount; nChunkIndex++)
	{
		if ( pChunk->mChunk == reinterpret_cast<EA::Allocator::SmallBlockAllocator::ChunkRef>(pAuditContext->mpChunkInfo[nChunkIndex].mpChunk) )
		{
			pChunkInfo = &(pAuditContext->mpChunkInfo[nChunkIndex]);
			break;
		}
	}

	EATEST_VERIFY(NULL != pChunk);
	EATEST_VERIFY(NULL != pChunkInfo);

	EATEST_VERIFY(pChunk->mnRequestedSize == pChunkInfo->mnRequestedSize);
	EATEST_VERIFY(pChunk->mnGroupId == pChunkInfo->mnGroupId);
	EATEST_VERIFY(0 == strcmp(pChunk->mpAllocName,pChunkInfo->mpAllocName));
	EATEST_VERIFY(0 == strcmp(pChunk->mpAllocFile,pChunkInfo->mpFileName));

	pAuditContext->nErrorCount += nErrorCount;
}

int SOPopulateAuditTestContext(EA::Allocator::SmallBlockAllocator* pAllocator, SOAuditTestContext* pTestContext, int32_t nGroupId, int numChunks)
{
	int nErrorCount = 0;

	pAllocator->SetGroupId(nGroupId);
	EATEST_VERIFY(nGroupId == pAllocator->GetGroupId());
	for (int nAllocIndex = 0; nAllocIndex < numChunks; nAllocIndex++)
	{
		SOAuditTestChunkInfo* pChunkInfo = &(pTestContext->mpChunkInfo[nAllocIndex]);

		if (pChunkInfo->mpChunk)
		{
			pAllocator->Free(pChunkInfo->mpChunk);
		}

		pChunkInfo->mnGroupId = pAllocator->GetGroupId();
		pChunkInfo->mpFileName = __FILE__;
		EA::StdC::Sprintf(pChunkInfo->mpAllocName, "Allocation %d", nAllocIndex);
		uint32_t nSize      = (uint32_t)gRand.RandRange((int)0, (int)pAllocator->GetMaxChunkSize());
		pChunkInfo->mnRequestedSize = nSize;
		pChunkInfo->mpChunk = pAllocator->Malloc(pChunkInfo->mnRequestedSize,EA_ALLOCINFO(pChunkInfo->mpAllocName));
	}

	return nErrorCount;
}

int SBTestExtensions()
{
	int nErrorCount = 0;

	#if EA_SOA_EXTENSIONS_ENABLED

		EA::EAMain::Report("    SBTestExtensions\n");

		SmallBlockAllocator soa;
		SBInit(&soa);
		soa.SetOption(EA::Allocator::SmallBlockAllocator::kOptionEnableExtensions,1);
		SOAuditTestContext testContext;
		memset(&testContext, 0, sizeof(testContext));

		// empty audit, should have no hits
		uint32_t nAuditChunkCount = soa.Audit(0,SOAuditFunction,NULL);
		EATEST_VERIFY(nAuditChunkCount == 0);

		// make some allocations and audit them
		int32_t nGroupId1 = 0xdeadbeef;
		nErrorCount += SOPopulateAuditTestContext(&soa,&testContext, nGroupId1, kDefaultAuditTestCount);
		testContext.mnChunkCount = 0;
		testContext.nErrorCount = 0;
		nAuditChunkCount = soa.Audit(nGroupId1,SOAuditFunction,reinterpret_cast<void*>(&testContext));
		EATEST_VERIFY(nAuditChunkCount == testContext.mnChunkCount);
		EATEST_VERIFY(nAuditChunkCount == kDefaultAuditTestCount);
		nErrorCount += testContext.nErrorCount;

		// replace those with some with a different group id and audit them
		int32_t nGroupId2 = 0xeddeaded;
		nErrorCount += SOPopulateAuditTestContext(&soa,&testContext, nGroupId2, kDefaultAuditTestCount/2);
		testContext.mnChunkCount = 0;
		testContext.nErrorCount = 0;
		nAuditChunkCount = soa.Audit(nGroupId2,SOAuditFunction,reinterpret_cast<void*>(&testContext));
		EATEST_VERIFY(nAuditChunkCount == testContext.mnChunkCount);
		EATEST_VERIFY(nAuditChunkCount == kDefaultAuditTestCount/2);
		nErrorCount += testContext.nErrorCount;

		// verify Chunks are still left from first audit
		testContext.mnChunkCount = 0;
		testContext.nErrorCount = 0;
		nAuditChunkCount = soa.Audit(nGroupId1,SOAuditFunction,reinterpret_cast<void*>(&testContext));
		EATEST_VERIFY(nAuditChunkCount == testContext.mnChunkCount);
		EATEST_VERIFY(nAuditChunkCount == kDefaultAuditTestCount/2);
		nErrorCount += testContext.nErrorCount;

		//free all of the Chunks
		for (size_t nChunkIndex = 0; nChunkIndex < kDefaultAuditTestCount; nChunkIndex++)
		{
			soa.Free(testContext.mpChunkInfo[nChunkIndex].mpChunk);
			testContext.mpChunkInfo[nChunkIndex].mpChunk = NULL;
		}

		// empty audit, should have no hits
		nAuditChunkCount = soa.Audit(0,SOAuditFunction,NULL);
		EATEST_VERIFY(nAuditChunkCount == 0);

	#endif

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestSmallBlockAllocator
//
// This is the main entry function for the SmallBlockAllocator unit test.
//
int TestSmallBlockAllocator()
{
	int nErrorCount = 0;

	// Set some parameters, based on the global settings.
	kDefaultAllocSlotCount      = gnAllocSlotCount * 2;
	kDefaultAllocIterationCount = gnAllocIterationCount * 2;
	kDefaultAllocCycleCount     = gnAllocCycleCount;

	nErrorCount += SBTestConstruction();
	nErrorCount += SBTestValidateAddress();
	nErrorCount += SBTestAllocation(true);    //test using optimization for power of two CoreBlock size & alignment
	nErrorCount += SBTestAllocation(false);   //test without optimization for power of two CoreBlock size & alignment
	nErrorCount += SBTestPathological();
	nErrorCount += SBTestCoreBlockRecycling();
	nErrorCount += SBTestReportHeap();
	nErrorCount += SBTestExtensions();

	return nErrorCount;
}
