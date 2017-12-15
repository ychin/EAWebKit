/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Collator.h"
#include <wtf/FastMalloc.h>

namespace EA
{
namespace WebKit
{
const char8_t* GetLocale();
}
}

#include EAWEBKIT_PLATFORM_HEADER
#define strdup _strdup

namespace WTF {

Collator::Collator(const char* locale)
	: m_locale(locale ? strdup(locale) : 0)
	, m_lowerFirst(false)
{
	/*
	if(locale && locale[0])
	{
		size_t length = strlen(locale);
		m_locale = (char*) fastMalloc(length + 1);
		strncpy(m_locale,locale,length);
		m_locale[length] = 0;
	}
	*/
}

Collator::~Collator()
{
	free(m_locale);
}

void Collator::setOrderLowerFirst(bool lowerFirst)
{
	m_lowerFirst = lowerFirst;
}

std::unique_ptr<Collator> Collator::userDefault()
{
    // Note by Arpit Baldeva: The JavaScript API specification says that the localCompare method should use the default system locale (Note that this may be
	// different from the locale selected by the user but that is what the standard says).
	// On Windows, default locale is identified by LOCALE_USER_DEFAULT. 
	// We request the user of this library to pass the default locale on Initialization. If NULL is passed, en-us is assumed.

	const char* userDefaultlocale = EA::WebKit::GetLocale();
	if(!(userDefaultlocale && userDefaultlocale[0]))
		userDefaultlocale = "en-US";

	return  std::make_unique<Collator>(userDefaultlocale);
}

Collator::Result Collator::collate(const UChar* lhs, size_t lhsLength, const UChar* rhs, size_t rhsLength) const
{
	// As it turns out, the collation code implementation in EAText is not currently finished. So for now, we disable the code and use 
	// old code. That code works correctly on Windows but has issues on platform other than Windows.
#if 0
	EA::Text::Collator collator;
	collator.SetLocale(m_locale);
	// We don't set any options and use the EAText defaults.
	//collator.SetSortOption(EA::Text::Collator::kSOLocaleFirst, 1);

	eastl::string16 lhsStr(lhs, lhsLength);
	eastl::string16 rhsStr(rhs, rhsLength);

	int result = collator.Compare(lhsStr.c_str(), rhsStr.c_str());
	return (Collator::Result)result;
	
#else
	// Test link - http://www.tutorialspoint.com/cgi-bin/practice.cgi?file=javascript_79 
	// A.localeCompare(B) returns -1 on all Browsers (including ours).
	// A.localeCompare(a) returns 1 on IE/Firefox. Chrome returns -32(basically -1). Safari in unable to handle the link.
	
	// In our implementation, A.localeCompare(a) returns 1 on Windows while returns -1 on Consoles. There is no easy way to fix this behavior consistently
	// across all locales. In practice, it is probably not that important for our usage.
		// The reason to subtract 2 is documented at http://msdn.microsoft.com/en-us/library/dd317761%28v=vs.85%29.aspx 
		//
		// Returns one of the following values if successful. To maintain the C runtime convention of comparing strings, the value 2 can be subtracted from a 
		// nonzero return value. Then, the meaning of <0, ==0, and >0 is consistent with the C runtime.
		//
		int result = CompareStringW(LOCALE_USER_DEFAULT, 0, lhs, (int)lhsLength, rhs, (int)rhsLength) - 2;
		return (Collator::Result)result;
#endif

}


}

