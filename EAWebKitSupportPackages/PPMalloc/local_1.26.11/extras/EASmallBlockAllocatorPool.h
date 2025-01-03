///////////////////////////////////////////////////////////////////////////////
// SmallBlockAllocatorPool
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Henry Allen
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EA_SMALLBLOCKALLOCATORPOOL_H
#define EA_SMALLBLOCKALLOCATORPOOL_H

#include <EABase/eabase.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/shared.h>
#include "EASmallBlockAllocator.h"


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


namespace EA
{
namespace Allocator
{
/// Pool
///
/// Pool - Supplies memory pooling functionality similar to a FixedAllocator
/// with a few important distinctions:
/// - Shrink support: pools are composed of one or more CoreBlocks and the free Chunks
/// are tracked per CoreBlock which allows us to free complete CoreBlocks when they are
/// no longer in use.
/// - 'Nested' CoreBlock support: a couple of methods are provided that allow you to
/// treat a Chunk from a given memory pool as a CoreBlock from another pool. This
/// allows for certain optimizations when operating on pools that have their CoreBlocks
/// supplied by another pool.
/// - The pool can be configured to either grow and shrink using an allocator or
/// by using Chunks from another pool as CoreBlocks.
///
/// Features:
///     - the only dynamic memory allocations are for growth CoreBlocks
///     - when memory is needed, a CoreBlock is obtained and divided up into as many
///         'Chunksize' Chunks as possible (based on CoreBlock size). Chunks are stored
///         in a free list where memory from the Chunks themselves is used to store next
///         pointers (so no per Chunk overhead except for some per CoreBlock overhead +
///         waste that occurs when the remaining CoreBlock size is not evenly divisible
///         by 'Chunksize'.
///     - supports shrinking the pool by 1 CoreBlock at a time when a CoreBlock becomes
///         completely unused.
///     - does not support user specified alignment per allocation, however, Chunks are
///         always gaurenteed to be aligned to the Chunk size relative to the CoreBlock
///         alignment. i.e., if Chunk size == 32, all Chunks will be an even multiple
///         of 32 bytes from the start of the CoreBlock. Therefore, given an appropriate
///         CoreBlock alignment you can gaurentee alignment that can satisfy some
///         requirements.
///     - Supports properly freeing pointers that point to within an allocated Chunk
///     - 'ValidateAddress' method you can use to query whether a Chunk of memory is
///         managed by this pool (i.e., whether it's correct to free the Chunk using
///         this pool)
///     - If EA_SOA_EXTENSIONS_ENABLED is defined to true (default for debug builds)
///         Severl extra features are enabled (if a debug allocator is supplied):
///          -tracking how many times the pool grows and shrinks
///          -high water mark stats tracking
///          -storage of allocation name, file, and line number and inclusion of this
///           data in memory dumps
///          -memory auditing support
///
/// Pool is similar to rw::Core::FixedAllocator but it supports shrinking of the pool as
/// well as growing the pool size (so that all pools don't always have to be at their
/// worst case) and it's easier to consolidate multiple like-size objects into sharing
/// one pool (since it's not type based and since it supports the concept of 'Nested'
/// CoreBlocks).
/// These features make SmallBlockAllcator::Pool a good choice for some applications.
/// However, if you just need a fixed allocator that can grow (and doesn't need to shrink),
/// something like rw::Core::FixedAllocator is probably more optimal since it doesn't have
/// the overhead associated with supporting shrinking the pool.
class PPM_API SmallBlockAllocator::Pool
{
	public:
		/// Pool
		/// Constructs the object. Init must before it's ready for use.
		Pool(SmallBlockAllocator::Callbacks& callbacks);

		/// ~Pool
		/// Shuts down and destructs the object
		~Pool();

		/// Init
		/// Initializes the pool so that it will use mCallbacks allocation callbacks for growing & shrinking
		void Init(uint16_t nChunkSize, uint16_t nInitCount, uint16_t nGrowChunkCount, uint16_t nMinAlignment);

		/// Init
		/// Initializes the pool so that it will use the specified pool for growing & shrinking
		void Init(uint16_t nChunkSize, Pool* pCoreBlockAllocator);

		/// Initialized
		/// Returns true if Init has been called
		bool Initialized() const;

		/// SetOption
		/// The user specifies one of enum SmallBlockAllocator::Option. Note that the argument
		/// 'option' is of type int instead of type Option because subclasses may want
		/// to extend the option set to include values not listed here.
		void SetOption(int nOption, int nValue);

		/// Alloc
		/// Allocates a memory Chunk
		/// size must be <= GetChunkSize
		void* Alloc(size_t size, const EA::Allocator::AllocInfo& allocInfo = EA::Allocator::AllocInfo::NULLREF);

		/// Free
		/// Frees the memory at pAddress which was acquired with Alloc
		void  Free(void* pAddress);

		/// GetChunkSize
		/// Returns the size of Chunks managed by this pool
		uint16_t GetChunkSize() const;

		/// IsFull
		/// true indicates that the next allocation will require the pool to grow
		bool IsFull() const;

		/// CanShrink
		/// returns true if memory could be reclaimed by calling Shrink
		bool CanShrink() const;

		/// Shrink
		/// will free any currently unused CoreBlocks that are still being retained by this pool
		void Shrink();

		/// CoreBlockOverhead
		/// this is how much memory users should expect to lose in each CoreBlock do to header overhead
		static uint32_t CoreBlockOverhead();

		/// GetStats
		/// Calculates current snapshot of stats for this memory pool
		bool GetStats(PoolInfo& stats) const;                //returns false if the stats are not available

		/// GetHighWaterMarkStats
		/// Gets snapshot of stats for this memory pool at high water mark if available
		bool GetHighWaterMarkStats(PoolInfo& stats) const;   //returns false if these stats are not available

		/// ReportBegin
		/// Call to initialize a ReportContext before calling ReportNext. When done, call ReportEnd.
		ReportContext* ReportBegin(ReportContext* pReportContext, uint32_t nNodeTypeFlags = kNodeTypeAll);

		/// ReportNext
		/// Used to walk the nodes of a report (returns the next node). Nodes may include the pool itself,
		/// allocated Chunks, free Chunks, etc as specified by nNodeTypeFlags.
		/// Returns NULL when there are no more nodes to traverse.
		const ReportNode* ReportNext(ReportContext* pReportContext, uint32_t nNodeTypeFlags = kNodeTypeAll);

		/// ReportEnd
		/// Call to deinitialize ReportContext when done calling report next.
		void ReportEnd(ReportContext* pReportContext);

		/// TraceMemory
		/// Writes out a description of existing memory using the specified TraceCallback or, if NULL, mCallbacks.Trace methods
		/// CoreBlocks and allocated Chunks by default but nNodeTypeFlags can be set to include free Chunks as well.
		/// If EA_SOA_EXTENSIONS_ENABLED the per alloc file, line, and allocation name will be included along with high water mark stats
		/// This function is unique amongst the reporting functions in that it is not re-entrant.
		/// Your custom tracing function cannot safely call functions that modify allocated memory.
		/// The reason for this limitation is that it would likely require too much backup memory to support this functionality.
		void TraceMemory(TraceCallback* pTraceCallback = NULL, uint32_t nNodeTypeFlags = kNodeTypeChunkAllocated | kNodeTypeCoreBlock);

		/// TraceStats
		/// Prints statistics about the pool to pTraceCallback if specified, otherwise to the SmallBlockAllocator::Callback specified
		/// at initialization.
		void TraceStats(TraceCallback* pTraceCallback = NULL);

		/// SetGroupId
		/// Sets an Id that will be stored with each subsequent allocation (until a new Id is specified).
		/// Use in conjunction with Audit to identify leaks.
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is on.
		int32_t SetGroupId(int32_t nGroupId);

		/// GetGroupId
		/// Returns the value passed to the most recent call to SetGroupId
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is on.
		int32_t GetGroupId() const;

		/// Audit
		/// Calls pAuditCallback for every currently allocated Chunk that has the specified GroupId.
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is on.
		uint32_t Audit(int32_t nGroupId, AuditFunction pAuditCallback, void* pAuditCallbackContext);

		/// NestedCoreBlockFree
		/// This method is provided as an optional optimization that can be used to free a Chunk from a Pool that is getting
		/// its CoreBlocks from this pool (in other words, Chunks allocated from this pool are being used as CoreBlocks by another pool)
		/// This method will return false if pAddress does not lie within this pool.
		/// If pAddress does lie within this pool, it will be assumed that the Chunk that contains this memory is actually being used
		/// as a CoreBlock by another pool (i.e., Chunks from this pool are being acquired and used as CoreBlocks for another pool that
		/// handles smaller Chunks). The nested pool will be found by converting the Chunk containing the memory into a CoreBlock and
		/// looking up the CoreBlocks owner pool, then the memory will be freed from that pool.
		/// This method is only safe to call if all Chunks from this pool are being used as CoreBlocks by other pools and nothing else
		/// (i.e. if pAddress does lie within this pool it must be from a nested CoreBlock or results will be undefined).
		/// This method provides a useful optimization to implementations that may have many nested pools using
		/// one main pool as a CoreBlock provider as this allows you to free a Chunk of memory that may be from any one of those pools
		/// without having to search all of the pools to determine which contains the memory.
		bool NestedCoreBlockFree(void* pAddress);

		/// NestedCoreBlockGetUsableSize
		/// See NestedCoreBlockFree comments for info and assumptions regarding NestedCoreBlocks.
		/// Assuming that this pool is a CoreBlock provider for some number of other pools, this method will determine the nested CoreBlock
		/// that contains this memory (i.e., it will assume that this Chunk is used by another pool as a CoreBlock) and then use it to
		/// calculate the usable size of the Chunk (if pAddress doesn't point to the front of the Chunk then the usable size returned
		/// will be the size from pAddress to the end of the Chunk).
		/// If the address does not belong to this pool, (size_t)-1 is returned.
		size_t NestedCoreBlockGetUsableSize(const void* pAddress) const;

		/// NestedCoreBlockGetAllocationCost
		/// See NestedCoreBlockFree comments for info and assumptions regarding NestedCoreBlocks.
		/// This method is for debugging/statistics gathering only to see how much memory allocations cost (the size returned is NOT useable size)
		/// See AllocationCost for details on the costs that can be calculated.
		/// If the address does not belong to this pool (or no flags are passed in) then 0.0f is returned.
		float NestedCoreBlockGetAllocationCost(const void* pAddress, int nAllocationCostFlags ) const;

		/// NestedCoreBlockGetPool
		/// See NestedCoreBlockFree comments for info and assumptions regarding NestedCoreBlocks.
		/// Assuming that this pool is a CoreBlock provider for some number of other pools, this method will determine the nested CoreBlock
		/// that contains this memory (i.e., it will assume that this Chunk is used by another pool as a CoreBlock) and return the pool
		/// that owns that nested CoreBlock
		Pool* NestedCoreBlockGetPool(const void* pAddress);

		/// AlignedCoreBlockFree
		///     Assumptions:
		///         CoreBlock alignment >= X
		///         CoreBlock size <= X
		///         nCoreBlockAlignmentMask == ( ~((uint32_t)(unsigned)X - 1) )
		/// This method assumes that pAddress lies within a CoreBlock that meets the above assumptions.
		/// If these assumptions are incorrect behavior is undefined, otherwise it will calculate
		/// the CoreBlock & pool that contain pAddress and free it. This method is the fastest way to
		/// free memory from a pool but it may only be used when the pool is configured such that
		/// the stated assumptions are gaurenteed to be true.
		static void AlignedCoreBlockFree(void* pAddress,uintptr_t nCoreBlockAlignmentMask);

		/// AssertionFailure
		/// Triggers an assertion failure. This function is generally intended for internal
		/// use but is available so that debug systems wrapped around this allocator can
		/// use the standard assertion facilities.
		void AssertionFailure(const char* pExpression, int nErrorId /*= kGAErrorUnknown*/, const char* pDescription = NULL,
							  const void* pData = NULL, const void* pExtra = NULL) const;

		/// ValidateAddress
		/// Tells you if the given address is valid.
		const void* ValidateAddress(const void* pAddress, int addressType = kAddressTypeAllocated) const;

		/// ValidatePool
		/// attempts to validate that the pool is not corrupted (returns true if valid, else false)
		/// if pErrorCount is supplied, it will be incremented by the number of errors found
		bool ValidatePool(int* pErrorCount = NULL) const;

	private:    // enums & types

		/// CoreBlockType
		/// Indicates whether the pool uses another pool for CoreBlocks or allocation callbacks.
		enum CoreBlockType
		{
			kCoreBlockTypeInvalid   = 0,
			kCoreBlockTypeHeap      = 1,
			kCoreBlockTypePooled    = 2
		};

		struct ChunkDebugData;      /// stores debug information about a Chunk of memory
		struct CoreBlockDebugData;  /// stores debug information for a CoreBlock
		struct PoolDebugData;       /// stores debug information for a pool

		/// Chunk
		/// Chunk of memory from the CoreBlock. The Chunk will be mnChunkSize bytes.
		struct PPM_API Chunk
		{
			Chunk* mpNextFreeChunk; /// Valid only when free. Points to the next free Chunk in a list of free Chunks.
		};

		/// CoreBlock
		/// Large CoreBlock of memory from which we will form our pool of Chunks.
		struct PPM_API CoreBlock
		{
			const uint8_t* Begin() const;                       /// const pointer to the first byte of memory in the CoreBlock
			const uint8_t* End() const;                         /// const pointer to the byte just beyond the end of the CoreBlock of memory
			uint8_t* Begin();                                   /// pointer to the first byte of memory in the CoreBlock
			uint8_t* End();                                     /// pointer to the byte just beyond the end of the CoreBlock of memory

			bool     ContainsMem(const void* pAddress) const;   /// true if pAddress lies in this CoreBlock (pAddress need not point to the front of a Chunk)
			uint16_t GetChunkSize() const;                      /// size of Chunks stored in this CoreBlock
			uint16_t GetNumFreeChunks() const;                  /// number of free Chunks in this CoreBlock
			uint16_t GetNumChunks() const;                      /// total number of Chunks in this CoreBlock
			bool     IsEmpty() const;                           /// true if the CoreBlock has no allocations in it
			bool     IsFree(const void* pAddress) const;        /// true if pAddress is a free Chunk in this CoreBlock, slow because we must traverse the free list!
			uint8_t* GetFirstChunk();                           /// pointer to first Chunk in the CoreBlock

			uint32_t GetWastedSpace() const;                    /// Wasted space is left-over memory due to ChunkSize not dividing evenly into CoreBlockSize-CoreBlockOverhead.

			/// GetNestedCoreBlockFromMem
			/// assumes the Chunks of this CoreBlock are used as CoreBlocks by another pool and returns the nested CoreBlock that contains pAddress
			CoreBlock* GetNestedCoreBlockFromMem(const void* pAddress);

			CoreBlock*          mpNext;             /// next CoreBlock in a list of CoreBlocks
			CoreBlock*          mpPrevious;         /// previous CoreBlock in a list of CoreBlocks
			Pool*               mpOwner;            /// mem pool that owns this CoreBlock
			Chunk*              mpFreeChunks;       /// list of free Chunks in this CoreBlock
			CoreBlockDebugData* mpDebugData;        /// debug data for this CoreBlock

			uint32_t            mnSize;             /// size of this CoreBlock of data ... includes sizeof(CoreBlock) plus some amount of additional contiguous memory
			uint16_t            mnNumChunks;        /// how many total Chunks in this CoreBlock?
			uint16_t            mnNumFreeChunks;    /// how many Chunks currently free in this CoreBlock?

			#if (EA_PLATFORM_PTR_SIZE == 4)
			uint32_t            mUnused;            /// This is present so that mData is aligned on an 8 byte boundary on both 32 bit and 64 bit platforms.
			#endif

			uint8_t             mData[4];           /// CoreBlock memory. mData is just the first 4 bytes of the rest of the CoreBlock, the next contiguous mnSize-sizeof(CoreBlock) bytes is also part of it.
		};

		/// CoreBlockList
		/// doubly linked list of CoreBlocks
		class PPM_API CoreBlockList
		{
			public:
				/// CoreBlockList
				CoreBlockList();

				/// ~CoreBlockList
				~CoreBlockList();

				/// GetFirst
				/// gets the start of the list
				CoreBlock* GetFirst() const { return mFirst; }

				/// PushFront
				/// insert the CoreBlock at the front of the list (modifies mFirst)
				/// to simplify things, we're assuming that inserts will only happen when all other CoreBlocks in the list are fully allocated
				void PushFront(CoreBlock* pCoreBlock);

				/// Remove
				/// remove this CoreBlock from the list (may modify mFirst)
				void Remove(CoreBlock* pCoreBlock);

				/// AdjustCoreBlockPositionFree
				/// Move CoreBlocks MORE likely to be completely freed towards the front of the list (i.e. CoreBlocks towards the end
				/// have a higher priority for being used for allocations since they are less likely to be freed up)
				/// this must be called any time a CoreBlock mnNumFreeChunks increases to keep the list in order
				void AdjustCoreBlockPositionFree(CoreBlock* pCoreBlock);

			private:
				/// InsertBefore
				/// insert the CoreBlock into the list before the specified CoreBlock
				void InsertBefore(CoreBlock* pCoreBlock, CoreBlock* pInsertPos);

			private:
				CoreBlock* mFirst;      /// first CoreBlock in the list
		};

		// ReportContextInternal
		// Same as ReportContext, except the types are valid and not void.
		struct ReportContextInternal
		{
			Pool*           mPool;              /// Current Pool
			CoreBlock*      mCoreBlock;         /// Current CoreBlock
			uint8_t*        mChunk;             /// Current Chunk
			uint32_t        mnPoolIndex;        /// Index of current pool
			uint32_t        mNodeTypeFlags;     /// Global Chunk type flags filter to apply. See enum NodeType.
			ReportNode      mNode;              /// Information about the currently iterated node.
		} PPM_MAY_ALIAS;

	private:    // methods

		/// AllocateCoreBlock
		/// allocates memory for a CoreBlock and returns the size
		void*  AllocateCoreBlock(uint32_t &nCoreBlockSize);

		/// FreeCoreBlock
		/// free memory acquired with AllocateCoreBlock
		void   FreeCoreBlock(void* pCoreBlock);

		/// Construct
		/// sets all member variables do default/NULL values
		void Construct();

		/// EnableExtensions
		/// Based on Extension flags included in nValue, may enable support for auditing, allocation file/line/name tracking, and high water mark tracking
		void EnableExtensions(int nValue);

		/// Grow
		/// allocates a CoreBlock (using AllocateCoreBlock), divides it up into free allocation Chunks, and adds the CoreBlock to the pool
		CoreBlock* Grow();

		/// Shrink
		/// removes the specified CoreBlock from the pool (CoreBlock must have all Chunks free!)
		void   Shrink(CoreBlock* pCoreBlock);

		/// GetNestedCoreBlockFromMem
		/// If this address is not owned by this pool, returns NULL.
		/// Else, gets a pointer to the CoreBlock that contains this memory,
		/// calculates the Chunk within the CoreBlock that contains the address,
		/// then converts that Chunk into a CoreBlock pointer (the 'Nested' CoreBlock)
		CoreBlock* GetNestedCoreBlockFromMem(const void* pAddress) const;

		/// Free
		/// faster version of free that can be used when it is already known
		/// what CoreBlock the memory belongs to (avoids having to lookup the CoreBlock)
		void Free(CoreBlock* pOwnerCoreBlock, void* pAddress);

		/// Verify
		// Returns 0 if OK and 1 if false. The purpose is in order to assist
		// in making functions that both assert while counting errors.
		int Verify(const EA::Allocator::SmallBlockAllocator::Pool* const pAllocator, bool bExpression,
						int id, const char* pDescription, const void* pData, const void* pLocation) const;

		/// CoreBlockDebugDataAlloc
		/// Allocates memory for debug data for the CoreBlock
		void  CoreBlockDebugDataAlloc(CoreBlock* pCoreBlock);

		/// CoreBlockDebugDataFree
		/// Frees the debug data on the CoreBlock which was allocated with CoreBlockDebugDataAlloc
		void  CoreBlockDebugDataFree(CoreBlock* pCoreBlock);

		/// ChunkDebugDataAlloc
		/// Allocates debug data for the Chunk and initializes it with nRequestedSize and allocInfo
		void  ChunkDebugDataAlloc(CoreBlock* pCoreBlock, ChunkRef pChunk, uint32_t nRequestedSize, const EA::Allocator::AllocInfo& allocInfo);

		/// ChunkDebugDataFree
		/// Frees the debug data for the specified Chunk which was allocated with ChunkDebugDataAlloc
		void  ChunkDebugDataFree(CoreBlock* pCoreBlock, ChunkRef pChunk);

	private:
		Pool& operator=(const Pool& rhs);   /// this method is not valid so it's private and undefined
		Pool();                             /// this method is not valid so it's private and undefined
		Pool(const Pool& obj);              /// this method is not valid so it's private and undefined

	private:    //Data section

		SmallBlockAllocator::Callbacks&    mCallbacks;             /// Reference to callbacks that may be shared by many or all pools
		CoreBlockList                       mCoreBlockList;         /// link list of memory CoreBlocks that have been used to populate this pool

		CoreBlock*                          mpAllocCoreBlock;       /// current CoreBlock in the list that we allocate from
		PoolDebugData*                      mpDebugData;            /// debug data for this pool

		union
		{
			struct
			{
				uint16_t                    mnGrowChunkCount;       /// how many bytes to use when we grow (valid if mCoreBlockType==kCoreBlockTypeHeap)
				uint16_t                    mnMinCoreBlockAlign;    /// min alignment of CoreBlocks            (valid if mCoreBlockType==kCoreBlockTypeHeap)
			}               mHeapParams;
			Pool*           mpCoreBlockPool;                        /// allocator to use when growing      (valid if mCoreBlockType==kCoreBlockTypePooled)
		};

		uint16_t                            mnChunkSize;            /// size of each Chunk in this pool
		int8_t                              mAutoShrink;            /// should pool shrink immediately when a CoreBlock becomes empty?
		uint8_t                             mInitialized   : 1;     /// is this pool initialized?
		uint8_t                             mCoreBlockType : 2;     /// One of CoreBlockType
		uint8_t                             mPoolStats     : 1;     /// Are pool debug stats enabled?
		uint8_t                             mChunkStats    : 1;     /// Are chunk debug stats enabled?
		uint8_t                             mUnused        : 3;     /// pad/unused
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
		inline bool       SmallBlockAllocator::Pool::Initialized() const { return mInitialized; }
		inline uint16_t   SmallBlockAllocator::Pool::GetChunkSize() const { return mnChunkSize; }
		inline bool       SmallBlockAllocator::Pool::IsFull() const { return (NULL == mpAllocCoreBlock); }
		inline bool       SmallBlockAllocator::Pool::CanShrink() const
		{
			CoreBlock* firstCoreBlock(mCoreBlockList.GetFirst());
			//CoreBlocks are sorted in order of number of free Chunks
			return ( firstCoreBlock && (firstCoreBlock->mnNumChunks == firstCoreBlock->mnNumFreeChunks) );
		}
		inline uint32_t       SmallBlockAllocator::Pool::CoreBlockOverhead()                                { return EA_OFFSETOF(CoreBlock, mData); }
		inline const uint8_t* SmallBlockAllocator::Pool::CoreBlock::Begin() const                           { return reinterpret_cast<const uint8_t*>(this); }
		inline const uint8_t* SmallBlockAllocator::Pool::CoreBlock::End() const                             { return (Begin() + mnSize); }
		inline uint8_t*       SmallBlockAllocator::Pool::CoreBlock::Begin()                                 { return reinterpret_cast<uint8_t*>(this); }
		inline uint8_t*       SmallBlockAllocator::Pool::CoreBlock::End()                                   { return (Begin() + mnSize); }
		inline bool           SmallBlockAllocator::Pool::CoreBlock::ContainsMem(const void* pAddress) const { return ((pAddress >= Begin()) && (pAddress < End())); }
		inline uint16_t       SmallBlockAllocator::Pool::CoreBlock::GetChunkSize() const                    { return mpOwner->GetChunkSize(); }
		inline uint16_t       SmallBlockAllocator::Pool::CoreBlock::GetNumFreeChunks() const                { return mnNumFreeChunks; }
		inline uint16_t       SmallBlockAllocator::Pool::CoreBlock::GetNumChunks() const                    { return mnNumChunks; }
		inline bool           SmallBlockAllocator::Pool::CoreBlock::IsEmpty() const                         { return (mnNumChunks == mnNumFreeChunks); }
		inline uint8_t*       SmallBlockAllocator::Pool::CoreBlock::GetFirstChunk()                         { return (End() - (GetNumChunks() * mpOwner->GetChunkSize())); }
		inline uint32_t       SmallBlockAllocator::Pool::CoreBlock::GetWastedSpace() const                  { return ((mnSize - (mnNumChunks * GetChunkSize())) - (CoreBlockOverhead())); }

		inline void SmallBlockAllocator::Pool::AlignedCoreBlockFree(void* pAddress,uintptr_t nCoreBlockAlignmentMask)
		{
			CoreBlock* pCurCoreBlock = reinterpret_cast<CoreBlock*>((uintptr_t)pAddress & nCoreBlockAlignmentMask);
			pCurCoreBlock->mpOwner->Free(pCurCoreBlock,pAddress);
		}

	} // namespace Allocator

} // namespace EA


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif // Header include guard

