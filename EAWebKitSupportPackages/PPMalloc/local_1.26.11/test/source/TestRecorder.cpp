///////////////////////////////////////////////////////////////////////////////
// TestRecorder.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <PPMallocTest/PPMallocTest.h>
#include <EAStdC/EAStopwatch.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/EAAllocationRecorder.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>



///////////////////////////////////////////////////////////////////////////////
// Global Stopwatch
//
// Useful for general timings.
//
EA::StdC::Stopwatch gStopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds);


///////////////////////////////////////////////////////////////////////////////
// HookFunction
//
void HookFunction(EA::Allocator::AllocationRecorder::HookInfo* pHookInfo, void* pContext)
{
	using namespace EA::Allocator;

	AllocationRecorder* const pAllocationRecorder = (AllocationRecorder*)pContext;
	GeneralAllocator*   const pAllocator          = pAllocationRecorder->GetPlaybackAllocator();

	gStopwatch.Start();

	switch(pHookInfo->mHookType)
	{
		case AllocationRecorder::kHookTypeMalloc:{
			pHookInfo->mpDataOutput = pAllocator->Malloc(pHookInfo->mnSizeInput, pHookInfo->mnAllocationFlags);
			break;
		}

		case AllocationRecorder::kHookTypeFree:{
			pAllocator->Free((void*)pHookInfo->mpDataInput);
			break;
		}

		case AllocationRecorder::kHookTypeMallocAligned:{
			pHookInfo->mpDataOutput = pAllocator->MallocAligned(pHookInfo->mnSizeInput, pHookInfo->mnAlignmentInput, pHookInfo->mnAlignmentOffsetInput, pHookInfo->mnAllocationFlags);
			break;
		}

		case AllocationRecorder::kHookTypeCalloc:{
			pHookInfo->mpDataOutput = pAllocator->Calloc(pHookInfo->mnCountInput, pHookInfo->mnSizeInput);
			break;
		}

		case AllocationRecorder::kHookTypeRealloc:{
			pHookInfo->mpDataOutput = pAllocator->Realloc((void*)pHookInfo->mpDataInput, pHookInfo->mnSizeInput, pHookInfo->mnAllocationFlags);
			break;
		}
	}

	gStopwatch.Stop();
}



///////////////////////////////////////////////////////////////////////////////
// TestRecorder
//
int TestRecorder()
{
	using namespace EA::Allocator;

	const char* kDefaultStreamName = "C:\\Temp\\Allocation Recording.txt";
	EA_UNUSED(kDefaultStreamName);

	////////////////////////////////////////////////////////
	// Test recorder on fake data.
	////////////////////////////////////////////////////////
	#if defined(EA_PLATFORM_WINDOWS)
	{
		AllocationRecorder allocationRecorder;
		AllocationRecorder allocationPlayer;

		allocationRecorder.InitFile(AllocationRecorder::kModeRecord, kDefaultStreamName);
		allocationRecorder.RecordMalloc((void*)0x1000001, 64);
		allocationRecorder.RecordMalloc((void*)0x1000002, 64);
		allocationRecorder.RecordMalloc((void*)0x1000003, 64);
		allocationRecorder.RecordMalloc((void*)0x20000004, 64);
		allocationRecorder.RecordRealloc((void*)0x1000004, (void*)0x20000004, 64);
		allocationRecorder.RecordCalloc((void*)0x1000005, 1, 32);
		allocationRecorder.RecordFree((void*)0x1000001);
		allocationRecorder.RecordFree((void*)0x1000002);
		allocationRecorder.RecordFree((void*)0x1000003);
		allocationRecorder.RecordFree((void*)0x1000004);
		allocationRecorder.RecordFree((void*)0x1000005);
		allocationRecorder.Shutdown();

		// Test player.
		allocationPlayer.InitFile(AllocationRecorder::kModePlayback, kDefaultStreamName);
		allocationPlayer.Playback();
		allocationPlayer.Shutdown();
	}
	#endif
	////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////
	// Test recorder on synthetically generated data.
	////////////////////////////////////////////////////////
	#if defined(EA_PLATFORM_WINDOWS)
	{
		AllocationRecorder    allocationRecorder;
		AllocationRecorder    allocationPlayer;
		GeneralAllocatorDebug allocator;
		const int             kArraySize = 100;
		void*                 pDataArray[kArraySize];
		void*                 pData;

		allocationRecorder.InitFile(AllocationRecorder::kModeRecord, kDefaultStreamName);
		memset(pDataArray, 0, sizeof(pDataArray));
		gRand.SetSeed(EA::UnitTest::GetRandSeed());

		for(int i(0); i < kArraySize * 10; i++)
		{
			const int nArrayIndex = gRand.RandRange(0, kArraySize);

			if(pDataArray[nArrayIndex])
			{
				allocator.Free(pDataArray[nArrayIndex]);
				allocationRecorder.RecordFree(pDataArray[nArrayIndex]);
				pDataArray[nArrayIndex] = NULL;
			}
			else
			{
				const int nSize = gRand.RandRange(32, 64);
				pData = allocator.Malloc(nSize);
				pDataArray[nArrayIndex] = pData;
				allocationRecorder.RecordMalloc(pDataArray[nArrayIndex], nSize);
			}
		}

		for(int nArrayIndex(0); nArrayIndex < kArraySize; nArrayIndex++)
		{
			if(pDataArray[nArrayIndex])
			{
				allocator.Free(pDataArray[nArrayIndex]);
				allocationRecorder.RecordFree(pDataArray[nArrayIndex]);
				pDataArray[nArrayIndex] = NULL;
			}
		}

		allocationRecorder.Shutdown();

		// Test player.
		allocationPlayer.InitFile(AllocationRecorder::kModePlayback, kDefaultStreamName);
		allocationPlayer.Playback();
		allocationPlayer.Shutdown();
	}
	#endif
	////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////
	// Test automated recording of GeneralAllocator
	////////////////////////////////////////////////////////
	#if defined(EA_PLATFORM_WINDOWS)
	{
		#if PPM_ENABLED
			AllocationRecorder    allocationRecorder;
			AllocationRecorder    allocationPlayer;
			const int             kArraySize = 100;
			void*                 pDataArray[kArraySize];
			void*                 pData;
			int                   i;
			int                   nArrayIndex;
			#ifdef EA_DEBUG
			GeneralAllocatorDebug allocator;
			#else
			GeneralAllocator      allocator;
			#endif

			allocationRecorder.InitFile(AllocationRecorder::kModeRecord, kDefaultStreamName);
			allocationRecorder.RecordAllocator(&allocator, true);
			memset(pDataArray, 0, sizeof(pDataArray));
			gRand.SetSeed(EA::UnitTest::GetRandSeed());

			for(i = 0; i < kArraySize * 10; i++)
			{
				nArrayIndex = gRand.RandRange(0, kArraySize);

				if(pDataArray[nArrayIndex])
				{
					allocator.Free(pDataArray[nArrayIndex]);
					pDataArray[nArrayIndex] = NULL;
				}
				else
				{
					const int nSize = gRand.RandRange(32, 64);
					pData = allocator.Malloc(nSize);
					pDataArray[nArrayIndex] = pData;
				}
			}

			for(nArrayIndex = 0; nArrayIndex < kArraySize; nArrayIndex++)
			{
				if(pDataArray[nArrayIndex])
				{
					allocator.Free(pDataArray[nArrayIndex]);
					pDataArray[nArrayIndex] = NULL;
				}
			}

			allocationRecorder.Shutdown();

			// Test player.
			allocationPlayer.InitFile(AllocationRecorder::kModePlayback, kDefaultStreamName);
			allocationPlayer.Playback();
			allocationPlayer.Shutdown();
		#endif // PPM_ENABLED
	}
	#endif
	////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////
	// Test player on recorded data.
	////////////////////////////////////////////////////////
	#if defined(EA_PLATFORM_WINDOWS)
	{
		AllocationRecorder allocationPlayer;
		const char* pPath = "C:\\Temp\\PPMallocRecorderTest.txt";
		//pPath = "D:\\Temp\\MOHAPS3\\ppMalloc.allocationRecording";
		pPath = "C:\\Temp\\bioware_memory_log_2.txt";

		// If there is a disk file in the given location, we test it. Otherwise we do nothing.
		if(allocationPlayer.InitFile(AllocationRecorder::kModePlayback, pPath, AllocationRecorder::kPlaybackAllocatorTypeGeneralDebug))
		{
			allocationPlayer.SetPlaybackHook(HookFunction, &allocationPlayer);

			uint64_t            nEventCount      = 0;
			const uint64_t      nEventCountTotal = allocationPlayer.GetTotalEventCount();
			uint64_t            nEventCountNext  = 0;       // The next number of events before we print info. 
			double              dReportIncrement = 0.05;
			double              dReportNext      = 0;       // The next percentage of completion before we print info.
			EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds, true);

			EA::UnitTest::ReportVerbosity(1, "%s\n", pPath);
			if(nEventCountTotal > 100)
				EA::UnitTest::ReportVerbosity(1, "Total event count: %I64u (estimated).\n", nEventCountTotal);
			else
				EA::UnitTest::ReportVerbosity(1, "Total event count: %I64u.\n", nEventCountTotal);

			while(allocationPlayer.PlaybackStep())
			{
				const double currentAmount = (double)nEventCount / (double)nEventCountTotal;

				if((currentAmount >= dReportNext) || (nEventCount >= nEventCountNext)) // Whichever comes first.
				{
					if(currentAmount >= dReportNext) // If amount occurred first.
						EA::UnitTest::ReportVerbosity(1, "%.0f%% complete (%I64u events).\n", dReportNext * 100, nEventCount);
					else
						EA::UnitTest::ReportVerbosity(1, "%I64u events.\n", nEventCount);

					dReportNext      += dReportIncrement;
					nEventCountNext  += 250000;
				}

				nEventCount++;
			}

			EA::UnitTest::ReportVerbosity(1, "Elapsed allocation time (allocation events alone): %I64u ms.\n", gStopwatch.GetElapsedTime());
			EA::UnitTest::ReportVerbosity(1, "Elapsed test time (includes parsing, allocation, filling): %I64u ms.\n", stopwatch.GetElapsedTime());
	 
			allocationPlayer.Shutdown();
		}
	}
	#endif
	////////////////////////////////////////////////////////


	return 0;
}















