/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VisitedLinkStoreEA_h
#define VisitedLinkStoreEA_h

//+EAWebKitChange
//04/17/15
// took original file from WebKit-r179714\Source\WebKit\win\WebCoreSupport\WebVisitedLinkStore.h
// edited <WebCore/LinkHash.h> and <WebCore/VisitedLinkStore.h> to <LinkHash.h> and <VisitedLinkStore.h>
#include <LinkHash.h>
#include <VisitedLinkStore.h>
//-EAWebKitChange
#include <wtf/PassRef.h>

class VisitedLinkStoreEA final : public WebCore::VisitedLinkStore {
public:
	static Ref<VisitedLinkStoreEA> create();
    virtual ~VisitedLinkStoreEA();

    static void setShouldTrackVisitedLinks(bool);
    static void removeAllVisitedLinks();

    void addVisitedLink(const String& urlString);

private:
	VisitedLinkStoreEA();
    virtual bool isLinkVisited(WebCore::Page&, WebCore::LinkHash, const WebCore::URL& baseURL, const AtomicString& attributeURL) override;
    virtual void addVisitedLink(WebCore::Page&, WebCore::LinkHash) override;

    void populateVisitedLinksIfNeeded(WebCore::Page&);
    void addVisitedLinkHash(WebCore::LinkHash);
    void removeVisitedLinkHashes();

    HashSet<WebCore::LinkHash, WebCore::LinkHashHash> m_visitedLinkHashes;
    bool m_visitedLinksPopulated;
};

#endif // VisitedLinkStoreEA_h
