/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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


#ifndef EASTL_FIXEDSTRING_H
    #include <EASTL/fixed_string.h>
#endif
#include <EAIO/internal/Config.h>
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
        ///   -- char8_t strings are presumed to be UTF8-encoded. char16_t strings
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
            static const size_t kNominalPathLength = 96;


            /// PathStringBase
            typedef eastl::fixed_string<char16_t, kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBase16;
            typedef eastl::fixed_string<char8_t,  kNominalPathLength, true, Allocator::EAIOPathStringCoreAllocator> PathStringBase8;


            /// kEndAuto16
            ///
            /// Refers to a string ending with '\0'. If you pass kEndAuto16 to a function, 
            /// that means that you want the end of the associated string to be a 0 char.
            ///
            static const PathStringBase16::iterator kEndAuto16 = NULL;
            static const PathStringBase8::iterator  kEndAuto8  = NULL;
            #if EAIO_BACKWARDS_COMPATIBILITY
                static const PathStringBase16::iterator kEndAuto   = NULL; // For backward compatibility.
            #endif


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
                PathString8(const char8_t* path);
                PathString8(const char8_t* path, size_type n);
                PathString8(size_type n, char8_t c);
                PathString8(const char8_t* first, const char8_t* last);
                PathString8(const PathString8& ps, size_type position, size_type n = base_type::npos);

                const PathString8& operator=(const PathString8& ps);
                const PathString8& operator=(const char8_t* path);
                const PathString8& operator=(const char8_t c);

                PathString8& operator+=(const PathString8& ps);
                PathString8& operator+=(const char8_t* p);
                PathString8& operator+=(const char8_t c);
            };




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
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator FindComponentFwd(PathString16::iterator first,
                                                             PathString16::iterator last = kEndAuto16);

            EAIO_API PathString8::iterator FindComponentFwd(PathString8::iterator first,
                                                            PathString8::iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Const version of above.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator FindComponentFwd(PathString16::const_iterator first,
                                                                   PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8::const_iterator FindComponentFwd(PathString8::const_iterator first,
                                                                  PathString8::const_iterator last = kEndAuto8);


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
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator FindComponentRvs(PathString16::iterator first,
                                                             PathString16::iterator last = kEndAuto16);

            EAIO_API PathString8::iterator FindComponentRvs(PathString8::iterator first,
                                                            PathString8::iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Const version of above.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator FindComponentRvs(PathString16::const_iterator first,
                                                                   PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8::const_iterator FindComponentRvs(PathString8::const_iterator first,
                                                                  PathString8::const_iterator last = kEndAuto8);


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
            ///     PathString16 path16(L"C:/baloney/salami/");
            ///
            ///     GetPathComponentStart(begin, end, 0) ==> "C:/baloney/salami/"
            ///     GetPathComponentStart(begin, end, 1) ==> "baloney/salami/"
            ///     GetPathComponentStart(begin, end, 2) ==> "salami/"
            ///
            ///     path16.assign(L"C:\\baloney\\salami\\");
            ///
            ///     GetPathComponentStart(begin, end, 0) ==> "C:\balony\salami\"
            ///     GetPathComponentStart(begin, end, 1) ==> "baloney\salami"
            ///     GetPathComponentStart(begin, end, 2) ==> "salami\"
            ///
            ///     path16.assign(L"/C:/baloney/salami/");
            ///
            ///     GetPathComponentStart(begin, end, 0) ==> "/C:/baloney/salami/"
            ///     GetPathComponentStart(begin, end, 1) ==> "C:/baloney/salami/"
            ///     GetPathComponentStart(begin, end, 2) ==> "baloney/salami/"
            ///
            ///     path16.assign(L"/usr/local/foo/");
            ///
            ///     GetPathComponentStart(begin, end, 0) ==> "/usr/local/foo/"
            ///     GetPathComponentStart(begin, end, 1) ==> "usr/local/foo/"
            ///     GetPathComponentStart(begin, end, 2) ==> "local/foo/"
            ///
            ///     path16.assign(L"usr/local/foo/");
            ///
            ///     GetPathComponentStart(begin, end, 0) ==> "usr/local/foo/"
            ///     GetPathComponentStart(begin, end, 1) ==> "local/foo/"
            ///     GetPathComponentStart(begin, end, 2) ==> "foo/"
            ///
            ///     GetPathComponentStart(begin, end, -1) ==> "foo/"
            ///     GetPathComponentStart(begin, end, -2) ==> "local/foo/"
            ///
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator GetPathComponentStart(PathString16::iterator first,
                                                                  PathString16::iterator last,
                                                                  int32_t nIndex);

            EAIO_API PathString8::iterator GetPathComponentStart(PathString8::iterator first,
                                                                 PathString8::iterator last,
                                                                 int32_t nIndex);


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
            ///     PathString16 path16(L"/usr/local/foo/bar/");
            ///
            ///     GetPathComponentEnd(begin, end, 0) ==> "/usr/local/foo/bar/"    // Zero-th component is "/"
            ///     GetPathComponentEnd(begin, end, 1) ==> "/local/foo/bar/"        // First component is "usr/"
            ///     GetPathComponentEnd(begin, end, 2) ==> "/foo/bar/"              // Second component is "local/"
            ///
            ///     GetPathComponentEnd(begin, end, -1) ==> "/"                     // First from end is "bar/" ==> "/"
            ///     GetPathComponentEnd(begin, end, -2) ==> "/bar/"                 // Second from end is "foo/bar/" ==> "/bar/"
            ///
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator GetPathComponentEnd(PathString16::iterator first,
                                                                PathString16::iterator last,
                                                                int32_t nIndex);

            EAIO_API PathString8::iterator GetPathComponentEnd(PathString8::iterator first,
                                                               PathString8::iterator last,
                                                               int32_t nIndex);


            //////////////////////////////////////////////////////////////////////////
            /// Returns the directory as a string. 
            /// Directory always ends with a path separator.
            /// If no directory is present then the return value will be empty.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16 GetDirectoryString(PathString16::const_iterator first,
                                                     PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8 GetDirectoryString(PathString8::const_iterator first,
                                                    PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns the file name (including any extension) as a string. 
            /// Directory always ends with a path separator.
            /// If no directory is present then the return value will be empty.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16 GetFileNameString(PathString16::const_iterator first,
                                                    PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8 GetFileNameString(PathString8::const_iterator first,
                                                   PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Return the character immediately following the last directory separator.
            /// Thus this returns the file name (including any extension). 
            /// Returns path.end() if not found.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator GetFileName(const PathString16& path);

            EAIO_API PathString8::const_iterator GetFileName(const PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Return the character immediately following the last directory separator.
            /// Returns a pointer to the terminating NULL if not found.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator GetFileName(PathString16::const_iterator first,
                                                        PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8::iterator GetFileName(PathString8::const_iterator first,
                                                       PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns the file extension (including '.' char) as a string. 
            /// The extension is defined beginning with the last '.' found in the file 
            /// name. If no '.' is found in the file name then the return value is empty.
            /// If no directory is present then the return value will be empty.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16 GetFileExtensionString(PathString16::const_iterator first,
                                                         PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8 GetFileExtensionString(PathString8::const_iterator first,
                                                        PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns a pointer to the file extension (including the '.' character)
            /// or a pointer to the path.end() if there is none.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator GetFileExtension(const PathString16& path);

            EAIO_API PathString8::const_iterator GetFileExtension(const PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Returns a pointer to the file extension (including the '.' character)
            /// or a pointer to the end of the string if there is none.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator GetFileExtension(PathString16::const_iterator first,
                                                             PathString16::const_iterator last = kEndAuto16);

            EAIO_API PathString8::iterator GetFileExtension(PathString8::const_iterator first,
                                                            PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns a pointer to the character immediately following the separator
            /// that divides the drive letter sequence or UNC path from the rest of the path.
            /// If there is none, then the start of the string is returned.
            /// Example use:
            ///     "C:\a\b.c" => "\a\b.c"
            ///     "\\Computer\Directory\FileName" => "\Directory\FileName"
            ///     "\\?\UNC\Computer\Directory\FileName" => "\Computer\Directory\FileName" (this is "long UNC")
            ///     "\\?\C:\a\b.c" => "\a\b.c"      (or should this return C:\a\b.c)
            ///     "/a/b/c" => "/a/b/c"
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator GetLocalRoot(const PathString16& path);

            EAIO_API PathString8::const_iterator GetLocalRoot(const PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Returns a pointer to the character immediately following the separator
            /// that divides the drive letter or UNC path from the rest of the path.
            /// If there is none, then the start of the string is returned.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::iterator GetLocalRoot(PathString16::iterator first,
                                                         PathString16::iterator last = kEndAuto16);

            EAIO_API PathString8::iterator GetLocalRoot(PathString8::iterator first,
                                                        PathString8::iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Does a simple path concatenation. Modifies the destination buffer
            /// to contain dst + '/' + suffix, unless the suffix is a non-relative
            /// path, in which case it will contain simply suffix.
            /// Does not normalize the path.
            ///
            /// The return value is the destination path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Append(PathString16& dst,
                                          PathString16::const_iterator suffix_first,
                                          PathString16::const_iterator suffix_last = kEndAuto16);

            EAIO_API PathString8& Append(PathString8& dst,
                                         PathString8::const_iterator suffix_first,
                                         PathString8::const_iterator suffix_last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Does a simple path concatenation. Modifies the destination buffer
            /// to contain dst + '/' + suffix, unless the suffix is a non-relative
            /// path, in which case it will contain simply suffix.
            /// Does not normalize the path.
            ///
            /// The return value is the destination path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Append(PathString16& dst, const PathString16& suffix);

            EAIO_API PathString8& Append(PathString8& dst, const PathString8& suffix);


            //////////////////////////////////////////////////////////////////////////
            /// Does a path concatenation and resolution. Collapses all relative path
            /// specifiers and normalizes the path. (Essentially this is just
            /// a convenient wrapper for Append() + Normalize().)
            ///
            /// The return value is the destination path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Join(PathString16& dst,
                                        PathString16::const_iterator suffix_first,
                                        PathString16::const_iterator suffix_last = kEndAuto16);

            EAIO_API PathString8& Join(PathString8& dst,
                                       PathString8::const_iterator suffix_first,
                                       PathString8::const_iterator suffix_last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Does a path concatenation and resolution. Collapses all relative path
            /// specifiers and normalizes the path. (Essentially this is just
            /// a convenient wrapper for Append() + Normalize().)
            ///
            /// The return value is the destination path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Join(PathString16& dst, const PathString16& suffix);

            EAIO_API PathString8& Join(PathString8& dst, const PathString8& suffix);


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
            EAIO_API void Split(const PathString16& path, PathString16* pDrive, PathString16* pDirectory, PathString16* pFileName, PathString16* pFileExtension);

            EAIO_API void Split(const PathString8& path, PathString8* pDrive, PathString8* pDirectory, PathString8* pFileName, PathString8* pFileExtension);


            //////////////////////////////////////////////////////////////////////////
            /// Truncates the path at the start of the Nth component. 
            /// If N is less than 0, then it truncates the Nth component from the end.
            /// Retunrs iterator to new end.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16::const_iterator TruncateComponent(PathString16& path, int nIndex);

            EAIO_API PathString8::const_iterator TruncateComponent(PathString8& path, int nIndex);


            //////////////////////////////////////////////////////////////////////////
            /// Collapse all relative path specifiers and convert all path separators
            /// to the canonical type for this platform.
            /// Returns the input path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Normalize(PathString16& path);

            EAIO_API PathString8& Normalize(PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Collapse all relative path specifiers, but do not convert separators.
            /// Returns the input path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& Simplify(PathString16& path);

            EAIO_API PathString8& Simplify(PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Convert separators, but do not simplify.
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
            EAIO_API PathString16& Canonicalize(PathString16& path, char16_t separator = EA::IO::kFilePathSeparator16);

            EAIO_API PathString8& Canonicalize(PathString8& path, char8_t separator = EA::IO::kFilePathSeparator8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns true if the specified path is a relative (not absolute) path.
            /// A relative path is one that starts with a directory name (no leading
            /// slash) or a relative path specifier.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool IsRelative(PathString16::const_iterator first,
                                     PathString16::const_iterator last = kEndAuto16);

            EAIO_API bool IsRelative(PathString8::const_iterator first,
                                     PathString8::const_iterator last = kEndAuto8);


            //////////////////////////////////////////////////////////////////////////
            /// Returns true if the specified path is a relative (not absolute) path.
            /// A relative path is one that starts with a directory name (no leading
            /// slash) or a relative path specifier.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool IsRelative(const PathString16& path);

            EAIO_API bool IsRelative(const PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Compare two paths.
            /// If either pPath1 or pPath2 is relative, they will be resolved relative
            /// to pBase before being compared. (Note that this does not actually
            /// concatenate them -- it merely acts as if it did.)
            /// Returns -1, 0 or 1 depending on the result of the comparison.
            /// If either path is invalid, it will consider that path to be of higher
            /// collating order than any possible valid path.
            ///
            /// Note: In order for the comparison to be meaningful, both paths must
            /// either be absolute or relative to the same base.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API int Compare(const PathString16& a, const PathString16& b);

            EAIO_API int Compare(const PathString8& a, const PathString8& b);

            EAIO_API int Compare(PathString16::const_iterator a, PathString16::const_iterator a_end,
                                 PathString16::const_iterator b, PathString16::const_iterator b_end);

            EAIO_API int Compare(PathString8::const_iterator a, PathString8::const_iterator a_end,
                                 PathString8::const_iterator b, PathString8::const_iterator b_end);

            //////////////////////////////////////////////////////////////////////////
            /// Compute the relative path between two absolute paths.
            ///
            /// Given two absolute paths, returns a path which represents the relative
            /// relationship between them (may return an absolute path if there is
            /// no commonality.)
            ///
            /// The result will be the relative path which, when concatenate onto
            /// pPath1 yields pPath2.
            ///
            /// Note: Both paths must be in simplified form.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& ComputeRelative(PathString16& result,
                                                   const PathString16& src,
                                                   const PathString16& target);

            EAIO_API PathString8& ComputeRelative(PathString8& result,
                                                  const PathString8& src,
                                                  const PathString8& target);


            //////////////////////////////////////////////////////////////////////////
            /// Returns true of pSubDirectory is a sub directory of pDirectory. 
            /// If either path is relative, they will be resolved relative to 
            /// pBasePath before the paths are compared.
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
            /// Example return values:
            ///     Call                                          Return value       Notes
            ///     ---------------------------------------------------------------------------------------
            ///     IsSubdirectory("/a/b/c/", "/a/b/c/d/");       true               This is the basic true case.
            ///     IsSubdirectory("/a/b/c/", "/a/b/c/");         true               Both directories are the same.
            ///     IsSubdirectory("/a/b/c",  "/a/b/c/d");        true               Parent input directory doesn't end with '/'.
            ///     IsSubdirectory("/a/b/c",  "/a/b/c");          true               Neither input directory ends with '/'.
            ///     IsSubdirectory("/a/b/c",  "/a/b/c/");         true               Child input directory doesn't end with '/'.
            ///     IsSubdirectory("", "");                       true               Empty parent always returns true.
            ///     IsSubdirectory("", "a");                      true               Empty parent always returns true.
            ///     IsSubdirectory("/a/b/c",  "/a/b/cd");         false              Child is not subdirectory of parent.
            ///     IsSubdirectory("/a/b/c/", "/a/b/");           false              Child is not subdirectory of parent.
            ///     IsSubdirectory("/a/b/c/", "/c/b/");           false              Child is not subdirectory of parent.
            ///     IsSubdirectory("/a/b/c",  "");                false              Nonempty parent, empty child.
            ///
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool IsSubdirectory(const PathString16& dir, const PathString16& subdir);

            EAIO_API bool IsSubdirectory(const PathString8& dir, const PathString8& subdir);


            //////////////////////////////////////////////////////////////////////////
            /// Return true if this path has a trailing separator
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool GetHasTrailingSeparator(const PathString16& path);

            EAIO_API bool GetHasTrailingSeparator(const PathString8& path);


            //////////////////////////////////////////////////////////////////////////
            /// Return true if this path has a trailing separator.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool GetHasTrailingSeparator(PathString16::const_iterator first, size_t nLength = kLengthNull);

            EAIO_API bool GetHasTrailingSeparator(PathString8::const_iterator first, size_t nLength = kLengthNull);


            //////////////////////////////////////////////////////////////////////////
            /// Add a trailing separator if there isn't one already.
            /// The separator is added in the case of an empty path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& EnsureTrailingSeparator(PathString16& path);

            EAIO_API PathString8& EnsureTrailingSeparator(PathString8& path);

            //////////////////////////////////////////////////////////////////////////
            /// Add a trailing separator if there isn't one already. 
            /// Returns true if a separator was added.
            /// The separator is added in the case of an empty path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API bool EnsureTrailingSeparator(char16_t* pDirName, size_t nMaxPermittedLength);

            EAIO_API bool EnsureTrailingSeparator(char8_t* pDirName, size_t nMaxPermittedLength);


            //////////////////////////////////////////////////////////////////////////
            /// Remove any trailing separator from the path.
            //////////////////////////////////////////////////////////////////////////
            EAIO_API PathString16& StripTrailingSeparator(PathString16& path);

            EAIO_API PathString8& StripTrailingSeparator(PathString8& path);

            EAIO_API void StripTrailingSeparator(char16_t* pPath, size_t nLength = kLengthNull);

            EAIO_API void StripTrailingSeparator(char8_t* pPath, size_t nLength = kLengthNull);


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
            // PathString16
            ///////////////////////////////////////////////////////////////////

            inline
            PathString16::PathString16()
                : base_type()
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(const PathString16& ps)
                : base_type(ps)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(const char16_t* path)
                : base_type(path)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(const char16_t* path, size_type n)
                : base_type(path, n)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(size_type n, char16_t c)
                : base_type(n, c)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(const char16_t* first, const char16_t* last)
                : base_type(first, last)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString16::PathString16(const PathString16& ps, size_type position, size_type n)
                : base_type(ps, position, n)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
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
            // PathString8
            ///////////////////////////////////////////////////////////////////

            inline
            PathString8::PathString8()
                : base_type()
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(const PathString8& ps)
                : base_type(ps)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(const char8_t* path)
                : base_type(path)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(const char8_t* path, size_type n)
                : base_type(path, n)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(size_type n, char8_t c)
                : base_type(n, c)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(const char8_t* first, const char8_t* last)
                : base_type(first, last)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            PathString8::PathString8(const PathString8& ps, size_type position, size_type n)
                : base_type(ps, position, n)
            {
                #if EASTL_NAME_ENABLED
                    mAllocator.set_name(EAIO_ALLOC_PREFIX "PathString");
                #endif
            }

            inline
            const PathString8& PathString8::operator=(const PathString8& ps)
            {
                assign(ps.data(), ps.length());
                return *this;
            }

            inline
            const PathString8& PathString8::operator=(const char8_t* path)
            {
                assign(path);
                return *this;
            }

            inline
            const PathString8& PathString8::operator=(const char8_t c)
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
            PathString8& PathString8::operator+=(const char8_t* p)
            {
                base_type::operator+=(p);
                return *this;
            }

            inline
            PathString8& PathString8::operator+=(const char8_t c)
            {
                base_type::operator+=(c);
                return *this;
            }



            ///////////////////////////////////////////////////////////////////
            // Misc
            ///////////////////////////////////////////////////////////////////

            inline PathString16::iterator StrEnd(PathString16::iterator it)
            {
                while(*it) 
                    ++it;
                return it;
            }

            inline PathString8::iterator StrEnd(PathString8::iterator it)
            {
                while(*it) 
                    ++it;
                return it;
            }

            inline PathString16::iterator StrEnd(PathString16::const_iterator it)
            {
                PathString16::iterator itEnd = const_cast<PathString16::iterator>(it);
                while(*itEnd)
                    ++itEnd;
                return itEnd;
            }

            inline PathString8::iterator StrEnd(PathString8::const_iterator it)
            {
                PathString8::iterator itEnd = const_cast<PathString8::iterator>(it);
                while(*itEnd) 
                    ++itEnd;
                return itEnd;
            }


            inline
            EAIO_API PathString16 GetDirectoryString(PathString16::const_iterator first,
                                                     PathString16::const_iterator last)
            {
                return PathString16(first, GetFileName(first, last));
            }

            inline
            EAIO_API PathString8 GetDirectoryString(PathString8::const_iterator first,
                                                    PathString8::const_iterator last)
            {
                return PathString8(first, GetFileName(first, last));
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
            EAIO_API PathString8 GetFileNameString(PathString8::const_iterator first,
                                                   PathString8::const_iterator last)
            {
                if(last == kEndAuto8) 
                    last = StrEnd(first);

                return PathString8(GetFileName(first, last), last);
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
            EAIO_API PathString8 GetFileExtensionString(PathString8::const_iterator first,
                                                        PathString8::const_iterator last)
            {
                if(last == kEndAuto8) 
                    last = StrEnd(first);

                return PathString8(GetFileExtension(first, last), last);
            }


        } // namespace Path

    } // namespac IO

} // namespace EA


#endif // Header include guard














