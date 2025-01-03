// (c) 2004 Electronic Arts Inc.

/// @brief This source file provides custom versions of the standard
/// new/delete operators.
///
/// These new and delete operators are intended to override the default
/// new and delete operators.  The executable author may wish to include
/// this source file into their project.  By overriding standard new and
/// delete, all heap allocation goes through the default allocator.  The
/// executable creator is responsible for providing a function called
/// EA::Allocator::ICoreAllocator::GetDefaultAllocator
///
/// This module cannot be in a library - it must be explicitly linked into
/// the executable.  If this module is in a library, the linker may decide
/// to ignore it in favour of the built-in standard new/delete operators.

#include <coreallocator/icoreallocator_interface.h>

// scalar new
void *operator new(size_t size)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(
		size, "Unnamed block", EA::Allocator::MEM_PERM);
}

// scalar delete
void operator delete(void *p)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

// array new
void *operator new[](size_t size)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(
		size, "Unnamed block", EA::Allocator::MEM_PERM);
}

// array delete
void operator delete[](void *p)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}
