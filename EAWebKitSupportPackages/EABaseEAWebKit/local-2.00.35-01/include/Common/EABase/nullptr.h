/*
Copyright (C) 2010-2013 Electronic Arts, Inc.  All rights reserved.

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


#include <EABase/eabase.h>
#include <EABase/eahave.h>


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
    #pragma once /* Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result. */
#endif


#if defined(EA_NO_HAVE_nullptr_IMPL) // If the compiler lacks a native version... 

    namespace std
    {
        class nullptr_t
        {
        public:
            template<class T>               // When tested a pointer, acts as 0.
            operator T*() const
                { return 0; }
         
            template<class C, class T>      // When tested as a member pointer, acts as 0.
            operator T C::*() const
                { return 0; }

            typedef void* (nullptr_t::*bool_)() const;
            operator bool_() const          // An rvalue of type std::nullptr_t can be converted to an rvalue of type bool; the resulting value is false.
                { return false; }           // We can't use operator bool(){ return false; } because bool is convertable to int which breaks other required functionality.

            // We can't enable this without generating warnings about nullptr being uninitialized after being used when created without "= {}".
            //void* mSizeofVoidPtr;         // sizeof(nullptr_t) == sizeof(void*). Needs to be public if nullptr_t is to be a POD.

        private:
            void operator&() const;         // Address cannot be taken.
        };

        inline nullptr_t nullptr_get()
        {
            nullptr_t nullptr = { }; // std::nullptr exists.
            return nullptr;
        }

        #define nullptr nullptr_get()

    } // namespace std


    template<class T>
    inline bool operator==(T* p, const std::nullptr_t)
    { return p == 0; }

    template<class T>
    inline bool operator==(const std::nullptr_t, T* p)
    { return p == 0; }

    template<class T, class U>
    inline bool operator==(T U::* p, const std::nullptr_t)
    { return p == 0; }

    template<class T, class U>
    inline bool operator==(const std::nullptr_t, T U::* p)
    { return p == 0; }

    inline bool operator==(const std::nullptr_t, const std::nullptr_t)
    { return true; }

    inline bool operator!=(const std::nullptr_t, const std::nullptr_t)
    { return false; }

    inline bool operator<(const std::nullptr_t, const std::nullptr_t)
    { return false; }

    inline bool operator>(const std::nullptr_t, const std::nullptr_t)
    { return false; }

    inline bool operator<=(const std::nullptr_t, const std::nullptr_t)
    { return true; }

    inline bool operator>=(const std::nullptr_t, const std::nullptr_t)
    { return true; }


    using std::nullptr_t;   // exported to global namespace.
    using std::nullptr_get; // exported to global namespace.

#endif // EA_NO_HAVE_nullptr_IMPL

