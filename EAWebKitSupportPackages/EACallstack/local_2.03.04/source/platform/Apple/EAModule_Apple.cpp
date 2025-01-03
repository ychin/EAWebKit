///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/Apple/EACallstackApple.h>
#include <EASTL/algorithm.h>

#include <mach/thread_act.h>
#include <mach-o/dyld_images.h>
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include <mach/task.h>

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Apple/EACallstackApple.h>

#include <EAStdC/EAString.h>
#include <EAStdC/EAScanf.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAAlignment.h>

#include <EAIO/PathString.h>
#include <EAIO/EAFileBase.h>

#include <EAAssert/eaassert.h>

#include <EASTL/algorithm.h>

#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#if defined(EA_PLATFORM_IPHONE)

	// On iPhone, this gets pulled in dynamically through libproc.dylib
	extern "C" int proc_regionfilename(int pid, uint64_t address, void * buffer, uint32_t buffersize);

#else

	#include <libproc.h> // proc_regionfilename

#endif // EA_PLATFORM_IPHONE


#if defined(__LP64__)

	typedef struct mach_header_64     MachHeader;
	typedef struct segment_command_64 SegmentCommand;
	typedef struct section_64         Section;
	#define kLCSegment                LC_SEGMENT_64

#else

	typedef struct mach_header        MachHeader;
	typedef struct segment_command    SegmentCommand;
	typedef struct section            Section;
	#define kLCSegment                LC_SEGMENT

#endif // __LP64__


namespace EA
{
namespace Callstack
{

/**
 * NOTE(mwinkler):
 *
 * On Apple, Dl_info->dli_fbase pointer points to the mach_header at which the image is mapped.
 * On Apple, the base address/image load address always points to the mach_header
 * On Apple, dlopen() pointers are tagged based on the mode flags, RTLD_FIRST as an example
 * On Apple, dlopen() returns malloc'ed pointers to an internal ImageLoader structure
 *
 * Because of all of this, our ModuleHandle is the base address of the loaded image which also is the mach_header
 */

namespace Detail
{

/**
 * Iterate over all currently loaded images calling, bool F(imageIndex, const MachHeader*), for each image where the
 * return value states whether to continue iterating
 */
template <typename F>
static void IterateMachOImages(F&& f)
{
	uint32_t imageCount = _dyld_image_count();

	for (uint32_t i = 0; i < imageCount; ++i)
	{
		const MachHeader* machHeader = reinterpret_cast<const MachHeader*>(_dyld_get_image_header(i));

		if (!f(i, machHeader))
		{
			return;
		}
	}
}

static const SegmentCommand* GetTextSegment(const MachHeader* machHeader, uint32_t* outIndex = nullptr)
{
	uintptr_t segmentCursor = reinterpret_cast<uintptr_t>(machHeader) + sizeof(MachHeader);
	for (uint32_t i = 0; i < machHeader->ncmds; ++i)
	{
		const SegmentCommand* segmentCommand = reinterpret_cast<const SegmentCommand*>(segmentCursor);
		if (segmentCommand && segmentCommand->cmd == kLCSegment &&
			EA::StdC::Strncmp(segmentCommand->segname, SEG_TEXT, sizeof(segmentCommand->segname)) == 0)
		{
			if (outIndex)
			{
				*outIndex = i;
			}

			return segmentCommand;
		}

		segmentCursor += segmentCommand->cmdsize;
	}

	return nullptr;
}

static uint64_t GetImageSize(const MachHeader* machHeader)
{
	constexpr size_t pageSize = 4096;
	uintptr_t currentStartAddr = 0;
	uintptr_t currentEndAddr = 0;

	uint32_t segmentIndex = 0;
	const SegmentCommand* firstSegment = Detail::GetTextSegment(machHeader, &segmentIndex);
	if (!firstSegment)
	{
		return 0;
	}

	currentStartAddr = EA::StdC::AlignDown(static_cast<uintptr_t>(firstSegment->vmaddr), pageSize);
	currentEndAddr = EA::StdC::AlignUp(static_cast<uintptr_t>(firstSegment->vmsize) + currentStartAddr, pageSize);

	uintptr_t segmentCursor = reinterpret_cast<uintptr_t>(firstSegment) + firstSegment->cmdsize;
	++segmentIndex;
	for (; segmentIndex < machHeader->ncmds; ++segmentIndex)
	{
		/**
		 * It is guaranted that loadable segments appear is ascending order, sorted on SegmentCommand->vmaddr
		 */

		const SegmentCommand* segmentCommand = reinterpret_cast<const SegmentCommand*>(segmentCursor);
		if (!(segmentCommand && segmentCommand->cmd == kLCSegment))
		{
			break;
		}

		uintptr_t segmentStart = EA::StdC::AlignDown(static_cast<uintptr_t>(segmentCommand->vmaddr), pageSize);
		if (segmentStart < currentEndAddr)
		{
			segmentStart = currentEndAddr;
		}
		uintptr_t segmentEnd = EA::StdC::AlignUp(static_cast<uintptr_t>(segmentCommand->vmsize) + segmentStart, pageSize);

		currentEndAddr = segmentEnd;

		segmentCursor += segmentCommand->cmdsize;
	}

	const uintptr_t imageSize = currentEndAddr - currentStartAddr;
	return static_cast<uint64_t>(imageSize);
}

static void FillOutModuleInfo(ModuleInfo& info, const MachHeader* machHeader, const char* modulePath)
{
	info.mBaseAddress = reinterpret_cast<uint64_t>(machHeader);

	info.mModuleHandle = reinterpret_cast<ModuleHandle>(const_cast<MachHeader*>(machHeader));

	info.mPath = modulePath;

	info.mName = EA::IO::Path::GetFileName(modulePath);

	info.mSize = Detail::GetImageSize(machHeader);
}

static void FillVMProt(ModuleInfoApple& info, const SegmentCommand* segmentCommand)
{
	info.mPermissions[0] = (segmentCommand->initprot & VM_PROT_READ)    ? 'r' : '-';
	info.mPermissions[1] = (segmentCommand->initprot & VM_PROT_WRITE)   ? 'w' : '-';
	info.mPermissions[2] = (segmentCommand->initprot & VM_PROT_EXECUTE) ? 'x' : '-';
	info.mPermissions[3] = '/';
	info.mPermissions[4] = (segmentCommand->maxprot & VM_PROT_READ)    ? 'r' : '-';
	info.mPermissions[5] = (segmentCommand->maxprot & VM_PROT_WRITE)   ? 'w' : '-';
	info.mPermissions[6] = (segmentCommand->maxprot & VM_PROT_EXECUTE) ? 'x' : '-';
	info.mPermissions[7] = '\0';
}

} // namespace Detail

namespace EAModuleImpl
{

///////////////////////////////////////////////////////////////////////////////
// GetModuleFromAddressImpl
//
EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* pCodeAddress, char* pModuleName, size_t moduleNameCapacity)
{
	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pCodeAddress), &dlInfo);

	if (ret != 0 && dlInfo.dli_fname)
	{
		return EA::StdC::Strlcpy(pModuleName, dlInfo.dli_fname, moduleNameCapacity);
	}

	pModuleName[0] = '\0';
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleFromAddressImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress)
{
	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pAddress), &dlInfo);

	if (ret != 0 && dlInfo.dli_fname)
	{
		return reinterpret_cast<ModuleHandle>(dlInfo.dli_fbase);
	}

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
	char pModuleFileName[EA::IO::kMaxPathLength];

	Utils::GetFileNameWithoutExt(pModuleFileName, EA::IO::kMaxPathLength, pModuleName);

	uint64_t baseAddress = kBaseAddressInvalid;

	auto matchImage = [&](uint32_t imageIndex, const MachHeader* machHeader)
					  {
						  const char* imageName = _dyld_get_image_name(imageIndex);

						  if (EA::StdC::Stristr(imageName, pModuleFileName))
						  {
							  baseAddress = reinterpret_cast<uint64_t>(machHeader);

							  return false;
						  }

						  return true;
					  };

	Detail::IterateMachOImages(matchImage);

	return baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandleImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle moduleHandle)
{
	return reinterpret_cast<uint64_t>(moduleHandle);
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void* pCodeAddress)
{
	Dl_info dlInfo;
	int ret = dladdr(const_cast<void*>(pCodeAddress), &dlInfo);

	if (ret != 0 && dlInfo.dli_fname)
	{
		return reinterpret_cast<uint64_t>(dlInfo.dli_fbase);
	}

	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleListImpl
///
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity)
{
	size_t moduleCount = 0;

	auto addImage = [&](uint32_t imageIndex, const MachHeader* machHeader)
					{
						if (pModuleHandleArray)
						{
							if (moduleCount < nModuleArrayCapacity)
							{
								pModuleHandleArray[moduleCount] = reinterpret_cast<ModuleHandle>(const_cast<MachHeader*>(machHeader));

								++moduleCount;
							}
							else
							{
								return false;
							}
						}
						else
						{
							++moduleCount;
						}

						return true;
					};

	Detail::IterateMachOImages(addImage);

	return moduleCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
	size_t moduleCount = 0;

	auto imageHandler = [&](uint32_t imageIndex, const MachHeader* machHeader)
						{
							if (pModuleInfoArray)
							{
								if (moduleCount < moduleArrayCapacity)
								{
									const char* imageName = _dyld_get_image_name(imageIndex);

									Detail::FillOutModuleInfo(pModuleInfoArray[moduleCount], machHeader, imageName);

									++moduleCount;
								}
								else
								{
									return false;
								}
							}
							else
							{
								++moduleCount;
							}

							return true;
						};

	Detail::IterateMachOImages(imageHandler);

	return moduleCount;
}


///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandle
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
	bool ret = false;

	auto findImage = [&](uint32_t imageIndex, const MachHeader* machHeader)
					 {
						 const MachHeader* lookingFor = reinterpret_cast<const MachHeader*>(moduleHandle);
						 if (machHeader == lookingFor)
						 {
							 const char* imageName = _dyld_get_image_name(imageIndex);

							 Detail::FillOutModuleInfo(moduleInfo, machHeader, imageName);

							 ret = true;

							 return false;
						 }

						 return true;
					 };

	Detail::IterateMachOImages(findImage);

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleOfExecImpl
///
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{
	ModuleHandle result = kModuleHandleInvalid;

	auto findFirst = [&](uint32_t imageIndex, const MachHeader* machHeader)
					 {
						 if (imageIndex == 0)
						 {
							 /* Our Executable is always the first image added to the dyld link image map */
							 result = static_cast<ModuleHandle>(const_cast<MachHeader*>(machHeader));

							 return false;
						 }

						 return true;
					 };

	Detail::IterateMachOImages(findFirst);

	return result;
}

} // namespace EAModuleImpl

namespace Detail
{

/**
 * Fills out a moduleInfoApple object with the information from all of the segments listed in the mach_header, starting at currentSegmentPos.
 *
 * @param pTypeFilter: is used to filter segment types
 * @param currentSegmentPos: is the starting segment we are iterating over
 * @param pMachHeader: is the mach_header
 */
static void CreateModuleInfoApple(ModuleInfoAppleArray& moduleInfoAppleArray, const char* pTypeFilter, const char* pModulePath,
								  uintptr_t currentSegmentPos, const MachHeader* pMachHeader, intptr_t offset)
{
	/* Segments immediately follow the mach_header */
	for (uint32_t i = 0; i < pMachHeader->ncmds; ++i)
	{
		const SegmentCommand* pSegmentCommand = reinterpret_cast<const SegmentCommand*>(currentSegmentPos);

		/**
		 * Each segment_command has a specific load commannd or lc for this segment.
		 * the LC_SEGMENT, or segment load command indicates that a part of this file is to be mmap'ed into the tasks' address space
		 */
		if (pSegmentCommand && pSegmentCommand->cmd == kLCSegment)
		{
			const size_t segnameBufferLen = sizeof(pSegmentCommand->segname) + 1;
			char segnameBuffer[segnameBufferLen];

			EA::StdC::Memset8(segnameBuffer, 0, segnameBufferLen);
			EA::StdC::Memcpy(segnameBuffer, pSegmentCommand->segname, sizeof(pSegmentCommand->segname));

			if (!pTypeFilter ||
				EA::StdC::Strncmp(segnameBuffer, pTypeFilter, eastl::min_alt(EA::StdC::Strlen(pTypeFilter), sizeof(segnameBuffer))))
			{
				ModuleInfoApple& info = moduleInfoAppleArray.push_back();

				Detail::FillOutModuleInfo(info, pMachHeader, pModulePath);

				Detail::FillVMProt(info, pSegmentCommand);

				EA::StdC::Strlcpy(info.mType, segnameBuffer, EAArrayCount(info.mType));
			}
		}

		currentSegmentPos += pSegmentCommand->cmdsize;
	}
}

} // namespace Detail

///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoApple
//
// This function exists for the purpose of being a central module/VM map info collecting function,
// used by a couple functions within EACallstack.
//
EACALLSTACK_API size_t GetModuleInfoApple(ModuleInfoAppleArray& moduleInfoAppleArray, const char* pTypeFilter, bool bEnableCache)
{
	struct task_dyld_info t_info;
	uint32_t t_info_count = TASK_DYLD_INFO_COUNT;
	kern_return_t kr = task_info(mach_task_self(), TASK_DYLD_INFO, (task_info_t)&t_info, &t_info_count);
	if (kr != KERN_SUCCESS)
	{
		return 0;
	}

	const struct dyld_all_image_infos* pAllImageInfos = ((const struct dyld_all_image_infos *)t_info.all_image_info_addr);

	for (uint32_t i = 0; i < pAllImageInfos->infoArrayCount; ++i)
	{
		const char* pModulePath = pAllImageInfos->infoArray[i].imageFilePath;

		if (pModulePath && EA::StdC::Strncmp(pModulePath, "", EA::IO::kMaxPathLength) != 0)
		{
			uintptr_t currentSegmentPos = reinterpret_cast<uintptr_t>(pAllImageInfos->infoArray[i].imageLoadAddress);

			const MachHeader* pMachHeader = reinterpret_cast<const MachHeader*>(currentSegmentPos);

			currentSegmentPos += sizeof(MachHeader);

			intptr_t offset = _dyld_get_image_vmaddr_slide(i);

			Detail::CreateModuleInfoApple(moduleInfoAppleArray, pTypeFilter, pModulePath, currentSegmentPos, pMachHeader, offset);
		}
	}

	// Iterating on dyld_all_image_infos->infoArray[] does not give us entries for /usr/lib/dyld.
	// We use the mach_header to get /usr/lib/dyld
	const MachHeader* pMachHeader = reinterpret_cast<const MachHeader*>(pAllImageInfos->dyldImageLoadAddress);
	uintptr_t currentSegmentPos = reinterpret_cast<uintptr_t>(pMachHeader) + sizeof(MachHeader);
	char modulePath[EA::IO::kMaxPathLength] = {};
	pid_t pid = getpid();
	int filenameLen = proc_regionfilename((int)pid, currentSegmentPos, modulePath, (uint32_t)sizeof(modulePath));

	EA_ASSERT(filenameLen > 0 && modulePath != NULL && EA::StdC::Strncmp(modulePath,"",sizeof(modulePath)) != 0);

	if (filenameLen > 0)
	{
		Detail::CreateModuleInfoApple(moduleInfoAppleArray, pTypeFilter, modulePath, currentSegmentPos, pMachHeader, 0);
	}

	return static_cast<size_t>(moduleInfoAppleArray.size());
}

} // namespace Callstack
} // namespace EA
