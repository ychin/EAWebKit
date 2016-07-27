/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2014 Electronic Arts, Inc. All rights reserved.
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

#ifndef InspectorClientEA_h
#define InspectorClientEA_h

#include "InspectorClient.h"
#include "InspectorFrontendChannel.h"
#include "InspectorFrontendClientLocal.h"
#include <wtf/Forward.h>
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>


#include <eastl/string.h>
#include <eastl/map.h>

namespace EA { namespace WebKit {
    class WebPage;
    class View;
}}

namespace WebCore {

class InspectorFrontendClientEA;
class Node;
class Page;
class RemoteFrontendChannel;

class InspectorClientEA : public InspectorClient, public InspectorFrontendChannel {
public:
    InspectorClientEA(EA::WebKit::WebPage*);

    virtual void inspectorDestroyed(void) OVERRIDE; 

    virtual WebCore::InspectorFrontendChannel* openInspectorFrontend(WebCore::InspectorController*) OVERRIDE;
    virtual void closeInspectorFrontend() OVERRIDE;
    virtual void bringFrontendToFront() OVERRIDE;

    virtual void highlight(void) OVERRIDE;
    virtual void hideHighlight(void) OVERRIDE;

    virtual bool sendMessageToFrontend(const String&) OVERRIDE;

    void releaseFrontendPage(void);


private:
    EA::WebKit::WebPage* mInspectedPage;
    EA::WebKit::WebPage* mInspectorPage;
    InspectorFrontendClientEA* mInspectorPageClient;
};

class InspectorFrontendClientEA : public InspectorFrontendClientLocal {
public:
    InspectorFrontendClientEA(EA::WebKit::WebPage* inspectedWebPage, EA::WebKit::View *inspectorView, InspectorClientEA* inspectorClient);
    virtual ~InspectorFrontendClientEA(void);

    virtual void frontendLoaded(void) OVERRIDE;
    virtual String localizedStringsURL(void) OVERRIDE;

    virtual void bringToFront(void) OVERRIDE;
    virtual void closeWindow(void) OVERRIDE;

    virtual void attachWindow(DockSide) OVERRIDE;
    virtual void detachWindow(void) OVERRIDE;

	virtual void setAttachedWindowHeight(unsigned height) OVERRIDE;
    virtual void setAttachedWindowWidth(unsigned) OVERRIDE;
    virtual void setToolbarHeight(unsigned) OVERRIDE;

    virtual void inspectedURLChanged(const String& newURL) OVERRIDE;

	void disconnectFromBackend(void);

    void inspectorClientDestroyed(void);

private:
    void updateWindowTitle(void);
    void destroyInspectorView(bool notifyInspectorController);
    
    EA::WebKit::WebPage *mInspectedWebPage;
    EA::WebKit::View *mInspectorView;
    eastl::string16 mInspectedURL;
    bool mDestroyingInspectorView;
    InspectorClientEA* mInspectorClient;
};

// This class is pretty much a no-op but is instantiated at runtime. WebInspector seems like has in-built support for saving settings.
// We can have a InspectorClientEA per view and every view inspected has a separate inspector. But settings are global so
// following is a shared instance.
class InspectorSettingsEA
{
public:
	static InspectorSettingsEA* sharedInstance();
	static void finalize();

	void populateSetting(const String& key, String* value);
	void storeSetting(const String& key, const String& value);


private:
	eastl::string8 mInspectorSettingsPath;
	typedef eastl::map<eastl::string8, eastl::string8> InspectorSettingsMap;
	InspectorSettingsMap mSettingsMap; 

	InspectorSettingsEA();
	~InspectorSettingsEA();
	InspectorSettingsEA(const InspectorSettingsEA&);
	InspectorSettingsEA& operator=(const InspectorSettingsEA&);

	void saveSettingsToFile();
	void readSettingsFromFile();

};

}

#endif
