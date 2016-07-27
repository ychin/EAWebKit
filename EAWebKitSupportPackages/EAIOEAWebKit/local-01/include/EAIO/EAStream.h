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
// EAStream.h
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements a general stream interface for reading and writing arbitrary
// data to an arbitrary source or destination.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAM_H) && !defined(FOUNDATION_EASTREAM_H)

#define EAIO_EASTREAM_H
#define FOUNDATION_EASTREAM_H


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <string.h>
    #include <sys/types.h>


/// EA_STREAM_SYSTEM_ENABLED
/// This define is deprecated; it is provided for backwards compatibility.
#define EA_STREAM_SYSTEM_ENABLED 1


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


        /// kSizeTypeError
        /// Used to designate an error condition for many functions that return size_type.
        const size_type kSizeTypeError = (size_type)-1;


        /// kSizeTypeDone
        /// Used to designate the condition whereby a requested operation is complete
        /// and no more processing can be done. Only applicable to functions that 
        /// document the use of kSizeTypeDone.
        const size_type kSizeTypeDone = (size_type)-2;


        /// kLengthNull
        /// Defines a value to be used for string conversion functions which means
        /// that the string length is specified by a wherever the terminating null
        /// character is. For the copying or converting of strings, the terminating
        /// null character is also copied to the destination.
        const size_t kLengthNull = (size_t)-1;


        /// enum AccessFlags
        /// Defines stream access flags, much like file access flags.
        enum AccessFlags
        {
            kAccessFlagNone      = 0x00,  /// No specified flags. Also used to indicate that a given IO stream is closed.
            kAccessFlagRead      = 0x01,  /// Used for identifying read access to an entity.
            kAccessFlagWrite     = 0x02,  /// Used for identifying write access to an entity.
            kAccessFlagReadWrite = 0x03   /// Used for identifying both read and write access to an entity.
        };

        /// enum CD (creation disposition)
        /// Specifies aspects of how to create or not create a file during opening of it.
        enum CD
        {
            kCDCreateNew        = 1,      /// Fails if file already exists.
            kCDCreateAlways     = 2,      /// Never fails, always opens or creates and truncates to 0.
            kCDOpenExisting     = 3,      /// Fails if file doesn't exist, keeps contents.
            kCDOpenAlways       = 4,      /// Never fails, creates if doesn't exist, keeps contents.
            kCDTruncateExisting = 5,      /// Fails if file doesn't exist, but truncates to 0 if it does.
            kCDDefault          = 6       /// Default (implementation-specific) disposition
        };

        /// enum PositionType
        /// Defines the positional basis for a user GetPosition or SetPosition action.
        enum PositionType
        {
            kPositionTypeBegin   = 0,    /// For GetPosition refers to absolute index of next byte to read; always positive. For SetPosition, refers to absolute index of next byte to be written; always positive.
            kPositionTypeCurrent = 1,    /// For GetPosition always returns zero. For SetPosition, refers to position relative to current position; can be positive or negative.
            kPositionTypeEnd     = 2     /// For GetPosition returns to position relative to end (i.e. the negative of bytes left to read); always negative. For SetPosition, refers to position relative to end; can be positive or negative.
        };

        /// enum LineEnd
        /// Defines textual line ending property types.
        enum LineEnd
        {
            kLineEndNone    = 0,     /// Refers to no line termination. When writing, it means to append nothing.
            kLineEndAuto    = 1,     /// Refers to automatic line termination. When writing, it means to append kLineTerminationNewline if there isn't one already.
            kLineEndNewline = 2,     /// Refers to "\n" line termination. When writing, it means to append a newline always.
            kLineEndUnix    = 2,     /// Same as kLineEndNewline.
            kLineEndWindows = 3      /// Refers to "\r\n" line termination. When writing, it means to append a newline always.
        };

        /// enum State
        /// Defines state values or function return values. Zero means success and non-zero means failure in general.
        /// Note that various stream types may define their own errors in addition to these generic errors.
        enum State
        {
            kStateSuccess =  0,
            kStateError   = -1,
            kStateNotOpen = -2
        };

        /// enum Endian
        /// Defines endian-ness. This is appropriate for working with binary numerical data. 
        enum Endian
        {

            kEndianBig       = 0,            /// Big endian.
            kEndianLittle    = 1,            /// Little endian.
            #ifdef EA_SYSTEM_BIG_ENDIAN
                kEndianLocal = kEndianBig    /// Whatever endian is native to the machine.
            #else 
                kEndianLocal = kEndianLittle /// Whatever endian is native to the machine.
            #endif
        };



        /// \class IStream
        /// \brief The base class for stream IO.
        ///
        /// Stream operation errors are identified by a function return value of (size_type)-1 (a.k.a. kSizeTypeError).
        /// Whether or not the stream becomes unusable after an error occurrs is dependent on 
        /// the particular stream type and implementation. It is up to the particular implementation
        /// to define its behaviour. It would not be best if this IStream interface declaration
        /// attempted to rigorously define the error state behaviour for all implementations.
        /// In practice, it is safest to assume that an IStream is unusable upon encountering 
        /// an error and that the only function the user can call without necessarily encountering
        /// anther error is the Close function.
        ///
        class EAIO_API IStream
        {
        public:
            /// ~IStream
            /// Virtual destructor. Having this destructor allows us to delete an IStream pointer
            /// without knowing its base class. Due to the C++ language design, you cannot have
            /// a pure virtual destructor but instead must implement it, at the least with { }.
            virtual ~IStream() { }

            /// AddRef
            /// Returns the new reference count. Implementations of the IStream interface are not 
            /// normally required to make their AddRef implementation thread-safe, as the sharing 
            /// of streams between threads is something that must be coordinated at a higher level.
            ///
            /// It often happens that users of streams would like to reference-count them so that
            /// streams can be handed off from a creator to an owner or can be shared between owners.
            ///
            /// Note that the use of reference counting by a system is optional and is determined 
            /// by the system and ideally documented as such by the system or the stream creator.
            /// A call to AddRef commits a system to using AddRef/Release to maintain the lifetime
            /// of the object thereafter.
            virtual int AddRef() = 0;

            /// Release
            /// Returns the new reference count, may be 0, in which case the object has been destroyed.
            /// Every call to AddRef must be matched by a call to Release.
            virtual int Release() = 0;

            /// GetType
            /// Returns the type of the stream, which is different for each Stream subclass.
            /// This function can be used for run-time type identification. A type of zero means
            /// the type is unknown or invalid.
            virtual uint32_t GetType() const = 0;

            /// AccessFlags
            /// Returns one of enum AccessFlags.
            /// This function also tells you if the stream is open, as a return value 
            /// of zero means the stream is not open. It is not allowed that a stream  
            /// be open with no type of access.
            virtual int GetAccessFlags() const = 0;

            /// GetState
            /// Returns the error state of the stream.
            /// Returns kStateSuccess if OK, else an error code.
            /// This function is similar to 'errno' or a 'get last error' facility.
            virtual int GetState() const = 0;

            /// Close
            /// Closes the stream and releases resouces associated with it.
            /// Returns true upon success, else false.
            /// If the return value is false, GetState will give the error code.
            /// If an IStream encounters an error during operations on an open
            /// stream, it is guaranteed that you can safely call the Close function 
            /// on the stream.
            virtual bool Close() = 0;

            /// GetSize
            /// Returns the size of the stream, which is not the same as the size
            /// of bytes remaining to be read from the stream. Returns (size_type)-2 
            /// if the stream is of indeterminate size. Returns (size_type)-1 
            /// (a.k.a. kSizeTypeError) upon error.
            virtual size_type GetSize() const = 0;

            /// SetSize
            /// Sets the size of the stream, if possible. It is debatable whether this
            /// function should be present in IStream or only in suclasses of 
            /// StreamBase which are writable. For consistency with GetSize, we put 
            /// the function here. But also consider that a SetSize function is not 
            /// necessarily a data writing function, depending on the stream implementation.
            virtual bool SetSize(size_type size) = 0;

            /// GetPosition
            /// Gets the current read/write position within the stream. The read and 
            /// write positions of a stream must be the same value; you cannot have a
            /// read position that is different from a write position. However, a 
            /// Stream subclass can provide such functionality if needed. 
            /// Returns (size_type)-1 (a.k.a. kSizeTypeError) upon error.
            virtual off_type GetPosition(PositionType positionType = kPositionTypeBegin) const = 0;

            /// SetPosition
            /// Sets the read/write position of the stream. If the specified position is 
            /// beyond the size of a fixed stream, the position is set to the end of the stream.
            /// A writable stream subclass may provide a policy whereby setting the position
            /// beyond the end of the stream results in an increase in the stream size.
            virtual bool SetPosition(off_type position, PositionType positionType = kPositionTypeBegin) = 0;

            /// GetAvailable
            /// Returns the number of bytes available for reading.
            /// Returns (size_type)-1 (a.k.a. kSizeTypeError) upon error.
            /// This function is non-blocking; it should return immediately.
            virtual size_type GetAvailable() const = 0;

            /// Read
            /// Reads bytes from the stream given by the input count 'nSize'.
            /// If less then nSize bytes are available, then those bytes will be read.
            /// Returns the number of bytes read. A return value of zero means that there
            /// were no bytes to be read or no bytes were requested to be read. 
            /// A return value of zero means the end of file was reached.
            /// A return value > 0 but < 'nSize' is possible, and it does not
            /// necessarily mean that the end of the file was reached.
            /// Returns (size_type)-1 (a.k.a. kSizeTypeError) if there was an error. 
            /// You can use this return value or IStream::GetState to determine the error.
            /// Input size values equal to (size_type)-1 (a.k.a. kSizeTypeError) are not valid input.
            /// Upon error, the stream pointer is at the position it was upon the error occurrence.
            virtual size_type Read(void* pData, size_type nSize) = 0;

            /// Flush
            /// Flush any non-empty stream write buffers. 
            /// If the return value is false, GetState will give the error code.
            /// This function implements the flushing as per the underlying file system.
            /// The behavior of the Flush function varies with the underlying platform.
            ///
            /// A common use of Flush is write a file to disk immediately in order to prevent
            /// the file from being corrupted if the application crashes before the file
            /// is closed. However, on desktop platforms such as Windows this strategy is
            /// unnecesary, as the Windows OS file flush doesn't write the file to disk as 
            /// might be expected. This actually is not a problem, because the Windows OS
            /// manages files outside the process and if your process crashes the OS will 
            /// take care of safely closing the files. Only if the machine power is lost or
            /// if certain kinds of kernel-level crashes occur may you lose file data.            
            virtual bool Flush() = 0;

            /// Write
            /// Writes bytes to the stream. 
            /// If false is returned, you can use IStream::GetState to determine the error.
            /// Upon error, the stream pointer is at the position it was upon the error occurrence.
            virtual bool Write(const void* pData, size_type nSize) = 0;
        };


        // For the time being, we define IStreamRC (RefCounted IStream) to be the same 
        // as IStream. This define will be removed eventually.
        #define IStreamRC IStream


    } // namespace IO

} // namespace EA



#endif // Header include guard







