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
// CoreAllocatorStack.h
// Created by Paul Pedriana - 2006
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements the ICoreAllocator interface with PPMalloc StackAllocator.
// Also implements instances of StackAllocator core alloc and free functions.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_COREALLOCATORSTACK_H
#define EATEXT_INTERNAL_COREALLOCATORSTACK_H


#include <coreallocator/icoreallocator_interface.h>
#include <PPMalloc/EAStackAllocator.h>
#include <EAText/EATextConfig.h>


namespace EA
{
    namespace Allocator
    {
        class StackAllocator;
    }


    namespace Text
    {
        /// CoreAllocatorStack
        ///
        /// Allocates memory linearly from a memory stack: PPMalloc StackAllocator
        ///
        /// Example usage:
        ///    CoreAllocatorStack ca(pStackAllocator);
        ///
        ///    Widget* pWidget = EA_CA_NEW(Widget, &ca, "UI/Widget");
        ///    EA_CA_DELETE(pWidget, &ca);
        ///
        class CoreAllocatorStack : public EA::Allocator::ICoreAllocator
        {
        public:
            CoreAllocatorStack(EA::Allocator::StackAllocator* pSA = NULL);
            CoreAllocatorStack(const CoreAllocatorStack& x);

            CoreAllocatorStack& operator=(const CoreAllocatorStack& x);

            EA::Allocator::StackAllocator* GetAllocator() const;
            void                           SetAllocator(EA::Allocator::StackAllocator* pSA);

            virtual void* Alloc(size_t size, const char* name, unsigned int flags);
            virtual void* Alloc(size_t size, const char* name, unsigned int flags, unsigned int align, unsigned int alignOffset = 0);
            virtual void  Free(void* block, size_t size = 0);

        protected:
            friend bool operator==(const CoreAllocatorStack& a, const CoreAllocatorStack& b);
            friend bool operator!=(const CoreAllocatorStack& a, const CoreAllocatorStack& b);

            EA::Allocator::StackAllocator* mpStackAllocator;
        };

        bool operator==(const CoreAllocatorStack& a, const CoreAllocatorStack& b);
        bool operator!=(const CoreAllocatorStack& a, const CoreAllocatorStack& b);




        /// StackAllocatorCoreAllocate
        ///
        /// Provides a StackAllocator CoreAllocationFunction that uses an ICoreAllocator.
        /// Thus if you have a library that uses a StackAllocator but you want the user 
        /// of the library to supply the core memory behind the StackAllocator, you could 
        /// use these functions in your library and the user can supply an ICoreAllocator
        /// instance which supplies the core memory.
        ///
        /// Example usage:
        ///     StackAllocator sa;
        ///     sa.Init(NULL, 2048, StackAllocatorCoreAllocate, StackAllocatorCoreFree, pSomeCoreAllocator);
        ///     sa.Malloc(37);
        ///
        void* StackAllocatorCoreAllocate(size_t nSize, size_t* pSizeResult, void* pContext);

        /// StackAllocatorCoreFree
        ///
        /// Provides a StackAllocator CoreFreeFunction that uses an ICoreAllocator.
        /// See StackAllocatorCoreAllocate for comments and example usage.
        ///
        void StackAllocatorCoreFree(void* pData, void* pContext);


    } // namespace Text

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


namespace EA
{
    namespace Text
    {

        inline CoreAllocatorStack::CoreAllocatorStack(EA::Allocator::StackAllocator* pGA)
            : mpStackAllocator(pGA) { }

        inline CoreAllocatorStack::CoreAllocatorStack(const CoreAllocatorStack& x)
            : EA::Allocator::ICoreAllocator(x), 
              mpStackAllocator(x.mpStackAllocator) { }

        inline CoreAllocatorStack& CoreAllocatorStack::operator=(const CoreAllocatorStack& x)
        {
            mpStackAllocator = x.mpStackAllocator;
            return *this;
        }

        inline EA::Allocator::StackAllocator* CoreAllocatorStack::GetAllocator() const
        {
            return mpStackAllocator;
        }

        inline void CoreAllocatorStack::SetAllocator(EA::Allocator::StackAllocator* pGA)
        {
            mpStackAllocator = pGA;
        }

        inline bool operator==(const CoreAllocatorStack& a, const CoreAllocatorStack& b)
        {
            return a.mpStackAllocator == b.mpStackAllocator;
        }

        inline bool operator!=(const CoreAllocatorStack& a, const CoreAllocatorStack& b)
        {
            return a.mpStackAllocator != b.mpStackAllocator;
        }




        inline void* StackAllocatorCoreAllocate(size_t nSize, size_t* pSizeResult, void* pContext)
        {
            EA::Allocator::ICoreAllocator* const pICoreAllocator = (EA::Allocator::ICoreAllocator*)pContext;
            *pSizeResult = nSize;
            return pICoreAllocator->Alloc(nSize, EATEXT_ALLOC_PREFIX "Stack", 0);  // This anonymous naming is not ideal, it would be better if the pContext held more information we could use.
        }

        inline void StackAllocatorCoreFree(void* pData, void* pContext)
        {
            EA::Allocator::ICoreAllocator* const pICoreAllocator = (EA::Allocator::ICoreAllocator*)pContext;
            pICoreAllocator->Free(pData);
        }




        inline void* CoreAllocatorStack::Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/)
        {
            return mpStackAllocator->Malloc(size, true);
        }


        inline void* CoreAllocatorStack::Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/, 
                                            unsigned int align, unsigned int alignOffset)
        {
            return mpStackAllocator->MallocAligned(size, (size_t)align, (size_t)alignOffset, true);
        }


        inline void CoreAllocatorStack::Free(void* pData, size_t /*size*/)
        {
            return mpStackAllocator->Free(pData);
        }


    } // namespace Text

} // namespace EA



#endif // Header include guard








