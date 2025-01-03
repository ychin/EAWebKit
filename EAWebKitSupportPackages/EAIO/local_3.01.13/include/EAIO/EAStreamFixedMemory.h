/////////////////////////////////////////////////////////////////////////////
// EAStreamFixedMemory.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Talin
// Based on EAStreamMemory.h by PaulPedriana.
//
// Implements a IO stream that reads from a fixed-length block of memory.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMFIXEDMEMORY_H) && !defined(FOUNDATION_EASTREAMFIXEDMEMORY_H)
#define EAIO_EASTREAMFIXEDMEMORY_H
#define FOUNDATION_EASTREAMFIXEDMEMORY_H    // For backward compatability. Eventually, we'll want to get rid of this.


#include <EAIO/internal/Config.h>
#include <EAIO/internal/EAIOStreamImpl.h>


namespace EA
{
	namespace IO
	{
		/// Implements an memory-based stream that supports the IStream interface.
		///
		/// This class is not inherently thread-safe. As a result, thread-safe usage 
		/// between multiple threads requires higher level coordination, such as a mutex.
		///
		class EAIO_API FixedMemoryStream : public StreamTmpl<0x02f2f470> // Random u32 that uniquely identifies the type of stream
		{
		public:
			static constexpr uint32_t kTypeFixedMemoryStream = kStreamType;

			FixedMemoryStream( void *pBuffer = NULL, size_type nSize = 0 );
			FixedMemoryStream( FixedMemoryStream& memoryStream );
			virtual ~FixedMemoryStream();

			void*       GetData() const;
			bool        SetData( void* pData, size_type nSize );

			size_type   GetCapacity() const;

			// IStream
			int         GetAccessFlags() const;
			int         GetState() const;
			bool        Close();

			size_type   GetSize() const;
			bool        SetSize(size_type size);

			off_type    GetPosition( PositionType positionType = kPositionTypeBegin ) const;
			bool        SetPosition( off_type position, PositionType positionType = kPositionTypeBegin );

			size_type   GetAvailable() const;
			size_type   Read(void* pData, size_type nSize);

			bool        Flush();
			bool        Write(const void* pData, size_type nSize);

		protected:
			void         * mpData;
			size_type      mnSize;              /// The size of the stream, in bytes.
			size_type      mnCapacity;          /// The size of the memory buffer, in bytes.
			size_type      mnPosition;          /// Current position within memory block.
		};

	} // namespace IO

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


inline EA::IO::FixedMemoryStream::~FixedMemoryStream() {}


inline void* EA::IO::FixedMemoryStream::GetData() const
{
	return mpData;
}


inline EA::IO::size_type EA::IO::FixedMemoryStream::GetCapacity() const
{
	return mnCapacity;
}


inline int EA::IO::FixedMemoryStream::GetAccessFlags() const
{
	return kAccessFlagReadWrite;
}


inline int EA::IO::FixedMemoryStream::GetState() const
{
	return kStateSuccess;
}


inline bool EA::IO::FixedMemoryStream::Close()
{
	return true;
}


inline EA::IO::size_type EA::IO::FixedMemoryStream::GetSize() const
{
	return mnSize;
}


inline EA::IO::size_type EA::IO::FixedMemoryStream::GetAvailable() const
{
	// assert(mnPosition <= mnSize);
	return (mnSize - mnPosition);
}


inline bool EA::IO::FixedMemoryStream::Flush()
{
	// Nothing to do.
	return true;
}




#endif // Header include guard







