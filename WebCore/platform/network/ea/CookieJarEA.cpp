/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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
 
/////////////////////////////////////////////////////////////////////////////
// CookieJarEA.cpp
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "config.h"
#include "CookieJar.h"

#include "Cookie.h"
#include "ResourceHandleManagerEA.h"
#include "URL.h"
#include "wtf/text/CString.h"
#include "NetworkStorageSession.h"
#include "CookieManagerEA.h"
#include <internal/include/EAWebKit_p.h>
#include <internal/include/EAWebKitAssert.h>

namespace WebCore {

 
///////////////////////////////////////////////////////////////////////////////
// Note by Paul Pedriana:
//
// See http://webmaster.info.aol.com/aboutcookies.html to understand a little about cookies in HTTP.
// Also http://msdn.microsoft.com/en-us/library/aa384321(VS.85).aspx
// Also http://www.faqs.org/rfcs/rfc2965.html
//
// Web pages that test cookie support can be found at:
//     http://www.tempesttech.com/cookies/cookietest1.asp  (very simple)
//     http://www.pageout.net/page.dyn/cookie_tester
//
// Set-Cookie HTTP header format (server to client):
//     Set-Cookie: <name>=<value>[; <name>=<value>]...[; expires=<date>][; domain=<domain_name>][; path=<some_path>][; secure][; httponly]
// Example:
//     Set-Cookie:Test=test_value; expires=Sat, 01-Jan-2000 00:00:00 GMT; path=/
//
// Cookie HTTP header format (client to server):
//     Cookie: <name>=<value>[; <name>=<value>]...
// Example:
//     Cookie: Test=test_value; color=green
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// WebKit expects cookie support to be handled by the HTTP transport system
// and WebKit doesn't do any work to parse or send cookies, with two exceptions:
//
//    - The HTML of a page has an http header directive in a <meta> tag within the <head> tag, as with:
//          <html><head><meta http-equiv="Set-Cookie" content="SomeCookie=SomeValue; expires=Sat, 01-Jan-2000 00:00:00 GMT; path=/"/></head></html>
//
//    - The JavaScript of a page directly sets a cookie, as with the following JavaScript:
//          document.cookie = "SomeCookie=SomeValue; expires=Sat, 01-Jan-2000 00:00:00 GMT; path=/"
//
// In either case we have a cookie being specified in the page instead of the 
// HTTP headers and the transport system doesn't see it. So WebKit calls the 
// setCookies function here and expects that somehow this is relayed to the transport system.
//
// Cookies that come via HTTP headers need to be handled by the transport system
// or some external entity that gets to see the headers upon receiving them.
// Similarly, sending of cookies is always done via HTTP headers and need to be sent by 
// the transport system or some external cookie management system that can set
// header entries before they are sent. 
//
// So what we need to do is implement cookie support for our transport system
// and have the functions here tie into it.
///////////////////////////////////////////////////////////////////////////////


// This function is called in situations where cookie needs to accessed outside the transport system. For example, from JavaScript in the page.
// A call to read document.cookie (For example, alert(document.cookie) will retrieve all the cookies separated using a ";"
/*
<script type="text/javascript">
function myfunction() {
document.cookie = "firstname=john";
document.cookie = "lastname=smith";
alert(document.cookie);
}
</script>
*/

String cookiesForDOM(const NetworkStorageSession&, const URL&, const URL& url)
{
    if(EA::WebKit::EAWebKitGetCookiesCallback pCallback = EA::WebKit::GetGetCookiesCallback())
	{
		auto urlStr = url.string().charactersWithNullTermination();
		EA::WebKit::EASTLFixedString16Wrapper cookiesStr;
		pCallback(urlStr.data(),cookiesStr,0);
		return String(cookiesStr.GetCharacters());
	}
	else if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
    {

		EA::WebKit::FixedString8_256 cookieStr8 = cookieManager->GetCookieTextForURL(url,false,true); //Get all the cookies except the ones set to HttpOnly
        return String(cookieStr8.c_str());
    }
    
	return String();
}

String cookieRequestHeaderFieldValue(const NetworkStorageSession&, const URL&, const URL& url)
{
	if(EA::WebKit::EAWebKitGetCookiesCallback pCallback = EA::WebKit::GetGetCookiesCallback())
	{
		auto urlStr = url.string().charactersWithNullTermination();
		EA::WebKit::EASTLFixedString16Wrapper cookiesStr;
		pCallback(urlStr.data(),cookiesStr,EA::WebKit::CookieFlags::HttpOnly);
		return String(cookiesStr.GetCharacters());
	}
	else if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
	{
		EA::WebKit::FixedString8_256 cookieStr8 = cookieManager->GetCookieTextForURL(url,true); 
		return String(cookieStr8.c_str());
	}

	return String();
}


// This function is called by the Document or JSDocument when the page itself specifies
// a cookie. We want to relay this to our central cookie manager.
// Contrary to it's name, the setCookies is called for just a single cookie and not multiple cookies. For example, you'll call
// document.cookie twice in order to set two cookies and not once.
// document.cookie = "firstname=john"; // First call to setCookies
// document.cookie = "lastname=smith"; // Second call to setCookies
void setCookiesFromDOM(const NetworkStorageSession&, const URL&, const URL& url, const String& value)
{
	if(EA::WebKit::EAWebKitSetCookieCallback pCallback = EA::WebKit::GetSetCookieCallback())
	{
		if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
		{
			EA::WebKit::Cookie* cookie = cookieManager->ParseCookieHeader(value.ascii().data(),url.string().ascii().data(),true);
			if(cookie && cookieManager->ValidateCookie(cookie,url.string().ascii().data()))
			{
				EA::WebKit::CookieEx cookieEx;
				auto cookieName = String(cookie->mName.c_str()).charactersWithNullTermination();
				auto cookieVal = String(cookie->mValue.c_str()).charactersWithNullTermination();
				if(cookie->mDomain.find_first_of('.') == 0)
					cookie->mDomain.erase(0,1);

				String urlBuilder = url.protocol() + EA_CHAR16("://") + String(cookie->mDomain.c_str()) + String(cookie->mPath.c_str());
				auto urlStr = urlBuilder.charactersWithNullTermination();
				
				cookieEx.pName	= cookieName.data();
				cookieEx.pValue = cookieVal.data();
				cookieEx.pUrl	= urlStr.data();
				
				cookieEx.expires = cookie->mExpirationTime;

				uint32_t flags = 0;
				if(cookie->mExpiresAtEndOfSession)
					flags |= EA::WebKit::CookieFlags::Session;
				if(cookie->mSecure)
					flags |= EA::WebKit::CookieFlags::Secure;

				EAW_ASSERT_MSG(!cookie->mHttpOnly, "Can not set a httpOnly cookie from JavaScript");

				cookieEx.flags = flags;
				bool success = pCallback(cookieEx);
				(void) success;
				EAW_ASSERT_MSG(success, "Failure in setting a cookie using platform callback.");
			}
			delete cookie;
		}
	}
	else if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
	{
		EA::WebKit::FixedString8_128 url8; 
		EA::WebKit::ConvertToString8(url.string(),url8);

		EA::WebKit::FixedString8_128 valueStr8; 
		EA::WebKit::ConvertToString8(value, valueStr8);
		cookieManager->ProcessCookieHeader(valueStr8.c_str(),url8.c_str(),true);
	}
}



bool cookiesEnabled(const NetworkStorageSession&, const URL&, const URL&)
{
    if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
	{
		const EA::WebKit::CookieManagerParameters& params = cookieManager->GetParameters();
		return (params.mMaxCookieCount > 0);
	}

	return false;
}

// The return value is a signal to the caller - true if we were able to retrieve the cookies, false if we were not
// (If the caller does receive false, they might try an alternate method like cookiesForDOM)
bool getRawCookies(const NetworkStorageSession&, const URL& first_party, const URL& url, Vector<Cookie>& list)
{
    if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
    {
        list.appendVector(cookieManager->GetCookiesForURL(url));

	    return true;
    }

    return false;
}

// This function is only a convenience function for Web Inspector. There is no method in JavaScript to directly delete a cookie except by setting it's date to a past date.
void deleteCookie(const NetworkStorageSession&, const URL& url, const String& name)
{
	if(EA::WebKit::CookieManager* cookieManager = WebCore::ResourceHandleManager::sharedInstance()->GetCookieManager())
	{
		cookieManager->deleteCookie(url,name);
	}
}

void getHostnamesWithCookies(const NetworkStorageSession&, HashSet<String>& hostnames)
{
	//implement
}
void deleteCookiesForHostname(const NetworkStorageSession&, const String& hostname)
{
	//implement
}
void deleteAllCookies(const NetworkStorageSession&)
{
	//implement
}

}



