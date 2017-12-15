/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc. All rights reserved.
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
#include <wtf/unicode/Collator.h>
#include "WTFString.h"

#if defined(EA_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace WTF {

Collator::Collator(const char* locale, bool shouldSortLowerCaseFirst)
{}

Collator::~Collator()
{}

int Collator::collate(StringView a, StringView b) const
{
	// As it turns out, the collation code implementation in EAText is not currently finished. So for now, we disable the code and use 
	// old code. That code works correctly on Windows but has issues on platform other than Windows.  Note that to complete this we would
	// need to include collator functions in the TextWrappter not call EA::Text directly.
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
		#if defined(EA_PLATFORM_WINDOWS)

			ASSERT(a.is8Bit() == b.is8Bit());
			int result = 0;
			if (a.is8Bit())
			{
				result = CompareStringA(LOCALE_USER_DEFAULT, 0, (const char *)a.characters8(), a.length(), (const char *)b.characters8(), b.length());
			}
			else
			{
				result = CompareStringW(LOCALE_USER_DEFAULT, 0, reinterpret_cast<PCNZWCH>(a.characters16()), a.length(), reinterpret_cast<PCNZWCH>(b.characters16()), b.length());
			}

			// The reason to subtract 2 is documented at http://msdn.microsoft.com/en-us/library/dd317761%28v=vs.85%29.aspx 
			//
			// Returns one of the following values if successful. To maintain the C runtime convention of comparing strings, the value 2 can be subtracted from a 
			// nonzero return value. Then, the meaning of <0, ==0, and >0 is consistent with the C runtime.
			//			
			ASSERT(result != 0);
			return result - 2;
		#else
			// Following does not conform to Unicode so it may be incorrect for some locales. Ported from CollatorDefault.cpp.
			unsigned commonLength = std::min(a.length(), b.length());
			for (unsigned i = 0; i < commonLength; ++i) {
				if (a[i] < b[i])
					return -1;
				if (a[i] > b[i])
					return 1;
			}

			if (a.length() < b.length())
				return -1;
			if (a.length() > b.length())
				return 1;

			return 0;
		#endif
	#endif
}

int Collator::collateUTF8(const char* a, const char* b) const
{
	return collate(String::fromUTF8(a), String::fromUTF8(b));
}

}

