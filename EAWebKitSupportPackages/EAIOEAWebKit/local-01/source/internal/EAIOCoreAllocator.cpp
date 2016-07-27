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
// EAIOCoreAllocator.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Written and maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


#if EAIO_DLL // If this library is built to be a DLL... provide our own default new/delete-based ICoreAllocator.


namespace EA
{
	namespace IO
	{
		class CoreAllocatorMalloc : public EA::Allocator::ICoreAllocator
		{
		public:
			void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/)
				{ return new char[size]; }

			void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/, unsigned int /*align*/, unsigned int /*alignOffset*/ = 0)
				{ return new char[size]; }

			void  Free(void* p, size_t /*size*/ = 0)
				{ delete[] (char*)p; }
		};

		CoreAllocatorMalloc gCoreAllocatorMalloc;
	}


    #if EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED
	    namespace Allocator
	    {
            ICoreAllocator* ICoreAllocator::GetDefaultAllocator()
            {
                return &IO::gCoreAllocatorMalloc;
            }
	    }
    #endif

} // namespace EA



#endif // EAIO_DLL


