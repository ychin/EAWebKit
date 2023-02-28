/////////////////////////////////////////////////////////////////////////////
// EAPathString.h
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Talin and Paul Pedriana
//
// Defines common path-manipulation functions on 16-bit buffers.
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_PATHSTRING_H
#define EAIO_PATHSTRING_H


#include <EAIO/internal/Config.h>
#ifndef EASTL_FIXEDSTRING_H
	#include <EASTL/fixed_string.h>
#endif
#ifndef EAIO_ALLOCATOR_H
	#include <EAIO/Allocator.h>
#endif
#ifndef EAIO_EASTREAM_H
	#include <EAIO/EAStream.h>
#endif
#ifndef EAIO_EAFILEBASE_H
	#include <EAIO/EAFileBase.h>
#endif
#ifndef EAIO_EASTLCOREALLOCATOR_H
	#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#endif

#if EA_WCHAR_UNIQUE
	#include <EAStdC/EAString.h>
#endif


namespace EA
{
	namespace IO
	{

		//////////////////////////////////////////////////////////////////////////
		/// The PathString classes operate on file paths stored in an eastl
		/// fixed_string16 class.
		///
		/// Usage Notes:
		///
		///   -- These functions are not templates. This means that only concrete
		///      classes can be used. This means that only fixed_strings with
		///      a nominal size of kMaxPathLength can be used, due to the fact
		///      that the nominal size of of a fixed string is a template argument.
		///
		///   -- char strings are presumed to be UTF8-encoded. char16_t strings
		///      are presumed to be UCS2-encoded.
		///
		///   -- All of the functions here are pure string manipulation functions.
		///      None of them require any Filesystem access.
		///
		///   -- None of the functions listed below rely on the existence of
		///      a global "current working directory" variable.
		///
		///   -- Input arguments can be either PathStrings or PathString
		///      iterators. Raw strings (i.e. char16_t*) can be cast to
		///      PathString::iterator. There is a special iterator value,
		///      kEndAuto16, which indicates that the end iterator position
		///      should be calculated automatically via strlen().
		///
		///   -- Output arguments are always fixed strings.
		///
		///   -- Functions which do not alter the path, but return a pointer will
		///      have both a const and a non-const version, so that if the path
		///      buffer is not const, we can use the resulting pointer to modify
		///      the path. The const version takes a PathBuffer argument
		///      (for maximum convenience), while the non-const version takes
		///      PathString::iterator arguments (for maximum flexibility.)
		///
		///   -- Follows the UTF convention that a path ending with a separator
		///      is presumed to refer to a directory rather than a file.
		//////////////////////////////////////////////////////////////////////////


		namespace Path
		{
			//////////////////////////////////////////////////////////////////////////
			/// The typical maximum path length (in other words, the largest size
			/// that's likely to occur). If it goes over this, then it will allocate
			/// memory for a larger buffer.
			//////////////////////////////////////////////////////////////////////////
#ifdef EA_PLATFORM_WINDOWS
			static const size_t kNominalPathLength = 256;
#else
			static const size_t kNominalPathLength = 96;
#endif

			/// PathStringBase
			typedef eastl::fixed_string<char,  kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBase8;
			typedef eastl::fixed_string<char16_t, kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBase16;
			typedef eastl::fixed_string<char32_t, kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBase32;

			#if EA_WCHAR_UNIQUE
				typedef eastl::fixed_string<wchar_t, kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBaseW;
			#else
				#if (EA_WCHAR_SIZE == 1)
					typedef PathStringBase8 PathStringBaseW;
				#elif(EA_WCHAR_SIZE == 2)
					typedef PathStringBase16 PathStringBaseW;
				#else
					typedef PathStringBase32 PathStringBaseW;
				#endif
			#endif


			/// kEndAuto
			///
			/// Refers to a string ending with '\0'. If you pass kEndAuto16 to a function, 
			/// that means that you want the end of the associated string to be a 0 char.
			///
			static const PathStringBase8::iterator  kEndAuto8  = NULL;
			static const PathStringBase16::iterator kEndAuto16 = NULL;
			static const PathStringBase32::iterator kEndAuto32 = NULL;
			static const PathStringBaseW::iterator kEndAutoW = NULL;
			#if EAIO_BACKWARDS_COMPATIBILITY
				static const PathStringBase16::iterator kEndAuto   = NULL; // For backward compatibility.
			#endif



			/// PathString8
			///
			/// The PathString type is simply a fixed_string of length kNominalPathLength.
			///
			class PathString8 : public PathStringBase8
			{
			public:
				typedef PathStringBase8 base_type;

				PathString8();
				PathString8(const PathString8& ps);
				PathString8(const char* path);
				PathString8(const char* path, size_type n);
				PathString8(size_type n, char c);
				PathString8(const char* first, const char* last);
				PathString8(const PathString8& ps, size_type position, size_type n = base_type::npos);

				const PathString8& operator=(const PathString8& ps);
				const PathString8& operator=(const char* path);
				const PathString8& operator=(const char c);

				PathString8& operator+=(const PathString8& ps);
				PathString8& operator+=(const char* p);
				PathString8& operator+=(const char c);
			};


			/// PathString16
			///
			/// The PathString type is simply a fixed_string of length kNominalPathLength.
			///
			class PathString16 : public PathStringBase16
			{
			public:
				typedef PathStringBase16 base_type;

				PathString16();
				PathString16(const PathString16& ps);
				PathString16(const char16_t* path);
				PathString16(const char16_t* path, size_type n);
				PathString16(size_type n, char16_t c);
				PathString16(const char16_t* first, const char16_t* last);
				PathString16(const PathString16& ps, size_type position, size_type n = base_type::npos);

				const PathString16& operator=(const PathString16& ps);
				const PathString16& operator=(const char16_t* path);
				const PathString16& operator=(const char16_t c);

				PathString16& operator+=(const PathString16& ps);
				PathString16& operator+=(const char16_t* p);
				PathString16& operator+=(const char16_t c);
			};
			#if EAIO_BACKWARDS_COMPATIBILITY
				#define PathString PathString16 // This is a define instead of a typedef because a typedef would preclude forward declaration of PathString.
			#endif


			/// PathString32
			///
			/// The PathString type is simply a fixed_string of length kNominalPathLength.
			///
			class PathString32 : public PathStringBase32
			{
			public:
				typedef PathStringBase32 base_type;

				PathString32();
				PathString32(const PathString32& ps);
				PathString32(const char32_t* path);
				PathString32(const char32_t* path, size_type n);
				PathString32(size_type n, char32_t c);
				PathString32(const char32_t* first, const char32_t* last);
				PathString32(const PathString32& ps, size_type position, size_type n = base_type::npos);

				const PathString32& operator=(const PathString32& ps);
				const PathString32& operator=(const char32_t* path);
				const PathString32& operator=(const char32_t c);

				PathString32& operator+=(const PathString32& ps);
				PathString32& operator+=(const char32_t* p);
				PathString32& operator+=(const char32_t c);
			};

			#if EA_WCHAR_UNIQUE
				/// PathStringW
				///
				/// The PathString type is simply a fixed_string of length kNominalPathLength.
				///
				class PathStringW : public PathStringBaseW
				{
				public:
					typedef PathStringBaseW base_type;

					PathStringW();
					PathStringW(const PathStringW& ps);
					PathStringW(const wchar_t* path);
					PathStringW(const wchar_t* path, size_type n);
					PathStringW(size_type n, wchar_t c);
					PathStringW(const wchar_t* first, const wchar_t* last);
					PathStringW(const PathStringW& ps, size_type position, size_type n = base_type::npos);
					PathStringW(const PathString16& pSrcPath16);
					PathStringW(const PathString32& pSrcPath32);

					const PathStringW& operator=(const PathStringW& ps);
					const PathStringW& operator=(const wchar_t* path);
					const PathStringW& operator=(const wchar_t c);

					PathStringW& operator+=(const PathStringW& ps);
					PathStringW& operator+=(const wchar_t* p);
					PathStringW& operator+=(const wchar_t c);
				};
			#else

				#if (EA_WCHAR_SIZE == 1)
					typedef PathString32 PathStringW;
				#elif(EA_WCHAR_SIZE == 2)
					typedef PathString16 PathStringW;
				#else
					typedef PathString32 PathStringW;
				#endif
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Generic, low-level path parsing function.
			///
			/// For purposes of this function, a 'path component' is defined
			/// as the following:
			///
			///   [unc-prefix]? [filename_char]+ [separator_char]?
			///
			/// In other words, it consists of an optional UNC prefix,
			/// followed by one or more file or directory name chars,
			/// which is in turn followed by a separator character.
			///
			/// Given an iterator to any point within a path component, as
			/// defined above, this function will return an iterator to the
			/// start of the next subsequent path component - in other words,
			/// the character immediately following the separator.
			///
			/// If there is no next component, then 'last' is returned.
			///
			/// Note: For purposes of this function, the drive and/or UNC part of the
			/// path are considered a single component.
			///
			/// Example use:
			///    FindComponentFwd("/abc/def/ghi/")    => "abc/def/ghi/"
			///    FindComponentFwd("abc/def/ghi/")     => "def/ghi/"
			///    FindComponentFwd("\\abc\def")        => "def"
			///    FindComponentFwd("C:\abc\def")       => "abc\def"
			///    FindComponentFwd("")                 => ""
			///    FindComponentFwd("abc")              => ""
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator FindComponentFwd(PathString8::iterator first,
															PathString8::iterator last = kEndAuto8);
			EAIO_API PathString16::iterator FindComponentFwd(PathString16::iterator first,
															 PathString16::iterator last = kEndAuto16);
			EAIO_API PathString32::iterator FindComponentFwd(PathString32::iterator first,
															 PathString32::iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator FindComponentFwd(PathStringW::iterator first,
																 PathStringW::iterator last = kEndAutoW);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Const version of above.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator FindComponentFwd(PathString8::const_iterator first,
																  PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16::const_iterator FindComponentFwd(PathString16::const_iterator first,
																   PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32::const_iterator FindComponentFwd(PathString32::const_iterator first,
																   PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator FindComponentFwd(PathStringW::const_iterator first,
																	  PathStringW::const_iterator last = kEndAutoW);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Generic, low-level path parsing function.
			///
			/// For purposes of this function, a 'path component' is defined
			/// as the following:
			///
			///   [unc-prefix]? [filename_char]+ [separator_char]?
			///
			/// In other words, it consists of an optional UNC prefix,
			/// followed by one or more file or directory name chars,
			/// which is in turn followed by a separator character.
			///
			/// Given an iterator to any point within a path component, as
			/// defined above, this function will return an iterator to the
			/// start of the preceding path component - in other words,
			/// to the start of the first separator character.
			///
			/// If there is no previous component, then 'first' is returned.
			///
			/// Note: For purposes of this function, the drive and/or UNC part of the
			/// path are considered a single component.
			///
			/// Example use:
			///    FindComponentRvs("/abc/def/ghi/")    =>  "ghi/"
			///    FindComponentRvs("/abc/def/ghi")     =>  "ghi"
			///    FindComponentRvs("\\abc\def")        =>  "def"
			///    FindComponentRvs("\\abc")            =>  "\\abc"
			///    FindComponentRvs("C:\abc\def")       =>  "def"
			///    FindComponentRvs("C:\abc")           =>  "abc"
			///    FindComponentRvs("C:\")              =>  "C:\"
			///    FindComponentRvs("")                 =>  ""
			///    FindComponentRvs("abc")              =>  "abc"
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator FindComponentRvs(PathString8::iterator first,
															PathString8::iterator last = kEndAuto8);
			EAIO_API PathString16::iterator FindComponentRvs(PathString16::iterator first,
															 PathString16::iterator last = kEndAuto16);
			EAIO_API PathString32::iterator FindComponentRvs(PathString32::iterator first,
															 PathString32::iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator FindComponentRvs(PathStringW::iterator first,
																 PathStringW::iterator last = kEndAutoW);
			#endif

			//////////////////////////////////////////////////////////////////////////
			/// Const version of above.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator FindComponentRvs(PathString8::const_iterator first,
																  PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16::const_iterator FindComponentRvs(PathString16::const_iterator first,
																   PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32::const_iterator FindComponentRvs(PathString32::const_iterator first,
																   PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator FindComponentRvs(PathStringW::const_iterator first,
																	  PathStringW::const_iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Generic, low-level path parsing function. Returns the start of the
			/// Nth directory component, or path.end() if there is no such component.
			///
			/// If nIndex is negative, then it counts from the end, where the
			/// last component has an index of -1. If there is a trailing separator,
			/// it is ignored - in other words, the end of the last component is
			/// considered to be the character before the final separator.
			///
			/// Note: For purposes of this function, the drive and/or UNC part of the
			/// path are considered a single component.
			///
			/// Example use:
			///     PathString16 path16(EA_CHAR16("C:/baloney/salami/"));
			///
			///     GetPathComponentStart(begin, end, 0) ==> "C:/baloney/salami/"
			///     GetPathComponentStart(begin, end, 1) ==> "baloney/salami/"
			///     GetPathComponentStart(begin, end, 2) ==> "salami/"
			///
			///     path16.assign(EA_CHAR16("C:\\baloney\\salami\\"));
			///
			///     GetPathComponentStart(begin, end, 0) ==> "C:\balony\salami\"
			///     GetPathComponentStart(begin, end, 1) ==> "baloney\salami"
			///     GetPathComponentStart(begin, end, 2) ==> "salami\"
			///
			///     path16.assign(EA_CHAR16("/C:/baloney/salami/"));
			///
			///     GetPathComponentStart(begin, end, 0) ==> "/C:/baloney/salami/"
			///     GetPathComponentStart(begin, end, 1) ==> "C:/baloney/salami/"
			///     GetPathComponentStart(begin, end, 2) ==> "baloney/salami/"
			///
			///     path16.assign(EA_CHAR16("/usr/local/foo/"));
			///
			///     GetPathComponentStart(begin, end, 0) ==> "/usr/local/foo/"
			///     GetPathComponentStart(begin, end, 1) ==> "usr/local/foo/"
			///     GetPathComponentStart(begin, end, 2) ==> "local/foo/"
			///
			///     path16.assign(EA_CHAR16("usr/local/foo/"));
			///
			///     GetPathComponentStart(begin, end, 0) ==> "usr/local/foo/"
			///     GetPathComponentStart(begin, end, 1) ==> "local/foo/"
			///     GetPathComponentStart(begin, end, 2) ==> "foo/"
			///
			///     GetPathComponentStart(begin, end, -1) ==> "foo/"
			///     GetPathComponentStart(begin, end, -2) ==> "local/foo/"
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator GetPathComponentStart(PathString8::iterator first,
																 PathString8::iterator last,
																 int32_t nIndex);
			EAIO_API PathString16::iterator GetPathComponentStart(PathString16::iterator first,
																  PathString16::iterator last,
																  int32_t nIndex);
			EAIO_API PathString32::iterator GetPathComponentStart(PathString32::iterator first,
																  PathString32::iterator last,
																  int32_t nIndex);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator GetPathComponentStart(PathStringW::iterator first,
																		   PathStringW::iterator last,
																		   int32_t nIndex);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Generic, low-level path parsing function. Returns the end of the
			/// Nth directory component, or path.end() if there is no such component.
			///
			/// If nIndex is negative, then it counts from the end, where the
			/// last component has an index of -1. If there is a trailing separator,
			/// it is ignored - in other words, the end of the last component is
			/// considered to be the character before the final separator.
			///
			/// Note: For purposes of this function, the drive and/or UNC part of the
			/// path are considered a directory component.
			///
			/// Example use:
			///     PathString16 path16(EA_CHAR16("/usr/local/foo/bar/"));
			///
			///     GetPathComponentEnd(begin, end, 0) ==> "/usr/local/foo/bar/"    // Zero-th component is "/"
			///     GetPathComponentEnd(begin, end, 1) ==> "/local/foo/bar/"        // First component is "usr/"
			///     GetPathComponentEnd(begin, end, 2) ==> "/foo/bar/"              // Second component is "local/"
			///
			///     GetPathComponentEnd(begin, end, -1) ==> "/"                     // First from end is "bar/" ==> "/"
			///     GetPathComponentEnd(begin, end, -2) ==> "/bar/"                 // Second from end is "foo/bar/" ==> "/bar/"
			///
			/// Example use:
			///     PathString8 path8("/");
			///     GetPathComponentEnd(begin, end, 0) ==> "/"                      // First component is ""
			///     GetPathComponentEnd(begin, end, 1) ==> ""                       // First component is "/"
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator GetPathComponentEnd(PathString8::iterator first,
															   PathString8::iterator last,
															   int32_t nIndex);
			EAIO_API PathString16::iterator GetPathComponentEnd(PathString16::iterator first,
																PathString16::iterator last,
																int32_t nIndex);
			EAIO_API PathString32::iterator GetPathComponentEnd(PathString32::iterator first,
																PathString32::iterator last,
																int32_t nIndex);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator GetPathComponentEnd(PathStringW::iterator first,
																   PathStringW::iterator last,
																   int32_t nIndex);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Returns the directory as a string. 
			/// Directory always ends with a path separator.
			/// If no directory is present then the return value will be empty.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8 GetDirectoryString(PathString8::const_iterator first,
													PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16 GetDirectoryString(PathString16::const_iterator first,
													 PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32 GetDirectoryString(PathString32::const_iterator first,
													 PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW GetDirectoryString(PathStringW::const_iterator first,
														PathStringW::const_iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Returns the file name (including any extension) as a string. 
			/// Directory always ends with a path separator.
			/// If no directory is present then the return value will be empty.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8 GetFileNameString(PathString8::const_iterator first,
												   PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16 GetFileNameString(PathString16::const_iterator first,
													PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32 GetFileNameString(PathString32::const_iterator first,
													PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW GetFileNameString(PathStringW::const_iterator first,
														PathStringW::const_iterator last = kEndAutoW);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Return the character immediately following the last directory separator.
			/// Thus this returns the file name (including any extension). 
			/// Returns path.end() if not found.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator  GetFileName(const PathString8& path);
			EAIO_API PathString16::const_iterator GetFileName(const PathString16& path);
			EAIO_API PathString32::const_iterator GetFileName(const PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator GetFileName(const PathStringW& path);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Return the character immediately following the last directory separator.
			/// Returns a pointer to the terminating NULL if not found.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator GetFileName(PathString8::const_iterator first,
													   PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16::iterator GetFileName(PathString16::const_iterator first,
														PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32::iterator GetFileName(PathString32::const_iterator first,
														PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator GetFileName(PathStringW::const_iterator first,
														   PathStringW::const_iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Returns the file extension (including '.' char) as a string. 
			/// The extension is defined beginning with the last '.' found in the file 
			/// name. If no '.' is found in the file name then the return value is empty.
			/// If no directory is present then the return value will be empty.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8 GetFileExtensionString(PathString8::const_iterator first,
														PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16 GetFileExtensionString(PathString16::const_iterator first,
														 PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32 GetFileExtensionString(PathString32::const_iterator first,
														 PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW GetFileExtensionString(PathStringW::const_iterator first,
															PathStringW::const_iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Returns a pointer to the file extension (including the '.' character)
			/// or a pointer to the path.end() if there is none.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator GetFileExtension(const PathString8& path);
			EAIO_API PathString16::const_iterator GetFileExtension(const PathString16& path);
			EAIO_API PathString32::const_iterator GetFileExtension(const PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator GetFileExtension(const PathStringW &path);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Returns a pointer to the file extension (including the '.' character)
			/// or a pointer to the end of the string if there is none.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator GetFileExtension(PathString8::const_iterator first,
															PathString8::const_iterator last = kEndAuto8);
			EAIO_API PathString16::iterator GetFileExtension(PathString16::const_iterator first,
															 PathString16::const_iterator last = kEndAuto16);
			EAIO_API PathString32::iterator GetFileExtension(PathString32::const_iterator first,
															 PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator GetFileExtension(PathStringW::const_iterator first,
																PathStringW::const_iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Returns a pointer to the character immediately following the separator
			/// that divides the drive letter sequence or UNC path from the rest of the path.
			/// If there is none, then the start of the string is returned.
			/// Example use:
			///     "C:\a\b.c"                            => "\a\b.c"
			///     "\\Computer\Directory\FileName"       => "\Directory\FileName"
			///     "\\?\UNC\Computer\Directory\FileName" => "\Computer\Directory\FileName" (this is "long UNC")
			///     "\\?\C:\a\b.c"                        => "\a\b.c"      (or should this return C:\a\b.c)
			///     "/a/b/c"                              => "/a/b/c"
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator GetLocalRoot(const PathString8& path);
			EAIO_API PathString16::const_iterator GetLocalRoot(const PathString16& path);
			EAIO_API PathString32::const_iterator GetLocalRoot(const PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator GetLocalRoot(const PathStringW& path);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Returns a pointer to the character immediately following the separator
			/// that divides the drive letter or UNC path from the rest of the path.
			/// If there is none, then the start of the string is returned.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::iterator GetLocalRoot(PathString8::iterator first,
														PathString8::iterator last = kEndAuto8);
			EAIO_API PathString16::iterator GetLocalRoot(PathString16::iterator first,
														 PathString16::iterator last = kEndAuto16);
			EAIO_API PathString32::iterator GetLocalRoot(PathString32::iterator first,
														 PathString32::iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::iterator GetLocalRoot(PathStringW::iterator first,
															PathStringW::iterator last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Does a simple path concatenation. Modifies the destination buffer
			/// to contain dst + '/' + suffix, unless the suffix is a non-relative
			/// path, in which case it will contain simply suffix.
			/// Does not normalize the path.
			///
			/// The return value is the destination path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Append(PathString8& dst,
										 PathString8::const_iterator suffix_first,
										 PathString8::const_iterator suffix_last = kEndAuto8);
			EAIO_API PathString16& Append(PathString16& dst,
										  PathString16::const_iterator suffix_first,
										  PathString16::const_iterator suffix_last = kEndAuto16);
			EAIO_API PathString32& Append(PathString32& dst,
										  PathString32::const_iterator suffix_first,
										  PathString32::const_iterator suffix_last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Append(PathStringW &dst,
											 PathStringW::const_iterator suffix_first,
											 PathStringW::const_iterator suffix_last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Does a simple path concatenation. Modifies the destination buffer
			/// to contain dst + '/' + suffix, unless the suffix is a non-relative
			/// path, in which case it will contain simply suffix.
			/// Does not normalize the path.
			///
			/// The return value is the destination path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Append(PathString8& dst, const PathString8& suffix);
			EAIO_API PathString16& Append(PathString16& dst, const PathString16& suffix);
			EAIO_API PathString32& Append(PathString32& dst, const PathString32& suffix);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Append(PathStringW &dst, const PathStringW& suffix);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Does a path concatenation and resolution. Collapses all relative path
			/// specifiers and normalizes the path. (Essentially this is just
			/// a convenient wrapper for Append() + Normalize().)
			///
			/// The return value is the destination path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Join(PathString8& dst,
									   PathString8::const_iterator suffix_first,
									   PathString8::const_iterator suffix_last = kEndAuto8);
			EAIO_API PathString16& Join(PathString16& dst,
										PathString16::const_iterator suffix_first,
										PathString16::const_iterator suffix_last = kEndAuto16);
			EAIO_API PathString32& Join(PathString32& dst,
										PathString32::const_iterator suffix_first,
										PathString32::const_iterator suffix_last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Join(PathStringW &dst,
										   PathStringW::const_iterator suffix_first,
										   PathStringW::const_iterator suffix_last = kEndAutoW);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Does a path concatenation and resolution. Collapses all relative path
			/// specifiers and normalizes the path. (Essentially this is just
			/// a convenient wrapper for Append() + Normalize().)
			///
			/// The return value is the destination path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Join(PathString8& dst, const PathString8& suffix);
			EAIO_API PathString16& Join(PathString16& dst, const PathString16& suffix);
			EAIO_API PathString32& Join(PathString32& dst, const PathString32& suffix);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Join(PathStringW &dst, const PathStringW& suffix);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Splits a file path into its individual components
			/// Any component that is NULL will be skipped.
			///
			/// Example file path:
			///     C:\Dir1\Dir2\File.txt
			/// Output:
			///     pDrive      - pointer to drive string. Includes ':' if a drive was given. Unused with Unix-like file paths. (Example: "C:")
			///     pDirectory  - pointer to subdirectory string. Includes leading and trailing '/' or '\', if any. (Example: "\Dir1\Dir2")
			///     pFileName   - pointer to file base name. (Example: "File")
			///     pExtension  - pointer to file extension, if any. Includes leading '.'. (Example: ".txt")
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API void Split(const PathString8&  path, PathString8*  pDrive, PathString8*  pDirectory, PathString8*  pFileName, PathString8*  pFileExtension);
			EAIO_API void Split(const PathString16& path, PathString16* pDrive, PathString16* pDirectory, PathString16* pFileName, PathString16* pFileExtension);
			EAIO_API void Split(const PathString32& path, PathString32* pDrive, PathString32* pDirectory, PathString32* pFileName, PathString32* pFileExtension);
			#if EA_WCHAR_UNIQUE
				EAIO_API void Split(const PathStringW& path, PathStringW* pDrive, PathStringW* pDirectory, PathStringW* pFileName, PathStringW* pFileExtension);
			#endif

			EAIO_API void Split(PathString8::const_iterator  path, PathString8::const_iterator&  pDrive, PathString8::const_iterator&  pDirectory, PathString8::const_iterator&  pFileName, PathString8::const_iterator&  pFileExtension);
			EAIO_API void Split(PathString16::const_iterator path, PathString16::const_iterator& pDrive, PathString16::const_iterator& pDirectory, PathString16::const_iterator& pFileName, PathString16::const_iterator& pFileExtension);
			EAIO_API void Split(PathString32::const_iterator path, PathString32::const_iterator& pDrive, PathString32::const_iterator& pDirectory, PathString32::const_iterator& pFileName, PathString32::const_iterator& pFileExtension);
			#if EA_WCHAR_UNIQUE
				EAIO_API void Split(PathStringW::const_iterator path, PathStringW::const_iterator& pDrive, PathStringW::const_iterator& pDirectory, PathStringW::const_iterator& pFileName, PathStringW::const_iterator& pFileExtension);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Truncates the path at the start of the Nth component. 
			/// If N is less than 0, then it truncates the Nth component from the end.
			/// Retunrs iterator to new end.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8::const_iterator TruncateComponent(PathString8& path, int nIndex);
			EAIO_API PathString16::const_iterator TruncateComponent(PathString16& path, int nIndex);
			EAIO_API PathString32::const_iterator TruncateComponent(PathString32& path, int nIndex);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW::const_iterator TruncateComponent(PathStringW& path, int nIndex);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Collapse all relative path specifiers and convert all path separators
			/// to the canonical type for this platform.
			/// Returns the input path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Normalize(PathString8& path);
			EAIO_API PathString16& Normalize(PathString16& path);
			EAIO_API PathString32& Normalize(PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Normalize(PathStringW& path);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Collapse all relative path specifiers, but do not convert separators.
			/// Returns the input path.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Simplify(PathString8& path);
			EAIO_API PathString16& Simplify(PathString16& path);
			EAIO_API PathString32& Simplify(PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Simplify(PathStringW& path);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Convert path separators (/ and \) to the form specified by 'separator',
			/// but do not simplify (convert relative directory sequences).
			/// Makes sure all path separators are consistent and makes sure the directory
			/// ends with a path separator. This function is not a generic broken path-fixing
			/// function, so an invalid path specification yields undefined behaviour.
			/// Destination pointer must point to enough space to hold a maximum possible length value.
			/// Returns the result directory.
			/// The source and destination cannot point to the same location in memory.
			///
			/// Example output (assuming Win32 directory separators):
			///     Input                                          Output
			///     -------------------------------------------------------------------------
			///     CanonicalizeDirectory("/a/b/c/d/", '/')        "/a/b/c/d/"
			///     CanonicalizeDirectory("/a/b/c/d" , '/')        "/a/b/c/d/"
			///     CanonicalizeDirectory("\a\b\c\d" , '/')        "/a/b/c/d/"
			///     CanonicalizeDirectory("\a\b\c\d\", '\')        "\a\b\c\d\"
			///     CanonicalizeDirectory("/a/b/c/d" , '/')        "/a/b/c/d/"
			///     CanonicalizeDirectory("/"        , '\')        "\"
			///     CanonicalizeDirectory(""         , '\')        "\"
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& Canonicalize(PathString8& path, char separator = EA_FILE_PATH_SEPARATOR_8);
			EAIO_API PathString16& Canonicalize(PathString16& path, char16_t separator = EA_FILE_PATH_SEPARATOR_16);
			EAIO_API PathString32& Canonicalize(PathString32& path, char32_t separator = EA_FILE_PATH_SEPARATOR_32);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& Canonicalize(PathStringW& path, wchar_t separator = EA_FILE_PATH_SEPARATOR_W);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Returns true if the specified path is a relative (not absolute) path.
			/// A relative path is one that starts with a directory name (no leading
			/// slash) or a relative path specifier.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API bool IsRelative(PathString8::const_iterator first,
									 PathString8::const_iterator last = kEndAuto8);
			EAIO_API bool IsRelative(PathString16::const_iterator first,
									 PathString16::const_iterator last = kEndAuto16);
			EAIO_API bool IsRelative(PathString32::const_iterator first,
									 PathString32::const_iterator last = kEndAuto32);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool IsRelative(PathStringW::const_iterator first,
										 PathStringW::const_iterator last = kEndAutoW);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Returns true if the specified path is a relative (not absolute) path.
			/// A relative path is one that starts with a directory name (no leading
			/// slash) or a relative path specifier.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API bool IsRelative(const PathString8& path);
			EAIO_API bool IsRelative(const PathString16& path);
			EAIO_API bool IsRelative(const PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool IsRelative(const PathStringW& path);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Compare two file paths.
			///
			/// Returns -1, 0 or 1, with -1 meaning that a was less than b and should
			/// sort before it.
			/// The comparison is aware of path components and thus sorts directories
			/// independently of files, which is usually what you want. If you don't 
			/// want a component-aware sort to occur, simply use strcmp to compare the 
			/// directory paths.
			/// The path is not simplified during processing, and thus relative path
			/// specifiers are not collapsed.
			//////////////////////////////////////////////////////////////////////////
			EAIO_API int Compare(const PathString8& a, const PathString8& b, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			EAIO_API int Compare(const PathString16& a, const PathString16& b, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			EAIO_API int Compare(const PathString32& a, const PathString32& b, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			#if EA_WCHAR_UNIQUE 
				EAIO_API int Compare(const PathStringW& a, const PathStringW& b, 
									 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
									 bool bDirectoriesFirst = true);
			#endif



			EAIO_API int Compare(PathString8::const_iterator a, PathString8::const_iterator a_end,
								 PathString8::const_iterator b, PathString8::const_iterator b_end, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			EAIO_API int Compare(PathString16::const_iterator a, PathString16::const_iterator a_end,
								 PathString16::const_iterator b, PathString16::const_iterator b_end, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			EAIO_API int Compare(PathString32::const_iterator a, PathString32::const_iterator a_end,
								 PathString32::const_iterator b, PathString32::const_iterator b_end, 
								 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
								 bool bDirectoriesFirst = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API int Compare(PathStringW::const_iterator a, PathStringW::const_iterator a_end,
									 PathStringW::const_iterator b, PathStringW::const_iterator b_end, 
									 CaseSensitivity caseSensitivity = kCaseSensitiveDefault, 
									 bool bDirectoriesFirst = true);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Compute the relative path between two absolute paths.
			///
			/// Given two absolute paths, returns a path which represents the relative
			/// relationship between them (may return an absolute path if there is
			/// no commonality.) Both paths must be in normalized or simplified form 
			/// (i.e. don't contain relative path specifiers). The result path string 
			/// must not be the same object as the src or target strings.
			///
			/// The result will be the relative path which, when concatenated onto
			/// src yields target. Thus source + result = target. The resulting path
			/// will have canonicalized path separators (i.e. the proper separators for 
			/// the current platform).
			///
			/// Example results:
			///     Input                                                 Result          Note
			///     ---------------------------------------------------------------------------------
			///     ComputeRelative(result, "",        "")                ""
			///     ComputeRelative(result, "",        "/a/b/c")          "/a/b/c"
			///     ComputeRelative(result, "",        "/a/b/c/")         "/a/b/c/"
			///     ComputeRelative(result, "/a/b",    "/a/b")            ""
			///     ComputeRelative(result, "/a/b/c/", "/a/b/c/d")        "d"
			///     ComputeRelative(result, "/a/b/c/", "/a/b/c/d/e/")     "d/e/"
			///     ComputeRelative(result, "/a/b/c/", "/x/y/z/w")        "/x/y/z/w"      No relation
			///     ComputeRelative(result, "/a/b/c/", "/c/d")            "/c/d"          No relation
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8& ComputeRelative(PathString8& result,
												  const PathString8& src,
												  const PathString8& target);
			EAIO_API PathString16& ComputeRelative(PathString16& result,
												   const PathString16& src,
												   const PathString16& target);
			EAIO_API PathString32& ComputeRelative(PathString32& result,
												   const PathString32& src,
												   const PathString32& target);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& ComputeRelative(PathStringW& result,
													   const PathStringW& src,
													   const PathStringW& target);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Returns true of pSubDirectory is a sub directory of pDirectory. 
			/// If either path is relative, they will be resolved before the paths are compared.
			///
			/// Note: In order for the comparison to be meaningful, both paths must
			/// either be absolute or relative to the same base.
			///
			/// Returns true if the input pPathSubdirectory is a child directory of the
			/// input pPathDirectory. Both arguments are expected to be full directory paths.
			/// The source and destination cannot point to the same location in memory.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			///
			/// This function doesn't attempt to tell if either path actually refers to a directory
			/// or refers to a directory or file; it assumes the two paths refer to directories.
			/// 
			/// This function looks only at the path strings and doesn't attempt to tell if 
			/// there is some OS mapping that allows one path to be mapped to another and 
			/// be a child of it despite having unrelated strings. 
			///
			/// Example return values:
			///     Call                                                       Return value       Notes
			///     -------------------------------------------------------------------------------------------------------------------
			///     IsSubdirectory("/a/b/c/", "/a/b/c/d/");                    true               This is the basic true case.
			///     IsSubdirectory("/a/b/c/", "/a/b/c/");                      true               Both directories are the same.
			///     IsSubdirectory("/a/b/c",  "/a/b/c/d/");                    false              Parent directory doesn't end with '/', so it's not a directory.
			///     IsSubdirectory("/a/b/c/", "/a/b/c/d");                     false              Child directory doesn't end with '/', so it's not a directory.
			///     IsSubdirectory("/a/b/c",  "/a/b/c");                       true               Neither directory ends with '/'.
			///     IsSubdirectory("/a/b/c",  "/a/b/c/");                      false              Parent directory doesn't end with '/', so it's not a directory.
			///     IsSubdirectory("", "");                                    true               Empty parent always returns true.
			///     IsSubdirectory("", "a");                                   true               Empty parent always returns true.
			///     IsSubdirectory("/a/b/c",  "/a/b/cd");                      false              Child is not subdirectory of parent.
			///     IsSubdirectory("/a/b/c/", "/a/b/");                        false              Child is not subdirectory of parent.
			///     IsSubdirectory("/a/b/c/", "/c/b/");                        false              Child is not subdirectory of parent.
			///     IsSubdirectory("/a/b/c",  "");                             false              Nonempty parent, empty child.
			///     IsSubdirectory("C:\",     "C:\a\");                        true               This is the basic true case, for a Microsoft drive-based path.
			///     IsSubdirectory("\\?\UNC\CPU\a\b", "\\?\UNC\CPU\a\b\c");    true               UNC directories are supported.
			///     IsSubdirectory("\\?\C:\a\b", "C:\a\b\c");                  false              IsSubDirectory doesn't try to be smart about paths. To consider: support simple smart cases like this.
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API bool IsSubdirectory(const PathString8& dir, const PathString8& subdir);
			EAIO_API bool IsSubdirectory(const PathString16& dir, const PathString16& subdir);
			EAIO_API bool IsSubdirectory(const PathString32& dir, const PathString32& subdir);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool IsSubdirectory(const PathStringW& dir, const PathStringW& subdir);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Return true if this path has a trailing separator.
			///
			/// A trailing (string ending) separator is identified by EA_FILE_PATH_SEPARATOR_8/16  
			/// and EA_FILE_PATH_SEPARATOR_ALT_8/16. EA_FILE_PATH_SEPARATOR_ALT_8/16 is applicable  
			/// only on some platforms.
			/// This function doesn't validate path correctnesss. For example, this
			/// function will return true if the path ends with two successive 
			/// separators, despite that being an ill-formed path.
			///
			/// Example return values:
			///     Call                                    Return value       
			///     --------------------------------------------------------
			///     GetHasTrailingSeparator("");            false
			///     GetHasTrailingSeparator("/");           true
			///     GetHasTrailingSeparator("////////");    true (despite the path being invalid)
			///     GetHasTrailingSeparator("/a/b/c/");     true
			///     GetHasTrailingSeparator("/a");          false
			///     GetHasTrailingSeparator("\");           true (Microsoft), false (others) (On unix this string represents a standalone file name)
			///     GetHasTrailingSeparator("/\");          true (Microsoft), false (others) (This is an invalid Microsoft path but a valid Unix path)
			//////////////////////////////////////////////////////////////////////////
			EAIO_API bool GetHasTrailingSeparator(const PathString8& path);
			EAIO_API bool GetHasTrailingSeparator(const PathString16& path);
			EAIO_API bool GetHasTrailingSeparator(const PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool GetHasTrailingSeparator(const PathStringW& path);
			#endif

			EAIO_API bool GetHasTrailingSeparator(PathString8::const_iterator  first, size_t nLength = kLengthNull);
			EAIO_API bool GetHasTrailingSeparator(PathString16::const_iterator first, size_t nLength = kLengthNull);
			EAIO_API bool GetHasTrailingSeparator(PathString32::const_iterator first, size_t nLength = kLengthNull);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool GetHasTrailingSeparator(PathStringW::const_iterator first, size_t nLength = kLengthNull);
			#endif


			//////////////////////////////////////////////////////////////////////////
			/// Add a trailing separator if there isn't one already.
			/// The separator is added in the case of an empty path.
			///
			/// Example behavior:
			///     Call                                    New value Unix      New value Microsoft      
			///     -----------------------------------------------------------------------------
			///     EnsureTrailingSeparator("");            "/"                 "\"
			///     EnsureTrailingSeparator("/");           "/"                 "/"
			///     EnsureTrailingSeparator("\");           "\/"                "\"
			///     EnsureTrailingSeparator("a/b");         "a/b/"              "a/b\"
			///     EnsureTrailingSeparator("//");          "//"                "//"
			///     EnsureTrailingSeparator("C:");          "C:/"               "C:\"
			///     EnsureTrailingSeparator("\a\");         "\a\/"              "\a\"
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8&  EnsureTrailingSeparator(PathString8& path);
			EAIO_API PathString16& EnsureTrailingSeparator(PathString16& path);
			EAIO_API PathString32& EnsureTrailingSeparator(PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& EnsureTrailingSeparator(PathStringW& path);
			#endif

			/// Returns true if a separator was added.
			EAIO_API bool EnsureTrailingSeparator(char*  pPath, size_t dirCapacity);
			EAIO_API bool EnsureTrailingSeparator(char16_t* pPath, size_t dirCapacity);
			EAIO_API bool EnsureTrailingSeparator(char32_t* pPath, size_t dirCapacity);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool EnsureTrailingSeparator(wchar_t* pPath, size_t dirCapacity);
			#endif



			//////////////////////////////////////////////////////////////////////////
			/// Remove any trailing separator from the path.
			/// Removes only a single separator if there happen to be two in a row.
			///
			/// Example behavior:
			///     Call                                   New value Unix     New value Microsoft      
			///     -----------------------------------------------------------------------------
			///     StripTrailingSeparator("");            ""                  ""
			///     StripTrailingSeparator("/");           ""                  ""
			///     StripTrailingSeparator("\");           "\"                 ""
			///     StripTrailingSeparator("//");          "/"                 "/"
			///     StripTrailingSeparator("\\");          "\\"                "\"
			///     StripTrailingSeparator("/\");          "/\"                "/"
			///     StripTrailingSeparator("\/");          "\"                 "\"
			///     StripTrailingSeparator("/a/");         "/a"                "/a"
			///     StripTrailingSeparator("/a\");         "/a\"               "/a"
			///
			//////////////////////////////////////////////////////////////////////////
			EAIO_API PathString8&  StripTrailingSeparator(PathString8& path);
			EAIO_API PathString16& StripTrailingSeparator(PathString16& path);
			EAIO_API PathString32& StripTrailingSeparator(PathString32& path);
			#if EA_WCHAR_UNIQUE
				EAIO_API PathStringW& StripTrailingSeparator(PathStringW& path);
			#endif

			EAIO_API void StripTrailingSeparator(char*  pPath, size_t nLength = kLengthNull);
			EAIO_API void StripTrailingSeparator(char16_t* pPath, size_t nLength = kLengthNull);
			EAIO_API void StripTrailingSeparator(char32_t* pPath, size_t nLength = kLengthNull);
			#if EA_WCHAR_UNIQUE
				EAIO_API void StripTrailingSeparator(wchar_t* pPath, size_t nLength = kLengthNull);
			#endif


		} // namespace Path

	} // namespac IO

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace IO
	{
		namespace Path
		{
			///////////////////////////////////////////////////////////////////
			// PathString8
			///////////////////////////////////////////////////////////////////

			inline
			PathString8::PathString8()
				: base_type()
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(const PathString8& ps)
				: base_type(ps)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(const char* path)
				: base_type(path)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(const char* path, size_type n)
				: base_type(path, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(size_type n, char c)
				: base_type(n, c)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(const char* first, const char* last)
				: base_type(first, last)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString8::PathString8(const PathString8& ps, size_type position, size_type n)
				: base_type(ps, position, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			const PathString8& PathString8::operator=(const PathString8& ps)
			{
				assign(ps.data(), ps.length());
				return *this;
			}

			inline
			const PathString8& PathString8::operator=(const char* path)
			{
				assign(path);
				return *this;
			}

			inline
			const PathString8& PathString8::operator=(const char c)
			{
				assign(1, c);
				return *this;
			}

			inline
			PathString8& PathString8::operator+=(const PathString8& ps)
			{
				return Join(*this, ps);
			}

			inline
			PathString8& PathString8::operator+=(const char* p)
			{
				base_type::operator+=(p);
				return *this;
			}

			inline
			PathString8& PathString8::operator+=(const char c)
			{
				base_type::operator+=(c);
				return *this;
			}



			///////////////////////////////////////////////////////////////////
			// PathString16
			///////////////////////////////////////////////////////////////////

			inline
			PathString16::PathString16()
				: base_type()
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(const PathString16& ps)
				: base_type(ps)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(const char16_t* path)
				: base_type(path)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(const char16_t* path, size_type n)
				: base_type(path, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(size_type n, char16_t c)
				: base_type(n, c)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(const char16_t* first, const char16_t* last)
				: base_type(first, last)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString16::PathString16(const PathString16& ps, size_type position, size_type n)
				: base_type(ps, position, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			const PathString16& PathString16::operator=(const PathString16& ps)
			{
				assign(ps.data(), ps.length());
				return *this;
			}

			inline
			const PathString16& PathString16::operator=(const char16_t* path)
			{
				assign(path);
				return *this;
			}

			inline
			const PathString16& PathString16::operator=(const char16_t c)
			{
				assign(1, c);
				return *this;
			}

			inline
			PathString16& PathString16::operator+=(const PathString16& ps)
			{
				return Join(*this, ps);
			}

			inline
			PathString16& PathString16::operator+=(const char16_t* p)
			{
				base_type::operator+=(p);
				return *this;
			}

			inline
			PathString16& PathString16::operator+=(const char16_t c)
			{
				base_type::operator+=(c);
				return *this;
			}



			///////////////////////////////////////////////////////////////////
			// PathString32
			///////////////////////////////////////////////////////////////////

			inline
			PathString32::PathString32()
				: base_type()
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(const PathString32& ps)
				: base_type(ps)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(const char32_t* path)
				: base_type(path)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(const char32_t* path, size_type n)
				: base_type(path, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(size_type n, char32_t c)
				: base_type(n, c)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(const char32_t* first, const char32_t* last)
				: base_type(first, last)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			PathString32::PathString32(const PathString32& ps, size_type position, size_type n)
				: base_type(ps, position, n)
			{
				#if EASTL_NAME_ENABLED
					get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
				#endif
			}

			inline
			const PathString32& PathString32::operator=(const PathString32& ps)
			{
				assign(ps.data(), ps.length());
				return *this;
			}

			inline
			const PathString32& PathString32::operator=(const char32_t* path)
			{
				assign(path);
				return *this;
			}

			inline
			const PathString32& PathString32::operator=(const char32_t c)
			{
				assign(1, c);
				return *this;
			}

			inline
			PathString32& PathString32::operator+=(const PathString32& ps)
			{
				return Join(*this, ps);
			}

			inline
			PathString32& PathString32::operator+=(const char32_t* p)
			{
				base_type::operator+=(p);
				return *this;
			}

			inline
			PathString32& PathString32::operator+=(const char32_t c)
			{
				base_type::operator+=(c);
				return *this;
			}


			#if EA_WCHAR_UNIQUE

				///////////////////////////////////////////////////////////////////
				// PathStringW
				///////////////////////////////////////////////////////////////////

				inline
				PathStringW::PathStringW()
					: base_type()
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const PathStringW& ps)
					: base_type(ps)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const wchar_t* path)
					: base_type(path)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const wchar_t* path, size_type n)
					: base_type(path, n)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(size_type n, wchar_t c)
					: base_type(n, c)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const wchar_t* first, const wchar_t* last)
					: base_type(first, last)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const PathStringW& ps, size_type position, size_type n)
					: base_type(ps, position, n)
				{
					#if EASTL_NAME_ENABLED
						get_allocator().set_name(EAIO_ALLOC_PREFIX "PathString");
					#endif
				}

				inline
				PathStringW::PathStringW(const PathString16& pSrcPath16)
				{
					const int nCharsNeeded = EA::StdC::Strlcpy((wchar_t*)NULL, &pSrcPath16[0], 0);
					resize(static_cast<eastl_size_t>(nCharsNeeded));
					EA::StdC::Strlcpy(&this->operator[](0), &pSrcPath16[0], static_cast<size_t>(nCharsNeeded + 1));
				}

				inline
				PathStringW::PathStringW(const PathString32& pSrcPath32)
				{
					const int nCharsNeeded = EA::StdC::Strlcpy((wchar_t*)NULL, &pSrcPath32[0], 0);
					resize(static_cast<eastl_size_t>(nCharsNeeded));
					EA::StdC::Strlcpy(&this->operator[](0), &pSrcPath32[0], static_cast<size_t>(nCharsNeeded + 1));
				}

				inline
				const PathStringW& PathStringW::operator=(const PathStringW& ps)
				{
					assign(ps.data(), ps.length());
					return *this;
				}

				inline
				const PathStringW& PathStringW::operator=(const wchar_t* path)
				{
					assign(path);
					return *this;
				}

				inline
				const PathStringW& PathStringW::operator=(const wchar_t c)
				{
					assign(1, c);
					return *this;
				}

				inline
				PathStringW& PathStringW::operator+=(const PathStringW& ps)
				{
					return Join(*this, ps);
				}

				inline
				PathStringW& PathStringW::operator+=(const wchar_t* p)
				{
					base_type::operator+=(p);
					return *this;
				}

				inline
				PathStringW& PathStringW::operator+=(const wchar_t c)
				{
					base_type::operator+=(c);
					return *this;
				}

			#endif


			////////////////////////////
			// StrEnd
			////////////////////////////

			inline PathString8::iterator StrEnd(PathString8::iterator it)
			{
				while(*it) 
					++it;
				return it;
			}

			inline PathString16::iterator StrEnd(PathString16::iterator it)
			{
				while(*it) 
					++it;
				return it;
			}

			inline PathString32::iterator StrEnd(PathString32::iterator it)
			{
				while(*it) 
					++it;
				return it;
			}


			#if EA_WCHAR_UNIQUE
				inline PathStringW::iterator StrEnd(PathStringW::iterator it)
				{
					while(*it) 
						++it;
					return it;
				}
			#endif


			////////////////////////////
			// StrEnd
			////////////////////////////

			inline PathString8::iterator StrEnd(PathString8::const_iterator it)
			{
				PathString8::iterator itEnd = const_cast<PathString8::iterator>(it);
				while(*itEnd) 
					++itEnd;
				return itEnd;
			}

			inline PathString16::iterator StrEnd(PathString16::const_iterator it)
			{
				PathString16::iterator itEnd = const_cast<PathString16::iterator>(it);
				while(*itEnd)
					++itEnd;
				return itEnd;
			}

			inline PathString32::iterator StrEnd(PathString32::const_iterator it)
			{
				PathString32::iterator itEnd = const_cast<PathString32::iterator>(it);
				while(*itEnd)
					++itEnd;
				return itEnd;
			}

			#if EA_WCHAR_UNIQUE
				inline PathStringW::iterator StrEnd(PathStringW::const_iterator it)
				{
					PathStringW::iterator itEnd = const_cast<PathStringW::iterator>(it);
					while(*itEnd)
						++itEnd;
					return itEnd;
				}
			#endif


			////////////////////////////
			// GetDirectoryString
			////////////////////////////

			inline
			EAIO_API PathString8 GetDirectoryString(PathString8::const_iterator first,
													PathString8::const_iterator last)
			{
				return PathString8(first, GetFileName(first, last));
			}

			inline
			EAIO_API PathString16 GetDirectoryString(PathString16::const_iterator first,
													 PathString16::const_iterator last)
			{
				return PathString16(first, GetFileName(first, last));
			}

			inline
			EAIO_API PathString32 GetDirectoryString(PathString32::const_iterator first,
													 PathString32::const_iterator last)
			{
				return PathString32(first, GetFileName(first, last));
			}


			#if EA_WCHAR_UNIQUE
				inline
				EAIO_API PathStringW GetDirectoryString(PathStringW::const_iterator first,
														 PathStringW::const_iterator last)
				{
					return PathStringW(first, GetFileName(first, last));
				}
			#endif



			////////////////////////////
			// GetFileNameString
			////////////////////////////

			inline
			EAIO_API PathString8 GetFileNameString(PathString8::const_iterator first,
												   PathString8::const_iterator last)
			{
				if(last == kEndAuto8) 
					last = StrEnd(first);

				return PathString8(GetFileName(first, last), last);
			}

			inline
			EAIO_API PathString16 GetFileNameString(PathString16::const_iterator first,
													PathString16::const_iterator last)
			{
				if(last == kEndAuto16) 
					last = StrEnd(first);

				return PathString16(GetFileName(first, last), last);
			}

			inline
			EAIO_API PathString32 GetFileNameString(PathString32::const_iterator first,
													PathString32::const_iterator last)
			{
				if(last == kEndAuto32) 
					last = StrEnd(first);

				return PathString32(GetFileName(first, last), last);
			}


			#if EA_WCHAR_UNIQUE
				inline
				EAIO_API PathStringW GetFileNameString(PathStringW::const_iterator first,
														PathStringW::const_iterator last)
				{
					if(last == kEndAutoW) 
						last = StrEnd(first);

					return PathStringW(GetFileName(first, last), last);
				}
			#endif


			////////////////////////////
			// GetFileExtensionString
			////////////////////////////

			inline
			EAIO_API PathString8 GetFileExtensionString(PathString8::const_iterator first,
														PathString8::const_iterator last)
			{
				if(last == kEndAuto8) 
					last = StrEnd(first);

				return PathString8(GetFileExtension(first, last), last);
			}

			inline
			EAIO_API PathString16 GetFileExtensionString(PathString16::const_iterator first,
														 PathString16::const_iterator last)
			{
				if(last == kEndAuto16) 
					last = StrEnd(first);

				return PathString16(GetFileExtension(first, last), last);
			}

			inline
			EAIO_API PathString32 GetFileExtensionString(PathString32::const_iterator first,
														 PathString32::const_iterator last)
			{
				if(last == kEndAuto32) 
					last = StrEnd(first);

				return PathString32(GetFileExtension(first, last), last);
			}


			#if EA_WCHAR_UNIQUE
				inline
				EAIO_API PathStringW GetFileExtensionString(PathStringW::const_iterator first,
															 PathStringW::const_iterator last)
				{
					if(last == kEndAutoW) 
						last = StrEnd(first);

					return PathStringW(GetFileExtension(first, last), last);
				}
			#endif

		} // namespace Path

	} // namespac IO

} // namespace EA


#endif // Header include guard














