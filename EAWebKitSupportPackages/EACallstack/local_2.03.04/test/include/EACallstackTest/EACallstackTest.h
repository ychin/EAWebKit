/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACKTEST_H
#define EACALLSTACKTEST_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/EACallstackRecorder.h>
#include <EACallstack/EADasm.h>
#include <EACallstack/Allocator.h>
#include <EACallstack/EAAddressRep.h>
#include <EACallstack/MapFile.h>
#include <EACallstack/DWARF3File.h>
#include <EACallstack/PDBFile.h>
#include <EACallstack/EASymbolUtil.h>
#include <EACallstack/version.h>

#include <EACallstackTest/TestCoreAllocator.h>
#include <EACallstackTest/TestStreamLog.h>

#include <eathread/eathread.h>

#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAProcess.h>
#include <EAStdC/EAEndian.h>

#include <EAIO/EAStream.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/Allocator.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/FnEncode.h>

#include <PPMalloc/EAGeneralAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>

#include <EAAssert/eaassert.h>

#include <EASTL/list.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_string.h>

#include <EATest/EATest.h>

#include <coreallocator/icoreallocator_interface.h>
#include <coreallocator/icoreallocatormacros.h>

#include <MemoryMan/CoreAllocatorGeneral.h>
#include <MemoryMan/MemoryMan.h>


EA_DISABLE_ALL_VC_WARNINGS();

#if defined(EA_PLATFORM_MICROSOFT)

#include <Windows.h>

#endif

EA_RESTORE_ALL_VC_WARNINGS();

#if defined EA_PLATFORM_NX

#include <nn/fs/fs_RomForDebug.h>

#endif

#include <gtest/gtest.h>


/// EACALLSTACK_NATIVE_FUNCTION_LOOKUP_AVAILABLE
///
/// Defined as 0 or 1.
/// Currently we support at least function name lookups via .map files.
///
#if !defined(EACALLSTACK_NATIVE_FUNCTION_LOOKUP_AVAILABLE)
	#if defined(EA_PLATFORM_DESKTOP)
		#define EACALLSTACK_NATIVE_FUNCTION_LOOKUP_AVAILABLE 1
	#else
		// Actually we can in fact do function name lookups via .map files, but
		// this running test app needs to be able to open that .map file, which
		// means we need to find a way to copy that .map file to the right place
		// on startup. As of this writing we don't have that in place.
		#define EACALLSTACK_NATIVE_FUNCTION_LOOKUP_AVAILABLE 0
	#endif
#endif


/// EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE
///
/// Defined as 0 or 1.
///
#if !defined(EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE)
	#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_DESKTOP)  // Our XBox 360 reader can't read file/line info yet.
		#define EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE 1
	#else
		// Our ELF/Dwarf reader doesn't always work natively. We have to fix something
		// about it which is a non-trivial task and so we'll have to disable this
		// until we get it done.
		#define EACALLSTACK_NATIVE_FILELINE_LOOKUP_AVAILABLE 0
	#endif
#endif


// ReferenceStack types used by this unit test
typedef eastl::fixed_string<char, 32, true> InfoString;


/* Global variables */
extern void*                        gpAddress;
extern CustomCoreAllocator          gCustomCoreAllocator;
extern eastl::list<eastl::wstring>  gTestFileList;
extern const char*                  gpApplicationPath;
extern EA::UnitTest::TestApplication* gpTestApplication;


#endif // EACALLSTACKTEST_H
