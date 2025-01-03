///////////////////////////////////////////////////////////////////////////////
// EAFixedAllocator.h
// 
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This module implements an allocator specialized for objects of fixed size.
//
// The benefit is that there is no overhead for object storage and that many
// similar objects can be allocated near each other for good cache behaviour.
// Also, allocation is very fast. The downside to this kind of allocator is 
// that any unallocated space is not available for use; this is what we call 
// "external fragmentation."
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EAFIXEDALLOCATOR_H
#define PPMALLOC_EAFIXEDALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class FixedAllocatorBase
		///
		/// This is a generic fixed-size allocator implementation. It allocates blocks
		/// of a single user-defined size and alignment. You can use this class or you
		/// can use the FixedAllocator template class defined alongside this class.
		///
		class PPM_API FixedAllocatorBase
		{
		public:
			static const size_t kDefaultCountPerCoreBlock = 128;

			typedef void* (*CoreAllocationFunction)(size_t nSize, void* pContext);
			typedef void  (*CoreFreeFunction)(void* pCore, void* pContext);

		public:
			/// FixedAllocatorBase
			/// Class constructor. Calls Init with the parameters passed in.
			/// However, an nInitialSize of -1 (~0) causes no initial allocation to take place.
			/// See Init for additional documentation.
			FixedAllocatorBase(size_t nObjectSize, size_t nObjectAlignment = 0, 
							   size_t nCountPerCoreBlock = kDefaultCountPerCoreBlock, 
							   void* pInitialCore = NULL, size_t nInitialCoreSize = (size_t)(~0), 
							   CoreAllocationFunction pCoreAllocationFunction = NULL, 
							   CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

			/// ~FixedAllocatorBase
			~FixedAllocatorBase();

			/// Init
			/// Initializes an instance of this class, possibly with initial core
			/// memory. Calling Init a second time will have no effect, even if 
			/// core memory is passed to the second call. This is for consistency
			/// with standard programming practices for Init/Shutdown behaviour.
			/// If pCore is non-NULL, it is used as initial core, and nCoreSize
			/// is expected to be the size of the input core. If pCore is NULL
			/// and nCoreSize is non-zero, then the given amount of core size is
			/// allocated. If both pCore and nCoreSize are zero, then the 
			/// nCountPerCoreBlock template parameter is consulted and used.
			/// If this class is not initialized, then Init is called internally
			/// with default values (specified by template parameter nCountPerCoreBlock)
			/// as needed during Malloc.
			///
			/// Note that the values of both nObjectAlignment and nObjectSize may
			/// adjusted to honor alignment requirements.
			///
			/// Note: FixedAllocatorBase has a small fixed-size memory overhead that 
			/// is needed to manage the core memory supplied by the user. Thus if 
			/// you want to store 100 objects of size 3, you need to supply more than
			/// 300 bytes of memory. Specifically, you need to supply an extra amount 
			/// of memory no more than (sizeof(void*) + sizeof(size_t) + objectAlignment).
			/// It's hard to make a simple formula for this, due to platform differences
			/// and the nature of structure packing. 
			bool Init(size_t nObjectSize = 0, size_t nObjectAlignment = 0, 
					  size_t nCountPerCoreBlock = kDefaultCountPerCoreBlock, void* pCore = NULL, size_t nCoreSize = 0, 
					  CoreAllocationFunction pCoreAllocationFunction = NULL, 
					  CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

			/// Shutdown
			/// Returns the state of this class object to the same as it was upon
			/// construction. All memory is freed, regardless of whether it is 
			/// in use.
			bool Shutdown();

			/// Malloc
			/// Returns an object of the size of tempalted type T. We return 
			/// void* and not T* because we are only returning the memory for 
			/// an object of type T and are not actually returning a constructed
			/// object of type T. The latter would incur more overhead and not
			/// be in line with the purpose of this allocator, which is generally
			/// to be an operator new() behind class T. To use this allocator to 
			/// allocate true objects of type T, use placement operator new:
			///     T* pT = new(FixedAllocator.Malloc()) T;
			/// Alternatively, you can override operator new for class T.
			void* Malloc();

			/// Free
			/// Frees an object of type T. This function takes type void* and not
			/// type T as this function only frees the memory associated with the 
			/// object and doesn't destroy the object itself. This function is meant
			/// as a custom allocator behind objects of type T. To destroy an object
			/// created as described in the Malloc function, do this:
			///     pT->~T();
			///     FixedAllocator.Free(pT);
			/// Alternatively, you can override operator delete for class T.
			void Free(void* pData);

			/// Reset
			/// Frees all memory currently allocated by the user.
			/// This function does not free internal core memory.
			void Reset();

			/// AddCore
			/// This function allows you to manually add more core to the 
			/// allocator. The Init function calls this function during its
			/// processing. AddCore is separate from Init because Init is 
			/// defined as a one-time only thing and not something that
			/// should do anything when called a second time. This core will
			/// be freed by the currently set allocation function and context.
			bool AddCore(void* pCore = NULL, size_t nCoreSize = 0);

			/// FreeUnusedCore
			/// Frees any core blocks that are no longer needed.
			/// This function does not free the initial core block.
			void FreeUnusedCore();

			/// GetAllocationFunctionContext
			/// Returns the context originally passed in to the constructor
			void* GetAllocationFunctionContext() const;

		protected:
			// Copying this structure doesn't make sense, and leads to an inconsistent state.
			FixedAllocatorBase(const FixedAllocatorBase&) {}
			void operator=(const FixedAllocatorBase&) {}

		protected:
			/// Chunk
			/// A Chunk is what is returned to the user when Malloc is called. When the 
			/// chunk is free, the mpNext member is used to store the chunk in a list of
			/// free chunks. When the chunk is allocated to the user, the mpNext member
			/// area and bytes after it are owned by the user.
			struct Chunk{
				Chunk* mpNext;
			};

			/// CoreBlock
			/// This is the large block of memory from which chunks come.
			struct CoreBlock{
				CoreBlock* mpNext;               /// This exists so we can have a linked list of CoreBlocks.
				size_t     mnSize;               /// Size of the entire CoreBlock.
				size_t     mnChunkCount;         /// Number of chunks in the CoreBlock.
				char       mData[sizeof(void*)]; /// The actual size will be greater than this. Note that for storage of objects with alignment requirements, such objects may be stored after the beginning of this space.
			};

			/// LinkChunks
			/// This function links all of the chunks in the given core block.
			/// The act of linking the chunks together makes them available for
			/// allocation (i.e., they're considered part of the free list which
			/// spans all of the linked core blocks).
			bool LinkChunks(CoreBlock* pCoreBlock);

			size_t                  mnObjectSize;              /// Desired size of the allocated objects.
			size_t                  mnObjectAlignment;         /// Desired minimum alignment of the allocated objects.
			size_t                  mnCountPerCoreBlock;       /// Each time we allocate a core block, how many objects should it be able to store.
			CoreBlock*              mpHeadCoreBlock;           /// The head of the core block list, of which each core block holds many chunks.
			Chunk*                  mpHeadChunk;               /// The head of the free chunk list for user allocations.
			CoreAllocationFunction  mpCoreAllocationFunction;  /// Callback for allocating a new core block.
			CoreFreeFunction        mpCoreFreeFunction;        /// Callback for freeing an existing core block.
			void*                   mpCoreFunctionContext;     /// User-specified context passed to CoreAllocationFunction, CoreFreeFunction.
		};



		///////////////////////////////////////////////////////////////////////////////
		/// class FixedAllocator
		///
		/// Implements an allocator for objects strictly of class T. Space is used
		/// very efficiently and allocation is very fast. If alignment is specified,
		/// then objects are aligned to the given alignment value. Alignment must be 
		/// zero or a positive power of two. An alignment of zero results in the 
		/// system default alignment being used, which is defined as sizeof(uint64_t).
		///
		template <class T, int alignment = 0>
		class FixedAllocator : public FixedAllocatorBase
		{
		public:
			/// this_type
			typedef FixedAllocator<T, alignment> this_type;

			/// value_type
			/// This is a synonym for type T, for convenience of external uses.
			typedef T value_type;

			/// FixedAllocator
			/// Default constructor.
			/// See FixedAllocatorBase for documentation.
			FixedAllocator(size_t nCountPerCoreBlock = kDefaultCountPerCoreBlock, void* pInitialCore = NULL, size_t nInitialCoreSize = (size_t)(~0), 
						   CoreAllocationFunction pCoreAllocationFunction = NULL, 
						   CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

			/// Init
			/// Allows you to initialize the FixedAllocator manually after construction.
			/// See FixedAllocatorBase for documentation.
			bool Init(size_t nCountPerCoreBlock = kDefaultCountPerCoreBlock, void* pCore = NULL, size_t nCoreSize = 0, 
					  CoreAllocationFunction pCoreAllocationFunction = NULL, 
					  CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

		protected:
			// Copying this structure doesn't make sense, and leads to an inconsistent state.
			FixedAllocator(const this_type& x) : FixedAllocatorBase(x) {}
			void operator=(const this_type&) {}
		};


		// Default allocation and free functions.
		PPM_API void* DefaultFixedAllocationFunction(size_t nSize, void* pContext);
		PPM_API void  DefaultFixedFreeFunction(void* pBlock, void* pContext);
		PPM_API void* NULLFixedAllocationFunction(size_t nSize, void* pContext);  /// This function unilaterally returns NULL. Useful for forcing the FixedAllocator to never allocate new core.
		PPM_API void  NULLFixedFreeFunction(void *pData, void* pContext);         /// This function unilaterally does nothing. Useful for forcing the FixedAllocator to never allocate new core.
		PPM_API void* NamedFixedAllocationFunction(size_t nSize, void* pContext);
		PPM_API void  NamedFixedFreeFunction(void* pBlock, void* pContext);


		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// struct NamedFixedAllocatorContext
		///
		/// Context information used to track memory allocations
		///
		struct NamedFixedAllocatorContext
		{
			const char*     mpName;
			uint32_t        mFlags;

			NamedFixedAllocatorContext(const char* pName, uint32_t flags = 0)
			{
				mpName = pName;
				mFlags = flags;
			}
		};

	} // namespace Allocator

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace Allocator
	{
		inline void* FixedAllocatorBase::GetAllocationFunctionContext() const
		{
			return mpCoreFunctionContext;
		}

		inline void* FixedAllocatorBase::Malloc()
		{
			Top:
			if(mpHeadChunk)
			{
				Chunk* const pChunk = mpHeadChunk;
				mpHeadChunk = pChunk->mpNext;
				return pChunk;
			}

			if(AddCore())
				goto Top; // We use goto instead of a recursive call in order to facilitate inlining. A loop would be less efficient due to bad branch prediction.
			return NULL;
		}


		inline void FixedAllocatorBase::Free(void* pData)
		{
			// We shouldn't need to check for pData == NULL here as we 
			// are emulating the C runtime library. C++ delete allows
			// you to pass NULL pointers to it. STL containers are 
			// supposed to check for NULL before freeing memory, but
			// at least one STL implementation (STLPort) disobeys the 
			// standard and will attempt to free NULL pointers.
			Chunk* const pChunk = static_cast<Chunk*>(pData);
			pChunk->mpNext = mpHeadChunk;
			mpHeadChunk = pChunk;
		}


		template <class T, int alignment>
		inline FixedAllocator<T, alignment>::FixedAllocator(size_t nCountPerCoreBlock, void* pInitialCore, size_t nInitialCoreSize, 
															CoreAllocationFunction pCoreAllocationFunction, 
															CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
			:  FixedAllocatorBase(sizeof(T), alignment, nCountPerCoreBlock, 
								  pInitialCore, nInitialCoreSize, pCoreAllocationFunction, 
								  pCoreFreeFunction, pCoreFunctionContext)
		{
		}


		template <class T, int alignment>
		inline bool FixedAllocator<T, alignment>::Init(size_t nCountPerCoreBlock, void* pCore, size_t nCoreSize, 
													   CoreAllocationFunction pCoreAllocationFunction, 
													   CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
		{
			return FixedAllocatorBase::Init(sizeof(T), alignment, nCountPerCoreBlock, 
								  pCore, nCoreSize, pCoreAllocationFunction, 
								  pCoreFreeFunction, pCoreFunctionContext);
		}


	} // namespace Allocator

} // namespace EA


#endif // PPMALLOC_EAFIXEDALLOCATOR_H









