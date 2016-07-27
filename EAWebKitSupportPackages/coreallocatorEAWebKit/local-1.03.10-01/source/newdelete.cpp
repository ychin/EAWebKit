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


/// @brief This source file provides custom versions of the standard
/// new/delete operators.
///
/// These new and delete operators are intended to override the default
/// new and delete operators.  The executable author may wish to include
/// this source file into their project.  By overriding standard new and
/// delete, all heap allocation goes through the default allocator.  The
/// executable creator is responsible for providing a function called
/// EA::Allocator::ICoreAllocator::GetDefaultAllocator
///
/// This module cannot be in a library - it must be explicitly linked into
/// the executable.  If this module is in a library, the linker may decide
/// to ignore it in favour of the built-in standard new/delete operators.

#include <coreallocator/icoreallocator_interface.h>

// scalar new
void *operator new(size_t size)
{
    return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(
        size, "Unnamed block", EA::Allocator::MEM_PERM);
}

// scalar delete
void operator delete(void *p)
{
    EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}

// array new
void *operator new[](size_t size)
{
    return EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(
        size, "Unnamed block", EA::Allocator::MEM_PERM);
}

// array delete
void operator delete[](void *p)
{
    EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(p);
}
