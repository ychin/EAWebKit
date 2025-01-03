///////////////////////////////////////////////////////////////////////////////
// EAFileBase.h
// 
// Copyright (c) 2003, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
// This file defines base information about file systems for various platforms.
///////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EAFILEBASE_H) && !defined(FOUNDATION_EAFILEBASE_H)
#define EAIO_EAFILEBASE_H
#define FOUNDATION_EAFILEBASE_H


#include <EAIO/internal/Config.h>
#include <EABase/eabase.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h> // Definition of off_t.

namespace EA
{
	namespace IO
	{
		/// size_type 
		///
		/// Used to denote data position offsets. This value must be unsigned.
		/// We define this so that we can smoothly migrate to 64 bit data.
		///
		#if defined(EAIO_64_BIT_SIZE_ENABLED) && EAIO_64_BIT_SIZE_ENABLED
			typedef uint64_t size_type;
		#else
			typedef size_t   size_type;
		#endif


		/// off_type 
		///
		/// Used to denote data position values.  This value must be signed.
		/// We define this so that we can smoothly migrate to 64 bit data.
		/// sizeof(off_type) should equal sizeof(size_type)
		///
		#if defined(EAIO_64_BIT_SIZE_ENABLED) && EAIO_64_BIT_SIZE_ENABLED
			typedef int64_t off_type;
		#else
			#if   defined(__APPLE__) && (EA_PLATFORM_PTR_SIZE == 4)
				typedef intptr_t off_type;      // iPhone defines off_t as 32 bit long even though it defines size_t as 64 bit.
			#elif defined(EA_PLATFORM_MICROSOFT) && (EA_PLATFORM_PTR_SIZE == 8)
				typedef intptr_t off_type;   	// Win64 defines off_t as 32 bit long even though it 
												// defines size_t as 64 bit uint64_t.
			#elif defined(EA_PLATFORM_NX)
				typedef intptr_t off_type;		// NX defined off_t as a 64-bit type even though size_t is 32-bit
			#else                                 
				typedef off_t off_type;
			#endif
		#endif


		/// EA_IO_SIZETYPE_SIZE
		///
		/// This allows conditional compilation. 
		///
		#ifndef EA_IO_SIZETYPE_SIZE
			#if defined(EAIO_64_BIT_SIZE_ENABLED) && EAIO_64_BIT_SIZE_ENABLED
				#define EA_IO_SIZETYPE_SIZE 8
			#else
				#define EA_IO_SIZETYPE_SIZE EA_PLATFORM_PTR_SIZE  // For systems with 32 bit addressing, this is 4.
			#endif
		#endif


		/// enum Attribute
		///
		/// Specifies basic volume/file/directory attributes.
		///
		enum Attribute
		{
			kAttributeNone       = 0x00,      /// Refers to no attribute in particular.
			kAttributeReadable   = 0x01,      /// The entity is readable. For multi-user systems, this refers to the current user.
			kAttributeWritable   = 0x02,      /// The entity is writable. For multi-user systems, this refers to the current user.
			kAttributeExecutable = 0x04,      /// The entity is executable. For multi-user systems, this refers to the current user.
			kAttributeDirectory  = 0x08,      /// The entity is a directory, as opposed to being a file.
			kAttributeAlias      = 0x10,      /// The entity is an alias (a.k.a. "symbolic link", or "shortcut").
			kAttributeArchive    = 0x20,      /// The entity has the archive attribute set.
			kAttributeHidden     = 0x40,      /// The entity is hidden. This is somewhat Microsoft-specific.
			kAttributeSystem     = 0x80       /// The entity is a system file. This is somewhat Microsoft-specific.
		};


		/// enum DirectoryEntryType
		///
		/// Defines different types of directory entries, such as (sub)directories and files.
		/// Values are powers of two so that they may also be used as flags.
		///
		enum DirectoryEntry
		{
			kDirectoryEntryNone       =  0x00,     /// No type in particular.
			kDirectoryEntryDirectory  =  0x01,     /// Refers to a directory. 
			kDirectoryEntryFile       =  0x02,     /// Refers to a file.
			kDirectoryEntryCurrent    =  0x04,     /// Refers to the "current" directory. Similar to the concept of "." on some file systems.
			kDirectoryEntryParent     =  0x08,     /// Refers to the "parent" directory of the current directory. Similar to the concetp of ".." on some file systems.
			kDirectoryEntryAll        =  0xff      /// This is a flag that refers to all types of file system entries.
		};


		/// enum FileTimeType
		///
		/// Defines a file attribute that is associated with a time. 
		///
		enum FileTimeType
		{
			kFileTimeTypeNone             = 0x0000, /// No time in particular.
			kFileTimeTypeCreation         = 0x0001, /// The time the file was created.
			kFileTimeTypeLastModification = 0x0002, /// The time the file was last modified.
			kFileTimeTypeLastAccess       = 0x0004  /// The time the file was last opened.
		};


		/// enum StatFlags
		/// 
		enum StatFlags
		{
			kStatFlagCreation         = kFileTimeTypeCreation,
			kStatFlagLastModification = kFileTimeTypeLastModification,
			kStatFlagLastAccess       = kFileTimeTypeLastAccess,
			kStatFlagSize             = 0x0100,
			kStatFlagAll              = 0x0107
		};

		/// StatData
		///
		struct StatData
		{
			uint32_t    mFlags;             /// Specifies which entries are valid. Input/output parameter.
			time_t      mCreationTime;      /// See enum FileTimeType.
			time_t      mModificationTime;  /// See enum FileTimeType.
			time_t      mAccessTime;        /// See enum FileTimeType.
			uint64_t    mSize;              /// Refers to the last saved size of the file.
		};


		/// enum CaseSensitivity
		/// 
		enum CaseSensitivity
		{
			kCaseSensitive,
			kCaseInsensitive,

			#if defined(_MSC_VER) || defined(__APPLE__)
				kCaseSensitiveDefault = kCaseInsensitive
			#else
				kCaseSensitiveDefault = kCaseSensitive
			#endif
		};


		/// enum FileSystem
		/// 
		/// Defines a file system. A file system has intrinsic attributes that relate
		/// to its organization and limitations. An well-known example of this is the 
		/// FAT file system's file name limitation to "8.3" names. Other attributes 
		/// include the file system's character set and encoding, maximum file count 
		/// per directory, maximum file size, and maximum file system size.
		///
		/// This enumeration is a little problematic because it covers both the concept
		/// of a disk format and an operating system API, yet the two are not always the
		/// same. For example, a disk format usually doesn't care about the concept of
		/// case sensitivity, whereas the operating sytem API cares very much. For any 
		/// given API function in the EAFile system, it should be made clear what if 
		/// any distinction matters.
		///
		enum FileSystem
		{
			kFileSystemNone,       /// No file system in particular.
			kFileSystemUnix,       /// Unix family of file systems. Various flavors, most of which support UTF8 Unicode path names.
			kFileSystemWindows,    /// Windows family of file systems, starting with Windows NT and later.
			kFileSystemFAT,        /// File Allocation Table. Used by small storage devices on multiple platforms.
			kFileSystemFATX,       /// File Allocation Table for XBox. Used by small storage devices on multiple platforms.
			kFileSystemFAT32,      /// 32 bit extentsion of FAT. Single byte and UCS2 Unicode path names.
			kFileSystemNTFS,       /// Windows NT File System. UCS2 Unicode path names.
			kFileSystemISO9660,    /// Basic CD file system, 31 character single byte path component names. See http://www.ecma-international.org/publications/files/ecma-st/Ecma-119.pdf
			kFileSystemJoliet,     /// An extension of ISO9660 which supports UCS2 Unicode in path component names. 
			kFileSystemUDF,        /// Universal Disk Format, used by modern CD and DVD systems. Specs are defined by http://www.osta.org/specs/
			kFileSystemUDFX,       /// Universal Disk Format for XBox, used by modern CD and DVD systems. Specs are defined by http://www.osta.org/specs/

			#if defined(EA_PLATFORM_WINDOWS)
				kFileSystemDefault = kFileSystemFAT32  /// Used to refer to whatever the default least-common demoninator file system for the platform is.
			#elif EAIO_USE_UNIX_IO
				kFileSystemDefault = kFileSystemUnix   /// Used to refer to whatever the default least-common demoninator file system for the platform is.
			#else
				kFileSystemDefault = kFileSystemFAT32  /// Used to refer to whatever the default least-common demoninator file system for the platform is.
			#endif
		};


		/// enum DriveType
		///
		/// Defines a disk or volume drive type. This is not the same thing as a FileSystem,
		/// as the DriveType defines the physical device used to store data and not necessarily
		/// the format of the data on that device. 
		///
		enum DriveType                 
		{
			kDriveTypeUnknown,    /// Unknown drive type.
			kDriveTypeFixed,      /// Fixed (normally internal) drives. FireWire and USB drives are normally considered 'fixed' drives, even though they are external to the computer enclosure.
			kDriveTypeCD,         /// Includes generic compact discs (CDs).
			kDriveTypeDVD,        /// Includes generic DVD discs. Similar to CDs but can store more data.
			kDriveTypeMemoryCard, /// Console memory cards as drives.
			kDriveTypeRAM,        /// A RAM disk is a virtual disk drive whose storage is implemented via RAM memory.
			kDriveTypeRemovable,  /// Generic removable drive. This encompasses removable drives not specifically enumerated.
			kDriveTypeRemote      /// Remote (normally network) drive.
		};


		/// enum SpecialDirectory
		///
		/// Defines directories that have special purposes for users or the 
		/// operating system.
		///
		/// This is mostly applicable to personal computer systems such as 
		/// PCs or Macintoshes and is less applicable to server systems such 
		/// as Linux and embedded or console systems such as PlayStation 
		/// gaming consoles.
		///
		enum SpecialDirectory
		{
			kSpecialDirectoryNone,                   /// This is a placeholder zero value.
			kSpecialDirectoryTemp,                   /// Windows example: "C:\Users\<username>\Temp\"
			kSpecialDirectoryOperatingSystem,        /// Windows example: "C:\Windows\"
			kSpecialDirectoryOperatingSystemTrash,   /// Windows example: "Recycle Bin"
			kSpecialDirectoryOperatingSystemFonts,   /// Windows example: "C:\WinNT\Fonts\"
			kSpecialDirectoryCurrentApplication,     /// Windows example: "C:\Program Files\Some App\"
			kSpecialDirectoryUserDesktop,            /// Windows example: "C:\Users\<username>\Desktop\"
			kSpecialDirectoryCommonDesktop,          /// Windows example: "C:\Users\All Users\Desktop\"
			kSpecialDirectoryUserApplicationData,    /// Windows example: "C:\Users\<username>\Application Data\"
			kSpecialDirectoryCommonApplicationData,  /// Windows example: "C:\Users\All Users\Application Data\"
			kSpecialDirectoryUserDocuments,          /// Windows example: "C:\Users\<username>\My Documents\"
			kSpecialDirectoryCommonDocuments,        /// Windows example: "C:\Users\All Users\Documents\"
			kSpecialDirectoryUserMusic,              /// Windows example: "C:\Users\<username>\My Documents\My Music\"
			kSpecialDirectoryCommonMusic,            /// Windows example: "C:\Users\All Users\Documents\My Music\"
			kSpecialDirectoryUserProgramsShortcuts,  /// Windows example: "C:\Users\<username>\Start Menu\Programs\"
			kSpecialDirectoryCommonProgramsShortcuts,/// Windows example: "C:\Users\All Users\Start Menu\Programs\"
			kSpecialDirectoryUserPictures,           /// Pictures directory, as is often found on mobile OSs and OS X.
			kSpecialDirectoryUserMovies,             /// Movies directory, as is often found on mobile OSs and OS X.
			kSpecialDirectoryDevScratch,             /// Available only in dev, not in retail. Example usage: log files and minidumps that are used during development. 
			kSpecialDirectoryDownloadData            /// Available only in retail for content not downloaded from 1st part
		};


		/// enum PathComponent
		///
		/// Defines a component of a path. This is useful for functionality that works on 
		/// the parsing or understanding of file or directory paths.
		///
		/// Note that directory paths in this EAFile/EADirectory system are defined as 
		/// ending with a directory separator. This is done because it makes it simpler
		/// to work with directories and makes the specification of what a directory 
		/// path is be more concise.
		///
		/// A drive is any prefix characters in the path up to but not including the first 
		/// directory separator. Thus in the path "c:\a\b\c" the drive is "c:". The UTF file 
		/// system treats drives and volumes equivalently. Thus in the UNC path "\\server\a\b\c" 
		/// the drive is "\\server".
		///
		/// A directory is the string of characters from the beginning of the path (including 
		/// an initial drive) up to the last path separator before a file name. If no file 
		/// name is present, the path is a directory path and ends with a path separator. 
		/// If no path separator was specified at the end of a directory path, it would be 
		/// ambiguous as to whether the path referred to a file or a directory. A directory 
		/// that refers to only the characters after the drive/volume is referred to as the 
		/// 'directory name alone', as with kPathComponentDirectoryAlone. 
		///
		/// A file is the string of all characters after the last directory separator in a 
		/// path string, including any characters after a dot ('.'). A file name that refers 
		/// to only the characters before the dot (i.e. extension) is referred to as the 
		/// 'file name alone', as with kPathComponentFileAlone.
		///
		/// A file extension is defined as the last dot ('.') and anything after it in the 
		/// file name. If no dot chars are present, there is no extension. If multiple dots 
		/// are present, the extension is the text from the last dot onward. If there is 
		/// nothing after the dot, the extension is simply ".".
		///
		enum PathComponent
		{
			kPathComponentNone           = 0x00,  /// No component in particular.
			kPathComponentDrive          = 0x01,  /// Also known as volume. Some platforms support volumes, some don't. An example for Windows is "C:" With respect to path strings, this includes the trailing ':' but not a path separator after it.
			kPathComponentDirectory      = 0x02,  /// Defines a directory. The standard convention is that this encompasses both drive and directory unless otherwise specified. With respect to path strings, this includes the trailing '/'. 
			kPathComponentDirectoryAlone = 0x04,  /// Same as kPathComponentDirectory except that for path specifications this refers to the directory without the leading drive or volume component.
			kPathComponentFile           = 0x08,  /// Defines a file name. The standard convention is that this encompasses the full file name (i.e. including the extension) unless otherwise specified.
			kPathComponentFileAlone      = 0x10,  /// Same as kPathComponentFile except that for path specifications this refers to the file name without the trailing extension component.
			kPathComponentFileExtension  = 0x20   /// Defines a file name extension alone. With respect to path strings, this includes the leading '.'. 
		};


		// Relative directory names
		//
		// Functions which recognize current and parent directories use them as defined here.
		// Functions which return current and parent directories return them as defined here.

		#if defined(EA_PLATFORM_MICROSOFT)
			#define EA_DIRECTORY_CURRENT_8             ".\\"
			#define EA_DIRECTORY_CURRENT_16  EA_CHAR16(".\\")
			#define EA_DIRECTORY_CURRENT_32  EA_CHAR32(".\\")
			#define EA_DIRECTORY_CURRENT_W    EA_WCHAR(".\\")

			#define EA_DIRECTORY_PARENT_8             "..\\"
			#define EA_DIRECTORY_PARENT_16  EA_CHAR16("..\\")
			#define EA_DIRECTORY_PARENT_32  EA_CHAR32("..\\")
			#define EA_DIRECTORY_PARENT_W    EA_WCHAR("..\\")
		#else
			#define EA_DIRECTORY_CURRENT_8             "./"
			#define EA_DIRECTORY_CURRENT_16  EA_CHAR16("./")
			#define EA_DIRECTORY_CURRENT_32  EA_CHAR32("./")
			#define EA_DIRECTORY_CURRENT_W    EA_WCHAR("./")

			#define EA_DIRECTORY_PARENT_8             "../"
			#define EA_DIRECTORY_PARENT_16  EA_CHAR16("../")
			#define EA_DIRECTORY_PARENT_32  EA_CHAR32("../")
			#define EA_DIRECTORY_PARENT_W    EA_WCHAR("../")
		#endif


		// Line end definitions
		//
		// There are two primary types of text line end implementations: Unix and Windows.
		// Most well-behaved software these days will deal with both types of line endings.
		// Nevertheless, we provide definitions here for the types.

		#define EA_LINEEND_WINDOWS_8               "\r\n"
		#define EA_LINEEND_WINDOWS_16    EA_CHAR16("\r\n")
		#define EA_LINEEND_WINDOWS_32    EA_CHAR32("\r\n")
		#define EA_LINEEND_WINDOWS_W      EA_WCHAR("\r\n")

		#define EA_LINEEND_UNIX_8                    "\n"
		#define EA_LINEEND_UNIX_16         EA_CHAR16("\n")
		#define EA_LINEEND_UNIX_32         EA_CHAR32("\n")
		#define EA_LINEEND_UNIX_W           EA_WCHAR("\n")

		#if EAIO_USE_UNIX_IO
			#define EA_LINEEND_NATIVE_8              "\n"
			#define EA_LINEEND_NATIVE_16   EA_CHAR16("\n")
			#define EA_LINEEND_NATIVE_32   EA_CHAR32("\n")
			#define EA_LINEEND_NATIVE_W     EA_WCHAR("\n")
		#else
			#define EA_LINEEND_NATIVE_8            "\r\n"
			#define EA_LINEEND_NATIVE_16 EA_CHAR16("\r\n")
			#define EA_LINEEND_NATIVE_32 EA_CHAR32("\r\n")
			#define EA_LINEEND_NATIVE_W   EA_WCHAR("\r\n")
		#endif


		/// EA_FILE_SYSTEM_CASE_SENSITIVE
		///
		/// Defines if the file system is case sensitive or case insensitive.
		///
		#ifndef EA_FILE_SYSTEM_CASE_SENSITIVE
			#if defined(_MSC_VER) || defined(__APPLE__)
				#define EA_FILE_SYSTEM_CASE_SENSITIVE 0
			#else
				#define EA_FILE_SYSTEM_CASE_SENSITIVE 1
			#endif
		#endif


		// File path delimiters
		//
		// We define these based on platform rather than file system because even 
		// though path separators are truly specific to a given file system, 
		// in practice a given platform tends to stick to a specific set of path
		// delimiters across its supported file systems. 
		//
		// Note that directory paths in this EAFile/EADirectory system are defined as 
		// ending with a directory separator. This is done because it makes it simpler
		// to work with directories and makes the specification of what a directory 
		// path is be more concise.
		//
		#if defined(EA_PLATFORM_MICROSOFT)
			#define                EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS    1

			#define                EA_FILE_PATH_HAS_DRIVE_SEPARATOR       1
			#define                EA_FILE_PATH_SEPARATOR_8               '\\'
			#define                EA_FILE_PATH_SEPARATOR_ALT_8           '/'
			#define                EA_FILE_PATH_DRIVE_SEPARATOR_8         ':'

			#define                EA_FILE_PATH_SEPARATOR_STRING_8        "\\"
			#define                EA_FILE_PATH_SEPARATOR_STRING_ALT_8    "/"
			#define                EA_FILE_PATH_DRIVE_SEPARATOR_STRING_8  ":"

			#define                EA_FILE_PATH_SEPARATOR_16              ((char16_t)('\\'))
			#define                EA_FILE_PATH_SEPARATOR_ALT_16          ((char16_t)('/'))
			#define                EA_FILE_PATH_DRIVE_SEPARATOR_16        ((char16_t)(':'))

			#if defined(EA_CHAR16)
				#define            EA_FILE_PATH_SEPARATOR_STRING_16       EA_CHAR16("\\")
				#define            EA_FILE_PATH_SEPARATOR_STRING_ALT_16   EA_CHAR16("/")
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16 EA_CHAR16(":")
			#endif

			#define                EA_FILE_PATH_SEPARATOR_32              ((char32_t)('\\'))
			#define                EA_FILE_PATH_SEPARATOR_ALT_32          ((char32_t)('/'))
			#define                EA_FILE_PATH_DRIVE_SEPARATOR_32        ((char32_t)(':'))

			#if defined(EA_CHAR32)
				#define            EA_FILE_PATH_SEPARATOR_STRING_32       EA_CHAR32("\\")
				#define            EA_FILE_PATH_SEPARATOR_STRING_ALT_32   EA_CHAR32("/")
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_32 EA_CHAR32(":")
			#endif

			inline bool IsFilePathSeparator(int c)
				{ return ((c == EA_FILE_PATH_SEPARATOR_8) || (c == EA_FILE_PATH_SEPARATOR_ALT_8)) || (c == EA_FILE_PATH_DRIVE_SEPARATOR_8); }
		#else
			#if defined(EA_PLATFORM_NX) || defined(EA_PLATFORM_SONY)
				#define            EA_FILE_PATH_HAS_DRIVE_SEPARATOR       1
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_8         ':'
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_8  ":"
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_16        ((char16_t)(':'))
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16 EA_CHAR16(":")
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_32        ((char32_t)(':'))
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_32 EA_CHAR32(":")
			#else
				#define            EA_FILE_PATH_HAS_DRIVE_SEPARATOR       0
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_8         '\0'
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_8  "\0"
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_16        ((char16_t)('\0'))
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16 EA_CHAR16("\0")
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_32        ((char32_t)('\0'))
				#define            EA_FILE_PATH_DRIVE_SEPARATOR_STRING_32 EA_CHAR32("\0")
			#endif

			#define                EA_FILE_PATH_SEPARATOR_TYPE_UNIX       1

			#define                EA_FILE_PATH_SEPARATOR_8               '/'
			#define                EA_FILE_PATH_SEPARATOR_ALT_8           '/'

			#define                EA_FILE_PATH_SEPARATOR_STRING_8        "/"
			#define                EA_FILE_PATH_SEPARATOR_STRING_ALT_8    "/"

			#define                EA_FILE_PATH_SEPARATOR_16              ((char16_t)('/'))
			#define                EA_FILE_PATH_SEPARATOR_ALT_16          ((char16_t)('/'))

			#if defined(EA_CHAR16)
				#define            EA_FILE_PATH_SEPARATOR_STRING_16       EA_CHAR16("/")
				#define            EA_FILE_PATH_SEPARATOR_STRING_ALT_16   EA_CHAR16("/")
			#endif

			#define                EA_FILE_PATH_SEPARATOR_32              ((char32_t)('/'))
			#define                EA_FILE_PATH_SEPARATOR_ALT_32          ((char32_t)('/'))

			#if defined(EA_CHAR32)
				#define            EA_FILE_PATH_SEPARATOR_STRING_32       EA_CHAR32("/")
				#define            EA_FILE_PATH_SEPARATOR_STRING_ALT_32   EA_CHAR32("/")
			#endif

			inline bool IsFilePathSeparator(int c)
				{ return (c == EA_FILE_PATH_SEPARATOR_8); }
		#endif

		#if EA_WCHAR_UNIQUE
			#if (EA_WCHAR_SIZE == 2)
				#define EA_FILE_PATH_SEPARATOR_W              ((wchar_t)EA_FILE_PATH_SEPARATOR_16)
				#define EA_FILE_PATH_SEPARATOR_ALT_W          ((wchar_t)EA_FILE_PATH_SEPARATOR_ALT_16)
				#define EA_FILE_PATH_DRIVE_SEPARATOR_W        ((wchar_t)EA_FILE_PATH_DRIVE_SEPARATOR_16)

				#define EA_FILE_PATH_SEPARATOR_STRING_W       ((const wchar_t *)EA_FILE_PATH_SEPARATOR_STRING_16)
				#define EA_FILE_PATH_SEPARATOR_STRING_ALT_W   ((const wchar_t *)EA_FILE_PATH_SEPARATOR_STRING_ALT_16)
				#define EA_FILE_PATH_DRIVE_SEPARATOR_STRING_W ((const wchar_t *)EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16)
			#elif (EA_WCHAR_SIZE == 4)
				#define EA_FILE_PATH_SEPARATOR_W              ((wchar_t)EA_FILE_PATH_SEPARATOR_32)
				#define EA_FILE_PATH_SEPARATOR_ALT_W          ((wchar_t)EA_FILE_PATH_SEPARATOR_ALT_32)
				#define EA_FILE_PATH_DRIVE_SEPARATOR_W        ((wchar_t)EA_FILE_PATH_DRIVE_SEPARATOR_32)

				#define EA_FILE_PATH_SEPARATOR_STRING_W       ((const wchar_t *)EA_FILE_PATH_SEPARATOR_STRING_32)
				#define EA_FILE_PATH_SEPARATOR_STRING_ALT_W   ((const wchar_t *)EA_FILE_PATH_SEPARATOR_STRING_ALT_32)
				#define EA_FILE_PATH_DRIVE_SEPARATOR_STRING_W ((const wchar_t *)EA_FILE_PATH_DRIVE_SEPARATOR_STRING_32)
			#endif
		#else
			#if (EA_WCHAR_SIZE == 1)
				#define EA_FILE_PATH_SEPARATOR_W              EA_FILE_PATH_SEPARATOR_8
				#define EA_FILE_PATH_SEPARATOR_ALT_W          EA_FILE_PATH_SEPARATOR_ALT_8
				#define EA_FILE_PATH_DRIVE_SEPARATOR_W        EA_FILE_PATH_DRIVE_SEPARATOR_8

				#define EA_FILE_PATH_SEPARATOR_STRING_W       EA_FILE_PATH_SEPARATOR_STRING_8
				#define EA_FILE_PATH_SEPARATOR_STRING_ALT_W   EA_FILE_PATH_SEPARATOR_STRING_ALT_8
				#define EA_FILE_PATH_DRIVE_SEPARATOR_STRING_W EA_FILE_PATH_DRIVE_SEPARATOR_STRING_8
			#elif (EA_WCHAR_SIZE == 2)
				#define EA_FILE_PATH_SEPARATOR_W              EA_FILE_PATH_SEPARATOR_16
				#define EA_FILE_PATH_SEPARATOR_ALT_W          EA_FILE_PATH_SEPARATOR_ALT_16
				#define EA_FILE_PATH_DRIVE_SEPARATOR_W        EA_FILE_PATH_DRIVE_SEPARATOR_16

				#define EA_FILE_PATH_SEPARATOR_STRING_W       EA_FILE_PATH_SEPARATOR_STRING_16
				#define EA_FILE_PATH_SEPARATOR_STRING_ALT_W   EA_FILE_PATH_SEPARATOR_STRING_ALT_16
				#define EA_FILE_PATH_DRIVE_SEPARATOR_STRING_W EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16
			#elif (EA_WCHAR_SIZE == 4)
				#define EA_FILE_PATH_SEPARATOR_W              EA_FILE_PATH_SEPARATOR_32
				#define EA_FILE_PATH_SEPARATOR_ALT_W          EA_FILE_PATH_SEPARATOR_ALT_32
				#define EA_FILE_PATH_DRIVE_SEPARATOR_W        EA_FILE_PATH_DRIVE_SEPARATOR_32

				#define EA_FILE_PATH_SEPARATOR_STRING_W       EA_FILE_PATH_SEPARATOR_STRING_32
				#define EA_FILE_PATH_SEPARATOR_STRING_ALT_W   EA_FILE_PATH_SEPARATOR_STRING_ALT_32
				#define EA_FILE_PATH_DRIVE_SEPARATOR_STRING_W EA_FILE_PATH_DRIVE_SEPARATOR_STRING_32
			#endif
		#endif


		// File path size constants
		//
		// It is often the case that the path component sizes (e.g. kMaxPathLength) 
		// differ between file systems supported by a given platform. In that case, 
		// these defines are useful for providing an upper limit which proper code 
		// must normally support. Thus if you want to read an arbitrary path into a     
		// buffer, you would want the buffer to be at least kMaxPathLength in size.
		//
		// The max value definitions (e.g. kMaxPathLength) include a terminating 
		// 0 char as part of the length. Thus a kMaxPathLength of 256 would mean
		// that the max path strlen would be 255. 
		//
		#if defined(EA_PLATFORM_MICROSOFT)
		  #if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
			const int              kMaxPathLength                    = 260;      // NTFS allows at least 32767 characters, but most of  the Windows API limits the value to 260.
			const int              kMaxDriveLength                   = 256;      // Drive paths may be quite long if UNC paths are involved.
			const int              kMaxDirectoryLength               = 256;      //
			const int              kMaxFileNameLength                = 256;      // 
			const int              kMaxDirectoryNameLength           = 256;      // 
			const int              kMaxExtensionLength               = 256;      // An extension includes the leading '.' char.
		  #else // XBox, others?
			const int              kMaxPathLength                    = 260;      // 
			const int              kMaxDriveLength                   = 256;      // 
			const int              kMaxDirectoryLength               = 256;      // 
			const int              kMaxFileNameLength                =  40;      // 
			const int              kMaxDirectoryNameLength           =  40;      // 
			const int              kMaxExtensionLength               =  40;      // 
		  #endif

			const int              kMaxVolumeNameLength              = 128;      // Volume name == drive name on Microsoft platforms.
			const int              kMaxVolumeSerialNumberLength      = 128;      // Volume name == drive name on Microsoft platforms.

		#else // defined(EA_PLATFORM_UNIX) // Includes Linux, Mac OSX, BSD, AIX, Solaris, iPhone
			#ifdef EA_PLATFORM_NX
				const int              kMaxPathLength                    = 260;
			#else
				const int              kMaxPathLength                    = 1024;            // This is actually dependent on the file system in place. Many have no limit. 1024 is long enough for us.
			#endif
			const int              kMaxDriveLength                   =    8;            // Unix doesn't support the concept of drives.
			const int              kMaxDirectoryLength               = kMaxPathLength;  // 
			const int              kMaxFileNameLength                = kMaxPathLength;  // 
			const int              kMaxDirectoryNameLength           = kMaxPathLength;  // 
			const int              kMaxExtensionLength               = kMaxPathLength;  // 

			const int              kMaxVolumeNameLength              =  255;  
			const int              kMaxVolumeSerialNumberLength      =  255;  

		#endif


		// Custom platform file system support
		//
		// Some platforms implement custom file systems. Usually these file systems are 
		// in addition to a regular file system and are often packed file systems (a file 
		// system within a file).

		#if defined(EA_PLATFORM_ANDROID)
			// Android has the concept of raw application assets, which is a file system packed into
			// distributed applications. This is accessed on the Android via the AssetManager interface,
			// but for consistency with other platforms, we refer to it as "app bundle". If the application
			// passes a path to EAIO that is prefixed by appbundle:/ then it will use the AssetManager 
			// file system and not the regular disk file system. 
			#define APPBUNDLE_PATH_PREFIX        "appbundle:/"
			#define APPBUNDLE_PATH_PREFIX_LENGTH 11             // strlen of "appbundle:/"
		#endif



		/// FilePathBuffer
		///
		/// Defines a character array which holds a path. It is the size of the 
		/// maximum defined path length for the given platform. It's often
		/// better to use PathString instead of FilePathBuffer.
		///
		typedef char  FilePathBuffer8[EA::IO::kMaxPathLength];
		typedef char16_t FilePathBuffer16[EA::IO::kMaxPathLength];
		typedef char32_t FilePathBuffer32[EA::IO::kMaxPathLength];

		#if EA_WCHAR_UNIQUE
			typedef wchar_t FilePathBufferW[EA::IO::kMaxPathLength];
		#else
			#if (EA_WCHAR_SIZE == 1)
				typedef FilePathBuffer8  FilePathBufferW;
			#elif (EA_WCHAR_SIZE == 2)
				typedef FilePathBuffer16 FilePathBufferW;
			#elif (EA_WCHAR_SIZE == 4)
				typedef FilePathBuffer32 FilePathBufferW;
			#endif
		#endif
		 
	} // namespace IO

} // namespace EA





///////////////////////////////////////////////////////////////////////
/// *** Deprecated ***
//
// The following have been replaced by #define equivalents which work
// better in practice.
///////////////////////////////////////////////////////////////////////
namespace EA
{
	namespace IO
	{
		const char  kLineEndUnix8[2]     = { '\n', 0 };
		const char16_t kLineEndUnix16[2]    = { '\n', 0 };
		const char32_t kLineEndUnix32[2]    = { '\n', 0 };

		const char  kLineEndWindows8[3]  = { '\r', '\n', 0 };
		const char16_t kLineEndWindows16[3] = { '\r', '\n', 0 };
		const char32_t kLineEndWindows32[3] = { '\r', '\n', 0 };

		#if EAIO_USE_UNIX_IO
			#define kLineEndNative8  kLineEndUnix8
			#define kLineEndNative16 kLineEndUnix16
			#define kLineEndNative32 kLineEndUnix32
		#else
			#define kLineEndNative8  kLineEndWindows8
			#define kLineEndNative16 kLineEndWindows16
			#define kLineEndNative32 kLineEndWindows32
		#endif

		#if (defined(EA_PLATFORM_UNIX) && !defined(EA_PLATFORM_OSX))
			const bool kFileSystemCaseSensitive = true;
		#else
			const bool kFileSystemCaseSensitive = false;
		#endif

		#if defined(EA_PLATFORM_MICROSOFT)
			const char             kFilePathSeparator8               = '\\';
			const char*      const kFilePathSeparatorString8         = "\\";
			const char             kFilePathSeparatorAlt8            = '/';      // Windows allows two different kinds of path separators: \ and /
			const char*      const kFilePathSeparatorAltString8      = "/";
			const char             kFilePathDriveSeparator8          = ':';
			const char*      const kFilePathDriveSeparatorString8    = ":";

			const char16_t         kFilePathSeparator16              = '\\';
			const char16_t         kFilePathSeparatorAlt16           =           '/';
			const char16_t         kFilePathDriveSeparator16         = ':';

			#if defined(EA_CHAR16)
			const char16_t*  const kFilePathSeparatorString16        = EA_CHAR16("\\");
			const char16_t*  const kFilePathSeparatorAltString16     = EA_CHAR16("/");
			const char16_t*  const kFilePathDriveSeparatorString16   = EA_CHAR16(":");
			#endif

			const char32_t         kFilePathSeparator32              = '\\';
			const char32_t         kFilePathSeparatorAlt32           = '/';
			const char32_t         kFilePathDriveSeparator32         = ':';

			#if defined(EA_CHAR32)
			const char32_t*  const kFilePathSeparatorString32        = EA_CHAR32("\\");
			const char32_t*  const kFilePathSeparatorAltString32     = EA_CHAR32("/");
			const char32_t*  const kFilePathDriveSeparatorString32   = EA_CHAR32(":");
			#endif

		#else // defined(EA_PLATFORM_UNIX) // Includes Linux, Mac OSX, BSD, AIX, Solaris, iPhone
			const char             kFilePathSeparator8               = '/';
			const char*      const kFilePathSeparatorString8         = "/";
			const char             kFilePathSeparatorAlt8            = '/';
			const char*      const kFilePathSeparatorAltString8      = "/";
			const char             kFilePathDriveSeparator8          = '\0';  // Unix doesn't support the concept of drives.
			const char*      const kFilePathDriveSeparatorString8    = "\0";

			#if (EA_WCHAR_SIZE == 2) // If we can use the L string prefix for 16 bit strings..
			const char16_t         kFilePathSeparator16              =           '/';
			const char16_t*  const kFilePathSeparatorString16        = EA_CHAR16("/");
			const char16_t         kFilePathSeparatorAlt16           =           '/';
			const char16_t*  const kFilePathSeparatorAltString16     = EA_CHAR16("/");
			const char16_t         kFilePathDriveSeparator16         =           '\0';
			const char16_t*  const kFilePathDriveSeparatorString16   = EA_CHAR16("\0");

			const char32_t         kFilePathSeparator32              =   '/';
			const char32_t         kFilePathSeparatorString32[]      = { '/', 0 };
			const char32_t         kFilePathSeparatorAlt32           =   '/';
			const char32_t*  const kFilePathSeparatorAltString32     = kFilePathSeparatorString32;
			const char32_t         kFilePathDriveSeparator32         = '\0';
			const char32_t         kFilePathDriveSeparatorString32[] = { 0 };
			#else
			const char16_t         kFilePathSeparator16              =   '/';
			const char16_t         kFilePathSeparatorString16[]      = { '/', 0 };
			const char16_t         kFilePathSeparatorAlt16           =   '/';
			const char16_t*  const kFilePathSeparatorAltString16     = kFilePathSeparatorString16;
			const char16_t         kFilePathDriveSeparator16         = '\0';
			const char16_t         kFilePathDriveSeparatorString16[] = { 0 };

			const char32_t         kFilePathSeparator32              =           '/';
			const char32_t*  const kFilePathSeparatorString32        = EA_CHAR32("/");
			const char32_t         kFilePathSeparatorAlt32           =           '/';
			const char32_t*  const kFilePathSeparatorAltString32     = EA_CHAR32("/");
			const char32_t         kFilePathDriveSeparator32         =           '\0';
			const char32_t*  const kFilePathDriveSeparatorString32   = EA_CHAR32("\0");
		#endif
		#endif

	} // namespace IO

} // namespace EA


#endif // Header include guard







