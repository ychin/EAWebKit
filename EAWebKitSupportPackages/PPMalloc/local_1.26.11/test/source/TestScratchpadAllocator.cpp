///////////////////////////////////////////////////////////////////////////////
// TestScratchpadAllocator
//
// Copyright (c) 2014, Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAScratchpadAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/internal/shared.h>
#include <eathread/eathread_thread.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAMemory.h>


namespace EA
{
	namespace Allocator
	{
		extern PPM_API GeneralAllocator* gpEAGeneralAllocator;
	}
}


///////////////////////////////////////////////////////////////////////////////
// ScratchpadAllocationFunction
//
static void* ScratchpadAllocationFunction(size_t nSize, size_t* pSizeResult, void* /*pContext*/)
{
	#if PPM_ENABLED
		void* const pReturnValue = EA::Allocator::gpEAGeneralAllocator->Malloc(nSize);
	#else
		void* const pReturnValue = new char[nSize];
	#endif

	if(pSizeResult)
	{
		if(pReturnValue)
			*pSizeResult = nSize;
		else
			*pSizeResult = 0;
	}

	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ScratchpadFreeFunction
//
void ScratchpadFreeFunction(void* pData, void* /*pContext*/)
{
	#if PPM_ENABLED
		EA::Allocator::gpEAGeneralAllocator->Free(pData);
	#else
		delete[] (char*)pData;
	#endif
}

#if PPM_THREAD_SAFETY_SUPPORTED
namespace
{
	struct ScratchpadTestThread : public EA::Thread::IRunnable
	{
		EA::Thread::Thread     mThread;
		EA::Thread::ThreadTime mQuitTime;
		EA::UnitTest::Rand     mRand;
		int                    nErrorCount;	// We use the name nErrorCount so that the EATEST_VERIFY macro works.

		enum {
			kArrayCount = 256, // Currently must be <= 256, as we are using bytes to store it in the memory fill below.
			kMaxAllocSize = 200
		};

		ScratchpadTestThread() : mThread(), mQuitTime(), mRand(1234), nErrorCount(0) {}

		void SetMemory(char* ptrArray[], size_t i, size_t allocSize)
		{
			memcpy(ptrArray[i], &allocSize, sizeof(uint32_t));
			memset(ptrArray[i] + sizeof(uint32_t), (int)i, allocSize - sizeof(uint32_t));
		}

		const char* VerifyMemory(const char* pMemory, size_t size)
		{
			uint32_t allocSize;
			memcpy(&allocSize, pMemory, sizeof(uint32_t));
			if (allocSize < kMaxAllocSize)
			{
				const void* pFailureAddress = EA::StdC::Memcheck8(pMemory + sizeof(uint32_t), (uint8_t)size, allocSize - sizeof(uint32_t));
				if (pFailureAddress) // We could just directly return pFailureAddress here, but using this if lets us put an unconditional breakpoint at the next line to catch failures.
					return (const char*)pFailureAddress;
			}
			return NULL;
		}

		void Function(int& depth)
		{
			if (++depth < 6)
			{
				EA::Allocator::ScratchpadAllocator scratch;
				char* ptrArray[kArrayCount];
				
				memset(ptrArray, 0, sizeof(ptrArray));
				for (size_t i = 0; i < EAArrayCount(ptrArray); ++i)
				{
					uint32_t allocSize = (uint32_t)mRand.RandRange(sizeof(uint32_t), kMaxAllocSize);
					ptrArray[i] = (char*)scratch.Malloc(allocSize);
					
					EATEST_VERIFY(ptrArray[i] != NULL);
					SetMemory(ptrArray, i, allocSize);

					size_t j = mRand.RandLimit((uint32_t)EAArrayCount(ptrArray)); // Possibly randomly free an entry.
					if (ptrArray[j]) // If allocated, make sure it's filled as intended and then free it.
					{
						const char* pFailureAddress = VerifyMemory(ptrArray[j], j);
						EATEST_VERIFY_F(pFailureAddress == NULL, "TestScratchpadAllocator: Memory validation failure for %p, size %zu, at index %zu", ptrArray[j], j, (size_t)(pFailureAddress - ptrArray[j]));
						scratch.Free(ptrArray[j]);
						ptrArray[j] = NULL;
					}
				}

				EA::UnitTest::ThreadSleepRandom(1, 40);
				Function(depth);

				for (size_t j = 0; j < EAArrayCount(ptrArray); ++j)
				{
					if (ptrArray[j]) // If allocated, make sure it's filled as intended and then free it.
					{
						const char* pFailureAddress = VerifyMemory(ptrArray[j], j);
						EATEST_VERIFY_F(pFailureAddress == NULL, "TestScratchpadAllocator: Memory validation failure for %p, size %zu, at index %zu", ptrArray[j], j, (size_t)(pFailureAddress - ptrArray[j]));
						scratch.Free(ptrArray[j]);
						ptrArray[j] = NULL;
					}
				}
			}
		}

		intptr_t Run(void*)
		{
			int depth = 0;
			while (EA::Thread::GetThreadTime() < mQuitTime)
				Function(depth);
			return 0;
		}
	};
}
#endif



///////////////////////////////////////////////////////////////////////////////
// TestScratchpadAllocator
//
// This is the main entry function for the ScratchpadAllocator unit test.
//
int TestScratchpadAllocator()
{
	using namespace EA::Allocator;
	using namespace EA::UnitTest;

	int nErrorCount = 0;

	// Run this set of tests twice, to exercize recreating ScratchAllocators for the same thread.
	for(int testRepeat = 0; testRepeat < 2; ++testRepeat)
	{
		EA::Allocator::ScratchpadManager gScratchpadManager;
		EA::Allocator::SetScratchpadManager(&gScratchpadManager);
		gScratchpadManager.SetAllocatorCallbacks(ScratchpadAllocationFunction, ScratchpadFreeFunction, NULL);
		gScratchpadManager.SetStackAllocatorMaxCacheCount(4);

		{   // Simple single-threaded test
			ScratchpadAllocator scratch1;

			void* p11 = scratch1.Malloc(100);
			void* p12 = scratch1.Malloc(200);
			void* p13 = scratch1.MallocAligned(300, 64);

			{
				ScratchpadAllocator scratch2;

				void* p21 = scratch2.Malloc(100);
				void* p22 = scratch2.Malloc(200);
				void* p23 = scratch2.MallocAligned(300, 64);

				scratch2.Free(p22);
				scratch2.Free(p21);
				scratch2.Free(p23);
			}

			scratch1.Free(p12);
			scratch1.Free(p11);
			scratch1.Free(p13);
		}

		{   // Alternative manager test.
			// Tests the use of a second ScratchpadManager in parallel with the default one.
			EA::Allocator::ScratchpadManager gScratchpadManager2(ScratchpadAllocationFunction, ScratchpadFreeFunction, NULL, 1);

			ScratchpadAllocator scratch11;
			ScratchpadAllocator scratch21(&gScratchpadManager2);

			void* p111 = scratch11.Malloc(100);
			void* p211 = scratch21.Malloc(100);
			void* p112 = scratch11.Malloc(200);
			void* p212 = scratch21.Malloc(200);
			void* p113 = scratch11.MallocAligned(300, 64);
			void* p213 = scratch21.MallocAligned(300, 64);

			{
				ScratchpadAllocator scratch12;
				ScratchpadAllocator scratch22(&gScratchpadManager2);

				void* p121 = scratch12.Malloc(100);
				void* p221 = scratch22.Malloc(100);
				void* p122 = scratch12.Malloc(200);
				void* p222 = scratch22.Malloc(200);
				void* p123 = scratch12.MallocAligned(300, 64);
				void* p223 = scratch22.MallocAligned(300, 64);

				scratch12.Free(p122);
				scratch22.Free(p222);
				scratch12.Free(p121);
				scratch22.Free(p221);
				scratch12.Free(p123);
				scratch22.Free(p223);
			}

			scratch11.Free(p112);
			scratch21.Free(p212);
			scratch11.Free(p111);
			scratch21.Free(p211);
			scratch11.Free(p113);
			scratch21.Free(p213);
		}

		#if PPM_THREAD_SAFETY_SUPPORTED
			{   // Multithreaded test
				#if defined(EA_PLATFORM_DESKTOP) || (EA_PLATFORM_WORD_SIZE == 8)
				const int kThreadCount = 20;
				#else
				const int kThreadCount = 10;
				#endif
				ScratchpadTestThread thread[kThreadCount];
				const int kTestTimeMS = 5000;

				for(int i = 0; i < kThreadCount; i++)
					thread[i].mQuitTime = EA::Thread::GetThreadTime() + kTestTimeMS;
				for(int i = 0; i < kThreadCount; i++)
					thread[i].mThread.Begin(&thread[i]);

				EA::UnitTest::ThreadSleep(static_cast<float>(kTestTimeMS));

				for(int i = 0; i < kThreadCount; i++)
				{
					thread[i].mThread.WaitForEnd();
					nErrorCount += thread[i].nErrorCount;
				}
			}
		#endif
	}

	return nErrorCount;
}
