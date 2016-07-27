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

#ifndef EAWEBKIT_EAWEBKIT_P_H
#define EAWEBKIT_EAWEBKIT_P_H

#include <EAWebKit/EAWebKit.h>
#include <EAIO/PathString.h>
#include "Color.h"

namespace EA
{
namespace WebKit
{
class ISurface;

bool Init(AppCallbacks* appCallbacks = NULL, AppSystems* appSystems = NULL);
void Shutdown();
void Destroy();

void Tick();

View* CreateView();
void  DestroyView(View* pView);

void			SetRAMCacheUsage(const RAMCacheInfo& ramCacheInfo);
void			GetRAMCacheUsage(RAMCacheUsageInfo& ramCacheUsageInfo);
bool			SetDiskCacheUsage(const DiskCacheInfo& diskCacheInfo); //Returns a bool that indicates if cache directory is successfully created.
void			GetDiskCacheUsage(DiskCacheUsageInfo& diskCacheUsageInfo);
void			SetCookieUsage(const CookieInfo& cookieInfo);
void            SetTextSystem(ITextSystem* pTextSystem);    // This replaces the old glyph cache and font server set.
ITextSystem*    GetTextSystem();


double GetTime();

void						    SetHighResolutionTimer(EAWebKitTimerCallback timer); 
EAWebKitTimerCallback		    GetHighResolutionTimer();

void                            SetMonotonicTimer(EAWebKitMonotonicTimerCallback timer);
EAWebKitMonotonicTimerCallback  GetMonotonicTimer();

void						    SetStackBaseCallback(EAWebKitStackBaseCallback);
EAWebKitStackBaseCallback	    GetStackBaseCallback();

void                        	SetCryptographicallyRandomValueCallback(EAWebKitCryptographicallyRandomValueCallback);

void							SetGetCookiesCallback(EAWebKitGetCookiesCallback getCookies);
EAWebKitGetCookiesCallback		GetGetCookiesCallback();

void							SetSetCookieCallback(EAWebKitSetCookieCallback setCookie);
EAWebKitSetCookieCallback		GetSetCookieCallback();


void ReattachCookies(EA::WebKit::TransportInfo* pTInfo);
void CookiesReceived(EA::WebKit::TransportInfo* pTInfo);

void		RemoveCookies();
const char8_t* GetVersion(); 

JavascriptValue *CreateJavascriptValue(View *view);
void DestroyJavascriptValue(JavascriptValue *v);

JavascriptValue *CreateJavascriptValueArray(View *view, size_t count);
void DestroyJavascriptValueArray(JavascriptValue *array);

void ClearMemoryCache();

void RegisterURLSchemeAsCORSEnabled(const char16_t* pScheme);
bool IsURLSchemeCORSEnabled(const char16_t* PScheme);

bool AddCookie(const char8_t* pHeaderValue);
uint16_t ReadCookies(char8_t** rawCookieData);
void SetPlatformSocketAPI(const EA::WebKit::PlatformSocketAPI& platformSocketAPI);
SocketTransportHandler* GetSocketTransportHandler();

//allowedDomain is something like .ea.com and excludedPaths is an optional semicolon separated list of paths they you may want do not include, 
//eg, "/ads;/scr" 
void		AddAllowedDomainInfo(const char8_t* allowedDomain, const char8_t* excludedPaths = 0);
//Pass in a fully qualified URL (eg, http://mygame.ea.com/ to see if it is in the White list setup using the API above. 
bool		CanNavigateToURL(const char8_t* url);


void NetworkStateChanged(bool isOnline);


// EAWEBKIT_API 
Parameters& GetParameters();
void SetParameters(const Parameters& params);



ThemeParameters& GetThemeParameters();
void SetThemeParameters(const ThemeParameters& themeParams);

EA::IO::Path::PathString8 GetFullPath(const char8_t* pRelativePath, bool useTempDirAsBasePath);
WebKitStatus			GetWebKitStatus();

void ClearSurfaceToColor(ISurface *surface, WebCore::Color color); //extracted so multiple code paths can use it

// 9/2/09 CSidhall - Added to get an approximate stack base.  This needs to be called 
// pretty high up to work and only works in a single threaded system.
// 2/10/10 - Made into small auto class instead for destructor call on exit
class AutoCollectorStackBase
{
public:
	AutoCollectorStackBase();
	~AutoCollectorStackBase();
};

void EAWebKitThreadErrorCheck(void);

} //WebKit
} //EA

#define SET_AUTO_COLLECTOR_STACK_BASE() EA::WebKit::AutoCollectorStackBase autoStackBase
#define EAWEBKIT_THREAD_CHECK() EA::WebKit::EAWebKitThreadErrorCheck()
#define EAWWBKIT_INIT_CHECK() EAW_ASSERT_MSG( (EA::WebKit::GetWebKitStatus() == EA::WebKit::kWebKitStatusActive), "EAWebKit must be init() and active before calling this function.");
#define EAWWBKIT_NOT_INACTIVE_CHECK() EAW_ASSERT_MSG( (EA::WebKit::GetWebKitStatus() != EA::WebKit::kWebKitStatusInactive), "EAWebKit must not be inactive before calling this function.");

#endif // EAWEBKIT_EAWEBKIT_P_H
