///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>

/**
 * NOTE(mwinkler): The folder layout is as follows:
 *                 compiler: Holds all compiler specific builtins
 *                 arch: Holds all processor specific code and processor specific manual stack walking
 *                 platform: Implements the public API using platform specific code or implementations from arch or other platforms if needed
 */

/**
 * Include Complier Specific Implementations that are agnostic to the platform that we are compiling for.
 * i.e. clang on linux/freebsd/windows all have the same clang implementations
 */
#if defined(EA_COMPILER_MSVC)

	#include "compiler/EACallstack_msvc.cpp"

#elif defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

	#include "compiler/EACallstack_gcc_clang.cpp"

#else

	#error "EACallstack: Compiler Not Supported!"

#endif

/**
 * Include Architecture/Processor Specific Implementation that is agnostic to the platform that we are compiling for.
 */
#if defined(EA_PROCESSOR_X86)

	#include "arch/x86/EACallstack_x86.cpp"

#elif defined(EA_PROCESSOR_X86_64)

	#include "arch/x64/EACallstack_x64.cpp"

#elif defined(EA_PROCESSOR_ARM)

	#include "arch/arm/EACallstack_arm.cpp"

#else

	#error "EACallstack: Processor Not Supported!"

#endif

/**
 *  Include the Platform Specific Code that implements Callstacks for that given Platform
 */
#if defined(EA_PLATFORM_WIN32) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

	#include "platform/Win32/EACallstack_Win32.cpp"

#elif (defined(EA_PLATFORM_WIN64) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) || defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)

	#include "platform/Win64/EACallstack_Win64.cpp"

#elif defined(EA_PLATFORM_SONY)

	#include "platform/Sony/EACallstack_Sony.cpp"

#elif defined(EA_PLATFORM_APPLE)

	#include "platform/Apple/EACallstack_Apple.cpp"

#elif defined(EA_PLATFORM_ANDROID)

	#include "platform/unix/EACallstack_unix.cpp"

#elif defined(EA_PLATFORM_NX)

	#include "platform/nx/EACallstack_nx.cpp"

#elif defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_FREEBSD)

	#include "platform/unix/EACallstack_unix.cpp"

#else

	#include "platform/null/EACallstack_null.cpp"

#endif
