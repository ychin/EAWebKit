///////////////////////////////////////////////////////////////////////////////
// FnMatch.h
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
//
// Implements a file path match much like the Unix fnmatch function.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_FNMATCH_H
#define EAIO_FNMATCH_H


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>


namespace EA
{
	namespace IO
	{
		enum FnMatchFlags
		{
			kFNMNone       = 0x00,  /// No flags.
			kFNMPathname   = 0x01,  /// Wildcard don't can't see past directory separator chars.
			kFNMNoEscape   = 0x02,  /// Escape sequences (e.g. '\t') are not recognized. Escape sequences are only ever recognized with Unix-style paths and not DOS-style paths that use '\' chars. It's as if this flag is always active when DOS paths are in use.
			kFNMPeriod     = 0x04,  /// A leading '.' is matched only explicitly and not by * or ?.
			kFNMLeadingDir = 0x08,  /// Match a leading directory and allow ignoring after it.
			kFNMPrefixDir  = 0x10,  /// Directory path text that precedes the string can match the pattern.
			kFNMCaseFold   = 0x20,  /// Act case-insensitively.
			kFNMDosPath    = 0x40,  /// Force the path to be interpreted as a DOS path (e.g. '\') instead of native path.
			kFNMUnixPath   = 0x80   /// Force the path to be interpreted as a Unix path (e.g. '/') instead of native path.
		};

		/// FnMatch
		///
		/// Supports the following special pattern symbols:
		///     ?   -- arbitrary character
		///     *   -- arbitrary characters
		///     []  -- selection of characters
		///
		/// The kFNMDosPath and kFNMUnixPath flags can be used to override the assumed
		/// path type, which defaults to the native path type of the current platform.
		///
		/// Example usage:
		///     bool result = FnMatch("a/b/c/*.?[ab]", "a/b/c/d.qa", kFNMCaseFold);
		///
		/// Expected behavior:
		///     Pattern               String              Flags                           Result
		///     ---------------------------------------------------------------------------------
		///     ""                    ""                  0                               true  
		///     "blah.txt",           "",                 0,                              false
		///     "*",                  "",                 0,                              true
		///     "",                   "blah.txt",         0,                              false
		///     "blah.txt"            "blah.txt"          0                               true  
		///     "a/b/c"               "a/b/c"             kFNMUnixPath                    true  
		///     "a/b/c"               "a/b/c"             kFNMUnixPath | kFNMPathname     true  
		///     "a\\b\\c"             "a\\b\\c"           kFNMDosPath                     true  
		///     "a\\b\\c"             "a\\b\\c"           kFNMDosPath | kFNMPathname      true  
		///     "a/b/c"               "a/_/c"             kFNMUnixPath                    false 
		///     "a/b/c"               "A/B/C"             kFNMUnixPath                    false 
		///     "a/b/c"               "A/B/C"             kFNMUnixPath | kFNMCaseFold     true  
		///     "a/b/?"               "a/b/c"             kFNMUnixPath                    true  
		///     "a/b/?"               "a/b/"              kFNMUnixPath                    false 
		///     "a\\b\\?"             "a\\b\\"            kFNMDosPath                     false 
		///     "*"                   "a/b/c"             kFNMUnixPath                    true  
		///     "*"                   "a\\b\\c"           kFNMDosPath                     true  
		///     "**"                  "a/b/c"             kFNMUnixPath                    true  
		///     "**"                  "a\\b\\c"           kFNMDosPath                     true  
		///     "???"                 "ab"                0                               false 
		///     "???"                 "abc"               0                               true  
		///     "a/b/c/*.?[ab]"       "a/b/c/d.qa"        kFNMUnixPath                    true  
		///     "a\\b\\c\\*.?[ab]"    "a\\b\\c\\d.qa"     kFNMDosPath                     true  
		///     "a/b/c/*.?[ab]"       "a/b/c/d.qq"        kFNMUnixPath                    false 
		///     "a\\b\\c\\*.?[ab]"    "a\\b\\c\\d.qq"     kFNMDosPath                     false 
		///     "a/*/*/d"             "a/bbbb/c/d"        kFNMUnixPath                    true  
		///     "a\\*\\*\\d"          "a\\bbbb\\c\\d"     kFNMDosPath                     true  
		///     "a/*/*/d"             "a/bbbb/c/d"        kFNMUnixPath | kFNMPathname     true  
		///     "a\\*\\*\\d"          "a\\bbbb\\c\\d"     kFNMDosPath | kFNMPathname      true  
		///     "/abc/def.txt"        "/abc/def.txt"      kFNMUnixPath                    true  
		///     "C:\\abc\\def.txt"    "C:\\abc\\def.txt"  kFNMDosPath                     true  
		///     "/abc/def.txt"        "/abc/Xef.txt"      kFNMUnixPath                    false 
		///     "C:\\abc\\def.txt"    "C:\\abc\\Xef.txt"  kFNMDosPath                     false 

		EAIO_API bool FnMatch(const char*  EA_RESTRICT pPattern, const char*  EA_RESTRICT pString, int fnMatchFlags = 0);
		EAIO_API bool FnMatch(const char16_t* EA_RESTRICT pPattern, const char16_t* EA_RESTRICT pString, int fnMatchFlags = 0);
		EAIO_API bool FnMatch(const char32_t* EA_RESTRICT pPattern, const char32_t* EA_RESTRICT pString, int fnMatchFlags = 0);
		#if EA_WCHAR_UNIQUE
			inline bool FnMatch(const wchar_t* EA_RESTRICT pPattern, const wchar_t* EA_RESTRICT pString, int fnMatchFlags = 0)
			{
				#if (EA_WCHAR_SIZE == 2)
					return FnMatch(reinterpret_cast<const char16_t *>(pPattern), reinterpret_cast<const char16_t *>(pString), fnMatchFlags);
				#else
					return FnMatch(reinterpret_cast<const char32_t *>(pPattern), reinterpret_cast<const char32_t *>(pString), fnMatchFlags);
				#endif
			}
		#endif

	} // namespace IO

} // namespace EA


#endif // Header include guard








