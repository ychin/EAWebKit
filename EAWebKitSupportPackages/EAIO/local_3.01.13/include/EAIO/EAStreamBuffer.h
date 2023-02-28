/////////////////////////////////////////////////////////////////////////////
// EAStreamBuffer.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements smart buffering around a random access IO stream.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMBUFFFER_H) && !defined(FOUNDATION_EASTREAMBUFFFER_H)
#define EAIO_EASTREAMBUFFFER_H
#define FOUNDATION_EASTREAMBUFFFER_H    // For backward compatability. Eventually, we'll want to get rid of this.


#include <EAIO/internal/Config.h>
#include <EAIO/internal/EAIOStreamImpl.h>
#include <string.h>



namespace EA
{
	namespace Allocator
	{
		class ICoreAllocator;
	}

	namespace IO
	{
		/// class StreamBuffer
		///
		/// Implements a smart buffer around a random-access stream.
		/// This buffering is significantly smarter than the file buffering 
		/// you get with the C or C++ standard libaries, and it gives the 
		/// user some configurability options as well.
		///
		/// Once you start using StreamBuffer associated with an IStream, you must
		/// not use the IStream directly, as doing so "goes around StreamBuffer's back"
		/// and any use of StreamBuffer afterwards can result in unexpected behavior.
		///
		/// Example usage:
		///     FileStream fileStream("/vol/test/file.txt");
		///     fileStream.AddRef(); // Stack instances should be AddRef'd with no matching Release.
		///
		///     if(fileStream.Open(kAccessFlagReadWrite))
		///     {
		///         StreamBuffer streamBuffer(StreamBuffer::kBufferSizeUnspecified, 8192, &fileStream);
		///         streamBuffer.Write("h", 1);
		///         streamBuffer.Write("e", 1);
		///         streamBuffer.Write("l", 1);
		///         streamBuffer.Write("l", 1);
		///         streamBuffer.Write("0", 1);
		///     }
		///
		class EAIO_API StreamBuffer : public StreamTmpl<0x12ea45bc> // Random u32 that uniquely identifies the type of stream
		{
		public:
			enum Option
			{
				kOptionCacheSize = 1   /// If enabled, then the size of the stream is cached, for higher performance. You must only enable this if you know the stream size is unchanging, as with a read-only file. This option can be set at any time, including after a stream has been used.
			};

			static const size_type kBufferSizeReadDefault  = (size_type)2048;   /// This is the default read buffer size for when you use kBufferSizeUseDefault.
			static const size_type kBufferSizeWriteDefault = (size_type)2048;   /// This is the default write buffer size for when you use kBufferSizeUseDefault.
			static const size_type kBufferSizeUseDefault   = (size_type)  -1;   /// This means that the buffer is used with the default size.
			static const size_type kBufferSizeUnspecified  = (size_type)  -2;   /// This means that the user means to leave the given buffer as-is.

			static constexpr uint32_t kTypeStreamBuffer = kStreamType;

			typedef Allocator::ICoreAllocator Allocator;

		public:
			// Buffer allocated by the supplied pAllocator or EA::IO::GetAllocator if NULL.
			StreamBuffer(size_type nReadBufferSize = kBufferSizeUnspecified,        // Unless specifically specified, no buffering is used.
						 size_type nWriteBufferSize = kBufferSizeUnspecified,       // Unless specifically specified, no buffering is used.
						 IStream* pStream = NULL, Allocator* pAllocator = NULL);

			// User-provided buffers.
			// See the SetBuffers function for more information.
			StreamBuffer(void* pReadBuffer, size_type nReadBufferSize, 
						 void* pWriteBuffer, size_type nWriteBufferSize,
						 IStream* pStream = NULL);

		   ~StreamBuffer();

			IStream* GetStream() const;
			bool     SetStream(IStream* pStream);

			void GetBufferSizes(size_type& nReadBufferSize, size_type& nWriteBufferSize) const;

			// This cannot be used if the user called SetBuffers.
			// Either of the buffer sizes may be zero, which indicates to not do that buffering.
			bool SetBufferSizes(size_type  nReadBufferSize, size_type  nWriteBufferSize);

			// Allows the user to provide their own buffer.
			// A user-provided buffer must have at least 16 bytes. There is no max size limit, and the 
			// size may be bigger than the file stream being used, though that provides no benefit.
			// Either (or both) of the two buffers provided may be NULL or of zero size, which indicates to not do that buffering.
			// The user cannot call SetBuffers once to set a read buffer but not a write buffer then 
			// again to set the write buffer but not the read buffer. Doing so will result in the second
			// call disabling the read buffer. You have to do both in one call to SetBuffers.
			void SetBuffers(void* pReadBuffer, size_type nReadBufferSize, void* pWriteBuffer, size_type nWriteBufferSize);

			void SetOption(int option, int value);
			void SetAllocator(Allocator* pAllocator);

			virtual int       GetAccessFlags() const;
			virtual int       GetState() const;
			virtual bool      Close();
			virtual size_type GetSize() const;
			virtual bool      SetSize(size_type size);
			virtual off_type  GetPosition(PositionType positionType = kPositionTypeBegin) const;
			virtual bool      SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);            

			virtual size_type GetAvailable() const;
			virtual size_type Read(void* pData, size_type nSize);

			virtual bool      Flush();
			virtual bool      Write(const void* pData, size_type nSize);

		protected:
			void        InternalCtor(Allocator* pAllocator);
			void        FreeBuffers();
			void*       Realloc(void* p, size_type prevSize, size_type newSize);
			void        FlushAndClearBuffers();
			void        ClearReadBuffer();
			bool        FillReadBuffer();
			void        ClearWriteBuffer();
			bool        FillWriteBuffer(const char* pData, size_type nSize);
			bool        FlushWriteBuffer();

		protected:
			IStream*            mpStream;                      /// The stream that we are buffering.
			bool                mbEnableSizeCache;             /// If true, allow caching of the size of mpStream, for performance improvement.
			mutable size_type   mnStreamSize;                  /// Cached version of the size of mpStream, for performance improvement. Can be used when mpStream is read-only.
			size_type           mnPositionExternal;            /// This is the position of the the file pointer as the the user sees it. It is where the next byte read or write will come from or go to.
			size_type           mnPositionInternal;            /// cached version of mpStream->GetPosition(). mnPositionInternal == mpStream->GetPosition().
			Allocator*          mpAllocator;                   /// If non-NULL, then mpReadBuffer/mpWriteBuffer were allocated with it and should be freed with it.
			bool                mbUserProvidedBuffers;         /// True if the buffers are user-provided (e.g. from SetBuffers or the buffer-providing constructor).

			char*               mpReadBuffer;                  /// Pointer to buffer data.
			size_type           mnReadBufferSize;              /// This is the size of the read buffer available for our usage.
			size_type           mnReadBufferStartPosition;     /// This is where in the file the beginning the read buffer corresponds to. 
			size_type           mnReadBufferUsed;              /// This is the count of bytes in the read buffer that are valid.

			char*               mpWriteBuffer;                 /// Pointer to buffer data.
			size_type           mnWriteBufferSize;             /// This is the size of the write buffer available for our usage.
			size_type           mnWriteBufferStartPosition;    /// This is where in the file the beginning the write buffer corresponds to.
			size_type           mnWriteBufferUsed;             /// This is the count of bytes in the write buffer that are valid.
		};


	} // namespace IO

} // namespace EA





/////////////////////////////////////////////////////////////////////////////
// inlines
/////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace IO
	{

		inline
		IStream* StreamBuffer::GetStream() const
		{
			// We do not AddRef the returned stream.
			return mpStream;
		}

		inline
		void StreamBuffer::GetBufferSizes(size_type& nReadBufferSize, size_type& nWriteBufferSize) const
		{
			nReadBufferSize  = mnReadBufferSize;
			nWriteBufferSize = mnWriteBufferSize;
		}


	} // namespace IO

} // namespace EA


#endif // Header include guard







