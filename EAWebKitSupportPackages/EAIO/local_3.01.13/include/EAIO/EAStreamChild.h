/////////////////////////////////////////////////////////////////////////////
// EAStreamChild.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
//
// Implements a fixed-size read-only stream which is a 'child' of a parent
// stream. This is useful if you have a system whereby a single large file
// consists of smaller files or a single large database record consists of
// multiple sub-records and you want each sub-record to look like a standalone
// stream to the user.
//
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMCHILD_H) && !defined(FOUNDATION_EASTREAM_CHILD_H)
#define EAIO_EASTREAMCHILD_H
#define FOUNDATION_EASTREAM_CHILD_H // For backward compatability. Eventually, we'll want to get rid of this.


#ifndef INCLUDED_eabase_H
	#include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#include <EAIO/internal/EAIOStreamImpl.h>


namespace EA
{
	namespace IO
	{
		/// class StreamChild
		///
		/// Implements a fixed-size read-only stream which is a 'child' of a parent
		/// stream. This is useful if you have a system whereby a single large file
		/// consists of smaller files or a single large database record consists of
		/// multiple sub-records and you want each sub-record to look like a standalone
		/// stream to the user.
		///
		/// This class is not inherently thread-safe. As a result, thread-safe usage 
		/// between multiple threads requires higher level coordination, such as a mutex.
		///
		class EAIO_API StreamChild : public StreamTmpl<0x3472233a> // Random u32 that uniquely identifies the type of stream
		{
		public:
			static constexpr uint32_t kTypeStreamChild = kStreamType;

		public:
			StreamChild(IStream* pStreamParent = NULL, size_type nPosition = 0, size_type nSize = 0);
		   ~StreamChild();

			IStream*  GetStream() const;
			bool      SetStream(IStream* pStream);

			bool      Open(IStream* pStreamParent, size_type nPosition, size_type nSize);
			bool      Close();
			int       GetAccessFlags() const;
			int       GetState() const;
			size_type GetSize() const;
			bool      SetSize(size_type);
			off_type  GetPosition(PositionType positionType = kPositionTypeBegin) const;
			bool      SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

			size_type GetAvailable() const;
			size_type Read(void* pData, size_type nSize);

			bool      Flush();
			bool      Write(const void* pData, size_type nSize);

		protected:
			int         mnAccessFlags;
			IStream*    mpStreamParent;
			size_type   mnPositionParent;                
			size_type   mnPosition;
			size_type   mnSize;
		};

	} // namespace IO

} // namespace EA



#endif // Header include guard
















