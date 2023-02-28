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
///////////////////////////////////////////////////////////////////////////////
// If you want to create an instance of this allocator on startup and have it
// initialize before other global objects (which may need it), you need to 
// tell the compiler to alter the initialization order. With VC++, it is 
// done at the file level with one of the following:
//     #pragma init_seg(compiler)
//     #pragma init_seg(lib)
// With GCC 3.x (and not 2.x), it is done at the object level with:
//     __attribute__ ((init_priority (n)))
// where n is a value between 101 and 65535 (65535 is default) and lower
// numbers mean earlier initialization. An example would be:
//     GeneralAllocator  gEAGeneralAllocator(arguments) __attribute__ ((init_priority (1000)));
//     GeneralAllocator* gpEAGeneralAllocator = &gEAGeneralAllocator;
// With GCC 2.x (e.g. the PS2 and GameCube compilers), there is no way to 
// do this (init_priority is broken in 2.x) and you have no choice but 
// to manually solve this problem. via link ordering or on-demand initialization.
// Note that most compilers allow forcing of init order by simply changing
// the link order of the files and placing the highest link order objects
// earlier in the file link list.
//
///////////////////////////////////////////////////////////////////////////////
//
// Lint settings
// The following error suppressions largely represent warnings that can't 
// be easily fixed in code but are also known to be insignificant for this module.
//lint -e826                // Suspicious pointer-to-pointer conversion (area too small).
//lint -e1551               // Function may throw exception.
//lint -e1740               // Pointer member not directly freed or zero'ed by destructor.
//lint -e1776               // Converting a string literal to void * is not const safe.
//lint -e641                // Converting enum 'Constants' to int.
//lint -e534                // Ignoring return value of function.
//lint -e539                // Did not expect positive indentation.
//lint -e525                // Negative indentation.
//lint -e506                // Constant value Boolean
//lint -esym(752,dummy__)   // Local declarator '' (same file location) not referenced
//lint -e801                // Use of goto is deprecated.
//lint -e613                // Possible use of null pointer.
//lint -e655                // Bit-wise operation uses (compatible) enum's
//lint -e825                // Control flows into case/default without -fallthrough comment
//lint -e616                // Control flows into case/default
//lint -e1055               // Symbol undeclared, assumed to return int
//lint -esym(530,pSnapshot) // Symbol not initialized
//lint -e661                // Possible access of out-of-bounds pointer by operator
//lint -e744                // Switch statement has no default.
//lint -e740                // Unusual pointer cast (incompatible indirect types).
//lint -e746                // Call to function not made in the presence of a prototype.
//lint -e526                // Symbol 'Symbol' (Location) not defined.
//lint -e628                // No argument information provided.
///////////////////////////////////////////////////////////////////////////////
 

#include <PPMalloc/EAGeneralAllocator.h>
#include <PPMalloc/internal/atomic.inl>
#include <PPMalloc/internal/DefaultCoreAllocator.h>

#include <stddef.h>    // For size_t, NULL, etc.
#include <stdio.h>     // For snprintf, etc.
#include <stdlib.h>    // For malloc, etc.
#include <string.h>    // For memcpy, memset.
#include <new>




///////////////////////////////////////////////////////////////////////////////
// PPM_VERIFY
// Returns 0 if OK and 1 if false. The purpose is in order to assist
// in making functions that both assert while counting errors.
static int PPM_VERIFY(const EA::Allocator::GeneralAllocator* const pPPMalloc, bool bExpression, 
						int id, const char* pDescription, const void* pData, const void* pLocation)
{
	EA_UNUSED(pPPMalloc); EA_UNUSED(id); EA_UNUSED(pDescription); EA_UNUSED(pData); EA_UNUSED(pLocation);

	if(bExpression)
		return 0;
	PPM_ASSERT(pPPMalloc, false, id, pDescription, (void*)pData, (void*)pLocation);
	return 1;
}
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#if (PPM_DEBUG >= 2) // Extended user debug builds.
	#define DebugCheckFreeChunk       CheckFreeChunk
	#define DebugCheckUsedChunk       CheckUsedChunk
	#define DebugCheckRemallocedChunk CheckRemallocedChunk
	#define DebugCheckMallocedChunk   CheckMallocedChunk
	#define DebugCheckMMappedChunk    CheckMMappedChunk
	#define DebugCheckState           CheckState
#else
	#define DebugCheckFreeChunk(a)
	#define DebugCheckUsedChunk(a)
	#define DebugCheckRemallocedChunk(a, b)
	#define DebugCheckMallocedChunk(a, b, c, d)
	#define DebugCheckMMappedChunk(a)
	#define DebugCheckState(a)
#endif
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Internal remappings.
// If some conditions are met, then we disable having separate internal
// versions of functions. This means that when you call the Malloc 
// member function, you directly call MallocInternal for efficiency.
// These defines are currently duplicated between GeneralAllocator.h 
// and GeneralAllocatorDebug.h.
//#if (!PPM_THREAD_SAFETY_SUPPORTED && !PPM_HOOKS_SUPPORTED)
//    #define MallocInternal        Malloc
//    #define FreeInternal          Free
//    #define ReallocInternal       Realloc
//    #define CallocInternal        Calloc
//    #define MallocAlignedInternal MallocAligned
//#endif
///////////////////////////////////////////////////////////////////////////////




// LocalChunkClear
// This was previously implemented as a macro, but it crashes the ARM compiler.
// We prefer it to be the same for all compilers, so we just make it an inline function.
static inline void LocalChunkClear(void* pData, size_t nSize)
{
	if((size_t)nSize > (size_t)(8 * sizeof(size_t)))
		memset(pData, 0, nSize);
	else{
		size_t* pDataDestN  = (size_t*)(pData);

		switch (nSize / sizeof(size_t)){
			case 8:
				*pDataDestN++ = 0; /* fall through */
			case 7:
				*pDataDestN++ = 0; /* fall through */
			case 6:
				*pDataDestN++ = 0; /* fall through */
			case 5:
				*pDataDestN++ = 0; /* fall through */
			case 4:
				*pDataDestN++ = 0; /* fall through */
			case 3:
				*pDataDestN++ = 0; /* fall through */
			case 2:
				*pDataDestN++ = 0; /* fall through */
			case 1:
				*pDataDestN   = 0; /* fall through */
		}
	}
}


// LocalChunkCopy
// This was previously implemented as a macro, but it crashes the ARM compiler.
// We prefer it to be the same for all compilers, so we just make it an inline function.
static inline void LocalChunkCopy(void* pDest, const void* pSrc, size_t nSize)
{
	if((size_t)nSize > (size_t)(8 * sizeof(size_t)))
		memcpy(pDest, (pSrc), (nSize));
	else{
		size_t* pDataDestN  = (size_t*)(pDest);
		size_t* pDataSrcN    = (size_t*)(pSrc);

		switch (nSize / sizeof(size_t)){
			case 8:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 7:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 6:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 5:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 4:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 3:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 2:
				*pDataDestN++ = *pDataSrcN++; /* fall through */
			case 1:
				*pDataDestN++ = *pDataSrcN;   /* fall through */
		}
	}
}



namespace EA{

namespace Allocator{


///////////////////////////////////////////////////////////////////////////////
// GeneralAllocator::mpInitCallbackNode
//
// Win32, XBox, XBox 360 note:
// As it stands now, the InitCallback system is not DLL-savvy. By this we mean
// that there isn't a mechanism for a GeneralAllocator in one DLL to detect
// instances of mpInitCallbackNode in another DLL. This can be resolved by 
// not using DLLs, putting GeneralAllocator in its own DLL, or by simply making
// sure you only register InitCallbacks from the main executable.
//
volatile GeneralAllocator::InitCallbackNode* GeneralAllocator::mpInitCallbackNode = NULL;


///////////////////////////////////////////////////////////////////////////////
// Parameters 
//
GeneralAllocator::Parameters::Parameters()
{
	mpCore               = NULL;
	mpCoreFreeFunction   = NULL;
	mpCoreExtendFunction = NULL;
	mpCoreFreeContext    = NULL;
	mpCoreExtendContext  = NULL;
	mnCoreSize           = 0;
	mnReservedSize       = 0;
	mbShouldFreeCore     = true;
	mbShouldTrimCore     = false;
}

///////////////////////////////////////////////////////////////////////////////
//
GeneralAllocator::Parameters::Parameters(void* pCore, size_t nCoreSize, size_t nReservedSize, bool bShouldFreeCore, bool bShouldTrimCore, 
										 CoreFreeFunction pCoreFreeFunc, void* pCoreFreeContext, CoreExtendFunction pCoreExtendFunc, void* pCoreExtendContext)
{
	mpCore               = pCore;
	mpCoreFreeFunction   = pCoreFreeFunc;
	mpCoreExtendFunction = pCoreExtendFunc;
	mpCoreFreeContext    = pCoreFreeContext;
	mpCoreExtendContext  = pCoreExtendContext;
	mnCoreSize           = nCoreSize;
	mnReservedSize       = nReservedSize;
	mbShouldFreeCore     = bShouldFreeCore;
	mbShouldTrimCore     = bShouldTrimCore;
}

///////////////////////////////////////////////////////////////////////////////
// GeneralAllocator
//
GeneralAllocator::GeneralAllocator(void* pCore, size_t nCoreSize, bool bShouldFreeCore, bool bShouldTrimCore, CoreFreeFunction pCoreFreeFunction, void* pCoreFreeContext,
								   CoreExtendFunction pCoreExtendFunction, void* pCoreExtendContext) 
{    	          
	Construct(Parameters(pCore, nCoreSize, nCoreSize, bShouldFreeCore, bShouldTrimCore, pCoreFreeFunction, pCoreFreeContext, pCoreExtendFunction, pCoreExtendContext));
}


///////////////////////////////////////////////////////////////////////////////
GeneralAllocator::GeneralAllocator(const GeneralAllocator::Parameters& parameters)
{
	Construct(parameters);
}

///////////////////////////////////////////////////////////////////////////////
void GeneralAllocator::Construct(const GeneralAllocator::Parameters& parameters)
{
	// Some tests to verify that our enums are consistent.
	PPM_CT_ASSERT(kMinEffectiveAlignment % PPM_MIN_ALIGN == 0); // If this fails then contact the package maintainer for advice.
	PPM_CT_ASSERT(((sizeof(size_type) == 4) && (kSizeTypeMinus1 == UINT32_C(0xffffffff))) || 
				  ((sizeof(size_type) == 8) && (kSizeTypeMinus1 == UINT64_C(0xffffffffffffffff))));
	PPM_CT_ASSERT(kChunkFlagFastBin > kChunkFlagLargeBin); // If this fails then the kMaxChunkSize definition needs to be revised.

	// General state
	mEventId = 0;
	mbInitialized = false;
	#if PPM_FASTBINS_ENABLED
		mnMaxFastBinChunkSize = 0;
		memset(mpFastBinArray, 0, sizeof(mpFastBinArray));
	#endif
	memset(mpBinArray, 0, sizeof(mpBinArray));
	memset(mBinBitmap, 0, sizeof(mBinBitmap));
	mpTopChunk = NULL;
	mpLastRemainderChunk = NULL;
	memset(&mHeadCoreBlock, 0, sizeof(mHeadCoreBlock));
	mpHighFence = NULL;
	mbHighFenceInternallyDisabled = false;
	mbSystemAllocEnabled = true;
	mbUnsortedBinEnabled = true;
	mbLockDuringHookCalls = false;
	mbExecutableCore = false;
	mbMallocFailureAssertEnabled = false;
	mbTrimToZero = false;
	mnCheckChunkReentrancyCount = 0;
	mcTraceFieldDelimiter  = (unsigned char)'\t';
	mcTraceRecordDelimiter = (unsigned char)'\n';

	// Auto heap validation
	mAutoHeapValidationLevel       = kHeapValidationLevelNone;
	mnAutoHeapValidationFrequency  = 0;
	mnAutoHeapValidationEventCount = 0;
	mbHeapValidationActive         = false;

	#if (EA_PLATFORM_PTR_SIZE >= 8)
	mbMMapTopDown     = true;   // High addresses on 64 bit platforms are better 
	mbCoreTopDown     = true;   // for debugging and catching problems.
	#else
	mbMMapTopDown     = false;
	mbCoreTopDown     = false;
	#endif

	// Memory map support
	mnMMapCount       = 0;
	mnMMapMallocTotal = 0;
	mnMMapMaxAllowed  = 0;
	mnMMapThreshold   = 0;
	memset(&mHeadMMapChunk, 0, sizeof(mHeadMMapChunk));

	mPhysicalFootprint = 0;
	mNumCores = 0;

	mCoreAddFunction         = NULL;
	mpCoreAddFunctionContext = NULL;

	// Callback
	#if PPM_HOOKS_SUPPORTED
		mpHookFunction        = NULL; 
		mpHookFunctionContext = NULL;
	#endif

	mpMallocFailureFunction        = NULL;
	mpMallocFailureFunctionContext = NULL;
	mnMaxMallocFailureCount        = 256;

	// Misc
	mnTrimThreshold        = 0;
	mnTopPad               = 0;
	mpInitialTopChunk      = NULL;
	mnPageSize             = kDefaultPageSize; // Default value.
	mnMMapPageSize         = kDefaultMMapPageSize;
	mnNewCoreSize          = PPM_NEW_CORE_SIZE_DEFAULT;
	mnCoreIncrementSize    = PPM_CORE_INCREMENT_SIZE_DEFAULT;
	mbTraceInternalMemory  = false;

	// Thread safety
	#if PPM_THREAD_SAFETY_SUPPORTED
		mpMutex = NULL;
		memset(mpMutexData, 0, sizeof(mpMutexData));
	#endif

	// Debug fill values
	mnFillFree        = kDefaultFillValueFree;
	mnFillDelayedFree = kDefaultFillValueDelayedFree;
	mnFillNew         = kDefaultFillValueNew;
	mnFillGuard       = kDefaultFillValueGuard;
	mnFillUnusedCore  = kDefaultFillValueUnusedCore;

	// Name
	mpName = NULL;

	// NotifyInitState
	mNotifyInitState = kNotifyInitStateInit;

	// We set these down here because we want this code to always execute.
	mpAssertionFailureFunction        = &AssertionFailureFunctionDefault;
	mpAssertionFailureFunctionContext = this;

	mpTraceFunction        = &TraceFunctionDefault;
	mpTraceFunctionContext = this;

	mRangeCallback = 0;
	mRangeUserData = 0;

	GeneralAllocator::Init(parameters); 
}

///////////////////////////////////////////////////////////////////////////////
void GeneralAllocator::SetMemoryRangeCallbackFunction(MemoryRangeCallbackFunction fn, void* userData)
{
	mRangeCallback = fn;
	mRangeUserData = userData;
}


///////////////////////////////////////////////////////////////////////////////
// ~GeneralAllocator
//
GeneralAllocator::~GeneralAllocator()
{
	GeneralAllocator::Shutdown(); // virtual function mechanism doesn't work in constructors or destructors.
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
// Returns false if core was requested but failed to allocate. Else returns true.
//
bool GeneralAllocator::Init(void* pInitialCore, size_t nInitialCoreSize, bool bShouldFreeInitialCore, bool bShouldTrimInitialCore, 
							CoreFreeFunction pInitialCoreFreeFunction, void* pInitialCoreFreeFunctionContext,
							CoreExtendFunction pInitialCoreExtendFunction, void* pInitialCoreExtendFunctionContext)
{
	return GeneralAllocator::Init(Parameters(pInitialCore, nInitialCoreSize, nInitialCoreSize, bShouldFreeInitialCore, bShouldTrimInitialCore,
						   pInitialCoreFreeFunction, pInitialCoreFreeFunctionContext, pInitialCoreExtendFunction, pInitialCoreExtendFunctionContext));
}

bool GeneralAllocator::Init(const GeneralAllocator::Parameters& parameters)
{
	bool bResult = true;

	if(!mbInitialized) // If we haven't been initialized...
	{
		mbInitialized = true;

		// Thread safety
		#if PPM_THREAD_SAFETY_SUPPORTED && PPM_THREAD_SAFETY_BY_DEFAULT
			SetOption(kOptionEnableThreadSafety, 1);
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		// State
		#if PPM_FASTBINS_ENABLED
			mnMaxFastBinChunkSize = kFastBinDefaultMaxDataSize;
			memset(mpFastBinArray, 0, sizeof(mpFastBinArray));
		#endif
		memset(mpBinArray, 0, sizeof(mpBinArray));
		for(int i = 1; i < kBinCount; ++i)
		{  // Establish circular links for normal bins
			SkipListChunk* const pBin = GetBin(i);

			pBin->mpNextChunk = pBin->mpPrevChunk = pBin;

			if(i >= kLargeBinIndex) // Leave the other bins as NULL. In debug builds we verify they stay NULL.
				pBin->mpLargerChunks = pBin->mpSmallerChunks = pBin;
		}
		memset(mBinBitmap, 0, sizeof(mBinBitmap));
		mpTopChunk = GetInitialTopChunk();
		mpLastRemainderChunk = NULL;
		memset(&mHeadCoreBlock, 0, sizeof(mHeadCoreBlock));
		mHeadCoreBlock.mpNextCoreBlock = mHeadCoreBlock.mpPrevCoreBlock = &mHeadCoreBlock;
		mpHighFence = NULL;
		mbHighFenceInternallyDisabled = false;
		SetMaxFastBinDataSize(kFastBinDefaultMaxDataSize);

		#if (EA_PLATFORM_PTR_SIZE >= 8)
		mbMMapTopDown     = true;
		mbCoreTopDown     = true;
		#else
		mbMMapTopDown     = false;
		mbCoreTopDown     = false;
		#endif

		// Memory map support
		mnMMapCount       = 0;
		mnMMapMallocTotal = 0;
		mnMMapMaxAllowed  = kDefaultMMapMaxCount;
		mnMMapThreshold   = kDefaultMMapThreshold;
		memset(&mHeadMMapChunk, 0, sizeof(mHeadMMapChunk));
		mHeadMMapChunk.mpNextChunk = mHeadMMapChunk.mpPrevChunk = &mHeadMMapChunk;

		// Callback
		// We don't set these because we want to allow the user to preserve 
		// these across a Shutdown, Init sequence. 
		// mpHookFunction        = NULL; 
		// mpHookFunctionContext = NULL;
		// mpMallocFailureFunction        = NULL;
		// mpMallocFailureFunctionContext = NULL;
		// mpTraceFunction        = &TraceFunctionDefault;
		// mpTraceFunctionContext = NULL;

		// Misc
		mnTrimThreshold   = kDefaultTrimThreshold;
		mnTopPad          = kDefaultTopPad;
		mpInitialTopChunk = (char*)mpTopChunk;
		mnPageSize        = GetPageSize();
		// mnMMapPageSize -- No need to set this, as it is a constant.
	}

	if(parameters.mpCore || parameters.mnCoreSize)
	{
		bResult = AddCoreEx(parameters.mpCore, parameters.mnCoreSize, kCoreTypeDefault, parameters.mnReservedSize,
							parameters.mbShouldFreeCore, parameters.mbShouldTrimCore, 
							parameters.mpCoreFreeFunction, parameters.mpCoreFreeContext, 
							parameters.mpCoreExtendFunction, parameters.mpCoreExtendContext);
	}

	// Possibly notify users of our initialization.
	if(mNotifyInitState == kNotifyInitStateInit)
	{
		mNotifyInitState = kNotifyInitStateShutdown;
		for(volatile InitCallbackNode* pCurrent = mpInitCallbackNode; pCurrent; pCurrent = pCurrent->mpNext)
			pCurrent->mpInitCallbackFunction(this, true, pCurrent->mpContext);
	}

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool GeneralAllocator::Shutdown()
{
	// Possibly notify users of our initialization.
	if(mNotifyInitState == kNotifyInitStateShutdown)
	{
		mNotifyInitState = kNotifyInitStateInit;
		for(volatile InitCallbackNode* pCurrent = mpInitCallbackNode; pCurrent; pCurrent = pCurrent->mpNext)
			pCurrent->mpInitCallbackFunction(this, false, pCurrent->mpContext);
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMMutexLock(mpMutex);
	#endif

	if(mbInitialized)
	{
		mbInitialized = false;

		// Verify initial good state.
		DebugCheckState(kHeapValidationLevelBasic);

		if(GetFastBinChunksExist())
			ClearFastBins();

		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			if(mpTraceFunction)
			{
				char pTraceHeading[128];
				PPM_Snprintf(pTraceHeading, 128, "GeneralAllocator (%s) shutdown. Outstanding allocations:\n", mpName ? mpName : "unnamed");
				mpTraceFunction(pTraceHeading, mpTraceFunctionContext);
			}

			SnapshotImage snapshotImage;
			TraceAllocatedMemory(NULL, NULL, &snapshotImage, sizeof(snapshotImage)); // Trace all allocated (i.e. leaked) memory.
		#endif

		// Free any MMap memory
		#if PPM_MMAP_SUPPORTED
			while(mHeadMMapChunk.mpNextChunk != &mHeadMMapChunk)
			{
				Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(mHeadMMapChunk.mpNextChunk);
				FreeInternal(GetDataPtrFromChunkPtr(pMMappedChunk));
			}
		#endif

		// Free core memory
		while(mHeadCoreBlock.mpNextCoreBlock != &mHeadCoreBlock)
		{
			CoreBlock* const pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;

			#if PPM_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
					const HookInfo hookInfo = { this, true, kHookTypeCoreFree, kHookSubTypeNone, pCoreBlock->mnSize, pCoreBlock->mpCore, 0, pCoreBlock->mnSize, NULL, 0, 0, NULL, NULL, pCoreBlock->mnSize, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif

			UnlinkCoreBlock(pCoreBlock);
			mPhysicalFootprint -= pCoreBlock->mnSize;
			mNumCores--;

			FreeCore(pCoreBlock, true); // 'true' because we are in Shutdown. Should otherwise be false.
		}

		mpTopChunk = GetInitialTopChunk();

		mpHighFence = NULL;
		mbHighFenceInternallyDisabled = false;

		// Verify final good state.
		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			DebugCheckState(kHeapValidationLevelBasic);
		#endif
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
		{
			void* const pMutex = mpMutex;
			mpMutex = NULL;
			PPMMutexUnlock(pMutex);
			PPMMutexDestroy(pMutex);
		}
	#endif

	return true;
}




///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* GeneralAllocator::AsInterface(int id)
{
	if(id == GeneralAllocator::kTypeId)
		return static_cast<GeneralAllocator*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Lock
//
#if PPM_THREAD_SAFETY_SUPPORTED
	void GeneralAllocator::Lock(bool bEnable)
	{
		if(bEnable)                 // This 'if' will be optimized away when bEnable is 
			PPMMutexLock(mpMutex);  // passed as 'true' or 'false', which it almost always is.
		else
			PPMMutexUnlock(mpMutex);
	}
#else
	// In this case GeneralAllocator::Lock is inlined to a no-op in the header file for this .cpp file.
#endif


///////////////////////////////////////////////////////////////////////////////
// GetOption
//
int64_t GeneralAllocator::GetOption(int option) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	switch (option)
	{
	#if PPM_THREAD_SAFETY_SUPPORTED
		case kOptionEnableThreadSafety:        return mpMutex ? 1 : 0;
	#endif
		case kOptionEnableSystemAlloc:         return mbSystemAllocEnabled ? 1 : 0;
		case kOptionNewCoreSize:               return (int64_t)mnNewCoreSize;
		case kOptionCoreIncrementSize:         return (int64_t)mnCoreIncrementSize;
		case kOptionMaxFastBinRequestSize:     return (int64_t)GetMaxFastBinChunkSize();
		case kOptionTrimThreshold:             return (int64_t)mnTrimThreshold;
		case kOptionTopPad:                    return (int64_t)mnTopPad;
		case kOptionMMapThreshold:             return (int64_t)mnMMapThreshold;
		case kOptionMMapMaxAllowed:            return (int64_t)mnMMapMaxAllowed;
		case kOptionMMapTopDown:               return mbMMapTopDown ? 1 : 0;
		case kOptionMMapPageSize:              return (int64_t)mnMMapPageSize;
		case kOptionTraceInternalMemory:       return mbTraceInternalMemory ? 1 : 0;
		case kOptionMaxMallocFailureCount:     return (int64_t)mnMaxMallocFailureCount;
		case kOptionEnableUnsortedBin:         return mbUnsortedBinEnabled ? 1 : 0;
		case kOptionCoreTopDown:               return mbCoreTopDown ? 1 : 0;
		case kOptionLockDuringHookCalls:       return mbLockDuringHookCalls ? 1 : 0;
		case kOptionExecutableCore:            return mbExecutableCore ? 1 : 0;
		case kOptionEnableMallocFailureAssert: return mbMallocFailureAssertEnabled ? 1 : 0;
		case kOptionEnableTrimToZero:          return mbTrimToZero ? 1 : 0;
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void GeneralAllocator::SetOption(int option, int64_t nValue)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		if(option != kOptionEnableThreadSafety)
			PPMMutexLock(mpMutex);
	#endif

	switch (option){
		case kOptionEnableThreadSafety:
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(nValue)
				{
					if(!mpMutex)
					{
						mpMutex = PPMMutexCreate(mpMutexData);
						PPM_ASSERT(this, mpMutex != NULL, kGAErrorOS, kGAErrorText[kGAErrorOS], NULL, NULL); // If this fails, the system kernel is exhausted or corrupted.
					}
				}
				else
				{
					if(mpMutex)
					{
						PPMMutexLock(mpMutex);
						void* const pMutex = mpMutex;
						mpMutex = NULL;
						PPMMutexUnlock(pMutex);
						PPMMutexDestroy(pMutex);
					}
				}
			#endif
			break;

		case kOptionEnableSystemAlloc:
			mbSystemAllocEnabled = (nValue != 0);
			break;

		case kOptionNewCoreSize:
			if(nValue == 0)
				mnNewCoreSize = PPM_NEW_CORE_SIZE_DEFAULT;
			else
			{
				mnNewCoreSize = ((size_t)nValue > mnPageSize ? (size_t)nValue : mnPageSize);
				mnNewCoreSize = PPM_AlignUp(mnNewCoreSize, mnPageSize);

				PPM_ASSERT(this, mnNewCoreSize <= kMaxCoreSize, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, NULL); // If this fails, the user specified a core memory block that was too big.
				if(mnNewCoreSize > kMaxCoreSize)
					mnNewCoreSize = kMaxCoreSize;
			}
			break;

		case kOptionCoreIncrementSize:
			if(nValue == 0)
				mnCoreIncrementSize = PPM_CORE_INCREMENT_SIZE_DEFAULT;
			else
			{
				mnCoreIncrementSize = ((size_t)nValue > mnPageSize ? (size_t)nValue : mnPageSize);
				mnCoreIncrementSize = PPM_AlignUp(mnCoreIncrementSize, mnPageSize);
				if(mnCoreIncrementSize > mnNewCoreSize)
					mnCoreIncrementSize = mnNewCoreSize;
			}
			break;

		case kOptionMaxFastBinRequestSize:
		{
			ClearFastBins(); // Ensure that we are initialized and 'consolidated', which means fast bins are cleared, etc.
			SetMaxFastBinDataSize((size_type)nValue);
			break;
		}

		case kOptionTrimThreshold:
			mnTrimThreshold = (size_type)nValue;
			break;

		case kOptionTopPad:
			mnTopPad = (size_type)nValue;
			break;

		case kOptionMMapThreshold:
			mnMMapThreshold = (size_type)nValue;
			break;

		case kOptionMMapMaxAllowed:
			mnMMapMaxAllowed = (int)nValue;
			break;

		case kOptionMMapTopDown:
			mbMMapTopDown = (nValue != 0);
			break;

		case kOptionMMapPageSize:
			PPM_ASSERT(this, PPM_IsPowerOf2((size_t)nValue) && (nValue >= 4096), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL); // If this fails, it is the user's fault.

			mnMMapPageSize = (size_t)nValue;
		  //mnPageSize     = (size_t)nValue;  // We don't set this because code here merely depends on mnPageSize being the minimum possible system page size.
			break;

		case kOptionTraceInternalMemory:
			mbTraceInternalMemory = (nValue != 0);
			break;

		case kOptionMaxMallocFailureCount:
			mnMaxMallocFailureCount = (unsigned)nValue;
			break;

		case kOptionEnableUnsortedBin:
			mbUnsortedBinEnabled = (nValue != 0);
			break;

		case kOptionCoreTopDown:
			mbCoreTopDown = (nValue != 0);
			break;

		case kOptionLockDuringHookCalls:
			mbLockDuringHookCalls = (nValue != 0);
			break;

		case kOptionExecutableCore:
			mbExecutableCore = (nValue != 0);
			break;

		case kOptionEnableMallocFailureAssert:
			mbMallocFailureAssertEnabled = (nValue != 0);
			break;

		case kOptionEnableTrimToZero:
			mbTrimToZero = (nValue != 0);
			break;
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(option != kOptionEnableThreadSafety)
			PPMMutexUnlock(mpMutex);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// SetInitCallback
//
void GeneralAllocator::SetInitCallback(InitCallbackNode* pInitCallbackNode, bool bSet)
{
	// Note: As of this writing, this function isn't thread safe. It is a global
	//       function and we have no mechanism for mutexing global state. This is 
	//       by design as we are trying to keep the system as simple as possible.

	if(bSet)
	{
		if(mpInitCallbackNode)
		{
			volatile InitCallbackNode* pCurrent = mpInitCallbackNode; // Having a volatile local variable seems silly, but the compiler errors without it.

			while(pCurrent->mpNext)
				pCurrent = pCurrent->mpNext;

			pCurrent->mpNext = pInitCallbackNode;
		}
		else
			mpInitCallbackNode = pInitCallbackNode;

		pInitCallbackNode->mpNext = NULL;
	}
	else
	{
		if(mpInitCallbackNode)
		{
			volatile InitCallbackNode* pCurrent  = mpInitCallbackNode; // Having a volatile local variable seems silly, but the compiler errors without it.
			volatile InitCallbackNode* pPrevious = NULL;

			while(pCurrent)
			{
				if(pCurrent == pInitCallbackNode)
				{
					if(pPrevious)
						pPrevious->mpNext = pCurrent->mpNext;
					else
						mpInitCallbackNode = pCurrent->mpNext;
					break;
				}

				pPrevious = pCurrent;
				pCurrent  = pCurrent->mpNext;
			}
		}
	}
}


void GeneralAllocator::SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void* pContext)
{
	mpMallocFailureFunction        = pMallocFailureFunction;
	mpMallocFailureFunctionContext = pContext;
}


void GeneralAllocator::SetHookFunction(HookFunction pHookFunction, void* pContext)
{
	EA_UNUSED(pHookFunction); // Prevent compiler warnings.
	EA_UNUSED(pContext);

	#if PPM_HOOKS_SUPPORTED
		mpHookFunction        = pHookFunction;
		mpHookFunctionContext = pContext;
	#endif
}


GeneralAllocator::TraceFunction GeneralAllocator::GetTraceFunction(void** pContext)
{
	if(pContext)
		*pContext = mpTraceFunctionContext;
	return mpTraceFunction;
}


void GeneralAllocator::SetTraceFunction(TraceFunction pTraceFunction, void* pContext)
{
	mpTraceFunction        = pTraceFunction;
	mpTraceFunctionContext = pContext;
}


void GeneralAllocator::SetTraceFieldDelimiters(unsigned char fieldDelimiter, unsigned char recordDelimiter)
{
	mcTraceFieldDelimiter  = fieldDelimiter;
	mcTraceRecordDelimiter = recordDelimiter;
}


void GeneralAllocator::UnlinkChunkFromBin(Chunk* pChunk)
{
	// Same as UnlinkChunk but also removes Chunk from large bin skip list if it's in a large bin.
	#if (PPM_DEBUG >= 2)
		CheckChunkRegularBin(pChunk);
	#endif

	// To consider: Don't use a flag but rather use the combination of (pChunk not in small bin and pChunk->mpSmallerChunks != NULL). Doing so requires mpSmallerChunks to be always NULL when it's in the unsorted bin and always non-NULL when in the large bins.
	if(GetChunkIsLargeBin(pChunk)) // If it's in a large bin then we need to maintain its skip list pointers.
	{
		// We have a number of cases we need to handle:
		//    pChunk is in the middle of a run (and thus its skip list pointers are unused and so we do nothing here about them).
		//    pChunk is the first in a run (and thus its skip list pointers are valid).
		//    pChunk is the the only chunk in a run in addition to being first.
		//    pChunk is the only chunk in the bin.
		SkipListChunk* pSkipListChunk   = (SkipListChunk*)pChunk;
		size_type      nChunkSize       = GetChunkSize(pChunk);
		int            nIndex           = GetLargeBinIndexFromChunkSize(nChunkSize);
		SkipListChunk* pBin             = GetBin(nIndex); 
		bool           bChunkFirstInRun = (pSkipListChunk->mpPrevChunk == pBin) || (GetChunkSize(pSkipListChunk->mpPrevChunk) != nChunkSize);  // We can't safely read the chunk size of the bin header chunk (it's unmaintained and undefined), so we have to do a special case for the prev chunk being the bin header.
		bool           bChunkSoloInRun  = (bChunkFirstInRun && (pSkipListChunk->mpNextChunk == pSkipListChunk->mpSmallerChunks));

		#if (PPM_DEBUG >= 2) // Verify that in fact the chunk is in a large bin.
			PPM_ASSERT(this, GetChunkIsInBin(pSkipListChunk, pBin), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
		#endif

		if(bChunkFirstInRun)    // If pChunk is the first in a list of one or more chunks of the same size.
		{
			if(bChunkSoloInRun) // If pChunk is the only chunk of its size...
			{
				pSkipListChunk->mpLargerChunks->mpSmallerChunks = pSkipListChunk->mpSmallerChunks;
				pSkipListChunk->mpSmallerChunks->mpLargerChunks = pSkipListChunk->mpLargerChunks;
			}
			else // Else the chunk is the first in a list or more than one of the same size.
			{
				SkipListChunk* pNextChunkS = (SkipListChunk*)pSkipListChunk->mpNextChunk;  // pNextChunkS will be the new first chunk in this run.

				pSkipListChunk->mpLargerChunks->mpSmallerChunks = pNextChunkS;
				pSkipListChunk->mpSmallerChunks->mpLargerChunks = pNextChunkS;
				pNextChunkS->mpLargerChunks  = pSkipListChunk->mpLargerChunks;
				pNextChunkS->mpSmallerChunks = pSkipListChunk->mpSmallerChunks;
			}
		}
		else
		{
			// Nothing to do since this chunk isn't the first of a run and thus doesn't participate in skip list pointer maintenance.
			#if (PPM_DEBUG >= 2) // In debug builds we clear the previous run head chunk skip list links, and verify here that we did so and things didn't get messed up.
				PPM_ASSERT(this, !pSkipListChunk->mpLargerChunks && !pSkipListChunk->mpSmallerChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			#endif
		}

		#if PPM_DEBUG_FILL
			// Now that we're taking pChunk out of a large bin, we need to set the mpLargerChunks/mpSmallerChunks memory to mnFillFree, so future debug fill checks will pass.
			DebugMemFill(GetPostHeaderPtrFromChunkPtr(pSkipListChunk), mnFillFree, sizeof(SkipListChunk) - sizeof(Chunk));
		#endif

		ClearChunkIsLargeBin(pChunk);
	}

	UnlinkChunk(pChunk); // Lower level (inlinable) unlink function.

	#if (PPM_DEBUG >= 2)
		CheckChunkRegularBin(pChunk);
	#endif
}


GeneralAllocator::Chunk* GeneralAllocator::FindPriorChunk(const Chunk* pChunk) const
{
	PPM_ASSERT(this, !GetChunkIsMMapped(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

	if(pChunk->mnPriorSize > 0) // If this is not the first chunk of a core block (we assume we have data integrity)... 
	{
		#if PPM_DEBUG_PRESERVE_PRIOR
			// When PPM_DEBUG_PRESERVE_PRIOR is defined, we reserve enough space so that 
			// the 'prior size' field of chunks is always valid and not overrun by user data. 
			return (Chunk*)((const char*)pChunk - pChunk->mnPriorSize);
		#else
			const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
			PPM_ASSERT(this, pCoreBlock, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk); // If this fails, the user probably corrupted the heap or removed a core block that was in use.

			if(pCoreBlock)
			{
				if((const void*)pChunk > pCoreBlock->mpCore) // If this isn't the first chunk in the core...
				{
					const Chunk* pCurrentChunk = (Chunk*)pCoreBlock->mpCore;
					const Chunk* pNextChunk    = GetNextChunk(pCurrentChunk);

					// This loop could be traversing many megabytes of data and might be somewhat slow.
					// This function will likely crash if there are any heap problems.
					while(pNextChunk < pChunk)
					{
						pCurrentChunk = pNextChunk;
						pNextChunk    = GetNextChunk(pNextChunk);
					}
					return const_cast<Chunk*>(pCurrentChunk);
				}
			}
		#endif
	}
	return NULL;
}



int GeneralAllocator::GetLargeBinIndexFromChunkSize(size_type nSize)
{
	// This is currently dependent on the values of kLargeBinMinChunkSize, kSmallBinCount, and kBinCount having specific values.
	// If you change these values then you will likely need to change the code below to match.
	// To consider: Come up with something a little more flexible than the expression below.
	PPM_CT_ASSERT((kSmallBinIndexEnd == 64) && (kLargeBinMinChunkSize == 512) && (kBinCount == 128)); // If this fails, PPMalloc is at fault.

	return (int)(((nSize >>  6) <= 32) ?  56 + (nSize >>  6) : // The lowest value this expression will generate is 64: (56 + (512 >> 6) == 64). 
				 ((nSize >>  9) <= 20) ?  91 + (nSize >>  9) :
				 ((nSize >> 12) <= 10) ? 110 + (nSize >> 12) :
				 ((nSize >> 15) <=  4) ? 119 + (nSize >> 15) :
				 ((nSize >> 18) <=  2) ? 124 + (nSize >> 18) : 126);
}



GeneralAllocator::Chunk* GeneralAllocator::FindAndSetNewTopChunk()
{
	// This function would only be called if memory was being cleaned up and what used to be 
	// the top chunk was cleaned away. Thus any new top chunk is going to have to be culled
	// from existing free memory in core blocks. It may or may not be the case that the last 
	// chunk in any of the core blocks is free, so we also accept top chunks as any chunk 
	// beyond a given size.

	Chunk*     pBestFit = GetInitialTopChunk();
	size_type nBestFitChunkSize = 0;

	// Walk all core blocks.
	for(CoreBlock* pCoreBlock = mHeadCoreBlock.mpPrevCoreBlock; pCoreBlock != &mHeadCoreBlock; pCoreBlock = pCoreBlock->mpPrevCoreBlock)
	{
		//Walk all chunks in this block.
		Chunk* pChunk        = (Chunk*)pCoreBlock->mpCore;
		Chunk* pFenceChunk = GetFenceChunk(pCoreBlock);

		while(pChunk < pFenceChunk)
		{
			Chunk* const pNextChunk = GetNextChunk(pChunk);

			if(!GetChunkIsInUse(pChunk)) // If the chunk is free...
			{
				const size_type nChunkSize = GetChunkSize(pChunk);

				if((nChunkSize > 8192) || (pNextChunk == pFenceChunk)) // Anything beyond a certain size is good enough. 
				{
					pBestFit = pChunk;
					goto End; // Break out of our double-loop.
				}

				// Should be instead just return the first free block we find?
				if(nChunkSize > nBestFitChunkSize)
				{
					pBestFit             = pChunk;
					nBestFitChunkSize = nChunkSize;
				}
			}

			pChunk = pNextChunk;
		}

		// As a speed improvement, consider using the pBestFit here if one has been set.
	}

	End:
	if(pBestFit != GetInitialTopChunk())
	{
		UnlinkChunkFromBin(pBestFit);  // Remove it from whatever list it is in.
		pBestFit->mpPrevChunk = pBestFit->mpNextChunk = pBestFit;
	}
	mpTopChunk = pBestFit;

	#if (PPM_DEBUG >= 3) // Developer builds only.
		DebugCheckState(kHeapValidationLevelBasic);
	#endif

	return mpTopChunk;
}


bool GeneralAllocator::GetChunkIsFenceChunk(const Chunk* pChunk) const
{
	const size_type nChunkSize = GetChunkSize(pChunk);
 
	if(nChunkSize < kMinChunkSize) // Quick first level check. Fence chunks are fake undersized chunks.
	{
		PPM_ASSERT(this, nChunkSize == kFenceChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

		// There one or two fence chunks at the top of a core block. One if the core 
		// block is the topmost of all and two if it is a lower core block in memory.
		const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
		if(pCoreBlock && (pChunk >= GetFenceChunk(pCoreBlock))) // We only need to check if > N, and not less than end.
			return true;

		// Only fence chunks may be of a size smaller than min chunk size.
		PPM_ASSERT(this, false, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
	}

	#if PPM_ASSERT_ENABLED
		const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
		PPM_ASSERT(this, (pCoreBlock == NULL) || (pChunk <= GetFenceChunk(pCoreBlock)), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk); // If this fails, the user probably corrupted the heap or removed a core block that was in use.
	#endif

	return false;
}


bool GeneralAllocator::GetChunkIsInBin(const Chunk* pChunk, const Chunk* pBin) const
{
	// This is an internal debugging/validation function.
	// As such we intentionally don't use skip list functionality in large bins to speed searches.
	const Chunk* pChunkCurrent = pBin->mpNextChunk;

	while(pChunkCurrent != pBin)
	{
		if(pChunkCurrent == pChunk)
			return true;
		pChunk = pChunk->mpNextChunk;
	}

	return false;
}


// If bAssumeValid is true then we don't go looking in places where the Chunk isn't supposed to be (e.g. a wrong bin).
// The primary reason to use bAssumeValid=true is for doing strong heap validation and to help debug GeneralAllocator.
GeneralAllocator::BinType GeneralAllocator::FindChunkBin(const Chunk* pChunk, bool bAssumeValid) const
{
	// This is an internal debugging/validation function.

	// Chunks that are in use won't be in any bin.
	// Disable this assert because we might want to intenally call this 
	// with an in-use chunk. We provide a similar assert later in this function.
	// PPM_ASSERT(this, !GetChunkIsInUse(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, NULL); 

	// Check top chunk
	if(pChunk == mpTopChunk)
	{
		PPM_ASSERT(this, (pChunk->mpNextChunk == pChunk) && (pChunk->mpPrevChunk == pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
		return kBinTypeTop;
	}

	const size_type nChunkSize = GetChunkSize(pChunk);
	int nIndex;

	// Check fast bins, first pass quick check.
	#if PPM_FASTBINS_ENABLED
		if((nChunkSize >= kMinChunkSize) && (nChunkSize <= mnMaxFastBinChunkSize)){ // Chunks smaller than min chunk size will cause GetFastBinIndexFromChunkSize() to give bad results.
			nIndex = (int)GetFastBinIndexFromChunkSize(nChunkSize);
			PPM_ASSERT(this, nIndex < (int)EAArrayCount(mpFastBinArray), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

			const Chunk* pChunkCurrent = mpFastBinArray[nIndex];
			while(pChunkCurrent){ // Recall that fast bins are singly-, not doubly-linked.
				if(pChunkCurrent == pChunk)
					return kBinTypeFast;
				pChunkCurrent = pChunkCurrent->mpNextChunk;
			}
		}
	#endif

	// Check regular bins, first pass quick check.
	// Check to see if the chunk is one of our bin list head chunks. As of this writing (PPMalloc v1.17.01, 6/2011), this function is never called with pChunk being a head chunk.
	{  // Give this block its own scope, so variables are scoped.

		nIndex = GetBinIndexFromChunkSize(nChunkSize);
		PPM_ASSERT(this, nIndex < (int)EAArrayCount(mpBinArray), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

		// Walk the list of chunks, returning kBinTypeNormal if found. To do: Speed this up using the skip list. Make sure the skip list is validated too.
		const Chunk* pBin = GetBin(nIndex);

		if(pChunk != pBin) // if pChunk isn't already the head chunk...
		{
			const Chunk* pChunkCurrent = pBin->mpNextChunk; 

			while((pChunkCurrent != pBin) && (pChunkCurrent != pChunk))
				pChunkCurrent = pChunkCurrent->mpNextChunk;

			if(pChunkCurrent == pBin) // If we didn't find it in the loop above...
				nIndex = 0;
		}

		if(nIndex)
		{
			if((nIndex >= kUnsortedBinIndex) && (nIndex < kUnsortedBinIndexEnd))
				return kBinTypeUnsorted;
			if((nIndex >= kSmallBinIndex) && (nIndex < kSmallBinIndexEnd))
				return kBinTypeSmall;
			if((nIndex >= kLargeBinIndex) && (nIndex < kLargeBinIndexEnd)) // This check shouldn't be necessary if all is well.
				return kBinTypeLarge;
		}
	}


	if(!bAssumeValid)
	{
		// At this point, the chunk is either in no bin or is in the wrong bin. 
		// The only way it should be in no bin is if it is unused. There is no
		// way it should ever be in the wrong bin.
		PPM_ASSERT(this, GetChunkIsInUse(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

		// Check each fast bin, one by one, for the chunk.
		#if PPM_FASTBINS_ENABLED
			for(nIndex = 0; nIndex < (int)EAArrayCount(mpFastBinArray); nIndex++){
				const Chunk* pChunkCurrent = mpFastBinArray[nIndex];
				while(pChunkCurrent){ // Recall that fast bins are singly-, not doubly-linked.
					if(pChunkCurrent == pChunk)
						return kBinTypeFast;
					pChunkCurrent = pChunkCurrent->mpNextChunk;
				}
			}
		#endif

		// Check each regular bin, one by one, for the chunk.
		for(nIndex = 2; nIndex < (int)EAArrayCount(mpBinArray); nIndex += 2){ // Start at 2 because first two are unused and NULL. '+=2' because entries are doubled.
			const Chunk* pChunkCurrent = mpBinArray[nIndex]->mpNextChunk;
			while(pChunkCurrent != mpBinArray[nIndex]){
				if(pChunkCurrent == pChunk){
					if((nIndex >= kUnsortedBinIndex) && (nIndex < kUnsortedBinIndexEnd))
						return kBinTypeUnsorted;
					if((nIndex >= kSmallBinIndex) && (nIndex < kSmallBinIndexEnd))
						return kBinTypeSmall;
					if((nIndex >= kLargeBinIndex) && (nIndex < kLargeBinIndexEnd)) // This check shouldn't be necessary if all is well.
						return kBinTypeLarge;
				}
				pChunkCurrent = pChunkCurrent->mpNextChunk;
			}
		}
	}

	return kBinTypeNone;
}


void GeneralAllocator::AdjustHighFence()
{
	PPM_ASSERT(this, !mbHighFenceInternallyDisabled, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault

	mpHighFence = (char*)mpTopChunk + (GetChunkSize(mpTopChunk) / 2);
}


void GeneralAllocator::SetNewTopChunk(Chunk* pNewTopChunk, bool bFreePreviousTopChunk)
{
	PPM_ASSERT(this, !GetChunkIsInUse(mpTopChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNewTopChunk, pNewTopChunk); // If this fails, the user probably corrupted the heap. There may be a bug in PPMalloc, however.

	Chunk* const pPreviousTopChunk = mpTopChunk;
	mpTopChunk = pNewTopChunk;
	mpTopChunk->mpPrevChunk = mpTopChunk->mpNextChunk = mpTopChunk;

	if(pPreviousTopChunk != GetInitialTopChunk())
	{
		if(bFreePreviousTopChunk)
		{
			Chunk* const pUnsortedBinHeadChunk = GetUnsortedBin();
			LinkChunk(pPreviousTopChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
			#if (PPM_DEBUG >= 3) // Developer builds only.
				DebugCheckFreeChunk(pPreviousTopChunk);
			#endif
		}
	}

	if(!mbHighFenceInternallyDisabled)
		AdjustHighFence();
}


// AdjustTopChunk
//
// This function is called with input pChunk as the recently set mpTopChunk.
//
void GeneralAllocator::AdjustTopChunk(Chunk* pTopChunk, size_type nChunkSize)
{
	// pNextChunk will often be the top fencepost, but not if high allocation is 
	// enabled and in use or possibly if multiple core blocks exist.
	Chunk* const pNextChunk = GetChunkAtOffset(pTopChunk, (ssize_type)nChunkSize);
	pTopChunk->mpPrevChunk = pTopChunk->mpNextChunk = pTopChunk; // The top chunk is standalone, so have it point to itself.
	SetChunkSize(pTopChunk, nChunkSize | kChunkFlagPrevInUse);
	pNextChunk->mnPriorSize = nChunkSize; // Always set this, regardless of PPM_DEBUG_PRESERVE_PRIOR, because the previous chunk is a free chunk.
	PPM_ASSERT(this, !GetPrevChunkIsInUse(pNextChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNextChunk, pNextChunk); // If this fails, the user probably corrupted the heap.

	if(!mbHighFenceInternallyDisabled)
		AdjustHighFence();
}


///////////////////////////////////////////////////////////////////////////////
// ChunkMatchesLowHigh
//
// We return true if the given chunk is entirely within the high or low area. If the chunk
// straddles the high fence, we return false. Consider changing this behaviour to allow 
// straddling of the fence.
//
bool GeneralAllocator::ChunkMatchesLowHigh(int bHigh, const Chunk* pChunk, const size_type nChunkSize) const
{
	if(mpHighFence)
	{
		if(bHigh)
			return (((char*)pChunk + nChunkSize) >= mpHighFence);
		else
			return (pChunk < mpHighFence);
	}

	return true;
}


int GeneralAllocator::ChunkMatchesBlockType(const Chunk* pChunk, int nBlockTypeFlags)
{
	if((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk))
	{
		if((nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) == (kBlockTypeAllocated | kBlockTypeFree))
			return 1;

		const int bIsAllocated = GetChunkIsInUse(pChunk) && !GetChunkIsFastBin(pChunk); // Note that fast bin chunks appear to be allocated but in fact are merely cached.

		if(nBlockTypeFlags & kBlockTypeAllocated)
			return bIsAllocated;
		else if(nBlockTypeFlags & kBlockTypeFree)
			return bIsAllocated ? 0 : 1;
	}

	return 0;
}


void GeneralAllocator::SetFencepostAfterChunk(const Chunk* pChunk, size_type nChunkSize)
{
	Chunk* pFenceChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
	pFenceChunk->mnPriorSize = nChunkSize;      // We don't currently have a SetPriorChunkSize function.
	SetChunkSize(pFenceChunk, kFenceChunkSize); // We are making sure the kChunkFlagPrevInUse flag is not set.
	//pFenceChunk->mpPrevChunk = pFenceChunk->mpNextChunk = pFenceChunk; // Can't do this, because these fields don't exist in this chunk. 

	pFenceChunk = (Chunk*)(uintptr_t)((uintptr_t)pFenceChunk + kFenceChunkSize); // Move to the second of the pair.
	pFenceChunk->mnPriorSize = kFenceChunkSize;
	SetChunkSize(pFenceChunk, kFenceChunkSize | kChunkFlagPrevInUse);
}



//void GeneralAllocator::AddFencepost(Chunk* pChunk, size_type nPrevChunkFlags)
//{
//    // Get the current size.
//    size_t nChunkSize = GetChunkSize(pChunk);
//
//    // Set the new size.
//    nChunkSize = (nChunkSize - kFenceChunkSize) & ~kMinAlignmentMask;
//    SetChunkSizePreserveFlags(pChunk, (size_type)nChunkSize);
//
//    // We intentionally set the fence-post chunk as having the 
//    // previous chunk flagged as in-use. The reason for this is to
//    // prevent any code from ever seeing this chunk and trying to 
//    // coalesce it with the chunk before it.
//    Chunk* const pFenceChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
//    pFenceChunk->mnPriorSize = (size_type)nChunkSize;
//    SetChunkSize(pFenceChunk, kFenceChunkSize | nPrevChunkFlags);
//}


void GeneralAllocator::AddDoubleFencepost(Chunk* pChunk, size_type nPrevChunkFlags)
{
	size_t nChunkSize = GetChunkSize(pChunk);

	// Shrink pChunk to insert fenceposts, keeping size a
	// multiple of kMinEffectiveAlignment. We know there is at least enough 
	// space in pChunk to do this because a valid chunk is always
	// at least 4 * size_type (2 * kChunkInternalHeaderSize) in size.
	nChunkSize = (nChunkSize - kDoubleFenceChunkSize) & ~kMinAlignmentMask;
	SetChunkSizePreserveFlags(pChunk, (size_type)nChunkSize);

	// Note that the following assignments completely overwrite pChunk 
	// when nChunkSize was previously kMinChunkSize. This is intentional. 
	// We need the fenceposting, even if pChunk otherwise gets lost.
	Chunk* pFenceChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
	pFenceChunk->mnPriorSize = (size_type)nChunkSize;
	SetChunkSize(pFenceChunk, kFenceChunkSize | nPrevChunkFlags);

	// We intentionally set the top fence-post chunk as having the 
	// previous chunk flagged as in-use. The reason for this is to
	// prevent any code from ever seeing this chunk and trying to 
	// coalesce it with the chunk before it.
	pFenceChunk = GetChunkAtOffset(pChunk, (ssize_type)(nChunkSize + kFenceChunkSize));
	pFenceChunk->mnPriorSize = kFenceChunkSize;
	SetChunkSize(pFenceChunk, kFenceChunkSize | kChunkFlagPrevInUse);
}



///////////////////////////////////////////////////////////////////////////////
// MakeChunkFromCore
//
// The returned chunk is itself not marked as in-use.
//
GeneralAllocator::Chunk* GeneralAllocator::MakeChunkFromCore(void* pCore, size_t nCoreSize, size_type nFlags)
{
	PPM_ASSERT_STATIC(nCoreSize <= kMaxChunkSize, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, NULL); // If this fails, the user specified a core memory block that was too big.

	Chunk* const pChunk = reinterpret_cast<Chunk*>(pCore);
	pChunk->mnPriorSize = 0;
	SetChunkSize(pChunk, (size_type)(nCoreSize | nFlags));
	AddDoubleFencepost(pChunk, 0);
	return pChunk;
}


void GeneralAllocator::GetBlockInfoForCoreBlock(const CoreBlock* pCoreBlock, BlockInfo* pBlockInfo) const
{
	pBlockInfo->mpCore            = pCoreBlock->mpCore;
	pBlockInfo->mBlockType        = kBlockTypeCore;
	pBlockInfo->mpBlock           = pCoreBlock->mpCore;
	pBlockInfo->mnBlockSize       = pCoreBlock->mnSize;
	pBlockInfo->mbMemoryMapped    = false; // False for mmapped because we don't currently distinguish between mapped and unmapped core.
	pBlockInfo->mCoreType         = pCoreBlock->mCoreType;
	pBlockInfo->mCoreReservedSize = pCoreBlock->mnReservedSize;
}


void GeneralAllocator::GetBlockInfoForChunk(const Chunk* pChunk, BlockInfo* pBlockInfo, const void* pCore) const
{
	const size_type bIsAllocated = GetChunkIsInUse(pChunk);
	const size_type bIsMMapped   = GetChunkIsMMapped(pChunk);
	const size_type nChunkSize   = GetChunkSize(pChunk);

	pBlockInfo->mpCore            = pCore;
	pBlockInfo->mCoreType         = kCoreTypeNone; // This is not core memory, but user memory, so use kCoreTypeNone.
	pBlockInfo->mCoreReservedSize = 0;

	if(bIsAllocated)
	{
		const size_type bIsInternal = GetChunkIsInternal(pChunk);
		const size_t    nUsableSize = GetUsableSize(GetDataPtrFromChunkPtr(pChunk));

		if(bIsMMapped)
		{
			pBlockInfo->mBlockType     = (char)(bIsInternal ? kBlockTypeInternal : kBlockTypeAllocated);
			pBlockInfo->mpBlock        = pChunk;
			pBlockInfo->mnBlockSize    = pChunk->mnPriorSize + nChunkSize + kMinChunkSize; // kMinChunkSize is due to trailing chunk appended to mmapped chunks for administrative purposes.
			pBlockInfo->mpData         = GetDataPtrFromChunkPtr(pChunk);
			pBlockInfo->mnDataSize     = nUsableSize;
			pBlockInfo->mbMemoryMapped = true;
		}
		else
		{
			pBlockInfo->mBlockType     = (char)(bIsInternal ? kBlockTypeInternal : kBlockTypeAllocated);
			pBlockInfo->mpBlock        = pChunk, // Note that in a debug build, the bytes from a previous allocated chunk actually run into the first size_type bytes of this chunk.
			pBlockInfo->mnBlockSize    = nChunkSize;
			pBlockInfo->mpData         = GetDataPtrFromChunkPtr(pChunk);
			pBlockInfo->mnDataSize     = nUsableSize;
			pBlockInfo->mbMemoryMapped = false;
		}
	}
	else
	{
		pBlockInfo->mBlockType     = kBlockTypeFree;
		pBlockInfo->mpBlock        = pChunk;
		pBlockInfo->mnBlockSize    = nChunkSize;
		pBlockInfo->mpData         = GetPostHeaderPtrFromChunkPtr(pChunk); // Note that this is a different offset than with the data ptr for an allocated chunk.
		pBlockInfo->mnDataSize     = GetChunkSize(pChunk) - sizeof(Chunk); // Don't call GetUsableSize here, as it doesn't work for freed chunks.
		pBlockInfo->mbMemoryMapped = false;
	}
}


int GeneralAllocator::CheckChunk(const Chunk* pChunk)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int       nErrorCount = 0;
	int       result;
	size_type nChunkSize;

	if(mnCheckChunkReentrancyCount <= 1) // We allow it to be zero or one, but no more.
	{
		mnCheckChunkReentrancyCount++;

		const size_type bChunkIsMMapped = GetChunkIsMMapped(pChunk);

		// Do some CoreBlock tests, which don't apply to MMapped chunks.
		if(!bChunkIsMMapped)
		{
			// Assert that pChunk is in a valid core block. We assume the core block data structures
			// are intact, which is almost always so. They don't tend to get corrupted very easily.
			const CoreBlock* const pCoreBlockCurrent = FindCoreBlockForAddress(pChunk);
			result = PPM_VERIFY(this, pCoreBlockCurrent != NULL, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk);
			nErrorCount += result;
			if(result != 0) // If there was a problem...
				goto End;

			// Assert that the next block is within the same core block as the current block.
			nChunkSize = GetChunkSize(pChunk);
			const CoreBlock* const pCoreBlockNext = FindCoreBlockForAddress((void*)((uintptr_t)pChunk + (uintptr_t)nChunkSize));
			result = PPM_VERIFY(this, pCoreBlockNext == pCoreBlockCurrent, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			nErrorCount += result;
			if(result != 0) // If there was a problem...
				goto End;

			// Assert that the chunk's prev size refers to a location within this core block.
			#if !PPM_DEBUG_PRESERVE_PRIOR 
			if(!GetPrevChunkIsInUse(pChunk)) 
			#endif
			{
				const Chunk* const pPrevChunk = GetPrevChunk(pChunk);
				const CoreBlock* const pCoreBlockPrev = FindCoreBlockForAddress(pPrevChunk);
				result = PPM_VERIFY(this, pCoreBlockPrev == pCoreBlockCurrent, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pPrevChunk, pPrevChunk);
				nErrorCount += result;
				if(result != 0) // If there was a problem...
					goto End;
			}

			// Assert that the next chunk's prev size is the same as this chunk's size.
			#if !PPM_DEBUG_PRESERVE_PRIOR 
			if(!GetChunkIsInUse(pChunk)) 
			#endif
			{
				const size_type nNextChunkPrevSize = GeneralAllocator::GetNextChunkPrevSize(pChunk, nChunkSize);
				result = PPM_VERIFY(this, nNextChunkPrevSize == nChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
				nErrorCount += result;
				if(result != 0) // If there was a problem...
					goto End;
			}
		}

		nChunkSize = GetChunkSize(pChunk);

		if(bChunkIsMMapped)
		{
			// Assert that memory mapping is available
			nErrorCount += PPM_VERIFY(this, PPM_MMAP_SUPPORTED != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

			// Assert that this chunk is independent of any core block.
			const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
			nErrorCount += PPM_VERIFY(this, pCoreBlock == NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk); 

			#if PPM_MMAP_SUPPORTED
				// Chunk beginning and end should be page-aligned.
				nErrorCount += PPM_VERIFY(this, PPM_IsAligned((char*)pChunk - pChunk->mnPriorSize, mnMMapPageSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);
				nErrorCount += PPM_VERIFY(this, PPM_IsAligned((char*)pChunk + nChunkSize + kMinChunkSize, mnMMapPageSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);

				// Memory should be minimally aligned.
				nErrorCount += PPM_VERIFY(this, GetIsMinAligned(GetDataPtrFromChunkPtr(pChunk)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);
			#endif
		}
		else
		{
			// In PPM_DEBUG_PRESERVE_PRIOR mode, the mnPriorSize field of the next contiguous chunk after this one 
			// should be a valid field and should have the correct size stored in it. If PPM_DEBUG_PRESERVE_PRIOR
			// is not enabled, then the bytes of a user allocation go into the space of the next
			// contiguous chunk's mnPriorSize field (for space efficiency).
			#if PPM_DEBUG_PRESERVE_PRIOR
				const size_type nNextChunkPrevSize = GetNextChunkPrevSize(pChunk, nChunkSize);
				nErrorCount += PPM_VERIFY(this, nNextChunkPrevSize == nChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			#endif

			const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
			nErrorCount += PPM_VERIFY(this, pCoreBlock != NULL, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk); // Assert that this is connected to a core block.

			// Assert that this chunk fits within its core block.
			nErrorCount += PPM_VERIFY(this, (char*)pChunk >= (char*)pCoreBlock->mpCore, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			nErrorCount += PPM_VERIFY(this, (char*)pChunk  < ((char*)pCoreBlock->mpCore + pCoreBlock->mnSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			nErrorCount += PPM_VERIFY(this, ((char*)pChunk + nChunkSize) < ((char*)pCoreBlock->mpCore + pCoreBlock->mnSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

			// Should have legal address
			if(pChunk == mpTopChunk)
			{
				// Top chunks have themselves as their next and prev list chunks.
				nErrorCount += PPM_VERIFY(this, (pChunk->mpPrevChunk == pChunk) && (pChunk->mpNextChunk == pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

				// Top chunk's size should always be at least kMinChunkSize.
				nErrorCount += PPM_VERIFY(this, nChunkSize >= kMinChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

				// Top chunk's predecessor should always be marked in-use.
				nErrorCount += PPM_VERIFY(this, GetPrevChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
			}
			else
			{
				if(GetCoreIsContiguous())
				{
					nErrorCount += PPM_VERIFY(this, (char*)pChunk >= (char*)pCoreBlock->mpCore, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk);

					const Chunk* const pFenceChunk = GetFenceChunk(pCoreBlock);
					nErrorCount += PPM_VERIFY(this, (((char*)pChunk + nChunkSize) <= (char*)pFenceChunk) || (pChunk == pFenceChunk), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pChunk, pChunk);
				}
			}

			#if (PPM_DEBUG >= 3) // Developer builds only.
				// Now check the chunk that is prior to this chunk in memory and check the chunk that is after 
				// this in memory. If the prior chunk is in use, then we can't find it by looking backward 
				// because our mnPriorSize field is not valid (it is owned by the previous chunk). In this case,
				// we can only find the previous chunk by starting at the beginning of the core block and 
				// walking all of it (potentially jumping through thousands of allocations over many megabytes).
				// If the previous chunk is not in use (i.e. it is free), then this chunk should be in use, as
				// we require any two contiguous chunks to be coalesced.
				if(mnCheckChunkReentrancyCount == 1)
				{
					{
						// Prior chunk
						Chunk* pPriorChunk;

						if(GetPrevChunkIsInUse(pChunk))
						{
							#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
								pPriorChunk = FindPriorChunk(pChunk);
							#else
								pPriorChunk = NULL; // FindPriorChunk is potentially very expensive when PPM_DEBUG_PRESERVE_PRIOR is disabled, so we simply don't do it.
							#endif
						}
						else
						{
							// As stated above, the current chunk must be in use if the prior is not.
							nErrorCount += PPM_VERIFY(this, GetChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
							pPriorChunk = GetPrevChunk(pChunk);
						}
						
						if(pPriorChunk) // Note that chunks at the beginning of core blocks have no previous chunks in memory.
						{
							// We don't need to check for pPriorChunk being a fence chunk (GetChunkIsFenceChunk()), 
							// as fence chunks are only at the top end of core blocks, not the bottom.
							if(GetChunkIsInUse(pPriorChunk))
								nErrorCount += CheckMallocedChunk(pPriorChunk, GetChunkSize(pPriorChunk) - kDataPtrOffset, false, false); // We don't know the allocation size, so we pick the max possible size.
							else
								nErrorCount += CheckFreeChunk(pPriorChunk);
						}
					}

					{
						// Next chunk
						Chunk* pNextChunk = GetNextChunk(pChunk);

						if(pNextChunk)
						{
							// Only check the next chunk if it is not a fence chunk (one of our fake fence chunks at the end of core blocks).
							if(!GetChunkIsFenceChunk(pNextChunk))
							{
								if(GetChunkIsInUse(pNextChunk)) // Note that if this chunk is a top-end fence chunk, then its size will be kSizeTypeSize (usually 4), but calling GetSize() on it will return 0. This is by lucky coincidence as otherwise we'd read to out of bounds memory.
									nErrorCount += CheckMallocedChunk(pNextChunk, GetChunkSize(pNextChunk) - kDataPtrOffset, false, false); // We don't know the allocation size, so we pick the max possible size.
								else
									nErrorCount += CheckFreeChunk(pNextChunk);
							}
						}
					}

					// Now check the chunk that is previous in the chunk's list and check the chunk that is next in
					// the chunk's list. Chunks that are in use are not in any list and have taken over ownership of
					// the memory of the previous and next list members.
					if(!GetChunkIsInUse(pChunk))
					{
						// Previous chunk
						Chunk* const pPreviousChunk = pChunk->mpPrevChunk;
						nErrorCount += PPM_VERIFY(this, pPreviousChunk != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pPreviousChunk, pPreviousChunk);

						if(pPreviousChunk != pChunk) // No point in re-checking self.
						{
							if(!GetChunkIsListHead(pPreviousChunk)) // We cannot do any checking on list head chunks, as they are fake chunks present only to hold lists.
							{
								// No fence chunk should be in any list.
								nErrorCount += PPM_VERIFY(this, !GetChunkIsFenceChunk(pPreviousChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pPreviousChunk, pPreviousChunk);

								if(GetChunkIsInUse(pPreviousChunk))
									nErrorCount += CheckMallocedChunk(pPreviousChunk, GetChunkSize(pPreviousChunk) - kDataPtrOffset, false, false); // We don't know the allocation size, so we pick the max possible size.
								else
									nErrorCount += CheckFreeChunk(pPreviousChunk);
							}
						}

						// Next chunk
						Chunk* const pNextChunk = pChunk->mpNextChunk;
						nErrorCount += PPM_VERIFY(this, pNextChunk != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

						if(pNextChunk != pChunk) // No point in re-checking self.
						{
							if(!GetChunkIsListHead(pNextChunk)) // We cannot do any checking on list head chunks, as they are fake chunks present only to hold lists.
							{
								// No fence chunk should be in any list.
								nErrorCount += PPM_VERIFY(this, !GetChunkIsFenceChunk(pNextChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);

								if(GetChunkIsInUse(pNextChunk))
									nErrorCount += CheckMallocedChunk(pNextChunk, GetChunkSize(pNextChunk) - kDataPtrOffset, false, false); // We don't know the allocation size, so we pick the max possible size.
								else
									nErrorCount += CheckFreeChunk(pNextChunk);
							}
						}
					}
				}
			#endif // PPM_DEBUG >= N
		}

		End:
		mnCheckChunkReentrancyCount--;
	}

	return nErrorCount;
}


// Checks the state of the regular bin (eg. not fast bin) that pChunk would go into. 
// pChunk doesn't need to currently be in the bin.
int GeneralAllocator::CheckChunkRegularBin(const Chunk* pChunk)
{
	int nErrorCount = 0;

	const size_type nChunkSize = GetChunkSize(pChunk);
	
	// Check large bin skip list integrity.
	if(nChunkSize >= (size_type)kLargeBinMinChunkSize)
	{
		const int      binIndex       = GetLargeBinIndexFromChunkSize(nChunkSize);
		SkipListChunk* pBin           = GetBin(binIndex); 
		SkipListChunk* pSkipListChunk = pBin->mpSmallerChunks;

		// The first chunk in every large bin is necessarily the start of the first run.
		PPM_ASSERT(this, pBin->mpNextChunk == pBin->mpSmallerChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

		// Verify that all Chunks in the list and all the skip list chunks they reference are large chunks.
		while(pSkipListChunk != pBin)
		{
			PPM_ASSERT(this, GetChunkIsLargeBin(pSkipListChunk), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			PPM_ASSERT(this, (pSkipListChunk->mpLargerChunks  == pBin) || GetChunkIsLargeBin(pSkipListChunk->mpLargerChunks),  kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			PPM_ASSERT(this, (pSkipListChunk->mpSmallerChunks == pBin) || GetChunkIsLargeBin(pSkipListChunk->mpSmallerChunks), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

			pSkipListChunk = pSkipListChunk->mpSmallerChunks;
		}

		// Walk the skip list and do some testing.
		pSkipListChunk = pBin->mpSmallerChunks;

		while(pSkipListChunk != pBin)
		{
			// pSkipListChunk is expected to be a skip list run head.
			PPM_ASSERT(this, pSkipListChunk->mpLargerChunks->mpSmallerChunks == pSkipListChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			PPM_ASSERT(this, pSkipListChunk->mpSmallerChunks->mpLargerChunks == pSkipListChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

			pChunk = pSkipListChunk->mpNextChunk;
			while((pChunk != pSkipListChunk->mpSmallerChunks) && (pChunk != pBin)) // The first != check should be sufficient. The second one is just a fallback in case our skip list is corrupt.
			{
				PPM_ASSERT(this, GetChunkSize(pChunk) == GetChunkSize(pChunk->mpPrevChunk), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
				#if (PPM_DEBUG >= 2)
					PPM_ASSERT(this, !((SkipListChunk*)pChunk)->mpLargerChunks && !((SkipListChunk*)pChunk)->mpSmallerChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
				#endif

				pChunk = pChunk->mpNextChunk;
			}

			pSkipListChunk = pSkipListChunk->mpSmallerChunks;
		}
	}
	// To do: Test other bin types.

	return nErrorCount;
}



int GeneralAllocator::CheckFreeChunk(const Chunk* pChunk)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	const size_type nChunkSize       = GetChunkSize(pChunk);
	const Chunk*    const pNextChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);

	nErrorCount += CheckChunk(pChunk);

	// Chunk should be set as free.
	nErrorCount += PPM_VERIFY(this, !GetChunkIsInUse(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
	nErrorCount += PPM_VERIFY(this, !GetChunkIsMMapped(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

	// Chunk should have proper free fill.
	#if PPM_DEBUG_FILL
		int nIndex = GetBinIndexFromChunkSize(nChunkSize);
		PPM_ASSERT(this, nIndex < (int)EAArrayCount(mpBinArray), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

		if(nIndex >= kLargeBinIndex)
		{
			// Large bins use an extra two pointers to maintain a skip list. See the SkipListChunk and Chunk struct declarations.
			// As a result the filled memory doesn't begin until two pointer worth or memory later in the Chunk.
			const SkipListChunk* const pSkipListChunk = (SkipListChunk*)pChunk;
			const void*          const pFill          = GetPostHeaderPtrFromSkipListChunkPtr(pSkipListChunk);
			nErrorCount += PPM_VERIFY(this, DebugMemCheck(pFill, nChunkSize - sizeof(SkipListChunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pChunk, pFill);
			// To do: Verify that pSkipListChunk->mpLargerChunks and pSkipListChunk->mpSmallerChunks are NULL (actually we currently null them only in PPM_DEBUG >= 2) or point to proper Chunks in the bin.
		}
		else
		{
			const void* const pFill = GetPostHeaderPtrFromChunkPtr(pChunk);
			nErrorCount += PPM_VERIFY(this, DebugMemCheck(pFill, nChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pChunk, pFill);
		}
	#endif

	if(nChunkSize >= kMinChunkSize) // If it's a normal (and not a fence) chunk...
	{
		nErrorCount += PPM_VERIFY(this, (nChunkSize & kMinAlignmentMask) == 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
		nErrorCount += PPM_VERIFY(this, GetIsMinAligned(GetDataPtrFromChunkPtr(pChunk)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);

		// Should have a matching footer field.
		nErrorCount += PPM_VERIFY(this, pNextChunk->mnPriorSize == nChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNextChunk, pNextChunk); // The next contiguous chunk after a free chunk should always be an allocated chunk and thus mnPriorSize should always be valid.

		// Should be fully coalesced (a.k.a. consolidated).
		nErrorCount += PPM_VERIFY(this, GetPrevChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
		nErrorCount += PPM_VERIFY(this, (pChunk == mpTopChunk) || GetChunkIsInUse(pNextChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

		// Should have minimally sane links.
		nErrorCount += PPM_VERIFY(this, pChunk->mpNextChunk->mpPrevChunk == pChunk, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
		nErrorCount += PPM_VERIFY(this, pChunk->mpPrevChunk->mpNextChunk == pChunk, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

		if((pChunk == pChunk->mpNextChunk) || (pChunk == pChunk->mpPrevChunk))
		{
			// If mpNextChunk points to self, then mpPrevChunk must also, else we don't have a proper linked list.
			nErrorCount += PPM_VERIFY(this, pChunk->mpPrevChunk == pChunk->mpNextChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);

			// The only chunks which have their next and prev pointers pointing to themselves are
			// supposed to be mpTopChunk. All other chunks should be in lists.
			nErrorCount += PPM_VERIFY(this, pChunk == mpTopChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);

			// Disabled because this is no longer necessarily the case: Assert that this is the top chunk for the core block.
			// nErrorCount += PPM_VERIFY(this, ((char*)pChunk + nChunkSize + kDoubleFenceChunkSize) == ((char*)pCoreBlock->mpCore + pCoreBlock->mnSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		}
	}
	else // Fence chunks are always of size kSizeTypeSize.
	{  // We check for <= kSizeTypeSize because the GetChunkSize function masks away the low bits and this 'hides' small values from us.
		nErrorCount += PPM_VERIFY(this, nChunkSize <= kSizeTypeSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);
	}

	return nErrorCount;
}



int GeneralAllocator::CheckUsedChunk(const Chunk* pChunk)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	nErrorCount += CheckChunk(pChunk);

	if(nErrorCount == 0)
	{
		if(GetChunkIsMMapped(pChunk)) // Memory mapped chunks have no next/prev.
		{
			//Possibly do something here.
		}
		else
		{
			// Check whether it claims to be in use.
			nErrorCount += PPM_VERIFY(this, GetChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

			// Since more things can be checked with free chunks than used ones,
			// if a used chunk borders them and debug is on, it's worth checking them.
			if(!GetPrevChunkIsInUse(pChunk))
			{
				// Note that we cannot even look at prev unless it is unused.
				Chunk* const pPrevChunk = GetPrevChunk(pChunk);
				nErrorCount += PPM_VERIFY(this, GetNextChunk(pPrevChunk) == pChunk, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pPrevChunk, pChunk);
				DebugCheckFreeChunk(pPrevChunk);
			}

			const Chunk* const pNextChunk = GetNextChunk(pChunk);

			nErrorCount += CheckChunk(pNextChunk);

			if(nErrorCount == 0)
			{
				if(pNextChunk == mpTopChunk)
				{
					nErrorCount += PPM_VERIFY(this, GetPrevChunkIsInUse(pNextChunk) != 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNextChunk, pNextChunk);
					nErrorCount += PPM_VERIFY(this, GetChunkSize(pNextChunk) >= kMinChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNextChunk, pNextChunk);
				}
				else if(!GetChunkIsInUse(pNextChunk))
				{
					DebugCheckFreeChunk(pNextChunk);
				}
			}
		}
	}

	return nErrorCount;
}


int GeneralAllocator::CheckRemallocedChunk(const Chunk* pChunk, size_type nRequestedChunkSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	const size_type nChunkSize = GetChunkSize(pChunk);

	nErrorCount += CheckUsedChunk(pChunk);

	// Check for legal size
	nErrorCount += PPM_VERIFY(this, (nChunkSize & kMinAlignmentMask) == 0, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk);
	nErrorCount += PPM_VERIFY(this, nChunkSize >= kMinChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

	// Check for legal alignment
	nErrorCount += PPM_VERIFY(this, GetIsMinAligned(GetDataPtrFromChunkPtr(pChunk)), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk);

	// Check for chunk size being large enough for request size.
	nErrorCount += PPM_VERIFY(this, nChunkSize >= kDataPtrOffset, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

	// Check that the returned chunk has very little internal fragmentation,
	// that its size is very close to the requested chunk size.
	nErrorCount += PPM_VERIFY(this, (nRequestedChunkSize + kMinChunkSize) >= nChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);

	return nErrorCount;
}


int GeneralAllocator::CheckMallocedChunk(const Chunk* pChunk, size_type nRequestedChunkSize, bool bNewlyMalloced, bool bAllowPrevNotInUse)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	nErrorCount += PPM_VERIFY(this, GetChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

	nErrorCount += CheckRemallocedChunk(pChunk, nRequestedChunkSize);

	if(bNewlyMalloced && !bAllowPrevNotInUse)
	{
		// The previous chunk of any *newly* allocated chunk should be used.
		// Each allocated chunk borders either a previously allocated and 
		// still in-use chunk, or the base of its core block. This is 
		// ensured by making all allocations from the lowest part of
		// free chunks. This does not necessarily hold however for 
		// chunks recycled via fastbins.
		nErrorCount += PPM_VERIFY(this, GetPrevChunkIsInUse(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);
	}

	return nErrorCount;
}


int GeneralAllocator::CheckMMappedChunk(const Chunk* pChunk)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	// Do standard chunk verification
	nErrorCount += CheckChunk(pChunk);

	// Verify that the chunk is in fact marked as mmapped.
	nErrorCount += PPM_VERIFY(this, GetChunkIsMMapped(pChunk) != 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);
	
	// Verify that this chunk is in our mHeadMMapChunk list.
	const Chunk* pCurrentChunk = mHeadMMapChunk.mpNextChunk;

	while(pCurrentChunk != &mHeadMMapChunk)
	{
		const Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(pCurrentChunk);

		if(pMMappedChunk == pChunk)
			break;

		pCurrentChunk = pCurrentChunk->mpNextChunk;
	}

	nErrorCount += PPM_VERIFY(this, pCurrentChunk != &mHeadMMapChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCurrentChunk, pCurrentChunk);

	return nErrorCount;
}


int GeneralAllocator::CheckState(HeapValidationLevel heapValidationLevel)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;
	int i, iEnd;

	#if (PPM_DEBUG >= 3) // Developer builds only.
		// For higher debug levels, we bump up the requested validation level.
		heapValidationLevel = (HeapValidationLevel)(heapValidationLevel + (PPM_DEBUG - 1));
	#endif

	if(heapValidationLevel >= kHeapValidationLevelBasic)
	{
		size_type nSizeTotal = 0;

		// Internal size_t must be no wider than pointer type.
		nErrorCount += PPM_VERIFY(this, sizeof(size_type) <= sizeof(char*), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		// Cannot run remaining checks until fully initialized.
		if(!mpTopChunk || (mpTopChunk == GetInitialTopChunk()))
			return nErrorCount;


		// Member data
		{
			// FastBin max chunk size must be sane and legal.
			nErrorCount += PPM_VERIFY(this, GetMaxFastBinChunkSize() <= kFastBinMaxDataSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				 #if PPM_FASTBINS_ENABLED
					// FastBin array must have sane values.
					// The FastBin array is a linearly linked list with the end item being NULL.
					for(i = 0, iEnd = sizeof(mpFastBinArray) / sizeof(mpFastBinArray[0]); i < iEnd; i++)
					{
						const Chunk* pChunk = mpFastBinArray[i];
						int j(0), jMax(2000000);

						while(pChunk && (j < jMax))
						{
							const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
							nErrorCount += PPM_VERIFY(this, pCoreBlock != NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk);

							// Verify that the size of the Chunk matches the size of the fast bin
							const size_type size  = GetChunkSize(pChunk);
							const unsigned  index = GetFastBinIndexFromChunkSize(size);
							nErrorCount += PPM_VERIFY(this, (int)index == i, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);  // All chunks are marked as fast binned when added to fast bins.

							#if (PPM_DEBUG >= 2) // Extended user debug builds. mpPreChunk is set to NULL when PPM_DEBUG >= 2.
								nErrorCount += PPM_VERIFY(this, pChunk->mpPrevChunk == NULL, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // See GeneralAllocator::UnlinkChunk for where this value is set to NULL.
							#endif

							#if PPM_DEBUG_FILL
								PPM_ASSERT(this, DebugMemCheck((char*)pChunk + sizeof(Chunk), size - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
							#endif

							nErrorCount += PPM_VERIFY(this, GetChunkIsFastBin(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);  // All chunks are marked as fast binned when added to fast bins.
							nErrorCount += PPM_VERIFY(this, GetChunkIsInternal(pChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // All chunks are marked as internal when added to fast bins.

							pChunk = pChunk->mpNextChunk;

							j++; // Verify that this list isn't circular in some way. It's supposed to be linear with a NULL at the end.
							nErrorCount += PPM_VERIFY(this, j < jMax, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
						}
					}
				#endif
			}

			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				// Bin array must have sane values.
				for(i = kUnsortedBinIndex, iEnd = kBinCount; i < iEnd; i++)
				{
					const SkipListChunk* pBin   = GetBin(i); // Save this for below.
					const SkipListChunk* pChunk = pBin;
					
					// Verify that bin skip-lists are being used for large bins, and unused (NULL) for others).
					bool bIsLarge = IsBinLarge(pBin); 
					if(bIsLarge)
						nErrorCount += PPM_VERIFY(this,  pBin->mpLargerChunks && pBin->mpSmallerChunks, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
					else
						nErrorCount += PPM_VERIFY(this, !pBin->mpLargerChunks && !pBin->mpSmallerChunks, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);

					// Look at the Chunks in the bin.
					nErrorCount += PPM_VERIFY(this, pChunk != NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL); // Even if the bin is empty, there is a head chunk which points to itself.

					PPM_VERIFY(this, pChunk != NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
					if(pChunk) // This should always be valid
					{
						pChunk = (SkipListChunk*)pChunk->mpNextChunk; // All Chunks in the bin array are treated as SkipListChunks.

						while(pChunk != pBin)
						{
							const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pChunk);
							nErrorCount += PPM_VERIFY(this, pCoreBlock != NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk);

							pChunk = (SkipListChunk*)pChunk->mpNextChunk; // All Chunks in the bin array are treated as SkipListChunks.
						}
					}
				}
			}

			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				// The following is no longer true except for possibly 
				// under Unix-like platforms that use brk() and sbrk():
				// 
				// Top chunk should be the higest chunk in memory.
				// CoreBlock* pCoreBlockTop = mHeadCoreBlock.mpNextCoreBlock;
				// CoreBlock* pCoreBlock     = mHeadCoreBlock.mpNextCoreBlock;
				// while(pCoreBlock != &mHeadCoreBlock) // For each core block...
				// {
				//     if(pCoreBlockTop->mpCore < pCoreBlock->mpCore)
				//         pCoreBlockTop = pCoreBlock;
				//     pCoreBlock = pCoreBlock->mpNextCoreBlock;
				// }
				// pCoreBlock = FindCoreBlockForAddress(mpTopChunk);
				// nErrorCount += PPM_VERIFY(this, pCoreBlockTop == pCoreBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

				// Misc
				nErrorCount += PPM_VERIFY(this, mnTopPad < 100000, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Should be some sane value.
				nErrorCount += PPM_VERIFY(this, mpInitialTopChunk != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				nErrorCount += PPM_VERIFY(this, (mnPageSize & (mnPageSize - 1)) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Page size should be an even power of 2
			}
		}


		// FastBin checks
		#if PPM_FASTBINS_ENABLED
			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				const unsigned nMaxFastBinChunkSize = GetMaxFastBinChunkSize();
				const unsigned nLastFastBinIndex    = GetFastBinIndexFromChunkSize(nMaxFastBinChunkSize);

				nErrorCount += PPM_VERIFY(this, nMaxFastBinChunkSize <= GetChunkSizeFromDataSize(kFastBinMaxDataSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

				for(i = 0; i < kFastBinCount; ++i)
				{
					const Chunk* pChunk = mpFastBinArray[i];

					// All fast bins past max should be empty.
					if(i > (int)nLastFastBinIndex)
						nErrorCount += PPM_VERIFY(this, pChunk == NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);

					while(pChunk)
					{
						DebugCheckUsedChunk(pChunk);
						nSizeTotal += GetChunkSize(pChunk);
						nErrorCount += PPM_VERIFY(this, GetFastBinIndexFromChunkSize(GetChunkSize(pChunk)) == (unsigned)i, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk);
						pChunk = pChunk->mpNextChunk;
					}
				}

				if(nSizeTotal)
					nErrorCount += PPM_VERIFY(this, GetFastBinChunksExist() != 0, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
				else if(!GetFastBinChunksExist())
					nErrorCount += PPM_VERIFY(this, nSizeTotal == 0, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
			}
		#endif


		// Normal bins
		if(heapValidationLevel >= kHeapValidationLevelDetail)
		{
			for(i = kUnsortedBinIndex; i < kBinCount; ++i) // For each bin... (bin index 0 is unaccessible)
			{
				const SkipListChunk* const pBin = GetBin(i);

				// Binmap should accurate, except for bin 1, which is the 'unsorted cache bin'.
				if(i >= kSmallBinIndex)
				{
					const bool bBitmapBitIsSet = (GetBinBitmapBit(i) != 0);
					const bool bBinNotEmpty    = (pBin != GetLastChunkInBin(pBin));

					// It is legal to have: bBitmapBitIsSet = true, bBinNotEmpty = true, because of lazy evaluation.
					if(!bBitmapBitIsSet)
						nErrorCount += PPM_VERIFY(this, !bBinNotEmpty, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
					else if(bBinNotEmpty)
						nErrorCount += PPM_VERIFY(this, bBitmapBitIsSet, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
				}

				// For each chunk in each bin...
				size_type nPrevChunkSize = 0;
				size_type nChunkSize     = 0;

				for(const SkipListChunk* pChunk = GetLastChunkInBin(pBin); pChunk != pBin; pChunk = (SkipListChunk*)pChunk->mpPrevChunk)
				{
					// Each chunk in regular bins should always be marked as free.
					nErrorCount   += CheckFreeChunk(pChunk);

					nPrevChunkSize = nChunkSize;
					nChunkSize     = GetChunkSize(pChunk);
					nSizeTotal    += nChunkSize;

					if(i >= kSmallBinIndex)
					{
						// Verify that chunk is in the correct bin.
						const int nBinIndex = GetBinIndexFromChunkSize(nChunkSize);
						nErrorCount += PPM_VERIFY(this, nBinIndex == i, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk);

						// Verify that for sorted bins (i.e. large bins), chunks are sorted from largest to smallest size.
						if((i >= kLargeBinIndex) && (pChunk->mpPrevChunk != pBin)) 
							nErrorCount += PPM_VERIFY(this, nChunkSize >= nPrevChunkSize, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL);
					}

					// Chunk should be followed by a legal chain of used chunks. Any free chunk should be followed by 
					// a used chunk. If following chunk is unused, then we have a coalescing problem. We check all 
					// successive used chunks, just for testing.
					for(const Chunk* pChunk2 = GetNextChunk(pChunk); 
						 !GetChunkIsFenceChunk(pChunk2) && GetChunkIsInUse(pChunk2) && (GetChunkSize(pChunk2) >= kMinChunkSize); 
						 pChunk2 = GetNextChunk(pChunk2))
					{
						nErrorCount += CheckUsedChunk(pChunk2);
					}
				}
			}
		}


		// MMapped chunk list
		#if PPM_MMAP_SUPPORTED
			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				// Verify that this chunk is in our mHeadMMapChunk list.
				const Chunk* pCurrentChunk = mHeadMMapChunk.mpNextChunk;
				while(pCurrentChunk != &mHeadMMapChunk){
					nErrorCount += PPM_VERIFY(this, GetChunkIsMMapped(pCurrentChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pCurrentChunk, pCurrentChunk); // We mark our list chunks as mmapped too.

					const Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(pCurrentChunk);
					nErrorCount += PPM_VERIFY(this, GetChunkIsMMapped(pMMappedChunk) != 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pMMappedChunk, pMMappedChunk);

					pCurrentChunk = pCurrentChunk->mpNextChunk;
				}
			}
		#endif

		// Top chunk
		nErrorCount += CheckChunk(mpTopChunk);

		// Statistics
		nErrorCount += PPM_VERIFY(this, mnMMapCount >= 0,                kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		nErrorCount += PPM_VERIFY(this, mnMMapCount <= mnMMapMaxAllowed, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		// To consider: Do more stats here.
	}

	if(heapValidationLevel >= kHeapValidationLevelDetail)
	{
		// Verify that no chunk is referenced in more than one list.
		// Verify that no free chunk is lost (outside of any list).
		// Core block list.
	}

	if(heapValidationLevel >= kHeapValidationLevelFull)
	{
		// Walk all blocks, verifying each as we go.

		// Memory mapped chunks
		#if PPM_MMAP_SUPPORTED
		{
			const Chunk* pChunk = mHeadMMapChunk.mpNextChunk;
			while(pChunk != &mHeadMMapChunk)
			{
				const Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(pChunk);
				nErrorCount += CheckMMappedChunk(pMMappedChunk);
				pChunk = pChunk->mpNextChunk;
			}
		}
		#endif

		// Core chunks
		CoreBlock* pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
		while((pCoreBlock != &mHeadCoreBlock) && (nErrorCount == 0)) // For each core block...
		{
			//Walk all chunks in this block.
			const Chunk* pChunk      = (Chunk*)pCoreBlock->mpCore;
			const Chunk* pFenceChunk = GetFenceChunk(pCoreBlock);

			while(pChunk < pFenceChunk) // For each chunk in the core block...
			{
				if(!ValidateChunk(pChunk, heapValidationLevel))
				{
					nErrorCount++; // We don't use a PPM_ASSERT here because ValidateChunk will take care of assertion failures. 
					break;
				}

				pChunk = GetNextChunk(pChunk);
			}

			pCoreBlock = pCoreBlock->mpNextCoreBlock;
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// ValidateChunk
//
bool GeneralAllocator::ValidateChunk(const void* pChunkVoid, HeapValidationLevel /*heapValidationLevel*/)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int nErrorCount = 0;

	const Chunk* const pChunk   = (const Chunk*)pChunkVoid;
	const size_type    bIsInUse = GetChunkIsInUse(pChunk);

	if(bIsInUse) 
		nErrorCount += CheckUsedChunk(pChunk);
	else
		nErrorCount += CheckFreeChunk(pChunk);

	if(nErrorCount == 0)
	{
		if(!bIsInUse && (pChunk == pChunk->mpNextChunk)) // If the chunk is pointing to itself (which is usually bad)...
		{
			// The one mpTopChunk has pointers to itself and would be OK. 
			// Otherwise this is bad because we would infinitely loop if we didn't catch it.
			nErrorCount += PPM_VERIFY(this, pChunk == mpTopChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk);
		}
	}

	return (nErrorCount == 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetDataPreview
//
// Given a block of binary data, this function makes a readable version of it
// for display on a text output. The output format is similar to that seen 
// in hex editors, with hex text on the left and ASCII text on the right.
// 
// Output example:
//     "48 65 6c 6c 6f\tHello"
//
DataPreviewType GeneralAllocator::GetDataPreview(const void* pDataVoid, size_t nDataSize, 
												 char* pBuffer, wchar_t* pBufferW, size_t nBufferLength) const
{
	return EA::Allocator::GetDataPreview(pDataVoid, nDataSize, pBuffer, pBufferW, nBufferLength);
}


///////////////////////////////////////////////////////////////////////////////
// AssertionFailureFunctionDefault
//
void GeneralAllocator::AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);
	PPM_DebugBreak();
}


///////////////////////////////////////////////////////////////////////////////
// SetAssertionFailureFunction
//
void GeneralAllocator::SetAssertionFailureFunction(GeneralAllocator::AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
{
	mpAssertionFailureFunction        = pAssertionFailureFunction;
	mpAssertionFailureFunctionContext = pContext;
}


///////////////////////////////////////////////////////////////////////////////
// SetAssertionFailureFunction
//
// *** This function is deprecated ***
//
void GeneralAllocator::SetAssertionFailureFunction(GeneralAllocator::AssertionFailureFunction /*pAssertionFailureFunction*/, void* /*pContext*/)
{
}


///////////////////////////////////////////////////////////////////////////////
// AssertionFailure
//
void GeneralAllocator::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const
{
	if(mpAssertionFailureFunction)
	{
		AssertionFailureInfo afi;

		afi.mpGeneralAllocator = this;
		afi.mpExpression       = pExpression;
		afi.mnErrorId          = nErrorId;
		afi.mpDescription      = pDescription;
		afi.mpData             = pData;
		afi.mpExtra            = pExtra;

		mpAssertionFailureFunction(&afi, mpAssertionFailureFunctionContext);
	}
}



///////////////////////////////////////////////////////////////////////////////
// DescribeChunk
//
// This is a basic chunk describing function.
// 
size_t GeneralAllocator::DescribeChunk(const Chunk* pChunk, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const
{
	int             nCount;
	char* const     pBufferStart = pBuffer;
	const void*     pData = (char*)GetDataPtrFromChunkPtr(pChunk);
	const size_type nDataSize = GetChunkSize(pChunk) - kDataPtrOffset; // The actual size initially requested by the user might be smaller than this value.


	// See if we can write the "addr" field.
	#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
		size_t nRequiredSpace = 28;  // "addr: 0x1234567812345678\t\0" + 2
	#else
		size_t nRequiredSpace = 20;  // "addr: 0x12345678\t\0" + 2
	#endif

	if(nBufferLength >= nRequiredSpace)
	{
		// Write the "addr" field.
		#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "addr: 0x%016" PRIx64 "%c", (uint64_t)(uintptr_t)pData, mcTraceFieldDelimiter);
		#else
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "addr: 0x%08x%c", (unsigned)(uintptr_t)pData, mcTraceFieldDelimiter);
		#endif

		PPM_ASSERT(this, nCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the size field.
	nRequiredSpace = 31; // "size: 1234567890 (12345678)\t\0" + 2

	if(nBufferLength >= nRequiredSpace)
	{
		// Write the "size" field.
		nCount = PPM_Snprintf(pBuffer, nBufferLength, "size: %10u (%8x)%c", (unsigned)nDataSize, (unsigned)nDataSize, mcTraceFieldDelimiter);
		PPM_ASSERT(this, nCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the data field.
	nRequiredSpace = 256 + 10; // We could write less data if the user had less space, but that would result in potentially inconsistent output as the user sees it.

	if(nBufferLength >= nRequiredSpace)
	{
		// Write the "data" field.
		const unsigned kDataStringCapacity = 256;
		char           dataString[kDataStringCapacity] = { 0 };

		GetDataPreview(pData, nDataSize, dataString, NULL, kDataStringCapacity);
		nCount = PPM_Snprintf(pBuffer, nBufferLength, "data: %s%c", dataString, mcTraceFieldDelimiter);
		PPM_ASSERT(this, nCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the attr field.
	if(GetChunkIsInternal(pChunk) || GetChunkIsMMapped(pChunk))
	{
		nRequiredSpace = 25; // "attr: internal mapped\t\0" + 2

		if(nBufferLength >= nRequiredSpace)
		{
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "attr: %s %s%c", GetChunkIsInternal(pChunk) ? "internal" : "", GetChunkIsMMapped(pChunk) ? "mapped" : "", mcTraceFieldDelimiter);
			PPM_ASSERT(this, nCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBuffer       += (size_t)nCount;
			nBufferLength -= (size_t)nCount;
		}
	}

	// See if we can write the space field.
	if(!GetChunkIsInUse(pChunk))
	{
		nRequiredSpace = 11; // "<space>\t\0" + 2

		if(nBufferLength >= nRequiredSpace)
		{
			// Write the "space" field.
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "<space>%c", mcTraceFieldDelimiter);
			PPM_ASSERT(this, nCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBuffer       += (size_t)nCount;
			nBufferLength -= (size_t)nCount;
		}
	}

	if(bAppendLineEnd)
	{
		// Append a trailing record delimiter.
		PPM_ASSERT(this, nBufferLength > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // The above logic is set up to always guarantee at least one free byte at the end. // If this fails, PPMalloc is at fault.
		*pBuffer++ = (char)mcTraceRecordDelimiter;
		*pBuffer   = 0;
	}

	return (size_t)(pBuffer - pBufferStart);
}



///////////////////////////////////////////////////////////////////////////////
// DescribeData
//
size_t GeneralAllocator::DescribeData(const void* pData, char* pBuffer, size_t nBufferLength)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	return DescribeChunk(GetChunkPtrFromDataPtr(pData), pBuffer, nBufferLength, true);
}



///////////////////////////////////////////////////////////////////////////////
// TraceAllocatedMemory
//
void GeneralAllocator::TraceAllocatedMemory(TraceFunction pTraceFunction, void* pTraceFunctionContext,
											void* pStorage, size_t nStorageSize, int nBlockTypeFlags)
{
	// This function is unique amongst the reporting functions in that it is not re-entrant.
	// If you specify a custom tracing function, it cannot safely call functions that
	// modify allocated memory. The reason for this limitation is that it would likely 
	// require too much backup memory to support this functionality. Similarly, this 
	// function keeps a mutex locked for the entire operation and thus external code 
	// must be careful in any mutexes it locks itself lest there be a potential for deadlock.
	// A simpler but re-entrant-safe memory trace can be obtained via the ReportHeap function
	// in snapshot-taking mode. 

	char pTraceBuffer[PPM_TRACE_BUFFER_SIZE];

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex); // We keep the mutex locked throughout this operation, even though we call externally.
	#endif

	if(GetFastBinChunksExist()) // We must clear fast bins because they are flagged as being allocated,
		ClearFastBins();

	if(!pTraceFunction)
		 pTraceFunction = mpTraceFunction;
	if(!pTraceFunctionContext)
		 pTraceFunctionContext = mpTraceFunctionContext;

	// Use the Report mechanism to iterate blocks in place.
	if(pTraceFunction)
	{
		const void* const pContext = ReportBegin(NULL, nBlockTypeFlags, false, pStorage, nStorageSize); // 'false' here means to iterate the heap in place (dynamically) instead of taking a snapshot of it.
		for(const BlockInfo* pBlockInfo = ReportNext(pContext); pBlockInfo; pBlockInfo = ReportNext(pContext))
		{
			// We have a chicken and egg problem here, as the pContext itself might likely have been allocated
			// via our own heap. But given that this function is used to trace leaked memory, we don't want to 
			// report the pContext memory as allocated (i.e. leaked) memory. So we do a check against this case.
			const Chunk* pChunk = (const Chunk*)pBlockInfo->mpBlock;

			if(mbTraceInternalMemory || !GetChunkIsInternal(pChunk))
			{
				DescribeChunk(pChunk, pTraceBuffer, sizeof(pTraceBuffer)/sizeof(pTraceBuffer[0]), true);
				pTraceFunction(pTraceBuffer, pTraceFunctionContext);
			}
		}
		ReportEnd(pContext);
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetBlockInfo
//
// void GeneralAllocator::GetBlockInfo(const void* pAddress, BlockInfo* pBlockInfo) const
// {
//     CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pAddress);
// 
//     if(pCoreBlock)
//     {
//         const Chunk* const pChunk = GetChunkPtrFromDataPtr(pAddress);
//         GetBlockInfoForChunk(pChunk, pBlockInfo, pCoreBlock);
//     }
//     else
//         memset(pBlockInfo, 0, sizeof(BlockInfo));
// }


///////////////////////////////////////////////////////////////////////////////
// ReportHeap
//
bool GeneralAllocator::ReportHeap(HeapReportFunction pHeapReportFunction, void* pContext, int nBlockTypeFlags, 
								  bool bMakeCopy, void* pStorage, size_t nStorageSize)
{
	bool bReturnValue(false);

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(pHeapReportFunction)
	{
		Snapshot* const pSnapshot = (Snapshot*)ReportBegin(NULL, nBlockTypeFlags, bMakeCopy, pStorage, nStorageSize);

		if(pSnapshot)
		{
			const BlockInfo* pBlockInfo = ReportNext(pSnapshot, nBlockTypeFlags);
			bReturnValue = true;

			while(bReturnValue && pBlockInfo)
			{
				bReturnValue = pHeapReportFunction(pBlockInfo, pContext);
				pBlockInfo   = ReportNext(pSnapshot, nBlockTypeFlags);
			}
		}

		ReportEnd(pSnapshot);
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ReportBegin
//
const void* GeneralAllocator::ReportBegin(void* pContext, int nBlockTypeFlags, bool bMakeCopy, void* pStorage, size_t nStorageSize)
{
	Snapshot* pSnapshot;

	Lock(true); // The ReportEnd function will call Lock(false).

	if(pContext)
	{
		pSnapshot = (Snapshot*)pContext;
		if(pSnapshot->mnMagicNumber != Snapshot::kSnapshotMagicNumber)
			pSnapshot = NULL;
	}
	else
	{
		pSnapshot = (Snapshot*)TakeSnapshot(nBlockTypeFlags, bMakeCopy, pStorage, nStorageSize);
		if(pSnapshot)
			pSnapshot->mbReport = true;
	}

	return pSnapshot;
}


///////////////////////////////////////////////////////////////////////////////
// ReportNext
//
const GeneralAllocator::BlockInfo* GeneralAllocator::ReportNext(const void* pContext, int nBlockTypeFlags)
{
	Snapshot* const pSnapshot = (Snapshot*)pContext;

	if(pSnapshot)
	{
		PPM_ASSERT(this, pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber, kGAErrorInvalidReportContext, kGAErrorText[kGAErrorInvalidReportContext], NULL, NULL); // If this fails, it is probably the user's fault.

		if(pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber) // If this appears to be a true snapshot...
		{
			// How the snapshot gets played back depends on pSnapshot->mbDynamic.
			if(pSnapshot->mbDynamic)
			{
				// In this case we iterate the heap as we go. The user must not modify the heap 
				// while we are doing this or else it is possible that an exception will occur.
				nBlockTypeFlags &= pSnapshot->mnBlockTypeFlags;

				NewCore:
				if((nBlockTypeFlags & kBlockTypeCore) && !pSnapshot->mbCoreBlockReported)
				{
					pSnapshot->mbCoreBlockReported = true;
					GetBlockInfoForCoreBlock(pSnapshot->mpCurrentCoreBlock, &pSnapshot->mBlockInfo[0]);

					return &pSnapshot->mBlockInfo[0];
				}

				while(pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock)
				{
					const Chunk* pFenceChunk = NULL;

					if(pSnapshot->mpCurrentCoreBlock->mnSize)
					{
						pFenceChunk = GetFenceChunk(pSnapshot->mpCurrentCoreBlock);

						if(pSnapshot->mpCurrentChunk)
						{
							pSnapshot->mpCurrentChunk = GetNextChunk(pSnapshot->mpCurrentChunk); //Go to the next Chunk in contiguous memory.
							PPM_ASSERT(this, FindCoreBlockForAddress(pSnapshot->mpCurrentChunk), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pSnapshot->mpCurrentChunk, pSnapshot->mpCurrentChunk); // If this fails, the user probably corrupted the heap or removed a core block that was in use.
						}
						else
						{
							pSnapshot->mpCurrentChunk = (Chunk*)pSnapshot->mpCurrentCoreBlock->mpCore;  // Go to the first chunk in the core block.
							PPM_ASSERT(this, !GetChunkIsFenceChunk(pSnapshot->mpCurrentChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pSnapshot->mpCurrentChunk, pSnapshot->mpCurrentChunk); // If this fails, the user probably corrupted the heap.
						}

						while((pSnapshot->mpCurrentChunk != pFenceChunk) && 
								!ChunkMatchesBlockType(pSnapshot->mpCurrentChunk, nBlockTypeFlags))
						{
							pSnapshot->mpCurrentChunk = GetNextChunk(pSnapshot->mpCurrentChunk); //Go to the next Chunk in contiguous memory.
							PPM_ASSERT(this, FindCoreBlockForAddress(pSnapshot->mpCurrentChunk), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pSnapshot->mpCurrentChunk, pSnapshot->mpCurrentChunk); // If this fails, the user probably corrupted the heap or removed a core block that was in use.
						}

						if(pSnapshot->mpCurrentChunk != pFenceChunk)
						{
							PPM_ASSERT(this, !GetChunkIsFenceChunk(pSnapshot->mpCurrentChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pSnapshot->mpCurrentChunk, pSnapshot->mpCurrentChunk); // If this fails, the user probably corrupted the heap.
							break; // We have found the next matching chunk.
						}
					}

					if(!pSnapshot->mpCurrentCoreBlock->mnSize || (pSnapshot->mpCurrentChunk == pFenceChunk)) // If we should move onto the next core block...
					{
						do{
							pSnapshot->mpCurrentCoreBlock = pSnapshot->mpCurrentCoreBlock->mpNextCoreBlock;
						} while((pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock) && !pSnapshot->mpCurrentCoreBlock->mnSize);

						pSnapshot->mbCoreBlockReported = false;
						pSnapshot->mpCurrentChunk      = NULL;
						goto NewCore;
					}
				}

				if(pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock) // If mpCurrentChunk comes from a core block...
				{
					// At this point, mpCurrentChunk has already been incremented to the chunk we want to return to the user.
					PPM_ASSERT(this, !GetChunkIsFenceChunk(pSnapshot->mpCurrentChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pSnapshot->mpCurrentChunk, pSnapshot->mpCurrentChunk); // If this fails, the user probably corrupted the heap.
					GetBlockInfoForChunk(pSnapshot->mpCurrentChunk, &pSnapshot->mBlockInfo[0], pSnapshot->mpCurrentCoreBlock->mpCore);
					return &pSnapshot->mBlockInfo[0];
				}
				#if PPM_MMAP_SUPPORTED
					else // Else we are done iterating core blocks and need to iterate mmapped chunks via mpCurrentMChunk.
					{
						if(nBlockTypeFlags & kBlockTypeAllocated) // All mmapped chunks are always allocated.
						{
							// At this point, mpCurrentChunk needs to be initialized or incremented here.
							while(pSnapshot->mpCurrentMChunk != &mHeadMMapChunk)
							{
								const Chunk* const pChunk = GetMMapChunkFromMMapListChunk(pSnapshot->mpCurrentMChunk);
								PPM_ASSERT(this, GetChunkIsMMapped(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the block/heap.
				 
								pSnapshot->mpCurrentMChunk = pSnapshot->mpCurrentMChunk->mpNextChunk;
				 
								if((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk))
								{
									GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[0], (char*)pChunk - pChunk->mnPriorSize); // MMapped chunks work a little differently. However, mnPriorSize is likely to be zero.
									return &pSnapshot->mBlockInfo[0];
								}
							}
						}
					}
				#endif
			}
			else // Else non-dynamic (i.e. recorded)
			{
				// In this case we recorded all blocks manually in a big array and are iterating that array.

				while((pSnapshot->mnBlockInfoIndex < pSnapshot->mnBlockInfoCount) && !(pSnapshot->mBlockInfo[pSnapshot->mnBlockInfoIndex].mBlockType & nBlockTypeFlags))
					++pSnapshot->mnBlockInfoIndex;

				if(pSnapshot->mnBlockInfoIndex < pSnapshot->mnBlockInfoCount)
					return (pSnapshot->mBlockInfo + pSnapshot->mnBlockInfoIndex++);
			}
		}
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// ReportEnd
//
void GeneralAllocator::ReportEnd(const void* pContext)
{
	Snapshot* const pSnapshot = (Snapshot*)pContext;

	if(pSnapshot)
	{
		if(pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber)
		{
			if(pSnapshot->mbReport)
				FreeSnapshot((void*)pContext);
			else
				pSnapshot->mnBlockInfoIndex = 0;
		}
	}

	Lock(false); // This matches the Lock(true) call from ReportBegin.
}



///////////////////////////////////////////////////////////////////////////////
size_t GeneralAllocator::PhysicalFootprint() const
{
	return mPhysicalFootprint;
}

///////////////////////////////////////////////////////////////////////////////

size_t GeneralAllocator::VirtualFootprint() const
{
	return mnMMapMallocTotal;
}

///////////////////////////////////////////////////////////////////////////////
size_t GeneralAllocator::NumCores() const
{
	return mNumCores;
}

///////////////////////////////////////////////////////////////////////////////
// Snapshot::Snapshot
//
GeneralAllocator::Snapshot::Snapshot(size_t nSizeOfThis, int nBlockTypeFlags)
{
	memset(this, 0, nSizeOfThis);

	mnMagicNumber       = kSnapshotMagicNumber; 
	mnSizeOfThis        = nSizeOfThis;
	mnBlockTypeFlags    = nBlockTypeFlags;
	mbUserAllocated     = false;
	mbReport            = false;
	mbDynamic           = false;
	mbCoreBlockReported = false;
	mpCurrentCoreBlock  = NULL;
	mpCurrentChunk      = NULL;
	mpCurrentMChunk     = NULL;
	mnBlockInfoCount    = 0; 
	mnBlockInfoIndex    = 0;
}


///////////////////////////////////////////////////////////////////////////////
// TakeSnapshot
//
void* GeneralAllocator::TakeSnapshot(int nBlockTypeFlags, bool bMakeCopy, void* pStorage, size_t nStorageSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	Snapshot* pSnapshot = NULL;

	if(bMakeCopy)
	{
		// Clear fast bins so that the snapshot doesn't change on us while we are taking it.
		if(GetFastBinChunksExist())
			ClearFastBins();

		size_t nBlockCount = 0;

		// Currently, we implement this by running through all blocks twice, once to
		// get a sum of the blocks and thus know how much snapshot space to allocate,
		// and once to actually fill the space with data.
		for(size_t nPass = 0; nPass < 2; nPass++)
		{
			size_t nBlockIndex = 0;

			if(nPass) // If this is the second time through this loop...
			{
				PPM_ASSERT(this, !GetFastBinChunksExist(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  // If this fails, PPMalloc is at fault.

				const size_t kBlockCountExtra = 4;
				nBlockCount += kBlockCountExtra; // This results in allocating more BlockInfo than needed. This is because MallocInternal below may add newly required blocks.
				const size_t nSizeOfSnapshot = sizeof(Snapshot) + (nBlockCount * sizeof(BlockInfo));

				if(pStorage && (nStorageSize < nSizeOfSnapshot))  // If the user supplied memory but it wasn't enough...
				{                                                 // Pretend the user didn't supply any pointer at all.
					pStorage = NULL;
					PPM_ASSERT(this, nStorageSize >= nSizeOfSnapshot, kGAErrorShapshotMemoryExhaustion, kGAErrorText[kGAErrorShapshotMemoryExhaustion], NULL, NULL); // If this fails, the user didn't pass enough space to store the snapshot.
				}

				void* const pData = pStorage ? pStorage : MallocInternal(nSizeOfSnapshot, kAllocationFlagInternal);
				if(pData)
				{
					if(!pStorage) // If pData was internally allocated...
						SetChunkIsInternal(GetChunkPtrFromDataPtr(pData));
					pSnapshot = new(pData) Snapshot(nSizeOfSnapshot, nBlockTypeFlags);
					pSnapshot->mnSizeOfThis     = nSizeOfSnapshot;
					pSnapshot->mbUserAllocated  = (pStorage != NULL);
					pSnapshot->mbDynamic        = false;
					pSnapshot->mnBlockInfoCount = nBlockCount;
					pSnapshot->mnBlockInfoIndex = 0;
				}
				else
					return NULL; // We cannot possibly take a shapshot.

				if(nBlockCount == kBlockCountExtra) // If there were no initial blocks...
					break; // There's no use iterating the chunks below if we know there are none of them to interest us.
			}

			// Core chunks
			CoreBlock* pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
			while(pCoreBlock != &mHeadCoreBlock)
			{
				if(nBlockTypeFlags & kBlockTypeCore)
				{
					if(nPass && (nBlockIndex < nBlockCount)) // Check index again in case something changed in the meantime.
						GetBlockInfoForCoreBlock(pCoreBlock, &pSnapshot->mBlockInfo[nBlockIndex]);
					nBlockIndex++;
				}

				if(nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) // If the user wants to read any chunks at all...
				{
					const Chunk* pChunk      = (Chunk*)pCoreBlock->mpCore;
					const Chunk* pFenceChunk = GetFenceChunk(pCoreBlock);

					// Walk all chunks in this core.
					while(pChunk < pFenceChunk)
					{
						const size_type bIsInUse = GetChunkIsInUse(pChunk);

						if(bIsInUse) 
						{
							if(nBlockTypeFlags & kBlockTypeAllocated)
							{
								if((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk))
								{
									 if(nPass && (nBlockIndex < nBlockCount)) // Check index again in case something changed in the meantime.
										 GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[nBlockIndex], pCoreBlock->mpCore);
									 nBlockIndex++;
								}
							}
						}
						else
						{
							if(nBlockTypeFlags & kBlockTypeFree)
							{
								if((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk))
								{
									 if(nPass && (nBlockIndex < nBlockCount)) // Check index again in case something changed in the meantime.
										 GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[nBlockIndex], pCoreBlock->mpCore);
									 nBlockIndex++;
								}
							}
						}

						if(!bIsInUse && (pChunk == pChunk->mpNextChunk)) // The one mpTopChunk has pointers to itself and thus we would infinitely loop if we didn't catch it.
							break;
						pChunk = GetNextChunk(pChunk);
					}
				}

				pCoreBlock = pCoreBlock->mpNextCoreBlock;
			}


			// Memory mapped chunks
			#if PPM_MMAP_SUPPORTED
				if(nBlockTypeFlags & kBlockTypeAllocated) // Any memory-mapped chunk that is in our list is always allocated. There are no such things as free memory-mapped chunks.
				{
					const Chunk* pChunk = mHeadMMapChunk.mpNextChunk;
					while(pChunk != &mHeadMMapChunk)
					{
						const Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(pChunk);
						PPM_ASSERT(this, GetChunkIsMMapped(pMMappedChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pMMappedChunk, pMMappedChunk); // If this fails, the user probably corrupted the block/heap.

						if((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk))
						{
							if(nPass && (nBlockIndex < nBlockCount)) // Check index again in case something changed in the meantime.
								GetBlockInfoForChunk(pMMappedChunk, &pSnapshot->mBlockInfo[nBlockIndex], (char*)pMMappedChunk - pChunk->mnPriorSize); // MMapped chunks work a little differently. However, mnPriorSize is likely to be zero.
							nBlockIndex++;
						}

						pChunk = pChunk->mpNextChunk;
					}
				}
			#endif

			nBlockCount = nBlockIndex; // Save this for the next time through the loop.
			if(nPass)
			{
				PPM_ASSERT(this, nBlockIndex <= pSnapshot->mnBlockInfoCount, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
				pSnapshot->mnBlockInfoCount = nBlockIndex;
			}
		}
	}
	else // else we are to make the snapshot an iterator instead of a full copy.
	{
		if(pStorage && (nStorageSize < sizeof(Snapshot)))  // If the user supplied memory but it wasn't enough...
		{                                                  // Pretend the user didn't supply any pointer at all.
			pStorage = NULL;
			PPM_ASSERT(this, nStorageSize >= sizeof(Snapshot), kGAErrorShapshotMemoryExhaustion, kGAErrorText[kGAErrorShapshotMemoryExhaustion], NULL, NULL); // If this fails, the user didn't pass enough space to store the snapshot.
		}

		const size_t nSizeOfSnapshot = sizeof(Snapshot);
		void* const  pData           = pStorage ? pStorage : MallocInternal(nSizeOfSnapshot, kAllocationFlagInternal);
 
		if(pData)
		{
			if(!pStorage) // If pData was internally allocated...
				SetChunkIsInternal(GetChunkPtrFromDataPtr(pData));
			pSnapshot = new(pData) Snapshot(nSizeOfSnapshot, nBlockTypeFlags);
			pSnapshot->mnSizeOfThis        = nSizeOfSnapshot;
			pSnapshot->mbUserAllocated     = (pStorage != NULL);
			pSnapshot->mbDynamic           = true;
			pSnapshot->mbCoreBlockReported = false;
			pSnapshot->mpCurrentCoreBlock  = mHeadCoreBlock.mpNextCoreBlock;
			pSnapshot->mpCurrentChunk      = NULL;
			pSnapshot->mpCurrentMChunk     = mHeadMMapChunk.mpNextChunk;
		}
	}

	return pSnapshot;
}



///////////////////////////////////////////////////////////////////////////////
// FreeSnapshot
//
void GeneralAllocator::FreeSnapshot(void* pContext)
{
	Snapshot* const pSnapshot = (Snapshot*)pContext;

	if(pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber) // If this snapshot looks legitimate...
	{
		pSnapshot->~Snapshot();
		if(!pSnapshot->mbUserAllocated) // If the snapshot context space wasn't user-supplied...
			FreeInternal(pSnapshot);
	}
}



///////////////////////////////////////////////////////////////////////////////
// ValidateAddress
//
const void* GeneralAllocator::ValidateAddress(const void* pAddress, int addressType) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pAddress);

	if(pCoreBlock) // If this address is in fact part of a core block (and thus is part of some chunk)...
	{
		if(addressType == kAddressTypeOwned) // If all the user wants to know is if this pointer is owned in any way by this allocator...
			return pAddress;

		// Since it is more or less theoretically impossible to look at 
		// a given bit pattern at the location pAddress and know where the
		// chunk header is that refers to the address, we have no choice
		// but to walk chunks manually till we cross the input address.
		const Chunk* pCurrentChunk = (Chunk*)pCoreBlock->mpCore;
		#ifndef PPM_DEBUG_PRESERVE_PRIOR
			const Chunk* pPrevChunk = pCurrentChunk;
		#endif
		const Chunk* pNextChunk = GetNextChunk(pCurrentChunk);

		// This loop could be traversing many megabytes of data and might be 
		// somewhat slow. This function will possibly crash if there are any 
		// heap problems. However, we have little alternative but to do this.
		// Keep in mind that it's not reading all this data but jumping 
		// forward through it in chunk sizes.
		while((pNextChunk < pAddress) && (pNextChunk != pCurrentChunk)) // Check for '!=' here in order to make sure we don't loop forever and to deal with core end fence chunks.
		{
			#ifndef PPM_DEBUG_PRESERVE_PRIOR
				pPrevChunk = pCurrentChunk;
			#endif 
			pCurrentChunk = pNextChunk;
			pNextChunk = GetNextChunk(pNextChunk);
		}

		#ifndef PPM_DEBUG_PRESERVE_PRIOR // In debug mode the mnPriorSize field of next chunks is valid, as we reserve the space for internal usage and don't give it to the user.
			if((pAddress >= pCurrentChunk) && (pAddress < ((char*)pCurrentChunk + kSizeTypeSize))) // If the address is within the 'mnPriorSize' field of pCurrentChunk...
			{
				// Recall that an allocated chunk will use the first four bytes (kSizeTypeSize)
				// of the next contiguous chunk in memory (its mnPriorSize field). Here we
				// deal with this case specifically.
				if(addressType == kAddressTypeAllocated) // We can only return non-null in this case if the user specifies kAddressTypeAllocated.
				{
					if(GetPrevChunkIsInUse(pCurrentChunk) != 0)
						return GetDataPtrFromChunkPtr(pPrevChunk);
				}
			}
			else 
		#endif

		if(GetChunkIsInUse(pCurrentChunk) && !GetChunkIsFastBin(pCurrentChunk)) // Fast bin chunks have the prev chunk in use flag set, as if they were allocated.
		{
			const void* const pData = GetDataPtrFromChunkPtr(pCurrentChunk);

			if(addressType == kAddressTypeSpecific)
			{
				if(pAddress == pData)
					return pData;
			}
			else if((pAddress >= pData) && ((Chunk*)pAddress < pNextChunk))
				return pData;
		}
	}

	#if PPM_MMAP_SUPPORTED
		for(const Chunk* pMMapChunk = mHeadMMapChunk.mpNextChunk; 
			pMMapChunk != &mHeadMMapChunk;
			pMMapChunk = pMMapChunk->mpNextChunk)
		{
			const Chunk* const pMMappedChunk = GetMMapChunkFromMMapListChunk(pMMapChunk);
			const void*  const pData         = GetDataPtrFromChunkPtr(pMMappedChunk);

			if(addressType == kAddressTypeSpecific)
			{
				if(pAddress == pData)
					return pData;
			}
			else
			{
				// This logic works for both kAddressTypeOwned and kAddressTypeAllocated.
				const size_type n = GetUsableChunkSize(pMMappedChunk);

				if((pAddress >= pData) && ((char*)pAddress < (char*)pData + n))
				{
					if(addressType == kAddressTypeOwned)
						return pAddress;
					else
						return pData;
				}
			}
		}
	#endif

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// IsAddressHigh
//
bool GeneralAllocator::IsAddressHigh(const void* pAddress)
{
	if(mpHighFence)
	{
		for(CoreBlock* pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
				pCoreBlock != &mHeadCoreBlock;
				pCoreBlock = pCoreBlock->mpNextCoreBlock)
		{
			if((pAddress >= (void*)pCoreBlock->mpCore) && // If the address belongs to this core block...
			   (pAddress <  (void*)(pCoreBlock->mpCore + pCoreBlock->mnSize))) 
			{
				return (pAddress > mpHighFence);
			}
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// ValidateHeap
//
// Does a heap validation, with optional levels of detail.
//
bool GeneralAllocator::ValidateHeap(HeapValidationLevel heapValidationLevel)
{
	int nErrorCount = 0;

	if(!mbHeapValidationActive) // Check for recursion
	{
		mbHeapValidationActive = true;
		nErrorCount += CheckState(heapValidationLevel);
		mbHeapValidationActive = false;
	}
	return (nErrorCount == 0);
}



///////////////////////////////////////////////////////////////////////////////
// SetAutoHeapValidation
//
void GeneralAllocator::SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency)
{
	mnAutoHeapValidationFrequency  = ((nFrequency > 0) ? nFrequency : 1);
	mAutoHeapValidationLevel       = heapValidationLevel;
	mnAutoHeapValidationEventCount = 0;
}



///////////////////////////////////////////////////////////////////////////////
// Malloc
//
void* GeneralAllocator::Malloc(size_t nSize, int nAllocationFlags)
{
	void* pReturnValue;

	// There is an option to lock mpMutex during this entire function execution
	// or to lock it only for the inner MallocInternal call. Locking it for 
	// the entire function duration has the benefit of synchronizing this entire
	// function, with the downside that it risks a user deadlock in the case that
	// the hook function locks some mutex of its own. The hookInfo.mId value can
	// be used to tell the order of malloc calls in the case that locking is not
	// done during the hook function call.
	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMalloc;
			hookInfo.mnSizeInputTotal   = nSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	pReturnValue = MallocInternal(nSize, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocInternal
//
// Algorithm:
//     Check FastBins.
//     Check Small-sized exact fit bins.
//     Check unsorted cache bin.
//         Clear unsorted cache bin if unsuccessful.
//     Check Large-sized sorted bins.
//     Check top-most (final) chunk.
//     Call SysMalloc.
// 
// Notes:
//    Almost all the function calls within this function are actually small 
//    inline worker functions made up of only a line or two of code. In other
//    variations of this algorithm written in C these same kinds of calls are
//    implemented as (rather tedious to debug/trace) macros.
//
void* GeneralAllocator::MallocInternal(size_t nSize, int nAllocationFlags)
{
	void*           pData;                    // Resulting data from resulting chunk.
	size_type       nRequestedChunkSize = 0;  // Resulting requested chunk size. Includes Chunk overhead.
	Chunk*          pResultChunk;             // Chunk for returned data ptr.
	size_type       nResultChunkSize;         // Size of result chunk.
  //int             nResultBinIndex;          // Bin index of result.
	Chunk*          pRemainderChunk;          // Chunk left after split, if split occurs.
	size_type       nRemainderChunkSize;      // Size of remainder chunk, if any.
	SkipListChunk*  pBin;                     // Bin temp.
	int             nBinIndex = 0;            // Bin index temp.
  //Chunk*          pNextChunk;               // Temp
	Chunk*          pPrevChunk;               // Temp
	unsigned        nBinBitmapWordIndex;      // Current word index.
	unsigned        nBinBitmapBitIndex;       // Current bit index of mBinBitmap word.
	unsigned        nBinBitmapWordValue;      // Current word of mBinBitmap.
	unsigned        nMallocFailureCount = 0;  // Specifies how many times to try allocating before giving up.


	#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
		if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
		{
			ValidateHeap(mAutoHeapValidationLevel);
			mnAutoHeapValidationEventCount = 0;
		}
	#endif

	#if (PPM_DEBUG >= 3) // Developer builds only.
		// Assign known invalid value to 'pData'. We use this below to verify that 
		// in fact we didn't need to assign pData to anything at the top of this function.
		pData = (void*)1;
	#else
		pData = NULL;
	#endif

	// We are testing disabling of the following code. Disabled because the GetChunkSizeFromDataSize call below will do this for us. This code and comment will be deleted in a future version, but remains here for the record.
	#if PPM_DEBUG_PRESERVE_PRIOR
		// Add extra space to size allocation so that the user's space doesn't overrun
		// the next contiguous chunk's 'prev size' field. This allows us to rely on the 
		// prev size field while debugging.
		nSize += kSizeTypeSize;
	#endif

	// Convert request size to chunk size.
	if(!RequestIsWithinRange(nSize))
	{
		PPM_ASSERT(this, false, kGAErrorRequestedSizeTooLarge, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, it is probably the user's fault. The allocation size is too big.
		return NULL;
	}

	#if PPM_MMAP_SUPPORTED
		if(nAllocationFlags & kAllocationFlagMMap)
		{
			// Note that if the kAllocationFlagMMap is specified then this function unilaterally
			// uses mapped memory and fails the allocation if mapped memory doesn't suffice. This is
			// by design, as when a user specifically requests mapped memory, it is for good reason.
			return MMapMalloc(nSize);
		}
	#endif

	MallocInternalTop:

	const int bHigh   = (nAllocationFlags & kAllocationFlagHigh);
  //const int bEndFit = (nAllocationFlags & kAllocationFlagEndFit); // This feature is not yet enabled. Implementing it is a little bit of a pain and so we hold off for now. To implement it we basically search not just for chunks that are high but search for the highest chunk possible. Note that the algorithm here already will try to use free space in the high range before carving new space from the top chunk. So we may actually never need to implement this.

	nRequestedChunkSize = GetChunkSizeFromDataSize((size_type)nSize); // = GetChunkSizeFromDataSizeChecked((size_type)nSize); //Enable checked version if we are worried about huge allocation wraparound.

	if((nAllocationFlags & kAllocationFlagUseTopChunk) == 0)
	{
		#if PPM_FASTBINS_ENABLED
			// If we can serve the request from a FastBin, do so.
			if((nRequestedChunkSize <= mnMaxFastBinChunkSize) && !bHigh)
			{
				// We never store high chunks in fast bins, as high chunks are meant to be 
				// somewhat permanent. This allows use to optimize our speed for the 90% case.

				Chunk** const ppFastBin = mpFastBinArray + GetFastBinIndexFromChunkSize(nRequestedChunkSize);
				pResultChunk = *ppFastBin;
				if(pResultChunk) // The fastbins are a singly-linked list with the final entry being NULL. If the fast bin is empty, its chunk pointer is thus simply NULL.
				{
					pResultChunk->mnSize &= ~(size_type)(kChunkFlagInternal | kChunkFlagFastBin); // ClearChunkIsInternal(pResultChunk); ClearChunkIsFastBin(pResultChunk);
					*ppFastBin = pResultChunk->mpNextChunk;
					DebugCheckRemallocedChunk(pResultChunk, nRequestedChunkSize); // Defined away unless debugging enabled.
					pData = GetDataPtrFromChunkPtr(pResultChunk);
					PPM_ASSERT(this, (GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.
					return pData;
				}
			}
		#endif

		// Go through the small bins.
		// If the request is small enough (< kLargeBinMinChunkSize), then try using the 
		// smaller bins from mpBinArray. Each of the smaller sized bins hold one size     
		// each and the bin members are thus not sorted and there is no best fit algo need.
		if(SizeIsWithinSmallBinRange(nRequestedChunkSize))
		{
			nBinIndex = GetSmallBinIndexFromChunkSize(nRequestedChunkSize);
			pBin      = GetBin(nBinIndex);

			pResultChunk = GetLastChunkInBin(pBin);
			if(pResultChunk != pBin) // If the bin was not empty...
			{
				// If pResultChunk is NULL, then we have not been initialized yet.
				// If you are having a problem whereby you are calling this allocator
				// before it is initialized, then you need to make sure this allocator
				// gets initialized first. See the documentation for more on this.
				PPM_ASSERT(this, pResultChunk, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL); // If this fails, it is probably the user's fault.

				if(ChunkMatchesLowHigh(bHigh, pResultChunk, nRequestedChunkSize))
				{
					SetChunkInUseOffset(pResultChunk, (ssize_type)nRequestedChunkSize); // Mark this chunk as used. Recall that a chunk is marked as used by setting the 'prev in use' flag of the next chunk in memory.
					UnlinkChunkFromBin(pResultChunk); // Remove from small bin.

					DebugCheckMallocedChunk(pResultChunk, nRequestedChunkSize, true, false); // Defined away unless debugging enabled.
					pData = GetDataPtrFromChunkPtr(pResultChunk);
					PPM_ASSERT(this, (GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.
					return pData;
				}
			}
		}
		else
		{
			// If this is a large request, consolidate mpFastBinArray before continuing.
			// While it might look excessive to kill all fast bin entries before
			// even seeing if there is space available, this avoids
			// fragmentation problems normally associated with fast bins.
			// Also, in practice, programs tend to have runs of either small or
			// large requests, but less often mixtures, so consolidation is not
			// invoked all that often in most programs. And the programs that
			// it is called frequently in otherwise tend to fragment anyway.

			nBinIndex = GetLargeBinIndexFromChunkSize(nRequestedChunkSize); // We'll use the bin index below.
			if(GetFastBinChunksExist())
				ClearFastBins();
		}
	}

	// Process recently freed or remaindered chunks, taking one only if
	// it is exact fit, or, if this a small request, the chunk is remainder from
	// the most recent non-exact fit.  Place other traversed chunks in
	// bins.  Note that this step is the only place in any routine where
	// chunks are placed in bins.

	// The outer loop here is needed because we might not realize until
	// near the end of malloc that we should have consolidated, so must
	// do so and retry. This happens at most once, and only when we would
	// otherwise need to expand memory to service a "small" request.

	SkipListChunk* const pUnsortedBin = GetUnsortedBin(); // GetUnsortedBin() compiles down to a constant memory address retrieval.

	for(;;)
	{
		if((nAllocationFlags & kAllocationFlagUseTopChunk) == 0)
		{
			// Go through the unsorted bin.
			while((pResultChunk = pUnsortedBin->mpPrevChunk) != pUnsortedBin)
			{
				nResultChunkSize = GetChunkSize(pResultChunk);
				pPrevChunk       = pResultChunk->mpPrevChunk;

				const bool bFlagsMatch = ChunkMatchesLowHigh(bHigh, pResultChunk, nResultChunkSize);

				// If a small request, try to use last remainder if it is the only 
				// chunk in unsorted bin. This helps promote locality for runs of 
				// consecutive small requests. This is the only exception to best-fit, 
				// and applies only when there is no exact fit for a small chunk.

				if(SizeIsWithinSmallBinRange(nRequestedChunkSize) &&  // SizeIsWithinSmallBinRange() is a quick inline int comparison.
					pResultChunk == mpLastRemainderChunk &&
					pPrevChunk   == pUnsortedBin &&
					nResultChunkSize > (nRequestedChunkSize + kMinAllocSize) &&
					bFlagsMatch
					) // If there is enough space to split the chunk into two chunks.
				{
					SplitChunkTail(pResultChunk, true, nResultChunkSize, nRequestedChunkSize); // true means it's currently in a bin, false means it's not in a large bin.

					pData = GetDataPtrFromChunkPtr(pResultChunk);
					return pData;
				}

				// Remove pResultChunk from the unsorted bin. We will either use it or we 
				// will put it into a sorted bin ourselves before going to the sorted bins.
				pUnsortedBin->mpPrevChunk = pPrevChunk;

				// Note that if you crash on this line of code, it is usually because of an earlier write after-free by
				// someone else to the first 8 bytes of that object, corrupting this free chain.  If you are really
				// lucky those first 8 bytes will be a vtable ptr and you can use that information to figure out what
				// kind of object stomped you!  Otherwise, its very hard to know who the previous owner of memory
				// was.  Try some fancy memory tracking or the stomp detector.
				pPrevChunk->mpNextChunk   = pUnsortedBin;

				// If we have an exact fit (unlikely), use this chunk now, as-is.
				if((nResultChunkSize == nRequestedChunkSize) && bFlagsMatch)
				{
					SetChunkInUseOffset(pResultChunk, (ssize_type)nResultChunkSize);

					DebugCheckMallocedChunk(pResultChunk, nRequestedChunkSize, true, false); // Defined away unless debugging enabled.
					pData = GetDataPtrFromChunkPtr(pResultChunk);
					PPM_ASSERT(this, (GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.
					return pData;
				}

				// The Chunk didn't work for us, so move it into a regular bin (small bin or large bin).
				PlaceUnsortedChunkInBin(pResultChunk, nResultChunkSize, false);

			} // GetUnsortedBin processing


			// Go through the large bins.
			// If a large request, scan through the chunks of current bin (nBinIndex) 
			// in sorted order to find smallest that fits. This is the only step
			// where an unbounded number of chunks might be scanned without doing
			// anything useful with them. However the lists tend to be short.

			if(!SizeIsWithinSmallBinRange(nRequestedChunkSize))
			{
				pBin = GetBin(nBinIndex);

				// For each bin of the bins that correspond to the size of the requested chunk...
				// We read through the bins backwardly because we want to read from smallest bin to largest bin.
				// SkipListChunk mod needed.
				for(SkipListChunk* pResultChunkS = pBin->mpLargerChunks; pResultChunkS != pBin; pResultChunkS = pResultChunkS->mpLargerChunks)
				{
					nResultChunkSize = GetChunkSize(pResultChunkS);

					if(nResultChunkSize >= nRequestedChunkSize) // The this chunk fits the requested chunk size...
					{
						if(ChunkMatchesLowHigh(bHigh, pResultChunkS, nResultChunkSize))
						{
							SplitChunkTail(pResultChunkS, true, nResultChunkSize, nRequestedChunkSize); // first true means it's currently in a bin, second true means it's in a large bin.

							pData = GetDataPtrFromChunkPtr(pResultChunkS);
							return pData;
						}
					}

					#if PPM_DEBUG >= 2
						PPM_ASSERT(this, pResultChunkS->mpLargerChunks != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault, though a rare kind of heap corruption could cause this.
					#endif
				} // for each chunk in the ideal bin...
			}

			// The ideal bin check above wasn't successful. Thus, we need to start checking larger bins.
			// Search for a chunk by scanning bins, starting with next largest bin. This search is strictly 
			// by best-fit; i.e., the smallest (with ties going to approximately the least recently used) 
			// chunk that fits is selected.
			//
			// The bitmap avoids needing to check that most blocks are non-empty. The particular case of 
			// skipping all bins during warm-up phases when no chunks have been returned yet is faster 
			// than it might look.

			nBinIndex++;
			pBin = GetBin(nBinIndex);
			nBinBitmapWordIndex = (unsigned)GetBinBitmapWordFromBinIndex(nBinIndex);
			nBinBitmapWordValue = mBinBitmap[nBinBitmapWordIndex];
			nBinBitmapBitIndex  = (unsigned)GetBinBitmapBitFromBinIndex(nBinIndex);

			for(;;)
			{
				if((nBinBitmapBitIndex > nBinBitmapWordValue) || (nBinBitmapBitIndex == 0)) // If there are no more bits set in this nBinBitmapWordIndex...
				{
					do
					{
						if(++nBinBitmapWordIndex >= kBinBitmapWordCount) // If we are completely out of bins...
							goto UseTopChunk;
					} while((nBinBitmapWordValue = mBinBitmap[nBinBitmapWordIndex]) == 0);

					pBin = GetBin((int)(nBinBitmapWordIndex << kBinBitmapShift)); // Go to the bin found directly above.
					nBinBitmapBitIndex = 1;
				}

				// Advance to next bin with set bit; there will always be one.
				while((nBinBitmapBitIndex & nBinBitmapWordValue) == 0)
				{
					pBin = GetNextBin(pBin);
					nBinBitmapBitIndex <<= 1;
					PPM_ASSERT(this, nBinBitmapBitIndex != 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault, though a rare kind of heap corruption could cause this.
				}

				// Inspect the bin. It is likely to be non-empty.
				pResultChunk = GetLastChunkInBin(pBin);

				if(pResultChunk == pBin) // If the bin is empty, clear the bin bitmap bit.
				{
					mBinBitmap[nBinBitmapWordIndex] = (nBinBitmapWordValue &= ~nBinBitmapBitIndex);
					pBin = GetNextBin(pBin);
					nBinBitmapBitIndex <<= 1;
					// Fall through and execute loop again.
				}
				else
				{
					// This chunk will definitely work for us (unless we have any special allocation flags set).
					nResultChunkSize = GetChunkSize(pResultChunk);

					// If the user is requesting high chunks while there are many free chunks in these bins, allocation
					// time will increase and go from go from O(1) to more like O(n) where n is the number of free
					// blocks. If this loop is spending a lot of time, is it possible that there are many free high
					// memory chunks and the ChunkMatchesLowHigh is not matching any of them?  If so then the question
					// to be asked is: why are there so many high (a.k.a. permanent) blocks when these blocks are
					// supposed to be permanent and not commonly free?

					///////////////////////////////////////////////////////////////////////////////////////////////////////
					// TODO:  Consider making the allocation of high/low memory chunks fast by preventing the list scan.  
					//
					// This code potentially scans a long list evaluting if each block matches the requested memory
					// region (high or low).  A potential solution would be to maintain pointers to the high and low
					// blocks of memory in the sorted list to enable skipping large portions of the list.
					//
					// [ HIGH REGION sorted by chunk size ] [ LOW REGION sorted by chunk size ]
					// ^                                    ^
					// |                                    |
					// high_ptr                             low_ptr
					//
					while((pResultChunk != pBin) && !ChunkMatchesLowHigh(bHigh, pResultChunk, nResultChunkSize))
					{
						pResultChunk     = pResultChunk->mpPrevChunk;
						nResultChunkSize = GetChunkSize(pResultChunk);
					}

					if(pResultChunk == pBin) // If this bin couldn't service the request... move onto the next bin (without clearing the existing bin).
					{
						pBin = GetNextBin(pBin);
						nBinBitmapBitIndex <<= 1;
					}
					else
					{
						// We know the first chunk in this bin is big enough to use.
						SplitChunkTail(pResultChunk, true, nResultChunkSize, nRequestedChunkSize); // true means it's currently in a bin.

						pData = GetDataPtrFromChunkPtr(pResultChunk);
						return pData;
					}
				} // <processing of a specific bin>
			} // for(;;)
		}

		UseTopChunk: // The above checks failed, so use the top-most chunk.
		// If large enough, split off the chunk bordering the end of memory
		// (held in top). Note that this is in accord with the best-fit
		// search rule.  In effect, top is treated as larger (and thus
		// less well fitting) than any other available chunk since it can
		// be extended to be as large as necessary (up to system
		// limitations).

		// We require that top always exists (i.e. nResultChunkSize >=
		// kMinAllocSize) after initialization, so if it would otherwise be
		// exhuasted by current request, it is replenished. The main reason 
		// for ensuring it exists is that we may need kMinAllocSize space
		// to put in fenceposts in SystemAlloc.

		pResultChunk     = mpTopChunk;
		nResultChunkSize = GetChunkSize(mpTopChunk);

		if(nResultChunkSize >= (nRequestedChunkSize + kMinAllocSize)) // If the top chunk has enough for the requested allocation...
		{
			if(bHigh) // If high (permanent) memory is requested...
			{
				// Get the data from the top of the chunk instead of the bottom.
				// In this case the remainder chunk is the lower chunk and the result
				// chunk is the upper chunk. Note also that the remainder chunk will
				// be the new mpTopChunk.
				pRemainderChunk = mpTopChunk;
				pResultChunk    = (Chunk*)(((uintptr_t)pRemainderChunk + nResultChunkSize) - nRequestedChunkSize);

				if(PPM_IsAligned(pResultChunk, kMinEffectiveAlignment))
					nRemainderChunkSize = nResultChunkSize - nRequestedChunkSize;
				else
				{
					PPM_CT_ASSERT(sizeof(uintptr_t) >= sizeof(void*)); // If this fails, PPMalloc is at fault.
					pResultChunk        = (Chunk*)((uintptr_t)pResultChunk & (uintptr_t)~(kMinEffectiveAlignment - 1)); // Align downward to kMinEffectiveAlignment.
					nRemainderChunkSize = (size_type)((char*)pResultChunk - (char*)pRemainderChunk);
					nRequestedChunkSize = nResultChunkSize - nRemainderChunkSize;
				}
				PPM_ASSERT(this, ((char*)pRemainderChunk + kMinChunkSize) <= (char*)pResultChunk, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pRemainderChunk, pRemainderChunk); // Assert that this was possible without screwing up the lower chunk. // If this fails, the user probably corrupted the heap.

				SetChunkSize(pResultChunk, nRequestedChunkSize); // Make sure kChunkFlagPrevInUse is off, because the remainder will not be in use.
				#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
					// pResultChunk->mnPriorSize = nRemainderChunkSize;    Not necessary because AdjustTopChunk below will take care of this.
				#endif
				// SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);    Not necessary because AdjustTopChunk below will take care of this.

				Chunk* const pNextChunk1 = GetChunkAtOffset(pResultChunk, (ssize_type)nRequestedChunkSize); // This chunk may or may not be the top fencepost chunk for
				pNextChunk1->mnPriorSize = nRequestedChunkSize;                                             // the core block, depending on if any other chunks have already
				pNextChunk1->mnSize     |= kChunkFlagPrevInUse;                                             // been allocated up here.
			}
			else
			{
				nRemainderChunkSize = nResultChunkSize - nRequestedChunkSize;
				SetChunkSize(pResultChunk, nRequestedChunkSize | kChunkFlagPrevInUse);
				#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
					SetNextChunkPrevSize(pResultChunk, nRequestedChunkSize);
				#endif

				pRemainderChunk = GetChunkAtOffset(pResultChunk, (ssize_type)nRequestedChunkSize);
				SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse); // Not necessary because AdjustTopChunk below will take care of this.
			}

			#if PPM_DEBUG_FILL
				// DebugMemFill(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), mnFillFree, nRemainderChunkSize - sizeof(Chunk)); // Should already be mem-filled.
				PPM_ASSERT(this, DebugMemCheck(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), nRemainderChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pRemainderChunk, pRemainderChunk); // If this fails, the user probably corrupted the heap.
			#endif

			mpTopChunk = pRemainderChunk;
			AdjustTopChunk(mpTopChunk, nRemainderChunkSize); // This sets its size, puts it in standalone self-list, etc.

			DebugCheckMallocedChunk(pResultChunk, nRequestedChunkSize, true, (bHigh != 0)); // Defined away unless debugging enabled.

			pData = GetDataPtrFromChunkPtr(pResultChunk);
			PPM_ASSERT(this, (GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.
			return pData;
		}

		// If there is space available in mpFastBinArray, consolidate and retry,
		// to possibly avoid expanding memory. This can occur only if nRequestedChunkSize is
		// in smallbin range so we didn't consolidate upon entry.
		else if(GetFastBinChunksExist())
		{
			PPM_ASSERT(this, SizeIsWithinSmallBinRange(nRequestedChunkSize), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL); // If this fails, the user probably corrupted the heap. There may be a bug in PPMalloc, however.
			ClearFastBins();
			nBinIndex = GetSmallBinIndexFromChunkSize(nRequestedChunkSize); // Restore original bin index
			// Now fall through and try again.
		}
		else if(nAllocationFlags & kAllocationFlagUseTopChunk)
		{
			// We have tried to force allocating from the top chunk, but failed. 
			// So we retry the allocation from any source.
			nAllocationFlags &= ~kAllocationFlagUseTopChunk;
			goto MallocInternalTop;
		}
		else 
		{
			// Try using mapped memory if the size is in the right range.
			#if PPM_MMAP_SUPPORTED
				if(mbSystemAllocEnabled && (nRequestedChunkSize >= mnMMapThreshold) && (mnMMapCount < mnMMapMaxAllowed)) 
				{
					pData = MMapMalloc(nSize);
					if(pData)
						return pData;
				}
				// Fall through to the system alloc below.
			#endif
 
			// If the request was for high memory but there isn't any left, we attempt to allocate from 
			// low (i.e. normal) memory. Alternatively, if the request was for low memory but there wasn't any left, 
			// we attempt to allocate from high memory.Only if that fails will be attempt to allocate new core. 
			if(!(nAllocationFlags & kAllocationFlagRetry) && !mbHighFenceInternallyDisabled) // If we haven't tried this already...
			{
				nAllocationFlags |= kAllocationFlagRetry;

				if(bHigh) // Note that we intentionally disable the virtual function mechanism and force calling ourselves directly back.
					return GeneralAllocator::MallocInternal(nSize, nAllocationFlags & ~(kAllocationFlagHigh | kAllocationFlagEndFit));
				else
					return GeneralAllocator::MallocInternal(nSize, nAllocationFlags | kAllocationFlagHigh);
			}


			// All else failed, so use system allocation, which will attempt to retrieve new core memory from the system.
			if(mbSystemAllocEnabled)
			{
				pResultChunk = ExtendCoreInternal(nRequestedChunkSize);

				if(pResultChunk)
				{
					SetChunkIsInUse(pResultChunk);
					pData = GetDataPtrFromChunkPtr(pResultChunk);
					PPM_ASSERT(this, (GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // If this fails, the user probably corrupted the heap. There may be a bug in PPMalloc, however.
					DebugCheckMallocedChunk(pResultChunk, nRequestedChunkSize, true, false); // Defined away unless debugging enabled.
					// Fall through.
				}
			}
			break; // break out of the 'for' loop, regardless of the success of the above system allocation logic.

		} // 'use top chunk' logic

	} // for(;;)

	// pData wasn't assigned a value at its declaration, but the logic above 
	// ensures that it was set before we got here. There is some debug
	// logic above that sets pData to 1, so in a debug build we deal
	// with that. This debug logic is to verify the mechanism of this function.
	#if (PPM_DEBUG >= 3) // Developer builds only.
		PPM_ASSERT(this, (pData == (void*)1) || FindCoreBlockForAddress(pData) || GetChunkIsMMapped(GetChunkPtrFromDataPtr(pData)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails it could be PPMalloc bug or the user could have corrupted the heap.
		if(pData == (void*)1)
			pData = NULL;
	#endif

	if(!pData)
	{
		if(mpMallocFailureFunction && (++nMallocFailureCount < mnMaxMallocFailureCount))
		{
			size_type allocatorRequiredSize = nSize + (4 * kMinChunkSize);
			allocatorRequiredSize = (size_type)PPM_AlignUp(allocatorRequiredSize, mnPageSize);

			const bool bResult = CallMallocFailureFunction(this, nSize, allocatorRequiredSize, mpMallocFailureFunctionContext);

			if(bResult)
				goto MallocInternalTop; // Try again.
		}

		// This function has failed to allocate memory after all attempts.
		#if PPM_ASSERT_ENABLED
			if(mbMallocFailureAssertEnabled)
				PPM_ASSERT(this, pData != NULL, kGAErrorMemoryExhaustion, kGAErrorText[kGAErrorMemoryExhaustion], NULL, NULL);
		#endif
	}

	// Assert that we are returning NULL or that the returned block has at least the capacity the user requested.
	// We do not unilaterally assert we successfully return a non-NULL pointer, as regular malloc doesn't do that. See kOptionEnableMallocFailureAssert.
	PPM_ASSERT(this, !pData || ((GetChunkSize(pResultChunk) <= (nSize + 32)) && (GetChunkSize(pResultChunk) > nSize)), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pResultChunk, pResultChunk); // If this fails, the user probably corrupted the heap.
	return pData;

} // GeneralAllocator::MallocInternal


///////////////////////////////////////////////////////////////////////////////
// PlaceUnsortedChunkInBin
//
void GeneralAllocator::PlaceUnsortedChunkInBin(Chunk* pChunk, size_type nChunkSize, bool bRemoveFromUnsortedBin)
{
	SkipListChunk* pNextChunk;                       // Temp
	SkipListChunk* pPrevChunk;                       // Temp
	SkipListChunk* pChunkS = (SkipListChunk*)pChunk; // Henceforth treat pChunk as a SkipListChunk.
	int            nResultBinIndex;                  // Bin index for pChunk.

	#if (PPM_DEBUG >= 2)
		CheckChunkRegularBin(pChunk);
	#endif

	if(bRemoveFromUnsortedBin)
	{
		pPrevChunk              = (SkipListChunk*)pChunkS->mpPrevChunk;
		pNextChunk              = GetUnsortedBin();
		pNextChunk->mpPrevChunk = pPrevChunk;
		pPrevChunk->mpNextChunk = pNextChunk;
	}

	// The chunk wasn't needed by us, so we move it from the unsorted bin into our regular bins.
	if(SizeIsWithinSmallBinRange(nChunkSize))
	{
		// To consider: Place pChunkS in the smallbin in sorted order so that future
		// requests for high memory that come from these bins go to highest memory first.

		// These smaller bins are not sorted, so we simply put at end of list.
		nResultBinIndex = GetSmallBinIndexFromChunkSize(nChunkSize);
		pPrevChunk      = GetBin(nResultBinIndex);
		pNextChunk      = (SkipListChunk*)pPrevChunk->mpNextChunk; // We'll use these values below.
	}
	else
	{
		SetChunkIsLargeBin(pChunk);

		// These larger bins are sorted, so we have more work to do. 
		nResultBinIndex = GetLargeBinIndexFromChunkSize(nChunkSize);
		pPrevChunk      = GetBin(nResultBinIndex);
		pNextChunk      = (SkipListChunk*)pPrevChunk->mpNextChunk; // We'll use these values below, but we might modify them in the meantime.

		// To consider: Make the code here intelligently decide which side of the list to start 
		// from based on nChunkSize, for a small performance improvement. If nChunkSize is small
		// then start from the back of the bin and iterate toward the front. 
		// SkipListChunk mod needed.
		if(pPrevChunk == pNextChunk)  // If the bin is empty...
		{
			// Set our chunk to be the only skip list run in the bin.
			pChunkS->mpLargerChunks         = pPrevChunk;
			pChunkS->mpSmallerChunks        = pPrevChunk;
			pPrevChunk->mpLargerChunks      = pChunkS;
			pPrevChunk->mpSmallerChunks     = pChunkS;
		}
		else // Else the bin is non-empty... walk the list from largest to smallest (left to right / begin to end), finding the appropriate insertion point.
		{               
			if(nChunkSize < GetChunkSize(pPrevChunk->mpPrevChunk)) // If smaller than smallest chunk currently in the bin...place it at the end of the bin.
			{
				pNextChunk = pPrevChunk;                                // pNextChunk now points to the bin head chunk.
				pPrevChunk = (SkipListChunk*)pPrevChunk->mpPrevChunk;   // pPrevChunk now points to what was previously the last chunk in the bin.

				// We need to find the first chunk in the skip-list run that pPrevChunk is part of. 
				// For a run of same-sized chunks in the bin, only the first chunk in the run has valid mpLargerChunks/mpSmallerChunks pointers.
				// Since we are working from the back of the list instead of the front, and since pPrevChunk might be the last in a run, we need
				// to walk backwards from pPrevChunk to find the first Chunk in its run (which might be itself for a run of 1).
				
				SkipListChunk*  pFirstChunkInRun = pPrevChunk;  // It's not the first chunk in the run right here as a result of this assigment, but will be so below.
				const size_type size             = GetChunkSize(pPrevChunk);

				while((pFirstChunkInRun->mpPrevChunk != pNextChunk) && GetChunkSize(pFirstChunkInRun->mpPrevChunk) == size) // While the previous one isn't the head and while it is the same size as the current...
					pFirstChunkInRun = (SkipListChunk*)pFirstChunkInRun->mpPrevChunk;

				// Now pFirstChunkInRun points to the first chunk in the run of same sizes.
				pChunkS->mpSmallerChunks          = pNextChunk;         // pNextChunk == pFirstChunkInRun->mpSmallerChunks.
				pChunkS->mpLargerChunks           = pFirstChunkInRun;
				pFirstChunkInRun->mpSmallerChunks = pChunkS;  
				pNextChunk->mpLargerChunks        = pChunkS;  
			}
			else // if(nChunkSize >= kMinSortedBinChunkSize) // Note by Paul P: I'm not sure this check is necessary. It seems to me that it should always be true.
			{
				// We walk the bin from left to right (front to back, largest to smallest), finding the proper insertion location.
				size_type nextChunkSize = GetChunkSize(pNextChunk);

				while(nChunkSize < nextChunkSize) // pChunk will never be the last skip-list segment, because we already tested for that above. So this loop will always exit before hitting the end of the bin.
				{
					pNextChunk    = pNextChunk->mpSmallerChunks;
					nextChunkSize = GetChunkSize(pNextChunk);
				}

				// At this point, our chunk (pChunkS/nChunkSize) might be the same size as pNextChunk or might be larger than it.
				// This matters because we need to maintain the skip list and it affects what we need to do to maintain it.
				if(nChunkSize == nextChunkSize) // If our chunk is going to be the new first chunk in the run that pNextChunk currently is the first of...
				{
					pChunkS->mpLargerChunks                  = pNextChunk->mpLargerChunks;
					pChunkS->mpSmallerChunks                 = pNextChunk->mpSmallerChunks;
					pChunkS->mpLargerChunks->mpSmallerChunks = pChunkS;
					pChunkS->mpSmallerChunks->mpLargerChunks = pChunkS;

					#if (PPM_DEBUG >= 2)    // In debug builds we clear the previous run head chunk skip list links, but we don't need to do it in a release build.
						pNextChunk->mpLargerChunks  = NULL;
						pNextChunk->mpSmallerChunks = NULL;
					#endif
				}
				else // Else our chunk will be the only chunk in its own run of one.
				{
					pChunkS->mpLargerChunks                   = pNextChunk->mpLargerChunks;
					pChunkS->mpSmallerChunks                  = pNextChunk;
					pChunkS->mpLargerChunks->mpSmallerChunks  = pChunkS;
					pNextChunk->mpLargerChunks                = pChunkS;
				}

				pPrevChunk = (SkipListChunk*)pNextChunk->mpPrevChunk;
			}
		}
	}

	MarkBinBitmapBit(nResultBinIndex); // We are adding to this bin, so set a bit telling us about it.

	pChunk->mpPrevChunk     = pPrevChunk;
	pChunk->mpNextChunk     = pNextChunk;
	pNextChunk->mpPrevChunk = pChunk;
	pPrevChunk->mpNextChunk = pChunk;

	#if (PPM_DEBUG >= 2)
		CheckChunkRegularBin(pChunk);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetAlignedChunkPositionChecked
//
void* GeneralAllocator::GetAlignedChunkPositionChecked(Chunk* pChunk, size_type nSize, size_t nAlignment, 
																size_t nAlignmentOffset, void*& pHighPosition)
{
	// This function takes a Chunk and returns the position of the lowest 
	// sub-chunk which is aligned to nAlignment/nAlignmentOffset. It may 
	// possibly return pChunk itself. To succeed, the returned Chunk must 
	// have the following properties:
	//    -It must fit entirely within pChunk.
	//    -If must leave leading and trailing space that is either zero
	//     in size or large enough to form a new Chunk (i.e. >= Chunk size)
	// This function also calculates the highest aligned position as well.
	// This function assumes that the size of pChunk >= nSize.
	// This function assumes that nAlignment >= kMinEffectiveAlignment.
	// Note that this function does not return a valid Chunk. It returns 
	// where a valid Chunk could be made within pChunk.
	const size_type nChunkSize = GetChunkSize(pChunk);

	// pLowChunk is guaranteed to be at a location >= pChunk.
	Chunk* pLowChunk = (Chunk*)(intptr_t)(((((intptr_t)pChunk + kDataPtrOffset + nAlignmentOffset + (nAlignment - 1))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));

	if(pLowChunk != pChunk)  // If it doesn't right as it is...
	{
		if(((uintptr_t)pLowChunk - (uintptr_t)pChunk) < kMinChunkSize) // If the Chunk leaves too small of a leading space...
		{
			// We find the next lowest position that satisfies the alignment requirements.  
			// Note that our calculation here is the same as above except that we add 
			// kMinChunkSize to the pChunk position. If this next-lowest position causes
			// pLowChunk to extend past pChunk, we'll catch the case below.
			pLowChunk = (Chunk*)(intptr_t)(((((intptr_t)pChunk + kMinChunkSize + kDataPtrOffset + nAlignmentOffset + (nAlignment - 1))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));
		}

		// Here we see that pResultChunk fits into pChunk with zero free space at the end 
		// or with enough free space that we can create a new Chunk with what remains.
		const intptr_t trailingSpace = ((intptr_t)pChunk + (intptr_t)nChunkSize) - ((intptr_t)pLowChunk + (intptr_t)nSize);

		if((trailingSpace != 0) && (trailingSpace < (intptr_t)kMinChunkSize))
			pLowChunk = NULL; // It doesn't fit, or it doesn't leave room on either side of itself for split off Chunk(s).
	}

	if(pLowChunk)
	{
		// Validate that pLowChunk is aligned as intended.
		PPM_ASSERT(this, PPM_IsAligned((char*)GetDataPtrFromChunkPtr(pLowChunk) + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

		// Now find pHighChunk. We know there is a valid pHighChunk, because in the 
		// worst case it is the same as pLowChunk.
		Chunk* pHighChunk = (Chunk*)(intptr_t)(((((intptr_t)pChunk + nChunkSize - nSize + (kDataPtrOffset + nAlignmentOffset))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));

		// Here we see that pResultChunk fits into pChunk with zero free space at the end 
		// or with enough free space that we can create a new Chunk with what remains.
		// const intptr_t trailingSpace = ((intptr_t)pChunk + (intptr_t)nChunkSize) - ((intptr_t)pHighChunk + (intptr_t)nSize);
		// 
		// if((trailingSpace != 0) && (trailingSpace < kMinChunkSize))
		//     pHighChunk = (Chunk*)(uintptr_t)((uintptr_t)pHighChunk - nAlignment); // Since we know it must fit

		// Validate that pHighChunk is proper.
		PPM_ASSERT(this, (pHighChunk >= pLowChunk) && 
						 (((uintptr_t)pHighChunk + nSize) <= ((uintptr_t)pChunk + nChunkSize)) && 
						 PPM_IsAligned((char*)GetDataPtrFromChunkPtr(pHighChunk) + nAlignmentOffset, nAlignment), 
				   kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

		pHighPosition = pHighChunk;
	}

	return pLowChunk; 
}



///////////////////////////////////////////////////////////////////////////////
// SplitChunkAligned
//
GeneralAllocator::Chunk* GeneralAllocator::SplitChunkAligned(Chunk* pChunk, size_type nChunkSize, void* pResultPosition, 
															 size_type nRequestedChunkSize)
{
	// This function is called only if it has already been determined that 
	// pChunk can be used to produce a Chunk that is suitably aligned.
	// pResultPosition denotes the new Chunk to be returned. Space may 
	// possibly be cleaved before or after pResultPosition.

	Chunk* const pChunkAligned = (Chunk*)pResultPosition;

	PPM_ASSERT(this, (pChunkAligned >= pChunk) && 
					 ((pChunkAligned == pChunk) || (((uintptr_t)pChunkAligned - (uintptr_t)pChunk) >= kMinChunkSize)) &&
					 (((uintptr_t)pChunkAligned + (uintptr_t)nRequestedChunkSize) <= ((uintptr_t)pChunk + (uintptr_t)nChunkSize)), 
			   kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

	// Get the size of the leading area, as that's what the SplitChunkTail function wants.
	const size_type leadingSize = (size_type)(((uintptr_t)pChunkAligned + (uintptr_t)nRequestedChunkSize) - (uintptr_t)pChunk);

	// Split off the trailing area (if there's enough free space at the tail to form a new Chunk).
	// The leadingSize is the space between pChunk and position to (possibly) cleave the tail off from. This position is also the end of the returned chunk.
	SplitChunkTail(pChunk, true, nChunkSize, leadingSize);

	// Get the size of the trailing area, as that's what the SplitChunkLead function wants.
	const size_type chunkSize = GetChunkSize(pChunk);

	// Possibly there is unneeded leading space. If so, free it also.
	SplitChunkLead(pChunk, chunkSize, pChunkAligned);

	return pChunkAligned;
}


///////////////////////////////////////////////////////////////////////////////
// SplitChunkLead
//
// Takes pChunk (which is in some free bin) and (possibly) split it into
// two Chunks, one to be returned to the user and one to go back into a bin.
// The returned Chunk is for the caller to use for an allocation.
//
GeneralAllocator::Chunk* GeneralAllocator::SplitChunkLead(Chunk* pChunk, size_type nChunkSize, Chunk* pChunkNew)
{
	// As of this writing, there's never a case where pChunk could be in a bin, and so we don't 
	// have (bool bChunkIsInBin, bool bChunkIsInLargeBin) arguments like we do with SplitChunkTail.
	#if (PPM_DEBUG >= 2)
		// PPM_ASSERT(this, pChunk is not in a bin, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pChunk, pChunk); // If this fails, it's PPMalloc's fault.
	#endif

	const size_type newChunkSize        = (size_type)(((uintptr_t)pChunk + (uintptr_t)nChunkSize) - (uintptr_t)pChunkNew);
	const size_type nRemainderChunkSize = nChunkSize - newChunkSize;
	PPM_ASSERT(this, nChunkSize >= newChunkSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

	if(nRemainderChunkSize < kMinAllocSize) // If the requested chunk uses up all the current chunk's available space...
		SetChunkInUseOffset(pChunk, (ssize_type)nChunkSize);
	else
	{
		// Else the current chunk can be split. 
		Chunk* const pUnsortedBin = GetUnsortedBin();

		LinkChunk(pChunk, pUnsortedBin, pUnsortedBin->mpNextChunk);

		// Advertise as last remainder chunk.
		if(SizeIsWithinSmallBinRange(nRemainderChunkSize))
			mpLastRemainderChunk = pChunk;

		SetChunkSize(pChunkNew, newChunkSize); // Prev flag is not in use.
		#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
			SetNextChunkPrevSize(pChunkNew, newChunkSize);
		#endif
		SetChunkInUseOffset(pChunkNew, (ssize_type)newChunkSize); // Mark this chunk as used. Recall that a chunk is marked as used by setting the 'prev in use' flag of the next chunk in memory.

		SetChunkSize(pChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
		SetNextChunkPrevSize(pChunk, nRemainderChunkSize);
		#if PPM_DEBUG_FILL
			// DebugMemFill(GetPostHeaderPtrFromChunkPtr(pChunk), mnFillFree, nRemainderChunkSize - sizeof(Chunk)); // Should already be mem-filled.
			PPM_ASSERT(this, DebugMemCheck(GetPostHeaderPtrFromChunkPtr(pChunk), nRemainderChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
		#endif
	}

	DebugCheckMallocedChunk(pChunkNew, newChunkSize, true, true); // Defined away unless debugging enabled.
	PPM_ASSERT(this, (GetChunkSize(pChunkNew) <= (newChunkSize + 32)) && (GetChunkSize(pChunkNew) >= newChunkSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunkNew, pChunkNew); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.

	return pChunkNew;
}


///////////////////////////////////////////////////////////////////////////////
// SplitChunkTail
//
// Takes a given Chunk which is in some free bin and (possibly) split it into
// two Chunks, one to be returned to the user and one to go back into a bin.
//
void GeneralAllocator::SplitChunkTail(Chunk* pChunk, bool bChunkIsInBin, size_type nChunkSize, size_type nLeadingSize)
{
	if(bChunkIsInBin)
		UnlinkChunkFromBin(pChunk);

	PPM_ASSERT(this, nChunkSize >= nLeadingSize, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
	const size_type nRemainderChunkSize = nChunkSize - nLeadingSize;

	if(nRemainderChunkSize < kMinAllocSize) // If the requested chunk uses up all the current chunk's available space...
		SetChunkInUseOffset(pChunk, (ssize_type)nChunkSize);
	else
	{
		// Else the current chunk can be split, and we put the split off part into the unsorted bin.
		Chunk* const pUnsortedBin    = GetUnsortedBin();
		Chunk* pRemainderChunk       = GetChunkAtOffset(pChunk, (ssize_type)nLeadingSize);

		LinkChunk(pRemainderChunk, pUnsortedBin, pUnsortedBin->mpNextChunk);

		// Advertise as last pRemainderChunk
		if(SizeIsWithinSmallBinRange(nLeadingSize))
			mpLastRemainderChunk = pRemainderChunk;

		SetChunkSize(pChunk, nLeadingSize | kChunkFlagPrevInUse);
		#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
			SetNextChunkPrevSize(pChunk, nLeadingSize);
		#endif

		SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
		SetNextChunkPrevSize(pRemainderChunk, nRemainderChunkSize);
		#if PPM_DEBUG_FILL
			// DebugMemFill(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), mnFillFree, nRemainderChunkSize - sizeof(Chunk)); // Should already be mem-filled.
			PPM_ASSERT(this, DebugMemCheck(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), nRemainderChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pRemainderChunk, pRemainderChunk); // If this fails, the user probably corrupted the heap.
		#endif
	}

	DebugCheckMallocedChunk(pChunk, nLeadingSize, true, false); // Defined away unless debugging enabled.
	PPM_ASSERT(this, (GetChunkSize(pChunk) <= (nLeadingSize + 32)) && (GetChunkSize(pChunk) >= nLeadingSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.
}

///////////////////////////////////////////////////////////////////////////////
// Free
//
void GeneralAllocator::Free(void* pData)
{
	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeFree;
			hookInfo.mHookSubType       = kHookSubTypeNone;
			hookInfo.mnSizeInputTotal   = 0;
			hookInfo.mpDataInput        = pData;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = 0;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = 0;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	FreeInternal(pData);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry = false;
			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// FreeInternal
//
// Algorithm:
//     If chunk is small, move to directly to FastBin and return.
//     Consolidate chunk with contiguous chunks on either side of it in memory.
//         Special case exists for consolidating upward with top chunk.
//     Place chunk into special unordered cache bin unless it is now part of the top chunk.
//
void GeneralAllocator::FreeInternal(void* pData)
{
	// The following 'if' statement is removed because the C free function doesn't 
	// require such functionality and C++ delete should take care of the check itself.
	// Thus this comparison should be superfluous in shipping builds. Enabling the
	// check might mask programming errors, which we don't want to do.
	#if PPM_NULL_POINTER_FREE_ENABLED
	if(pData){
	#endif
		Chunk*      pChunk;                // Chunk to be freed.
		size_type   nChunkSize;            // Size of chunk to be freed.
		Chunk*      pNextChunkContig;      // Next contiguous chunk.
		size_type   nNextChunkSizeContig;  // Next contiguous chunk size.
		size_type   nPrevChunkSizeContig;  // Size of previous contiguous chunk.

		PPM_ASSERT(this, pData, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
		PPM_ASSERT(this, (((char*)pData <= (char*)this) || ((char*)pData >= ((char*)this + sizeof(*this)))), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pData, pData); // If this fails, it is the user's fault.

		#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
			if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
			{
				ValidateHeap(mAutoHeapValidationLevel);
				mnAutoHeapValidationEventCount = 0;
			}
		#endif

		pChunk     = GetChunkPtrFromDataPtr(pData);
		nChunkSize = GetChunkSize(pChunk);

		DebugCheckUsedChunk(pChunk); // Defined away in a release build.

		#if PPM_FASTBINS_ENABLED
			// If eligible, place chunk on a FastBin so it can be found and used quickly in malloc.
			if(nChunkSize <= mnMaxFastBinChunkSize
				#if PPM_FASTBIN_TRIM_ENABLED // If PPM_FASTBIN_TRIM_ENABLED set, don't place chunks bordering top into FastBins
					&& (GetChunkAtOffset(pChunk, (ssize_type)nChunkSize) != mpTopChunk)
				#endif
					&& ChunkMatchesLowHigh(kAllocationFlagLow, pChunk, nChunkSize) // Don't place high chunks into FastBins.
			  ) 
			{
				SetFastBinChunksExist(true);
				Chunk** ppFastBin = mpFastBinArray + GetFastBinIndexFromChunkSize(nChunkSize); // Associated fastbin for chunk.
				pChunk->mpNextChunk = *ppFastBin;
				#if (PPM_DEBUG >= 2) // Extended user debug builds.
					pChunk->mpPrevChunk = NULL; // Chunks in fast bins are only singly-linked. Assigning a prev chunk value has little purpose other than to help debugging.
				#endif
				pChunk->mnSize |= (kChunkFlagInternal | kChunkFlagFastBin); // SetChunkIsInternal(pChunk); SetChunkIsFastBin(pChunk); // We mark fast bin chunks as being internal, because they look allocated but belong to the allocator's cache.
				*ppFastBin = pChunk;
				DebugMemFill(GetPostHeaderPtrFromChunkPtr(pChunk), mnFillFree, nChunkSize - sizeof(Chunk));
			}
			else 
		#endif

		if(!GetChunkIsMMapped(pChunk)) // Consolidate other non-mmapped chunks as they arrive.
		{
			// There is no special processing for high chunks, as we really do little more here than 
			// coalesce and place the result in the unsorted bin.

			pNextChunkContig     = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
			nNextChunkSizeContig = GetChunkSize(pNextChunkContig);
			PPM_ASSERT(this, nNextChunkSizeContig > 0, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, NULL); // If this fails, the user probably corrupted the heap.

			// Try coalescing with the previous chunk in memory.
			if(!GetPrevChunkIsInUse(pChunk)) 
			{
				nPrevChunkSizeContig = pChunk->mnPriorSize;

				DebugMemFill(pChunk, mnFillFree, nChunkSize); // Overwrite the entire contents of this chunk with free fill.

				nChunkSize += nPrevChunkSizeContig;
				pChunk = GetChunkAtOffset(pChunk, -(ssize_type)nPrevChunkSizeContig);
				UnlinkChunkFromBin(pChunk);                                     // Remove the chunk from whatever list it may be in. This is OK if pChunk is the top chunk and is in its own list. It's important that we do this before changing the chunk size and flags, as the UnlinkChunkFromBin function needs to know what they are before this operation completes.
				SetChunkSize(pChunk, nChunkSize | kChunkFlagPrevInUse);
				pNextChunkContig->mnPriorSize = nChunkSize; // We can set the next chunk prev size field because we no longer own it, as we are being freed.
			}

			// Try coalescing with the next contiguous chunk in memory.
			if(GetChunkInUseOffset(pNextChunkContig, (ssize_type)nNextChunkSizeContig)) // If the next contiguous chunk is in use...
			{
				// We may or may not have coalesced with the previous chunk above.
				DebugMemFill(GetDataPtrFromChunkPtr(pChunk), mnFillFree, nChunkSize - kDataPtrOffset); // Write our user data area.
				ClearChunkInUseOffset(pNextChunkContig, 0); // Clear pNextChunkContig's 'prev in use' flag (which refers to us)
				pNextChunkContig->mnPriorSize = nChunkSize; // We can set the next chunk prev size field because we no longer own it, as we are being freed.
			}
			else
			{  // Coalesce with the next contiguous chunk.
				UnlinkChunkFromBin(pNextChunkContig);  // Pull the next contiguous chunk out of whatever list we have it stored in.

				#if PPM_DEBUG_FILL
					DebugMemFill(GetDataPtrFromChunkPtr(pChunk), mnFillFree, (nChunkSize - kDataPtrOffset) + sizeof(Chunk)); // Write our user data area and also the Chunk part of the next contiguous chunk.
					PPM_ASSERT(this, DebugMemCheck(pNextChunkContig, nNextChunkSizeContig, mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pNextChunkContig, pNextChunkContig); // Make sure next contig chunk is already filled as expected, because we don't do it here. // If this fails, the user probably corrupted the heap.
				#endif

				nChunkSize += nNextChunkSizeContig;
				SetChunkSize(pChunk, nChunkSize | kChunkFlagPrevInUse);
				SetNextChunkPrevSize(pChunk, nChunkSize); // We can set the next chunk prev size field because we no longer own it, as we are being freed.
			}

			if((pChunk != mpTopChunk) && (pNextChunkContig != mpTopChunk)) // If the top chunk wasn't affected by any coalescing above...
			{
				if(mbUnsortedBinEnabled)
				{
					// Place the chunk in unsorted chunk list. Chunks are not placed into regular 
					// bins until after they have been given one chance to be used by the next Malloc.
					Chunk* const pUnsortedBinHeadChunk = GetUnsortedBin();
					LinkChunk(pChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
				}
				else
					PlaceUnsortedChunkInBin(pChunk, nChunkSize, false);
			}
			else //Else the newly freed chunk coalesced up or down with mpTopChunk.
			{
				mpTopChunk = pChunk;
				AdjustTopChunk(mpTopChunk, nChunkSize);
			}

			DebugCheckFreeChunk(pChunk);

			// If freeing a large space, consolidate possibly-surrounding
			// chunks. Then, if the total unused top-most memory exceeds trim
			// threshold, have Trim to reduce top usage.
			// 
			// Unless mnMaxFastBinChunkSize is 0, we don't know if there are fastbins
			// bordering top, so we cannot tell for sure whether threshold
			// has been reached unless fastbins are consolidated. But we
			// don't want to consolidate on each free. As a compromise,
			// consolidation is performed if kFastBinConsolidationThreshold
			// is reached.
			#if PPM_FASTBINS_ENABLED
			if(nChunkSize >= kFastBinConsolidationThreshold) // If the chunk size is large enough to trigger FastBin clearing...
			#endif
			{
				// This fast bin clearing code is disabled because it is quite possible that 
				// during ClearFastBins() that pChunk will become invalidated. 
				// if(GetFastBinChunksExist())
				//     ClearFastBins();

				// Get the chunk after the newly freed chunk. If it is the top fence
				// chunk of its core block, then consider trimming the core block.
				Chunk* const pNextChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);

				if(GetChunkIsFenceChunk(pNextChunk)) // If newly freed pChunk was at the top of memory.
				{
					if(nChunkSize >= mnTrimThreshold)
						TrimCore(mnTopPad);
				}
			}
		}
		else // Else the chunk is a memory-mapped chunk.
		{
			// If the chunk was allocated via mmap, release via MMapFreeInternal. Note
			// that if PPM_MMAP_SUPPORTED is false but GetChunkIsMMapped is true, then the
			// user must have overwritten memory. There's nothing we can do to
			// catch this error unless PPM_DEBUG is set, in which case
			// DebugCheckUsedChunk (above) will have flagged the error.
			#if PPM_MMAP_SUPPORTED
				DebugCheckMMappedChunk(pChunk);

				char* const pMMapMemory = (char*)pChunk - pChunk->mnPriorSize;
				PPM_ASSERT(this, PPM_IsAligned(pMMapMemory, mnMMapPageSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the block/heap.

				// mnPriorSize has special use for MMap chunks. Memory-mapped chunks are stored 
				// possibly some bytes in down from the beginning of the memory-mapped space.
				// kMinChunkSize refers to the empty chunk we store at the end of the mmap space
				// for our tracking purposes.
				const size_type nMMapMemorySize = pChunk->mnPriorSize + nChunkSize + kMinChunkSize; 

				// Note that we store MMapped chunks int the mmap list via their fence chunk at 
				// the end of their memory and not at the beginning. The reason for this that the
				// space at the beginning of their memory is used by data and the list pointers
				// are not available.
				Chunk* pFenceChunk = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
				PPM_ASSERT(this, PPM_IsAligned((char*)pFenceChunk + kMinChunkSize, mnMMapPageSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
				UnlinkChunkFromBin(pFenceChunk); // Remove Chunk from mHeadMMapChunk list.

				PPM_ASSERT(this, mnMMapMallocTotal >= nMMapMemorySize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
				mnMMapMallocTotal -= nMMapMemorySize;
				PPM_ASSERT(this, mnMMapCount >= 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
				mnMMapCount--;

				MMapFreeInternal(pMMapMemory, nMMapMemorySize);
			#else
				PPM_ASSERT(this, false, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc or the configuration of PPMalloc is probably at fault.
			#endif
		}
	#if PPM_NULL_POINTER_FREE_ENABLED
	} // if(pData){
	#endif

} // GeneralAllocator::FreeInternal

///////////////////////////////////////////////////////////////////////////////
// MallocAligned
//
void* GeneralAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	void* pReturnValue;

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMallocAligned;
			hookInfo.mnSizeInputTotal   = nSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = nAlignment;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = nAlignmentOffset;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	pReturnValue = MallocAlignedInternal(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if (mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif
		
	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// MallocAlignedInternal
//
// Returns a chunk aligned to a given power of two value address increment.
//
void* GeneralAllocator::MallocAlignedInternal(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	void*        pMallocValue;        // The unaligned return value from Malloc.
	void*        pReturnValue;        // The returned memory, may be same as pMallocValue.
	Chunk*       pChunk;              // Chunk for allocated memory.
	size_type    nRequestedChunkSize; // Required chunk size.
	Chunk*       pReturnChunk;        // Chunk to return.
	size_type    nNewChunkSize;       // Size of chunk to return.
	size_type    nLeadingSize;        // leading space before nAlignment point
	Chunk*       pRemainderChunk;     // 
	size_type    nRemainderChunkSize; // 

	PPM_ASSERT(this, (nAlignment == 0) || PPM_IsPowerOf2(nAlignment), kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], NULL, NULL);  // Alignments must be a perfect power of 2. // If this fails, it is the user's fault.
	PPM_ASSERT(this, nAlignment <= (1024 * 1024), kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], NULL, NULL); // Assert via some arbitrarily high value. // If this fails, it is the user's fault.

	if((nAlignment <= kMinEffectiveAlignment) && (nAlignmentOffset == 0) && ((nAllocationFlags & kAllocationFlagAlwaysAlign) == 0))
	{
		pReturnValue = MallocInternal(nSize, nAllocationFlags);
		PPM_ASSERT(this, (nAlignment == 0) || PPM_IsAligned((char*)pReturnValue + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
		return pReturnValue;
	}

	// If the alignment is less than the minimum allocation size, bump it up.
	// Note that if you request alignment of 4 but are given alignment of 8, 
	// you still have alignment of 4. So it is OK to bump it up like this.
	if(nAlignment < kMinAllocSize)
		nAlignment = kMinAllocSize;

	// Make sure alignment is an even power of two.
	if(nAlignment & (nAlignment - 1))
	{
		// This following is a trick to round a value up to nearest power of two without branching.
		nAlignment--;
		nAlignment |= (nAlignment >> 1);
		nAlignment |= (nAlignment >> 2);
		nAlignment |= (nAlignment >> 4);
		nAlignment |= (nAlignment >> 8);
		nAlignment |= (nAlignment >> 16);
		nAlignment++;
	}
	PPM_ASSERT(this, PPM_IsPowerOf2(nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

	// Make sure alignment offset, if specified, is a multiple of kMinEffectiveAlignment.
	PPM_ASSERT(this, (nAlignmentOffset % kMinEffectiveAlignment) == 0, kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], NULL, NULL); // If this fails, the user is at fault. See the documented specifications for MallocAligned.
	if(nAlignmentOffset % kMinEffectiveAlignment)
		nAlignmentOffset = PPM_AlignUp(nAlignmentOffset, kMinEffectiveAlignment);

	nRequestedChunkSize = GetChunkSizeFromDataSize((size_type)nSize);

	int bBestFit = (nAllocationFlags & kAllocationFlagBestFit);
	int bEndFit  = (nAllocationFlags & kAllocationFlagEndFit);
	int bHigh    = (nAllocationFlags & kAllocationFlagHigh);

	if(bBestFit && bEndFit)
	{
		// In this case, we look in the bins for blocks that have a perfect alignment match.
		// This is useful for blocks that are allocated, freed, and then another 
		// identically sized and aligned block is needed.
		Chunk*          pBestLowChunk      = NULL;   // Best Chunk to use for the request
		void*           pBestLowChunkPos   = NULL;   // Position within pBestLowChunk to 
		Chunk*          pBestHighChunk     = NULL;
		void*           pBestHighChunkPos  = NULL;
	  //Chunk*          pDiscardedChunk    = NULL;
		Chunk*          pEnd               = NULL;
		const int       binIndex           = GetBinIndexFromChunkSize(nRequestedChunkSize);
		int             i;

		for(i = 0; i < kBinCount; i++)
		{
			// First time through, look through the unsorted bin, second time through, look at the regular bins.
			if(i == 0)
			{
				pEnd = GetUnsortedBin();
				i    = binIndex - 1;
			}                
			else
				pEnd = GetBin(i); // It turns out that bin 0 is unusuble anyway.

			// SkipListChunk mod needed.
			// We are looking for more than just a size match below, we are also looking for an alignment and low/high memory match.
			// As a result, we can't use the skip list optimizations and just need to check every chunk. 
			for(pChunk = pEnd->mpNextChunk; pChunk != pEnd; pChunk = pChunk->mpNextChunk)
			{
				const size_type nChunkSize = GetChunkSize(pChunk);

				if(nChunkSize >= nRequestedChunkSize)
				{
					// Note that pChunkAlignedDown is not necessarily a valid Chunk; it is the position where a valid Chunk could be made within pChunk.
					void* pChunkAlignedUp = NULL;
					void* pChunkAlignedDown = GetAlignedChunkPositionChecked(pChunk, nRequestedChunkSize, nAlignment, nAlignmentOffset, pChunkAlignedUp);

					//pDiscardedChunk = pChunk; // Set it to this until otherwise.

					if(pChunkAlignedDown) // If pChunk can provide this allocation...
					{
						if(((uintptr_t)pChunk < ((uintptr_t)mpTopChunk - (20 * 1024 * 1024))) && // If the chunk is considered low and
							(!pBestLowChunk || (pChunk < pBestLowChunk)))                        // if it's lower than the previous most low chunk.
						{
							//pDiscardedChunk = pBestLowChunk; // OK if pBestLowChunk is NULL.
							pBestLowChunk    = pChunk;
							pBestLowChunkPos = pChunkAlignedDown;
						}

						if(((uintptr_t)pChunk >= ((uintptr_t)mpTopChunk + (0 * 1024 * 1024))) && // If the chunk is considered high and
							(pChunk > pBestHighChunk))                                           // if it's higher than the previous most high chunk.
						{
							//pDiscardedChunk = pBestHighChunk; // OK if pBestLowChunk is NULL.
							pBestHighChunk    = pChunk;
							pBestHighChunkPos = pChunkAlignedUp;
						}
					}

					// We need to implement this, but need to do it correctly. Watch out for the fact that we could have to remove two chunks this loop. Watch out for the fact that we are iterating while removing.
					//if((i == 0) && pDiscardedChunk) // If going through the unsorted bin...
					//    PlaceUnsortedChunkInBin(pDiscardedChunk, GetChunkSize(pDiscardedChunk), true);
				}
			} // for()...
		} // for()...

		// If the above search succeeded, use it.
		if(pBestHighChunk && bHigh)
		{
			pBestHighChunk = SplitChunkAligned(pBestHighChunk, GetChunkSize(pBestHighChunk), pBestHighChunkPos, nRequestedChunkSize);
			pReturnValue   = GetDataPtrFromChunkPtr(pBestHighChunk);

			return pReturnValue;
		}

		// If the user wanted high memory but couldn't get it, possibly we use low memory instead of eating into the top chunk.
		const size_type topChunkSize = GetChunkSize(mpTopChunk);
		if(topChunkSize > (20 * 1024 * 1024)) // To do: Make this configurable.
			nAllocationFlags &= ~kAllocationFlagAvoidTopChunk;


		if(pBestLowChunk && (!bHigh || (nAllocationFlags & kAllocationFlagAvoidTopChunk)))
		{
			pBestLowChunk = SplitChunkAligned(pBestLowChunk, GetChunkSize(pBestLowChunk), pBestLowChunkPos, nRequestedChunkSize);
			pReturnValue  = GetDataPtrFromChunkPtr(pBestLowChunk);

			return pReturnValue;
		}


		// We use the top chunk if the user allows it or if we have no other choice.
		// Otherwise we fall through and use the basic logic of Malloc a slightly
		// larger size than necessary and trimming the leftover space.
		const bool bShouldUseTopChunk = ((nAllocationFlags & kAllocationFlagAvoidTopChunk) == 0);

		if(bShouldUseTopChunk)
		{
			const size_type nMaxRequiredSize = (size_type)(nRequestedChunkSize + nAlignment + nAlignmentOffset + kMinAllocSize);

			// Use top chunk.
			nNewChunkSize = GetChunkSize(mpTopChunk);
			bHigh         = (nAllocationFlags & kAllocationFlagHigh);

			if(nNewChunkSize >= nMaxRequiredSize) // If the top chunk has enough for the requested allocation...
			{
				pReturnChunk = mpTopChunk;

				if(bHigh) // If high (permanent) memory is requested...
				{
					// Get the data from the top of the chunk instead of the bottom.
					// In this case the remainder chunk is the lower chunk and the result
					// chunk is the upper chunk. Note also that the remainder chunk will
					// be the new mpTopChunk.
					pRemainderChunk = mpTopChunk;

					// This calculation aligns pReturnChunk down to the address where the data portion of it satisfies nAlignment and nAlignmentOffset.
					//pReturnChunk = (Chunk*)(((((intptr_t)pRemainderChunk + nNewChunkSize - nRequestedChunkSize + (kDataPtrOffset + nAlignmentOffset))) & (-(intptr_t)nAlignment)) - (kDataPtrOffset + nAlignmentOffset));
					pReturnChunk = GetHighestAlignedChunkPosition(pRemainderChunk, nRequestedChunkSize, nAlignment, nAlignmentOffset);

					PPM_ASSERT(this, PPM_IsAligned((char*)GetDataPtrFromChunkPtr(pReturnChunk) + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					PPM_ASSERT(this, ((char*)pRemainderChunk + kMinChunkSize) <= (char*)pReturnChunk, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pRemainderChunk, pRemainderChunk); // Assert that this was possible without screwing up the lower chunk. // If this fails, the user probably corrupted the heap.

					nRemainderChunkSize = (size_type)((uintptr_t)pReturnChunk - (uintptr_t)pRemainderChunk);
					nRequestedChunkSize = (size_type)(nNewChunkSize - nRemainderChunkSize);

					SetChunkSize(pReturnChunk, nRequestedChunkSize); // Make sure kChunkFlagPrevInUse is off, because the remainder will not be in use.
					#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
						// pReturnChunk->mnPriorSize = nRemainderChunkSize;    Not necessary because AdjustTopChunk below will take care of this.
					#endif
					// SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);    Not necessary because AdjustTopChunk below will take care of this.

					Chunk* const pNextChunk1 = GetChunkAtOffset(pReturnChunk, (ssize_type)nRequestedChunkSize); // This chunk may or may not be the top fencepost chunk for the core block, depending on if any other chunks have already been allocated up here.
					pNextChunk1->mnPriorSize = nRequestedChunkSize;
					pNextChunk1->mnSize     |= kChunkFlagPrevInUse;

					// To do: Implement splitting off trailing memory that isn't needed. Without this, 
					// a significant amount of memory could be wasted. Note that we can't quite use SplitChunkTail,
					// as it assumes the Chunk is coming from a bin. But we would use similar code.

				// To do: Implement the lower memory version. Note that we may need to split off a leading Chunk as well as the trailing remainder.
				//}
				//else
				//{
				//    nRemainderChunkSize = nNewChunkSize - nRequestedChunkSize;
				//
				//    SetChunkSize(pReturnChunk, nRequestedChunkSize | kChunkFlagPrevInUse);
				//
				//    #if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
				//        SetNextChunkPrevSize(pReturnChunk, nRequestedChunkSize);
				//    #endif
				//
				//    pRemainderChunk = GetChunkAtOffset(pReturnChunk, (ssize_type)nRequestedChunkSize);
				//    SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse); // Not necessary because AdjustTopChunk below will take care of this.
				//}

					// Common code to both low and high pathways.
					#if PPM_DEBUG_FILL
						// DebugMemFill(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), mnFillFree, nRemainderChunkSize - sizeof(Chunk)); // Should already be mem-filled.
						PPM_ASSERT(this, DebugMemCheck(GetPostHeaderPtrFromChunkPtr(pRemainderChunk), nRemainderChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pRemainderChunk, pRemainderChunk); // If this fails, the user probably corrupted the heap.
					#endif

					mpTopChunk = pRemainderChunk;
					AdjustTopChunk(mpTopChunk, nRemainderChunkSize); // This sets its size, puts it in standalone self-list, etc.

					DebugCheckMallocedChunk(pReturnChunk, nRequestedChunkSize, true, (bHigh != 0)); // Defined away unless debugging enabled.

					// This assertion is disabled until we can make it work and until we finish the implementation of this function. For the code we currently have here, we often go over (nSize + 32).
					//PPM_ASSERT(this, (GetChunkSize(pReturnChunk) <= (nSize + 32)) && (GetChunkSize(pReturnChunk) > nSize), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pReturnChunk, pReturnChunk); // Assert that the chunk was not mis-returned or in the wrong cache. // If this fails, the user probably corrupted the heap.

					pReturnValue = GetDataPtrFromChunkPtr(pReturnChunk);
					return pReturnValue;
				}

			} // if(top chunk has enough space)

		} // if(bShouldUseTopChunk)

	} // if(bBestFit)


	// At this point, either no flags were specified or the specified flags were unable to be 
	// satisified. So we back down to less efficient behaviour involving a call to Malloc.

	// What we do below is freshly allocate a block that is the max possible size that an aligned
	// allocation might need. This is fast, but it may fragment memory more than necessary, because
	// what if there is a block in the free list that is already just the right size and alignment
	// that is requested? This case can be common if an aligned block is frequently freed and soon
	// allocated again.

	pMallocValue = MallocInternal(nRequestedChunkSize + nAlignment + nAlignmentOffset + kMinAllocSize, nAllocationFlags); // Add extra space so that we know we will be able to align the return value within this chunk and be able to split this chunk appropriately.

	if(pMallocValue)
	{
		pChunk = GetChunkPtrFromDataPtr(pMallocValue);
		PPM_ASSERT(this, GetChunkSize(pChunk) >= (nRequestedChunkSize + nAlignment + nAlignmentOffset + kMinAllocSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

		if(((uintptr_t)pMallocValue + nAlignmentOffset) % nAlignment) // If the returned value isn't already aligned as required...
		{
			// Find an aligned spot inside chunk. Since we need to give back
			// leading space in a chunk of at least kMinAllocSize, if the first
			// calculation places us at a spot with less than kMinAllocSize leader,
			// we can move to the next aligned spot -- we've allocated enough
			// total room so that this is always possible.
			PPM_CT_ASSERT(sizeof(size_t) >= sizeof(intptr_t)); // If this fails, PPMalloc is at fault.

			void*  pAlignedAddress = (void*)((intptr_t)((char*)pMallocValue + nAlignmentOffset + (nAlignment - 1)) & (-(intptr_t)nAlignment));
			char*  pAlignedChunk   = (char*)GetChunkPtrFromDataPtr(pAlignedAddress) - nAlignmentOffset;

			if((uintptr_t)(pAlignedChunk - (char*)pChunk) < kMinAllocSize)
				pAlignedChunk += nAlignment; // We should have allocated enough space above to deal with this.

			pReturnChunk  = (Chunk*)pAlignedChunk;
			nLeadingSize  = (size_type)(pAlignedChunk - (char*)pChunk);
			nNewChunkSize = GetChunkSize(pChunk) - nLeadingSize;

			// For memory mapped chunks, just adjust the offset. We do this because we never split memory mapped chunks.
			if(GetChunkIsMMapped(pChunk))
			{
				pReturnChunk->mnPriorSize = pChunk->mnPriorSize + nLeadingSize; // We don't currently have a SetPriorChunkSize function. This is actually only useful for debugging. 
				SetChunkSize(pReturnChunk, nNewChunkSize | kChunkFlagMMapped);
				SetNextChunkPrevSize(pReturnChunk, nNewChunkSize); // Fix up mnPriorSize of the link chunk at the end of the memory mapped area

				pReturnValue = GetDataPtrFromChunkPtr(pReturnChunk);
				PPM_ASSERT(this, PPM_IsAligned((char*)pReturnValue + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pReturnChunk, pReturnChunk); // If this fails, PPMalloc is probably at fault.
				return pReturnValue;
			}
			else
			{
				// Split off the leading portion of the chunk that precedes the alignment point.
				// We will always have enough space to do this because we gave enough space above.
				SetChunkSize(pReturnChunk, nNewChunkSize | kChunkFlagPrevInUse); // Set prev as in-use because we will free pChunk below.
				SetChunkInUseOffset(pReturnChunk, (ssize_type)nNewChunkSize);
				SetNextChunkPrevSize(pReturnChunk, nNewChunkSize);
				SetChunkSizePreserveFlags(pChunk, nLeadingSize);
				pReturnChunk->mnPriorSize = nLeadingSize; // We don't currently have a SetPriorChunkSize function. This is actually only useful for debugging. 

				const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
				mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.
				FreeInternal(GetDataPtrFromChunkPtr(pChunk)); // Free the unused leading portion.
				mAutoHeapValidationLevel = hvlSaved;

				pChunk = pReturnChunk;

				PPM_ASSERT(this, (nNewChunkSize >= nRequestedChunkSize) && (((size_t)GetDataPtrFromChunkPtr(pChunk) + nAlignmentOffset) % nAlignment) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			}
		}

		// If there is any useful space at the end of the chunk, split it off.
		if(!GetChunkIsMMapped(pChunk))
		{
			nNewChunkSize = GetChunkSize(pChunk);
			if(nNewChunkSize > (nRequestedChunkSize + kMinAllocSize)) // If the chunk can be split...
			{
				nRemainderChunkSize = nNewChunkSize - nRequestedChunkSize;
				pRemainderChunk = GetChunkAtOffset(pChunk, (ssize_type)nRequestedChunkSize);
				SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
				#if PPM_DEBUG_PRESERVE_PRIOR
					SetNextChunkPrevSize(pRemainderChunk, nRemainderChunkSize);
					pRemainderChunk->mnPriorSize = nRequestedChunkSize; // We don't currently have a SetPriorChunkSize function. This is actually only useful for debugging. 
				#endif
				SetChunkSizePreserveFlags(pChunk, nRequestedChunkSize);

				// If the chunk after pRemainderChunk is free, then immediately coalesce it with the next chunk.
				// This is useful because otherwise multiple calls to MallocAligned in a row would fragment 
				// memory more than needed due to a stranded remainder chunk that could have been used for 
				// the next aligned chunk request.
				Chunk* const pNextChunk = (Chunk*)((uintptr_t)pRemainderChunk + nRemainderChunkSize);

				if(GetChunkIsInUse(pNextChunk))
				{
					const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
					mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.
					FreeInternal(GetDataPtrFromChunkPtr(pRemainderChunk));
					mAutoHeapValidationLevel = hvlSaved;
				}
				else // Else we can coalesce pNextChunk with the chunk after it in memory.
				{
					const size_type nNextChunkSize = GetChunkSize(pNextChunk);

					UnlinkChunkFromBin(pNextChunk); // Pull the next contiguous chunk out of whatever list we have it stored in.

					DebugMemFill(pNextChunk, mnFillFree, sizeof(Chunk)); // Debug mem fill the header portion of the chunk we are coalescing with.
					DebugMemFill((char*)pRemainderChunk + nRemainderChunkSize - 2, mnFillFree, 2); // We fill these two bytes because GeneralAllocatorDebug will have set these to some value during allocation above.

					nRemainderChunkSize += nNextChunkSize;

					#if PPM_DEBUG_FILL
						PPM_ASSERT(this, DebugMemCheck((char*)pRemainderChunk + kDataPtrOffset, nRemainderChunkSize - kDataPtrOffset, mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pRemainderChunk, pRemainderChunk); // Make sure the entire newly coalesced chunk is now filled as expected. // If this fails, the user probably corrupted the heap.
					#endif

					SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
					SetNextChunkPrevSize(pRemainderChunk, nRemainderChunkSize); // We can set the next chunk prev size field because we no longer own it, as we are being freed.

					if((pRemainderChunk != mpTopChunk) && (pNextChunk != mpTopChunk)) // If the top chunk wasn't affected by any coalescing directly above...
					{
						Chunk* const pUnsortedBinHeadChunk = GetUnsortedBin();
						LinkChunk(pRemainderChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
					}
					else //Else the newly freed chunk coalesced up or down with mpTopChunk.
					{
						mpTopChunk = pRemainderChunk;
						AdjustTopChunk(mpTopChunk, nRemainderChunkSize);
					}

					#if (PPM_DEBUG >= 2) // Extended user debug builds.
						DebugCheckFreeChunk(pRemainderChunk);
					#endif
				}
			}
		}

		DebugCheckUsedChunk(pChunk);
		pReturnValue = GetDataPtrFromChunkPtr(pChunk);
		PPM_ASSERT(this, PPM_IsAligned((char*)pReturnValue + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
		// Fall through with the current value of pReturnValue.
	}
	else
		pReturnValue = NULL;

	#if (PPM_DEBUG >= 3) // Developer builds only.
		DebugCheckState(kHeapValidationLevelFull);
	#endif

	return pReturnValue;
}


/////////////////////////////////////////////////////////////////////////////////
// Realloc
//
void* GeneralAllocator::Realloc(void* pData, size_t nNewSize, int nAllocationFlags)
{
	void* pReturnValue;

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeRealloc;
			hookInfo.mnSizeInputTotal   = nNewSize;
			hookInfo.mpDataInput        = pData;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nNewSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	pReturnValue = ReallocInternal(pData, nNewSize, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}


/////////////////////////////////////////////////////////////////////////////////
// ReallocInternal
//
// The ISO C++ standard does not describe realloc but instead defers to the 
// ISO C89 standard.
//
// The ISO C89 standard states:
//     (1) realloc(NULL, newsize) is equivalent to malloc(newsize).
//     (2) realloc(pblock, 0) is equivalent to free(pblock) (except that NULL is returned).
//     (3) if the realloc() fails, the object pointed to by pblock is left unchanged.
//
// The ISO C99 standard states:
//     The realloc function deallocates the old object pointed to by ptr and 
//     returns a pointer to a new object that has the size specified by size. 
//     The contents of the new object shall be the same as that of the old 
//     object prior to deallocation, up to the lesser of the new and old sizes. 
//     Any bytes in the new object beyond the size of the old object have
//     indeterminate values.
//
//     If ptr is a null pointer, the realloc function behaves like the malloc 
//     function for the specified size. Otherwise, if ptr does not match a 
//     pointer earlier returned by the calloc, malloc, or realloc function, 
//     or if the space has been deallocated by a call to the free or realloc 
//     function, the behavior is undefined. If memory for the new object 
//     cannot be allocated, the old object is not deallocated and its value 
//     is unchanged.
//     
//     The realloc function returns a pointer to the new object (which may have 
//     the same value as a pointer to the old object), or a null pointer if 
//     the new object could not be allocated.
//
// Note that the C99 standard does not state that reallocing a pointer to 
// zero size acts the same as free and returns NULL. However, many C implementations
// and definitions of realloc specify this behaviour. It seems to this author
// that the C99 standard is more proper, as it specifies consistent behaviour
// regardless of the specified size.
//
// Our behaviour
//     ptr      size    return     semantics (meaning of the return value)
//     -------------------------------------------------------------
//     NULL     >0      NULL       allocation failed.
//     NULL     >0      valid      allocation succeeded.
//     valid    >0      NULL       allocation failed, original pointer not freed.
//     valid    >0      valid      allocation failed, original pointer freed.
//  PPM_REALLOC_C99_ENABLED => 0
//     NULL      0      NULL       no-op.
//     valid     0      NULL       original pointer freed.
//  PPM_REALLOC_C99_ENABLED => 1 (this is the default)
//     NULL      0      NULL       allocation of zero size block failed.
//     NULL      0      valid      allocation of zero size block succeeded.
//     valid     0      NULL       allocation of zero size block failed, original pointer not freed.
//     valid     0      valid      allocation of zero size block succeeded, original pointer freed.
//
//     Reallocating a pointer to a smaller size is guaranteed to succeed, 
//     but is not guaranteed to return the original pointer.
//
// One of the results of all this is that strictly speaking it is unsafe to 
// write code like this:
//     pData = realloc(pData, 100);
// because if the realloc fails, the original pointer isn't freed. But the code 
// above loses the original pointer.
//
void* GeneralAllocator::ReallocInternal(void* pData, size_t nNewSize, int nAllocationFlags)
{
	void*       pReturnValue = NULL;
	Chunk*      pOldChunk;
	size_type   nOldChunkSize;
	size_type   nNewChunkRequestSize;
	Chunk*      pNewChunk;
	size_type   nNewChunkSize;
	Chunk*      pNextChunkContig;
	Chunk*      pRemainderChunk;
	size_type   nRemainderChunkSize;

	// For new size of zero, C89 but not C99 standard calls for the same behaviour as free.
	#if !PPM_REALLOC_C99_ENABLED // If C89 behaviour...
		if(EA_UNLIKELY(nNewSize == 0))
		{
			if(pData)
				FreeInternal(pData);
			return NULL;
		}
	#endif

	// For old pointer of NULL, C89 and C99 standards call for same behaviour as malloc.
	if(EA_UNLIKELY(pData == NULL))
	{
		pReturnValue = MallocInternal(nNewSize, nAllocationFlags);
		return pReturnValue;
	}

	#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
		if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
		{
			ValidateHeap(mAutoHeapValidationLevel);
			mnAutoHeapValidationEventCount = 0;
		}
	#endif

	nNewChunkRequestSize = GetChunkSizeFromDataSize((size_type)nNewSize); // Possibly use GetChunkSizeFromDataSizeChecked instead.
	pOldChunk            = GetChunkPtrFromDataPtr(pData);
	nOldChunkSize        = GetChunkSize(pOldChunk);

	DebugCheckUsedChunk(pOldChunk); // Defined away in a release build.

	if(!GetChunkIsMMapped(pOldChunk)) // If we have a normally allocated chunk...
	{
		if(nNewChunkRequestSize <= nOldChunkSize) // If the user is requesting a no-op (we assume this is rare) or shrink...
		{
			pNewChunk     = pOldChunk;        // Save these off now. We'll possibly do a split below.
			nNewChunkSize = nOldChunkSize;
		}
		else // Else the user is requesting growth...
		{
			pNextChunkContig = GetChunkAtOffset(pOldChunk, (ssize_type)nOldChunkSize);

			if((pNextChunkContig == mpTopChunk) && // If the request goes into the top chunk and the top chunk has enough space as-is...
				(nNewChunkSize = nOldChunkSize + GetChunkSize(pNextChunkContig)) >= (nNewChunkRequestSize + kMinAllocSize)) 
			{
				SetChunkSizePreserveFlags(pOldChunk, nNewChunkRequestSize);

				mpTopChunk = GetChunkAtOffset(pOldChunk, (ssize_type)nNewChunkRequestSize); 
				#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
					mpTopChunk->mnPriorSize = nNewChunkRequestSize;
				#endif
				AdjustTopChunk(mpTopChunk, nNewChunkSize - nNewChunkRequestSize);

				DebugCheckUsedChunk(pOldChunk);
				pReturnValue = GetDataPtrFromChunkPtr(pOldChunk);
				return pReturnValue;
			}
			else if((pNextChunkContig != mpTopChunk) &&     // If request goes into the next contiguous chunk and the next 
					!GetChunkIsInUse(pNextChunkContig) &&   // contiguous chunk is unused and has enough space...
					(nNewChunkSize = (nOldChunkSize + GetChunkSize(pNextChunkContig))) >= nNewChunkRequestSize)
			{
				pNewChunk = pOldChunk;
				UnlinkChunkFromBin(pNextChunkContig);
			}
			else // Else we need to allocate completely new space.
			{
				pReturnValue = MallocInternal(nNewChunkRequestSize - kMinAlignmentMask, nAllocationFlags);

				if(pReturnValue)
				{
					pNewChunk     = GetChunkPtrFromDataPtr(pReturnValue);
					nNewChunkSize = GetChunkSize(pNewChunk);

					if(pNewChunk == pNextChunkContig)   // If the new chunk is the next contiguous chunk... 
					{                                   // The logic about 20 lines above should actually catch this case already.
						nNewChunkSize += nOldChunkSize; // Merge the chunks and fall through.
						pNewChunk      = pOldChunk;
					}
					else
					{
						const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
						mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.

						LocalChunkCopy(pReturnValue, pData, nOldChunkSize - kSizeTypeSize);
						FreeInternal(pData);

						mAutoHeapValidationLevel = hvlSaved;

						DebugCheckUsedChunk(pNewChunk);
						pReturnValue = GetDataPtrFromChunkPtr(pNewChunk);
						return pReturnValue;
					}
				}
				else  //Else the malloc failed. So we quit.
				{
					// Note that the C89 and C99 standards state that we return NULL but don't free the original pointer (pData).
					return NULL;
				}
			}
		}
 
		// At this point we have a new chunk with data in it and the new chunk may or may not need to be trimmed (split).
		PPM_ASSERT(this, nNewChunkSize >= nNewChunkRequestSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

		nRemainderChunkSize = nNewChunkSize - nNewChunkRequestSize;

		if(nRemainderChunkSize < kMinAllocSize) // If there isn't enough space to trim the chunk..
		{ 
			SetChunkSizePreserveFlags(pNewChunk, nNewChunkSize);
			SetChunkInUseOffset(pNewChunk, (ssize_type)nNewChunkSize);
			#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
				SetNextChunkPrevSize(pNewChunk, nNewChunkSize);
			#endif
		}
		else // Else split the chunk.
		{
			pRemainderChunk = GetChunkAtOffset(pNewChunk, (ssize_type)nNewChunkRequestSize);
			SetChunkSizePreserveFlags(pNewChunk, nNewChunkRequestSize);
			SetChunkSize(pRemainderChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
			#if PPM_DEBUG_PRESERVE_PRIOR // Only in PPM_DEBUG_PRESERVE_PRIOR mode is the allocated chunk's next-chunk-prev-size usable.
				pRemainderChunk->mnPriorSize = nNewChunkRequestSize;
			#endif
			SetNextChunkPrevSize(pRemainderChunk, nRemainderChunkSize);

			// Make sure pRemainderChunk is marked as in use so FreeInternal won't complain.
			SetChunkInUseOffset(pRemainderChunk, (ssize_type)nRemainderChunkSize);

			const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
			mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.
			FreeInternal(GetDataPtrFromChunkPtr(pRemainderChunk));
			mAutoHeapValidationLevel = hvlSaved;
		}

		DebugCheckUsedChunk(pNewChunk);
		pReturnValue = GetDataPtrFromChunkPtr(pNewChunk);
	}
	else // Else the chunk is memory-mapped.
	{
		// As of this writing, it is not necessary to lock a mutex if we have
		// thread safety enabled. This section doesn't itself manipulate member data.

		#if PPM_MMAP_SUPPORTED
			if((nNewChunkRequestSize + kSizeTypeSize) <= nOldChunkSize) // We check for '+ kSizeTypeSize' because memory mapped chunks add this to the end of their allocation. They do this because they have no next contiguous chunk but need to access the next contiguous chunk's prevsize field.
			{
				pReturnValue = pData; // Do nothing
			}
			else // Else we must malloc a new larger block.
			{
				pReturnValue = MallocInternal(nNewChunkRequestSize - kMinAlignmentMask, nAllocationFlags); // This Malloc call may or may not return a new pointer which is memory-mapped, but it will probably be memory mapped.
				if(pReturnValue)
				{
					memcpy(pReturnValue, pData, nOldChunkSize - kDataPtrOffset);

					const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
					mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.
					FreeInternal(pData);
					mAutoHeapValidationLevel = hvlSaved;
				}
			}

		#else
			// This is bad. The memory-mapped bit is set but we don't have support for memory mapped files.
			#if (PPM_DEBUG >= 2) // Extended user debug builds.
				DebugCheckState(kHeapValidationLevelBasic);
			#endif
		#endif // PPM_MMAP_SUPPORTED
	}

	return pReturnValue;
} // Realloc


///////////////////////////////////////////////////////////////////////////////
// Calloc
//
void* GeneralAllocator::Calloc(size_t nElementCount, size_t nElementSize, int nAllocationFlags)
{
	void* pReturnValue;

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeCalloc;
			hookInfo.mnSizeInputTotal   = nElementCount * nElementSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = nElementCount;
			hookInfo.mnSizeInput        = nElementSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	pReturnValue = CallocInternal(nElementCount, nElementSize, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// CallocInternal
//
void* GeneralAllocator::CallocInternal(size_t nElementCount, size_t nElementSize, int nAllocationFlags)
{
	void* const pReturnValue = MallocInternal(nElementCount * nElementSize, nAllocationFlags);

	if(pReturnValue)
	{
		const Chunk* const pChunk      = GetChunkPtrFromDataPtr(pReturnValue);
		const size_type     nChunkSize = GetChunkSize(pChunk);

		#if PPM_MMAP_CLEARS
			if(!GetChunkIsMMapped(pChunk)) // Don't need to clear memory-mapped space
		#endif
			{
				#if PPM_DEBUG_PRESERVE_PRIOR
					// In debug mode we reserve space for the mnPriorSize field of the next contiguous chunk.
					LocalChunkClear(pReturnValue, nChunkSize - kDataPtrOffset);  // '- kDataPtrOffset' because the chunk has the mnPriorSize and mnSize reserved front members.
				#else
					// We clear (nChunkSize - kDataPtrOffset) + kSizeTypeSize) because we need to not 
					// clear the first two size_types of the chunk, but we do want to clear the one 
					// size_type of the following chunk. That leaves us with a net +1 size_type to clear.
					LocalChunkClear(pReturnValue, (nChunkSize - kDataPtrOffset) + kSizeTypeSize); // '+ kSizeTypeSize' because we go into the mnPriorSize field of the next chunk.
				#endif
			}
  }

  return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocMultiple
//
void** GeneralAllocator::MallocMultiple(size_t nElementCount, size_t nElementSize, void* pResultArray[], int nAllocationFlags)
{
	void** ppReturnValue;

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMallocMultiple1;
			hookInfo.mnSizeInputTotal   = nElementCount * nElementSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = nElementCount;
			hookInfo.mnSizeInput        = nElementSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = pResultArray;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	ppReturnValue = MallocMultipleInternal(nElementCount, 1, &nElementSize, pResultArray, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpArrayOutput = ppReturnValue;
		  //hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true); To do: Implement this.

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return ppReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocMultiple
//
void** GeneralAllocator::MallocMultiple(size_t nElementCount, const size_t pElementSizes[], void* pResultArray[], int nAllocationFlags)
{
	void** ppReturnValue;

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		size_t   nElementSizeSum = 0;
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			for(size_t i = 0; i < nElementCount; i++)
				nElementSizeSum += pElementSizes[i];

			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMallocMultiple2;
			hookInfo.mnSizeInputTotal   = nElementCount * nElementSizeSum;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = nElementCount;
			hookInfo.mnSizeInput        = nElementSizeSum;
			hookInfo.mpSizeInputArray   = pElementSizes;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = pResultArray;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	ppReturnValue = MallocMultipleInternal(nElementCount, nElementCount, pElementSizes, pResultArray, nAllocationFlags);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpArrayOutput = ppReturnValue;
		  //hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true); To do: Implement this.

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return ppReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocMultipleInternal
//
void** GeneralAllocator::MallocMultipleInternal(size_t nElementCount, size_t nSizeCount, const size_t pElementSizes[], void* pResultArray[], int nAllocationFlags)
{
	Chunk*    pReturnArrayChunk;      //
	void**    pReturnArray;           // Holds either 'pResultArray' or a newly allocated array.
	size_type nReturnArraySize;       // If we need to allocate pReturnArray here, this holds its size. Otherwise zero.
	void*     pAggregateMemory;        
	size_type nAggregateMemorySize;
	Chunk*    pAggregateMemoryChunk;
	size_type nChunkMemorySize;       // Size of chunk memory requested by user, i.e. not memory allocated to hold the array to return to the user.
	size_type nRemainderChunkSize;
	size_t    i;

	if(pResultArray)
	{
		if(nElementCount == 0)
			return pResultArray;
		pReturnArray      = pResultArray;
		nReturnArraySize = 0;
	}
	else
	{  // Else we need to calculate the array nAggregateMemorySize ourselves.
		if(nElementCount == 0)
		{
			pResultArray = (void**)MallocInternal(0, nAllocationFlags);
			return pResultArray; 
		}
		pReturnArray     = NULL;
		nReturnArraySize = GetChunkSizeFromDataSize((size_type)(nElementCount * sizeof(void*)));
	}

	// Calculate nAggregateMemorySize.
	for(i = 0, nChunkMemorySize = 0; i < nSizeCount; i++)
		nChunkMemorySize += GetChunkSizeFromDataSize((size_type)pElementSizes[i]);

	// Subtract out alignment bytes from total to minimize overallocation.
	nAggregateMemorySize = nChunkMemorySize + nReturnArraySize - kMinAlignmentMask;  // '- kMinAlignmentMask' because the Malloc will effectively add it back without adding too much.

	// Ensure consolidation.
	if(GetFastBinChunksExist()) 
		ClearFastBins();

	// Allocate the aggregate chunk.
	// But first disable mmap so Malloc won't use it, since we would not be 
	// able to later free/realloc space internal to a segregated mmap region.
	const int nMMapMaxAllowedSaved = mnMMapMaxAllowed;
	mnMMapMaxAllowed = 0;
	pAggregateMemory = MallocInternal(nAggregateMemorySize, nAllocationFlags);
	mnMMapMaxAllowed = nMMapMaxAllowedSaved;

	if(pAggregateMemory)
	{
		pAggregateMemoryChunk = GetChunkPtrFromDataPtr(pAggregateMemory);
		PPM_ASSERT(this, !GetChunkIsMMapped(pAggregateMemoryChunk), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pAggregateMemoryChunk, pAggregateMemoryChunk); // If this fails, PPMalloc is at fault.
		nRemainderChunkSize = GetChunkSize(pAggregateMemoryChunk);

		if(pReturnArray == 0) // If the user was requesting that we allocate the return array ourselves...
		{
			pReturnArrayChunk   = GetChunkAtOffset(pAggregateMemoryChunk, (ssize_type)nChunkMemorySize);
			pReturnArray        = (void**)GetDataPtrFromChunkPtr(pReturnArrayChunk);
			SetChunkSize(pReturnArrayChunk, (nRemainderChunkSize - nChunkMemorySize) | kChunkFlagPrevInUse);
			nRemainderChunkSize = nChunkMemorySize;
		}

		// Take the aggregate chunk memory and build new chunks out of it. 
		for(i = 0; ; ++i)
		{
			pReturnArray[i] = GetDataPtrFromChunkPtr(pAggregateMemoryChunk);
			if(i < (nElementCount - 1))
			{
				nAggregateMemorySize = GetChunkSizeFromDataSize((size_type)pElementSizes[i]);
				nRemainderChunkSize -= nAggregateMemorySize;
				SetChunkSize(pAggregateMemoryChunk, nAggregateMemorySize | kChunkFlagPrevInUse);
				pAggregateMemoryChunk = GetChunkAtOffset(pAggregateMemoryChunk, (ssize_type)nAggregateMemorySize);
			}
			else
			{  
				PPM_ASSERT(this, nRemainderChunkSize > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault. // Note by Paul P. It seems to me that this assert will trigger under some conditions.
				SetChunkSize(pAggregateMemoryChunk, nRemainderChunkSize | kChunkFlagPrevInUse);
				break;
			}
		}

		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			if(pReturnArray != pResultArray) // If the user passed in NULL for pReturnArray and we needed to allocate the return ourselves...
			{
				// The final element must have exactly exhausted the chunk.
				PPM_ASSERT(this, nRemainderChunkSize == GetChunkSizeFromDataSize((size_type)pElementSizes[i]), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				DebugCheckUsedChunk(GetChunkPtrFromDataPtr(pReturnArray));
			}

			for(i = 0; i != nElementCount; ++i)
				DebugCheckUsedChunk(GetChunkPtrFromDataPtr(pReturnArray[i]));
		#endif
	}
	else
		pReturnArray = NULL;

	return pReturnArray;
}



///////////////////////////////////////////////////////////////////////////////
// ClearCache
//
void GeneralAllocator::ClearCache()
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex); // This is a user-level function, so we lock the mutex.
	#endif

	ClearFastBins();
}


///////////////////////////////////////////////////////////////////////////////
// ClearFastBins
//
void GeneralAllocator::ClearFastBins()
{
	#if PPM_FASTBINS_ENABLED
		Chunk**     ppFastBin;              // Current fastbin being consolidated.
		Chunk**     ppFastBinLast;          // Last FastBin (for loop control).
		Chunk*      pChunk;                 // Current chunk being consolidated.
		Chunk*      pNextChunkClear;        // Next chunk to clear.
		Chunk*      pBinUnsorted;           // Bin for newly freed chunks.
		Chunk*      pNextChunkContig;
		size_type   nChunkSize;
		size_type   nNextChunkSizeContig;
		size_type   nPrevChunkSizeContig;
		#if PPM_DEBUG_FILL
			size_type   nPrevCoalesceOffset;
		#endif

		// No mutex lock here because this is an internal function.
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, mpMutex); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
		#endif

		// We don't put high chunks into fast fins, so there is no special processing 
		// to do here for fast bins. Even if there were high chunks in these bins,
		// the processing we do here wouldn't likely be much or any different.

		if(mnMaxFastBinChunkSize) // If mnMaxFastBinChunkSize is 0, we know that we haven't been initialized.
		{
			pBinUnsorted = GetUnsortedBin();

			// We can't just free the chunks in the FastBins because free would
			// simply return the chuncks right back to the FastBins.

			// Remove each chunk from fast bin and consolidate it, placing it
			// then in unsorted bin. Among other reasons for doing this,
			// placing in unsorted bin avoids needing to calculate actual bins
			// until malloc is sure that chunks aren't immediately going to be
			// reused anyway.

			ppFastBinLast = mpFastBinArray + GetFastBinIndexFromChunkSize(mnMaxFastBinChunkSize);
			ppFastBin     = mpFastBinArray;

			do
			{ 
				pChunk = *ppFastBin;
				if(pChunk)
				{
					*ppFastBin = NULL; // We are clearing this bin, so we might as well set it to NULL now.

					do
					{
						#if (PPM_DEBUG >= 2) // Extended user debug builds.
							DebugCheckUsedChunk(pChunk);
						#endif
						pNextChunkClear = pChunk->mpNextChunk;
						pChunk->mnSize &= ~(size_type)(kChunkFlagInternal | kChunkFlagFastBin); // ClearChunkIsInternal(pChunk); ClearChunkIsFastBin(pChunk);

						// Slightly streamlined version of consolidation code in the free function
						nChunkSize           = GetChunkSize(pChunk);
						pNextChunkContig     = GetChunkAtOffset(pChunk, (ssize_type)nChunkSize);
						nNextChunkSizeContig = GetChunkSize(pNextChunkContig);
						PPM_ASSERT(this, nNextChunkSizeContig > 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

						// Try coalescing with the previous chunk in memory.
						#if PPM_DEBUG_FILL
							nPrevCoalesceOffset = 0;
						#endif
						if(!GetPrevChunkIsInUse(pChunk)) // If we can coalesce with the previous chunk in memory...
						{
							nPrevChunkSizeContig = pChunk->mnPriorSize;

							DebugMemFill(pChunk, mnFillFree, nChunkSize); // Overwrite the entire contents of this chunk with free fill.

							#if PPM_DEBUG_FILL
								nPrevCoalesceOffset = nPrevChunkSizeContig;
							#endif
							nChunkSize += nPrevChunkSizeContig;
							pChunk = GetChunkAtOffset(pChunk, -(ssize_type)nPrevChunkSizeContig);
							UnlinkChunkFromBin(pChunk);                                 // Remove the chunk from whatever list it may be in. This is OK if pChunk is the top chunk and is in its own list. Important to do this before changing the chunk's size and flags.
							SetChunkSize(pChunk, nChunkSize | kChunkFlagPrevInUse);
							pNextChunkContig->mnPriorSize = nChunkSize; // We can set the next chunk prev size field because we no longer own it, as we are being freed.
						}

						// Try coalescing with the next contiguous chunk in memory.
						const size_type bNextChunkContigIsUse = GetChunkInUseOffset(pNextChunkContig, (ssize_type)nNextChunkSizeContig);

						if(bNextChunkContigIsUse) // If the next contiguous chunk is in use...
						{
							// We may or may not have coalesced with the previous chunk above.
							// A DebugMemFill of fastbin data shouldn't be necessary, as it was already filled upon being added to the fastbin.
							// DebugMemFill(GetDataPtrFromChunkPtr(pChunk), mnFillFree, nChunkSize - kDataPtrOffset); // Write our user data area.
							#if PPM_DEBUG_FILL
								PPM_ASSERT(this, DebugMemCheck((char*)pChunk + sizeof(Chunk), nChunkSize - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
							#endif
							ClearChunkInUseOffset(pNextChunkContig, 0); // Clear pNextChunkContig's 'prev in use' flag (which refers to us)
							pNextChunkContig->mnPriorSize = nChunkSize; // We can set the next chunk prev size field because we no longer own it, as we are being freed.
						}
						else
						{  // Coalesce with the next contiguous chunk.
							UnlinkChunkFromBin(pNextChunkContig);  // Pull the next contiguous chunk out of whatever list we have it stored in.

							#if PPM_DEBUG_FILL
								DebugMemFill((char*)GetDataPtrFromChunkPtr(pChunk) + nPrevCoalesceOffset, mnFillFree, (nChunkSize - nPrevCoalesceOffset - kDataPtrOffset) + sizeof(Chunk)); // Write our user data area and also the Chunk part of the next contiguous chunk.
								PPM_ASSERT(this, DebugMemCheck(pNextChunkContig, nNextChunkSizeContig, mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pNextChunkContig, pNextChunkContig); // Make sure next contig chunk is already filled as expected, because we don't do it here. // If this fails, the user probably corrupted the heap.
							#endif

							nChunkSize += nNextChunkSizeContig;
							SetChunkSize(pChunk, nChunkSize | kChunkFlagPrevInUse);
							SetNextChunkPrevSize(pChunk, nChunkSize); // We can set the next chunk prev size field because we no longer own it, as we are being freed.
						}

						if((pChunk != mpTopChunk) && (pNextChunkContig != mpTopChunk)) // If the top chunk wasn't affected by any coalescing above...
							LinkChunk(pChunk, pBinUnsorted, pBinUnsorted->mpNextChunk);
						else //Else the newly freed chunk coalesced up or down with mpTopChunk.
						{
							#if PPM_DEBUG_FILL
								PPM_ASSERT(this, bNextChunkContigIsUse || DebugMemCheck((char*)pNextChunkContig + sizeof(Chunk), nNextChunkSizeContig - sizeof(Chunk), mnFillFree), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pNextChunkContig, pNextChunkContig); // Make sure next contig chunk is already filled as expected, because we don't do it here. // If this fails, the user probably corrupted the heap.
							#endif
							mpTopChunk = pChunk;
							AdjustTopChunk(mpTopChunk, nChunkSize);
						}

						#if (PPM_DEBUG >= 2) // Extended user debug builds.
							DebugCheckFreeChunk(pChunk);
						#endif

						pChunk = pNextChunkClear;

					} while(pChunk);
				}
			} while(ppFastBin++ != ppFastBinLast);

			SetFastBinChunksExist(false);
		}
		else
		{
			Init(NULL, 0);
			#if (PPM_DEBUG >= 2) // Extended user debug builds.
				DebugCheckState(kHeapValidationLevelBasic);
			#endif
		}

		PPM_ASSERT(this, !GetFastBinChunksExist(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

	#endif // #if PPM_FASTBINS_ENABLED
}



///////////////////////////////////////////////////////////////////////////////
// GetUsableSize
//
// Given an input pointer to allocated memory, returns the amount of memory
// that is usable by the user of that memory. It is possible that this amount
// is greater than the amount the user initially requested, but it can be 
// no less. 
// 
size_t GeneralAllocator::GetUsableSize(const void* pData)
{
	// We return -1 for NULL pointers and pointers to memory that isn't 
	// allocated. We could do more validation on the input pointer but that
	// isn't the business of this function. 
	if(pData)
	{
		const Chunk* const pChunk     = GetChunkPtrFromDataPtr(pData);
		const size_type    nChunkSize = GetChunkSize(pChunk);

		// If the MMapped bit is set then we know the chunk is in use and
		// thus don't need to check the in-use flag.
		if(GetChunkIsMMapped(pChunk))
		{
			// Note: This calculation is correct, but do recall that it is possible
			// that we used the mnPriorSize field of the chunk to indicate any bytes
			// that are before the chunck but part of the mmaped block. These don't
			// affect this calculation but do affect other calculations.
			return nChunkSize - kDataPtrOffset; // '- kDataPtrOffset' because the chunk has leading mnPriorSize and mnSize. Unlike regular chunks, MMap chunks don't creep into the next contiguous chunk.
		}
		else if(GetChunkIsInUse(pChunk))
		{
			#if PPM_DEBUG_PRESERVE_PRIOR // In debug mode we reserve space for the mnPriorSize field of the next contiguous chunk.
				return (nChunkSize - kDataPtrOffset); // '- kDataPtrOffset' because the chunk has the mnPriorSize and mnSize reserved front members.
			#else
				return (nChunkSize - kDataPtrOffset) + kSizeTypeSize;  // '+ kSizeTypeSize' because the we go into the mnPriorSize field of the next chunk.
			#endif
		}
	}
	return (size_t)-1;
}


size_t GeneralAllocator::GetBlockSize(const void* pData, bool bNetSize)
{
	// Note that we simply return the size of the chunk. It happens that 
	// in an optimized build that chunks extend by size_type bytes into 
	// the chunk after them. Thus the sum of chunk sizes will be greater 
	// than the sum of used memory.
	const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
	size_t nChunkSize = GetChunkSize(pChunk);
	if(bNetSize)
	{
		#if PPM_DEBUG_PRESERVE_PRIOR
			nChunkSize -= kSizeTypeSize;
		#endif
	}
	return nChunkSize;
}


size_t GeneralAllocator::GetLargestFreeBlock(bool bClearCache)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex); // This is a user-level function, so we lock the mutex.
	#endif

	size_t nLargest = 0;

	if(mpTopChunk) // If we have been initialized...
	{
		if(bClearCache)
			ClearFastBins();

		// Check the top chunk. It will often be the biggest chunk.
		nLargest = GetChunkSize(mpTopChunk);

		// Check the chunk bins. These are sorted by size (largest to smallest)
		// and so we need only check the first chunk in the last bin.
		// Any size chunk can possibly be in the regular bins.
		for(size_t i((kBinCount) - 1); i >= 1; --i)
		{
			const SkipListChunk* const pBin = GetBin((int)i);
			if(pBin->mpNextChunk != pBin) // If this bin is non-empty get the size of the first chunk in the list
			{
				const size_t n = GetChunkSize(pBin->mpNextChunk);
				if(n > nLargest)
					nLargest = n;
				break;
			}
		}

		// Check the unsorted bin. These chunks are not sorted with respect to size.
		// Any size chunk can possibly be in the unsorted bin.
		const SkipListChunk* const pBegin = GetUnsortedBin();
		for(const SkipListChunk* pChunk = (SkipListChunk*)pBegin->mpNextChunk; pChunk != pBegin; pChunk = (SkipListChunk*)pChunk->mpNextChunk)
		{
			const size_t n = GetChunkSize(pChunk);
			if(n > nLargest)
				nLargest = n;
		}

		#if PPM_FASTBINS_ENABLED
			// Possibly check fast bins
			// Only chunks below a certain size can be in fast bins.
			// All chunks in a given fast bin are the same size.
			if(!bClearCache && (nLargest < mnMaxFastBinChunkSize))
			{
				for(int i = (kFastBinCount - 1); i >= 0; i--)
				{
					const Chunk* pChunk = mpFastBinArray[i];
					if(pChunk)
					{
						const size_t n = GetChunkSize(pChunk);
						if(n > nLargest)
							nLargest = n;
						break;
					}
				}
			}
		#endif
	}

	return nLargest;
}


void GeneralAllocator::LinkCoreBlock(CoreBlock* pCoreBlock, CoreBlock* pNext)
{
	pCoreBlock->mpNextCoreBlock = pNext;
	pCoreBlock->mpPrevCoreBlock = pNext->mpPrevCoreBlock;
	pNext->mpPrevCoreBlock      = pCoreBlock;
	pCoreBlock->mpPrevCoreBlock->mpNextCoreBlock = pCoreBlock;
}


void GeneralAllocator::UnlinkCoreBlock(CoreBlock* pCoreBlock)
{
	pCoreBlock->mpPrevCoreBlock->mpNextCoreBlock = pCoreBlock->mpNextCoreBlock;
	pCoreBlock->mpNextCoreBlock->mpPrevCoreBlock = pCoreBlock->mpPrevCoreBlock;
}


GeneralAllocator::CoreBlock* GeneralAllocator::FindCoreBlockForAddress(const void* pAddress) const
{
	for(CoreBlock* pCoreBlock = mHeadCoreBlock.mpPrevCoreBlock; 
		 pCoreBlock != &mHeadCoreBlock; 
		 pCoreBlock = pCoreBlock->mpPrevCoreBlock)
	{
		#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)
			if((pAddress >= (void*)pCoreBlock) && (pAddress < ((char*)pCoreBlock + pCoreBlock->mnSize)))
				return pCoreBlock;
		#else // Faster to do it this way on non-desktop processors.
			if(((uintptr_t)pAddress - (uintptr_t)pCoreBlock) < (uintptr_t)pCoreBlock->mnSize)
				return pCoreBlock;
		#endif
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetAddCoreFunction
//
void GeneralAllocator::SetAddCoreFunction(CoreAddFunction pCoreAddFunction, void* pCoreAddFunctionContext)
{
	mCoreAddFunction         = pCoreAddFunction;
	mpCoreAddFunctionContext = pCoreAddFunctionContext;
}


///////////////////////////////////////////////////////////////////////////////
// AddCoreEx
//
// This is a user-level function which allows the user to add additional 
// usable core memory for use. For embedded systems it is best if this is 
// called once on startup with all memory that will be needed.
//
// Win32: If you pass in core with bShouldFreeCore as true, but don't pass
//          in a pCoreFreeFunction, the core must be memory obtained via
//          VirtualAlloc and be either committed (MEM_COMMIT) or reserved (MEM_RESERVE).
//          Under any other conditions, the passed in core must be committed
//          memory (i.e. able to write to now).
// 
bool GeneralAllocator::AddCoreEx(void* pCore, size_t nCoreSize, uint32_t coreType, size_t nReservedSize, 
								 bool bShouldFreeCore, bool bShouldTrimCore, 
								 CoreFreeFunction pCoreFreeFunction, void* pCoreFreeFunctionContext,
								 CoreExtendFunction pCoreExtendFunction, void* pCoreExtendFunctionContext)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	// It's nonsensical to request core memory but request it to be of no type. Use kCoreTypeDefault to request a default type of core memory.
	PPM_ASSERT(this, coreType != kCoreTypeNone, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.

	if(pCore)
	{
		PPM_ASSERT(this, nCoreSize >= (4 * kMinChunkSize), kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.

		if(nCoreSize >= (4 * kMinChunkSize))
		{
			#if PPM_DEBUG_FILL
				DebugMemFill(pCore, mnFillFree, nCoreSize);
			#endif

			// Core must correctly align.
			PPM_ASSERT(this, PPM_IsAligned(pCore, kMinEffectiveAlignment), kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.

			// Core needs to be a multiple of the system page size for some systems.
			if(nCoreSize > mnPageSize)
			{
				if(nCoreSize % mnPageSize)
					nCoreSize &= ~(mnPageSize - 1); // Round down to nearest page size.
			} // Else we assume the user intentionally wants a small core size of their own management.

			// The following has been disabled because it's possible with some settings that nCoreSize will
			// be chopped to a 24 bit boundary, which isn't a power of two. This interferes with the possibility
			// of extending core.
			//
			// Core size needs to be a multiple of the min chunk size.
			// Note that the min chunk size varies, depending on whether the system is 32 bit or 64 bit.
			//if(nCoreSize % kMinChunkSize)
			//    nCoreSize = (nCoreSize / kMinChunkSize) * kMinChunkSize;

			#if defined(EA_PLATFORM_WINDOWS)
				// If bShouldTrimCore is true, then we will be calling VirtualAlloc(MEM_COMMIT and MEM_RESERVE) with this
				// block over time, and in that case it needs to be a multiple of 65536.
				PPM_ASSERT(this, !bShouldTrimCore || ((nCoreSize % 65536) == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCore); // If this fails, it is probably PPMalloc's fault.
			#endif

			// We provide the following coreType adjustment for the purpose of backwards-compatibility  
			// with the original AddCore function, which didn't have a CoreType argument.
			if(pCoreFreeFunction && ((coreType == kCoreTypeNone) || (coreType == kCoreTypeDefault)))
				coreType = kCoreTypeUserDefined;

			// Create a core block for this memory.
			CoreBlock* const pCoreBlock = (CoreBlock*)pCore;

			pCoreBlock->mpCore                      = PPM_AlignUp(pCoreBlock + 1, kMinEffectiveAlignment); // Make mpCore point to right after the CoreBlock header.
			pCoreBlock->mnSize                      = nCoreSize;
			pCoreBlock->mnMemBlockId                = 0;
			pCoreBlock->mCoreType                   = coreType;
			pCoreBlock->mnReservedSize              = nReservedSize;
			pCoreBlock->mbMMappedMemory             = false;
			pCoreBlock->mbShouldFree                = bShouldFreeCore;
			pCoreBlock->mbShouldFreeOnShutdown      = bShouldFreeCore;
			pCoreBlock->mbShouldTrim                = bShouldTrimCore;
			pCoreBlock->mbPageExecutable            = false;  // By default. May set it to true later.
			pCoreBlock->mpCoreFreeFunction          = pCoreFreeFunction;
			pCoreBlock->mpCoreFreeFunctionContext   = pCoreFreeFunctionContext;
			pCoreBlock->mpCoreExtendFunction        = pCoreExtendFunction;
			pCoreBlock->mpCoreExtendFunctionContext = pCoreExtendFunctionContext;
 
			LinkCoreBlock(pCoreBlock, &mHeadCoreBlock);

			mPhysicalFootprint += pCoreBlock->mnSize;
			mNumCores++;

			#if PPM_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
					const HookInfo hookInfo = { this, false, kHookTypeCoreAlloc, kHookSubTypeNone, nCoreSize, NULL, 0, nCoreSize, NULL, 0, 0, pCoreBlock->mpCore, NULL, nCoreSize, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif

			#if !PPM_MALLOC_AS_COREALLOC
				if(!pCoreFreeFunction) // If the user supplies a core free function, then the memory can be any kind from any source.
				{
					// In this case the memory must be either reserved or committed. Note that if the user
					// passes in a block of memory that is obtained by malloc or HeapAlloc, then the
					// GetMemoryState() call below will see the memory as committed and things are OK. 
					// If the user passes in memory obtained by malloc or HealAlloc, then the user 
					// must set bShouldTrimCore to false or else bad things can happen. 

					CoreMemoryAllocator::CoreMemoryState memstate = CoreMemoryAllocator::GetMemoryState(pCore);

					#if defined(EA_PLATFORM_WINDOWS)
					   PPM_ASSERT(this, (memstate == CoreMemoryAllocator::kCoreMemTypeReserved) || (memstate == CoreMemoryAllocator::kCoreMemTypeCommitted), kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.
					#endif

					if (memstate == CoreMemoryAllocator::kCoreMemTypeReserved) // If the user passed in memory allocated by VirtualAlloc and that memory
						pCoreBlock->mnSize = 0;    // is reserved, we note this and set our committed size (mnSize) to zero.
				}
			#endif

			// Make a chunk out of the new core
			Chunk* const pChunk = MakeChunkFromCore(pCoreBlock->mpCore, nCoreSize - (size_t)(pCoreBlock->mpCore - (char*)pCoreBlock), kChunkFlagPrevInUse);
			SetNewTopChunk(pChunk, mpTopChunk != GetInitialTopChunk()); // Second argument to SetNewTopChunk is 'bool bFreePreviousTopChunk'.
			return true;
		}
	}
	else if(nCoreSize > 0)
	{
		// In this case, no core memory was passed to us, but we were 
		// requested to allocate a given amount of core by our own means.
		// nCoreSize = PPM_AlignUp(nCoreSize, mnPageSize); Removed because AddCoreInternal already does this.

		Chunk* const pChunk = AddCoreInternal(nCoreSize, false, true);
		if(pChunk)
		{
			SetNewTopChunk(pChunk, true); // Second argument to SetNewTopChunk is 'bool bFreePreviousTopChunk'.
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// CallMallocFailureFunction
//
bool GeneralAllocator::CallMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		int nSavedLockCount = 0;

		if(mpMutex)
		{
			nSavedLockCount = PPMMutexGetLockCount(mpMutex); // We have the mutex locked, so this is safe to call.
			for(int i = 0; i < nSavedLockCount; i++)
				PPMMutexUnlock(mpMutex);
		}
	#endif

	// It is risky to call an internal function while our mutex is locked.
	// Call the malloc failure function, passing it nSize + some extra with the assumption 
	// that MallocFailureFunction will use the passed in value to create new core.
	// Alternatively, the MallocFailureFunction may want to free allocated but otherwise
	// not strictly needed memory.

	// This version just calls mpMallocFailureFunction, but a GeneralAllocator subclass might do something more.
	const bool result = mpMallocFailureFunction(pGeneralAllocator, nMallocRequestedSize, nAllocatorRequestedSize, pContext);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
		{
			for(int i = 0; i < nSavedLockCount; i++)
				PPMMutexLock(mpMutex);
		}
	#endif

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// ExtendCoreInternal
//
// Extends the core by at least nMinSize and returns a chunk of nMinSize.
// The returned chunk is itself not marked as in-use.
//
GeneralAllocator::Chunk* GeneralAllocator::ExtendCoreInternal(size_t nMinSize)
{
	// If the system supports extending of core as an alternative to creating
	// new core, then we attempt to do so. Note that Windows can do this via
	// the reserved memory mechanism and Unix can do this via brk extension.    
	// Other systems, such as those that use malloc as the system allocation
	// scheme, cannot extend core, as each core block is a fully committed 
	// an non-resizable entity.
	bool      bShouldSetNewTopChunk = false;
	Chunk*    pChunk = NULL;
	size_type nNewChunkSize = 0;

	{
		// We see if there are any core blocks that have >= nSize of reserved
		// memory that can be used. If so, then we use it.
		void*      pExtendedCore = NULL;
		size_t     nExtendedSize = 0;
		CoreBlock* pCoreBlock    = mHeadCoreBlock.mpPrevCoreBlock;

		while(pCoreBlock != &mHeadCoreBlock) // For each existing core block...
		{
			if ((pCoreBlock->mCoreType == kCoreTypeMMap) ||
				(pCoreBlock->mCoreType >= kCoreTypeUserDefined)) // If its an extendable type of memory...
			{
				if (nMinSize <= (pCoreBlock->mnReservedSize - pCoreBlock->mnSize)) // If this block has enough reserved space to satisfy this request... (also, this expression will be false in the case that user supplied core manually)
				{
					if (pCoreBlock->mpCoreExtendFunction)
					{
						size_t nMinExtendedSize = nMinSize; // To consider: Bump nMinExtendedSize up to some increment.
						nExtendedSize = pCoreBlock->mpCoreExtendFunction(this, pCoreBlock, pCoreBlock->mnSize, nMinExtendedSize, pCoreBlock->mnReservedSize, pCoreBlock->mpCoreExtendFunctionContext);

						if (nExtendedSize)
						{
							mPhysicalFootprint += nExtendedSize;
							pExtendedCore = reinterpret_cast<char*>(pCoreBlock)+pCoreBlock->mnSize;
							break;
						}
					}
					else
					{
						mPhysicalFootprint += nExtendedSize;
						pExtendedCore = DefaultCoreExtendFunction(pCoreBlock, nMinSize, nExtendedSize);
						break;
					}
				}
			}

			pCoreBlock = pCoreBlock->mpPrevCoreBlock;
		} // while loop


		if(pExtendedCore) // If a suitable core block was found (and extended) above...
		{
			if (mRangeCallback)
				(*mRangeCallback)(mRangeUserData, pExtendedCore, nExtendedSize, true);

			DebugMemFill(pExtendedCore, mnFillFree, nExtendedSize);

			if(pCoreBlock->mnSize) // If this block already has chunks in it...
			{
				// We need to either create a new chunk or extend the last chunk, 
				// depending on whether the last chunk is in use or not.
				Chunk* const pFenceChunk = GetFenceChunk(pCoreBlock);

				if(GetPrevChunkIsInUse(pFenceChunk))
				{
					// Make a new chunk and move the fence chunk to the end.
					pChunk        = pFenceChunk;
					nNewChunkSize = (size_type)nExtendedSize;
					SetChunkSize(pChunk, nNewChunkSize | kChunkFlagPrevInUse);
					PPM_ASSERT(this, (mpTopChunk < pChunk) || (mpTopChunk > pFenceChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk); // If this fails, the user probably corrupted the heap.
				}
				else
				{
					// Merge with the prior unused chunk. If it is currently the top chunk,
					// it should have a size < (nMinSize + kMinAllocSize). Otherwise, it should
					// have a size < nMinSize. Larger chunks would have been used before we
					// got here. But we leave the possibility open and don't assert here.
					pChunk = GetChunkAtOffset(pFenceChunk, -(long)pFenceChunk->mnPriorSize);
					UnlinkChunkFromBin(pChunk); // Remove this Chunk from whatever list it is in.
					nNewChunkSize = (size_type)(pFenceChunk->mnPriorSize + nExtendedSize); // mnPriorSize should be a valid field, because the previous chunk is not in use.
					SetChunkSize(pChunk, nNewChunkSize | kChunkFlagPrevInUse);
					DebugMemFill(pFenceChunk, mnFillFree, kDoubleFenceChunkSize);
					bShouldSetNewTopChunk = (pChunk == mpTopChunk);
				}

				SetFencepostAfterChunk(pChunk, nNewChunkSize);
			}
			else // Else this core block was completely empty.
			{
				// Create a completely new and independent chunk from this core.
				PPM_ASSERT(this, false, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // As of this writing, this pathway should no longer execute (because core blocks always have at least the header). We leave this assert here to verify so while verifying the new code. // If this fails, PPMalloc is at fault.

				pCoreBlock->mpCore = PPM_AlignUp(pCoreBlock + 1, kMinEffectiveAlignment); // Make mpCore point to right after the CoreBlock header.
				pChunk             = MakeChunkFromCore(pCoreBlock->mpCore, nExtendedSize - (size_t)(pCoreBlock->mpCore - (char*)pCoreBlock), kChunkFlagPrevInUse);
				nNewChunkSize      = (size_type)nExtendedSize;
				SetNewTopChunk(pChunk, true); // Second argument here is 'bool bFreePreviousTopChunk'.
				bShouldSetNewTopChunk = true;
			}

			PPM_ASSERT(this, nExtendedSize > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
			#if PPM_HOOKS_SUPPORTED
				const size_t nOriginalSize = pCoreBlock->mnSize; // Save this because we'll need it below.
			#endif
			pCoreBlock->mnSize += nExtendedSize;

			#if PPM_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
					const HookInfo hookInfo = { this, false, kHookTypeCoreExtended, kHookSubTypeNone, nOriginalSize, pCoreBlock->mpCore, 0, nOriginalSize, NULL, 0, 0, pCoreBlock->mpCore, NULL, nExtendedSize, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif
		}
	}


	if(!pChunk) // If we were unable to extend core, try to allocate new core from the system. 
	{
		size_t nCoreSize = nMinSize;
			  
		// This is what the size would be internally in AddCoreInternal when calculating size to take from system
		const size_t nCoreSizePlusOverhead = (nCoreSize + sizeof(CoreBlock) + kDoubleFenceChunkSize);

		// When taking new cores using the new core option we don't want to add extra overhead since that will take an extra page
		const bool bAddOverheadToMinSize = (nCoreSizePlusOverhead > mnNewCoreSize);

		if(!bAddOverheadToMinSize)
			nCoreSize = PPM_AlignUp(mnNewCoreSize, mnPageSize);

		pChunk = AddCoreInternal(nCoreSize, bAddOverheadToMinSize, false);

		if(pChunk)
		{
			nNewChunkSize = GetChunkSize(pChunk);
			PPM_ASSERT(this, nNewChunkSize >= nMinSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault due to the logic above being faulty.

			if(nNewChunkSize > (nMinSize + kMinChunkSize))  // If there will be remaining space for a new top chunk...
			{
				SetNewTopChunk(pChunk, true);   // Second argument here is 'bool bFreePreviousTopChunk'.
				bShouldSetNewTopChunk = true;
			}
		}
	}

	PPM_ASSERT(this, !pChunk || !GetChunkIsInUse(pChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

	// Possibly split the new chunk into two chunks.
	if(pChunk)
	{
		if(nNewChunkSize >= (nMinSize + kMinChunkSize)) // If the new chunk has more space than we need (and enough to create a new chunk after it)... split it.
		{
			SetChunkSize(pChunk, (size_type)(nMinSize | kChunkFlagPrevInUse)); // By design the previous chunk must always be in-use, else pChunk would be coalesced with it.

			Chunk* const    pRemainderChunk     = GetChunkAtOffset(pChunk, (ssize_type)nMinSize);
			const size_type nRemainderChunkSize = (size_type)(nNewChunkSize - nMinSize);

			pRemainderChunk->mnPriorSize = (size_type)nMinSize;
			SetChunkSize(pRemainderChunk, nRemainderChunkSize);         // Leave kChunkFlagPrevInUse off because we don't want the return chunk to be flagged as in-use.
			SetNextChunkPrevSize(pRemainderChunk, nRemainderChunkSize); // Leave kChunkFlagPrevInUse off because we will be freeing it below.

			if(bShouldSetNewTopChunk)
				SetNewTopChunk(pRemainderChunk, false); // Second argument here is 'bool bFreePreviousTopChunk'.
			else
			{
				Chunk* const pUnsortedBinHeadChunk = GetUnsortedBin();
				LinkChunk(pRemainderChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
			}

			#if (PPM_DEBUG >= 2) // Extended user debug builds.
				SetChunkInUseOffset(pChunk, (ssize_type)nMinSize);   // Do this just so the DebugCheckFreeChunk call doesn't thin we have two contiguous free chunks.
				DebugCheckFreeChunk(pRemainderChunk);
				ClearChunkInUseOffset(pChunk, (ssize_type)nMinSize); // We turn off the in-use flag because that's what this function calls for.
			#endif
		}

		PPM_ASSERT(this, !GetChunkIsInUse(pChunk), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // If this fails, the user probably corrupted the heap.

		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			SetChunkInUseOffset(pChunk, (ssize_type)nMinSize);   // Temporarily set in-use to true here so that the heap check below won't think it is an unaccounted for free chunk.
			DebugCheckState(kHeapValidationLevelBasic);          // This check will expect that pChunk is marked as in-use, otherwise the chunk would appear to be lost. 
			ClearChunkInUseOffset(pChunk, (ssize_type)nMinSize); // We turn off the in-use flag because that's what this function calls for.
		#endif
	}

	return pChunk;
}


///////////////////////////////////////////////////////////////////////////////
// AddCoreInternal
//
// If bAddOverheadToMinSize is true then we add core block overhead size as required
// for the creation of a new block. If bAddOverheadToMinSize is false then we assume
// that the caller wants a core block of exactly nMinSize (rounded up to the 
// page size, which always must be so). 
//
GeneralAllocator::Chunk* GeneralAllocator::AddCoreInternal(size_t nMinSize, bool bAddOverheadToMinSize, bool bCommitAll)
{
	void*     pCore        = NULL;
	uintptr_t memBlockId   = 0;                     // This is used by only some platforms.
	uint32_t  coreType     = kCoreTypeDefault;      // To consider: Provide a user Option to explicitly control the CoreType of newly added core memory.
	size_type nChunkFlags  = kChunkFlagPrevInUse;
	bool      bPageExecute = false;                 // True if the core memory was allocated as executable. False by default here but may be changed below.

	PPM_ASSERT(this, nMinSize <= kMaxCoreSize, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, NULL); // If this fails, the user specified a core memory block that was too big.

	if(bAddOverheadToMinSize)
		nMinSize = PPM_AlignUp(sizeof(CoreBlock) + nMinSize + kDoubleFenceChunkSize, mnPageSize); // Enough for a CoreBlock header, the nMinSize Chunk the caller needs this core for, and a "double-fence" core block tail (all CoreBlocks have a double fence at the end.

	size_t             nCoreSize                 = nMinSize;
	const size_t       nCoreSizeSaved            = nCoreSize; // Save the original request size for possible use in MMap allocation below.
	size_t             nReservedSize             = 0;
	CoreExtendFunction coreExtendFunction        = NULL;
	void*              coreExtendFunctionContext = NULL;
	CoreFreeFunction   coreFreeFunction          = NULL;
	void*              coreFreeFunctionContext   = NULL;

	EA_UNUSED(nCoreSizeSaved); // Prevent possible compiler warning regarding non-use.
	EA_UNUSED(bCommitAll);
	EA_UNUSED(nReservedSize);


	if (mCoreAddFunction)
	{
		nReservedSize = PPM_AlignUp(nCoreSize, 65536);
		coreType = kCoreTypeUserDefined;   // May be overwritten in the mCoreAddFunction call. We set it here just to make sure it's at least the most safe value.
		pCore = (*mCoreAddFunction)(&nCoreSize, &nReservedSize, &coreFreeFunction, &coreFreeFunctionContext, &coreExtendFunction, &coreExtendFunctionContext, coreType, mpCoreAddFunctionContext);
		PPM_ASSERT(this, pCore, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, the system is probably exhausted of memory or address space.
	}
	else
	{
		pCore = DefaultAddCoreMemoryFunction(nMinSize, bCommitAll, nReservedSize, nCoreSize, coreType, bPageExecute);
	}

	#if PPM_MMAP_SUPPORTED
		if(!pCore)
		{
			// Use mapped memory fallback.
			nCoreSize = nCoreSizeSaved;
			pCore     = MMapAllocInternal(nCoreSize, &nCoreSize);

			if(pCore)
			{
				coreType = kCoreTypeMMap;

				// nChunkFlags |= kChunkFlagMMapped;     <-- Removed by Paul Pedriana, June 2011. While the core block is memory-mapped and should be freed via MMapFreeInternal, the chunk returned to the user isn't an "mmapped chunk" which is freed via MMapFreeInternal.
				#if defined(EA_PLATFORM_WINDOWS) && !PPM_MALLOC_AS_COREALLOC
					nReservedSize = nCoreSize;
				#endif
			}
		}
	#endif // PPM_MMAP_SUPPORTED
	// Possibly enable this.
	//if(!pCore)
	//{   
	//    // Final malloc fallback.
	//    for(pCore = malloc(nCoreSize); !pCore && (nCoreSize >= nMinSize); pCore = malloc(nCoreSize))
	//        nCoreSize = nCoreSize * 3 / 4;
	//    if(pCore)
	//        coreType = kCoreTypeMalloc;
	//}

	if(pCore)
	{
		if (mRangeCallback)
			(*mRangeCallback)(mRangeUserData, pCore, nCoreSize, true);

		DebugMemFill(pCore, mnFillFree, nCoreSize);

		CoreBlock* const pCoreBlock = (CoreBlock*)pCore;

		pCoreBlock->mpCore                      = PPM_AlignUp(pCoreBlock + 1, kMinEffectiveAlignment); // Make mpCore point to right after the CoreBlock header.
		pCoreBlock->mnSize                      = nCoreSize;
		pCoreBlock->mnMemBlockId                = memBlockId;
		pCoreBlock->mCoreType                   = coreType;
		pCoreBlock->mnReservedSize              = nReservedSize;
		pCoreBlock->mbMMappedMemory             = ((nChunkFlags & kChunkFlagMMapped) != 0);
		pCoreBlock->mbShouldFree				= (mHeadCoreBlock.mpNextCoreBlock != &mHeadCoreBlock) || mbTrimToZero; // For console projects where you want to disable freeing, you really should set core properties manually via AddCore.
		pCoreBlock->mbShouldFreeOnShutdown      = true; // Any memory we allocate ourselves will always be freed on shutdown.
		pCoreBlock->mbShouldTrim                = (pCoreBlock->mbShouldFree) && (coreType == kCoreTypeMMap); // If this is the first core block being added to the list, then disable trimming. Only MMap memory can be trimmed (partially deallocated), and even then not all MMap memory can be.
		pCoreBlock->mbPageExecutable            = bPageExecute;
		pCoreBlock->mpCoreFreeFunction          = coreFreeFunction;
		pCoreBlock->mpCoreFreeFunctionContext   = coreFreeFunctionContext;
		pCoreBlock->mpCoreExtendFunction        = coreExtendFunction;
		pCoreBlock->mpCoreExtendFunctionContext = coreExtendFunctionContext;

		LinkCoreBlock(pCoreBlock, &mHeadCoreBlock);

		mPhysicalFootprint += pCoreBlock->mnSize;
		mNumCores++;

		// pCoreBlock->mCoreType must refer to a specific type of core memory, so we can know how free it later.
		// It can't refer to one of the non-type kCoreTypeNone or pseudo-type kCoreTypeDefault. 
		PPM_ASSERT(this, (pCoreBlock->mCoreType != kCoreTypeNone) && (pCoreBlock->mCoreType != kCoreTypeDefault), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCoreBlock); // If this fails, it is the PPMalloc's fault.

		#if defined(EA_PLATFORM_WINDOWS) && PPM_MMAP_SUPPORTED
			// If bShouldTrimCore is true, then we will be calling VirtualAlloc(MEM_COMMIT and MEM_RESERVE) with this
			// block over time, and in that case it needs to be a multiple of 65536.
			PPM_ASSERT(this, !pCoreBlock->mbShouldTrim || ((nCoreSize % 65536) == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCoreBlock); // If this fails, it is the PPMalloc's fault.
		#endif

		#if defined(EA_PLATFORM_WINDOWS) && !PPM_MALLOC_AS_COREALLOC
			// CoreBlock::mnSize refers to the amount of committed (and thus usable) memory in the block. 
			// CoreBlock::mnReservedSize refers to the amount of reserved memory in the block.
			pCoreBlock->mnReservedSize = nReservedSize;
		#endif

		if(!mbHighFenceInternallyDisabled && !GetCoreIsContiguous()) // If high fence was previously enabled and we have 2 or more core blocks...
		{
			// Disable high/low memory distinction. Since we will have multiple core 
			// blocks or resized core blocks, high vs. low has somewhat lost its meaning.
			mbHighFenceInternallyDisabled = true;
			mpHighFence = NULL;
		}

		#if PPM_HOOKS_SUPPORTED
			if(mpHookFunction)
			{
				// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
				const HookInfo hookInfo = { this, false, kHookTypeCoreAlloc, kHookSubTypeNone, nCoreSize, NULL, 0, nCoreSize, NULL, 0, 0, pCoreBlock->mpCore, NULL, nCoreSize, 0 };
				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#endif

		Chunk* const pChunk = MakeChunkFromCore(pCoreBlock->mpCore, nCoreSize - (size_t)(pCoreBlock->mpCore - (char*)pCoreBlock), nChunkFlags);
		return pChunk;
	}

	return NULL;
}



///////////////////////////////////////////////////////////////////////////////
// FreeCore
//
// Returns true if the memory was freed.
//
bool GeneralAllocator::FreeCore(CoreBlock* pCoreBlock, bool bInShutdown)
{
	bool bReturnValue = false;

	PPM_ASSERT(this, pCoreBlock != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCoreBlock); // If this fails, it is the PPMalloc's fault.

	if (pCoreBlock && (pCoreBlock->mbShouldFree || (bInShutdown && (pCoreBlock->mbShouldFreeOnShutdown))))
	{

		if (mRangeCallback)
			(*mRangeCallback)(mRangeUserData, pCoreBlock, pCoreBlock->mnSize, false);

		if (pCoreBlock->mpCoreFreeFunction)
		{
			// The CoreType should be a user-defined type.
			PPM_ASSERT(this, pCoreBlock->mCoreType >= kCoreTypeUserDefined, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, pCoreBlock); // If this fails, it is the PPMalloc's fault.

			#if defined(EA_PLATFORM_WINDOWS) && !PPM_MALLOC_AS_COREALLOC
				(void)pCoreBlock->mpCoreFreeFunction(this, pCoreBlock, pCoreBlock->mnReservedSize, pCoreBlock->mpCoreFreeFunctionContext);
			#else
				(void)pCoreBlock->mpCoreFreeFunction(this, pCoreBlock, pCoreBlock->mnSize, pCoreBlock->mpCoreFreeFunctionContext);
			#endif

			// We intentionally ignore the CoreFreeFunction return value. The reason why is that we are going to proceed with disposing 
			// of the block regardless of whether the user could free it. We might want to consider using the user return value, but if 
			// the user screws up in any way and returns the wrong value, there could be a very rare scary crash. So for now it's the user's
			// responsibility to do the right thing in that callback above.
			bReturnValue = true;
		}
		else
		{
			#if PPM_INTERNAL_CORE_FREE_ENABLED
				bReturnValue = DefaultCoreFreeFunction(pCoreBlock);
			#endif
		} // if(mpCoreFreeFunction)
	} // if(pCoreBlockEx->mbShouldFree)

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// TrimCore
//
// This is a user-level function which trims the core memory usage.
//
size_t GeneralAllocator::TrimCore(size_t nPadding)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	size_t nCoreTrimmed = 0;

	for(CoreBlock* pCoreBlock = mHeadCoreBlock.mpPrevCoreBlock; 
		 pCoreBlock != &mHeadCoreBlock; 
		 pCoreBlock = pCoreBlock->mpPrevCoreBlock)
	{
		if(pCoreBlock->mbShouldFree)
		{
			const Chunk* const pFenceChunk = GetFenceChunk(pCoreBlock);

			if(!GetPrevChunkIsInUse(pFenceChunk)) // If the highest chunk is unused... (we can't do anything unless at least the previous chunk is free)
			{
				Chunk* const pHighestChunk = GetPrevChunk(pFenceChunk);

				// If core is fully free, then free it. 
				if((char*)pHighestChunk < ((char*)pCoreBlock->mpCore + kMinChunkSize)) // If the highest chunk is only chunk... (Don't compare to == mpCore because it's possible mpCore wasn't aligned perfectly for chunk storage.)
				{
					#if PPM_HOOKS_SUPPORTED
						if(mpHookFunction)
						{
							// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
							const HookInfo hookInfo = { this, true, kHookTypeCoreFree, kHookSubTypeNone, pCoreBlock->mnSize, pCoreBlock->mpCore, 0, pCoreBlock->mnSize, NULL, 0, 0, NULL, NULL, pCoreBlock->mnSize, 0 };
							mpHookFunction(&hookInfo, mpHookFunctionContext);
						}
					#endif

					UnlinkChunkFromBin(pHighestChunk); // Remove pHighestChunk from whatever list(s) it is in.

					// Unlink the core block for the following operations, as it will likely be freed below.
					CoreBlock* const pCoreBlockSaved = pCoreBlock;
					pCoreBlock = pCoreBlock->mpNextCoreBlock; // Need to assign this so we can continue the outer loop.
					UnlinkCoreBlock(pCoreBlockSaved);
					const size_t nCoreBlockSize = pCoreBlockSaved->mnSize;

					mPhysicalFootprint -= pCoreBlockSaved->mnSize;
					mNumCores--;

					// Free the core block core memory. It will only be freed if the freeing is enabled for the core block.
					if(FreeCore(pCoreBlockSaved, false))
					{
						// Update statistics.
						nCoreTrimmed += nCoreBlockSize;

						// Possibly find a new mpTopChunk from the existing core blocks.
						if(pHighestChunk == mpTopChunk)
						{
							mpTopChunk = GetInitialTopChunk();
							FindAndSetNewTopChunk();
						}
					}
					else
					{
						// The free failed, so retain this block. 
						// Undo the unlinking operations above.
						PlaceUnsortedChunkInBin(pHighestChunk, GetChunkSize(pHighestChunk), false);        

						// Place the saved core block back in the linked list behind the new pCoreBlock (so it won't get processed again).
						LinkCoreBlock(pCoreBlockSaved, pCoreBlock);
						pCoreBlock = pCoreBlockSaved; // It's important that we do this so we don't get into an infinite loop.

						mPhysicalFootprint += pCoreBlockSaved->mnSize;
						mNumCores++;
					}

					#if (PPM_DEBUG >= 2) // Extended user debug builds.
						DebugCheckState(kHeapValidationLevelBasic);
					#endif

					continue; // Go on to processing the next core block.
				}


				// If core is partially free, perhaps we can trim it.
				if(pCoreBlock->mbShouldTrim)
				{
					if(pCoreBlock->mCoreType == kCoreTypeMMap)
					{
						const size_type nHighestChunkSize = GetChunkSize(pHighestChunk);
						const size_type nPaddingSize      = (size_type)(nPadding + (2 * kMinChunkSize) + sizeof(CoreBlock)); // Add size of CoreBlock so we don't kill the administrative header information.

						if(nHighestChunkSize > nPaddingSize)
						{
							#if defined(EA_PLATFORM_WINDOWS) && !PPM_MALLOC_AS_COREALLOC
								size_t nDecommitSizeIncrement = PPM_AlignUp(mnCoreIncrementSize / 2, 65536);
								size_t nDecommitSize          = nDecommitSizeIncrement;
								size_t nDecommitLimit         = nHighestChunkSize - nPaddingSize;

								while((nDecommitSize + nDecommitSizeIncrement) < nDecommitLimit)
									nDecommitSize += nDecommitSizeIncrement;

								if(nDecommitSize < nDecommitLimit) // If we can decommit nDecommitSize bytes...
								{
									char* const pVirtualFreeLocation = (char*)pCoreBlock + pCoreBlock->mnSize - nDecommitSize;
									PPM_ASSERT(this, pVirtualFreeLocation >= (char*)(pCoreBlock + 1), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Assert that we are not freeing the first part of the core block which holds administrative information. // If this fails, PPMalloc is at fault.

									if(CoreMemoryAllocator::ReleaseCoreMemory(pVirtualFreeLocation, nDecommitSize))
									{
										nCoreTrimmed += nDecommitSize;
										mPhysicalFootprint -= nDecommitSize;

										pCoreBlock->mnSize -= nDecommitSize;
										PPM_ASSERT(this, ((pCoreBlock->mnSize % 65536) == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pCoreBlock); // If this fails, it is PPMalloc's fault.

										if(pHighestChunk != mpTopChunk) // If it's in some specifically-sized bin...
											UnlinkChunkFromBin(pHighestChunk); // Remove it from whatever bin list it was in, as it now likely goes into a new bin.
										SetChunkSizePreserveFlags(pHighestChunk, (size_type)((nHighestChunkSize - nDecommitSize) + kDoubleFenceChunkSize)); // Add Double fence size because AddDoubleFencepost will subtract it below.
										AddDoubleFencepost(pHighestChunk, 0);

										if(pHighestChunk != mpTopChunk) // If it's in some specifically-sized bin...
										{
											// We place the newly shrunk chunk into the unsorted bin. Actually, it would be best if 
											// instead we found the right sized bin. Todo: Consider doing the aforementioned positioning..
											Chunk* const pUnsortedBinHeadChunk = GetUnsortedBin();
											LinkChunk(pHighestChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
										}

										#if (PPM_DEBUG >= 2) // Extended user debug builds.
											DebugCheckState(kHeapValidationLevelBasic);
										#endif
									}
									else
									{
										PPM_ASSERT(this, false, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // The above really should have succeeded. // If this fails, PPMalloc is at fault.
									}
								}
							#elif defined(EA_PLATFORM_UNIX) && !PPM_MALLOC_AS_COREALLOC && !defined(__APPLE__)
								// Todo: Possibly trim top of core via a call to sbrk().
							#endif
						}
					}
				}
			}
		}
	}

	return nCoreTrimmed;
}



#if PPM_MMAP_SUPPORTED

///////////////////////////////////////////////////////////////////////////////
// MMapMalloc
//
void* GeneralAllocator::MMapMalloc(size_t nSize)
{
	PPM_ASSERT(this, mnNewCoreSize >= mnMMapThreshold, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this assertion fails, then it is possible for allocations to fail due to not being servicable by either core or mmapped memory. // If this fails, PPMalloc is at fault.

	size_t nMMapChunkSize = GetMMapChunkSizeFromDataSize((size_type)nSize);

	// If the size calculation didn't cause integer wraparound or if we are allocating within the maximum amount allowed...
	if((nMMapChunkSize >= nSize) && (mnMMapCount < mnMMapMaxAllowed)) 
	{
		char* const pMMapMemory = (char*)MMapAllocInternal(nMMapChunkSize, &nMMapChunkSize);

		if(pMMapMemory)
		{
			mnMMapCount++;
			mnMMapMallocTotal += nMMapChunkSize;

			// Since we put an empty chunk at the end of this MMapped chunk, 
			// we subtract out its size. We'll need to add it back in when
			// freeing the MMapped chunk.
			nMMapChunkSize -= kMinChunkSize;

			Chunk* pResultChunk = (Chunk*)pMMapMemory; // Chunk for returned data ptr.
			PPM_ASSERT(this, GetIsMinAligned(pResultChunk), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pResultChunk, pResultChunk); // If this fails, PPMalloc is at fault.
			pResultChunk->mnPriorSize = 0;
			SetChunkSize(pResultChunk, (size_type)(nMMapChunkSize | kChunkFlagMMapped));

			// Set up the empty top end chunk, whose purpose is to allow us to put these
			// mmapped chunks into a linked list for tracking.
			Chunk* const pFenceChunk = GetChunkAtOffset(pResultChunk, (ssize_type)nMMapChunkSize); // Recall that we subtracted kMinChunkSize above.
			pFenceChunk->mnPriorSize = (size_type)nMMapChunkSize;
			pFenceChunk->mnSize      = kMinChunkSize | kChunkFlagPrevInUse | kChunkFlagMMapped;
			LinkChunk(pFenceChunk, &mHeadMMapChunk, mHeadMMapChunk.mpNextChunk); // Add the MMap chunk into our MMapped chunk list.

			DebugCheckMMappedChunk(pResultChunk);
			PPM_ASSERT(this, GetChunkSize(pResultChunk) > nSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pResultChunk, pResultChunk); // If this fails, PPMalloc is at fault.
			void* pData = GetDataPtrFromChunkPtr(pResultChunk);
			return pData;
		}
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// MMapAllocInternal
//
// Allocate mapped memory via the virtual memory system. Only applies to systems
// that have virtual memory support.
// The nSize parameter doesn't need to be aligned to any boundary. However, the
// actual allocation is typically rounded up to some nominal page size. 
//
void* GeneralAllocator::MMapAllocInternal(size_t nSize, size_t* pSizeAllocated)
{
	return CoreMemoryAllocator::AllocMemoryMap(nSize, mnMMapPageSize, mbMMapTopDown, mbExecutableCore, mRangeCallback, mRangeUserData, pSizeAllocated, this);
}


///////////////////////////////////////////////////////////////////////////////
// MMapFreeInternal
//
void GeneralAllocator::MMapFreeInternal(void* pAddress, size_t nSize)
{
	CoreMemoryAllocator::FreeMemoryMap(pAddress, nSize, mRangeCallback, mRangeUserData, this);
}

#endif // PM_MMAP_SUPPORTED

void* GeneralAllocator::DefaultAddCoreMemoryFunction(size_t nMinSize, bool bCommitAll, size_t& nReservedSize, size_t&  nCoreSize, uint32_t& coreType, bool& bPageExecute)
{
	return CoreMemoryAllocator::AddCoreMemory(nMinSize, mbExecutableCore, mbCoreTopDown, bCommitAll, mnCoreIncrementSize, nCoreSize, nReservedSize, coreType, bPageExecute, this);
}

void* GeneralAllocator::DefaultCoreExtendFunction(CoreBlock* pCoreBlock, size_t nSize, size_t& nExtendedSize)
{
	#if defined(EA_PLATFORM_MICROSOFT) && PPM_MMAP_SUPPORTED
		PPM_ASSERT(this, !pCoreBlock->mbMMappedMemory, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // This should never be the case, as the above size comparison should prevent so. // If this fails, PPMalloc is at fault.
	#endif

	bool bIsExtensionSupported = false;
	void* pExtendedCore = CoreMemoryAllocator::ExtendCoreMemory(pCoreBlock, pCoreBlock->mnSize, nSize, pCoreBlock->mnReservedSize, mnCoreIncrementSize, pCoreBlock->mbPageExecutable, bIsExtensionSupported, nExtendedSize, this);
	if (bIsExtensionSupported && pExtendedCore == NULL)
	{
		PPM_ASSERT(this, false, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], NULL, pExtendedCore); // Did somebody steal our reserved memory? // If this fails, the user probably corrupted the heap. There may be a bug in PPMalloc, however.
		// In this case, something bad has happened behind our backs, 
		// so we mark the rest of the block as being unusable and 
		// just forget about the apparently lost reserved memory.
		// Note that this is reserved memory, not committed memory,
		// so it's not as bad as actually leaking memory.
		pCoreBlock->mnReservedSize = pCoreBlock->mnSize;
	}
	return pExtendedCore;
}

bool GeneralAllocator::DefaultCoreFreeFunction(CoreBlock* pCoreBlock)
{
	return CoreMemoryAllocator::FreeCoreMemory(pCoreBlock, pCoreBlock->mCoreType, pCoreBlock->mnSize, mRangeCallback, mRangeUserData, this);
}


} // namespace Allocator

} // namespace EA













