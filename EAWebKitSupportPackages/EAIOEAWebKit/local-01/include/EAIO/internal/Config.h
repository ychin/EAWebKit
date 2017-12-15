/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// Config.h
//
// Copyright (c) 2006 Electronic Arts Inc.
// Written and maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_INTERNAL_CONFIG_H
#define EAIO_INTERNAL_CONFIG_H


#include <EABase/eabase.h>


///////////////////////////////////////////////////////////////////////////////
// EAIO_VERSION
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
//     printf("EAIO version: %s", EAIO_VERSION);
//     printf("EAIO version: %d.%d.%d", EAIO_VERSION_N / 10000 % 100, EAIO_VERSION_N / 100 % 100, EAIO_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EAIO_VERSION
    #define EAIO_VERSION   "2.09.01"
    #define EAIO_VERSION_N  20901
#endif





///////////////////////////////////////////////////////////////////////////////
// EAIO_INIFILE_ENABLED
//
// If defined as 1, then the IniFile class is made available. The reason it is 
// disabled by default, at least for the time being, is that it conflicts with
// the existing UTFFoundation identical class. EAIniFile is in fact in the 
// process of being moved from UTFFoundation to EAIO so that UTFFoundation can
// be formally deprecated.
//
#ifndef EAIO_INIFILE_ENABLED
    #define EAIO_INIFILE_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_ALLOC_PREFIX
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
//     void* p = pCoreAllocator->Alloc(37, EAIO_ALLOC_PREFIX, 0);
//
// Example usage:
//     gMessageServer.GetMessageQueue().get_allocator().set_name(EAIO_ALLOC_PREFIX "MessageSystem/Queue");
//
#ifndef EAIO_ALLOC_PREFIX
    #define EAIO_ALLOC_PREFIX "EAIO/"
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
// EAIO_64_BIT_SIZE_ENABLED
//
// Defined as 0 or 1. Default is 0.
//
#ifndef EAIO_64_BIT_SIZE_ENABLED
    #define EAIO_64_BIT_SIZE_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED
//
// Defined as 0 or 1. Default is 1 (for backward compatibility).
// Enables the implementation of ICoreAllocator::GetDefaultAllocator
// in DLL builds. If you build EAIO as a standalone DLL then you typically
// need provide a local implementation of the ICoreAllocator::GetDefaultAllocator 
// function. However, some user usage patterns are such that they would prefer
// to disable this implementation, because they link in a way that it causes 
// problems.
//
#ifndef EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED
    #define EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_DEFAULT_ALLOCATOR_ENABLED
//
// Defined as 0 or 1. Default is 1 (for backward compatibility).
// Enables the use of ICoreAllocator::GetDefaultAllocator.
// If disabled then the user is required to explicitly specify the 
// allocator used by EAIO with EA::IO::SetAllocator() or on a class by
// class basis within EAIO. Note that this option is different from the 
// EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED option. This option enables the 
// calling of GetDefaultAllocator by this package, the former enables the 
// actual implementation of GetDefaultAllocator within this package.
//
#ifndef EAIO_DEFAULT_ALLOCATOR_ENABLED
    #define EAIO_DEFAULT_ALLOCATOR_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_DIRECTORY_ITERATOR_USE_PATHSTRING
//
// Defined as 0 or 1. 1 means to use EAIO PathString.
// This controls whether the DirectoryIterator class in EAFileDirectory.h 
// uses EAIO PathString or straight EASTL string16. The latter is the old 
// setting, and the former is the new setting. The problem with the old
// use of EASTL string16 is that it doesn't use the EAIO memory allocator
// and instead uses whatever global EASTL allocator is defined.
//
#ifndef EAIO_DIRECTORY_ITERATOR_USE_PATHSTRING
    #define EAIO_DIRECTORY_ITERATOR_USE_PATHSTRING 0  // Defaults to zero until old package versions which depend on it as zero are obsolete.
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_FILEPATH_ENABLED
//
// Defined as 0 or 1. Default is 0.
// Defines if the old EAFilePath.h/cpp functionality in the compat directory
// is enabled and available. This #define doesn't enable compiling of that
// module; the build system has to do that.
//
#ifndef EAIO_FILEPATH_ENABLED
    #define EAIO_FILEPATH_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_BACKWARDS_COMPATIBILITY
//
// Defined as 0 or 1. Default is 1 (for backward compatibility).
// Enables some deprecated features.
// To do: Set EAIO_BACKWARDS_COMPATIBILITY to default to zero (disabled) 
// by about Spring of 2009.
//
#ifndef EAIO_BACKWARDS_COMPATIBILITY
    #define EAIO_BACKWARDS_COMPATIBILITY 1
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EAIO_DLL is 1, else EAIO_DLL is 0.
// EA_DLL is a define that controls DLL builds within the EAConfig build system. 
// EAIO_DLL controls whether EAIO is built and used as a DLL. 
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#ifndef EAIO_DLL
    #if defined(EA_DLL)
        #define EAIO_DLL 1
    #else
        #define EAIO_DLL 0
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EAIO_THREAD_SAFETY_ENABLED
//
// Defined as 0 or 1.
// If defined as 1 then thread safety is enabled where appropriate. See the documentation
// for individual classes and functions or information about how thread safety impacts them.
//
#ifndef EAIO_THREAD_SAFETY_ENABLED

    #define EAIO_THREAD_SAFETY_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EAIO_CPP_STREAM_ENABLED
//
// Defined as 0 or 1.
// Some platforms don't support the C++ standard library.
//
#ifndef EAIO_CPP_STREAM_ENABLED
        #define EAIO_CPP_STREAM_ENABLED 1

#endif



///////////////////////////////////////////////////////////////////////////////
// FCN_POLL_ENABLED
//
// Defined as 0 or 1, with the default being 1 only for platforms that have 
// no other option (i.e. typically console platforms).
// Enables file change notification via manual polling instead of via an 
// automatic (possibly threaded) background operation. Polling is the only 
// option on some platforms, and for these platforms it doesn't matter what
// value FCN_POLL_ENABLED is set to.
//
#ifndef FCN_POLL_ENABLED
        #define FCN_POLL_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EAIO_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EAIO as a DLL and EAIO's
// non-templated functions will be exported. EAIO template functions are not
// labelled as EAIO_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of EAIO_API:
//    EAIO_API int someVariable = 10;  // Export someVariable in a DLL build.
//
//    struct EAIO_API SomeClass{       // Export SomeClass and its member functions in a DLL build.
//    };
//
//    EAIO_API void SomeFunction();    // Export SomeFunction in a DLL build.
//
//
#ifndef EAIO_API // If the build file hasn't already defined this to be dllexport...
    #if EAIO_DLL && defined(_MSC_VER)
        #define EAIO_API           __declspec(dllimport)
        #define EAIO_TEMPLATE_API  // Not sure if there is anything we can do here.
    #else
        #define EAIO_API
        #define EAIO_TEMPLATE_API
    #endif
#endif



#endif // Header include guard

















