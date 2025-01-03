// (c) 2004 Electronic Arts Inc.

#ifndef	EA_ALLOCATOR_ICOREALLOCATOR_H
#define	EA_ALLOCATOR_ICOREALLOCATOR_H

#ifdef _MSC_VER
#pragma once
#endif

#ifndef INCLUDED_eabase_H
#include <EABase/eabase.h>
#endif

#include <coreallocator/icoreallocator_interface.h>

/// @brief This header file provides custom new operators
/// for the ICoreAllocator interface.
///
/// These custom new operators pass a name, flags, and
/// optional alignment to the DefaultAllocator.
///
/// The alignment that is passed in is more of a hint in
/// the sense that the minimal alignment of the returned
/// memory is at least equal to this alignment, but in 
/// practice memory with a greater alignment is returned.
///
/// Note that EABase defines EA_PLATFORM_MIN_MALLOC_ALIGNMENT
/// which is an indication of the minimal alignment that
/// can be returned. For example asking for an alignment
/// of 1 usually still returns a memory block block with 
/// an alignment of 4 or 8 bytes alignment.
///
/// In general an implementation of ICoreAllocator should
/// return allocations that are aligned to at least
/// EA_PLATFORM_MIN_MALLOC_ALIGNMENT.  However, in specific
/// situations a user/library may require less aligned memory, and
/// an implementation of ICoreAllocator could return memory
/// that is aligned only to the amount requested.  Just be
/// aware that such an implementation is not guaranteed to
/// be valid for all users of the interface.
///
/// These operators should not be used by library code.
/// Library code that needs to do heap allocations should
/// be designed to allow the library caller to provide an
/// ICoreAllocator object to the library which the library
/// would then use for performing heap management.  Or
/// library code should simply use standard new and delete,
/// which allows the library caller to override those
/// operators at their discretion.
///
/// Standard versions of the new new and delete operators
/// can be found in ../../source/newdelete.cpp.  That file
/// should be linked into the executable so all new and
/// delete operations use a consistent memory manager.
inline void *operator new(size_t size, const char *name, unsigned int flags)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags);
}

inline void *operator new(size_t size, const char *name, unsigned int flags, unsigned int align)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags, align);
}

inline void *operator new[](size_t size, const char *name, unsigned int flags)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags);
}

inline void *operator new[](size_t size, const char *name, unsigned int flags, unsigned int align)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags, align);
}

// NOTE: 
// These are specialized delete operators required to match our new operators when exceptions are enabled. 
// They are not intended for explicit use in client code.  Prefer calling operator delete(void*p).
//
// For more details refer to the details of C4291 below.
// http://msdn.microsoft.com/en-us/library/cxdxz3x6.aspx
inline void operator delete(void *p, const char *, unsigned int)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

inline void operator delete(void *p, const char *, unsigned int, unsigned int)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

inline void operator delete[](void *p, const char *, unsigned int)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

inline void operator delete[](void *p, const char *, unsigned int, unsigned int)
{
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

#endif
