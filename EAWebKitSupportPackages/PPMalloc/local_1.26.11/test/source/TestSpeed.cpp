///////////////////////////////////////////////////////////////////////////////
// TestSpeed
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAAlignment.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EARandomDistribution.h>
#include <EAStdC/EADateTime.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <eathread/eathread_thread.h>
#include <EATest/EATest.h>

#if defined(EA_PLATFORM_DESKTOP) // This is too heavy for many other platforms.
	#define PPM_TEST_SPEED_ENABLED 0
#else
	#define PPM_TEST_SPEED_ENABLED 0
#endif


#if PPM_TEST_SPEED_ENABLED 

	// We weight the result towards the small side of the range.
	uint32_t RandomUint32Weighted(EA::StdC::RandomFast& r, uint32_t minValue, uint32_t maxValue)
	{
		// Question: What's the fastest way to get the desired outcome? Our quick solution
		// to get this initially running is to just call rand three times and pick the lowest value.
		// Surely there is a faster way of achieving similar results.

		uint32_t vs[3] = { r.RandomUint32Uniform(), r.RandomUint32Uniform(), r.RandomUint32Uniform() };
		uint32_t v     = *eastl::min_element(vs, vs + 3);

		return minValue + (uint32_t)((v * (uint64_t)maxValue - (uint64_t)minValue) >> 32); // Scales the value from [0, MAX_UINT32] to a range of [0, nLimit).
	}


	///////////////////////////////////////////////////////////////////////////////
	// BenchmarkThreaded
	//
	static int BenchmarkThreaded(bool shouldReport, uint32_t threadCount, uint32_t testTimeMs, uint32_t slotCount, uint32_t minSize, uint32_t maxSize)
	{
		struct ThreadTestInfo
		{
			uint64_t                        mTotalThreadCount;      //
			EA::Thread::AtomicUint32        mCreatedThreadCount;    // Incremented by each thread as it begins.
			EA::Thread::AtomicUint32        mEndedThreadCount;      // Incremented by each thread as it ends.
			EA::Thread::Thread              mThread[32];            //  
			uint64_t                        mThreadStartTimeNs;     // The time that the first thread started executing.
			uint64_t                        mThreadEndTimeNs;       // The time that the last thread ended executing.
			EA::Thread::AtomicUint64        mAllocationCount;       // Number of times malloc was called
			EA::Thread::AtomicUint64        mFreeCount;             // Number of times free was called.
			uint64_t                        mTestTimeMs;            // The amount of time we should run the test for. It's not critical that we stop exactly at this time.
			uint32_t                        mSlotCount;             // 
			uint32_t                        mMinSize;               // 
			uint32_t                        mMaxSize;               // 
			EA::Allocator::GeneralAllocator mGeneralAllocator;

			ThreadTestInfo()
			  : mTotalThreadCount(0), mCreatedThreadCount(0), mEndedThreadCount(0), mThreadStartTimeNs(0), mThreadEndTimeNs(0), 
				mAllocationCount(0), mFreeCount(0), mTestTimeMs(0), mSlotCount(0), mMinSize(0), mMaxSize(0) {}

			static intptr_t ThreadFunction(void* pContext)
			{
				ThreadTestInfo* const pThreadTestInfo = (ThreadTestInfo*)pContext;

				char threadDescription[128];
				EA::StdC::Snprintf(threadDescription, EAArrayCount(threadDescription), "BenchmarkThreaded: Thread Id: %s, Thread SysId: %s", EAThreadThreadIdToString(EA::Thread::GetThreadId()), EAThreadSysThreadIdToString(EA::Thread::GetSysThreadId()));

				eastl::vector<void*> ptrArray(pThreadTestInfo->mSlotCount, 0);

				EA::StdC::RandomFast rng(EA::UnitTest::GetRandSeed());

				uint64_t startTimeNs   = EA::StdC::GetTime();
				uint64_t endTimeNs     = startTimeNs + (pThreadTestInfo->mTestTimeMs * 1000000);
				uint64_t currentTimeNs = startTimeNs;

				if(pThreadTestInfo->mCreatedThreadCount.Increment() == 1)  // If this is the first thread created...
					pThreadTestInfo->mThreadStartTimeNs = EA::StdC::GetTime();

				for(uint32_t j = 0; currentTimeNs < endTimeNs; ++j)
				{
					uint32_t slot = rng.RandomUint32Uniform(pThreadTestInfo->mSlotCount);
					uint32_t n    = RandomUint32Weighted(rng, pThreadTestInfo->mMinSize, pThreadTestInfo->mMaxSize);

					if(ptrArray[slot])
					{
						pThreadTestInfo->mGeneralAllocator.Free(ptrArray[slot]);
						ptrArray[slot] = NULL;
						pThreadTestInfo->mFreeCount++;
					}
					else
					{
						ptrArray[slot] = pThreadTestInfo->mGeneralAllocator.Malloc(n);
						//EATEST_VERIFY(ptrArray[slot]);
						pThreadTestInfo->mAllocationCount++;
					}

					if((j % 64) == 0) // Don't execute the current time update every loop, as it gets in the way of the allocation performance measurement.
						currentTimeNs = EA::StdC::GetTime();
				}

				for(eastl_size_t slot = 0; slot < ptrArray.size(); ++slot) // Free any final pointers.
				{
					if(ptrArray[slot])
					{
						pThreadTestInfo->mGeneralAllocator.Free(ptrArray[slot]);
						ptrArray[slot] = NULL;
					}
				}

				if(pThreadTestInfo->mEndedThreadCount.Increment() == pThreadTestInfo->mTotalThreadCount) // If this is the last thread exiting...
					pThreadTestInfo->mThreadEndTimeNs = EA::StdC::GetTime();

				return 0;
			}
		}; // struct ThreadTestInfo



		ThreadTestInfo threadTestInfo;

		threadTestInfo.mTotalThreadCount = threadCount; 
		threadTestInfo.mTestTimeMs       = testTimeMs;
		threadTestInfo.mSlotCount        = slotCount;
		threadTestInfo.mMinSize          = minSize;
		threadTestInfo.mMaxSize          = maxSize;

	   // if(threadCount == 1) // If testing only a single thread, disable thread safety.
	   //     threadTestInfo.mGeneralAllocator.SetOption(EA::Allocator::GeneralAllocator::kOptionEnableThreadSafety, 0);

		for(uint32_t j = 0, jEnd = threadCount; j < jEnd; j++)
			threadTestInfo.mThread[j].Begin(ThreadTestInfo::ThreadFunction, &threadTestInfo);

		while(threadTestInfo.mEndedThreadCount < threadCount)
			EA::Thread::ThreadSleep(1000);  // Wait for the threads to all end.

		for(uint32_t j = 0, jEnd = threadCount; j < jEnd; j++)
			threadTestInfo.mThread[j].WaitForEnd();

		// Report results.
		// A basic metric is the total operations done per thread per second.
		double operationCount = (double)(threadTestInfo.mAllocationCount + threadTestInfo.mFreeCount);
		double seconds        = (double)(threadTestInfo.mThreadEndTimeNs - threadTestInfo.mThreadStartTimeNs) / 1000000000.f;
		double throughput     = (operationCount / seconds) / threadCount;

		if(shouldReport)
		{
			EA::UnitTest::Report("    %I32u threads, %I32u slots, %I32u byte min, %I32u byte max: %'.0f operations per thread per second.\n", 
								threadCount, slotCount, minSize, maxSize, throughput);
		}

		return 0; // We don't currently track errors.
	}

#endif // #if defined(EA_PLATFORM_DESKTOP)



///////////////////////////////////////////////////////////////////////////////
// TestSpeed
//
int TestSpeed()
{
	int nErrorCount = 0;

	#if PPM_TEST_SPEED_ENABLED 
		// We want to execute this an initial time in order to "warm up" the thread cache and core memory.
		nErrorCount += BenchmarkThreaded(false, 8, 1000 /*milliseconds*/, 100000, 4, 16384);

		nErrorCount += BenchmarkThreaded(true, 1, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 2, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 3, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 4, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 5, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 6, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 7, 5000, 100000, 4, 16384);
		nErrorCount += BenchmarkThreaded(true, 8, 5000, 100000, 4, 16384);
	#endif

	return nErrorCount;
}













