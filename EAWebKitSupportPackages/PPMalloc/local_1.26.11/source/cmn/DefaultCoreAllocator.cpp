///////////////////////////////////////////////////////////////////////////////////////////////
// DefaultCoreAllocator.cpp
//
// Copyright (c) 2014, Electronic Arts. All Rights Reserved.
//
// This module implements default core memory operations.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include <PPMalloc/internal/DefaultCoreAllocator.h>
#include <PPMalloc/internal/config.h>

#include <stddef.h>    // For size_t, NULL, etc.
#include <stdio.h>     // For snprintf, etc.
#include <stdlib.h>    // For malloc, etc.
#include <string.h>    // For memcpy, memset.
#include <new>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: When CoreMemoryAllocator is moved to a separate package these need to be moved to PPMalloc (config.h?)
//
	#if defined(CMA_MMAP_SUPPORTED)
		#undef CMA_MMAP_SUPPORTED
	#endif
	#define CMA_MMAP_SUPPORTED PPM_MMAP_SUPPORTED

	#if defined(CMA_MALLOC_AS_COREALLOC)
		#undef CMA_MALLOC_AS_COREALLOC
	#endif
	#define CMA_MALLOC_AS_COREALLOC PPM_MALLOC_AS_COREALLOC

	#if defined(CMA_DEBUG)
		#undef CMA_DEBUG
	#endif
	#define CMA_DEBUG PPM_DEBUG

	#if defined(CMA_ASSERT_ENABLED)
		#undef CMA_ASSERT_ENABLED
	#endif
	#define CMA_ASSERT_ENABLED PPM_ASSERT_ENABLED
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if   defined(EA_PLATFORM_MICROSOFT)
	#pragma warning(push, 0)
	#include <Windows.h> // For a little bit of debug code below.
	#pragma warning(pop)
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
		#include <xmem.h>
	#endif
#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__)
	#include <unistd.h>         // sbrk(), getpagesize(), mmap, munmap, etc.
	#include <fcntl.h>          // open

	#if CMA_MMAP_SUPPORTED
		#if defined(_YVALS) // If using the Dinkumware Standard library...
			#include <sys/types.h>  // mmap, etc.
		#else
			#include <sys/mman.h>   // mmap, etc.
		#endif
	#endif
#elif defined(EA_PLATFORM_SONY)
	#include <kernel.h>
	#include <sceerror.h>
#endif

#if   defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6326) // Potential comparison of a constant with another constant
	#pragma warning(disable: 6011) // Dereferencing NULL pointer
	#pragma warning(disable: 6250) // Calling 'VirtualFree' without the MEM_RELEASE flag might free memory but not address descriptors (VADs).
#endif

#if defined(__GNUC__) || defined(__clang__)
	#define CMA_UNUSED_ATTRIBUTE   __attribute__((unused))
#else
	#define CMA_UNUSED_ATTRIBUTE
#endif



///////////////////////////////////////////////////////////////////////////////
// CMA_MMAP_SUPPORTED (defined as 0 or 1; default is 0)
//
// If true, GeneralAllocator::Malloc makes use mapped memory to allocate very large blocks. 
// This also enables the use of mapped memory if normal core memory allocation 
// mechanisms fail.
//
#ifndef CMA_MMAP_SUPPORTED
	#if defined(EA_PLATFORM_WINDOWS_PHONE) || (defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) // If Windows Metro / WinRT...
		#define CMA_MMAP_SUPPORTED 0    // VirtualAlloc is not supported.
	#elif defined(EA_PLATFORM_MICROSOFT) 
		#define CMA_MMAP_SUPPORTED 1    // VirtualAlloc is supported.
	#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__) || defined(EA_PLATFORM_SONY)
		#define CMA_MMAP_SUPPORTED 1    // mmap is supported.
	#else
		// Note that the XBox and XBox 2 platforms have virtual memory (not to be
		// confused with paged memory) via VirtualAlloc. However, since XBox 
		// doesn't have paged memory, VirtualAlloc gets its memory from free
		// physical RAM. But most usually all free physical RAM is given to the 
		// GeneralAllocator or some other pool. The only way VirtualAlloc would
		// be useful is if the user reserved some space for VirtualAlloc calls.
		// So what we do is disable mapped memory by default in XBox platforms
		// but allow the user to override it if desired.
		#define CMA_MMAP_SUPPORTED 0
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// CMA_MALLOC_AS_COREALLOC (defined as 0 or 1; default is 1 unless the platform supports advanced memory management)
//
// When defined to 1, this instructs the system to get core from 
// malloc and not directly from the system. You must be careful with 
// this in that if you are using this allocator to directly replace
// malloc, you probably don't want to set CMA_MALLOC_AS_COREALLOC
// to be 1, as it would cause an infinite loop at runtime if CoreAlloc
// was called.
#ifndef CMA_MALLOC_AS_COREALLOC
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) || (defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_WINDOWS) && !defined(EA_PLATFORM_WINDOWS_PHONE))
		#define CMA_MALLOC_AS_COREALLOC 0 // VirtualAlloc is supported.
	#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__) || defined(EA_PLATFORM_SONY)
		#define CMA_MALLOC_AS_COREALLOC 0 // mmap or another alternative to malloc is supported.
	#else // Console platforms.
		#define CMA_MALLOC_AS_COREALLOC 1
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// CMA_MALLOC_AS_MMAPALLOC (defined as 0 or 1; defaults to 0)
//
// When defined to 1, this causes the memory mapped allocations to instead
// use plain old malloc to retrieve the memory. You probably don't want
// to change this value unless you are doing some kind of testing during
// library maintenance.
//
#ifndef CMA_MALLOC_AS_MMAPALLOC
	#define CMA_MALLOC_AS_MMAPALLOC 0
#endif

///////////////////////////////////////////////////////////////////////////////
// CMA_WIN_MEM_WATCH
//
// This is currently specific to GeneralAllocator.
// Defined as an integral flag value. The value is 0 if disabled/unused.
// This is a wrapper for the Win32/Win64 MEM_WRITE_WATCH flag used by VirtualAlloc.
//
#if !defined(CMA_WIN_MEM_WATCH)
	#if defined(EA_PLATFORM_WINDOWS) && (CMA_DEBUG >= 1)
		#if defined(MEM_WRITE_WATCH)
			#define CMA_WIN_MEM_WATCH MEM_WRITE_WATCH // MEM_WRITE_WATCH is defined in Microsoft SDK headers.
		#else
			#define CMA_WIN_MEM_WATCH 0x00200000
		#endif
	#else
		#define CMA_WIN_MEM_WATCH 0
	#endif
#endif

#ifndef CMA_ASSERT_ENABLED
	#define CMA_ASSERT_ENABLED CMA_DEBUG
#endif



#ifndef CMA_ASSERT
	#if CMA_ASSERT_ENABLED
		#define CMA_ASSERT(pAssertFuncObj, expression, id, pDescription, pData, pExtra) \
					EA_DISABLE_VC_WARNING(4127) \
					do { \
						EA_ANALYSIS_ASSUME(expression); \
						(void)((expression) || (pAssertFuncObj ? pAssertFuncObj->AssertionFailure(#expression, id, pDescription, pData, pExtra) : assert(!#expression), 0)); \
								} while (0) \
					EA_RESTORE_VC_WARNING()
		#define CMA_ASSERT_STATIC(expression, id, pDescription, pData, pExtra) \
					EA_DISABLE_VC_WARNING(4127) \
					do { \
						EA_ANALYSIS_ASSUME(expression); \
						(void)((expression) || (assert(!#expression), 0)); \
								} while (0) \
					EA_RESTORE_VC_WARNING()
	#else
		#define CMA_ASSERT(pAllocator, expression, id, pDescription, pData, pExtra) 
		#define CMA_ASSERT_STATIC(expression, id, pDescription, pData, pExtra)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// CMA_16MB_PAGE_SIZE
//
// Used when 16MB pages are specifically requested.
//
#define CMA_16MB_PAGE_SIZE (16 * 1024 * 1024)
#define CMA_4MB_PAGE_SIZE  (4 * 1024 * 1024)
#define CMA_64KB_PAGE_SIZE (64 * 1024)

namespace EA
{
	namespace Allocator
	{
		inline static char* CMA_AlignUp(const void* p, size_t nAlignment) CMA_UNUSED_ATTRIBUTE;

		inline static char* CMA_AlignUp(const void* p, size_t nAlignment)
		{
			return (char*)(((uintptr_t)p + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		inline static size_t CMA_AlignUp(size_t nValue, size_t nAlignment) CMA_UNUSED_ATTRIBUTE;
		inline static size_t CMA_AlignUp(size_t nValue, size_t nAlignment)
		{
			return (size_t)((nValue + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		inline char* CMA_AlignUp(const char* p, size_t nAlignment) CMA_UNUSED_ATTRIBUTE;
		inline char* CMA_AlignUp(const char* p, size_t nAlignment)
		{
			return (char*)(((uintptr_t)p + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		inline static bool CMA_IsAligned(const void* p, size_t nAlignment) CMA_UNUSED_ATTRIBUTE;
		inline static bool CMA_IsAligned(const void* p, size_t nAlignment)
		{
			return ((uintptr_t)p & (nAlignment - 1)) == 0;
		}

		inline static bool CMA_IsAligned(size_t nValue, size_t nAlignment) CMA_UNUSED_ATTRIBUTE;
		inline static bool CMA_IsAligned(size_t nValue, size_t nAlignment)
		{
			return (nValue & (nAlignment - 1)) == 0;
		}

		void* CoreMemoryAllocator::AddCoreMemory(size_t nMinSize, bool bExecutableCore, bool bCoreTopDown, bool bCommitAll, size_t nCoreIncrementSize, 
			size_t& nCoreSize, size_t& nReservedSize, uint32_t& coreType, bool& bPageExecute, IAllocAssertFunc* pAssertFuncObj)

		{
			void* pCore = NULL;
			
			nCoreSize = nMinSize;

			EA_UNUSED(bCoreTopDown);
			EA_UNUSED(bCommitAll);
			EA_UNUSED(nCoreIncrementSize);
			EA_UNUSED(nReservedSize);
			EA_UNUSED(coreType);
			EA_UNUSED(bPageExecute);
			EA_UNUSED(bExecutableCore);	
			EA_UNUSED(pAssertFuncObj);

			// Note that XBox and XBox 360 can also use the pathway below. For the time being we
			// don't use this pathway on XBox / XBox 360 as it wouldn't gain all that much there.
			#if defined(EA_PLATFORM_MICROSOFT) && CMA_MMAP_SUPPORTED && !CMA_MALLOC_AS_COREALLOC
				// Under Windows, we create a new core block of reserved but not
				// committed memory. Then we commit some of this memory and return
				// a chunk from it.
				nReservedSize = CMA_AlignUp(nCoreSize, 65536); // 65536 is Windows VirtualAlloc granularity.
				DWORD dwFlags = MEM_RESERVE | CMA_WIN_MEM_WATCH;

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
					if ((nReservedSize % CMA_4MB_PAGE_SIZE) == 0)
						dwFlags |= MEM_2MB_PAGES;
					else if ((nReservedSize % CMA_64KB_PAGE_SIZE) == 0)
						dwFlags |= MEM_64K_PAGES;
				#elif defined(EA_PLATFORM_XBOXONE)
					if ((nReservedSize % CMA_4MB_PAGE_SIZE) == 0)
						dwFlags |= MEM_4MB_PAGES;
					else if ((nReservedSize % CMA_64KB_PAGE_SIZE) == 0)
						dwFlags |= MEM_LARGE_PAGES;
				#endif

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // If executable memory is recognized and allowed...
					DWORD  dwPermissions = (DWORD)(bExecutableCore ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
				#else
					DWORD  dwPermissions = PAGE_READWRITE;
				#endif

				if (bCoreTopDown)
					dwFlags |= MEM_TOP_DOWN;

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
					pCore = XMemVirtualAlloc(NULL, nReservedSize, dwFlags, XMEM_CPU, dwPermissions);
				#else
					pCore = VirtualAlloc(NULL, nReservedSize, dwFlags, dwPermissions);
				#endif

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // If executable memory is recognized and allowed...
					if (bExecutableCore && (pCore != NULL))
						bPageExecute = true;
				#endif

				CMA_ASSERT(pAssertFuncObj, pCore, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, the system is probably exhausted of memory or address space.
				if (pCore)
				{
					coreType = kCoreTypeMMap;

					size_t nCommitSize = CMA_AlignUp(nMinSize, 65536);
					if (nCommitSize < nCoreIncrementSize)
						nCommitSize = CMA_AlignUp(nCoreIncrementSize, 65536);
					if ((nCommitSize > nReservedSize) || bCommitAll)
						nCommitSize = nReservedSize;

					#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // If executable memory is recognized and allowed...
						dwPermissions = (DWORD)(bPageExecute ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
					#else
						dwPermissions = PAGE_READWRITE;
					#endif

					#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
						void* pReserve = XMemVirtualAlloc(pCore, nCommitSize, MEM_COMMIT, XMEM_CPU, dwPermissions);
					#else
						void* pReserve = VirtualAlloc(pCore, nCommitSize, MEM_COMMIT, dwPermissions);
					#endif

					if (pReserve)
					{
						CMA_ASSERT(pAssertFuncObj, pReserve == pCore, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, NULL); // If this fails, the system is probably exhausted of memory or address space.

						nCoreSize = nCommitSize; // For processing below, we reset core size down to min size.
						CMA_ASSERT(pAssertFuncObj, (nCoreSize % 65536) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCore); // If this fails, it is PPMalloc's fault.
					}
					else
					{
						VirtualFree(pCore, 0, MEM_RELEASE);
						pCore = NULL;
					}
				}

			#elif CMA_MALLOC_AS_COREALLOC
				// Under XBox we might want to use its physical memory allocation functionality.
				// What we do here is try to allocate nCoreSize but if that fails then gradually
				// reduce the value down to nMinSize while attempting to allocate.
				for(;;)
				{
						// Note that on platforms such as PS3 this malloc is implemented as a mapped memory allocation underneath.
						pCore    = malloc(nCoreSize);
						coreType = kCoreTypeMalloc;

					if(pCore)
						break;
					else
						coreType = kCoreTypeNone;  // Set to kCoreTypeNone becuase it is invalid and not of any core type.

					nCoreSize = nCoreSize * 3 / 4;
					if(nCoreSize < nMinSize)
						break;
				}
			#endif

			return pCore;
		}

		void* CoreMemoryAllocator::ExtendCoreMemory(void* pCore, size_t nSize, size_t nMinExtendedSize, size_t nReservedSize, size_t nCoreIncrementSize, bool bPageExecutable, bool& bIsExtensionSupported, size_t& nExtendedSize, IAllocAssertFunc* pAssertFuncObj)
		{
			void* pExtendedCore = NULL;

			EA_UNUSED(pCore);
			EA_UNUSED(nSize);
			EA_UNUSED(nMinExtendedSize);
			EA_UNUSED(nReservedSize);
			EA_UNUSED(nCoreIncrementSize);
			EA_UNUSED(bPageExecutable);
			EA_UNUSED(pAssertFuncObj);

			#if defined(EA_PLATFORM_MICROSOFT) && CMA_MMAP_SUPPORTED
				// In the case that we will be calling VirtualAlloc, we need to make sure that 
				// the VirualAlloc sizes are on 65536 boundaries. But if we won't be calling 
				// VirtualAlloc, in particular when the core is user-supplied, then we don't need 
				// this boundary. So the assertion here is disabled and we call it a few lines
				// below instead.
				CMA_ASSERT(pAssertFuncObj, ((nSize % 65536) == 0) && ((nReservedSize % 65536) == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCore); // 65536 is Windows VirtualAlloc granularity. // If this fails, PPMalloc is probably at fault.

				nExtendedSize = CMA_AlignUp(nMinExtendedSize, 65536); // We need to recalculate nExtendedSize from scratch each time through this loop, as it changes.
				if (nExtendedSize < nCoreIncrementSize)
					nExtendedSize = nCoreIncrementSize;
				CMA_ASSERT(pAssertFuncObj, (nCoreIncrementSize % 65536) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // 65536 is Windows VirtualAlloc granularity. // If this fails, PPMalloc is at fault.

				if (nExtendedSize >(nReservedSize - nSize))
					nExtendedSize = (nReservedSize - nSize);  // Extensions need to occur in blocks of 65536 modularity.
				CMA_ASSERT(pAssertFuncObj, (nExtendedSize % 65536) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // 65536 is Windows VirtualAlloc granularity. // If this fails, PPMalloc is at fault.

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // If executable memory is recognized and allowed...
					DWORD dwPermissions = (DWORD)(bPageExecutable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
				#else
					DWORD dwPermissions = PAGE_READWRITE;
				#endif

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
					pExtendedCore = XMemVirtualAlloc((char*)pCore + nSize, nExtendedSize, MEM_COMMIT, XMEM_CPU, dwPermissions);
				#else
					pExtendedCore = VirtualAlloc((char*)pCore + nSize, nExtendedSize, MEM_COMMIT, dwPermissions);
				#endif

				bIsExtensionSupported = true;
				
			#else
					nExtendedSize = 0;
					bIsExtensionSupported = false;

			#endif
			return pExtendedCore;
		}

		bool CoreMemoryAllocator::FreeCoreMemory(void* pCore, uint32_t coreType, size_t nSize, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, IAllocAssertFunc* pAssertFuncObj)
		{
			bool bReturnValue = false;

			switch (coreType)
			{
			case kCoreTypeDefault:
				// Normally this shouldn't occur. kCoreTypeDefault is a pseudo-type which is used in calls to AddCore to 
				// indicate that we are to use whatever actual type is default. 
				break;

			case kCoreTypeNone:
				// Do nothing. We assume in this case that the core memory is not intended to be freed or freeable.
				break;

			case kCoreTypeMalloc:
				free(pCore);
				bReturnValue = true;
				break;

			case kCoreTypeNew:
				delete[] reinterpret_cast<char*>(pCore);
				bReturnValue = true;
				break;

			case kCoreTypeMMap:
				#if CMA_MMAP_SUPPORTED
					FreeMemoryMap(pCore, nSize, memoryRangeFunc, userData, pAssertFuncObj);
					bReturnValue = true;
				#endif
				break;

			case kCoreTypeKernelAlloc:
				break;

			default:
			case kCoreTypeUserDefined:
				// This case should have been handled with a user defined callback. 
				//TODO - move this assert to PPM
			   // CMA_ASSERT(pAssertFuncObj, pCoreBlock->mpCoreFreeFunction, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, pCoreBlock); // If this fails, it is the PPMalloc's fault.
				break;
			}
			return bReturnValue;
		}

		void CoreMemoryAllocator::FreeMemoryMap(void* pAddress, size_t nSize, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, IAllocAssertFunc* pAssertFuncObj)
		{
			EA_UNUSED(pAssertFuncObj);

		#if CMA_MMAP_SUPPORTED

			if (memoryRangeFunc)
				(*memoryRangeFunc)(userData, pAddress, nSize, false);

			#if defined(EA_PLATFORM_MICROSOFT)
				#if CMA_ASSERT_ENABLED
					const BOOL bResult = // fall through to VirtualFree() below.
				#endif
				VirtualFree(pAddress, 0, MEM_RELEASE);
				CMA_ASSERT(pAssertFuncObj, bResult, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, NULL); // If this fails, it is probably the user's fault.

			#elif (defined(EA_PLATFORM_UNIX) || defined(__APPLE__))
				#if CMA_ASSERT_ENABLED
					const int nResult = // fall through to munmap() below.
				#endif
				munmap(pAddress, nSize);
				CMA_ASSERT(pAssertFuncObj, nResult == 0, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, NULL); // If this fails, it is probably the user's fault.

			#elif defined(EA_PLATFORM_SONY)
				// We need to call sceKernelReleaseDirectMemory, but to call that we need the physical memory location of pAddress.
				// It turns out we can get that via sceKernelVirtualQuery.
				SceKernelVirtualQueryInfo info;

				// The 'info' returned actually stores information about the memory region maintained by the system that contains 'addr'.  That region may 
				// not match up exactly with the original system memory allocation because the OS may merge multiple allocations together.  In general the
				// region represented by 'info' may be larger than the original allocation request.
				int32_t result = sceKernelVirtualQuery(pAddress, 0, &info, sizeof(info));
				CMA_ASSERT(pAssertFuncObj, result == SCE_OK, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, NULL); // If this fails, it's probably the user's fault.

				if(result == SCE_OK)
				{
					// The Sony documentation regarding sceKernelMapDirectMemory is (or at one point was) incorrect and 
					// neglected to state that you need to call sceKernelMunmap to match sceKernelMapDirectMemory.

					result = sceKernelMunmap(pAddress, nSize);
					CMA_ASSERT(pAssertFuncObj, result == SCE_OK, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, NULL); // If this fails, it is probably the user's fault.

					if(result == SCE_OK)
					{
						// Determine the virtual memory offset of the address being freed.  This offset should be the same size as the offset from the physical memory start address.
						off_t offsetFromVirtualAddressBase = reinterpret_cast<intptr_t>(pAddress)-reinterpret_cast<intptr_t>(info.start);
						// The physical address base is the same as info.offset for direct memory allocations
						off_t physicalAddressBase = info.offset;
						// Compute the actual physical address to be released
						off_t physicalAddressStart = physicalAddressBase + offsetFromVirtualAddressBase;
						result = sceKernelReleaseDirectMemory(physicalAddressStart, nSize);
						CMA_ASSERT(pAssertFuncObj, result == SCE_OK, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, NULL); // If this fails, it's probably the user's fault for releasing the memory behind our back.
					}
				}

			#else
				free(pAddress);
			#endif
		#else
			EA_UNUSED(pAddress);
			EA_UNUSED(nSize);
			EA_UNUSED(memoryRangeFunc);
			EA_UNUSED(userData);
		#endif
		}

		void* CoreMemoryAllocator::AllocMemoryMap(size_t nSize, size_t nMMapPageSize, bool bMMapTopDown, bool bExecutableCore, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, size_t* pSizeAllocated, IAllocAssertFunc* pAssertFuncObj)
		{
			void* pReturnValue = NULL;

			EA_UNUSED(nSize);
			EA_UNUSED(nMMapPageSize);
			EA_UNUSED(bMMapTopDown);
			EA_UNUSED(bExecutableCore);
			EA_UNUSED(pAssertFuncObj);

		#if CMA_MMAP_SUPPORTED
			// Note that just because we define MMapAllocInternal for various platforms doesn't 
			// mean the system is enabled to use MMapAllocInternal. See the CMA_MMAP_SUPPORTED
			// define for this.
			#if defined(EA_PLATFORM_MICROSOFT) && CMA_MMAP_SUPPORTED && !CMA_MALLOC_AS_MMAPALLOC
				// We use top-down because this way it will be less likely to collide with our normal core allocation access space.
				// However, watch out because MEM_TOP_DOWN is much slower when there are very many such pages allocated.
				// VirtualAlloc guarantees a 0 fill unless MEM_RESET (Windows) or MEM_NOZERO (Xenon) is specified.
				nSize = CMA_AlignUp(nSize, nMMapPageSize);
				DWORD dwFlags = MEM_RESERVE | MEM_COMMIT | CMA_WIN_MEM_WATCH;

				if(bMMapTopDown)
					dwFlags |= MEM_TOP_DOWN;

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
					if ((nMMapPageSize % CMA_4MB_PAGE_SIZE) == 0)
						dwFlags |= MEM_2MB_PAGES;
					else if ((nMMapPageSize % CMA_64KB_PAGE_SIZE) == 0)
						dwFlags |= MEM_64K_PAGES;
				#elif defined(EA_PLATFORM_XBOXONE)
					if((nMMapPageSize % CMA_4MB_PAGE_SIZE) == 0)
						dwFlags |= MEM_4MB_PAGES;
					else if((nMMapPageSize % CMA_64KB_PAGE_SIZE) == 0)
						dwFlags |= MEM_LARGE_PAGES;
				#endif

				#if defined(EA_PLATFORM_WINDOWS)
					const DWORD dwPermissions = (DWORD)(bExecutableCore ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
				#else // Not allowed to allocate executable memory on non-desktop Microsoft platforms.
					const DWORD dwPermissions = PAGE_READWRITE;
				#endif

				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)
					pReturnValue = XMemVirtualAlloc(NULL, nSize, dwFlags, XMEM_CPU, dwPermissions);
				#else
					pReturnValue = VirtualAlloc(NULL, nSize, dwFlags, dwPermissions);
				#endif

				#if CMA_ASSERT_ENABLED
					MEMORY_BASIC_INFORMATION mbi;
					memset(&mbi, 0, sizeof(mbi));
					VirtualQuery(pReturnValue, &mbi, sizeof(mbi));
					CMA_ASSERT(pAssertFuncObj, mbi.BaseAddress == pReturnValue, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					CMA_ASSERT(pAssertFuncObj, (pReturnValue == NULL) || (mbi.RegionSize >= nSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // We can't test for '== nSize' because there may be additional pages that have the same attributes. // If this fails, PPMalloc is at fault.
				#endif

				if(pReturnValue && pSizeAllocated)
					*pSizeAllocated = nSize; // This is not necessary as it is already aligned: (nSize + nMMapPageSize - 1) & ~(nMMapPageSize - 1); // Round nSize up to page size.

			#elif (defined(EA_PLATFORM_UNIX) || defined(__APPLE__)) && !CMA_MALLOC_AS_MMAPALLOC
				// mmap guarantees a zero fill.
				// Most Unixes support MAP_ANON/MAP_ANONYMOUS, which is the same thing as /dev/zero except  
				// that it is faster because there is no need to open the /dev/zero device.
				#if !defined(MAP_FAILED)
					#define MAP_FAILED ((void*)-1)
				#endif
				#if !defined(MAP_ANONYMOUS)
					#define MAP_ANONYMOUS MAP_ANON
				#endif

				const int kProtectionFlags = (PROT_READ | PROT_WRITE | (bExecutableCore ? PROT_EXEC : 0));
				const int kMapFlags        = (MAP_PRIVATE | MAP_ANON);

				nSize = CMA_AlignUp(nSize, nMMapPageSize);
				pReturnValue = mmap(NULL, nSize, kProtectionFlags, kMapFlags, -1, 0);

				if(pReturnValue == MAP_FAILED) // mmap returns MAP_FAILED (-1) upon error, so we convert it to zero, 
					pReturnValue = 0;          // which is likely to be a safe thing for us to do.
				else if(pSizeAllocated)
					*pSizeAllocated = nSize; 

			#elif defined(EA_PLATFORM_SONY) && !CMA_MALLOC_AS_MMAPALLOC
				// We cannot use sceKernelMmap, despite its similarity to unix mmap, 
				// as sceKernelMmap doesn't support using -1 as a file id to mean no file mapping.
				// Instead we need to use sceKernelAllocateDirectMemory, sceKernelMapDirectMemory,
				// sceKernelMapNamedDirectMemory, sceKernelReleaseDirectMemory.

				CMA_ASSERT(pAssertFuncObj, CMA_IsAligned(nMMapPageSize, 16384) && (nMMapPageSize >= 16384), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL); // If this fails then it's probably the user's fault for calling SetOption with an invalid value.
				if(!CMA_IsAligned(nMMapPageSize, 16384))
				   nMMapPageSize = CMA_AlignUp(nMMapPageSize, 16384);

				nSize = CMA_AlignUp(nSize, nMMapPageSize);

				// We need to first allocate direct (physical) memory with sceKernelAllocateDirectMemory, then map it to our address space with sceKernelMapDirectMemory.
				off_t   pDirectMemory;
				int32_t result = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, nSize, nMMapPageSize,
#if defined(EA_PLATFORM_PS4)
					SCE_KERNEL_WB_ONION,
#else
					SCE_KERNEL_MTYPE_C_SHARED,
#endif
					&pDirectMemory);

				if(result == SCE_OK)
				{
					const int kProtectionFlags = (SCE_KERNEL_PROT_CPU_READ | SCE_KERNEL_PROT_CPU_WRITE); // Also possible are SCE_KERNEL_PROT_GPU_READ and SCE_KERNEL_PROT_GPU_WRITE
					pReturnValue = NULL;  // We have the option of specifying a non-null value here to tell sceKernelMapDirectMemory to use at least that address for the memory it returns. 
					result = sceKernelMapNamedDirectMemory(&pReturnValue, nSize, kProtectionFlags, 0, pDirectMemory, nMMapPageSize, "PPMalloc::GeneralAllocator");

					if(result == SCE_OK)
					{
						// This shouldn't be necessary:
						// memset(pReturnValue, 0, nSize);
					}
					else
						sceKernelReleaseDirectMemory(pDirectMemory, nSize);
				}

				if(result != SCE_OK)
					pReturnValue = NULL;
				else
				{
					CMA_ASSERT(pAssertFuncObj, CMA_IsAligned(pReturnValue, nMMapPageSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					*pSizeAllocated = nSize; 
				}
			#else
				// malloc does not fill the memory with anything consistently. 
				// calloc does fill with zeroes, so it could be used instead of malloc+memset.
				pReturnValue = malloc(nSize);

				if(pReturnValue)    
				{ 
					// This shouldn't be necessary:
					// memset(pReturnValue, 0, nSize);

					if(pSizeAllocated)
						*pSizeAllocated = nSize;
				}

			#endif

				if (pReturnValue && memoryRangeFunc)
					(*memoryRangeFunc)(userData, pReturnValue, nSize, true);
	
		#endif
			return pReturnValue;
		}

		CoreMemoryAllocator::CoreMemoryState CoreMemoryAllocator::GetMemoryState(void* pAddress)
		{
			EA_UNUSED(pAddress);

			CoreMemoryState state = kCoreMemTypeNone;
			#if defined(EA_PLATFORM_WINDOWS)
				MEMORY_BASIC_INFORMATION mbi;
				mbi.BaseAddress = pAddress;
				VirtualQuery(mbi.BaseAddress, &mbi, sizeof(mbi));

				if (mbi.State == MEM_COMMIT)
				{
					state = kCoreMemTypeCommitted;
				}
				else if (mbi.State == MEM_RESERVE)
				{
					state = kCoreMemTypeReserved;
				}
			#endif
			return state;
		}

		bool CoreMemoryAllocator::ReleaseCoreMemory(void* pAddress, size_t nSize)
		{
			EA_UNUSED(pAddress);
			EA_UNUSED(nSize);
			#if defined(EA_PLATFORM_WINDOWS)
			if(VirtualFree(pAddress, nSize, MEM_DECOMMIT))
			{
				return true;
			}
			#endif
			return false;
		}


	} // namespace Allocator

} // namespace EA


#if   defined(_MSC_VER)
	#pragma warning(pop)
#endif











