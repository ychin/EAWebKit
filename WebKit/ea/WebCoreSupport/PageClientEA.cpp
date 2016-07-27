/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "PageClientEA.h"

#if USE(ACCELERATED_COMPOSITING)
#include "TextureMapper.h"
#include "TextureMapperLayerClientEA.h"
#include "texmap/TextureMapperPlatformLayer.h"
#endif

#if USE(TILED_BACKING_STORE)
#include "TiledBackingStore.h"
#endif


#include "Page.h"
#include "RenderBlock.h"
#include "RenderView.h"
#include "HTMLNames.h"

#include "WebFrame_p.h"
#include "WebPage.h"
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>

namespace WebCore {

#if USE(TILED_BACKING_STORE)
void PageClientWebView::removeNonVisibleTiles(WebCore::Timer<PageClientWebView>*)
{
	view->Page()->handle()->page->mainFrame().tiledBackingStore()->removeAllNonVisibleTiles();
}
#endif
void PageClientWebView::scroll(int dx, int dy, const WebCore::IntRect& rectToScroll)
{
    view->AddDirtyRegion(rectToScroll);

#if USE(TILED_BACKING_STORE)
	if(view->IsUsingTiledBackingStore()) // We need to invalidate below to deal with the fixed elements on the page. For example, facebook chat panel. // EAWebKitTODO: May be we do this only when there are fixed elements on the page
	{
		IntPoint currentPosition = page->mainFrame()->scrollPosition();
		IntRect r(currentPosition.x() + rectToScroll.x(), currentPosition.y() + rectToScroll.y(), rectToScroll.width(), rectToScroll.height());
		view->Page()->handle()->page->mainFrame().tiledBackingStore()->invalidate(r);
	}
	//EAWebKitTODO - May be resurrect path for removing non-visible tiles 
#endif
}

void PageClientWebView::update(const WebCore::IntRect& dirtyRect)
{
	view->AddDirtyRegion(dirtyRect);
}

void PageClientWebView::repaintViewport()
{
    update(IntRect(IntPoint(0,0), (IntSize)view->GetSize()));
    
}
void PageClientWebView::setInputMethodEnabled(bool enable)
{
}

bool PageClientWebView::inputMethodEnabled() const
{
	return false;
}

PageClientWebView::~PageClientWebView()
{
}

void PageClientWebView::updateCursor(int type)
{
	using namespace EA::WebKit;
    if(EAWebKitClient* const pClient = GetEAWebKitClient(view))
    {
        CursorChangeInfo cci( view, view->GetUserData(), (EA::WebKit::CursorType) type); 
        pClient->CursorChanged(cci);
    }
}


int PageClientWebView::screenNumber() const
{
    return 0;
}

WebCore::IntRect PageClientWebView::geometryRelativeToOwnerWidget() const
{
    return WebCore::IntRect();//eawebkitTODO?
}

WebCore::IntRect PageClientWebView::windowRect() const
{
    return WebCore::IntRect();//eawebkitTODO?
}
} // namespace WebCore
