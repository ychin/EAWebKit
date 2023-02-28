/////////////////////////////////////////////////////////////////////////////
// EATextTest.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Maintained by Paul Pedriana, Maxis.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <eathread/eathread.h>
#include <MemoryMan/MemoryMan.h>
#include <EAText/EATextFontServer.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextUnicode.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAProcess.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/FnEncode.h>
#include <EATest/EATest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <new>
#include <time.h>
#include <wchar.h>
#if defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS();
	#include <crtdbg.h>
	#if defined(EA_PLATFORM_WINDOWS)
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif
		#pragma warning(push, 0)
		#include <Windows.h>
		#pragma warning(pop)
	#elif defined(EA_PLATFORM_XENON)
		#include <xtl.h>
	#endif
	EA_RESTORE_ALL_VC_WARNINGS();
	#pragma warning(disable : 4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif


#if defined (EA_PLATFORM_IPHONE)
	namespace EA{ namespace IO {
		int appleGetHomeDirectory(EA::Text::FilePathChar* pDirectory, uint32_t nMaxPermittedLength);
		int appleGetAppName(EA::Text::FilePathChar* pDirectory, uint32_t nMaxPermittedLength);
	}}
#endif


#include "EATextTest.h"  // Include this last, as it enables compiler warnings.



using namespace EA::Text;


///////////////////////////////////////////////////////////////////////////////
// gEAText_TestLevel
//
int gEAText_TestLevel = 0;


///////////////////////////////////////////////////////////////////////////////
// gRandom
//
EA::UnitTest::Rand gRandom(0); // We'll initialize it for real in main().


// Required by EASTL.
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t*  pFormat, va_list arguments)
{
	return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
	return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

#if (EASTDC_VERSION_N >= 10600)
	int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}
#endif



int EATextTest_CheckMemory_Imp(const char* pFile, int nLine)
{
	int  nErrorCount(0);
	bool bMemoryOK(true);

	#if defined(_DEBUG) && defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT)
		if(!_CrtCheckMemory())
			bMemoryOK = false;
	#endif

	#ifdef EA_DEBUG
		if(!EA::Allocator::gpEAGeneralAllocator->ValidateHeap(EA::Allocator::GeneralAllocator::kHeapValidationLevelDetail))
			bMemoryOK = false;
	#endif

	if(!bMemoryOK)
	{
		nErrorCount++;

		EA::UnitTest::Report("Memory check failure:\n%s: line %d\n\n", pFile, nLine);
	}

	return nErrorCount;
}



/// GetAppCommandLine
///
EA::EAMain::CommandLine* gpAppCommandLine = NULL;

const EA::EAMain::CommandLine& GetAppCommandLine()
{
	return *gpAppCommandLine;
}




uint32_t GetApplicationFontDirectory(FilePathChar* pFontDirectory, uint32_t nFontDirectoryCapacity)
{
	size_t nStrlen = EA::Text::GetSystemFontDirectory(pFontDirectory, nFontDirectoryCapacity);

	if(!nStrlen)
	{
		#if defined(EA_PLATFORM_PS3)
			EA::StdC::Strcpy(pFontDirectory, EATEXT_FILE_PATH_CHAR("/app_home/data/"));
		#elif defined(EA_PLATFORM_XENON)
			EA::StdC::Strcpy(pFontDirectory, EATEXT_FILE_PATH_CHAR("D:\\data\\"));
		#elif defined(EA_PLATFORM_IPHONE)
			FilePathChar homeDirectory[EA::IO::kMaxDirectoryLength];
			FilePathChar appName[EA::IO::kMaxDirectoryLength];
			EA::IO::appleGetAppName(appName, EA::IO::kMaxDirectoryLength);
			EA::StdC::GetCurrentProcessDirectory(homeDirectory);
		#elif defined(EA_PLATFORM_ANDROID)
			EA::StdC::Strcpy(pFontDirectory, EATEXT_FILE_PATH_CHAR("appbundle:/data/"));            
		#elif defined(EA_PLATFORM_CAPILANO)
			EA::StdC::Strcpy(pFontDirectory, EATEXT_FILE_PATH_CHAR("G:\\data\\"));           
		#elif !defined(EA_PLATFORM_UNIX)
			// Assume assets have been uploaded to working directory of test
			EA::IO::Directory::GetCurrentWorkingDirectory(pFontDirectory, nFontDirectoryCapacity);
		#else
			// Leave pFontDirectory empty.
			EA_ASSERT(pFontDirectory[0] == 0);
		#endif

		nStrlen = EA::StdC::Strlen(pFontDirectory);
	}

	return (uint32_t)nStrlen;        
}



uint32_t GetTestDataDirectory(EA::Text::FilePathChar* pTestDataDirectory, uint32_t nTestDataDirectoryDirectoryCapacity)
{
		#if defined EA_PLATFORM_IPHONE
			EA::Text::FilePathChar homeDirectory[EA::IO::kMaxDirectoryLength];
			EA::StdC::GetCurrentProcessDirectory(homeDirectory);
			EA::StdC::Strlcpy(pTestDataDirectory, homeDirectory, nTestDataDirectoryDirectoryCapacity);
			EA::StdC::Strlcat(pTestDataDirectory, "data/", nTestDataDirectoryDirectoryCapacity);
		#elif defined EA_PLATFORM_ANDROID
			EA::StdC::Strlcpy(pTestDataDirectory, "appbundle:/data/", nTestDataDirectoryDirectoryCapacity);
		#else
			EA::IO::Directory::GetCurrentWorkingDirectory(pTestDataDirectory, nTestDataDirectoryDirectoryCapacity);
			EA::StdC::Strlcat(pTestDataDirectory, EA::IO::kFilePathSeparatorString8, nTestDataDirectoryDirectoryCapacity);
			EA::StdC::Strlcat(pTestDataDirectory, "data", nTestDataDirectoryDirectoryCapacity);
			EA::StdC::Strlcat(pTestDataDirectory, EA::IO::kFilePathSeparatorString8, nTestDataDirectoryDirectoryCapacity);			
		#endif

		return (uint32_t) EA::StdC::Strlen(pTestDataDirectory);
}



void RandomizeTextStyle(EA::Text::TextStyle& textStyle)
{
	textStyle.mnDecorationFlags = (uint32_t)(int32_t)gRandom.RandRange(1, 8);
		//kDFUnderline   = 1,
		//kDFLineThrough = 2,
		//kDFOverline    = 4,

	textStyle.mDirection = (Direction)gRandom.RandLimit(2);
		//kDirectionLTR     = 0,
		//kDirectionRTL     = 1,

	textStyle.mOrientation = (Orientation)gRandom.RandLimit(2);
		// kOrientationHorizontal,
		// kOrientationVertical,

	textStyle.mHAlignment = (HAlignment)gRandom.RandLimit(4);
		// kHALeft,
		// kHACenter,
		// kHARight,
		// kHAJustify,

	textStyle.mVAlignment = (VAlignment)gRandom.RandLimit(3);
		// kVATop,
		// kVACenter,
		// kVABottom,

	textStyle.mJustify = (Justify)gRandom.RandLimit(6);
		// kJustifyInterWord,
		// kJustifyInterIdeograph,
		// kJustifyDistribute,
		// kJustifyNewspaper,
		// kJustifyInterCluster,
		// kJustifyKashida,

	textStyle.mTextOverflow = (TextOverflow)gRandom.RandLimit(3);
		// kTONone,
		// kTOClip,
		// kTOEllipsis,

	textStyle.mEmphasisStyle = (EmphasisStyle)gRandom.RandLimit(5);
		// kESNone,
		// kESAccent,
		// kESDot,
		// kESCircle,
		// kESDisc,

	textStyle.mEmphasisPosition = (EmphasisPosition)gRandom.RandLimit(2);
		// kEPBefore,
		// kEPAfter,

	textStyle.mDigitSubstitution = (DigitSubstitution)gRandom.RandLimit(5);
		// kDSNone,
		// kDSContext,
		// kDSWestern,
		// kDSNational,
		// kDSTraditional,

	textStyle.mPasswordMode = (PasswordMode)gRandom.RandLimit(2);
		// kPMNone,
		// kPMPassword,

	textStyle.mfStretch = (float)gRandom.RandRange(90, 110) / 100.f;  // Random in range of [0.9, 1.1].

	textStyle.mfLetterSpacing = (float)gRandom.RandRange(0, 2);

	textStyle.mfWordSpacing = (float)gRandom.RandRange(0, 4);

	textStyle.mfLineSpace = (float)gRandom.RandRange(0, 20);
}


static const Char gCharSelection16[] = 
{
	kCharHT,   kCharESC,  kCharSpace, kCharDEL,    kCharNBSP,  kCharCGJ,   kCharNQSP,  kChar3MSP,
	kCharFSP,  kCharPSP,  kCharTHSP,  kCharZWSP,   kCharZWNJ,  kCharZWJ,   kCharWJoin, kCharNADS,
	kCharNODS, kCharPASS, kCharIDSP,  kCharZWNBSP, kCharSpace, kCharSpace, kCharSpace, kCharSpace,
	'!', '\'', '\"', ',', '#', '$', '%', '&', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', 
	'<', '=', '>', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~', 
	0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00BF,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 
	0x010F, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011A, 0x011B, 0x011C, 
	0x03A0, 0x03A1, 0x03A2, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7, 0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 
	0x6830, 0x6831, 0x6832, 0x6833, 0x6834, 0x6835, 0x6836, 0x6837, 0x6838, 0x6839, 0x683A, 0x683B, 0x683C, 0x683D, 
	0xB3A0, 0xB3A1, 0xB3A2, 0xB3A3, 0xB3A4, 0xB3A5, 0xB3A6, 0xB3A7, 0xB3A8, 0xB3A9, 0xB3AA, 0xB3AB, 0xB3AC, 0xB3AD 
};

static const uint8_t gCharSelection8[] = 
{
	kCharHT,   kCharESC,  kCharSpace, kCharDEL,    kCharNBSP,  
	'!', '\'', '\"', ',', '#', '$', '%', '&', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', 
	'<', '=', '>', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~', 
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00BF,
};


void GenerateRandomString(eastl::string32& s32, int nMinSize, int nMaxSize, int nSpaceFrequency)
{
	const eastl_size_t nSize = (eastl_size_t)(size_t)gRandom.RandRange(nMinSize, nMaxSize + 1);

	s32.resize(nSize);

	for(eastl_size_t i = 0; i < nSize; i++)
	{
		size_t   index = (size_t)gRandom.RandRange(0, sizeof(gCharSelection16)/sizeof(gCharSelection16[0]));
		char32_t c     = (char32_t)gCharSelection16[index];

		if(nSpaceFrequency && (gRandom.RandLimit(static_cast<uint32_t>(nSpaceFrequency)) == 0))
			c = ' ';

		s32[i] = c;
	}
}


void GenerateRandomString(eastl::string16& s16, int nMinSize, int nMaxSize, int nSpaceFrequency)
{
	const eastl_size_t nSize = (eastl_size_t)(size_t)gRandom.RandRange(nMinSize, nMaxSize + 1);

	s16.resize(nSize);

	for(eastl_size_t i = 0; i < nSize; i++)
	{
		size_t index = (size_t)gRandom.RandRange(0, sizeof(gCharSelection16)/sizeof(gCharSelection16[0]));
		char16_t c     = (char16_t)gCharSelection16[index];

		if(nSpaceFrequency && (gRandom.RandLimit(static_cast<uint32_t>(nSpaceFrequency)) == 0))
			c = ' ';

		s16[i] = c;
	}
}


void GenerateRandomString(eastl::string8& s8, int nMinSize, int nMaxSize, int nSpaceFrequency)
{
	const eastl_size_t nSize = (eastl_size_t)(size_t)gRandom.RandRange(nMinSize, nMaxSize + 1);

	s8.resize(nSize);

	for(eastl_size_t i = 0; i < nSize; i++)
	{
		size_t index = (size_t)gRandom.RandRange(0, sizeof(gCharSelection8)/sizeof(gCharSelection8[0]));
		char8_t c = (char8_t)gCharSelection8[index];

		if(nSpaceFrequency && (gRandom.RandLimit(static_cast<uint32_t>(nSpaceFrequency)) == 0))
			c = ' ';

		s8[i] = c;
	}
}





