///////////////////////////////////////////////////////////////////////////////
// EAGeneralAllocator (a.k.a. PPMalloc) 
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This code is based on the venerable dlmalloc algorithm. This allocator 
// is very fast, yet also very efficient with memory and tends to reduce 
// fragmentation fairly well for a general allocator. It has been studied
// by a number of researchers and has proven successful across a fairly 
// wide range of platforms. For more about dlmalloc see:
//     http://gee.cs.oswego.edu/dl/html/malloc.html
//
// Supported user-modifiable defines
//     PPM_DEBUG
//     PPM_VIRTUAL_ENABLED
//     PPM_HOOKS_SUPPORTED
//     PPM_THREAD_SAFETY_SUPPORTED
//     PPM_THREAD_SAFETY_BY_DEFAULT
//     PPM_INTERRUPT_DISABLING_ENABLED
//     PPM_ASSERT_ENABLED
//     PPM_DEBUG_FILL
//     PPM_DEBUG_PRESERVE_PRIOR
//     PPM_AUTO_HEAP_VALIDATION_SUPPORTED
//     PPM_FASTBIN_TRIM_ENABLED
//     PPM_INTERNAL_CORE_FREE_ENABLED
//     PPM_MALLOC_AS_COREALLOC
//     PPM_MALLOC_AS_MMAPALLOC
//     PPM_MMAP_SUPPORTED
//     PPM_MIN_ALIGN
//     PPM_NEW_CORE_SIZE_DEFAULT
//     PPM_CORE_INCREMENT_SIZE_DEFAULT
//     PPM_NULL_POINTER_FREE_ENABLED
//     PPM_REALLOC_C99_ENABLED
//     PPM_TRACE_BUFFER_SIZE
//     
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EAGENERALALLOCATOR_H
#define PPMALLOC_EAGENERALALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/shared.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/mutex.h>
#include <assert.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class GeneralAllocator
		/// 
		/// This class implements a generic memory allocator. Its design is based on the 
		/// venerable dlmalloc but extends dlmalloc in a number of ways and cleans up
		/// the dlmalloc interface and implementation. Most significantly, PPMalloc adds
		/// memory tracking functionality to dlmalloc and significantly increases the 
		/// debugging functionality. The general algorithm is mostly the same as dlmalloc.
		/// 
		class PPM_API GeneralAllocator : public IAllocAssertFunc
		{ 
		public:
			/// kTypeId
			/// Defines what type of allocator this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class 
			/// such as GeneralAllocator to a subclass such as GeneralAllocatorDebug.
			static const int kTypeId = 0x34ab224b; // This is just some unique value.

			/// AllocationFlags
			/// These are values that can be or'd together and passed to allocation 
			/// functions to specify hints or directives regarding the allocation request.
			enum AllocationFlags
			{
				kAllocationFlagNone          = 0x00000000,      /// Shorthand meaning 'no flags'.
				kAllocationFlagLow           = 0x00000000,      /// Low is default.
				kAllocationFlagHigh          = 0x00000001,      /// Try to allocate the requested block from high memory. This allocator is optimized for the case of high memory blocks being long-lasting or permanent. kOptionEnableHighAllocation must be enabled for high allocation to work.
				kAllocationFlagPerm          = 0x00000001,      /// Same as kAllocationFlagHigh.
				kAllocationFlagEndFit        = 0x00000002,      /// For high memory allocations, make an extra effort to find a block that comes from the highest part of memory, even if other blocks lower in high memory suffice. For low memory allocations, try to find the lowest block. This option slows allocation by some amount but yields better high memory packing.
				kAllocationFlagBestFit       = 0x00000004,      /// Causes the allocation mechanism to sacifice speed for a fit that is less fragmenting.
				kAllocationFlagMMap          = 0x00000008,      /// Request mapped memory instead of normal heap memory. If mapped memory is not available, the allocation does not attempt to allocate via heap memory. On systems without mapped memory, this flag will be ignored and allocation will proceed as if it was never specified.
				kAllocationFlagReserved      = 0xff000000,      /// Reserved for internal usage.
				kAllocationFlagRetry         = 0x40000000,      /// The allocation request is an internal retry.
				kAllocationFlagInternal      = 0x80000000,      /// The allocation request is internally derived. This does not cause Malloc to set the block's internal flag; the internal caller must decide if that's the right thing to do. 
				kAllocationFlagAvoidTopChunk = 0x00000100,      /// 
				kAllocationFlagUseTopChunk   = 0x00000200,      /// 
				kAllocationFlagAlwaysAlign   = 0x00000400       /// 
			};

			/// Alignment
			/// Specifies constants related to alignment properties.
			enum Alignment
			{
				kAlignmentMin       = PPM_MIN_ALIGN,    /// Minimum alignment of allocated memory.
				kAlignmentMax       = 65536,            /// Maximum supported alignment of allocated memory.
				kAlignmentOffsetMin = PPM_MIN_ALIGN,    /// Minimum supported offset of allocated aligned memory.
				kAlignmentOffsetMax = 65536             /// Maximum supported offset of allocated aligned memory.
			};

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

			/// CoreFreeFunction
			/// This is the type of function you must use to pass core freeing 
			/// callbacks to GeneralAllocator. Returns the amount of bytes freed,
			/// which should be zero or >= the same as input nSize. If the CoreFreeFunction
			/// returns zero, GeneralAllocator nevertheless assumes that the memory is 
			/// henceforth unusable.
			typedef size_t (*CoreFreeFunction)(GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, void* pContext);

			/// CoreExtendFunction
			/// Allows the user to extend the given existing core memory, if possible.
			/// The CoreExtendFunction is called when a given core block is exhausted but
			/// it can be extended in-place, which is possible with some platform memory managers.
			/// The CoreExtendFunction is tied to a given core block and is specified at the 
			/// same time the core block is requested (e.g. in AddCore).
			/// This function is not used to allocate new core; that's for the CoreAddFunction.
			/// A return value of 0 means the memory could not be extended.
			/// Otherwise the return value is size of the newly extended memory,
			/// which is presumed to begin at pCore + nSize.
			/// We don't call this CoreReallocFunction because realloc implies that a new 
			/// pointer could be made, whereas extension is strictly the in-place resizing 
			/// that realloc supports.
			typedef size_t (*CoreExtendFunction)(GeneralAllocator* pGeneralAllocator, void* pCore, size_t nSize, size_t nMinExtendedSize, size_t nReservedSize, void* pContext);


			/// Parameters
			/// This struct is used to configure EAGeneralAllocator.
			/// Default values can be viewed in the construction implementation.
			struct PPM_API Parameters
			{
				void*              mpCore;
				void*              mpCoreFreeContext;
				void*              mpCoreExtendContext;
				size_t             mnCoreSize;
				size_t             mnReservedSize;
				bool               mbShouldFreeCore;
				bool               mbShouldTrimCore;
				CoreFreeFunction   mpCoreFreeFunction;
				CoreExtendFunction mpCoreExtendFunction;
 
				Parameters();                                   /// Constructs with default parameters.
				Parameters(void* pCore, size_t nCoreSize, size_t nReservedSize, bool bShouldFreeCore, bool bShouldTrimCore, 
						   CoreFreeFunction pCoreFreeFunction, void* pCoreFreeContext, CoreExtendFunction pCoreExtendFunction, void* pCoreExtendContext);
			};


			/// GeneralAllocator
			/// Constructs an instance of an allocator.
			/// Parameters act the same way as with the Init function. If values are zero, 
			/// then no core is allocated and you must either manually Init with core or
			/// call AddCore. If you don't add core yourself, GeneralAllocator will attempt
			/// to add it from the system store upon first allocation. If this constructor 
			/// is called with initial core supplied or requested, Init is not required to 
			/// be called after construction.
			GeneralAllocator(void* pInitialCore = NULL, size_t nInitialCoreSize = 0,
							 bool bShouldFreeInitialCore = true, bool bShouldTrimInitialCore = false, 
							 CoreFreeFunction pInitialCoreFreeFunction = NULL, void* pInitialCoreFreeFunctionContext = NULL,
							 CoreExtendFunction pInitialCoreExtendFunction = NULL, void* pInitialCoreExtendFunctionContext = NULL);

			/// Parameters structure overload
			GeneralAllocator(const Parameters& parameters);


			/// ~GeneralAllocator
			/// Destroys the allocator instance. Calls Shutdown if not called already.
			PPM_VIRTUAL ~GeneralAllocator();


			/// IsConstructed
			/// Returns true if the allocator is constructed and ready for use.
			/// The user need not call the Init function to use an instance of this class.
			/// This function works only in the case that the GeneralAllocator memory is 
			/// known to be zeroed before construction, as with a global variable.
			bool IsConstructed() const;


			/// Init
			/// Initializes the allocator. 
			/// The pInitialCore and nInitialCoreSize arguments are used as in the AddCore function.
			/// Init is not required if the GeneralAllocator constructor was called with initial core.
			/// However, any subsequent Shutdown calls require a call to Init to re-initialize.
			PPM_VIRTUAL bool Init(void* pInitialCore = NULL, size_t nInitialCoreSize = 0, bool bShouldFreeInitialCore = true, bool bShouldTrimInitialCore = false, 
								  CoreFreeFunction pInitialCoreFreeFunction = NULL, void* pInitialCoreFreeFunctionContext = NULL,
								  CoreExtendFunction pInitialCoreExtendFunction = NULL, void* pInitialCoreExtendFunctionContext = NULL);

			/// Parameters structure overload
			PPM_VIRTUAL bool Init(const GeneralAllocator::Parameters& parameters); 


			/// Shutdown
			/// Frees all memory and clears all internal state to be the same as just after ctor.
			/// Debug builds may do internal checking and memory leak reporting.
			PPM_VIRTUAL bool Shutdown();


			/// AsInterface
			/// Safely and dynamically casts this instance to a parent or child class.
			/// A primary usage of this is to allow runtime tools to work with GeneralAllocator
			/// instances whereby it isn't known if they are subclasses of GeneralAllocator.
			///
			/// Example usage:
			///    GeneralAllocatorDebug* pGAD = (GeneralAllocatorDebug*)pGA->AsInterface(GeneralAllocatorDebug::kTypeId);
			///    if(pGAD)
			///        pGAD->Blah();
			PPM_VIRTUAL void* AsInterface(int typeId);


			/// Malloc
			/// Allocate 'nSize' bytes of memory.
			/// Malloc follows the C89 (and thus C99, which hasn't substantially changed) standard for 
			/// malloc behaviour. Specifically:
			///    - Returns a pointer to memory which has at least nSize bytes of usable space.
			///    - The returned pointer is of a minimum alignment level of 8 bytes for 
			///      32 bit systems, and 8 or 16 bytes for 64 bit systems, depending on a 
			///      configurable option.
			///    - NULL is returned if the allocation was not possible.
			///    - The returned pointer must eventually be freed by a call to Free.
			///    - A request size of 0 is not special. It will return a value which cannot
			///      be written to or read from, but it must be freed like any other Malloc pointer.
			///      Much like with non-zero request sizes, a Malloc of zero bytes can fail and
			///      return NULL.
			/// 
			PPM_VIRTUAL void* Malloc(size_t nSize, int nAllocationFlags = 0);


			/// Calloc
			/// Allocate 'nElementCount' elements of 'nElementSize' bytes each, all initialized to 0.
			/// Otherwise, this function is similar to Malloc.
			/// An element count and/or element size of zero is not special.
			PPM_VIRTUAL void* Calloc(size_t nElementCount, size_t nElementSize, int nAllocationFlags = 0);


			/// Realloc
			/// Re-allocate the previously allocated block in p, making the new block nNewSize bytes long.
			///
			/// Detailed behaviour description:
			///     ptr      size    return     semantics
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
			///
			///     Reallocating a pointer to a smaller size is guaranteed to succeed, 
			///     but is not guaranteed to return the original pointer.
			PPM_VIRTUAL void* Realloc(void* p, size_t nNewSize, int nAllocationFlags = 0);


			/// Free
			/// Frees a block allocated by Malloc, Realloc or Calloc.
			/// Follows the C89 (and thus C99, which hasn't substantially changed) standard 
			/// for behaviour. If PPM_NULL_POINTER_FREE_ENABLED is enabled (which it is by default),
			/// then the input pointer may be NULL.
			PPM_VIRTUAL void Free(void* p);


			/// MallocAligned
			/// Allocate 'nSize' bytes allocated to 'nAlignment' bytes.
			/// Input 'nAlignment' must be a multiple power of 2. If it is not 
			/// a power of 2, the next greater power of 2 is used. The minimum 
			/// alignment returned is 8, unless using a 64 bit addressing platform,
			/// or kAlignmentMin == 16, in which case the min alignment is 16 
			/// and not 8 and all 8s referred to here should be 16s. The alignment 
			/// offset parameter specifies how many bytes into the allocation that 
			/// the alignment should be found.
			/// Input 'nAlignmentOffset' must be a multiple of 8 or 16 (depending on kAlignmentMin;
			/// this is so because the minimum alignment for any returned memory is 8 or 16 and 
			/// thus an offset that is not a multiple of 8 or 16 would not be possible.
			/// The kAllocationFlagBestFit allocation flag can be used to help
			/// find a less fragmenting block of memory, though this flag can 
			/// also slow down the allocation function by a potentially undesirable
			/// amount of time, depending on the state of the heap.
			PPM_VIRTUAL void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0);


			/// MallocMultiple
			/// Allocates multiple adjacent but independent objects of the same size. Useful for creating fixed-size pools up front.
			/// Return value is pointer to resulting pointer array, allocated by user or function (see below).
			/// If input 'pResultArray' is not NULL, then it will hold the resulting allocated pointers.
			/// If input 'pResultArray' is NULL, then the return value will be an array allocated via Malloc and which must be freed via Free.
			/// If input 'nElementCount' is zero, input 'pResultArray' is returned if non-null, else a zero-sized array will be allocated via Malloc which must be freed via Free.
			/// Each returned object must be freed independently. 
			PPM_VIRTUAL void** MallocMultiple(size_t nElementCount, size_t nElementSize, void* pResultArray[], int nAllocationFlags = 0);


			/// MallocMultiple
			/// Similar to MallocMultiple but each element size is uniquely specified.
			PPM_VIRTUAL void** MallocMultiple(size_t nElementCount, const size_t nElementSizes[], void* pResultArray[], int nAllocationFlags = 0);


			/// GetMaxMallocSize
			/// Returns the largest size that you can possibly allocate via Malloc, MallocAligned, etc.
			size_t GetMaxMallocSize() const;


			/// GetMaxCoreSize
			/// Returns the max size for a block of core memory, as supplied in the constructor,
			/// Init, AddCore, or SetOption(kOptionNewCoreSize).
			size_t GetMaxCoreSize() const;


			/// SetAddCoreFunction
			/// Specifies a callback function to be called when a GeneralAllocator instance
			/// needs to add new core memory, or when AddCore is called with a NULL pData argument.
			/// When a CoreAddFunction is specified, it completely takes over the allocation of 
			/// new core, and if it returns NULL then no fallback code will execute to back it up.
			/// The CoreAddFunction returns the void* address of the core block and fills in the ppCoreFreeFunction and 
			/// ppCoreExtendFunction pointers and contexts and coreType which will be later used when it's time to 
			/// free or extend the core memory. *ppCoreFreeFunctionContext and *ppCoreExtendFunctionContext
			/// may be set to NULL if the CoreFreeFunction and CoreExtendFunction don't need any context
			/// information passed to them. CoreFreeFunction and CoreExtendFunction must be set or else
			/// default behavior will be used for the given platform, which may result in unexpected behavior.
			/// The CoreAddFunction should use the value in pMinSize to determine how much memory to commit.  If it chooses
			/// to commit a different amount of memory, it should write the amount committed out to pMinSize.
			/// The CoreAddFunction must set *pReservedSize appropriately.
			/// coreType is a user-specified value that defines the type of core memory added. See enum CoreType.
			typedef void* (*CoreAddFunction)(size_t* pMinSize, size_t* pReservedSize, 
											 CoreFreeFunction* ppCoreFreeFunction, void** ppCoreFreeFunctionContext, 
											 CoreExtendFunction* ppCoreExtendFunction, void** ppCoreExtendFunctionContext,
											 uint32_t& coreType, void* pContext);
			void SetAddCoreFunction(CoreAddFunction pCoreAddFunction, void* pCoreAddFunctionContext);


			/// AddCore
			/// If pCore is non-null, it and nSize are used to give the allocator more core memory. 
			/// In this case nSize must be in system memory page size (EA::Allocator::kDefaultPageSize)
			/// increments, else it will be dropped to the next lowest increment and the core will 
			/// not be used. If pInitialCore is NULL but nInitialCoreSize is greater than zero, 
			/// AddCore will attempt to acquire nSize bytes of memory; If it is unable to allocate
			/// the specified amount of memory then the function fails. It will not attempt to 
			/// allocate any smaller amount than nSize. If both input arguments are zero, then 
			/// nothing is done. If input bShouldTrimCore is true, then the core can be freed during 
			/// a core trimming phase (assuming bShouldFreeCore is true). If input bShouldFreeCore 
			/// is true, then the core will be freed on shutdown. If the passed in callback function 
			/// is non-null, then it is freed via a call to the callback function, else the core 
			/// is assumed to be of the same type as native core and freed by the native core freeing 
			/// mechanism. The return value of the CoreFreeFunction callback is the number of bytes 
			/// freed, which should normally be the same as the input size. 
			/// This function calls the CoreAddFunction if pCore is NULL and the user specified a 
			/// CoreAddFunction via SetAddCoreFunction.
			PPM_VIRTUAL bool AddCore(void* pCore, size_t nSize, bool bShouldFreeCore = false, bool bShouldTrimCore = false, CoreFreeFunction pCoreFreeFunction = NULL,
									 void* pCoreFreeFunctionContext = NULL, CoreExtendFunction pCoreExtendFunction = NULL, void* pCoreExtendFunctionContext = NULL);

			PPM_VIRTUAL bool AddCore(const Parameters& parameters);

			/// AddCoreEx
			/// This is the same as AddCore except it allows for specifying desired CoreType and allows
			/// for identifying through coreType and nReservedSize that pCore (if not NULL) refers to 
			/// memory that is extendable (e.g. via Windows VirtualAlloc MEM_)COMMIT). nReservedSize must 
			/// zero or >= nSize.
			PPM_VIRTUAL bool AddCoreEx(void* pCore, size_t nSize, uint32_t coreType, size_t nReservedSize, 
									   bool bShouldFreeCore, bool bShouldTrimCore, 
									   CoreFreeFunction pCoreFreeFunction, void* pCoreFreeFunctionContext,
									   CoreExtendFunction pCoreExtendFunction, void* pCoreExtendFunctionContext);
	
			/// TrimCore
			/// Returns unused memory back to the system, leaving at least input 'nPadding' unused 
			/// bytes unreturned. Return value is the number of bytes returned to the system.
			PPM_VIRTUAL size_t TrimCore(size_t nPadding);


			/// ClearCache
			/// This function clears the internal cache. This allocation keeps a small cache of 
			/// frequently used small block sizes in order to improve both speed and fragmentation
			/// characteristics. Normally, the cache is cleared by internally driven mechanisms 
			/// when the the situation demands it. However, there are some circumstances whereby 
			/// the user may want to force a cache clear to occur. An example of such a situation
			/// might be if the user wants to analyze the free block state; an uncleared cache
			/// can make the heap appear to be more fragmented than it actually is.
			PPM_VIRTUAL void ClearCache();


			/// SetMallocFailureFunction
			/// Allows user to install a function which is called if a malloc request could not
			/// be honored, usually because memory is exhausted. The MallocFailureFunction could
			/// be used to notify of a condition, do garbage collection, or supply more core to 
			/// the allocator. If the MallocFailureFunction returns true, the allocation is 
			/// retried one time; if it fails the second time, the MallocFailureFunction won't 
			/// be called. If the MallocFailureFunction returns false, the allocation is not retried.
			/// Input nMallocRequestedSize is the size requested by the caller during which the 
			/// attempt failed. Input nAllocatorRequestedSize is the size requested by the allocator,
			/// which will be greater or equal to nMallocRequestedSize.
			typedef bool (*MallocFailureFunction)(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
			PPM_VIRTUAL void SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void* pContext);

			/// SetMemoryRangeCallbackFunction
			/// Callback intended to be used to update virtual memory map of heaps.
			/// The callback will be invoked whenever virtual memory is added or removed.
			typedef void (*MemoryRangeCallbackFunction)(void* userData, void* address, size_t size, bool allocated);
			void SetMemoryRangeCallbackFunction(MemoryRangeCallbackFunction fn, void* userData);

			/// AssertionFailureInfo
			/// Struct passed to user upon assertion failure.
			struct AssertionFailureInfo
			{
				const GeneralAllocator* mpGeneralAllocator;   /// The allocator the assertion is associated with. May be NULL for failures that are without allocator context.
				const char*             mpExpression;         /// The assertion expression that failed.
				int                     mnErrorId;            /// One of enum GAError or a user-defined or subclass-defined id.
				const char*             mpDescription;        /// A description of the assertion failure.
				const void*             mpData;               /// The user allocation the assertion is associated with. May be NULL for failures that are without allocation context.
				const void*             mpExtra;              /// Other information related to the failure. The meaning depends on the failure id.
			};

			/// SetAssertionFailureFunction
			/// Allows the user to specify a callback function to trap assertion failures.
			/// The user must not attempt to modify the allocator while in this failure callback.
			typedef void (*AssertionFailureInfoFunction)(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
			void SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext);

			// *** Deprecated ***
			typedef void (*AssertionFailureFunction)(const char* pExpression, void* pContext);
			void SetAssertionFailureFunction(AssertionFailureFunction pAssertionFailureFunction, void* pContext);


			/// AssertionFailure
			/// Triggers an assertion failure. This function is generally intended for internal
			/// use but is available so that debug systems wrapped around this allocator can
			/// use the standard assertion facilities.
			void AssertionFailure(const char* pExpression, int nErrorId = kGAErrorUnknown, const char* pDescription = NULL, 
									const void* pData = NULL, const void* pExtra = NULL) const;

			/// HookType
			/// Enumerates high level allocation types.
			enum HookType
			{
				kHookTypeMalloc,        /// Malloc or similar was called. See HookSubType to find out which.
				kHookTypeFree,          /// Free was called.
				kHookTypeCoreAlloc,     /// Core was allocated or donated. The hook is called when the user explicitly supplies core. The hook is not called when core is extended on platforms that support core extension (e.g. Win32/Win64).
				kHookTypeCoreFree,      /// Core will be freed. The hook is called for core allocated internally and for core explicitly supplied by the user.
				kHookTypeCoreExtended   /// Core was extended, as opposed to newly allocated. We don't call this CoreRealloc or use kHookSubTypeRealloc for this because realloc implies that a new pointer could be made, whereas extension is strictly the in-place resizing that realloc supports.
			};

			/// HookSubType
			/// Provides more detail on a specific allocation for HookType::kHookTypeMalloc.
			enum HookSubType
			{
				kHookSubTypeNone,
				kHookSubTypeMalloc,
				kHookSubTypeCalloc,
				kHookSubTypeRealloc,
				kHookSubTypeMallocAligned,
				kHookSubTypeMallocMultiple1,
				kHookSubTypeMallocMultiple2
			};

			/// HookInfo
			/// Struct used for completely describing an allocation request or result.
			struct HookInfo
			{
				GeneralAllocator* mpGeneralAllocator;       /// Pointer to the GeneralAllocator that is calling the hook.
				bool              mbEntry;                  /// If true, then hook is being called on function entry (before the operation is done), else being called on function exit (after the operation is done).
				HookType          mHookType;                /// One of enum HookType.
				HookSubType       mHookSubType;             /// One of enum HookSubType. For kHookTypeCoreAlloc, it is always kHookSubTypeNone.
				size_t            mnSizeInputTotal;         /// For Malloc, Calloc, Realloc (size before), MallocAligned, MallocMultiple, CoreAlloc (allocated size), CoreFree (freed size), CoreExtend (size before extending).
				const void*       mpDataInput;              /// For Realloc, Free, CoreAlloc, CoreFree.
				size_t            mnCountInput;             /// For Calloc, MallocMultiple.
				size_t            mnSizeInput;              /// For Malloc, Realloc, CoreAlloc, CoreFree, CoreExtend (size before extending).
				const size_t*     mpSizeInputArray;         /// For MallocMultiple.
				size_t            mnAlignmentInput;         /// For MallocAligned.
				int               mnAllocationFlags;        /// For all kHookTypeMalloc sub types.
				void*             mpDataOutput;             /// For Malloc, Calloc, Realloc, MallocAligned.
				void**            mpArrayOutput;            /// For MallocMultiple.
				size_t            mnSizeOutput;             /// For Malloc, MallocAligned (alignment offset) Realloc (size after), CoreAlloc, CoreFree (size after), CoreExtend (size after).
				uint64_t          mEventId;                 /// Event id, which is incremented for each call. Allows for knowing the order of events. Pre and post event hook calls use the same id.
			};

			/// SetHookFunction
			/// This function lets you specify a callback function which will be called upon
			/// entering an allocation and upon leaving an allocation. The difference is that
			/// upon exiting the function the heap has changed and the user now has a return
			/// value to work with. The hook function is called with GeneralAllocator unlocked,
			/// so thread safety issues are not a concern for the called hook function. However,
			/// subclasses of GeneralAllocator may not have the same thread safety regarding
			/// these hooks as GeneralAllocator itself.
			typedef void (*HookFunction)(const HookInfo* pHookInfo, void* pContext); 
			PPM_VIRTUAL void SetHookFunction(HookFunction pHookFunction, void* pContext);


			/// Lock
			/// Locks or unlocks the class object for thread-safe usage across calls.
			/// While locked, all other threads must wait until a matching unlock is called.
			/// Locks are recursive, and thus two successive locks are expected to be matched
			/// by two successive unlocks before other threads can proceed.
			void Lock(bool bEnable);


			/// GetName
			/// Returns the name of this instance of GeneralAllocator. 
			/// The return value will be NULL if no name has been set. This function is
			/// useful for heap metrics collection and reporting, especially in the presence
			/// of multiple heaps. 
			const char* GetName() const;


			/// SetName
			/// Sets the name of this instance of GeneralAllocator.
			/// The pName pointer will be stored directly, as opposed to the contents of 
			/// the pointer being copied. This means that the pointer must remain valid
			/// for the lifetime of the GeneralAllocator instance which holds it.
			/// This function is useful for heap metrics collection and reporting,  
			/// especially in the presence of multiple heaps. 
			void SetName(const char* pName);


			/// Runtime Options
			enum Option
			{
				kOptionNone                      =  0,
				kOptionEnableThreadSafety        =  1,      /// If non-zero, enabled thread-safety, otherwise disable it.
				kOptionEnableHighAllocation      =  2,      /// This option is deprecated, as it is now considered always enabled. If set to a non-zero value, then high-memory allocation is enabled. The default is disabled, as this functionality has a runtime cost.
				kOptionEnableSystemAlloc         =  3,      /// Enables or disables the use of the system as a source for more core memory when existing core memory is exhausted. If system allocation is disabled, then mapped memory is also disabled. If system allocation is disabled, the only way to increase core memory is to call AddCore. AddCore can be called at any time or can be called from a user-installed MallocFailureFunction.
				kOptionNewCoreSize               =  4,      /// Size of new core memory blocks when they are automatically internally allocated. Default is system-specific.
				kOptionCoreIncrementSize         =  5,      /// Size to add to core memory blocks when the system supports extending existing blocks. Must be less or equal to kOptionNewCoreSize. Must be called after kOptionNewCoreSize. Default is system-specific.
				kOptionMaxFastBinRequestSize     =  6,      /// Max size held by FastBins. Refers to user request size, not full chunk size. 
				kOptionTrimThreshold             =  7,      /// TrimThreshold is the maximum amount of unused top-most memory to keep before automatically freeing via Trim().
				kOptionTopPad                    =  8,      /// TopPad is the amount of extra 'padding' space to allocate or retain whenever MoreCore is called.
				kOptionMMapThreshold             =  9,      /// MMapThreshold is the request size threshold for using mmap(), VirtualAlloc, etc. to service a request. You can set this to a high value in order to disable mapped memory allocations.
				kOptionMMapMaxAllowed            = 10,      /// Max number of MMap allocations to have at a time.
				kOptionMMapTopDown               = 11,      /// If non-zero, allocate mapped memory from the top of address space if possible. Default is zero (disabled) for 32 bit platforms and enabled for 64 bit+ platforms.
				kOptionMMapPageSize              = 12,      /// Set the page size in bytes used for memory mappped allocations. Must be equal to a supported system page size. Some platforms allow for different sizes, with larger sizes resulting in better hardware performance.
				kOptionTraceInternalMemory       = 13,      /// If enabled, then TraceAllocatedMemory includes internal memory. Default is disabled since it might confuse the user by reporting leaks that aren't really leaks or blocks that the user doesn't expect. Note that the ReportHeap and TakeSnapshot functions allow you to specify the inclusion of internal blocks as part of their API. We have a separate option here because TraceAllocatedMemory is sometimes called internally and not by any user call.
				kOptionMaxMallocFailureCount     = 14,      /// Max number of times to call the MallocFailure function successively. Default is 256.
				kOptionEnableUnsortedBin         = 15,      /// If disabled, then the 'unsorted bin' cache is not used and freed chunks are immediately placed into sized bins. Default is enabled.
				kOptionCoreTopDown               = 16,      /// If non-zero, allocate new core memory from the top of address space if possible. Default is zero (disabled) for 32 bit platforms and enabled for 64 bit+ platforms.
				kOptionLockDuringHookCalls       = 17,      /// Specifies if our mutex remains locked during Hook callbacks. Defaults to unlocked. It is *safer* to be unlocked, as being locked during a hook call invites a potential deadlock if the user isn't careful.
				kOptionExecutableCore            = 18,      /// If enabled then internally allocated core memory is allocated as executable in addition to read/write. This is disabled by default.
				kOptionEnableMallocFailureAssert = 19,      /// If enabled then Malloc functions execute PPM_ASSERT(false) before returning. This is disabled by default.
				kOptionEnableTrimToZero			 = 20       /// If enabled then when TrimCore is called, PPMalloc will remove all trimmable cores including the last core leaving the allocator empty and forced to allocate a new core on the next alloc. This is off by default, but useful to use if you know you have short lived allocators with large initial cores (say render targets) which need to linger but you'd prefer to take the performance hit than risk running out of memory.
			};

			/// GetOption
			/// Returns the value of the given option, which is one of enum Option or a subclass' option.
			PPM_VIRTUAL int64_t GetOption(int option) const;

			/// SetOption
			/// Runtime options
			/// The user specifies one of enum Option. Note that the argument 'option' is 
			/// of type int instead of type Option because subclasses may want to extend
			/// the option set to include values not listed here.
			PPM_VIRTUAL void SetOption(int option, int64_t nValue);


			/// SetInitCallback
			/// This function allows the user to register a function that's called whenever
			/// A GeneralAllocator instance is initialized or shut down. The callback function 
			/// is called with bInit == true right after the completion of Init and it is called
			/// with bInit == false right before the execution of Shutdown.
			///
			/// The callback function is specified by an InitCallbackNode instead of just a function
			/// pointer so that GeneralAllocator can keep a linked list of these (without allocating
			/// memory) and thus that multiple listeners can be registered at the same time. 
			/// The first callback registered will be the first one to be called in the case of 
			/// there being multiple registered callbacks.
			///
			/// This function is not thread-safe, as it is maintaining global state instead of 
			/// instance-specific state and we avoid the overhead of implementing global state  
			/// thread safety. Thus the user should ensure that this function is called only from
			/// a single thread or otherwise not at the same time from multiple threads.
			///
			/// As of this writing, the callback function is called from the constructor of the 
			/// GeneralAllocator. This is done in order to simplify GeneralAllocator usage for
			/// the user. The downside to this is that the callback function cannot immediately
			/// use the allocator in the callback, as the allocator is in the process of constructing.
			/// The user can use allocator at any later time, however.
			///
			/// Example usage:
			///    class Listener {
			///        InitCallbackNode icn;
			///        Listener() {
			///            icn.mpInitCallbackFunction = &Callback;
			///            icn.mpContext = this;
			///            GeneralAllocator::SetInitCallback(&icn, true);
			///        }
			///        ~Listener() {
			///            GeneralAllocator::SetInitCallback(&icn, false);
			///        }
			///        static void Callback(GeneralAllocator* pGeneralAllocator, bool bInit, void* pContext) {
			///            Listener* pListener = (Listener*)pContext; 
			///        }
			///    };
			///
			typedef void (*InitCallbackFunction)(GeneralAllocator* pGeneralAllocator, bool bInit, void* pContext);
			struct InitCallbackNode
			{
				InitCallbackFunction mpInitCallbackFunction;
				void*                mpContext;
				InitCallbackNode*    mpNext;
			};
			static void SetInitCallback(InitCallbackNode* pInitCallbackNode, bool bSet = true);


			/// GetUsableSize
			/// Report the number of usable bytes associated with 'pData'. Thus if the user allocates
			/// 30 bytes it is possible that the block returned to the user actually has 40 bytes of 
			/// usable space. The non-NULL input pointer must be valid or the results of the function are 
			/// undefined. A NULL input pointer results in a return value of -1.
			/// The block must be valid for the duration of the call.
			static size_t GetUsableSize(const void* pData);

			/// GetBlockSize
			/// Returns the size of the block of memory used to hold pData. This size is going to be a
			/// a value greater than the size requested by the user. It will be as few as four bytes 
			/// greater but can be higher depending on circumstances. This function is intended for 
			/// debugging and metrics purposes and shouldn't be relied upon for anything beyond this. 
			/// The bNetSize argument specifies whether the function should return the true block
			/// size or the block's "net size." In an optimized build blocks (internally called chunks)
			/// overlap by size_type (usually 4) bytes and thus three blocks in a row will share 8 bytes
			/// between themselves. Thus a user who is wanting to measure memory usage may want to set
			/// the bNetSize argument to true, as that will result a better measurement of memory usage sums.
			/// The block must be valid for the duration of the call.
			static size_t GetBlockSize(const void* pData, bool bNetSize = false);

			/// GetLargestFreeBlock
			/// Returns the largest current block. If bClearCache is true, then any present cache
			/// will be cleared before the check is done. Note that clearing the cache may result
			/// in a larger largest free block, but clearing the cache has some minor performance costs.
			PPM_VIRTUAL size_t GetLargestFreeBlock(bool bClearCache = false);


			/// SetDebugTraceFunction
			/// Allows you to override where traces go. If not specified, traces are sent
			/// via printf to 'stdout'. Strings passed to the DebugTraceFunction function 
			/// will have trailing newlines.
			typedef void (*TraceFunction)(const char* pTraceText, void* pContext);
			PPM_VIRTUAL TraceFunction GetTraceFunction(void** pContext);
			PPM_VIRTUAL void          SetTraceFunction(TraceFunction pTraceFunction, void* pContext);


			/// SetTraceFieldDelimiters
			/// Allows the user to specify the formatting delimiters of traced output. This is 
			/// useful for assisting in automated parsing of trace output. For example, you might
			/// want to be able to directly read the trace output with a spreadsheet application.
			/// The default field delimiter is the tab character and the default record delimiter
			/// is the newline character.
			PPM_VIRTUAL void SetTraceFieldDelimiters(unsigned char fieldDelimiter, unsigned char recordDelimiter);


			/// HeapValidationLevel
			/// Defines levels of extensiveness for the ValidateHeap function.
			enum HeapValidationLevel
			{
				kHeapValidationLevelNone,
				kHeapValidationLevelBasic,
				kHeapValidationLevelDetail,
				kHeapValidationLevelFull
			};


			/// ValidateChunk
			/// Does a check of the given heap Chunk for errors. 
			/// A Chunk is the basic unit of memory allocation within the heap. Both allocated
			/// and free memory is stored as Chunks. Chunks have a small header and then 
			/// are followed by the user data area.
			/// The GetChunkPtrFromDataPtr function converts a user pointer to a Chunk 
			/// by subtracting a few bytes from it, since the Chunk consists of a header 
			/// and the user data.
			PPM_VIRTUAL bool ValidateChunk(const void* pChunk, HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);


			/// ValidateHeap
			/// Does a check of the heap for errors.
			/// This function is the primary function for validating the integrity of the heap.
			/// The large majority of heap integrity failures will be successfully detected
			/// by the ValidateHeap function, especially as you raise the validation level 
			/// and the PPM_DEBUG level. The higher the validation level and the larger the
			/// number of blocks in the heap, the slower the ValidateHeap function will be.
			/// Returns true if the validation proceeded without any integrity errors being
			/// found. Also, if you have PPM_DEBUG set to anything but zero, the ValidateHeap
			/// function may internally generate an assertion failure. If the heap validation
			/// detects an integrity error, the large majority of the time it is due to user
			/// error, such as memory overruns.
			PPM_VIRTUAL bool ValidateHeap(HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);


			/// SetAutoHeapValidation
			/// Enables or disables automatic heap validation of a given leven and given
			/// event frequency. An event is a user-level allocation action such as malloc
			/// or free. Automatic heap validation is useful for catching problems sooner
			/// rather than later. heapValidationLevel can be kHeapValidationLevelNone 
			/// to disable automatic heap validation. Automatic heap validation is off by default.
			///
			/// The nFrequency argument designates that the auto validation will occur every
			/// nFrequency allocation event. Thus 1 means to validate on every single event.
			/// A value of 0 will act the same as 1.
			///
			/// Example usage:
			///    pAllocator->SetAutoHeapValidation(GeneralAllocator::kHeapValidationLevelDetail, 32);
			/// 
			PPM_VIRTUAL void SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency);


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
			/// If pStorage is non-NULL, it is used to hold scratch memory needed to implement the trace.
			/// pStorage must be at least as great as SnapshotImage in terms of both size and alignment. 
			/// If pStorage is NULL or doesn't hold enough space, memory needed to implement the trace  
			/// is allocated from the heap.
			PPM_VIRTUAL void TraceAllocatedMemory(TraceFunction pTraceFunction = NULL, void* pTraceFunctionContext = NULL, 
									void* pStorage = NULL, size_t nStorageSize = 0, int nBlockTypeFlags = kBlockTypeAllocated);


			/// DescribeData
			/// Given an allocated pointer to data, returns a string description of that data.
			/// The return value is the strlen of the returned data. 
			PPM_VIRTUAL size_t DescribeData(const void* pData, char* pBuffer, size_t nBufferLength);


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
			/// The validity is determined by the addressType parameter, which is expected to be
			/// one of enum AddressType. It is currently an int instead of enum AddressType due to 
			/// backwards compatibility requirements.
			/// If the AddressType is kAddressTypeOwned, then the return value is equal to the 
			/// input address if the address is owned by the allocator (see kAddressTypeOwned).
			/// The kAddressTypeOwned check is the fastest check and may be useful for doing a 
			/// quick test of whether a address (valid for user use or not) belongs to a given heap.
			/// If the AddressType is kAddressTypeAllocated, then the return value is equal to the 
			/// *allocated pointer* for the block that the address comes from. This will be a value
			/// that is <= the input address. This mode is useful for converting an arbitrary address 
			/// to the allocated pointer for that address. To be successful, the input address must 
			/// come from within the valid user accessible area of the block.
			/// If the AddressType is kAddressTypeSpecific, then the return value is equal to the 
			/// input pointer if the input pointer refers to an allocated pointer returned by the
			/// allocator. Thus, it is a pointer value that can be freed.
			/// If any validation fails, the return value is NULL.
			/// This function is not guaranteed to do debug checking of the allocation associated 
			/// with pAddress. For example, it isn't guaranteed to do guard fill checking or 
			/// allocation header sanity checking. This function validates only the input address.
			/// For such debug functionality, you should use the ValidateChunk function.
			///
			/// Example usage:
			///    void* p = pAllocator->Malloc(1);
			///    void* pResult = pAllocator->ValidateAddress(pData, GeneralAllocator::kAddressTypeSpecific);
			///
			PPM_VIRTUAL const void* ValidateAddress(const void* pAddress, int addressType = kAddressTypeAllocated) const;

			/// IsAddressHigh
			/// Returns true if the given address is part of the high memory area. The high memory area
			/// is usually used for storage of semi-permanent or otherwise long-lasting user allocations.
			/// Note that boundary between low and high memory can move and under some circumstances, 
			/// an address which used to be considered high may no longer be so. This behaviour only 
			/// results from unusual allocation activity, however.
			PPM_VIRTUAL bool IsAddressHigh(const void* pAddress);


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

			/// BlockInfo
			/// Defines allocation block information for use with heap enumeration.
			/// We call this BlockInfo instead of ChunkInfo because most users are 
			/// familiar with the term 'block' more than 'chunk'.
			struct BlockInfo
			{
				const void*  mpCore;            /// The core to which this block belongs. Recall that the heap consists of one or more large master "core blocks" which are subdivided into the blocks handed to users.
				const void*  mpBlock;           /// The block itself (Chunk or CoreBlock). Will be a Chunk when mBlockType is kBlockTypeAllocated or kBlockTypeFree. Will be equal to mpCore when mBlockType is kBlockTypeCore or if mbMemoryMapped is true.
				size_t       mnBlockSize;       /// The size of the block.
				void*        mpData;            /// The user data within this block. Note that for regular allocations on 32 bit systems, mpData is usually equal to mpBlock + 8, with the 8 bytes being internal block info.
				size_t       mnDataSize;        /// The size of the user data within this block.
				char         mBlockType;        /// One of enum BlockType
				bool         mbMemoryMapped;    /// True if the memory is mapped. If true, then mpBlock will be equal to mpCore.
				uint32_t     mCoreType;         /// One of enum CoreType or a user-defined value, if mBlockType is kBlockTypeCore. Unused if mBlockType is not kBlockTypeCore.
				size_t       mCoreReservedSize; /// For extendable core, refers to the amount of space the memory can grow into, like Windows MEM_RESERVE.
			};

			// GetBlockInfo
			// Gets the block info for the given allocated pData.
			// void GetBlockInfo(const void* pData, BlockInfo* pBlockInfo) const;

			/// SnapshotImage
			/// Used to specify an opaque buffer the user can supply for the TakeSnapshot function.
			/// The user is not forced into using SnapshotImage, as the user may have a better 
			/// source of scratch memory.
			struct SnapshotImage
			{
				uint64_t mData[32]; // Defined as uint64_t in order to ensure 8 byte alignment.
			};

			/// TakeSnapshot
			/// Takes a 'snapshot' of the heap for the user. This snapshot can be used to do 
			/// comparisons and recordings of state, depending on the snapshot and what you 
			/// do with it. Note that a snapshot of the heap is not a full copy of the heap
			/// but is a record of what blocks are allocated and free in the heap. If the 
			/// input bMakeCopy is true, then a copy of the heap information is taken; this 
			/// what is l meant by taking a snapshot. But if bMakeCopy is false, then
			/// instead of a copy being made, the snapshot is set up as a heap in-place 
			/// iterator which can be used as if it was a regular copy.
			/// If pStorage is non-NULL, it is used to hold scratch memory needed to implement the trace.
			/// If bMakeCopy is false, the size and alignment of pStorage must be at least equal to 
			/// that of struct SnapshotImage. Otherwise, the required size of pStorage varies but will 
			/// likely be significantly larger than SnapshotImage, but must have at least 64 bit alignment.
			/// If pStorage is NULL or doesn't hold enough space, memory needed to implement the trace 
			/// is allocated from the heap.
			PPM_VIRTUAL void* TakeSnapshot(int nBlockTypeFlags = kBlockTypeAll, bool bMakeCopy = false, void* pStorage = NULL, size_t nStorageSize = 0);
			PPM_VIRTUAL void  FreeSnapshot(void* pSnapshot);


			/// ReportHeap
			/// This function reports the entire heap layout in detail. It should allow for the 
			/// fairly detailed analysis of memory usage. If the user returns false from the 
			/// HeapReportFunction, the heap reporting stops. ReportHeap itself returns the 
			/// last value returned by the HeapReportFunction. If bMakeCopy is true, then a 
			/// snapshot is taken of the heap before starting the callbacks, and thus it is 
			/// safe to do heap allocations during the reporting in this case. The Heap will 
			/// begin by reporting a core block and then report each allocated or free block 
			/// within that core block. After all blocks for all cores are reported, memory mapped chunks are reported.
			/// If pStorage is non-NULL, it is used to hold scratch memory needed to implement the trace.
			/// If bMakeCopy is false, the size of pStorage must be at least 256 bytes. Otherwise, the 
			/// required size of pStorage varies but will likely be significantly larger than 256 bytes.
			/// If pStorage is NULL or doesn't hold enough space, memory needed to implement the trace 
			/// is allocated from the heap.
			typedef bool (*HeapReportFunction)(const BlockInfo* pBlockInfo, void* pContext); 
			PPM_VIRTUAL bool ReportHeap(HeapReportFunction, void* pContext, int nBlockTypeFlags, bool bMakeCopy = false, void* pStorage = NULL, size_t nStorageSize = 0);


			/// ReportBegin
			/// Begins heap iteration, Returns a context to be used for ReportNext and 
			/// ReportEnd calls. Must be matched with a final ReportEnd. If the input
			/// pSnapshot is present, then the iteration is done on the given snapshot,
			/// else the heap reporting is done in with a snapshot of the current state.
			/// A snapshot is taken of the heap before starting the callbacks, so it is 
			/// safe to do heap allocations during the reporting. However, if you want
			/// read the heap data that the BlockInfo refers to and if thread safety is
			/// enabled, you will need to use Lock to lock the heap for thread-safe 
			/// access during the iteration. If bMakeCopy is true, then the allocator
			/// takes a snapshot of the heap and works from that. If bMakeCopy is 
			/// false, then the report is done directly from the working heap and you
			/// must not making heap calls while iterating the heap.
			PPM_VIRTUAL const void*      ReportBegin(void* pSnapshot = NULL, int nBlockTypeFlags = kBlockTypeAll, bool bMakeCopy = false, void* pStorage = NULL, size_t nStorageSize = 0);
			PPM_VIRTUAL const BlockInfo* ReportNext(const void* pContext, int nBlockTypeFlags = kBlockTypeAll); /// Gets the next block in the report. Returns NULL when there is nothing left to report.
			PPM_VIRTUAL void             ReportEnd(const void* pContext);                                       /// Ends the report, matching an intial ReportBegin.

			size_t PhysicalFootprint() const;
			size_t VirtualFootprint() const;
			size_t NumCores() const;
			size_t CoreIncrementSize() { return mnCoreIncrementSize; }

		protected:
			friend class HandleAllocator;
			friend class NonLocalAllocator;

			#if defined(PPM_SIZE_TYPE)
				typedef  PPM_SIZE_TYPE  size_type;
				typedef PPM_SSIZE_TYPE ssize_type;
			#elif (EA_PLATFORM_PTR_SIZE >= 8)
				typedef uint64_t        size_type;  /// Must be an unsigned type.
				typedef  int64_t       ssize_type;  /// Signed equivalent of size_type.
			#else
				typedef uint32_t        size_type;  /// Must be an unsigned type.
				typedef  int32_t       ssize_type;  /// Signed equivalent of size_type.
			#endif

			/// Chunk
			/// Defines the basic user-level allocation block.
			/// All allocated memory is broken down into Chunks, which are our internal allocation unit.
			struct Chunk{
				size_type mnPriorSize;   /// Refers to size of prior chunk in memory. Size of prior chunk if the prior chunk is free, else this space is used by the last bytes of the prior chunk's allocated bytes. Stored as size_type instead of size_t because size_t may be 64 bits, which we don't want.
				size_type mnSize;        /// Size in bytes, including overhead. Some unused bits from mnSize are used for other purposes. This is possible because mnSize is always at least 8 byte aligned. Also, one or two top bits might be usable too.
				Chunk*    mpPrevChunk;   /// Previous chunk in whatever list this chunk is in. Only used in free chunks.
				Chunk*    mpNextChunk;   /// Next chunk in whatever list this chunk is in. Only used in free chunks.
			};

			/// SkipListChunk
			/// Chunks stored in the large bins are sorted from largest to smallest. When there are huge numbers of chunks in the 
			/// bins then we use a skip list to quickly jump from one group of equally sized chunks to the next. Imagine that the 
			/// sizes of chunks in the bins could be like this: 648 648 648 648 648 620 620 620 620 620 620 620 604 604 604 604 604 604. 
			/// The skip list allows us to quickly find the first 620-sized chunk from the first 648-sized chunk so we don't have 
			/// to walk the long linked list of 648-sized chunks. This feature was added to prevent huge slowdowns when the large   
			/// bins accumulated huge amounts of similarly-sized chunks. See the .cpp file for more documentation about the skip list.
			struct SkipListChunk : public Chunk{
				SkipListChunk* mpLargerChunks;              /// Points back to the first Chunk in the skip list that is of the next larger size.
				SkipListChunk* mpSmallerChunks;             /// Points forward to the first Chunk in the skip list that is of the next smaller size.
			};

			/// CoreBlock
			/// Defines the large blocks that are used to hold Chunks.
			/// The head bytes (e.g. mpCore, mnSize, etc.) of the CoreBlock are used for 
			/// administrative purposes, while the space after the end of the CoreBlock is 
			/// used to store the actual (usually large) space for Chunks.
			struct CoreBlock{
				char*              mpCore;                      /// Points to the start of core memory, which is at the end of this block or at the end of a subclass of this block.
				size_t             mnSize;                      /// Size of the entire core block. For mapped memory systems, this is size of committed (and thus usable) memory.
				uintptr_t          mnMemBlockId;                /// Some systems may use a seperate handle to manager memory blocks, rather than a raw pointer
				uint32_t           mCoreType;                   /// One of enum CoreType or a user-defined value.
				size_t             mnReservedSize;              /// Size of reserved (but not necessarily entirely committed) memory of the CoreBlock. Used by systems that support mapped memory.
				bool               mbMMappedMemory;             /// If true, then this is mapped memory.
				bool               mbShouldFree;                /// If true, the memory should be freed when not needed. 
				bool               mbShouldFreeOnShutdown;      /// If true, the memory should be freed on shutdown, even if mbShouldFree is false.
				bool               mbShouldTrim;                /// If true, the memory should be trimmed (partially freed) when not needed. Only applicable to advanced memory management systems such as Windows.
				bool               mbPageExecutable;            /// If true, the memory was allocated as executable in addition to read and write..
				CoreFreeFunction   mpCoreFreeFunction;          /// If non-NULL, frees are done via this function.
				void*              mpCoreFreeFunctionContext;   /// This is passed to the CoreFreeFunction; it allows the core free function to have a context to work with.
				CoreExtendFunction mpCoreExtendFunction;        /// 
				void*              mpCoreExtendFunctionContext; /// 
				CoreBlock*         mpPrevCoreBlock;             /// Prev CoreBlock in a list that manages all core blocks.
				CoreBlock*         mpNextCoreBlock;             /// Next CoreBlock in a list that manages all core blocks.
			};
			friend struct CoreBlockEx; // Defined so that such a struct can be created which inherits from CoreBlock.

			/// Snapshot
			/// Internal-use struct for managing heap snapshots.
			struct Snapshot{
				enum{ kSnapshotMagicNumber = 0x534e4150 }; // 0x534e4150 = 'SNAP' 

				int        mnMagicNumber;      /// Used to verify if this is a valid Snapshot struct.
				size_t     mnSizeOfThis;       /// The size of the snapshot structure itself.
				int        mnBlockTypeFlags;   /// Global block type flags filter to apply. See enum BlockType.
				bool       mbUserAllocated;    /// If true, then the memory for this Snapshot was supplied by the user and we never free it ourselves.
				bool       mbReport;           /// If true, then we allocated the Snapshot internally in ReportBegin and need to free it ourselves in ReportEnd (if mbUserAllocated is false).
				bool       mbDynamic;          /// If true, then we iterate dynamically instead of via BlockInfo recording.
				bool       mbCoreBlockReported;/// If true, then we have iterated the CoreBlock for the current set of Chunks.
				CoreBlock* mpCurrentCoreBlock; /// For dynamic heap iteration. Used if mbDynamic = true.
				Chunk*     mpCurrentChunk;     /// For dynamic heap iteration. Used if mbDynamic = true.
				Chunk*     mpCurrentMChunk;    /// For dynamic heap iteration. Used if mbDynamic = true.
				size_t     mnBlockInfoCount;   /// The count of block info items. Used if mbDynamic = false.
				size_t     mnBlockInfoIndex;   /// The block info to next return. Used if mbDynamic = false.
				BlockInfo  mBlockInfo[1];      /// Recorded block info. Actual size is usually greater than 1. Used if mbDynamic = false.

				Snapshot(size_t nSizeOfThis, int nBlockTypeFlags);
			};

			// Chunk flags
			static const size_type kChunkFlagPrevInUse = 1;                                                           /// Stored in Chunk::mnSize. If set, the previous chunk in memory is in use (allocated).
			static const size_type kChunkFlagMMapped   = 2;                                                           /// Stored in Chunk::mnSize. If set, the chunk uses mapped memory.
			static const size_type kChunkFlagInternal  = 4;                                                           /// Stored in Chunk::mnSize. If set, the chunk is internally allocated and owned.
			static const size_type kChunkFlagFastBin   = (size_type)((size_type)1 << ((sizeof(size_type) * 8) - 1));  /// Stored in Chunk::mnSize. If set, the chunk is in the fastbin cache.
			static const size_type kChunkFlagLargeBin  = (size_type)((size_type)1 << ((sizeof(size_type) * 8) - 2));  /// Stored in Chunk::mnSize. If set, the chunk is in a large bin and participates in skip lists.
			static const size_type kChunkFlagAll       = (kChunkFlagPrevInUse | kChunkFlagMMapped | kChunkFlagInternal | kChunkFlagFastBin | kChunkFlagLargeBin);
			static const size_type kChunkSizeMask      = ~kChunkFlagAll;

			// Chunk constants
			static const size_type kSizeTypeSize                  = sizeof(size_type);                          /// Used for knowing how much space a size_type object takes up.
			static const size_type kSizeTypeMinus1                = ~size_type(0);                              /// 
			static const size_type kChunkInternalHeaderSize       = 2 * kSizeTypeSize;                          /// First two members of Chunk.
			static const size_type kFenceChunkSize                = kChunkInternalHeaderSize;                   /// This is a special fake chunk at the end of core blocks (and thus top chunks) which is there to provide an 'end-cap' of sorts.
			static const size_type kDoubleFenceChunkSize          = 2 * kFenceChunkSize;                        /// This is a special fake chunk at the end of core blocks (and thus top chunks) which is there to provide an 'end-cap' of sorts.
			static const size_type kChunkListPointersSize         = 2 * sizeof(Chunk*);                         /// sizeof(mpPrevChunk) + sizeof(mpNextChunk).
		  #if (PPM_MIN_ALIGN == 16) && (EA_PLATFORM_PTR_SIZE < 8)
			static const size_type kDataPtrOffset                 = sizeof(Chunk);
			static const size_type kMinEffectiveAlignment         = sizeof(Chunk);                              /// Must be an even power of two. Will be <= kAlignmentMin.
		  #elif defined(EA_PLATFORM_ANDROID)
			static const size_type kDataPtrOffset                 = kChunkInternalHeaderSize;
			static const size_type kMinEffectiveAlignment         = kAlignmentMin;                              /// Must be an even power of two. Will be <= kAlignmentMin.
		  #else
			static const size_type kDataPtrOffset                 = 2 * kSizeTypeSize;
			static const size_type kMinEffectiveAlignment         = 2 * kSizeTypeSize;                          /// Must be an even power of two. Will be <= kAlignmentMin.
		  #endif
			static const size_type kMinAlignmentMask              = kMinEffectiveAlignment - 1;                 /// Must be of bit pattern ending with contiguous ones, as with 0x0000003.
			static const size_type kCoreSizeRoundUpMax            = 65536;                                      /// Max amount a requested core size is rounded up.
			static const size_type kMMapSizeRoundUpMax            = 65536;                                      /// Max amount a requested mmap size is rounded up.
			static const size_type kMinChunkSize                  = sizeof(Chunk);                              /// Actual chunks will usually be larger.
			static const size_type kMaxChunkSize                  = kChunkFlagLargeBin - kCoreSizeRoundUpMax;   /// 
			static const size_type kMinAllocSize                  = ((kMinChunkSize + kMinAlignmentMask) & ~kMinAlignmentMask);
			static const size_type kMaxAllocSize                  = (kMaxChunkSize - kMinAllocSize);            /// The largest possible single allocation request size.
			static const size_type kMinCoreSize                   = kMinChunkSize;                              /// The smallest possible size for a CoreBlock's memory. Hard to define at compile time because it depends on the system page size.
			static const size_type kMaxCoreSize                   = kMaxChunkSize - kCoreSizeRoundUpMax;        /// The largest possible size for a CoreBlock's memory. Hard to define at compile time because it depends on the system page size.

			enum Bin // This maps to a signed or unsigned 32 bit value.
			{
				// The following four constants dictate the other constants below.
				kBinCount                      = 128,                                               /// If we change this then we likely will need to tweak some other constants and code.
				kSmallBinSizeIncrement         = 8,                                                 /// Must be a power of two. There is little or no reason to change this.
				kLargeBinMinChunkSize          = 512,                                               /// Chunk size for first large bin.
				kMinSortedBinChunkSize         = 512,                                               /// Size of first bin which whose members are sorted. Sorting is from largest to smallest in the bin.

				kUnsortedBinIndex              = 1,                                                 /// The first bin (i.e. mpBinArray[0]) is invalid and unused.
				kUnsortedBinIndexEnd           = 2,                                                 /// There is only a single unsorted bin.
				kUnsortedBinCount              = kUnsortedBinIndexEnd - kUnsortedBinIndex,          /// 
				kSmallBinIndex                 = kUnsortedBinIndex + kUnsortedBinCount,             /// 
				kSmallBinIndexEnd              = kLargeBinMinChunkSize / kSmallBinSizeIncrement,    /// 
				kSmallBinCount                 = kSmallBinIndexEnd - kSmallBinIndex,                /// bins in the range of [2,64).
				kLargeBinIndex                 = kSmallBinIndex + kSmallBinCount,                   /// 
				kLargeBinIndexEnd              = kBinCount,                                         /// 
				kLargeBinCount                 = kLargeBinIndexEnd - kLargeBinIndex,                /// bins in the range of [64,128).

				kBinPointerCount               = 4,                                                 /// mpPrevChunk, mpNextChunk, mpLargerChunks, mpSmallerChunks. Due to our trick structure we inherited from dlmalloc.
				kBinBitmapShift                = 5,                                                 /// 1 << 5 => 32 bits.
				kBitsPerBinBitmapWord          = 1 << kBinBitmapShift,                              /// 32 bits. We assume sizeof(int) >= 32 bits.
				kBinBitmapWordCount            = kBinCount / kBitsPerBinBitmapWord                  /// This will be 4 for a bin count of 128, as four 32 bit ints hold 128 bits.
			};

			enum FastBin // This maps to a signed or unsigned 32 bit value.
			{
				kFastBinConsolidationThreshold = 65536,                                      /// If a chunk beyond this size is freed, fast bins are consolidated.
				kFastBinDefaultMaxDataSize     = 64,                                         /// Must be no greater than kFastBinMaxDataSize.
				kFastBinMaxDataSize            = 80,                                         /// Must be no greater than the '80' below.
				kFastBinChunkSizeMax           = (2 * kSizeTypeSize) + 80,                   ///  
				kFastBinChunkSizeMin           = kMinChunkSize,                              /// Minimum fast bin chunk size. 
				kFastBinChunkSizeIncrement     = 8,                                          /// Fast bin chunks are 16, 32, 64, etc. in size.
				kFastBinChunkSizeShift         = 3,                                          /// 1 << kFastBinChunkSizeShift => kFastBinChunkSizeIncrement.
				kFastBinIndexMin               = 2,                                          /// Same as: kMinChunkSize / kFastBinChunkSizeIncrement.
				kFastBinCount                  = (kFastBinChunkSizeMax / kFastBinChunkSizeIncrement) - kFastBinIndexMin + 1,
				kFlagFastBinChunksExist        = 1                                           /// Stored in low bit of mnMaxFastBinChunkSize, as the low bits aren't needed.
			};

			enum Default // This maps to a signed or unsigned 32 bit value.
			{
				kDefaultTrimThreshold          = 256 * 1024,                                 /// See enum Option
				kDefaultTopPad                 = 64 * 1024,                                  /// See enum Option
				kDefaultMMapThreshold          = 128 * 1024,                                 /// See enum Option
				kDefaultMMapMaxCount           = 65536,                                      /// See enum Option
				kDefaultMMapAsCoreSize         = 1024 * 1024                                 /// See enum Option
			};

			enum BinType{          /// Normally used to denote where unused chunks are.
				kBinTypeNone,      /// no container
				kBinTypeFast,      /// mpFastBinArray
				kBinTypeUnsorted,  /// mpBinArray's unsorted bin.
				kBinTypeSmall,     /// mpBinArray's small bins.
				kBinTypeLarge,     /// mpBinArray's large bins.
				kBinTypeTop        /// mpTopChunk, which actually isn't any any bin other than itself.
			};

			// Internal versions of main entrypoint functions, which are required if we enable thread safety or allow user allocation hooks.
			//#if (PPM_THREAD_SAFETY_SUPPORTED || PPM_HOOKS_SUPPORTED)
				PPM_VIRTUAL void*  MallocInternal(size_t nSize, int nAllocationFlags = 0);
				PPM_VIRTUAL void*  CallocInternal(size_t nElementCount, size_t nElementSize, int nAllocationFlags = 0);
				PPM_VIRTUAL void*  ReallocInternal(void* p, size_t nNewSize, int nAllocationFlags = 0);
				PPM_VIRTUAL void   FreeInternal(void* p);
				PPM_VIRTUAL void*  MallocAlignedInternal(size_t nAlignment, size_t nSize, size_t nAlignmentOffset, int nAllocationFlags = 0);
			//#endif
			PPM_VIRTUAL void** MallocMultipleInternal(size_t nElementCount, size_t nSizeCount, const size_t nElementSizes[], void* pResultArray[], int nAllocationFlags = 0);

			PPM_VIRTUAL bool       CallMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
			PPM_VIRTUAL Chunk*     ExtendCoreInternal(size_t nMinSize);
			PPM_VIRTUAL Chunk*     AddCoreInternal(size_t nMinSize, bool bRoundUpMinSize, bool bCommitAll);
			PPM_VIRTUAL bool       FreeCore(CoreBlock* pCoreBlock, bool bInShutdown);
			#if PPM_MMAP_SUPPORTED
				PPM_VIRTUAL void* MMapMalloc(size_t nSize);
				PPM_VIRTUAL void* MMapAllocInternal(size_t nSize, size_t* pSizeAllocated = NULL);
				PPM_VIRTUAL void  MMapFreeInternal(void* pAddress, size_t nSize);
			#endif

			PPM_VIRTUAL void ClearFastBins();

			// Misc functionality
			static bool       GetIsMinAligned(void* pAddress);
			static bool       RequestIsWithinRange(size_t nBytes);
			bool              GetCoreIsContiguous() const;
			void              AdjustHighFence();
			void              SetNewTopChunk(Chunk* pChunk, bool bFreePreviousTopChunk);
			void              AdjustTopChunk(Chunk* pChunk, size_type nChunkSize);
			bool              ChunkMatchesLowHigh(int bHigh, const Chunk* pChunk, const size_type nChunkSize) const;
			static int        ChunkMatchesBlockType(const Chunk* pChunk, int nBlockTypeFlags);
			static void       SetFencepostAfterChunk(const Chunk* pChunk, size_type nChunkSize);
		  //static void       AddFencepost(Chunk* pChunk, size_type nPrevChunkFlags);
			static void       AddDoubleFencepost(Chunk* pChunk, size_type nPrevChunkFlags);
			static Chunk*     MakeChunkFromCore(void* pCore, size_t nCoreSize, size_type nFlags);
			void              GetBlockInfoForCoreBlock(const CoreBlock* pCoreBlock, BlockInfo* pBlockInfo) const;
			void              GetBlockInfoForChunk(const Chunk* pChunk, BlockInfo* pBlockInfo, const void* pCore) const;

			// Chunk functionality
			static void*      GetPostHeaderPtrFromChunkPtr(const Chunk* pChunk);
			static void*      GetPostHeaderPtrFromSkipListChunkPtr(const SkipListChunk* pSkipListChunk);
			static void*      GetDataPtrFromChunkPtr(const Chunk* pChunk);
			static Chunk*     GetChunkPtrFromDataPtr(const void* pData);
			static size_type  GetChunkSizeFromDataSize(size_type nDataSize);
			static size_type  GetChunkSizeFromDataSizeChecked(size_type nDataSize);
				   size_type  GetMMapChunkSizeFromDataSize(size_type nDataSize);
			static Chunk*     GetMMapChunkFromMMapListChunk(const Chunk* pChunk);
			static size_type  GetChunkIsInternal(const Chunk* pChunk);
			static void       SetChunkIsInternal(Chunk* pChunk);
			static void       ClearChunkIsInternal(Chunk* pChunk);
			static size_type  GetChunkIsFastBin(const Chunk* pChunk);
			static void       SetChunkIsFastBin(Chunk* pChunk);
			static void       ClearChunkIsFastBin(Chunk* pChunk);
			static size_type  GetChunkIsLargeBin(const Chunk* pChunk);
			static void       SetChunkIsLargeBin(Chunk* pChunk);
			static void       ClearChunkIsLargeBin(Chunk* pChunk);
			static size_type  GetChunkIsMMapped(const Chunk* pChunk);
			static size_type  GetPrevChunkIsInUse(const Chunk* pChunk);
			static size_type  GetChunkSize(const Chunk* pChunk);
			static size_type  GetUsableChunkSize(const Chunk* pChunk);
			static Chunk*     GetNextChunk(const Chunk* pChunk);
			static Chunk*     GetPrevChunk(const Chunk* pChunk);
			static Chunk*     GetChunkAtOffset(const Chunk* pChunk, ssize_type nOffset);
			static size_type  GetChunkIsInUse(const Chunk* pChunk);
			static void       SetChunkIsInUse(Chunk* pChunk);
			static void       ClearChunkInUse(Chunk* pChunk);
			static size_type  GetChunkInUseOffset(const Chunk* pChunk, ssize_type nOffset);
			static void       SetChunkInUseOffset(Chunk* pChunk, ssize_type nOffset);
			static void       ClearChunkInUseOffset(Chunk* pChunk, ssize_type nOffset);
			static void       SetChunkSize(Chunk* pChunk, size_type nSize);
			static void       SetChunkSizePreserveFlags(Chunk* pChunk, size_type nSize);
			static size_type  GetNextChunkPrevSize(const Chunk* pChunk, size_type nSize);
			static void       SetNextChunkPrevSize(Chunk* pChunk, size_type nSize);
			static void       LinkChunk(Chunk* pChunk, Chunk* pPrev, Chunk* pNext);
			static void       UnlinkChunk(const Chunk* pChunk);             // Unlinks a chunk's mpNext/mpPrev from whatever list it is in (unsorted list, small bins, large bins, delayed free list).
			void              UnlinkChunkFromBin(Chunk* pChunk);            // Same as UnlinkChunk but also removes Chunk from large bin skip list if it's in a large bin.
			Chunk*            FindPriorChunk(const Chunk* pChunk) const;
			Chunk*            GetInitialTopChunk() const;
			Chunk*            FindAndSetNewTopChunk();
			bool              GetChunkIsListHead(const Chunk* pChunk) const;
			bool              GetChunkIsFenceChunk(const Chunk* pChunk) const;
			static Chunk*     GetFenceChunk(const CoreBlock* pCoreBlock);
			BinType           FindChunkBin(const Chunk* pChunk, bool bAssumeValid) const;
			bool              GetChunkIsInBin(const Chunk* pChunk, const Chunk* pBin) const;
			void              PlaceUnsortedChunkInBin(Chunk* pChunk, size_type nChunkSize, bool bRemoveFromUnsortedBin);
			Chunk*            SplitChunkAligned(Chunk* pChunk, size_type nChunkSize, void* pResultPosition, size_type nRequestedChunkSize);
			Chunk*            SplitChunkLead(Chunk* pChunk, size_type nChunkSize, Chunk* pChunkNew);
			void              SplitChunkTail(Chunk* pChunk, bool bChunkIsInBin, size_type nChunkSize, size_type nLeadingSize);
			Chunk*            GetLowestAlignedChunkPosition        (Chunk* pChunk,                  size_t nAlignment, size_t nAlignmentOffset);
			void*             GetAlignedChunkPositionChecked       (Chunk* pChunk, size_type nSize, size_t nAlignment, size_t nAlignmentOffset, void*& pHighPosition);
			Chunk*            GetHighestAlignedChunkPosition       (Chunk* pChunk, size_type nSize, size_t nAlignment, size_t nAlignmentOffset);
			Chunk*            GetHighestAlignedChunkPositionChecked(Chunk* pChunk, size_type nSize, size_t nAlignment, size_t nAlignmentOffset);

			// Bin functionality
			SkipListChunk*          GetBin(int nIndex) const;
			int                     GetBinIndex(const SkipListChunk* pBin) const;         // This is the inverse of GetBin(int nIndex)
			SkipListChunk*          GetUnsortedBin() const;
			bool                    IsBinLarge(const SkipListChunk* pBin) const;          // Returns true if pBin is a 'large bin'. Not the same as asking if the bin as a lot of entries in it.
			static SkipListChunk*   GetFirstChunkInBin(const SkipListChunk* pBin);
			static SkipListChunk*   GetLastChunkInBin(const SkipListChunk* pBin);
			static SkipListChunk*   GetNextBin(SkipListChunk* pBin);
			static bool             SizeIsWithinSmallBinRange(size_type nSize);
			static bool             SizeIsWithinLargeBinRange(size_type nSize);
		  //static bool             ChunkGoesIntoLargeBin(Chunk* pChunk);
		  //bool                    IsChunkInLargeBin(Chunk* pChunk);
			static int              GetSmallBinIndexFromChunkSize(size_type nSize);
			static int              GetLargeBinIndexFromChunkSize(size_type nSize);
			static int              GetBinIndexFromChunkSize(size_type nSize);
			static int              GetBinBitmapWordFromBinIndex(int nBinIndex);
			static int              GetBinBitmapBitFromBinIndex(int nBinIndex);
			void                    MarkBinBitmapBit(int nBinIndex);
			void                    UnmarkBinBitmapBit(int nBinIndex);
			int                     GetBinBitmapBit(int nBinIndex) const;

			// FastBin functionality
			static unsigned   GetFastBinIndexFromChunkSize(size_type nSize);
			size_type         GetFastBinChunksExist() const;
			void              SetFastBinChunksExist(bool bExist);
			unsigned          GetMaxFastBinChunkSize() const;
			void              SetMaxFastBinDataSize(size_type nSize);

			// Core blocks
			PPM_VIRTUAL void       LinkCoreBlock(CoreBlock* pCoreBlock, CoreBlock* pNext);
			PPM_VIRTUAL void       UnlinkCoreBlock(CoreBlock* pCoreBlock);
			PPM_VIRTUAL CoreBlock* FindCoreBlockForAddress(const void* pAddress) const;

			// Debug functionality
			PPM_VIRTUAL int             CheckChunkRegularBin(const Chunk* pChunk);
			PPM_VIRTUAL int             CheckChunk(const Chunk* pChunk);
			PPM_VIRTUAL int             CheckFreeChunk(const Chunk* pChunk);
			PPM_VIRTUAL int             CheckUsedChunk(const Chunk* pChunk);
			PPM_VIRTUAL int             CheckRemallocedChunk(const Chunk* pChunk, size_type nRequestedChunkSize);
			PPM_VIRTUAL int             CheckMallocedChunk(const Chunk* pChunk, size_type nRequestedChunkSize, bool bNewlyMalloced, bool bAllowPrevNotInUse);
			PPM_VIRTUAL int             CheckMMappedChunk(const Chunk* pChunk);
			PPM_VIRTUAL int             CheckState(HeapValidationLevel heapValidationLevel);
			PPM_VIRTUAL size_t          DescribeChunk(const Chunk* pChunk, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const;
			PPM_VIRTUAL DataPreviewType GetDataPreview(const void* pData, size_t nDataSize, char* pBuffer, wchar_t* pBufferW, size_t nBufferLength) const;
			static void                 AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);

			// Default Core Memory methods:
			void* DefaultAddCoreMemoryFunction(size_t nMinSize, bool bCommitAll, size_t& nReservedSize, size_t&  nCoreSize, uint32_t& coreType, bool& bPageExecute);

			void* DefaultCoreExtendFunction(CoreBlock* pCoreBlock, size_t nSize, size_t& nExtendedSize);

			bool DefaultCoreFreeFunction(CoreBlock* pCoreBlock);

		protected:
			// General state
			AtomicUint       mEventId;                                  /// Puts a number on every Malloc, Free, Realloc, etc call the user does.
			bool             mbInitialized;                             /// True if we've been initialized.
			#if PPM_FASTBINS_ENABLED
			size_type        mnMaxFastBinChunkSize;                     /// The maximum chunk size to be eligible for FastBin. Low 2 bits used as flags.
			Chunk*           mpFastBinArray[kFastBinCount];             /// FastBins. Like bins but chunks are neither broken up nor coalesced. This is a singly-linked list which uses Chunk::mpNextChunk and not mpPrevChunk. The final Chunk* in the list (or the only chunk in the case of an empty list) is NULL.
			#endif
			SkipListChunk*   mpBinArray[kBinCount * kBinPointerCount];  /// Normal bins, each sorted from largest to smallest Chunk. '*4' because due to this trick structure we are making each entry in the struct also act like the four SkipListChunk pointers. See http://stackoverflow.com/questions/2731806/bin-at-in-dlmalloc for an analagous description which applies to open source ptmalloc.
			unsigned         mBinBitmap[kBinBitmapWordCount];           /// Bitmap of bins, for fast checking.
			Chunk*           mpTopChunk;                                /// Base of the topmost chunk -- not otherwise kept in a bin.
			Chunk*           mpLastRemainderChunk;                      /// The remainder from the most recent split of a small request.
			CoreBlock        mHeadCoreBlock;                            /// Head of linked list of blocks of core memory.
			void*            mpHighFence;                               /// Address beyond which is considered 'high' memory as for when the user asks to allocate high. If NULL, then high vs. low memory distinction is disabled.
			bool             mbHighFenceInternallyDisabled;             /// If true, then the high/low fence was internally disabled and should not be enabled again.
			bool             mbSystemAllocEnabled;                      /// If true, then we use SystemAlloc to retrieve new memory when core is exhausted.
			bool             mbUnsortedBinEnabled;                      /// If true, then we enable the 'unsorted bin' cache of recently freed blocks. Enabling it allows for faster frees but can cause some memory patterns to be slow if the cache gets large.
			bool             mbLockDuringHookCalls;                     /// See option kOptionLockDuringHookCalls. Specifies if our mutex remains locked during Hook callbacks. Defaults to unlocked. It is *safer* to be unlocked, as being locked during a hook call invites a potential deadlock if the user isn't careful.
			bool             mbExecutableCore;                          /// See kOptionExecutableCore.
			bool             mbMallocFailureAssertEnabled;              /// See kOptionEnableMallocFailureAssert.
			bool			 mbTrimToZero;								/// See kOptionEnableTrimToZero.
			int              mnCheckChunkReentrancyCount;               /// 
			unsigned char    mcTraceFieldDelimiter;                     /// Delimits fields in an allocation trace. Defaults to '\t'.
			unsigned char    mcTraceRecordDelimiter;                    /// Delimits records in an allocation trace. Defaults to '\n'.

			// Auto heap validation
			HeapValidationLevel mAutoHeapValidationLevel;       /// Used to indicate a heap validation level or to indicate that heap validation is disabled.
			size_t              mnAutoHeapValidationFrequency;  /// This is a value from 1 to N, where 1 means to validate every frame. 
			size_t              mnAutoHeapValidationEventCount; /// This is the current number of events to consider for validation. 
			bool                mbHeapValidationActive;         /// Used to prevent recursion.

			// Memory map support
			int                 mnMMapCount;
			size_t              mnMMapMallocTotal;
			int                 mnMMapMaxAllowed;
			size_type           mnMMapThreshold;                /// Minimum size for a requested chunk before we use mapped memory to implement it.
			bool                mbMMapTopDown;                  /// If true, attempt to allocate mapped memory from the top of address space.
			Chunk               mHeadMMapChunk;                 /// Head of linked list of known chunks. Exists so that we know all existing mmapped chunks.

			CoreAddFunction     mCoreAddFunction;               /// Callback to be called when new core needs to be allocated.
			void*               mpCoreAddFunctionContext;       /// This is passed to the user callback.

			// Callback
			#if PPM_HOOKS_SUPPORTED
			HookFunction        mpHookFunction; 
			void*               mpHookFunctionContext;
			#endif

			MallocFailureFunction mpMallocFailureFunction;
			void*                 mpMallocFailureFunctionContext;
			unsigned              mnMaxMallocFailureCount;      /// Max number of times to call the MallocFailure function successively.

			AssertionFailureInfoFunction mpAssertionFailureFunction;
			void*                        mpAssertionFailureFunctionContext;

			TraceFunction       mpTraceFunction;
			void*               mpTraceFunctionContext;

			// Misc
			size_type           mnTrimThreshold;        /// If there is this much or more free memory at the top of a core block, trim the core block. Applies only to platforms with core blocks that are memory mapped, such as Win32 and Unix.
			size_type           mnTopPad;               /// 
			char*               mpInitialTopChunk;      /// 
			size_t              mnPageSize;             /// Size of 'system' page; an internal constant. All system core allocations are rounded up to this amount.
			size_t              mnMMapPageSize;         /// Size of pages for mapped memory. May be different from mnPageSize.
			size_t              mnNewCoreSize;          /// Size of new core memory blocks when they are automatically internally allocated.
			size_t              mnCoreIncrementSize;    /// Size to add to core memory blocks when the system supports extending existing blocks.
			bool                mbTraceInternalMemory;  /// If enabled, then TraceAllocatedMemory traces internal memory. Default is disabled since it might confuse the user by reporting leaks that aren't really leaks.
			bool                mbCoreTopDown;          /// If enabled, new core memory is allocated top-down from the system.

			size_t				mPhysicalFootprint;		/// The amount of memory currently reserved from the system by the allocator.
			size_t				mNumCores;				/// The amount of cores currently allocated to the allocator.

			// Thread safety
			#if PPM_THREAD_SAFETY_SUPPORTED
				void*    mpMutex;                       /// Pointer to mutex or NULL if thread safety is disabled.
				uint64_t mpMutexData[kMutexBufferSize]; /// Provides memory from which the mutex is allocated (so we don't have to allocate it from a heap). Format of this data is platform-specific.
			#endif

			// Debug fill values
			// We don't put these declarations within an '#if PPM_DEBUG_FILL' statement,
			// as that would complicate the implementation of GeneralAllocatorDebug too 
			// much to be worth the 20 bytes that the values below take up.
			unsigned char mnFillFree;
			unsigned char mnFillDelayedFree;
			unsigned char mnFillNew;
			unsigned char mnFillGuard;
			unsigned char mnFillUnusedCore;

			// Name
			const char* mpName;

			// Init callback
			enum NotifyInitState
			{
				kNotifyInitStateInit,
				kNotifyInitStateShutdown
			};
			int8_t mNotifyInitState;

			static volatile InitCallbackNode* mpInitCallbackNode;

			MemoryRangeCallbackFunction mRangeCallback;
			void* mRangeUserData;


		private:

			/// Common constructor function 
			void Construct(const Parameters& parameters);

		}; // GenearlAllocator

	} // namespace Allocator

} // namespace EA



namespace EA
{
	 namespace Allocator
	 {
		  /// SetGeneralAllocator takes an instance of a GeneralAllocator
		  /// for use by the EAFixedAllocator and EAStackAllocator. With a
		  /// statically-linked library, you can declare a global instance
		  /// of the allocator and set the global variable
		  /// EA::Allocator::gpEAGeneralAllocator to that instance. However,
		  /// this is not possible in a DLL, so we instead declare the 
		  /// gpEAGeneralAllocator inside the library and provide a set
		  /// function which can be called by the application.
		  /// An obvious implication of this is that it is not possible to
		  /// your GeneralAllocator to allocate memory for objects that are
		  /// instantiated by a global constructor.
		  PPM_API void SetGeneralAllocator(GeneralAllocator* pAllocator);
	 }
}



///////////////////////////////////////////////////////////////////////////////
// Inlines
//
// Here we define various inlines that are appropriate due to their very 
// small and simple nature. To be a candiate for inlining, we follow the 
// general rule that the function must be <= five lines of code and those
// lines must be reasonably amenable to inlining. This allocator is built
// upon these inlines and for high performance you definitely want to 
// compile allocator source code with inlining enabled. It will not only
// make the code faster, but it will make it smaller as well.
//
namespace EA
{
	namespace Allocator
	{
		inline 
		bool GeneralAllocator::IsConstructed() const
		{
			return mbInitialized; // The constructor calls the Init function, so mbInitialized is true if the constructor was called or if Init was called.
		}


		inline 
		const char* GeneralAllocator::GetName() const
		{
			return mpName;
		}


		inline 
		void GeneralAllocator::SetName(const char* pName)
		{
			mpName = pName;
		}


		inline size_t GeneralAllocator::GetMaxMallocSize() const
		{
			return (size_t)kMaxAllocSize;
		}

		inline
		size_t GeneralAllocator::GetMaxCoreSize() const
		{
			return (size_t)kMaxCoreSize;
		}

		inline 
		bool GeneralAllocator::AddCore(void* pCore, size_t nSize, bool bShouldFreeCore, bool bShouldTrimCore, 
									   CoreFreeFunction pCoreFreeFunction, void* pCoreFreeFunctionContext,
									   CoreExtendFunction pCoreExtendFunction, void* pCoreExtendFunctionContext)
		{
			return AddCoreEx(pCore, nSize, kCoreTypeDefault, nSize, bShouldFreeCore, bShouldTrimCore, 
							 pCoreFreeFunction, pCoreFreeFunctionContext, pCoreExtendFunction, pCoreExtendFunctionContext);
		}

		inline 
		bool GeneralAllocator::AddCore(const Parameters& parameters)
		{
			return AddCoreEx(parameters.mpCore, parameters.mnCoreSize, kCoreTypeDefault, parameters.mnReservedSize, parameters.mbShouldFreeCore, parameters.mbShouldTrimCore, 
							 parameters.mpCoreFreeFunction, parameters.mpCoreFreeContext, parameters.mpCoreExtendFunction, parameters.mpCoreExtendContext);
		}

		#if !PPM_THREAD_SAFETY_SUPPORTED
			inline
			void GeneralAllocator::Lock(bool /*bEnable*/)
			{
				// Empty
			}
		#endif


		inline
		bool GeneralAllocator::GetIsMinAligned(void* pAddress)
		{
			return (((uintptr_t)pAddress & kMinAlignmentMask) == 0);
		}


		inline
		bool GeneralAllocator::RequestIsWithinRange(size_t nSize)
		{
			return (nSize < kMaxAllocSize);
		}


		inline
		void* GeneralAllocator::GetPostHeaderPtrFromChunkPtr(const Chunk* pChunk)
		{
			return (char*)pChunk + sizeof(Chunk);
		}

		inline
		void* GeneralAllocator::GetPostHeaderPtrFromSkipListChunkPtr(const SkipListChunk* pSkipListChunk)
		{
			return (char*)pSkipListChunk + sizeof(SkipListChunk);
		}

		inline 
		void* GeneralAllocator::GetDataPtrFromChunkPtr(const Chunk* pChunk)
		{
			return (char*)pChunk + kDataPtrOffset;
		}


		inline 
		GeneralAllocator::Chunk* GeneralAllocator::GetChunkPtrFromDataPtr(const void* pData)
		{
			return (Chunk*)(uintptr_t)((uintptr_t)pData - kDataPtrOffset);
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkSizeFromDataSize(size_type nDataSize)
		{
			#if PPM_DEBUG_PRESERVE_PRIOR
				// In debug mode, we preserve the 'mnPriorSize' field of chunks after 
				// the allocated chunk in memory.
				const size_type sum = (size_type)(nDataSize + kDataPtrOffset + kMinAlignmentMask);
			#else
				// Note that we add kSizeTypeSize here. The reason for this is that each 
				// chunk's 'mnPriorSize' field (its first member) is  not used to store 
				// the prior size when the prior chunk is in use but rather its bytes are 
				// used by the prior chunk as its last allocated bytes. This allows us to 
				// save a some memory at runtime, though the way it packs like this can 
				// be a little confusing at first.
				const size_type sum = (size_type)(nDataSize + (kDataPtrOffset - kSizeTypeSize) + kMinAlignmentMask);
			#endif

			if(sum > kMinChunkSize)
				return (sum & ~kMinAlignmentMask);
			return kMinChunkSize;
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetMMapChunkSizeFromDataSize(size_type nDataSize)
		{
			// For mmapped chunks, the overhead is one kSizeTypeSize unit larger 
			// than for normal chunks, because there is no following chunk whose 
			// prev_size field could be used.
			//
			// We add kMinChunkSize because we put a phony chunk at the end of
			// the returned chunk in order to allow us to put this block in our 
			// list of chunks, for tracking. kMinChunkSize is usually 16 bytes
			// and MMapped chunks are usually very large (100K+) so we don't 
			// consider the 16 bytes to be much of an addition.
			//
			// We add kMinAlignmentMask because its possible that alignment 
			// requirements will push our mmapped memory requirement a little
			// higher. Given that we already round up to the next page, this 
			// addition will usually have no effect.
			//
			// We do '+ kPageMask' and '& ~kPageMask' to round up to the nearest page.

			const size_t kPageMask(mnMMapPageSize - 1);
			return (size_type)((kChunkInternalHeaderSize + nDataSize + kMinChunkSize + kMinAlignmentMask + kPageMask) & ~kPageMask);
		}
		 

		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkSizeFromDataSizeChecked(size_type nDataSize)
		{
			if(RequestIsWithinRange(nDataSize))
				return GetChunkSizeFromDataSize(nDataSize);
			return 0;
		}


		inline
		unsigned GeneralAllocator::GetFastBinIndexFromChunkSize(size_type nChunkSize)
		{
			return (unsigned)((nChunkSize >> kFastBinChunkSizeShift) - kFastBinIndexMin);
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetPrevChunkIsInUse(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkFlagPrevInUse);
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetMMapChunkFromMMapListChunk(const Chunk* pChunk)
		{
			// Recall that we store our mmapped chunks in a list using an empty chunk
			// at the end of the mmapped memory. We do this because our normal list member 
			// pointers are unavailable for use.
			return (Chunk*)((uintptr_t)pChunk - pChunk->mnPriorSize);
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkIsMMapped(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkFlagMMapped);
		}

		
		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkIsInternal(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkFlagInternal);
		}


		inline
		void GeneralAllocator::SetChunkIsInternal(Chunk* pChunk)
		{
			pChunk->mnSize |= kChunkFlagInternal;
		}


		inline
		void GeneralAllocator::ClearChunkIsInternal(Chunk* pChunk)
		{
			pChunk->mnSize &= ~kChunkFlagInternal;
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkIsFastBin(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkFlagFastBin);
		}


		inline
		void GeneralAllocator::SetChunkIsFastBin(Chunk* pChunk)
		{
			pChunk->mnSize |= kChunkFlagFastBin;
		}


		inline
		void GeneralAllocator::ClearChunkIsFastBin(Chunk* pChunk)
		{
			pChunk->mnSize &= ~kChunkFlagFastBin;
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkIsLargeBin(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkFlagLargeBin);
		}


		inline
		void GeneralAllocator::SetChunkIsLargeBin(Chunk* pChunk)
		{
			pChunk->mnSize |= kChunkFlagLargeBin;
		}


		inline
		void GeneralAllocator::ClearChunkIsLargeBin(Chunk* pChunk)
		{
			pChunk->mnSize &= ~kChunkFlagLargeBin;
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetChunkSize(const Chunk* pChunk)
		{
			return (pChunk->mnSize & kChunkSizeMask);
		}


		/// GetUsableChunkSize
		/// Returns the amount of memory in the chunk which is usable. In an optimized
		/// shipping build (i.e. PPM_DEBUG_PRESERVE_PRIOR == 0) the user's space runs
		/// into the first size_type (usually 4) bytes of the next contiguous chunk.
		/// This function can only be properly used with allocated chunks and not 
		/// freed chunks. Note that this is not the same thing as GetUsableSize, which 
		/// itself is a user-level function.
		inline
		GeneralAllocator::size_type GeneralAllocator::GetUsableChunkSize(const Chunk* pChunk)
		{
			#if PPM_DEBUG_PRESERVE_PRIOR
				return GetChunkSize(pChunk);
			#else
				// In this case, the user's space runs size_type bytes into the next chunk.
				// Except for MMapped chunks, whose user space never runs into the next chunk,
				// as it always needs that space, even in an optimized build.
				if(!GetChunkIsMMapped(pChunk)) // Do negative logic here because that's most branch-prediction-savvy in this case.
					return GetChunkSize(pChunk) + kSizeTypeSize;
				return GetChunkSize(pChunk);
			#endif
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetNextChunk(const Chunk* pChunk)
		{
			return (Chunk*)(uintptr_t)((uintptr_t)pChunk + (pChunk->mnSize & kChunkSizeMask));
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetPrevChunk(const Chunk* pChunk)
		{
			#if !PPM_DEBUG_PRESERVE_PRIOR
				// The prior chunk owns our mnPriorSize field when it is in use, 
				// so in that case it would be unreadable. In PPM_DEBUG_PRESERVE_PRIOR 
				// mode, the prior size field is preserved, so there is no need to assert.
				PPM_ASSERT_STATIC(!GetPrevChunkIsInUse(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			#endif

			return (Chunk*)((uintptr_t)pChunk - pChunk->mnPriorSize);
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetChunkAtOffset(const Chunk* pChunk, ssize_type nOffset)
		{
			return (Chunk*)((uintptr_t)pChunk + nOffset);
		}


		inline 
		GeneralAllocator::size_type GeneralAllocator::GetChunkIsInUse(const Chunk* pChunk)
		{  
			const Chunk* const pNextChunk = (Chunk*)(uintptr_t)((uintptr_t)pChunk + (pChunk->mnSize & kChunkSizeMask));
			return (pNextChunk->mnSize & kChunkFlagPrevInUse);
		}


		inline 
		void GeneralAllocator::SetChunkIsInUse(Chunk* pChunk)
		{
			// Note that to get a given chunk's in-use flag, we need to check the following chunk's 'prev in use' flag.
			((Chunk*)(uintptr_t)((uintptr_t)pChunk + (pChunk->mnSize & kChunkSizeMask)))->mnSize |= kChunkFlagPrevInUse;
		}


		inline void GeneralAllocator::ClearChunkInUse(Chunk* pChunk)
		{
			// Note that to get a given chunk's in-use flag, we need to check the following chunk's 'prev in use' flag.
			((Chunk*)(uintptr_t)((uintptr_t)pChunk + (pChunk->mnSize & kChunkSizeMask)))->mnSize &= (size_t)~kChunkFlagPrevInUse;
		}


		inline 
		GeneralAllocator::size_type GeneralAllocator::GetChunkInUseOffset(const Chunk* pChunk, ssize_type nOffset)
		{
			return (((Chunk*)((uintptr_t)pChunk + nOffset))->mnSize & kChunkFlagPrevInUse);
		}


		inline 
		void GeneralAllocator::SetChunkInUseOffset(Chunk* pChunk, ssize_type nOffset)
		{
			(((Chunk*)((uintptr_t)pChunk + nOffset))->mnSize |= kChunkFlagPrevInUse);
		}


		inline 
		void GeneralAllocator::ClearChunkInUseOffset(Chunk* pChunk, ssize_type nOffset)
		{
			(((Chunk*)((uintptr_t)pChunk + nOffset))->mnSize &= ~kChunkFlagPrevInUse);
		}


		inline
		void GeneralAllocator::SetChunkSize(Chunk* pChunk, size_type nSize)
		{
			pChunk->mnSize = nSize;
		}


		inline
		void GeneralAllocator::SetChunkSizePreserveFlags(Chunk* pChunk, size_type nSize)
		{
			// This function intentionally doesn't mask nSize to kChunkSizeMask.
			// PPM_ASSERT(this, (nSize & (kChunkFlagPrevInUse | kChunkFlagMMapped)) == 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, NULL);
			pChunk->mnSize = ((pChunk->mnSize & kChunkFlagAll) | nSize);
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetNextChunkPrevSize(const Chunk* pChunk, size_type nSize)
		{
			return ((Chunk*)((uintptr_t)pChunk + nSize))->mnPriorSize;
		}


		inline
		void GeneralAllocator::SetNextChunkPrevSize(Chunk* pChunk, size_type nSize)
		{
			((Chunk*)((uintptr_t)pChunk + nSize))->mnPriorSize = nSize;
		}


		inline
		void GeneralAllocator::LinkChunk(Chunk* pChunk, Chunk* pPrev, Chunk* pNext)
		{
			pChunk->mpPrevChunk = pPrev;
			pChunk->mpNextChunk = pNext; 
			pPrev->mpNextChunk  = pChunk;
			pNext->mpPrevChunk  = pChunk; 
		}


		inline
		void GeneralAllocator::UnlinkChunk(const Chunk* pChunk)
		{
			pChunk->mpPrevChunk->mpNextChunk = pChunk->mpNextChunk;
			pChunk->mpNextChunk->mpPrevChunk = pChunk->mpPrevChunk;
			#if (PPM_DEBUG >= 2) // Extended user debug builds.
				const_cast<Chunk*>(pChunk)->mpNextChunk = NULL; // We do this for ease of data inspection during debugging.
				const_cast<Chunk*>(pChunk)->mpPrevChunk = NULL; // 
			#endif
		}


		inline
		GeneralAllocator::SkipListChunk* GeneralAllocator::GetFirstChunkInBin(const SkipListChunk* pBin)
		{
			return (SkipListChunk*)(uintptr_t)pBin->mpNextChunk;   // We can downcast to SkipListChunk because all chunks in Bins are treated as SkipListChunks.
		}


		inline
		GeneralAllocator::SkipListChunk* GeneralAllocator::GetLastChunkInBin(const SkipListChunk* pBin)
		{
			return (SkipListChunk*)(uintptr_t)pBin->mpPrevChunk;   // We can downcast to SkipListChunk because all chunks in Bins are treated as SkipListChunks.
		}

		inline
		GeneralAllocator::SkipListChunk* GeneralAllocator::GetNextBin(SkipListChunk* pBin)
		{   // '* kBinPointerCount' because due to this trick structure we are making each entry in the struct also act like the four SkipListChunk pointers. See http://stackoverflow.com/questions/2731806/bin-at-in-dlmalloc for an analagous situation in ptmalloc.
			return (SkipListChunk*) ((uintptr_t)pBin + (sizeof(SkipListChunk*) * kBinPointerCount)); 
		}


		inline
		GeneralAllocator::SkipListChunk* GeneralAllocator::GetBin(int nIndex) const
		{   // '* kBinPointerCount' because due to this trick structure we are making each entry in the struct also act like the four SkipListChunk pointers. See http://stackoverflow.com/questions/2731806/bin-at-in-dlmalloc for an analagous situation in ptmalloc.
			// '- EA_OFFSETOF(Chunk, mpPrevChunk)' because the design of the bin array is such that each element is aligned so that initially its mpPrev and mpNext point to self.
			// An nIndex of 0 is invalid. We don't use an assert here because this function is called so much and it would harm debug builds too much.
			// Index 1 happens to be the 'unsorted bin'. Successive bins are the 'small bins', and then the last bins in the array are the 'large bins.'
			return (SkipListChunk*)(uintptr_t)((uintptr_t)(mpBinArray + (nIndex * kBinPointerCount)) - EA_OFFSETOF(SkipListChunk, mpPrevChunk));
		}


		inline
		int GeneralAllocator::GetBinIndex(const SkipListChunk* pBin) const
		{
			// This is the inverse of GetBin above.
			return (int)((((uintptr_t)pBin + EA_OFFSETOF(SkipListChunk, mpPrevChunk)) - (uintptr_t)mpBinArray) / (sizeof(SkipListChunk*) * kBinPointerCount));
		}


		inline
		bool GeneralAllocator::SizeIsWithinSmallBinRange(size_type nSize)
		{
			return nSize < (size_type)kLargeBinMinChunkSize;
		}


		inline
		bool GeneralAllocator::SizeIsWithinLargeBinRange(size_type nSize)
		{
			return nSize >= (size_type)kLargeBinMinChunkSize;
		}

		/*
		inline
		bool GeneralAllocator::ChunkGoesIntoLargeBin(Chunk* pChunk) // Would the Chunk go into the large bin when freed?
		{
			return GetChunkSize(pChunk) >= (size_type)kLargeBinMinChunkSize;
		}

		bool GeneralAllocator:IsChunkInLargeBin(Chunk* pChunk)
		{
			const size_type nSize = GetChunkSize(pChunk);

			if(nSize >= (size_type)kLargeBinMinChunkSize) // Quick size check. I think we can also do a mmap check if in fact mmap chunks can never be in a (large) bin.
			{
				const int    index = GetLargeBinIndexFromChunkSize(nSize);
				const Chunk* pBin  = GetBin(index);

				return GetChunkIsInBin(pChunk, pBin);
			}

			return false;
		}
		*/

		inline
		int GeneralAllocator::GetSmallBinIndexFromChunkSize(size_type nSize)
		{
			return (int)(nSize / kSmallBinSizeIncrement); // Compiler will optimize this to a right shift, as kSmallBinSizeIncrement will be a power of 2.
		}


		inline
		int GeneralAllocator::GetBinIndexFromChunkSize(size_type nSize)
		{
			if(SizeIsWithinSmallBinRange(nSize))
				return GetSmallBinIndexFromChunkSize(nSize);
			return GetLargeBinIndexFromChunkSize(nSize);
		}



		inline
		GeneralAllocator::SkipListChunk* GeneralAllocator::GetUnsortedBin() const
		{  // The otherwise unindexable 1-bin is used to hold unsorted chunks.
			return GetBin(1);
		}


		inline
		bool GeneralAllocator::IsBinLarge(const SkipListChunk* pBin) const
		{
			return (GetBinIndex(pBin) >= kLargeBinIndex);
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetInitialTopChunk() const
		{
			return GetUnsortedBin();
		}


		inline
		bool GeneralAllocator::GetChunkIsListHead(const Chunk* pChunk) const
		{
			return (((uintptr_t)pChunk >= (uintptr_t)mpBinArray) && ((uintptr_t)pChunk < ((uintptr_t)mpBinArray + sizeof(mpBinArray))));
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetFenceChunk(const CoreBlock* pCoreBlock)
		{
			return (Chunk*)(uintptr_t)(((uintptr_t)pCoreBlock + pCoreBlock->mnSize) - kDoubleFenceChunkSize);
		}


		inline
		int GeneralAllocator::GetBinBitmapWordFromBinIndex(int nBinIndex)
		{
			return (nBinIndex >> kBinBitmapShift);
		}

		inline
		int GeneralAllocator::GetBinBitmapBitFromBinIndex(int nBinIndex)
		{
			return (1 << (nBinIndex & ((1 << kBinBitmapShift) - 1)));
		}


		inline
		void GeneralAllocator::MarkBinBitmapBit(int nBinIndex)
		{
			const int nWord = GetBinBitmapWordFromBinIndex(nBinIndex);
			const int nBit  = GetBinBitmapBitFromBinIndex(nBinIndex);
			mBinBitmap[nWord] |= (unsigned)nBit;
		}


		inline
		void GeneralAllocator::UnmarkBinBitmapBit(int nBinIndex)
		{
			const int nWord = GetBinBitmapWordFromBinIndex(nBinIndex);
			const int nBit  = GetBinBitmapBitFromBinIndex(nBinIndex);
			mBinBitmap[nWord] &= (unsigned)~nBit;
		}


		inline
		int GeneralAllocator::GetBinBitmapBit(int nBinIndex) const
		{
			const int nWord = GetBinBitmapWordFromBinIndex(nBinIndex);
			const int nBit  = GetBinBitmapBitFromBinIndex(nBinIndex);
			return (int)(mBinBitmap[nWord] & (unsigned)nBit);
		}


		inline
		GeneralAllocator::size_type GeneralAllocator::GetFastBinChunksExist() const
		{
			#if PPM_FASTBINS_ENABLED
				return (mnMaxFastBinChunkSize & kFlagFastBinChunksExist);
			#else
				return 0;
			#endif
		}

		inline
		void GeneralAllocator::SetFastBinChunksExist(bool bExist)
		{
			#if PPM_FASTBINS_ENABLED
				if(bExist)
					mnMaxFastBinChunkSize |= (size_type) kFlagFastBinChunksExist;
				else
					mnMaxFastBinChunkSize &= (size_type)~kFlagFastBinChunksExist;
			#else
				EA_UNUSED(bExist);
			#endif
		}


		inline
		bool GeneralAllocator::GetCoreIsContiguous() const
		{
			// Return true if there are zero or one entries in the core block list.
			return (mHeadCoreBlock.mpNextCoreBlock == mHeadCoreBlock.mpPrevCoreBlock);
		}


		inline
		unsigned GeneralAllocator::GetMaxFastBinChunkSize() const
		{
			#if PPM_FASTBINS_ENABLED
				return ((unsigned)mnMaxFastBinChunkSize & ~kFlagFastBinChunksExist);
			#else
				return 0;
			#endif
		}


		inline
		void GeneralAllocator::SetMaxFastBinDataSize(size_type nSize)
		{
			#if PPM_FASTBINS_ENABLED
				const unsigned nExistingFlags = (mnMaxFastBinChunkSize & kFlagFastBinChunksExist);
				if(nSize)
				{
					if(nSize > kFastBinMaxDataSize)
						nSize = kFastBinMaxDataSize;
					mnMaxFastBinChunkSize = GetChunkSizeFromDataSize(nSize) | nExistingFlags;
				}
				else
					mnMaxFastBinChunkSize = nExistingFlags;
			#else
				EA_UNUSED(nSize);
			#endif
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetLowestAlignedChunkPosition(Chunk* pChunk, size_t nAlignment, size_t nAlignmentOffset)
		{
			return (Chunk*)(intptr_t)(((((intptr_t)pChunk + kDataPtrOffset + nAlignmentOffset + (nAlignment - 1))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetHighestAlignedChunkPosition(Chunk* pChunk, size_type nSize, size_t nAlignment, size_t nAlignmentOffset)
		{
			return (Chunk*)(intptr_t)(((((intptr_t)pChunk + GetChunkSize(pChunk) - nSize + (kDataPtrOffset + nAlignmentOffset))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));
		}


		inline
		GeneralAllocator::Chunk* GeneralAllocator::GetHighestAlignedChunkPositionChecked(Chunk* pChunk, size_type nSize, size_t nAlignment, size_t nAlignmentOffset)
		{
			Chunk* const pReturnValue = (Chunk*)(intptr_t)(((((intptr_t)pChunk + GetChunkSize(pChunk) - nSize + (kDataPtrOffset + nAlignmentOffset))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));

			if(pReturnValue >= pChunk)
				return pReturnValue;

			return NULL; // It doesn't fit.
		}

	} // namespace Allocator

} // namespace EA



#endif // PPMALLOC_EAGENERALALLOCATOR_H













