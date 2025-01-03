///////////////////////////////////////////////////////////////////////////////
// TestStackAllocator
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <PPMalloc/EAStackAllocator.h>


class TestAllocator: public EA::Allocator::StackAllocator
{
public:
	size_t GetBookmarkSpace()
	{
		return (((sizeof(Bookmark) + kMinAlignmentMask) & kMinAlignmentMaskInverse)
			* kMaxEquivalentBookmarkCount);
	}
};

void* NullStackAllocationFunction(size_t /*nSize*/, size_t* /*pSizeResult*/, void* /*pContext*/)
{
	return NULL;
}

void NullStackFreeFunction(void* /*pData*/, void* /*pContext*/)
{
}



int TestStackAllocator()
{
	using namespace EA::Allocator;

	int nErrorCount = 0;

	size_t  nAllocSizeMin(gnAllocSizeMin);
	size_t  nAllocSizeMax(gnAllocSizeMax);
	size_t  nAllocIterationCount(gnAllocIterationCount);

	{  // Basic functionality test
		StackAllocator memoryStack;
		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		size_t nMinClearSize(100000);
		size_t nMaxClearSize(1000000);

		for(size_t i(0); i < nAllocIterationCount; i++)
		{
			const size_t nClearSize = gRand.RandRange((int)nMinClearSize, (int)nMaxClearSize);
			size_t nCurrentSize(0);

			while(nCurrentSize < nClearSize)
			{
				void* pResult;

				const size_t nAllocSize            = gRand.RandRange((int)nAllocSizeMin, (int)nAllocSizeMax);
				const size_t nAllocAlignment       = (gRand.RandLimit(10) == 0) ? (1 << gRand.RandLimit(8)) : 0;
				const size_t nAllocAlignmentOffset = nAllocAlignment ? (gRand.RandLimit(8) * 16) : 0;

				if(nAllocAlignment)
					pResult = memoryStack.MallocAligned(nAllocSize, nAllocAlignment, nAllocAlignmentOffset, true);
				else
					pResult = memoryStack.Malloc(nAllocSize, true);
				pResult = memset(pResult, 0x33, nAllocSize);

				nCurrentSize += nAllocSize;
			}

			memoryStack.FreeObjects(NULL);
		}

		memoryStack.Shutdown();
	}


	{ // Error handling test
		StackAllocator memoryStack(NULL, (size_t)-1, NullStackAllocationFunction, NullStackFreeFunction, NULL);

		void* pResult = memoryStack.MallocAligned(100, 64, 0, true);
		EATEST_VERIFY(pResult == NULL);

		pResult = memoryStack.Malloc(100, true);
		EATEST_VERIFY(pResult == NULL);

		memoryStack.FreeObjects(NULL);
		memoryStack.Shutdown();
	}


	{ // Basic bookmark test
		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		StackAllocator memoryStack;
		const float    systemSpeed    = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeCPU);
		const int      iterationCount = (int)(10 * systemSpeed);

		for(int i = 0; i < iterationCount; i++)
		{
			const size_t   kBlockSize       = 1024 * gRand.RandRange(1, 8);
			const size_t   kMaxMallocVolume = 32 * kBlockSize;
			size_t         nMallocVolume    = 0;
			void**         pBookmarkArray   = new void*[kMaxMallocVolume]; // kMaxMallocVolume is a safe max count.
			size_t         nBookmarkIndex   = 0;

			if(pBookmarkArray)
			{
				memoryStack.SetDefaultBlockSize(kBlockSize);

				while(nMallocVolume < kMaxMallocVolume)
				{
					void* pResult;

					const size_t nAllocSize            = gRand.RandRange((int)kBlockSize / 32, (int)kBlockSize / 8);
					const size_t nAllocAlignment       = (gRand.RandLimit(10) == 0) ? (1 << gRand.RandLimit(8)) : 0;
					const size_t nAllocAlignmentOffset = nAllocAlignment ? (gRand.RandLimit(8) * 16) : 0;

					if(nAllocAlignment)
						pResult = memoryStack.MallocAligned(nAllocSize, nAllocAlignment, nAllocAlignmentOffset, true);
					else
						pResult = memoryStack.Malloc(nAllocSize, true);

					pResult = memset(pResult, 0x33, nAllocSize);

					EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));

					// We create a bookmark for every single allocation. 
					pBookmarkArray[nBookmarkIndex++] = memoryStack.PushBookmark(pResult);
					if(gRand.RandLimit(10) == 0)
						pBookmarkArray[nBookmarkIndex++] = memoryStack.PushBookmark(pResult); // Test double-pushing a bookmark.
					nMallocVolume += nAllocSize;

					EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));
				}

				while(nBookmarkIndex > 0)
				{
					void* const pBookmark         = pBookmarkArray[--nBookmarkIndex];
					void* const pBookmarkExpected = memoryStack.GetTopBookmark();

					EATEST_VERIFY(pBookmark == pBookmarkExpected);
					EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));

					memoryStack.PopBookmark();

					EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));
				}

				delete[] pBookmarkArray;
			}
		}

		memoryStack.Shutdown();
	}

	{ // Test edge case where pushing a bookmark is very close to the boundary
		
		TestAllocator memoryStack;
		const size_t smallValue = 16;
							
		void* pResult;
						
		size_t blockSize =  memoryStack.GetDefaultBlockSize();

		// First do a small allocation where it allocates blockSize
		pResult = memoryStack.Malloc(smallValue, true);

		// Now allocate almost the entire amount.  There is some align ups so we add some padding.
		pResult = memoryStack.Malloc(blockSize - (smallValue * 2) - memoryStack.GetBookmarkSpace(), true);
				   
		memoryStack.PushBookmark(pResult);
		memoryStack.PushBookmark(pResult); // Test double-pushing a bookmark.
				  
		memoryStack.PopBookmark();
		EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));

		memoryStack.PopBookmark();
		EATEST_VERIFY(memoryStack.ValidateHeap(StackAllocator::kHeapValidationLevelFull));

		memoryStack.Shutdown();
	}


	{
		{ // test passing the assert handler to Init
			static bool bAssertExecuted = false;
			auto OnAssertionFailure = [](EA::Allocator::StackAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
			{ bAssertExecuted = true; };

			StackAllocator memoryStack;
			memoryStack.Init(NULL, 0, NULL, NULL, NULL, OnAssertionFailure);
			memoryStack.MallocAligned(0, 0x7); // force an assert
		#if PPM_ASSERT_ENABLED
			EATEST_VERIFY(bAssertExecuted);
		#endif
			memoryStack.Shutdown();
		}
		{ // test passing the assert handler to SetAssertionFailureFunction 
			static bool bAssertExecuted = false;
			auto OnAssertionFailure = [](EA::Allocator::StackAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
			{ bAssertExecuted = true; };

			StackAllocator memoryStack(NULL, 0);
			memoryStack.SetAssertionFailureFunction(OnAssertionFailure, NULL);
			memoryStack.MallocAligned(0, 0x7); // force an assert
		#if PPM_ASSERT_ENABLED
			EATEST_VERIFY(bAssertExecuted);
		#endif
			memoryStack.Shutdown();
		}
		{ // test passing the assert handler to StackAllocator constructor 
			static bool bAssertExecuted = false;
			auto OnAssertionFailure = [](EA::Allocator::StackAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
			{ bAssertExecuted = true; };

			StackAllocator memoryStack(NULL, 0, NULL, NULL, NULL, OnAssertionFailure);
			memoryStack.MallocAligned(0, 0x7); // force an assert
		#if PPM_ASSERT_ENABLED
			EATEST_VERIFY(bAssertExecuted);
		#endif
			memoryStack.Shutdown();
		}
		{ // test passing the assert handler a context 
			static bool bAssertContextMagicMatches = false;
			struct TestStackAllocatorAssertContext { int mMagicValue; };

			auto OnAssertionFailure = [](EA::Allocator::StackAllocator::AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
			{
				bAssertContextMagicMatches = (((TestStackAllocatorAssertContext*)pContext)->mMagicValue == 0xD15EA5E);
			};

			TestStackAllocatorAssertContext context{0xD15EA5E};

			StackAllocator memoryStack;
			memoryStack.Init(NULL, 0, NULL, NULL, NULL, OnAssertionFailure, &context);
			memoryStack.MallocAligned(0, 0x7); // force an assert
		#if PPM_ASSERT_ENABLED
			EATEST_VERIFY(bAssertContextMagicMatches);
		#endif
			memoryStack.Shutdown();
		}
	}

	return nErrorCount;
}



