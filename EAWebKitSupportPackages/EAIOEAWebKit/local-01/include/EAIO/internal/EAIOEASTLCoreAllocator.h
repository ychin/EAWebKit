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
// EAIOEASTLCoreAllocator.h
// 
// Copyright (c) 2006, Electronic Arts. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements an EASTL allocator that uses an arbitrary ICoreAllocator.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EASTLCOREALLOCATOR_H
#define EAIO_EASTLCOREALLOCATOR_H


#include <EAIO/internal/Config.h>
#ifndef EA_ALLOCATOR_ICOREALLOCATOR_INTERFACE_H
    #include <coreallocator/icoreallocator_interface.h>
#endif
#ifndef EASTL_ALLOCATOR_H
    #include <EASTL/allocator.h>
#endif


namespace EA
{
    namespace IO
    {
        // Forward declarations
        EAIO_API Allocator::ICoreAllocator* GetAllocator();
    }


    namespace Allocator
    {

        /// EAIOEASTLCoreAllocator
        ///
        /// Implements the EASTL allocator interface.
        /// Allocates memory from an instance of ICoreAllocator.
        ///
        /// Example usage:
        ///     eastl::list<Widget, EAIOEASTLCoreAllocator> widgetList("UI/WidgetList", pSomeCoreAllocator);
        ///     widgetList.push_back(Widget());
        ///
        /// Example usage:
        ///    // Note that the CoreAllocator is declared before and thus destroyed after the widget list.
        ///    typedef eastl::list<Widget, EAIOEASTLCoreAllocator> WidgetList;
        ///    CoreAllocatorFixed<WidgetList::node_type> widgetCoreAllocator(pFixedAllocatorForWidgetListValueType);
        ///    WidgetList widgetList(EAIOEASTLCoreAllocator("UI/WidgetList", &widgetCoreAllocator));
        ///
        class EAIO_API EAIOEASTLCoreAllocator
        {
        public:
            EAIOEASTLCoreAllocator(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME));
            EAIOEASTLCoreAllocator(const char* pName, ICoreAllocator* pAllocator);
            EAIOEASTLCoreAllocator(const EAIOEASTLCoreAllocator& x);
            EAIOEASTLCoreAllocator(const EAIOEASTLCoreAllocator& x, const char* pName);

            EAIOEASTLCoreAllocator& operator=(const EAIOEASTLCoreAllocator& x);

            void* allocate(size_t n, int flags = 0);
            void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
            void  deallocate(void* p, size_t n);

            ICoreAllocator* get_allocator() const;
            void            set_allocator(ICoreAllocator* pAllocator);

            int  get_flags() const;
            void set_flags(int flags);

            const char* get_name() const;
            void        set_name(const char* pName);

        protected:
            friend bool operator==(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b);
            friend bool operator!=(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b);

            ICoreAllocator* mpCoreAllocator;
            int             mnFlags;    // Allocation flags. See ICoreAllocator/AllocFlags.

            #if EASTL_NAME_ENABLED
                const char* mpName; // Debug name, used to track memory.
            #endif
        };

        bool operator==(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b);
        bool operator!=(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b);



        /// EAIOPathStringCoreAllocator
        ///
        /// Specific for EAIO/PathString
        ///
        class EAIO_API EAIOPathStringCoreAllocator : public EAIOEASTLCoreAllocator
        {
        public:
            EAIOPathStringCoreAllocator(const char* pName = EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "PathString"))
              : EAIOEASTLCoreAllocator(pName) { }

            EAIOPathStringCoreAllocator(const char* pName, ICoreAllocator* pAllocator)
              : EAIOEASTLCoreAllocator(pName, pAllocator) { }

            EAIOPathStringCoreAllocator(const EAIOEASTLCoreAllocator& x)
              : EAIOEASTLCoreAllocator(x) { }

            EAIOPathStringCoreAllocator(const EAIOEASTLCoreAllocator& x, const char* pName)
              : EAIOEASTLCoreAllocator(x, pName) { }

            EAIOPathStringCoreAllocator& operator=(const EAIOEASTLCoreAllocator& x)
                { EAIOEASTLCoreAllocator::operator=(x); return *this; }
        };

    } // namespace Allocator

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace Allocator
    {

        // The default constructor gets the default EAIO allocator.
        inline EAIOEASTLCoreAllocator::EAIOEASTLCoreAllocator(const char* EASTL_NAME(pName))
          : mpCoreAllocator(EA::IO::GetAllocator()), mnFlags(MEM_TEMP)
        {
            #if EASTL_NAME_ENABLED
                mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
            #endif
        }

        inline EAIOEASTLCoreAllocator::EAIOEASTLCoreAllocator(const char* EASTL_NAME(pName), ICoreAllocator* pCoreAllocator)
          : mpCoreAllocator(pCoreAllocator), mnFlags(MEM_TEMP)
        {
            #if EASTL_NAME_ENABLED
                mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
            #endif
        }

        inline EAIOEASTLCoreAllocator::EAIOEASTLCoreAllocator(const EAIOEASTLCoreAllocator& x)
          : mpCoreAllocator(x.mpCoreAllocator), mnFlags(x.mnFlags)
        {
            #if EASTL_NAME_ENABLED
                mpName = x.mpName;
            #endif
        }

        inline EAIOEASTLCoreAllocator::EAIOEASTLCoreAllocator(const EAIOEASTLCoreAllocator& x, const char* EASTL_NAME(pName))
          : mpCoreAllocator(x.mpCoreAllocator), mnFlags(x.mnFlags)
        {
            #if EASTL_NAME_ENABLED
                mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
            #endif
        }

        inline EAIOEASTLCoreAllocator& EAIOEASTLCoreAllocator::operator=(const EAIOEASTLCoreAllocator& x)
        {
            // In order to be consistent with EASTL's allocator implementation, 
            // we don't copy the name from the source object.
            mpCoreAllocator = x.mpCoreAllocator;
            mnFlags         = x.mnFlags;
            return *this;
        }

        inline void* EAIOEASTLCoreAllocator::allocate(size_t n, int /*flags*/)
        {
            // It turns out that EASTL itself doesn't use the flags parameter, 
            // whereas the user here might well want to specify a flags 
            // parameter. So we use ours instead of the one passed in.
            return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags);
        }

        inline void* EAIOEASTLCoreAllocator::allocate(size_t n, size_t alignment, size_t offset, int /*flags*/)
        {
            // It turns out that EASTL itself doesn't use the flags parameter, 
            // whereas the user here might well want to specify a flags 
            // parameter. So we use ours instead of the one passed in.
            return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags, (unsigned)alignment, (unsigned)offset);
        }

        inline void EAIOEASTLCoreAllocator::deallocate(void* p, size_t n)
        {
            return mpCoreAllocator->Free(p, n);
        }

        inline ICoreAllocator* EAIOEASTLCoreAllocator::get_allocator() const
        {
            return mpCoreAllocator;
        }

        inline void EAIOEASTLCoreAllocator::set_allocator(ICoreAllocator* pAllocator)
        {
            mpCoreAllocator = pAllocator;
        }

        inline int EAIOEASTLCoreAllocator::get_flags() const
        {
            return mnFlags;
        }

        inline void EAIOEASTLCoreAllocator::set_flags(int flags)
        {
            mnFlags = flags;
        }

        inline const char* EAIOEASTLCoreAllocator::get_name() const
        {
            #if EASTL_NAME_ENABLED
                return mpName;
            #else
                return EASTL_ALLOCATOR_DEFAULT_NAME;
            #endif
        }

        inline void EAIOEASTLCoreAllocator::set_name(const char* pName)
        {
            #if EASTL_NAME_ENABLED
                mpName = pName;
            #else
                (void)pName;
            #endif
        }



        inline bool operator==(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b)
        {
            return (a.mpCoreAllocator == b.mpCoreAllocator) &&
                   (a.mnFlags         == b.mnFlags);
        }

        inline bool operator!=(const EAIOEASTLCoreAllocator& a, const EAIOEASTLCoreAllocator& b)
        {
            return (a.mpCoreAllocator != b.mpCoreAllocator) ||
                   (a.mnFlags         != b.mnFlags);
        }


    } // namespace Allocator

} // namespace EA



#endif // Header include guard








