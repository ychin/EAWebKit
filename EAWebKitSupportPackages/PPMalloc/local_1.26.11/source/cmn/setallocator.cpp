///////////////////////////////////////////////////////////////////////////////
// setallocator.cpp
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLED (defined as 0 or 1, default is 1)
//
// When this is enabled, this module recognizes PPMalloc modules external to 
// this module. If disabled, then this module has no dependencies on anything
// except standard C/C++ libraries and you can thus use it with other
// allocation systems.
//
#ifndef PPM_ENABLED
	#define PPM_ENABLED 1
#endif


#if PPM_ENABLED

	#include <PPMalloc/EAGeneralAllocator.h>
	#include <PPMalloc/EAGeneralAllocatorDebug.h>

	namespace EA
	{
		namespace Allocator
		{
			PPM_API GeneralAllocator*      gpEAGeneralAllocator = NULL;
			PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug = NULL;

			PPM_API void SetGeneralAllocator(GeneralAllocator* pAllocator)
			{
				gpEAGeneralAllocator = pAllocator;

				if(pAllocator) // Note that AsInterface will return NULL if pAllocator is GeneralAllocator instead of GeneralAllocatorDebug.
					gpEAGeneralAllocatorDebug = (GeneralAllocatorDebug*)pAllocator->AsInterface(GeneralAllocatorDebug::kTypeId);
				else
					gpEAGeneralAllocatorDebug = NULL;
			}
		}
	}

#endif  // PPM_ENABLED
















