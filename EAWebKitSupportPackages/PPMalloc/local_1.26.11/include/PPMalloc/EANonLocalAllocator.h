///////////////////////////////////////////////////////////////////////////////
// EANonLocalAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EANONLOCALALLOCATOR_H
#define PPMALLOC_EANONLOCALALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/mutex.h>
#include <PPMalloc/internal/shared.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		/// NonLocalAllocator
		///
		/// This class implements a generalized heap whereby the allocated memory is separate from the 
		/// data structures used to manage allocated memory. The allocated memory may be memory that
		/// is not easily accessible to the CPU or it may not even be conventional memory at all but
		/// instead might be disk file data.
		///
		/// Note that the term "core memory" used here is the non-local memory used as the basis for the heap.
		/// Note that you must manually initialize NonLocalAllocator with core memory and a local
		/// allocator callback function. There are no default implementations of these for any platform.
		///
		/// Example usage:
		///    NonLocalAllocator nla;
		///    nla.Init(SomeLocalAllocationFunction, SomeLocalFreeFunction, pSomeContext, pCoreMemory, coreMemorySize);
		///    void* p = nla.Malloc(100);
		///    nla.Free(p);
		///
		class PPM_API NonLocalAllocator
		{
		public:
			/// AllocationFlags
			/// These are values that can be or'd together and passed to allocation 
			/// functions to specify hints or directives regarding the allocation request.
			enum AllocationFlags
			{
				kAllocationFlagNone     = 0x00000000,    /// Shorthand meaning 'no flags'.
				kAllocationFlagLow      = 0x00000000,    /// Low is default.
				kAllocationFlagHigh     = 0x00000001,    /// Try to allocate the requested block from high memory. This allocator is optimized for the case of high memory blocks being long-lasting or permanent. PPM_HIGH_SUPPORTED must be #defined and kOptionEnableHighAllocation must be enabled for high allocation to work.
				kAllocationFlagPerm     = 0x00000001,    /// Same as kAllocationFlagHigh.
				kAllocationFlagEndFit   = 0x00000002,    /// For high memory allocations, make an extra effort to find a block that comes from the highest part of memory, even if other blocks lower in high memory suffice. For low memory allocations, try to find the lowest block. This option slows allocation by some amount but yields better high memory packing.
				kAllocationFlagBestFit  = 0x00000004,    /// Causes the allocation mechanism to sacifice speed for a fit that is less fragmenting.
				kAllocationFlagContig   = 0x00000008     /// Causes MallocMultiple to attempt to return all allocations contiguous to each other.
			};

			// Typdefs which control allocation limits
			#if (EA_PLATFORM_PTR_SIZE >= 8)
				typedef uint64_t  size_type;  /// Must be an unsigned type.
				typedef  int64_t ssize_type;  /// Signed equivalent of size_type.
			#else
				typedef uint32_t  size_type;  /// Must be an unsigned type.
				typedef  int32_t ssize_type;  /// Signed equivalent of size_type.
			#endif

			// Constants which control allocation minimums/maximums
			static const size_t kMinAllocSize = PPM_NLA_MIN_ALLOC_SIZE;                                           /// 
			static const size_t kMaxAllocSize = ((size_type)1 << ((sizeof(size_type) * 8) - 1)) - kMinAllocSize;  /// The largest possible single allocation request size.
			static const size_t kMinAlignment = PPM_NLA_MIN_ALIGN;                                                /// Must be an even power of two and must be >= kMinAllocSize

			// Forward declarations
			struct CoreBlock;
			struct Node;

			/// CoreFreeFunction
			/// This is the type of function you must use to pass core freeing 
			/// callbacks to NonLocalAllocator. Returns the amount of bytes freed,
			/// which should be zero or >= the same as input nSize. If the CoreFreeFunction
			/// returns zero, NonLocalAllocator nevertheless assumes that the memory is 
			/// henceforth unusable.
			typedef size_t (*CoreFreeFunction)(NonLocalAllocator* pAllocator, void* pCore, size_t nSize, void* pContext);

			/// LocalAllocationFunction
			/// Used to allocate NonLocalAllocator's internal Node, CoreBlock, and hash table objects (local memory used to manage non-local memory). 
			/// The returned pointer must have alignment >= alignof(void*).
			typedef void* (*LocalAllocationFunction)(NonLocalAllocator* pAllocator, size_t nSize, void* pContext);

			/// LocalFreeFunction
			/// Used to free memory allocated by LocalAllocationFunction.
			typedef size_t (*LocalFreeFunction)(NonLocalAllocator* pAllocator, void* pNode, size_t nSize, void* pContext);

		public:
			/// NonLocalAllocator
			/// Default constructor. This must be followed by a call to Init before using this allocator.
			NonLocalAllocator();

			/// NonLocalAllocator
			/// This constructor constructs the class instance and calls Init.
			/// See SetLocalAllocationFunction for information about pLocalAllocationFunction and pLocalFreeFunction.
			/// The nInitialBucketCount is a hint of what you expect the max number of outstanding allocations
			/// to be. A hash table is used to track allocations and starts with nInitialBucketCount. Too low of an
			/// initial bucket count will trigger the hash table to repeatedly resize as allocations are made.
			NonLocalAllocator(LocalAllocationFunction pLocalAllocationFunction, LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext,
							  void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFreeFunctionContext = NULL,
							  size_t nInitialBucketCount = 0);

			/// ~NonLocalAllocator
			/// Shuts down the allocator if not already shut down.
			PPM_VIRTUAL ~NonLocalAllocator();

			/// Init
			/// The Init function calls SetLocalAllocationFunction and SetLocalDebugAllocationFunction 
			/// with pLocalAllocationFunction. The user can call these functions after calling the 
			/// Init function in order to override the values set by Init.
			/// The nInitialBucketCount is a hint of what you expect the max number of outstanding allocations
			/// to be. A hash table is used to track allocations and starts with nInitialBucketCount. Too low of an
			/// initial bucket count will trigger the hash table to repeatedly resize as allocations are made.
			PPM_VIRTUAL bool Init(LocalAllocationFunction pLocalAllocationFunction, LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext,
								  void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFreeFunctionContext = NULL,
								  size_t nInitialBucketCount = 0);

			/// InitWithoutCore
			/// The InitWithoutCore function calls the initialization routines of the allocator but does not add a core
			/// block.  The allocator is in an initialized state however it will return NULL until it has a valid core block.  
			/// Example usage:
			///    NonLocalAllocator nla;
			///    nla.InitWithoutCore(SomeLocalAllocationFunction, SomeLocalFreeFunction, pSomeContext);
			///    void* p = nla.Malloc(1024);
			///    if(p == NULL)
			///         printf("allocation failed\n");
			///
			///    nla.AddCore();
			///    p = nla.Malloc(1024);
			///    if(p)
			///         printf("allocation succeed.\n");
			///
			///    nla.Free(p);
			///      
			PPM_VIRTUAL bool InitWithoutCore(LocalAllocationFunction pLocalAllocationFunction, LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext, 
											 size_t nInitialBucketCount = 0);

			/// Shutdown
			PPM_VIRTUAL bool Shutdown();


			/// SetLocalAllocationFunction
			/// Sets the function used to allocate Node, CoreBlock, and hash table objects (local memory used to manage non-local memory). 
			/// You must set the node allocation callback function for this class before calling AddCore,
			/// as this class needs to allocate an initial node for each new core.
			PPM_VIRTUAL void SetLocalAllocationFunction(LocalAllocationFunction pLocalAllocationFunction, 
														LocalFreeFunction pLocalFreeFunction, void* pContext = NULL);

			/// SetLocalDebugAllocationFunction
			/// Sets the allocator used to allocate local debug (callstack, file/line, group id, etc.) memory.
			PPM_VIRTUAL void SetLocalDebugAllocationFunction(LocalAllocationFunction pLocalAllocationFunction, 
															 LocalFreeFunction pLocalFreeFunction, void* pContext = NULL);

			/// AddCore
			/// Supplies core memory for the allocator which the allocator uses to parcel out 
			/// blocks in the Malloc family of functions. The input pCore must be valid and 
			/// aligned to at least 16 bytes. The input nSize must be a multiple of 1024 bytes.
			PPM_VIRTUAL bool AddCore(void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFreeFunctionContext = NULL);


			/// ExtendCore
			/// Specifies that the pCore previously added via AddCore or Init has been extended 
			/// in place by nSize bytes. This allows the user to increase the amount of available 
			/// memory in a way that's more efficient than calling AddCore, as the problem with 
			/// multiple core blocks is that memory cannot straddle them. ExtendCore is usable 
			/// only under conditions whereby the user is capable of extending pCore. One example
			/// of these is where core memory doesn't come from RAM but comes from a disk file.
			/// The extended nSize must be a multiple of kMinAllocSize.
			PPM_VIRTUAL bool ExtendCore(void* pCore, size_t nSize);


			/// TrimCore
			/// Frees the core memory identified by pCore if possible. If pCore is NULL then all
			/// possible core memory is freed. The memory is freed with the allocator set by 
			/// the Init function or SetLocalAllocationFunction. If the given allocator is NULL then 
			/// the memory is not freed but it is assumed the user will take care of freeing it. 
			/// If the user frees the memory then it must be freed after calling this function and not before.
			/// If pCore is NULL and all blocks are freed then the user cannot tell from the return
			/// value of this function which blocks were freed. However, the core block enumeration
			/// mechanism can be indirectly used to tell this.
			/// Returns the number of freed core blocks.
			/// 
			PPM_VIRTUAL size_t TrimCore(void* pCore);


			/// Malloc
			/// Allocate nSize bytes of memory. An nSize of zero is valid. 
			/// Returns memory aligned to kMinAlignment.
			/// Upon failure the return value is NULL (or the failure value set by kOptionFailureValue).
			PPM_VIRTUAL void* Malloc(size_t nSize, int nAllocationFlags = 0);
			PPM_VIRTUAL void* MallocDebug(size_t nSize, int nAllocationFlags = 0, 
											unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			/// MallocAligned
			/// Allocate 'nSize' bytes allocated to 'nAlignment' bytes.
			/// Input 'nAlignment' must be a multiple power of 2. If it is not a power of 2, 
			/// the next greater power of 2 is used. The minimum alignment returned is 
			/// kMinAlignment. The alignment offset parameter specifies how many bytes 
			/// into the allocation that the alignment should be found. 
			/// The kAllocationFlagBestFit allocation flag can be used to help
			/// find a less fragmenting block of memory, though this flag can 
			/// also slow down the allocation function by a potentially undesirable
			/// amount of time, depending on the state of the heap.
			/// Upon failure the return value is NULL (or the failure value set by kOptionFailureValue).
			/// It is valid for the user to specify an nAlignmentOffset that is larger than nSize, 
			/// in which case the aligned position will be beyond the allocated block size.
			PPM_VIRTUAL void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0);
			PPM_VIRTUAL void* MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags = 0,
													unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			/// MallocMultiple
			/// Allows you to allocate multiple blocks simultaneously. 
			/// An alignment of zero for any entry causes that entry to be allocated
			/// as if Malloc was called instead of MallocAligned.
			/// Returns pResultArray if successfull; else NULL. If any allocation fails 
			/// then all fail. There are no partially successfull allocations.
			PPM_VIRTUAL void** MallocMultiple(size_t count, const size_t* pSizeArray, const size_t* pAlignmentArray, 
											 const size_t* pAlignmentOffsetArray, void* pResultArray[], int nAllocationFlags = 0);
			PPM_VIRTUAL void** MallocMultipleDebug(size_t count, const size_t* pSizeArray, const size_t* pAlignmentArray, 
													const size_t* pAlignmentOffsetArray, void* pResultArray[], int nAllocationFlags = 0,
													unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			/// Realloc
			/// Re-allocate the previously allocated block in p, making the new block nNewSize bytes long.
			/// The returned block may be of aligment as low as NonLocalAllocator::kMinAlignment, so if 
			/// the allocation being reallocated was originally of higher alignment, you must use ReallocAligned
			/// to ensure that the newly returned memory is of at least that alignment.
			/// When memory is relocated, it is moved via the memcpy function specified by SetMemcpyFunction.
			/// Upon failure the return value is NULL (or the failure value set by kOptionFailureValue).
			///
			/// Detailed behaviour description. NULL below refers to whatever the user-specified (kOptionFailureValue) invalid pointer value is.
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
			///  Reallocating a pointer to a smaller size is guaranteed to succeed, 
			///  but is not guaranteed to return the original pointer.
			PPM_VIRTUAL void* Realloc(void* p, size_t nNewSize, int nAllocationFlags = 0);
			PPM_VIRTUAL void* ReallocDebug(void* p, size_t nNewSize, int nAllocationFlags = 0,
											unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);

			/// ReallocAligned
			///
			PPM_VIRTUAL void* ReallocAligned(void* p, size_t nNewSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0);
			PPM_VIRTUAL void* ReallocAlignedDebug(void* p, size_t nNewSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0,
													unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);


			/// Split
			/// Allows splitting an allocation without having to reallocate it.  
			/// Takes the current allocation node and inserts a new one into the allocation list.  The previous block
			/// size is adjusted to reflect the memory removed.
			PPM_VIRTUAL void* Split(void* p, size_t nOffset);


			/// Free
			/// Frees a block allocated by Malloc, Realloc or Calloc.
			/// Follows the C89 (and thus C99, which hasn't substantially changed) standard 
			/// for behaviour. If PPM_NULL_POINTER_FREE_ENABLED is enabled (which it is by default),
			/// then the input pointer may be NULL (or the failure value set by kOptionFailureValue).
			PPM_VIRTUAL void Free(void* p);


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
			typedef bool (*MallocFailureFunction)(NonLocalAllocator* pAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
			PPM_VIRTUAL void SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void* pContext);


			/// SetMemcpyFunction
			/// Specifies a function which is used to copy memory in the destination.
			/// This function is used during user-requested reallocation functions.
			/// By default this function is set to the C memcpy function. If you set
			/// this function to NULL, then memcpy of the managed memory never occurs.
			/// If NULL then the Realloc function cannot work as expected.
			typedef void* (*MemcpyFunction)(void* pDest, const void* pSource, size_t count);
			PPM_VIRTUAL void SetMemcpyFunction(MemcpyFunction pMemcpyFunction);


			/// SetMemsetFunction
			/// Specifies a function which is used to set memory in the destination.
			/// Defaults to memset.
			typedef void* (*MemsetFunction)(void* pDest, int value, size_t count);
			PPM_VIRTUAL void SetMemsetFunction(MemsetFunction pMemsetFunction);


			/// SetMemoryFillCheckFunction
			/// Specifies a function which is used to verify a memory fill in the destination.
			/// Defaults to Allocator::MemoryFillCheck.
			/// A custom memory fill check function might want to employ a strategy for dealing
			/// with large data sizes, as a full brute force memory check on large data sizes
			/// can result in unacceptable performance in a debug build. See Allocator::MemoryFillCheck.
			typedef bool (*MemoryFillCheckFunction)(const unsigned char* pData, size_t nDataSize, unsigned char c);
			PPM_VIRTUAL void SetMemoryFillCheckFunction(MemoryFillCheckFunction pMemoryFillCheckFunction);


			/// AssertionFailureInfo
			/// Struct passed to user upon assertion failure.
			struct AssertionFailureInfo
			{
				const NonLocalAllocator* mpAllocator;          /// The allocator the assertion is associated with. May be NULL for failures that are without allocator context.
				const char*              mpExpression;         /// The assertion expression that failed.
				int                      mnErrorId;            /// One of enum GAError or a user-defined or subclass-defined id.
				const char*              mpDescription;        /// A description of the assertion failure.
				const void*              mpData;               /// The user allocation the assertion is associated with. May be NULL for failures that are without allocation context.
				const void*              mpExtra;              /// Other information related to the failure. The meaning depends on the failure id.
			};

			/// SetAssertionFailureFunction
			/// Allows the user to specify a callback function to trap assertion failures.
			/// The user must not attempt to modify the allocator while in this failure callback.
			typedef void (*AssertionFailureInfoFunction)(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
			PPM_VIRTUAL void SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext);


			/// AssertionFailure
			/// Triggers an assertion failure. This function is generally intended for internal
			/// use but is available so that debug systems wrapped around this allocator can
			/// use the standard assertion facilities.
			PPM_VIRTUAL void AssertionFailure(const char* pExpression, int nErrorId = kGAErrorUnknown, const char* pDescription = NULL, 
												const void* pData = NULL, const void* pExtra = NULL) const;

			/// Lock
			/// Locks or unlocks the class object for thread-safe usage across calls.
			/// While locked, all other threads must wait until a matching unlock is called.
			/// Locks are recursive, and thus two successive locks are expected to be matched
			/// by two successive unlocks before other threads can proceed.
			PPM_VIRTUAL void Lock(bool bEnable);


			/// Runtime Options
			enum Option
			{
				kOptionNone                      =  0,
				kOptionEnableThreadSafety        =  1,      /// If non-zero, enabled thread-safety, otherwise disable it.
				kOptionEnableHighAllocation      =  2,      /// If set to a non-zero value, then high-memory allocation is enabled. The default is disabled, as this functionality has a runtime cost.
				kOptionMaxMallocFailureCount     =  3,      /// Max number of times to call the MallocFailure function successively. Default is 256.
				kOptionEnableDebugReads          =  4,      /// If enabled, reads of the allocated memory are done, such as during block data dumps. Disabled by default.
				kOptionEnableDebugWrites         =  5,      /// If enabled, debug info such as guard fills and free fills are written to the non-local memory and are checked in debug builds. Disabled by default.
				kOptionEnableMetrics             =  6,      /// If non-zero, enabled metrics collection, otherwise disable it. Disabled by default. This option incurs some runtime time cost.
				kOptionMinSplitSize              =  7,      /// Specifies the minimum size for a block to be split off of another during allocation. The default and minimum value is kMinAllocSize. Setting this to higher values causes less free blocks to lie around but also takes away memory that might be coalescable with contiguous free memory.
				kOptionFailureValue              =  8,      /// Specifies the return value that means failure. Default is 0 (NULL), but sometimes you might want 0 to be a valid address and set (void*)-1 to be the failure return value instead of 0. This option should be set before any memory is allocated and any core is added.
				kOptionEnableMallocFailureAssert =  9       /// If enabled then Malloc functions execute PPM_ASSERT(false) before returning. This is disabled by default.
			};

			/// SetOption
			/// The user specifies one of enum Option. Note that the argument 'option' is 
			/// of type int instead of type Option because subclasses may want to extend
			/// the option set to include values not listed here.
			PPM_VIRTUAL void SetOption(int option, intptr_t nValue);


			/// GetOption
			PPM_VIRTUAL intptr_t GetOption(int option) const;


			/// GetUsableSize
			/// Report the number of usable bytes associated with 'pData'. Thus if the user allocates
			/// 30 bytes it is possible that the block returned to the user actually has 40 bytes of 
			/// usable space. The input pointer must be valid or the return value is (size_t)-1.
			PPM_VIRTUAL size_t GetUsableSize(const void* pData) const;


			/// GetLargestFreeBlock
			/// Returns the largest current block. If bClearCache is true, then any present cache
			/// will be cleared before the check is done. Note that clearing the cache may result
			/// in a larger largest free block, but clearing the cache has some minor performance costs.
			PPM_VIRTUAL size_t GetLargestFreeBlock();


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
			/// Enables or disables automatic heap validation of a given level and a given
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
			///    pAllocator->SetAutoHeapValidation(kHeapValidationLevelDetail, 32);
			/// 
			PPM_VIRTUAL void SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency);


			/// SetTraceFunction
			/// Allows you to override where traces go. If not specified, traces are sent via 
			/// printf to 'stdout'. Strings passed to the TraceFunction will have trailing newlines.
			typedef void (*TraceFunction)(const char* pTraceText, void* pContext);
			PPM_VIRTUAL void SetTraceFunction(TraceFunction pTraceFunction, void* pContext);


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
			PPM_VIRTUAL void TraceAllocatedMemory(TraceFunction pTraceFunction = NULL, void* pTraceFunctionContext = NULL, 
													int nBlockTypeFlags = kBlockTypeAllocated);


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
			/// If any validation fails, the return value is NULL (or the failure value set by kOptionFailureValue).
			///
			/// Example usage:
			///    void* p = pAllocator->Malloc(1);
			///    void* pResult = pAllocatorValidateAddress(pData, kAddressTypeSpecific);
			///
			PPM_VIRTUAL const void* ValidateAddress(const void* pAddress, int addressType = kAddressTypeAllocated) const;


			/// BlockType
			/// Used in memory reports and snapshots.
			enum BlockType 
			{
				kBlockTypeNone      =  0, ///
				kBlockTypeInvalid   =  1, /// The block information is invalid.
				kBlockTypeAllocated =  2, /// The block is allocated.
				kBlockTypeFree      =  4, /// The block is not allocated.
				kBlockTypeCore      =  8, /// The block is a core area, which is a large area of memory from which regular blocks come.
				kBlockTypeInternal  = 16, /// The block is internal, allocated by the allocator itself.
				kBlockTypeAll       = 31  /// This is a flag that includes all other flags.
			};


			/// BlockInfo
			/// Defines allocation block information for use with heap enumeration.
			/// We call this BlockInfo instead of ChunkInfo because most users are 
			/// familiar with the term 'block' more than 'chunk'.
			struct BlockInfo
			{
				const char*  mpCore;              /// The core to which mpNode and mpData belongs. Recall that the heap consists of one or more large master "core blocks" which are subdivided into the blocks handed to users.
				const char*  mpNode;              /// The Node to which mpData belongs. Will be equal to mpCore when mBlockType is kBlockTypeCore.
				size_t       mnSize;              /// The size of the data managed by the Node. Will be >= mnDataSize.
				void*        mpData;              /// The user data within this block. Note that for regular allocations on 32 bit systems, mpData is usually equal to mpBlock + 8, with the 8 bytes being internal block info.
				size_t       mnDataSize;          /// The size of the user data within this block.
				char         mBlockType;          /// One of enum BlockType
			};


			/// ReportContext
			struct ReportContext
			{
				int         mnBlockTypeFlags;    /// Global block type flags filter to apply. See enum BlockType.
				CoreBlock*  mpCurrentCoreBlock;  /// Current CoreBlock.
				Node*       mpCurrentNode;       /// Current memory Node within current CoreBlock.
				BlockInfo   mBlockInfo;          /// Information about the currently iterated node.
				bool        mbCoreBlockReported; /// If true, then we have iterated the CoreBlock for the current set of Chunks.
			};


			/// ReportHeap
			/// This function reports the entire heap layout in detail. It should allow for the 
			/// fairly detailed analysis of memory usage. If the user returns false from the 
			/// HeapReportFunction, the heap reporting stops. ReportHeap itself returns the 
			/// last value returned by the HeapReportFunction. 
			typedef bool (*HeapReportFunction)(const BlockInfo* pBlockInfo, void* pContext); 
			PPM_VIRTUAL bool ReportHeap(HeapReportFunction, void* pContext, int nBlockTypeFlags);


			/// ReportBegin
			/// Begins heap iteration, Returns a context to be used for ReportNext and 
			/// ReportEnd calls. Must be matched with a final ReportEnd. ReportBegin locks
			/// the allocator mutex and blocks all other threads from executing until 
			/// ReportEnd, which unlocks the mutex.
			PPM_VIRTUAL ReportContext*    ReportBegin(ReportContext* pReportContext, int nBlockTypeFlags = kBlockTypeAll);
			PPM_VIRTUAL const BlockInfo*  ReportNext(ReportContext* pContext, int nBlockTypeFlags = kBlockTypeAll); /// Gets the next block in the report. Returns NULL when there is nothing left to report.
			PPM_VIRTUAL void              ReportEnd(ReportContext* pContext);                                       /// Ends the report, matching an intial ReportBegin.


			/// GetDefaultDebugDataFlags
			/// Returns all default DebugDataId flags. These flags are indicated by 
			/// enum DebugDataId. To specify that you want to check the state of  
			/// kDebugDataIdCallStack you would compare the resulting flags to 
			/// '1 << kDebugDataIdCallStack'.
			///
			/// Example usage:
			///    unsigned flags = pAllocator->GetDefaultDebugDataFlags();
			///    if(flags & (1 << kDebugDataIdSize))
			///        printf("kDebugDataIdSize is enabled\n").
			///
			PPM_VIRTUAL unsigned GetDefaultDebugDataFlags() const;


			/// SetDefaultDebugDataFlags
			/// Sets the default debug data flags to use for all allocations.
			/// To specify that you want store the call stack with all allocations, 
			/// you use '1 << kDebugDataIdCallStack' as an input flag.
			///
			/// Example usage:
			///    pAllocator->SetDefaultDebugDataFlags((1 << kDebugDataIdCallStack) | (1 << kDebugDataIdSize));
			///
			PPM_VIRTUAL void SetDefaultDebugDataFlags(unsigned flags);


			/// SetDefaultDebugDataFlag
			/// Sets an individual flag, indicated by enum DebugDataId. To enable
			/// the kDebugDataIdCallStack flag, you simply call this function with
			/// kDebugDataIdCallStack as an argument. This function has the same
			/// effect as: currentFlags |= (1 << debugDataFlag).
			///
			/// Example usage:
			///    pAllocator->SetDefaultDebugDataFlag(kDebugDataIdCallStack);
			///
			PPM_VIRTUAL void SetDefaultDebugDataFlag(unsigned debugDataFlag);


			/// SetGuardSize
			/// Sets the size of the guard fill, which refers to extra memory associated with each
			/// block which is expected to be filled with a constant value. This is useful in 
			/// detecting bad memory writes. Setting fGuardSizeRatio to <= 0 or setting nMaxGuardSize
			/// to zero will have the effect of disabing guard fills. It will do so by acting as if
			/// SetDefaultDebugDataFlag(kDebugDataIdGuard) was called.
			///
			/// Example usage:
			///    // Set the guard to be 3 times the size of the user's requested size, 
			///    // but make it at always at leasdt 128 but never more than 1024.
			///    pAllocator->SetGuardSize(3.f, 128, 1024);
			///
			PPM_VIRTUAL void SetGuardSize(float fGuardSizeRatio, size_t nMinGuardSize, size_t nMaxGuardSize);


			/// GetCurrentGroupId
			/// Returns the current group id. Group ids are only applied if the 
			/// kDebugDataIdGroup flag is set via SetDefaultDebugDataFlags. 
			/// The initial default group id is zero.
			/// The primary use of group ids is to categorize a large set of user
			/// allocations as being related. If you have an app whereby you go 
			/// into some mode and then before exiting that mode you want to verify
			/// that no memory allocated in that mode currently exists, you can use
			/// the group id to tag all allocations during that mode as belonging 
			/// to that mode.
			PPM_VIRTUAL int GetCurrentGroupId() const;


			/// SetCurrentGroupId
			/// Sets the current group id, returning the previous one. If you want to 
			/// disable the assignment of group ids, use SetDefaultDebugDataFlags.
			PPM_VIRTUAL int SetCurrentGroupId(int nGroupId);


			/// enum DebugDataId
			/// Defines standard names for debug information tied to a block. The list of possible
			/// debug information tied to a block is not limited to these values. For blocks that
			/// are reallocated, the kDebugDataIdSize and kDebugDataIdOverhead data are updated,
			/// but no other debug data is automatically updated.
			enum DebugDataId
			{
				kDebugDataIdNone            =    0,   /// Identifies unused debug data.
				kDebugDataIdAllocator       =    1,   /// Pointer to NonLocalAllocator which the block belongs to. Format: struct{ NonLocalAllocator* p; };
				kDebugDataIdSize            =    2,   /// Size requested by user for block. Format: struct{ size_t n; };
				kDebugDataIdFlags           =    3,   /// Allocation flags requested by user for block. Field is absent if flags were zero. Format: struct{ int flags; };
				kDebugDataIdAlignment       =    4,   /// The alignment and possible alignment offset the user requested for the block. Format: struct AlignmentInfo{ size_t a; size_t o; }; The o is ommitted if it is zero. The record itself is omitted if a is zero.
				kDebugDataIdName            =    5,   /// Name for allocation. Format: struct{ char name[]; }; where name is 0-terminated.
				kDebugDataIdPlace           =    6,   /// Place of allocation as string: __FILE__ / __LINE__. Format: struct FileLineInfo{ const char* path, int line };
				kDebugDataIdCallStack       =    7,   /// Call stack for allocation. Format: struct{ void* addr[]; }; 
				kDebugDataIdChecksum        =    8,   /// Checksum for user user data, for blocks allocated as read-only. Format: struct{ size_t n; };
				kDebugDataIdAllocationCount =    9,   /// Index of allocation, unique for each allocation. Format: struct{ uint64_t n; }; Use the GetAllocationCount function to get the value most recently assigned to a block. The value stored with the block is the same as the GetAllocationCount after the block was allocated. The first allocation is given a value of zero.
				kDebugDataIdAllocationTime  =   10,   /// Time of allocation. Format: struct{ size_t n; };
				kDebugDataIdGuard           =   11,   /// Guard data between blocks. Format: struct{ size_t n; };
				kDebugDataIdOverhead        =   12,   /// The system overhead for this block. Format: struct{ size_t n; }; 
				kDebugDataIdGroup           =   13,   /// Allocation family group id. Most common use is to detect leaks by asserting that there are no more blocks of a given group after leaving a given stage of the app. Format: struct{ int id; };
				kDebugDataIdInternalCount   =   14,   /// Count of internal types.
				kDebugDataIdUser            = 1000    /// User-defined values begin here.
			};

			struct FileLineInfo
			{
				const char* mpFile;
				int         mnLine;
			};

			struct AlignmentInfo
			{
				size_t mnAlignment; 
				size_t mnAlignmentOffset;
			};

			typedef uint16_t DebugDataIdType;
			typedef uint16_t DebugDataLengthType;

			/// enum DebugRecordConstants
			/// These values are used to define debug record field constants.
			enum DebugRecordConstants
			{
				kDebugBlockSizeSize      = sizeof(DebugDataLengthType),
				kDebugRecordSizeSize     = sizeof(DebugDataLengthType),
				kDebugRecordIdSize       = sizeof(DebugDataIdType),
				kDebugRecordOverheadSize = kDebugRecordSizeSize + kDebugRecordIdSize
			};


			/// GetDebugDataLength
			/// Returns length of all debug data in a record. You can use this
			/// information to tell how much overhead the record has in debug data.
			/// If the input ppDebugData is non-NULL, it will be returned with a
			/// pointer to the beginning of the debug data in the block.
			/// Returns 0 if debug data is not present, in which case ppDebugData 
			/// is unmodified.
			PPM_VIRTUAL size_t GetDebugDataLength(const void* pData, void** ppDebugData = NULL) const;


			/// GetDebugData
			/// Gets debug data identified by an id from a user block.
			/// Returns the pointer to the data within the block.
			/// If the input 'pDebugData' or input 'nDataLength' is zero, the data is not copied.
			/// This function is thread-safe.
			PPM_VIRTUAL void* GetDebugData(const void* pData, DebugDataIdType id, void* pDebugData, size_t nDataLength, size_t* pActualDataLength = NULL) const;

			/// SetDebugData
			/// Adds the given debug data identified by id to a user block.
			/// If debug data already exists for the given id, it is overwritten if the new
			/// size is less or equal to the existing size. Otherwise the function fails.
			/// Returns the pointer to the memory for the debug data in the user block.
			/// Returns NULL if the operation failed, usually due to an out-of-memory condition.
			/// If the input 'pDebugData' is null or input 'nDataLength' is zero, the data is not written.
			/// This function is thread-safe.
			PPM_VIRTUAL void* SetDebugData(void* pData, DebugDataIdType id, const void* pDebugData, size_t nDataLength);


			/// GetMallocDebugInfo
			/// Returns the allocation name, file, line that is currently active for the 
			/// current allocation. This function might be used in a callback from this allocator.
			/// Returns true if the information is available. It may be unavailable if 
			/// PPM_NLA_DEBUG_DATA_SUPPORTED is disabled or if the user simply didn't set
			/// the allocation name, file, or line due to calling Malloc instead of MallocDebug.
			/// If the return value is true, then the returned name, file, line are guaranteed to be non-NULL.
			PPM_VIRTUAL bool GetMallocDebugInfo(const char*& pAllocationName, const char*& pAllocationFile, int& nAllocationLine);


			/// HookType
			/// Enumerates high level allocation types.
			enum HookType
			{
				kHookTypeMalloc,     /// Malloc or similar was called. See HookSubType to find out which.
				kHookTypeFree,       /// Free was called.
				kHookTypeCoreAlloc,  /// Core was allocated or donated. The hook is called when the user explicitly supplies core. The hook is not called when core is extended on platforms that support core extension (e.g. Win32/Win64).
				kHookTypeCoreFree    /// Core will be freed. The hook is called for core allocated internally and for core explicitly supplied by the user.
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
				NonLocalAllocator* mpAllocator;              /// Pointer to the NonLocalAllocator that is calling the hook.
				bool               mbEntry;                  /// If true, then hook is being called on function entry (before the operation is done), else being called on function exit (after the operation is done).
				HookType           mHookType;                /// One of enum HookType.
				HookSubType        mHookSubType;             /// One of enum HookSubType. For kHookTypeCoreAlloc, it is always kHookSubTypeNone.
				size_t             mnSizeInputTotal;         /// For Malloc, Calloc, Realloc, MallocAligned, MallocMultiple, CoreAlloc, CoreFree.
				const void*        mpDataInput;              /// For Realloc, Free, CoreAlloc, CoreFree.
				size_t             mnCountInput;             /// For Calloc, MallocMultiple.
				size_t             mnSizeInput;              /// For Malloc, Realloc, CoreAlloc, CoreFree.
				const size_t*      mpSizeInputArray;         /// For MallocMultiple.
				size_t             mnAlignmentInput;         /// For MallocAligned.
				int                mnAllocationFlags;        /// For all kHookTypeMalloc sub types.
				void*              mpDataOutput;             /// For Malloc, Calloc, Realloc, MallocAligned.
				void**             mpArrayOutput;            /// For MallocMultiple.
				size_t             mnSizeOutput;             /// For Malloc, MallocAligned (alignment offset) Realloc, CoreAlloc, CoreFree.
			};

			/// SetHookFunction
			/// This function lets you specify a callback function which will be called upon
			/// entering an allocation and upon leaving an allocation. The difference is that
			/// upon exiting the function the heap has changed and the user now has a return
			/// value to work with. The hook function is called with NonLocalAllocator unlocked,
			/// so thread safety issues are not a concern for the called hook function. However,
			/// subclasses of NonLocalAllocator may not have the same thread safety regarding
			/// these hooks as NonLocalAllocator itself.
			/// Note that you can call NonLocalAllocator functions such as GetMallocDebugInfo in
			/// order to get more information about the allocation or the allocator.
			typedef void (*HookFunction)(const HookInfo* pHookInfo, void* pContext); 
			PPM_VIRTUAL void SetHookFunction(HookFunction pHookFunction, void* pContext);


			// Metrics
			struct Metrics
			{
				uint64_t   mnAllocationCount;               /// Current number of blocks allocated to user.
				uint64_t   mnAllocationCountHistorical;     /// Total number of blocks ever allocated to user.
				uint64_t   mnAllocationCountMax;            /// High water mark of concurrent allocation count.
				uint64_t   mnAllocationVolume;              /// Sum of all memory currently allocated by user.
				uint64_t   mnAllocationVolumeHistorical;    /// Sum of all memory ever allocated by user.
				uint64_t   mnAllocationVolumeMax;           /// High water mark of concurrent allocation volume.
				uint64_t   mnFreeCountHistorical;           /// Total number of blocks ever freed by user.
				uint64_t   mnFreeVolumeHistorical;          /// Sum of memory ever freed by user.
			};

			enum MetricType
			{
				kMetricTypeAll,     /// Summary, regardless of system, debug, user usage.
				kMetricTypeSystem,  /// System memory. This doesn't include debug or user memory. This is essentially per-system and per-chunk overhead.
				kMetricTypeDebug,   /// Debug memory. This is memory which is exclusively allocated by NonLocalAllocator. 
				kMetricTypeUser,    /// User memory. This is memory which is requested by the user. Includes internally fragmented memory (unrequested memory that comes with a block returned to a user).
				kMetricTypeCount    /// A count of the available types.
			};

			/// GetMetrics
			/// Returns metrics of the given type. Returns NULL if PPM_NLA_METRICS_SUPPORTED == 0.
			PPM_VIRTUAL const Metrics* GetMetrics(MetricType metricsType);

		public: // To consider: Make this section protected instead of public. 
			/// Node
			/// Note that the mnBlockSize member is a bitfield, but not declared as such.
			/// The reason for this is that it turns out that we can often make assumptions
			/// about its value and avoid the bitfield manipulation.
			/// To consider: Make this a bitfield nevertheless.
			struct PPM_API Node
			{
				char*       mpBlock;                /// Beginning of this managed block, <= mpData and often == mpData.
				size_type   mnBlockSize;            /// Size of mpBlock. User data size == (mnBlockSize - (mpData - mpBlock)).
				char*       mpData;                 /// User pointer. (mpBlock <= mpData) && (mpData < (mpBlock + mnBlockSize)).
				Node*       mpMemPrev;              /// Previous node in memory.
				Node*       mpMemNext;              /// Next node in memory.
				Node*       mpListPrev;             /// Previous node in free list (if free). Used in the free list when the node is free.
				Node*       mpListNext;             /// Next node in free list (if free). Used in free list when the node is free; used in the hash table when the node is allocated.
				char*       mpDebugData;            /// This must point to memory that is at least 4 byte aligned. To consider: Remove this variable in a release build. Also possibly store it at the end of the Node data.

				size_type   IsInUse() const;
				void        SetIsInUse(bool bInUse);
				size_type   GetSize() const;                    /// Returns mnBlockSize minus any private bits that are set in it.
				void        SetSize(size_type size);            /// Sets mnBlockSize plus any private bits we use.
				size_type   GetUserSize() const;                ///
				Node*       CoalesceWithNext(Node* pMemNext);   /// Returns GetSize - any space between mpBlock and mpData.
				void        InsertIntoMemList(Node* pMemPrev);
				void        SplitNext(size_type nNewSize, Node* pMemNewNext);
				void        SplitPrev(size_type nNewPrevSize, Node* pMemNewPrev);
				void        RemoveFromFreeList();
				void        InsertIntoFreeList(Node* pListPrev);
			};

			struct PPM_API CoreBlock : public Node
			{
				CoreFreeFunction mpCoreFreeFunction;            /// If non-NULL, frees are done via this function.
				void*            mpCoreFreeFunctionContext;     /// This is passed to the CoreFreeFunction; it allows the core free function to have a context to work with.
			};

		protected:
			/// HashTable
			/// This hashtable stores all allocated blocks and any debug information for
			/// them if debug info is enabled.
			class PPM_API HashTable
			{
			public:
				HashTable();
			   ~HashTable();

				size_t  Hash(const void* pData) const;
				void    Insert(Node* pNode);
				Node*   Find(const void* pData) const;
				Node*   Remove(const void* pData);
				bool    Remove(Node* pNode);
				void    Clear(bool bDestroyTable);
				bool    ReHash(size_t nNewBucketCount);
				void    SetAllocationFunction(NonLocalAllocator* pNonLocalAllocator, LocalAllocationFunction pAllocationFunction, LocalFreeFunction pFreeFunction, void* pFunctionContext);

			public:
				Node**                  mpBucketArray;           // We would declare this as Node* mpBucketArray[] if we had a fixed bucket count.
				size_t                  mnBucketCount;
				size_t                  mnBucketCountMin;
				size_t                  mnElementCount;
				NonLocalAllocator*      mpNonLocalAllocator;
				LocalAllocationFunction mpAllocationFunction;
				LocalFreeFunction       mpFreeFunction;
				void*                   mpFunctionContext;
				bool                    mbReHashing;
			}; // HashTable

			struct DebugDataInfoShared
			{
				size_t mnRecordSize;
				size_t mnRecordDataSize;
			};

			struct DebugDataInfo
			{
				DebugDataInfoShared mShared[kDebugDataIdInternalCount];
				size_t              mnSeparateUsage;
				size_t              mnUserSize;                 /// kDebugDataIdSize
				int                 mnAllocationFlags;          /// kDebugDataIdFlags
				size_t              mnAlignment;                /// kDebugDataIdAlignment
				size_t              mnAlignmentOffset;          /// kDebugDataIdAlignment
				FileLineInfo        mFileLineInfo;              /// kDebugDataIdPlace
				size_t              mnGuardFillSize;            /// kDebugDataIdGuard
				void*               mpReturnAddressArray[24];   /// kDebugDataIdCallStack
				unsigned            mnNextDebugDataFlags;       /// Temporary debug flags for the next allocation.
				const char*         mpNextAllocationName;       /// Temporary name for the next allocation.
				const char*         mpNextAllocationFile;       /// Temporary file name (e.g. __FILE__) tied to the next allocation.
				int                 mnNextAllocationLine;       /// Temporary line (e.g. __LINE__) tied to the next allocation.
			};

		protected:
			static const size_t     kSizeTypeSize       = sizeof(size_type);                             /// Used for knowing how much space a size_type object takes up.
			static const size_t     kMinAlignmentMask   = kMinAlignment - 1;                             /// Must be of bit pattern ending with contiguous ones, as with 0x0000003.
			static const size_t     kBinCount           = PPM_NLA_BIN_COUNT;                             /// 
			static const size_t     kFirstBinPower      = 2;                                             /// First bin stores a size of 2^2, next bin 2^3, etc for kBinCount bins.
			static const size_type  kNodeInUseBit       = (size_type)1 << ((sizeof(size_type) * 8) - 1); /// 

			void             Construct();
			Node*            AllocateNode();
			void             DeallocateNode(Node* pNode);
			CoreBlock*       AllocateCoreBlock();
			void             DeallocateCoreBlock(CoreBlock* pCoreBlock);
			Node*            MallocNode(size_t nSize, int nAllocationFlags);
			Node*            MallocNodeAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags);
			void             AddNodeToFreeList(Node* pNode);
			Node*            FindNodeInFreeList(const Node* pNode);

		public:
			static uint32_t  GetBinIndexAndSize(size_type blockSize, size_type& binSize);
			static uint32_t  GetBinIndex(size_type blockSize);

		protected:
			static void      AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);
			void             DoFailureNotification(const Node* pNode, const char* pMessage, int id) const;
			CoreBlock*       FindCoreBlockForAddress(const void* pAddress) const;
			size_t           DescribeNode(const Node* pNode, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const;
			void             GetBlockInfoForCoreBlock(const CoreBlock* pCoreBlock, BlockInfo* pBlockInfo) const;
			void             GetBlockInfoForNode(const Node* pNode, BlockInfo* pBlockInfo, const void* pCore) const;
			static size_type NodeMatchesBlockType(const Node* pNode, int nBlockTypeFlags);

			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				void             GetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, size_t nSize, int nAllocationFlags, size_t nAlignment = 0, size_t nAlignmentOffset = 0);
				bool             SetDebugInfoForAllocation(const DebugDataInfo& debugDataInfo, Node* pNode);
				void             AttachDebugDataToNode(const DebugDataInfo& debugDataInfo, Node* pNode, bool skipNewFill);
				size_t           GetGuardSizeForBlock(size_t nBlockSize);
				static size_t    GetGuardSize(const Node* pNode);
				bool             SetGuardSize(const Node* pNode, size_t nSize, void* pGuardFillBegin, bool bDoFill = true);
				void             WriteGuardFill(const Node* pNode, size_t nSize);
				bool             VerifyGuardFill(const Node* pNode) const;
				bool             VerifyFreeFill(const Node* pNode) const;
				static size_t    GetDebugDataRecordLength(size_t nDebugDataLength);
				size_t           GetDebugDataLength(const Node* pNode, void** ppDebugData) const;
				static void*     GetDebugData(const Node* pNode, DebugDataIdType id, void* pDebugData, size_t nDataLength, size_t* pActualDataLength);
				static void*     SetDebugData(Node* pNode, DebugDataIdType id, const void* pDebugData, size_t nDataLength);
			#endif

			#if PPM_NLA_METRICS_SUPPORTED
				void             CollectMetrics(const Node* pNode, bool bMalloc);
			#endif

		protected:
			bool                    mbInitialized;                    /// True if we are intialized.
			bool                    mbEnableDebugReads;               /// Defaults to false;
			bool                    mbEnableDebugWrites;              /// Defaults to false.
			CoreBlock               mHeadCoreBlock;                   /// Linked list of CoreBlocks.
			CoreBlock               mCoreBlockFirst;                  /// The first CoreBlock, which is not allocated dynamically but is stored here.
			HashTable               mHashTable;                       /// Tracks all allocated blocks.
			Node                    mFreeListArray[kBinCount];        /// Free blocks, with bins ordered by size and blocks within bins ordered by size.
			size_type               mMinSplitSize;                    /// Minimum size for a block to be split off of another during allocation.
			char*                   mFailureValue;                    /// The value we return to indicate allocation failure. Default is NULL (0).
			bool                    mbMallocFailureAssertEnabled;     /// See kOptionEnableMallocFailureAssert.

			MemcpyFunction          mpMemcpyFunction;                 /// Function used to copy non-local memory.
			MemsetFunction          mpMemsetFunction;                 /// Function used to set non-local memory.
			MemoryFillCheckFunction mpMemoryFillCheckFunction;        /// Function used to check non-local memory.

			LocalAllocationFunction mpLocalAllocationFunction;
			LocalFreeFunction       mpLocalFreeFunction;
			void*                   mpLocalAllocationContext;

			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				LocalAllocationFunction mpLocalDebugAllocationFunction;
				LocalFreeFunction       mpLocalDebugFreeFunction;
				void*                   mpLocalDebugAllocationContext;
			#endif

			MallocFailureFunction mpMallocFailureFunction;
			void*                 mpMallocFailureFunctionContext;
			uint32_t              mnMaxMallocFailureCount;          /// Max number of times to call the MallocFailure function successively.

			AssertionFailureInfoFunction mpAssertionFailureFunction;
			void*                        mpAssertionFailureFunctionContext;

			TraceFunction       mpTraceFunction;
			void*               mpTraceFunctionContext;
			unsigned char       mcTraceFieldDelimiter;            /// Delimits fields in an allocation trace. Defaults to '\t'.
			unsigned char       mcTraceRecordDelimiter;           /// Delimits records in an allocation trace. Defaults to '\n'.

			// Auto heap validation
			bool mbHeapValidationActive; /// Used to prevent recursion.

			#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
				HeapValidationLevel mAutoHeapValidationLevel;           /// Used to indicate a heap validation level or to indicate that heap validation is disabled.
				size_t              mnAutoHeapValidationFrequency;      /// This is a value from 1 to N, where 1 means to validate every frame. 
				size_t              mnAutoHeapValidationEventCount;     /// This is the current number of events to consider for validation. 
			#endif

			// Thread safety
			#if PPM_THREAD_SAFETY_SUPPORTED
				void*    mpMutex;                       /// Pointer to mutex or NULL if thread safety is disabled.
				uint64_t mpMutexData[kMutexBufferSize]; /// Provides memory from which the mutex is allocated (so we don't have to allocate it from a heap). Format of this data is platform-specific.
			#endif

			// Debug options
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				DebugDataInfo   mDebugDataInfo;
				unsigned        mnDebugDataFlags;        /// Global debug flags for any allocation.
				int             mnCurrentGroupId;        /// Current group Id. Only used if mnDebugDataFlags has the kDebugDataIdGroup flag on.

				// Guard settings
				float       mfGuardSizeRatio;        /// How much guard fill to have relative to allocation size. 0.5 means use 8 bytes of guard fill for a 16 byte allocation.
				size_t      mnMinGuardSize;          /// Min guard size not to go below.
				size_t      mnMaxGuardSize;          /// Max guard size not to go above.

				// Debug fill values
				unsigned char mnFillFree;
				unsigned char mnFillDelayedFree;
				unsigned char mnFillNew;
				unsigned char mnFillGuard;
				unsigned char mnFillUnusedCore;
			#endif

			// Hooks
			#if PPM_NLA_HOOKS_SUPPORTED
				HookFunction mpHookFunction; 
				void*        mpHookFunctionContext;
			#endif

			// Metrics
			#if PPM_NLA_METRICS_SUPPORTED
				bool          mbMetricsEnabled;                       /// If true, then extensive allocation metrics are tracked. 
				uint64_t      mnAllocationCountHistorical;            /// The number of allocations that have ever occurred. Maintained even if mbMetricsEnabled is false. This value is also the value assigned to a block with the kDebugDataIdAllocationCount tag.
				Metrics       mAllocationMetrics[kMetricTypeCount];   /// Stores general allocation metrics. Maintained if mbMetricsEnabled is true.
			#endif

		}; // NonLocalAllocator

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
		unsigned NonLocalAllocator::GetDefaultDebugDataFlags() const
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				return mnDebugDataFlags;
			#else
				return 0;
			#endif
		}


		inline
		void NonLocalAllocator::SetDefaultDebugDataFlags(unsigned flags)
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				mnDebugDataFlags = flags;
			#else
				EA_UNUSED(flags);
			#endif
		}


		inline
		void NonLocalAllocator::SetDefaultDebugDataFlag(unsigned debugDataFlag)
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				mnDebugDataFlags = mnDebugDataFlags | (1 << debugDataFlag);
			#else
				EA_UNUSED(debugDataFlag);
			#endif
		}


		#if PPM_NLA_DEBUG_DATA_SUPPORTED
			inline
			size_t NonLocalAllocator::GetDebugDataRecordLength(size_t nDebugDataLength)
			{
				return (nDebugDataLength > 0) ? (nDebugDataLength + kDebugRecordOverheadSize) : 0;
			}
		#endif


		inline
		int NonLocalAllocator::GetCurrentGroupId() const
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				return mnCurrentGroupId;
			#else
				return 0;
			#endif
		}


		inline
		int NonLocalAllocator::SetCurrentGroupId(int nGroupId)
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				const int nPreviousGroupId = mnCurrentGroupId;
				mnCurrentGroupId = nGroupId;
				return nPreviousGroupId;
			#else
				EA_UNUSED(nGroupId);
				return 0;
			#endif
		}


		inline
		NonLocalAllocator::size_type NonLocalAllocator::Node::IsInUse() const
		{
			return (mnBlockSize & kNodeInUseBit);
		}


		inline
		void NonLocalAllocator::Node::SetIsInUse(bool bInUse)
		{
			if(bInUse)
				mnBlockSize |= kNodeInUseBit;
			else
				mnBlockSize &= ~kNodeInUseBit;
		}


		inline
		NonLocalAllocator::size_type NonLocalAllocator::Node::GetSize() const
		{
			return (mnBlockSize & ~kNodeInUseBit);
		}


		inline
		void NonLocalAllocator::Node::SetSize(size_type size)
		{
			mnBlockSize = ((size & ~kNodeInUseBit) | (mnBlockSize & kNodeInUseBit));
		}


		inline
		NonLocalAllocator::size_type NonLocalAllocator::Node::GetUserSize() const
		{
			return (mnBlockSize & ~kNodeInUseBit) - (size_type)(uintptr_t)(mpData - mpBlock);
		}


		inline
		void NonLocalAllocator::Node::InsertIntoMemList(Node* pMemPrev)
		{
			mpMemPrev = pMemPrev;
			mpMemNext = pMemPrev->mpMemNext;
			 pMemPrev->mpMemNext = this;
			mpMemNext->mpMemPrev = this;
		}


		inline
		void NonLocalAllocator::Node::RemoveFromFreeList()
		{
			mpListPrev->mpListNext = mpListNext;
			mpListNext->mpListPrev = mpListPrev;

			#if (PPM_DEBUG >= 1)
				mpListPrev = NULL;
				mpListNext = NULL;
			#endif
		}


		inline
		void NonLocalAllocator::Node::InsertIntoFreeList(Node* pListPrev)
		{
			mpListPrev = pListPrev;
			mpListNext = pListPrev->mpListNext;
			 pListPrev->mpListNext = this;
			mpListNext->mpListPrev = this;
		}

		inline
		size_t NonLocalAllocator::HashTable::Hash(const void* pData) const
		{
			// To consider: Make bucket counts always be a power-of-two, and make this be an AND instead of a modulus.
			return (size_t)(((uintptr_t)pData / kMinAlignment) % mnBucketCount);
		}


		inline
		NonLocalAllocator::Node* NonLocalAllocator::AllocateNode()
		{
			// By design we don't check the pointer for NULL. That would use CPU cycles to 
			// check for something that simply always must be true.
			Node* const pNode = (Node*)mpLocalAllocationFunction(this, sizeof(Node), mpLocalAllocationContext);
			return pNode;
		}


		inline
		void NonLocalAllocator::DeallocateNode(NonLocalAllocator::Node* pNode)
		{
			// By design we don't check the pointer for NULL. That would use CPU cycles to 
			// check for something that simply always must be true.
			mpLocalFreeFunction(this, pNode, sizeof(Node), mpLocalAllocationContext);
		}


		inline
		NonLocalAllocator::CoreBlock* NonLocalAllocator::AllocateCoreBlock()
		{
			// By design we don't check the pointer for NULL. That would use CPU cycles to 
			// check for something that simply always must be true.
			CoreBlock* const pCoreBlock = (CoreBlock*)mpLocalAllocationFunction(this, sizeof(CoreBlock), mpLocalAllocationContext);
			return pCoreBlock;
		}


		inline
		void NonLocalAllocator::DeallocateCoreBlock(CoreBlock* pCoreBlock)
		{
			// By design we don't check the pointer for NULL. That would use CPU cycles to 
			// check for something that simply always must be true.
			mpLocalFreeFunction(this, pCoreBlock, sizeof(CoreBlock), mpLocalAllocationContext);
		}


	} // namespace Allocator

} // namespace EA



#endif // Header include guard


