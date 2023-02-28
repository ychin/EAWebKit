///////////////////////////////////////////////////////////////////////////////
// Config.h
//
// Copyright (c) 2013 Electronic Arts Inc.
// Written and maintained by Kevin MacAulay Vacheresse.
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// EAUSER_VERSION
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
//     printf("EAUSER_VERSION version: %s", EAUSER_VERSION);
//     printf("EAUSER_VERSION version: %d.%d.%d", EAUSER_VERSION / 10000 % 100, EAUSER_VERSION_N / 100 % 100, EAUSER_VERSION_N % 100);
//
#ifndef EAUSER_VERSION
	#define EAUSER_VERSION   "1.09.08"
	#define EAUSER_VERSION_N   10908

	// Older style version info
	#define EAUSER_VERSION_MAJOR (EAUSER_VERSION_N / 100 / 100 % 100)
	#define EAUSER_VERSION_MINOR (EAUSER_VERSION_N       / 100 % 100)
	#define EAUSER_VERSION_PATCH (EAUSER_VERSION_N             % 100)
#endif


///////////////////////////////////////////////////////////////////////////////
// EAUSER_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EAUSER_DLL is 1, else IEAUSER_DLL is 0.
// EA_DLL is a define that controls DLL builds within the eaconfig build system. 
// EAUSER_DLL controls whether IEAUser is built and used as a DLL. 
//
#ifndef EAUSER_DLL
	#if defined(EA_DLL)
		#define EAUSER_DLL 1
	#else
		#define EAUSER_DLL 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EAUSER_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EAUser as a DLL and its
// non-templated functions will be exported.
//
#ifndef EAUSER_API // If the build file hasn't already defined this to be dllexport...
	#if EAUSER_DLL && defined(_MSC_VER)
		#define EAUSER_API __declspec(dllimport)
	#else
		#define EAUSER_API // Neither dllexport nor dllimport; just regular static linking.
	#endif
#endif
