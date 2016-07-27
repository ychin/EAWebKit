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
// EAStackAllocatorNew.h
// Created by Paul Pedriana - 2006
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file provides the following macros:
//     EATEXT_SA_NEW(Class, pAllocator, pName) 
//     EATEXT_SA_NEW_ARRAY(Class, pAllocator, pName, nCount)
//     EATEXT_SA_DELETE(pObject, pAllocator)
//     EATEXT_SA_DELETE_ARRAY(pArray, pAllocator)
//
// These are useful for creating and destroying arbitrary C++ class instances 
// via ICoreAllocator-derived memory. 
//
// Note that you don't need to use these macros nor the create_object 
// functions if you can instead overload operator new for a given class. 
// This is often the case when you are defining the class yourself as opposed 
// to working with some arbitrary class that somebody else created and you 
// are merely working with.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_STACKALLOCATORNEW_H
#define EATEXT_INTERNAL_STACKALLOCATORNEW_H


#include <EABase/eabase.h>
#include <PPMalloc/EAStackAllocator.h>
#include <new>

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4127) // conditional expression is constant
#endif


/// EATEXT_SA_NEW
///
/// Macro to use operator new via PPMalloc/Stack interface. This macro is not 
/// required, as you can directly use Allocator::create_object / delete_object.
/// This is the single object version of new, to create an array of objects,
/// use EATEXT_SA_NEW_ARRAY instead.
///
/// See Allocator::create_object for details regarding behaviour of this 
/// system of object allocation. 
///
/// Note that you don't need to use these macros nor the create_object functions
/// if you can instead overload operator new for a given class. This is often
/// the case when you are defining the class yourself as opposed to working with
/// some arbitrary class that somebody else created and you are merely working with.
///
/// To do: Find a way to implement file/line recording functionality.
///
/// Example usage:
///     Widget* pWidget = EATEXT_SA_NEW(Widget, pAllocator);
///     EATEXT_SA_DELETE(pWidget, pAllocator);
///
#define EATEXT_SA_NEW(Class, pAllocator) EA::Text::create_object<Class>(pAllocator)


/// EATEXT_SA_NEW_ARRAY
///
/// This is an array version of EATEXT_SA_NEW
///
/// Example usage:
///     Widget* pWidget = EATEXT_SA_NEW_ARRAY(Widget, pAllocator, 10);
///     EATEXT_SA_DELETE_ARRAY(pWidget, pAllocator);
///
#define EATEXT_SA_NEW_ARRAY(Class, pAllocator, nCount) EA::Text::create_array<Class>(pAllocator, nCount)


/// EATEXT_SA_DELETE
///
/// This is a matching delete for EATEXT_SA_NEW.
/// See EATEXT_SA_NEW for example code.
///
#define EATEXT_SA_DELETE(pObject) EA::Text::delete_object(pObject)


/// EATEXT_SA_DELETE_ARRAY
///
/// This is a matching delete for EATEXT_SA_NEW_ARRAY.
/// See EATEXT_SA_NEW_ARRAY for example code.
///
#define EATEXT_SA_DELETE_ARRAY(pArray)  EA::Text::delete_array(pArray)



namespace EA
{
    namespace Text
    {

        /// create_object
        ///
        /// Creates an object of templated type T via memory obtained from an ICoreAllocator
        /// interface instance. To create an array of objects, use the create_array function.
        /// The pAllocator instance must be a valid non-NULL pointer.
        /// For specifications on how pName is used, see the ICoreAllocator interface documentation.
        ///
        /// It is not possible to have this functionality via an override of operator new,
        /// as operator new/delete only understand custom implementations on a class-by-class 
        /// basis and not on an allocator instance basis. The C++ language provides no way to
        /// call operator delete with an allocator parameter.
        ///
        /// Note that you don't need to use this function if you can instead overload operator 
        /// new for a given class. This is often the case when you are defining the class yourself 
        /// as opposed to working with some arbitrary class that somebody else created and you 
        /// are merely working with.
        ///
        /// Example usage:
        ///    Widget* pWidget = create_object<Widget>(pAllocator);
        ///    delete_object(pWidget, pAllocator);
        ///
        template <typename T>
        inline T* create_object(Allocator::StackAllocator* pAllocator)
        {
            const size_t kAlignOfT = EA_ALIGN_OF(T);
            void* p;

            // We assume that pAllocator allocates on the C/C++ max natural 
            // alignment which is generally 8 (sizeof double).
            // If the allocator allocates on an alignment of only 4, then it 
            // needs to document so and the user should be careful.
            if(kAlignOfT <= 8) // To do: Make this a compile-time template decision.
                p = pAllocator->Malloc(sizeof(T), 0);
            else
                p = pAllocator->MallocAligned(sizeof(T), kAlignOfT, 0);

            // To consider: If exceptions handling is enabled (at least by the 
            // compiler), then put an exception handler around this new() call and
            // if the exception triggers, then call pAllocator->Free and return NULL.
            return new(p) T;
        }


        /// create_array
        ///
        /// This is an array version of create_object.
        /// See create_object for behaviour details.
        ///
        /// Example usage:
        ///    Widget* pWidget = create_array<Widget>(pAllocator, 10);
        ///    delete_array(pWidget, pAllocator);
        ///
        template <typename T>
        inline T* create_array(Allocator::StackAllocator* pAllocator, size_t count)
        {
            uintptr_t*   p;
            const size_t kAlignOfT = EA_ALIGN_OF(T);

            // We put a uintptr_t count in front of the array allocation.
            if(kAlignOfT <= sizeof(uintptr_t)) // Note that this comparison assumes that the Malloc function returns uintptr_t-aligned objects at a minimum. EAStackAllocator has a 'kMinAlignment' enum, but it's a protected member instead of being private. // To consider: Make this a compile-time template decision.
                p = (uintptr_t*)pAllocator->Malloc(sizeof(uintptr_t) + (sizeof(T) * count));
            else
                p = (uintptr_t*)pAllocator->MallocAligned(sizeof(uintptr_t) + (sizeof(T) * count), kAlignOfT, sizeof(uintptr_t));

            if(p)
            {
                *p++ = (uintptr_t)count;

                // Placement new for arrays doesn't work in a portable way in C++, so we implement it ourselves.
                // Note that we don't handle exceptions here. An exception-savvy implementation would undo any
                // object constructions and re-throw.
                for(T* pObject = (T*)(void*)p, *pObjectEnd = pObject + count; pObject != pObjectEnd; ++pObject)
                    new(pObject) T;
            }

            return (T*)(void*)p;
        }


        /// delete_object
        ///
        /// Deletes an object created by create_object.
        /// See create_object for specifications and examples.
        ///
        template <typename T>
        inline void delete_object(T* pObject)
        {
            if(pObject) // As per the C++ standard, deletion of NULL results in a no-op.
                pObject->~T();
        }


        /// delete_array
        ///
        /// Deletes an object created by create_array.
        /// See create_array for specifications and examples.
        ///
        template <typename T>
        void delete_array(T* pArray)
        {
            if(pArray) // As per the C++ standard, deletion of NULL results in a no-op.
            {
                // Destruct objects in reverse order.
                T* pArrayEnd = pArray + ((uintptr_t*)pArray)[-1];
                while(pArrayEnd-- != pArray)
                    pArrayEnd->~T();
            }
        }

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#endif  // Header include guard.













