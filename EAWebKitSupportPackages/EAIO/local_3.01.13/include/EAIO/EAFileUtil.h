///////////////////////////////////////////////////////////////////////////////
// EAFileUtil.h
// 
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
// Implements various file and directory manipulation functions.
///////////////////////////////////////////////////////////////////////////////

#if !defined(EAIO_EAFILEUTIL_H) && !defined(FOUNDATION_EAFILEUTIL_H)
#define EAIO_EAFILEUTIL_H
#define FOUNDATION_EAFILEUTIL_H // For backward compatability, eventually we'll want to remove all refernces to FOUNDATION_EAFILEUTIL_H.


#ifndef INCLUDED_eabase_H
	#include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#ifndef EAIO_EAFILEBASE_H
	#include <EAIO/EAFileBase.h>
#endif
#ifndef EAIO_EASTREAM_H
	#include <EAIO/EAStream.h>
#endif
#ifndef EASTL_STRING_H
   #include <EASTL/string.h>
#endif
#ifndef EASTL_FIXEDSTRING_H
   #include <EASTL/fixed_string.h>
#endif
#ifndef EAIO_EASTLCOREALLOCATOR_H
	#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#endif

#include <stddef.h>
#include <time.h>


#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
	namespace IO
	{
		// Note that file functions are generally in namespace File, while directory
		// functions are in namespace Directory. The reason for this is to avoid
		// a bunch of annoying defines in <windows.h>, most notably CreateFile,
		// DeleteFile, CopyFile, MoveFile, SetFileAttributes, GetFileAttributes,
		// CreateDirectory, and RemoveDirectory.
		//
		// It is suggested that you not 'using namespace' the File and Directory
		// inner namespaces to avoid conflicts.

		namespace File
		{
			/// File::Exists
			/// Returns true if the given file exists. If the path exists but is a path
			/// to a directory, the return value will be false.
			EAIO_API bool Exists(const char* pPath);
			EAIO_API bool Exists(const char16_t* pPath);
			EAIO_API bool Exists(const char32_t* pPath);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Exists(const wchar_t* pPath);
			#endif

			/// File::PatternExists
			/// Returns true if the given file exists. Returns false if the file 
			/// doesn't exist or if there is a directory of the same name
			/// as the input file. The input path may use simple wildcard
			/// matching with '?' and '*' characters.
			EAIO_API bool PatternExists(const char* pPathPattern); 
			EAIO_API bool PatternExists(const char16_t* pPathPattern); 
			EAIO_API bool PatternExists(const char32_t* pPathPattern); 
			#if EA_WCHAR_UNIQUE
				EAIO_API bool PatternExists(const wchar_t* pPathPattern);
			#endif

			/// File::Create
			/// Creates a file at the given path. If the file already exists at the 
			/// given path, then if bTruncate is true the file is truncated to zero bytes.
			/// The return value is true if upon completion of this function there 
			/// is a file at the given path and the bTruncate value could be honored. 
			/// Otherwise the return value is false and the error can be retrived via errno.
			/// The created file is not open upon return of this function.
			/// If you want finer control over how the file is created, use the FileStream class.
			EAIO_API bool Create(const char* pPath, bool bTruncate = false);
			EAIO_API bool Create(const char16_t* pPath, bool bTruncate = false);
			EAIO_API bool Create(const char32_t* pPath, bool bTruncate = false);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Create(const wchar_t* pPath, bool bTruncate = false);
			#endif

			/// File::Remove
			/// Deletes the given file. Returns true if it could be deleted.
			/// Returns false if the file doesn't exist.
			EAIO_API bool Remove(const char* pPath);
			EAIO_API bool Remove(const char16_t* pPath);
			EAIO_API bool Remove(const char32_t* pPath);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Remove(const wchar_t* pPath);
			#endif

			/// File::Move
			/// Moves the given file. If the old and new path refer to the same
			/// directories, the move acts as a file rename. The return value is
			/// true if the file could be moved to the new location and the old
			/// file could be removed. 
			/// On some platforms, file moves are not natively supported and 
			/// the move must be emulated by a copy + delete. It is possible in 
			/// these cases the copy may succeed but the delete may fail and 
			/// thus leave the source file as it was.
			EAIO_API bool Move(const char*  pPathSource, const char*  pPathDestination, bool bOverwriteIfPresent = true);
			EAIO_API bool Move(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent = true);
			EAIO_API bool Move(const char32_t* pPathSource, const char32_t* pPathDestination, bool bOverwriteIfPresent = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Move(const wchar_t* pPathSource, const wchar_t* pPathDestination, bool bOverwriteIfPresent = true);
			#endif

			/// File::Rename
			/// Renames a file. Returns true if the file could be renamed. If the
			/// return value if false, the original file will remain as it was before.
			inline
			bool Rename(const char* pPathSource, const char* pPathDestination, bool bOverwriteIfPresent = true)
				{ return Move(pPathSource, pPathDestination, bOverwriteIfPresent); }

			inline
			bool Rename(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent = true)
				{ return Move(pPathSource, pPathDestination, bOverwriteIfPresent); }

			inline
			bool Rename(const char32_t* pPathSource, const char32_t* pPathDestination, bool bOverwriteIfPresent = true)
				{ return Move(pPathSource, pPathDestination, bOverwriteIfPresent); }

			#if EA_WCHAR_UNIQUE
				inline
				bool Rename(const wchar_t* pPathSource, const wchar_t* pPathDestination, bool bOverwriteIfPresent = true)
					{ return Move(pPathSource, pPathDestination, bOverwriteIfPresent); }
			#endif

			/// File::Copy
			/// Copies a file from one location to another. Returns true if the 
			/// copy could be successfully completed as specified. If bOverwriteIfPresent 
			/// is true, then this function will overwrite an existing destination 
			/// file if it already exists. If false, then the copy will do nothing
			/// and return false if the destination file already exists.
			EAIO_API bool Copy(const char*  pPathSource, const char*  pPathDestination, bool bOverwriteIfPresent = true);
			EAIO_API bool Copy(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent = true);
			EAIO_API bool Copy(const char32_t* pPathSource, const char32_t* pPathDestination, bool bOverwriteIfPresent = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Copy(const wchar_t* pPathSource, const wchar_t* pPathDestination, bool bOverwriteIfPresent = true);
			#endif

			/// File::Stat
			EAIO_API bool Stat(const char*  pPath, StatData& statData);
			EAIO_API bool Stat(const char16_t* pPath, StatData& statData);
			EAIO_API bool Stat(const char32_t* pPath, StatData& statData);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Stat(const wchar_t* pPath, StatData& statData);
			#endif

			/// File::GetSize
			/// Returns the length of the file at the given path. Returns (size_type)-1
			/// (a.k.a. kSizeTypeError) if the file doesn't exist or the length could 
			/// not be determined.
			EAIO_API size_type GetSize(const char*  pPath);
			EAIO_API size_type GetSize(const char16_t* pPath);
			EAIO_API size_type GetSize(const char32_t* pPath);
			#if EA_WCHAR_UNIQUE
				EAIO_API size_type GetSize(const wchar_t* pPath);
			#endif

			/// File::IsWritable
			/// Returns true if the file can be written to. This is a true writability test
			/// and not simply a test for a 'writable' file attribute.
			EAIO_API bool IsWritable(const char*  pPath);
			EAIO_API bool IsWritable(const char16_t* pPath);
			EAIO_API bool IsWritable(const char32_t* pPath);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool IsWritable(const wchar_t* pPath);
			#endif

			/// File::GetAttributes
			/// Gets a bitmask of enum Attribute for the file at the given path.
			/// Returns zero (kAttributeNone) if the file could not be found.
			EAIO_API int GetAttributes(const char*  pPath);
			EAIO_API int GetAttributes(const char16_t* pPath);
			EAIO_API int GetAttributes(const char32_t* pPath);
			#if EA_WCHAR_UNIQUE
				EAIO_API int GetAttributes(const wchar_t* pPath);
			#endif

			/// File::SetAttributes
			/// Sets or clears the specified attributes of the given file. Has no 
			/// effect on other attributes. Returns true if the attributes could
			/// all be set as desired.
			EAIO_API bool SetAttributes(const char*  pPath, int nAttributeMask, bool bEnable);
			EAIO_API bool SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable);
			EAIO_API bool SetAttributes(const char32_t* pPath, int nAttributeMask, bool bEnable);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool SetAttributes(const wchar_t* pPath, int nAttributeMask, bool bEnable);
			#endif

			/// File::GetTime
			/// Allows you to get creation time, last modification time, last access time
			/// or any other timeType as identified by enum TimeType. Return value is in
			/// the format of the C time_t data type. A return value of zero means the  
			/// operation was not possible.
			EAIO_API time_t GetTime(const char*  pPath, FileTimeType timeType);
			EAIO_API time_t GetTime(const char16_t* pPath, FileTimeType timeType);
			EAIO_API time_t GetTime(const char32_t* pPath, FileTimeType timeType);
			#if EA_WCHAR_UNIQUE
				EAIO_API time_t GetTime(const wchar_t* pPath, FileTimeType timeType);
			#endif

			/// File::SetTime
			/// Sets the time of the given FileTimeType attribute(s) via the specified 
			/// nTime input. nTime is of the C time_t format, which can be read via 
			/// the C time() function (from time.h). Returns true if the request could
			/// be successfully honored.
			EAIO_API bool SetTime(const char*  pPath, int nFileTimeTypeFlags, time_t nTime); 
			EAIO_API bool SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			EAIO_API bool SetTime(const char32_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			#if EA_WCHAR_UNIQUE
				EAIO_API bool SetTime(const wchar_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			#endif


			/// ResolveAliasResult
			/// Defines valid return values for the ResolveAlias function.
			enum ResolveAliasResult {
				kRARInvalid,    /// The path was invalid
				kRARNone,       /// The path is not an alias
				kRARAlias       /// The path is an alias.
			};

			/// File::ResolveAlias
			/// Some platforms (e.g. Windows) have the concept of file aliases or shortcuts. 
			/// This function converts such a path to its final destination path.
			/// If the destination path is non-NULL, the result path is copied to it. In this
			/// case even if the source is not an alias, it is copied to the destination. This is
			/// so even if the source file could not be found. The destination and source paths
			/// may be the same block of memory. The destination path is filled in regardless of
			/// the return value. If the path is not an alias, the source is copied to the destination.
			/// 
			/// Windows supports explicit aliases (.lnk files) and implicit aliases (junctions).
			/// Unix supports implicit aliases (symbolic links and hard links).
			/// Mac supports implicit aliases (symbolic links and hard links) and smart aliases (called simply "aliases" by Apple).
			/// Console platforms support no kind of file system aliases.
			///
			/// This function does not work on all platforms. As of this writing, we have support 
			/// only for creating and resolving Windows explicit aliases.
			/// 
			EAIO_API ResolveAliasResult ResolveAlias(const char*  pPathSource, char*  pPathDestination, uint32_t nDestPathCapacity = kMaxPathLength);
			EAIO_API ResolveAliasResult ResolveAlias(const char16_t* pPathSource, char16_t* pPathDestination, uint32_t nDestPathCapacity = kMaxPathLength);
			EAIO_API ResolveAliasResult ResolveAlias(const char32_t* pPathSource, char32_t* pPathDestination, uint32_t nDestPathCapacity = kMaxPathLength);
			#if EA_WCHAR_UNIQUE
				EAIO_API ResolveAliasResult ResolveAlias(const wchar_t* pPathSource, wchar_t* pPathDestination, uint32_t nDestPathCapacity = kMaxPathLength);
			#endif


			/// File::CreateAlias
			/// Creates a shortcut file with a given set of parameters.
			/// Shortcuts here are not the same thing as WinNT junctions and Unix symbolic links.
			/// The pDestinationPath and pShortcutPath parameters must be valid.
			/// The pShortcutDescription and pShortcutArguments parameters may be NULL, in which
			/// case they will be ignored.
			/// This function does not work on all platforms. As of this writing, we have support 
			/// only for creating and resolving Windows explicit aliases.
			///
			EAIO_API bool CreateAlias(const char* pDestinationPath, const char* pShortcutPath, 
									  const char* pShortcutDescription, const char* pShortcutArguments);
			EAIO_API bool CreateAlias(const char16_t* pDestinationPath, const char16_t* pShortcutPath, 
									  const char16_t* pShortcutDescription, const char16_t* pShortcutArguments);
			EAIO_API bool CreateAlias(const char32_t* pDestinationPath, const char32_t* pShortcutPath, 
									  const char32_t* pShortcutDescription, const char32_t* pShortcutArguments);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool CreateAlias(const wchar_t* pDestinationPath, const wchar_t* pShortcutPath, 
										  const wchar_t* pShortcutDescription, const wchar_t* pShortcutArguments);
			#endif

		} // namespace File


		namespace Directory
		{
			/// Directory::Exists
			/// Returns true if the given input pDirectoryPath exists. If the path exists
			/// but the path refers to a file and not a directory, the return value is false.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool Exists(const char*  pDirectory);
			EAIO_API bool Exists(const char16_t* pDirectory);
			EAIO_API bool Exists(const char32_t* pDirectory);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Exists(const wchar_t* pDirectory);
			#endif

			/// Directory::EnsureExists
			/// Makes sure the directory exists, creates it if needed. 
			/// Returns true if the directory existed or could be created.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool EnsureExists(const char*  pDirectory);
			EAIO_API bool EnsureExists(const char16_t* pDirectory);
			EAIO_API bool EnsureExists(const char32_t* pDirectory);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool EnsureExists(const wchar_t* pDirectory);
			#endif

			/// Directory::Create
			/// Makes sure the directory exists; creates it if it doesn't exist. 
			/// The specified directory must end with a trailing path separator.
			/// Returns true if it exists after the function return. This function is 
			/// smart enough to create the parent directories of the directory if they 
			/// happen to also not exist. This is as opposed to the standard C library 
			/// directory creation function which only succeeds if the directory's parent
			/// already exists. If unable to build the directory path, this function 
			/// restores the directory system state to what it was before the attempt
			/// to create the directory; thus any directories created are removed.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool Create(const char*  pDirectory);
			EAIO_API bool Create(const char16_t* pDirectory);
			EAIO_API bool Create(const char32_t* pDirectory);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Create(const wchar_t* pDirectory);
			#endif

			/// Directory::Remove
			/// Removes the given directory. If the input 'bAllowRecursiveRemoval' is 
			/// true, then this function will also remove all files and directories below
			/// the input directory. If 'bAllowRecursiveRemoval' is false and there are 
			/// child files or directories, the removal attempt will fail.
			/// The specified directory must end with a trailing path separator.
			/// Returns true if the directory could be removed. Returns false if the directory could
			/// not be removed or if the directory didn't exist.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool Remove(const char*  pDirectory, bool bAllowRecursiveRemoval = true);
			EAIO_API bool Remove(const char16_t* pDirectory, bool bAllowRecursiveRemoval = true);
			EAIO_API bool Remove(const char32_t* pDirectory, bool bAllowRecursiveRemoval = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Remove(const wchar_t* pDirectory, bool bAllowRecursiveRemoval = true);
			#endif


			/// Directory::Move
			/// Moves the given directory. If the old and new path refer to the same
			/// directories, the move acts as a directory rename. The return value is
			/// true if the directory could be moved to the new location and the old
			/// directory could be removed.
			/// On some platforms, directory moves are not natively supported and
			/// the move must be emulated by a copy + delete. It is possible in
			/// these cases the copy may succeed but the delete may fail and
			/// thus leave the source directory as it was.
			///
			/// This API has not yet been implemented. Contact EATechCoreSupport@ea.com if you require it.
			// EAIO_API bool Move(const char*  pDirectorySource, const char*  pDirectoryDestination, bool bOverwriteIfPresent = true);
			// EAIO_API bool Move(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bOverwriteIfPresent = true);
			// EAIO_API bool Move(const char32_t* pDirectorySource, const char32_t* pDirectoryDestination, bool bOverwriteIfPresent = true);
			#if EA_WCHAR_UNIQUE
			//    EAIO_API bool Move(const wchar_t* pDirectorySource, const wchar_t* pDirectoryDestination, bool bOverwriteIfPresent = true);
			#endif

			/// Directory::Rename
			/// Renames the directory. This function cannot be used to move a directory; the old
			/// and new directory names must refer to a directory with the same parent. The input
			/// 'pDirectoryOld' must be a full directory path. The input 'pDirectoryNew' can be 
			/// either a directory path or just an individual directory name but if it is a path 
			/// then only the last name within the path will be used.
			/// The specified directory must end with a trailing path separator.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool Rename(const char*  pDirectoryOld, const char*  pDirectoryNew);
			EAIO_API bool Rename(const char16_t* pDirectoryOld, const char16_t* pDirectoryNew);
			EAIO_API bool Rename(const char32_t* pDirectoryOld, const char32_t* pDirectoryNew);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Rename(const wchar_t* pDirectoryOld, const wchar_t* pDirectoryNew);
			#endif

			/// Directory::Copy
			/// This function performs a deep copy of a directory structure from the source to the 
			/// destination, creating any entries it needs along the way. If bOverwriteIfAlreadyPresent
			/// is true, then any entries that exist at the destination before the copy are overwritten
			/// by the source. Any entries that exist at the destination that aren't present in the 
			/// source are left in place; you must use RemoveDirectory if you want to ensure that these
			/// are removed as well.
			/// Returns true if the entire operation could be completed without copy error. If an error
			/// copying a file or subdirectory is encountered, the operation continues to proceed.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool Copy(const char*  pDirectorySource, const char*  pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
			EAIO_API bool Copy(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
			EAIO_API bool Copy(const char32_t* pDirectorySource, const char32_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool Copy(const wchar_t* pDirectorySource, const wchar_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
			#endif

			/// Directory::GetAttributes
			/// Directory::SetAttributes
			/// Currently these are the same as for files.
			using File::GetAttributes;
			using File::SetAttributes;

			/// Directory::SetAttributes
			/// Sets or clears the given attributes (see enum Attribute) for all directory
			/// entries recursively. This is useful for preparing for a mass deletion of 
			/// a directory tree, for example. The attribute application applies to both 
			/// file and directory entries. If bIncludeBaseDirectory is true, the attributes
			/// are applied to the base directory entry itself as wel as its children.
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool SetAttributes(const char*  pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
			EAIO_API bool SetAttributes(const char16_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
			EAIO_API bool SetAttributes(const char32_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool SetAttributes(const wchar_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
			#endif

			/// Directory::GetTime
			/// Allows you to get creation time, last modification time, last access time
			/// or any other timeType as identified by enum TimeType. Return value is in
			/// the format of the C time_t data type. A return value of zero means the  
			/// operation was not possible.
			EAIO_API time_t GetTime(const char*  pPath, FileTimeType timeType);
			EAIO_API time_t GetTime(const char16_t* pPath, FileTimeType timeType);
			EAIO_API time_t GetTime(const char32_t* pPath, FileTimeType timeType);
			#if EA_WCHAR_UNIQUE
				EAIO_API time_t GetTime(const wchar_t* pPath, FileTimeType timeType);
			#endif

			/// Directory::SetTime
			/// Sets the time of the given FileTimeType attribute(s) via the specified 
			/// nTime input. nTime is of the C time_t format, which can be read via 
			/// the C time() function (from time.h). Returns true if the request could
			/// be successfully honored.
			EAIO_API bool SetTime(const char*  pPath, int nFileTimeTypeFlags, time_t nTime); 
			EAIO_API bool SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			EAIO_API bool SetTime(const char32_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			#if EA_WCHAR_UNIQUE
				EAIO_API bool SetTime(const wchar_t* pPath, int nFileTimeTypeFlags, time_t nTime); 
			#endif

			/// Directory::GetCurrentWorkingDirectory
			/// Returns a full path to the current working directory.
			/// The output pDirectory must be of at least length kMaxDirectoryLength.
			/// Returns the strlen of the returned path, which may be zero if the 
			/// working directory is unspecified. Note that an empty string is not 
			/// the same as a string of "/", as "/" refers to the file system root.
			/// The returned directory will end with a trailing path separator.
			EAIO_API int GetCurrentWorkingDirectory(char*  pDirectory, uint32_t nPathCapacity = kMaxPathLength);
			EAIO_API int GetCurrentWorkingDirectory(char16_t* pDirectory, uint32_t nPathCapacity = kMaxPathLength);
			EAIO_API int GetCurrentWorkingDirectory(char32_t* pDirectory, uint32_t nPathCapacity = kMaxPathLength);
			#if EA_WCHAR_UNIQUE
				EAIO_API int GetCurrentWorkingDirectory(wchar_t* pDirectory, uint32_t nPathCapacity = kMaxPathLength);
			#endif

			/// Directory::SetCurrentWorkingDirectory
			/// Sets the current working directory to that specified. 
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			EAIO_API bool SetCurrentWorkingDirectory(const char*  pDirectory);
			EAIO_API bool SetCurrentWorkingDirectory(const char16_t* pDirectory);
			EAIO_API bool SetCurrentWorkingDirectory(const char32_t* pDirectory);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool SetCurrentWorkingDirectory(const wchar_t* pDirectory);
			#endif


			/// Directory::IsDirectoryEmpty
			/// Returns true if the directory is empty. The nDirectoryEntryFlags parameter specifies if 
			/// you want to see if the directory is empty of files or empty of directories. The bRecursive
			/// parameter specifies if the emptiness check applies to files within subdirectories in addition
			/// to the current directory. Returns true if the directory doesn't exist (non-existant directory is empty).
			/// For example, you want to see if a directory has no files in it nor any of its subdirectories,
			/// use IsDirectoryEmpty(<dirpath>, kDirectoryEntryFile, true). If you want to see if a directory 
			/// has any subdirectories, use IsDirectoryEmpty(<dirpath>, kDirectoryEntryDirectory, false).
			EAIO_API bool IsDirectoryEmpty(const char*  pDirectory, int nDirectoryEntryFlags = kDirectoryEntryFile, bool bRecursive = true);
			EAIO_API bool IsDirectoryEmpty(const char16_t* pDirectory, int nDirectoryEntryFlags = kDirectoryEntryFile, bool bRecursive = true);
			EAIO_API bool IsDirectoryEmpty(const char32_t* pDirectory, int nDirectoryEntryFlags = kDirectoryEntryFile, bool bRecursive = true);
			#if EA_WCHAR_UNIQUE
				EAIO_API bool IsDirectoryEmpty(const wchar_t* pDirectory, int nDirectoryEntryFlags = kDirectoryEntryFile, bool bRecursive = true);
			#endif

		} // namespace Directory


		/// InitializeFileSystem
		/// This is a manual file system initialization/shutdown function.
		/// You usually won't need to call this, as it is called automatically on startup.
		/// However, there may be cases where this is not so.
		EAIO_API bool InitializeFileSystem(bool bInitialize);

		/// MakeTempPathName
		/// Makes a temporary file name path for the user.
		/// Upon return, a file at the given path will be created with read/write 
		/// attributes and a size of zero. This is done so in order to reserve the 
		/// path's usability for the caller.
		/// The return value is true if the given path could be created.
		/// If the input pDirectory is non-NULL, then the temp file is created in 
		/// that directory. Otherwise, a default temp directory will be used.
		/// An input directory must end with a trailing path separator.
		/// If the input pFileName is non-NULL, it is used as a prefix for the temp
		/// file name. Otherwise an internal default will be used.
		/// If the pExtension parameter is non-NULL, then the returned path will have a file name with the given
		/// extension. The input pExtension should begin with a '.' character and must not be any longer than 32 characters.
		/// The output pPath must be able to hold at least kMaxPathLength characters.
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
		EAIO_API bool MakeTempPathName(char*  pDestPath, const char*  pDirectory = NULL, const char*  pFileName = NULL, const char*  pExtension = NULL, uint32_t nDestPathCapacity = kMaxPathLength);
		EAIO_API bool MakeTempPathName(char16_t* pDestPath, const char16_t* pDirectory = NULL, const char16_t* pFileName = NULL, const char16_t* pExtension = NULL, uint32_t nDestPathCapacity = kMaxPathLength);
		EAIO_API bool MakeTempPathName(char32_t* pDestPath, const char32_t* pDirectory = NULL, const char32_t* pFileName = NULL, const char32_t* pExtension = NULL, uint32_t nDestPathCapacity = kMaxPathLength);
		#if EA_WCHAR_UNIQUE
			EAIO_API bool MakeTempPathName(wchar_t* pDestPath, const wchar_t* pDirectory = NULL, const wchar_t* pFileName = NULL, const wchar_t* pExtension = NULL, uint32_t nDestPathCapacity = kMaxPathLength);
		#endif

		/// GetTempDirectory
		/// Retrieves a standard temporary directory for the current user.
		/// Returns the strlen of the given name or -1 upon error.
		/// If an error occurs, the output directory will be unmodified.
		/// The output pDirectory should have a capacity of at least kMaxDirectoryLength 
		/// characters (for a maximum strlen of kMaxDirectoryLength - 1).
		/// The returned output directory string will end with a trailing path separator.
		/// The output directory will exist upon return of this function or else this function
		/// will return -1.
		/// If the user has specified a non-empty temp directory via SetTempDirectory,
		/// that directory will be returned. Otherwise a system-default temp directory
		/// will be returned.
		/// The user should beware that the returned directory may have access rights 
		/// limitations on some specific systems that prevent writing to the directory
		/// for various reasons. While this is uncommon it should be considered a possibility
		/// on some platforms.
		EAIO_API int GetTempDirectory(char*  pDirectory, uint32_t nPathCapacity = kMaxDirectoryLength);
		EAIO_API int GetTempDirectory(char16_t* pDirectory, uint32_t nPathCapacity = kMaxDirectoryLength);
		EAIO_API int GetTempDirectory(char32_t* pDirectory, uint32_t nPathCapacity = kMaxDirectoryLength);
		#if EA_WCHAR_UNIQUE
			EAIO_API int GetTempDirectory(wchar_t* pDirectory, uint32_t nPathCapacity = kMaxDirectoryLength);
		#endif

		/// SetTempDirectory
		/// Sets the temp directory that the user wishes to use. Future GetTempDirectory
		/// calls will return this directory. If SetTempDirectory is not called, a default
		/// directory path will be returned. The input pDirectory strlen must be no longer
		/// than kMaxDirectoryLength characters, or else the function will fail. 
		/// If the input directory doesn't end in a trailing path separator, one will be 
		/// appended. This trailing path charcater counts towards the input directory length.
		/// If the input directory doesn't exist, this function attempts to create it.
		/// Returns true if the specified directory exists upon the return of this function.
		/// The user can clear the temp directory by passing in an empty string.
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
		EAIO_API bool SetTempDirectory(const char*  pDirectory);
		EAIO_API bool SetTempDirectory(const char16_t* pDirectory);
		EAIO_API bool SetTempDirectory(const char32_t* pDirectory);
		#if EA_WCHAR_UNIQUE
			EAIO_API bool SetTempDirectory(const wchar_t* pDirectory);
		#endif

		/// GetDriveFreeSpace
		/// Returns an estimate of the amount of free space available on the 
		/// drive/volume that contains the given input path. The path can be 
		/// a file or directory path. Returns UINT64_C(0xffffffffffffffff) 
		/// (i.e. (uint64_t)-1) if the value could not be determined.
		EAIO_API uint64_t GetDriveFreeSpace(const char32_t* pPath);
		EAIO_API uint64_t GetDriveFreeSpace(const char16_t* pPath);
		EAIO_API uint64_t GetDriveFreeSpace(const char* pPath);
		#if EA_WCHAR_UNIQUE
			EAIO_API uint64_t GetDriveFreeSpace(const wchar_t* pPath);
		#endif

		/// GetDriveName
		/// Gets the name for the drive/volume specified by the given input path.
		/// The output name should be able to hold at least kMaxVolumeNameLength characters.
		/// Note that the name of the drive isn't usually the same as the volume name, 
		/// though it may be. The name is the user-applied name to the drive, regardless
		/// of its drive letter or directory tree location.
		/// Returns the strlen of the given name or -1 upon error.
		/// If an error occurs, the output name will be unmodified.
		EAIO_API int GetDriveName(const char*  pPath, char*  pName);
		EAIO_API int GetDriveName(const char16_t* pPath, char16_t* pName);
		EAIO_API int GetDriveName(const char32_t* pPath, char32_t* pName);
		#if EA_WCHAR_UNIQUE
			EAIO_API int GetDriveName(const wchar_t* pPath, wchar_t* pName);
		#endif

		/// GetDriveSerialNumber
		/// Gets the serial number for drive/volume specified by the given input path. 
		/// The path can consist of as little as a volume name (e.g. "d:").
		/// The output name should be able to hold at least kMaxVolumeSerialNumberLength characters.
		/// Returns the strlen of the given name or -1 upon error.
		/// If an error occurs, the output name will be unmodified.
		EAIO_API int GetDriveSerialNumber(const char*  pPath, char*  pSerialNumber);
		EAIO_API int GetDriveSerialNumber(const char16_t* pPath, char16_t* pSerialNumber);
		EAIO_API int GetDriveSerialNumber(const char32_t* pPath, char32_t* pSerialNumber);
		#if EA_WCHAR_UNIQUE
			EAIO_API int GetDriveSerialNumber(const wchar_t* pPath, wchar_t* pSerialNumber);
		#endif

		/// GetDriveTypeValue
		/// Returns the DriveType associated with the given file path.
		/// The path can consist of as little as a volume name (e.g. "d:")
		/// or can contain a file path. A full path is useful or even necessary
		/// on platforms such as Unix which use mounted volumes.
		/// This function is called GetDriveTypeValue instead of GetDriveType
		/// because Microsoft #defines GetDriveType to something and breaks code.
		EAIO_API DriveType GetDriveTypeValue(const char*  pPath);
		EAIO_API DriveType GetDriveTypeValue(const char16_t* pPath);
		EAIO_API DriveType GetDriveTypeValue(const char32_t* pPath);
		#if EA_WCHAR_UNIQUE
			EAIO_API DriveType GetDriveTypeValue(const wchar_t* pPath);
		#endif

		/// DriveInfo
		/// Used to store drive/volume infomation.
		/// Where the file system is case-independent, the return value will always be lower case.
		/// Where the file system has drives that end with ':', the ':' is the last character.
		struct EAIO_API DriveInfo8
		{
			DriveInfo8(const char* pName = NULL, DriveType driveType = kDriveTypeUnknown);

			typedef eastl::fixed_string<char, 20, true, Allocator::EAIOEASTLCoreAllocator> DriveInfoName;

			DriveType     mType;  /// e.g. kDriveTypeFixed, kDriveTypeDVD, etc.
			DriveInfoName mName;  /// The format is dependent on the platform. For Win32-like platforms, the drive name is of the form "c:" (lower-case, no trailing slash).
		};

		struct EAIO_API DriveInfo16
		{
			DriveInfo16(const char16_t* pName = NULL, DriveType driveType = kDriveTypeUnknown);

			typedef eastl::fixed_string<char16_t, 20, true, Allocator::EAIOEASTLCoreAllocator> DriveInfoName;

			DriveType     mType;  /// e.g. kDriveTypeFixed, kDriveTypeDVD, etc.
			DriveInfoName mName;  /// The format is dependent on the platform. For Win32-like platforms, the drive name is of the form "c:" (lower-case, no trailing slash).
		};
		typedef DriveInfo16 DriveInfo;

		struct EAIO_API DriveInfo32
		{
			DriveInfo32(const char32_t* pName = NULL, DriveType driveType = kDriveTypeUnknown);

			typedef eastl::fixed_string<char32_t, 20, true, Allocator::EAIOEASTLCoreAllocator> DriveInfoName;

			DriveType     mType;  /// e.g. kDriveTypeFixed, kDriveTypeDVD, etc.
			DriveInfoName mName;  /// The format is dependent on the platform. For Win32-like platforms, the drive name is of the form "c:" (lower-case, no trailing slash).
		};

		#if EA_WCHAR_UNIQUE
			struct EAIO_API DriveInfoW
			{
				DriveInfoW(const wchar_t* pName = NULL, DriveType driveType = kDriveTypeUnknown);

				typedef eastl::fixed_string<wchar_t, 20, true, Allocator::EAIOEASTLCoreAllocator> DriveInfoName;

				DriveType     mType;  /// e.g. kDriveTypeFixed, kDriveTypeDVD, etc.
				DriveInfoName mName;  /// The format is dependent on the platform. For Win32-like platforms, the drive name is of the form "c:" (lower-case, no trailing slash).
			};
		#else
			#if(EA_WCHAR_SIZE == 1)
				typedef DriveInfo8  DriveInfoW;
			#elif(EA_WCHAR_SIZE == 2)
				typedef DriveInfo16 DriveInfoW;
			#else
				typedef DriveInfo32 DriveInfoW;
			#endif
		#endif



		/// GetDriveInfo
		/// Gets an array of existing file system drive info.
		/// Returns the total count of DriveInfo available, which will be greater
		/// than nDriveInfoArrayCapacity if nDriveInfoArrayCapacity is too small.
		/// Regardless of the return value, up to nDriveInfoArrayCapacity values
		/// are written to the drive info array. If pDriveInfoArray is NULL, 
		/// then nDriveInfoArrayCapacity must be zero.
		EAIO_API size_t GetDriveInfo(DriveInfo8*  pDriveInfoArray = NULL, size_t nDriveInfoArrayCapacity = 0);
		EAIO_API size_t GetDriveInfo(DriveInfo16* pDriveInfoArray = NULL, size_t nDriveInfoArrayCapacity = 0);
		EAIO_API size_t GetDriveInfo(DriveInfo32* pDriveInfoArray = NULL, size_t nDriveInfoArrayCapacity = 0);
		#if EA_WCHAR_UNIQUE
			EAIO_API size_t GetDriveInfo(DriveInfoW* pDriveInfoArray = NULL, size_t nDriveInfoArrayCapacity = 0);
		#endif

		/// IsVolumeAvailable
		/// Determines if the mounted volume associated with the given path is available.
		/// Example usage:
		///     bool available = IsVolumeAvailable(EA_CHAR16("/app_home/"), 1000);
		EAIO_API bool IsVolumeAvailable(const char*  pPath, int timeoutMS = 2000);
		EAIO_API bool IsVolumeAvailable(const char16_t* pPath, int timeoutMS = 2000);
		EAIO_API bool IsVolumeAvailable(const char32_t* pPath, int timeoutMS = 2000);
		#if EA_WCHAR_UNIQUE
			EAIO_API bool IsVolumeAvailable(const wchar_t* pPath, int timeoutMS = 2000);
		#endif

		/// GetSpecialDirectory
		/// Gets the 'special directory' as defined by enum SpecialDirectory.
		/// This function is generally specific to desktop and server systems and not to 
		/// embedded or console systems.
		/// The output pDirectory must be of at least length kMaxDirectoryLength.
		/// If 'bEnsureDirectoryExistence' is true, then this function creates the given
		/// directory if not found.
		/// The return directory will end with a trailing path separator.
		/// Returns the strlen of the output directory or -1 upon error.
		/// If an error occurs, the output will be unmodified.
		/// If 'bEnsureDirectoryExistence' is true, then the directory must also 
		/// exist after the return of this function for the return value to be success.
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths.
		EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char*  pDirectory, bool bEnsureDirectoryExistence = false, uint32_t nPathCapacity = kMaxDirectoryLength);
		EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char16_t* pDirectory, bool bEnsureDirectoryExistence = false, uint32_t nPathCapacity = kMaxDirectoryLength);
		EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char32_t* pDirectory, bool bEnsureDirectoryExistence = false, uint32_t nPathCapacity = kMaxDirectoryLength);
		#if EA_WCHAR_UNIQUE
			EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, wchar_t* pDirectory, bool bEnsureDirectoryExistence = false, uint32_t nPathCapacity = kMaxDirectoryLength);
		#endif

		/// Returns true if the input character can always be used in a path file name.
		/// You cannot use this function one-by-one on a file name to determine if the file
		/// name is valid, as there are special combination cases that make this so.
		bool IsFileNameCharValid(char32_t c, FileSystem fileSystemType);

		/// Returns true if the input file name is valid for the file system.
		bool IsFileNameStringValid(const char* pName, FileSystem fileSystemType);
		bool IsFileNameStringValid(const char16_t* pName, FileSystem fileSystemType);
		bool IsFileNameStringValid(const char32_t* pName, FileSystem fileSystemType);
		#if EA_WCHAR_UNIQUE
			bool IsFileNameStringValid(const wchar_t* pName, FileSystem fileSystemType);
		#endif

		/// Returns true if the input directory name is valid for the file system.
		bool IsDirectoryNameStringValid(const char* pName, FileSystem fileSystemType);
		bool IsDirectoryNameStringValid(const char16_t* pName, FileSystem fileSystemType);
		bool IsDirectoryNameStringValid(const char32_t* pName, FileSystem fileSystemType);
		#if EA_WCHAR_UNIQUE
			bool IsDirectoryNameStringValid(const wchar_t* pName, FileSystem fileSystemType);
		#endif

		/// Returns true if the given path is valid. The path can consist of a full
		/// directory and file path, just a full directory path, or just a file name.
		/// fileSystemType refers to the file system, which defaults to the system's
		/// standard file system. 
		EAIO_API bool IsFilePathStringValid(const char* pPath, FileSystem fileSystemType);
		EAIO_API bool IsFilePathStringValid(const char16_t* pPath, FileSystem fileSystemType);
		EAIO_API bool IsFilePathStringValid(const char32_t* pPath, FileSystem fileSystemType);
		#if EA_WCHAR_UNIQUE
			EAIO_API bool IsFilePathStringValid(const wchar_t* pPath, FileSystem fileSystemType);
		#endif


		///////////////////////////////////////////////////////////////////////
		// Platform-specific functionality
		///////////////////////////////////////////////////////////////////////
 
		#if defined(EA_PLATFORM_SONY)

			/// PS4GetTemporaryDataMount
			///
			/// This is an optional wrapper for the sceAppContentTemporaryDataMount-related functionality.
			/// The application can call this on startup to request the OS to provision a temp directory and 
			/// return its path. If registerAsEAIOTempDirectory is true then EA::IO::SetTempDirectory is 
			/// called with the path before returning. 
			/// If pDirectory is NULL then registerAsEAIOTempDirectory must be true and the resulting mount
			/// point will be passed to EA::IO::SetTempDirectory and the caller doesn't need to see the result.
			/// Returns SCE_OK (0) upon success. Returns an SCE error (e.g. SCE_APP_CONTENT_ERROR_BUSY) upon 
			/// error. The primary cause of failure (SCE_APP_CONTENT_ERROR_BUSY) is if some other entity has 
			/// already called this function, at which point this function has no way of knowing the directory path.
			/// Note that users of this need to make sure libSceAppContent_stub_weak.a is linked into the application.
			///
			/// Example usage:
			///     char tempPath[EA::IO::kMaxDirectoryLength];
			///     int result = EA::IO::PS4GetTemporaryDataMount(tempPath, EAArrayCount(tempPath), true);
			///     if(result == SCE_OK)
			///         printf("%s", tempPath);
			/// 
			EAIO_API int PS4GetTemporaryDataMount(char* pDirectory, size_t nDirectoryPathCapacity, bool registerAsEAIOTempDirectory);

		#endif

	} // namespace IO


} // namespace EA


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard










