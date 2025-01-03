////////////////////////////////////////////////////////////////////////////
// EAFileDirectory.h
//
// Copyright (c) 2003 Electronic Arts, Inc. All Rights Reserved.
// Created by Paul Pedriana
//
// Implements file system directory searching and manipulation.
////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EAFILEDIRECTORY_H) && !defined(EAFILEDIRECTORY_H)
#define EAIO_EAFILEDIRECTORY_H
#define FOUNDATION_EAFILEDIRECTORY_H // For backward compatability, eventually we'll want to remove all refernces to FOUNDATION_EAFILEDIRECTORY_H.


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#include <EASTL/list.h>
#include <EAIO/PathString.h>
#include <EASTL/string.h>
#include <stddef.h>
#include <string.h>
#include <time.h>


#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
	namespace IO
	{
		/// DirectoryIterator
		///
		/// Represents a view of a directory listing.
		///
		class EAIO_API DirectoryIterator
		{
		public:
			/// kMaxEntryCountDefault
			/// The default max count of Entries to enumerate at a time.
			static const size_t kMaxEntryCountDefault = 1048576;

			/// EntryString
			/// The string type that DirectoryIterator uses for path entries.
			#if EAIO_DIRECTORY_ITERATOR_USE_PATHSTRING
				typedef Path::PathStringW EntryString;
			#else // Else old mode, for backward compatibility.
				typedef eastl::wstring EntryString;
			#endif


			/// class Entry
			/// Represents a single file system directory entry, such as a file or a directory.
			/// To do: Switch the msName member from a string16 to a fixed_string or to a string
			/// that uses the user-supplied ICoreAllocator. The problem is that there is existing 
			/// code that is dependent on this being a string16 object, so this change will 
			/// require some coordination.
			struct EAIO_API Entry
			{
				Entry(DirectoryEntry entry = kDirectoryEntryNone, const wchar_t* pName = NULL);

				DirectoryEntry  mType;              /// Either kDirectoryEntryFile or kDirectoryEntryDirectory.
				EntryString     msName;             /// This may refer to the directory or file name alone, or may be a full path. It depends on the documented use.
				time_t          mCreationTime;      /// See enum FileTimeType.
				time_t          mModificationTime;  /// See enum FileTimeType.
				uint64_t        mSize;              /// Refers to the last saved size of the file.
			};

			typedef eastl::list<Entry, Allocator::EAIOEASTLCoreAllocator> EntryList;


		public:
			/// DirectoryIterator
			DirectoryIterator();

			/// DirectoryIterator
			DirectoryIterator(const DirectoryIterator& x);

			/// operator=
			DirectoryIterator& operator=(const DirectoryIterator& x);


			/// Read
			///
			/// Yields a list of the directory entries that match the input criteria.
			/// Returns the number of entries added to the list.
			/// Returns 0 for an empty or non-existant directory.
			///
			/// This function will not store the results of this read internally.
			/// This function is not recursive; it will not read the contents of subdirectories.
			/// The returned list contains only the entry name and not the full path to that
			/// entry. You can prepend the base path to get the full path.
			/// The returned list will not contain special directory values "./" and "../".
			///
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			///
			/// Example usage:
			///     // How to search for a file in a directory tree.
			///     DirectoryIterator di;
			///     DirectoryIterator::EntryList entryList;
			///
			///     const bool fileWasFound = (di.Read(L"/UserData/", entryList, L"*", kDirectoryEntryFile, 1000, false) > 0);
			///     if(fileWasFound)
			///         printf("File found: %ls.\n", entryList.front().c_str());
			///
			size_t Read(const wchar_t* pDirectory, EntryList& entryList, const wchar_t* pFilterPattern = NULL, 
						int nDirectoryEntryFlags = kDirectoryEntryFile, size_t maxResultCount = kMaxEntryCountDefault,
						bool bReadFileStat = true);


			/// ReadRecursive
			///
			/// Yields a list of all paths that match the input criteria.
			/// Returns the number of items added to the list.
			/// Returns 0 for an empty or non-existant directory.
			///
			/// This function can be used both as a file system enumeration function and 
			/// as a search function. To use this function to search for a file, set the 
			/// maxResultCount to 1 and it will return as soon as it finds a matching file.
			///
			/// As indicated in the name, this function is recursive. It will search all 
			/// subdirectories recursively for matching data. The returned list is in
			/// an order consistent with a depth-first search. The returned list contains 
			/// partial paths to each entry if bFullPaths is false; you can prepend the 
			/// base path to get the full path in this case. If bFullPaths is true,
			/// all entries contain full paths.
			///
			/// If the entryList is non-empty, this function will append to the list.
			/// The returned list will not contain special directory values "./" and "../".
			///
			/// Note that this function and all other functions in the EAFile/EADirectory system requires
			/// a directory path name that ends in a path separator. This is by design as it simplifies
			/// the specification of and manipulation of paths.
			///
			/// Example usage:
			///     // How to search for a file in a directory tree.
			///     DirectoryIterator di;
			///     DirectoryIterator::EntryList entryList;
			///
			///     const bool fileWasFound = (di.ReadRecursive(L"/UserData/", entryList, L"CustomMusic.mp3", kDirectoryEntryFile, true, true, 1, false) == 1);
			///     if(fileWasFound)
			///         printf("File found: %ls.\n", entryList.front().c_str());
			/// 
			size_t ReadRecursive(const char* pBaseDirectory, EntryList& entryList, const char* pFilterPattern = NULL, 
								 int nDirectoryEntryFlags = kDirectoryEntryFile, bool bIncludeBaseDirectoryInSearch = true, 
								 bool bFullPaths = true, size_t maxResultCount = kMaxEntryCountDefault, bool bReadFileStat = true);

			size_t ReadRecursive(const char16_t* pBaseDirectory, EntryList& entryList, const char16_t* pFilterPattern = NULL, 
								 int nDirectoryEntryFlags = kDirectoryEntryFile, bool bIncludeBaseDirectoryInSearch = true, 
								 bool bFullPaths = true, size_t maxResultCount = kMaxEntryCountDefault, bool bReadFileStat = true);

			size_t ReadRecursive(const char32_t* pBaseDirectory, EntryList& entryList, const char32_t* pFilterPattern = NULL, 
								 int nDirectoryEntryFlags = kDirectoryEntryFile, bool bIncludeBaseDirectoryInSearch = true, 
								 bool bFullPaths = true, size_t maxResultCount = kMaxEntryCountDefault, bool bReadFileStat = true);
			#if EA_WCHAR_UNIQUE
				size_t ReadRecursive(const wchar_t* pBaseDirectory, EntryList& entryList, const wchar_t* pFilterPattern = NULL, 
									 int nDirectoryEntryFlags = kDirectoryEntryFile, bool bIncludeBaseDirectoryInSearch = true, 
									 bool bFullPaths = true, size_t maxResultCount = kMaxEntryCountDefault, bool bReadFileStat = true);
			#endif


		protected:
			size_t          mnListSize;             /// Size of list. Used to compare to maxResultCount. Can't use list::size at runtime due to performance reasons and because the user list may be non-empty.
			int             mnRecursionIndex;       /// The ReadRecursive recursion level. Used by recursive member functions.
			const wchar_t*  mpBaseDirectory;        /// Saved copy of pBaseDirectory for first ReadRecursive call. Used by recursive member functions.
			eastl_size_t    mBaseDirectoryLength;   /// Strlen of mpBaseDirectory.

		}; // class DirectoryIterator





		/// kEntryFindDataSize
		///
		/// Defines size of extra data for the EntryFindData struct.
		///
		const size_t kEntryFindDataSize = 64; // This should be tailored for each platform.


		/// EntryFindData
		///
		/// Directory names are suffixed with path separators.
		///
		/// Special directory values "./" and "../" are enumerated by these functions.
		/// The reason for this is to allow the testing of the presence of a directory
		/// and the testing of the presence of a parent directory.
		///
		struct EAIO_API EA_PREFIX_ALIGN(8) EntryFindData
		{
			// Input parameters                                 /// The user sets these values.
			bool      mbReadFileStat;                           /// If true then mCreationTime, mModificationTime, and mSize are read during iteration and are valid. Defaults to true. Is always true on Microsoft platforms.

			// Output parameters                                /// The EntryFind functions set these values.
			wchar_t   mName[kMaxPathLength];                    /// The individual resulting file or directory entry name itself. Not a full path.
			bool      mbIsDirectory;                            /// True if the found file is a directory and not a file.
			bool      mbIsAllocated;                            /// True if this EntryFindData struct was allocated by EntryFindFirst.
			time_t    mCreationTime;                            /// See enum FileTimeType.
			time_t    mModificationTime;                        /// See enum FileTimeType.
			uint64_t  mSize;                                    /// Refers to the last saved size of the file. 
			wchar_t   mDirectoryPath[kMaxPathLength];           /// The directory specified in EntryFindFirst.
			wchar_t   mEntryFilterPattern[kMaxPathLength];      /// The filter pattern specified in EntryFindFirst.

			// Private parameters                               /// These should never be modified by the user.
			uintptr_t mPlatformHandle;                          /// Platform or implementation-specific handle, if needed.
			char      mPlatformData[kEntryFindDataSize];        /// Platform or implementation-specific data.

			EntryFindData();
			EntryFindData(const EntryFindData&);
			EntryFindData& operator=(const EntryFindData&);
		} EA_POSTFIX_ALIGN(8);


		/// EntryFindFirst
		///
		/// This function returns an internally allocated EntryFindData* or returns NULL. 
		/// You must call EntryFindFinish to free the EntryFindData*; do not attempt to 
		/// delete the pointer yourself. This function allocates memory from the global
		/// heap; the amount of memory allocated is relative to the number of matching 
		/// entries in the given directory.
		///
		/// Only Unicode-named file system entries are found, and any others are ignored, as there
		/// is no way to know what their encoding is and thus no way to represent them in a portable
		/// with with Unicode (wchar_t or UTF-8).
		/// Microsoft plaform file systems are universally Unicode. Unix file systems are 
		/// usually UTF8 Unicode but you can encounter non-UTF8 files, especially if mounting
		/// an older CD or a Windows file share. Macintosh file systems universally Unicode
		/// though with the characters decomposed (a`) is named on disk as (a + `). Gaming 
		/// console platforms are mostly Unicode, but some (e.g. Sony platforms) let applications 
		/// write non-UTF8 file names like with Unix.
		///
		/// Note that this function and all other functions in the EAFile/EADirectory system requires
		/// a directory path name that ends in a path separator. This is by design as it simplifies
		/// the specification of and manipulation of paths. Similarly, directory entries returned
		/// by this function end with a path separator.
		///
		/// To do: Add a flag parameter that allows the user to specify that he 
		/// wants files, directories, or both. Use enum DirectoryEntry.
		///
		/// Example usage:
		///    EntryFindData efd;
		///    
		///    if(EntryFindFirst(L"/somedir/", L"*.html", &efd)) {
		///        do {
		///            Printf8("%ls\n", efd.mName);
		///        } while(EntryFindNext(&efd));
		///    
		///        EntryFindFinish(&efd);
		///    }
		///    
		EAIO_API EntryFindData* EntryFindFirst(const char*   pDirectoryPath, const char*  pFilterPattern = NULL, EntryFindData* pEntryFindData = NULL);
		EAIO_API EntryFindData* EntryFindFirst(const char16_t*  pDirectoryPath, const char16_t* pFilterPattern = NULL, EntryFindData* pEntryFindData = NULL);
		EAIO_API EntryFindData* EntryFindFirst(const char32_t*  pDirectoryPath, const char32_t* pFilterPattern = NULL, EntryFindData* pEntryFindData = NULL);
		#if EA_WCHAR_UNIQUE
			EAIO_API EntryFindData* EntryFindFirst(const wchar_t* pDirectory, const wchar_t* pFilterPattern = NULL, EntryFindData* pEntryFindData = NULL);
		#endif

		/// EntryFindNext
		///
		/// Input is the 'EntryFindData*' returned from EntryFindFirst. 
		/// You must call EntryFindFinish to free the EntryFindData*; 
		/// do not attempt to delete the pointer yourself.
		///
		EAIO_API EntryFindData* EntryFindNext(EntryFindData* pData);


		/// EntryFindFinish
		///
		/// Input is the 'EntryFindData*' returned from EntryFindFirst. 
		/// This is the only safe way to free a EntryFindData*.
		///
		EAIO_API void EntryFindFinish(EntryFindData* pData);

	} // namespace IO

} // namespace EA



///////////////////////////////////////////////////////////////////////////////
// inlines
///////////////////////////////////////////////////////////////////////////////

#include <string.h>

namespace EA
{
	namespace IO
	{

		inline DirectoryIterator::Entry::Entry(DirectoryEntry entry, const wchar_t* pName)
		  : mType(entry)
			#if !EAIO_DIRECTORY_ITERATOR_USE_PATHSTRING
		  , msName(EASTLAllocatorType(EAIO_ALLOC_PREFIX "FileDirectory"))
			#endif
		  , mCreationTime(0)
		  , mModificationTime(0)
		  , mSize(0)
		{
			if(pName)
				msName.assign(pName);
		}

		inline DirectoryIterator::DirectoryIterator()
		  : mnListSize(0),
			mnRecursionIndex(0),
			mpBaseDirectory(NULL),
			mBaseDirectoryLength(0)
		{
		}

		inline DirectoryIterator::DirectoryIterator(const DirectoryIterator&)
		  : mnListSize(0),
			mnRecursionIndex(0),
			mpBaseDirectory(NULL),
			mBaseDirectoryLength(0)
		{
			// Copying member values has no meaning. 
		}

		inline DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator&)
		{
			// Copying member values has no meaning. 
			return *this;
		}



		inline EntryFindData::EntryFindData()
		{
			memset(this, 0, sizeof(EntryFindData));
			mbReadFileStat = true;
		}

		inline EntryFindData::EntryFindData(const EntryFindData& x)
		{
			memcpy(this, &x, sizeof(EntryFindData));
		}

		inline EntryFindData& EntryFindData::operator=(const EntryFindData& x)
		{
			memcpy(this, &x, sizeof(EntryFindData));
			return *this;
		}

	} // namespace IO

} // namespace EA


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard






