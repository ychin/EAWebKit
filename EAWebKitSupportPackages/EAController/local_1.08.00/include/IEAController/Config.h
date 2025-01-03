#pragma once

#include <EAController/internal/Config.h>

///////////////////////////////////////////////////////////////////////////////
// IEACONTROLLER_VERSION
//
#ifndef IEACONTROLLER_VERSION
	#define IEACONTROLLER_VERSION		EACONTROLLER_VERSION
	#define IEACONTROLLER_VERSION_N  	EACONTROLLER_VERSION_N
#endif


///////////////////////////////////////////////////////////////////////////////
// EACONTROLLER_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EACONTROLLER_DLL is 1, else EACONTROLLER_DLL is 0.
// EA_DLL is a define that controls DLL builds within the eaconfig build system. 
// EACONTROLLER_DLL controls whether EACONTROLLER is built and used as a DLL. 
//
#ifndef EACONTROLLER_DLL
	#if defined(EA_DLL)
		#define EACONTROLLER_DLL 1
	#else
		#define EACONTROLLER_DLL 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// IEACONTROLLER_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EACONTROLLER as a DLL and its
// non-templated functions will be exported.
//
#ifndef EACONTROLLER_API // If the build file hasn't already defined this to be dllexport...
	#if EACONTROLLER_DLL && defined(_MSC_VER)
		#define EACONTROLLER_API __declspec(dllimport)
	#else
		#define EACONTROLLER_API // Neither dllexport nor dllimport; just regular static linking.
	#endif
#endif