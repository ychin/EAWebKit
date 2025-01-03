///////////////////////////////////////////////////////////////////////////////////////////////
// DefaultCoreAllocator.cpp
//
// Copyright (c) 2014, Electronic Arts. All Rights Reserved.
//
// This module implements default core memory operations.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COREMEMORYALLOCATOR_H
#define COREMEMORYALLOCATOR_H


#include <PPMalloc/internal/shared.h>

namespace EA
{
	namespace Allocator
	{
		class CoreMemoryAllocator
		{
		public:

			/// CoreType
			/// Identifies the source of core memory, for the purpose of knowing how to free it.
			/// Core memory is the underlying large area of memory used internally by GeneralAllocator to 
			/// carve individual user allocations from.
			enum CoreType
			{
				kCoreTypeNone,              /// The CoreType is unapplicable. This is not a valid argument to any public function which takes a CoreType parameter.
				kCoreTypeDefault,           /// Default core type; used as an argument to AddCore, Init, and the ctor.
				kCoreTypeMalloc,            /// Created from malloc, destroyed with free.
				kCoreTypeNew,               /// Createed from new char[], destroyed with delete[] char.
				kCoreTypeMMap,              /// Created from mmap (Unix), VirtualAlloc(Microsoft), PS3 sys_memory_allocate, and similar for other platforms.
				kCoreTypeKernelAlloc,       /// XBox XPhysicalAlloc, PSVita sceKernelAllocMemBlock.
				kCoreTypeUserDefined = 32   /// Only the user can know how to free this or any numerical type not enumerated here. The user can use any value >= kCoreTypeUserDefined.
			};

			enum CoreMemoryState
			{
				kCoreMemTypeNone,           /// Memory type is uknown.
				kCoreMemTypeReserved,       /// Memory was is reserved.
				kCoreMemTypeCommitted        /// Memory was is commited.
			};


			/// SetMemoryRangeCallbackFunction
			/// Callback intended to be used to update virtual memory map of heaps.
			/// The callback will be invoked whenever virtual memory is added or removed.
			typedef void(*MemoryRangeCallbackFunction)(void* userData, void* address, size_t size, bool allocated);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Allocate Core System memory.
			// nMinSize - requested size
			// bExecutableCore - request executable memory
			// bCoreTopDown - alloc top down
			// nCoreIncrementSize - min memory increment
			// nComittedSize - return committed size
			// nReservedSize - return reserved size
			// coreType - type of the core memory - one of the CoreTypes
			// pAssertFuncObj - assert function context. Implements IAllocAssertFunc function

			static void* AddCoreMemory(size_t nMinSize, bool bExecutableCore, bool bCoreTopDown, bool bCommitAll, size_t nCoreIncrementSize,
				size_t& nComittedSize, size_t& nReservedSize, uint32_t& coreType, bool& bPageExecute, IAllocAssertFunc* pAssertFuncObj);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Extend Core memory if posible.
			// pCore - existing memory address
			// nMinSize - requested size
			// nMinExtendedSize - requested size to extend
			// nReservedSize - currently reserved size
			// nCoreIncrementSize - min memory increment
			// bExecutableCore - request executable memory
			// bIsExtensionSupported - returns true if extension of core memory is supported on a given OS
			// nExtendedSize - returns extended memory size
			// pAssertFuncObj - assert function context. Implements IAllocAssertFunc function

			static void* ExtendCoreMemory(void* pCore, size_t nSize, size_t nMinExtendedSize, size_t nReservedSize, size_t nCoreIncrementSize, 
											bool bPageExecutable, bool& bIsExtensionSupported, size_t& nExtendedSize, IAllocAssertFunc* pAssertFuncObj);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Free Core memory
			// pCore - existing memory address
			// coreType - type of the core memory, one of the CoreTypes
			// nSize - size of the core memory block
			// memoryRangeFunc - see MemoryRangeCallbackFunction type.
			// userData - context passed to the memoryRangeFunc
			// pAssertFuncObj - assert function context. Implements IAllocAssertFunc function

			static bool FreeCoreMemory(void* pCore, uint32_t coreType, size_t nSize, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, IAllocAssertFunc* pAssertFuncObj);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Allocate memory map

			static void FreeMemoryMap(void* pAddress, size_t nSize, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, IAllocAssertFunc* pAssertFuncObj);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Free memory map

			static void* AllocMemoryMap(size_t nSize, size_t nMMapPageSize, bool bMMapTopDown, bool bExecutableCore, MemoryRangeCallbackFunction memoryRangeFunc, void* userData, size_t* pSizeAllocated, IAllocAssertFunc* pAssertFuncObj);

			static CoreMemoryState GetMemoryState(void* pAddress);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Decommit some of allocated memory. Used in Core trim functionality

			static bool ReleaseCoreMemory(void* pAddress, size_t nSize);
		};

	} // namespace Allocator

} // namespace EA


#endif // COREMEMORYALLOCATOR_H













