#ifndef	EA_ALLOCATOR_ICOREALLOCLISTENER_H
#define	EA_ALLOCATOR_ICOREALLOCLISTENER_H

#ifdef _MSC_VER
#pragma once
#endif

// (c) 2004 Electronic Arts Inc.

/// @brief This header file defines the IAllocListener interface.

#include <stddef.h>     // size_t definition

namespace EA
{
	namespace Allocator
	{
		///	ICoreAllocListener is the base level abstract interface that allows
		/// 'listening' to memory allocation events.
		class ICoreAllocListener 
		{
		public:

			/// This function gets called before an Alloc call
			virtual void PreAlloc(size_t size, const char *name, unsigned int flags,
									unsigned int align, unsigned int alignOffset) = 0;

			/// This function gets called after an Alloc call
			virtual void PostAlloc(size_t size, const char *name, unsigned int flags,
									unsigned int align, unsigned int alignOffset, void *p) = 0;

			/// This function get called before a Free call
			virtual void PreFree(void *block, size_t size=0) = 0;

			/// This function gets called after a Free call
			virtual void PostFree(void *block, size_t size=0) = 0;
		};
	}
}

#endif
