/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef TESTCOREALLOCATOR_H
#define TESTCOREALLOCATOR_H

#include <EACallstack/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAAssert/eaassert.h>


///////////////////////////////////////////////////////////////////////////////
// CustomCoreAllocator
//
class CustomCoreAllocator : public EA::Allocator::ICoreAllocator
{
public:
	CustomCoreAllocator()
	  : mAllocationCount(0), mFreeCount(0), mAllocationVolume(0)
	{ }

	CustomCoreAllocator(const CustomCoreAllocator&)
	  : mAllocationCount(0), mFreeCount(0), mAllocationVolume(0)
	{ }

	~CustomCoreAllocator()
	{ }

	CustomCoreAllocator& operator=(const CustomCoreAllocator&)
	{ return *this; }

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/)
	{
		mAllocationCount++;
		mAllocationVolume += size;
		void* pMemory = malloc(size);
		EA_ASSERT(pMemory); // This malloc failure may be intentionally please look at what is being tested.
		return pMemory;
	}

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/,
				unsigned /*align*/, unsigned /*alignOffset*/)
	{
		mAllocationCount++;
		mAllocationVolume += size;
		void* pMemory = malloc(size);
		EA_ASSERT(pMemory); // This malloc failure may be intentionally please look at what is being tested.
		return pMemory;
	}

	void Free(void* block, size_t size)
	{
		mFreeCount++;
		mAllocationVolume -= size;  // It turns out that size is often zero (unspecified), so this is almost useless.
		free(block);
	}

	bool AnyLeaks() const
	{
		return (mAllocationCount != mFreeCount);
	}

	void ResetCounters()
	{
		mAllocationCount = 0;
		mFreeCount = 0;
		mAllocationVolume = 0;
	}

public:

	int32_t mAllocationCount;
	int32_t mFreeCount;
	int64_t mAllocationVolume;
};

#endif // TESTCOREALLOCATOR_H
