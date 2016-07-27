/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebPageClientEA_h
#define WebPageClientEA_h

#if USE(ACCELERATED_COMPOSITING)
#include <GraphicsLayer.h>
#endif

#include "IntRect.h"

class WebPageClient {
public:
    WebPageClient(): m_lastCursorType(0) {} 
    virtual ~WebPageClient() {}

	virtual void scroll(int dx, int dy, const WebCore::IntRect&) = 0;
	virtual void update(const WebCore::IntRect&) = 0;
    virtual void setInputMethodEnabled(bool enable) = 0;
    virtual bool inputMethodEnabled() const = 0;
#if USE(ACCELERATED_COMPOSITING)
    virtual void setRootGraphicsLayer(WebCore::GraphicsLayer* layer) { }

    // this gets called when the compositor wants us to sync the layers
    // if scheduleSync is true, we schedule a sync ourselves. otherwise,
    // we wait for the next update and sync the layers then.
    virtual void markForSync(bool scheduleSync = false) {}
#endif
	virtual bool allowsAcceleratedCompositing() const { return false; }

	inline void resetCursor()
    {
        updateCursor(m_lastCursorType);
    }

    inline void setCursor(int type)
    {
        if(m_lastCursorType != type)
        {
            m_lastCursorType = type;
            updateCursor(type);
        }
    }

	virtual int screenNumber() const = 0;
	virtual WebCore::IntRect geometryRelativeToOwnerWidget() const = 0;
    virtual WebCore::IntRect graphicsItemVisibleRect() const { return WebCore::IntRect(); }

    virtual bool viewResizesToContentsEnabled() const = 0;

    virtual WebCore::IntRect windowRect() const = 0;

protected:
    virtual void updateCursor(int type) = 0;

private:
    int m_lastCursorType;       // We store the last cursor to minimize updates.

};

#endif
