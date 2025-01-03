/////////////////////////////////////////////////////////////////////////////
// EAStompAllocator.cpp
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#include "EAStompAllocator.h"
#include <PPMalloc/internal/shared.h>
#include <stdlib.h>
#include <stdio.h>




namespace EA
{
	namespace Allocator
	{
		namespace StompAllocatorLocal
		{
				const uint32_t kPageSize     = 0x00001000;   // 4K
		}


		StompAllocator::StompAllocator()
		  : mAllocCount(0)
		{
		}


		StompAllocator::~StompAllocator()
		{
			Shutdown();
		}


		void StompAllocator::Init()
		{
		}


		void StompAllocator::Shutdown()
		{
			// Nothing to do.
		}


		void* StompAllocator::Malloc(size_t nSize, int nAllocationFlags)
		{
			return MallocDebug(nSize, nAllocationFlags, NULL, NULL, 0);
		}


		void* StompAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
		{
			return MallocAlignedDebug(nSize, nAlignment, nAlignmentOffset, nAllocationFlags, NULL, NULL, 0);
		}


		void* StompAllocator::MallocDebug(size_t nSize, int nAllocationFlags, const char* pName, const char* pFile, int nLine)
		{
			return MallocAlignedDebug(nSize, kMinAlignment, 0, nAllocationFlags, NULL, NULL, 0);
		}


		void* StompAllocator::MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t /*nAlignmentOffset*/, int nAllocationFlags, 
												 const char* pName, const char* pFile, int nLine)
		{
			// To do: Implement support for alignment offset.

			if(nAlignment < kMinAlignment) 
				nAlignment = kMinAlignment;

			// Round aligned nSize up to alignment. This works for power of 2 alignments up to 4K.
			size_t aligned_size = (size_t)((nSize + (nAlignment - 1)) & -(intptr_t)(uintptr_t)nAlignment);
			size_t size_alloc   = aligned_size + sizeof(MemInfo);

			#if defined(EA_PLATFORM_WINDOWS)
				using namespace StompAllocatorLocal;

				#if defined(EA_PLATFORM_WINDOWS)
					// Round the maximum we need up to a multiple of 4K.
					size_alloc = (size_t)((size_alloc + (kPageSize - 1)) & -(intptr_t)(uintptr_t)kPageSize);

					// Reserve the memory + guard in the virtual address space.
					// Add 4K for the guard page.
					// nAllocationFlags
						void* p = VirtualAlloc(NULL, size_alloc + kPageSize, MEM_RESERVE, PAGE_READWRITE);

					if(p)
					{
						// Mark all but the last page as READWRITE.
							p = VirtualAlloc(p, size_alloc, MEM_COMMIT, PAGE_READWRITE);

						if(p)
						{
							// Squish the allocation up against the end of the page.
							void* const pReturnValue = (char*)p + size_alloc - aligned_size;

							// Record what we need for the rest of the functions.
							MemInfo* const pMemInfo = (MemInfo*)pReturnValue - 1;

							memset(pMemInfo, 0, sizeof(MemInfo));
							pMemInfo->mAvailable       = aligned_size;
							pMemInfo->mSize            = nSize;
							pMemInfo->mAllocationFlags = nAllocationFlags;
							pMemInfo->mBase            = (uintptr_t)p;
							pMemInfo->mSentinel        = kSentinelValue;

							// Set up the stack info.
							#if PPM_DEBUG_CALLSTACK_AVAILABLE
								const size_t kStackCapacity = sizeof(pMemInfo->mStack) / sizeof(pMemInfo->mStack[0]);
								GetCallStack(pMemInfo->mStack, kStackCapacity);
							#endif

							// Set up the description info
							if(!pName)
								pName = "No Name";
							const size_t kDescriptionCapacity = sizeof(pMemInfo->mDescription) / sizeof(pMemInfo->mDescription[0]);
							strncpy(pMemInfo->mDescription, pName, kDescriptionCapacity);
							pMemInfo->mDescription[kDescriptionCapacity - 1] = NULL;

							// Set up the MemInfo pointer. To consider: Do we actually need to do this?
							*(uintptr_t**)p = (uintptr_t*)pMemInfo;

							mAllocCount++;
							return pReturnValue;
						}
					}

					return NULL;

				#endif
			#else
				return malloc(nSize);
			#endif
		}


		void StompAllocator::Free(void* p)
		{
			if(p)
			{
				#if defined(EA_PLATFORM_WINDOWS)
					MemInfo* const pMemInfo = (MemInfo*)p - 1;

					if(pMemInfo->mSentinel != kSentinelValue)
						PPM_DebugBreak();

					#if   defined(EA_PLATFORM_WINDOWS)
						VirtualFree((void*)pMemInfo->mBase, 0, MEM_RELEASE);

					#endif
				#else
					free(p);
				#endif
			}
		}


		size_t StompAllocator::GetSize(const void* p) const
		{
			const MemInfo* const pMemInfo = (MemInfo*)p - 1;

			return pMemInfo->mSize;
		}


		size_t StompAllocator::GetUsableSize(const void* p) const
		{
			const MemInfo* const pMemInfo = (MemInfo*)p - 1;

			return pMemInfo->mAvailable;
		}


	} // namespace Allocator

} // namespace EA





