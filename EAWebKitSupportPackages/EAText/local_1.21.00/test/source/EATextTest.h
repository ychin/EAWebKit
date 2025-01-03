/////////////////////////////////////////////////////////////////////////////
// EATextTest.h
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis.
/////////////////////////////////////////////////////////////////////////////


#ifndef EATEXTTEST_H
#define EATEXTTEST_H


#include <EABase/eabase.h>
#include <EASTL/algorithm.h>
#include <EASTL/string.h>
#include <EATest/EATest.h>
#include <EAText/EAText.h>
#include <EAMain/EAMain.h>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4530) // C++ exception handler used, but unwind semantics are not enabled.
#endif

#include <stdio.h>
#include <stdarg.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif



/// EAText_TestLevel
///
/// Defines how extensive our testing is. A low level is for a desktop or 
/// nightly build in which the test can run quickly but still hit the 
/// majority of functionality. High level is for heavy testing and internal
/// validation which may take numerous hours to run.
///
enum EAText_TestLevel
{
	kEAText_TestLevelLow  = 1,   /// ~10 seconds for test completion.
	kEAText_TestLevelHigh = 10   /// Numerous hours for test completion.
};

extern int gEAText_TestLevel;


/// gEATextTestTrace
///
/// If true, then we trace output that is unrelated to success / failure.
///
extern bool gEATextTestTrace;


/// gRandom
///
extern EA::UnitTest::Rand gRandom;


/// EATextTest_CheckMemory
///
/// Does a global memory heap validation check. Returns 0 if OK and 
/// an error count if there is a problem. 
///
/// Example usage:
///    EATextTest_CheckMemory();
///
int EATextTest_CheckMemory_Imp(const char* pFile, int nLine);
#define EATextTest_CheckMemory() EATextTest_CheckMemory_Imp(__FILE__, __LINE__)



/// GetAppCommandLine
///
/// Returns the CommandLine for the application.
///
extern EA::EAMain::CommandLine* gpAppCommandLine;

const EA::EAMain::CommandLine& GetAppCommandLine();



/// GetApplicationFontDirectory
///
/// For systems that support a standardized central font repository, this function
/// yields the location of that repository. 
/// The pFontDirectory parameter should hold enough space to store a standard path
/// for the given operating sytem. If there isn't enough space to store the directory,
/// or if pFontDirectory is NULL, no characters are copied to pFontDirectory.
/// The returned directory string will be terminated by a platform-specific path
/// separator.
/// The return value is the Strlen of system font directory, regarless of the 
/// value of the nFontDirectoryCapacity argument. If the system font directory 
/// is unknown, the return value is zero and pFontDirectory is set to an empty string.
/// If pFontDirectory is non-NULL, nFontDirectoryCapacity must be at least 1.
///
uint32_t GetApplicationFontDirectory(EA::Text::FilePathChar* pFontDirectory, uint32_t nFontDirectoryCapacity);

/// GetTestDataDirectory
///
/// Returns the location of test data deployed / available to the device during testing.
/// For PC this will return the location of the deployed file path (usually directory of the exe)
/// whereas on mobile devices this will be the location of the assets bundled with the binary or deployed
/// on to the device directly.
uint32_t GetTestDataDirectory(EA::Text::FilePathChar* pTestDataDirectory, uint32_t nTestDataDirectoryDirectoryCapacity);


/// EAArrayCount
///
/// This is defined by recent versions of EABase, but for the time being we 
/// provide backward compatibility here.
///
#ifndef EAArrayCount
	#define EAArrayCount(x) (sizeof(x) / sizeof(x[0]))
#endif


/// TestString
///
/// An eastl string of EA::Text::Char.
///
typedef eastl::basic_string<EA::Text::Char> TestString;


/// GenerateRandomString
///
void GenerateRandomString(eastl::string32& s32, int nMinSize, int nMaxSize, int nSpaceFrequency = 0);
void GenerateRandomString(eastl::string16& s16, int nMinSize, int nMaxSize, int nSpaceFrequency = 0);
void GenerateRandomString(eastl::string8&  s8,  int nMinSize, int nMaxSize, int nSpaceFrequency = 0);


/// RandomizeTextStyle
///
namespace EA{ namespace Text{ struct TextStyle; } }
void RandomizeTextStyle(EA::Text::TextStyle& textStyle);


#endif // Header include guard


