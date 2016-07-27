/*
Copyright (C) 2009 Electronic Arts, Inc.  All rights reserved.

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
#ifndef	EA_ALLOCATOR_ICOREALLOCATOR_H
#define	EA_ALLOCATOR_ICOREALLOCATOR_H

#ifdef _MSC_VER
#pragma once
#endif

#ifndef INCLUDED_eabase_H
#include <EABase/eabase.h>
#endif

#include <coreallocator/icoreallocator_interface.h>

/// @brief This header file provides custom new operators
/// for the ICoreAllocator interface.
///
/// These custom new operators pass a name, flags, and
/// optional alignment to the DefaultAllocator.
///
/// These operators should not be used by library code.
/// Library code that needs to do heap allocations should
/// be designed to allow the library caller to provide an
/// ICoreAllocator object to the library which the library
/// would then use for performing heap management.  Or
/// library code should simply use standard new and delete,
/// which allows the library caller to override those
/// operators at their discretion.
///
/// Standard versions of the new new and delete operators
/// can be found in ../../source/newdelete.cpp.  That file
/// should be linked into the executable so all new and
/// delete operations use a consistent memory manager.
inline void *operator new(size_t size, const char *name, unsigned int flags)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags);
}

inline void *operator new(size_t size, const char *name, unsigned int flags, unsigned int align)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags, align);
}

inline void *operator new[](size_t size, const char *name, unsigned int flags)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags);
}

inline void *operator new[](size_t size, const char *name, unsigned int flags, unsigned int align)
{
	return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(size, name, flags, align);
}

#endif
