/*
Copyright (C) 2009-2011 Electronic Arts, Inc.  All rights reserved.

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
// config.h
//
// Copyright (c) 2009, Electronic Arts. All Rights Reserved.
// By Paul Pedriana
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
// The version is a 5 digit number: Major:MinorMinor:PatchPatch.
// A value of 11002 refers to version 1.10.02.
//
#ifndef PPMALLOC_VERSION
    #define PPMALLOC_VERSION    "1.16.01"
    #define PPMALLOC_VERSION_N  11601

    // The following identifiers are deprecated and are present for backward compatibility:
    #define PPM_GENERAL_ALLOCATOR_VERSION        PPMALLOC_VERSION_N
    #define PPM_GENERAL_ALLOCATOR_DEBUG_VERSION  PPMALLOC_VERSION_N
    #define PPMALLOC_VERSION_MAJOR              (PPMALLOC_VERSION_N / 100 / 100 % 100)
    #define PPMALLOC_VERSION_MINOR              (PPMALLOC_VERSION_N       / 100 % 100)
    #define PPMALLOC_VERSION_PATCH              (PPMALLOC_VERSION_N             % 100)
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_PLATFORM_MICROSOFT
//
// Defined as 1 or undefined.
// Implements support for the definition of EA_PLATFORM_MICROSOFT for the case
// of using EABase versions prior to the addition of its EA_PLATFORM_MICROSOFT support.
//



///////////////////////////////////////////////////////////////////////////////
// PPM_API
//
#if (defined(EA_DLL) || defined(PPMALLOC_DLL)) && defined(_MSC_VER)
    // If PPM_API is not defined, assume this package is being
    // included by something else. When PPMalloc is built as a DLL, 
    // PPM_API is defined to __declspec(dllexport) by the
    // package build script.
    #if !defined(PPM_API)
        #define PPM_API          __declspec(dllimport)
        #define PPM_TEMPLATE_API
    #endif
#else
    // PPM_API has no meaning except for DLL builds.
    #define PPM_API
    #define PPM_TEMPLATE_API
#endif


#if !defined(PPM_DEBUG) || (PPM_DEBUG < 0)
    #if defined(EA_DEBUG) || defined(_DEBUG)
        #define PPM_DEBUG 1
    #else
        #define PPM_DEBUG 0
    #endif
#endif


#if !defined(PPM_DEBUG_FILL) || (PPM_DEBUG_FILL < 0)
    #if (PPM_DEBUG >= 2)
        #define PPM_DEBUG_FILL PPM_DEBUG
    #else
        #define PPM_DEBUG_FILL 0
    #endif
#endif


#if !defined(PPM_CT_ASSERT) && (defined(EA_DEBUG) || defined(_DEBUG))
    #ifdef EA_COMPILER_GNUC
        #define PPM_CT_ASSERT(expression) { extern int dummy__[bool(expression) ? 1 : -1] __attribute__ ((unused));}
    #else
        #define PPM_CT_ASSERT(expression) { extern int dummy__[bool(expression) ? 1 : -1]; }
    #endif
#else
    #define PPM_CT_ASSERT(expression)
#endif


#ifndef PPM_ASSERT_ENABLED
    #define PPM_ASSERT_ENABLED PPM_DEBUG
#endif

#ifndef PPM_ASSERT
    #if PPM_ASSERT_ENABLED
        #define PPM_ASSERT(pAllocator, expression, id, pDescription, pData, pExtra) \
            (void)((expression) || (pAllocator ? pAllocator->AssertionFailure(#expression, id, pDescription, pData, pExtra) : assert(!#expression), 0))
    #else
        #define PPM_ASSERT(pAllocator, expression, id, pDescription, pData, pExtra)
    #endif
#endif


#ifndef PPM_NULL_POINTER_FREE_ENABLED
    #define PPM_NULL_POINTER_FREE_ENABLED 1
#endif


#ifndef PPM_REALLOC_C99_ENABLED
    #define PPM_REALLOC_C99_ENABLED 1
#endif


#if !defined(PPM_DEBUG_FAILURE) && (defined(EA_DEBUG) || defined(_DEBUG))
    #define PPM_DEBUG_FAILURE(pAllocator, pMessage, id, pDescription, pData, pLocation) \
        { pAllocator->AssertionFailure(pMessage, id, pDescription, pData, pLocation); }
#else
    #define PPM_DEBUG_FAILURE(pAllocator, pMessage, id, pDescription, pData, pLocation)
#endif


#ifndef PPM_VIRTUAL_ENABLED
    #define PPM_VIRTUAL_ENABLED PPM_DEBUG
#endif

#if PPM_VIRTUAL_ENABLED
    #define PPM_VIRTUAL virtual
#else
    #define PPM_VIRTUAL
#endif


#ifndef PPM_THREAD_SAFETY_SUPPORTED

        #define PPM_THREAD_SAFETY_SUPPORTED 1
#endif


#ifndef PPM_THREAD_SAFETY_BY_DEFAULT
    #define PPM_THREAD_SAFETY_BY_DEFAULT PPM_THREAD_SAFETY_SUPPORTED
#endif


#ifndef PPM_INTERRUPT_DISABLING_ENABLED
    
#endif


#ifndef PPM_AUTO_HEAP_VALIDATION_SUPPORTED
    #define PPM_AUTO_HEAP_VALIDATION_SUPPORTED PPM_DEBUG
#endif


#ifndef PPM_HOOKS_SUPPORTED
    #define PPM_HOOKS_SUPPORTED PPM_DEBUG
#endif


#ifndef PPM_INTERNAL_CORE_FREE_ENABLED
    #define PPM_INTERNAL_CORE_FREE_ENABLED 1
#endif


#ifndef PPM_MALLOC_AS_COREALLOC
        #define PPM_MALLOC_AS_COREALLOC 0
#endif


#ifndef PPM_MALLOC_AS_MMAPALLOC
    #define PPM_MALLOC_AS_MMAPALLOC 0
#endif


#ifndef PPM_MMAP_CLEARS
        #define PPM_MMAP_CLEARS 1
#endif


#ifndef PPM_HIGH_SUPPORTED
    #define PPM_HIGH_SUPPORTED PPM_MALLOC_AS_COREALLOC // If malloc is used for core, we assume console-like behaviour.
#endif


#ifndef PPM_TRACE_BUFFER_SIZE
       #define PPM_TRACE_BUFFER_SIZE 4000
#endif


#ifndef PPM_ENABLE_EAALLOCATORINTERFACE
    #define PPM_ENABLE_EAALLOCATORINTERFACE 0
#endif


#ifndef PPM_ENABLE_EANEWDELETE
    #define PPM_ENABLE_EANEWDELETE 0
#endif



#ifndef PPM_FASTBIN_TRIM_ENABLED
    #define PPM_FASTBIN_TRIM_ENABLED 0
#endif



#ifndef PPM_DEBUG_CALLSTACK_AVAILABLE
        #define PPM_DEBUG_CALLSTACK_AVAILABLE 1
#endif


#ifndef PPM_NEW_CORE_SIZE_DEFAULT
        #define PPM_NEW_CORE_SIZE_DEFAULT        (16 * 1024 * 1024) // 16 Mebibytes. Same as 256 * 65536, with 65536 being default Win32 region size.
#endif


#ifndef PPM_CORE_INCREMENT_SIZE_DEFAULT
    #define  PPM_CORE_INCREMENT_SIZE_DEFAULT (PPM_NEW_CORE_SIZE_DEFAULT / 4)
#endif



#ifndef PPM_DEBUG_PRESERVE_PRIOR
    #define PPM_DEBUG_PRESERVE_PRIOR PPM_DEBUG
#endif


#if !defined(PPM_MIN_ALIGN)
    #define PPM_MIN_ALIGN 8
#endif



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


#ifndef PPM_INCREMENTAL_OBJECTS_ENABLED
    #define PPM_INCREMENTAL_OBJECTS_ENABLED 1
#endif


#ifndef PPM_ENABLED
    #define PPM_ENABLED 0
#endif


#ifndef PPM_NLA_DEBUG_DATA_SUPPORTED
    #if PPM_DEBUG
        #define PPM_NLA_DEBUG_DATA_SUPPORTED 1
    #else
        #define PPM_NLA_DEBUG_DATA_SUPPORTED 0
    #endif
#endif

#ifndef PPM_NLA_METRICS_SUPPORTED
    #if PPM_DEBUG
        #define PPM_NLA_METRICS_SUPPORTED 1
    #else
        #define PPM_NLA_METRICS_SUPPORTED 0
    #endif
#endif


#ifndef PPM_NLA_HOOKS_SUPPORTED
    #if PPM_DEBUG
        #define PPM_NLA_HOOKS_SUPPORTED 1
    #else
        #define PPM_NLA_HOOKS_SUPPORTED 0
    #endif
#endif


#ifndef PPM_NLA_HEAP_VALIDATION_SUPPORTED
    #if PPM_DEBUG
        #define PPM_NLA_HEAP_VALIDATION_SUPPORTED 1
    #else
        #define PPM_NLA_HEAP_VALIDATION_SUPPORTED 0
    #endif
#endif


#if !defined(PPM_NLA_MIN_ALIGN)
    #define PPM_NLA_MIN_ALIGN 16
#endif

#endif // Header include guard




