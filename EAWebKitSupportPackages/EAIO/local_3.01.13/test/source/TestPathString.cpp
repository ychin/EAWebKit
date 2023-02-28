/////////////////////////////////////////////////////////////////////////////
// TestPathString.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Talin.
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileUtil.h>
#include <EAStdC/EAString.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <stdio.h>
#include EA_ASSERT_HEADER


/////////////////////////////////////////////////////////////////////////////
// 16 bit tests
/////////////////////////////////////////////////////////////////////////////

static bool TestFindComponentFwd(const char16_t* pPath, const char16_t* pExpectedResult) 
{
	const char16_t* const pResult = EA::IO::Path::FindComponentFwd(pPath);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestFindComponentRvs(const char16_t* pPath, const char16_t* pExpectedResult) 
{
	const char16_t* const pResult = EA::IO::Path::FindComponentRvs(pPath);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetStart(const char16_t* pPath, int index, const char16_t* pExpectedResult) 
{
	EA::IO::Path::PathString path(pPath);
	const char16_t* const pResult = EA::IO::Path::GetPathComponentStart(path.begin(), path.end(), index);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetEnd(const char16_t* pPath, int index, const char16_t* pExpectedResult) 
{
	EA::IO::Path::PathString path(pPath);
	const char16_t* const pResult = EA::IO::Path::GetPathComponentEnd(path.begin(), path.end(), index);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestSimplify(const char16_t* pPath, const char16_t* pExpectedResult) 
{
	EA::IO::Path::PathString path(pPath);
	EA::IO::Path::Simplify(path);

	return path == pExpectedResult;
}


static bool TestAppend(const char16_t* pPath, const char16_t* pPath2, const char16_t* pExpectedResult) 
{
	EA::IO::Path::PathString path(pPath);
	EA::IO::Path::Append(path, pPath2);

	return path == pExpectedResult;
}


static bool TestJoin(const char16_t* pPath, const char16_t* pPath2, const char16_t* pExpectedResult) 
{
	EA::IO::Path::PathString path(pPath);
	EA::IO::Path::Join(path, pPath2);

	return path == pExpectedResult;
}


static bool TestGetFileName(const char16_t* pPath, const char16_t* pExpectedResult)
{
	EA::IO::Path::PathString path(pPath);
	const char16_t* const pResult = EA::IO::Path::GetFileName(path);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetFileExtension(const char16_t* pPath, const char16_t* pExpectedResult)
{
	EA::IO::Path::PathString path(pPath);
	const char16_t* const pResult = EA::IO::Path::GetFileExtension(path);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


/////////////////////////////////////////////////////////////////////////////
// 8 bit tests
/////////////////////////////////////////////////////////////////////////////


static bool TestFindComponentFwd(const char* pPath, const char* pExpectedResult) 
{
	const char* const pResult = EA::IO::Path::FindComponentFwd(pPath);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestFindComponentRvs(const char* pPath, const char* pExpectedResult) 
{
	const char* const pResult = EA::IO::Path::FindComponentRvs(pPath);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetStart(const char* pPath, int index, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	const char* const pResult = EA::IO::Path::GetPathComponentStart(path.begin(), path.end(), index);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetEnd(const char* pPath, int index, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	const char* const pResult = EA::IO::Path::GetPathComponentEnd(path.begin(), path.end(), index);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestSimplify(const char* pPath, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	EA::IO::Path::Simplify(path);

	return path == pExpectedResult;
}


static bool TestAppend(const char* pPath, const char* pPath2, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	EA::IO::Path::Append(path, pPath2);

	return path == pExpectedResult;
}


static bool TestJoin(const char* pPath, const char *pPath2, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	EA::IO::Path::Join(path, pPath2);

	return path == pExpectedResult;
}


static bool TestGetFileName(const char* pPath, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	const char* const pResult = EA::IO::Path::GetFileName(path);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}


static bool TestGetFileExtension(const char* pPath, const char* pExpectedResult)
{
	EA::IO::Path::PathString8 path(pPath);
	const char* const pResult = EA::IO::Path::GetFileExtension(path);

	return EA::StdC::Strcmp(pResult, pExpectedResult) == 0;
}

#if EA_PLATFORM_UNIX || EA_PLATFORM_WINDOWS || EA_PLATFORM_XBOXONE || (defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX)
static bool TestIsFilePathStringValid(const char* pPath, bool bExpectedResult)
{
#if EA_PLATFORM_UNIX || (defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX)
	auto fileSystem = EA::IO::kFileSystemUnix;
#else
	auto fileSystem = EA::IO::kFileSystemDefault;
#endif
	
	const bool bTestResult = EA::IO::IsFilePathStringValid(pPath, fileSystem);
	
	EA::UnitTest::Report("\t%-40s Expected:%d Result:%d\n", pPath, bExpectedResult, bTestResult);

	return (bExpectedResult == bTestResult);
}
#endif

///////////////////////////////////////////////////////////////////////////////
// TestPathString
//
int TestPathString()
{
	int nErrorCount = 0;

	using namespace EA::IO;
	using namespace EA::IO::Path;

	// Allocator tests
	{
		PathString8  path8;
		PathString16 path16;

		// Trigger the overflow allocator.
		path8.resize(5000);
		path16.resize(5000);
	}

	///////////////////////
	// IsFilePathStringValid
	///////////////////////
	{
	#if EA_PLATFORM_UNIX || (defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX)
		EATEST_VERIFY(TestIsFilePathStringValid("usr/local/foo", true));
		EATEST_VERIFY(TestIsFilePathStringValid("usr/local/foo/", true));
		
		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/foo", true));
		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/foo/", true));

		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/foo.txt", true));
		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/foo.txt/", true));
		
		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/com1.txt", false));		// Invalid - Restricted name com1
		EATEST_VERIFY(TestIsFilePathStringValid("/usr/local/<om1.txt", false));		// Invalid - Restricted character <
	#elif EA_PLATFORM_WINDOWS || EA_PLATFORM_XBOXONE
		EATEST_VERIFY(TestIsFilePathStringValid("C:\\abc\\def", true));
		EATEST_VERIFY(TestIsFilePathStringValid("C:\\abc\\def\\", true));

		EATEST_VERIFY(TestIsFilePathStringValid("abc\\def\\ghi", true));
		EATEST_VERIFY(TestIsFilePathStringValid("abc\\def\\ghi\\", true));

		EATEST_VERIFY(TestIsFilePathStringValid("C:\\abc\\com1.txt", false));		// Invalid - Restricted name com1
		EATEST_VERIFY(TestIsFilePathStringValid("C:\\abc\\<om1.txt", false));		// Invalid - Restricted character <

		EATEST_VERIFY(TestIsFilePathStringValid("\\\\server\\volume\\dir\\file.txt", true));
	#endif
	}

	///////////////////////
	// FindComponent
	///////////////////////

	{
		// PathString16::iterator FindComponentFwd(PathString16::iterator first,
		//                                         PathString16::iterator last = kEndAuto16);
		// PathString8::iterator FindComponentFwd(PathString8::iterator first,
		//                                        PathString8::iterator last = kEndAuto8);
		// PathString16::const_iterator FindComponentFwd(PathString16::const_iterator first,
		//                                               PathString16::const_iterator last = kEndAuto16);
		// PathString8::const_iterator FindComponentFwd(PathString8::const_iterator first,
		//                                              PathString8::const_iterator last = kEndAuto8);

		EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16("/usr/local/foo/"), EA_CHAR16("usr/local/foo/")));
		EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16("usr/local/foo/"), EA_CHAR16("local/foo/")));
		EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16(""), EA_CHAR16("")));
		EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16("abc"), EA_CHAR16("")));
		#if defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX
			// Possibly unix-specific stuff.
		#else
			EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16("\\\\abc\\def"), EA_CHAR16("def")));
			EATEST_VERIFY(TestFindComponentFwd(EA_CHAR16("C:\\abc\\def"), EA_CHAR16("abc\\def")));
		#endif

		EATEST_VERIFY(TestFindComponentFwd("/usr/local/foo/", "usr/local/foo/"));
		EATEST_VERIFY(TestFindComponentFwd("usr/local/foo/", "local/foo/"));
		EATEST_VERIFY(TestFindComponentFwd("", ""));
		EATEST_VERIFY(TestFindComponentFwd("abc", ""));
		#if defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX
			// Possibly unix-specific stuff.
		#else
			EATEST_VERIFY(TestFindComponentFwd("\\\\abc\\def", "def"));
			EATEST_VERIFY(TestFindComponentFwd("C:\\abc\\def", "abc\\def"));
		#endif
	}

	{
		// PathString16::iterator FindComponentRvs(PathString16::iterator first,
		//                                         PathString16::iterator last = kEndAuto16);
		// PathString8::iterator FindComponentRvs(PathString8::iterator first,
		//                                        PathString8::iterator last = kEndAuto8);
		// PathString16::const_iterator FindComponentRvs(PathString16::const_iterator first,
		//                                               PathString16::const_iterator last = kEndAuto16);
		// PathString8::const_iterator FindComponentRvs(PathString8::const_iterator first,
		//                                              PathString8::const_iterator last = kEndAuto8);

		EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("/usr/local/foo/"), EA_CHAR16("foo/")));
		EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("/usr/local/foo"), EA_CHAR16("foo")));
		EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16(""), EA_CHAR16("")));
		EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("abc"), EA_CHAR16("abc")));
		#if defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX
			// Possibly unix-specific stuff.
		#else
			EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("\\\\abc\\def"), EA_CHAR16("def")));
			EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("\\\\abc"), EA_CHAR16("\\\\abc")));
			EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("C:\\abc\\def"), EA_CHAR16("def")));
			EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("C:\\abc"), EA_CHAR16("abc")));
			EATEST_VERIFY(TestFindComponentRvs(EA_CHAR16("C:\\"), EA_CHAR16("C:\\")));
		#endif

		EATEST_VERIFY(TestFindComponentRvs("/usr/local/foo/", "foo/"));
		EATEST_VERIFY(TestFindComponentRvs("/usr/local/foo", "foo"));
		EATEST_VERIFY(TestFindComponentRvs("", ""));
		EATEST_VERIFY(TestFindComponentRvs("abc", "abc"));
		#if defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX
			// Possibly unix-specific stuff.
		#else
			EATEST_VERIFY(TestFindComponentRvs("\\\\abc\\def", "def"));
			EATEST_VERIFY(TestFindComponentRvs("\\\\abc", "\\\\abc"));
			EATEST_VERIFY(TestFindComponentRvs("C:\\abc\\def", "def"));
			EATEST_VERIFY(TestFindComponentRvs("C:\\abc", "abc"));
			EATEST_VERIFY(TestFindComponentRvs("C:\\", "C:\\"));
		#endif
	}


	///////////////////////
	// GetPathComponentStart
	///////////////////////

	// PathString16::iterator GetPathComponentStart(PathString16::iterator first,
	//                                              PathString16::iterator last,
	//                                              int32_t nIndex);
	// PathString8::iterator GetPathComponentStart(PathString8::iterator first,
	//                                             PathString8::iterator last,
	//                                             int32_t nIndex);

	{
		EATEST_VERIFY(TestGetStart(EA_CHAR16("/usr/local/foo/"),  0,  EA_CHAR16("/usr/local/foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("/usr/local/foo/"),  1,  EA_CHAR16("usr/local/foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("/usr/local/foo/"),  2,  EA_CHAR16("local/foo/")));

		EATEST_VERIFY(TestGetStart(EA_CHAR16("usr/local/foo/"),   0,  EA_CHAR16("usr/local/foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("usr/local/foo/"),   1,  EA_CHAR16("local/foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("usr/local/foo/"),   2,  EA_CHAR16("foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("usr/local/foo/"),  -1,  EA_CHAR16("foo/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("usr/local/foo/"),  -2,  EA_CHAR16("local/foo/")));

		EATEST_VERIFY(TestGetStart(EA_CHAR16("C:"), 0,         EA_CHAR16("C:")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("C:"), 1,         EA_CHAR16("")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("C:"),-1,         EA_CHAR16("C:")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:"), 0,     EA_CHAR16("drive:")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:"), 1,     EA_CHAR16("")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:"), -1,    EA_CHAR16("drive:")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/"), 0,    EA_CHAR16("drive:/")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/"), 1,    EA_CHAR16("")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/"), -1,   EA_CHAR16("drive:/")));

		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"),  0,   EA_CHAR16("drive:/a/b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"),  1,   EA_CHAR16("a/b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"),  2,   EA_CHAR16("b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"),  3,   EA_CHAR16("c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"), -1,   EA_CHAR16("c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"), -2,   EA_CHAR16("b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"), -3,   EA_CHAR16("a/b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:/a/b/c"), -4,   EA_CHAR16("drive:/a/b/c")));

		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 0,   EA_CHAR16("drive:a/b/c")));
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 1,   EA_CHAR16("b/c")));
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 2,   EA_CHAR16("c")));
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 3,   EA_CHAR16("")));

			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"),-3,   EA_CHAR16("drive:a/b/c")));
		#else
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 1,   EA_CHAR16("a/b/c")));
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 2,   EA_CHAR16("b/c")));
			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"), 3,   EA_CHAR16("c")));

			EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"),-3,   EA_CHAR16("a/b/c")));
		#endif
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"),-1,   EA_CHAR16("c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"),-2,   EA_CHAR16("b/c")));
		EATEST_VERIFY(TestGetStart(EA_CHAR16("drive:a/b/c"),-4,   EA_CHAR16("drive:a/b/c")));
	}

	{
		EATEST_VERIFY(TestGetStart("/usr/local/foo/",  0,  "/usr/local/foo/"));
		EATEST_VERIFY(TestGetStart("/usr/local/foo/",  1,  "usr/local/foo/"));
		EATEST_VERIFY(TestGetStart("/usr/local/foo/",  2,  "local/foo/"));

		EATEST_VERIFY(TestGetStart("usr/local/foo/",   0,  "usr/local/foo/"));
		EATEST_VERIFY(TestGetStart("usr/local/foo/",   1,  "local/foo/"));
		EATEST_VERIFY(TestGetStart("usr/local/foo/",   2,  "foo/"));
		EATEST_VERIFY(TestGetStart("usr/local/foo/",  -1,  "foo/"));
		EATEST_VERIFY(TestGetStart("usr/local/foo/",  -2,  "local/foo/"));

		EATEST_VERIFY(TestGetStart("C:", 0,         "C:"));
		EATEST_VERIFY(TestGetStart("C:", 1,         ""));
		EATEST_VERIFY(TestGetStart("C:",-1,         "C:"));
		EATEST_VERIFY(TestGetStart("drive:", 0,     "drive:"));
		EATEST_VERIFY(TestGetStart("drive:", 1,     ""));
		EATEST_VERIFY(TestGetStart("drive:",-1,     "drive:"));
		EATEST_VERIFY(TestGetStart("drive:/", 0,    "drive:/"));
		EATEST_VERIFY(TestGetStart("drive:/", 1,    ""));
		EATEST_VERIFY(TestGetStart("drive:/",-1,    "drive:/"));

		EATEST_VERIFY(TestGetStart("drive:/a/b/c", 0,   "drive:/a/b/c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c", 1,   "a/b/c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c", 2,   "b/c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c", 3,   "c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c",-1,   "c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c",-2,   "b/c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c",-3,   "a/b/c"));
		EATEST_VERIFY(TestGetStart("drive:/a/b/c",-4,   "drive:/a/b/c"));

		EATEST_VERIFY(TestGetStart("drive:a/b/c", 0,   "drive:a/b/c"));
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 1,   "b/c"));
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 2,   "c"));
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 3,   ""));

			EATEST_VERIFY(TestGetStart("drive:a/b/c",-3,   "drive:a/b/c"));
		#else
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 1,   "a/b/c"));
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 2,   "b/c"));
			EATEST_VERIFY(TestGetStart("drive:a/b/c", 3,   "c"));

			EATEST_VERIFY(TestGetStart("drive:a/b/c",-3,   "a/b/c"));
		#endif
		EATEST_VERIFY(TestGetStart("drive:a/b/c",-1,   "c"));
		EATEST_VERIFY(TestGetStart("drive:a/b/c",-2,   "b/c"));
		EATEST_VERIFY(TestGetStart("drive:a/b/c",-4,   "drive:a/b/c"));
	}

	
	///////////////////////
	// GetPathComponentEnd
	///////////////////////

	// PathString16::iterator GetPathComponentEnd(PathString16::iterator first,
	//                                            PathString16::iterator last,
	//                                            int32_t nIndex);
	// PathString8::iterator GetPathComponentEnd(PathString8::iterator first,
	//                                           PathString8::iterator last,
	//                                           int32_t nIndex);

	{
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("/usr/local/foo/bar/"),  0, EA_CHAR16("/usr/local/foo/bar/")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("/usr/local/foo/bar/"),  1, EA_CHAR16("/local/foo/bar/")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("/usr/local/foo/bar/"),  2, EA_CHAR16("/foo/bar/")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("/usr/local/foo/bar/"), -1, EA_CHAR16("/")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("/usr/local/foo/bar/"), -2, EA_CHAR16("/bar/")));

		EATEST_VERIFY(TestGetEnd(EA_CHAR16("C:"), 0,           EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("C:"), 1,           EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("C:"),-1,           EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:"), 0,       EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:"), 1,       EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:"),-1,       EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:/"), 0,      EA_CHAR16("/")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:/"), 1,      EA_CHAR16("")));
		EATEST_VERIFY(TestGetEnd(EA_CHAR16("drive:/"),-1,      EA_CHAR16("/")));
	}

	{
		EATEST_VERIFY(TestGetEnd("/usr/local/foo/bar/",  0, "/usr/local/foo/bar/"));
		EATEST_VERIFY(TestGetEnd("/usr/local/foo/bar/",  1, "/local/foo/bar/"));
		EATEST_VERIFY(TestGetEnd("/usr/local/foo/bar/",  2, "/foo/bar/"));
		EATEST_VERIFY(TestGetEnd("/usr/local/foo/bar/", -1, "/"));
		EATEST_VERIFY(TestGetEnd("/usr/local/foo/bar/", -2, "/bar/"));

		EATEST_VERIFY(TestGetEnd("C:", 0,           ""));
		EATEST_VERIFY(TestGetEnd("C:", 1,           ""));
		EATEST_VERIFY(TestGetEnd("C:",-1,           ""));
		EATEST_VERIFY(TestGetEnd("drive:", 0,       ""));
		EATEST_VERIFY(TestGetEnd("drive:", 1,       ""));
		EATEST_VERIFY(TestGetEnd("drive:",-1,       ""));
		EATEST_VERIFY(TestGetEnd("drive:/", 0,      "/"));
		EATEST_VERIFY(TestGetEnd("drive:/", 1,      ""));
		EATEST_VERIFY(TestGetEnd("drive:/",-1,      "/"));
	}



	///////////////////////
	// GetDirectoryString
	///////////////////////

	// PathString16 GetDirectoryString(PathString16::const_iterator first,
	//                                 PathString16::const_iterator last = kEndAuto16);
	// PathString8 GetDirectoryString(PathString8::const_iterator first,
	//                                PathString8::const_iterator last = kEndAuto8);



	///////////////////////
	// GetFileName
	///////////////////////

	// PathString16 GetFileNameString(PathString16::const_iterator first,
	//                                PathString16::const_iterator last = kEndAuto16);
	// PathString8 GetFileNameString(PathString8::const_iterator first,
	//                               PathString8::const_iterator last = kEndAuto8);
	// PathString16::const_iterator GetFileName(const PathString16& path);
	// PathString8::const_iterator GetFileName(const PathString8& path);
	// PathString16::iterator GetFileName(PathString16::const_iterator first,
	//                                    PathString16::const_iterator last = kEndAuto16);
	// PathString8::iterator GetFileName(PathString8::const_iterator first,
	//                                   PathString8::const_iterator last = kEndAuto8);

	{
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:"),             EA_CHAR16("C:")));
			EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:"),         EA_CHAR16("drive:")));
			EATEST_VERIFY(TestGetFileName(EA_CHAR16("\\\\name"),       EA_CHAR16("\\\\name")));
		#else
			EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:"),             EA_CHAR16("")));
			EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:"),         EA_CHAR16("")));
			#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
				EATEST_VERIFY(TestGetFileName(EA_CHAR16("\\\\name"),   EA_CHAR16("")));
			#else
				EATEST_VERIFY(TestGetFileName(EA_CHAR16("\\\\name"), EA_CHAR16("\\\\name")));
			#endif
		#endif
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:/name"),        EA_CHAR16("name")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:/name/"),       EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:/one/two"),     EA_CHAR16("two")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("C:/one/two/"),    EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:/name"),    EA_CHAR16("name")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:/name/"),   EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:/one/two"), EA_CHAR16("two")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("drive:/one/two/"),EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("//"),             EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("//name"),         EA_CHAR16("name")));
		EATEST_VERIFY(TestGetFileName(EA_CHAR16("//one/two"),      EA_CHAR16("two")));
	}

	{
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestGetFileName("C:",             "C:"));
			EATEST_VERIFY(TestGetFileName("drive:",         "drive:"));
		#else
			EATEST_VERIFY(TestGetFileName("C:",             ""));
			EATEST_VERIFY(TestGetFileName("drive:",         ""));
			#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
				EATEST_VERIFY(TestGetFileName("\\\\name",   ""));
			#else
				EATEST_VERIFY(TestGetFileName("\\\\name",   "\\\\name"));
			#endif
		#endif
		EATEST_VERIFY(TestGetFileName("C:/name",        "name"));
		EATEST_VERIFY(TestGetFileName("C:/name/",       ""));
		EATEST_VERIFY(TestGetFileName("C:/one/two",     "two"));
		EATEST_VERIFY(TestGetFileName("C:/one/two/",    ""));
		EATEST_VERIFY(TestGetFileName("drive:/name",    "name"));
		EATEST_VERIFY(TestGetFileName("drive:/name/",   ""));
		EATEST_VERIFY(TestGetFileName("drive:/one/two", "two"));
		EATEST_VERIFY(TestGetFileName("drive:/one/two/",""));
		EATEST_VERIFY(TestGetFileName("//",             ""));
		EATEST_VERIFY(TestGetFileName("//name",         "name"));
		EATEST_VERIFY(TestGetFileName("//one/two",      "two"));
	}


	///////////////////////
	// GetFileExtension
	///////////////////////

	// PathString16 GetFileExtensionString(PathString16::const_iterator first,
	//                                     PathString16::const_iterator last = kEndAuto16);
	// PathString8 GetFileExtensionString(PathString8::const_iterator first,
	//                                    PathString8::const_iterator last = kEndAuto8);
	// PathString16::const_iterator GetFileExtension(const PathString16& path);
	// PathString8::const_iterator GetFileExtension(const PathString8& path);
	// PathString16::iterator GetFileExtension(PathString16::const_iterator first,
	//                                         PathString16::const_iterator last = kEndAuto16);
	// PathString8::iterator GetFileExtension(PathString8::const_iterator first,
	//                                        PathString8::const_iterator last = kEndAuto8);

	{
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:"),                EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:.ext"),            EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/name"),           EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/name.ext"),       EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/name/"),          EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/name/.ext"),      EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/one/two"),        EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/one/two.ext"),    EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/one/two/"),       EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("C:/one/two/.ext"),   EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//"),                EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("\\\\.ext"),          EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//.ext"),            EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("\\\\name"),          EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//name"),            EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("\\\\name.ext"),      EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//name.ext"),        EA_CHAR16(".ext")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//one/two"),         EA_CHAR16("")));
		EATEST_VERIFY(TestGetFileExtension(EA_CHAR16("//one/two.ext"),     EA_CHAR16(".ext")));
	}

	{
		EATEST_VERIFY(TestGetFileExtension("C:",                ""));
		EATEST_VERIFY(TestGetFileExtension("C:.ext",            ".ext"));
		EATEST_VERIFY(TestGetFileExtension("C:/name",           ""));
		EATEST_VERIFY(TestGetFileExtension("C:/name.ext",       ".ext"));
		EATEST_VERIFY(TestGetFileExtension("C:/name/",          ""));
		EATEST_VERIFY(TestGetFileExtension("C:/name/.ext",      ".ext"));
		EATEST_VERIFY(TestGetFileExtension("C:/one/two",        ""));
		EATEST_VERIFY(TestGetFileExtension("C:/one/two.ext",    ".ext"));
		EATEST_VERIFY(TestGetFileExtension("C:/one/two/",       ""));
		EATEST_VERIFY(TestGetFileExtension("C:/one/two/.ext",   ".ext"));
		EATEST_VERIFY(TestGetFileExtension("//",                ""));
		EATEST_VERIFY(TestGetFileExtension("\\\\.ext",          ""));
		EATEST_VERIFY(TestGetFileExtension("//.ext",            ".ext"));
		EATEST_VERIFY(TestGetFileExtension("\\\\name",          ""));
		EATEST_VERIFY(TestGetFileExtension("//name",            ""));
		EATEST_VERIFY(TestGetFileExtension("\\\\name.ext",      ""));
		EATEST_VERIFY(TestGetFileExtension("//name.ext",        ".ext"));
		EATEST_VERIFY(TestGetFileExtension("//one/two",         ""));
		EATEST_VERIFY(TestGetFileExtension("//one/two.ext",     ".ext"));
	}


	///////////////////////
	// GetLocalRoot
	///////////////////////

	// PathString16::const_iterator GetLocalRoot(const PathString16& path);
	// PathString8::const_iterator GetLocalRoot(const PathString8& path);
	// PathString16::iterator GetLocalRoot(PathString16::iterator first,
	//                                     PathString16::iterator last = kEndAuto16);
	// PathString8::iterator GetLocalRoot(PathString8::iterator first,
	//                                    PathString8::iterator last = kEndAuto8);



	///////////////////////
	// Simplify
	// Append
	///////////////////////

	// PathString16& Simplify(PathString16& path);
	// PathString8& Simplify(PathString8& path);
	// PathString16& Append(PathString16& dst,
	//                      PathString16::const_iterator suffix_first,
	//                      PathString16::const_iterator suffix_last = kEndAuto16);
	// PathString8& Append(PathString8& dst,
	//                     PathString8::const_iterator suffix_first,
	//                     PathString8::const_iterator suffix_last = kEndAuto8);
	// PathString16& Append(PathString16& dst, const PathString16& suffix);
	// PathString8& Append(PathString8& dst, const PathString8& suffix);

	{
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:"),            EA_CHAR16("C:")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/"),           EA_CHAR16("C:/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/name/"),      EA_CHAR16("C:/name/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/name//"),     EA_CHAR16("C:/name/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/name/./"),    EA_CHAR16("C:/name/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/name/../"),   EA_CHAR16("C:/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("name/../"),      EA_CHAR16("")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("../"),           EA_CHAR16("../")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("\\\\name/../"),  EA_CHAR16("\\\\name/../")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("//name/../"),    EA_CHAR16("/")));
		EATEST_VERIFY(TestSimplify(EA_CHAR16("//one/two/three/../../"), EA_CHAR16("/one/")));
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/../"),    EA_CHAR16("")));
			EATEST_VERIFY(TestAppend(EA_CHAR16("C:"),          EA_CHAR16("name"),        EA_CHAR16("C:/name")));
			EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one"),      EA_CHAR16("name"),        EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("name"),        EA_CHAR16("C:/one/two/name")));
			EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("../name"),     EA_CHAR16("C:/one/two/../name")));
		#else
			EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/../"),    EA_CHAR16("C:/../")));
			#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:"),          EA_CHAR16("name"),        EA_CHAR16("C:\\name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one"),      EA_CHAR16("name"),        EA_CHAR16("C:/one\\name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("name"),        EA_CHAR16("C:/one/two\\name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("../name"),     EA_CHAR16("C:/one/two\\../name")));
			#else
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:"),          EA_CHAR16("name"),        EA_CHAR16("C:/name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one"),      EA_CHAR16("name"),        EA_CHAR16("C:/one/name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("name"),        EA_CHAR16("C:/one/two/name")));
				EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two"),  EA_CHAR16("../name"),     EA_CHAR16("C:/one/two/../name")));
			#endif
		#endif
		EATEST_VERIFY(TestSimplify(EA_CHAR16("\\\\one/two/three/../../"), EA_CHAR16("\\\\one/")));
		EATEST_VERIFY(TestAppend(EA_CHAR16("C:"),          EA_CHAR16("/name"),       EA_CHAR16("/name")));
		EATEST_VERIFY(TestAppend(EA_CHAR16("C:"),          EA_CHAR16(""),            EA_CHAR16("C:")));
		EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one"),      EA_CHAR16("/name"),       EA_CHAR16("/name")));
		EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/"),     EA_CHAR16("name"),        EA_CHAR16("C:/one/name")));
		EATEST_VERIFY(TestAppend(EA_CHAR16("C:/one/two/"), EA_CHAR16("name"),        EA_CHAR16("C:/one/two/name")));
	}

	{
		EATEST_VERIFY(TestSimplify("C:",            "C:"));
		EATEST_VERIFY(TestSimplify("C:/",           "C:/"));
		EATEST_VERIFY(TestSimplify("C:/name/",      "C:/name/"));
		EATEST_VERIFY(TestSimplify("C:/name//",     "C:/name/"));
		EATEST_VERIFY(TestSimplify("C:/name/./",    "C:/name/"));
		EATEST_VERIFY(TestSimplify("C:/name/../",   "C:/"));
		EATEST_VERIFY(TestSimplify("name/../",      ""));
		EATEST_VERIFY(TestSimplify("../",           "../"));
		EATEST_VERIFY(TestSimplify("\\\\name/../",  "\\\\name/../"));
		EATEST_VERIFY(TestSimplify("//name/../",    "/"));
		EATEST_VERIFY(TestSimplify("//one/two/three/../../", "/one/"));
		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestSimplify(EA_CHAR16("C:/../"),    EA_CHAR16("")));
			EATEST_VERIFY(TestAppend("C:",          "name",        "C:/name"));
			EATEST_VERIFY(TestAppend("C:/one",      "name",        "C:/one/name"));
			EATEST_VERIFY(TestAppend("C:/one/two",  "name",        "C:/one/two/name"));
			EATEST_VERIFY(TestAppend("C:/one/two",  "../name",     "C:/one/two/../name"));
		#else
			EATEST_VERIFY(TestSimplify("C:/../",        "C:/../"));
			#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
				EATEST_VERIFY(TestAppend("C:",          "name",    "C:\\name"));
				EATEST_VERIFY(TestAppend("C:/one",      "name",    "C:/one\\name"));
				EATEST_VERIFY(TestAppend("C:/one/two",  "name",    "C:/one/two\\name"));
				EATEST_VERIFY(TestAppend("C:/one/two",  "../name", "C:/one/two\\../name"));
			#else
				EATEST_VERIFY(TestAppend("C:",          "name",    "C:/name"));
				EATEST_VERIFY(TestAppend("C:/one",      "name",    "C:/one/name"));
				EATEST_VERIFY(TestAppend("C:/one/two",  "name",    "C:/one/two/name"));
				EATEST_VERIFY(TestAppend("C:/one/two",  "../name", "C:/one/two/../name"));
			#endif
		#endif
		EATEST_VERIFY(TestSimplify("\\\\one/two/three/../../", "\\\\one/"));
		EATEST_VERIFY(TestAppend("C:",          "/name",       "/name"));
		EATEST_VERIFY(TestAppend("C:",          "",            "C:"));
		EATEST_VERIFY(TestAppend("C:/one",      "/name",       "/name"));
		EATEST_VERIFY(TestAppend("C:/one/",     "name",        "C:/one/name"));
		EATEST_VERIFY(TestAppend("C:/one/two/", "name",        "C:/one/two/name"));
	}


	///////////////////////
	// Join
	///////////////////////

	// PathString16& Join(PathString16& dst,
	//                    PathString16::const_iterator suffix_first,
	//                    PathString16::const_iterator suffix_last = kEndAuto16);
	// PathString8& Join(PathString8& dst,
	//                   PathString8::const_iterator suffix_first,
	//                   PathString8::const_iterator suffix_last = kEndAuto8);
	// PathString16& Join(PathString16& dst, const PathString16& suffix);
	// PathString8& Join(PathString8& dst, const PathString8& suffix);

	{
		EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),          EA_CHAR16(""),            EA_CHAR16("C:")));

		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),          EA_CHAR16("name"),        EA_CHAR16("C:/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),          EA_CHAR16("/name"),       EA_CHAR16("/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),      EA_CHAR16("/name"),       EA_CHAR16("/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),      EA_CHAR16("name"),        EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/"),     EA_CHAR16("name"),        EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),  EA_CHAR16("name"),        EA_CHAR16("C:/one/two/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two/"), EA_CHAR16("name"),        EA_CHAR16("C:/one/two/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),  EA_CHAR16("../name"),     EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name"),   EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name/./../six"),   EA_CHAR16("C:/one/six")));
		#elif defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),          EA_CHAR16("name"),        EA_CHAR16("C:\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),          EA_CHAR16("/name"),       EA_CHAR16("\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),      EA_CHAR16("/name"),       EA_CHAR16("\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),      EA_CHAR16("name"),        EA_CHAR16("C:\\one\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/"),     EA_CHAR16("name"),        EA_CHAR16("C:\\one\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),  EA_CHAR16("name"),        EA_CHAR16("C:\\one\\two\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two/"), EA_CHAR16("name"),        EA_CHAR16("C:\\one\\two\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),  EA_CHAR16("../name"),     EA_CHAR16("C:\\one\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name"),   EA_CHAR16("C:\\one\\name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name/./../six"),   EA_CHAR16("C:\\one\\six")));
		#else
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),            EA_CHAR16("name"),             EA_CHAR16("C:/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:"),            EA_CHAR16("/name"),            EA_CHAR16("/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),        EA_CHAR16("/name"),            EA_CHAR16("/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one"),        EA_CHAR16("name"),             EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/"),       EA_CHAR16("name"),             EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),    EA_CHAR16("name"),             EA_CHAR16("C:/one/two/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two/"),   EA_CHAR16("name"),             EA_CHAR16("C:/one/two/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/two"),    EA_CHAR16("../name"),          EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name"),          EA_CHAR16("C:/one/name")));
			EATEST_VERIFY(TestJoin(EA_CHAR16("C:/one/./two"),  EA_CHAR16("../name/./../six"), EA_CHAR16("C:/one/six")));
		#endif
	}

	{
		EATEST_VERIFY(TestJoin("C:",          "",            "C:"));

		#if !defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) || !EA_FILE_PATH_HAS_DRIVE_SEPARATOR
			// Unix filesystems don't support the concept of drives.
			EATEST_VERIFY(TestJoin("C:",          "name",        "C:/name"));
			EATEST_VERIFY(TestJoin("C:",          "/name",       "/name"));
			EATEST_VERIFY(TestJoin("C:/one",      "/name",       "/name"));
			EATEST_VERIFY(TestJoin("C:/one",      "name",        "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/",     "name",        "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/two",  "name",        "C:/one/two/name"));
			EATEST_VERIFY(TestJoin("C:/one/two/", "name",        "C:/one/two/name"));
			EATEST_VERIFY(TestJoin("C:/one/two",  "../name",     "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/./two",  "../name",   "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/./two",  "../name/./../six",   "C:/one/six"));
		#elif defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
			EATEST_VERIFY(TestJoin("C:",           "name",             "C:\\name"));
			EATEST_VERIFY(TestJoin("C:",           "/name",            "\\name"));
			EATEST_VERIFY(TestJoin("C:/one",       "/name",            "\\name"));
			EATEST_VERIFY(TestJoin("C:/one",       "name",             "C:\\one\\name"));
			EATEST_VERIFY(TestJoin("C:/one/",      "name",             "C:\\one\\name"));
			EATEST_VERIFY(TestJoin("C:/one/two",   "name",             "C:\\one\\two\\name"));
			EATEST_VERIFY(TestJoin("C:/one/two/",  "name",             "C:\\one\\two\\name"));
			EATEST_VERIFY(TestJoin("C:/one/two",   "../name",          "C:\\one\\name"));
			EATEST_VERIFY(TestJoin("C:/one/./two", "../name",          "C:\\one\\name"));
			EATEST_VERIFY(TestJoin("C:/one/./two", "../name/./../six", "C:\\one\\six"));
		#else
			EATEST_VERIFY(TestJoin("C:",            "name",               "C:/name"));
			EATEST_VERIFY(TestJoin("C:",            "/name",              "/name"));
			EATEST_VERIFY(TestJoin("C:/one",        "/name",              "/name"));
			EATEST_VERIFY(TestJoin("C:/one",        "name",               "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/",       "name",               "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/two",    "name",               "C:/one/two/name"));
			EATEST_VERIFY(TestJoin("C:/one/two/",   "name",               "C:/one/two/name"));
			EATEST_VERIFY(TestJoin("C:/one/two",    "../name",            "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/./two",  "../name",            "C:/one/name"));
			EATEST_VERIFY(TestJoin("C:/one/./two",  "../name/./../six",   "C:/one/six"));
		#endif
	}


	/*
	void Split(const PathString16& path, PathString16* pDrive, PathString16* pDirectory, PathString16* pFileName, PathString16* pFileExtension);
	void Split(const PathString8&  path, PathString8*  pDrive, PathString8*  pDirectory, PathString8*  pFileName, PathString8*  pFileExtension);

	void Split(PathString16::const_iterator path, PathString16::const_iterator& pDrive, PathString16::const_iterator& pDirectory, PathString16::const_iterator& pFileName, PathString16::const_iterator& pFileExtension);
	void Split(PathString8::const_iterator  path, PathString8::const_iterator&  pDrive, PathString8::const_iterator&  pDirectory, PathString8::const_iterator&  pFileName, PathString8::const_iterator&  pFileExtension);

	PathString16::const_iterator TruncateComponent(PathString16& path, int nIndex);
	PathString8::const_iterator TruncateComponent(PathString8& path, int nIndex);

	PathString16& Normalize(PathString16& path);
	PathString8& Normalize(PathString8& path);

	PathString16& Simplify(PathString16& path);
	PathString8& Simplify(PathString8& path);
	*/


	///////////////////////
	// Canonicalize
	///////////////////////

	{
		// PathString16& Canonicalize(PathString16& path, char16_t separator = EA_FILE_PATH_SEPARATOR_16);
		// PathString8& Canonicalize(PathString8& path, char separator = EA_FILE_PATH_SEPARATOR_8);
	}


	///////////////////////
	// IsRelative
	///////////////////////

	// bool IsRelative(PathString16::const_iterator first,
	//                 PathString16::const_iterator last = kEndAuto16);
	// bool IsRelative(PathString8::const_iterator first,
	//                 PathString8::const_iterator last = kEndAuto8);
	// bool IsRelative(const PathString16& path);
	// bool IsRelative(const PathString8& path);
	{
#if defined(EA_FILE_PATH_HAS_DRIVE_SEPARATOR) && EA_FILE_PATH_HAS_DRIVE_SEPARATOR
		EATEST_VERIFY(!EA::IO::Path::IsRelative("drive:/a/b/"));
		EATEST_VERIFY(EA::IO::Path::IsRelative("a/b/"));
		EATEST_VERIFY(!EA::IO::Path::IsRelative("/a/b/"));
#endif
	}


	///////////////////////
	// Compare
	///////////////////////

	{
		// int Compare(const PathString16& a, const PathString16& b);
		// int Compare(const PathString8& a, const PathString8& b);
		// int Compare(PathString16::const_iterator a, PathString16::const_iterator a_end,
		//             PathString16::const_iterator b, PathString16::const_iterator b_end);
		// int Compare(PathString8::const_iterator a, PathString8::const_iterator a_end,
		//             PathString8::const_iterator b, PathString8::const_iterator b_end);

		int result;
		PathString8 path1, path2;

		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 0);

		path1 = "a";
		path2 = "b";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == -1);

		path1 = "a";
		path2 = "A";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "A";
		path2 = "b";
		result = Compare(path1, path2, kCaseInsensitive, true);
		EATEST_VERIFY(result == -1);

		path1 = "b";
		path2 = "a";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "B";
		path2 = "a";
		result = Compare(path1, path2, kCaseInsensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "abcd";
		path2 = "abce";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == -1);

		path1 = "abc";
		path2 = "abcd";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == -1);

		path1 = "abcd";
		path2 = "abc";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "/abc/abc";
		path2 = "/abc/abc";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 0);

		path1 = "/abc";
		path2 = "/abc/";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "/abc";
		path2 = "/abc/";
		result = Compare(path1, path2, kCaseSensitive, false);
		EATEST_VERIFY(result == -1);

		path1 = "/abc/";
		path2 = "/abc";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == -1);

		path1 = "/abc/abc.txt";
		path2 = "/ABC/ABC.TXT";
		result = Compare(path1, path2, kCaseInsensitive, true);
		EATEST_VERIFY(result == 0);

		path1 = "/a.txt";
		path2 = "/a/b";
		result = Compare(path1, path2, kCaseSensitive, false);
		EATEST_VERIFY(result == -1);

		path1 = "/a.txt";
		path2 = "/a/b";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == 1);

		path1 = "/a/b";
		path2 = "/a.txt";
		result = Compare(path1, path2, kCaseSensitive, true);
		EATEST_VERIFY(result == -1);
	}


	///////////////////////
	// Compare
	///////////////////////

	{
		// bool IsSubdirectory(const PathString16& dir, const PathString16& subdir);
		// bool IsSubdirectory(const PathString8& dir, const PathString8& subdir);

		bool bResult;

		bResult = IsSubdirectory(PathString8("/a/b/c/"), PathString8("/a/b/c/d/"));
		EATEST_VERIFY(bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c/"), PathString8("/a/b/c/"));
		EATEST_VERIFY(bResult);

		//Disabled until the implementation can be fixed.
		//bResult = IsSubdirectory(PathString8("/a/b/c/"), PathString8("/a/b/c/d"));
		//EATEST_VERIFY(!bResult); // Second argument isn't a directory

		bResult = IsSubdirectory(PathString8("/a/b/c"),  PathString8("/a/b/c"));
		EATEST_VERIFY(bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c"),  PathString8("/a/b/c/"));
		EATEST_VERIFY(!bResult); // First argument isn't a directory.

		bResult = IsSubdirectory(PathString8(""), PathString8(""));
		EATEST_VERIFY(bResult);

		bResult = IsSubdirectory(PathString8(""), PathString8("a"));
		EATEST_VERIFY(bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c"),  PathString8("/a/b/cd"));
		EATEST_VERIFY(!bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c/"), PathString8("/a/b/"));
		EATEST_VERIFY(!bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c/"), PathString8("/c/b/"));
		EATEST_VERIFY(!bResult);

		bResult = IsSubdirectory(PathString8("/a/b/c"),  PathString8(""));
		EATEST_VERIFY(!bResult);

		#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS) && EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS
			bResult = IsSubdirectory(PathString8("C:\\"),  PathString8("C:\\a\\"));
			EATEST_VERIFY(bResult);

			// This test is temporarily disabled until IsSubdirectory is fixed/modified.
			//bResult = IsSubdirectory(PathString8("\\\\?\\UNC\\CPU\\a\\b"),  PathString8("\\\\?\\UNC\\CPU\\a\\b\\c"));
			//EATEST_VERIFY(bResult);

		
			bResult = IsSubdirectory(PathString8("\\\\?\\C:\\a\\b"),  PathString8("C:\\a\\b\\c"));
			EATEST_VERIFY(!bResult);
		#endif
	}


	
	///////////////////////
	// ComputeRelative
	///////////////////////

	{
		// PathString16& ComputeRelative(PathString16& result, const PathString16& src, const PathString16& target);
		// PathString8&  ComputeRelative(PathString8&  result, const PathString8& src,  const PathString8&  target);

		PathString16   sSource16, sTarget16, sResult16, sResultActual16;
		PathString8    sSource8,  sTarget8,  sResult8,  sResultActual8;
		const int      kTestCount = 7;
		const char* pSourceTargetResult[7][3] = 
		{ 
			// source    target          result     <-- Yes these are in a different order from the function arguments.
			{ "",        "",             ""                 }, 
			{ "",        "/a/b/c",       "/a/b/c"           }, 
			{ "/a/b/c/", "/a/b/c/d",     "d"                }, 
			{ "/a/b",    "/a/b",         ""                 }, 
			{ "/a/b/c/", "/a/b/c/d/e/",  "d/e/"             }, 
			{ "/a/b/c/", "/x/y/z/w",     "../../../x/y/z/w" }, 
			{ "/a/b/c/", "/c/d",         "../../../c/d"     }
		};

		for(int i = 0; i < kTestCount; i++)
		{
			sSource16 = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(pSourceTargetResult[i][0]));
			sTarget16 = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(pSourceTargetResult[i][1]));
			sResult16 = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(pSourceTargetResult[i][2]));

			Canonicalize(sSource16);
			Canonicalize(sTarget16);
			Canonicalize(sResult16);

			ComputeRelative(sResultActual16, sSource16, sTarget16);
			EATEST_VERIFY_F(sResultActual16 == sResult16, "Expected: ComputeRelative(result, \"%I16s\", \"%I16s\") -> \"%I16s\" Actual: \"%I16s\"", sSource16.c_str(), sTarget16.c_str(), sResult16.c_str(), sResultActual16.c_str());
		}

		for(int i = 0; i < kTestCount; i++)
		{
			sSource8 = pSourceTargetResult[i][0];
			sTarget8 = pSourceTargetResult[i][1];
			sResult8 = pSourceTargetResult[i][2];

			Canonicalize(sSource8);
			Canonicalize(sTarget8);
			Canonicalize(sResult8);

			ComputeRelative(sResultActual8, sSource8, sTarget8);
			EATEST_VERIFY_F(sResultActual8 == sResult8, "Expected: ComputeRelative(result, \"%I8s\", \"%I8s\") -> \"%I8s\" Actual: \"%I8s\"", sSource8.c_str(), sTarget8.c_str(), sResult8.c_str(), sResultActual8.c_str());
		}
	}


	{
		// EAIO_API bool GetHasTrailingSeparator(const PathString16& path);
		// EAIO_API bool GetHasTrailingSeparator(const PathString8& path);
		// EAIO_API bool GetHasTrailingSeparator(PathString16::const_iterator first, size_t nLength = kLengthNull);
		// EAIO_API bool GetHasTrailingSeparator(PathString8::const_iterator first, size_t nLength = kLengthNull);
		bool bResult; 

		struct Results
		{
			const char* pPath8;
			bool           bResult[2]; // [0] Microsoft, [1] Unix.
		};

		const Results resultArray[9] = {
		   // Path            Unix       Microsoft
			{ "",           { false,     false } },
			{ "/",          { true,      true  } },
			{ "\\",         { false,     true  } },
			{ "////////",   { true,      true  } },
			{ "/a/b/",      { true,      true  } },
			{ "\\a\\b\\",   { false,     true  } },
			{ "/a",         { false,     false } },
			{ "\\",         { false,     true  } },
			{ "/\\",        { false,     true  } }
		};

		#if defined(EA_PLATFORM_MICROSOFT)
			const int kResultIndex = 1;
		#else
			const int kResultIndex = 0;
		#endif

		for(size_t i = 0; i < EAArrayCount(resultArray); i++)
		{
			PathString16 s16 = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(resultArray[i].pPath8));
			PathString8  s8  = resultArray[i].pPath8;

			bResult = GetHasTrailingSeparator(s16);
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);

			bResult = GetHasTrailingSeparator(s16.c_str(), s16.length());
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);

			bResult = GetHasTrailingSeparator(s16.c_str(), kLengthNull);
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);


			bResult = GetHasTrailingSeparator(s8);
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);

			bResult = GetHasTrailingSeparator(s8.c_str(), s8.length());
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);

			bResult = GetHasTrailingSeparator(s8.c_str(), kLengthNull);
			EATEST_VERIFY(bResult == resultArray[i].bResult[kResultIndex]);
		}

		// Additional case which isn't exercised above:
		bResult = GetHasTrailingSeparator(EA_CHAR16("/a/b/c"), 3); // Test just the "/a/" portion.
		EATEST_VERIFY(bResult);
		bResult = GetHasTrailingSeparator(EA_CHAR16("/a/b/c"), 0);
		EATEST_VERIFY(!bResult);

		bResult = GetHasTrailingSeparator("/a/b/c", 3);
		EATEST_VERIFY(bResult);
		bResult = GetHasTrailingSeparator("/a/b/c", 0);
		EATEST_VERIFY(!bResult);
	}

	{
		// EAIO_API PathString16& EnsureTrailingSeparator(PathString16& path);
		// EAIO_API PathString8& EnsureTrailingSeparator(PathString8& path);
		// EAIO_API bool EnsureTrailingSeparator(char16_t* pDirName, size_t nMaxPermittedLength);
		// EAIO_API bool EnsureTrailingSeparator(char* pDirName, size_t nMaxPermittedLength);
	}

	{
		// EAIO_API PathString16& StripTrailingSeparator(PathString16& path);
		// EAIO_API PathString8& StripTrailingSeparator(PathString8& path);
		// EAIO_API void StripTrailingSeparator(char16_t* pPath, size_t nLength = kLengthNull);
		// EAIO_API void StripTrailingSeparator(char* pPath, size_t nLength = kLengthNull);
		struct Results2
		{
			const char* pPathIn8;
			const char* pPathOut8[2];    // [0] Microsoft, [1] Unix.
		};

		const Results2 resultArray[9] = {
			// Path       Unix       Microsoft
			{ "",       { "",        ""     } },
			{ "/",      { "",        ""     } },
			{ "\\",     { "\\",      ""     } },
			{ "//",     { "/",       "/"    } },
			{ "\\\\",   { "\\\\",    "\\"   } },
			{ "/\\",    { "/\\",     "/"    } },
			{ "\\/",    { "\\",      "\\"   } },
			{ "/a/",    { "/a",      "/a"   } },
			{ "/a\\",   { "/a\\",    "/a" } }
		};

		#if defined(EA_PLATFORM_MICROSOFT)
			const int kResultIndex = 1;
		#else
			const int kResultIndex = 0;
		#endif

		for(size_t i = 0; i < EAArrayCount(resultArray); i++)
		{
			PathString16 s16InOriginal = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(resultArray[i].pPathIn8));
			PathString16 s16Out        = EA::StdC::ConvertString<PathString8, PathString16>(PathString8(resultArray[i].pPathOut8[kResultIndex]));
			PathString16 s16In;

			PathString8  s8InOriginal  = resultArray[i].pPathIn8;
			PathString8  s8Out         = resultArray[i].pPathOut8[kResultIndex];
			PathString8  s8In;

			s16In = s16InOriginal;
			StripTrailingSeparator(s16In);
			EATEST_VERIFY(s16In == s16Out);

			s16In   = s16InOriginal;
			StripTrailingSeparator(&s16In[0]);
			if(s16In != s16InOriginal)
				s16In.pop_back();
			EATEST_VERIFY(s16In == s16Out);


			s8In = s8InOriginal;
			StripTrailingSeparator(s8In);
			EATEST_VERIFY(s8In == s8Out);

			s8In = s8InOriginal;
			StripTrailingSeparator(&s8In[0]);   // This call doesn't realize s8In is a string object, so we need to pop_back ourselves on the next line.
			if(s8In != s8InOriginal)            // In user code this isn't important because the user wouldn't be using a string object with this function.
				s8In.pop_back();
			EATEST_VERIFY(s8In == s8Out);
		}
	}

	return nErrorCount;
}





