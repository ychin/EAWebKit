#ifndef MINIMAL_ALIGNMENT_ALLOCATOR_H_INCLUDED
#define MINIMAL_ALIGNMENT_ALLOCATOR_H_INCLUDED

#include "coreallocator/icoreallocator_interface.h"

// This class will make sure that the memory returned will be aligned by
// 'align' bytes, and additionally ensure the result is not aligned on the next
// largest power of 2.
//
// IMPORTANT: Please note that this allocator will allocate memory with less
// alignment than many systems expect (because in general an ICoreAllocator
// implementation should return memory aligned to at least
// EA_PLATFORM_MIN_MALLOC_ALIGNMENT).  So, this example is only really useful
// for testing situations or in situations where a user/library of the
// ICoreAllocator instance has ensured it can actually work properly when
// alignment is possibly less than EA_PLATFORM_MIN_MALLOC_ALIGNMENT.
//
// Minimally aligning memory allocations is useful for testing purposes.
// Sometimes a user of the allocator may request alignment less than the actual
// memory required.  But since, in general, an allocator can return a block
// that is more aligned that the requested amount, it can hide the problem.
//
// Example: if you request an allocation that is aligned to 4 bytes, then the
// result will be aligned to 4 bytes.  However the allocator will additionally
// make sure that the allocation is not aligned on 8 bytes.
class MinimalAlignmentAllocator : public EA::Allocator::ICoreAllocator
{
public:
	MinimalAlignmentAllocator();    

	// Sets the allocator used "under the hood" to make memory allocations.
	// The allocator should be set only once before performing any allocations/frees operations.
	void SetAllocator(EA::Allocator::ICoreAllocator *allocator);

	// Gets the allocator used "under the hood" to make memory allocations.
	EA::Allocator::ICoreAllocator * GetAllocator();

	//
	// These methods are virtual methods for the ICoreAllocator interface.
	//
	~MinimalAlignmentAllocator();    
	void* Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset);
	void* Alloc(size_t size, const char *name, unsigned int flags);
	void Free(void *block, size_t size = 0);

private:
	EA::Allocator::ICoreAllocator *mAllocator;
};

#endif
