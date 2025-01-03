/////////////////////////////////////////////////////////////////////////////
// TestFnMatch.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/FnMatch.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <stdio.h>
#include EA_ASSERT_HEADER


namespace 
{
	using namespace EA::IO;

	struct FnMatchTestData
	{
		const char* mpPattern;
		const char* mpString;
		int         mMatchFlags;
		bool        mbExpectedResult;
	};

	FnMatchTestData gFnMatchTestData[] = 
	{
		{ "",                   "",                 0,                              true  },
		{ "blah.txt",           "",                 0,                              false },
		{ "*",                  "",                 0,                              true  },
		{ "",                   "blah.txt",         0,                              false },
		{ "blah.txt",           "blah.txt",         0,                              true  },
		{ "a/b/c",              "a/b/c",            kFNMUnixPath,                   true  },
		{ "a/b/c",              "a/b/c",            kFNMUnixPath | kFNMPathname,    true  },
	  //{ "a/b/c",              "a/b/c",            kFNMDosPath | kFNMPathname,     false }, // Should this fail or succeed?
		{ "a\\b\\c",            "a\\b\\c",          kFNMDosPath,                    true  },
		{ "a\\b\\c",            "a\\b\\c",          kFNMDosPath | kFNMPathname,     true  },
	  //{ "a\\b\\c",            "a\\b\\c",          kFNMUnixPath | kFNMPathname,    false }, // Should this fail or succeed?
		{ "a/b/c",              "a/_/c",            kFNMUnixPath,                   false },
		{ "a/b/c",              "A/B/C",            kFNMUnixPath,                   false },
		{ "a/b/c",              "A/B/C",            kFNMUnixPath | kFNMCaseFold,    true  },
		{ "a/b/?",              "a/b/c",            kFNMUnixPath,                   true  },
		{ "a/b/?",              "a/b/",             kFNMUnixPath,                   false },
		{ "a\\b\\?",            "a\\b\\",           kFNMDosPath,                    false },
		{ "*",                  "a/b/c",            kFNMUnixPath,                   true  },
		{ "*",                  "a\\b\\c",          kFNMDosPath,                    true  },
		{ "**",                 "a/b/c",            kFNMUnixPath,                   true  },
		{ "**",                 "a\\b\\c",          kFNMDosPath,                    true  },
		{ "???",                "ab",               0,                              false },
		{ "???",                "abc",              0,                              true  },
		{ "a/b/c/*.?[ab]",      "a/b/c/d.qa",       kFNMUnixPath,                   true  },
		{ "a\\b\\c\\*.?[ab]",   "a\\b\\c\\d.qa",    kFNMDosPath,                    true  },
		{ "a/b/c/*.?[ab]",      "a/b/c/d.qq",       kFNMUnixPath,                   false },
		{ "a\\b\\c\\*.?[ab]",   "a\\b\\c\\d.qq",    kFNMDosPath,                    false },
		{ "a/*/*/d",            "a/bbbb/c/d",       kFNMUnixPath,                   true  },
		{ "a\\*\\*\\d",         "a\\bbbb\\c\\d",    kFNMDosPath,                    true  },
		{ "a/*/*/d",            "a/bbbb/c/d",       kFNMUnixPath | kFNMPathname,    true  },
		{ "a\\*\\*\\d",         "a\\bbbb\\c\\d",    kFNMDosPath | kFNMPathname,     true  },
		{ "/abc/def.txt",       "/abc/def.txt",     kFNMUnixPath,                   true  },
		{ "C:\\abc\\def.txt",   "C:\\abc\\def.txt", kFNMDosPath,                    true  },
		{ "/abc/def.txt",       "/abc/Xef.txt",     kFNMUnixPath,                   false },
		{ "C:\\abc\\def.txt",   "C:\\abc\\Xef.txt", kFNMDosPath,                    false }

		// Additional tests to run:
		//    kFNMPathname   = 0x01,  /// Wildcard don't can't see past directory separator chars.
		//    kFNMNoEscape   = 0x02,  /// Escape sequences (e.g. '\t') are not recognized.
		//    kFNMPeriod     = 0x04,  /// A leading '.' is matched only explicitly and not by * or ?.
		//    kFNMLeadingDir = 0x08,  /// Match a leading directory and allow ignoring after it.
		//    kFNMPrefixDir  = 0x10,  /// Directory path text that precedes the string can match the pattern.
		//    kFNMCaseFold   = 0x10   /// Act case-insensitively.
		//    kFNMDosPath    = 0x40,  /// Force the path to be interpreted as a DOS path (e.g. '\') instead of native path.
		//    kFNMUnixPath   = 0x80   /// Force the path to be interpreted as a Unix path (e.g. '/') instead of native path.
	};

} // namespace



///////////////////////////////////////////////////////////////////////////////
// TestFnMatch
//
int TestFnMatch()
{
	using namespace EA::IO;
	using namespace EA::UnitTest;

	int nErrorCount = 0;

	for(size_t i = 0; i < EAArrayCount(gFnMatchTestData); i++)
	{
		const FnMatchTestData& data = gFnMatchTestData[i];

		bool bResult = FnMatch(data.mpPattern, data.mpString, data.mMatchFlags);
		EATEST_VERIFY(bResult == data.mbExpectedResult);

		char16_t pattern16[20], string16[20];
		EA::StdC::Strlcpy(pattern16, data.mpPattern, EAArrayCount(pattern16));
		EA::StdC::Strlcpy(string16,  data.mpString,  EAArrayCount(string16));
		bResult = FnMatch(pattern16, string16, data.mMatchFlags);
		EATEST_VERIFY(bResult == data.mbExpectedResult);

		char32_t pattern32[20], string32[20];
		EA::StdC::Strlcpy(pattern32, data.mpPattern, EAArrayCount(pattern32));
		EA::StdC::Strlcpy(string32,  data.mpString,  EAArrayCount(string32));
		bResult = FnMatch(pattern32, string32, data.mMatchFlags);
		EATEST_VERIFY(bResult == data.mbExpectedResult);
	}

	return nErrorCount;
}







