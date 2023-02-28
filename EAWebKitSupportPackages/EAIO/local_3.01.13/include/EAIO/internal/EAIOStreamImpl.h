/////////////////////////////////////////////////////////////////////////////
// EAIOStreamImpl.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
//
// Base implementation for all streams
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAIOSTREAMIMPL_H
#define EAIO_EAIOSTREAMIMPL_H

#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/internal/Config.h>
#include <EAIO/EAStream.h>
#include <eathread/eathread_atomic.h>

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4275) // non dll-interface class used as base for DLL-interface classkey 'identifier'
#endif


namespace EA
{
	namespace IO
	{

		class EAIO_API StreamImpl
			: public IStream
		{
		public:
			StreamImpl(EA::Allocator::ICoreAllocator* releaseAllocator = nullptr)
				: mReleaseAllocator(releaseAllocator) {}
			StreamImpl(const StreamImpl& rhs)
				: mReleaseAllocator(rhs.mReleaseAllocator) {}

			virtual ~StreamImpl() = default;

			/// AddRef
			///
			/// Increments the object's reference count; returns new reference count.
			///
			virtual int AddRef()
			{
				return (int)((mRefCount++) + 1);     // Must use postinc for AtomicInt32. See Release for an explanation.
			}

			/// Release
			///
			/// Decrements the object's reference count; returns new reference count.
			/// When the reference count is decremented to zero, the object deletes itself.
			/// When we are in thread-safe mode, the final Release is assumed to be thread-safe.
			/// By this we mean that no other object will attempt to AddRef us at the same time
			/// as or after this Release is called for the last time.
			///
			virtual int Release()
			{
				const int rc = (int)((mRefCount--) - 1);    // Must use postdec for AtomicInt32. This is because we act on the resulting value, 
															// and if two threads simultaneously call Release with a starting refcount of 2, they 
															// could both see the new value as 0, whereas we want only the last one to see it as 0.
				if (rc)
					return rc;

				mRefCount = 1;                               // Prevent double destroy if AddRef/Release pair occurs during destruction

				if (mReleaseAllocator)
				{
					this->~StreamImpl();
					mReleaseAllocator->Free(this, sizeof(*this));
				}
				else
					delete this; // We don't use mpAllocator, as that's for our member data.

				return 0;
			}

			/// Sets the allocator to use for freeing this object itself when the user calls 
			/// Release for the last time. If SetReleaseAllocator is not used then global delete
			/// is used. By default the global heap is used to free the memory and not the 
			/// allocator set by SetAllocator.
			///
			/// If you copy this object, the new copy will inherit the ReleaseAllocator from the original
			/// MemoryStream if there is one. Otherwise the copy will be freed using global delete
			/// 
			/// Example usage:
			///     MemoryStream* pStream = new(GetMyAllocator()->Alloc(sizeof(MemoryStream))) MemoryStream;
			///     pStream->AddRef();
			///     pStream->SetReleaseAllocator(GetMyAllocator());
			///     pStream->Release(); // Will use GetMyAllocator() to free pStream.
			void SetReleaseAllocator(EA::Allocator::ICoreAllocator* releaseAllocator) { mReleaseAllocator = releaseAllocator; }
			EA::Allocator::ICoreAllocator* GetReleaseAllocator() const { return mReleaseAllocator; }

		private:
			EA::Allocator::ICoreAllocator* mReleaseAllocator{ nullptr };		/// Allocator used to 'delete this'.
			EA::Thread::AtomicInt32 mRefCount{ 0 };
		};

		template <uint32_t StreamType>
		class EAIO_API StreamTmpl
			: public StreamImpl
		{
		public:
			static constexpr uint32_t kStreamType = StreamType;

		public:
			using StreamImpl::StreamImpl;

			// IStream implementation
			virtual uint32_t GetType() const { return kStreamType; }
		};

	} // namespace IO

} // namespace EA


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif  // Header include guard



