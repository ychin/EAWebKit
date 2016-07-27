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
// EAWebKitDomainFilter.cpp
// By Arpit Baldeva 
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <internal/include/EAWebKitDomainFilter.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKitString.h>

#include <EAIO/FnEncode.h> 

#include <URL.h>

namespace EA
{
namespace WebKit
{
EAWebKitDomainFilter::EAWebKitDomainFilter()
{
	mDomainInfoCollection.clear();
}

void EAWebKitDomainFilter::Shutdown()
{
	mDomainInfoCollection.clear();
}

void EAWebKitDomainFilter::AddAllowedDomainInfo(const char8_t* allowedDomain, const char8_t* excludedPaths)
{
	if(!allowedDomain || !allowedDomain[0])
	{
		EAW_ASSERT_MSG(false, "Can't add a null/empty Domain");
		return;
	}
	
	mDomainInfoCollection.push_back(DomainInfo(allowedDomain, excludedPaths));
}

bool EAWebKitDomainFilter::CanNavigateToURL(const char8_t* pURL) const
{
	//If there is nothing in the domain info, we assume that it is OK to go to any link. The reason for this is that we don't want teams 
	//to do anything unnecessarily. If they want to use the domain filtering feature, they can add their Info.
	if(mDomainInfoCollection.empty())
		return true;

	bool urlAllowed = false;
	for(DomainInfoCollection::const_iterator domainInfoIter = mDomainInfoCollection.begin(); domainInfoIter != mDomainInfoCollection.end(); ++domainInfoIter)
	{
		urlAllowed = URLAllowed(pURL, *domainInfoIter);	

		if(urlAllowed)
			break;
	}
	return urlAllowed;

}

bool EAWebKitDomainFilter::CanNavigateToURL(const WebCore::URL& kurl) const
{
	//If there is nothing in the domain info, we assume that it is OK to go to any link. The reason for this is that we don't want teams 
	//to do anything unnecessarily. If they want to use the domain filtering feature, they can add their Info.
	if(mDomainInfoCollection.empty())
		return true;
	
	FixedString8_128 url; 
	EA::WebKit::ConvertToString8(kurl.string(), url);

	return CanNavigateToURL(url.c_str());

}

bool EAWebKitDomainFilter::URLAllowed(const char8_t* pURL, const DomainInfo& domainInfo) const
{
	bool urlMatched = false;
	WebCore::URL url(WebCore::URL(), pURL);

	//It may seem a little strange but only try to block http(s) schemes. This is so that the teams don't have to register their own schemes 
	//in order to get past the filtering system. Transport handler would handle that.
	if(!url.protocolIsInHTTPFamily())
		return true;

	FixedString8_128 host; 
	EA::WebKit::ConvertToString8(url.host(), host);

	FixedString8_128 path; 
	EA::WebKit::ConvertToString8(url.path(), path);


	if(DomainAllowed(host, domainInfo.mAllowedDomain))
	{
		if(PathAllowed(path, domainInfo.mExcludedPaths))
		{
			urlMatched = true;
		}
	}

	return urlMatched;
}

//replicate the code from the cookie manager. This code can use a little cleanup. But since it is tried and tested, keeping it as is.
bool EAWebKitDomainFilter::DomainAllowed(const FixedString8_128& host, const FixedString8_128& allowedDomain) const
{
	int32_t start  = host.length() - allowedDomain.length();
	if(start >= 0 )
	{
		FixedString8_128 hostDomain = host.substr(start,FixedString8_128::npos);
		if( hostDomain.comparei(allowedDomain) == 0)
		{
			if( (start == 0) || (allowedDomain[0] == '.') || (host[start-1] == '.'))
			{
				return true;
			}
		}
	}
	else if (start == -1)
	{
		FixedString8_128 hostWithDot(FixedString8_128::CtorSprintf(),".%s",host.c_str());
		if( hostWithDot.comparei(allowedDomain) == 0)
			return true;
	}
	
	return false;
}

static void EnsurePathSeparators(FixedString8_128& path)
{
	//Ensure that we have '/' at the front and back of the path. This is so that we don't end up matching 
	// /mypath to /mypathfoo.

	if(path[0] != '/')
		path = '/' + path;
	if(path[path.length()-1] != '/')
		path.push_back('/');
}

bool EAWebKitDomainFilter::PathAllowed(const FixedString8_128& path, const FixedString8_128& excludedPaths) const
{
	//We make a copy because we do custom processing on it.
	FixedString8_128 excludedPathsLocal(excludedPaths);
	FixedString8_128 pathLocal(path);
	EnsurePathSeparators(pathLocal);

	//If the excludedPaths is empty, we assume that all the paths are okay.
	if(excludedPathsLocal.empty())
		return true;

	//Expect the user to put a ";" at the end and remove it if found.
	if(excludedPathsLocal[excludedPathsLocal.length()-1] == ';')
		excludedPathsLocal.pop_back();

	FixedString8_128 excludedPath;
	FixedString8_128::size_type lastDelimPos = 0;
	FixedString8_128::size_type delimPos = excludedPathsLocal.find(";");
	while(delimPos != FixedString8_128::npos)
	{
		excludedPath = excludedPathsLocal.substr(lastDelimPos, delimPos);
		EnsurePathSeparators(excludedPath);
		if(pathLocal.find(excludedPath) != FixedString8_128::npos)
			return false;

		lastDelimPos = delimPos+1; //Doing this simplifies the code
		delimPos = excludedPathsLocal.find(";",lastDelimPos);
	}

	excludedPath = excludedPathsLocal.substr(lastDelimPos, excludedPathsLocal.length());
	EnsurePathSeparators(excludedPath);
	if(pathLocal.find(excludedPath) != FixedString8_128::npos)
		return false;

	return true;
}


} //namespace WebKit
} // namespace EA







