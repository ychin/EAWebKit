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

#include <EAIO/EAStream.h>
#include <iostream>


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
        class EAIO_API StreamCpp : public IStream
        {
        public:
            static const uint32_t kStreamType = 0x040311cf; // Random guid.

            // Note that std::iostream inherits from istream and ostream and thus
            // you can pass an iostream as both arguments here.
            StreamCpp();
            StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream);
            StreamCpp(const StreamCpp& x);
            StreamCpp& operator=(const StreamCpp& x);

            int AddRef();
            int Release();

            void SetStream(std::istream* pStdIstream, std::ostream* pStdOstream)
            {
                if (pStdIstream != mpStdIstream)
                    mpStdIstream = pStdIstream;

                if (pStdOstream != mpStdOstream)
                    mpStdOstream = pStdOstream;
            }

            uint32_t GetType() const;

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
            int           mnRefCount;
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
            mpStdOstream(NULL),
            mnRefCount(0)
        { }


        inline
        StreamCpp::StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream)
          : mnRefCount(0)
        {
            SetStream(pStdIstream, pStdOstream);
        }

        inline
        StreamCpp::StreamCpp(const StreamCpp& x)
            : IStream(), mnRefCount(0)
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







