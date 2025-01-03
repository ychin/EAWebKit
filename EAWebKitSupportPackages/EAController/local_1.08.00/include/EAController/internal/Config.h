///////////////////////////////////////////////////////////////////////////////
// Config.h
//
// Copyright (c) 2013 Electronic Arts Inc.
// Written and maintained by Kevin MacAulay Vacheresse.
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// EACONTROLLER_VERSION
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
//     printf("EACONTROLLER_VERSION version: %s", EACONTROLLER_VERSION);
//     printf("EACONTROLLER_VERSION version: %d.%d.%d", EACONTROLLER_VERSION / 10000 % 100, EACONTROLLER_VERSION_N / 100 % 100, EACONTROLLER_VERSION_N % 100);
//
#ifndef EACONTROLLER_VERSION
	#define EACONTROLLER_VERSION   	"1.08.00"
	#define EACONTROLLER_VERSION_N  10800

	// Older style version info
	#define EACONTROLLER_VERSION_MAJOR (EACONTROLLER_VERSION_N / 100 / 100 % 100)
	#define EACONTROLLER_VERSION_MINOR (EACONTROLLER_VERSION_N       / 100 % 100)
	#define EACONTROLLER_VERSION_PATCH (EACONTROLLER_VERSION_N             % 100)
#endif
