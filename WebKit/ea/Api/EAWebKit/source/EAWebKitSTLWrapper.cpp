/*
Copyright (C) 2009, 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitSTLWrapper.cpp
// By Arpit Baldeva
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <EAWebKit/EAWebkitSTLWrapper.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <internal/include/EAWebKitNewDelete.h>
#include <internal/include/EAWebKitAssert.h>
#include <EAAssert/eaassert.h>


namespace EA 
{
namespace WebKit
{
/******************************************EASTLFixedString8Wrapper******************************/
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
EA_COMPILETIME_ASSERT(EASTLFixedString8Wrapper::kFixedStringSize == sizeof(FixedString8_128));
EA_COMPILETIME_ASSERT(EA_PLATFORM_PTR_SIZE == EA_ALIGN_OF(FixedString8_128));
EA_COMPILETIME_ASSERT(EA_PLATFORM_PTR_SIZE == EA_ALIGN_OF(EASTLFixedString8Wrapper));
#endif
EASTLFixedString8Wrapper::EASTLFixedString8Wrapper()
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString8) FixedString8_128();
#else
	mString8 = new FixedString8_128();
#endif
}

EASTLFixedString8Wrapper::EASTLFixedString8Wrapper(const char8_t* str)
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString8) FixedString8_128(str);
#else
	mString8 = new FixedString8_128(str);
#endif
}

EASTLFixedString8Wrapper::EASTLFixedString8Wrapper(const EASTLFixedString8Wrapper& rhs)
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString8) FixedString8_128();
#else
	mString8 = new FixedString8_128();
#endif
	*(reinterpret_cast<FixedString8_128*>(mString8)) = *(reinterpret_cast<FixedString8_128*> ((rhs).GetImpl()));

}

EASTLFixedString8Wrapper& EASTLFixedString8Wrapper::operator =(const EASTLFixedString8Wrapper& rhs)
{
	if(this == &rhs)
		return *this;

#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	reinterpret_cast<FixedString8_128*>(mString8)->~FixedString8_128();
	new(mString8) FixedString8_128();
#else
	delete reinterpret_cast<FixedString8_128*>(mString8);
	mString8 = new FixedString8_128();
#endif
	*(reinterpret_cast<FixedString8_128*>(mString8)) = *(reinterpret_cast<FixedString8_128*> ((rhs).GetImpl()));
	return *this;

}

EASTLFixedString8Wrapper::~EASTLFixedString8Wrapper()
{
	//Need to reinterpre_cast here otherwise the destructor won't execute
	FixedString8_128* stringPtr = reinterpret_cast<FixedString8_128*>(mString8);
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	stringPtr->~FixedString8_128();
#else
	delete stringPtr;
	mString8 = NULL;
#endif
}

const char8_t*	EASTLFixedString8Wrapper::
#if USE_INDIRECT_VIRTUALCALL
	GetCharactersInternal()
#else
	GetCharacters()
#endif
{
	return reinterpret_cast<EA::WebKit::FixedString8_128*>(mString8)->c_str();
}
void EASTLFixedString8Wrapper::
#if USE_INDIRECT_VIRTUALCALL
	SetCharactersInternal(const char8_t* chars)
#else
	SetCharacters(const char8_t* chars)
#endif
{
	reinterpret_cast<EA::WebKit::FixedString8_128*>(mString8)->assign(chars);
}

void* EASTLFixedString8Wrapper::GetImpl() const
{
	return (void*)mString8;
}

/******************************************EASTLFixedString16Wrapper******************************/
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
EA_COMPILETIME_ASSERT(EASTLFixedString16Wrapper::kFixedStringSize == sizeof(FixedString16_128));
EA_COMPILETIME_ASSERT(EA_PLATFORM_PTR_SIZE == EA_ALIGN_OF(FixedString16_128));
EA_COMPILETIME_ASSERT(EA_PLATFORM_PTR_SIZE == EA_ALIGN_OF(EASTLFixedString16Wrapper));
#endif

EASTLFixedString16Wrapper::EASTLFixedString16Wrapper()
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString16) FixedString16_128();
#else
	mString16 = new FixedString16_128();
#endif
}

EASTLFixedString16Wrapper::EASTLFixedString16Wrapper(const char16_t* str)
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString16) FixedString16_128(str);
#else
	mString16 = new FixedString16_128(str);
#endif
}

EASTLFixedString16Wrapper::EASTLFixedString16Wrapper(const EASTLFixedString16Wrapper& rhs)
{
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	new(mString16) FixedString16_128();
#else
	mString16 = new FixedString16_128();
#endif
	*(reinterpret_cast<FixedString16_128*>(mString16)) = *(reinterpret_cast<FixedString16_128*> ((rhs).GetImpl()));
}

EASTLFixedString16Wrapper& EASTLFixedString16Wrapper::operator =(const EASTLFixedString16Wrapper& rhs)
{
	if(this == &rhs)
		return *this;
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	reinterpret_cast<FixedString16_128*>(mString16)->~FixedString16_128();
	new(mString16) FixedString16_128();
#else
	delete reinterpret_cast<FixedString16_128*>(mString16);
	mString16 = new FixedString16_128();
#endif
	*(reinterpret_cast<FixedString16_128*>(mString16)) = *(reinterpret_cast<FixedString16_128*> ((rhs).GetImpl()));
	return *this;
}

EASTLFixedString16Wrapper::~EASTLFixedString16Wrapper()
{
	//Need to reinterpre_cast here otherwise the destructor won't execute
	FixedString16_128* stringPtr = reinterpret_cast<FixedString16_128*>(mString16);
#if STRINGWRAPPER_AVOID_HEAP_ALLOCATION
	stringPtr->~FixedString16_128();
#else
	delete stringPtr;
	mString16 = NULL;
#endif
}
const char16_t*	EASTLFixedString16Wrapper::
#if USE_INDIRECT_VIRTUALCALL
	GetCharactersInternal()
#else
	GetCharacters()
#endif
{
	return reinterpret_cast<EA::WebKit::FixedString16_128*>(mString16)->c_str();
}

void EASTLFixedString16Wrapper::
#if USE_INDIRECT_VIRTUALCALL
	SetCharactersInternal(const char16_t* chars)
#else
	SetCharacters(const char16_t* chars)
#endif
{
	reinterpret_cast<EA::WebKit::FixedString16_128*>(mString16)->assign(chars);
}

void* EASTLFixedString16Wrapper::GetImpl() const
{
	return (void*)mString16;
}


/******************************************EASTLHeaderMapWrapper******************************/
EASTLHeaderMapWrapper::EASTLHeaderMapWrapper()
	: mHeaderMap(0)
	, mHeaderMapWrapperIterator(0)
{
	mHeaderMap = new HeaderMap();
}

EASTLHeaderMapWrapper::EASTLHeaderMapWrapper(const EASTLHeaderMapWrapper& rhs)
{
	mHeaderMap = new HeaderMap();
	*(reinterpret_cast<HeaderMap*>(mHeaderMap)) = *(reinterpret_cast<HeaderMap*> ((rhs).GetImpl()));

	mHeaderMapWrapperIterator = 0; //Copying it does not make sense

}

EASTLHeaderMapWrapper& EASTLHeaderMapWrapper::operator =(const EASTLHeaderMapWrapper& rhs)
{
	if(this == &rhs)
		return *this;

	delete reinterpret_cast<HeaderMap*>(mHeaderMap);
	mHeaderMap = new HeaderMap();
	*(reinterpret_cast<HeaderMap*>(mHeaderMap)) = *(reinterpret_cast<HeaderMap*> ((rhs).GetImpl()));

	delete reinterpret_cast<EASTLHeaderMapWrapperIterator*>(mHeaderMapWrapperIterator);
	mHeaderMapWrapperIterator = 0; //Copying it does not make sense

	return *this;
}

EASTLHeaderMapWrapper::~EASTLHeaderMapWrapper()
{
	//Need to reinterpre_cast here otherwise the destructor won't execute
	HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);
	delete headerMapPtr;
	mHeaderMap = 0;

	delete mHeaderMapWrapperIterator;
	mHeaderMapWrapperIterator = 0;
}

const char16_t* EASTLHeaderMapWrapper::
#if USE_INDIRECT_VIRTUALCALL
	GetValueInternal(const char16_t* pKey)
#else
	GetValue(const char16_t* pKey)
#endif
	const
{
	HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);
	
	EA::WebKit::HeaderMap::iterator itWK = headerMapPtr->find_as(pKey, EA::WebKit::str_iless());
	if(itWK != headerMapPtr->end())  
		return itWK->second.c_str();

	return 0;
}

void EASTLHeaderMapWrapper::
#if USE_INDIRECT_VIRTUALCALL
	SetValueInternal(const char16_t* pKey, const char16_t* pValue)
#else
	SetValue(const char16_t* pKey, const char16_t* pValue)
#endif
{
	HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);

	EA::WebKit::HeaderMap::iterator itWK = headerMapPtr->find_as(pKey, EA::WebKit::str_iless());
	if(itWK != headerMapPtr->end())  
	{
		itWK->second = pValue;   
	}
	else
	{
		const EA::WebKit::HeaderMap::value_type entry(pKey, pValue);
		headerMapPtr->insert(entry);
	}
}
void EASTLHeaderMapWrapper::
#if USE_INDIRECT_VIRTUALCALL
	EraseValueInternal(const char16_t* pKey)
#else
	EraseValue(const char16_t* pKey)
#endif
{
	HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);
	headerMapPtr->erase(pKey);
}

void* EASTLHeaderMapWrapper::GetImpl() const
{
	return mHeaderMap;
}

EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator* EASTLHeaderMapWrapper::First()
{
	HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);
	if(headerMapPtr && !headerMapPtr->empty())
	{
		if(!mHeaderMapWrapperIterator)
			mHeaderMapWrapperIterator = new EASTLHeaderMapWrapperIterator();
		
		*(reinterpret_cast<HeaderMap::iterator*>(mHeaderMapWrapperIterator->mIterator)) = headerMapPtr->begin();
		
		return mHeaderMapWrapperIterator;
	}

	return 0;
}

EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator* EASTLHeaderMapWrapper::GetNext()
{
	EAW_ASSERT_MSG(mHeaderMapWrapperIterator, "Please make sure to call EASTLHeaderMapWrapper::First() before this. See header file for documentation.\n");
	if(mHeaderMapWrapperIterator)
	{
		HeaderMap* headerMapPtr = reinterpret_cast<HeaderMap*>(mHeaderMap);
		HeaderMap::iterator& headerMapIterator = *(reinterpret_cast<HeaderMap::iterator*>(mHeaderMapWrapperIterator->mIterator));
		
		++headerMapIterator;

		if(headerMapIterator != headerMapPtr->end())
			return mHeaderMapWrapperIterator;
	}

	return 0;
}

EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator::EASTLHeaderMapWrapperIterator()
	: mIterator(0)
{
	mIterator = new HeaderMap::iterator();
}

EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator::~EASTLHeaderMapWrapperIterator()
{
	if(mIterator)
	{
		HeaderMap::iterator* headerMapIterator = reinterpret_cast<HeaderMap::iterator*>(mIterator);
		delete headerMapIterator;
		mIterator = 0;
	}
}

const char16_t* EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator::GetKey() const
{
	HeaderMap::iterator& headerMapIterator = *(reinterpret_cast<HeaderMap::iterator*>(mIterator));
	return headerMapIterator->first.c_str();
}

const char16_t* EASTLHeaderMapWrapper::EASTLHeaderMapWrapperIterator::GetValue() const
{
	HeaderMap::iterator& headerMapIterator = *(reinterpret_cast<HeaderMap::iterator*>(mIterator));
	return headerMapIterator->second.c_str();
}
} // namespace WebKit
} // namespace EA