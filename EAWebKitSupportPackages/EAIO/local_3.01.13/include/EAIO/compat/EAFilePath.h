///////////////////////////////////////////////////////////////////////////////
// EAFilePath.h
// 
// Copyright (c) 2003, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAFILEPATH_H
#define EAIO_EAFILEPATH_H


#ifndef INCLUDED_eabase_H
   #include <EABase/eabase.h>
#endif
#ifndef EAIO_EAFILEBASE_H
   #include <EAIO/EAFileBase.h>
#endif
#ifndef EASTL_STRING_H
   #include <EASTL/string.h>
#endif
#ifndef EAIO_EASTREAM_H
	#include <EAIO/EAStream.h>
#endif


namespace EA
{
	namespace IO
	{
		/// FilePath
		///
		/// The purpose of this class is to assist in constructing and deconstructing
		/// file paths in an easy and platform-independent way. It is common to want
		/// to take a file path and get just the directory from it. Another common 
		/// usage is to take a directory and file name and try appending different 
		/// extensions to the end of the file name.
		///
		/// This class implements path string and component construction and destruction
		/// and does not implement file, directory, and drive operations themselves.
		/// Please see the File and Directory IO modules for this functionality.
		/// Similarly, this class does not define constants such as kMaxPathLength
		/// and EA_FILE_PATH_SEPARATOR_8/16 as these constants are defined elsewhere 
		/// (in EAFileBase.h as of this writing).
		///
		/// This class allocates no memory at any time. It works on an internal buffer
		/// declared as a member of the class.
		///
		/// Instances of this class are not thread-safe. Use of an instance of this class 
		/// by multiple threads simultaneously must be protected by a synchronization 
		/// primitive such as a mutex.
		///
		/// Note that directory paths in this EAFile/EADirectory system are defined as 
		/// ending with a directory separator. This is done because it makes it simpler
		/// to work with directories and makes the specification of what a directory 
		/// path is be more concise.
		///
		/// See enum PathComponent in EAFileBase.h for definitions of each path component.
		///
		/// Note that this class includes the concept of a path 'drive'. This is a concept
		/// that is specific to some platform's paths. On operating systems that don't
		/// recognize the concept of drives (e.g. Unix), the drive component should 
		/// simply be left empty and not used. If you pass in a Unix path string and 
		/// ask FilePath what the drive is, you will get an empty string in return. 
		/// Under Windows, the drive consists of an alphabetic letter and a colon. 
		/// The alphabetic character is case-insensitive; "C:" and "c:" are equivalent.
		/// 
		/// Note: The FilePath class doesn't support the specification of paths via a
		/// "current working directory." If you want to supply a relative path to the 
		/// FilePath class, you need to call FullPath before passing the path to an
		/// instance of the FilePath class. This design is so in order to simplify the
		/// implementation of the FilePath class.
		///
		class EAIO_API FilePath
		{
		public:
			// Constructors
			FilePath();
			
			FilePath(const char*  pPath);
			FilePath(const char16_t* pPath);
			FilePath(const char*  pDrive, const char*  pDirectory, const char*  pFileName, const char*  pExtension);
			FilePath(const char16_t* pDrive, const char16_t* pDirectory, const char16_t* pFileName, const char16_t* pExtension);

			// Accessors
			const char16_t* GetPath                () const;
			const char16_t* GetFileNameAndExtension() const;
			const char16_t* GetExtension           () const;


			// reference accessors
			//
			// Return value is the required strlen for the buffer, which will be >= the 
			// buffer capacity if the capacity was not large enough.
			// If the buffer capacity is too small, nothing is written to the buffer.
			//
			size_t GetDrive                (char*  buf, size_t capacity = kMaxDriveLength    ) const;
			size_t GetDrive                (char16_t* buf, size_t capacity = kMaxDriveLength    ) const;
			size_t GetDirectory            (char*  buf, size_t capacity = kMaxDirectoryLength) const;
			size_t GetDirectory            (char16_t* buf, size_t capacity = kMaxDirectoryLength) const;
			size_t GetFileName             (char*  buf, size_t capacity = kMaxFileNameLength ) const;
			size_t GetFileName             (char16_t* buf, size_t capacity = kMaxFileNameLength ) const;
			size_t GetExtension            (char*  buf, size_t capacity = kMaxExtensionLength) const;
			size_t GetExtension            (char16_t* buf, size_t capacity = kMaxExtensionLength) const;

			size_t GetPath                 (char*  buf, size_t capacity = kMaxPathLength) const;
			size_t GetPath                 (char16_t* buf, size_t capacity = kMaxPathLength) const;
			size_t GetDriveAndDirectory    (char*  buf, size_t capacity = kMaxPathLength) const;
			size_t GetDriveAndDirectory    (char16_t* buf, size_t capacity = kMaxPathLength) const;
			size_t GetFileNameAndExtension (char*  buf, size_t capacity = kMaxPathLength) const;
			size_t GetFileNameAndExtension (char16_t* buf, size_t capacity = kMaxPathLength) const;

			// Mutators
			void   TrimToDriveAndDirectory();          //< Removes the filename and extension.
			void   TrimToRoot();                       //< Removes the directory, filename, and extension. Preserves a leading path separator on the directory, i.e. "c:/test.txt" -> "c:/"
			bool   RemoveLastDirectory();              //< Removes the last directory in the path; filename and extension are preserved. Returns false if there was no directory to remove.
			bool   AppendDirectory(const char* );   //< Adds a directory component to the path; filename and extension are preserved. Returns false if there is not enough room.
			bool   AppendDirectory(const char16_t*);   //< Adds a directory component to the path; filename and extension are preserved. Returns false if there is not enough room.

			bool   SetDrive                  (const char*  pDrive,                bool bInputIsPath = false);  // This function has no effect on non-Win32 file systems.
			bool   SetDrive                  (const char16_t* pDrive,                bool bInputIsPath = false);  // This function has no effect on non-Win32 file systems.
			bool   SetDirectory              (const char*  pDirectory,            bool bInputIsPath = false);
			bool   SetDirectory              (const char16_t* pDirectory,            bool bInputIsPath = false);
			bool   SetFileName               (const char*  pFileName,             bool bInputIsPath = false);
			bool   SetFileName               (const char16_t* pFileName,             bool bInputIsPath = false);
			bool   SetExtension              (const char*  pExtension,            bool bInputIsPath = false);
			bool   SetExtension              (const char16_t* pExtension,            bool bInputIsPath = false);
			bool   SetDriveAndDirectory      (const char*  pDriveAndDirectory,    bool bInputIsPath = false);
			bool   SetDriveAndDirectory      (const char16_t* pDriveAndDirectory,    bool bInputIsPath = false);
			bool   SetFileNameAndExtension   (const char*  pFileNameAndExtension, bool bInputIsPath = false);
			bool   SetFileNameAndExtension   (const char16_t* pFileNameAndExtension, bool bInputIsPath = false);
			bool   SetPath                   (const char*  pPath);
			bool   SetPath                   (const char16_t* pPath);
			bool   SetPath                   (const char*  pDrive, const char*  pDirectoryOnly, const char*  pFileNameOnly, const char*  pExtension);
			bool   SetPath                   (const char16_t* pDrive, const char16_t* pDirectoryOnly, const char16_t* pFileNameOnly, const char16_t* pExtension);
			void   Clear();

			// Comparison
			//
			// These functions return an integer in the same way that the C runtime library
			// returns an integer for strcmp. '< 0' means that we are less than argument.
			// '== 0' means that we are equal to the argument. '> 0' means we are greater
			// than the argument.
			//
			// Note that below some functions have a variable called 'bComponentwise'.
			// When set to true, it compares by individual components instead of by 
			// final path string. The result is that sorting of paths is more sensible 
			// for most cases. Consider doing a sort of a list containing the following paths:
			//    /a/b/c.txt
			//    /a/b/c/d.txt
			// In the first path, 'c' is part of the file name, whereas in the second 
			// path it is part of a directory name. Comparing component-wise will put 
			// all equivalent directories next to each other and not have other directories 
			// interspersed in-between.

			int   CompareDrive                (const FilePath& path, FileSystem fileSystemType = kFileSystemDefault) const;
			int   CompareDirectory            (const FilePath& path, FileSystem fileSystemType = kFileSystemDefault) const;
			int   CompareFileName             (const FilePath& path, FileSystem fileSystemType = kFileSystemDefault) const;
			int   CompareExtension            (const FilePath& path, FileSystem fileSystemType = kFileSystemDefault) const;
			int   ComparePath                 (const FilePath& path, bool bComponentwise = false, FileSystem fileSystemType = kFileSystemDefault) const;
			int   CompareFileNameAndExtension (const FilePath& path, bool bComponentwise = false, FileSystem fileSystemType = kFileSystemDefault) const;
			int   CompareDriveAndDirectory    (const FilePath& path, bool bComponentwise = false, FileSystem fileSystemType = kFileSystemDefault) const;

			bool  operator==                  (const FilePath& rhs) const { return ComparePath(rhs) == 0; }
			bool  operator!=                  (const FilePath& rhs) const { return ComparePath(rhs) != 0; }
			bool  operator<                   (const FilePath& rhs) const { return ComparePath(rhs)  < 0; }
			bool  operator<=                  (const FilePath& rhs) const { return ComparePath(rhs) <= 0; }
			bool  operator>                   (const FilePath& rhs) const { return ComparePath(rhs)  > 0; }
			bool  operator>=                  (const FilePath& rhs) const { return ComparePath(rhs) >= 0; }

		protected:
			enum PathPos
			{
				kPosDriveStart,
				kPosDirectoryStart,
				kPosFileNameStart,
				kPosExtensionStart,
				kPosEnd,
				kPosCount
			};

			static int  CompareSubstrings(const char16_t* pStart1, const char16_t* pEnd1, const char16_t* pStart2, const char16_t* pEnd2, FileSystem nFilesystemType);
			static int  CompareSubstrings(PathPos start, PathPos end, const char16_t* p1, const char16_t* p2, FileSystem nFilesystemType, bool bComponentwise);

			static size_t SafeSubstringCopy(const char16_t* begin, const char16_t* end, char16_t* dst, size_t capacity);

			bool SafeReplace(char16_t* replaceBegin, char16_t* replaceEnd, const char16_t* srcBegin, const char16_t* srcEnd, char16_t prefixChar, char16_t suffixChar);
			bool ReplacePathComponent(PathPos start, PathPos end, const char*  src, bool bSrcIsPath);
			bool ReplacePathComponent(PathPos start, PathPos end, const char16_t* src, bool bSrcIsPath);

		protected: // Data members
			char16_t mPath[kMaxPathLength * 2];

		}; // class FilePath





		/// GetHasTrailingPathSeparator
		/// Returns true if the given input path string ends with a trailing path separator.
		EAIO_API bool GetHasTrailingPathSeparator(const char*  pDirectory, size_t nDirectoryLength = kLengthNull);
		EAIO_API bool GetHasTrailingPathSeparator(const char16_t* pDirectory, size_t nDirectoryLength = kLengthNull);

		/// EnsureTrailingPathSeparator
		/// Given an input directory string, this function ensures that it ends with a trailing 
		/// path separator. If it doesn't then one will be appended. The input nDirectoryLength
		/// specifies the strlen of the input directory or is -1 (kLengthNull) to indicate that 
		/// this function should determine that for itself. The return value is true if a separator 
		/// was appended and false if not needed.
		EAIO_API bool EnsureTrailingPathSeparator(char16_t* pDirectory, size_t nDirectoryLength = kLengthNull);
		EAIO_API bool EnsureTrailingPathSeparator(eastl::string16& sDirectory);

		/// RemoveTrailingPathSeparator
		/// Removes a trailing directory separator if one is present. Returns true if a separator
		/// was present and was removed. The input nDirectoryLength specifies the strlen of the 
		/// input directory or is -1 (kLengthNull) to indicate that this function should determine 
		/// that for itself. 
		///
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
		///
		EAIO_API bool RemoveTrailingPathSeparator(char*  pDirectory, size_t nDirectoryLength = kLengthNull);
		EAIO_API bool RemoveTrailingPathSeparator(char16_t* pDirectory, size_t nDirectoryLength = kLengthNull);

		/// ConcatenatePathComponents
		/// If pPrefix is non-NULL, it is appended to the destination
		/// Then if the resulting path doesn't end in a path separator, one is appended to the destination.
		/// Then if pSuffix is non-NULL, it is appended to the destination.
		/// This is a common path-building utility function.
		/// Output parameter pDestination must be long enough for the resulting path. Usually this
		/// means that it must be at least kMaxPathLength is length.
		/// Returns Strlen of output parameter pDestination.
		EAIO_API size_t ConcatenatePathComponents(char16_t* pDestination, const char16_t* pPrefix, const char16_t* pSuffix);

		/// StripTrailingPathComponent
		/// Takes a path that possibly ends with a file name (or directory or extension) and strips it.
		/// Returns new strlen of path. The input pPath must be null-terminated and 
		/// the returned path will be null-terminated. If the input path is merely 
		/// a file name alone, the return value will be an empty string. Otherwise,
		/// the returned path will end with a file path separator (e.g. '/').
		/// If the given trailing component is not present, the path is not changed.
		/// As such, this function is also useful for telling if a path ends with a given component.
		/// In general, this function is useful for getting the directory that a file is in.
		EAIO_API size_t StripTrailingPathComponent(char16_t* pPath, PathComponent pathComponent);

		/// SplitPath
		/// Given an input path, returns drive (if applicable to the platform), directory, 
		/// file name, and file name extension. Destination pointers must point to enough 
		/// space (e.g. kMaxDirectoryNameLength) to hold a maximum possible length value.
		/// Any of the parameters pDrive, pDirectoryOnly, pFileNameOnly, or pExtension can
		/// be NULL, in which case nothing is written to them.
		/// This is a generic function which can break apart a path into desired components.
		EAIO_API void SplitPath(const char16_t* pPath, char16_t* pDrive, char16_t* pDirectoryOnly, 
						char16_t* pFileNameOnly, char16_t* pExtension, FileSystem fileSystemType = kFileSystemDefault);

		/// SplitPathPtrs
		/// Given an input path, returns pointers to the start of the drive, directory,
		/// file name, and file name extension sections. All pointers are always valid
		/// and ordered, although some pointers may be equal to others or point to the
		/// null terminator if components are absent.
		EAIO_API void SplitPathPtrs(char16_t       *pPath, char16_t       *&pDirStart, char16_t       *&pFileStart, char16_t       *&pExtStart);
		EAIO_API void SplitPathPtrs(const char16_t *pPath, const char16_t *&pDirStart, const char16_t *&pFileStart, const char16_t *&pExtStart);

		EAIO_API void SplitPathPtrs(char       *pPath, char       *&pDirStart, char       *&pFileStart, char       *&pExtStart);
		EAIO_API void SplitPathPtrs(const char *pPath, const char *&pDirStart, const char *&pFileStart, const char *&pExtStart);


		/// SplitDirectory
		/// Given an input directory, returns the individual directories which make up the path.
		/// Returns the number of array entries that were written. If the input pDirectoryNames 
		/// is NULL, then the return value is the number of entries that would be written if the 
		/// array was sufficiently large.
		/// The input nDirectoryNamesCount specifies the number of entries in pDirectoryNameArray
		/// but is unused if pDirectoryNames is NULL.
		/// This function is useful for taking a directory path and walking up it (towards the root)
		/// progressively; You would do this by passing in 2 for the directory names count.
		///
		/// Given an input directory such as:
		///    /Directory0/Directory1/Directory2/Directory3/
		/// This function reads the separate directory componenets and puts each one
		/// into the array 'pDirectoryNames' in order from right to left. 
		/// Returns the number of array entries that were written. If the input 
		/// pDirectoryNames is NULL, then the return value is the number of entries
		/// that would be written if the array was sufficiently large.
		///
		/// The way this returns directories may seem backwards, but it is more convenient 
		/// this way. For the above example, the array contents would become:
		///    Directory3/         (at array index 0)
		///    Directory2/
		///    Directory1/
		///    Directory0/
		///    /                   (at array index 4)
		///
		/// Likewise with Win32, "C:/Dir0/Dir1/" gets parsed as:
		///    Dir1/
		///    Dir0/
		///    C:/
		///
		/// If you pass in a 'nDirectoryNamesCount' that is less than the actual number
		/// of directories, it will work its way from the right side leftward. Thus, if
		/// in the above example you pass in '2' for a count, you get this array result:
		///    Directory3/
		///    /Directory0/Directory1/Directory2/
		///
		/// If you pass in "Dir0/Dir1/"(no leading directory), it gets parsed as:
		///    Dir1/
		///    Dir0/
		///
		/// This allows you to just pull off the last directory of two if you wish.
		///
		/// One thing to watch out for when using this code is that you might want to
		/// pass in a directory name array with something like 32 or more entries.
		/// This isn't a problem. However, if you declare the array as a local
		/// variable like this:
		///     char16_t pDirectoryNames[48][kMaxDirectoryNameLength];
		/// you risk blowing the stack. This can happen on some modern operating systems,
		/// including Win32 under some circumstances. You need to allocate the array
		/// dynamically, especially if you want to be portable. How do you do this?
		/// One simple way to is to this:
		///     struct Temp{
		///        char16_t pDirectoryNames[48][kMaxDirectoryNameLength];
		///     };
		///     Temp* pTemp = new Temp;
		/// This way you can allocate it with operator new.
		/// 
		EAIO_API size_t SplitDirectory(const char16_t* pDirectory, char16_t pDirectoryNameArray[][kMaxPathLength], 
								size_t nDirectoryNamesCount, FileSystem fileSystemType = kFileSystemDefault);

		/// SplitDirectory
		/// Splits the directory path into two strings, the path up till the last directory
		/// and the last directory. Both parts will end with a path separator.
		/// The ouput pFirstPath and pLastPart buffers must refer to be least kMaxDirectoryLength.
		/// For example, with Win32 the following input will be returned as the following output:
		///    C:\a\b\c\d\   Input
		///    C:\a\b\c\     First part
		///    d\            Second part
		/// Returns false if input path is empty, but otherwise never fails.
		/// If the input path has only one directory, then it will be copied to pLastPart and pFirstPart
		/// will be returned empty.
		/// If the pFirstPart or pLastPart parameters are NULL, that parameter will not be written to.
		EAIO_API bool SplitDirectory(const char16_t* pDirectoryInput, char16_t* pFirstPart, char16_t* pLastPart);


		/// MakePath
		/// Given a drive (if applicable), directory, file name, and file name extension, this 
		/// function returns a complete path. Destination pointers must point to enough space to 
		/// hold a maximum possible length value (see kMaxDriveLength etc.)
		///
		/// Input parameters:
		///     char16_t* pPath         - pointer to buffer for constructed path
		///     char16_t* pDrive        - pointer to drive component, may or may not contain trailing ':'
		///     char16_t* pDirectory    - pointer to subdirectory component, may or may not include
		///                               leading and/or trailing '/' or '\' characters
		///     char16_t* pFileName     - pointer to file base name component
		///     char16_t* pExtension    - pointer to extension component, may or may not contain a leading '.'.
		///
		/// Output parameters:
		///      pPathResult            - pointer to constructed path name
		///
		/// This function expects paths to be in a format compatible with the operating system 
		/// being run. It is an error, for example, if we are running Unix and the path being 
		/// passed in has DOS driver letters in it.
		///
		/// We assume that the arguments are in the following form:
		///    Drive/Volume (which may be absent for Unix-style file paths):
		///        A    <or>
		///        A:
		///    DirectoryOnly:
		///        \top\next\last\     <or>
		///        /top/next/last/     <or>
		///        Either of the above forms with either/both the leading
		///        and trailing / or \ removed. Mixed use of '/' and '\' is
		///        also tolerated
		///    FileNameOnly:
		///        Any valid file name
		///    Extension:
		///        .extension    <or>
		///         extension
		///
		EAIO_API void MakePath(char16_t* pPathResult, const char16_t* pDrive, const char16_t* pDirectoryOnly, 
						const char16_t* pFileNameOnly, const char16_t* pExtension, FileSystem fileSystemType = kFileSystemDefault);


		/// FullPath
		/// Fixes a path that is implicitly relative and/or has specific relative path specifiers.
		/// Returns the input buffer or NULL upon failure.
		/// Destination pointer must point to enough space to hold a maximum possible length value (kMaxPathLength).
		/// If the input pBaseDirectory is NULL, the current default directory is specified.
		/// If you want the base directory to be nothing, then pass a valid empty string ("") as pBaseDirectory.
		EAIO_API char16_t* FullPath(char16_t* pPathResult, const char16_t* pPath, const char16_t* pBaseDirectory = NULL, FileSystem fileSystemType = kFileSystemDefault);

		/// IsFullPath
		/// Returns true if the given path argument is an absolute path and is not relative.
		EAIO_API bool IsFullPath(const char16_t* pPath, FileSystem fileSystemType = kFileSystemDefault);

		// IsPathRelative
		// Returns true if the input path has any relative path specifiers.
		EAIO_API bool IsPathRelative(const char16_t* pPath, FileSystem fileSystemType = kFileSystemDefault);

		/// IsPathRooted
		/// Returns true if the given path argument begins with an root filesystem component (i.e. C:\).
		EAIO_API bool IsPathRooted(const char16_t* pPath, FileSystem fileSystemType = kFileSystemDefault);

		/// ResolvePath
		/// Resolves a relative path into an absolute path using a base path.
		/// If the input path is already absolute, it simply returns a copy of the input argument.
		/// This does the same thing as FullPath except when the input pPath argument is already absolute.
		/// Destination pointer must point to enough space to hold a maximum possible length value (kMaxPathLength).
		/// If the input pBaseDirectory is NULL, the current default directory is specified.
		/// If you want the base directory to be nothing, then pass a valid empty string ("") as pBaseDirectory.
		/// Returns a pointer to pPathResult if the operation succeeded; returns NULL if the operation
		/// failed, as might happen if the required destination capacity is greater than kMaxPathLength.
		EAIO_API char16_t* ResolvePath(char16_t* pPathResult, const char16_t* pPath, const char16_t* pBaseDirectory, FileSystem fileSystemType = kFileSystemDefault);

		/// ComparePaths
		/// Return value is the same as with the standard C strcmp (string comparison) function.
		/// Takes into account things like case sensitivity, relative paths, Win32 aliases, and Win32 short file names.
		/// This function does not do "component-wise" comparison; use the FilePath class to do so.
		EAIO_API int ComparePaths(const char16_t* pPath1, const char16_t* pPath2, FileSystem fileSystemType = kFileSystemDefault);

		/// GetFileSystem
		/// Returns the file system for the given path.
		/// Some file systems have different limitations than others.
		EAIO_API FileSystem GetFileSystem(const char16_t* pPath);

		/// IsFileSystemCaseSensitive
		/// Returns true if the given file system is case-sensitive
		EAIO_API bool IsFileSystemCaseSensitive(FileSystem fileSystemType = kFileSystemDefault);

		/// IsPathValid
		/// Returns true if the given path is valid. The path can consist of a full
		/// directory and file path, just a full directory path, or just a file name.
		/// fileSystemType refers to the file system, which defaults to the system's
		/// standard file system. For Windows, valid systems include "FAT" and "NTFS".
		/// For Macintosh it would be "HFS". For Unix, it would be "UNIX". The valid
		/// list of file system types is: "UNIX", "HFS", "FAT", "HPFS", "NTFS", "UDF", "CDFS", "Joliet", "Rockridge"
		EAIO_API bool IsPathValid(const char16_t* pPath, FileSystem fileSystemType = kFileSystemDefault);

		/// MakePathValid
		/// Strips invalid characters from the input path or file name to make it 
		/// valid. The result path argument can be the same string as the input.
		/// Destination pointer must point to enough space to hold a maximum possible length value (kMaxPathLength).
		/// Returns true if the resulting file name (altered or otherwise) is of greater
		/// than zero length.
		EAIO_API bool MakePathValid(const char16_t* pPath, char16_t* pPathResult, FileSystem fileSystemType = kFileSystemDefault);

		/// IsValidFileNameChar
		/// Returns true if the input character can always be used in a path file name.
		/// You cannot use this function one-by-one on a file name to determine if the file
		/// name is valid, as there are special combination cases that make this so.
		EAIO_API bool IsValidFileNameChar(char16_t c, FileSystem fileSystemType = kFileSystemDefault);

		/// IsFileNameValid
		/// Returns true if the input file name is valid for the file system.
		EAIO_API bool IsFileNameValid(const char16_t* pName, FileSystem fileSystemType = kFileSystemDefault);

		/// MakeFileNameValid
		/// Fixes an improper file name by removing invalid characters as necessary.
		/// Returns true if the resulting file name (altered or otherwise) is of greater
		/// than zero length.
		EAIO_API bool MakeFileNameValid(const char16_t* pName, char16_t* pNameResult, FileSystem fileSystemType = kFileSystemDefault);

		/// IsValidDirectoryNameChar
		/// Returns true if the input character can always be used in a path directory.
		EAIO_API bool IsValidDirectoryNameChar(char16_t c, FileSystem fileSystemType = kFileSystemDefault);

		/// IsDirectoryNameValid
		/// Returns true if the input directory name is valid for the file system.
		/// This function takes a directory name alone and not a directory path.
		/// As such, the name is not required to end with a path separator.
		EAIO_API bool IsDirectoryNameValid(const char16_t* pName, FileSystem fileSystemType = kFileSystemDefault);

		/// MakeDirectoryNameValid
		/// Fixes an improper directory name by removing invalid characters as necessary.
		/// Returns true if the resulting file name (altered or otherwise) is of greater
		/// than zero length.
		/// Destination pointer must point to enough space to hold a maximum possible length value.
		/// This function takes a directory name alone and not a directory path.
		/// As such, the name is not required to end with a path separator.
		EAIO_API bool MakeDirectoryNameValid(const char16_t* pName, char16_t* pNameResult, FileSystem fileSystemType = kFileSystemDefault);

		/// CanonicalizeDirectory
		/// Makes sure all path separators are consistent and makes sure the directory
		/// ends with a path separator. This function is not a generic broken path-fixing
		/// function, so an invalid path specification yields undefined behaviour.
		/// Destination pointer must point to enough space to hold a maximum possible length value.
		/// Returns the result directory.
		/// The source and destination cannot point to the same location in memory.
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
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
		EAIO_API char16_t* CanonicalizeDirectory(const char16_t* pDirectory, char16_t* pDirectoryResult, char16_t pathSeparator = EA_FILE_PATH_SEPARATOR_16);

		/// IsSubdirectory
		/// Returns true if the input pPathSubdirectory is a child directory of the
		/// input pPathDirectory. Both arguments are expected to be full directory paths.
		/// The source and destination cannot point to the same location in memory.
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
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
		EAIO_API bool IsSubdirectory(const char16_t* pDirectory, const char16_t* pSubdirectory, FileSystem fileSystemType = kFileSystemDefault);




		///////////////////////////////////////////////////////////////////////
		// Helper functions
		///////////////////////////////////////////////////////////////////////

		/// GetFileName
		/// Return a pointer to the filename part of the path
		inline const char* GetFileName(const char* pPath)
		{
			const char* pDirStart;
			const char* pFileStart = NULL;
			const char* pExtStart;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pFileStart;
		}

		inline const char16_t* GetFileName(const char16_t* pPath)
		{
			const char16_t* pDirStart;
			const char16_t* pFileStart = NULL;
			const char16_t* pExtStart;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pFileStart;
		}

		/// GetFileName
		/// Return a pointer to the filename part of the path
		inline char* GetFileName(char* pPath)
		{
			char* pDirStart;
			char* pFileStart = NULL;
			char* pExtStart;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pFileStart;
		}

		inline char16_t* GetFileName(char16_t* pPath)
		{
			char16_t* pDirStart;
			char16_t* pFileStart = NULL;
			char16_t* pExtStart;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pFileStart;
		}

		/// GetFileExtension
		/// Return a pointer to the file extension part of the path
		inline const char* GetFileExtension(const char* pPath)
		{
			const char* pDirStart;
			const char* pFileStart;
			const char* pExtStart = NULL;
			
			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pExtStart;
		}

		inline const char16_t* GetFileExtension(const char16_t* pPath)
		{
			const char16_t* pDirStart;
			const char16_t* pFileStart;
			const char16_t* pExtStart = NULL;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pExtStart;
		}

		/// GetFileExtension
		/// Return a pointer to the file extension part of the path
		inline char* GetFileExtension(char* pPath)
		{
			char* pDirStart;
			char* pFileStart;
			char* pExtStart = NULL;
			
			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pExtStart;
		}

		inline char16_t* GetFileExtension(char16_t* pPath)
		{
			char16_t* pDirStart;
			char16_t* pFileStart;
			char16_t* pExtStart = NULL;

			SplitPathPtrs(pPath, pDirStart, pFileStart, pExtStart);
			return pExtStart;
		}
 

	} // namespace IO


} // namespace EA


#endif // EAIO_EAFILEPATH_H



















