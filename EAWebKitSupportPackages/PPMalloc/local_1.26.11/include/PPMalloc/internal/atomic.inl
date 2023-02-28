///////////////////////////////////////////////////////////////////////////////
// atomic.h
//
// Copyright (c) Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef PPMALLOC_INTERNAL_ATOMIC_INL
#define PPMALLOC_INTERNAL_ATOMIC_INL

#include <eathread/eathread_atomic.h>

namespace EA
{
	namespace Allocator
	{
		inline uint64_t PPMAtomicIncrement(uint64_t &x)
		{
			return Thread::AtomicFetchIncrement(&x);
		}
	}
}

#endif

