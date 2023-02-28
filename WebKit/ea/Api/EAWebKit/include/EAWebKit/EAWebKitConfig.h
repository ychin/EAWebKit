/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2018, 2020 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EAWebKitConfig.h
// By Paul Pedriana 
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITCONFIG_H
#define EAWEBKIT_EAWEBKITCONFIG_H

#include <EABase/eabase.h>


#ifndef EAWEBKIT_VERSION
	#define EAWEBKIT_VERSION_YEAR   16
	#define EAWEBKIT_VERSION_SEASON 4
	#define EAWEBKIT_VERSION_MAJOR  1
	#define EAWEBKIT_VERSION_MINOR  0
	#define EAWEBKIT_VERSION_PATCH  2
	#define EAWEBKIT_MAKE_VERSION(Year, Season, Major, Minor, Patch) (Year * 100000000 + Season * 1000000 + Major * 10000 + Minor * 100 + Patch)
	#define EAWEBKIT_VERSION  EAWEBKIT_MAKE_VERSION(EAWEBKIT_VERSION_YEAR,EAWEBKIT_VERSION_SEASON,EAWEBKIT_VERSION_MAJOR,EAWEBKIT_VERSION_MINOR,EAWEBKIT_VERSION_PATCH)      
	
	#define EAWEBKIT_VERSION_STRINGIFY_HELPER(s) #s
	#define EAWEBKIT_MAKE_VERSION_S(Year, Season, Major, Minor, Patch) EAWEBKIT_VERSION_STRINGIFY_HELPER(Year)"." EAWEBKIT_VERSION_STRINGIFY_HELPER(Season)"." EAWEBKIT_VERSION_STRINGIFY_HELPER(Major)"." EAWEBKIT_VERSION_STRINGIFY_HELPER(Minor)"." EAWEBKIT_VERSION_STRINGIFY_HELPER(Patch)
	#define EAWEBKIT_VERSION_S  EAWEBKIT_MAKE_VERSION_S(EAWEBKIT_VERSION_YEAR,EAWEBKIT_VERSION_SEASON,EAWEBKIT_VERSION_MAJOR,EAWEBKIT_VERSION_MINOR,EAWEBKIT_VERSION_PATCH)      
#endif



/************************************************************************************************
*																								*
*																								*
Since EAWebKit is not statically linked, following defines are not useful for the application.	*
They are kept here just for convenience.														*
*																								*
*																								*
*************************************************************************************************/




///////////////////////////////////////////////////////////////////////////////
// EAWEBKIT_DEBUG
//
// Defined as an integer >= 0. Default is 1 for debug builds and 0 for 
// release builds. This define is also a master switch for the default value 
// of some other settings.
//
// Example usage:
//    #if EAWEBKIT_DEBUG
//       ...
//    #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_DEBUG
    #if defined(EA_DEBUG) || defined(_DEBUG)
        #define EAWEBKIT_DEBUG 1
    #else
        #define EAWEBKIT_DEBUG 0
    #endif
#endif

///////////////////////////////////////////////////////////////////////////////
// EAWEBKIT_MEMORY_DEBUG
//
// Defined as an integer >= 0. Default is 1 for debug builds and 0 for 
// release builds. 
//
// Example usage:
//    #if EAWEBKIT_MEMORY_DEBUG
//       ...
//    #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_MEMORY_DEBUG
    #if defined(EA_DEBUG) || defined(_DEBUG)
        #define EAWEBKIT_MEMORY_DEBUG 1
    #else
        #define EAWEBKIT_MEMORY_DEBUG 0
    #endif
#endif




///////////////////////////////////////////////////////////////////////////////
// EAWEBKIT_ASSERT_ENABLED
//
// Defined as 0 or 1. The default is to equal EAWEBKIT_DEBUG.
// Enables the EAW_ASSERT macros.
//
#ifndef EAWEBKIT_ASSERT_ENABLED
    #define EAWEBKIT_ASSERT_ENABLED EAWEBKIT_DEBUG
#endif



#endif // EAWEBKIT_EAWEBKITCONFIG_H
