///////////////////////////////////////////////////////////////////////////////
// shared.h
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_INTERNAL_SHARED_H
#define PPMALLOC_INTERNAL_SHARED_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>


namespace EA
{
	namespace Callstack
	{
		class IAddressRepLookupBase;
	}

	namespace Allocator
	{

		///////////////////////////////////////////////////////////////////////////////
		// PPM_MIN / PPM_MAX
		//
		#define PPM_MIN(a, b) ((a) < (b) ? (a) : (b))
		#define PPM_MAX(a, b) ((a) > (b) ? (a) : (b))


		///////////////////////////////////////////////////////////////////////////////
		// PPM_MAY_ALIAS
		//
		#if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
			#define PPM_MAY_ALIAS __attribute__((__may_alias__))
		#else
			#define PPM_MAY_ALIAS
		#endif


		/// PPM_Snprintf
		///
		/// Implements a single consistent version of the snprintf function. 
		/// snprintf is not a standard C or C++ function, except for C99 and later. 
		///
		#if defined(EA_PLATFORM_MICROSOFT)
			#define PPM_Snprintf _snprintf
		#else
			#define PPM_Snprintf snprintf
		#endif


		/// PPM_Vsnprintf
		///
		/// Implements a single consistent version of the vsnprintf function. 
		/// vsnprintf is not a standard C or C++ function, except for C99 and later. 
		///
		#define PPM_Vsnprintf vsnprintf


		/// PPM_DebugBreak
		///
		/// Implements a non-inline debugger break (stop).
		///
		PPM_API void PPM_DebugBreak();


		/// DefaultFillValue
		/// Ideally, these values would be tailored for each processor, as different
		/// processors have different instruction sets and it's best if these fill 
		/// values don't correspond to legal instructions on the target mahine.
		/// Note: 0x9f, 0x9e, 0x9d, 0x9c are better fill values for XBox 360 than
		/// 0xcd, 0xdd, etc. This is because writing to 0x9f9f9f9f will trigger an
		/// an exception rather than possibly write to memory.
		enum DefaultFillValue
		{
				kDefaultFillValueDisabled    = 0x01,  /// Fill value which means to disable a given fill. Doesn't apply to guard fills or delayed free fills, which are controlled by the guard and delayed free settings.
				kDefaultFillValueFree        = 0xdd,  /// Fill value for user memory that has been freed.
				kDefaultFillValueDelayedFree = 0xde,  /// Fill value for user memory which is in delayed free state.
				kDefaultFillValueNew         = 0xcd,  /// Fill value for newly allocated user memory.
				kDefaultFillValueGuard       = 0xab,  /// Fill value for guard memory returned in new user memory.
				kDefaultFillValueUnusedCore  = 0xfe   /// Fill value for core that is unassigned to free or allocated memory.
		};


		/// Error definitions
		enum GAError
		{
			kGAErrorUnknown,                      /// This is a placeholder. There should be no reported errors of this type.
			kGAErrorOS,                           /// Some type of operating system error occurred.
			kGAErrorInternal,                     /// The allocator itself appears to have a bug.
			kGAErrorUninitializedHeap,            /// The heap is being used while uninitialized.
			kGAErrorCorruptedHeap,                /// The heap structures themselves appear to be corrupted.
			kGAErrorCorruptedChunk,               /// The Chunk currently being examined appears to be corrupted, usually in its header area (first four bytes of Chunk).
			kGAErrorChunkFillError,               /// The Chunk guard fill has been modified, due to some kind of overrun or illegal memory write.
			kGAErrorChunkOutsideHeap,             /// The Chunk currently being examined appears to not belong to this heap.
			kGAErrorInvalidReportContext,         /// The report context the user is passing is invalid.
			kGAErrorShapshotMemoryExhaustion,     /// The report snapshot buffer memory is exhausted.
			kGAErrorDebugMemoryExhaustion,        /// The debug info memory is exhausted. This is usually the allocator's fault.
			kGAErrorRequestedSizeTooLarge,        /// The allocation size request is too large.
			kGAErrorRequestedAlignmentInvalid,    /// The allocation alignment is invalid. Must be zero or a power of two and less that some very large value.
			kGAErrorSuppliedCoreInvalid,          /// The users attempt to add core memory is somehow invalid.
			kGAErrorBadUserPtr,                   /// The pointer supplied by the user was invalid for this heap. A double free attempt on a pointer can cause this.
			kGAErrorInvalidInput,                 /// The user supplied an invalid input to a function. 
			kGAErrorMemoryExhaustion,             /// The core memory for this heap is exhausted and cannot provide the user with the memory requested (e.g. by Malloc).
			kGAErrorCount                         /// (size of this enumeration)
		};

		// GeneralAllocator error string table.
		extern const char* kGAErrorText[kGAErrorCount];


		/// GetPPMallocDebugTime    
		///
		/// Gets the current time in seconds.
		/// 
		PPM_API unsigned GetPPMallocDebugTime();


		/// GetPPMallocDebugTimeUs    
		///
		/// Gets the current time in microseconds.
		/// 
		PPM_API uint64_t GetPPMallocDebugTimeUs();


		/// DataPreviewType
		///
		/// Used by the GetDataPreview function.
		///
		enum DataPreviewType
		{
			kDataPreviewTypeBinary,  /// Unrecognizable raw binary data.
			kDataPreviewTypeChar,    /// ASCII, UTF8, etc.
			kDataPreviewTypeWChar    /// wchar_t, whether via UCS2 or UCS4. We only expect English characters, as the rest would require more intelligence than we are interested in supportintg.
		};

		/// GetDataPreview
		///
		/// Given a block of binary data, this function makes a readable version of it
		/// for display on a text output. The output format is similar to that seen 
		/// in hex editors, with hex text on the left and ASCII text on the right.
		/// 
		/// Output example:
		///     "48 65 6c 6c 6f\tHello"
		///
		PPM_API DataPreviewType GetDataPreview(const void* pData, size_t nDataSize, char* pBuffer, wchar_t* pBufferW, size_t nBufferLength);


		/// TraceFunctionDefault
		///
		/// Implements a default trace function which writes to stdout.
		///
		PPM_API void TraceFunctionDefault(const char* pTraceText, void* pContext);


		/// MemoryFillCheck
		///
		/// Returns true if the given data/size is completely filled with the char c.
		///
		PPM_API bool MemoryFillCheck(const unsigned char* pData, size_t nDataSize, unsigned char c);


		/// DebugMemFill / DebugMemCheck
		///
		/// Optionally enables a memset for memory filling with fixed values. 
		/// Function signatures:
		///     void DebugMemFill(void* p, int c, size_t n);                        // Same as the memset function.
		///     bool DebugMemCheck(const void* pData, size_t n, unsigned char c);   // Note that we have an inconsistency between the arguments of DebugMemFill and DebugMemCheck.
		///
		#if PPM_DEBUG_FILL
			#define DebugMemFill memset
			bool    DebugMemCheck(const void* pData, size_t n, unsigned char c);
		#else
			#define DebugMemFill(p, c, n)
			#define DebugMemCheck(p, n, c)
		#endif


		/// InitCallstack
		/// Allows the user to explicitly initialize the callstack mechanism.
		/// Only the first call to InitCallstack will have effect. Calls to 
		/// InitCallstack must be matched by calls to ShutdownCallstack.
		PPM_API void InitCallstack();

		/// ShutdownCallstack
		/// Allows the user to explicitly shutdown the callstack mechanism.
		/// Calls to InitCallstack must be matched by calls to ShutdownCallstack.
		/// The last call to ShutdownCallstack will shutdown and free the callstack mechanism.
		PPM_API void ShutdownCallstack();

		/// GetCallStack
		/// Returns number of non-zero entries written. The return value may be zero.
		/// Note that the memory size of the entries is thus the return value * sizeof(void).
		/// The first entry in the returned array is the return address of the calling function;
		/// the second entry is the returned addess of the first entry's calling function, etc.
		/// Due to the complications that are often involved in doing stack traces, this function
		/// may return addresses that are invalid (NULL pointers or otherwise). The user should 
		/// be prepared for this and gracefully deal with such addresses.
		PPM_API size_t GetCallStack(void* pReturnAddressArray[], size_t nReturnAddressArrayLength);

		/// DescribeCallStack
		/// Returns number of bytes written to the string buffer. Note that the buffer written
		/// to is not null-terminated and the return value does not include any such null terminator.
		/// The reason for this is that these call stacks are often concatenated into a stream of 
		/// contiguous data and a null terminator within this stream would detrimentally affect 
		/// data output.
		PPM_API size_t DescribeCallStack(const void* pReturnAddressArray[], size_t nReturnAddressArrayLength, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase* addressRepLookup=nullptr);


		/// kDefaultPageSize
		/// The default system page size that is assumed.
		const unsigned kDefaultPageSize = 4096;


		/// GetPageSize
		/// Returns the platform default page size. Some platforms
		/// have dynamically determined page sizes and so this is 
		/// expressed via a function instead of a constant.
		PPM_API unsigned GetPageSize();


		/// kDefaultPageSize
		/// The default system page size that is assumed.
		#if defined (EA_PLATFORM_SONY)
			const unsigned kDefaultMMapPageSize = 4 * 4096; // 16KB logical page size = 4 x 4KB physical pages
		#else
			const unsigned kDefaultMMapPageSize = 4096;
		#endif


		union FloatUint32
		{
			float    f;
			uint32_t u;
		};

		union DoubleUint64
		{
			double   d;
			uint64_t u;
		};

		inline uint32_t PPM_Log2(uint32_t x)
		{
			FloatUint32 t;
			t.f = (float)x;
			return (t.u >> 23) - 127;
		}

		inline uint64_t PPM_Log2(uint64_t x)
		{
			DoubleUint64 t;
			t.d = (double)x;
			return ((t.u >> 52) & 0x7ff) - 1023;
		}

		inline bool PPM_IsPowerOf2(size_t nValue)
		{
			return (nValue > 0) && ((nValue & (nValue - 1)) == 0);
		}

		inline bool PPM_IsAligned(const void* p, size_t nAlignment)
		{
			return ((uintptr_t)p & (nAlignment - 1)) == 0;
		}

		inline bool PPM_IsAligned(size_t nValue, size_t nAlignment)
		{
			return (nValue & (nAlignment - 1)) == 0;
		}

		inline char* PPM_AlignUp(const void* p, size_t nAlignment)
		{
			return (char*)(((uintptr_t)p + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		inline char* PPM_AlignUp(const char* p, size_t nAlignment)
		{
			return (char*)(((uintptr_t)p + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		inline size_t PPM_AlignUp(size_t nValue, size_t nAlignment)
		{
			return (size_t)((nValue + (nAlignment - 1)) & ~(nAlignment - 1));
		}

		class PPM_API IAllocAssertFunc
		{
		public:
			virtual void AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const = 0;
		protected:
			virtual ~IAllocAssertFunc() {};

		};

	} // namespace Allocator

} // namespace EA


#endif // Header include guard




