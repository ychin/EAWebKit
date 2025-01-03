///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EACallstackUtils.h>

#include <EAStdC/EAProcess.h>
#include <EAStdC/EAAlignment.h>

#include <EAIO/PathString.h>
#include <EAIO/EAFileBase.h>

#if defined(EA_PLATFORM_ANDROID)

	#if __ANDROID_API__ >= 21

		#define EAMODULE_DL_EXTENSIONS_ENABLED 1

	#elif __ANDROID_API__ < 21

		#define EAMODULE_PROC_MAPS_ENABLED 1

	#else

		#define EAMODULE_DL_EXTENSIONS_ENABLED 0

	#endif // __ANDROID_API__

#elif defined(EA_PLATFORM_LINUX)

	#define EAMODULE_DL_EXTENSIONS_ENABLED 1

#endif // EA_PLATFORM_*


#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

/**
 * NOTE(mwinkler): dladdr() is a nonstandard posix extension that is present on Linux, Solaris, Illumos and the BSDs
 *                 with some caveats between the different implementations. For now, support only Linux.
 */

#if !defined(_GNU_SOURCE)

	#define _GNU_SOURCE

#endif

#if EAMODULE_DL_EXTENSIONS_ENABLED

	#include <link.h>
	#include <elf.h>

#endif // EAMODULE_DL_EXTENSIONS_ENABLED

#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * On glibc, a module handle from dl_open() points to a link_map.
 * The dl_phdr_info and link_map base address pointers are both the difference from the ELF File vaddr and the in memory load address
 * The Dl_info->dli_fbase is actually the link_map->l_map_start because of this.
 */

#endif // EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

#include <stdio.h>

#if defined(EA_PLATFORM_ANDROID) || defined(EA_PLATFORM_LINUX)

	#define EAMODULE_DLOPEN_FLAGS (RTLD_LAZY | RTLD_NOLOAD)

#else

	#define EAMODULE_DLOPEN_FLAGS (RTLD_LAZY)

#endif

/**
 * Base Address
 * Executable and shared object files have a base address, which is the lowest virtual address associated with the memory image of the
 * program's object file. One use of the base address is to relocate the memory image of the program during dynamic linking.
 *
 * An executable or shared object file's base address is calculated during execution from three values: the memory load address,
 * the maximum page size, and the lowest virtual address of a program's loadable segment. The virtual addresses in the program headers
 * might not represent the actual virtual addresses of the program's memory image.
 *
 * To compute the base address, you determine the memory address associated with the lowest p_vaddr value for a PT_LOAD segment.
 * You then obtain the base address by truncating the memory address to the nearest multiple of the maximum page size.
 * Depending on the kind of file being loaded into memory, the memory address might not match the p_vaddr values.
 *
 * This means the base address of a module is the base address of the first loaded PT_LOAD segment of the ELF File.
 * Region Size being page multiples of p_memsz for each PT_LOAD segment, where each segment is contiguous but does not
 * come immediately after the previous segment in memory.
 */


namespace EA
{
namespace Callstack
{

namespace Detail
{

EA_DISABLE_CLANG_WARNING(-Wunused-function);
EA_DISABLE_VC_WARNING(4505); // unreferenced local function has been removed

#if EAMODULE_PROC_MAPS_ENABLED || EAMODULE_DL_EXTENSIONS_ENABLED

/**
 * Call bool F(addrStart, addrEnd, offset, baseAddr, regsionSize, perms, filename) for each line in the /proc/self/maps file.
 * F() return value indicates whether to continue iterating.
 * ParseMaps() returns true only when F() indicates to stop iterating as that means an entry was found, false otherwise.
 */
template <typename F>
static bool ParseMaps(F&& f)
{
	/**
	 * It is common practice in Linux to expose user-space APIs via the VFS.
	 * One such example is procfs mounted under /proc during Kernel Boot
	 * Parse /proc/[pid]/maps to get the current Address Space map of process pid.
	 * /proc/self is a symlink to the calling process reading that file.
	 * The /proc/[pid]/maps virtual file format is as follows:
	 *
	 * address           perms offset   dev   inode pathname
	 * 08048000-08056000 r-xp  00000000 03:0c 64555 /usr/lib/libgcc.so
	 *
	 * address: The starting and ending address of the region
	 *
	 * perms: read, write, execute, and private/shared mapping
	 *
	 * offset: If the region is a file-backed mapping, this is the offset into the file where the mapping begins.
	 *         Anon mappings are always 0
	 *
	 * dev: If the region is a file-backed mapping, major:minor device number of the device that holds the file
	 *
	 * inode: If the region is a file-backed mapping, this is the inode number of the file for the given filesystem
	 *
	 * pathname: If the region is a file-backed mapping, this is the name of file.
	 *           Anon mappings are blank.
	 *           [heap], [stack], [vdso] are examples of some special regions set up by the Kernel.
	 */

	FILE* mapsFile = fopen("/proc/self/maps", "r");
	if (mapsFile)
	{
		constexpr unsigned lineLength = EA::IO::kMaxPathLength + 256;
		char line[lineLength];

		while (fgets(line, lineLength, mapsFile))
		{
		#if (EA_PLATFORM_PTR_SIZE == 4)

			static_assert(sizeof(unsigned) == 4, "sizeof(unsigned) must be 4 bytes on 32-bit unix platforms!");
			constexpr char scanString[] = "%x-%x %c%c%c%c %x %s %x %s";
			unsigned addrStart;
			unsigned addrEnd;
			unsigned offset;
			unsigned inode;

		#else

			static_assert(sizeof(unsigned long long) == 8, "sizeof(unsigned long long) must be 8 bytes on 64-bit unix platforms!");
			constexpr char scanString[] = "%llx-%llx %c%c%c%c %llx %s %llx %s";
			unsigned long long addrStart;
			unsigned long long addrEnd;
			unsigned long long offset;
			unsigned long long inode;

		#endif

			char perms[4] = {};
			char filename[EA::IO::kMaxPathLength];
			filename[0] = '\0';
			char dev[16];
			dev[0] = '\0';

			sscanf(line, scanString, &addrStart, &addrEnd, &perms[0], &perms[1], &perms[2], &perms[3], &offset, dev, &inode, filename);

			const uint64_t addrStart64 = static_cast<uint64_t>(addrStart);
			const uint64_t addrEnd64 = static_cast<uint64_t>(addrEnd);
			const uint64_t offset64 = static_cast<uint64_t>(offset);
			const uint64_t inode64 = static_cast<uint64_t>(inode);

			if (!f(addrStart64, addrEnd64, offset64, inode64, perms, filename))
			{
				return true;
			}

		}

		return false;
	}

	return false;
}

static void FillOutModuleInfoFromMapsWithoutSize(ModuleInfo& moduleInfo, uint64_t baseAddr, const char* pathname)
{
	moduleInfo.mBaseAddress = baseAddr;

	moduleInfo.mModuleHandle = reinterpret_cast<ModuleHandle>(baseAddr);

	moduleInfo.mPath = pathname;

	moduleInfo.mName = EA::IO::Path::GetFileName(pathname);
}

static void FillOutModuleInfoFromMapsSizeOnly(ModuleInfo& moduleInfo, uint64_t size)
{
	moduleInfo.mSize = size;
}

#endif // EAMODULE_PROC_MAPS_ENABLED || EAMODULE_DL_EXTENSIONS_ENABLED

#if EAMODULE_DL_EXTENSIONS_ENABLED

template <typename F>
static int DLIteratePHDR(F& f)
{
	auto moduleHandler = [](struct dl_phdr_info* info, size_t size, void* data) -> int
						 {
							 F& functor = *(reinterpret_cast<F*>(data));

							 return functor(info);
						 };

	return dl_iterate_phdr(moduleHandler, &f);
}

static uint64_t GetBaseAddressFromDLPHDR(struct dl_phdr_info* info)
{
	if (!info->dlpi_name || (info->dlpi_name[0] == '\0' && info->dlpi_addr == 0))
	{
		/* This is the main executable that isn't PIE */
		const ElfW(Phdr) *phdr = info->dlpi_phdr;
		for (uint64_t i = 0; i < info->dlpi_phnum; ++i)
		{
			if (phdr[i].p_type == PT_LOAD)
			{
				return static_cast<uint64_t>(phdr[i].p_vaddr);
			}
		}
	}
	else
	{
		/* This is a Dynamic Library */
		return static_cast<uint64_t>(info->dlpi_addr);
	}

	return kBaseAddressInvalid;
}

static uint64_t GetModuleSize(struct dl_phdr_info* info)
{
	constexpr size_t pageSize = 4096;
	uintptr_t currentStartAddr = 0;
	uintptr_t currentEndAddr = 0;

	uint32_t phdrIndex = 0;
	const ElfW(Phdr)* firstLoadSegment = nullptr;
	for (; phdrIndex < info->dlpi_phnum; ++phdrIndex)
	{
		if (info->dlpi_phdr[phdrIndex].p_type == PT_LOAD)
		{
			firstLoadSegment = &(info->dlpi_phdr[phdrIndex]);
			break;
		}
	}

	if (!firstLoadSegment)
	{
		return 0;
	}

	/**
	 * Loadable segment entries in the program header table appear in ascending order, sorted on the p_vaddr member
	 */

	currentStartAddr = EA::StdC::AlignDown(static_cast<uintptr_t>(firstLoadSegment->p_vaddr), pageSize);
	currentEndAddr = EA::StdC::AlignUp(static_cast<uintptr_t>(firstLoadSegment->p_memsz) + currentStartAddr, pageSize);
	++phdrIndex;

	for (; phdrIndex < info->dlpi_phnum; ++phdrIndex)
	{
		if (info->dlpi_phdr[phdrIndex].p_type != PT_LOAD)
		{
			break;
		}

		uintptr_t segmentStart = EA::StdC::AlignDown(static_cast<uintptr_t>(info->dlpi_phdr[phdrIndex].p_vaddr), pageSize);
		if (segmentStart < currentEndAddr)
		{
			segmentStart = currentEndAddr;
		}
		uintptr_t segmentEnd = EA::StdC::AlignUp(static_cast<uintptr_t>(info->dlpi_phdr[phdrIndex].p_memsz) + segmentStart, pageSize);

		currentEndAddr = segmentEnd;
	}

	const uint64_t moduleSize = currentEndAddr - currentStartAddr;
	return static_cast<uint64_t>(moduleSize);
}

static void FillOutModuleInfoFromDLPHDR(ModuleInfo& moduleInfo, struct dl_phdr_info* info)
{
	uint64_t baseAddr = Detail::GetBaseAddressFromDLPHDR(info);

	moduleInfo.mBaseAddress = baseAddr;

	moduleInfo.mModuleHandle = reinterpret_cast<ModuleHandle>(baseAddr);

	char pathArray[EA::IO::kMaxPathLength];
	pathArray[0] = '\0';
	const char* pathName = nullptr;
	if (info->dlpi_name && info->dlpi_name[0] != '\0')
	{
		pathName = info->dlpi_name;
	}
	else
	{
		pathName = pathArray;
		EA::StdC::GetCurrentProcessPath(pathArray, EA::IO::kMaxPathLength, EA::StdC::kPathFlagNone);
	}

	moduleInfo.mPath = pathName;

	moduleInfo.mName = EA::IO::Path::GetFileName(pathName);

	moduleInfo.mSize = Detail::GetModuleSize(info);
}

#endif // EAMODULE_DL_EXTENSIONS_ENABLED

EA_RESTORE_VC_WARNING();
EA_RESTORE_CLANG_WARNING();

} // namespace Detail

namespace EAModuleImpl
{

///////////////////////////////////////////////////////////////////////////////
// GetModuleFromAddressImpl
//
EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* pAddress, char* pModuleFileName, size_t moduleNameCapacity)
{
#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pAddress), &dlInfo);

	if (ret != 0 && dlInfo.dli_fname)
	{
		return EA::StdC::Strlcpy(pModuleFileName, dlInfo.dli_fname, moduleNameCapacity);
	}

#else

	EA_UNUSED(pAddress);
	EA_UNUSED(pModuleFileName);
	EA_UNUSED(moduleNameCapacity);

#endif

	pModuleFileName[0] = '\0';
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleFromAddressImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress)
{
#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pAddress), &dlInfo);

	if (ret != 0 && dlInfo.dli_fname)
	{
		return reinterpret_cast<ModuleHandle>(dlInfo.dli_fbase);
	}

	return kModuleHandleInvalid;

#else

	EA_UNUSED(pAddress);

	return kModuleHandleInvalid;

#endif
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

	char pModuleFileName[EA::IO::kMaxPathLength];

	Utils::GetFileNameWithoutExt(pModuleFileName, EA::IO::kMaxPathLength, pModuleName);

#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

	uint64_t result = kBaseAddressInvalid;

	auto matchModule = [&](uint64_t addrStart, uint64_t addrEnd, uint64_t offset, uint64_t inode, const char* perms, const char* moduleName)
					   {
						   /**
							* Get the first mapping, as that is the base address and everything is relative to the first loaded segment
							*/
						   const char* mapsModuleFileName = EA::IO::Path::GetFileName(moduleName);
						   if (EA::StdC::Stristr(mapsModuleFileName, pModuleFileName))
						   {
							   result = addrStart;

							   return false;
						   }

						   return true;
					   };

	Detail::ParseMaps(matchModule);

	return result;

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

#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

	return reinterpret_cast<uint64_t>(moduleHandle);

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
#if EAMODULE_DL_EXTENSIONS_ENABLED || EAMODULE_PROC_MAPS_ENABLED

	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pCodeAddress), &dlInfo);

	if (ret == 0 || !dlInfo.dli_fname)
	{
		return kBaseAddressInvalid;
	}
	else
	{
		return reinterpret_cast<uint64_t>(dlInfo.dli_fbase);
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
#if EAMODULE_DL_EXTENSIONS_ENABLED

	size_t moduleCount = 0;

	auto moduleHandler = [&](struct dl_phdr_info* info) -> int
						 {
							 if (pModuleHandleArray)
							 {
								 if (moduleCount < nModuleArrayCapacity)
								 {
									 uint64_t baseAddress = Detail::GetBaseAddressFromDLPHDR(info);
									 pModuleHandleArray[moduleCount] = reinterpret_cast<ModuleHandle>(baseAddress);

									 ++moduleCount;

									 return 0;
								 }
								 else
								 {
									 return -1;
								 }
							 }
							 else
							 {
								 ++moduleCount;

								 return 0;
							 }
						 };

	Detail::DLIteratePHDR(moduleHandler);

	return moduleCount;

#elif EAMODULE_PROC_MAPS_ENABLED

	size_t moduleCount = 0;
	uint64_t foundInode = 0;
	uint64_t foundStartAddr = 0;
	uint64_t prevAddrEnd = 0;

	auto moduleHandler = [&](uint64_t addrStart, uint64_t addrEnd, uint64_t offset, uint64_t inode,
							 const char* perms, const char* moduleName) -> bool
						 {
							 if (pModuleHandleArray)
							 {
								 if (moduleCount < nModuleArrayCapacity)
								 {
									 if (foundInode > 0)
									 {
										 if (inode != foundInode)
										 {
											 foundInode = 0;

											 pModuleHandleArray[moduleCount] = reinterpret_cast<ModuleHandle>(foundStartAddr);

											 ++moduleCount;
										 }
										 else
										 {
											 prevAddrEnd = addrEnd;
										 }
									 }

									 if ((moduleCount < nModuleArrayCapacity) && (inode > 0) && (inode != foundInode))
									 {
										 foundInode = inode;

										 foundStartAddr = addrStart;
										 prevAddrEnd = addrEnd;
									 }

									 return true;
								 }
								 else
								 {
									 return false;
								 }
							 }
							 else
							 {
								 if (foundInode > 0)
								 {
									 if (inode != foundInode)
									 {
										 foundInode = 0;

										 ++moduleCount;
									 }
									 else
									 {
										 prevAddrEnd = addrEnd;
									 }
								 }

								 if ((inode > 0) && (inode != foundInode))
								 {
									 foundInode = inode;

									 foundStartAddr = addrStart;
									 prevAddrEnd = addrEnd;
								 }

								 return true;
							 }
						 };

	Detail::ParseMaps(moduleHandler);

	return moduleCount;

#else

	EA_UNUSED(pModuleHandleArray);
	EA_UNUSED(nModuleArrayCapacity);

#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
#if EAMODULE_DL_EXTENSIONS_ENABLED

	size_t moduleCount = 0;

	auto moduleHandler = [&](struct dl_phdr_info* info) -> int
						 {
							 if (pModuleInfoArray)
							 {
								 if (moduleCount < moduleArrayCapacity)
								 {
									 Detail::FillOutModuleInfoFromDLPHDR(pModuleInfoArray[moduleCount], info);

									 ++moduleCount;

									 return 0;
								 }
								 else
								 {
									 return -1;
								 }
							 }
							 else
							 {
								 ++moduleCount;

								 return 0;
							 }
						 };

	Detail::DLIteratePHDR(moduleHandler);

	return moduleCount;

#elif EAMODULE_PROC_MAPS_ENABLED

	/**
	 * NOTE:
	 * With parsing /proc/self/maps it's hard to determine if a mmap() file is an ELF file or just a regular file
	 * unless we open each file and check for the ELF Magic. This why dl_iterate_phdr() is preferred.
	 */

	size_t moduleCount = 0;
	uint64_t foundInode = 0;
	uint64_t foundStartAddr = 0;
	uint64_t prevAddrEnd = 0;

	auto moduleHandler = [&](uint64_t addrStart, uint64_t addrEnd, uint64_t offset, uint64_t inode,
							 const char* perms, const char* moduleName) -> bool
					   {
						   if (pModuleInfoArray)
						   {
							   if (moduleCount < moduleArrayCapacity)
							   {
								   if (foundInode > 0)
								   {
									   if (inode != foundInode)
									   {
										   foundInode = 0;

										   Detail::FillOutModuleInfoFromMapsSizeOnly(pModuleInfoArray[moduleCount],
																					 (prevAddrEnd - foundStartAddr));

										   ++moduleCount;
									   }
									   else
									   {
										   prevAddrEnd = addrEnd;
									   }
								   }

								   if ((moduleCount < moduleArrayCapacity) && (inode > 0) && (inode != foundInode))
								   {
									   foundInode = inode;

									   foundStartAddr = addrStart;
									   prevAddrEnd = addrEnd;

									   Detail::FillOutModuleInfoFromMapsWithoutSize(pModuleInfoArray[moduleCount], addrStart, moduleName);
								   }

								   return true;
							   }
							   else
							   {
								   return false;
							   }
						   }
						   else
						   {
							   if (foundInode > 0)
							   {
								   if (inode != foundInode)
								   {
									   foundInode = 0;

									   ++moduleCount;
								   }
								   else
								   {
									   prevAddrEnd = addrEnd;
								   }
							   }

							   if ((inode > 0) && (inode != foundInode))
							   {
								   foundInode = inode;

								   foundStartAddr = addrStart;
								   prevAddrEnd = addrEnd;
							   }

							   return true;
						   }
					   };

	Detail::ParseMaps(moduleHandler);

	return moduleCount;

#else

	EA_UNUSED(pModuleInfoArray);
	EA_UNUSED(moduleArrayCapacity);

#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandleImpl
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
#if EAMODULE_DL_EXTENSIONS_ENABLED

	bool ret = false;

	auto findModule = [&](struct dl_phdr_info* info) -> int
					  {
						  uint64_t baseAddr = Detail::GetBaseAddressFromDLPHDR(info);

						  if (baseAddr == reinterpret_cast<uint64_t>(moduleHandle))
						  {
							  Detail::FillOutModuleInfoFromDLPHDR(moduleInfo, info);

							  ret = true;

							  return -1;
						  }

						  return 0;
					  };

	Detail::DLIteratePHDR(findModule);

	return ret;

#elif EAMODULE_PROC_MAPS_ENABLED

	bool ret = false;
	int64_t foundInode = 0;
	uint64_t prevAddrEnd = 0;
	uint64_t foundStartAddr = 0;

	auto findModule = [&](uint64_t addrStart, uint64_t addrEnd, uint64_t offset, uint64_t inode, const char* perms, const char* moduleName) -> bool
					  {
						  if (foundInode > 0)
						  {
							  if (inode != foundInode)
							  {
								  Detail::FillOutModuleInfoFromMapsSizeOnly(moduleInfo, (prevAddrEnd - foundStartAddr));
								  return false;
							  }
							  else
							  {
								  prevAddrEnd = addrEnd;
								  return true;
							  }
						  }

						  if (addrStart == reinterpret_cast<uint64_t>(moduleHandle))
						  {
							  foundInode = inode;

							  foundStartAddr = addrStart;
							  prevAddrEnd = addrEnd;

							  Detail::FillOutModuleInfoFromMapsWithoutSize(moduleInfo, addrStart, moduleName);

							  ret = true;

							  return true;
						  }

						  return true;
					  };

	Detail::ParseMaps(findModule);

	return ret;

#else

	EA_UNUSED(moduleHandle);
	EA_UNUSED(moduleInfo);

#endif

	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleOfExecImpl
///
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{

#if EAMODULE_DL_EXTENSIONS_ENABLED

	ModuleHandle result = kModuleHandleInvalid;

	auto findFirst = [&](struct dl_phdr_info* info) -> int
					 {
						 /* Our Executable is always the first image added to the link map */
						 uint64_t baseAddr = Detail::GetBaseAddressFromDLPHDR(info);

						 result = reinterpret_cast<ModuleHandle>(baseAddr);

						 return -1;
					 };

	Detail::DLIteratePHDR(findFirst);

	return result;

#endif

	return kModuleHandleInvalid;
}

} // namespace EAModuleImpl


} // namespace Callstack
} // namspace EA
