/*
Copyright (C) 2004, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextRefCount.h
//
// Written by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTREFCOUNT_H
#define EATEXT_EATEXTREFCOUNT_H


#ifndef INCLUDED_eabase_H
   #include <EABase/eabase.h>
#endif


namespace EA
{
namespace Text
{

    //////////////////////////////////////////////////////////////////////////////
    /// GetRefCount
    /// 
    /// Example usage:
    ///     const int currentRefCount = GetRefCount(pFont);
    ///     assert(currentRefCount >= 2);
    ///     (void)currentRefCount; // Prevent compiler warning regarding unused variable.
    ///
    /// or simply:
    ///     assert(GetRefCount(pFont) >= 2);
    /// 
    template <typename T>
    int GetRefCount(T* pT)
    {
        const int refCount = pT->AddRef() - 1;
        pT->Release();
        return refCount;
    }

 

    //////////////////////////////////////////////////////////////////////////////
    /// AutoRefCount
    /// 
    /// This is a class that acts like the C++ auto_ptr class except that instead
    /// of deleting its member data when it goes out of scope, it Releases its
    /// member data when it goes out of scope. This class thus requires that the 
    /// templated data type have an AddRef and Release function. Normally this 
    /// means the templated type is an IUnknown32 or IRefCount.
    /// This class is useful for automatically releasing an object when this 
    /// class goes out of scope. See below for some usage.
    /// You should be careful about putting instances of this class as members of 
    /// another class. If you do so, then the AutoRefCount destructor will only
    /// be called if the object that owns it is destructed. This creates a potential
    /// chicken-and-egg situation. What if the AutoRefCount member contains a 
    /// pointer to an object that has a reference on the object that owns the 
    /// AutoRefCount member? The answer is that the neither object can ever be
    /// destructed. The solution is to:
    ///    1) Be very careful about what objects you put into member AutoRefCount objects.
    ///    2) Clear out your AutoRefCount members in your shutdown function.
    ///    3) Simply don't use AutoRefCount objects as class members.
    ///
    /// Example usage:
    ///    AutoRefCount<IWidget> pWidget = new Widget;
    ///    pWidget = new Widget;
    ///    pWidget->Reset();
    ///
    template <typename T>
    class AutoRefCount
    {
    public:
        /// element_type
        ///
        /// This typedef is present for consistency with the C++ standard library 
        /// auto_ptr template. It allows users to refer to the templated type via
        /// a typedef. This is sometimes useful to be able to do.
        /// 
        /// Example usage:
        ///    AutoRefCount<IWidget> autoRC;
        ///    void DoSomething(autoRC::element_type someType);
        ///
        typedef T element_type;

        /// mpObject
        ///
        /// This field is made public to allow direct tricks that reduce or
        /// eliminate extraneous AddRef/Release calls. To consider: the current 
        /// suite of functionality may be sufficient to obviate the need to make
        /// this member public, and it might possibly be able to be protected.
        ///
        T* mpObject;

    public:
        /// AutoRefCount
        /// Default constructor. The member object is NULL.
        ///
        AutoRefCount()
            : mpObject(NULL) {}


        /// AutoRefCount
        ///
        /// Provides a constructor which takes ownership of a pointer.
        /// The incoming pointer is AddRefd.
        ///
        /// Example usage:
        ///    AutoRefCount<Widget> pWidget(new Widget);
        ///
        AutoRefCount(T* pObject)
            : mpObject(pObject) 
        { 
            if(mpObject)
                mpObject->AddRef();
        } 


        /// AutoRefCount
        ///
        /// Provides a constructor which copies a pointer from another AutoRefCount.
        /// The incoming pointer is AddRefd. The source AutoRefCount object maintains
        /// its AddRef on the pointer.
        ///
        /// Example usage:
        ///    AutoRefCount<Widget> pWidget1;
        ///    AutoRefCount<Widget> pWidget2(pWidget1);
        ///
        AutoRefCount(const AutoRefCount& rhs) 
            : mpObject(rhs.mpObject) 
        { 
            if(mpObject)
                mpObject->AddRef();
        }


        /// AutoRefCount
        ///
        /// Provides a constructor which doesn't AddRef the incoming pointer.
        ///
        /// Example usage:
        ///    Widget* pWidget = new Widget;
        ///    pWidget->AddRef();
        ///    AutoRefCount<Widget> autoRC(pWidget, AutoRefCount::kNoAddRef);
        ///
        enum NoAddRef{ kNoAddRef };

        AutoRefCount(T* pObject, NoAddRef)
            : mpObject(pObject) {}


        /// ~AutoRefCount
        ///
        /// Releases the owned pointer.
        ///
        ~AutoRefCount() 
        {
            if(mpObject)
                mpObject->Release();
        }


        /// operator =
        ///
        /// Assigns an AutoRefCount object to this AutoRefCount object.
        /// The incoming pointer is AddRefd. The source AutoRefCount object 
        /// maintains its AddRef on the pointer. If there is an existing member
        /// pointer, it is Released before the incoming pointer is assigned.
        /// If the incoming pointer is equal to the existing pointer, no    
        /// action is taken. The incoming pointer is AddRefd before any 
        /// member pointer is Released.
        ///
        AutoRefCount& operator=(const AutoRefCount& source)       
        {         
            return operator=(source.mpObject);
        }


        /// operator=
        ///
        /// Assigns an AutoRefCount object to this AutoRefCount object.
        /// The incoming pointer is AddRefd. If there is an existing member
        /// pointer, it is Released before the incoming pointer is assigned.
        /// If the incoming pointer is equal to the existing pointer, no    
        /// action is taken. The incoming pointer is AddRefd before any 
        /// member pointer is Released.
        ///
        AutoRefCount& operator=(T* pObject)
        {         
            if(pObject != mpObject)
            {
                T* const pTemp = mpObject; // Create temporary to prevent possible problems with re-entrancy.
                if(pObject)
                    pObject->AddRef();
                mpObject = pObject;
                if(pTemp)
                    pTemp->Release();
            }
            return *this;
        }


        /// operator *
        ///
        /// This dereferences the AutoRefCount object as if it were a pointer.
        /// This allows the AutoRefCount object to transparently act like the type it contains.
        ///
        T& operator *()  const 
            { return *mpObject; }


        /// operator ->
        ///
        /// This references the AutoRefCount object as if it were a pointer.
        /// This allows the AutoRefCount object to transparently act like the type it contains.
        ///
        T* operator ->() const
            { return  mpObject; }


        /// operator T*
        ///
        /// Casts the AutoRefCount object to the contained type pointer.
        /// This allows the AutoRefCount object to transparently act like the type it contains.
        ///
        operator T*() const 
            { return  mpObject; }


        /// Reset
        ///
        /// Reset has equivalent behaviour operator=(NULL).
        /// It exists as a separate function because it is slightly more efficient
        /// in implementation than operator=.
        /// 
        void Reset() 
        {
            T* const pTemp = mpObject;
            mpObject = NULL;
            if(pTemp)
                pTemp->Release();
        }


        /// Attach
        ///
        /// Attach sets an AutoRefCount pointer without calling AddRef on
        /// the pointed object. The AutoRefCount thus eventually only does a
        /// Release on the object. This is useful for assuming a reference
        /// that someone else has handed you and making sure it is always
        /// released, even if you return in the middle of a function or an
        /// exception is thrown.
        ///
        void Attach(T* pObject)
        {
            T* const pTemp = mpObject;
            mpObject = pObject;
            if(pTemp)
                pTemp->Release();
        }


        /// Detach
        ///
        /// Detach surrenders the reference held by an AutoRefCount pointer -- 
        /// it returns the current reference and nulls the pointer. If the returned
        /// pointer is non-null it must be released. This is useful in functions
        /// that must return a reference while possibly being aborted by a return
        /// or thrown exception:
        ///
        /// Example usage:
        ///     bool GetFoo(T** pp) {
        ///         AutoRefCount<T> p(PrivateGetFoo());
        ///         if(p->Method())
        ///             return false;
        ///         *pp = p.Detach();
        ///         return true;
        ///     }
        ///
        T* Detach()
        {
            T* const pTemp = mpObject;
            mpObject = NULL;
            return pTemp;
        }


        /// AsPPVoidParam
        ///
        /// This is a utility function which enables the usage of AutoRefCount with
        /// functions that return pointers in void** arguments. It is convenient because
        /// it internally does the release of the old pointer and does proper casting 
        /// of the new incoming pointer. 
        ///
        /// If there is an existing member pointer, it is Released and cleared.
        ///
        /// Note that with the new EACOM that this function is not as important as 
        /// with previous COM systems because the new EACOM doesn't have an AddRefing
        /// QueryInterface function and there are no standardized functions that 
        /// take void** as did QueryInterface. EACOM's AsInterface doesn't AddRef.
        ///
        /// Example usage:
        ///    extern void ReturnAnAddRefdInterface(int id, void** ppObject);
        ///    AutoRefCount<IWidget> autoWidget;
        ///    ReturnAnAddRefdInterface(kIWidget, autoWidget.AsPPVoidParam());
        ///
        void** AsPPVoidParam()                  // This function is meant to be used where one would
        {                                       // use the construct '(void**)&pInterface', as in:
            if(mpObject){                       //    AutoRefCount<cIA> pIA;
                T* const pTemp = mpObject;      //    pSomeObject->QueryInterface(IID, pIA.AsPPVoidParam());
                mpObject = NULL;                //    pSomeObject->GetClassObject(CLSID, IID, pIA.AsPPVoidParam());
                pTemp->Release();               //
            }                                   // Without this function one would have to write something like:
            return (void**)&mpObject;           //    pIA = AutoRefCount<cIA>(); // or pIA = (cIA*)NULL;
        }                                       //    pSomeObject->QueryInterface(IID, (void**)&pIA.mpData);
                                                // in order to make sure that QI doesn't produce interface leak.

        /// AsPPTypeParam
        /// 
        /// This function is similar to AsPPVoidParam but returns a pointer of the 
        /// same type as the held object instead of void**. See the comments for 
        /// the AsPPVoidParam function.
        ///
        /// Example usage:
        ///    extern void ReturnAnAddRefdInterface(int id, IWidget** ppObject);
        ///    AutoRefCount<IWidget> autoWidget;
        ///    ReturnAnAddRefdInterface(kIWidget, autoWidget.AsPPTypeParam());
        ///
        T** AsPPTypeParam()                     
            { return (T**)AsPPVoidParam(); } 

    }; // class AutoRefCount


} // namespace Text

} // namespace EA


#endif // Header include guard
