/*
 * Copyright (C) 2010 codeaurora.org All rights reserved.
 * Copyright (C) 2011 Collabora Ltd.
 * Copyright (C) 2011 Samsung Electronics
 * Copyright (C) 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "TileCairo.h"

//+EAWebKitChange
//8/6/2014 - Disable TileCairo implementation as we use TileEA with GPU compositing
#if USE(TILED_BACKING_STORE) && USE(CAIRO) && !PLATFORM(EA)
//-EAWebKitChange
#include "CairoUtilities.h"
#include "GraphicsContext.h"
#include "PlatformContextCairo.h"
#include "TiledBackingStore.h"
#include "TiledBackingStoreClient.h"
#include <RefPtrCairo.h>

//+EAWebKitChange
//5/1/2014
#if PLATFORM(EA)
#include "Frame.h"
#include "WebFrame_p.h"
#endif
//-EAWebKitChange

namespace WebCore {

TileCairo::TileCairo(TiledBackingStore* backingStore, const Coordinate& tileCoordinate)
    : m_backingStore(backingStore)
    , m_coordinate(tileCoordinate)
    , m_rect(m_backingStore->tileRectForCoordinate(tileCoordinate))
	//+EAWebKitChange
	//5/1/2014
	, m_view(0)
	//-EAWebKitChange
{
    cairo_rectangle_int_t rect = m_rect;
    m_dirtyRegion = adoptRef(cairo_region_create_rectangle(&rect));

	//+EAWebKitChange
	//5/1/2014
#if PLATFORM(EA)
	WebCore::Frame *frame = static_cast<WebCore::Frame*>(backingStore->client());
	EA::WebKit::WebPage *eaPage = EA::WebKit::WebFramePrivate::kit(frame)->page();
	m_view = eaPage->view();

	m_view->AddTile(this);
#endif
	//-EAWebKitChange
}

TileCairo::~TileCairo()
{
	//+EAWebKitChange
	//5/1/2014
#if PLATFORM(EA)
	m_view->RemoveTile(this);
#endif
	//-EAWebKitChange
}

bool TileCairo::isDirty() const
{
    return !cairo_region_is_empty(m_dirtyRegion.get());
}

bool TileCairo::isReadyToPaint() const
{
    return m_buffer;
}
//+EAWebKitChange
//5/1/2014
// Figure out what part of the dirty rect intersects in this tile. Only mark that part dirty. 
//-EAWebKitChange
void TileCairo::invalidate(const IntRect& dirtyRect)
{
    IntRect tileDirtyRect = intersection(dirtyRect, m_rect);
    if (tileDirtyRect.isEmpty())
        return;

    cairo_rectangle_int_t rect = tileDirtyRect;
    cairo_region_union_rectangle(m_dirtyRegion.get(), &rect);
}

Vector<IntRect> TileCairo::updateBackBuffer()
{
    if (m_buffer && !isDirty())
        return Vector<IntRect>();

    if (!m_buffer)
    {
		
		m_buffer = adoptRef(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                       m_backingStore->tileSize().width(),
                                                       m_backingStore->tileSize().height()));
	}

    RefPtr<cairo_t> cr = adoptRef(cairo_create(m_buffer.get()));
    GraphicsContext context(cr.get());
	//+EAWebKitChange
	//5/2/2014
	// Translate the context so that the paint happens starting the surface origin and not in absolute coordinates. If you don't do this, you'll only see the first tile render
	// as rest will get clipped out.  
	//-EAWebKitChange
	context.translate(-m_rect.x(), -m_rect.y());  

    Vector<IntRect> updateRects;
    cairo_rectangle_int_t rect;

    int rectCount = cairo_region_num_rectangles(m_dirtyRegion.get());
    for (int i = 0; i < rectCount; ++i) {
        cairo_region_get_rectangle(m_dirtyRegion.get(), i, &rect);
        updateRects.append(IntRect(rect));

        context.save();
        context.clip(FloatRect(rect));
        context.scale(FloatSize(m_backingStore->contentsScale(), m_backingStore->contentsScale()));
		//+EAWebKitChange
		//5/2/2014
		//Use the FrameView to actually paint content. Equivalent of the frame->render path on non-tiled mode.
		//-EAWebKitChange
		m_backingStore->client()->tiledBackingStorePaint(&context, m_backingStore->mapToContents(rect));  
		context.restore();

		//+EAWebKitChange
		//5/1/2014
		if(m_view->ShouldDrawDebugVisuals())
		{
			context.save();
			context.setStrokeStyle(SolidStroke);

			//Draw the dirty rect in Red color
			context.setStrokeColor(Color(1.0f,0.0f,0.0f,1.0f), ColorSpaceDeviceRGB);
			context.strokeRect(FloatRect(rect.x,rect.y,rect.width,rect.height),2.0f); 

			//Draw the tile boundary in blue color. We don't need to do this per dirty rect but do it here to localize the code. This is debug only code so performance does not matter much.
			context.setStrokeColor(Color(0.0f,0.0f,1.0f,1.0f), ColorSpaceDeviceRGB);
			context.strokeRect(FloatRect(m_rect.x(),m_rect.y(),m_rect.width(),m_rect.height()),2.0f);

			context.restore();
		}
		//-EAWebKitChange
        
    }

	

    m_dirtyRegion.clear();
    m_dirtyRegion = adoptRef(cairo_region_create());

    return updateRects;
}

void TileCairo::swapBackBufferToFront()
{
}

void TileCairo::paint(GraphicsContext* context, const IntRect& rect)
{
    if (!m_buffer)
        return;

    IntRect target = intersection(rect, m_rect);

    cairo_t* cr = context->platformContext()->cr();
    cairo_set_source_surface(cr, m_buffer.get(),m_rect.x(), m_rect.y());
    cairo_rectangle(cr, target.x(), target.y(), target.width(), target.height());
    cairo_fill(cr);
}

void TileCairo::resize(const WebCore::IntSize& newSize)
{
    IntRect oldRect = m_rect;
    m_rect = IntRect(m_rect.location(), newSize);
    if (m_rect.maxX() > oldRect.maxX())
        invalidate(IntRect(oldRect.maxX(), oldRect.y(), m_rect.maxX() - oldRect.maxX(), m_rect.height()));
    if (m_rect.maxY() > oldRect.maxY())
        invalidate(IntRect(oldRect.x(), oldRect.maxY(), m_rect.width(), m_rect.maxY() - oldRect.maxY()));
}

//+EAWebKitChange
//5/2/2014
#if PLATFORM(EA)
void TileCairo::saveToPng(const char8_t *filepath) const
{
	if(m_buffer)
		cairo_surface_write_to_png(m_buffer.get(), filepath);
}
#endif
//-EAWebKitChange

}

#endif
