/////////////////////////////////////////////////////////////////////////////
// EAStreamMemory.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements a IO stream that reads and writes to a block of memory.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMMEMORY_H) && !defined(FOUNDATION_EASTREAMMEMORY_H)
#define EAIO_EASTREAMMEMORY_H
#define FOUNDATION_EASTREAMMEMORY_H // For backward compatability. Eventually, we'll want to get rid of this.


#include <EAIO/internal/Config.h>
#include <EAIO/internal/EAIOStreamImpl.h>
#ifndef EAIO_ZONEOBJECT_H
	#include <EAIO/internal/EAIOZoneObject.h>
#endif
#include <string.h>
#include <stddef.h>



/// namespace EA
/// Standard Electronic Arts Framework namespace.
namespace EA
{
	/// namespace IO
	/// The namespace for general IO (input/output) functionality.
	namespace IO
	{
		/// SharedPointer
		///
		/// Implements a basic ref-counted pointer.
		///
		/// This class is meant to be used like a COM object. When this object's 
		/// reference count goes to zero, the memory it holds is deleted and then
		/// this object calls 'delete this'. This class is similar to but doesn't
		/// work exactly the same as Boost's shared_ptr template.
		/// A typical usage pattern is like so:
		///     SharedPointer* pSP = new SharedPointer(1000);
		///     pSP->AddRef();
		///     pSP->Release();
		///
		class EAIO_API SharedPointer : public EA::Allocator::EAIOZoneObject
		{
		public:
			typedef EA::Allocator::ICoreAllocator Allocator;

			SharedPointer(void* pData = NULL, bool bFreeData = true, Allocator* pAllocator = NULL);
			SharedPointer(size_type nSize, const char* pName = NULL);
			SharedPointer(size_type nSize, Allocator* pAllocator, const char* pName = NULL);

			void*      GetPointer();
			int        AddRef();
			int        Release();
			Allocator* GetAllocator() const;

		protected:
			Allocator*  mpAllocator;
			uint8_t*    mpData;
			int         mnRefCount;
			bool        mbFreeData; // If true, we free the data when done.
		};


		/// class MemoryStream
		///
		/// Implements an memory-based stream that supports the IStream interface.
		///
		/// This class is not inherently thread-safe. As a result, thread-safe usage 
		/// between multiple threads requires higher level coordination, such as a mutex.
		///
		class EAIO_API MemoryStream : public StreamTmpl<0x347223d2> // Random u32 that uniquely identifies the type of stream
		{
		public:
			typedef EA::Allocator::ICoreAllocator Allocator;
			static constexpr uint32_t kTypeMemoryStream = kStreamType;

			/// enum Options
			/// Specifies policies regarding the internal operation of this class.
			enum Options
			{
				kOptionNone            =  0,    /// No options
				kOptionResizeEnabled   =  1,    /// 0 or 1. Default is disabled.  If set, then the buffer is automatically resized on beyond-bounds position sets, beyond-bounds writes, and beyond-bounds SetSize calls.
				kOptionResizeFactor    =  4,    /// 1.0+    Default is 1.5.       Specifies how much a resize multiplies in size; is applied before kOptionResizeIncrement. Can be 1.0 if kOptionResizeIncrement > 0.
				kOptionResizeIncrement =  8,    /// 0.0+    Default is 0.0.       Specifies how much a resize increments; is applied after kOptionResizeFactor. Can be set to zero if kOptionResizeFactor is > 1.
				kOptionResizeMaximum   = 16     /// 0+      Default is 0.         Specifies the maximum size (0 = unlimited).
			  //kOptionClearNewMemory  = 32     /// 0 or 1. Default is 0.         If set, then newly allocated space is cleared to zero. Otherwise the space is left as it comes from the memory allocator.
			};

			MemoryStream(SharedPointer* pSharedPointer = NULL, size_type nSize = 0, const char* pName = NULL);
			MemoryStream(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator* pAllocator = NULL, const char* pName = NULL); // If bUsePointer is true, then we take over ownership of pData instead of copying from it.
			MemoryStream(const MemoryStream& memoryStream);
			virtual ~MemoryStream();

			float   GetOption(Options option) const;
			void    SetOption(Options option, float fValue);

			// Specifies the allocator used by internal operations. If you want to specify the allocator
			/// used to delete this instance, use SetReleaseAllocator.
			void       SetAllocator(Allocator* pAllocator);
			Allocator* GetAllocator() const;

			void*   GetData() const;
			bool    SetData(SharedPointer* pSharedPointer, size_type nSize);
			bool    SetData(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator *pAllocator = NULL);

			SharedPointer* GetSharedPointer();

			size_type   GetCapacity() const;
			bool        SetCapacity(size_type size);

			// IStream
			int         GetAccessFlags() const;
			int         GetState() const;
			bool        Close();
			// To consider: Add an Open function which re-activates a Closed MemoryStream.

			size_type   GetSize() const;
			bool        SetSize(size_type size);

			off_type    GetPosition(PositionType positionType = kPositionTypeBegin) const;
			bool        SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

			size_type   GetAvailable() const;
			size_type   Read(void* pData, size_type nSize);

			bool        Flush();
			bool        Write(const void* pData, size_type nSize);

		protected:
			bool Realloc(size_type nSize);

			SharedPointer* mpSharedPointer;     /// Pointer to memory block.
			Allocator*     mpAllocator;         /// Allocator used to allocate internal memory.
			//Allocator*     mpReleaseAllocator;  /// Allocator used to 'delete this'.
			const char*    mpName;              /// Memory allocation name.
			//int            mnRefCount;          /// Reference count. May or may not be in use.
			size_type      mnSize;              /// The size of the stream, in bytes.
			size_type      mnCapacity;          /// The size of the memory buffer, in bytes.
			size_type      mnPosition;          /// Current position within memory block.
		  //bool           mbClearNewMemory;    /// True if clearing of newly allocated memory is enabled.
			bool           mbResizeEnabled;     /// True if resizing is enabled.
			float          mfResizeFactor;      /// Specifies how capacity is increased.
			int            mnResizeIncrement;   /// Specifies how capacity is increased.
			int            mnResizeMax;         /// Maximum resize amount
		};

	} // namespace IO

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// SharedPointer
///////////////////////////////////////////////////////////////////////////////

inline void* EA::IO::SharedPointer::GetPointer()
{
	return mpData;
}


inline EA::IO::SharedPointer* EA::IO::MemoryStream::GetSharedPointer()
{
	return mpSharedPointer;
}


inline int EA::IO::SharedPointer::AddRef()
{
	return ++mnRefCount;
}


inline EA::IO::SharedPointer::Allocator* EA::IO::SharedPointer::GetAllocator() const
{
	return mpAllocator;
}



///////////////////////////////////////////////////////////////////////////////
// MemoryStream
///////////////////////////////////////////////////////////////////////////////

inline EA::IO::MemoryStream::~MemoryStream()
{
	if(mpSharedPointer)
		mpSharedPointer->Release();
}

inline void EA::IO::MemoryStream::SetAllocator(Allocator* pAllocator)
{
	mpAllocator = pAllocator;
}


inline EA::IO::MemoryStream::Allocator* EA::IO::MemoryStream::GetAllocator() const
{
	return mpAllocator;
}


inline void* EA::IO::MemoryStream::GetData() const
{
	if(mpSharedPointer)
		return mpSharedPointer->GetPointer();
	return 0;
}


inline EA::IO::size_type EA::IO::MemoryStream::GetCapacity() const
{
	return mnCapacity;
}

inline int EA::IO::MemoryStream::GetAccessFlags() const
{
	return kAccessFlagReadWrite;    // To do: We should return 0 here when Closed.
}


inline int EA::IO::MemoryStream::GetState() const
{
	return kStateSuccess; // To do: We should return kStateNotOpen here when Closed instead of always returning kStateSuccess.
}


inline bool EA::IO::MemoryStream::Close()
{
	return SetData(NULL, 0, false);
}


inline EA::IO::size_type EA::IO::MemoryStream::GetSize() const
{
	return mnSize;
}


inline EA::IO::size_type EA::IO::MemoryStream::GetAvailable() const
{
	// assert(mnPosition <= mnSize);
	return (mnSize - mnPosition);
}


inline bool EA::IO::MemoryStream::Flush()
{
	// Nothing to do.
	return true;
}




#endif // Header include guard







