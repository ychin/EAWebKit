#include "EAAssert/eaassert.h"
#include "minimal_alignment_allocator.h"

MinimalAlignmentAllocator::MinimalAlignmentAllocator()
	:mAllocator(NULL)
{
}

void MinimalAlignmentAllocator::SetAllocator(EA::Allocator::ICoreAllocator *allocator)
{
	EA_ASSERT_MSG(mAllocator==NULL, "The allocator should only be set once");
	mAllocator = allocator;
}

EA::Allocator::ICoreAllocator * MinimalAlignmentAllocator::GetAllocator()
{
	return mAllocator;
}

MinimalAlignmentAllocator::~MinimalAlignmentAllocator()
{
}

void *MinimalAlignmentAllocator::Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset)
{
	EA_UNUSED(alignOffset);
	EA_ASSERT_MSG(mAllocator!=NULL, "The allocator has not been set via SetAllocator");
	EA_ASSERT_MSG(alignOffset==0, "This allocator does not support a non-zero value for 'alignOffset'.");

	void *allocation = mAllocator->Alloc( size + align, name, flags, align * 2, 0 );
	EA_ASSERT_MSG(allocation != NULL, "Unable to satisfy memory request");
	uintptr_t allocationAddress = reinterpret_cast<uintptr_t>(allocation);
	uintptr_t minimallyAlignedAddress = allocationAddress + align;

	// Make sure we are aligned on the requested alignment.
	EA_ASSERT( ( minimallyAlignedAddress & (align-1) ) == 0 );
	// Make sure we are not "over" aligning the memory.
	EA_ASSERT( ( minimallyAlignedAddress & ((2*align)-1) ) != 0 );

	return reinterpret_cast<void*>(minimallyAlignedAddress);
}

void *MinimalAlignmentAllocator::Alloc(size_t size, const char *name, unsigned int flags)
{
	EA_ASSERT_MSG(mAllocator!=NULL, "The allocator has not been set via SetAllocator");

	return Alloc(size, name, flags, 1, 0);
}

void MinimalAlignmentAllocator::Free(void *block, size_t size)
{
	EA_ASSERT_MSG(mAllocator!=NULL, "The allocator has not been set via SetAllocator");

	// Determine the alignement of the block.  This can be used to determine where the actual allocation
	// returned by 'mAllocator' begins.
	unsigned int blockAlignment = 0;
	uintptr_t blockAddress = reinterpret_cast<uintptr_t>(block);
	for(unsigned int alignmentToTest = 1;; alignmentToTest *=2 )
	{
		if(blockAddress & alignmentToTest)
		{
			blockAlignment = alignmentToTest;
			break;
		}
	}

	uintptr_t allocationAddress = blockAddress - blockAlignment;
	mAllocator->Free(reinterpret_cast<void*>(allocationAddress), size);
}
