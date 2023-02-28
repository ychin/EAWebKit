/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_INTERNAL_CONFIG_H
#define EACALLSTACK_INTERNAL_CONFIG_H


#include <EABase/eabase.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_VERSION
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
//     printf("EACALLSTACK version: %s", EACALLSTACK_VERSION);
//     printf("EACALLSTACK version: %d.%d.%d", EACALLSTACK_VERSION_N / 10000 % 100, EACALLSTACK_VERSION_N / 100 % 100, EACALLSTACK_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_VERSION

	#define EACALLSTACK_VERSION   "2.03.02"
	#define EACALLSTACK_VERSION_N  20302

#endif // EACALLSTACK_VERSION


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_SCEDBG_ENABLED
//
// Defined as 0 or 1, with 1 being the default for debug builds.
// This controls whether sceDbg library usage is enabled on Sony platforms. This library
// allows for runtime debug functionality, but shipping applications are not
// allowed to use sceDbg.
//
#if !defined(EACALLSTACK_SCEDBG_ENABLED)

	#if defined(EA_SCEDBG_ENABLED) && EA_SCEDBG_ENABLED

		#define EACALLSTACK_SCEDBG_ENABLED EA_SCEDBG_ENABLED

	#elif defined(EA_DEBUG)

		#define EACALLSTACK_SCEDBG_ENABLED 1

	#else

		#define EACALLSTACK_SCEDBG_ENABLED 0

	#endif

#endif // EACALLSTACK_SCEDBG_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE
//
// Defined as 0 or 1. The value depends on the compilation environment.
// Indicates if we can use system-provided abi::__cxa_demangle() at runtime.
//
#if !defined(EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE)

	#if (defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_APPLE)) && defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE 1

	#else

		#define EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE 0

	#endif

#endif // EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_LLVM_DEMANGLE_AVAILABLE
//
// Defined as 0 or 1. The value depends on the compilation environment.
// Indicates if we can use our local abi::__cxa_demangle() implementation at runtime.
// Our local implementation is MIT licensed open source, but projects still need to
// get formal approval for using it.
//
#if !defined(EACALLSTACK_LLVM_DEMANGLE_AVAILABLE)

	#if EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE // If the system already provides it, no need to do so ourselves.

		#define EACALLSTACK_LLVM_DEMANGLE_AVAILABLE 0

	#elif (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_LLVM_DEMANGLE_AVAILABLE 1

	#else

		#define EACALLSTACK_LLVM_DEMANGLE_AVAILABLE 0

	#endif

#endif // EACALLSTACK_LLVM_DEMANGLE_AVAILABLE


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_MODULE_INFO_SUPPORTED
//
// Defined as 0 or 1.
// Indicates if the given platform reliably supports the GetModule* functions.
//
#if !defined(EACALLSTACK_MODULE_INFO_SUPPORTED)

	#if defined(EA_PLATFORM_MICROSOFT)

		#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

	#elif defined(EA_PLATFORM_APPLE)

		#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

	#elif defined(EA_PLATFORM_SONY)

		#if EACALLSTACK_SCEDBG_ENABLED

			#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

		#else

			#define EACALLSTACK_MODULE_INFO_SUPPORTED 0

		#endif

	#elif defined(EA_PLATFORM_ANDROID)

		#if __ANDROID_API__ >= 21

			#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

		#elif __ANDROID_API__ < 21

			#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

		#else

			#define EACALLSTACK_MODULE_INFO_SUPPORTED 0

		#endif

	#elif defined(EA_PLATFORM_LINUX)

		#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

	#elif defined(EA_PLATFORM_NX)

		#define EACALLSTACK_MODULE_INFO_SUPPORTED 1

	#else

		#define EACALLSTACK_MODULE_INFO_SUPPORTED 0

	#endif

#endif // EACALLSTACK_MODULE_INFO_SUPPORTED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_MODULE_MAX_CAPACITY
//
// Defined to be the Maximum number of modules supported for the GetModule*()
// functions.
//
#if !defined(EACALLSTACK_MODULE_MAX_CAPACITY)

	#if defined(EA_PLATFORM_WINDOWS)

		#define EACALLSTACK_MODULE_MAX_CAPACITY 1024

	#elif defined(EA_PLATFORM_SONY)

		#define EACALLSTACK_MODULE_MAX_CAPACITY 96

	#else

		#define EACALLSTACK_MODULE_MAX_CAPACITY 128

	#endif

#endif // EACALLSTACK_MODULE_MAX_CAPACITY


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_MS_PDB_READER_ENABLED
//
// Defined as 0 or 1, with 1 being the default under Windows.
// If enabled, causes the Microsoft-supplied PDB reader to be used whenever
// possible (on platforms where it is available). Usually you will want to
// have this enabled, but sometimes you might want to disable it, such as
// when you want to do cross-platform code debugging.
//
#if !defined(EACALLSTACK_MS_PDB_READER_ENABLED)

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		#define EACALLSTACK_MS_PDB_READER_ENABLED 1

	#else

		#define EACALLSTACK_MS_PDB_READER_ENABLED 0

	#endif

#endif // EACALLSTACK_MS_PDB_READER_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_MSVC_DEMANGLE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the VC++ symbol name demangling code is enabled.
//
#if !defined(EACALLSTACK_MSVC_DEMANGLE_ENABLED)

	#if defined(EA_PLATFORM_MICROSOFT)

		#define EACALLSTACK_MSVC_DEMANGLE_ENABLED 1

	#else

		#define EACALLSTACK_MSVC_DEMANGLE_ENABLED 0

	#endif

#endif // EACALLSTACK_MSVC_DEMANGLE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_GCC_DEMANGLE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the GCC symbol name demangling code is enabled. Note that GCC
// mangling is used by the SN compiler and EDG-based compilers.
//
#if !defined(EACALLSTACK_GCC_DEMANGLE_ENABLED)

	#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_UNIX) || defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

		#define EACALLSTACK_GCC_DEMANGLE_ENABLED 1

	#else

		#define EACALLSTACK_GCC_DEMANGLE_ENABLED 0

	#endif

#endif // EACALLSTACK_GCC_DEMANGLE_ENABLED



///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_MSVC_MAPFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the VC++ .map file address/symbol lookup is enabled.
//
#if  !defined(EACALLSTACK_MSVC_MAPFILE_ENABLED)

	#if defined(EA_COMPILER_MSVC) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_MSVC_MAPFILE_ENABLED 1

	#else

		#define EACALLSTACK_MSVC_MAPFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_MSVC_MAPFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_GCC_MAPFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the GCC .map file address/symbol lookup is enabled.
// Note that this does not refer to Apple's GCC (which is entirely different),
// which is handled by EACALLSTACK_APPLE_MAPFILE_ENABLED.
//
#if !defined(EACALLSTACK_GCC_MAPFILE_ENABLED)

	#if defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_GCC_MAPFILE_ENABLED 1

	#else

		#define EACALLSTACK_GCC_MAPFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_GCC_MAPFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_APPLE_MAPFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the Apple GCC .map file address/symbol lookup is enabled.
//
#if !defined(EACALLSTACK_APPLE_MAPFILE_ENABLED)

	#if defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_APPLE_MAPFILE_ENABLED 1

	#else

		#define EACALLSTACK_APPLE_MAPFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_APPLE_MAPFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_SN_MAPFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the SN .map file address/symbol lookup is enabled.
// This format also covers the clang/llvm combination under Sony platforms.
//
#if !defined(EACALLSTACK_SN_MAPFILE_ENABLED)

	#if (defined(EA_COMPILER_CLANG) && defined(EA_PLATFORM_SONY)) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_SN_MAPFILE_ENABLED 1

	#else

		#define EACALLSTACK_SN_MAPFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_SN_MAPFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_PDBFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the VC++ .pdb file address/symbol lookup is enabled.
//
#if !defined(EACALLSTACK_PDBFILE_ENABLED)

	#if defined(EA_COMPILER_MSVC) || defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_PDBFILE_ENABLED 1

	#else

		#define EACALLSTACK_PDBFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_PDBFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DWARFFILE_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the VC++ .pdb file address/symbol lookup is enabled.
//
#if !defined(EACALLSTACK_DWARFFILE_ENABLED)

	#if defined(EA_PLATFORM_WINDOWS) || defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

		#define EACALLSTACK_DWARFFILE_ENABLED 1

	#else

		#define EACALLSTACK_DWARFFILE_ENABLED 0

	#endif

#endif // EACALLSTACK_DWARFFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DASM_ARM_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the ARM disassembler code is enabled.
//
#if !defined(EACALLSTACK_DASM_ARM_ENABLED)

	#if defined(EA_PLATFORM_DESKTOP) || defined(EA_PROCESSOR_ARM)

		#define EACALLSTACK_DASM_ARM_ENABLED 1

	#else

		#define EACALLSTACK_DASM_ARM_ENABLED 0

	#endif

#endif // EACALLSTACK_DASM_ARM_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DASM_POWERPC_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the PowerPC disassembler code is enabled.
//
#if !defined(EACALLSTACK_DASM_POWERPC_ENABLED)

	#if defined(EA_PLATFORM_DESKTOP) || defined(EA_PROCESSOR_POWERPC)

		#define EACALLSTACK_DASM_POWERPC_ENABLED 1

	#else

		#define EACALLSTACK_DASM_POWERPC_ENABLED 0

	#endif

#endif // EACALLSTACK_DASM_POWERPC_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DASM_X86_ENABLED
//
// Defined as 0 or 1, with 1 being the default on platforms that benefit from it.
// If enabled, the x86 disassembler code is enabled.
//
#if !defined(EACALLSTACK_DASM_X86_ENABLED)

	#if defined(EA_PLATFORM_DESKTOP) || defined(EA_PROCESSOR_X86)

		#define EACALLSTACK_DASM_X86_ENABLED 1

	#else

		#define EACALLSTACK_DASM_X86_ENABLED 0

	#endif

#endif // EACALLSTACK_DASM_X86_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE
//
// You generally need to be using GCC, GLIBC, and Linux for backtrace to be available.
// see "platform/unix/EACallstack_execinfo.cpp" for a full comment
//
#if !defined(EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE)

	#if (defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_APPLE)) && !defined(EA_PLATFORM_ANDROID)

		#define EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE 1

	#else

		#define EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE 0

	#endif

#endif // EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_GETCALLSTACK_SUPPORTED
//
// Defined as 0 or 1.
// Identifies whether runtime callstack unwinding (i.e. GetCallstack()) is
// supported for the given platform. In some cases it may be that unwinding
// support code is present but it hasn't been tested for reliability and may
// have bugs preventing it from working properly.
//
#if defined(EA_PLATFORM_MICROSOFT)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#elif defined(EA_PLATFORM_APPLE)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#elif defined(EA_PLATFORM_SONY)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#elif defined(EA_PLATFORM_NX)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#elif defined(EA_PLATFORM_ANDROID)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#elif defined(EA_PLATFORM_LINUX)

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 1

#else

	#define EACALLSTACK_GETCALLSTACK_SUPPORTED 0

#endif // EACALLSTACK_GETCALLSTACK_SUPPORTED
/* Defined below for backwards-compatibility */
#define EA_CALLSTACK_GETCALLSTACK_SUPPORTED EACALLSTACK_GETCALLSTACK_SUPPORTED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED
//
// Defined as 0 or 1.
// Identifies whether the GetCallstackContext(CallstackContext& context, intptr_t threadId)
// function is supported. This is significant because it identifies whether one
// thread can read the callstack of another thread.
//
#if defined(EA_PLATFORM_MICROSOFT)

	#define EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED 1

#elif defined(EA_PLATFORM_APPLE)

	#define EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED 1

#else

	#define EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED 0

#endif // EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED
/* Defined below for backwards-compatibility */
#define EA_CALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED EACALLSTACK_OTHER_THREAD_CONTEXT_SUPPORTED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_OFFLINE
//
// Defined as 0 or 1.
// Online means that EACallstack is being used to read the current process.
// Offline means that EACallstack is being used to read data from another
// process, in a way that supports multiple platforms, as with a tool.
//
#if !defined(EACALLSTACK_OFFLINE)

	#if defined(EA_PLATFORM_DESKTOP)

		#define EACALLSTACK_OFFLINE 1 // Actually it may or may not be offline. For runtime apps it is, for tools it isn't.

	#else

		#define EACALLSTACK_OFFLINE 0 // Unless somebody is writing a user tool to run on a console/phone, this is 0.

	#endif

#endif // EACALLSTACK_OFFLINE



///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DEBUG_DETAIL_ENABLED
//
// Defined as 0 or 1.
// If true then detailed debug info is displayed. Can be enabled in opt builds.
//
#if !defined(EACALLSTACK_DEBUG_DETAIL_ENABLED)

	#define EACALLSTACK_DEBUG_DETAIL_ENABLED 0

#endif // EACALLSTACK_DEBUG_DETAIL_ENABLED



///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_ALLOC_PREFIX
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
//     void* p = pCoreAllocator->Alloc(37, EACALLSTACK_ALLOC_PREFIX, 0);
//
// Example usage:
//     gMessageServer.GetMessageQueue().get_allocator().set_name(EACALLSTACK_ALLOC_PREFIX "MessageSystem/Queue");
//
#if !defined(EACALLSTACK_ALLOC_PREFIX)

	#define EACALLSTACK_ALLOC_PREFIX "EACallstack/"

#endif // EACALLSTACK_ALLOC_PREFIX


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DEFAULT_ALLOCATOR_ENABLED
//
// Defined as 0 or 1. Default is 1 (for backward compatibility).
// Enables the use of ICoreAllocator::GetDefaultAllocator.
// If disabled then the user is required to explicitly specify the
// allocator used by EACALLSTACK with EA::Callstack::SetAllocator() or
// on a class by class basis within EACALLSTACK.
//
#if !defined(EACALLSTACK_DEFAULT_ALLOCATOR_ENABLED)

	#define EACALLSTACK_DEFAULT_ALLOCATOR_ENABLED 1

#endif // EACALLSTACK_DEFAULT_ALLOCATOR_ENABLED


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_ENABLE_ELF_SPAWN
//
// Defined as 0 or 1. Default is 1.
// Enables use of a spawned elf reading utility (addr2line.exe) to deal with
// elf files that somehow can't be read by our native code. On Windows platforms
// only (not supported on others).
// This can be enabled for shipping products only if you get legal clearance
// to distribute addr2line.exe (part of gnu binutils). This is not the same
// issue as the issue of building code with gnu licensed code, as this is
// the distribution of a pre-existing binary, which is much less of a concern.
//
#if !defined(EACALLSTACK_ENABLE_ELF_SPAWN)

	#define EACALLSTACK_ENABLE_ELF_SPAWN 1

#endif // EACALLSTACK_ENABLE_ELF_SPAWN


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EACALLSTACK_DLL is 1, else EACALLSTACK_DLL is 0.
// EA_DLL is a define that controls DLL builds within the EAConfig build system.
// EACALLSTACK_DLL controls whether EACallstack is built and used as a DLL.
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#if !defined(EACALLSTACK_DLL)

	#if defined(EA_DLL)

		#define EACALLSTACK_DLL 1

	#else

		#define EACALLSTACK_DLL 0

	#endif

#endif // EACALLSTACK_DLL


///////////////////////////////////////////////////////////////////////////////
// EACALLSTACK_API / EACALLSTACK_LOCAL
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EACallstack as a DLL and EACallstack's
// non-templated functions will be exported. EACallstack template functions are not
// labelled as EACALLSTACK_API (and are thus not exported in a DLL build). This is
// because it's not possible (or at least unsafe) to implement inline templated
// functions in a DLL.
//
// Example usage of EACALLSTACK_API:
//    EACALLSTACK_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct EACALLSTACK_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        EACALLSTACK_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    EACALLSTACK_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#if !defined(EACALLSTACK_API) // If the build file hasn't already defined this to be dllexport...

	#if EACALLSTACK_DLL

		#if defined(EA_COMPILER_MSVC)
			#define EACALLSTACK_API      __declspec(dllimport)
		#elif defined(__CYGWIN__)
			#define EACALLSTACK_API      __attribute__((dllimport))
		#elif (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4000)) || (defined(EA_COMPILER_CLANG))
			#define EACALLSTACK_API      __attribute__ ((visibility("default")))
		#else
			#define EACALLSTACK_API
		#endif

	#else

		#define EACALLSTACK_API

	#endif // EACALLSTACK_DLL

#endif // EACALLSTACK_API

#if !defined(EACALLSTACK_LOCAL)

	#if EACALLSTACK_DLL

		#if defined(EA_COMPILER_MSVC)
			#define EACALLSTACK_LOCAL
		#elif defined(__CYGWIN__)
			#define EACALLSTACK_LOCAL
		#elif (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4000)) || (defined(EA_COMPILER_CLANG))
			#define EACALLSTACK_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define EACALLSTACK_LOCAL
		#endif

	#else

		#define EACALLSTACK_LOCAL

	#endif // EACALLSTACK_DLL

#endif // EACALLSTACK_LOCAL


#endif // EACALLSTACK_INTERNAL_CONFIG_H
