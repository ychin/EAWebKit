///////////////////////////////////////////////////////////////////////////////
// Config.h
//
// Copyright (c) 2006 Electronic Arts Inc.
// Written and maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAMESSAGE_INTERNAL_CONFIG_H
#define EAMESSAGE_INTERNAL_CONFIG_H


#include <EABase/eabase.h>


///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_VERSION
//
// We more or less follow the conventional EA packaging approach to versioning 
// here. A primary distinction here is that minor versions are defined as two
// digit entities (e.g. .03") instead of minimal digit entities ".3"). The logic
// here is that the value is a counter and not a floating point fraction.
// Note that the major version doesn't have leading zeros.
//
// Example version strings:
//      "0.91.00"   // Major version 0, minor version 91, patch version 0. 
//      "1.00.00"   // Major version 1, minor and patch version 0.
//      "3.10.02"   // Major version 3, minor version 10, patch version 02.
//     "12.03.01"   // Major version 12, minor version 03, patch version 
//
// Example usage:
//     printf("EAMESSAGE version: %s", EAMESSAGE_VERSION);
//     printf("EAMESSAGE version: %d.%d.%d", EAMESSAGE_VERSION_N / 10000 % 100, EAMESSAGE_VERSION_N / 100 % 100, EAMESSAGE_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EAMESSAGE_VERSION
	#define EAMESSAGE_VERSION   "2.10.06"
	#define EAMESSAGE_VERSION_N  21006
#endif


///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_ALLOC_PREFIX
//
// Defined as a string literal. Defaults to this package's name.
// Can be overridden by the user by predefining it or by editing this file.
// This define is used as the default name used by this package for naming
// memory allocations and memory allocators.
//
// All allocations names follow the same naming pattern:
//     <package>/<module>[/<specific usage>]
// 
// Example usage:
//     void* p = pCoreAllocator->Alloc(37, EAMESSAGE_ALLOC_PREFIX, 0);
//
// Example usage:
//     gMessageServer.GetMessageQueue().get_allocator().set_name(EAMESSAGE_ALLOC_PREFIX "MessageSystem/Queue");
//
#ifndef EAMESSAGE_ALLOC_PREFIX
	#define EAMESSAGE_ALLOC_PREFIX "EAMessage/"
#endif



///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_ENABLE_DEFAULT_ALLOCATOR
//
// Defines if EA::Allocator::ICoreAllocator::GetDefaultAllocator is used.
// Default is 1 if GetDefaultAllocator exists in the CoreAllocator package.
// The long term goal is to eliminate GetDefaultAllocator usage. This feature
// here is a bridge towards this goal.
//
#ifndef EAMESSAGE_ENABLE_DEFAULT_ALLOCATOR
	#define EAMESSAGE_ENABLE_DEFAULT_ALLOCATOR 1
#endif

#if EAMESSAGE_ENABLE_DEFAULT_ALLOCATOR
	#define EAMESSAGE_GET_DEFAULT_ALLOCATOR() EA::Allocator::ICoreAllocator::GetDefaultAllocator()
#else
	#define EAMESSAGE_GET_DEFAULT_ALLOCATOR() NULL
#endif



///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_DEFER_ENABLED
// 
// Defined as 0 or 1. Default is 1 but may be overridden by your project file.
// Defines whether EAMessageDefer.h/cpp are enabled. EAMessageDefer extends
// EAMessage functionality but has a dependency on the UTFFoundation package.
//
#ifdef EA_MESSAGE_DEFER_ENABLED // Backwards compatibility with old name.
	#undef  EAMESSAGE_DEFER_ENABLED
	#define EAMESSAGE_DEFER_ENABLED EA_MESSAGE_DEFER_ENABLED
#endif
#ifndef EAMESSAGE_DEFER_ENABLED
	#define EAMESSAGE_DEFER_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
/// EAMESSAGE_VIRTUAL_ENABLED
/// 
/// Defined as 0 or 1. Default is 1 for desktop platforms.
/// Controls the use of virtual functions for this system.
/// Some functions/classes are necessarily virtual by design and 
/// this setting does not affect them.
///
#ifdef EAM_VIRTUAL_ENABLED // Backwards compatibility with old name.
	#undef  EAMESSAGE_VIRTUAL_ENABLED
	#define EAMESSAGE_VIRTUAL_ENABLED EAM_VIRTUAL_ENABLED
#endif
#ifndef EAMESSAGE_VIRTUAL_ENABLED
	#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_UNIX)
		#define EAMESSAGE_VIRTUAL_ENABLED 1
	#else
		#define EAMESSAGE_VIRTUAL_ENABLED 0
	#endif
#endif
#if EAMESSAGE_VIRTUAL_ENABLED
	#define EAM_VIRTUAL virtual
#else
	#define EAM_VIRTUAL
#endif



///////////////////////////////////////////////////////////////////////////////
/// EAMESSAGE_C_HANDLER_ENABLED
/// 
/// Defined as 0 or 1. Default is 1 for the time being.
/// Enables the use of the (now deprecated) C (as opposed to C++) message handling
/// functionality. The C functionality will be removed algother in the future, 
/// but we provide temporary support when EAMESSAGE_C_HANDLER_ENABLED is defined to 1.
///
#ifdef EAM_C_HANDLER_ENABLED // Backwards compatibility with old name.
	#undef  EAMESSAGE_C_HANDLER_ENABLED
	#define EAMESSAGE_C_HANDLER_ENABLED EAM_C_HANDLER_ENABLED
#endif
#ifndef EAMESSAGE_C_HANDLER_ENABLED
	#define EAMESSAGE_C_HANDLER_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
/// EAMESSAGE_TRACE_ENABLED
///
/// Defined as 0 or 1. Default is to be the same as EA_TRACE_ENABLED.
///
#ifdef EAM_TRACE_ENABLED // Backwards compatibility with old name.
	#define EAMESSAGE_TRACE_ENABLED EAM_TRACE_ENABLED
#endif
#ifndef EAMESSAGE_TRACE_ENABLED
	// It is expected that the user make EA_TRACE_ENABLED visible manually.
	// We intentionally do not make a dependency in EATrace.h
	#if defined(EA_TRACE_ENABLED) && (EA_TRACE_ENABLED != 0)
		#define EAMESSAGE_TRACE_ENABLED 1
	#else
		#define EAMESSAGE_TRACE_ENABLED 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// UTF_USE_EATRACE    Defined as 0 or 1. Default is 1 but may be overridden by your project file.
// UTF_USE_EAASSERT   Defined as 0 or 1. Default is 0.
//
// Defines whether this package uses the EATrace or EAAssert package to do assertions.
// EAAssert is a basic lightweight package for portable asserting.
// EATrace is a larger "industrial strength" package for assert/trace/log.
// If both UTF_USE_EATRACE and UTF_USE_EAASSERT are defined as 1, then UTF_USE_EATRACE
// overrides UTF_USE_EAASSERT.
//
#if defined(UTF_USE_EATRACE) && UTF_USE_EATRACE
	#undef  UTF_USE_EAASSERT
	#define UTF_USE_EAASSERT 0
#elif !defined(UTF_USE_EAASSERT)
	#define UTF_USE_EAASSERT 0
#endif

#undef UTF_USE_EATRACE
#if defined(UTF_USE_EAASSERT) && UTF_USE_EAASSERT
	#define UTF_USE_EATRACE 0
#else
	#define UTF_USE_EATRACE 1
#endif

#if !defined(EA_ASSERT_HEADER)
	#if defined(UTF_USE_EAASSERT) && UTF_USE_EAASSERT
		#define EA_ASSERT_HEADER <EAAssert/eaassert.h>
	#else
		#define EA_ASSERT_HEADER <EATrace/EATrace.h>
	#endif
#endif
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EAMESSAGE_DLL is 1, else EAMESSAGE_DLL is 0.
// EA_DLL is a define that controls DLL builds within the EAConfig build system. 
// EAMESSAGE_DLL controls whether EAMESSAGE is built and used as a DLL. 
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#ifndef EAMESSAGE_DLL
	#if defined(EA_DLL)
		#define EAMESSAGE_DLL 1
	#else
		#define EAMESSAGE_DLL 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EAMESSAGE_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EAMessage as a DLL and EAMessage's
// non-templated functions will be exported. EAMessage template functions are not
// labelled as EAMESSAGE_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of EAMESSAGE_API:
//    EAMESSAGE_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct EAMESSAGE_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        EAMESSAGE_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    EAMESSAGE_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#ifndef EAMESSAGE_API // If the build file hasn't already defined this to be dllexport...
	#if EAMESSAGE_DLL 
		#if defined(_MSC_VER)
			#define EAMESSAGE_API      __declspec(dllimport)
			#define EAMESSAGE_LOCAL
		#elif defined(__CYGWIN__)
			#define EAMESSAGE_API      __attribute__((dllimport))
			#define EAMESSAGE_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define EAMESSAGE_API      __attribute__ ((visibility("default")))
			#define EAMESSAGE_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define EAMESSAGE_API
			#define EAMESSAGE_LOCAL
		#endif
	#else
		#define EAMESSAGE_API
		#define EAMESSAGE_LOCAL
	#endif
#endif



#endif // Header include guard

















