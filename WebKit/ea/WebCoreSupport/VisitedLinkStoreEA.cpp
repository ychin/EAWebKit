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

//+EAWebKitChange
//04/17/15
// took original file from WebKit-r179714\Source\WebKit\win\WebCoreSupport\WebVisitedLinkStore.cpp
// added "config.h", removed "WebHistory.h" and "WebView.h", edited <WebCore/PageCache.h> to <PageCache.h>
#include "config.h"
#include "VisitedLinkStoreEA.h"
#include <PageCache.h>
//-EAWebKitChange
#include <wtf/NeverDestroyed.h>

using namespace WebCore;

static bool s_shouldTrackVisitedLinks;

static HashSet<VisitedLinkStoreEA*>& visitedLinkStores()
{
	static NeverDestroyed<HashSet<VisitedLinkStoreEA*> > visitedLinkStores;
	return visitedLinkStores;
}

Ref<VisitedLinkStoreEA> VisitedLinkStoreEA::create()
{
	return adoptRef(*new VisitedLinkStoreEA);
}

VisitedLinkStoreEA::VisitedLinkStoreEA()
    : m_visitedLinksPopulated(false)
{
	visitedLinkStores().add(this);
}

VisitedLinkStoreEA::~VisitedLinkStoreEA()
{
	visitedLinkStores().remove(this);
}

void VisitedLinkStoreEA::setShouldTrackVisitedLinks(bool shouldTrackVisitedLinks)
{
    if (s_shouldTrackVisitedLinks == shouldTrackVisitedLinks)
        return;

    s_shouldTrackVisitedLinks = shouldTrackVisitedLinks;
    if (!s_shouldTrackVisitedLinks)
        removeAllVisitedLinks();
}

void VisitedLinkStoreEA::removeAllVisitedLinks()
{
	for (auto& visitedLinkStore : visitedLinkStores())
		visitedLinkStore->removeVisitedLinkHashes();
	PageCache::singleton().markPagesForVisitedLinkStyleRecalc();
}

void VisitedLinkStoreEA::addVisitedLink(const String& urlString)
{
    addVisitedLinkHash(visitedLinkHash(urlString));
}

bool VisitedLinkStoreEA::isLinkVisited(Page& page, LinkHash linkHash, const URL& baseURL, const AtomicString& attributeURL)
{
    populateVisitedLinksIfNeeded(page);

    return m_visitedLinkHashes.contains(linkHash);
}

void VisitedLinkStoreEA::addVisitedLink(Page&, LinkHash linkHash)
{
    if (!s_shouldTrackVisitedLinks)
        return;

    addVisitedLinkHash(linkHash);
}

void VisitedLinkStoreEA::populateVisitedLinksIfNeeded(Page& sourcePage)
{   
    //EAWebKitTODO: not implimented. Not needed right now, only if we keep history too
    /*
    if (m_visitedLinksPopulated)
        return;

    m_visitedLinksPopulated = true;

    WebView* webView = kit(&sourcePage);
    if (!webView)
        return;

    COMPtr<IWebHistoryDelegate> historyDelegate;
    webView->historyDelegate(&historyDelegate);
    if (historyDelegate) {
        historyDelegate->populateVisitedLinksForWebView(webView);
        return;
    }

    WebHistory* history = WebHistory::sharedHistory();
    if (!history)
        return;
    history->addVisitedLinksToVisitedLinkStore(*this);
    */
}

void VisitedLinkStoreEA::addVisitedLinkHash(LinkHash linkHash)
{
    ASSERT(s_shouldTrackVisitedLinks);
    m_visitedLinkHashes.add(linkHash);

    invalidateStylesForLink(linkHash);
    PageCache::singleton().markPagesForVisitedLinkStyleRecalc();
}

void VisitedLinkStoreEA::removeVisitedLinkHashes()
{
    m_visitedLinksPopulated = false;
    if (m_visitedLinkHashes.isEmpty())
        return;
    m_visitedLinkHashes.clear();

    invalidateStylesForAllLinks();
}
