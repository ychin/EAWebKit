///////////////////////////////////////////////////////////////////////////////
// EAStreamCpp.h
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Pedriana
//
// Implements a stream which wraps around a C++ std::istream and std::ostream.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EASTREAMCPP_H
#define EAIO_EASTREAMCPP_H


#include <EAIO/internal/Config.h>


#if EAIO_CPP_STREAM_ENABLED

#include <EAIO/internal/EAIOStreamImpl.h>

#if defined(_MSC_VER)
	#pragma warning(push, 0)
	#pragma warning(disable: 4350) // for whatever reason, the push,0 above does not turn this warning off with vs2012.
#endif                             // VC++ 2012 STL headers generate this. warning C4350: behavior change: 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc(const std::_Wrap_alloc<_Alloc> &) throw()' called instead of 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc<std::_Wrap_alloc<_Alloc>>(_Other &) throw()'

#include <iostream>

#if defined(_MSC_VER)
	#pragma warning(pop)
	#pragma warning(disable: 4275) // non dll-interface class 'stdext::exception' used as base for dll-interface class 'std::bad_cast'
#endif


namespace EA
{
	namespace IO
	{
		/// StreamCpp
		///
		/// Implements an IStream that uses a C++ std::istream, std::ostream,
		/// and/or std::iostream as an underlying interface.
		///
		/// In order to deal with std C++ istream, ostream, and iostream, 
		/// this class works with istream and ostream independently. 
		///
		/// Example usage:
		///    std::fstream fileStream;
		///    StreamCpp    streamCpp(&fileStream, &fileStream);
		///
		class EAIO_API StreamCpp : public StreamTmpl<0x040311cf> // Random u32 that uniquely identifies the type of stream
		{
		public:

			// Note that std::iostream inherits from istream and ostream and thus
			// you can pass an iostream as both arguments here.
			StreamCpp();
			StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream);
			StreamCpp(const StreamCpp& x);
			StreamCpp& operator=(const StreamCpp& x);

			void SetStream(std::istream* pStdIstream, std::ostream* pStdOstream)
			{
				if (pStdIstream != mpStdIstream)
					mpStdIstream = pStdIstream;

				if (pStdOstream != mpStdOstream)
					mpStdOstream = pStdOstream;
			}

			int  GetAccessFlags() const;

			int  GetState() const;

			// This is a no-op as std C++ streams don't have a close() method but the IStream interface
			// has a pure-virtual Close() method.
			bool Close()
			{
				// There isn't any way to close a std C++ stream. You would 
				// need a higher level fstream to do something like that.
				return true;
			}

			size_type GetSize() const;
			bool      SetSize(size_type size);

			off_type GetPosition(PositionType positionType = kPositionTypeBegin) const;
			bool     SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

			size_type GetAvailable() const;
			size_type Read(void* pData, size_type nSize);

			bool      Flush();
			bool      Write(const void* pData, size_type nSize);

			// Clear maps directly to the C++ stream clear() function.
			void      Clear(bool clearInput = true, bool clearOutput = true);

		protected:
			std::istream* mpStdIstream;
			std::ostream* mpStdOstream;
		};

	} // namespace IO

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace IO
	{
		inline
		StreamCpp::StreamCpp()
		  : mpStdIstream(NULL),
			mpStdOstream(NULL)
		{ }


		inline
		StreamCpp::StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream)
		  : mpStdIstream(NULL),
			mpStdOstream(NULL)
		{
			SetStream(pStdIstream, pStdOstream);
		}

		inline
		StreamCpp::StreamCpp(const StreamCpp& x)
			: StreamTmpl(x),
			  mpStdIstream(NULL),
			  mpStdOstream(NULL)
		{
			SetStream(x.mpStdIstream, x.mpStdOstream);
		}

		inline
		StreamCpp& StreamCpp::operator=(const StreamCpp& x)
		{
			SetStream(x.mpStdIstream, x.mpStdOstream);

			return *this;
		}

	} // namespace IO

} // namespace EA


#endif // EAIO_CPP_STREAM_ENABLED

#endif // Header include guard







