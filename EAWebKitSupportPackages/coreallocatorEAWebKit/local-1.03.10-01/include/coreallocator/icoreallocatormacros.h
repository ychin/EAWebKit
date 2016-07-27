/*
Copyright (C) 2009, 2012 Electronic Arts, Inc.  All rights reserved.

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


#ifndef EA_ALLOCATOR_ICOREALLOCATORMACROS_H
#define EA_ALLOCATOR_ICOREALLOCATORMACROS_H

#ifdef _MSC_VER
#pragma once
#endif

/// @brief This header file defines macros for accessing ICoreAllocator.

#include "coreallocator/icoreallocator_interface.h"

/// @remarks
/// CORE_ALLOCATOR_DEBUGPARAMS_LEVEL controls what debug information is passed
/// through to ICoreAllocator Alloc methods by the CORE_NEW & CORE_ALLOC macros.
/// This value may be defined by the user ... if not it will default to 1 for
/// EA_DEBUG builds, otherwise 0.
///
/// 0 - no debug information is passed through to CORE_NEW/CORE_ALLOC calls.
/// 1 - 'name' is passed through to CORE_NEW/CORE_ALLOC calls.
/// >1 - 'name', __FILE__, and __LINE__ are passed through to CORE_NEW/CORE_ALLOC calls.
///
/// CORE_ALLOCATOR_DEBUGPARAMS_LEVEL can be redefined as needed in different builds
/// or sections of code (or more likely different libraries) within the same build
/// (thus some code can pass debug parameters while other code does not).
/// This define should not be used to alter function signatures, object sizes or
/// anything else that would make code compiled with it on incompatible with code
/// compiled with it off.

#if !defined(CORE_ALLOCATOR_DEBUGPARAMS_LEVEL) || (CORE_ALLOCATOR_DEBUGPARAMS_LEVEL < 0)
   #if defined(EA_DEBUG)
      #define CORE_ALLOCATOR_DEBUGPARAMS_LEVEL (1)
   #else
      #define CORE_ALLOCATOR_DEBUGPARAMS_LEVEL (0)
   #endif
#endif

/// @remarks
/// Helper macro for passing debug params to Alloc methods only if desired.
/// See CORE_ALLOCATOR_DEBUGPARAMS_LEVEL for details.

#if CORE_ALLOCATOR_DEBUGPARAMS_LEVEL > 0
    #if CORE_ALLOCATOR_DEBUGPARAMS_LEVEL > 1
        #define CORE_ALLOCATOR_DEBUGPARAMS(_allocname)    EA::Allocator::ICoreAllocator::DebugParams((_allocname), __FILE__, __LINE__)
        #define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED        1
    #else
        #define CORE_ALLOCATOR_DEBUGPARAMS(_allocname)    (_allocname)
        #define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED        0
    #endif
#else
    #define CORE_ALLOCATOR_DEBUGPARAMS(_allocname)        NULL
    #define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED            0
#endif

#include "coreallocator/detail/macro_impl.h"

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _allocname   string to give allocation name
/// @param _flags:      additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
/// @return             Allocated and constructed object
///
/// Example usage:
///    Widget* widget = CORE_NEW(allocator, "UI/Widget", EA::Allocator::MEM_TEMP) Widget;
///    CORE_DELETE(allocator,widget);
///
/// @remarks
/// It is NOT valid to create an array using this macro, use CORE_NEW_ARRAY instead.
/// If you attempt to create an array (by putting brackets after the class name) a link
/// error will be generated.
///
/// It is not possible to have this functionality via an override of operator new,
/// as operator new/delete only understand custom implementations on a class-by-class
/// basis and not on an allocator instance basis. The C++ language provides no way to
/// call operator delete with an allocator parameter.

#define CORE_NEW(_allocator, _allocname, _flags) \
    new(EA::Allocator::detail::opNonArrayNew(), (_allocator), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags))

#define CORE_GLOBAL_NEW(_allocator, _allocname, _flags) ::CORE_NEW(_allocator, _allocname, _flags)


/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _allocname   string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param _align       Memory alignment to give (must be power of 2)
/// @return             Allocated and constructed object
///
/// Example usage: //allocate a widget aligned to 32 bytes, then delete it
///    Widget* widget = CORE_NEW_ALIGN(allocator, "UI/Widget", EA::Allocator::MEM_TEMP, 32) Widget;
///    CORE_DELETE(allocator,widget);

#define CORE_NEW_ALIGN(_allocator, _allocname, _flags, _align) \
    new(EA::Allocator::detail::opNonArrayNew(), (_allocator), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags), (_align))

#define CORE_GLOBAL_NEW_ALIGN(_allocator, _allocname, _flags, _align) ::CORE_NEW_ALIGN(_allocator, _allocname, _flags, _align)


/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _ptr         pointer to the block previously allocated with CORE_NEW or CORE_NEW_ALIGN

#define CORE_DELETE(_allocator, _ptr) EA::Allocator::detail::DeleteObject((_allocator),(_ptr))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _type        Type of the object being deleted.
/// @param _ptr         pointer to the block previously allocated with CORE_NEW or CORE_NEW_ALIGN
///
/// @remarks
/// This method may be needed when a class with a private destructor needs to delete an instance of
/// the class. Using CORE_DELETE in this situation may cause a compile error since CORE_DELETE calls a
/// helper method that will attempt to call the private destructor. This version of the macro avoids
/// this problem but the user has to supply the type to be deleted.

#define CORE_DELETE_PRIVATE(_allocator, _type, _ptr) ((_ptr) ? (_ptr->~_type(), _allocator->Free(_ptr)) : (void)(_ptr))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _objtype     Type of object to allocate
/// @param _count       number of objects to allocate
/// @param _allocname   string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated and constructed objects
///
/// Example usage: //Allocates an array of 42 widgets, then destroys it
///    Widget* widgetArray = CORE_NEW_ARRAY(allocator, Widget, 42, "42 Widgets", EA::Allocator::MEM_TEMP);
///    CORE_DELETE_ARRAY(allocator,widgetArray);

#define CORE_NEW_ARRAY(_allocator, _objtype, _count, _allocname, _flags)   \
    EA::Allocator::detail::CreateArray<_objtype>((_allocator), (_count), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _objtype     Type of object to allocate
/// @param _count       number of objects to allocate
/// @param _allocname   string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated and constructed objects
///
/// Example usage: //Allocates an array of 42 widgets, then destroys it
///    Widget* widgetArray = CORE_NEW_ARRAY(allocator, Widget, 42, "42 Widgets", EA::Allocator::MEM_TEMP);
///    CORE_DELETE_ARRAY(allocator,widgetArray);

#define CORE_NEW_ARRAY_UNINITIALIZED(_allocator, _objtype, _count, _allocname, _flags)   \
    EA::Allocator::detail::CreateArrayUninitialized<_objtype>((_allocator), (_count), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _objtype     Type of object to allocate
/// @param _count       number of objects to allocate.
/// @param _allocname   string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param _align       Memory alignment to give (must be power of 2)
/// @return             Allocated and constructed objects

#define CORE_NEW_ARRAY_ALIGN(_allocator, _objtype, _count, _allocname, _flags, _align) \
    EA::Allocator::detail::CreateArray<_objtype>((_allocator), (_count), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags), (_align))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _ptr         pointer to the array previously allocated with CORE_NEW_ARRAY or CORE_NEW_ARRAY_ALIGN

#define CORE_NEW_ARRAY_ALIGN_UNINITIALIZED(_allocator, _objtype, _count, _allocname, _flags, _align) \
    EA::Allocator::detail::CreateArrayUninitialized<_objtype>((_allocator), (_count), CORE_ALLOCATOR_DEBUGPARAMS((_allocname)), (_flags), (_align))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _ptr         pointer to the array previously allocated with CORE_NEW_ARRAY or CORE_NEW_ARRAY_ALIGN

#define CORE_DELETE_ARRAY(_allocator, _ptr) EA::Allocator::detail::DeleteArray((_allocator),(_ptr))

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _size        size of allocation
/// @param _name        string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated memory
#if CORE_ALLOCATOR_DEBUGPARAMS_ENABLED
    #define CORE_ALLOC(_allocator, _size, _name, _flags) ((_allocator)->AllocDebug((_size), CORE_ALLOCATOR_DEBUGPARAMS((_name)), (_flags)))
#else
    #define CORE_ALLOC(_allocator, _size, _name, _flags) ((_allocator)->Alloc((_size), CORE_ALLOCATOR_DEBUGPARAMS((_name)), (_flags)))
#endif

/// @param _allocator   _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _size        size of allocation
/// @param _name        string to give allocation name
/// @param _flags       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param _align       desired alignment
/// @param _alignoffset desired offset that requires _align
/// @return             Allocated memory
#if CORE_ALLOCATOR_DEBUGPARAMS_ENABLED
    #define CORE_ALLOC_ALIGN(_allocator, _size, _name, _flags, _align, _alignoffset) ((_allocator)->AllocDebug((_size), CORE_ALLOCATOR_DEBUGPARAMS((_name)), (_flags), (_align), (_alignoffset)))
#else
    #define CORE_ALLOC_ALIGN(_allocator, _size, _name, _flags, _align, _alignoffset) ((_allocator)->Alloc((_size), CORE_ALLOCATOR_DEBUGPARAMS((_name)), (_flags), (_align), (_alignoffset)))
#endif

/// @param _allocator:      _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _block:           pointer to the block to be freed.

#define CORE_FREE(_allocator, _block) ((_allocator)->Free((_block),0))

/// @param _allocator:      _allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param _block:           pointer to the block to be freed.
/// @param _size:            size of allocation may be used to find the pool

#define CORE_FREE_SIZE(_allocator, _block, _size) ((_allocator)->Free((_block),(_size)))

#endif

