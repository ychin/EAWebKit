/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_APPLE_EACALLSTACKAPPLE_H
#define EACALLSTACK_APPLE_EACALLSTACKAPPLE_H


#include <EACallstack/EACallstack.h>
#include <EACallstack/EAAddressRep.h>
#include <EASTL/fixed_vector.h>


namespace EA
{
namespace Callstack
{

	///////////////////////////////////////////////////////////////////////////////
	/// ModuleInfoApple
	///
	struct ModuleInfoApple : public ModuleInfo
	{
		char mType[32];             // The type field (e.g. __TEXT) from the vmmap output.
		char mPermissions[16];      // The permissions "r--/rwx" kind of string from the vmmap output.
	};

	typedef eastl::fixed_vector<ModuleInfoApple, 128, true> ModuleInfoAppleArray;


	///////////////////////////////////////////////////////////////////////////////
	// GetModuleInfoApple
	//
	// This function exists for the purpose of being a central module/VM map info collecting function,
	// used by a couple functions within EACallstack.
	//
	EACALLSTACK_API size_t GetModuleInfoApple(ModuleInfoAppleArray& moduleInfoAppleArray, const char* pTypeFilter = nullptr,
											  bool bEnableCache = true);


} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_APPLE_EACALLSTACKAPPLE_H
