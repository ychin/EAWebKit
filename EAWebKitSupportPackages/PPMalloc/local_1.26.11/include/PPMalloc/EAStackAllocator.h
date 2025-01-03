///////////////////////////////////////////////////////////////////////////////
// EAStackAllocator
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This is a custom C++ implementation of a stack allocator much like 
// the 'obstack' allocator used in the GNUC compiler project (libiberty), 
// among other things. The GNUC objstack allocator is a GPL
// (GNU Public License) piece of code and thus cannot be used
// without adhering to GPL provisions. Thus we implement an allocator
// here which has the same functionality but is not encumbered by 
// the GPL license. If you are familiar with the obstack allocator 
// then you should be comfortable with this allocator. Documentation
// for standard GNUC obstacks can be found at:
//     http://gcc.gnu.org/onlinedocs/libiberty/Obstacks.html#Obstacks
// This allocator is also similar to other memory stack allocators 
// such as the old Graphics Gems III fast allocator:
//     http://www.cse.ucsc.edu/~pang/160/f98/Gems/GemsIII/alloc/
// 
// Supported user-modifyable defines
//     PPM_INCREMENTAL_OBJECTS_ENABLED
//
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASTACKALLOCATOR_H
#define PPMALLOC_EASTACKALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/shared.h>
#include <stddef.h>
#include <string.h>


// Disable optimization under VC++ in order to track down a release build crash specific to VC++.
#if defined(_MSC_VER)
	#pragma optimize("", off)
#endif


/// namespace EA
///
/// This is the standard Electronic Arts C++ namespace.
///
namespace EA
{
	/// namespace Allocator
	///
	/// This is the standard Electronic Arts memory allocator C++ namespace.
	///
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class StackAllocator
		///
		/// Implements a memory stack, which allows you to allocate memory very 
		/// quickly. It works by starting with a block of memory and incrementing
		/// a pointer as the user makes allocation requests from it. Its primary
		/// advantage is that it is very fast and is very efficient with space. 
		/// Its primary disadvantage is that you cannot arbitrarily free the 
		/// memory you allocate from it. You can only free backwards from your
		/// most recent allocations. This allocator is nevertheless useful for 
		/// a number of situations.
		///
		/// This kind of allocator is sometimes referred to as a LinearAllocator.
		///
		/// Note that you can swap two StackAllocators via the std::swap function 
		/// or like so:
		///     void swap(StackAllocator& s1, StackAllocator& s2)
		///     {
		///         StackAllocator temp(s1);
		///         s1 = s2;
		///         s2 = temp;
		///     }
		/// 
		class PPM_API StackAllocator
		{
		public:
			// pSizeResult is the size the CoreAllocationFunction actually returned to the user. Upon success it 
			// will be >= nSize. If NULL is passed to pSizeResult, it should not be written to.
			typedef void* (*CoreAllocationFunction)(size_t nSize, size_t* pSizeResult, void* pContext);
			typedef void  (*CoreFreeFunction)(void* pBlock, void* pContext);

			/// AssertionFailureInfoFunction
			/// function signature of the allocators assert failure callback
			struct AssertionFailureInfo;
			typedef void (*AssertionFailureInfoFunction)(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);

			/// StackAllocator
			/// Class constructor
			///
			/// The arguments for this class work the same as with the Init function.
			/// An nInitialSize of -1 (~0) causes no initial allocation to take place.
			StackAllocator(void* pData = NULL,
						   size_t nInitialSize = (size_t)(~0),
						   CoreAllocationFunction pCoreAllocationFunction = NULL,
						   CoreFreeFunction pCoreFreeFunction = NULL,
						   void* pCoreFunctionContext = NULL,
						   AssertionFailureInfoFunction pAssertionFailureFunction = NULL, 
						   void* pAssertionFailureContext = NULL);

			/// StackAllocator
			/// Class copy constructor
			///
			/// Implements a conventional copy constructor
			StackAllocator(const StackAllocator& stackAllocator); 

			/// ~StackAllocator
			/// Class destructor
			~StackAllocator();

			/// operator=
			/// Class assignment operator.
			///
			/// Implements a coventional assignment operator.
			StackAllocator& operator=(const StackAllocator& stackAllocator); 

			/// Init
			/// Initialize the memory stack.
			///
			/// If the input pData is non-null, then it is assumed to have nInitialSize
			/// bytes of data. If pData is non-null, then memory is allocated internally
			/// of either nInitialSize bytes or a default value if nInitialSize is 0.
			/// nDefaultAlignment is the default alignment with which to allocate objects.
			/// pCoreAllocationFunction and pCoreFreeFunction are callback with which 
			/// more block memory will be allocated if it is exhausted. If these are 
			/// NULL then the default system allocator is used.
			void Init(void* pData = NULL,
					  size_t nInitialSize = 0,
					  CoreAllocationFunction pCoreAllocationFunction = NULL,
					  CoreFreeFunction pCoreFreeFunction = NULL,
					  void* pCoreFunctionContext = NULL,
					  AssertionFailureInfoFunction pAssertionFailureFunction = NULL,
					  void* pAssertionFailureContext = NULL);

			/// Shutdown
			/// Returns the instance to its originally constructed state.
			void Shutdown();

			/// Malloc
			/// Allocate an object of a given size and default alignment.
			void* Malloc(size_t nSize, bool bBoundsCheck = true);

			/// Calloc
			/// Allocate 'nElementCount' elements of 'nElementSize' bytes each, all initialized to 0.
			void* Calloc(size_t nElementCount, size_t nElementSize, bool bBoundsCheck = true);

			/// Realloc
			/// Re-allocate the previously allocated block in p, making the new block nNewSize bytes in size.
			///
			/// Due to the nature of this stack-like allocator, a new block is always created. 
			/// This function is not very efficient but is provided for compatibility with 
			/// conventional generalized allocators.
			void* Realloc(void* pData, size_t nNewSize, bool bBoundsCheck = true);

			/// MallocAligned
			/// Allocate 'nSize' bytes allocated to 'nAlignment' bytes.
			///
			/// Input 'nAlignment' must be a multiple power of 2. If it is not 
			/// a power of 2, the next greater power of 2 is used. The minimum 
			/// alignment returned is 8. This is much like the commonly seen 
			/// memalign function. The alignment offset parameter specifies 
			/// how many bytes into the allocation that the alignment should 
			/// be found.
			void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, bool bBoundsCheck = true);

			/// Free
			/// Frees a given object which was allocated via Malloc, etc.
			///
			/// This function is basically implemented as a no-op but is provided for
			/// compatibility with conventional generalized allocators. 
			void Free(void* pData);

			/// FreeObjects
			/// Frees all memory that is after the input pAddress.
			///
			/// This function acts as a rollback of the memory stack to a previous point in time. 
			/// The passed in pointer need not point to the base of a previously allocated object 
			/// but instead can refer to the middle of an object. However, the object which such 
			/// a use might split would be henceforth unusable.
			/// Any bookmarks that referred to space that was freed will be removed.
			void FreeObjects(const void* pAddress);

			/// Reset
			/// Frees all space allocated by the user and clears all bookmarks.
			///
			/// This function does not free internal core memory.
			void Reset();

			/// GetDefaultBlockSize
			/// Returns the default block size used to allocate new core blocks.
			size_t GetDefaultBlockSize();

			/// SetDefaultBlockSize
			/// Sets the default block size used to allocate new core blocks.
			void SetDefaultBlockSize(size_t nDefaultBlockSize);

			/// PushBookmark
			/// Records an allocation position for a later call to PopBookmark.
			///
			/// Returns the address the bookmark refers to or NULL if the operation 
			/// could not occur (typically due to an out-of-memory condition).
			/// If the input pAddress is NULL, the current stack top is used.
			/// PushBookmark must later be matched by a PopBookmark, or else a memory
			/// leak may occur. The matching PopBookmark calls must be called in the 
			/// reverse order of the PushBookmark calls, else a memory leak may occur.
			/// The same address may be used for PushBookmark multiple times in a row.
			/// An address passed to PushBookmark cannot be older than another address
			/// previously passed to PushBookmark. For example, it is invalid to do this:
			///     void* p1 = stackAllocator.Malloc(10);
			///     void* p2 = stackAllocator.Malloc(20);
			///     stackAllocator.PushBookmark(p2);
			///     stackAllocator.PushBookmark(p1);  <--- Invalid.
			void* PushBookmark(const void* pAddress = NULL);

			/// PopBookmark
			/// Calls FreeObjects with the address that was last pushed via PushBookmark.
			/// Bookmarks must be popped in the opposite order they were pushed. However,
			/// you can pop multiple bookmarks at a time by popping the oldest of them.
			/// For example, this is valid:
			///     void* p1 = stackAllocator.Malloc(10);
			///     void* p2 = stackAllocator.Malloc(20);
			///     void* p3 = stackAllocator.Malloc(30);
			///     stackAllocator.PushBookmark(p1);
			///     stackAllocator.PushBookmark(p2);
			///     stackAllocator.PushBookmark(p3);
			///     stackAllocator.PopBookmark(p1);    <--- Has the effect of popping p3, p2, and p1.
			void PopBookmark();

			/// GetTopBookmark
			/// Returns the value of the most recently pushed bookmark.
			void* GetTopBookmark();

			/// GetMemoryUsage
			/// Returns the amount of memory currently allocated.
			size_t GetMemoryUsage();

			/// Runtime Options
			enum Option
			{
				kOptionNone                      =  0,
				kOptionEnableMallocFailureAssert =  1       /// If enabled then Malloc functions execute PPM_ASSERT(false) before returning. This is disabled by default.
			};

			/// The user specifies one of enum Option. Note that the argument 'option' is 
			/// of type int instead of type Option because subclasses may want to extend
			/// the option set to include values not listed here.
			void    SetOption(int option, int64_t nValue);
			int64_t GetOption(int option) const;

			/// AddressType
			/// Used to categorize pointers for the ValidateAddress function.
			enum AddressType
			{
				kAddressTypeOwned     = -1, /// The memory referred to by the pointer is owned by the given allocator. It may be allocated or free.
				kAddressTypeAllocated =  0, /// The memory referred to by the pointer is both owned by the given allocator and refers to valid user-allocated memory, but may not be the allocated pointer itself but merely with the allocated block.
				kAddressTypeSpecific  =  1  /// The memory referred to by the pointer is both owned by the given allocator, refers to valid user-allocated memory, and is also the allocated pointer itself and not merely any memory within the allocated block.
			};

			/// ValidateAddress
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
			bool ValidateHeap(HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);

			////////////////////////////////////////////////////////////////////////////
			// Incremental object building functionality
			//
			// This is useful for building streams of data on the fly and is also 
			// useful for building static string tables or similar tables on the fly.
			////////////////////////////////////////////////////////////////////////////
			#if PPM_INCREMENTAL_OBJECTS_ENABLED

				/// GetCurrentObjectBegin
				/// Returns the current value for the beginning of the current object. It is 
				/// possible that subsequent calls that append data to the current object could
				/// change the object's base position due to exhaustion of the current block.
				void* GetCurrentObjectBegin();

				/// GetCurrentObjectEnd
				/// Reurns the memory address of the memory right after the current object.
				/// This is the same as GetCurrentObjectBegin if no data has been appended 
				/// to the current object. It is also the address where the next data in the
				/// current object would be placed or where the next object would begin if 
				/// the current object were to be subsequently finished.
				void* GetCurrentObjectEnd();

				/// GetCurrentObjectSize
				/// Gets the size of the current object. The size of an object can continue
				/// to grow until FinishCurrentObject is called.
				size_t GetCurrentObjectSize();

				/// GetCurrentFreeSpace
				/// Returns the amount of space that is available in the current block for
				/// the current object. This function is useful for knowing how much space
				/// can be added to the current object before the current block is exhausted.
				size_t GetCurrentFreeSpace();

				/// ExtendCurrentObject
				/// Increases the object currently being allocated by nSize bytes.
				/// 
				/// The data is uninitialized. If bBoundsCheck is false, then it is assumed 
				/// that the current object can be extended without running out of block 
				/// space and so bounds checks are skipped. It is intended that this function 
				/// be called with bBoundsCheck as a constant (e.g. 'true') expression, 
				/// as this function is implemented inline and a constant expression will 
				/// cause any optimizing compiler to discard all but the code that is 
				/// necessary to complete the operation.
				bool ExtendCurrentObject(size_t nSize, bool bBoundsCheck = true);

				/// AppendToCurrentObject
				/// This function is like ExtendCurrentObject except that the newly extended
				/// space is initialized with data pointed at by pData. As with ExtendCurrentObject,
				/// the bBoundsCheck argument should be a constant expression if you want to maximize
				/// efficiency.
				bool AppendToCurrentObject(const void* pData, size_t nSize, bool bBoundsCheck = true);

				/// AppendToCurrentObject
				/// This function is the same as AppendToCurrentObjectMemcpy except that the 
				/// object is copied by a C++-level object copy and placement operator new at 
				/// the destination. This allows a deep copy of an object. In using this 
				/// version you must ensure that the destination is aligned properly for
				/// the object. The copied object's constructor is called upon copy and
				/// thus it is possible that if the current object gets moved as a result
				/// of exhaustion of the current block space. As a result, objects appended
				/// via this means must execute code in their constructors which makes them
				/// dependent on their location at the time of the construction. 
				template<class T>
				bool AppendToCurrentObject(const T& t, bool bBoundsCheck = true);

				/// AppendToCurrentObjectMemcpy
				/// This function appends an arbitrary object to the object currently being 
				/// allocated. In this version of the function, the input object t is copied
				/// via memcpy and not by a C++-level object copy. 
				template<class T>
				bool AppendToCurrentObjectMemcpy(const T& t, bool bBoundsCheck = true);

				/// FinishCurrentObject. 
				/// Completes the allocation of the current object and starts a new object.
				void* FinishCurrentObject();

			#endif // PPM_INCREMENTAL_OBJECTS_ENABLED


			/// AssertionFailureInfo
			/// Struct passed to user upon assertion failure.
			struct AssertionFailureInfo
			{
				const StackAllocator*   mpAllocator;          /// The allocator the assertion is associated with. May be NULL for failures that are without allocator context.
				const char*             mpExpression;         /// The assertion expression that failed.
				int                     mnErrorId;            /// One of enum GAError or a user-defined or subclass-defined id.
				const char*             mpDescription;        /// A description of the assertion failure.
				const void*             mpData;               /// The user allocation the assertion is associated with. May be NULL for failures that are without allocation context.
				const void*             mpExtra;              /// Other information related to the failure. The meaning depends on the failure id.
			};

			/// SetAssertionFailureFunction
			/// Allows the user to specify a callback function to trap assertion failures.
			/// The user must not attempt to modify the allocator while in this failure callback.
			void SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext);

			/// AssertionFailure
			/// Triggers an assertion failure. This function is generally intended for internal
			/// use but is available so that debug systems wrapped around this allocator can
			/// use the standard assertion facilities.
			void AssertionFailure(const char* pExpression, int nErrorId = kGAErrorUnknown, const char* pDescription = NULL, 
									const void* pData = NULL, const void* pExtra = NULL) const;

		protected:
			// Internal data types
			enum Constants{
				kMinAlignment               = 8,    /// Eight because that's the size of uint64_t and usually the size of double and long long.
				kMinAlignmentMask           = kMinAlignment - 1,
				kMinAlignmentMaskInverse    = ~kMinAlignmentMask,
				kDefaultBlockSize           = 8192,
				kMaxEquivalentBookmarkCount = 2     /// Max number of times you can call PushBookmark on the same address. It turns out that due to the way this allocator works, if you have multiple core blocks then the system has a limit on the number of times a bookmark can be created for a location. This number is arbitrary and can be any number > 0. Higher numbers give more flexibility but cause new blocks to be allocated sooner.
			};

			struct Block{
				Block* mpPrevBlock;  /// Address of previous block or NULL.
				char*  mpEnd;        /// Points to the end of this block (and thus the end of mData).
				char   mData[4];     /// Allocated data begins here.
			};

			struct Bookmark{
				 Bookmark*  mpPrevBookmark;
				 void*      mpCurrentObjectBegin;                 /// Address of object we are building.
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					 void*  mpCurrentObjectEnd;                   /// Where to add next bytes to current object.
				#endif
			};

			// Internal functions
			Block* GetBlockForAddress(const void* pAddress);
			bool   AllocateNewBlock(size_t nExtraSize); // Allocates a new core block with the assumption that nExtraSize bytes need to be added to the current object.
			void   FreeObjectsEx(const void* pAddress);
			int    CompareAddresses(const void* pAddr1, const void* pAddr2); // Returns -1 if pAddr1 is less than pAddr2; 0 if equal, +1 if greater than. Results are undefined if the addresses are not from this heap.

			// Member data
			size_t                               mnDefaultBlockSize;           /// Preferred size to allocate blocks in.
			Block*                               mpCurrentBlock;               /// Address of current Block.
			char*                                mpCurrentBlockEnd;            /// Address of char after current block.
			char*                                mpCurrentObjectBegin;         /// Address of object we are building.
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
			char*                                mpCurrentObjectEnd;           /// Where to add next bytes to current object.
			#endif
			CoreAllocationFunction               mpCoreAllocationFunction;     /// Callback for allocating a new Block.
			CoreFreeFunction                     mpCoreFreeFunction;           /// Callback for freeing an existing Block.
			void*                                mpCoreFunctionContext;        /// Context passed to CoreAllocationFunction, CoreFreeFunction.
			Bookmark*                            mpTopBookmark;
			bool                                 mbMallocFailureAssertEnabled; /// See kOptionEnableMallocFailureAssert.

			AssertionFailureInfoFunction         mpAssertionFailureFunction;   /// Callback for allocator asserts.
			void*                                mpAssertionFailureFunctionContext;  /// Context passed to the assert function.


			/// AssertionFailureFunctionDefault
			/// Default assert implementation
			static void AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext);

		}; // class StackAllocator



		/// An alternative block allocation function that enables memory tracking.
		/// You should pass in the tracking name as the "pContext" parameter of
		/// the constructor.
		/// 
		/// Example usage:
		///     StackAllocator sa(NULL, 0, TrackedStackAllocationFunction, TrackedStackFreeFunction, "My Custom Name");
		///         or
		///     StackAllocator sa(...);
		///     sa.Init(NULL, 0, TrackedStackAllocationFunction, TrackedStackFreeFunction, "My Custom Name");
		///
		void* TrackedStackAllocationFunction(size_t nSize, size_t* pSizeResult, void* pContext);
		void  TrackedStackFreeFunction(void* pData, void* pContext);

	} // namespace Allocator

} // namespace EA






namespace EA
{
	namespace Allocator
	{

		inline StackAllocator& StackAllocator::operator=(const StackAllocator& stackAllocator)
		{
			if(this != &stackAllocator)
				memcpy(this, &stackAllocator, sizeof(stackAllocator)); // A simple memcpy is all that's needed.
			return *this;
		}


		inline StackAllocator::StackAllocator(const StackAllocator& stackAllocator)
		{
			operator=(stackAllocator);
		}


		inline void* StackAllocator::Calloc(size_t nElementCount, size_t nElementSize, bool bBoundsCheck)
		{
			const size_t nSize = nElementCount * nElementSize;
			void* const pReturnValue = Malloc(nSize, bBoundsCheck);

			if(!bBoundsCheck || pReturnValue) // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
				memset(pReturnValue, 0, nSize);
			return pReturnValue;
		}


		inline void StackAllocator::Free(void* /*pData*/)
		{
			// We do nothing, as you can't free an individual object.
			// If you want to free memory, then you need to call FreeObjects.
		}


		inline void StackAllocator::FreeObjects(const void* pAddress)
		{
			if((pAddress > mpCurrentBlock) && (pAddress < (void*)mpCurrentBlockEnd))
			{
				mpCurrentObjectBegin = (char*)pAddress;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					mpCurrentObjectEnd = mpCurrentObjectBegin;
				#endif
			}
			else
				FreeObjectsEx(pAddress);
		}


		inline void StackAllocator::Reset()
		{
			// Currently we implement this via the brute-force Shutdown function.
			Shutdown();
		}


		inline size_t StackAllocator::GetDefaultBlockSize()
		{
			return mnDefaultBlockSize;
		}


		inline void StackAllocator::SetDefaultBlockSize(size_t nDefaultBlockSize)
		{
			mnDefaultBlockSize = nDefaultBlockSize;
		}


		inline void* StackAllocator::FinishCurrentObject()
		{
			void* const pReturnValue = mpCurrentObjectBegin;

			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				mpCurrentObjectEnd = (char*)(((size_t)mpCurrentObjectEnd + kMinAlignmentMask) & kMinAlignmentMaskInverse);
				if(mpCurrentObjectEnd > (char*)mpCurrentBlockEnd)
					mpCurrentObjectEnd = mpCurrentBlockEnd;
				mpCurrentObjectBegin = mpCurrentObjectEnd;
			#endif

			return pReturnValue;
		}


		inline void* StackAllocator::PushBookmark(const void* pAddress)
		{
			void *pBegin, *pEnd;

			if(pAddress)
				pBegin = pEnd = (void*)pAddress;
			else
			{
				pBegin = mpCurrentObjectBegin;
			  #if PPM_INCREMENTAL_OBJECTS_ENABLED
				pEnd   = mpCurrentObjectEnd;
			  #else
				pEnd   = pBegin;
			  #endif
			}

			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				if(mpCurrentObjectBegin != mpCurrentObjectEnd)
					FinishCurrentObject();
			#endif

			// Bookmarks are allocated with the same memory that's used for the stack itself.
			// So bookmarks are inline within the stack memory. 
			Bookmark* const pBookmark = (Bookmark*)Malloc(sizeof(Bookmark), false);

			if(pBookmark)
			{
				#if (PPM_DEBUG >= 1)
					// If a bookmark couldn't be allocated above within the current block, the Malloc function
					// will have allocated a new block. But that's a potential big problem because that bookmark's 
					// memory (its core block) could be freed before the memory that it points to. We have the 
					// kMaxEquivalentBookmarkCount variable to deal with it, but if the user goes over that 
					// count then we could have this situation. One possible solution is to have the FreeObjects
					// function make sure there aren't any outstanding bookmarks in the top-most core block before
					// freeing its memory, but that introduces a couple of complications itself, and we need this
					// allocator to be as simple and fast as possible, as that's its mandate.

					// Verify that we did not overflow the block with the last bookmark
					PPM_ASSERT(this, (intptr_t)mpCurrentBlockEnd >= (intptr_t)mpCurrentObjectBegin, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
				#endif

				pBookmark->mpPrevBookmark = mpTopBookmark;
				pBookmark->mpCurrentObjectBegin = pBegin;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					pBookmark->mpCurrentObjectEnd = pEnd;
				#endif
				mpTopBookmark = pBookmark;

				return pBegin;
			}

			return NULL;
		}


		inline void StackAllocator::PopBookmark()
		{
			PPM_ASSERT(this, mpTopBookmark != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			if(mpTopBookmark != NULL)
			{
				void* const pCurrentObjectBegin = mpTopBookmark->mpCurrentObjectBegin;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					void* const pCurrentObjectEnd = mpTopBookmark->mpCurrentObjectEnd;
				#endif

				Bookmark* const pCurrentBookmark = mpTopBookmark;
				mpTopBookmark = mpTopBookmark->mpPrevBookmark;
				Free(pCurrentBookmark);

				FreeObjects(pCurrentObjectBegin);
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					mpCurrentObjectEnd = (char*)pCurrentObjectEnd;
				#endif
			 }
		}

		inline void* StackAllocator::GetTopBookmark()
		{
			return mpTopBookmark ? mpTopBookmark->mpCurrentObjectBegin : NULL;
		}


		#if PPM_INCREMENTAL_OBJECTS_ENABLED

		inline void* StackAllocator::GetCurrentObjectBegin()
		{
			return mpCurrentObjectBegin;
		}


		inline void* StackAllocator::GetCurrentObjectEnd()
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				return mpCurrentObjectEnd;
			#else
				return mpCurrentObjectBegin;
			#endif
		}


		inline size_t StackAllocator::GetCurrentObjectSize()
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				return (size_t)(mpCurrentObjectEnd - mpCurrentObjectBegin);
			#else
				return 0;
			#endif
		}


		inline size_t StackAllocator::GetCurrentFreeSpace()
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				return (size_t)(mpCurrentBlockEnd - mpCurrentObjectEnd);
			#else
				return (size_t)(mpCurrentBlockEnd - mpCurrentObjectBegin);
			#endif
		}


		inline bool StackAllocator::ExtendCurrentObject(size_t nSize, bool bBoundsCheck)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				// In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
				if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + nSize) > mpCurrentBlockEnd)))
				{
					if(!AllocateNewBlock(nSize))
						return false;
				}

				mpCurrentObjectEnd += nSize;
			#else
				EA_UNUSED(nSize);
				EA_UNUSED(bBoundsCheck);
			#endif

			return true;
		}


		inline bool StackAllocator::AppendToCurrentObject(const void* pData, size_t nSize, bool bBoundsCheck)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				// In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
				if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + nSize) > mpCurrentBlockEnd)))
				{
					if(!AllocateNewBlock(nSize))
						return false;
				}

				memcpy(mpCurrentObjectEnd, pData, nSize);
				mpCurrentObjectEnd += nSize;
			#else
				EA_UNUSED(nSize);
				EA_UNUSED(bBoundsCheck);
			#endif

			return true;
		}


		template <class T>
		inline bool StackAllocator::AppendToCurrentObjectMemcpy(const T& t, bool bBoundsCheck)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				// In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
				if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + sizeof(T)) > mpCurrentBlockEnd)))
				{
					if(!AllocateNewBlock(sizeof(T)))
						return false;
				}

				memcpy(mpCurrentObjectEnd, &t, sizeof(T));
				mpCurrentObjectEnd += sizeof(T);
			#else
				EA_UNUSED(nSize);
				EA_UNUSED(bBoundsCheck);
			#endif

			return true;
		}


		template <class T>
		inline bool StackAllocator::AppendToCurrentObject(const T& t, bool bBoundsCheck)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				// In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
				if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + sizeof(T)) > mpCurrentBlockEnd)))
				{
					if(!AllocateNewBlock(sizeof(T)))
						return false;
				}

				new(mpCurrentObjectEnd) T(t);      // We assume the destination is properly aligned for T. 
				mpCurrentObjectEnd += sizeof(T);  // For built-in types like 'int', the above 'new' statement becomes just an assignment.
			#else
				EA_UNUSED(nSize);
				EA_UNUSED(bBoundsCheck);
			#endif

			return true;
		}


		#endif // PPM_INCREMENTAL_OBJECTS_ENABLED


	}  // namespace Allocator

} // namespace EA


// Matches the "off" above.
#if defined(_MSC_VER)
	#pragma optimize("", on)
#endif


#endif // Header include guard


