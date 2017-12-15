/*
Copyright (C) 2003,2009,2010,2012,2013,2014 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAFileBase.h
// 
// Created by Paul Pedriana - 2003
//
// This file defines base information about file systems for various platforms.
///////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EAFILEBASE_H) && !defined(FOUNDATION_EAFILEBASE_H)
#define EAIO_EAFILEBASE_H
#define FOUNDATION_EAFILEBASE_H


#include <EAIO/internal/Config.h>
#include <EABase/eabase.h>
#include <stddef.h>
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
        #if EAIO_64_BIT_SIZE_ENABLED
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
        #if EAIO_64_BIT_SIZE_ENABLED
            typedef int64_t off_type;
        #else
            #if defined(EA_PLATFORM_WIN64)
                typedef intptr_t off_type;        // Win64 defines off_t as 32 bit long even though it 
            #else                                 // defines size_t as 64 bit uint64_t.
                typedef off_t off_type;
            #endif
        #endif


        /// EA_IO_SIZETYPE_SIZE
        ///
        /// This allows conditional compilation. 
        ///
        #ifndef EA_IO_SIZETYPE_SIZE
            #if EAIO_64_BIT_SIZE_ENABLED
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
            kFileTimeTypeNone             = 0x00, /// No time in particular.
            kFileTimeTypeCreation         = 0x01, /// The time the file was created.
            kFileTimeTypeLastModification = 0x02, /// The time the file was last modified.
            kFileTimeTypeLastAccess       = 0x04  /// The time the file was last opened.
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

                kFileSystemDefault = kFileSystemFAT32  /// Used to refer to whatever the default least-common demoninator file system for the platform is.
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
        /// as Linux and embedded or console systems. 
        ///
        enum SpecialDirectory
        {
            kSpecialDirectoryNone,                   /// This is a placeholder zero value.
            kSpecialDirectoryTemp,                   /// Windows example: "C:\WinNT\Temp\"
            kSpecialDirectoryOperatingSystem,        /// Windows example: "C:\WinNT\"
            kSpecialDirectoryOperatingSystemTrash,   /// Windows example: "Recycle Bin"
            kSpecialDirectoryOperatingSystemFonts,   /// Windows example: "C:\WinNT\Fonts\"
            kSpecialDirectoryCurrentApplication,     /// Windows example: "C:\Program Files\Some App\"
            kSpecialDirectoryUserDesktop,            /// Windows example: "C:\Documents and Settings\<username>\Desktop\"
            kSpecialDirectoryCommonDesktop,          /// Windows example: "C:\Documents and Settings\All Users\Desktop\"
            kSpecialDirectoryUserApplicationData,    /// Windows example: "C:\Documents and Settings\<username>\Application Data\"
            kSpecialDirectoryCommonApplicationData,  /// Windows example: "C:\Documents and Settings\All Users\Application Data\"
            kSpecialDirectoryUserDocuments,          /// Windows example: "C:\Documents and Settings\<username>\My Documents\"
            kSpecialDirectoryCommonDocuments,        /// Windows example: "C:\Documents and Settings\All Users\Documents\"
            kSpecialDirectoryUserMusic,              /// Windows example: "C:\Documents and Settings\<username>\My Documents\My Music\"
            kSpecialDirectoryCommonMusic,            /// Windows example: "C:\Documents and Settings\All Users\Documents\My Music\"
            kSpecialDirectoryUserProgramsShortcuts,  /// Windows example: "C:\Documents and Settings\<username>\Start Menu\Programs\"
            kSpecialDirectoryCommonProgramsShortcuts /// Windows example: "C:\Documents and Settings\All Users\Start Menu\Programs\"
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

            #define EA_DIRECTORY_CURRENT_8    ".\\"
            #define EA_DIRECTORY_CURRENT_16  L".\\"

            #define EA_DIRECTORY_PARENT_8    "..\\"
            #define EA_DIRECTORY_PARENT_16  L"..\\"


        // Line end definitions
        //
        // There are two primary types of text line end implementations: Unix and Windows.
        // Most well-behaved software these days will deal with both types of line endings.
        // Nevertheless, we provide definitions here for the types.

        #define EA_LINEEND_WINDOWS_8   "\r\n"
        #define EA_LINEEND_WINDOWS_16 L"\r\n"

        #define EA_LINEEND_UNIX_8      "\n"
        #define EA_LINEEND_UNIX_16    L"\n"  // We assume the user is compiling with 16 bit wchar_t.

        // This means Unix *style*; it is applicable on any platfom:
        const char8_t  kLineEndUnix8[2]     = { '\n', 0 };
        const char16_t kLineEndUnix16[2]    = { '\n', 0 };

        // This means Windows *style*; it is applicable on any platfom:
        const char8_t  kLineEndWindows8[3]  = { '\r', '\n', 0 };
        const char16_t kLineEndWindows16[3] = { '\r', '\n', 0 };

            #define kLineEndNative8  kLineEndWindows8
            #define kLineEndNative16 kLineEndWindows16

            #define EA_LINEEND_NATIVE_8   "\r\n"
            #define EA_LINEEND_NATIVE_16 L"\r\n"


        /// kFileSystemCaseSensitive
        ///
        /// Defines if the file system is case sensitive or case insensitive.
        ///
            const bool kFileSystemCaseSensitive = false;

            #ifndef EA_FILE_SYSTEM_CASE_SENSITIVE
                #define EA_FILE_SYSTEM_CASE_SENSITIVE 0
            #endif


        // File path delimiters
        //
        // We define these based on platform rather than file system because even 
        // though path separators are truly specific to a given file system, 
        // in practice a given platform tends to stick to a specific set of path
        // delimiters across its supported file systems. However, it is often the 
        // case that the path component sizes (e.g. kMaxPathLength) differ between
        // file systems supported by a given platform. In that case, these defines
        // are useful for providing an upper limit which proper code must normally
        // support. Thus if you want to read an arbitrary path into a buffer, you
        // would want the buffer to be at least kMaxPathLength in size.
        //
        // The max value definitions (e.g. kMaxPathLength) include a terminating 
        // 0 char as part of the length. Thus a kMaxPathLength of 256 would mean
        // that the max path strlen would be 255. 
        //
        // Note that directory paths in this EAFile/EADirectory system are defined as 
        // ending with a directory separator. This is done because it makes it simpler
        // to work with directories and makes the specification of what a directory 
        // path is be more concise.
        //
            #define                EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS    1
            #define                EA_FILE_PATH_SEPARATOR_8               '\\'
            #define                EA_FILE_PATH_SEPARATOR_STRING_8        "\\"
            #define                EA_FILE_PATH_SEPARATOR_ALT_8           '/'
            #define                EA_FILE_PATH_SEPARATOR_STRING_ALT_8    "/"
            #define                EA_FILE_PATH_DRIVE_SEPARATOR_8         ':'
            #define                EA_FILE_PATH_DRIVE_SEPARATOR_STRING_8  ":"

            #define                EA_FILE_PATH_SEPARATOR_16              L'\\'
            #define                EA_FILE_PATH_SEPARATOR_STRING_16       L"\\"
            #define                EA_FILE_PATH_SEPARATOR_ALT_16          L'/'
            #define                EA_FILE_PATH_SEPARATOR_STRING_ALT_16   L"/"
            #define                EA_FILE_PATH_DRIVE_SEPARATOR_16        L':'
            #define                EA_FILE_PATH_DRIVE_SEPARATOR_STRING_16 L":"

            // The following const-based definitions are generally less desirable, due to the 
            // way compilers work. We may want to deprecate them in favor of the above macros.
            const char             kFilePathSeparator8               = '\\';
            const char*      const kFilePathSeparatorString8         = "\\";
            const char             kFilePathSeparatorAlt8            = '/';      // Windows allows two different kinds of path separators: \ and /
            const char*      const kFilePathSeparatorAltString8      = "/";
            const char             kFilePathDriveSeparator8          = ':';
            const char*      const kFilePathDriveSeparatorString8    = ":";

            const char16_t         kFilePathSeparator16              = '\\';
            const char16_t*  const kFilePathSeparatorString16        = L"\\";
            const char16_t         kFilePathSeparatorAlt16           = '/';
            const char16_t*  const kFilePathSeparatorAltString16     = L"/";
            const char16_t         kFilePathDriveSeparator16         = ':';
            const char16_t*  const kFilePathDriveSeparatorString16   = L":";

            const int              kMaxPathLength                    = 260;      // NTFS allows at least 32767 characters, but most of  the Windows API limits the value to 260.
            const int              kMaxDriveLength                   = 256;      // Drive paths may be quite long if UNC paths are involved.
            const int              kMaxDirectoryLength               = 256;      
            const int              kMaxFileNameLength                = 256;      // 
            const int              kMaxDirectoryNameLength           = 256;      // 
            const int              kMaxExtensionLength               = 256;      // 

            const int              kMaxVolumeNameLength              = 128;      // Volume name == drive name on Microsoft platforms.
            const int              kMaxVolumeSerialNumberLength      = 128;      // Volume name == drive name on Microsoft platforms.

            inline bool IsFilePathSeparator(int c)
                { return ((c == (int)kFilePathSeparator8) || (c == (int)kFilePathSeparatorAlt8)) || (c == (int)kFilePathDriveSeparator8); }



        /// FilePathBuffer
        ///
        /// Defines a character array which holds a path. It is the size of the 
        /// maximum defined path length for the given platform.
        ///
        typedef char8_t  FilePathBuffer8[kMaxPathLength];
        typedef char16_t FilePathBuffer16[kMaxPathLength];


    } // namespace IO

} // namespace EA


#endif // Header include guard







