///////////////////////////////////////////////////////////////////////////////
// CircularAllocator implements a "circular" heap. Newly allocated 
// objects will be placed at the end of the heap, wrapping around 
// as needed (no individual allocation will be split). When the 
// heap runs out of space, allocations will fail, at which point
// you can call Free on older objects to make more space available.
//
// This source uses three functions: EA_NEW, EA_DELETE, and EA_ASSERT.
// These act as you would expect and you can define them to be whatever
// you want.
//
// We provide both the .h and .cpp files here in this source file,
// as these sources aren't meant to be used as-is and are unsupported.
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// CircularAllocator.h
//
// Copyright (c) 2005, Electronic Arts Inc. All rights reserved.
// Written by Talin
//
// A linear allocator that operates like a FIFO.
///////////////////////////////////////////////////////////////////////////////


#ifndef ALLOCATOR_CIRCULARALLOCATOR_H
#define ALLOCATOR_CIRCULARALLOCATOR_H


#include <EABase/eabase.h>



namespace EA
{
	namespace Allocator
	{
		//////////////////////////////////////////////////////////////////////////
		/// Implements a circular heap. Newly allocated objects will
		/// be placed at the end of the heap, wrapping around as needed
		/// (no individual allocation will be split). When the heap
		/// runs out of space, allocations will fail, at which point
		/// you can call Free on older objects to make more space
		/// available. You can also call Available to determine the
		/// size of the largest possible allocation. Generally what
		/// you'll want to do is call Free in a loop until Available
		/// reports that enough space is available.
		///        
		/// Note that you must free objects in the order
		/// allocated (although skipping over objects is allowed.)
		///
		class CircularAllocator
		{
		public:

			enum Constants
			{
				kMinAlignment            = 8, // Eight because that's the size of uint64_t and usually the size of double and long long.
				kMinAlignmentMask        = kMinAlignment - 1,
				kMinAlignmentMaskInverse = ~kMinAlignmentMask,
				kDefaultBlockSize        = 8192,
			};

			/// Constructor
			CircularAllocator(size_t heapSize = 0);

			/// Destructor
			~CircularAllocator();

			/// Set the heap size of this allocator. Note that this destroys the
			/// pre-existing heap.
			void SetHeapSize(size_t heapSize, const char* pHeapName = NULL);

			/// Returns the number of bytes available in the buffer.
			size_t Available(size_t nAlignment = kMinAlignment);

			/// Allocate a block of memory in the buffer.
			void* Allocate(size_t size, size_t nAlignment = kMinAlignment);

			/// Frees the object at the given address and all objects
			/// allocated prior to it.
			void Free(void* pAddress);

			/// Clear the heap.
			void Clear();

		public:
			uintptr_t   mHeapStart; /// Beginning of heap buffer
			uintptr_t   mHeapEnd;   /// End of heap
			uintptr_t   mHeapAlloc; /// Pointer to last allocated object
			uintptr_t   mHeapFree;  /// Pointer to last freed object
		};
	}
}

#endif // Header include guard.






///////////////////////////////////////////////////////////////////////////////
// CircularAllocator.cpp
//
// Copyright (c) 2005, Electronic Arts Inc. All rights reserved.
// Written by Talin
//
// A linear allocator that operates like a FIFO.
//
// Implementation note: Because this is a FIFO heap (unlike PPMalloc 
// StackAllocator which is a LIFO heap), it allocates from the top down 
// rather than bottom up.
//
// Reasoning: A pointer to an allocated block of memory is actually
// a pointer to the starting address of a block, in other words the
// lowest-numbered address. In a linear allocator such as this one,
// setting the 'free memory' pointer to an address implies that
// all subsequent addresses are free as well.
//
// In the case of StackAllocator, where a call to Free(mem)
// releases not only 'mem' but all subsequent allocations,
// we should insure that subsequent allocations are located
// at higher-numbered memory addresses.
//
// However, in the cases of CircularAllocator, Free(mem) releases
// mem and all *prior* allocations. In which case, we want the
// prior allocations to be higher in memory than 'mem'.
//
// In order to insure this, we allocate downwards - that is,
// each newly allocated block will be at a lower address than
// the previous block (that is, until we wrap around).
///////////////////////////////////////////////////////////////////////////////

#include <PPMalloc/CircularAllocator.h>



using namespace EA::Allocator;


static inline uintptr_t AlignUp(uintptr_t ptr, size_t nAlignment)
{
	return (ptr + nAlignment - 1) & ~(nAlignment - 1);
}


static inline uintptr_t AlignDown(uintptr_t ptr, size_t nAlignment)
{
	return ptr & ~(nAlignment - 1);
}



CircularAllocator::CircularAllocator(size_t nHeapSize)
	: mHeapStart(NULL)
	, mHeapEnd(NULL)
	, mHeapAlloc(NULL)
	, mHeapFree(NULL)
{
	SetHeapSize(nHeapSize);
}


CircularAllocator::~CircularAllocator()
{
	EA_DELETE((void*)mHeapStart);
}


/// Set the heap size of this allocator. Note that this destroys the
/// pre-existing heap.
void CircularAllocator::SetHeapSize(size_t nHeapSize, const char* pHeapName)
{
	if(mHeapEnd - mHeapStart != nHeapSize)
	{
		if(mHeapStart)
		{
			EA_DELETE((void*)mHeapStart);
			mHeapStart = mHeapEnd = mHeapAlloc = mHeapFree = NULL;
		}

		if(nHeapSize > 0)
		{
			mHeapStart = (uintptr_t)EA_NEW(pHeapName ? pHeapName : "CircularAllocator") uint8_t[nHeapSize];
			mHeapEnd = mHeapStart + nHeapSize;
			Clear();
		}
	}
}


void CircularAllocator::Clear()
{
	mHeapAlloc = mHeapFree = mHeapStart;
}


size_t CircularAllocator::Available(size_t nAlignment)
{
	// Calculate alignment.
	nAlignment = ((nAlignment - 1) | kMinAlignmentMask) + 1;

	// Note that when the two pointers are the same, then the
	// alloc pointer is considered "below" (in other words,
	// the heap is empty).
	if(mHeapAlloc <= mHeapFree)
	{
		// The alloc pointer is below the free pointer, 
		// so the space available is the space between those
		// pointers and the heap start/end.
		return max(mHeapAlloc - AlignUp(mHeapStart, nAlignment),
				   mHeapEnd   - AlignUp(mHeapFree + 1, nAlignment));
	}
	else
	{
		// The alloc pointer is above the free pointer, 
		// so the space available is the distance between them.
		return mHeapAlloc - AlignUp(mHeapFree + 1, nAlignment);
	}
}



void* CircularAllocator::Allocate(size_t size, size_t nAlignment)
{
	// Calculate alignment.
	nAlignment = ((nAlignment - 1) | kMinAlignmentMask) + 1;

	// If the empty space is in the middle.
	if(mHeapAlloc > mHeapFree)
	{
		// Grow downward from alloc ptr.
		uintptr_t pResult = AlignDown(mHeapAlloc - size, nAlignment);

		// Note that we don't allow the two pointers to be the same
		if(pResult > mHeapFree)
		{
			mHeapAlloc = pResult;
			return (void*)pResult;
		}
	}
	else
	{
		// Grow downward from alloc ptr.
		uintptr_t pResult = AlignDown(mHeapAlloc - size, nAlignment);

		// Its ok for an alloc to land exactly on heap start
		if(pResult >= mHeapStart)
		{
			mHeapAlloc = pResult;
			return (void*)pResult;
		}

		// Grow downward from top of heap.
		pResult = AlignDown(mHeapEnd - size, nAlignment);

		// Note that we don't allow the two pointers to be the same
		if(pResult > mHeapFree)
		{
			mHeapAlloc = pResult;
			return (void*)pResult;
		}
	}
	return NULL;
}



void CircularAllocator::Free(void* pAddress)
{
	#if defined(EA_DEBUG)
		uintptr_t pHeapFree = (uintptr_t)pAddress;

		// If the empty space is in the middle.
		if(mHeapAlloc > mHeapFree)
			EA_ASSERT((pHeapFree >= mHeapStart && pHeapFree <= mHeapFree) || (pHeapFree >= mHeapAlloc && pHeapFree <= mHeapEnd));
		else
			EA_ASSERT(pHeapFree >= mHeapAlloc && pHeapFree <= mHeapFree);
	#endif

	// Free simply sets the free pointer.
	mHeapFree = (uintptr_t)pAddress;
}










