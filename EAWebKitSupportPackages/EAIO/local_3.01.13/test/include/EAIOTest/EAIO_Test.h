/////////////////////////////////////////////////////////////////////////////
// EAIO_Test.h
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EABase/eabase.h>
#include <EAIO/EAFileBase.h>
#include <coreallocator/icoreallocator_interface.h>
#include <stdlib.h>


int TestFileBase();
int TestPathString();
int TestFnMatch();
int TestEAStreamCpp();
int TestStream();
int TestTextEncoding();
int TestFile();
int TestFileNotification();
int TestIniFile();
int TestFilePath();
int TestAsync();


///////////////////////////////////////////////////////////////////////////////
// EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
//
// Defined as 0 or 1. 
// Specifies whether application data "bundles" are supported by the given 
// platform. Some platforms want you to store data in a custom container 
// system they provide as opposed to being stored in the conventional 
// file system, though often you can use the conventional file system as well.
//
#if defined(EA_PLATFORM_ANDROID)
	#define EAIOTEST_APPLICATION_BUNDLES_SUPPORTED 1
#else
	#define EAIOTEST_APPLICATION_BUNDLES_SUPPORTED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// gRootDirectory
//
// We define a per-platform root directory to work from. Due to the fact that we are 
// programming gaming consoles, they have file system conventions that are different
// from those like Unix and Windows.
//
extern char  gRootDirectory8 [EA::IO::kMaxPathLength];
extern wchar_t  gRootDirectoryW[EA::IO::kMaxPathLength];


///////////////////////////////////////////////////////////////////////////////
// gbVerboseOutput
//
// Defines if the unit tests that run should print extra information that 
// isn't directly related to the failing of a test. Normally the only text
// unit tests write is failure explanation text.
//
extern bool gbVerboseOutput;


///////////////////////////////////////////////////////////////////////////////
// CoreAllocatorMalloc
//
class CoreAllocatorMalloc : public EA::Allocator::ICoreAllocator 
{
public:
	CoreAllocatorMalloc();
	CoreAllocatorMalloc(const CoreAllocatorMalloc&);
	~CoreAllocatorMalloc();
	CoreAllocatorMalloc& operator=(const CoreAllocatorMalloc&);

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/)
	{
		mAllocationCount++;
		void* pMemory = malloc(size);
		return pMemory;
	}

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/, 
				unsigned /*align*/, unsigned /*alignOffset*/)
	{
		mAllocationCount++;
		void* pMemory = malloc(size);
		return pMemory;
	}

	void Free(void* block, size_t /*size*/)
	{
		mFreeCount++;
		free(block);
	}

public:
	int32_t mAllocationCount;
	int32_t mFreeCount;
};



///////////////////////////////////////////////////////////////////////////////
// EA_WCHAR / EA_CHAR16 / EA_CHAR32
//
// We define these in the case that EABase doesn't define them so that our 
// tests can exercize them. For example, on Windows EABase doesn't define 
// EA_CHAR32, as it's impossible to do so in an efficient way and would be
// bad for user code. But for our unit tests we don't care about this and 
// it's useful to have it.
///////////////////////////////////////////////////////////////////////////////

// Always defind as L""
// The purpose of this in our unit tests is to make it explicit in 
// the test code that we really mean L""/L'' and not EA_CHAR16 or EA_CHAR32.
#if !defined(EA_WCHAR)
	#define EA_WCHAR(s) L ## s
#endif


#if !defined(EA_CHAR16)

	#include <stdlib.h>
	#include <string.h>
	#include <EASTL/map.h>
	#include <EAStdC/EAString.h>

	// This class has no purpose other than to convert 32 bit wchar_t strings to 16 bit char16_t strings.
	// The code below probably wouldn't compile or make sense unless wchar_t was 32 bits or more.
	class ea_char16_str
	{
	public:
		ea_char16_str() : mpData(NULL), mLength(0) {}
		
		ea_char16_str(const wchar_t* pText) 
		{
			mLength = EA::StdC::Strlen(pText);
			mpData  = (char16_t*)malloc((mLength + 1) * sizeof(char16_t));
			for(size_t i = 0; i < (mLength + 1); ++i)
				mpData[i] = (char16_t)pText[i];
		}
		
		ea_char16_str(const ea_char16_str& str)
		{
			CopyFrom(str);
		}

		ea_char16_str& operator=(const ea_char16_str& str)
		{
			if(&str != this)
			{
				if(mpData)
					free(mpData); 
				CopyFrom(str);
			}
			return *this;
		}

		void CopyFrom(const ea_char16_str& str)
		{
			mLength = str.mLength;
			mpData  = (char16_t*)malloc((mLength + 1) * sizeof(char16_t));
			for(size_t i = 0; i < (mLength + 1); ++i)
				mpData[i] = str.mpData[i];
		}

		~ea_char16_str()
		{
			if(mpData)
				free(mpData); 
		}

		char16_t* mpData;
		size_t    mLength;
	};

	inline char16_t* char16_convert(const wchar_t* pText)
	{
		typedef eastl::map<const wchar_t*, ea_char16_str> WChar16Map; // Map of wchar_t* to char16_t* 

		static WChar16Map sTable; // Not thread safe.
		ea_char16_str s = ea_char16_str(pText);
		eastl::pair<WChar16Map::iterator, bool> result = sTable.insert(eastl::make_pair(pText, s));
		return result.first->second.mpData;
	}

	inline char16_t char16_convert(wchar_t c)
	{
		return (char16_t)c;
	}

	#define EA_CHAR16(s) char16_convert(L ## s)

#endif


#if !defined(EA_CHAR32)

	#include <stdlib.h>
	#include <string.h>
	#include <EASTL/map.h>
	#include <EAStdC/EAString.h>

	// This class has no purpose other than to convert 16 bit wchar_t strings to 32 bit char32_t strings.
	// The code below probably wouldn't compile or make sense unless wchar_t was 16 bits or less.
	class ea_char32_str
	{
	public:
		ea_char32_str() : mpData(NULL), mLength(0) {}
		
		ea_char32_str(const wchar_t* pText) 
		{
			mLength = EA::StdC::Strlen(pText);
			mpData  = (char32_t*)malloc((mLength + 1) * sizeof(char32_t));
			if(mpData)
			{
				for(size_t i = 0; i < (mLength + 1); ++i)
					mpData[i] = (char32_t)pText[i];
			}
		}
		
		ea_char32_str(const ea_char32_str& str)
		{
			CopyFrom(str);
		}

		ea_char32_str& operator=(const ea_char32_str& str)
		{
			if(&str != this)
			{
				if(mpData)
					free(mpData); 
				CopyFrom(str);
			}
			return *this;
		}

		void CopyFrom(const ea_char32_str& str)
		{
			mLength = str.mLength;
			mpData  = (char32_t*)malloc((mLength + 1) * sizeof(char32_t));
			if(mpData)
			{
				for(size_t i = 0; i < (mLength + 1); ++i)
					mpData[i] = str.mpData[i];
			}
		}

		~ea_char32_str()
		{
			if(mpData)
				free(mpData); 
		}

		char32_t* mpData;
		size_t    mLength;
	};

	inline char32_t* char32_convert(const wchar_t* pText)
	{
		typedef eastl::map<const wchar_t*, ea_char32_str> WChar32Map; // Map of wchar_t* to char32_t* 

		static WChar32Map sTable; // Not thread-safe, but our tests that use this aren't currently multithreaded and so it doesn't matter.
		ea_char32_str s = ea_char32_str(pText);
		eastl::pair<WChar32Map::iterator, bool> result = sTable.insert(eastl::make_pair(pText, s));
		return result.first->second.mpData;
	}

	inline char32_t char32_convert(wchar_t character)
	{
		return (char32_t)character;
	}

	#define EA_CHAR32(s) char32_convert(L ## s)

#endif


