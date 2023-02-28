///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <coreallocator/icoreallocatormacros.h>
#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/ELF.h>
#include <EACallstack/internal/EACallstackUtils.h>
#include <EACallstack/EAModule.h>
#include <EACallstack/EAAddressRep.h>
#include <EACallstack/Allocator.h>
#include <EACallstack/MapFile.h>
#include <EACallstack/PDBFile.h>
#include <EACallstack/DWARF3File.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/EASymbolUtil.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAScanf.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAProcess.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileDirectory.h>
#include <EAAssert/eaassert.h>

EA_DISABLE_CLANG_WARNING(-Wunused-function);
EA_DISABLE_VC_WARNING(4505); // unreferenced local function has been removed

namespace EA
{
namespace Callstack
{

namespace EAModuleImpl
{
/**
 * These are the functions that must be implemented per platform.
 * Any error checking, asserting, or common code is done once
 * with the actual work delegated to these platform functions.
 */

EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* address, char* pModuleName, size_t moduleNameCapacity);
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress);
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const wchar_t* pModuleName);
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const char* pModuleName);
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle moduleHandle);
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void* pCodeAddress);
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity);
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo);
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity);

/**
 * Returns the ModuleHandle to the executable that created the calling process, (.elf, .exe).
 */
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl();


} // namespace EAModuleImpl

/**
 * Helpers to transform from UTF8 to UCS4 and vice versa.
 */
template <typename F, typename Ret>
static auto TransformToWChar(const char* str, F&& f, Ret defaultReturnValue)
{
	wchar_t moduleFileNameW[EA::IO::kMaxPathLength];
	moduleFileNameW[0] = '\0';

	int result = EA::StdC::Strlcpy(moduleFileNameW, str, EA::IO::kMaxPathLength);
	if ((result >= 0) && (result < EA::IO::kMaxPathLength))
	{
		return f(moduleFileNameW);
	}

	return defaultReturnValue;
}

template <typename F, typename Ret>
static auto TransformToChar(const wchar_t* str, F&& f, Ret defaultReturnValue)
{
	char moduleFileName8[EA::IO::kMaxPathLength];
	moduleFileName8[0] = '\0';

	int result = EA::StdC::Strlcpy(moduleFileName8, str, EA::IO::kMaxPathLength);
	if ((result >= 0) && (result < EA::IO::kMaxPathLength))
	{
		return f(moduleFileName8);
	}

	return defaultReturnValue;
}

///////////////////////////////////////////////////////////////////////////////
/// FileSystemCaseCompare
///
/// Compares two FixedString8 based on whether the filesystem is case insensitive
/// or case sensitive.
static bool FileSystemCaseCompare(const FixedString8& lhs, const FixedString8& rhs)
{
#if EA_FILE_SYSTEM_CASE_SENSITIVE

	return lhs == rhs;

#else

	return (lhs.comparei(rhs) == 0);

#endif // EA_FILE_SYSTEM_CASE_SENSITIVE
}

///////////////////////////////////////////////////////////////////////////////
/// FileSystemCaseCompare
///
/// Compares a FixedString8 to a const char*
/// based on whether the filesystem is case insensitive or case sensitive.
static bool FileSystemCaseCompare(const FixedString8& lhs, const char* rhs)
{
#if EA_FILE_SYSTEM_CASE_SENSITIVE

	return lhs == rhs;

#else

	return (lhs.comparei(rhs) == 0);

#endif // EA_FILE_SYSTEM_CASE_SENSITIVE
}

///////////////////////////////////////////////////////////////////////////////
/// ClearModuleInfo
///
static void ClearModuleInfo(ModuleInfo& moduleInfo)
{
	moduleInfo.mModuleHandle = kModuleHandleInvalid;
	moduleInfo.mPath.clear();
	moduleInfo.mName.clear();
	moduleInfo.mBaseAddress = kBaseAddressInvalid;
	moduleInfo.mSize = 0;
}

///////////////////////////////////////////////////////////////////////////////
/// operator ==(const ModuleInfo& lhs, const ModuleInfo& rhs)
///
EACALLSTACK_API bool operator ==(const ModuleInfo& lhs, const ModuleInfo& rhs)
{
	return (lhs.mSize == rhs.mSize) && (lhs.mBaseAddress == rhs.mBaseAddress) &&
			FileSystemCaseCompare(lhs.mName, rhs.mName) && FileSystemCaseCompare(lhs.mPath, rhs.mPath);
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleFromAddress
///
EACALLSTACK_API size_t GetModuleFromAddress(const void* pAddress, char* pModuleFileName, size_t nModuleNameCapacity)
{
	EA_ASSERT(((pModuleFileName != nullptr) && (nModuleNameCapacity > 0)));

	size_t ret = 0;

	if (pAddress)
	{
		ret = EAModuleImpl::GetModuleFromAddressImpl(pAddress, pModuleFileName, nModuleNameCapacity);
	}

	if (ret == 0)
	{
		pModuleFileName[0] = '\0';
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleFromAddress
///
EACALLSTACK_API ModuleHandle GetModuleHandleFromAddress(const void* pAddress)
{
	if (pAddress == nullptr)
	{
		return kModuleHandleInvalid;
	}

	return EAModuleImpl::GetModuleHandleFromAddressImpl(pAddress);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddress
//
EACALLSTACK_API uint64_t GetModuleBaseAddress(const char* pModuleName)
{
	if (!pModuleName || pModuleName[0] == '\0')
	{
		return GetModuleBaseAddressByHandle(kModuleHandleInvalid);
	}

	return EAModuleImpl::GetModuleBaseAddressImpl(pModuleName);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddress
//
EACALLSTACK_API uint64_t GetModuleBaseAddress(const wchar_t* pModuleName)
{
	if (!pModuleName || pModuleName[0] == '\0')
	{
		return GetModuleBaseAddressByHandle(kModuleHandleInvalid);
	}

	return EAModuleImpl::GetModuleBaseAddressImpl(pModuleName);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandle
//
EACALLSTACK_API uint64_t GetModuleBaseAddressByHandle(ModuleHandle moduleHandle)
{
	if (moduleHandle == kModuleHandleInvalid)
	{
		moduleHandle = EAModuleImpl::GetModuleHandleOfExecImpl();
	}

	if (moduleHandle == kModuleHandleInvalid)
	{
		return kBaseAddressInvalid;
	}

	uint64_t ret = kBaseAddressInvalid;

	ret = EAModuleImpl::GetModuleBaseAddressByHandleImpl(moduleHandle);

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddress
//
EACALLSTACK_API uint64_t GetModuleBaseAddressByAddress(const void* pCodeAddress)
{
	if (pCodeAddress == nullptr)
	{
		return kBaseAddressInvalid;
	}

	return EAModuleImpl::GetModuleBaseAddressByAddressImpl(pCodeAddress);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleInfo
//
EACALLSTACK_API size_t GetModuleInfo(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
	EA_ASSERT((pModuleInfoArray != nullptr) || (moduleArrayCapacity == 0));

	return EAModuleImpl::GetModuleInfoImpl(pModuleInfoArray, moduleArrayCapacity);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoByHandle
//
EACALLSTACK_API bool GetModuleInfoByHandle(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
	EA_ASSERT(moduleHandle != kModuleHandleInvalid);

	return EAModuleImpl::GetModuleInfoByHandleImpl(moduleHandle, moduleInfo);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleList
//
EACALLSTACK_API size_t GetModuleHandleList(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity)
{
	EA_ASSERT((pModuleHandleArray != nullptr) || (nModuleArrayCapacity == 0));

	return EAModuleImpl::GetModuleHandleListImpl(pModuleHandleArray, nModuleArrayCapacity);
}


///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByName
///
EACALLSTACK_API bool GetModuleInfoByName(const char* pModuleName, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray, size_t moduleCount)
{
	for (size_t i = 0; i < moduleCount; ++i)
	{
		if (EA::StdC::Stristr(pModuleInfoArray[i].mName.c_str(), pModuleName))
		{
			moduleInfo = pModuleInfoArray[i];
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandle
///
EACALLSTACK_API bool GetModuleInfoByHandle(ModuleHandle moduleHandle, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray, size_t moduleCount)
{
	for (size_t i = 0; i < moduleCount; ++i)
	{
		if (pModuleInfoArray[i].mModuleHandle == moduleHandle)
		{
			moduleInfo = pModuleInfoArray[i];
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByAddress
///
EACALLSTACK_API bool GetModuleInfoByAddress(const void* pCodeAddress, ModuleInfo& moduleInfo, ModuleInfo* pModuleInfoArray, size_t moduleCount)
{
	uint64_t address = reinterpret_cast<uint64_t>(pCodeAddress);
	for (size_t i = 0; i < moduleCount; ++i)
	{
		if ((pModuleInfoArray[i].mBaseAddress <= address) &&
			(address < (pModuleInfoArray[i].mBaseAddress + pModuleInfoArray[i].mSize)))
		{
			moduleInfo = pModuleInfoArray[i];
			return true;
		}
	}
	return false;
}

} // namespace Callstack
} // namespace EA


/**
 *  Include the Platform Specific Code that implements Modules for that given Platform
 */
#if defined(EA_PLATFORM_MICROSOFT)

	#include "platform/Win32/EAModule_Win32.cpp"

#elif defined(EA_PLATFORM_SONY)

	#include "platform/Sony/EAModule_Sony.cpp"

#elif defined(EA_PLATFORM_APPLE)

	#include "platform/Apple/EAModule_Apple.cpp"

#elif defined(EA_PLATFORM_LINUX)

	#include "platform/unix/EAModule_unix.cpp"

#elif defined(EA_PLATFORM_NX)

	#include "platform/nx/EAModule_nx.cpp"

#elif defined(EA_PLATFORM_ANDROID)

	#include "platform/unix/EAModule_unix.cpp"

#else

	#include "platform/null/EAModule_null.cpp"

#endif

EA_RESTORE_CLANG_WARNING();
EA_RESTORE_VC_WARNING();
