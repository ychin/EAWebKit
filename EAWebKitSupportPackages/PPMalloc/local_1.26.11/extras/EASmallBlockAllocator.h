///////////////////////////////////////////////////////////////////////////////
// SmallBlockAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Henry Allen
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EA_SMALLBLOCKALLOCATOR_H
#define EA_SMALLBLOCKALLOCATOR_H

#include <EABase/eabase.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/shared.h>
#include "EAAllocInfo.h"

///////////////////////////////////////////////////////////////////////////////
// EA_SOA_EXTENSIONS_ENABLED (defined as 0 or 1. Default is true only for EA_DEBUG)
//
// - Enables Malloc name, file, & line tracking (and enhances memory dumps with this info)
// - Enables high water mark and high water mark stats tracking
// - Enables grow/shrink count tracking
// - Enables Auditing methods
//
#ifndef EA_SOA_EXTENSIONS_ENABLED
	#ifdef EA_DEBUG
		#define EA_SOA_EXTENSIONS_ENABLED 1
	#else
		#define EA_SOA_EXTENSIONS_ENABLED 0
	#endif
#endif
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// EA_SOA_HOOKS_ENABLED (defined as 0 or 1. Default is true only for EA_DEBUG)
//
// - Enables Hooks support which allows the user to recieve notifications
//   whenver an alloc/free/grow/shrink occurs
//
#ifndef EA_SOA_HOOKS_ENABLED
	#ifdef EA_DEBUG
		#define EA_SOA_HOOKS_ENABLED 1
	#else
		#define EA_SOA_HOOKS_ENABLED 0
	#endif
#endif
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
namespace Allocator
{

/// SmallBlockAllocator
///
/// Implements a zero external fragmentation memory heap for objects
/// below a configurable size. It is very fast compared to general
/// allocators. In terms of memory efficiency, it is extremely good for
/// small allocations. It can be configured to manage larger allocations
/// but memory efficiency is significantly worse.
///
/// This class has no internally provided thread safety. Use of this
/// allocator by multiple threads currently must be manually coordinated.
///
/// Small object allocators typically provide optimizations in the way
/// of speed and fragmentation reduction. However, there is a tradeoff
/// between these two features whereby you can improve fragmentation
/// behaviour further at the cost of some speed. SmallBlockAllocator
/// is designed to improve fragmentation more than speed where there
/// happens to be such a tradeoff.
///
/// This class does not implement allocation of memory beyond the max
/// small object size. It does not forward Malloc calls to any other
/// allocator in the case that it cannot allocate the size you request.
/// In these cases you need to wrap SmallBlockAllocator and implement
/// the error handling/allocation forwarding yourself. The reason for
/// this is that SmallBlockAllocator couldn't provide any better
/// handling than you could yourself, whereas you get more flexibility
/// if you handle it yourself.
class PPM_API SmallBlockAllocator
{
	public:

		/// Default
		/// various default values
		enum Default
		{
			kDefaultCoreBlockSize       = 2048,
			kDefaultInitCoreBlockCount  =   25,
			kDefaultGrowCoreBlockCount  =    8,
			kDefaultMinAlignment        =    8,
			kDefaultMaxAlignment        =   64,
			kDefaultMaxMallocWaste      =  256,
			kDefaultMaxMallocSize       =    0      /// 0 indicates ((mnCoreBlockSize-CoreBlockOverhead())/2), if > 0 indicates maximum allocation size to accept
		};

		/// Parameters
		/// This struct is used to configure SmallBlockAllocator.
		/// The default constructor sets the parameters to values as defined by the Default enum. These should be adequate to get users compiling and running though most users will
		/// want to tune mnInitCoreBlockCount (if nothing else) to match their circumstances.
		struct PPM_API Parameters
		{
			uint16_t    mnCoreBlockSize;            /// Size of CoreBlocks (when an individual pool grows or shrinks it does so one 'CoreBlock' at a time). Must be a multiple of mnMaxAlignment.
													/// A reasonable value would be SmallBlockAllocator::CoreBlockOverhead() + N times the max allocation size you want to manage where
													/// N is somewhere between 1-4. Larger core block sizes mean more allocations fit per core block (better memory efficiency) but it
													/// also means more memory is required to fund a pool, even a pool with a single allocation (wasting the rest of that core block until
													/// that alloc is freed or more allocations of that size come in).

			uint16_t    mnInitCoreBlockCount;       /// Initial number of CoreBlocks to allocate ... these will be allocated in one big core block allocation and the individual CoreBlocks will
													/// be divied out to the individual pools as needed.
													/// It is recommended that this value be maintained such that it's always >= the total core blocks needed in your worse-case memory
													/// usage scenario because it can have performance implications if it's smaller.
													/// Ideally mnInitCoreBlockCount will provide enough blocks that we will not need to grow much (or at all). When an allocation is freed,
													/// the core block allocation list must be searched to find the allocation that contains core block that contains the memory. Once found
													/// freeing is constant time. Therefore, if you have no grows in your main page pool you can expect freeing to be reasonably fast (and
													/// constant time) while having a great many grows can slow things down quite a bit. If you're concerned about performance, you'll want
													/// to pick a number initially then run through and get a high-watermark (from a memory dump), then update mnInitCoreBlockCount to handle
													/// your high watermark.

			uint16_t    mnGrowCoreBlockCount;       /// How many additional CoreBlocks to allocate at once (as one allocation) when the intial set has all been consumed.

			uint16_t    mnMinAlignment;             /// specifies minimum alignment to allow for allocations of size >= mnMinAlignment

			uint16_t*   mnCustomPoolSize;           /// [optional] 0 terminated list of pool sizes. mnCustomPoolSize[n] must be > mnCustomPoolSize[n-1]. The largest size must fit within
													/// a CoreBlock (with CoreBlockOverhead() to spare). Each size must be a multiple of mnMinAlignment. By default the SmallBlockAllocator
													/// will have a pool for each allocation size between mnMinAlignment & mnMaxAllocSize in mnMinAlignment increments.
													/// Ex: with mnMinAlignment == 8 and mnMaxAllocSize = 128 the default pools will be: 8, 16, 24, 32, 40, 48, 56, ... 128.
													/// Unused pools have very little overhead so the default pools should be fine for most uses. However, some users may find it useful to
													/// limit the allocator to a smaller or custom set of pool sizes that is optimal for a specific set of allocations they want to manage.

			uint16_t    mnMaxAlignment;             /// Specifies the max alignment that the system needs to support (requests for larger alignments will return NULL).  Note that the system
													/// does not currently support alignments > allocation size (i.e., a request for size=8, align=16 will fail).
													/// All pages of coreblocks (allocated through CoreAllocationFunction) will be required to be aligned to this value (thus whatever size is
													/// specified here will also be specified to the CoreAllocation callback when the system allocates the sets of core blocks).
													/// Setting this value == mnCoreBlockSize allows Free to function significantly faster (as it can use a mask to calculate the location of
													/// the core block for any allocation) but, as noted, requires all core blocks to be aligned to mnCoreBlockSize...

			uint16_t    mnMaxAllocSize;             /// maximum allocation size to support (must be <= mnCoreBlockSize-CoreBlockOverhead()), kDefaultMaxMallocSize == (mnCoreBlockSize-CoreBlockOverhead())/2.
													/// This value is ignored if mnCustomPoolSize is used.

			Parameters();
			Parameters(uint16_t nCoreBlockSize, uint16_t nInitCoreBlockCount, uint16_t nGrowCoreBlockCount, uint16_t nMinAlign = kDefaultMinAlignment, uint16_t nMaxAlign = kDefaultMaxAlignment, uint16_t nMaxAllocSize = kDefaultMaxMallocSize);
		};

		/// CoreAllocationFunction
		/// This function is used to allocate dynamic memory.
		typedef void* (*CoreAllocationFunction)(SmallBlockAllocator* pAllocator, size_t nSize, size_t nAlignment, size_t nAlignmentOffset, void* pContext);

		/// CoreFreeFunction
		/// Frees memory allocated by CoreAllocationFunction.
		typedef void (*CoreFreeFunction)(SmallBlockAllocator* pAllocator, void* pCore, void* pContext);

		/// SmallBlockAllocator
		/// Default constructs SmallBlockAllocator. This function does not initialize
		/// the class and thus needs to be followed by an Init call.
		SmallBlockAllocator();

		/// SmallBlockAllocator
		/// Construct and Init the allocator with the given parameters and callback functions.
		SmallBlockAllocator(SmallBlockAllocator::Parameters* pPoolParams, CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext = NULL);

		/// ~SmallBlockAllocator
		/// Shuts down the SmallBlockAllocator if not already shut down.
		virtual ~SmallBlockAllocator();

		/// Init
		/// Sets up a SmallBlockAllocator with a given set of parameters.
		/// You do not need to call Init if you use the non-default constructor,
		/// as it will call Init for you.
		/// CoreAllocationFunction and CoreFreeFunction must be non-NULL, the will
		/// be used to manage memory for the pool array, index table, and other
		/// overhead required by the system.
		/// pAllocationDebugContext will be passed to the allocation and free
		/// methods when managing debug info allocations (for file, line, name,
		/// tracking, etc). This allows the user to recognize these non-ship
		/// type allocations and manage them separately from 'normal' allocations
		void Init(SmallBlockAllocator::Parameters* pPoolParams, CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext = NULL);

		/// Shutdown
		/// Returns a SmallBlockAllocator to essentially the state it was in prior to Init.
		virtual void Shutdown();

		/// IsInitialized
		/// Returns true if Init has been called.
		bool IsInitialized() const;

		/// Malloc
		/// Returns a Chunk of the given size and of kMinAlignment.
		/// Returns NULL if memory is exhausted or out of the supported range specified
		/// by the configuration Parameters.
		/// This function is very fast for the case where there is free memory in the
		/// memory pools for the given size.
		void* Malloc(size_t nSize, const EA::Allocator::AllocInfo& allocInfo = EA::Allocator::AllocInfo::NULLREF);

		/// Malloc
		/// Returns a Chunk that is of the given size and alignment.
		/// Returns NULL if memory is exhausted or out of the supported range specified
		/// by the configuration Parameters.
		/// This function is slower than the version that doesn't support alignment
		void* Malloc(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, const EA::Allocator::AllocInfo& allocInfo = EA::Allocator::AllocInfo::NULLREF);

		inline void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, const EA::Allocator::AllocInfo& allocInfo = EA::Allocator::AllocInfo::NULLREF)
			{ return Malloc(nSize, nAlignment, nAlignmentOffset, allocInfo); }

		/// Realloc
		/// Reallocates the memory pAddress to the new size nNewSize.
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
		///     NULL      0      NULL      allocation of zero size Chunk failed.
		///     NULL      0      valid     allocation of zero size Chunk succeeded.
		///     valid     0      NULL      allocation of zero size Chunk succeeded, original pointer not freed.
		///     valid     0      valid     allocation of zero size Chunk succeeded, original pointer freed.
		void* Realloc(void* pAddress, size_t nNewSize, size_t nOldSize = 0);

		/// FreeSafe
		/// Returns false if the memory is not owned by this heap, else frees the memory and returns true
		/// nSize is ignored currently
		bool  FreeSafe(void* pAddress, size_t nSize = 0);

		/// Free
		/// Behavior is undefined if the memory is not owned by this heap!
		/// else frees the memory and returns true
		/// nSize is ignored currently
		void  Free(void* pAddress, size_t nSize = 0);

		/// GetUsableSize
		/// Report the number of usable bytes associated with pAddress. Thus if the user allocates
		/// 30 bytes it is possible that the Chunk returned to the user actually has 40 bytes of
		/// usable space. The input pointer must be valid or the return value is (size_t)-1.
		/// This function is not necessarily fast and should not be frequently called in
		/// code that needs to be fast. Note that if you use page-aligned/sized core blocks
		/// then this function can in fact be fairly fast.
		size_t GetUsableSize(const void* pAddress) const;

		/// GetAllocationCost
		/// This method is for debugging/statistics gathering only to see how much memory allocations cost (the size returned is NOT useable size)
		/// See AllocationCost for details on the costs that can be calculated.
		/// If the address does not belong to this allocator (or no flags are passed in) then 0.0f is returned.
		enum AllocationCost
		{
			AllocationCostNone          = 0,
			AllocationCostChunkSize     = 1,    //Size of the chunk of memory that was used to fund the allocation. May be bigger than usable size if the address isn't pointing to the start of a chunk (due to alignment).
			AllocationCostHeader        = 2,    //This chunks share of the header cost for this CoreBlock. E.g., if there are 42 Chunks in this CoreBlock then this value will equal: CoreBlockOverhead()/42.
			AllocationCostWaste         = 4     //This chunks share of any left-over space in the CoreBlock. Example: CoreBlockSize=4096, MinAlign=4, ChunkSize=12 fits 338 Chunks with 8 bytes left-over: 8/338 = 0.02.
		};
		float GetAllocationCost(const void* pAddress, int nAllocationCostFlags) const;

		/// GetMaxChunkSize
		/// Returns the maximum allocation size request that can be handled by this allocator
		uint32_t GetMaxChunkSize() const;

		/// Runtime Options
		enum Option
		{
			kOptionNone                 = 0,
			kOptionAutoShrink           = 1,    /// [Default: 1] If non-zero, specifies that any time a pool has more than one free CoreBlock the extra(s) will be freed. This option can be set at any time.
			kOptionEnableExtensions     = 2,    /// [Default: kExtensionNone] Extensions to enable, allows enabling of Auditing, Grow/Shrink counting, high water mark tracking, and per allocation file, line, & name storage. This option can be set at any time.
			kOptionMaxAllocWaste        = 3,    /// [Default: kDefaultMaxMallocWaste] Maximum waste allowed due to user size not exactly equaling an available Chunk size. Only change this value if it's ok for allocations to 'fail' because they would waste too much memory.
			kOptionGrowCoreBlockCount   = 4     /// [Default: Parameters.mnGrowCoreBlockCount] Allows you to alter the number of blocks to grow by when the pools need to grow.
		};

		enum Extension
		{
			kExtensionNone                  =      0,
			kExtensionAll                   = 1 << 0,   /// 1 is used as a special case for 'All' for backwards compatibilty
			kExtensionPoolStats             = 1 << 1,   /// This makes per pool debug data available: High Watermark stats, num grows and num shrinks.
			kExtensionAllocStats            = 1 << 2,   /// Enables per allocation tracking of file, line, alloc name and requested size.
			kExtensionAuditing              = 1 << 3,   /// Enables auditing support (Audit, SetGroupId, and GetGroupId). NOTE: currently this option implicitly enables both PoolStats & AllocStats but in the future it may be optimized to be independent if desired.
			kExtensionRestrictToPagePool    = 1 << 4    /// Whatever extension is being enabled should only be enabled for the page pool, not all of the individual pools
		};


		/// SetOption
		/// The user specifies one of enum Option. Note that the argument 'option' is
		/// of type int instead of type Option because subclasses may want to extend
		/// the option set to include values not listed here.
		void SetOption(int nOption, int nValue);

		/// ShrinkPools
		/// Iterates through all pools and frees any currently un-used (cached) CoreBlocks
		void ShrinkPools();

		typedef const uint8_t* PoolRef;         /// Pointer to a pool
		typedef const uint8_t* CoreBlockRef;    /// Pointer to a CoreBlock
		typedef const uint8_t* ChunkRef;        /// Pointer to a Chunk

		/// NodeType
		/// Used in memory reports
		enum NodeType
		{
			kNodeTypeInvalid            = 0,    /// The node information is invalid
			kNodeTypePool               = 1,    /// The node contains information about a pool
			kNodeTypeCoreBlock          = 2,    /// The node contains information about a CoreBlock
			kNodeTypeChunkAllocated     = 4,    /// The node contains information about an allocated Chunk
			kNodeTypeChunkFree          = 8,    /// The node contains information about a free Chunk
			kNodeTypeAll                = 31    /// This is a flag that includes all other flags.
		};

		/// PoolInfo
		/// Information and stats for a pool
		struct PPM_API PoolInfo
		{
			uint32_t    CoreBlockCount() const;                       /// total number of CoreBlocks assigned to this pool
			uint32_t    CoreBlockSizeTotal() const;                   /// total size of all CoreBlocks assigned to this pool
			uint32_t    CoreBlockWasteTotal() const;                  /// waste occurs due to Chunks not dividing evenly into CoreBlocks
			uint32_t    CoreBlockHeadersTotal() const;                /// total size of all CoreBlock headers
			uint32_t    CoreBlockOverheadTotal() const;               /// total overhead for headers and waste in all CoreBlocks
			uint32_t    CoreBlockEmptyCount() const;                   /// How many of CoreBlockCount are actually empty (this happens if AutoShrink is not enabled, the pool will hold empty blocks for future use until Shrink is called)

			uint32_t    ChunkSize() const;                            /// size of Chunks that this pool manages
			uint32_t    ChunkCount() const;                           /// number of Chunks currently managed by this pool
			uint32_t    ChunkSizeTotal() const;                       /// total size of Chunks managed by this pool
			uint32_t    FreeChunkCount() const;                       /// number of Chunks currently available in this pool
			uint32_t    FreeChunkSizeTotal() const;                   /// total size of currently available Chunks in this pool

			float       PerChunkCoreBlockWaste() const;               ///
			uint32_t    TotalOverhead() const;                        ///
			float       PerChunkOverhead() const;                     ///
			float       PerAllocatedChunkOverheadPlusFree() const;    /// overhead and unused space divided by currently allocated Chunk count
			uint32_t    NumGrows() const;                             /// returns static_cast<uint32_t>(-1) if not available (if !EA_SOA_EXTENSIONS_ENABLED or kOptionEnableExtensions has not been set to kExtensionPoolStats or All)
			uint32_t    NumShrinks() const;                           /// returns static_cast<uint32_t>(-1) if not available (if !EA_SOA_EXTENSIONS_ENABLED or kOptionEnableExtensions has not been set to kExtensionPoolStats or All)


			PoolRef         mPool;
			uint32_t        mnCoreBlockCount;
			uint32_t        mnCoreBlockEmptyCount;
			uint32_t        mnChunkCount;
			uint32_t        mnFreeChunkCount;
			uint32_t        mnChunkSize;
			uint32_t        mnCoreBlockWasteTotal;
			uint32_t        mnCoreBlockSizeTotal;
			uint32_t        mnNumGrows;
			uint32_t        mnNumShrinks;
		};

		/// CoreBlockInfo
		/// Information about a specific CoreBlock
		struct PPM_API CoreBlockInfo
		{
			uintptr_t Begin() const;            /// address of the first byte of memory in the CoreBlock
			uintptr_t End() const;              /// address of the byte just beyond the end of the CoreBlock of memory

			CoreBlockRef    mCoreBlock;         /// CoreBlock which mData belongs to.
			uint32_t        mnSize;
			uint32_t        mnChunkSize;
			uint32_t        mnNumChunks;
			uint32_t        mnNumFreeChunks;
		};

		/// ChunkInfo
		/// Information about a specific Chunk
		struct PPM_API ChunkInfo
		{
			CoreBlockRef    mCoreBlock;      /// CoreBlock which mData belongs to.
			ChunkRef        mChunk;          /// The user data within this CoreBlock.

			uint32_t        mnCoreBlockSize;     /// The size of the CoreBlock (including its user data).
			uint32_t        mnChunkSize;     /// The size of the Chunk (>= user requested size).
			uint32_t        mnRequestedSize; /// The size the user actually requsted for this Chunk
			int32_t         mnGroupId;       /// groupId for this Chunk if extensions are enabled, else 0

			const char*     mpAllocName;     /// name associated with this allocation (or NULL if not available)
			const char*     mpAllocFile;     /// file the Chunk was allocated from (or NULL if not available)
			uint32_t        mnAllocLine;     /// line number the Chunk was allocated from
		};

		/// ReportNode
		/// Used by memory reporting methods. Contains information about one pool, CoreBlock,
		/// or Chunk as indicated by mNodeType
		struct PPM_API ReportNode
		{
			union
			{
				PoolInfo        mPoolInfo;      /// Valid if mNodeType == kNodeTypePool
				CoreBlockInfo   mCoreBlockInfo; /// Valid if mNodeType == kNodeTypeCoreBlock
				ChunkInfo       mChunkInfo;     /// Valid if mNodeType == kNodeTypeChunkAllocated || kNodeTypeChunkFree
			};
			uint32_t      mNodeType;            /// One of enum NodeType
		};

		/// ReportContext
		/// Data needed by ReportBegin/ReportNext/ReportEnd to track current report state
		struct PPM_API ReportContext
		{
			PoolRef         mPool;              /// Current Pool
			CoreBlockRef    mCoreBlock;         /// Current CoreBlock
			ChunkRef        mChunk;             /// Current Chunk
			uint32_t        mnPoolIndex;        /// Index of current pool
			uint32_t        mNodeTypeFlags;     /// Global Chunk type flags filter to apply. See enum NodeType.
			ReportNode      mNode;              /// Information about the currently iterated node.
		} PPM_MAY_ALIAS;

		/// ReportBegin
		/// Call to initialize a ReportContext before calling ReportNext. When done, call ReportEnd.
		ReportContext* ReportBegin(ReportContext* context, uint32_t nodeTypeFlags = kNodeTypeAll);

		/// ReportNext
		/// Used to walk the nodes of a report (returns the next node). Nodes may include pools,
		/// allocated Chunks, free Chunks, etc as specified by nNodeTypeFlags.
		/// Returns NULL when there are no more nodes to traverse.
		const ReportNode* ReportNext(ReportContext* context, uint32_t nodeTypeFlags = kNodeTypeAll);

		/// ReportEnd
		/// Call to deinitialize ReportContext when done calling report next.
		void ReportEnd(ReportContext* context);

		/// SetGroupId
		/// Sets an Id that will be stored with each subsequent allocation (until a new Id is specified).
		/// Use in conjunction with Audit to identify leaks.
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is set to kExtensionAuditing (or all).
		int32_t SetGroupId(int32_t groupId);

		/// GetGroupId
		/// Returns the value passed to the most recent call to SetGroupId
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is set to kExtensionAuditing (or all). 
		int32_t GetGroupId() const;

		/// Audit
		/// Calls pAuditCallback for every currently allocated Chunk that has the specified GroupId.
		/// Functions only if EA_SOA_EXTENSIONS_ENABLED and the option kOptionEnableExtensions is set to kExtensionAuditing (or all). 
		/// Returns the number of Chunks found
		typedef void (*AuditFunction)(const ChunkInfo* pChunk, void* pContext);
		uint32_t Audit(int32_t groupId, AuditFunction pCallback, void* pCallbackContext);

		/// HookType
		/// Enumerates different hooks that are supported
		enum HookType
		{
			kHookTypeMalloc,            /// Malloc was called.
			kHookTypeFree,              /// Free was called.
			kHookTypeGrowHeap,          /// Grow acquired memory using the CoreAllocationFunction.
			kHookTypeGrowHeapDisabled,  /// Grow wants to acquire memory but allocator is not allowed to grow (allocs will overflow).
			kHookTypeShrinkHeap,        /// Shrink freed memory using CoreFreeFunction.
			kHookTypeGrowPooled,        /// Grow, satisfied using memory from a page pool within this system.
			kHookTypeShrinkPooled       /// Shrink, returned memory to a page pool within this system.
		};

		/// HookInfo
		/// describes an allocation or grow result
		struct PPM_API HookInfo
		{
			HookType    mType;      /// One of HookType
			size_t      mnSize;     /// Size of the CoreBlock or Chunk being allocated or freed

			HookInfo(HookType Type, size_t nSize);
		};

		/// SetHookFunction
		/// Lets you specify a function that will be called when an allocation is made, a free is called,
		/// and when grows & shrinks occur
		typedef void (*HookFunction)(const HookInfo* pHookInfo, void* pContext);
		void SetHookFunction(HookFunction pHookFunction, void* pContext);

		/// SetTraceFunction
		/// Allows you to override where traces go. If not specified, traces are sent via
		/// printf to 'stdout'. Strings passed to the TraceFunction will have trailing newlines.
		typedef void (*TraceFunction)(const char* pTraceText, void* pContext);
		void SetTraceFunction(TraceFunction pTraceFunction, void* pContext);
		class PPM_API TraceCallback
		{
			public:
				TraceCallback();
				TraceCallback(TraceFunction pTraceFunction, void* pContext);
				~TraceCallback();
				void SetTraceFunction(TraceFunction pTraceFunction, void* pContext);
				void Trace(const char* pTraceText);
				int32_t Tracef(const char *fmt, ...);

			private:
				TraceFunction                   mpTrace;
				void*                           mpTraceContext;
		};

		/// DumpPools
		/// This method is being retained for now for backwards compatibility with CMemPool
		/// prints stats about the contained mempools to the callBack
		///     kNodeTypeCoreBlock will include stats on each core block
		///     kNodeTypeChunkAllocated will includes a complete memory dump 
		///      (with per allocation file/line/name if EA_SOA_EXTENSIONS_ENABLED AND kOptionEnableExtensions has been set to to kExtensionAllocStats or all). 
		void DumpPools(TraceCallback* pTraceCallback = NULL, uint32_t nodeTypeFlags = (kNodeTypeChunkAllocated | kNodeTypeCoreBlock));

		/// AssertionFailureInfo
		/// Struct passed to user upon assertion failure.
		struct PPM_API AssertionFailureInfo
		{
			const SmallBlockAllocator*  mpAllocator;          /// The allocator the assertion is associated with. May be NULL for failures that are without allocator context.
			const char*                 mpPool;               /// pool if any
			const char*                 mpExpression;         /// The assertion expression that failed.
			const char*                 mpDescription;        /// A description of the assertion failure.
			const void*                 mpData;               /// The user allocation the assertion is associated with. May be NULL for failures that are without allocation context.
			const void*                 mpExtra;              /// Other information related to the failure. The meaning depends on the failure id.
			int                         mnErrorId;            /// One of enum GAError or a user-defined or subclass-defined id.
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
		void AssertionFailure(const char* pExpression, int nErrorId /*= kGAErrorUnknown*/, const char* pDescription = NULL,
							  const void* pData = NULL, const void* pExtra = NULL) const;

		/// AddressType
		/// Used to categorize pointers for the ValidateAddress function.
		enum AddressType
		{
			kAddressTypeOwned     = -1, /// The memory referred to by the pointer is owned by the given allocator. It may be allocated or free.
			kAddressTypeAllocated =  0, /// The memory referred to by the pointer is both owned by the given allocator and refers to valid user-allocated memory, but may not be the allocated pointer itself but merely with the allocated Chunk.
			kAddressTypeSpecific  =  1  /// The memory referred to by the pointer is both owned by the given allocator, refers to valid user-allocated memory, and is also the allocated pointer itself and not merely any memory within the allocated Chunk.
		};

		/// ValidateAddress
		/// Tells you if the given address is valid.
		const void* ValidateAddress(const void* pAddress, int addressType = kAddressTypeAllocated) const;

		/// Validate
		/// Checks member variables and all pools for corruption (returns false if corrupted)
		/// if pErrorCount is supplied, it will be incremented by the number of errors found
		bool Validate(int* pErrorCount = NULL) const;

		/// Callbacks
		/// stores various callbacks and helper methods for calling them
		/// this is public so that it can be shared with the individual pools
		class PPM_API Callbacks
		{
			public:
				Callbacks(SmallBlockAllocator* poolMgr);
				~Callbacks();

				void SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext);
				void AssertionFailure(AssertionFailureInfo* pAssertionFailureInfo) const;

				void SetAllocationFunctions(CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext);
				void* CoreAlloc(size_t nSize, size_t nAlignment = 0, size_t nAlignmentOffset = 0);
				void  CoreFree(void* pCore);
				void* DebugAlloc(size_t nSize, size_t nAlignment = 0, size_t nAlignmentOffset = 0);
				void  DebugFree(void* pCore);

				void SetTraceFunction(TraceFunction pTraceFunction, void* pContext);

				void SetHookFunction(HookFunction pHookFunction, void* pContext);
				void Hook(const HookInfo* pHookInfo);

				void FreeCachedChunks();

				SmallBlockAllocator*            mpSmallBlockAllocator;
				TraceCallback                   mTrace;

				CoreAllocationFunction          mpAlloc;
				CoreFreeFunction                mpFree;
				void*                           mpAllocationContext;
				void*                           mpAllocationDebugContext;

				AssertionFailureInfoFunction    mpAssertionFailure;
				void*                           mpAssertionFailureContext;

				HookFunction                    mpHookFunction;
				void*                           mpHookFunctionContext;
		};

		/// CoreBlockOverhead
		/// Returns size of per CoreBlock overhead (the header)
		static uint32_t CoreBlockOverhead();

		/// PoolOverhead
		/// Returns size of per pool overhead
		static uint32_t PoolOverhead();

	private:
		/// operator =
		/// this method is not valid so it's private and undefined
		SmallBlockAllocator& operator=(const SmallBlockAllocator& rhs);
		/// copy constructor
		/// this method is not valid so it's private and undefined
		SmallBlockAllocator(const SmallBlockAllocator& obj);

	protected:
		class Pool;

		/// AssertionFailureFunctionDefault
		/// Default assert implementation
		static void  AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);

		/// GetMaxAlignment
		/// Returns nDesiredMaxAlignment if it can be supported by nCoreBlockSize, else returns
		/// max alignment possible with nCoreBlockSize
		uint16_t GetMaxAlignment(uint16_t nCoreBlockSize, uint16_t nDesiredMaxAlignment);

		/// Verify
		// Returns 0 if OK and 1 if false. The purpose is in order to assist
		// in making functions that both assert while counting errors.
		int Verify(const EA::Allocator::SmallBlockAllocator* const pAllocator, bool bExpression,
						int id, const char* pDescription, const void* pData, const void* pLocation) const;

	protected:
		Callbacks    mCallbacks;                 /// Stores various callback data so it can be shared with pools
		Pool*        mPagePool;                  /// Pool which other pools will use as a CoreBlock allocator
		Pool*        mPools;                     /// Array of pools for each supported allocation size
		uint32_t     mnPoolCount;                /// number of pools in mPools array
		uintptr_t    mnCoreBlockAlignmentMask;   /// mask used to convert an address to a CoreBlock pointer

		uint16_t     mnMaxChunkSize;             /// maximum allocation size supported
		uint16_t     mnMaxAlignment;             /// maximum allocation alignment supported
		uint16_t     mnMaxWastePerChunk;         /// maximum wasted space per allocation
		uint16_t     mnPoolIncrementSize;        /// allocation size increment between pools (granularity between pool sizes). Also min alignment of allocations.

		uint16_t*    mPoolRemap;                 /// remaps calculated index (based on size & mnPoolIncrementSize) to actual pool index
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
		////////////////////////////////////
		// SmallBlockAllocator::PoolInfo  //
		////////////////////////////////////

		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockCount() const { return mnCoreBlockCount; }
		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockSizeTotal() const { return mnCoreBlockSizeTotal; }
		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockWasteTotal() const { return mnCoreBlockWasteTotal; }
		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockHeadersTotal() const { return ((CoreBlockOverhead()*mnCoreBlockCount)); }
		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockOverheadTotal() const { return (CoreBlockWasteTotal() + CoreBlockHeadersTotal()); }
		inline uint32_t SmallBlockAllocator::PoolInfo::CoreBlockEmptyCount() const { return mnCoreBlockEmptyCount; }
		inline uint32_t SmallBlockAllocator::PoolInfo::ChunkSize() const { return mnChunkSize; }
		inline uint32_t SmallBlockAllocator::PoolInfo::ChunkCount() const { return mnChunkCount; }
		inline uint32_t SmallBlockAllocator::PoolInfo::ChunkSizeTotal() const { return mnChunkSize*mnChunkCount; }
		inline uint32_t SmallBlockAllocator::PoolInfo::FreeChunkCount() const { return mnFreeChunkCount; }
		inline uint32_t SmallBlockAllocator::PoolInfo::FreeChunkSizeTotal() const { return mnChunkSize*mnFreeChunkCount; }
		inline float    SmallBlockAllocator::PoolInfo::PerChunkCoreBlockWaste() const { return (mnChunkCount ? (static_cast<float>(CoreBlockWasteTotal())/static_cast<float>(mnChunkCount)) : 0.0f); }
		inline uint32_t SmallBlockAllocator::PoolInfo::TotalOverhead() const { return (CoreBlockOverheadTotal() + PoolOverhead()); }
		inline float    SmallBlockAllocator::PoolInfo::PerChunkOverhead() const { return (mnChunkCount ? (static_cast<float>(TotalOverhead())/static_cast<float>(mnChunkCount)) : 0.0f); }
		inline float    SmallBlockAllocator::PoolInfo::PerAllocatedChunkOverheadPlusFree() const
		{
			const uint32_t nAllocatedChunkCount(mnChunkCount-mnFreeChunkCount);
			const float   overheadPlusFreeSize(static_cast<float>(TotalOverhead() + FreeChunkSizeTotal()));
			return ( nAllocatedChunkCount ? (overheadPlusFreeSize/static_cast<float>(nAllocatedChunkCount)) : overheadPlusFreeSize);
		}
		inline uint32_t SmallBlockAllocator::PoolInfo::NumGrows() const { return mnNumGrows; }       /// returns static_cast<uint32_t>(-1) if not available (if !EA_SOA_EXTENSIONS_ENABLED or kOptionEnableExtensions has not been set to kExtensionPoolStats or All)
		inline uint32_t SmallBlockAllocator::PoolInfo::NumShrinks() const { return mnNumShrinks; }   /// returns static_cast<uint32_t>(-1) if not available (if !EA_SOA_EXTENSIONS_ENABLED or kOptionEnableExtensions has not been set to kExtensionPoolStats or All) 


		/////////////////////////////////////////
		// SmallBlockAllocator::CoreBlockInfo  //
		/////////////////////////////////////////

		inline uintptr_t SmallBlockAllocator::CoreBlockInfo::Begin() const { return reinterpret_cast<uintptr_t>(mCoreBlock); }
		inline uintptr_t SmallBlockAllocator::CoreBlockInfo::End() const { return (Begin() + mnSize); }


		/////////////////////////////////////////
		// SmallBlockAllocator::TraceCallback  //
		/////////////////////////////////////////

		inline void SmallBlockAllocator::TraceCallback::SetTraceFunction(TraceFunction pTraceFunction, void* pContext)
		{
			mpTrace = pTraceFunction;
			mpTraceContext = pContext;
		}
		inline void SmallBlockAllocator::TraceCallback::Trace(const char* pTraceText) { mpTrace(pTraceText,mpTraceContext); }


		////////////////////////////////////
		// SmallBlockAllocator::Callbacks //
		////////////////////////////////////

		inline void  SmallBlockAllocator::Callbacks::SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
		{
			mpAssertionFailure = pAssertionFailureFunction;
			mpAssertionFailureContext = pContext;
		}
		inline void  SmallBlockAllocator::Callbacks::AssertionFailure(AssertionFailureInfo* pAssertionFailureInfo) const
		{
			pAssertionFailureInfo->mpAllocator = mpSmallBlockAllocator;
			mpAssertionFailure(pAssertionFailureInfo, mpAssertionFailureContext);
		}
		inline void  SmallBlockAllocator::Callbacks::SetAllocationFunctions(CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext)
		{
			mpAlloc = pAllocationFunction;
			mpFree = pFreeFunction;
			mpAllocationContext = pAllocationContext;
			mpAllocationDebugContext = pAllocationDebugContext;
		}
		inline void* SmallBlockAllocator::Callbacks::CoreAlloc(size_t nSize, size_t nAlignment, size_t nAlignmentOffset) { return mpAlloc(mpSmallBlockAllocator, nSize, nAlignment, nAlignmentOffset, mpAllocationContext); }
		inline void  SmallBlockAllocator::Callbacks::CoreFree(void* pCore) { mpFree(mpSmallBlockAllocator, pCore, mpAllocationContext); }
		inline void* SmallBlockAllocator::Callbacks::DebugAlloc(size_t nSize, size_t nAlignment, size_t nAlignmentOffset) { return mpAlloc(mpSmallBlockAllocator, nSize, nAlignment, nAlignmentOffset, mpAllocationDebugContext); }
		inline void  SmallBlockAllocator::Callbacks::DebugFree(void* pCore) { mpFree(mpSmallBlockAllocator, pCore, mpAllocationDebugContext); }
		inline void  SmallBlockAllocator::Callbacks::SetTraceFunction(TraceFunction pTraceFunction, void* pContext) { mTrace.SetTraceFunction(pTraceFunction,pContext); }
		inline void  SmallBlockAllocator::Callbacks::SetHookFunction(HookFunction pHookFunction, void* pContext) { mpHookFunction = pHookFunction; mpHookFunctionContext = pContext; }
		inline void  SmallBlockAllocator::Callbacks::Hook(const HookInfo* pHookInfo) { if ( mpHookFunction ) mpHookFunction(pHookInfo,mpHookFunctionContext); }
		inline void  SmallBlockAllocator::Callbacks::FreeCachedChunks() { mpSmallBlockAllocator->ShrinkPools(); }


		////////////////////////////////////
		// SmallBlockAllocator            //
		////////////////////////////////////

		inline bool     SmallBlockAllocator::IsInitialized() const { return (mnPoolCount > 0); }
		inline uint32_t SmallBlockAllocator::GetMaxChunkSize() const { return mnMaxChunkSize; }
		inline void     SmallBlockAllocator::SetHookFunction(HookFunction pHookFunction, void* pContext) { mCallbacks.SetHookFunction(pHookFunction,pContext); }

	} // namespace Allocator

} // namespace EA

#endif    // EA_SMALLOBJECTALLOCATOR_H

