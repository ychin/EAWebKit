/////////////////////////////////////////////////////////////////////////////
// PPMallocTest.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOCTEST_H
#define PPMALLOCTEST_H


#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <EABase/eabase.h>
#include <EATest/EATest.h>
#include <PPMalloc/internal/config.h>
#include <stdlib.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif


// PPM_TEST_THREADING_ENABLED
//
// Defined as 0 or 1.
//
#if !defined(PPM_TEST_THREADING_ENABLED)
	#if PPM_THREAD_SAFETY_SUPPORTED 
		#define PPM_TEST_THREADING_ENABLED 1
	#else
		#define PPM_TEST_THREADING_ENABLED 0
	#endif
#endif

extern unsigned int gTestLengthSeconds;

// Test declarations
//
int TestSpeed();
int TestStackAllocator();
int TestFixedAllocator();
int TestNewDelete();
int TestRecorder();
int TestGeneralAllocator();
int TestSmallObjectAllocator();
int TestNonLocalAllocator();
int TestSmallBlockAllocator();
int TestScratchpadAllocator();


// Utility functions
//
void   ClampRange(size_t& nValue, size_t nMin, size_t nMax);
void   SetupParameters(int argc, char* argv[]);


// PPMallocTest_CheckMemory
//
// Does a global memory heap validation check. 
// Returns 0 if OK and an error count if there is a problem. 
//
// Example usage:
//    EASTLTest_CheckMemory();
//
int PPMallocTest_CheckMemory_Imp(const char* pFile, int nLine);
#define PPMallocTest_CheckMemory() PPMallocTest_CheckMemory_Imp(__FILE__, __LINE__)



struct MallocInfo
{
	void*   mpData;
	size_t  mSize;
	uint8_t mFill;
};


// Globals
extern size_t             gnAllocSizeMin;
extern size_t             gnAllocSizeMax;
extern size_t             gnAllocSlotCount;
extern size_t             gnAllocIterationCount;
extern size_t             gnAllocCycleCount;
extern size_t             gnNewCoreSize;
extern size_t             gnThreadCount;
extern int                gnAutoHeapValidationLevel;
extern size_t             gnAutoHeapValidationFrequency;
extern bool               gbExtraHeapValidation;
extern EA::UnitTest::Rand gRand;


// Utility functions
void ClampRange(size_t& nValue, size_t nMin, size_t nMax);


#endif // Header include guard







