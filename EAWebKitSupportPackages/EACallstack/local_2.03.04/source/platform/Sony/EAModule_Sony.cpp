///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EACallstackUtils.h>

#include <sdk_version.h>
#include <scebase_common.h>
#include <kernel.h>
#include <sys/dmem.h>

#if SCE_ORBIS_SDK_VERSION >= 0x04000000u || SCE_PROSPERO_SDK_VERSION

	#if EACALLSTACK_SCEDBG_ENABLED

		#include <libdbg.h>

		#define EACALLSTACK_SONYGETMODULE_AVAILABLE
		#define EACALLSTACK_SCEMODULE SceDbgModule
		#define EACALLSTACK_SCEMODULEINFO SceDbgModuleInfo
		#define EACALLSTACK_SCESEGMENTINFO SceDbgModuleSegmentInfo
		#define EACALLSTACK_SCEMODULEINFO_FUNCTION sceDbgGetModuleInfo
		#define EACALLSTACK_SCEMODULELIST_FUNCTION sceDbgGetModuleList

	#endif

#else

	#define EACALLSTACK_SONYGETMODULE_AVAILABLE
	#define EACALLSTACK_SCEMODULE SceKernelModule
	#define EACALLSTACK_SCEMODULEINFO SceKernelModuleInfo
	#define EACALLSTACK_SCESEGMENTINFO SceKernelModuleSegmentInfo
	#define EACALLSTACK_SCEMODULEINFO_FUNCTION sceKernelGetModuleInfo
	#define EACALLSTACK_SCEMODULELIST_FUNCTION sceKernelGetModuleList

#endif

#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAAlignment.h>

#include <EAIO/EAFileBase.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileUtil.h>

namespace EA
{
namespace Callstack
{

namespace Detail
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

static void SceModuleInfoInit(EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
{
	sceModuleInfo.size = sizeof(EACALLSTACK_SCEMODULEINFO);
}

static const EACALLSTACK_SCESEGMENTINFO* GetFirstSegment(const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
{
	if (sceModuleInfo.numSegments > 0)
	{
		return &(sceModuleInfo.segmentInfo[0]);
	}

	return nullptr;
}

static const EACALLSTACK_SCESEGMENTINFO* GetMatchingSegment(const void* pAddress, const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
{
	const uint64_t address = reinterpret_cast<uint64_t>(pAddress);
	for (uint32_t i = 0; i < sceModuleInfo.numSegments; ++i)
	{
		const uint64_t baseAddress = reinterpret_cast<uint64_t>(sceModuleInfo.segmentInfo[i].baseAddr);
		const uint64_t endAddress = baseAddress + static_cast<uint64_t>(sceModuleInfo.segmentInfo[i].size);
		/**
		 * Segment Info refers to ELF segments that were flagged to be mmap into memory.
		 * Find a matching segment that had the EXEC flag, meaning this section can be executed.
		 */
		const int32_t protection = sceModuleInfo.segmentInfo[i].prot;
		if ((protection & SCE_DBG_PROT_CPU_EXEC) == SCE_DBG_PROT_CPU_EXEC)
		{
			if ((address >= baseAddress) && (address < endAddress))
			{
				return &(sceModuleInfo.segmentInfo[i]);
			}
		}
	}

	return nullptr;
}

static uint64_t GetModuleSize(const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
{
	constexpr size_t pageSize = 4096;
	uintptr_t currentStartAddr = 0;
	uintptr_t currentEndAddr = 0;

	const EACALLSTACK_SCESEGMENTINFO* firstSegment = Detail::GetFirstSegment(sceModuleInfo);
	if (!firstSegment)
	{
		return 0;
	}

	currentStartAddr = EA::StdC::AlignDown(reinterpret_cast<uintptr_t>(firstSegment->baseAddr), pageSize);
	currentEndAddr = EA::StdC::AlignUp(static_cast<uintptr_t>(firstSegment->size) + currentStartAddr, pageSize);

	for (unsigned i = 1; i < sceModuleInfo.numSegments; ++i)
	{
		const EACALLSTACK_SCESEGMENTINFO* currentSegment = &(sceModuleInfo.segmentInfo[i]);

		if (EA::StdC::AlignUp(reinterpret_cast<uintptr_t>(currentSegment->baseAddr) + static_cast<uintptr_t>(currentSegment->size),
							  pageSize) < currentEndAddr)
		{
			/**
			 * Segments may not be ordered, if this segment is overlapped with the current loadable region, than its size is already
			 * accounted for.
			 */

			continue;
		}

		uintptr_t segmentStart = EA::StdC::AlignDown(reinterpret_cast<uintptr_t>(currentSegment->baseAddr), pageSize);
		if (segmentStart < currentEndAddr)
		{
			segmentStart = currentEndAddr;
		}
		uintptr_t segmentEnd = EA::StdC::AlignUp(static_cast<uintptr_t>(currentSegment->size) + segmentStart, pageSize);

		currentEndAddr = segmentEnd;
	}

	const uintptr_t imageSize = currentEndAddr - currentStartAddr;
	return static_cast<uint64_t>(imageSize);
}

static void FillOutModuleInfo(bool emptyFill, ModuleInfo& moduleInfo, ModuleHandle moduleHandle,
							  const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
{
	if (!emptyFill)
	{
		moduleInfo.mModuleHandle = moduleHandle;

		moduleInfo.mPath = sceModuleInfo.name; // "/app_home/prx/libkernel.spx"

		moduleInfo.mName = EA::IO::Path::GetFileName(sceModuleInfo.name); // "libkernel.spx"

		const EACALLSTACK_SCESEGMENTINFO* firstSegment = Detail::GetFirstSegment(sceModuleInfo);
		moduleInfo.mBaseAddress = firstSegment ? reinterpret_cast<uint64_t>(firstSegment->baseAddr) : kBaseAddressInvalid;

		moduleInfo.mSize = Detail::GetModuleSize(sceModuleInfo);
	}
	else
	{
		ClearModuleInfo(moduleInfo);
	}
}

/**
 * Iterate over all loaded modules, calling bool F(EACALLSTACK_SCEMODULE, EACALLSTACK_SCEMODULEINFO) where the return
 * value indicates whether to continue iterating.
 * IterateSceModuleList() returns true only when F() indicates to stop iterating as that means an entry was found, false otherwise.
 */
template <typename F>
static bool IterateSceModuleList(F&& f)
{
	EACALLSTACK_SCEMODULE sceModuleArray[EA::Callstack::kModuleMaxCapacity];
	size_t moduleCount = 0;

	int32_t ret = EACALLSTACK_SCEMODULELIST_FUNCTION(sceModuleArray, EA::Callstack::kModuleMaxCapacity, &moduleCount);
	if (ret != SCE_OK)
	{
		return false;
	}

	for (size_t i = 0; i < moduleCount; ++i)
	{
		EACALLSTACK_SCEMODULEINFO sceModuleInfo;
		Detail::SceModuleInfoInit(sceModuleInfo);

		ret = EACALLSTACK_SCEMODULEINFO_FUNCTION(sceModuleArray[i], &sceModuleInfo);
		if (ret != SCE_OK)
		{
			return false;
		}

		if (!f(sceModuleArray[i], sceModuleInfo))
		{
			return true;
		}
	}

	return false;
}

#endif // EACALLSTACK_SONYGETMODULE_AVAILABLE

} // namespace Detail


namespace EAModuleImpl
{

///////////////////////////////////////////////////////////////////////////////
// GetModuleFromAddressImpl
//
EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* pAddress, char* pModuleName, size_t nModuleNameCapacity)
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	int result = -1;

	if (Detail::IterateSceModuleList([&](EACALLSTACK_SCEMODULE sceModule, const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
									 {
										 const EACALLSTACK_SCESEGMENTINFO* segment = Detail::GetMatchingSegment(pAddress, sceModuleInfo);
										 if (segment)
										 {
											 const char* filename = EA::IO::Path::GetFileName(sceModuleInfo.name);

											 result = EA::StdC::Strlcpy(pModuleName, filename, nModuleNameCapacity);

											 return false;
										 }

										 return true;
									 }))
	{
		if (result > 0)
		{
			return static_cast<size_t>(result);
		}
	}

#endif

	pModuleName[0] = '\0';
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleFromAddressImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress)
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	ModuleHandle moduleHandle = kModuleHandleInvalid;

	if (Detail::IterateSceModuleList([&](EACALLSTACK_SCEMODULE sceModule, const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
									 {
										 const EACALLSTACK_SCESEGMENTINFO* segment = Detail::GetMatchingSegment(pAddress, sceModuleInfo);
										 if (segment)
										 {
											 moduleHandle = static_cast<ModuleHandle>(sceModule);

											 return false;
										 }

										 return true;
									 }))
	{
		return moduleHandle;
	}

#else

	EA_UNUSED(pAddress);

#endif

	return kModuleHandleInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const wchar_t* pModuleName)
{
	return TransformToChar(pModuleName,
						   [](const char* moduleFileName) -> uint64_t { return GetModuleBaseAddressImpl(moduleFileName); },
						   kBaseAddressInvalid);
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const char* pModuleName)
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	char moduleFileName[EA::IO::kMaxPathLength];

	Utils::GetFileNameWithoutExt(moduleFileName, EA::IO::kMaxPathLength, pModuleName);


	uint64_t baseAddress = kBaseAddressInvalid;

	Detail::IterateSceModuleList([&](EACALLSTACK_SCEMODULE sceModule, const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
								 {
									 // "scePrxCallPrxExport" or "/app_home/prx/call-prx-export.sprx"
									 if (EA::StdC::Stristr(sceModuleInfo.name, moduleFileName))
									 {
										 const EACALLSTACK_SCESEGMENTINFO* firstSegment = Detail::GetFirstSegment(sceModuleInfo);
										 if (firstSegment)
										 {
											 baseAddress = reinterpret_cast<uint64_t>(firstSegment->baseAddr);
										 }

										 return false;
									 }

									 return true;
								 });

	return baseAddress;

#else

	EA_UNUSED(pModuleName);

#endif

	return kBaseAddressInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandleImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle moduleHandle)
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	EACALLSTACK_SCEMODULE sceModuleHandle = static_cast<EACALLSTACK_SCEMODULE>(moduleHandle);

	EACALLSTACK_SCEMODULEINFO sceModuleInfo;
	Detail::SceModuleInfoInit(sceModuleInfo);

	int32_t result = EACALLSTACK_SCEMODULEINFO_FUNCTION(sceModuleHandle, &sceModuleInfo);
	if (result == SCE_OK)
	{
		const EACALLSTACK_SCESEGMENTINFO* firstSegment = Detail::GetFirstSegment(sceModuleInfo);
		if (firstSegment)
		{
			return reinterpret_cast<uint64_t>(firstSegment->baseAddr);
		}
	}

#else

	EA_UNUSED(moduleHandle);

#endif

	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void* pCodeAddress)
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	uint64_t baseAddress = kBaseAddressInvalid;

	if (Detail::IterateSceModuleList([&](EACALLSTACK_SCEMODULE sceModule, const EACALLSTACK_SCEMODULEINFO& sceModuleInfo)
									 {
										 const EACALLSTACK_SCESEGMENTINFO* segment = Detail::GetMatchingSegment(pCodeAddress, sceModuleInfo);
										 if (segment)
										 {
											 /**
											  * The given address is in on the of the modules segments.
											  * Return the First segment base address which is the load address of the entire module
											  * since debug info is relative to the load address of the module, not the load address
											  * of a segment, since each segment is relative to the first segment
											  */
											 baseAddress = reinterpret_cast<uint64_t>(Detail::GetFirstSegment(sceModuleInfo)->baseAddr);

											 return false;
										 }

										 return true;
									 }))
	{
		return baseAddress;
	}

#else

	EA_UNUSED(pCodeAddress);

#endif

	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleListImpl
///
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity)
{
	size_t moduleCount = 0;

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	EACALLSTACK_SCEMODULE sceModuleArray[EA::Callstack::kModuleMaxCapacity];

	int32_t ret = EACALLSTACK_SCEMODULELIST_FUNCTION(sceModuleArray, EA::Callstack::kModuleMaxCapacity, &moduleCount);
	if (ret == SCE_OK)
	{
		if (pModuleHandleArray)
		{
			moduleCount = eastl::min_alt(moduleCount, nModuleArrayCapacity);

			EA::StdC::Memcpy(pModuleHandleArray, sceModuleArray, sizeof(ModuleHandle) * moduleCount);
		}
	}

#else

	EA_UNUSED(pModuleHandleArray);
	EA_UNUSED(nModuleArrayCapacity);

#endif

	return moduleCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
	size_t moduleCount = 0;

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	EACALLSTACK_SCEMODULE sceModuleArray[EA::Callstack::kModuleMaxCapacity];

	int32_t result = EACALLSTACK_SCEMODULELIST_FUNCTION(sceModuleArray, EA::Callstack::kModuleMaxCapacity, &moduleCount);
	if (result == SCE_OK)
	{
		if (pModuleInfoArray)
		{
			moduleCount = eastl::min_alt(moduleCount, moduleArrayCapacity);

			for (size_t i = 0; i < moduleCount; ++i)
			{
				EACALLSTACK_SCEMODULEINFO sceModuleInfo;
				Detail::SceModuleInfoInit(sceModuleInfo);

				ModuleInfo& moduleInfo = pModuleInfoArray[i];

				result = EACALLSTACK_SCEMODULEINFO_FUNCTION(sceModuleArray[i], &sceModuleInfo);

				Detail::FillOutModuleInfo(result != SCE_OK, moduleInfo, sceModuleArray[i], sceModuleInfo);
			}
		}
	}

#else

	EA_UNUSED(pModuleInfoArray);
	EA_UNUSED(moduleArrayCapacity);

#endif

	return moduleCount;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandleImpl
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
	bool ret = false;

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	EACALLSTACK_SCEMODULE sceModule = static_cast<EACALLSTACK_SCEMODULE>(moduleHandle);
	EACALLSTACK_SCEMODULEINFO sceModuleInfo;
	Detail::SceModuleInfoInit(sceModuleInfo);

	int32_t result = EACALLSTACK_SCEMODULEINFO_FUNCTION(sceModule, &sceModuleInfo);
	if (result == SCE_OK)
	{
		Detail::FillOutModuleInfo(false, moduleInfo, sceModule, sceModuleInfo);

		ret = true;
	}

#else

	EA_UNUSED(moduleHandle);
	EA_UNUSED(moduleInfo);

#endif

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleOfExecImpl
///
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{

#if defined(EACALLSTACK_SONYGETMODULE_AVAILABLE)

	size_t moduleCount = 0;

	EACALLSTACK_SCEMODULE sceModuleArray[EA::Callstack::kModuleMaxCapacity];

	int32_t result = EACALLSTACK_SCEMODULELIST_FUNCTION(sceModuleArray, EA::Callstack::kModuleMaxCapacity, &moduleCount);
	if (result == SCE_OK)
	{
		/* Sony guarantees that the first element, sceModuleArray[0] is the handle to the executable */
		ModuleHandle moduleHandle = static_cast<ModuleHandle>(sceModuleArray[0]);

		return moduleHandle;
	}

#endif

	return kModuleHandleInvalid;

}

} // namespace EAModuleImpl

} // namespace Callstack
} // namespace EA
