///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_INTERNAL_VERSION_H
#define PPMALLOC_INTERNAL_VERSION_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>


namespace EA
{
	namespace Allocator
	{

		/// Version contains the version of the library when it was built.
		/// This can be used to verify the correct version has been linked
		/// into the executable or loaded by the O/S (in the case of a DLL).
		struct Version
		{
			int mMajor;
			int mMinor;
			int mPatch;
		};


		/// Get library version information.
		///
		PPM_API const Version* GetVersion();


		/// Check that the linked/loaded library is the same as the headers are expecting.
		/// If the version numbers passed to CheckVersion match those
		/// built into the library when it was compiled, true is returned. 
		/// If not, false is returned.
		PPM_API bool CheckVersion(int majorVersion, int minorVersion, int patchVersion);


	} // namespace Allocator

} // namespace EA


#endif // PPMALLOC_VERSION_H
