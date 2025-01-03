///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>

namespace EA
{
namespace Callstack
{

namespace EAModuleImpl
{

EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void*, char*, size_t)
{
	return 0;
}

EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void*)
{
	return kModuleHandleInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const wchar_t*)
{
	return kBaseAddressInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const char*)
{
	return kBaseAddressInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandleImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle)
{
	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void*)
{
	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleListImpl
///
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle*, size_t)
{
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo*, size_t)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandleImpl
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle, ModuleInfo&)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleOfExecImpl
///
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{
	return kModuleHandleInvalid;
}

} // namespace EAModuleImpl

} // namespace Callstack
} // namespace EA
