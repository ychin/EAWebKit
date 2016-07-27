/*
Copyright (C) 2006,2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// EATextCoreAllocatorNew.h
// Created by Paul Pedriana - 2006
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file provides the following macros:
//     EATEXT_CA_NEW(Class, pAllocator, pName) 
//     EATEXT_CA_NEW_ARRAY(Class, pAllocator, pName, nCount)
//     EATEXT_CA_DELETE(pObject, pAllocator)
//     EATEXT_CA_DELETE_ARRAY(pArray, pAllocator)
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

#ifndef EATEXT_INTERNAL_EATEXTCOREALLOCATORNEW_H
#define EATEXT_INTERNAL_EATEXTCOREALLOCATORNEW_H


#include <EAText/EAText.h>
#include <EAText/EATextConfig.h>
#include <coreallocator/icoreallocator_interface.h>
#include <new>


#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4127) // conditional expression is constant
#endif


/// EATEXT_CA_NEW
///
/// Macro to use operator new via ICoreAllocator interface. This macro is not 
/// required, as you can directly use Allocator::create_object / delete_object.
/// This is the single object version of new, to create an array of objects,
/// use EATEXT_CA_NEW_ARRAY instead.
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
///     Widget* pWidget = EATEXT_CA_NEW(Widget, pAllocator, "UI/Widget");
///     EATEXT_CA_DELETE(pWidget, pAllocator);
///
#ifndef EATEXT_CA_NEW
    #define EATEXT_CA_NEW(Class, pAllocator, pName) \
        new ((pAllocator)->Alloc(sizeof(Class), pName, 0, EA_ALIGN_OF(Class), 0)) Class
#endif


/// EATEXT_CA_NEW_ARRAY
///
/// This is an array version of EATEXT_CA_NEW
///
/// Example usage:
///     Widget* pWidget = EATEXT_CA_NEW_ARRAY(Widget, pAllocator, "UI/Widget", 10);
///     EATEXT_CA_DELETE_ARRAY(pWidget, pAllocator);
///
#ifndef EATEXT_CA_NEW_ARRAY
    #define EATEXT_CA_NEW_ARRAY(Class, pAllocator, pName, nCount) EA::Text::create_array<Class>(pAllocator, nCount, pName)
#endif


/// EATEXT_CA_DELETE
///
/// This is a matching delete for EATEXT_CA_NEW.
/// See EATEXT_CA_NEW for example code.
///
#ifndef EATEXT_CA_DELETE
    #define EATEXT_CA_DELETE(pObject, pAllocator) EA::Text::delete_object(pObject, pAllocator)
#endif


/// EATEXT_CA_DELETE_ARRAY
///
/// This is a matching delete for EATEXT_CA_NEW_ARRAY.
/// See EATEXT_CA_NEW_ARRAY for example code.
///
#ifndef EATEXT_CA_DELETE_ARRAY
    #define EATEXT_CA_DELETE_ARRAY(pArray, pAllocator)  EA::Text::delete_array(pArray, pAllocator)
#endif


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
        ///    Widget* pWidget = create_object<Widget>(pAllocator, "UI/Widget");
        ///    delete_object(pWidget, pAllocator);
        ///
        template <typename T>
        inline T* create_object(Allocator::ICoreAllocator* pAllocator, const char* pName)
        {
            const size_t kAlignOfT = EA_ALIGN_OF(T);
            void* p;

            // We assume that pAllocator allocates on the C/C++ max natural 
            // alignment which is generally 8 (sizeof double).
            // If the allocator allocates on an alignment of only 4, then it 
            // needs to document so and the user should be careful.
            if(kAlignOfT <= 8) // To do: Make this a compile-time template decision.
                p = pAllocator->Alloc(sizeof(T), pName, 0);
            else
                p = pAllocator->Alloc(sizeof(T), pName, 0, kAlignOfT, 0);

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
        ///    Widget* pWidget = create_array<Widget>(pAllocator, 10, "UI/Widget");
        ///    delete_array(pWidget, pAllocator);
        ///
        template <typename T>
        inline T* create_array(Allocator::ICoreAllocator* pAllocator, size_t count, const char* pName)
        {
            uint32_t*    p;
            const size_t kAlignOfT = EA_ALIGN_OF(T);

            // We put a uint32_t count in front of the array allocation.
            if(kAlignOfT <= sizeof(uint32_t)) // To do: Make this a compile-time template decision.
                p = (uint32_t*)pAllocator->Alloc(sizeof(uint32_t) + (sizeof(T) * count), pName, 0);
            else
                p = (uint32_t*)pAllocator->Alloc(sizeof(uint32_t) + (sizeof(T) * count), pName, 0, kAlignOfT, sizeof(uint32_t));

            if(p)
            {
                *p++ = (uint32_t)count;

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
        inline void delete_object(T* pObject, Allocator::ICoreAllocator* pAllocator)
        {
            if(pObject) // As per the C++ standard, deletion of NULL results in a no-op.
            {
                pObject->~T();
                pAllocator->Free(pObject);
            }
        }


        /// delete_array
        ///
        /// Deletes an object created by create_array.
        /// See create_array for specifications and examples.
        ///
        template <typename T>
        void delete_array(T* pArray, Allocator::ICoreAllocator* pAllocator)
        {
            if(pArray) // As per the C++ standard, deletion of NULL results in a no-op.
            {
                // Destruct objects in reverse order.
                T* pArrayEnd = pArray + ((uint32_t*)pArray)[-1];
                while(pArrayEnd-- != pArray)
                    pArrayEnd->~T();

                pAllocator->Free(static_cast<void*>((uint32_t*)pArray - 1));
            }
        }

    } // namespace CoreAllocator

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#endif  // Header include guard













