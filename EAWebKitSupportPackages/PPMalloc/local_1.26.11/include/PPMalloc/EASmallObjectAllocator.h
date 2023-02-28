///////////////////////////////////////////////////////////////////////////////
// EASmallObjectAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana
//
// This implementation has roots in various small object allocators present 
// at EA, including DynamicSOA, CMemPool, and LionSmallAlloc.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASMALLOBJECTALLOCATOR_H
#define PPMALLOC_EASMALLOBJECTALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/shared.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		/// SmallObjectAllocator
		///
		/// Implements a memory heap tailored for small objects.
		///
		/// This class has no internally provided thread safety. Use of this 
		/// allocator by multiple threads currently must be manually coordinated.
		///
		/// Small object allocators typically provide optimizations in the way
		/// of speed and fragmentation reduction. However, there is a tradeoff
		/// between these two features whereby you can improve fragmentation
		/// behaviour further at the cost of some speed. SmallObjectAllocator
		/// is designed to improve fragmentation more than speed where there 
		/// happens to be such a tradeoff. 
		///
		/// This class does not implement allocation of memory beyond the max
		/// small object size. It does not forward Malloc calls to any other
		/// allocator in the case that it cannot allocate the size you request.
		/// In these cases you need to wrap SmallObjectAllocator and implement
		/// the error handling/allocation forwarding yourself. The reason for 
		/// this is that SmallObjectAllocator couldn't provide any better 
		/// handling than you could yourself, whereas you get more flexibility
		/// if you handle it yourself.
		///
		/// This allocator is quite fast. However, if you want the absolute fastest
		/// allocator, you may want to use your own hand-tailored small block 
		/// allocator or use PPMalloc StackAllocator. SmallBlockAllocator favors
		/// reduction of fragmentation above absolute maximization of speed. 
		/// It also implements a reasonable compromise for genericity and speed,
		/// whereas a hand-tailored implementation can be hard-coded directly 
		/// for your own circumstances. You may find it best to start with 
		/// SmallObjectAllocator and once you see how your application works,
		/// you can write your own, make a modified branch of SmallObjectAllocator,
		/// or just stick with SmallObjectAllocator.
		///
		/// Example usage:
		///     SmallObjectAllocator soa;
		///     soa.Init();
		///     soa.SetCoreAllocationFunction(pSomeAllocationFunction, pSomeFreeFunction, pSomeContext);
		///     void* p = soa.Malloc(17); 
		///     soa.Free(p);
		///     soa.Shutdown();
		///
		class PPM_API SmallObjectAllocator
		{
		public:
			enum Const
			{
				kMinAlignment      =              8,    /// Min alignment the user can be guaranteed. Note that larger pools can and will guarantee larger alignments.
				kMinPoolSize       =              8,    /// The min size of Chunks that are returned to users.
				kMaxPoolSize       =          65535,    /// The max size of Chunks that are returned to users (and thus the largest possible allocation size).
				kMinPoolIncrement  =   kMinPoolSize,    /// Successive pools must be of at least this increment larger in allocation size.
				kMaxPoolCount      =             32     /// The maximum number of pools possible.
			};

			enum Default
			{
				kDefaultPoolCount           =   16,
				kDefaultPoolIncrement       =    8,
				kDefaultCoreBlockCapacity   =   32,     /// We specify CoreBlock defaults in terms of capacity instead of size.
				kDefaultMinAlignment        =    8,
				kDefaultMaxAlignment        =   64,
				kDefaultMaxMallocWaste      =   12,
				kDefaultMaxPoolSize         =   kDefaultPoolCount * kDefaultPoolIncrement
			};

			/// Parameters
			/// This struct is used to configure SmallObjectAllocator.
			/// Its default constructor sets the parameters to default 
			/// values as defined by the Default enum. 
			struct PPM_API Parameters
			{
				uint32_t mnPoolCount;                           /// The number of Pools, each of increasing allocation capacity. The amount of capacity that is increased is determined by pool increment. Must be <= kMaxPoolCount.
				uint32_t mnPoolSize[kMaxPoolCount];             /// The size of the given pool. mnPoolSize[n] must be >= mnPoolSize[n-1]. There must be at least mnPoolCount entries specified.
				uint32_t mnCoreBlockSize[kMaxPoolCount];        /// The CoreBlock size in bytes used by the Pools (one entry per Pool). There must be at least mnPoolCount entries specified. Any unspecified entries use the previous entry.
				uint32_t mnMinAlignment;                        /// Min alignment for all user allocated chunks. Must be an even power of two.
				uint32_t mnMaxMallocWaste;                      /// Max space the user is willing to waste within a user allocated chunk. 
				uint32_t mnMaxMallocSize;                       /// Max size for an allocation. This is a calculated value which depends on the other values. Do not set it. It is returned in the GetParameters function.
				void*    mpPoolMemory;                          /// Memory used to implement mnPoolCount pools. If non-NULL, must be kPoolSize * mnPoolCount in size and kPoolAlignment in alignment. You can also just use SmallObjectAllocatorN. If NULL, the pool memory is allocated by user-specified CoreAllocationFunction.

				Parameters();                                   /// Constructs with default parameters.
			};

			/// GetCoreBlockCapacityFromSize
			/// Given a CoreBlock allocation count capacity, this function returns the associated 
			/// allocation count capacity that results. This might be useful if you want to define
			/// CoreBlock size via how many allocation they can serve than by what their size is.
			/// This function can be used to calculate values for the Parameters::mnCoreBlockSize fields.
			static uint32_t GetCoreBlockSizeFromCapacity(uint32_t nCoreBlockCapacity, uint32_t nPoolSize);

			/// GetCoreBlockCapacityFromSize
			static uint32_t GetCoreBlockCapacityFromSize(uint32_t nCoreBlockSize, uint32_t nPoolSize);

			/// CoreAllocationFunction
			/// This function is used to allocate CoreBlocks which fund the individual memory pools.  / The alignment
			/// requested will be the same alignment required for the pool this CoreBlock / will be associated with. The
			/// alignment offset will be a value which reflects the CoreBlock / header which will be placed at the front
			/// of the CoreBlock. If the memory returned by this / function doesn't have the required alignment/alignment
			/// offset, the user data will be non-optimally / organized and thus some space will be wasted.
			typedef void* (*CoreAllocationFunction)(SmallObjectAllocator* pAllocator, size_t nSize, size_t nAlignment, size_t nAlignmentOffset, void* pContext);

			/// CoreFreeFunction
			/// Frees memory allocated by CoreAllocationFunction.
			typedef void (*CoreFreeFunction)(SmallObjectAllocator* pAllocator, void* pCoreBlock, void* pContext);

		public:
			/// SmallObjectAllocator
			/// Default constructs SmallObjectAllocator. This function does not initialize
			/// the class and thus needs to be followed by an Init call.
			SmallObjectAllocator();

			/// SmallObjectAllocator
			/// Construct and Init the allocator with the given parameters and callback functions.
			SmallObjectAllocator(const Parameters& parameters, CoreAllocationFunction pAllocationFunction = NULL, 
									CoreFreeFunction pFreeFunction = NULL, void* pContext = NULL);

			/// ~SmallObjectAllocator
			/// Shuts down the allocator if not already shut down.
		   ~SmallObjectAllocator();

			/// Init
			/// Sets up a SmallObjectAllocator with a given set of parameters.
			/// You do not need to call Init if you use the non-default constructor, 
			/// as it will call Init for you.
			/// The user can directly supply the Pool housekeeping memory (pPoolMemory), which
			/// is used to maintain pools but is dynamic in size based on the desired pool count.
			/// If parameters.mpPoolMemory is NULL, it will be dynamically allocated by 
			/// Init via calls to the specified CoreAllocationFunction (which in this case 
			/// must be non-NULL) with:
			///     size      = parameters.mnPoolCount * kPoolSize
			///     alignment = kPoolAlignment
			///     offset    = 0
			bool Init(const Parameters& parameters, CoreAllocationFunction pAllocationFunction = NULL, 
						CoreFreeFunction pFreeFunction = NULL, void* pContext = NULL);

			/// Shutdown
			/// Returns a SmallObjectAllocator to essentially the state it was in prior to Init.
			bool Shutdown();

			/// SetCoreAllocationFunction
			/// Sets the callback function used to allocate a new CoreBlock for a fixed 
			/// size memory pool. This function needs to be called before any memory is 
			/// allocated from the current allocator. It also needs to be called before 
			/// Init is called if the user is not supplying pool housekeeping memory to Init.
			void SetCoreAllocationFunction(CoreAllocationFunction pCoreAllocationFunction, 
										   CoreFreeFunction pCoreFreeFunction, void* pCoreBlockAllocationContext);

			/// GetParameters
			/// Returns the parameters that were set during initialization.
			/// Parameters are immutable after initialization.
			/// Returns true if SmallObjectAllocator is initialized, else false.
			bool GetParameters(Parameters& parameters);

			/// Malloc
			/// Returns a block of the given size and of kMinAlignment.
			/// Returns NULL if memory is exhausted or out of the supported range specified 
			/// by the configuration Parameters.
			/// This function is very fast for the case where there is free memory in the
			/// memory pools for the given size. 
			void* Malloc(size_t nSize);

			/// MallocAligned
			/// Returns a block that is of the given size and alignment.
			/// Returns NULL if memory is exhausted or out of the supported range specified 
			/// by the configuration Parameters.
			/// This function is slower than the Malloc function as it searches
			/// for a suitable block to return to the user instead of quickly popping
			/// a block off an appropriately sized bin.  
			void* MallocAligned(size_t nSize, size_t nAlignment);


			/// Realloc
			/// Reallocates the memory p to the new size nNewSize.
			/// Returns the pointer to the new memory or NULL if memory is exhausted or 
			/// out of the supported range specified by the configuration Parameters.
			///
			/// Detailed behaviour description:
			///     p      nNewSize  return    semantics
			///     -------------------------------------------------------------
			///     NULL     >0      NULL      allocation failed.
			///     NULL     >0      valid     allocation succeeded.
			///     valid    >0      NULL      allocation failed, original pointer not freed.
			///     valid    >0      valid     allocation failed, original pointer freed.
			///  PPM_REALLOC_C99_ENABLED => 0
			///     NULL      0      NULL      no-op.
			///     valid     0      NULL      original pointer freed.
			///  PPM_REALLOC_C99_ENABLED => 1 (this is the default)
			///     NULL      0      NULL      allocation of zero size block failed.
			///     NULL      0      valid     allocation of zero size block succeeded.
			///     valid     0      NULL      allocation of zero size block failed, original pointer not freed.
			///     valid     0      valid     allocation of zero size block succeeded, original pointer freed.
			void* Realloc(void* p, size_t nNewSize, size_t nOldSize = 0);


			/// Free
			/// If the nSize parameter is non-zero, it specifies the size of the memory to free.
			/// A non-size nSize parameter allows the Free function to execute significantly
			/// faster than otherwise.
			void Free(void* p, size_t nSize = 0);


			/// GetUsableSize
			/// Report the number of usable bytes associated with p. Thus if the user allocates
			/// 30 bytes it is possible that the block returned to the user actually has 40 bytes of 
			/// usable space. A non-NULL input pointer must be valid or the return value is (size_t)-1.
			/// This function is not necessarily fast and should not be frequently called in 
			/// code that needs to be fast. Note that if you use page-aligned/sized core blocks 
			/// then this function can in fact be fairly fast.
			size_t GetUsableSize(const void* p, size_t nSize = 0) const;

			/// Runtime Options
			enum Option
			{
				kOptionNone                      =   0,
				kOptionMaxChunkWaste             =   1,   /// Specifies the maximum amount of allowable wasted space within memory returned to the user. This option can be set at any time.
				kOptionGuardFill                 =   2,   /// Specifies the number of guard fill bytes to append to allocated memory when PPM_DEBUG >= 2. Default is zero. This option must be set before any Malloc functions are called.
				kOptionPagedCoreBlockSize        =   3,   /// If non-zero, enables paged-size CoreBlocks with a CoreBlock alignment of the given value, which must be a power of two. The CoreBlock size itself must be <= its alignment value. This option must be set before any Malloc functions are called.
				kOptionAutoShrinkAll             =   4,   /// If set to non-zero, specifies that for any pool which has a free CoreBlock, free the CoreBlock from the pool. This option can be set at any time.
				kOptionAlwaysFreeCoreBlocks      =   5,   /// If set to non-zero, a pool's core block will always be freed, even if it would otherwise be retained due to it being the last one for the pool. Default is zero (disabled).
				kOptionEnableMallocFailureAssert =   6,   /// If enabled then Malloc functions execute PPM_ASSERT(false) before returning. This is disabled by default.
				kOptionAutoShrinkPoolBegin       = 100    /// Same as kOptionAutoShrinkAll, but can be applied to an individual pool. kOptionAutoShrinkPoolBegin + 3 refers to pool[3]. This option can be set at any time.
			};

			/// SetOption
			/// The user specifies one of enum Option. Note that the argument 'option' is 
			/// of type int instead of type Option because subclasses may want to extend
			/// the option set to include values not listed here.
			void SetOption(int option, int nValue);

			/// GetOption
			/// Returns a value that is logically equivalent to one set by SetOption or whatever the default is.
			int GetOption(int option) const;


			/// AssertionFailureInfo
			/// Struct passed to user upon assertion failure.
			struct AssertionFailureInfo
			{
				const SmallObjectAllocator* mpAllocator;          /// The allocator the assertion is associated with. May be NULL for failures that are without allocator context.
				const char*                 mpExpression;         /// The assertion expression that failed.
				int                         mnErrorId;            /// One of enum GAError or a user-defined or subclass-defined id.
				const char*                 mpDescription;        /// A description of the assertion failure.
				const void*                 mpData;               /// The user allocation the assertion is associated with. May be NULL for failures that are without allocation context.
				const void*                 mpExtra;              /// Other information related to the failure. The meaning depends on the failure id.
			};

			/// SetAssertionFailureFunction
			/// Allows the user to specify a callback function to trap assertion failures.
			/// The user must not attempt to modify the allocator while in this failure callback.
			typedef void (*AssertionFailureInfoFunction)(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
			void SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext);


			/// AssertionFailure
			/// Triggers an assertion failure. This function is generally intended for internal
			/// use but is available so that debug systems wrapped around this allocator can
			/// use the standard assertion facilities.
			void AssertionFailure(const char* pExpression, int nErrorId = kGAErrorUnknown, const char* pDescription = NULL, 
								  const void* pData = NULL, const void* pExtra = NULL) const;

			/// AddressType
			/// Used to categorize pointers for the ValidateAddress function.
			enum AddressType
			{
				kAddressTypeOwned     = -1, /// The memory referred to by the pointer is owned by the given allocator. It may be allocated or free.
				kAddressTypeAllocated =  0, /// The memory referred to by the pointer is both owned by the given allocator and refers to valid user-allocated memory, but may not be the allocated pointer itself but merely with the allocated block.
				kAddressTypeSpecific  =  1  /// The memory referred to by the pointer is both owned by the given allocator, refers to valid user-allocated memory, and is also the allocated pointer itself and not merely any memory within the allocated block.
			};

			/// ValidateAddress
			/// Tells you if the given address is valid.
			const void* ValidateAddress(const void* pAddress, int addressType = kAddressTypeAllocated) const;


			/// HeapValidationLevel
			/// Defines levels of extensiveness for the ValidateHeap function.
			enum HeapValidationLevel
			{
				kHeapValidationLevelNone,
				kHeapValidationLevelBasic,
				kHeapValidationLevelDetail,
				kHeapValidationLevelFull
			};

			/// ValidateHeap
			/// Does a check of the heap for errors.
			bool ValidateHeap(HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);


			/// BlockType
			/// Used in memory reports and snapshots.
			enum BlockType
			{
				kBlockTypeNone      =  0,  ///
				kBlockTypeInvalid   =  1,  /// The block information is invalid.
				kBlockTypeAllocated =  2,  /// The block is allocated.
				kBlockTypeFree      =  4,  /// The block is not allocated.
				kBlockTypeCore      =  8,  /// The block is a core area, which is a large area of memory from which regular blocks come.
				kBlockTypeInternal  = 16,  /// The block is internal, allocated by the allocator itself.
				kBlockTypeAll       = 31    /// This is a flag that includes all other flags.
			};


			/// SetTraceFunction
			/// Allows you to override where traces go. If not specified, traces are sent via 
			/// printf to 'stdout'. Strings passed to the TraceFunction will have trailing newlines.
			typedef void (*TraceFunction)(const char* pTraceText, void* pContext);
			void SetTraceFunction(TraceFunction pTraceFunction, void* pContext);


			/// TraceAllocatedMemory
			/// Writes out a description of existing allocated memory to the input pTraceFunction.
			/// If the input pTraceFunction is NULL, the default member trace function is used.
			/// This function is unique amongst the reporting functions in that it is not re-entrant.
			/// If you specify a custom tracing function, it cannot safely call functions that
			/// modify allocated memory. The reason for this limitation is that it would likely 
			/// require too much backup memory to support this functionality. Similarly, this 
			/// function keeps a mutex locked for the entire operation and thus external code 
			/// must be careful in any mutexes it locks itself lest there be a potential for deadlock.
			/// A simpler but re-entrant-safe memory trace can be obtained via the ReportHeap function. 
			void TraceAllocatedMemory(TraceFunction pTraceFunction = NULL, void* pTraceFunctionContext = NULL, 
										int nBlockTypeFlags = kBlockTypeAllocated);


			struct BlockInfo
			{
				const char*  mpCore;          /// CoreBlock which mpData belongs to.
				size_t       mnCoreSize;      /// The size of the CoreBlock (including its user data).
				void*        mpData;          /// The user data within this block.
				size_t       mnDataSize;      /// The size of the block (>= user requested size).
				char         mBlockType;      /// One of enum BlockType
			};

			struct ReportContext
			{
				int         mnBlockTypeFlags;   /// Global block type flags filter to apply. See enum BlockType.
				const void* mpPool;             /// Current Pool
				const void* mpCoreBlock;        /// Current CoreBlock
				const void* mpChunk;            /// Current Chunk
				BlockInfo   mBlockInfo;         /// Information about the currently iterated node.
			} PPM_MAY_ALIAS;

			ReportContext*    ReportBegin(ReportContext* pReportContext, int nBlockTypeFlags = kBlockTypeAll);
			const BlockInfo*  ReportNext(ReportContext* pContext, int nBlockTypeFlags = kBlockTypeAll);
			void              ReportEnd(ReportContext* pContext);

		protected:
			// Constants
			static const uint32_t kPoolIndexInvalid        =        0xffffffff;
			static const  int32_t kPoolIndexTableIncrement = kMinPoolIncrement;   // This needs to be <= kMinPoolSize and be signed.
		  #if EA_PLATFORM_PTR_SIZE == 8
			static const uint32_t kMinCoreBlockDataAlignment = 16;   // Minimum value for CoreBlock::mData.
		  #else
			static const uint32_t kMinCoreBlockDataAlignment =  8;   // Minimum value for CoreBlock::mData.
		  #endif

			// Forward declarations
			struct Chunk;
			struct CoreBlock;
			struct CoreBlockList;
			struct Pool;


			/// Chunk
			/// A Chunk is what is returned to the user, though the user interprets
			/// the Chunk* as void* of some size >= sizeof(Chunk). Chunk memory comes
			/// from CoreBlocks.
			struct Chunk
			{
				Chunk* mpNext;  // Ideally this would be a union of Chunk* and void* or char[1].
			};


			/// CoreBlock
			/// A CoreBlock is a contiguous slab of memory which is divided up into Chunks and
			/// given to the user. Each Chunk is of the same size. A given CoreBlock is owned by 
			/// a Pool and not owned by any other Pool. However, as Pool may have more than
			/// one CoreBlock (in its CoreBlockList).
			struct PPM_API CoreBlock
			{
				CoreBlock*  mpPrev;                 /// Previous CoreBlock in doubly-linked list.
				CoreBlock*  mpNext;                 /// Next CoreBlock in doubly-linked list. However, the list is not circular: the last CoreBlock in the list has an mpNext of NULL.
				Chunk*      mpChunkData;            /// Points to where the Chunk data is within mData[]. Thus: ((mpChunkData >= mData) && (mpChunkData < (this + mnSize)))
				Chunk*      mpChunkList;            /// Singly-linked list of free Chunks.
				Pool*       mpPool;                 /// The Pool this CoreBlock is owned by.
				uint32_t    mnSize;                 /// Size of this CoreBlock, including mData[] memory at end.
				uint32_t    mnFreeChunkCount;       /// Count of free Chunks in this CoreBlock. Will be <= mnChunkCount.
				uint16_t    mnChunkSize;            /// Size of individual Chunks. Will be >= sizeof(Chunk) and < mSize.
				uint16_t    mnChunkCount;           /// Total count (free + allocated) of Chunks in this CoreBlock. This is a constant value for the CoreBlock.
				#if (EA_PLATFORM_PTR_SIZE == 8)
				uint32_t    mUnused1;               /// This is present so that mData is aligned on a 16 byte boundary.
				uint32_t    mUnused2;
				uint32_t    mUnused3;
				#endif
				uint8_t     mData[sizeof(Chunk)];   /// Actual data size will be >= this size.

				bool    Contains(const void* p) const;
				Chunk*  GetChunk(const void* p);
				bool    IsChunkFree(const Chunk* pChunk) const;
				void    RemoveChunk(const Chunk* pChunk, Chunk* pPrev);
			};


			/// CoreBlockList
			/// Implements a list of CoreBlocks.
			/// The list is ordered such that the entries at the front of the list have the
			/// most free Chunks (highest mnFreeChunkCount).
			struct PPM_API CoreBlockList
			{
				CoreBlockList();

				bool Contains(const CoreBlock* pCoreBlock) const;
				void PushFront(CoreBlock* pCoreBlock);
				void InsertBefore(CoreBlock* pCoreBlock, CoreBlock* pCoreBlockNext);
				void InsertAfter(CoreBlock* pCoreBlock, CoreBlock* pCoreBlockPrev);
				void Remove(CoreBlock* pCoreBlock);
				void AdjustCoreBlockPositionMalloc(CoreBlock* pCoreBlock);
				void AdjustCoreBlockPositionFree(CoreBlock* pCoreBlock);

				CoreBlock* mpCoreBlockFirst;         /// First CoreBlock in a doubly-linked (but not circular) CoreBlock list.
			};


			/// Pool
			/// Implements a pool with one or more core memory sources.
			/// Each Pool supplies memory of a fixed size. 
			struct PPM_API Pool
			{
				Pool();

				CoreBlockList   mCoreBlockList;         /// Core blocks used to fund this Pool. It is sorted in order of free space. The first CoreBlock has the most free space, the last one has the least free space.
				CoreBlock*      mpCoreBlockCurrent;     /// The CoreBlock we allocate from. This is the CoreBlock in CoreBlockList with the least amount of free space.
				uint32_t        mnChunkSize;            /// Size of each Chunk.
				uint32_t        mnChunkAlignment;       /// Alignment of Chunks in this Pool.
				uint32_t        mnCoreBlockSize;        /// Size of CoreBlock in bytes. The number of Chunks present in the CoreBlock depends on the chunk size and alignment.
				bool            mbAutoShrink;           /// If enabled, newly empty CoreBlocks from mCoreBlockList are freed.
			};


			// ReportContextInternal
			// Same as ReportContext, except the types are valid and not void.
			struct ReportContextInternal
			{
				int              mnBlockTypeFlags;   /// Global block type flags filter to apply. See enum BlockType.
				Pool*            mpPool;             /// Current Pool
				CoreBlock*       mpCoreBlock;        /// Current CoreBlock
				Chunk*           mpChunk;            /// Current Chunk
				BlockInfo        mBlockInfo;         /// Information about the currently iterated node.
			} PPM_MAY_ALIAS;

		public:
			// These definitions are down here in the class definition because they depend on protected data declared directly above.
			static const size_t kPoolSize       = sizeof(Pool);
			static const size_t kPoolAlignment  = EA_ALIGN_OF(Pool);

		protected:
			void            Construct();
			void*           AllocatePoolMemory(uint32_t poolCount);
			void            FreePoolMemory(void* pPoolMemory);
			CoreBlock*      AllocateCoreBlock(uint32_t nSize, uint32_t nMinChunkAlignment);
			void            FreeCoreBlock(CoreBlock* pCoreBlock);
			CoreBlock*      GrowPool(Pool* pPool);
			bool            ChunkMatchesBlockType(const CoreBlock* pCoreBlock, const Chunk* pChunk, int nBlockTypeFlags, bool& bChunkIsFree);
			const Chunk*    GetAllocationInfo(const void* p, size_t nSize, Pool*& pPool, CoreBlock*& pCoreBlock) const;  
			static void     AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);

		protected:
			Pool*       mPoolArray;                             /// Array of Pool[mnPoolCount].
			uint32_t    mnPoolCount;                            /// Value in range of [1, kMaxPoolCount].
			bool        mbPoolAllocated;                        /// True if mPoolArray was allocated by us instead of provided by the user.
			bool        mbAlwaysFreeCoreBlocks;                 /// If true then a pool's core block will always be freed, even if it would otherwise be retained due to it being the last one for the pool.
			bool        mbMallocFailureAssertEnabled;           /// See kOptionEnableMallocFailureAssert.
			uintptr_t   mnCoreBlockAlignmentMask;               /// Used to tell if all CoreBlocks are of the same alignment and page size.
			uint32_t    mnMaxMallocSize;                        /// This is a calculated value equal to (mnPoolCount * mnPoolIncrement).
			uint32_t    mnPoolIndexCountMax;                    /// 
			uint8_t*    mPoolIndexTable;                        /// mPoolIndexTable[alloc_size / kPoolIndexTableIncrement] == pool index. To consider: make this an array of uint32_t instead of uint8_t.
			uint32_t    mnMinAlignment;                         /// Specifies the minimum alignment for any user allocated memory. Will be >= kMinAlignment.
			uint32_t    mnMaxMallocWaste;                       /// Specifies the most amount of memory internal to an allocation (Chunk) that the user is willing to waste.

			CoreAllocationFunction mpCoreAllocationFunction;
			CoreFreeFunction       mpCoreFreeFunction;
			void*                  mpCoreAllocationContext;

			AssertionFailureInfoFunction mpAssertionFailureFunction;
			void*                        mpAssertionFailureFunctionContext;

			TraceFunction       mpTraceFunction;
			void*               mpTraceFunctionContext;
			unsigned char       mcTraceFieldDelimiter;            /// Delimits fields in an allocation trace. Defaults to '\t'.
			unsigned char       mcTraceRecordDelimiter;           /// Delimits records in an allocation trace. Defaults to '\n'.

		}; // SmallObjectAllocator



		/// SmallObjectAllocatorN
		/// 
		/// Implements a SmallObjectAllocator with built-in Pool memory.
		/// This class can help performance because it results in the pool memory being 
		/// close to the other SmallObjectAllocator memory.
		///
		/// Example usage:
		///     SmallObjectAllocator::Parameters param;
		///     param.mnPoolCount = 5;
		///
		///     SmallObjectAllocatorN<5> soa;
		///     soa.Init(param);
		///     soa.SetCoreAllocationFunction(pSomeAllocationFunction, pSomeFreeFunction, pSomeContext);
		///
		///     void* p = soa.Malloc(17); 
		///     soa.Free(p);
		///
		///     soa.Shutdown();
		///
		template <size_t kPoolCount>
		class SmallObjectAllocatorN : public SmallObjectAllocator
		{
		public:
			SmallObjectAllocatorN();
			SmallObjectAllocatorN(const Parameters& parameters);

			bool Init(Parameters& parameters);

		protected:
			uintptr_t mpPoolMemory[kPoolCount * sizeof(Pool) / sizeof(uintptr_t)];
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

		inline
		bool SmallObjectAllocator::CoreBlock::Contains(const void* p) const 
		{
			return (((uintptr_t)p >= (uintptr_t)mpChunkData) && ((uintptr_t)p < ((uintptr_t)this + (uintptr_t)mnSize)));
		}

		inline
		SmallObjectAllocator::Chunk* SmallObjectAllocator::CoreBlock::GetChunk(const void* p) 
		{
			return (Chunk*)((uintptr_t)mpChunkData + ((((uintptr_t)p - (uintptr_t)mpChunkData) / mnChunkSize) * mnChunkSize));
		}

		inline
		void SmallObjectAllocator::CoreBlock::RemoveChunk(const Chunk* pChunk, Chunk* pPrev)
		{
			if(pPrev)
				pPrev->mpNext = pChunk->mpNext;
			else
				mpChunkList = pChunk->mpNext;
		}

		inline
		void SmallObjectAllocator::FreePoolMemory(void* pPoolArray)
		{
			mpCoreFreeFunction(this, pPoolArray, mpCoreAllocationContext);
		}

		inline
		void SmallObjectAllocator::FreeCoreBlock(CoreBlock* pCoreBlock)
		{
			// To consider: If there are no more CoreBlocks at all, then we can reset mnCoreBlockAlignmentMask.
			mpCoreFreeFunction(this, pCoreBlock, mpCoreAllocationContext);
		}


		template <size_t kPoolCount>
		inline SmallObjectAllocatorN<kPoolCount>::SmallObjectAllocatorN() 
		  : SmallObjectAllocator()
		{
		}

		template <size_t kPoolCount>
		inline SmallObjectAllocatorN<kPoolCount>::SmallObjectAllocatorN(const Parameters& parameters)
			: SmallObjectAllocator()
		{
			Init(parameters);
		}

		template <size_t kPoolCount>
		inline bool SmallObjectAllocatorN<kPoolCount>::Init(Parameters& parameters)
		{
			// PPM_ASSERT_STATIC(parameters.mnPoolCount <= (uint32_t)kPoolCount, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			parameters.mnPoolCount  = kPoolCount;
			parameters.mpPoolMemory = mpPoolMemory;

			return SmallObjectAllocator::Init(parameters);
		}


	} // namespace Allocator

} // namespace EA

#endif // Header include guard
