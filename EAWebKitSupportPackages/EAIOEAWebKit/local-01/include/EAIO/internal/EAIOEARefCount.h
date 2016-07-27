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
// EARefCount.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
//
// Implements basic reference counting functionality.
//
// This file was copied from Foundation/EARefCount.h
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAREFCOUNT_H
#define EAIO_EAREFCOUNT_H



/// namespace EA
/// The standard Electronic Arts namespace
namespace EA
{
    template <typename T>
    class EAIOAutoRefCount
    {
    public:
        typedef T element_type;

        T* mpObject;

    public:
        EAIOAutoRefCount()
            : mpObject(NULL) {}

        EAIOAutoRefCount(T* pObject)
            : mpObject(pObject) 
        { 
            if(mpObject)
                mpObject->AddRef();
        } 

        EAIOAutoRefCount(const EAIOAutoRefCount& rhs) 
            : mpObject(rhs.mpObject) 
        { 
            if(mpObject)
                mpObject->AddRef();
        }

        ~EAIOAutoRefCount() 
        {
            if(mpObject)
                mpObject->Release();
        }

        EAIOAutoRefCount& operator=(const EAIOAutoRefCount& source)       
        {         
            return operator=(source.mpObject);
        }

        EAIOAutoRefCount& operator=(T* pObject)
        {         
            if(pObject != mpObject)
            {
                T* const pTemp = mpObject;
                if(pObject)
                    pObject->AddRef();
                mpObject = pObject;
                if(pTemp)
                    pTemp->Release();
            }
            return *this;
        }

        T& operator *()  const 
        {
            return *mpObject;
        }

        T* operator ->() const
        {
            return  mpObject;
        }

        operator T*() const 
        {
            return  mpObject;
        }

    }; // class EAIOAutoRefCount


} // namespace EA


#endif  // Header include guard
