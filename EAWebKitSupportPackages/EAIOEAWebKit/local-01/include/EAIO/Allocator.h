/*
Copyright (C) 2007,2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// Allocator.h
// Created by Paul Pedriana - 2007
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_ALLOCATOR_H
#define EAIO_ALLOCATOR_H


#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


namespace EA
{
    namespace IO
    {
        /// GetAllocator
        /// 
        /// Gets the default EAIO ICoreAllocator set by the SetAllocator function.
        /// If SetAllocator is not called, the ICoreAllocator::GetDefaultAllocator 
        /// allocator is returned.
        ///
        EAIO_API Allocator::ICoreAllocator* GetAllocator();


        /// SetAllocator
        /// 
        /// This function lets the user specify the default memory allocator this library will use.
        /// For the most part, this library avoids memory allocations. But there are times 
        /// when allocations are required, especially during startup. Currently the Font Fusion
        /// library which sits under EAIO requires the use of a globally set allocator.
        ///
        EAIO_API void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

    }

}

#endif // Header include guard














