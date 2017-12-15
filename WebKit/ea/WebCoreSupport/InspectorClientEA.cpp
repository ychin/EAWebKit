/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
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
#include "InspectorClientEA.h"

#include "FrameView.h"
#include "InspectorController.h"
#include "InspectorFrontend.h"
#include "MainFrame.h"
#include "NotImplemented.h"
#include "Page.h"
#include "ScriptDebugServer.h"
#include <wtf/text/CString.h>

#include "webinspector.h"
#include "webpage.h"
#include "webpage_p.h"

#include <EAWebkit/EAWebkitView.h>
#include <internal/include/EAWebKit_p.h>
#include <EAWebkit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebKitAssert.h>

#if ENABLE(INSPECTOR)

// The set up here and management of object ownership is overly complex and pretty much like a spaghetti. Not making any changes because it works.
namespace WebCore {

InspectorClientEA::InspectorClientEA(EA::WebKit::WebPage* page)
    : mInspectedPage(page)
    , mInspectorPage(0)
    , mInspectorPageClient(0)
{

	WebCore::InspectorSettingsEA::sharedInstance(); //This implicitly reads the inspector settings.

}

void InspectorClientEA::inspectorDestroyed(void)
{
    closeInspectorFrontend();
    delete this;
}

    
WebCore::InspectorFrontendChannel*  InspectorClientEA::openInspectorFrontend(WebCore::InspectorController* inspectorController)
{
    WebCore::InspectorFrontendChannel* frontendChannel = 0;
    
	// Create the view and page for the inspector.
    if(EA::WebKit::EAWebKitClient* const pClient = EA::WebKit::GetEAWebKitClient(mInspectedPage->view()))
    {
        EA::WebKit::CreateViewInfo createViewInfo;
        createViewInfo.mpView = mInspectedPage->view();
        createViewInfo.mpUserData = createViewInfo.mpView->GetUserData();
		createViewInfo.mEventType = EA::WebKit::CreateViewInfo::kEventInspectorOpen;
        pClient->CreateView(createViewInfo);

        if(createViewInfo.mpCreatedView)
        {
            EA::WebKit::View* inspectorView = createViewInfo.mpCreatedView;
            EA::WebKit::WebPage* inspectorPage = inspectorView->Page();
            
            // Load the inspector
            EA::WebKit::WebInspector* inspector = mInspectedPage->GetInspector();
            inspector->SetFrontend(inspectorView);

            EA::WebKit::WebFrame* mainFrame = inspectorPage->mainFrame();

            EA::IO::Path::PathString8 fullPath;
            fullPath.assign(EA::WebKit::GetFullPath("WebInspectorUI/UserInterface/main.html", false));
            EA::IO::Path::PathString8 url = "file:///";
            url.append(fullPath);

            WebCore::URL kurl(WebCore::URL(), url.c_str());
            mainFrame->load(kurl);

            // Create the frontend client
            mInspectorPageClient = new InspectorFrontendClientEA(mInspectedPage, inspectorView, this);
            EA::WebKit::WebPagePrivate::core(inspectorView->Page())->inspectorController()->setInspectorFrontendClient(adoptPtr(mInspectorPageClient));
            mInspectorPage = inspectorPage;
			mInspectorPage->handle()->page->setGroupName("__WebInspectorPageGroup__");
            inspector->SetFrontendClient(mInspectorPageClient);
			frontendChannel = this;
        }
    }
	return frontendChannel;
}

void InspectorClientEA::closeInspectorFrontend()
{
	if (mInspectorPageClient)
		mInspectorPageClient->closeWindow();
}


void InspectorClientEA::bringFrontendToFront()
{
	notImplemented();
}

void InspectorClientEA::releaseFrontendPage(void)
{
    mInspectorPage = 0;
    mInspectorPageClient = 0;
}

void InspectorClientEA::highlight(void)
{
    hideHighlight();
}

void InspectorClientEA::hideHighlight(void)
{
	WebCore::Frame& frame = mInspectedPage->d->page->mainFrame();
	WebCore::IntRect rect = mInspectedPage->mainFrame()->geometry();
	if (!rect.isEmpty())
		frame.view()->invalidateRect(rect);
}

bool InspectorClientEA::sendMessageToFrontend(const String& message)
{
    if (!mInspectorPage)
        return false;

    Page* frontendPage = mInspectorPage->d->page;
    return doDispatchMessageOnFrontendPage(frontendPage, message);
}

class InspectorSettingsEA;


class InspectorFrontendSettingsEA : public InspectorFrontendClientLocal::Settings {
public:
    virtual ~InspectorFrontendSettingsEA() { }
    // These functions are no longer called. 
	virtual String getProperty(const String& name)
    {
		String value;
		InspectorSettingsEA::sharedInstance()->populateSetting(name, &value);
		return value;
    }

    virtual void setProperty(const String& name, const String& value)
    {
        InspectorSettingsEA::sharedInstance()->storeSetting(name, value);
    }
};

InspectorFrontendClientEA::InspectorFrontendClientEA(EA::WebKit::WebPage* inspectedWebPage, EA::WebKit::View *inspectorView, InspectorClientEA* inspectorClient)
    : InspectorFrontendClientLocal(inspectedWebPage->d->page->inspectorController(), inspectorView->Page()->d->page, adoptPtr(new InspectorFrontendSettingsEA())) 
    , mInspectedWebPage(inspectedWebPage)
    , mInspectorView(inspectorView)
    , mDestroyingInspectorView(false)
    , mInspectorClient(inspectorClient)
{
    // Do nothing.
}

InspectorFrontendClientEA::~InspectorFrontendClientEA(void)
{
    ASSERT(mDestroyingInspectorView);
    if (mInspectorClient)
        mInspectorClient->releaseFrontendPage();
}

void InspectorFrontendClientEA::frontendLoaded(void)
{
    InspectorFrontendClientLocal::frontendLoaded();
    setAttachedWindow(DOCKED_TO_BOTTOM);
}

String InspectorFrontendClientEA::localizedStringsURL(void)
{
	EA::IO::Path::PathString8 fullPath;
	fullPath.assign(EA::WebKit::GetFullPath("WebInspectorUI/Localizations/en.lproj/localizedStrings.js", false));
	EA::IO::Path::PathString8 url = "file:///";
	url.append(fullPath);

	return String(url.c_str());
}



void InspectorFrontendClientEA::bringToFront(void)
{
    //updateWindowTitle();
}

void InspectorFrontendClientEA::closeWindow(void)
{
    destroyInspectorView(true);
}

void InspectorFrontendClientEA::disconnectFromBackend(void)
{
    destroyInspectorView(false);
}

void InspectorFrontendClientEA::attachWindow(DockSide)
{
    notImplemented();
}

void InspectorFrontendClientEA::detachWindow(void)
{
    notImplemented();
}

void InspectorFrontendClientEA::setAttachedWindowHeight(unsigned)
{
    notImplemented();
}

void InspectorFrontendClientEA::setAttachedWindowWidth(unsigned)
{
    notImplemented();
}

void InspectorFrontendClientEA::setToolbarHeight(unsigned)
{
    notImplemented();
}

void InspectorFrontendClientEA::inspectedURLChanged(const String& newURL)
{
    //mInspectedURL.assign = newURL;
    //updateWindowTitle();
}

void InspectorFrontendClientEA::updateWindowTitle(void)
{
}

void InspectorFrontendClientEA::destroyInspectorView(bool notifyInspectorController)
{
    if (mDestroyingInspectorView)
    {
        return;
    }

    mDestroyingInspectorView = true;

    // Inspected page may have already been destroyed.
    if (mInspectedWebPage) 
    {
        mInspectedWebPage->GetInspector()->SetFrontend(NULL);
        mInspectedWebPage->DestroyInspector();
    }

    if (notifyInspectorController && (mInspectedWebPage))
        mInspectedWebPage->d->page->inspectorController()->disconnectFrontend();

    if (mInspectorClient)
        mInspectorClient->releaseFrontendPage();


    // Clear pointer before deleting WebView to avoid recursive calls to its destructor.
    if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(mInspectorView))
    {
        EA::WebKit::DestroyViewInfo destroyInfo;
        destroyInfo.mpView = mInspectorView;
        destroyInfo.mpUserData = mInspectorView->GetUserData();
		destroyInfo.mEventType = EA::WebKit::DestroyViewInfo::kEventInspectorClose;
        pClient->DestroyView(destroyInfo);
        mInspectorView = NULL;
    }
}

void InspectorFrontendClientEA::inspectorClientDestroyed(void)
{
    mInspectorClient = 0;
    mInspectedWebPage = 0;
}


static const char8_t* sEAWebKitInspectorSettingsFormatVersion = "InspectorSettingsFormat:1.0";
static const char8_t* sEAWebKitInspectorKey = "EAInspectorKey:";
static const char8_t* sEAWebKitInspectorValue = ";EAInspectorValue:";
static const char8_t* INSPECTOR_SETTINGS_DELIMITER = "\n";

const int32_t kMaxInspectorFileSize = 32 * 1024; //32K, This is mostly for avoiding some pathological situation. Little value in exposing it and let user deal with another API. 
static InspectorSettingsEA* spInspectorSettingsEA = NULL;

InspectorSettingsEA* InspectorSettingsEA::sharedInstance()
{
	if(!spInspectorSettingsEA)
		spInspectorSettingsEA = new InspectorSettingsEA();
	return spInspectorSettingsEA;
}

void InspectorSettingsEA::finalize()
{
	delete spInspectorSettingsEA;
	spInspectorSettingsEA = NULL;
}

InspectorSettingsEA::InspectorSettingsEA()
{
	mInspectorSettingsPath = EA::WebKit::GetFullPath("InspectorSettings.txt",true).c_str();
	readSettingsFromFile();
}

InspectorSettingsEA::~InspectorSettingsEA()
{
	saveSettingsToFile();
}

void InspectorSettingsEA::populateSetting(const String& key, String* value)
{
	eastl::string8 keyEA = key.ascii().data();

	InspectorSettingsMap::iterator iter = mSettingsMap.find(keyEA);
	if( iter != mSettingsMap.end())
	{
		*value = iter->second.c_str();
	}
}

void InspectorSettingsEA::storeSetting(const String& key, const String& value)
{
	eastl::string8 keyEA = key.ascii().data();
	InspectorSettingsMap::iterator iter = mSettingsMap.find(keyEA);
	if( iter != mSettingsMap.end())
	{
		iter->second = value.ascii().data();
	}
	else
	{
		InspectorSettingsMap::value_type entry(key.ascii().data(), value.ascii().data());
		mSettingsMap.insert(entry);
	}
}
void InspectorSettingsEA::readSettingsFromFile()
{
	EA::WebKit::FileSystem* pFileSystem = EA::WebKit::GetFileSystem();
	EA::WebKit::FileSystem::FileObject fileObject = pFileSystem->CreateFileObject();
	if(pFileSystem->OpenFile(fileObject, mInspectorSettingsPath.c_str(), EA::WebKit::FileSystem::kRead, EA::WebKit::FileSystem::kCDOOpenExisting))
	{
		int64_t numBytes;
		pFileSystem->GetFileSize(mInspectorSettingsPath.c_str(),numBytes);

		EAW_ASSERT_FORMATTED(numBytes < kMaxInspectorFileSize, "InspectorSettings higher than %d bytes - Not loaded",kMaxInspectorFileSize);

		if(numBytes > 0 && numBytes < kMaxInspectorFileSize) 
		{
			eastl::string8 inspectorSettings;
			inspectorSettings.resize(numBytes+1);
			pFileSystem->ReadFile(fileObject, (void*)inspectorSettings.data(),numBytes);
			inspectorSettings[numBytes] = '\0';

			// First we have the version no:
			eastl_size_t versionEnd = inspectorSettings.find(INSPECTOR_SETTINGS_DELIMITER,0);
			if(versionEnd != eastl::string8::npos)
			{
				eastl::string8 versionStr = inspectorSettings.substr(0,versionEnd);
				if(versionStr.compare(sEAWebKitInspectorSettingsFormatVersion) == 0)
				{
					eastl::string8 keyPrefix(sEAWebKitInspectorKey);
					eastl_size_t keyPrefixLength = keyPrefix.length();

					eastl::string8 valuePrefix(sEAWebKitInspectorValue);
					eastl_size_t valuePrefixLength = valuePrefix.length();

					eastl_size_t settingBegin = versionEnd+1;
					eastl_size_t settingEnd = inspectorSettings.find(INSPECTOR_SETTINGS_DELIMITER, settingBegin);
					while(settingEnd != eastl::string8::npos)
					{
						eastl::string8 settingsStr = inspectorSettings.substr(settingBegin, settingEnd-settingBegin);

						eastl_size_t valueLoc = settingsStr.find(valuePrefix,0);
						eastl::string8 value = settingsStr.substr(valueLoc+valuePrefixLength);
						eastl::string8 key = settingsStr.substr(0,valueLoc);
						key = key.substr(keyPrefixLength);

						mSettingsMap.insert(InspectorSettingsMap::value_type(key,value));

						settingBegin = settingEnd+1;
						settingEnd = inspectorSettings.find(INSPECTOR_SETTINGS_DELIMITER, settingBegin);
					}
				}
				else
				{
					EAW_ASSERT_MSG(false, "InspectorSettings version mismatch - Not loaded");
				}
			}

		}
		pFileSystem->CloseFile(fileObject);

	}
	pFileSystem->DestroyFileObject(fileObject);

}

void InspectorSettingsEA::saveSettingsToFile()
{
	if(!mInspectorSettingsPath.empty())
	{
		EA::WebKit::FileSystem* pFileSystem = EA::WebKit::GetFileSystem();
		EA::WebKit::FileSystem::FileObject fileObject = pFileSystem->CreateFileObject();

		if(pFileSystem->OpenFile(fileObject, mInspectorSettingsPath.c_str(), EA::WebKit::FileSystem::kWrite, EA::WebKit::FileSystem::kCDOCreateAlways))
		{
			eastl::string8 versionStr(eastl::string8::CtorSprintf(),"%s%s",sEAWebKitInspectorSettingsFormatVersion,INSPECTOR_SETTINGS_DELIMITER);	
			pFileSystem->WriteFile(fileObject,versionStr.data(),versionStr.size());

			InspectorSettingsMap::iterator iter = mSettingsMap.begin();
			while(iter != mSettingsMap.end())
			{
				eastl::string8 settingStr(eastl::string8::CtorSprintf(),"%s%s%s%s%s",sEAWebKitInspectorKey,iter->first.c_str(),
					sEAWebKitInspectorValue,iter->second.c_str(),
					INSPECTOR_SETTINGS_DELIMITER);
				pFileSystem->WriteFile(fileObject, settingStr.data(),settingStr.size());
				++iter;
			}
			pFileSystem->CloseFile(fileObject);
		}

		pFileSystem->DestroyFileObject(fileObject);

	}

}
}

#endif
