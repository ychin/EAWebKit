///////////////////////////////////////////////////////////////////////////////
// EAFixedAllocator
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/version.h>


namespace EA
{
	namespace Allocator
	{
		const Version gVersion =
		{
			PPMALLOC_VERSION_MAJOR,
			PPMALLOC_VERSION_MINOR,
			PPMALLOC_VERSION_PATCH
		};

		const Version *GetVersion()
		{
			return &gVersion;
		}

		bool CheckVersion(int majorVersion, int minorVersion, int patchVersion)
		{
			return \
				majorVersion == PPMALLOC_VERSION_MAJOR && \
				minorVersion == PPMALLOC_VERSION_MINOR && \
				patchVersion == PPMALLOC_VERSION_PATCH;
		}
	}
}













