/////////////////////////////////////////////////////////////////////////////
// EAStompAllocator.cpp
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASTOMPALLOCATOR_H
#define PPMALLOC_EASTOMPALLOCATOR_H


#include <EABase/eabase.h>
#include <stdlib.h>


namespace EA
{
	namespace Allocator
	{
		/// StompAllocator
		///
		/// This is a debug-only allocator whose purpose is to find memory overruns in user
		/// allocations by using the page-fault mechanism of the system to detect errors.
		///
		/// StompAllocator works by allocating memory up against the end of a virtual page
		/// where then next page is not committed to RAM. By doing this, reads or writes 
		/// off the end of an allocation cause an exception. If the memory is deleted it 
		/// in unmapped from virtual memory, and again an exception occurs on reads or writes.
		/// Writes before the being of the block can be detected using a sentinel.
		///
		/// This class is not thread-safe on the PS3 platform, but is thread-safe on all 
		/// other supported platforms. Thus if you want to access this allocator from multiple
		/// threads on the PS3 platform you will need to manually coordinate thread safety.
		///
		/// This allocator is based on the work of Kevin Perry.
		///
		class StompAllocator
		{
		public:
			enum Constants
			{
				kMinAlignment = 16
			};

			enum AllocationFlags
			{
				kAllocationFlagNone     = 0x00000000,    /// Shorthand meaning 'no flags'.
				kAllocationFlagPhysical = 0x00000100     /// Physical memory (on systems that support it). Ignored by systems that don't support it.
			};

		public:
			StompAllocator();
		   ~StompAllocator();

			void Init();
			void Shutdown();

			void* Malloc(size_t size, int nAllocationFlags = 0);

			void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0);

			void* MallocDebug(size_t nSize, int nAllocationFlags = 0, 
							  const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			void* MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags = 0, 
									 const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			void Free(void* p);

			// Amount of memory requested by the user.
			size_t GetSize(const void* p) const;

			// Amount of memory available. Will be >= the value requested by the user.
			size_t GetUsableSize(const void* p) const;

		protected:
			static const uint32_t kSentinelValue = 0xbaadbaad;

			struct MemInfo
			{
				size_t      mAvailable;         // Actual size available to the user
				size_t      mSize;              // Amount actually allocated
				int         mAllocationFlags;   // 
				uintptr_t   mBase;              // 
				char        mDescription[64];   // Something to ID this alloc.
				void*       mStack[10];         // Stack that allocated this block
				unsigned    mSentinel;          // Used to detect underrun writes
			};

		protected:
			uint32_t mAllocCount;

		}; // class StompAllocator

	} // namespace Allocator

} // namespace EA


#endif // Header include guard




