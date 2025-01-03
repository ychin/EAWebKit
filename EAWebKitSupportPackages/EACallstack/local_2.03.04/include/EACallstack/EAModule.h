/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/IAddressRep.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

namespace EA
{
namespace Callstack
{

///////////////////////////////////////////////////////////////////////////////
/// ModuleHandle
///
/// This is a runtime module identifier. For Microsoft Windows-like platforms
/// this is the same thing as HMODULE. For other platforms it is a shared library
/// runtime library pointer, id, or handle. For Microsoft platforms, each running
/// DLL has a module handle.
#if defined(EA_PLATFORM_MICROSOFT)

	typedef void*            ModuleHandle;  // HMODULE, from LoadLibrary()

#elif defined(EA_PLATFORM_SONY)

	typedef int32_t          ModuleHandle;  // SceDbgModule

#elif defined(EA_PLATFORM_ANDROID) || defined(EA_PLATFORM_LINUX)

	typedef void*            ModuleHandle; // BaseAddress of the loaded ELF image, see EAModule_unix.cpp

#elif defined(EA_PLATFORM_APPLE)

	typedef void*            ModuleHandle; // BaseAddress of the loaded image, see EAModule_Apple.cpp

#elif defined(EA_PLATFORM_NX)

	typedef uintptr_t        ModuleHandle; // BaseAddress from the nn::diag::ModuleInfo struct

#elif defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_POSIX)

	typedef void*            ModuleHandle;  // void*, from dlopen()

#else

	typedef uintptr_t        ModuleHandle;

#endif


///////////////////////////////////////////////////////////////////////////////
/// kModuleHandleInvalid
///
/// Shorthand for a handle that is default initialized or othewise invalid.
///
static const ModuleHandle kModuleHandleInvalid = ((ModuleHandle)(-1));

///////////////////////////////////////////////////////////////////////////////
/// kModuleMaxCapacity
///
/// Shorthand for the Maximum number of modules supported for enumeration
///
static constexpr size_t kModuleMaxCapacity = EACALLSTACK_MODULE_MAX_CAPACITY;

///////////////////////////////////////////////////////////////////////////////
/// ModuleInfo
///
struct EACALLSTACK_API ModuleInfo
{
	ModuleHandle mModuleHandle;  // The ModuleHandle for the module.
	FixedString8 mPath;          // File name or file path
	FixedString8 mName;          // Module name.
	uint64_t     mBaseAddress;   // Base address in memory.
	uint64_t     mSize;          // Module size in memory.

	ModuleInfo()
		: mModuleHandle(kModuleHandleInvalid)
		, mBaseAddress(kBaseAddressInvalid)
		, mSize(0)
	{
	}

	ModuleInfo(EA::Allocator::ICoreAllocator* pCoreAllocator)
		: mModuleHandle(kModuleHandleInvalid)
		, mPath(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "ModuleInfo/Path", pCoreAllocator))
		, mName(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "ModuleInfo/Name", pCoreAllocator))
		, mBaseAddress(kBaseAddressInvalid)
		, mSize(0)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////
/// GetModuleFromAddress
///
/// Given an address, this function tells what module it comes from.
/// The primary use of this is to tell what DLL an instruction pointer comes from.
/// Returns the required strlen of the pModuleFileName. If the return value is >= nModuleNameCapacity,
/// there wasn't enough space. pModuleFileName is written with as many characters as possible
/// and will always be null-terminated. nModuleNameCapacity must be at least one.
///
/// @param pAddress: The address to find the module of
///
/// @param pModuleFileName: The output buffer for the filename of the module. Always null-terminated.
///
/// @param nModuleNameCapacity: The total size of the output buffer
///
/// @return: Returns the number of characters written. If the return value >= nModuleNameCapacity then the buffer wasn't big enough
///
EACALLSTACK_API size_t GetModuleFromAddress(const void* pAddress, char* pModuleFileName, size_t nModuleNameCapacity);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleFromAddress
///
/// Returns the module handle from a code address.
/// Returns 0/NULL if no associated module could be found.
///
/// @param pAddress: The address to find the module of
///
/// @return: Returns the Module or kModuleHandleInvalid if no module was found
///
EACALLSTACK_API ModuleHandle GetModuleHandleFromAddress(const void* pAddress);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleBaseAddress
///
/// Returns the module (e.g. .exe, .dll) current base address in memory.
/// This is required for doing symbol lookups against symbol database files.
/// The pModuleName must be the file path or file name. If pModuleName is
/// NULL, the module used to create the calling process (.exe, .elf) is assumed.
/// The module name is case-sensitive on some platforms.
/// You don't need the full path of the module if the module is part of your
/// currently running app.
/// This function returns the runtime base address for the given module and
/// does not return the default or preferred base address which is sometimes
/// stored in the .map or .pdb symbol file for the module. Returns kBaseAddressInvalid if the
/// base address is unknown.
/// This function is useful only when run live in a running application and
/// not in a tool or external application.
///
/// Example usage:
///     GetModuleBaseAddress("MyApp.exe"); // MyApp.exe is part of the currently running module.
///     GetModuleBaseAddress("C:\\Program Files\\MyApp\\MyPlugin.dll");
///     GetModuleBaseAddress("MyPlugin.dll");
///     GetModuleBaseAddress("MyPlugin");
///
/// @param pModuleName: File Path or File Name of the loaded module.
///                     pModuleName == nullptr, the module of the calling process is used, (.exe, .elf).
///
/// @return: Runtime base address that the given module was loaded at. kBaseAddressInvalid on failure.
///
EACALLSTACK_API uint64_t GetModuleBaseAddress(const wchar_t* pModuleName);
EACALLSTACK_API uint64_t GetModuleBaseAddress(const char* pModuleName = nullptr);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleBaseAddressByHandle
///
/// Returns the base address for the module (e.g. exe, dll)
/// via the given handle. Note that the meaning of ModuleHandle differs
/// between platforms. Returns kBaseAddressInvalid if the base address is unknown.
/// This function is useful only when run live in a running application and
/// not in a tool or external application.
///
/// Example usage:
///     void DoSomething()
///     {
///         GetModuleBaseAddressByHandle(GetModuleHandle(NULL)); // Windows code.
///     }
///
/// @param moduleHandle: handle to the loaded module.
///                      if handle == kModuleHandleInvalid, the module of the calling process is used, (.exe, .elf).
///
/// @return: Runtime base address that the given module was loaded. kBaseAddressInvalid on failure.
///
EACALLSTACK_API uint64_t GetModuleBaseAddressByHandle(ModuleHandle moduleHandle = kModuleHandleInvalid);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleBaseAddressByAddress
///
/// Returns the base address for the module (e.g. exe, dll)
/// that has code at the given code location. Returns kBaseAddressInvalid if the base address is unknown.
/// This function is useful only when run live in a running application and
/// not in a tool or external application.
///
/// Example usage:
///     uint64_t GetBaseAddressForCurrentModule()
///     {
///         void* pInstruction;
///         EAGetInstructionPointer(pInstruction);
///         return GetModuleBaseAddressByAddress(pInstruction);
///     }
///
/// @param pCodeAddress: An address to a piece of executable code in the given module.
///
/// @return: Runtime base address that the given module was loaded. kBaseAddressInvalid on failure.
///
EACALLSTACK_API uint64_t GetModuleBaseAddressByAddress(const void* pCodeAddress);

///////////////////////////////////////////////////////////////////////////////
/// operator==(const ModuleInfo& lhs, const ModuleInfo& rhs)
///
/// Compares the contents of lhs and rhs and returns true if they are equal.
/// lhs and rhs member variable mPath is expected to be a normalized path (not a relative path specifier or a symbolic link)
///
/// @param lhs: first ModuleInfo to compare
///
/// @param rhs: second ModuleInfo to compare
///
/// @return: true if the ModuleInfo's are equivalent. false otherwise.
EACALLSTACK_API bool operator ==(const ModuleInfo& lhs, const ModuleInfo& rhs);


///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfo
///
/// Copies up to moduleArrayCapacity ModuleInfo objects to pModuleInfoArray.
/// Returns the number of modules copied to pModuleInfoArray.
/// If pModuleInfoArray is NULL, then the return value is the required moduleArrayCapacity.
///
/// @param pModuleInfoArray: array of ModuleInfos to be copied into.
///                          if pmoduleinfoarray == NULL, then the return value is the required moduleArrayCapacity
///
/// @return: returns the number of modules copied into pModuleInfoArray.
///
EACALLSTACK_API size_t GetModuleInfo(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity);


///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandle
///
/// Obtains the ModuleInfo for a given ModuleHandle.
/// Populates the ModuleInfo into the given outparam
/// Returns true if the ModuleInfo could be obtained
///
/// @param moduleHandle: The handle to the module to get the info for
///
/// @param moduleInfo: The info related to moduleHandle
///
/// @return: true if the ModuleInfo could be obtained. false otherwise.
///
EACALLSTACK_API bool GetModuleInfoByHandle(ModuleHandle moduleHandle, ModuleInfo& moduleInfo);


///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleList
///
/// Obtains the ModuleHandles of the modules currently loaded in a process
/// Copies up to nModuleArrayCapacity ModuleHandle objects to pModuleHandleArray.
/// Returns the number of modules copied to pModuleHandleArray.
/// If pModuleHandleArray is NULL, then the return value is the required moduleArrayCapacity.
///
/// @param pModuleHandleArray: array of ModuleHandles to be copied into.
///                            if pModuleHandleArray == NULL, then the return value is the required nModuleArrayCapacity
///
/// @return: returns the number of modules copied into pModuleHandleArray.
///
EACALLSTACK_API size_t GetModuleHandleList(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity);


///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByAddress
///
/// Loops through pModuleInfoArray and finds module which contains the given pCodeAddress.
/// Copies the ModuleInfo into the given outparam
/// Returns true if module was found in pModuleInfoArray
///
/// @param pCodeAddress: The address to find in a module
///
/// @param moduleInfo: The given ModuleInfo that was found
///
/// @param pModuleInfoArray: The array of ModuleInfos to search
///
/// @param moduleCount: The size of pModuleInfoArray
///
/// @return: true if the module was found in the given array. false otherwise.
///
EACALLSTACK_API bool GetModuleInfoByAddress(const void* pCodeAddress, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray,
											size_t moduleCount);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandle
///
/// Loops through pModuleInfoArray and finds the module with the same handle as moduleHandle.
/// Copies the ModuleInfo into the given outparam.
/// Returns true if module was found in pModuleInfoArray
///
/// @param moduleHandle: The handle of the module to find in the array
///
/// @param moduleInfo: The given ModuleInfo that was found
///
/// @param pModuleInfoArray: The array of ModuleInfos to search
///
/// @param moduleCount: The size of pModuleInfoArray
///
/// @return: true if the module was found in the given array. false otherwise.
///
EACALLSTACK_API bool GetModuleInfoByHandle(ModuleHandle moduleHandle, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray,
										   size_t moduleCount);

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByName
///
/// Loops through pModuleInfoArray and finds the module with same name as pModuleName.
/// Copies the ModuleInfo into the given outparam.
/// Returns true if module was found in pModuleInfoArray
///
/// @param pModuleName: The name of the module to find in the array
///
/// @param moduleInfo: The given ModuleInfo that was found
///
/// @param pModuleInfoArray: The array of ModuleInfos to search
///
/// @param moduleCount: The size of pModuleInfoArray
///
/// @return: true if the module was found in the given array. false otherwise.
///
EACALLSTACK_API bool GetModuleInfoByName(const char* pModuleName, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray,
										 size_t moduleCount);


} // namespace Callstack
} // namespace EA
