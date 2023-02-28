///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_INTERNAL_CONFIG_H
#define PPMALLOC_INTERNAL_CONFIG_H


#include <EABase/eabase.h>


///////////////////////////////////////////////////////////////////////////////
// PPMALLOC_USER_CONFIG_HEADER
//
// This allows the user to define a header file to be #included before the 
// PPMalloc config.h contents are compiled. A primary use of this is to 
// override the contents of this config.h file. Note that all the settings 
// below in this file are user-overridable.
//
// It's important to recognize that this header affects both how PPMalloc is 
// compiled and how other code compiles the PPMalloc headers. Thus if there 
// is a custom config header specified then it has to be specified both while
// compiling PPMalloc and while compiling user code that #includes PPMalloc
// headers. That is, it must be globally applied.
///////////////////////////////////////////////////////////////////////////////

#ifdef PPMALLOC_USER_CONFIG_HEADER
	#include PPMALLOC_USER_CONFIG_HEADER
#endif


///////////////////////////////////////////////////////////////////////////////
// Version
//
// The version is typically a 5 digit number: Major:MinorMinor:PatchPatch.
// A value of 11002 refers to version 1.10.02.
//
#ifndef PPMALLOC_VERSION
	#define PPMALLOC_VERSION    "1.26.10"
	#define PPMALLOC_VERSION_N  12610

	// The following identifiers are deprecated and are present for backward compatibility:
	#define PPM_GENERAL_ALLOCATOR_VERSION        PPMALLOC_VERSION_N
	#define PPM_GENERAL_ALLOCATOR_DEBUG_VERSION  PPMALLOC_VERSION_N
	#define PPMALLOC_VERSION_MAJOR              (PPMALLOC_VERSION_N / 100 / 100 % 100)
	#define PPMALLOC_VERSION_MINOR              (PPMALLOC_VERSION_N       / 100 % 100)
	#define PPMALLOC_VERSION_PATCH              (PPMALLOC_VERSION_N             % 100)
#endif




///////////////////////////////////////////////////////////////////////////////
// PPM_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building PPMalloc as a DLL and PPMalloc's
// non-templated functions will be exported. PPMalloc template functions are not
// labelled as PPM_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of PPM_API:
//    PPM_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct PPM_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        PPM_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    PPM_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#ifndef PPM_API // If the build file hasn't already defined this to be dllexport...
	// If PPM_API is not defined, assume this package is being included by something else. When PPMalloc is 
	// built as a DLL, PPM_API is defined to __declspec(dllexport) by the package build script.

	#if (defined(EA_DLL) || defined(PPMALLOC_DLL)) 
		#if defined(_MSC_VER)
			#define PPM_API      __declspec(dllimport)
			#define PPM_LOCAL
		#elif defined(__CYGWIN__)
			#define PPM_API      __attribute__((dllimport))
			#define PPM_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define PPM_API      __attribute__ ((visibility("default")))
			#define PPM_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define PPM_API
			#define PPM_LOCAL
		#endif
	#else
		#define PPM_API
		#define PPM_LOCAL
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_DEBUG (defined as 0, 1 or 2; default is 0)
//
// If you compile with PPM_DEBUG defined as non-zero, a number of assertion 
// checks are enabled that will catch more memory errors. You probably won't 
// be able to make much sense of the actual assertion errors, but they
// should help you locate incorrectly overwritten memory. The checking
// is fairly extensive, and will slow down execution noticeably. The higher 
// the defined value, the more extensive the debugging. Basically, release
// builds should have PPM_DEBUG = 0, debug builds should have a PPM_DEBUG >= 1,
// and builds done by maintainers of this library should have PPM_DEBUG >= 3.
// 
// Setting PPM_DEBUG may also be helpful if you are trying to modify
// this code. The assertions in the check routines spell out in more
// detail the assumptions and invariants underlying the algorithms.
// 
// Setting PPM_DEBUG does not provide an automated mechanism for
// checking that all accesses to malloced memory stay within their
// bounds. This kind of functionality is generally considered to be
// outside the scope of the allocator, as it requires functionality
// that is often compiler- or platform-specific.
//
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// | PPM_DEBUG value | Purpose                  | Meaning for GeneralAllocator
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// |    0            | Release builds           | - No GeneralAllocator automatic validation functionality is present.
// |                 |                          | - GeneralAllocatorDebug is not usable.
// |                 |                          | - No GeneralAllocatorDebuig automatic validation functionality is present.
// |                 |                          | - GeneralAllocator and GeneralAllocatorDebug asserts are disabled.
// |                 |                          | - PPM_DEBUG_PRESERVE_PRIOR disabled by default. You can override this.
// |                 |                          | - PPM_VIRTUAL_ENABLED disabled by default. You can override this.
// |                 |                          | - Automatic heap validation is not supported. You can still manually call the function.
// |                 |                          | - This yields the highest performance.
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// |    1            | Basic debug builds       | - Very minimal GeneralAllocator automatic validation functionality is present.
// |                 |                          | - GeneralAllocatorDebug is usable.
// |                 |                          | - Very minimal GeneralAllocatorDebug automatic validation functionality is present.
// |                 |                          | - PPM_DEBUG_PRESERVE_PRIOR enabled by default. You can override this.
// |                 |                          | - PPM_VIRTUAL_ENABLED enabled by default. You can override this, 
// |                 |                          |    but PPM_VIRTUAL_ENABLED is required in order to use GeneralAllocatorDebug.
// |                 |                          | - PPM_AUTO_HEAP_VALIDATION_SUPPORTED is enabled by default. This merely means that
// |                 |                          |    you can now set up automatic heap validations to occur.
// |                 |                          | - This yields the highest performance possible while using GeneralAllocatorDebug;
// |                 |                          |    performance should be fast enough for all game and tool applications.
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// |    2            | Extended debug builds    | - Primary GeneralAllocator automatic validation functionality is present.
// |                 |                          | - Primary GeneralAllocatorDebug automatic validation functionality is present.
// |                 |                          | - GeneralAllocatorDebug guard fills enabled by default.
// |                 |                          | - This yields performance that will be adequate for some purposes but slow for others.
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// |    3            | Developer builds         | - Extended GeneralAllocator automatic validation functionality is present.
// |                 |                          | - Extended GeneralAllocatorDebug automatic validation functionality is present.
// |                 |                          | - This mode is only intended for developing and testing the library; 
// |                 |                          |    its performance will be slow for many large game applications.
// |-----------------|--------------------------|-----------------------------------------------------------------------------------------
// |    4            | Developer builds         | - Heavy GeneralAllocator automatic validation functionality is present.
// |                 |                          | - Heavy GeneralAllocatorDebug automatic validation functionality is present.
// |                 |                          | - This mode is only intended for developing and testing the library; 
// |                 |                          |    its performance will be slow for most large game applications.
// |-----------------|--------------------------------------------------------------------------------------------------------------------
//
#if !defined(PPM_DEBUG) || (PPM_DEBUG < 0)
	#if defined(EA_DEBUG) || defined(_DEBUG)
		#define PPM_DEBUG 1
	#else
		#define PPM_DEBUG 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_DEBUG_FILL (defined as 0 or 2+. Default 0 for PPM_DEBUG <= 1; otherwise same as PPM_DEBUG)
//
#if !defined(PPM_DEBUG_FILL) || (PPM_DEBUG_FILL < 0)
	#if (PPM_DEBUG >= 2)
		#define PPM_DEBUG_FILL PPM_DEBUG
	#else
		#define PPM_DEBUG_FILL 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_CT_ASSERT   
//
// Compile-time assertion for this module.
// C-like declaration:
//    void PPM_CT_ASSERT(bool bExpression);
//
#define PPM_CT_ASSERT(expression) static_assert(expression, #expression)


///////////////////////////////////////////////////////////////////////////////
// PPM_ASSERT_ENABLED (defined as 0 or 1, default is same as PPM_DEBUG)
//
// If true, then assertion failures are reported via pAllocator->AssertionFailure. 
// Note that PPM_ASSERT_ENABLED is usable even if PPM_DEBUG is 0; this comes into 
// play in particular when doing heap validations, as all validation failures are 
// reported through the AssertionFailure mechanism. Note that you do not want this
// feature enabled in a shipping build, as it costs CPU cycles.
//
// C-like declaration:
//    void PPM_ASSERT(GeneralAllocator*  pAllocator, bool bExpression, const char* pDescription, void* pData, void* pLocation);
//    void PPM_ASSERT(NonLocalAllocator* pAllocator, bool bExpression, const char* pDescription, void* pData, void* pLocation);
//    void PM_ASSERT_STATIC(bool bExpression, const char* pDescription, void* pData, void* pLocation);
//
// Example usage:
//    PPM_ASSERT(this, nMaxAlignment >= 8, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
//    PPM_ASSERT_STATIC(pPositionEnd >= pDebugBlock, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, pDebugBlock);
//
#ifndef PPM_ASSERT_ENABLED
	#define PPM_ASSERT_ENABLED PPM_DEBUG
#endif

#ifndef PPM_ASSERT
	#if PPM_ASSERT_ENABLED
		#define PPM_ASSERT(pAllocator, expression, id, pDescription, pData, pExtra) \
			EA_DISABLE_VC_WARNING(4127) \
			do { \
				EA_ANALYSIS_ASSUME(expression); \
				(void)((expression) || (pAllocator ? pAllocator->AssertionFailure(#expression, id, pDescription, pData, pExtra) : assert(!#expression), 0)); \
			} while (0) \
			EA_RESTORE_VC_WARNING()
		#define PPM_ASSERT_STATIC(expression, id, pDescription, pData, pExtra) \
			EA_DISABLE_VC_WARNING(4127) \
			do { \
				EA_ANALYSIS_ASSUME(expression); \
				(void)((expression) || (assert(!#expression), 0)); \
			} while (0) \
			EA_RESTORE_VC_WARNING()
	#else
		#define PPM_ASSERT(pAllocator, expression, id, pDescription, pData, pExtra)
		#define PPM_ASSERT_STATIC(expression, id, pDescription, pData, pExtra)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NULL_POINTER_FREE_ENABLED (defined as 0 or 1, 1 is default)
//
// Defines whether or not the Free function allows passing of a NULL pointer
// as an argument. While the C89 and C99 standards specify that free(NULL)
// should do nothing, many C libraries in fact disallow free of NULL and 
// crash if you attempt it. By default we follow what the C standard specifies.
//
#ifndef PPM_NULL_POINTER_FREE_ENABLED
	#define PPM_NULL_POINTER_FREE_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_REALLOC_C99_ENABLED (defined as 0 or 1, 1 is default)
//
// Defines whether or not the Realloc function behaves as per the C89 standard
// or per the C99 standard. There are small differences between the two which
// are documented with the GeneralAllocator::Realloc source code.
//
#ifndef PPM_REALLOC_C99_ENABLED
	#define PPM_REALLOC_C99_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_DEBUG_FAILURE
//
// Implements allocator failure for user data integrity.
// C-like declaration:
//    void PPM_DEBUG_FAILURE(Allocator* pAllocator, const char* pMessage, int id, const char* pDescription, void* pData, void* pLocation);
//
#if !defined(PPM_DEBUG_FAILURE) && (defined(EA_DEBUG) || defined(_DEBUG))
	#define PPM_DEBUG_FAILURE(pAllocator, pMessage, id, pDescription, pData, pLocation) \
		{ pAllocator->AssertionFailure(pMessage, id, pDescription, pData, pLocation); }
#else
	#define PPM_DEBUG_FAILURE(pAllocator, pMessage, id, pDescription, pData, pLocation)
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_VIRTUAL_ENABLED (defined as 0 or 1. Default is same as PPM_DEBUG)
//
// PPM_VIRTUAL is optionally defined as 'virtual' in order to allow PPMalloc
// functions to become virtual. For normal usage, virtual isn't required and 
// would impose a performance penalty. However, there are times where virtual
// may be useful, particularly in the case where you want to subclass PPMalloc
// in order to provide a debug-time variation (e.g. class PPMallocDebug).
//
#ifndef PPM_VIRTUAL_ENABLED
	#define PPM_VIRTUAL_ENABLED PPM_DEBUG
#endif

#if PPM_VIRTUAL_ENABLED
	#define PPM_VIRTUAL virtual
#else
	#define PPM_VIRTUAL
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_THREAD_SAFETY_SUPPORTED (defined as 0 or 1; default is 1 if platform supports it)
//
// Defines whether thread safety is supported, which is not the same thing
// as thread safety being enabled. You need the former before you can have
// the latter. If thread safety is supported, then you can optionally define
// PPM_THREAD_SAFETY_BY_DEFAULT to 1 to enable it by default. It can always be 
// enabled or disabled at runtime by calling SetThreadSafetyEnabled, but 
// if GeneralAllocator is being statically created, then you might need to have 
// thread safety enabling defined ahead of time.
//
#ifndef PPM_THREAD_SAFETY_SUPPORTED
	// If you are porting PPMalloc to a new platform which supports 
	// multithreading for the first time,  it's important that you  
	// add multithread support via implementing the PPMMutex* functions 
	// currently declared in mutex.h. Otherwise initial platform users 
	// are likely to expect that PPMalloc is thread-safe and have odd
	// crashes that are hard to diagnose. 
	#define PPM_THREAD_SAFETY_SUPPORTED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_THREAD_SAFETY_BY_DEFAULT (defined as 0 or 1; default is same as PPM_THREAD_SAFETY_SUPPORTED)
//
// Defines if upon initialization of a GeneralAllocation instance it is thread safe.
//
#ifndef PPM_THREAD_SAFETY_BY_DEFAULT
	#define PPM_THREAD_SAFETY_BY_DEFAULT PPM_THREAD_SAFETY_SUPPORTED
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_AUTO_HEAP_VALIDATION_SUPPORTED (defined as 0 or 1; default is same as PPM_DEBUG)
//
// If PPM_AUTO_HEAP_VALIDATION_SUPPORTED is defined as non-zero, then automatic 
// heap validation code is compiled and usable via the SetAutomaticHeapValidation
// user function.
//
#ifndef PPM_AUTO_HEAP_VALIDATION_SUPPORTED
	#define PPM_AUTO_HEAP_VALIDATION_SUPPORTED PPM_DEBUG
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_HOOKS_SUPPORTED (defined as 0 or 1. Default is same as PPM_DEBUG)
//
// If true, then the user can install callback hooks that notify the user
// of high level allocation events.
//
#ifndef PPM_HOOKS_SUPPORTED
	#define PPM_HOOKS_SUPPORTED PPM_DEBUG
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_INTERNAL_CORE_FREE_ENABLED (defined as 0 or 1; default is 1)
// *** This option is deprecated as of PPMalloc 1.18.01 ***
//
// When defined to 1, core that is set to be freed but has no callback
// function will be freed via a call to the platform-specific memory
// freeing function. For basic console platforms this function is 
// usually malloc, while for PC platforms this is VirtualFree.
//
#ifndef PPM_INTERNAL_CORE_FREE_ENABLED
	#define PPM_INTERNAL_CORE_FREE_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_MALLOC_AS_COREALLOC (defined as 0 or 1; default is 1 unless the platform supports advanced memory management)
//
// When defined to 1, this instructs the system to get core from 
// malloc and not directly from the system. You must be careful with 
// this in that if you are using this allocator to directly replace
// malloc, you probably don't want to set PPM_MALLOC_AS_COREALLOC
// to be 1, as it would cause an infinite loop at runtime if CoreAlloc
// was called.
#ifndef PPM_MALLOC_AS_COREALLOC
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) || (defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_WINDOWS) && !defined(EA_PLATFORM_WINDOWS_PHONE))
		#define PPM_MALLOC_AS_COREALLOC 0 // VirtualAlloc is supported.
	#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__) || defined(EA_PLATFORM_SONY)
		#define PPM_MALLOC_AS_COREALLOC 0 // mmap or another alternative to malloc is supported.
	#else // Console platforms.
		#define PPM_MALLOC_AS_COREALLOC 1
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_MALLOC_AS_MMAPALLOC (defined as 0 or 1; defaults to 0)
//
// When defined to 1, this causes the memory mapped allocations to instead
// use plain old malloc to retrieve the memory. You probably don't want
// to change this value unless you are doing some kind of testing during
// library maintenance.
//
#ifndef PPM_MALLOC_AS_MMAPALLOC
	#define PPM_MALLOC_AS_MMAPALLOC 0
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_MMAP_CLEARS (defined as 0 or 1; default is 0)
//
// True if new memory mapped data is cleared to zero by the system.
// This is present because it allows small optimizations to be made.
// This is not a setting that should be altered, as it is endemic to
// the given system.
//
#ifndef PPM_MMAP_CLEARS
	#if defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_MICROSOFT) || defined(__APPLE__)
		#define PPM_MMAP_CLEARS 1 // Under Microsoft, VirtualAlloc clears memory unless MEM_NOZERO is specified.  
	#else
		#define PPM_MMAP_CLEARS 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_HIGH_SUPPORTED (defined as 0 or 1; default is 1 for console platforms, and 0 for others)
//
// Defines whether or not the concept of requesting from 'high' memory is 
// supported. The idea behind requesting high memory allocation is that you 
// are requesting the allocator to return memory that is separate from 
// normal memory. Usually this is done with the intent of reducing 
// fragmentation. If you allocate memory that you know will be permanent
// as high but leave other more dynamic allocations as 'low', then you 
// will likely get less fragmentation due to there being no holes in the
// high memory. The concept of high memory can be implemented by truly
// using high memory addresses or can also be implemented by having
// separate pools for high and low memory.
//
#ifndef PPM_HIGH_SUPPORTED
	// With consoles you typically give GeneralAllocator a big core block to 
	// work from for the entire life of the application, as the memory on 
	// the console device is limited. With paged memory systems the amount 
	// of memory available in the system is variable between systems and 
	// applications often scale to the system. As a result, this latter
	// type of system doesn't lend itself to the concept of high memory very 
	// well, simply because it's hard to draw a line between any two types 
	// of core, being that they are always changing. Supporting high memory 
	// on such a system is likely to hurt performance while having little benefit.
	#define PPM_HIGH_SUPPORTED PPM_MALLOC_AS_COREALLOC // If malloc is used for core, we assume console-like behaviour.
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_SIZE_TYPE / PPM_SSIZE_TYPE
//
// PPM_SIZE_TYPE is defined as an unsigned integral type, such as uint32_t or size_t.
// Must be at least 32 bits. PPM_SSIZE_TYPE must be the signed equivalent of it.
// If PPM_SIZE_TYPE is defined then PPM_SSIZE_TYPE must also be defined.
//
#if !defined(PPM_SIZE_TYPE)
	// No need to define defaults here, but the following provide an example.
	//#if (EA_PLATFORM_PTR_SIZE >= 8)
	//    #define PPM_SIZE_TYPE uint64_t
	//#else
	//    #define PPM_SIZE_TYPE uint32_t
	//#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_TRACE_BUFFER_SIZE (defined as an integer value, 2500/4000 is default)
//
// Defines the stack buffer size to use for the trace buffer for the 
// TraceAllocatedMemory function. If you have a very large amount of block
// tagging and need a larger buffer, you can change this value or you can
// do the reporting manually with any size buffer you want.
//
#ifndef PPM_TRACE_BUFFER_SIZE
	#define PPM_TRACE_BUFFER_SIZE 4000
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLE_EAALLOCATORINTERFACE (defined as 0 or 1; 0 is default)
//
// Enables default compiling of EAAllocatorInterface.cpp.
// As of November 2009, EAAllocator interface has been removed from the 
// package but this define remains for the time being.
//
#ifndef PPM_ENABLE_EAALLOCATORINTERFACE
	#define PPM_ENABLE_EAALLOCATORINTERFACE 0
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLE_EANEWDELETE (defined as 0 or 1; 0 is default)
//
// Enables default compiling of EANewDelete.cpp
//
#ifndef PPM_ENABLE_EANEWDELETE
	#define PPM_ENABLE_EANEWDELETE 0
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_FASTBIN_TRIM_ENABLED (defined as 0 or 1; default is 0)
//
// This is currently specific to GeneralAllocator.
// This controls whether Free() of a very small chunk can immediately lead 
// to trimming. Setting to non-zero can reduce memory footprint, but will 
// almost always slow down programs that use a lot of small chunks.
//
#ifndef PPM_FASTBIN_TRIM_ENABLED
	#define PPM_FASTBIN_TRIM_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_FASTBINS_ENABLED (defined as 0 or 1; default is 1)
//
// If disabled then the GeneralAllocator "fast bin" code is not compiled.
// It may be useful to disable fast bins if you are always supplying your own
// small block allocator and don't need the one within GeneralAllocator.
//
#ifndef PPM_FASTBINS_ENABLED
	#define PPM_FASTBINS_ENABLED 1
#endif

///////////////////////////////////////////////////////////////////////////////
// PPM_DEBUG_CALLSTACK_AVAILABLE
//
// If PPM_DEBUG_CALLSTACK_AVAILABLE is defined, then we assume that the 
// following functions are available:
//
//    void   InitCallstack();
//    void   ShutdownCallstack();
//    size_t GetCallStack(void* pReturnAddressArray[], size_t nReturnAddressArrayLength);
//    size_t DescribeCallStack(const void* pReturnAddressArray[], size_t nReturnAddressArrayLength, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase* addressRepLookup=nullptr);
//
// For the platforms below, these functions are provided by PPMalloc, 
// but for others the user would have to implement them. Callstack functionality
// is optional but allows for built-in callstack debug tracing. 
// See PPMalloc/internal/shared.h for a description of the functions
// inmore detail.
// 
#ifndef PPM_DEBUG_CALLSTACK_AVAILABLE
	#if defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) && defined(EA_PROCESSOR_X86) 
		// Windows 8 Metro apps do not have access to DbgHelp APIs and so callstack support for x86 is not yet implemented.
		#define PPM_DEBUG_CALLSTACK_AVAILABLE 0
	#elif defined(EA_PLATFORM_MICROSOFT) || defined(__APPLE__) || defined(EA_PLATFORM_ANDROID) || (defined(__GNUC__) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)))
		#define PPM_DEBUG_CALLSTACK_AVAILABLE 1
	#else
		#define PPM_DEBUG_CALLSTACK_AVAILABLE 0 // Adjust this setting as we obtain code for additional platforms.
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// PPM_BACKTRACE_AVAILABLE
//
// PPM_BACKTRACE_AVAILABLE is defined to 1 if the backtrace/backtrace_symbols
// functions are available on the target platform, 0 otherwise.
#if defined(EA_PLATFORM_APPLE) || (defined(EA_PLATFORM_LINUX) && !defined(EA_PLATFORM_ANDROID)) || defined(EA_PLATFORM_BSD)
	#define PPM_BACKTRACE_AVAILABLE 1
#else
	#define PPM_BACKTRACE_AVAILABLE 0
#endif

///////////////////////////////////////////////////////////////////////////////
// PPM_NEW_CORE_SIZE_DEFAULT (unsigned integer, platform-specific)
//
// This is currently specific to GeneralAllocator.
// Specifies the default size of new core memory blocks when they are  
// automatically internally allocated.
//
#ifndef PPM_NEW_CORE_SIZE_DEFAULT
	#if defined(EA_PLATFORM_WINDOWS)
		#define PPM_NEW_CORE_SIZE_DEFAULT        (16 * 1024 * 1024) // 16 Mebibytes. Same as 256 * 65536, with 65536 being default Win32 region size.
	#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__) // Note that this covers all Unix variants, including MacOSX.
		#define PPM_NEW_CORE_SIZE_DEFAULT        (16 * 1024 * 1024)
	#elif defined(EA_PLATFORM_MICROSOFT)
		#define PPM_NEW_CORE_SIZE_DEFAULT        (16 * 1024 * 1024)
	#elif defined(EA_PLATFORM_SONY)
		#define PPM_NEW_CORE_SIZE_DEFAULT        (16 * 1024 * 1024)
	#else
		#define PPM_NEW_CORE_SIZE_DEFAULT        ( 4 * 1024 * 1024)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_CORE_INCREMENT_SIZE_DEFAULT (unsigned integer, platform-specific)
//
// This is currently specific to GeneralAllocator.
// Default size to add to core memory blocks when the system supports 
// extending existing blocks.
//
#ifndef PPM_CORE_INCREMENT_SIZE_DEFAULT
	#define  PPM_CORE_INCREMENT_SIZE_DEFAULT (PPM_NEW_CORE_SIZE_DEFAULT / 4)
#endif



///////////////////////////////////////////////////////////////////////////////
// PPM_DEBUG_PRESERVE_PRIOR (defined as 0 or 1. Default is same as PPM_DEBUG)
//
// This is currently specific to GeneralAllocator.
// This is a define that tells whether we are saving the mnPrior field of chunks
// for debug purposes instead of letting them be (intentionally) overrun by the
// previous chunk (in order to save memory). You generally don't want to change
// this value unless you are doing maintenance and testing work on this library.
//
#ifndef PPM_DEBUG_PRESERVE_PRIOR
	#define PPM_DEBUG_PRESERVE_PRIOR PPM_DEBUG
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_MMAP_SUPPORTED (defined as 0 or 1; default is 0)
//
// This is currently specific to GeneralAllocator.
// If true, GeneralAllocator::Malloc makes use mapped memory to allocate very large blocks. 
// This also enables the use of mapped memory if normal core memory allocation 
// mechanisms fail.
//
#ifndef PPM_MMAP_SUPPORTED
	#if defined(EA_PLATFORM_WINDOWS_PHONE) || (defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) // If Windows Metro / WinRT...
		#define PPM_MMAP_SUPPORTED 0    // VirtualAlloc is not supported.
	#elif defined(EA_PLATFORM_MICROSOFT) 
		#define PPM_MMAP_SUPPORTED 1    // VirtualAlloc is supported.
	#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__) || defined(EA_PLATFORM_SONY) || defined(EA_PLATFORM_NX)
		#define PPM_MMAP_SUPPORTED 1    // mmap is supported.
	#else
		// Note that the XBox and XBox 2 platforms have virtual memory (not to be
		// confused with paged memory) via VirtualAlloc. However, since XBox 
		// doesn't have paged memory, VirtualAlloc gets its memory from free
		// physical RAM. But most usually all free physical RAM is given to the 
		// GeneralAllocator or some other pool. The only way VirtualAlloc would
		// be useful is if the user reserved some space for VirtualAlloc calls.
		// So what we do is disable mapped memory by default in XBox platforms
		// but allow the user to override it if desired.
		#define PPM_MMAP_SUPPORTED 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_PLATFORM_MIN_MALLOC_ALIGNMENT
// 
// This indicates the lowest memory alignment support by the GeneralAllocator.
// We default to the lowest support alignment for the target platform C-runtime
// malloc implementation.
//
#if defined(EA_PLATFORM_MIN_MALLOC_ALIGNMENT) // Recent versions of EABase now have a define for this.
	#define PPM_MIN_ALIGN_LOWEST_ALLOWED EA_PLATFORM_MIN_MALLOC_ALIGNMENT
#else
	#define PPM_MIN_ALIGN_LOWEST_ALLOWED (EA_PLATFORM_PTR_SIZE * 2)
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_MIN_ALIGN (defined as 8, 16, 32, 64, etc.; default is (EA_PLATFORM_PTR_SIZE * 2))
//
// This is currently specific to GeneralAllocator.
// If true, then GeneralAllocator allocations have a minimum alignment of N instead
// of PPM_MIN_ALIGN_LOWEST_ALLOWED. Platforms with 64 bit pointers already have a min 
// alignment of N and this setting has no effect in that case. Platforms with 32 bit 
// pointers default to having 8 bit minimum alignment. However, some of these 32 bit 
// platforms have 128 bit data types (e.g. vpu vector types) which need to 
// be aligned on 16 byte boundaries. If you are allocating a lot of these
// objects or structs that contain these objects, you may be better off setting
// the default alignment to 16 instead of 8, as otherwise you'd have to call
// MallocAligned, which necessarily fragments memory. However, consider that
// code you write under 16 byte min alignment may silently break if you move 
// it to another environment which doesn't have such alignment.
//
#if defined(PPM_MIN_ALIGN) && (PPM_MIN_ALIGN < PPM_MIN_ALIGN_LOWEST_ALLOWED)
	#undef  PPM_MIN_ALIGN 
#endif

#if !defined(PPM_MIN_ALIGN)
	#define PPM_MIN_ALIGN PPM_MIN_ALIGN_LOWEST_ALLOWED
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_WIN_MEM_WATCH
//
// This is currently specific to GeneralAllocator.
// Defined as an integral flag value. The value is 0 if disabled/unused.
// This is a wrapper for the Win32/Win64 MEM_WRITE_WATCH flag used by VirtualAlloc.
//
#if !defined(PPM_WIN_MEM_WATCH)
	#if defined(EA_PLATFORM_WINDOWS) && (PPM_DEBUG >= 1)
		#if defined(MEM_WRITE_WATCH)
			#define PPM_WIN_MEM_WATCH MEM_WRITE_WATCH // MEM_WRITE_WATCH is defined in Microsoft SDK headers.
		#else
			#define PPM_WIN_MEM_WATCH 0x00200000
		#endif
	#else
		#define PPM_WIN_MEM_WATCH 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_INCREMENTAL_OBJECTS_ENABLED (defined as 0 or 1; default is 1)
//
// This is currently specific to StackAllocator.
// Defines whether the building of incremental (or streamed) objects is enabled.
// When incremental objects are enabled, you can build an allocation incrementally
// instead of all at once. Building an object incrementally via multiple calls 
// to AppendToCurrentObject is not the same as making multiple calls to Malloc,
// as the latter aligns allocations whereas the former doesn't. Also, the former
// is a little faster as it has less to do. Incremental objects are useful for
// building streams of data on the fly and is also useful for building static 
// string tables or similar tables on the fly.
//
// Given that this class is entirely inlined, it makes little or no performance
// difference whether PPM_INCREMENTAL_OBJECTS_ENABLED is enabled or not. It is 
// provided here as a conveniece and as a mechanism for preventing its usage in 
// the case where you want to minimize code usage on tiny platforms.
//
#ifndef PPM_INCREMENTAL_OBJECTS_ENABLED
	#define PPM_INCREMENTAL_OBJECTS_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLED (defined as 0 or 1, default is 1)
//
// When this is enabled, this module recognizes PPMalloc modules external to 
// the currently compiled module. If disabled, then this module has no 
// dependencies on anything except standard C/C++ libraries and you can thus 
// use it with other allocation systems.
//
#ifndef PPM_ENABLED
	#define PPM_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_DEBUG_DATA_SUPPORTED (defined as 0 or 1; default is 0 in debug, 1 for release builds)
//
// This is currently specific to NonLocalAllocator.
// Defines if debug functionality capabilities are compiled into NonLocalAllocator.
// Even if PPM_NLA_DEBUG_DATA_SUPPORTED is 1, the debug functionality still needs
// to be enabled on a feature-by-feature basis in the NonLocalAllocator API.
// The reason we have PPM_NLA_DEBUG_DATA_SUPPORTED is that when it is set to 0 then
// the generated code is simpler and smaller than otherwise. 
//
#ifndef PPM_NLA_DEBUG_DATA_SUPPORTED
	#if PPM_DEBUG
		#define PPM_NLA_DEBUG_DATA_SUPPORTED 1
	#else
		#define PPM_NLA_DEBUG_DATA_SUPPORTED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_METRICS_SUPPORTED (defined as 0 or 1; default is 0 in debug, 1 for release builds)
//
// This is currently specific to NonLocalAllocator.
// Defines if metrics functionality capabilities are compiled into NonLocalAllocator.
// Even if PPM_NLA_METRICS_SUPPORTED is 1, the metrics functionality still needs
// to be enabled in the NonLocalAllocator API.
// The reason we have PPM_NLA_METRICS_SUPPORTED is that when it is set to 0 then
// the generated code is simpler and smaller than otherwise. 
//
#ifndef PPM_NLA_METRICS_SUPPORTED
	#if PPM_DEBUG
		#define PPM_NLA_METRICS_SUPPORTED 1
	#else
		#define PPM_NLA_METRICS_SUPPORTED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_HOOKS_SUPPORTED (defined as 0 or 1; default is 0 in debug, 1 for release builds)
//
// This is currently specific to NonLocalAllocator.
//
#ifndef PPM_NLA_HOOKS_SUPPORTED
	#if PPM_DEBUG
		#define PPM_NLA_HOOKS_SUPPORTED 1
	#else
		#define PPM_NLA_HOOKS_SUPPORTED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_HEAP_VALIDATION_SUPPORTED (defined as 0 or 1; default is 0 in debug, 1 for release builds)
//
// This is currently specific to NonLocalAllocator.
//
#ifndef PPM_NLA_HEAP_VALIDATION_SUPPORTED
	#if PPM_DEBUG
		#define PPM_NLA_HEAP_VALIDATION_SUPPORTED 1
	#else
		#define PPM_NLA_HEAP_VALIDATION_SUPPORTED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_MIN_ALLOC_SIZE (defined as 8, 16, 32, 64, etc.; default is 1)
//
// This is currently specific to NonLocalAllocator.
//
#if !defined(PPM_NLA_MIN_ALLOC_SIZE)
	#define PPM_NLA_MIN_ALLOC_SIZE 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_MIN_ALIGN (defined as 8, 16, 32, 64, etc.; default is 1)
//
// This is currently specific to NonLocalAllocator.
// PPM_NLA_MIN_ALIGN must be >= PPM_NLA_MIN_ALLOC_SIZE. This is verified in the 
// source implementation at compile-time.
//
#if !defined(PPM_NLA_MIN_ALIGN)
	#define PPM_NLA_MIN_ALIGN 1
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_64BIT_BIN_COUNTS_ENABLED
//
#if !defined(PPM_NLA_64BIT_BIN_COUNTS_ENABLED)
	#if defined(EA_PROCESSOR_X86_64) || defined(EA_PROCESSOR_ARM64)
		#define PPM_NLA_64BIT_BIN_COUNTS_ENABLED 1      // Use higher bin count on Gen4
	#else
		#define PPM_NLA_64BIT_BIN_COUNTS_ENABLED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_NLA_BIN_COUT
//
#if PPM_NLA_64BIT_BIN_COUNTS_ENABLED
	#define PPM_NLA_BIN_COUNT 40
#else
	#define PPM_NLA_BIN_COUNT 12 
#endif

///////////////////////////////////////////////////////////////////////////////
// PPM_FREE_LIST_SCALABILITY_ENABLED
//
// Defined as 0 or 1. Defaults to 1 on platforms that might need it (e.g. Windows, Unix).
// If enabled then implements a system that allows the free lists to be more 
// scalable in the face of huge numbers (e.g. tens of thousands) of free blocks.
// However, this feature comes at the cost of 1K of additional memory usage by 
// a GeneralAllocator instance and a small performance drop due to the maintenance
// of additional data structures.
//
#if !defined(PPM_FREE_LIST_SCALABILITY_ENABLED)
	#if defined(EA_PLATFORM_DESKTOP)
		#define PPM_FREE_LIST_SCALABILITY_ENABLED 1
	#else
		#define PPM_FREE_LIST_SCALABILITY_ENABLED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_FOPEN_AVAILABLE
//
// Defines as 0 or 1.
//
#if defined(EA_PLATFORM_NINTENDO)  // On Nintendo platforms fopen and siblings will crash or be unlinkable.
	#define PPM_FOPEN_AVAILABLE 0 
#else
	#define PPM_FOPEN_AVAILABLE 1 
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_XBDM_ENABLED
//
// Defined as 0 or 1, with 1 being the default for debug builds.
// This controls whether xbdm library usage is enabled on XBox 360. This library
// allows for runtime debug functionality. But shipping applications are not
// allowed to use xbdm. 
//
#if !defined(EA_XBDM_ENABLED)
	#if defined(EA_DEBUG)
		#define EA_XBDM_ENABLED 1
	#else
		#define EA_XBDM_ENABLED 0
	#endif
#endif




#endif // Header include guard




