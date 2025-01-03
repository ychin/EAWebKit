///////////////////////////////////////////////////////////////////////////////
// mutex.h
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_INTERNAL_MUTEX_H
#define PPMALLOC_INTERNAL_MUTEX_H


EA_DISABLE_VC_WARNING(4574 4836)
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
EA_RESTORE_VC_WARNING()


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////
		// AtomicUint
		///////////////////////////////////////////////////////////////////////
		#if defined(EA_COMPILER_MSVC) && (defined(_MANAGED) || (EA_COMPILER_VERSION >= 1900) /* VS2015+ */)
			typedef uint64_t AtomicUint;
		#else
			typedef EA_ALIGN(8) uint64_t AtomicUint;
		#endif

		#if !defined(_MANAGED)
			static_assert(EA_ALIGN_OF(AtomicUint) == 8, "Incorrect alignment of AtomicUint for atomic operations");
		#endif

		///////////////////////////////////////////////////////////////////////
		// PPMMutex
		///////////////////////////////////////////////////////////////////////

		#if PPM_THREAD_SAFETY_SUPPORTED
			// To do: We need to migrate towards PPMalloc being directly dependent on EAThread and use 
			// its Mutex directly instead of via an opaque buffer like we do here for backward compatibility.

			#if defined(EA_PLATFORM_MICROSOFT) && EA_PLATFORM_PTR_SIZE == 8
				const size_t kMutexBufferSize = 6;
			#elif defined(EA_PLATFORM_WIN32)
				const size_t kMutexBufferSize = 4;
			#elif defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_APPLE)
				const size_t kMutexBufferSize = 6 * ((EA_PLATFORM_PTR_SIZE) / 4);
			#elif defined(EA_PLATFORM_SONY)
				const size_t kMutexBufferSize = 5;
			#elif defined(EA_PLATFORM_NX64)
				const size_t kMutexBufferSize = 8;
			#elif defined(EA_PLATFORM_NX)
				const size_t kMutexBufferSize = 7;
			#else
				const size_t kMutexBufferSize = 4; // To do: Reduce this appropriate for platforms such as PS3 and Unix.
			#endif

			PPM_API void* PPMMutexCreate(void* pData);              /// Returns an initialized mutex object, created from a sufficiently large block of memory (pData).
			PPM_API void  PPMMutexDestroy(void* pMutex);            /// Destroys a mutex object.
			PPM_API void  PPMMutexLock(void* pMutex);               /// Locks a mutex. This lock must support recursiveness. Note that you can add recursiveness to a non-recursive lock by hand.
			PPM_API void  PPMMutexUnlock(void* pMutex);             /// Unlocks a locked mutex.
			PPM_API bool  PPMMutexTryLockFor(void* pMutex, int duration);  /// Attempts to acquire the mutex for the specified duration (relative time) value.
			PPM_API int   PPMMutexGetLockCount(const void* pMutex);

			struct PPMAutoMutex
			{
				void* mpMutex;
				PPMAutoMutex(void* pMutex);
			   ~PPMAutoMutex();
			};

			inline PPMAutoMutex::PPMAutoMutex(void* pMutex)
			  : mpMutex(pMutex)
			{
				PPMMutexLock(mpMutex);
			}

			inline PPMAutoMutex::~PPMAutoMutex()
			{
				PPMMutexUnlock(mpMutex);
			}
		#endif


	} // namespace Allocator

} // namespace EA


#endif // Header include guard




