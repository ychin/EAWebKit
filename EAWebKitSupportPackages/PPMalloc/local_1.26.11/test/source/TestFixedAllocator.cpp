///////////////////////////////////////////////////////////////////////////////
// TextFixedAllocator.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMallocTest/PPMallocTest.h>
#include <PPMalloc/EAFixedAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <string.h>


struct TestClass
{
   uint32_t mA;
   uint32_t mB;
   uint32_t mC;
};


int TestFixedAllocator()
{
	char coreBlock[4096];
	EA::Allocator::FixedAllocator<TestClass, 16> test(128, coreBlock, sizeof(coreBlock), NULL, EA::Allocator::NULLFixedFreeFunction);

	const size_t nAllocSlotCount = 128;
	TestClass* pDataArray[nAllocSlotCount];
	memset(pDataArray, 0, sizeof(TestClass*) * nAllocSlotCount);

	for(size_t k(0), kEnd(gnAllocCycleCount * 2); k < kEnd; k++)
	{
		for(size_t j(0); j < gnAllocSlotCount; j++) // For each iteration...
		{
			 size_t i = gRand.RandRange(0, nAllocSlotCount);

			 if(pDataArray[i])
			 {
				  test.Free(pDataArray[i]);
				  pDataArray[i] = NULL;
			 }
			 else
			 {
				  pDataArray[i] = (TestClass*)test.Malloc();

				  if (pDataArray[i] == NULL)
				  {
					  return 1;
				  }

				  memset(pDataArray[i], 0xee, sizeof(TestClass));
			 }
		}

		if(k < (kEnd - 1)) // For every iteration but the last...
		{
			test.Reset();
			memset(pDataArray, 0, sizeof(TestClass*) * nAllocSlotCount);
		}
	}

	for(size_t i = 0; i < nAllocSlotCount; i++)
	{
		if(pDataArray[i])
		{
			test.Free(pDataArray[i]);
			pDataArray[i] = NULL;
		}
	}

	test.Shutdown();

	return 0;
}













