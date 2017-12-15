///////////////////////////////////////////////////////////////////////////////
// EAIniFile.cpp
//
// Copyright (c) 2001, 2013 Electronic Arts Inc. All rights reserved.
// Programmer: Paul Pedriana, Alex Bilyk
//
// Implements a portable .ini file, like those commonly seen on the Windows platform.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAIniFile.h>
#include <EAIO/Allocator.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAStream.h>
#include <EAIO/FnEncode.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#if EAIO_THREAD_SAFETY_ENABLED
    #include <eathread/eathread.h>
#endif
#include EA_ASSERT_HEADER
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

#if EAIO_INIFILE_ENABLED



#if defined(_MSC_VER) || defined(CS_UNDEFINED_STRING) || (defined(__GNUC__) && (__GNUC__ < 3))
    // Microsoft doesn't provide vsscanf/vswscanf.
    // Metrowerks doesn't provide vswscanf.
    // GCC 2.x doesn't provide vswscanf.
    #define EAINIFILE_CUSTOM_VSSCANF
#endif


// If building a DLL, we have to provide a linkable version of the following functions:
#ifdef EA_DLL
    int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
    {
        #ifdef _MSC_VER
            return _vsnprintf(pDestination, n, pFormat, arguments);
        #else
            return vsprintf(pDestination, n, pFormat, arguments);
        #endif
    }

    int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
    {
        #ifdef _MSC_VER
            return _vsnwprintf(pDestination, n, pFormat, arguments);
        #else
            return vswprintf(pDestination, n, pFormat, arguments);
        #endif
    }
#endif


namespace EA{

namespace IO{


namespace IFLocal // Local namespace
{
    // We define these as character arrays instead of strings because some compilers
    // set wchar_t strings to be 32 bits (char32_t) instead of char16_t.
        const char16_t kTextFileNewLineString[] = { '\r', '\n', 0 };
    const char16_t kSectionFormat[] = { '[', '%', 's', ']', '%', 's', 0 }; // "[%s]%s"
    const char16_t kKeyFormat[]     = { '%', 'l', 's', ' ', '=', ' ', '%', 'l', 's', '%', 'l', 's', 0 }; // %ls = %ls%ls
    const char16_t kValueFormat[]   = { '%', 's', 0 }; // "%s"


    enum Encoding
    {
        kEncodingUTF8 = 8,
        kEncodingUTF16 = 16,
        kEncodingUTF32 = 32,
        kEncodingUCS2LittleEndian = 1200,
        kEncodingUCS2BigEndian = 1201,
        kEncodingUCS4LittleEndian  = 1202,
        kEncodingUCS4BigEndian = 1203
    };


    static inline int GetLocalUCS2Encoding()
    {
        #ifdef EA_SYSTEM_LITTLE_ENDIAN
            return kEncodingUCS2LittleEndian;
        #else
            return kEncodingUCS2BigEndian;
        #endif
    }

    static size_t GetCharacterSize(int nEncoding)
    {
        switch(nEncoding)
        {
            case kEncodingUTF16:
            case kEncodingUCS2LittleEndian:
            case kEncodingUCS2BigEndian:
                return 2;
            case kEncodingUTF32:
                return 4;
        }
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // DetectEncoding
    //
    static Encoding DetectEncoding(const void* pText, size_t nSize, const int* /*pEncodingOptions*/ = 0, size_t /*nEncodingOptionsSize*/ = 0)
    {
        Encoding encoding = kEncodingUTF8;

        if(nSize > 1)
        {
            const unsigned char* pText8 = (const unsigned char*)pText;

            // Check for standard byte ordering markers.
            if(nSize >= 2)
            {
                if((pText8[0] == 0xfe) && (pText8[1] == 0xff))
                    return kEncodingUCS2BigEndian;
                if((pText8[0] == 0xff) && (pText8[1] == 0xfe))
                    return kEncodingUCS2LittleEndian;
                if((nSize >= 3) && (pText8[0] == 0xef) && (pText8[1] == 0xbb) && (pText8[2] == 0xbf)) // U+FEFF in UTF-8 -ab
                    return kEncodingUTF8;
            } 

            // Do a statistical analyis of the data.
            size_t nZeroCount(0);
            size_t nOddZeroCount(0);
            size_t nEvenZeroCount(0);
            size_t nASCIICount(0);
            size_t nUTF8LeadByteCount(0);
            size_t nUTF8FollowByteCount(0);

            for(size_t i(0); i < nSize; i++)
            {
                if(pText8[i] == 0)
                {
                    nZeroCount++;
                    if(i & 1)
                        nOddZeroCount++;
                    else
                        nEvenZeroCount++;
                }
                else if(pText8[i] <= 0x7f)
                    nASCIICount++;
                else if((pText8[i] >= 0xc2) && (pText8[i] < 0xf0))
                    nUTF8LeadByteCount++;
                else if((pText8[i] >= 0x80) && (pText8[i] <= 0xbf))
                    nUTF8FollowByteCount++;
            }
            
            if(nASCIICount == nSize) // all ASCII? report it as UTF-8
                return kEncodingUTF8;

            // If we have a lot of zeroes, then we assume that we have a wide character encoding such as UTF16.
            else if(nZeroCount > (nSize / 4)) // If there are a significant number of zeroes...
            {
                if((nEvenZeroCount > (nSize / 8)) && (nOddZeroCount > (nSize / 8))) //If we have back-to-back zeroes...
                {
                    if(pText8[0] == 0) // Ideally we'd check for more than just the first byte...
                        encoding = kEncodingUCS4BigEndian;
                    else
                        encoding = kEncodingUCS4LittleEndian;
                }
                else
                {
                    if(nEvenZeroCount > nOddZeroCount)
                        encoding = kEncodingUCS2BigEndian;
                    else
                        encoding = kEncodingUCS2LittleEndian;
                }
            }
            else // Else we have either one byte per character or we have wide character encoding with predominantly European characters.
            {
                if((nZeroCount > 20) && nUTF8LeadByteCount < (nSize / 4)) // If there are at least some wide ASCII characters and we have few or no UTF8 characters..
                {
                    // We have 16 bit encoding, most likely.
                    // It's impossible to tell endian-ness with some examples of 16 bit text.
                    encoding = kEncodingUCS2LittleEndian; // For now we just pick one.
                }
            }
        }

        // Todo: Implement pEncodingOptions

        return encoding;
    }


    ///////////////////////////////////////////////////////////////////////////
    // StackArray
    // Todo: Consider replacing this code with EAFixedBufferAllocator.h:
    //
    template <typename T, size_t initial_size>
    class StackArray
    {
        typedef T element_type;
        T      mLocalArray[initial_size];
        T*     mpArray;
        T*     mpArrayEx;
        size_t mSize;

        void reset()
        {
            if (mpArray != &mLocalArray[0] && mpArrayEx != mpArray )
            {
                EA::Allocator::ICoreAllocator* const pAllocator = EA::IO::GetAllocator();
                EA_ASSERT(pAllocator);  // Have you called SetAllocator?

                pAllocator->Free(mpArray);
                mpArray = &mLocalArray[0]; 
            }
        }
        
    public:
        StackArray(T* pArray = 0, size_t elements = 0)
            : mpArray(&mLocalArray[0])
            , mpArrayEx(pArray)
            , mSize(initial_size)
        {
            if(pArray && elements)
                {Init(pArray, elements);}
        }
        ~StackArray(){reset();}

        operator void* () {return mpArray;}
        operator T* ()    {return mpArray;}
        T* operator ->()  {return mpArray;}

        T* Init(T* pArray, size_t elements)
        {
            EA_ASSERT(pArray && elements);
            if(mpArray != pArray && pArray && elements)
            {
                reset();
                mpArray = pArray;
                mSize = elements;
            }
            return mpArray;
        }

        T* Resize(size_t elements)
        {
            if(elements > mSize)
            {
                reset();

                EA::Allocator::ICoreAllocator* const pAllocator = EA::IO::GetAllocator();
                EA_ASSERT(pAllocator);  // Have you called SetAllocator?

                mpArray = (T*)pAllocator->Alloc(elements * sizeof(T), EAIO_ALLOC_PREFIX "EAIniFile", 0);
                mSize   = elements;
            }
            return mpArray;
        }
        
        size_t GetSize() const
        {
            return mSize;
        }

    }; // class StackArray


    //////////////////////////////////////////////////////////////////////////////
    // ConvertASCIIArrayToBinaryData (16 bit version)
    //
    // We have a boolean return value because it is possible that the ascii data is
    // corrupt. We check for this corruption and return false if so, while converting
    // all corrupt bytes to valid ones.
    //
    static bool ConvertASCIIArrayToBinaryData(const char16_t* pASCIIArray, size_t nASCIIArrayLength, void* pBinaryData)
    {
        uint8_t* pBinaryData8 = (uint8_t*)pBinaryData;
        const char16_t* pEnd = pASCIIArray + nASCIIArrayLength;
        char16_t        cTemp;
        bool            bReturnValue(true);

        while(pASCIIArray < pEnd)
        {
            *pBinaryData8 = 0;

            for(int j = 4; j >= 0; j -= 4)
            {
                cTemp = *pASCIIArray;

                if(cTemp < '0') // Do some bounds checking.
                {
                    cTemp = '0';
                    bReturnValue = false;
                }
                else if(cTemp > 'F') // Do some bounds checking.
                {
                    if(cTemp >= 'a' && cTemp <= 'f')
                        cTemp -= 39; // Convert 'a' to ':'.
                    else
                    {
                        cTemp = '0';
                        bReturnValue = false;
                    }
                }
                else if(cTemp > '9' && cTemp < 'A') // Do some bounds checking.
                {
                    cTemp = '0';
                    bReturnValue = false;
                }
                else if(cTemp >= 'A')
                    cTemp -= 7;

                *pBinaryData8 = (uint8_t)(*pBinaryData8 + ((cTemp - '0') << j));
                pASCIIArray++;
            }

            pBinaryData8++;
        }

        return bReturnValue;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // ConvertBinaryDataToASCIIArray
    //
    // Since every binary byte converts to exactly 2 ascii bytes, the ASCII
    // array  must have space for at least twice the amount of bytes
    // as 'nBinaryDataLength' + 1.
    //
    void ConvertBinaryDataToASCIIArray(const void* _pBinaryData, size_t nBinaryDataLength, char16_t* pASCIIArray)
    {
        const uint8_t* pBinaryData = (uint8_t*)_pBinaryData;
        const uint8_t* pEnd = pBinaryData + nBinaryDataLength;

        while(pBinaryData < pEnd)
        {
            *pASCIIArray = (char16_t)('0' + ((*pBinaryData & 0xf0) >> 4));  // Convert the high byte to a number between 1 and 15.
            if(*pASCIIArray > '9')
                *pASCIIArray += 7; // Convert the ':' to 'A', for example.
            pASCIIArray++;
            *pASCIIArray = (char16_t)('0' + (*pBinaryData & 0x0f));         // Convert the low byte to a number between 1 and 15.
            if(*pASCIIArray > '9')
                *pASCIIArray += 7; // Convert the ':' to 'A', for example.
            pASCIIArray++;
            pBinaryData++;
        }

        *pASCIIArray = '\0';
    }


    #if defined(EAINIFILE_CUSTOM_VSSCANF)
        int Vsscanf(const char16_t* pString, const char16_t* pFormat, va_list arg_list)
        {
            char16_t  fmtBuffer[256];
            char16_t* format = fmtBuffer;
            int       i;
            int       n = 0;
            int       convtot = 0;

            EA::IO::EAIOStrlcpy16(fmtBuffer, pFormat, 256);

            for(i = 0; format[i]; i++)
            {
                if(format[i] == '%')
                {
                    if(format[i+1] == '%' || format[i+1] == '*' || format[i+1] == '\0')
                        i++;
                    else
                    {
                        if(n == 0)
                            n++;
                        else
                        {
                            char16_t f[3];
                            f[1] = format[i+1]; 
                            f[2] = format[i+2];

                            format[i+1] = 'n';
                            format[i+2] = '\0';

                            void* p    = va_arg(arg_list, void*);
                            int   lu   = -1;
                            int   conv = swscanf(pString, format, p, &lu);

                            if(lu == -1)
                            {
                                convtot += conv;
                                break;
                            }

                            convtot++;
                            pString += lu;
                            format[i+1] = f[1];
                            format[i+2] = f[2];
                            format += i; 
                            i = -1;
                            n = 0;
                        }
                    }
                }
            }

            if((format[i] == '\0') && (n > 0))
            {
                void* p    = va_arg(arg_list, void*);
                int   conv = swscanf(pString, format, p);

                convtot += conv;
            }

            return convtot;
        }
    #endif

} // namespace IFLocal


//////////////////////////////////////////////////////////////////////////////
// IniFile
//
IniFile::IniFile(const char16_t* pPath, int optionFlags, EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mFileStream(),
    mpStream(NULL),
    mnEncodingSrc(IFLocal::kEncodingUTF8),
    mbFileIsOpenForWriting(false),
    mbLeaveFileOpenBetweenOperations((optionFlags & kOptionLeaveFileOpen) != 0),
    mbReadEntryCacheReady(false),
    mFileBusyWaitMs(0),
    mSectionPositionMap(Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAIniFile", 
                        pCoreAllocator ? pCoreAllocator : EA::Allocator::ICoreAllocator::GetDefaultAllocator())),
    mSectionNameMap(Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAIniFile", 
                        pCoreAllocator ? pCoreAllocator : EA::Allocator::ICoreAllocator::GetDefaultAllocator()))
{
    mPath[0] = 0;
    SetPath(pPath);
}


//////////////////////////////////////////////////////////////////////////////
// IniFile
//
IniFile::IniFile(EA::IO::IStream* pStream, EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mFileStream(),
    mpStream(NULL),
    mnEncodingSrc(IFLocal::kEncodingUTF8),
    mbFileIsOpenForWriting(false),
    mbLeaveFileOpenBetweenOperations(true),
    mbReadEntryCacheReady(false),
    mFileBusyWaitMs(0),
    mSectionPositionMap(Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAIniFile", 
                        pCoreAllocator ? pCoreAllocator : EA::Allocator::ICoreAllocator::GetDefaultAllocator())),
    mSectionNameMap(Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAIniFile", 
                        pCoreAllocator ? pCoreAllocator : EA::Allocator::ICoreAllocator::GetDefaultAllocator()))
{
    mPath[0] = 0;
    mpStream = pStream;

    if(mpStream)
    {
        const int nAccessFlags = mpStream->GetAccessFlags();
        mbFileIsOpenForWriting = (nAccessFlags & IO::kAccessFlagWrite) != 0;
    }
}

void IniFile::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
    mSectionPositionMap.get_allocator().set_allocator(pCoreAllocator);
    mSectionNameMap.get_allocator().set_allocator(pCoreAllocator);
}


//////////////////////////////////////////////////////////////////////////////
// ~IniFile
//
IniFile::~IniFile()
{
    // There is nothing to do.
    // If mpStream was user-specified, we leave it as-is.
    // If mpStream points to mFileStream, it is automatically closed here.
}


//////////////////////////////////////////////////////////////////////////////
// GetOption
//
int IniFile::GetOption(int option) const
{
     if(option == kOptionLeaveFileOpen)
          return mbLeaveFileOpenBetweenOperations ? 1 : 0;
     return 0;
}


//////////////////////////////////////////////////////////////////////////////
// SetOption
//
void IniFile::SetOption(int option, int value)
{
    if(option == kOptionLeaveFileOpen)
    {
        const bool bLeaveOpen = (value != 0);

        if(mbLeaveFileOpenBetweenOperations != bLeaveOpen)
        {
            mbLeaveFileOpenBetweenOperations = bLeaveOpen;
            if(!mbLeaveFileOpenBetweenOperations)
                Close(); // Since we are inherently between operations, we close the file.
        }
    }
    else if(option == kOptionBusyWaitTime)
    {
        mFileBusyWaitMs = (uint32_t)(unsigned)value;
    }
}


//////////////////////////////////////////////////////////////////////////////
// GetPath
//
const char16_t* IniFile::GetPath() const
{
    return mPath;
}


//////////////////////////////////////////////////////////////////////////////
// SetPath
//
bool IniFile::SetPath(const char16_t* pPath)
{
    if(pPath && pPath[0])
    {
        if(!mbReadEntryCacheReady) // If the user wants to set a new path and it is safe to do so...
            mpStream = NULL;

        if(!mpStream)
        {
            EA::IO::EAIOStrlcpy16(mPath, pPath, sizeof(mPath)/sizeof(mPath[0]));
            mFileStream.SetPath(pPath);
            mpStream = &mFileStream;
            return true;
        }
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// GetStream
//
EA::IO::IStream* IniFile::GetStream() const
{
    // We do not AddRef the returned stream.
    return mpStream;
}


//////////////////////////////////////////////////////////////////////////////
// SetStream
//
bool IniFile::SetStream(EA::IO::IStream* pStream)
{
    if(pStream && !mbReadEntryCacheReady && mpStream) // If the user wants to set a new stream and it is safe to do so...
    {
        mpStream->Close();
        mpStream = NULL;
    }

    if(!mpStream)
    {
        mpStream = pStream;
        mPath[0] = 0;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// Open
//
// We implement a timer because the file we are opening may possibly be shared.
// That kind of thing happens with ini files.
//
bool IniFile::Open(int nAccessFlags)
{
    using namespace EA::IO;

    bool bReturnValue = false;

    if(mpStream)
    {
        if(nAccessFlags & IO::kAccessFlagWrite)
            nAccessFlags |= kAccessFlagRead; // Just to make sure.

        // See if the file is already open in the mode we want. If so, then just return.
        const int nAccessFlagsCurrent = mpStream->GetAccessFlags();

        if(nAccessFlagsCurrent == nAccessFlags || (nAccessFlagsCurrent == kAccessFlagReadWrite && nAccessFlags == kAccessFlagRead)) // If already open the way we need it...
        {
            // We set the file to read mode, even if it was previously in write mode.
            // Perhaps this isn't what we want to do.
            mbFileIsOpenForWriting = (nAccessFlags & IO::kAccessFlagWrite) != 0;
            bReturnValue = true;
        }
        else if(mpStream == &mFileStream)
        {
            const int nCreationMode = (nAccessFlags & IO::kAccessFlagWrite) ? IO::kCDOpenAlways      : IO::kCDOpenExisting;
            const int nShareMode    = (nAccessFlags & IO::kAccessFlagWrite) ? FileStream::kShareNone : FileStream::kShareRead;

            uint32_t nCurrentTime = 0;
            unsigned nSleepTime = 20;

            do {
                if(mFileStream.Open(nAccessFlags, nCreationMode, nShareMode))
                {
                    mbFileIsOpenForWriting = (nAccessFlags & IO::kAccessFlagWrite) != 0;
                    bReturnValue = true;
                }
                else if(!File::Exists(mPath))
                    break;
                else
                {
                    nSleepTime += 20; // Each time we try, we wait a little longer.

                    #if EAIO_THREAD_SAFETY_ENABLED
                        EA::Thread::ThreadSleep(nSleepTime);
                    #else
                        Sleep((DWORD)nSleepTime);
                    #endif
                }
            }
            while(!bReturnValue && ((nCurrentTime += nSleepTime) < mFileBusyWaitMs));
        }
    }

    if(bReturnValue)
        mnEncodingSrc = GetEncoding();

    return bReturnValue;
}


//////////////////////////////////////////////////////////////////////////////
// Close
//
bool IniFile::Close()
{
    if(mpStream)
    {
        if((mpStream == &mFileStream) && mpStream->GetAccessFlags()) // If stream is internal and it is open...
            mpStream->Close();

        mSectionPositionMap.clear();
        mbReadEntryCacheReady = false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// GetEncoding
//
int IniFile::GetEncoding()
{
    using namespace IFLocal;

    unsigned char       buffer[128];
    int                 encoding = kEncodingUTF8;
    const IO::off_type  nSavedPosition = mpStream->GetPosition();
    const IO::size_type nSize          = mpStream->Read(buffer, sizeof(buffer));

    if((nSize != EA::IO::kSizeTypeError) && (nSize >= 2))
        encoding = DetectEncoding(buffer, (size_t)nSize);

    mpStream->SetPosition(nSavedPosition);
    return encoding;
}


//////////////////////////////////////////////////////////////////////////////
// ReadEntry
//
int IniFile::ReadEntry(const char16_t* pSection, const char16_t* pKey, char16_t* pValue, size_t nValueLength)
{
    String16 sValue;

    const int nReturnValue = ReadEntry(pSection, pKey, sValue);

    if(nReturnValue >= 0)
        EA::IO::EAIOStrlcpy16(pValue, sValue.c_str(), nValueLength);

    return nReturnValue;
}


//////////////////////////////////////////////////////////////////////////////
// ReadEntry
//
int IniFile::ReadEntry(const char16_t* pSection, const char16_t* pKey, String16& sValue)
{
    if(mpStream && pSection && pSection[0] && pKey && pKey[0]) // If there is a place to read the value from...
    {
        if(!mbReadEntryCacheReady) // Make sure we have cached the .ini file's section names
        {
            if(!LoadSectionNames(IO::kAccessFlagRead))
            {
                if(!mbLeaveFileOpenBetweenOperations)
                    Close();
                return -1;
            }
        }

        // Act like Win32 GetPrivateProfileString() and be case-insensitive with the section and key names.
        String16 sSectionLower(pSection);
        sSectionLower.make_lower();

        String16 sKeyLower(pKey);
        sKeyLower.make_lower();

        // Seek to the start of the given section.
        SectionLowerToPositionMap::iterator it = mSectionPositionMap.find(sSectionLower);

        if(it == mSectionPositionMap.end())
        {
            if(!mbLeaveFileOpenBetweenOperations)
                Close();
            return -1;  // Section doesn't exist
        }

        const SectionLowerToPositionMap::value_type& spp = *it;
        if(!mpStream->SetPosition(spp.second))
        {
            if(!mbLeaveFileOpenBetweenOperations)
                Close();
            return -1; // Invalid data.
        }

        // Read lines in the section
        String16 sKeyCurrentLower;
        String16 sLine;

        if(GetFileLine(sLine)) // Skip the current section header, which ought to be the current line.
        {
            // Now read subsequent lines until either EOF or a new section header is found.
            while(GetFileLine(sLine)) // Run this loop once per line.
            {
                sLine.ltrim(); // Remove any leading spaces from the line.

                if(sLine.length())
                {
                    if(sLine[0] != (char16_t)';') // Skip comment lines
                    {
                        if(sLine[0] == (char16_t)'[') // If this is a new section header start...
                        {
                            sLine.clear();
                            break; // Quit the enumeration of the current section if a new header (section) is encountered.
                        }

                        const eastl_size_t nEquals = sLine.find((char16_t)'=');

                        if(nEquals != String16::npos) // If this line was a key/value line
                        {
                            sKeyCurrentLower.assign(sLine, 0, nEquals);
                            sKeyCurrentLower.rtrim();
                            sKeyCurrentLower.make_lower();

                            if(sKeyCurrentLower == sKeyLower) // If this key matches the user's requested key...
                            {
                                sLine.erase(0, nEquals + 1);
                                sLine.trim();
                                sValue = sLine;

                                if(!mbLeaveFileOpenBetweenOperations)
                                    Close();
                                return int(sLine.length());
                            }
                        }
                    }
                }
            }
        }

        if(!mbLeaveFileOpenBetweenOperations)
            Close();
    }
    return -1;
}


//////////////////////////////////////////////////////////////////////////////
// ReadEntryFormatted
//
int IniFile::ReadEntryFormatted(const char16_t* pSection, const char16_t* pKey , const char16_t* pValueFormat, ...)
{
    int converted = 0;

    va_list arguments;
    va_start(arguments, pValueFormat);

    String16 value;

    if(ReadEntry(pSection, pKey, value))
    {
        #if defined(__CYGWIN__)
            converted = 0;
        #elif defined(EAINIFILE_CUSTOM_VSSCANF)
            converted = EA::IO::IFLocal::Vsscanf(value.data(), pValueFormat, arguments);
        #else
            converted = vswscanf(value.data(), pValueFormat, arguments);
        #endif
    }

    va_end(arguments);
    return converted;
}



//////////////////////////////////////////////////////////////////////////////
// WriteEntry
//
bool IniFile::WriteEntry(const char16_t* pSection, const char16_t* pKey, const char16_t* pValue)
{
    using namespace EA::IO;
    using namespace IFLocal;

    if(mpStream && pSection && pSection[0] && pKey && pKey[0]) // If there is a place to write the value...
    {
        String16 sToWrite1;
        String16 sToWrite2;

        if(!(mpStream->GetAccessFlags() & IO::kAccessFlagWrite)) // If the stream isn't currently open for writing...
        {
            if(mpStream != &mFileStream)
                return false; // Cannot open an externally specified stream.

            if(!File::Exists(mPath)) // If we have a file path (as opposed to a user-supplied stream) and the ini file doesn't exist (yet)...
            {
                mbReadEntryCacheReady = false;

                if(Open(IO::kAccessFlagReadWrite)) // This will create the file if it doesn't already exist...
                {
                    sToWrite1.sprintf(kSectionFormat, pSection, kTextFileNewLineString);
                    sToWrite2.sprintf(kKeyFormat, pKey, pValue, kTextFileNewLineString);

                    ConvertAndWriteStream(sToWrite1.data(), sToWrite1.length());
                    ConvertAndWriteStream(sToWrite2.data(), sToWrite2.length());

                    if(!mbLeaveFileOpenBetweenOperations)
                        Close();
                    return true;
                }
                return false;
            }
        }

        // The ini file does exist. We'll write into this file ourselves.
        if(!mbReadEntryCacheReady)
        {
            if(!LoadSectionNames(IO::kAccessFlagReadWrite))
                return false;
        }

        if(!mbFileIsOpenForWriting && mpStream->GetState() == IO::kStateSuccess)    // It is possible that the file was previously open in read mode only.
            mpStream->Close();                                                      // In this case, we'll need to re-open it in write mode.

        if(mpStream->GetState() == IO::kStateSuccess || Open(IO::kAccessFlagReadWrite))
        {
            EA::Allocator::ICoreAllocator* const pAllocator = EA::IO::GetAllocator();
            EA_ASSERT(pAllocator);  // Have you called SetAllocator?

            // Act like Win32 GetPrivateProfileString() and be case-insensitive with section and key.
            String16 sSectionLower(pSection);
            sSectionLower.make_lower();

            String16 sKeyLower(pKey);
            sKeyLower.make_lower();

            // Seek to the start of the given section
            SectionLowerToPositionMap::iterator it = mSectionPositionMap.find(sSectionLower);
            if(it == mSectionPositionMap.end()) // If the section doesn't exist already...
            {
                // We're going to write a new section header string (e.g. "[Settings]") here.
                // What we do first is make it so that there is a blank line between the end
                // of the file and the new section header line.
                mpStream->SetPosition(0, IO::kPositionTypeEnd);
                const IO::off_type nCurrentPosition(mpStream->GetPosition());
                bool bFileHasNewlineAtEndAlready(false);

                if(nCurrentPosition > 0)
                {
                    char16_t nLastCharInFile(0);
                    
                    mpStream->SetPosition(-off_type(sizeof(char16_t)), kPositionTypeCurrent);
                    
                    if(mpStream->Read(&nLastCharInFile, sizeof(char16_t)) == sizeof(char16_t))
                        bFileHasNewlineAtEndAlready = ((nLastCharInFile == '\r') || (nLastCharInFile == '\n'));
                }

                if(mpStream->GetSize() > 0)
                {
                    if(!bFileHasNewlineAtEndAlready)
                        sToWrite1.sprintf(kValueFormat, kTextFileNewLineString);
                }

                ConvertAndWriteStream(sToWrite1.data(), sToWrite1.length());        // Todo: Now that RZXGlobal has a decent suite or Unicode support, we can make a Unicode .ini file.

                // Now write the section header itself.
                const IO::off_type nHeaderSectionPosition(mpStream->GetPosition()); // The position will be at the end of the file.

                sToWrite1.sprintf(kSectionFormat, pSection, kTextFileNewLineString);
                ConvertAndWriteStream(sToWrite1.data(), sToWrite1.length());

                sToWrite2.sprintf(kKeyFormat, pKey, pValue, kTextFileNewLineString);
                ConvertAndWriteStream(sToWrite2.data(), sToWrite2.length());

                // Update our data map with the new data.
                mSectionPositionMap.insert(SectionLowerToPositionMap::value_type(sSectionLower, nHeaderSectionPosition));

                String16 sSection(pSection);
                mSectionNameMap.insert(SectionLowerToSectionMap::value_type(sSectionLower, sSection));

                if(!mbLeaveFileOpenBetweenOperations)
                    Close();
                return true;
            }
            else // The section does exist already...
            {
                IO::off_type nPositionOfCurrentLine;

                const SectionLowerToPositionMap::value_type& theEntry = *it;
                if(IO::size_type(theEntry.second) < mpStream->GetSize() && mpStream->SetPosition(theEntry.second))
                {
                    //Now we are at the beginning of the section, see if the current key is being used.
                    String16 sKeyCurrentLower;
                    String16 sKeyLower(pKey);
                    String16 sValueCurrent;

                    sKeyLower.make_lower();
                    if(GetFileLine(sValueCurrent)) // Skip the current section header, which ought to be the current line.
                    {
                        nPositionOfCurrentLine = mpStream->GetPosition();

                        // Now read subsequent lines until either EOF or a new section header is found.
                        while(GetFileLine(sValueCurrent)) // Run this loop once per line.
                        {
                            sValueCurrent.ltrim(); // Remove any leading spaces from the line.
                            if(sValueCurrent.length())
                            {
                                if(sValueCurrent[0] != ';')  // Skip comment lines
                                {
                                    if(sValueCurrent[0] == '[') // If we run into the next header...
                                    {
                                        IO::size_type   nBytesToSave = mpStream->GetSize() - nPositionOfCurrentLine;
                                        char16_t* const pData        = (char16_t*)pAllocator->Alloc((size_t)nBytesToSave * sizeof(char16_t), EAIO_ALLOC_PREFIX "EAIniFile", 0);

                                        mpStream->SetPosition(nPositionOfCurrentLine);
                                        nBytesToSave = mpStream->Read(pData, nBytesToSave);
                                        if(nBytesToSave != kSizeTypeError)
                                        {
                                            mpStream->SetPosition(nPositionOfCurrentLine);
                                            sToWrite2.sprintf(kKeyFormat, pKey, pValue, kTextFileNewLineString);
                                            ConvertAndWriteStream(sToWrite2.data(), sToWrite2.length());
                                            mpStream->Write(pData, nBytesToSave);                   // To do: Check the return value from Write.
                                            mpStream->SetSize((size_type)mpStream->GetPosition());  // To do: Check the return value from SetSize.
                                        }

                                        pAllocator->Free(pData);
                                        mbReadEntryCacheReady = false; // We are writing into the middle of the file, so header offsets have almost certainly changed.
                                        if(!mbLeaveFileOpenBetweenOperations)
                                            Close();
                                        return true;
                                    }

                                    const eastl_size_t nEquals = sValueCurrent.find((char16_t)'=');

                                    if(nEquals != String16::npos) // If this line was a key/value line
                                    {
                                        sKeyCurrentLower.assign(sValueCurrent, 0, nEquals);
                                        sKeyCurrentLower.rtrim();
                                        sKeyCurrentLower.make_lower();

                                        if(!sKeyCurrentLower.empty())
                                        {
                                            sValueCurrent.erase(0, nEquals + 1);
                                            sValueCurrent.trim();
                                        }
                                    }

                                    if(sKeyCurrentLower == sKeyLower) // If we found the key we want...
                                    {
                                        const IO::off_type nPositionOfNextLine = mpStream->GetPosition();
                                        size_type nBytesToSave                 = mpStream->GetSize() - nPositionOfNextLine;
                                        char16_t* const pData                  = (char16_t*)pAllocator->Alloc((size_t)nBytesToSave * sizeof(char16_t), EAIO_ALLOC_PREFIX "EAIniFile", 0);

                                        // mpStream->SeekToPosition(nPositionOfNextLine); // Not necessary because we are already there.
                                        nBytesToSave = mpStream->Read(pData, nBytesToSave);
                                        if(nBytesToSave != kSizeTypeError)
                                        {
                                            mpStream->SetPosition(nPositionOfCurrentLine);
                                            sToWrite2.sprintf(kKeyFormat, pKey, pValue, kTextFileNewLineString);
                                            ConvertAndWriteStream(sToWrite2.data(), sToWrite2.length());
                                            ConvertAndWriteStream(kTextFileNewLineString, EA::IO::EAIOStrlen16(kTextFileNewLineString)); // right before section name
                                            mpStream->Write(pData, nBytesToSave);                   // To do: Check the return value from Write.
                                            mpStream->SetSize((size_type)mpStream->GetPosition());  // To do: Check the return value from SetSize.
                                        }

                                        pAllocator->Free(pData);
                                        mbReadEntryCacheReady = false; // We are writing into the middle of the file, so header offsets have almost certainly changed.
                                        if(!mbLeaveFileOpenBetweenOperations)
                                            Close();
                                        return true;
                                    }
                                } //End of: if(sValueCurrent[0] != ';')
                                else
                                {
                                    // Line is a comment line
                                }
                            } //End of: if(sValueCurrent.length())
                            else
                            {
                                // Empty line
                            }
                            nPositionOfCurrentLine = mpStream->GetPosition();
                        } //End of: //while(GetFileLine(sValueCurrent))

                        //We hit the end of the file without finding the key nor the next header. So just append the key-value data.
                        mpStream->SetPosition(0, IO::kPositionTypeEnd);
                        sToWrite2.sprintf(kKeyFormat, pKey, pValue, kTextFileNewLineString);
                        ConvertAndWriteStream(sToWrite2.data(), sToWrite2.length());

                        if(!mbLeaveFileOpenBetweenOperations)
                            Close();
                        return true;
                    } //End of: if(GetFileLine(sValueCurrent))
                    else
                    {
                        EA_FAIL_FORMATTED(("IniFile::WriteEntry: Section offsets are corrupt or file section read error.\n"));
                    }
                } //End of: if(mpStream->SeekToPosition(theEntry.second) < mpStream->Length())
                else
                {
                     EA_FAIL_FORMATTED(("IniFile::WriteEntry: Section offsets are corrupt or file position error.\n"));
                }
            } //End of: else{ //The section does exist already...

            if(!mbLeaveFileOpenBetweenOperations)
                Close();
        } //End of: if(Open(IO::kAccessFlagReadWrite))
        else
        {
            EA_FAIL_FORMATTED(("IniFile::WriteEntry: Couldn't open file.\n"));
        }
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////
// WriteEntryFormatted
//
bool IniFile::WriteEntryFormatted(const char16_t* pSection, const char16_t* pKey, const char16_t* pFormat, ...)
{
    IFLocal::StackArray<char16_t, 256> buffer;

    va_list arguments;
    va_start(arguments, pFormat);

    #if defined(_MSC_VER)
        size_t size_needed = _vsnwprintf(buffer, buffer.GetSize(), pFormat, arguments);
    #elif defined(__CYGWIN__)
        buffer[0] = 0;
        size_t size_needed = 0;
    #else
        size_t size_needed = vswprintf(buffer, buffer.GetSize(), pFormat, arguments);
    #endif

    if(size_needed > buffer.GetSize())
    {
        buffer.Resize(size_needed);

        #if defined(_MSC_VER)
            _vsnwprintf(buffer, buffer.GetSize(), pFormat, arguments);
        #elif defined(__CYGWIN__)
            buffer[0] = 0;
        #else
            vswprintf(buffer, buffer.GetSize(), pFormat, arguments);
        #endif

    }

    bool bResult = WriteEntry(pSection, pKey, buffer);

    return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// EnumSections
//
int IniFile::EnumSections(EnumCallbackFunction pCallback, void* pContext)
{
    int nSectionCount = 0;

    if(!mbReadEntryCacheReady)
    {
        if(!LoadSectionNames(IO::kAccessFlagRead))
        {
            nSectionCount = -1;
            goto Done;
        }
    }

    {
        // Now get the sections into a temp list and then callback with the temp list items.
        const char16_t cEmpty(0);
        typedef eastl::list<String16, EA::Allocator::EAIOEASTLCoreAllocator> SectionList;
        SectionList sectionList;

        // We make a temporary list of sections. This is for re-entrancy safety.
        // Our section to position map stores the section in lower case. 
        // We convert to the actual case before calling the user back.
        for(SectionLowerToPositionMap::iterator it(mSectionPositionMap.begin()); it != mSectionPositionMap.end(); ++it)
        {
            const String16& sSection = (*it).first;

            SectionLowerToSectionMap::const_iterator it1 = mSectionNameMap.find(sSection);
            if(it1 == mSectionNameMap.end())
            {
                EA_FAIL_MESSAGE("IniFile::EnumSections: map failure."); // This is never supposed to happen. It should always be present.
                sectionList.push_back(sSection);
            }
            else
                sectionList.push_back((*it1).second);
            nSectionCount++;
        }

        // Now iterate through this temporary list.
        for(SectionList::iterator it2(sectionList.begin()); it2 != sectionList.end(); ++it2)
        {
            const String16& sSection = *it2;
            if(!pCallback(sSection.c_str(), &cEmpty, pContext))
                {break;}
        }
    }

    Done:
    if(!mbLeaveFileOpenBetweenOperations)
        Close();

    return nSectionCount;
}



///////////////////////////////////////////////////////////////////////////////
// EnumEntries
//
int IniFile::EnumEntries(const char16_t* pSection, EnumCallbackFunction pCallback, void* pContext)
{
    int nEntryCount = 0;
    String16 sKey;
    String16 sValue;
    String16 sSectionLower(pSection);
    SectionLowerToPositionMap::iterator it;
    
    // Make sure we have cached the .ini file's section names
    if(!mbReadEntryCacheReady)
    {
        if(!LoadSectionNames(IO::kAccessFlagRead))
        {
            nEntryCount = -1;
            goto End;
        }
    }

    // Act like Win32 GetPrivateProfileString() and be case-insensitive.
    sSectionLower.make_lower();

    // Find the start of the given section
    it = mSectionPositionMap.find(sSectionLower);

    if(it != mSectionPositionMap.end()) // If the section is known...
    {
        const SectionLowerToPositionMap::value_type& spp = *it;

        if(mpStream->SetPosition(spp.second)) // If the section's position is valid...
        {
            if(GetFileLine(sValue)) // Skip the current section header, which ought to be the current line.
            {
                typedef eastl::vector<eastl::pair<String16, String16>, Allocator::EAIOEASTLCoreAllocator> DataList_t;

                DataList_t data_list;
                data_list.reserve(32);

                // Now read subsequent lines until either EOF or a new section header is found.
                while(GetFileLine(sValue)) // Run this loop once per line.
                {
                    sValue.ltrim(); // Remove any leading spaces from the line.

                    if(!sValue.empty())
                    {
                        if(sValue[0] != ';') // If not a comment line...
                        {
                            if(sValue[0] == '[') // If this line is the start of a new section...
                                break;           // We quit the enumeration of the current section if a new header (section) is encountered.

                            const eastl_size_t nEquals = sValue.find((char16_t)'=');

                            if(nEquals != String16::npos) // If this line was a key/value line
                            {
                                sKey.assign(sValue, 0, nEquals);
                                sKey.rtrim();

                                if(!sKey.empty())
                                {
                                    sValue.erase(0, nEquals + 1);
                                    sValue.trim();

                                    data_list.push_back(DataList_t::value_type(sKey, sValue));
                                    nEntryCount++;
                                }
                            }
                        }
                    }
                }

                // Now that we have a list with all the key/value pairs in it, call the callback.
                for(DataList_t::iterator it(data_list.begin()); it != data_list.end(); ++it)
                {
                    DataList_t::value_type value = *it;

                    if(!pCallback(value.first.data(), value.second.data(), pContext))
                        break;
                }
            }
        }
    }

    End:
    if(!mbLeaveFileOpenBetweenOperations)
        Close();

    return nEntryCount;
}



///////////////////////////////////////////////////////////////////////////////
// LoadSectionNames
//
bool IniFile::LoadSectionNames(int nAccessFlags)
{
    mSectionPositionMap.clear();
    mbReadEntryCacheReady = false;

    if(!mpStream || (mpStream->GetAccessFlags() != nAccessFlags && !Open(nAccessFlags)))
    {
        // We don't assert here because it is not a violation of the specification if the file can't be opened.
        // EA_FAIL_MESSAGE("iniFile::LoadSectionNames: Couldn't open or read the source stream.\n");
        return false;
    }

    String16 sLine;
    String16 sSectionName;
    String16 sSectionNameLower;

    mpStream->SetPosition(0);
    for(IO::off_type nFilePosition = mpStream->GetPosition(); GetFileLine(sLine); nFilePosition = mpStream->GetPosition()) // Run this loop once per line.
    {
        // If the text begins with a UTF16 identification sequence, skip it.
        if((nFilePosition == 0) && (sLine.length() >= 1) && ((sLine[0] == 0xfffe) || (sLine[0] == 0xfeff)))
            sLine.erase(0, 1);

        sLine.ltrim(); // Remove any leading spaces from the line.

        if(!sLine.empty())
        {
            if(sLine[0] == '[') // If we have the beginning of a section...
            {
                for(String16::size_type i = 1, iEnd = sLine.length(); i < iEnd; i++) // Now find the first ']' char, if there is one...
                {
                    if(sLine[i] == ']')
                    {
                        if(i >= 2) // If the string is more than just empty brackets ("[]").
                        {
                            sSectionName.assign(sLine.data() + 1, i - 1);
                            sSectionNameLower = sSectionName;
                            sSectionNameLower.make_lower();

                            const SectionLowerToPositionMap::value_type spp(sSectionNameLower, nFilePosition);
                            mSectionPositionMap.insert(spp);

                            const SectionLowerToSectionMap::value_type ssp(sSectionNameLower, sSectionName);
                            mSectionNameMap.insert(ssp);
                        }

                        break;
                    }
                }
            }
        }
    }

    mbReadEntryCacheReady = true;

    return true; // Note that we don't call check 'mbLeaveFileOpenBetweenOperations' here and possibly close anything.
}


///////////////////////////////////////////////////////////////////////////////
// GetFileLine
//
bool IniFile::GetFileLine(String16& sLine)
{
    using namespace IFLocal;

    sLine.clear();
    
    if(GetCharacterSize(mnEncodingSrc) == 2)
        return GetFileLine16To16(sLine);

    String8 sLine8;

    if(GetFileLine8To8(sLine8))
    {
        // Convert sLine8 to sLine (16)
        sLine.resize(sLine8.size());
        const size_t nRequiredDestLength = EA::IO::StrlcpyUTF8ToUTF16(&sLine[0], sLine.length() + 1, sLine8.c_str(), sLine8.length());

        if((unsigned)nRequiredDestLength > sLine.length())
        {
            if(nRequiredDestLength < 16384)
            {
                sLine.resize((eastl_size_t)nRequiredDestLength);
                EA::IO::StrlcpyUTF8ToUTF16(&sLine[0], sLine.length() + 1, sLine8.c_str(), sLine8.length());
            }
            else
                return false;
        } 
       
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetFileLine8To8
//
// Reads 8 bit text into an 8 bit UTF8 string.
//
// A line can end with \n, \r, or \r\n and this function should still work properly.
//
// Gets the (current) line from the current position to the end of the
// current line. The '\r' and/or '\n' at the end of the line is not
// returned. Thus a true return value is possible when the returned
// line is empty.
//
// Sets the new file position to be after the current line, which means either the
// first char of the next line or the end of the file.
//
// Returns true if any line could be discerned. Basically, returns false if
// the file position was already at the end of the line.
//
bool IniFile::GetFileLine8To8(String8& sLine)
{
    using namespace EA::IO;
    typedef char8_t cType;

    bool            bResult          = false;
    IO::off_type    nCurrentPosition = mpStream->GetPosition();
    bool            bEOLFound        = false;
    const size_type kBufferLength    = 64 * sizeof(cType); // A reasonable chunk size for an .ini file line. The final line may end up being longer.
    const size_type kBufferSize      = kBufferLength * sizeof(cType);
    cType           buffer[kBufferLength];

    sLine.clear();
    memset(buffer, 0, sizeof(buffer));

    while(!bEOLFound)
    {
        const size_type nBytesRead = mpStream->Read(&buffer, kBufferSize);

        if((nBytesRead == 0) || (nBytesRead == kSizeTypeError)) // If we are at the end of the file or there is an error...
            break;

        bResult = true; // If we have successfully read anything, then the return value is going to be true.

        // Search for EOL in this chunk. EOL means any of 0x0d or 0x0a. We are trying to be somewhat platform-independent here.
        IO::off_type pos;
        for(pos = 0; pos < (IO::off_type)nBytesRead; pos++)
        {
            if((buffer[pos] == 0x0d) || (buffer[pos] == 0x0a)) // If the character is one of the newline characters...
            {
                bool  bEOFFound = false;
                cType c;

                bEOLFound = true;

                // Now seek to the beginning of the next (non-empty) line.
                // With some deftness, this code below can be made to work without using the stream as much.
                mpStream->SetPosition(nCurrentPosition + pos); // We need to seek here because our initial read was to the end of 'buffer'.
                do{
                    if(mpStream->Read(&c, sizeof(c)) != sizeof(c))
                        bEOFFound = true;
                } while(!bEOFFound && ((c == 0x0d) || (c == 0x0a)));

                if(!bEOFFound)
                    mpStream->SetPosition(-IO::off_type(uint32_t(sizeof(c))), IO::kPositionTypeCurrent); // Seek back by one character, since we we'll need that last one next round.
                break;
            }
        }

        sLine.append(buffer, (eastl_size_t)pos);
        nCurrentPosition = mpStream->GetPosition();
    }

    if(mnEncodingSrc != IFLocal::kEncodingUTF8)
    {
        // Todo: Convert sLine from mnEncodingSrc to UTF8.
    }

    return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// GetFileLine16To16
//
// Reads 16 bit text into a 16 bit UTF16 string.
//
bool IniFile::GetFileLine16To16(String16& sLine)
{
    using namespace EA::IO;
    using namespace IFLocal;
    typedef char16_t cType;

    bool            bResult             = false;
    IO::off_type    nCurrentPosition    = mpStream->GetPosition();
    bool            bEOLFound           = false;
    const size_type kBufferLength       = 64 * sizeof(cType); // A reasonable chunk size for an .ini file line. The final line may end up being longer.
    const size_type kBufferSize         = kBufferLength * sizeof(cType);
    cType           buffer[kBufferLength];

    sLine.clear();
    memset(buffer, 0, sizeof(buffer));

    while(!bEOLFound)
    {
        const size_type nBytesRead = mpStream->Read(&buffer, kBufferSize);

        if((nBytesRead == 0) || (nBytesRead == kSizeTypeError)) // If we are at the end of the file or there is an error...
            break;

        bResult = true; // If we have successfully read anything, then the return value is going to be true.

        // Search for EOL in this chunk. EOL means any of 0x0d or 0x0a. We are trying to be somewhat platform-independent here.
        IO::off_type pos;

        for(pos = 0; pos < (IO::off_type)nBytesRead; pos++)
        {
            if((buffer[pos] == 0x0d) || (buffer[pos] == 0x0a)) // If the character is one of the newline characters...
            {
                bool  bEOFFound = false;
                cType c;

                bEOLFound = true;

                // Now seek to the beginning of the next (non-empty) line.
                // With some deftness, this code below can be made to work without using the stream as much.
                mpStream->SetPosition(nCurrentPosition + pos); // We need to seek here because our initial read was to the end of 'buffer'.
                do{
                    if(mpStream->Read(&c, sizeof(c)) != sizeof(c))
                        bEOFFound = true;
                } while(!bEOFFound && ((c == 0x0d) || (c == 0x0a)));

                if(!bEOFFound)
                    mpStream->SetPosition(-(IO::off_type)sizeof(c), IO::kPositionTypeCurrent); // Seek back by one character, since we we'll need that last one next round.
                break;
            }
        }

        sLine.append(buffer, (eastl_size_t)pos);
        nCurrentPosition = mpStream->GetPosition();
    }

    // Convert sLine from mnEncodingSrc to local endian UTF16 if mnEncodingSrc is not already local-endian UTF16.
    if(mnEncodingSrc != GetLocalUCS2Encoding())
    {
        for(String16::size_type i(0), iEnd(sLine.length()); i < iEnd; i++)
            sLine[i] = (cType)(sLine[i] << 8 | sLine[i] >> 8);
    }

    return bResult;
}


bool IniFile::SectionExists(const char16_t* pSection)
{
    // Make sure we have cached the .ini file's section names
    if(!mbReadEntryCacheReady)
    {
        if(!LoadSectionNames(IO::kAccessFlagRead))
            return false;
    }

    // Act like Win32 GetPrivateProfileString() and be case-insensitive.
    String16 sSectionLower(pSection);
    sSectionLower.make_lower();

    const bool bReturnValue = (mSectionPositionMap.find(sSectionLower) != mSectionPositionMap.end());

    if(!mbLeaveFileOpenBetweenOperations)
        Close();

    return bReturnValue;
}


int IniFile::ReadBinary(const char16_t* pSection, const char16_t* pKey, void* pData, size_t nDataLength)
{
    IFLocal::StackArray<char16_t, 256> dataASCII;

    int nValueLength = ReadEntry(pSection, pKey, dataASCII, dataASCII.GetSize());

    if(nValueLength >= 0)
    {
        if(nValueLength > (int)dataASCII.GetSize()) // If we didn't have enough space... resize and try again.
        {
            dataASCII.Resize(nValueLength + 1);
            ReadEntry(pSection, pKey, dataASCII, nValueLength);
        }

        IFLocal::StackArray<uint8_t, 256> binBuffer((uint8_t*)pData, nDataLength);
        
        size_t binBufferSize = nValueLength / 2;
        if(binBufferSize > nDataLength)
            binBuffer.Resize(binBufferSize);

        EA::IO::IFLocal::ConvertASCIIArrayToBinaryData(dataASCII, nValueLength, binBuffer); 

        if(binBuffer != pData)  
        {
            nValueLength = (int)(unsigned)((binBuffer.GetSize() > nDataLength) ? nDataLength : binBuffer.GetSize());
            memcpy(pData, binBuffer, nValueLength);
        }
    }

    return nValueLength; // Return the size of the written data.
}


bool IniFile::WriteBinary(const char16_t* pSection, const char16_t* pKey, const void* pData, size_t nDataLength)
{
    bool bResult = false;

    IFLocal::StackArray<char16_t, 256> stackData;
    const size_t nConvertedSize = (nDataLength * 2) + 1;

    if(nConvertedSize > stackData.GetSize()) 
        stackData.Resize(nConvertedSize);

    EA::IO::IFLocal::ConvertBinaryDataToASCIIArray(pData, nDataLength, stackData);

    if(WriteEntry(pSection, pKey, stackData))
        bResult = true;

    return bResult;
}


bool IniFile::ConvertAndWriteStream(const char16_t* pText16, size_t count)
{
    using namespace IFLocal;

    if(mnEncodingSrc == kEncodingUTF16) // If the IniFile is 16 bit...
    {
        if(mpStream && pText16 && count)
            return mpStream->Write(pText16, count);
    }
    else if(mnEncodingSrc == kEncodingUTF8) // else if the IniFile is 8 bit...
    {
        // Convert pText16/count to conversionBuffer/conversionBufferSize
        size_t conversionBufferSize = (count * 6);  // '* 6' is a theoretical upper limit for UTF16 to UTF8 conversion.
        StackArray<char8_t, 256> conversionBuffer;
        conversionBuffer.Resize(conversionBufferSize);
        conversionBufferSize = EA::IO::StrlcpyUTF16ToUTF8(conversionBuffer, conversionBufferSize, pText16, count);

        if(mpStream)
            return mpStream->Write(conversionBuffer, conversionBufferSize);
    }

    return false;
}



#undef EA_INIFILE_SLEEP


} // namespace IO

} // namespace EA



#endif // EAIO_INIFILE_ENABLED

