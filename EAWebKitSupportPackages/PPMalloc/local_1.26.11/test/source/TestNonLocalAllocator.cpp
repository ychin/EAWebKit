///////////////////////////////////////////////////////////////////////////////
// TestNonLocalAllocator
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EANonLocalAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <EAStdC/EASprintf.h>
#include <PPMalloc/internal/shared.h>
#include <eathread/eathread_atomic.h>
#include <eathread/eathread_thread.h>


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
	kOperationMallocMultiple,
	kOperationRealloc,
	kOperationCount
};

void* const pFakeCore     = (void*)(uintptr_t)0;
void* const kFailureValue = (void*)(intptr_t)-1;

// static/const variables
static const    uint8_t gFill[kOperationCount] = { 0xe0, 0xe1, 0xe2, 0xe3 };
static volatile bool    gAssertionFailureOccurred = false; // We use this to tell us when an assertion failed so we can immediately stop the test, instead of spewing more assertion failures.

// Helper functions
bool   NLInit(NonLocalAllocator* pNLA, size_t nCoreSize = 1024*1024, size_t nCoreSize2 = 0);
void*  NLNodeAllocationFunction(NonLocalAllocator* pAllocator, size_t nSize, void* pContext);
size_t NLNodeFreeFunction(NonLocalAllocator* pAllocator, void* pNode, size_t nSize, void* pContext);
void   NLAssertionFailureInfoFunctionNop(NonLocalAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
void   NLAssertionFailureInfoFunction(NonLocalAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
bool   NLMallocFailureFunction(NonLocalAllocator* pNonLocalAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
size_t NLCoreFreeFunction(NonLocalAllocator* pNonLocalAllocator, void* pCore, size_t nSize, void* pContext);
int    NLTestAllocationGeneric(NonLocalAllocator* pAllocator, size_t nAllocSizeMin, size_t nAllocSizeMax, size_t nAllocSlotCount, size_t nAllocIterationCount, size_t nAllocCycleCount, bool bRandomTrimCore, bool bHighFrequencyTrimMode = false);
int    NLTestAllocationPathological(NonLocalAllocator* pAllocator);

// High level test functions
int    NLTestConstruction();
int    NLTestAddCore();
int    NLTestInitWithoutCore();
int    NLTestValidateAddress();
int    NLTestAllocation();
int    NLTestThreading();
int    NLTestAllocationFlags();
int    NLTestDebugNaming();
int    NLTestDebugGroupId();
int    NLTestReportHeap();
int    NLTestNonLocal();
int    NLTest64Bit();
int    NLTestSplit();


bool NLInit(NonLocalAllocator* pNLA, size_t nCoreSize1, size_t nCoreSize2)
{
	bool bReturnValue = true;

	pNLA->SetMallocFailureFunction(NLMallocFailureFunction, gpEAGeneralAllocator);
	pNLA->SetAssertionFailureFunction(NLAssertionFailureInfoFunction, pNLA);
	gAssertionFailureOccurred = false;

	if(bReturnValue && nCoreSize1)
	{
		void* const pCore1 = gpEAGeneralAllocator->MallocAligned(nCoreSize1, 64);

		if(pCore1)
			bReturnValue = pNLA->Init(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator, pCore1, nCoreSize1, NLCoreFreeFunction, gpEAGeneralAllocator);
		else
			bReturnValue = false;
	}

	if(bReturnValue && nCoreSize2)
	{
		void* const pCore2 = gpEAGeneralAllocator->MallocAligned(nCoreSize2, 64);

		if(pCore2)
			bReturnValue = pNLA->AddCore(pCore2, nCoreSize2, NLCoreFreeFunction, gpEAGeneralAllocator);
		else
			bReturnValue = false;
	}

	return bReturnValue;
}


void* NLNodeAllocationFunction(NonLocalAllocator* /*pAllocator*/, size_t nSize, void* pContext)
{
	GeneralAllocator*      const pGA  = (GeneralAllocator*)pContext;
	GeneralAllocatorDebug* const pGAD = (GeneralAllocatorDebug*)pGA->AsInterface(GeneralAllocatorDebug::kTypeId);

	if(pGAD)
		return pGAD->MallocDebug(nSize, 0, 0, "NLNodeAllocationFunction", __FILE__, __LINE__);
	else
		return pGA->Malloc(nSize);
}


size_t NLNodeFreeFunction(NonLocalAllocator* /*pAllocator*/, void* pNode, size_t nSize, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	pGeneralAllocator->Free(pNode);
	return nSize;
}


size_t NLCoreFreeFunction(NonLocalAllocator* /*pNonLocalAllocator*/, void* pCore, size_t nSize, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;
	pGeneralAllocator->Free(pCore);
	return nSize;
}


void NLAssertionFailureInfoFunctionNop(NonLocalAllocator::AssertionFailureInfo*, void*)
{
	// Intentionally do nothing.
}


void NLAssertionFailureInfoFunction(NonLocalAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	if(!gAssertionFailureOccurred) // Print this out only the first time.
	{
		gAssertionFailureOccurred = true;

		TraceFunctionDefault("NLAssertionFailureInfoFunction:\n        ", pContext);
		TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
		TraceFunctionDefault("\n", pContext);

		if(EA::UnitTest::GetInteractive())
			 EATEST_DEBUG_BREAK();
	}
}


bool NLMallocFailureFunction(NonLocalAllocator* pNonLocalAllocator, size_t /*nMallocRequestedSize*/, size_t nAllocatorRequestedSize, void* pContext)
{
	GeneralAllocator* const pGeneralAllocator = (GeneralAllocator*)pContext;

	if(nAllocatorRequestedSize < (1000000 / 65536 * 65536))
	   nAllocatorRequestedSize = (1000000 / 65536 * 65536);

	// We set an upper bound of memory allocation and assume that any requests beyond that are 
	// unfullable, even if they happen to be so on the current machine. As we move to large 
	// 64 bit system we begin to see machines with 16GB of RAM or more.
	const size_t kMaxCoreSize = (16 * 1024 * 1024);

	if(nAllocatorRequestedSize <= kMaxCoreSize)
	{
		void* const pNewCore = pGeneralAllocator->MallocAligned(nAllocatorRequestedSize, 256);

		if(pNewCore)
			pNonLocalAllocator->AddCore(pNewCore, nAllocatorRequestedSize, NLCoreFreeFunction, pGeneralAllocator);

		return (pNewCore != NULL);
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestAllocationGeneric
//
// Generic testing of an allocator.
//
int NLTestAllocationGeneric(NonLocalAllocator* pAllocator,
                            size_t nAllocSizeMin,
                            size_t nAllocSizeMax,
                            size_t nAllocSlotCount,
                            size_t nAllocIterationCount,
                            size_t nAllocCycleCount,
                            bool bRandomTrimCore,
                            bool bHighFrequencyTrimMode)
{
	int         nErrorCount   = 0;
	const bool  bShouldMemset = pAllocator->GetOption(NonLocalAllocator::kOptionEnableDebugWrites) != 0;
	void* const kLocalFailureValue = (void*)pAllocator->GetOption(NonLocalAllocator::kOptionFailureValue);

	MallocInfo* pMallocInfoArray = new MallocInfo[nAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * nAllocSlotCount);

	for(size_t i = 0; i < nAllocSlotCount; i++)
		pMallocInfoArray[i].mpData = kLocalFailureValue;

	// Run tests
	for(size_t c(0); c < nAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); (j < nAllocIterationCount) && !gAssertionFailureOccurred; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, nAllocIterationCount, c, nAllocCycleCount);

			size_t i = gRand.RandRange(0, (int)nAllocSlotCount);
			EATEST_VERIFY(i < nAllocSlotCount);

			const size_t n = gRand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);

			if(pMallocInfoArray[i].mpData != kLocalFailureValue) // If the data is allocated...
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(!bShouldMemset || EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);

				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
				pMallocInfoArray[i].mpData = kLocalFailureValue;
			}
			else
			{
				pMallocInfoArray[i].mSize = n;
				pMallocInfoArray[i].mFill = gFill[kOperationMalloc];
				pMallocInfoArray[i].mpData = pAllocator->Malloc(n);

				if(pMallocInfoArray[i].mpData != kLocalFailureValue)
				{
					if(bShouldMemset)
						memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
				}
				else
				{
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
					pMallocInfoArray[i].mpData = kLocalFailureValue;
				}
			}

			if(bRandomTrimCore)
			{
				if(gRand.RandLimit(100) == 0 || bHighFrequencyTrimMode)
					pAllocator->TrimCore(NULL);
			}
		}

		for(size_t i = 0; i < nAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData != kLocalFailureValue) // If the data is allocated...
			{
				const size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(!bShouldMemset || EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
				pMallocInfoArray[i].mpData = kLocalFailureValue;
			}

			if(bRandomTrimCore)
			{
				if(gRand.RandLimit(100) == 0 || bHighFrequencyTrimMode)
					pAllocator->TrimCore(NULL);
			}
		}

		if(bRandomTrimCore)
		{
			if(gRand.RandLimit(2) == 0 || bHighFrequencyTrimMode)
				pAllocator->TrimCore(NULL);
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// NLTestAllocationPathological
//
int NLTestAllocationPathological(NonLocalAllocator* pAllocator)
{
	size_t       i;
	int          nErrorCount = 0;
	const size_t kAllocSlotCount      = gnAllocSlotCount / 2;
	const size_t kAllocIterationCount = gnAllocIterationCount / 2;
	const size_t kAllocCycleCount     = gnAllocCycleCount * 2;
	const int    kAllocSizeMin        = 0;
	const int    kAllocSizeMax        = 1000;
	const bool   bShouldMemset        = pAllocator->GetOption(NonLocalAllocator::kOptionEnableDebugWrites) != 0;

	MallocInfo* pMallocInfoArray = new MallocInfo[kAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * kAllocSlotCount);

	// Run tests
	for(size_t c(0); c < kAllocCycleCount; c++) // For each cycle...
	{
		for(size_t j(0); (j < kAllocIterationCount) && !gAssertionFailureOccurred; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %u of %u; Cycle %u of %u\n", (unsigned)j, (unsigned)kAllocIterationCount, (unsigned)c, (unsigned)kAllocCycleCount);

			i = gRand.RandRange(0, (int)kAllocSlotCount);
			void*& pDest = pMallocInfoArray[i].mpData;

			if(pDest)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(!bShouldMemset || EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

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
					const size_t y = gRand.RandRange(0, 11);

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
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 
																					16384 << gRand.RandRange(0, 3));
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 2:
						{
							// Zero sized allocation with large alignment and offset.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 
																					2048 << gRand.RandRange(0, 3), 
																					1536 + (32 * gRand.RandRange(0, 32)));
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 3:
						{
							// Realloc NULL to zero or small size with perm flag.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationRealloc];
							pMallocInfoArray[i].mpData = pAllocator->Realloc(NULL, 
																			  pMallocInfoArray[i].mSize,
																			  NonLocalAllocator::kAllocationFlagPerm);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 4:
						{
							// Try all flags.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 4);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize, 
																			 0xff);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 5:
						{
							// Try all flags.
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 10000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(9999, 
																			 0xff);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 6:
						{
							// Try all flags. 
							pMallocInfoArray[i].mSize  = gRand.RandRange(0, 10);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize,
																					128 << gRand.RandRange(0, 4),
																					NonLocalAllocator::kMinAlignment,
																					0xff);
							EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
							break;
						}

						case 7:
						{
							// This should fail. Size is too large.
							pMallocInfoArray[i].mSize  = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
							pMallocInfoArray[i].mpData = pAllocator->Malloc(pMallocInfoArray[i].mSize);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL);
							break;
						}

						case 8:
						{
							// This should fail. Alignment is too large, but if it does validate the pointer alignment.
							pMallocInfoArray[i].mSize = 0xffff0000 + gRand.RandRange(0, 0x1000);
							pMallocInfoArray[i].mFill = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x00100000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL || PPM_IsAligned(pMallocInfoArray[i].mpData, 0x10000000));
							break;
						}

						case 9:
						{
							// This should fail. Alignment is too large, but if it does validate the pointer alignment.
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL || PPM_IsAligned(pMallocInfoArray[i].mpData, 0x10000000));
							break;
						}

						case 10:
						{
							// This should fail. Alignment is too large (tested with a large alignment offset), but if it does validate the pointer alignment.
							pMallocInfoArray[i].mSize  = 64 + gRand.RandRange(0, 128);
							pMallocInfoArray[i].mFill  = gFill[kOperationMallocAligned];
							pMallocInfoArray[i].mpData = pAllocator->MallocAligned(pMallocInfoArray[i].mSize, 0x10000000, 0x10000000);
							EATEST_VERIFY(pMallocInfoArray[i].mpData == NULL || PPM_IsAligned(pMallocInfoArray[i].mpData, 0x10000000));
							break;
						}
					}
				}

				if(pMallocInfoArray[i].mpData && bShouldMemset)
					memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
				else
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}

		for(i = 0; i < kAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				size_t s = pAllocator->GetUsableSize(pMallocInfoArray[i].mpData);
				EATEST_VERIFY(s >= pMallocInfoArray[i].mSize);
				EATEST_VERIFY(!bShouldMemset || EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));

				pAllocator->Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestConstruction
//
int NLTestConstruction()
{
	EA::UnitTest::Report("    NLTestConstruction\n");

	int nErrorCount = 0;

	const size_t kCoreSize = 1000000;

	{
		// Test default construction and destruction.
		NonLocalAllocator nla;

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	{
		// Test default construction.
		NonLocalAllocator nla;

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
		nla.Shutdown();

		// The allocator can't be used without core, so we don't add any nor do allocation tests.
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	{
		// Test construction with core added via Init.
		NonLocalAllocator nla;

		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 1);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugReads,  1);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);

		NLInit(&nla, kCoreSize);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 0); // Intentionally use our own values below as opposed to global values.
		nErrorCount += NLTestAllocationGeneric(&nla, 0, 1000, gnAllocSlotCount / 2, gnAllocIterationCount / 2, gnAllocCycleCount, false);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
		nla.Shutdown();
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// NLTestInitWithoutCore
//
int NLTestInitWithoutCore()
{
	int kUserRequestedSize = 8192;
	EA::UnitTest::Report("    NLTestInitWithoutCore\n");
	int nErrorCount = 0;

	{
		// Test NLA will return NULL when its a default constructed.
		NonLocalAllocator nla;
		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelFull, 1);

		void* const pData1 = nla.Malloc(kUserRequestedSize);
		EATEST_VERIFY(pData1 == NULL);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
		nla.Shutdown();
	}

	{
		// Test NLA initialized with InitWithoutCore. Verify it allocates memory when core block has been added.
		NonLocalAllocator nla;
		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelFull, 1);

		nla.InitWithoutCore(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator);
		void* pData1 = nla.Malloc(kUserRequestedSize);
		EATEST_VERIFY(pData1 == NULL);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		void* const pCore2 = gpEAGeneralAllocator->MallocAligned(kUserRequestedSize, 64);
		nla.AddCore(pCore2, kUserRequestedSize, NLCoreFreeFunction, gpEAGeneralAllocator);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		pData1 = nla.Malloc(kUserRequestedSize);
		EATEST_VERIFY(pData1 != NULL);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		nla.Free(pData1);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// NLTestAddCore
//
int NLTestAddCore()
{
	EA::UnitTest::Report("    NLTestAddCore\n");

	int nErrorCount = 0;

	const size_t kCoreSize = 1000000;

	{
		// Test construction with core added via AddCore.
		NonLocalAllocator nla;

		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 1);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);

		NLInit(&nla, kCoreSize, kCoreSize);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 1); // Intentionally use our own values below as opposed to global values.
		nErrorCount += NLTestAllocationGeneric(&nla, 0, 1000, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount, false); // Max concurrent core use should be ~2,500,000 bytes

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
		nla.Shutdown();
	}

	{   // Test ExtendCore
		NonLocalAllocator nla;

		nla.SetMallocFailureFunction(NULL, NULL);

		// Malloc 16384 but give nla only 8192 of it.
		void* const pCore1 = gpEAGeneralAllocator->MallocAligned(16384, 64);
		nla.Init(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator, 
				 pCore1, 8192, NLCoreFreeFunction, gpEAGeneralAllocator);

		void* const pData1 = nla.Malloc(8192);
		EATEST_VERIFY(pData1 != NULL);

		void* const pData2 = nla.Malloc(8192);
		EATEST_VERIFY(pData2 == NULL);

		// Give nla the other 8192 of it.
		nla.ExtendCore(pCore1, 8192);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		void* const pData3 = nla.Malloc(8192);
		EATEST_VERIFY(pData3 != NULL);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		// Malloc 16384 but give nla only 8192 of it.
		void* const pCore2 = gpEAGeneralAllocator->MallocAligned(16384, 64);
		nla.AddCore(pCore2, 8192, NLCoreFreeFunction, gpEAGeneralAllocator);

		void* const pData4 = nla.Malloc(4096);
		EATEST_VERIFY(pData4 != NULL);

		void* const pData5 = nla.Malloc(8192);
		EATEST_VERIFY(pData5 == NULL);

		// Give nla the other 8192 of it.
		nla.ExtendCore(pCore2, 8192);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		void* const pData6 = nla.Malloc(8192);
		EATEST_VERIFY(pData6 != NULL);

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// NLTestTrimCore
//
int NLTestTrimCore()
{
	EA::UnitTest::Report("    NLTestTrimCore\n");

	int nErrorCount = 0;

	const size_t kCoreSize = 16384;
	size_t nFreeCount;

	{   // Test on uninitialized allocator.
		NonLocalAllocator nla;

		nFreeCount = nla.TrimCore(NULL);
		EATEST_VERIFY(nFreeCount == 0);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	{   // Test on uninitialized allocator.
		NonLocalAllocator nla;

		nFreeCount = nla.TrimCore((void*)(uintptr_t)0x00000001);
		EATEST_VERIFY(nFreeCount == 0);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	{   // Test on initialized but unused allocator.
		NonLocalAllocator nla;

		NLInit(&nla, kCoreSize, kCoreSize);

		// This should free both cores.
		nFreeCount = nla.TrimCore(NULL); 
		EATEST_VERIFY(nFreeCount == 2);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	}

	{   // Test on allocator with a bunch of memory allocated.
		NonLocalAllocator nla;

		NLInit(&nla, kCoreSize, kCoreSize);

		for(int i = 0; i < 2; i++)
		{
			void* p0 = nla.Malloc(kCoreSize / 4);
			void* p1 = nla.Malloc(kCoreSize / 4);
			void* p2 = nla.Malloc(kCoreSize / 4);
			void* p3 = nla.Malloc(kCoreSize / 4);
			void* p4 = nla.Malloc(kCoreSize / 4);

			nFreeCount = nla.TrimCore(NULL); // This should do nothing because both CoreBlocks should be busy.
			EATEST_VERIFY(nFreeCount == 0);
			EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

			nla.Free(p4);
			nla.Free(p3);
			nla.Free(p2);

			nFreeCount = nla.TrimCore(NULL);
			EATEST_VERIFY(nFreeCount == 1);
			EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

			nla.Free(p1);
			nla.Free(p0);

			nFreeCount = nla.TrimCore(NULL);
			EATEST_VERIFY(nFreeCount == 1);
			EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

			// Restore the core memory to how it was before we started freeing it.
			void* const pCore1 = gpEAGeneralAllocator->MallocAligned(kCoreSize, 64);
			nla.AddCore(pCore1, kCoreSize, NLCoreFreeFunction, gpEAGeneralAllocator);

			void* const pCore2 = gpEAGeneralAllocator->MallocAligned(kCoreSize, 64);
			nla.AddCore(pCore2, kCoreSize, NLCoreFreeFunction, gpEAGeneralAllocator);
		}
	}

	{
		// Test random calls to TrimCore.
		NonLocalAllocator nla;

		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 1);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);

		NLInit(&nla, kCoreSize, kCoreSize);
		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 1); // Intentionally use our own values below as opposed to global values.
		nErrorCount += NLTestAllocationGeneric(&nla, 0, 1000, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount, true); // Max concurrent core use should be ~2,500,000 bytes

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
		nla.Shutdown();
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestValidateAddress
//
int NLTestValidateAddress()
{
	EA::UnitTest::Report("    NLTestValidateAddress\n");

	int nErrorCount = 0;

	NonLocalAllocator nla;
	NLInit(&nla, 16384);


	// Test the allocated pointer itself.
	void* pAlloc = nla.Malloc(100, NonLocalAllocator::kAllocationFlagNone);

	const void* pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == pAlloc);

	// Test the allocated pointer + n, which is still valid, but isn't the allocated ptr itself.
	pTest = nla.ValidateAddress((char*)pAlloc + 40, NonLocalAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = nla.ValidateAddress((char*)pAlloc + 40, NonLocalAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == pAlloc);

	pTest = nla.ValidateAddress((char*)pAlloc + 40, NonLocalAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == (char*)pAlloc + 40);

	// Test the allocated pointer - n, which is not valid.
	pTest = nla.ValidateAddress((char*)pAlloc - 4, NonLocalAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = nla.ValidateAddress((char*)pAlloc - 4, NonLocalAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == NULL);

	pTest = nla.ValidateAddress(NULL, NonLocalAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest == NULL);


	// Test the freed pointer.
	nla.Free(pAlloc);

	pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeSpecific);
	EATEST_VERIFY(pTest == NULL);

	pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeAllocated);
	EATEST_VERIFY(pTest == NULL);

	pTest = nla.ValidateAddress(pAlloc, NonLocalAllocator::kAddressTypeOwned);
	EATEST_VERIFY(pTest != NULL);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestAllocation
//
// This is test that's useful for testing things that the other tests don't cover.
//
int NLTestAllocation()
{
	EA::UnitTest::Report("    NLTestAllocation\n");

	int nErrorCount = 0;

	// Test regular functionality
	{
		NonLocalAllocator nla;

		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);
		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);

		NLInit(&nla);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 2);
		nErrorCount += NLTestAllocationGeneric(&nla, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount, false);
	}

	// Test debug functionality
	{
		NonLocalAllocator nla;

		// Enable all options.
		nla.SetOption(NonLocalAllocator::kOptionEnableThreadSafety, 1);
		nla.SetOption(NonLocalAllocator::kOptionEnableHighAllocation, 1);
		nla.SetOption(NonLocalAllocator::kOptionMaxMallocFailureCount, 2);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);
		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);

		// Enable all debug flags.
		unsigned flags = 0;
		for(int i = 0; i < NonLocalAllocator::kDebugDataIdInternalCount; i++)
			flags |= (1 << i);
		nla.SetDefaultDebugDataFlags(flags);

		NLInit(&nla);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 3);
		nErrorCount += NLTestAllocationGeneric(&nla, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount, false);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestPathological
//
int NLTestPathological()
{
	EA::UnitTest::Report("    NLTestPathological\n");

	int nErrorCount = 0;

	// Test regular functionality
	{
		NonLocalAllocator nla;

		NLInit(&nla);

		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);
		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunctionNop, &nla);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 4);
		nErrorCount += NLTestAllocationPathological(&nla);
	}

	// Test debug functionality
	{
		NonLocalAllocator nla;

		NLInit(&nla);

		// Enable all options.
		nla.SetOption(NonLocalAllocator::kOptionEnableThreadSafety, 1);
		nla.SetOption(NonLocalAllocator::kOptionEnableHighAllocation, 1);
		nla.SetOption(NonLocalAllocator::kOptionMaxMallocFailureCount, 2);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);
		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunctionNop, &nla);

		// Enable all debug flags.
		unsigned flags = 0;
		for(int i = 0; i < NonLocalAllocator::kDebugDataIdInternalCount; i++)
			flags |= (1 << i);
		nla.SetDefaultDebugDataFlags(flags);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 5);
		nErrorCount += NLTestAllocationPathological(&nla);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestThreading
//
#if PPM_TEST_THREADING_ENABLED
	struct NLThreadTestInfo
	{
	    NonLocalAllocator* mpAllocator;
	    EA::Thread::AtomicInt32 mnThreadCount;
	    EA::Thread::AtomicInt32 mnErrorCount;
	    bool mbTrimCore;
		bool mbHighFrequencyTrimMode;

	    NLThreadTestInfo(NonLocalAllocator* pAllocator = NULL,
	                     int32_t nThreadCount = 0,
	                     bool bTrimCore = false,
	                     bool bHighFreqTrimMode = false)
	        : mpAllocator(pAllocator)
	        , mnThreadCount(nThreadCount)
	        , mnErrorCount(0)
	        , mbTrimCore(bTrimCore)
	        , mbHighFrequencyTrimMode(bHighFreqTrimMode)
	    {
	    }
    };

	static intptr_t NLTestThreadingThreadFunction(void* pContext)
	{
		NLThreadTestInfo* const pThreadTestInfo = (NLThreadTestInfo*)pContext;

	    pThreadTestInfo->mnErrorCount += NLTestAllocationGeneric(
	        pThreadTestInfo->mpAllocator, gnAllocSizeMin, gnAllocSizeMax, gnAllocSlotCount, gnAllocIterationCount,
	        gnAllocCycleCount, pThreadTestInfo->mbTrimCore, pThreadTestInfo->mbHighFrequencyTrimMode);

	    pThreadTestInfo->mnThreadCount--;

		pThreadTestInfo->mpAllocator->Lock(true); // Just to exercise this function.
		pThreadTestInfo->mpAllocator->Lock(false);

		return 0;
	}
#endif



#if PPM_TEST_THREADING_ENABLED
	int TestNonLocalAllocatorMultiThreaded(bool bTrimCore, bool bHighFrequencyTrim = false)
	{
		const int numThreads = 
			#ifdef EA_PLATFORM_SONY
				6
			#else
				4
			#endif
		;

		EA::Thread::Thread thread;
		NonLocalAllocator  nla;

		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);

		NLInit(&nla);
		NLThreadTestInfo threadTestInfo(&nla, numThreads, bTrimCore, bHighFrequencyTrim);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 6);

		for(int j = 0, jEnd = threadTestInfo.mnThreadCount; j < jEnd; j++)
			thread.Begin(NLTestThreadingThreadFunction, &threadTestInfo);

		while(threadTestInfo.mnThreadCount.GetValue() > 0)
			EA::Thread::ThreadSleep(500);

		return threadTestInfo.mnErrorCount.GetValue();
	}
#endif


int NLTestThreading()
{
	EA::UnitTest::Report("    NLTestThreading\n");

	int nErrorCount = 0;

	#if PPM_TEST_THREADING_ENABLED
		nErrorCount += TestNonLocalAllocatorMultiThreaded(false);          // Do not call TrimCore
		nErrorCount += TestNonLocalAllocatorMultiThreaded(true, false);    // Call TrimCore with low probability
		for(int i = 10; i; i--)
		{
			EA::UnitTest::Report(".");
			nErrorCount += TestNonLocalAllocatorMultiThreaded(true, true); // Call TrimCore with high probability
		}
		EA::UnitTest::Report("\n");
	#else
		EA::UnitTest::ReportVerbosity(1, "NLTestThreading: test not currently implmented for this platform/compiler.\n");
		// For platforms that aren't pre-emptive, we'll need to have the test call
		// a yield function in order to properly test multiple threads.
	#endif

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestAllocationFlags
//
int NLTestAllocationFlags()
{
	EA::UnitTest::Report("    NLTestAllocationFlags\n");

	int nErrorCount = 0;

	{
		// Regression to exercize new logic we wrote in MallocNodeAligned to find kAllocationFlagBestFit blocks via 'bBestFitTried' and 'bNonBestFitTried'.
		// The easiest way to test this is to know what the bin sizes are for free blocks and request an allocation size that's just below the max for 
		// some bin. From EANonLocalAllocator.cpp:
		//      Map 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, etc.
		//      to bins:
		//           0,   1,   2,   3,    4,    5,    6,    7,     8, etc.
		NonLocalAllocator nla;

		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetOption(NonLocalAllocator::kOptionEnableHighAllocation, 0);
		nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunction, &nla);
		nla.SetOption(NonLocalAllocator::kOptionFailureValue, (intptr_t)kFailureValue);
		nla.Init(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator, pFakeCore, 4096);

		void* p;
		void* pTemp;

		// The following should initially fail in the kAllocationFlagBestFit loop and then succeed
		// in the kAllocationFlagNone loop.
		pTemp = nla.Malloc(16);
		EATEST_VERIFY(pTemp != kFailureValue);
		p = nla.MallocAligned(1024, 2048, 0, NonLocalAllocator::kAllocationFlagBestFit);
		EATEST_VERIFY(p != kFailureValue);
		if(p != kFailureValue)
			nla.Free(p);
		nla.Free(pTemp);

		// The following will cause the code to initially fail to find a block in the first loop,  
		// but then find it when switching to kAllocationFlagBestFit for the next time through the loop;
		p = nla.MallocAligned(4096, 2048, 0, NonLocalAllocator::kAllocationFlagNone);
		EATEST_VERIFY(p != kFailureValue);
		if(p != kFailureValue)
			nla.Free(p);
	}

	{
		NonLocalAllocator nla;

		nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
		nla.SetOption(NonLocalAllocator::kOptionEnableHighAllocation, 1);

		NLInit(&nla);

		gRand.SetSeed(EA::UnitTest::GetRandSeed() + 7);

		// Test AllocationFlags
		MallocInfo* pMallocInfoArray = new MallocInfo[gnAllocSlotCount];
		memset(pMallocInfoArray, 0, sizeof(MallocInfo) * gnAllocSlotCount);

		int nFlagOptions[4] = 
		{ 
			NonLocalAllocator::kAllocationFlagNone,
			NonLocalAllocator::kAllocationFlagHigh,
			NonLocalAllocator::kAllocationFlagEndFit,
			NonLocalAllocator::kAllocationFlagHigh | NonLocalAllocator::kAllocationFlagEndFit
		};

		#if PPM_NULL_POINTER_FREE_ENABLED
			nla.Free(NULL); // Make sure Free of NULL pointer works
		#endif

		// Run tests
		for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
		{
			size_t i;

			for(size_t j(0); (j < gnAllocIterationCount) && !gAssertionFailureOccurred; j++) // For each iteration...
			{
				if((j % 1000) == 0)
					EA::UnitTest::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, gnAllocIterationCount, c, gnAllocCycleCount);

				i = gRand.RandRange(0, (int)gnAllocSlotCount);

				const size_t nOperation = gRand.RandRange(0, kOperationCount);         // 0, 1, 2, 3, or 4.
					  size_t nSize      = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
				const int    nFlags     = nFlagOptions[gRand.RandRange(0, 4)];
				const size_t nAlignment = NonLocalAllocator::kMinAlignment * gRand.RandRange(0, 2);  // 0, 16, or 32.
				const size_t nOffset    = NonLocalAllocator::kMinAlignment * gRand.RandRange(0, 2);  // 0, 16, or 32.

				if(pMallocInfoArray[i].mpData)
				{
					// Test freeing of memory
					EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
					nla.Free(pMallocInfoArray[i].mpData);
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
							pMallocInfoArray[i].mpData = nla.Malloc(pMallocInfoArray[i].mSize, nFlags);
							EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "NLTestAllocationFlags error: Failed to allocate memory.\n");

							if(pMallocInfoArray[i].mpData)
								memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
							else
								memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

							break;
						}

						case kOperationMallocMultiple:
						{
							void*  pPtrArray[32];
							size_t sizeArray[32];
							size_t alignmentArray[32];
							size_t alignmentOffsetArray[32];
							size_t n = gRand.RandRange(0, 32);

							if((i + n) > gnAllocSlotCount)
								i = (gnAllocSlotCount - n);

							memset(pPtrArray,            0, sizeof(pPtrArray));
							memset(sizeArray,            0, sizeof(sizeArray));
							memset(alignmentArray,       0, sizeof(alignmentArray));
							memset(alignmentOffsetArray, 0, sizeof(alignmentOffsetArray));

							for(size_t q = 0; q < n; q++)
							{
								sizeArray[q]            = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
								alignmentArray[q]       = gRand.RandRange(0, 2) * 16;
								alignmentOffsetArray[q] = gRand.RandRange(0, 2) * 16;
							}

							void** const pResult = nla.MallocMultiple(n, sizeArray, alignmentArray, alignmentOffsetArray, pPtrArray, 0);

							if(pResult)
							{
								for(size_t s = 0; s < n; s++)
								{
									if(pMallocInfoArray[i].mpData)
									{
										EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
										nla.Free(pMallocInfoArray[i].mpData);
										memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
									}

									pMallocInfoArray[i].mSize  = sizeArray[s];
									pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
									pMallocInfoArray[i].mpData = pPtrArray[s];
									EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "NLTestAllocationFlags error: Failed to allocate memory.\n");

									if(pMallocInfoArray[i].mpData)
										memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
									else
										memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
								}
							}

							break;
						}

						case kOperationRealloc:
						{
							pMallocInfoArray[i].mSize  = nSize;
							pMallocInfoArray[i].mFill  = gFill[kOperationRealloc];
							pMallocInfoArray[i].mpData = nla.Realloc(pMallocInfoArray[i].mpData, nSize, nFlags);
							EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "NLTestAllocationFlags error: Failed to allocate memory.\n");

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
							pMallocInfoArray[i].mpData = nla.MallocAligned(pMallocInfoArray[i].mSize, nAlignment, nOffset, nFlags | NonLocalAllocator::kAllocationFlagBestFit);
							EATEST_VERIFY_MSG(pMallocInfoArray[i].mpData != NULL, "NLTestAllocationFlags error: Failed to allocate memory.\n");

							if(pMallocInfoArray[i].mpData)
								memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
							else
								memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

							break;
						}
					}
				}
			}

			for(i = 0; i < gnAllocSlotCount; i++)
			{
				if(pMallocInfoArray[i].mpData)
				{
					EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
					nla.Free(pMallocInfoArray[i].mpData);
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
				}
			}
		}

		delete[] pMallocInfoArray;
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestDebugNaming
//
// Tests named allocations
//
int NLTestDebugNaming()
{
	EA::UnitTest::Report("    NLTestDebugNaming\n");

	int nErrorCount = 0;
	NonLocalAllocator nla;

	nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	nla.SetOption(NonLocalAllocator::kOptionEnableHighAllocation, 1);

	NLInit(&nla);

	gRand.SetSeed(EA::UnitTest::GetRandSeed() + 8);

	// Test AllocationFlags
	MallocInfo* pMallocInfoArray = new MallocInfo[gnAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * gnAllocSlotCount);

	int nFlagOptions[4] = 
	{ 
		NonLocalAllocator::kAllocationFlagNone,
		NonLocalAllocator::kAllocationFlagHigh,
		NonLocalAllocator::kAllocationFlagEndFit,
		NonLocalAllocator::kAllocationFlagHigh | NonLocalAllocator::kAllocationFlagEndFit
	};

	// Run tests
	for(size_t c(0); c < gnAllocCycleCount; c++) // For each cycle...
	{
		size_t i;

		for(size_t j(0); (j < gnAllocIterationCount) && !gAssertionFailureOccurred; j++) // For each iteration...
		{
			if((j % 1000) == 0)
				EA::UnitTest::ReportVerbosity(1, "Iteration %d of %d; Cycle %d of %d\n", j, gnAllocIterationCount, c, gnAllocCycleCount);

			i = gRand.RandRange(0, (int)gnAllocSlotCount);

			if(pMallocInfoArray[i].mpData)
			{
				#if PPM_NLA_DEBUG_DATA_SUPPORTED
					// Verify name is as intended
					size_t nRequestedSize = 0xffffffff;
					char   pStoredName[256] = { 0 };
					char   pExpectedName[256] = { 0 };

					nla.GetDebugData(pMallocInfoArray[i].mpData, NonLocalAllocator::kDebugDataIdSize, &nRequestedSize, sizeof(nRequestedSize));
					nla.GetDebugData(pMallocInfoArray[i].mpData, NonLocalAllocator::kDebugDataIdName, &pStoredName, sizeof(pStoredName)/sizeof(pStoredName[0]));
					EA::StdC::Sprintf(pExpectedName, "%u", (unsigned)nRequestedSize);
					if(strcmp(pExpectedName, pStoredName) != 0)
					{
						EA::UnitTest::ReportVerbosity(1, "Expected: %s Stored: %s\n", pExpectedName, pStoredName);
						EATEST_VERIFY(strcmp(pExpectedName, pStoredName) == 0);
					}
				#endif

				// Free the space.
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
				nla.Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
			else
			{
				const size_t   nOperation  = (gRand.RandRange(0, 2) == 0) ? kOperationMalloc : kOperationMallocAligned;            // 0, 1.
				const size_t   nSize       = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
				const int      nFlags      = nFlagOptions[gRand.RandRange(0, 4)];
				const size_t   nAlignment  = gRand.RandLimit(2) ? (size_t)0 : (size_t)(NonLocalAllocator::kMinAlignment << gRand.RandRange(0, 3)); // 0, 16, 32, 64.
				const size_t   nOffset     = gRand.RandRange(0, 5) * NonLocalAllocator::kMinAlignment;     // 0, 16, 24, 32.
				const size_t   bFileLine   = gRand.RandRange(0, 2);                         // 0, 1.
				const unsigned nDebugFlags = (1 << NonLocalAllocator::kDebugDataIdSize) | (1 << NonLocalAllocator::kDebugDataIdFlags);

				char pName[256];
				EA::StdC::Sprintf(pName, "%u", (unsigned)nSize);

				switch (nOperation)
				{
					case kOperationMalloc:
						pMallocInfoArray[i].mSize  = nSize;
						pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];

						if(bFileLine)
							pMallocInfoArray[i].mpData = nla.MallocDebug(nSize, nFlags, nDebugFlags, pName, __FILE__, __LINE__);
						else
							pMallocInfoArray[i].mpData = nla.MallocDebug(nSize, nFlags, nDebugFlags, pName);
						break;

					default:
						pMallocInfoArray[i].mSize  = nSize;
						pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];

						if(bFileLine)
							pMallocInfoArray[i].mpData = nla.MallocAlignedDebug(nSize, nAlignment, nOffset, nFlags, nDebugFlags, pName, __FILE__, __LINE__);
						else
							pMallocInfoArray[i].mpData = nla.MallocAlignedDebug(nSize, nAlignment, nOffset, nFlags, nDebugFlags, pName);
						break;
				}

				EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);
				if(pMallocInfoArray[i].mpData)
					memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
				else
					memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));                    
			}
		}

		for(i = 0; i < gnAllocSlotCount; i++)
		{
			if(pMallocInfoArray[i].mpData)
			{
				EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
				nla.Free(pMallocInfoArray[i].mpData);
				memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
			}
		}
	}

	delete[] pMallocInfoArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestDebugGroupId
//
// Tests kDebugDataIdGroup.
//
int NLTestDebugGroupId()
{
	EA::UnitTest::Report("    NLTestDebugGroupId\n");

	size_t i;
	int nErrorCount = 0;

	NonLocalAllocator nla;
	nla.SetAutoHeapValidation((NonLocalAllocator::HeapValidationLevel)gnAutoHeapValidationLevel, gnAutoHeapValidationFrequency);
	nla.SetDefaultDebugDataFlag(NonLocalAllocator::kDebugDataIdGroup);

	NLInit(&nla);
	
	gRand.SetSeed(EA::UnitTest::GetRandSeed() + 9);

	MallocInfo* pMallocInfoArray = new MallocInfo[gnAllocSlotCount];
	memset(pMallocInfoArray, 0, sizeof(MallocInfo) * gnAllocSlotCount);


	// Allocate memory, assigning different groups.
	for(i = 0; (i < gnAllocSlotCount) && !gAssertionFailureOccurred; i++)
	{
		const size_t nSize           = gRand.RandRange((int)gnAllocSizeMin, (int)gnAllocSizeMax);
		const bool   bDisableGroupId = ((i % 20) == 0);

		nla.SetCurrentGroupId((int)(i % 2)); // Every other allocation belongs to the same group.

		if(bDisableGroupId)
			nla.SetDefaultDebugDataFlags(nla.GetDefaultDebugDataFlags() & ~(1 << NonLocalAllocator::kDebugDataIdGroup));

		pMallocInfoArray[i].mSize  = nSize;
		pMallocInfoArray[i].mFill  = gFill[kOperationMalloc];
		pMallocInfoArray[i].mpData = nla.Malloc(nSize);
		EATEST_VERIFY(pMallocInfoArray[i].mpData != NULL);

		if(pMallocInfoArray[i].mpData)
			memset(pMallocInfoArray[i].mpData, pMallocInfoArray[i].mFill, pMallocInfoArray[i].mSize);
		else
			memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));

		if(bDisableGroupId)
			nla.SetDefaultDebugDataFlag(NonLocalAllocator::kDebugDataIdGroup);
	}


	// Report existing memory
	NonLocalAllocator::ReportContext reportContext;
	nla.ReportBegin(&reportContext, NonLocalAllocator::kBlockTypeAllocated);

	for(const NonLocalAllocator::BlockInfo* pBlockInfo = nla.ReportNext(&reportContext); 
		 pBlockInfo; 
		 pBlockInfo = nla.ReportNext(&reportContext))
	{
		int    nGroupId;                // Actual found group id for allocation.
		size_t nIndex(0);               // Original index for allocation.
		bool   bDisableGroupId = false; // Original value for disabling.

		for(i = 0; i < gnAllocSlotCount; i++) // Find out which index this block is.
		{
			if(pMallocInfoArray[i].mpData == pBlockInfo->mpData)
			{
				nIndex = i;
				bDisableGroupId = ((i % 20) == 0);
				break;
			}
		}

		if(i < 100) // If the block was one of ours...
		{
			if(nla.GetDebugData(pBlockInfo->mpData, NonLocalAllocator::kDebugDataIdGroup, &nGroupId, sizeof(nGroupId)))
			{
				EATEST_VERIFY_MSG(!bDisableGroupId,              "TestDebugGroupId: Group id was found but shouldn't have been.\n");
				EATEST_VERIFY_MSG(((int)nIndex % 2) == nGroupId, "TestDebugGroupId: Group id was incorrect.\n");
			}
			else
			{
				#if PPM_NLA_DEBUG_DATA_SUPPORTED // If it's expected to work...
					EATEST_VERIFY_MSG(bDisableGroupId, "TestDebugGroupId: Group id wasn't found but should have been.\n");
				#endif
			}
		}
		else
		{
			// Verify that GetDebugData works OK (regardless of return value) for any block, including those we may not have allocated.
			nla.GetDebugData(pBlockInfo->mpData, NonLocalAllocator::kDebugDataIdGroup, &nGroupId, sizeof(nGroupId));
		}
	}
	nla.ReportEnd(&reportContext);


	// Free all memory
	for(i = 0; (i < gnAllocSlotCount) && !gAssertionFailureOccurred; i++)
	{
		if(pMallocInfoArray[i].mpData)
		{
			EATEST_VERIFY(EA::Allocator::MemoryFillCheck((uint8_t*)pMallocInfoArray[i].mpData, pMallocInfoArray[i].mSize, pMallocInfoArray[i].mFill));
			nla.Free(pMallocInfoArray[i].mpData);
			memset(&pMallocInfoArray[i], 0, sizeof(pMallocInfoArray[i]));
		}
	}


	delete[] pMallocInfoArray;

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTestReportHeap
//
static bool NLDescribeBlock(const NonLocalAllocator::BlockInfo* pBlockInfo, void* /*pContext*/)
{
	const char* blockType = "Unknown";

	switch(pBlockInfo->mBlockType)
	{
		case NonLocalAllocator::kBlockTypeNone:
			blockType = "None";
			break;
		case NonLocalAllocator::kBlockTypeInvalid:
			blockType = "Invalid";
			break;
		case NonLocalAllocator::kBlockTypeAllocated:
			blockType = "Allocated";
			break;
		case NonLocalAllocator::kBlockTypeFree:
			blockType = "Free";
			break;
		case NonLocalAllocator::kBlockTypeCore:
			blockType = "Core";
			break;
	}

	EA::UnitTest::ReportVerbosity(1, "Block type: %12.12s addr: 0x%08x, size: %8d, data addr: 0x%08x, data size: %8d\n", 
						blockType, (unsigned)(uintptr_t)pBlockInfo->mpNode, (unsigned)pBlockInfo->mnSize, 
						(unsigned)(uintptr_t)pBlockInfo->mpData, (unsigned)pBlockInfo->mnDataSize);
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// NLValidateReportCounts
//
int  NLValidateReportCounts(NonLocalAllocator& nla, int expectedAllocatedCount, int expectedFreeCount, int expectedCoreCount, int nBlockTypeFlags = 0xff)
{
	int nErrorCount = 0;
	NonLocalAllocator::ReportContext reportContext;
	nla.ReportBegin(&reportContext, nBlockTypeFlags);

	int blockTypeCounts[NonLocalAllocator::BlockType::kBlockTypeAll] = { 0 };
	for (const NonLocalAllocator::BlockInfo* pBlockInfo = nla.ReportNext(&reportContext); pBlockInfo; pBlockInfo = nla.ReportNext(&reportContext))
	{
		NLDescribeBlock(pBlockInfo, NULL);
		blockTypeCounts[static_cast<unsigned int>(pBlockInfo->mBlockType)]++;
	}

	EATEST_VERIFY_MSG(blockTypeCounts[NonLocalAllocator::BlockType::kBlockTypeAllocated] == expectedAllocatedCount, "Unexpected kBlockTypeAllocated from report.");
	EATEST_VERIFY_MSG(blockTypeCounts[NonLocalAllocator::BlockType::kBlockTypeFree] == expectedFreeCount, "Unexpected kBlockTypeFree from report.");
	EATEST_VERIFY_MSG(blockTypeCounts[NonLocalAllocator::BlockType::kBlockTypeCore] == expectedCoreCount, "Unexpected kBlockTypeCore from report.");

	nla.ReportEnd(&reportContext);

	return nErrorCount;
}


int NLTestReportHeap()
{
	EA::UnitTest::Report("    NLTestReportHeap\n");

	int nErrorCount = 0;

	{
		NonLocalAllocator nla;
		nla.SetDefaultDebugDataFlag(NonLocalAllocator::kDebugDataIdCallStack);

		NLInit(&nla, 1024*1024);


		// Allocate some blocks
		void* p0 = nla.MallocDebug(128, 0, 0, NULL, __FILE__, __LINE__);
		void* p1 = nla.Malloc(256);
		void* p2 = nla.Malloc(64);
		void* p3 = nla.MallocDebug(73, 0, 1 << NonLocalAllocator::kDebugDataIdAllocationTime, "Test", __FILE__, __LINE__);
		void* p4 = nla.Malloc(512);
		void* p5 = nla.Malloc(514);
		void* p6 = nla.Malloc(88888);
		void* p7 = nla.Malloc(4);
		void* p8 = nla.MallocDebug(644664, 0, (1 << NonLocalAllocator::kDebugDataIdInternalCount) | (1 << NonLocalAllocator::kDebugDataIdOverhead), NULL, __FILE__, __LINE__);
		void* p9 = nla.Malloc(12);


		// Free a few blocks.
		nla.Free(p3);
		nla.Free(p5);
		nla.Free(p6);

		// Do an allocated memory report.
		if(EA::UnitTest::GetVerbosity() >= 1)
			nla.TraceAllocatedMemory();

		// Call GetLargestFreeBlock.
		const size_t n = nla.GetLargestFreeBlock();
		EATEST_VERIFY(n > 0);

		// Test ReportHeap.
		nla.ReportHeap(NLDescribeBlock, NULL, 0xff);

		// Validate the NonLocalAllocator Report
		nErrorCount += NLValidateReportCounts(nla, 7, 3, 1);

		// Validate block type masks
		nErrorCount += NLValidateReportCounts(nla, 7, 3, 0, (NonLocalAllocator::BlockType::kBlockTypeAll & ~NonLocalAllocator::BlockType::kBlockTypeCore));
		nErrorCount += NLValidateReportCounts(nla, 7, 0, 1, (NonLocalAllocator::BlockType::kBlockTypeAll & ~NonLocalAllocator::BlockType::kBlockTypeFree));
		nErrorCount += NLValidateReportCounts(nla, 0, 3, 1, (NonLocalAllocator::BlockType::kBlockTypeAll & ~NonLocalAllocator::BlockType::kBlockTypeAllocated));
		nErrorCount += NLValidateReportCounts(nla, 0, 0, 1, (NonLocalAllocator::BlockType::kBlockTypeAll & ~(NonLocalAllocator::BlockType::kBlockTypeFree | NonLocalAllocator::BlockType::kBlockTypeAllocated)));
		nErrorCount += NLValidateReportCounts(nla, 7, 3, 0, (NonLocalAllocator::BlockType::kBlockTypeFree | NonLocalAllocator::BlockType::kBlockTypeAllocated));
		nErrorCount += NLValidateReportCounts(nla, 7, 0, 0, NonLocalAllocator::BlockType::kBlockTypeAllocated);
		nErrorCount += NLValidateReportCounts(nla, 0, 0, 0, 0x0);
		

		// Free the rest of the blocks.
		nla.Free(p0);
		nla.Free(p1);
		nla.Free(p2);
		nla.Free(p4);
		nla.Free(p7);
		nla.Free(p8);
		nla.Free(p9);

		nErrorCount += NLValidateReportCounts(nla, 0, 1, 1); // free blocks coalesed into a single block.
	}

	{
		const int CoreSize = 1024 * 1024;

		NonLocalAllocator nla;
		nla.SetDefaultDebugDataFlag(NonLocalAllocator::kDebugDataIdCallStack);

		NLInit(&nla, CoreSize, CoreSize); // allocate 2 core blocks
		nErrorCount += NLValidateReportCounts(nla, 0, 2, 2);

		void* p1 = nla.Malloc(256);
		nErrorCount += NLValidateReportCounts(nla, 1, 2, 2); // consume part of a core block

		void* p2 = nla.Malloc(CoreSize - 256);
		nErrorCount += NLValidateReportCounts(nla, 2, 1, 2); // validate we've concume a single core block.

		void* blocksToFree[] = {
		    nla.Malloc(256),
		    nla.Malloc(2),
		    nla.Malloc(13),
		    nla.Malloc(46500),
		    nla.Malloc(627),
		    nla.Malloc(1283),
		    nla.Malloc(999895),
		};
		nErrorCount += NLValidateReportCounts(nla, 9, 0, 2); // validate both core blocks are consumed.

		for(size_t i = 0; i < EAArrayCount(blocksToFree); i++)
			nla.Free(blocksToFree[i]);

		nErrorCount += NLValidateReportCounts(nla, 2, 1, 2); // validate a single core block has been released.

		nla.Free(p2);
		nla.Free(p1);
		nErrorCount += NLValidateReportCounts(nla, 0, 2, 2); // validate the remaining core has been released.
	}

	return nErrorCount;
}






///////////////////////////////////////////////////////////////////////////////
// NLTestNonLocal
//
// Tests using NonLocalAllocator to test allocation from a file as opposed to 
// allocation from memory.
//
bool NLMallocFailureFunctionFile(NonLocalAllocator* pNonLocalAllocator, size_t /*nMallocRequestedSize*/, size_t nAllocatorRequestedSize, void* /*pContext*/)
{
	if(nAllocatorRequestedSize < (1000000 / 65536 * 65536))
	   nAllocatorRequestedSize = (1000000 / 65536 * 65536);

	return pNonLocalAllocator->ExtendCore(pFakeCore, nAllocatorRequestedSize);
}

int NLTestNonLocal()
{
	EA::UnitTest::Report("    NLTestNonLocal\n");

	int nErrorCount = 0;

	NonLocalAllocator nla;

	// What we do here is give the allocator invalid memory to work with. We should entirely
	// be able to work with this invalid memory as if it were valid, as the purpose of 
	// NonLocalAllocator is to be able to work with memory that is in fact entirely inaccessible.
	nla.SetMallocFailureFunction(NULL, NULL);
	nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunction, &nla);
	nla.SetMemcpyFunction(NULL);
	nla.SetMemsetFunction(NULL);
	nla.SetMemoryFillCheckFunction(NULL);
	nla.SetOption(NonLocalAllocator::kOptionFailureValue, (intptr_t)kFailureValue);
	nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 0);  // Intentionally set this to zero, as we are working with invalid memory.
	nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 0);
	nla.Init(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator, pFakeCore, 65536, NULL, NULL);

	gAssertionFailureOccurred = false;
	gRand.SetSeed(EA::UnitTest::GetRandSeed() + 10); // Intentionally use our own values below as opposed to global values.
	NLTestAllocationGeneric(&nla, 0, 1000, gnAllocSlotCount, gnAllocIterationCount, gnAllocCycleCount, false);

	EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// NLTest64Bit
//
// Tests using NonLocalAllocator to test allocation from a file as opposed to 
// allocation from memory.
//
int NLTest64Bit()
{
	int nErrorCount = 0;

	#if (EA_PLATFORM_PTR_SIZE >= 8) || defined(_WIN64)
		EA::UnitTest::Report("    NLTest64Bit\n");

		const size_t k4TB  = (uint64_t)1024 * 1024 * 1024 * 1024 * 4;
		const size_t k5TB  = (uint64_t)1024 * 1024 * 1024 * 1024 * 5;

		NonLocalAllocator nla;

		// What we do here is give the allocator invalid memory to work with. We should entirely
		// be able to work with this invalid memory as if it were valid, as the purpose of 
		// NonLocalAllocator is to be able to work with memory that is in fact entirely inaccessible.
		nla.SetMallocFailureFunction(NULL, NULL);
		nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunction, &nla);
		nla.SetMemcpyFunction(NULL);
		nla.SetMemsetFunction(NULL);
		nla.SetMemoryFillCheckFunction(NULL);
		nla.SetOption(NonLocalAllocator::kOptionFailureValue, (intptr_t)kFailureValue);
		nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 0);  // Intentionally set this to zero, as we are working with invalid memory.
		nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 0);

		bool bResult = nla.Init(NLNodeAllocationFunction, NLNodeFreeFunction, gpEAGeneralAllocator, pFakeCore, k5TB, NULL, NULL);
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			void* pHuge = nla.Malloc(k4TB);
			EATEST_VERIFY(pHuge != kFailureValue);

			if(pHuge != kFailureValue)
				nla.Free(pHuge);

			gAssertionFailureOccurred = false;
			NLTestAllocationGeneric(&nla, 0, 10000000, 400, 1500, 2, false);
		}

		EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));
	#endif

	return nErrorCount;
}
///////////////////////////////////////////////////////////////////////////////
// NLTestSplit
//
int NLTestSplit()
{
	int nErrorCount = 0;

	EA::UnitTest::Report("    NLTestSplit\n");

	const size_t MAX_CORE_SIZE = 1u << 24;

	NonLocalAllocator nla;
	nla.SetAssertionFailureFunction(NLAssertionFailureInfoFunction, &nla);
	nla.SetAutoHeapValidation(NonLocalAllocator::kHeapValidationLevelDetail, 1);
	nla.SetOption(NonLocalAllocator::kOptionFailureValue, (intptr_t)kFailureValue);
	nla.SetOption(NonLocalAllocator::kOptionEnableDebugWrites, 1);
	NLInit(&nla, MAX_CORE_SIZE);
	EATEST_VERIFY(nla.GetLargestFreeBlock() == MAX_CORE_SIZE);

	// Allocate the entire core block
	void* p1 = nla.Malloc(MAX_CORE_SIZE);
	EATEST_VERIFY(p1 != nullptr);
	EATEST_VERIFY(nla.GetLargestFreeBlock() == 0);

	// Incrementally split and free the remainding blocks.
	size_t sizeFreed = 0;
	size_t coreSize = MAX_CORE_SIZE;
	while ((coreSize >>= 1) != 0)	
	{
		void* p2 = nla.Split(p1, coreSize);
		nla.Free(p2);
		sizeFreed += coreSize;

		EATEST_VERIFY(nla.GetLargestFreeBlock() == sizeFreed);		
	}

	nla.Free(p1);
	EATEST_VERIFY(nla.GetLargestFreeBlock() == MAX_CORE_SIZE);	
	EATEST_VERIFY(nla.ValidateHeap(NonLocalAllocator::kHeapValidationLevelFull));

	// Attempt to allocate the entire core block again after all the splits have been released.
	void* p3 = nla.Malloc(MAX_CORE_SIZE);
	EATEST_VERIFY(p3 != nullptr);
	EATEST_VERIFY(nla.GetLargestFreeBlock() == 0);
	nla.Free(p3);
	EATEST_VERIFY(nla.GetLargestFreeBlock() == MAX_CORE_SIZE);	

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestNonLocalAllocator
//
int TestNonLocalAllocator()
{
	int nErrorCount = 0;
	
	nErrorCount += NLTestThreading();
	nErrorCount += NLTest64Bit();
	nErrorCount += NLTestNonLocal();
	nErrorCount += NLTestConstruction();
	nErrorCount += NLTestInitWithoutCore();
	nErrorCount += NLTestAddCore();
	nErrorCount += NLTestTrimCore();
	nErrorCount += NLTestValidateAddress();
	nErrorCount += NLTestAllocation();
	nErrorCount += NLTestPathological();
	nErrorCount += NLTestAllocationFlags();
	nErrorCount += NLTestDebugNaming();
	nErrorCount += NLTestDebugGroupId();
	nErrorCount += NLTestReportHeap();
	nErrorCount += NLTestSplit();

	return nErrorCount;
}






