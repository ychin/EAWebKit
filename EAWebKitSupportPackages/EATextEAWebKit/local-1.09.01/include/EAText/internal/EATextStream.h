/*
Copyright (C) 2006, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextStream.h
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSTREAM_H
#define EATEXT_INTERNAL_EATEXTSTREAM_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileStream.h>


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }

    namespace IO // We put this in the IO namespace instead of EAText because some compilers don't work with cross-namespaces very well, and so this is safer.
    {

        /// EATextFileStream
        ///
        /// This class exists for the sole purpose of providing a means to be allocated and freed by 
        /// an ICoreAllocator while supporting reference-counting. 
        ///
        class EATextFileStream : public FileStream
        {
        public:
            EATextFileStream(const char16_t* pPath)
                : FileStream(pPath), mpCoreAllocator(NULL) { }

            int Release();

            EA::Allocator::ICoreAllocator* mpCoreAllocator; // Allocator used to allocate and free this instance.
        };


        /// EATextMemoryStream
        ///
        /// This class exists for the sole purpose of providing a means to be allocated and freed by 
        /// an ICoreAllocator while supporting reference-counting. 
        ///
        class EATextMemoryStream : public MemoryStream
        {
        public:
            EATextMemoryStream(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, MemoryStream::Allocator* pAllocator = NULL)
                : MemoryStream(pData, nSize, bUsePointer, bFreePointer, pAllocator), mpCoreAllocator(NULL) { }

            int Release();

            EA::Allocator::ICoreAllocator* mpCoreAllocator; // Allocator used to allocate and free this instance.
        };

    } // namespace IO

} // namespace EA



#endif // Header include guard













