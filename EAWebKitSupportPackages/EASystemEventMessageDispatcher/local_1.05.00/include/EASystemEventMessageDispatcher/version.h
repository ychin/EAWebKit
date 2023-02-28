///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "internal/Config.h"

namespace EA
{
	namespace SEMD
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

		/// Get the library version information.
		EASEMD_API const Version *GetVersion();

		/// Check that the linked/loaded library is the same as the headers 
		/// are expecting.
		///
		/// If the version numbers passed to CheckVersion match those
		/// built into the library when it was compiled, true is returned. 
		/// If not, false is returned.
		EASEMD_API bool CheckVersion(int majorVersion, int minorVersion, int patchVersion);
	}
}
