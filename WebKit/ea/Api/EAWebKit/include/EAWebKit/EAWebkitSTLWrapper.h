/*
Copyright (C) 2009, 2010, 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAWebkitSTLWrapper.h
// By Arpit Baldeva
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITSTLWRAPPER_H
#define EAWEBKIT_EAWEBKITSTLWRAPPER_H


///////////////////////////////////////////////////////////////////////
// The purpose of this file is to stop the template expansion inside the main app exe(and thus exe contain code due to the template instantiation) for the users of the EAWebkit.
// This is required for LGPL compliance.
// We keep all the STL related classes in this single file to start with. It can be broken down if necessary in future.
///////////////////////////////////////////////////////////////////////

#include <EABase/config/eacompilertraits.h>
#include <EABase/eabase.h>

// Compatibility with older versions of EABase
#ifndef EA_ALIGN
#define EA_ALIGN EA_PREFIX_ALIGN
#endif

// Documentation : Normally, it is usually alongside the methods but this header file contains a little too much implementation details.
// To avoid clutter, we document the classes separately here.

// 1. EASTLFixedString8Wrapper
// This wraps eastl::fixed_string<char8_t,  128, true, EASTLAllocator>.
// You can not create/copy an instance of this class. The cross-platform user interface of this class is limited to SetCharacters() 
// and GetCharacters() methods.

// 2. EASTLFixedString16Wrapper
// This wraps eastl::fixed_string<char16_t,  128, true, EASTLAllocator>.
// You can not create/copy an instance of this class. The cross-platform user interface of this class is limited to SetCharacters() 
// and GetCharacters() methods.

// 3. EASTLHeaderMapWrapper
// This wraps typedef eastl::fixed_multimap<FixedString16_64, FixedString16_64, 8, true, fstr_iless, EASTLAllocator> HeaderMap;
// You can not create/copy an instance of this class. The cross-platform interface of this class is limited to
// 
// const char16_t* GetValue(const char16_t* pKey)				- If key found, returns the char16_t* for of the value. Otherwise, returns NULL.
// void SetValue(const char16_t* pKey, const char16_t* pValue)	- Overwrites the entry if present, else creates it.
// void EraseValue(const char16_t* pKey)						- If key found, erase the element.
//
//
// In addition, this class exposes an iterator to allow you to iterate over the map. This can be done as follows.
// 			EA::WebKit::EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator* iter = pTInfo->mHeaderMapOut.First();
// 			while(iter)
// 			{
// 				const char16_t* key = iter->GetKey();
// 				const char16_t* value = iter->GetValue();
// 				(void) key;
// 				(void) value;
// 				iter = pTInfo->mHeaderMapOut.GetNext();
// 			}

/************************************************************************************************
*																								*
*																								*
If you are an integrator of EAWebKit, you don't need to read this header further. The			*
documentation above should be sufficient.														*
*																								*
*																								*
*************************************************************************************************/

namespace EA
{
namespace WebKit
{
// On certain platforms, the actual STL object is newed in the wrapper classes instead of placement new in an aligned buffer. This is
// because we may have different compilers on same platform. It is possible that using a different compiler/different compiler pre-processors on application
// side can cause the alignment to go wonky. Though this claim needs to be verified, we are not in a position to do the most complete analysis here so we
// take the safer route.

// Following classes have some specific #ifdef for STRINGWRAPPER_AVOID_HEAP_ALLOCATION. While it makes code hard to read, this class is a pretty low maintenance class
// and is not expected to cause a significant burden.

// Having a STRINGWRAPPER_AVOID_HEAP_ALLOCATION define instead of platform defines makes it easy to enable/disable code on
// new platforms or test existing platform.
#define STRINGWRAPPER_AVOID_HEAP_ALLOCATION 1
#define USE_INDIRECT_VIRTUALCALL 0

class EASTLFixedString8Wrapper
{
public:
#if (EA_PLATFORM_PTR_SIZE == 8)
	static const int kFixedStringSize = 168;
#elif (EA_PLATFORM_PTR_SIZE == 4)
	static const int kFixedStringSize = 148;
#endif
    EASTLFixedString8Wrapper();
	EASTLFixedString8Wrapper(const char8_t* str);
	EASTLFixedString8Wrapper(const EASTLFixedString8Wrapper& rhs);
	EASTLFixedString8Wrapper& operator = (const EASTLFixedString8Wrapper& rhs);
	virtual ~EASTLFixedString8Wrapper();
	
#if USE_INDIRECT_VIRTUALCALL
	const char8_t*	GetCharacters()
	{
		return GetCharactersInternal();
	}
	void SetCharacters(const char8_t* chars)
	{
		SetCharactersInternal(chars);
	}
#else
	virtual const char8_t*	GetCharacters();
	virtual void			SetCharacters(const char8_t* chars);
#endif
	void* GetImpl() const;
private:
#if USE_INDIRECT_VIRTUALCALL
	virtual const char8_t*	GetCharactersInternal();
	virtual void			SetCharactersInternal(const char8_t* chars);
#endif

#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	EA_ALIGN(EA_PLATFORM_PTR_SIZE) char8_t mString8[kFixedStringSize];
#else
	void* mString8;
#endif

};

class EASTLFixedString16Wrapper
{
public:
#if (EA_PLATFORM_PTR_SIZE == 8)
	static const int kFixedStringSize = 296;
#elif (EA_PLATFORM_PTR_SIZE == 4)
	static const int kFixedStringSize = 276;
#endif

	EASTLFixedString16Wrapper();
	EASTLFixedString16Wrapper(const char16_t* str);
	EASTLFixedString16Wrapper(const EASTLFixedString16Wrapper& rhs);
	EASTLFixedString16Wrapper& operator = (const EASTLFixedString16Wrapper& rhs);
	virtual ~EASTLFixedString16Wrapper();
	
#if USE_INDIRECT_VIRTUALCALL
	const char16_t*	GetCharacters()
	{
		return GetCharactersInternal();
	}
	void SetCharacters(const char16_t* chars)
	{
		SetCharactersInternal(chars);
	}
#else
	virtual const char16_t*	GetCharacters();
	virtual void			SetCharacters(const char16_t* chars);
#endif

	void* GetImpl() const;
private:
#if USE_INDIRECT_VIRTUALCALL
	virtual const char16_t*	GetCharactersInternal();
	virtual void			SetCharactersInternal(const char16_t* chars);
#endif
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	EA_ALIGN(EA_PLATFORM_PTR_SIZE) char8_t mString16[kFixedStringSize];
#else
	void* mString16;
#endif
};

class EASTLHeaderMapWrapper
{
public:
	EASTLHeaderMapWrapper();
	EASTLHeaderMapWrapper(const EASTLHeaderMapWrapper& rhs);
	EASTLHeaderMapWrapper& operator = (const EASTLHeaderMapWrapper& rhs);
	virtual ~EASTLHeaderMapWrapper(); // Only reason to make it virtual is to stop the compiler from complaining. This class is not intended to be a base class.
	
#if USE_INDIRECT_VIRTUALCALL
	const char16_t* GetValue(const char16_t* pKey) const
	{
		return GetValueInternal(pKey);
	}
	void SetValue(const char16_t* pKey, const char16_t* pValue)
	{
		SetValueInternal(pKey,pValue);
	}
	void EraseValue(const char16_t* pKey)
	{
		EraseValueInternal(pKey);
	}
#else
	virtual const char16_t* GetValue(const char16_t* pKey) const;
	virtual void SetValue(const char16_t* pKey, const char16_t* pValue);
	virtual void EraseValue(const char16_t* pKey);
#endif
	void* GetImpl() const;

	//
	// Iterator support
	//
	class EASTLHeaderMapWrapperIterator
	{
		friend class EASTLHeaderMapWrapper;
	public:
		virtual const char16_t* GetKey() const;
		virtual const char16_t* GetValue() const;

	private:
		EASTLHeaderMapWrapperIterator();
		EASTLHeaderMapWrapperIterator(const EASTLHeaderMapWrapperIterator& rhs); //Simply declared, No copy supported
		EASTLHeaderMapWrapperIterator& operator =(const EASTLHeaderMapWrapperIterator& rhs);//Simply declared, No assignment supported

		virtual ~EASTLHeaderMapWrapperIterator();
		void* mIterator;
	};

	virtual EASTLHeaderMapWrapperIterator* First();
	virtual EASTLHeaderMapWrapperIterator* GetNext();
private:
	void* mHeaderMap;
	EASTLHeaderMapWrapperIterator* mHeaderMapWrapperIterator;
#if USE_INDIRECT_VIRTUALCALL
	virtual const char16_t* GetValueInternal(const char16_t* pKey) const;
	virtual void SetValueInternal(const char16_t* pKey, const char16_t* pValue);
	virtual void EraseValueInternal(const char16_t* pKey);
#endif
};
}
}


#endif // Header include guard
