/*
Copyright (C) 2001 Electronic Arts, Inc.  All rights reserved.

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
// EAIniFile.h
//
// Programmers: Paul Pedriana, Alex Bilyk
//
// Implements a .ini file, as commonly seen on the Windows platform.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAINIFILE_H
#define EAIO_EAINIFILE_H


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#include <EASTL/core_allocator_adapter.h>
#include <EASTL/map.h>
#include <EASTL/fixed_string.h>
#include <coreallocator/icoreallocator_interface.h>


#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251) // class needs to have dll-interface to be used by clients
#endif


#if EAIO_INIFILE_ENABLED


namespace EA
{
    namespace IO
    {
        typedef eastl::fixed_string<char8_t,  256, true, Allocator::EAIOEASTLCoreAllocator> String8;
        typedef eastl::fixed_string<char16_t, 256, true, Allocator::EAIOEASTLCoreAllocator> String16;


        ///////////////////////////////////////////////////////////////////////
        /// class IniFile
        ///
        /// This class implements '.ini' file manipulation. These are files that use
        /// a format like the Windows '.ini' file format. This class's implementation
        /// of ini files is one or two orders of magnitude faster than the implementation
        /// present in the Windows ini file manipulation API. 
        ///
        /// Here are example file contents:
        ///      [Admin]
        ///      Company = Electronic Arts
        ///      Version = 1.0
        ///      Enabled = On
        ///      ; Comment string
        ///
        ///      [Users]
        ///      Joe = First
        ///
        /// The "[Admin]" part is a section called "Admin".
        /// The "Company" part is a key called "Company".
        /// The "Electronic Arts" part is a value called "Electronic Arts".
        /// The "; Comment string" part is a comment line that has no effect.
        ///
        /// Some format specification details:
        ///    - Leading and trailing space is removed from section, key, and value strings returned to the user.
        ///    - If you want to preserve leading and trailing space, surround the text with quote characters. 
        ///    - Spaces are valid in section, key, and value strings.
        ///    - The only reserved characters are the [] characters in the section string and the = char in the key string. 
        ///    - The key = section sequence need not have spaces between the key and = and the = and section. 
        ///    - Section and key strings are case-insensitive when the user is doing reads. 
        ///    - The above-mentioned case-sensitivity is applied only to ASCII characters only.
        ///    - Newlines can be in either Unix format ("\r") or Windows format ("\r\n").
        ///    - Supported text encodings for .ini files include ASCII, UTF8 Unicode, and UTF16 Unicode.
        ///
        /// Example usage:
        ///    IniFile iniFile(L"/somedir/somefile.ini");      // Note that when using GCC you need to compile with -fshort-wchar
        ///    
        ///    String16 sValue;
        ///    if(iniFile.ReadEntry(L"User", L"Name", sValue) > 0)
        ///        printf("User name is %ls.", sValue.c_str());
        ///    
        ///    int rating;
        ///    if(iniFile.ReadEntryFormatted(L"User", L"Rating", L"%d", &rating) > 0)
        ///        printf("User rating is %d.", rating);
        ///    
        ///    BoardBitmapData bbd;
        ///    if(iniFile.ReadBinary(L"Board", L"Bitmap", &bbd, sizeof(bbd));
        ///        printf("Board bitmap binary data read successfully.");
        ///    
        ///////////////////////////////////////////////////////////////////////
        ///
        class EAIO_API IniFile
        {
        public:
            /// Options
            /// Specifies options which control the behaviour of this class.
            enum Options
            {
                kOptionNone,
                kOptionLeaveFileOpen,   /// Enabled by default. Setting to 1 allows for faster but non-shared access, but setting to 0 allows slower but shareable access.
                kOptionBusyWaitTime     /// Zero by default. The number of milliseconds to wait for an un-openable file to open. It sometimes happens that shared contention for an ini file causes a reader to not be able to read the file right away.
            };

            /// IniFile
            IniFile(const char16_t* pPath = NULL, int optionFlags = kOptionLeaveFileOpen, EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);

            /// IniFile
            IniFile(EA::IO::IStream* pStream, EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);

            /// ~IniFile
            /// This destructor will not close the stream if the stream was user-specified.
            virtual ~IniFile();

            /// SetAllocator
            /// Sets the memory allocator to use with this class.
            /// This allocator is used to allocate mSectionPositionMap and mSectionNameMap elements.
            /// This function must be called before any calls to WriteEntry and LoadSectionNames.
            virtual void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);
            
            /// GetOption
            /// Returns the value of a given option. The input 'option' is one of enum Option.
            /// The data type of the input 'option' is of type int instead of type Option because
            /// int allows the flexibility of overriding this class with additonal options.
            virtual int GetOption(int option) const;

            /// SetOption
            /// Sets a given option to a value. 
            virtual void SetOption(int option, int value);

            /// GetPath
            /// May return an empty string. Will not return a NULL pointer.
            virtual const char16_t* GetPath() const;

            /// SetPath
            /// Sets the path if the stream is not in use.
            virtual bool SetPath(const char16_t* pPath);

            /// GetStream
            virtual EA::IO::IStream* GetStream() const;

            /// SetStream
            /// Sets the stream to use for read/write. This stream is not freed 
            /// by the IniFile object; the caller owns the stream.
            virtual bool SetStream(EA::IO::IStream* pStream);

            /// Open
            /// Allows you to explcitly open the file with the given permissions. 
            /// If you plan on writing as well as reading then you might want to 
            /// pre-open the file with kAccessFlagReadWrite.
            /// If the file is already open, this function has no effect.
            virtual bool Open(int nAccessFlags = IO::kAccessFlagRead);

            /// Close
            /// Close the ini file and flush all knowledge of it.
            /// There is no corresponding Open because the read/write functions
            /// cause opening to happen as needed.
            virtual bool Close();

            /// ReadEntry
            /// Reads the value specified by the given section and key into pValue.
            /// Returns the length of the value. If the section and key are not
            /// present, the return value is -1. In the case that a 
            /// key is present but the value is empty, the return value is zero.
            /// If the input nValueLength is not large enough to hold the value,
            /// the copy will be truncated and the return value should be consulted
            /// to tell if there wasn't enough space.
            /// If the value was not present, the output parameter pValue is left unchanged. 
            /// If the parameter pValue is NULL, the return value indicates wheither the 
            /// value specified by the section and key is present.
            virtual int ReadEntry(const char16_t* pSection, const char16_t* pKey, String16& sValue);

            virtual int ReadEntryFormatted(const char16_t* pSection, const char16_t* pKey, const char16_t* pValueFormat, ...);

            /// WriteEntry
            /// Writes the given value to the given section and key, possibly overwriting
            /// an existing value. If the section doesn't already exist, it is created.
            /// If the key doesn't already exist, it is created.
            /// The return value is true if the value could be written.
            virtual bool WriteEntry(const char16_t* pSection, const char16_t* pKey, const char16_t* pValue);

            virtual bool WriteEntryFormatted(const char16_t* pSection, const char16_t* pKey, const char16_t* pValueFormat, ...);

            /// ReadBinary
            /// Reads a binary value for a given section and key. 
            /// The format of the text value is a hex string.
            /// If pData is NULL, the return value is the required size for pData.
            /// Returns the length of the value. If the section and key are not
            /// present, the return value is -1. In the case that a 
            /// key is present but the value is empty, the return value is zero.
            virtual int ReadBinary(const char16_t* pSection, const char16_t* pKey, void* pData, size_t nDataLength);

            /// WriteBinary
            /// Writes a binary value to a given section and key. 
            /// The format of the text value is a hex string.
            /// Returns true if the operation was possible.
            /// If you think like INI files are just the place to store graphics data, do some more thinking!
            virtual bool WriteBinary(const char16_t* pSection, const char16_t* pKey, const void* pData, size_t nDataLength);

            /// IniFileCallback
            /// Defines a callback function the user supplies for IniFile enumeration callbacks.
            typedef bool (*EnumCallbackFunction)(const char16_t*, const char16_t*, void* pContext);

            /// EnumSections
            /// Enumerates all sections. The section is passed back in the first 
            /// parameter of the callback function.
            /// Returns the number of entries present or -1 upon error.
            virtual int EnumSections(EnumCallbackFunction pFunction, void* pContext = NULL);

            /// EnumEntries
            /// Enumerates all entries (key/value pairs) of a given section. Key is passed as the 
            /// first parameter of pFunction with value being the second.
            /// Returns the number of entries present or -1 upon error.
            virtual int EnumEntries(const char16_t* pSection, EnumCallbackFunction pFunction, void* pContext = NULL);

            /// GetSectionExists
            /// Returns true if the section exists.
            virtual bool SectionExists(const char16_t* pSection);

        protected:
            virtual int  GetEncoding();
            virtual bool LoadSectionNames(int nAccessFlags);
            virtual bool GetFileLine8To8(String8& sLine);
            virtual bool GetFileLine16To16(String16& sLine);
            virtual bool GetFileLine(String16& sLine);
            virtual bool ConvertAndWriteStream(const char16_t* pchar, size_t size);
            virtual int  ReadEntry(const char16_t* pSection, const char16_t* pKey, char16_t* pValue, size_t nValueLength);

            // Member typedef
            typedef eastl::pair<const String16, IO::off_type> PositionMapPair;
            typedef eastl::map<String16, IO::off_type, eastl::less<String16>, Allocator::EAIOEASTLCoreAllocator> SectionLowerToPositionMap;

            typedef eastl::pair<const String16, String16> SectionMapPair;
            typedef eastl::map<String16, String16, eastl::less<String16>, Allocator::EAIOEASTLCoreAllocator> SectionLowerToSectionMap;

            // Member data
            char16_t                    mPath[EA::IO::kMaxPathLength];      ///< The path to an ini file. May be empty if we are using an externally specified stream.
            EA::IO::FileStream          mFileStream;                        ///< The stream to an ini file.
            EA::IO::IStream*            mpStream;                           ///< The stream we use for operations; may point to mFileStream.
            int                         mnEncodingSrc;                      ///< The encoding of the text in the source.
            bool                        mbFileIsOpenForWriting;             ///< True if file is open in write mode. Else read mode.
            bool                        mbLeaveFileOpenBetweenOperations;   ///< This allows for faster access, but not shared acess.
            bool                        mbReadEntryCacheReady;              ///< ReadEntry caching
            uint32_t                    mFileBusyWaitMs;                    ///< The number of milliseconds to wait for an un-openable file to open. Defaults to zero. It sometimes happens that shared contention for an ini file causes a reader to not be able to read the file right away.
            SectionLowerToPositionMap   mSectionPositionMap;                ///< Map of sections names to starting position in file.
            SectionLowerToSectionMap    mSectionNameMap;                    ///< Converts section names from stored lower case to actual case.
        };

    } // namespace IO

} // namespace EA


#endif // EAIO_INIFILE_ENABLED

#endif // Header include guard


#ifdef _MSC_VER
    #pragma warning(pop)
#endif

