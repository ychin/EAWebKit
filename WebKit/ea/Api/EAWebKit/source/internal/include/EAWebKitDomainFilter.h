/*
Copyright (C) 2010, 2011, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitDomainFilter.h
//
// By Arpit Baldeva 
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITDOMAINFILTER_H
#define EAWEBKIT_EAWEBKITDOMAINFILTER_H

#include <EABase/eabase.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <EASTL/vector.h>



namespace WebCore
{
	class URL;
}

namespace EA
{
    namespace WebKit
    {
		struct DomainInfo
		{
			EA::WebKit::FixedString8_128 mAllowedDomain;
			EA::WebKit::FixedString8_128 mExcludedPaths;

			DomainInfo(const char8_t* domain, const char8_t* excludedPaths)
				: mAllowedDomain(domain)
			{
				if(excludedPaths)
					mExcludedPaths.assign(excludedPaths);
				else
					mExcludedPaths.clear();//Just to be safe
			}
		};
		
		class EAWebKitDomainFilter
		{
		public:
			static EAWebKitDomainFilter& GetInstance()
			{
				static EAWebKitDomainFilter instance;
				return instance;
			}
			void Shutdown();

			void AddAllowedDomainInfo(const char8_t* allowedDomain, const char8_t* excludedPaths);
			bool CanNavigateToURL(const char8_t* pURL) const;
			bool CanNavigateToURL(const WebCore::URL& kurl) const;
		private:
			EAWebKitDomainFilter();
			EAWebKitDomainFilter(const EAWebKitDomainFilter& instance);
			EAWebKitDomainFilter& operator = (const EAWebKitDomainFilter& instance);

			bool URLAllowed(const char8_t* pURL, const DomainInfo& domainInfo) const;
			bool DomainAllowed(const FixedString8_128& host, const FixedString8_128& allowedDomain) const;
			bool PathAllowed(const FixedString8_128& path, const FixedString8_128& excludedPaths) const;
			//We are storing by value but we do not expect to have more than 2-5 instances so we are good.
			typedef eastl::vector<DomainInfo> DomainInfoCollection;
			DomainInfoCollection mDomainInfoCollection;

		};
    }
}

#endif // EAWEBKIT_EAWEBKITDOMAINFILTER_H
