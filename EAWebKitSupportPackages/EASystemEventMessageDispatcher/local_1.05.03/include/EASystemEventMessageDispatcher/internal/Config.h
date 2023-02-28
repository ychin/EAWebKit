///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <EABase/eabase.h>

///////////////////////////////////////////////////////////////////////////////
// EASEMD_VERSION
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
//     printf("EASEMD_VERSION version: %s", EASEMD_VERSION);
//     printf("EASEMD_VERSION version: %d.%d.%d", EASEMD_VERSION / 10000 % 100, EASEMD_VERSION_N / 100 % 100, EASEMD_VERSION_N % 100);
//
#ifndef EASEMD_VERSION
	#define EASEMD_VERSION   "1.05.03"
	#define EASEMD_VERSION_N   10503

	// Older style version info
	#define EASEMD_VERSION_MAJOR (EASEMD_VERSION_N / 100 / 100 % 100)
	#define EASEMD_VERSION_MINOR (EASEMD_VERSION_N       / 100 % 100)
	#define EASEMD_VERSION_PATCH (EASEMD_VERSION_N             % 100)
#endif


///////////////////////////////////////////////////////////////////////////////
// EASEMD_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building the package as a DLL hence
// non-templated functions will be exported. The package template functions are not
// labelled as EASEMD_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of EASEMD_API:
//    EASEMD_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct EASEMD_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        EASEMD_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    EASEMD_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#ifndef EASEMD_API // If the build file hasn't already defined this to be dllexport...
	#if EASEMD_DLL 
		#if defined(_MSC_VER)
			#define EASEMD_API      __declspec(dllimport)
			#define EASEMD_LOCAL
		#elif defined(__CYGWIN__)
			#define EASEMD_API      __attribute__((dllimport))
			#define EASEMD_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define EASEMD_API      __attribute__ ((visibility("default")))
			#define EASEMD_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define EASEMD_API
			#define EASEMD_LOCAL
		#endif
	#else
		#define EASEMD_API
		#define EASEMD_LOCAL
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASEMD_DEBUG
//
// Enables extra reporting for the library such as failing when system 
// messages generate errors
///////////////////////////////////////////////////////////////////////////////
#ifndef EASEMD_DEBUG
	#if defined EA_DEBUG || defined EASEMD_ENABLE_DEBUG /* defined in build file */
		#define EASEMD_DEBUG EA_ENABLED
	#else
		#define EASEMD_DEBUG EA_DISABLED
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASEMD_DEBUG
//
// Enables use of the companion libraries. These libraries are necessary to 
// respond to events related to companion applications for a title, such as 
// connection events.
///////////////////////////////////////////////////////////////////////////////
#ifndef EASEMD_USE_COMPANION_LIBS
	#if defined EASEMD_ENABLE_COMPANION_LIBS /* defined in build file */
		#define EASEMD_USE_COMPANION_LIBS EA_ENABLED
	#else
		#define EASEMD_USE_COMPANION_LIBS EA_DISABLED
	#endif
#endif 
