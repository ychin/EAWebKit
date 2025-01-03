///////////////////////////////////////////////////////////////////////////////
// EAGeneralAllocatorDebug (a.k.a. PPMallocDebug)
// 
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This module implements a user-level debug layer on top of GeneralAllocator. 
//
// GeneralAllocator provides some basic self-debugging functionality but doesn't 
// provide the user with higher level debugging functionality that would be
// useful for tracking and debugging memory at the application level.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EAGENERALALLOCATORDEBUG_H
#define PPMALLOC_EAGENERALALLOCATORDEBUG_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class GeneralAllocatorDebug
		///
		/// \brief Implements a user debug layer on top of GeneralAllocator.
		///
		/// If you want this class to work as expected, then you will need to 
		/// #define PPM_VIRTUAL in order to make key GeneralAllocator functions virtual.
		///
		/// Debug information tied to blocks uses a flexible tagged format which 
		/// allows arbitrary data to a block, including custom user data. This
		/// debug information is either stored within the allocation block itself
		/// or stored independently, depending on the user's preference. The format
		/// of the debug information is the same either way:
		///        <data2 data><data2 id><data2 size> <data1 data><data1 id><data1 size> <size of all debug data>
		/// Note that the ordering of the fields is reverse from what would seem
		/// natural. The reason for this is that this data is at the *end* of the 
		/// allocated block and so it is walked right-to left-rather than the more
		/// natural left-to-right.
		///
		class PPM_API GeneralAllocatorDebug : public GeneralAllocator
		{ 
		public:
			/// kTypeId
			/// Defines what type of allocator this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class 
			/// such as GeneralAllocator to a subclass such as GeneralAllocatorDebug.
			static const int kTypeId = 0x8843221a; // This is just some unique value.

			/// GeneralAllocatorDebug
			GeneralAllocatorDebug(void* pInitialCore = NULL, size_t nInitialCoreSize = 0, 
								  bool bShouldFreeInitialCore = true, bool bShouldTrimInitialCore = false, 
								  CoreFreeFunction pInitialCoreFreeFunction = NULL, void* pInitialCoreFreeFunctionContext = NULL,
								  CoreExtendFunction pInitialCoreExtendFunction = NULL, void* pInitialCoreExtendFunctionContext = NULL);

			GeneralAllocatorDebug(const Parameters& parameters);


			/// ~GeneralAllocatorDebug
		   ~GeneralAllocatorDebug();

			/// Init
			/// Works the same way as GeneralAllocator::Init.
			bool Init(void* pInitialCore = NULL, size_t nInitialCoreSize = 0,
					  bool bShouldFreeInitialCore = true, bool bShouldTrimInitialCore = false, 
					  CoreFreeFunction pInitialCoreFreeFunction = NULL, void* pInitialCoreFreeFunctionContext = NULL,
					  CoreExtendFunction pInitialCoreExtendFunction = NULL, void* pInitialCoreExtendFunctionContext = NULL);

			bool Init(const Parameters& parameters);

			/// Shutdown
			/// Works the same way as GeneralAllocator::Shutdown.
			bool Shutdown();


			////////////////////////////////////////////////////////////////////////////
			// GeneralAllocator functionality
			////////////////////////////////////////////////////////////////////////////

			PPM_VIRTUAL void*  Malloc(size_t nSize, int nAllocationFlags = 0);
			PPM_VIRTUAL void*  MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, int nAllocationFlags = 0);
			PPM_VIRTUAL void*  Calloc(size_t nElementCount, size_t nElementSize, int nAllocationFlags = 0);
			PPM_VIRTUAL void*  Realloc(void* p, size_t nNewSize, int nAllocationFlags = 0);
			PPM_VIRTUAL void   Free(void* p);

			PPM_VIRTUAL size_t GetUsableSize(const void* pData) const;
			PPM_VIRTUAL bool   ValidateChunk(const void* pChunk, HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);
			PPM_VIRTUAL bool   ValidateHeap(HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);
			PPM_VIRTUAL void   TraceAllocatedMemory(TraceFunction pTraceFunction = NULL, void* pTraceFunctionContext = NULL, void* pStorage = NULL, size_t nStorageSize = 0, int nBlockTypeFlags = kBlockTypeAllocated);
			PPM_VIRTUAL void*  TakeSnapshot(int nBlockTypeFlags = kBlockTypeAll, bool bMakeCopy = false, void* pStorage = NULL, size_t nStorageSize = 0);
			PPM_VIRTUAL void   FreeSnapshot(void* pSnapshot);
			PPM_VIRTUAL void*  AsInterface(int id);

			////////////////////////////////////////////////////////////////////////////
			// GeneralAllocatorDebug functionality
			////////////////////////////////////////////////////////////////////////////

			/// SetAllocator
			/// Sets the allocator which this class uses internally for data tracking.
			/// By default, this class uses itself for data tracking allocations, but 
			/// you can have it use an independent external allocator if desired. 
			/// This independent allocator must be set before any allocations are done.
			void SetAllocator(GeneralAllocator* pAllocator);


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
			unsigned GetDefaultDebugDataFlags() const;


			/// GetDefaultDebugDataFlag
			/// Returns true if the given debug data flag is set. You pass one
			/// of enum DebugDataId. This function thus returns true if the 
			/// following is non-zero: currentFlags & (1 << debugDataFlag)
			///
			/// Example usage:
			///    bool result = pAllocator->GetDefaultDebugDataFlag(kDebugDataIdSize);
			///    if(result)
			///        printf("kDebugDataIdSize is enabled\n").
			///
			bool GetDefaultDebugDataFlag(unsigned debugDataFlag) const;


			/// SetDefaultDebugDataFlags
			/// Sets the default debug data flags to use for all allocations.
			/// To specify that you want store the call stack with all allocations, 
			/// you use '1 << kDebugDataIdCallStack' as an input flag.
			///
			/// Example usage:
			///    pAllocator->SetDefaultDebugDataFlags((1 << kDebugDataIdCallStack) | (1 << kDebugDataIdSize));
			///
			void SetDefaultDebugDataFlags(unsigned flags);


			/// SetDefaultDebugDataFlag
			/// Sets an individual flag, indicated by enum DebugDataId. To enable
			/// the kDebugDataIdCallStack flag, you simply call this function with
			/// kDebugDataIdCallStack as an argument. This function has the same
			/// effect as: currentFlags |= (1 << debugDataFlag).
			///
			/// Example usage:
			///    pAllocator->SetDefaultDebugDataFlag(kDebugDataIdCallStack);
			///
			void SetDefaultDebugDataFlag(unsigned debugDataFlag);


			// Common debug parameter versions of allocation functions.
			// These functions are extended versions of the standard functions
			// but let you specify debug settings per-allocation.
			// The nDebugDataFlags argument is a set of flags that derive 
			// from enum DebugDataId by shifts. To specify that you want the 
			// allocation to store the call stack, you use '1 << kDebugDataIdCallStack'
			// as the debug data flag. See GetDefaultDebugDataFlags() for more.
			// These settings don't replace the default settings but are in 
			// addition to them. With the current implementation of this library,
			// the allocator is thread-locked during any calls to external hooks
			// that have been installed. This may be different from the normal
			// locking behaviour with GeneralAllocator.
			PPM_VIRTUAL void*  MallocDebug(size_t nSize, int nAllocationFlags = 0,
										   unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);
			PPM_VIRTUAL void*  MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags = 0,
												  unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);
			PPM_VIRTUAL void*  CallocDebug(size_t nElementCount, size_t nElementSize, int nAllocationFlags = 0,
										   unsigned nDebugDataFlags = 0, const char* pName = NULL, const char* pFile = NULL, int nLine = 0);


			/// DelayedFreePolicy
			enum DelayedFreePolicy
			{
				kDelayedFreePolicyNone,    /// Delayed free not in effect.
				kDelayedFreePolicyCount,   /// Free after a certain count of memory allocations are freed.
				kDelayedFreePolicyVolume,  /// Free after a certain volume of of bytes of memory is freed.
				kDelayedFreePolicyTime     /// Free after a certain amount of time in seconds has passed.
			};

			/// SetDelayedFreePolicy
			/// Setting the delayed free policy to kDelayedFreePolicyNone will leave the 
			/// delayed free fill value as-is. However, setting the delayed free fill value to 
			/// kDefaultFillValueDisabled will set the delayed free policy to kDelayedFreePolicyNone.
			/// It is debateable whether this is consistent or inconsistent behaviour; there are 
			/// arguments both ways. Setting the delayed free fill to a value other than 
			/// kDefaultFillValueDisabled does not enable delayed freeing, as a policy must
			/// be selected via the SetDelayedFreePolicy function.
			///
			/// Example usage:
			///    // Delay the freeing of allocations till another 1000 allocations have been Free'd.
			///    pAllocator->SetDelayedFreePolicy(GeneralAllocatorDebug::kDelayedFreePolicyCount, 1000);
			///
			PPM_VIRTUAL void SetDelayedFreePolicy(DelayedFreePolicy policy, int value);


			/// ClearDelayedFreeList
			/// Removes all delayed free blocks unilaterally.
			PPM_VIRTUAL void ClearDelayedFreeList();


			/// AllocationIsDelayedFree
			/// Returns true if the given previously freed pointer is part of the delayed free list.
			/// This function scans the list linearly and thus is slower as the delayed free list is larger.
			PPM_VIRTUAL bool AllocationIsDelayedFree(const void* pData) const;


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
			/// allocated pointer for the block that the address comes from. This will be a value
			/// that is <= the input address.
			/// If the AddressType is kAddressTypeSpecific, then the return value is equal to the 
			/// input pointer if the input pointer refers to an allocated pointer returned by the
			/// allocator. Thus, it is a pointer value that can be freed.
			/// If any validation fails, the return value is NULL.
			///
			/// This function will internally check guard fills if kDebugDataIdGuard is set as
			/// a default flag or if PPM_DEBUG is >= 2. Note that it is possible that 
			/// blocks can have guard fills even if kDebugDataIdGuard is not a default flag.
			///
			/// Example usage:
			///    void* p = pAllocator->Malloc(1);
			///    void* pResult = pAllocatorValidateAddress(pData, GeneralAllocator::kAddressTypeSpecific);
			///
			PPM_VIRTUAL const void* ValidateAddress(const void* pData, int addressType = kAddressTypeAllocated) const;


			/// SetFillValues
			/// Sets fill values for newly allocated or newly freed memory. 
			/// Specifying a fill value of kDefaultFillValueDisabled will disable the given fill.
			/// Setting the delayed free fill value to kDefaultFillValueDisabled will set also 
			/// the delayed free policy to kDelayedFreePolicyNone. Setting the guard fill value 
			/// to kDefaultFillValueDisabled will also have the effect of calling
			/// SetDefaultDebugDataFlag(kDebugDataIdGuard) in addition to setting the fill value.
			/// However, setting delayed free or guard fill values to something other than 
			/// kDefaultFillValueDisabled, will not automatically enable those options, as more 
			/// information would be needed about hot to enable them. 
			PPM_VIRTUAL void SetFillValues(unsigned char cFree        = kDefaultFillValueFree, 
										   unsigned char cDelayedFree = kDefaultFillValueDelayedFree, 
										   unsigned char cNew         = kDefaultFillValueNew, 
										   unsigned char cGuard       = kDefaultFillValueGuard,
										   unsigned char cUnusedCore  = kDefaultFillValueUnusedCore);

			/// SetGuardSize
			/// Sets the size of the guard fill, which refers to extra memory associated with each
			/// block which is expected to be filled with a constant value. This is useful in 
			/// detecting bad memory writes. Setting fGuardSizeRatio to <= 0 or setting nMaxGuardSize
			/// to zero will have the effect of disabing guard fills. It will do so by acting as if
			/// SetDefaultDebugDataFlag(kDebugDataIdGuard) was called.
			///
			/// Example usage:
			///    // Set the guard to be 3 times the size of the user's requested size, 
			///    // but make it at always at least 128 but never more than 1024.
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
			int GetCurrentGroupId() const;


			/// SetCurrentGroupId
			/// Sets the current group id, returning the previous one. If you want to 
			/// disable the assignment of group ids, use SetDefaultDebugDataFlags.
			int SetCurrentGroupId(int nGroupId);


			/// enum DebugDataLocation
			enum DebugDataLocation
			{
				kDebugDataLocationBlock,    /// Refers to debug data stored within a block.
				kDebugDataLocationSeparate, /// Refers to debug data stored in a separate location.
				kDebugDataLocationDefault   /// Refers to whatever is the default location.
			};

			/// SetDefaultDebugDataLocation
			/// Sets where debug data records go by default.
			/// Currently you must call this function before any allocations have occurred.
			PPM_VIRTUAL void SetDefaultDebugDataLocation(DebugDataLocation debugDataLocation);


			/// enum DebugDataId
			/// Defines standard names for debug information tied to a block. The list of possible
			/// debug information tied to a block is not limited to these values. For blocks that
			/// are reallocated, the kDebugDataIdSize and kDebugDataIdOverhead data are updated,
			/// but no other debug data is automatically updated.
			///
			/// Example usage of kDebugDataIdCallStack:
			///     size_t addrCount = 0;
			///     void*  pReturnAddressArray[24];
			///
			///     if(pAllocator->GetDebugData(pData, kDebugDataIdCallStack, pReturnAddressArray, sizeof(pReturnAddressArray), &addrCount) && addrCount) {
			///         addrCount /= sizeof(void*);
			///
			///         if((pBufferEnd - pBufferCurrent) > 32) {  // If there's enough room for at least "stack: 0x1234567812345678\t\0"
			///             char buffer[512]; // You'll want to use a package like EACallstack to handle the description of callstacks across many platforms.
			///             const size_t nCharsWritten = EA::Allocator::DescribeCallStack((const void**)pReturnAddressArray, addrCount, buffer, EAArrayCount(buffer) - 1, nullptr);
			///             buffer[nCharsWritten] = 0;
			///             printf("%s", buffer);
			///         }
			///     }

			enum DebugDataId
			{
				kDebugDataIdNone            =    0,   /// Identifies unused debug data.
				kDebugDataIdAllocator       =    1,   /// Pointer to GeneralAllocatorDebug which the block belongs to. Format: struct{ GeneralAllocatorDebug* p; };
				kDebugDataIdSize            =    2,   /// Size requested by user for block. Format: struct{ size_t n; };
				kDebugDataIdFlags           =    3,   /// Allocation flags requested by user for block. Field is absent if flags were zero. Format: struct{ int flags; };
				kDebugDataIdAlignment       =    4,   /// The alignment and possible alignment offset the user requested for the block. Format: struct AlignmentInfo{ size_t a; size_t o; }; The o is ommitted if it is zero. The record itself is omitted if a is zero.
				kDebugDataIdName            =    5,   /// Name for allocation. Format: struct{ char name[]; }; where name is 0-terminated.
				kDebugDataIdPlace           =    6,   /// Place of allocation as string: __FILE__ / __LINE__. Format: struct FileLineInfo{ const char* path, int line };
				kDebugDataIdCallStack       =    7,   /// Call stack for allocation. Format: struct{ void* addr[]; };  The count of the array returned from GetDebugData is (pActualDataLength / sizeof(void*)).
				kDebugDataIdChecksum        =    8,   /// Checksum for user user data, for blocks allocated as read-only. Format: struct{ size_t n; };
				kDebugDataIdAllocationCount =    9,   /// Index of allocation, unique for each allocation. Format: struct{ uint64_t n; }; Use the GetAllocationCount function to get the value most recently assigned to a block. The value stored with the block is the same as the GetAllocationCount after the block was allocated. The first allocation is given a value of zero.
				kDebugDataIdAllocationTime  =   10,   /// Time of allocation. Format: struct{ size_t n; };
				kDebugDataIdGuard           =   11,   /// Guard data between blocks. Format: struct{ char fill[]; };
				kDebugDataIdOverhead        =   12,   /// The system overhead for this block. Format: struct{ size_t n; }; There is a problem here though because the system overhead for a debug block is always higher than with an optimized build. In an optimized build, the nominal overhead for a block is four bytes (you heard that right).
				kDebugDataIdGroup           =   13,   /// Allocation family group id. Most common use is to detect leaks by asserting that there are no more blocks of a given group after leaving a given stage of the app. Format: struct{ int id; };
				kDebugDataIdInternalCount   =   14,   /// Count of internal types.
				kDebugDataIdUser            = 1000    /// User-defined values begin here.
			};

			struct FileLineInfo{
				const char* mpFile;
				int         mnLine;
			};

			struct AlignmentInfo{
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
				kDebugRecordOverheadSize = kDebugRecordSizeSize + kDebugRecordIdSize,
				kMaxDebugDataSize        = (1 << (kDebugRecordSizeSize * 8)) - 1
			};


			/// GetDebugDataLength
			/// Returns length of all debug data in a record. You can use this
			/// information to tell how much overhead the record has in debug data.
			/// If the input ppDebugData is non-NULL, it will be returned with a
			/// pointer to the beginning of the debug data in the block.
			/// Returns 0 if debug data is not present.
			PPM_VIRTUAL size_t GetDebugDataLength(const void* pData, DebugDataLocation debugDataLocation = kDebugDataLocationDefault, void** ppDebugData = NULL);


			/// GetDebugDataRecordLength
			/// Returns how much total memory is used to store debug information.
			static size_t GetDebugDataRecordLength(size_t nDebugDataLength);


			/// GetDebugData
			/// Gets debug data identified by an id from a user block.
			/// Returns the pointer to the data within the block.
			/// If the input 'pDebugData' or input 'nDataLength' is zero, the data is not copied.
			/// This function is thread-safe.
			PPM_VIRTUAL void* GetDebugData(const void* pData, DebugDataIdType id, 
										   void* pDebugData, size_t nDataLength, size_t* pActualDataLength = NULL, 
										   DebugDataLocation debugDataLocation = kDebugDataLocationDefault) const;

			/// GetDebugDataStatic
			/// Gets debug data from an arbitrary user block.
			/// This function is similar to GetDebugData except that there is no DebugDataLocation
			/// and thus this function can operate on any allocation block. It is the user's 
			/// responsibility to ensure that the input pData value is valid and doesn't change
			/// during the course of this function.
			/// This function is not thread-safe. If the memory is freed during the course of this
			/// function, the results are unpredictable.
			static void* GetDebugDataStatic(const char* pData, size_t nDebugBlockLength, DebugDataIdType id,
											void* pDebugData, size_t nDataLength, size_t* pActualDataLength = NULL);

			/// SetDebugData
			/// Adds the given debug data identified by id to a user block.
			/// If debug data already exists for the given id, it is overwritten if the new
			/// size is less or equal to the existing size. Otherwise the function fails.
			/// Returns the pointer to the memory for the debug data in the user block.
			/// Returns NULL if the operation failed, usually due to an out-of-memory condition.
			/// If the input 'pDebugData' is null or input 'nDataLength' is zero, the data is not written.
			/// This function is thread-safe.
			PPM_VIRTUAL void* SetDebugData(void* pData, DebugDataIdType id, 
										   const void* pDebugData, size_t nDataLength, 
										   DebugDataLocation debugDataLocation = kDebugDataLocationDefault);

			/// SetDebugData
			/// Adds the given debug data identified by id to a user block.
			/// This function is similar to GetDebugData except that there is no DebugDataLocation
			/// and thus this function can operate on any allocation block.
			/// This function is not thread-safe. If the memory is freed during the course of this
			/// function, the results are unpredictable.
			static void* SetDebugDataStatic(char* pDebugBlock, size_t nDebugBlockLength, DebugDataIdType id, 
											const void* pDebugData, size_t nDataLength);

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
				//uint64_t mnFreeVolume;                    /// Sum of free memory. Disabled until there is a practical way to use it.
				uint64_t   mnFreeVolumeHistorical;          /// Sum of memory ever freed by user.
			};

			enum MetricType
			{
				kMetricTypeAll,     /// Summary, regardless of system, debug, user usage.
				kMetricTypeSystem,  /// System memory. This doesn't include debug or user memory. This is essentially per-system and per-chunk overhead.
				kMetricTypeDebug,   /// Debug memory. This is memory which is exclusively allocated by GeneralAllocatorDebug. 
				kMetricTypeUser,    /// User memory. This is memory which is requested by the user. Includes internally fragmented memory (unrequested memory that comes with a block returned to a user).
				kMetricTypeCount    /// A count of the available types.
			};

			/// GetMetrics
			/// Reutrns metrics of the given type.
			PPM_VIRTUAL const Metrics& GetMetrics(MetricType metricsType);

			/// GetAllocationCount
			/// Returns the number of allocations which have occurred. An allocation refers to 
			/// any call to Malloc, MallocAligned, Calloc, or Realloc. The allocation count is 
			/// an ever-increasing value; it is not a count of the total number of currently  
			/// allocated blocks. The allocation count metric is maintained and thus thisfunction
			/// is usable even if metrics collection is disabled via kOptionEnableMetrcis.
			/// The return value will be zero if no allocations have occurred. This function can 
			/// be used in tandem with the kDebugDataIdAllocationCount debug tag.
			PPM_VIRTUAL uint64_t GetAllocationCount() const;

			/// enum OptionDebug
			/// This is an extension of the GeneralAllocator::Option enumeration.
			enum OptionDebug
			{
				kOptionEnableMetrics = 1000, /// If non-zero, enabled metrics collection, otherwise disable it. Disabled by default. This option incurs some runtime time cost.
				kOptionEnablePtrValidation   /// If non-zero, enabled pointer validation, otherwise disable it. Disabled by default. This option incurs some runtime memory and time costs. If kDebugDataLocationSeparate is set for the default debug data location, then this option is enabled.
			};

			/// SetOption
			/// The user specifies one of enum Option. Note that the argument 'option' is 
			/// of type int instead of type Option because subclasses may want to extend
			/// the option set to include values not listed here.
			PPM_VIRTUAL void SetOption(int option, int64_t nValue);

			/// SetAddressRepLookup
			/// Set's an IAddressRepLookupBase instance which will be used for generating more user
			/// friendly callstacks with source/symbol information when possible.
			/// Passing a value of nullptr will prevent an instance from being used to resolve callstacks
			/// and a basic "hex dump" for callstacks will be used.
			void SetAddressRepLookup(EA::Callstack::IAddressRepLookupBase* addressRepLookup)
			{
				mAddressRepLookup = addressRepLookup;
			}

		protected:
			friend class NonLocalAllocator;

			struct DebugDataInfoShared{
				size_t            mnRecordSize;
				size_t            mnRecordDataSize;
				DebugDataLocation mDebugDataLocation;
			};

			struct DebugDataInfo{
				size_t              mnDebugDataSize;
				DebugDataInfoShared mShared[kDebugDataIdInternalCount];
				size_t              mnBlockUsage;
				size_t              mnSeparateUsage;

				size_t              mnUserSize;                // kDebugDataIdSize
				int                 mnAllocationFlags;         // kDebugDataIdFlags
				size_t              mnAlignment;               // kDebugDataIdAlignment
				size_t              mnAlignmentOffset;         // kDebugDataIdAlignment
				FileLineInfo        mFileLineInfo;             // kDebugDataIdPlace
				void*               mpReturnAddressArray[24];  // kDebugDataIdCallStack
			};

			void*             MallocInternal(size_t nSize, int nAllocationFlags = 0); // Used to trap internal allocations so we can attach empty debug data to them.
			bool              CallMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext);
			DebugDataLocation GetDefaultDebugDataLocation(DebugDataIdType id) const;
			size_t            GetDebugDataLengthInternal(const char* pDebugBlock, size_t nDebugBlockLength, void** ppDebugData) const;
			size_t            FormatPlace(char* pBuffer, size_t nBufferLength, const char* pFile, int nLine, const char* pFunction) const;
			void              GetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, size_t nSize, int nAllocationFlags, size_t nAlignment = 0, size_t nAlignmentOffset = 0);
			bool              SetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, void* pData, bool bAttemptNewFill);
			void              ClearDelayedFreeList(DelayedFreePolicy delayedFreePolicy, int nDelayedFreePolicyValue);
			size_t            GetGuardSizeForBlock(size_t nBlockSize);
			bool              VerifyGuardFill(const Chunk* pChunk, bool bChunkIsInAList, bool bForceVerify) const;
			bool              VerifyDelayedFreeFill(const Chunk* pChunk) const;
			void              FreeDelayedFreeChunk(const Chunk* pChunk);
			void              DoFailureNotification(const Chunk* pChunk, const char* pMessage) const;
			int               CheckDelayedFreeChunk(const Chunk* pChunk);
			size_t            DescribeChunk(const Chunk* pChunk, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const;
			void              CollectMetrics(const Chunk* pChunk, bool bMalloc);
			bool              AddToDebugDataMap(const void* pData, size_t nDebugDataLength, char* pDebugBlock, bool bAllowDuplicate);
			bool              RemoveFromDebugDataMap(const void* pData, bool bFreeDebugData);
			bool              VerifyFromDebugDataMap(const void* pData) const;
			static void*      HashAllocationFunction(size_t n, void* pContext);
			static void       HashFreeFunction(void* p, void* pContext);
			void*             AllocatorMalloc(size_t n);
			void              AllocatorFree(void* p);

		protected:
			bool        mbInitializedDebug;      /// If this class is initialized, separate from GeneralAllocator::mbInitialized.

			// Debug options
			unsigned    mnDebugDataFlags;        /// Global debug flags for any allocation.
			unsigned    mnNextDebugDataFlags;    /// Temporary debug flags for the next allocation.
			const char* mpNextAllocationName;    /// Temporary name for the next allocation.
			const char* mpNextAllocationFile;    /// Temporary file name (e.g. __FILE__) tied to the next allocation.
			int         mnNextAllocationLine;    /// Temporary line (e.g. __LINE__) tied to the next allocation.
			int         mnCurrentGroupId;        /// Current group Id. Only used if mnDebugDataFlags has the kDebugDataIdGroup flag on.
			int         mnPtrValidationDisabled; /// If non-zero, ptr validation is (temporarily) disabled. This allows us to do memory checks but intentionally skip the ptr validation part of the check.

			// Guard settings
			float       mfGuardSizeRatio;        /// How much guard fill to have relative to allocation size. 0.5 means use 8 bytes of guard fill for a 16 byte allocation.
			size_t      mnMinGuardSize;          /// Min guard size not to go below.
			size_t      mnMaxGuardSize;          /// Max guard size not to go above.

			// Delayed free settings
			DelayedFreePolicy mDelayedFreePolicy;
			int               mDelayedFreePolicyValue;
			Chunk             mDelayedFreeChunkListHead;
			size_t            mDelayedFreeListSize;
			size_t            mDelayedFreeVolume;

			// Metrics
			bool          mbMetricsEnabled;                       /// If true, then extensive allocation metrics are tracked. 
			uint64_t      mnAllocationCountHistorical;            /// The number of allocations that have ever occurred. Maintained even if mbMetricsEnabled is false. This value is also the value assigned to a block with the kDebugDataIdAllocationCount tag.
			Metrics       mAllocationMetrics[kMetricTypeCount];   /// Stores general allocation metrics. Maintained if mbMetricsEnabled is true.

			// Debug data map and valid pointer tracking.
			GeneralAllocator* mpAllocator;
			DebugDataLocation mnDebugDataLocationDefault;

			EA::Callstack::IAddressRepLookupBase* mAddressRepLookup;

			void Construct(const Parameters& parameters);

			/// HashTable
			/// Used for storing valid pointer lists and separated debug data.
			class PPM_API HashTable
			{
			public:
				typedef void* (*AllocationFunction)(size_t n, void* pContext);
				typedef void  (*FreeFunction)(void* p, void* pContext);

				HashTable() : mppHashTable(NULL), mbReHashing(false), mnHashTableSize(0), mnHashTableSizeMin(4096), mnHashElementCount(0), 
							  mpAllocationFunction(NULL), mpFreeFunction(NULL), mpFunctionContext(NULL) { }
			   ~HashTable() { Clear(true, false); }

				bool    Insert(const void* pKey, char* pValue);
				bool    Find(const void* pKey, char*** pppValue) const;
				bool    Remove(const void* pKey, bool bFreeValue);
				void    Clear(bool bDestroyTable, bool bFreeValues);
				void    Nuke();
				void    SetAllocationFunction(AllocationFunction pAllocationFunction, FreeFunction pFreeFunction, void* pFunctionContext)
							 { mpAllocationFunction = pAllocationFunction; mpFreeFunction = pFreeFunction; mpFunctionContext = pFunctionContext; }

			public: // This may or may not be better as protected or private.
				bool    ReHash(size_t nNewTableSize);
				size_t Hash(const void* pKey) const { return (size_t)(((uintptr_t)pKey / 8) % mnHashTableSize); }

				struct HashPair{
					HashPair(const void* pKey = NULL, char* pValue = NULL, HashPair* pNext = NULL) : mpKey(pKey), mpValue(pValue), mpNext(pNext) {} 

					const void* mpKey;
					char*       mpValue;
					HashPair*   mpNext;
				};

				HashPair**         mppHashTable;
				bool               mbReHashing;
				size_t             mnHashTableSize;
				size_t             mnHashTableSizeMin;
				size_t             mnHashElementCount;
				AllocationFunction mpAllocationFunction;
				FreeFunction       mpFreeFunction;
				void*              mpFunctionContext;
			};

			bool      mbHashTableEnabled;
			HashTable mHashTable;
		};

	} // namespace Allocator

} // namespace EA




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
		size_t GeneralAllocatorDebug::GetDebugDataRecordLength(size_t nDebugDataLength)
		{
			return (nDebugDataLength > 0) ? (nDebugDataLength + kDebugRecordOverheadSize) : 0;
		}


		inline
		unsigned GeneralAllocatorDebug::GetDefaultDebugDataFlags() const
		{
			return mnDebugDataFlags;
		}


		inline
		bool GeneralAllocatorDebug::GetDefaultDebugDataFlag(unsigned debugDataFlag) const
		{
			return (mnDebugDataFlags & (1 << debugDataFlag)) != 0;
		}


		inline
		void GeneralAllocatorDebug::SetDefaultDebugDataFlags(unsigned flags)
		{
			mnDebugDataFlags = flags;
		}


		inline
		void GeneralAllocatorDebug::SetDefaultDebugDataFlag(unsigned debugDataFlag)
		{
			mnDebugDataFlags = mnDebugDataFlags | (1 << debugDataFlag);
		}


		inline
		int GeneralAllocatorDebug::GetCurrentGroupId() const
		{
			return mnCurrentGroupId;
		}


		inline
		int GeneralAllocatorDebug::SetCurrentGroupId(int nGroupId)
		{
			const int nPreviousGroupId = mnCurrentGroupId;
			mnCurrentGroupId = nGroupId;
			return nPreviousGroupId;
		}


		inline
		GeneralAllocatorDebug::DebugDataLocation GeneralAllocatorDebug::GetDefaultDebugDataLocation(DebugDataIdType id) const
		{
			if(id == kDebugDataIdGuard)
				return kDebugDataLocationBlock;
			return mnDebugDataLocationDefault;
		}


		inline
		void GeneralAllocatorDebug::SetDefaultDebugDataLocation(DebugDataLocation debugDataLocation)
		{
			if(debugDataLocation != kDebugDataLocationDefault)
			{
				mnDebugDataLocationDefault = debugDataLocation;
				if(debugDataLocation == kDebugDataLocationSeparate)    // Currently the user must call this function before any alloations have occurred.
					SetOption(kOptionEnablePtrValidation, true);       // The reason is that if we enable pointer validation after allocations have occurred,
			}                                                          // the system will assert that the first allocations are not known to be valid pointers.
		}


	} // namespace Allocator

} // namespace EA





#endif // PPMALLOC_EAGENERALALLOCATORDEBUG_H












