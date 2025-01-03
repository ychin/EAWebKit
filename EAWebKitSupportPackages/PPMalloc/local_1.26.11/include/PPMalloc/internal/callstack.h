///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_INTERNAL_CALLSTACK_H
#define PPMALLOC_INTERNAL_CALLSTACK_H


#include <eathread/internal/config.h>


///////////////////////////////////////////////////////////////////////////////
// PPM_USABLE_CALLSTACK
//
// We are using EAThread for cross platform callstack generation.
//
#define PPM_USABLE_CALLSTACK 1


///////////////////////////////////////////////////////////////////////////////
// PPM_USE_EATHREAD_CALLSTACK
// 
// This symbol is defined to 1 if a sufficiently new version of PPMalloc is 
// available and will instead use its callstack facilities instead of the
// ones provided in this library.
//
#if PPM_USABLE_CALLSTACK // IF EAThread's callstack functionality is available and if we deem it to be viable...
	#define PPM_USE_EATHREAD_CALLSTACK 1

	#ifdef EA_DEBUG
		#ifdef PPM_DEBUG_CALLSTACK_AVAILABLE
			#undef PPM_DEBUG_CALLSTACK_AVAILABLE
		#endif
		#define PPM_DEBUG_CALLSTACK_AVAILABLE 1
	#endif
#else
	#define PPM_USE_EATHREAD_CALLSTACK 0
#endif

#endif // Header include guard

