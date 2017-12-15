/*
Copyright (C) 2012, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

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

#include "config.h"

#if USE(TILED_BACKING_STORE)

#include "TileEA.h"

#include "GraphicsContext.h"
#include "TiledBackingStore.h"
#include "TiledBackingStoreClient.h"

#include "Frame.h"
#include "Page.h"
#include "WebPage.h"
#include "WebFrame_p.h"

#include <EAWebkit/EAWebKitHardwareRenderer.h>
#include <EAWebkit/EAWebKitSurface.h>

#include "RefPtrCairo.h"
#include "PlatformContextCairo.h"
#include "TransformationMatrix.h"


namespace WebCore {

TileEA::TileEA(TiledBackingStore* backingStore, const Coordinate& tileCoordinate, EA::WebKit::View *view)
: mBackingStore(backingStore)
, mCoordinate(tileCoordinate)
, mRect(mBackingStore->tileRectForCoordinate(tileCoordinate))
, mDirtyRect(mRect)
, mSurface(NULL)
, mRenderer(view->GetHardwareRenderer())
, mView(view)
{
    mView->AddTile(this);
}

TileEA::~TileEA(void)
{
    if (mSurface)
    {
        mSurface->Release();
        mSurface = NULL;
    }
	mView->RemoveTile(this);
}

bool TileEA::isDirty(void) const
{ 
    return !mDirtyRect.isEmpty();
}

bool TileEA::isReadyToPaint(void) const
{ 
    return mSurface != NULL;
}

void TileEA::invalidate(const IntRect& dirtyRect)
{
    IntRect tileDirtyRect = intersection(dirtyRect, mRect);
    if (!tileDirtyRect.isEmpty())
    {
        mDirtyRect.unite(tileDirtyRect);
    }
}

Vector<IntRect> TileEA::updateBackBuffer(void)
{
    Vector<IntRect> returnRects;

    if (isDirty())
    {
        if (!mSurface)
        {
            mSurface = mRenderer->CreateSurface(EA::WebKit::SurfaceTypeTile);
            mSurface->SetContentDimensions(mRect.width(), mRect.height());
        }

        EA::WebKit::ISurface::SurfaceDescriptor surfaceDescriptor = {0};
        EA::WebKit::IntRect eaRect(0, 0, mRect.width(), mRect.height());
        mSurface->Lock(&surfaceDescriptor, &eaRect);

        RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, eaRect.mSize.mWidth, eaRect.mSize.mHeight, surfaceDescriptor.mStride));    
        RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

        WebCore::GraphicsContext graphicsContext(cairoContext.get());

        graphicsContext.translate(-mRect.x(), -mRect.y());

        graphicsContext.clip(mDirtyRect);
        graphicsContext.scale(FloatSize(mBackingStore->contentsScale(), mBackingStore->contentsScale()));
        mBackingStore->client()->tiledBackingStorePaint(&graphicsContext, mBackingStore->mapToContents(mDirtyRect));

		if(mView->ShouldDrawDebugVisuals())
		{
			graphicsContext.save();

			// dirty rects
			graphicsContext.setStrokeStyle(WebCore::SolidStroke);
			graphicsContext.setStrokeColor(WebCore::Color(1.0f,0.0f,0.0f,1.0f), WebCore::ColorSpaceDeviceRGB);
			graphicsContext.strokeRect(WebCore::FloatRect(mDirtyRect.x(),mDirtyRect.y(),mDirtyRect.width(),mDirtyRect.height()),2.0f); 

			// tiles
			graphicsContext.setStrokeStyle(WebCore::SolidStroke);
			graphicsContext.setStrokeColor(WebCore::Color(0.0f,0.0f,1.0f,1.0f), WebCore::ColorSpaceDeviceRGB);
			graphicsContext.strokeRect(WebCore::FloatRect(mRect.x(),mRect.y(),mRect.width(),mRect.height()),2.0f); 

			graphicsContext.restore();
		}

        mSurface->Unlock();

        returnRects.append(mDirtyRect);
        mDirtyRect = IntRect();
    }

    return returnRects;
}

void TileEA::swapBackBufferToFront(void)
{
    // We don't need this.
}

void TileEA::paint(GraphicsContext* context, const IntRect& rect)
{
    if (mSurface)
    {
        IntRect subRect = intersection(rect, mRect);
        if (!subRect.isEmpty())
        {
            // Draw the entire tile.
            EA::WebKit::FloatRect target(0.0f, 0.0f, mRect.width(), mRect.height());
            EA::WebKit::TransformationMatrix transform(context->getCTM().toTransformationMatrix().translate(mRect.x(), mRect.y()));
            EA::WebKit::Filters filters;
            mRenderer->RenderSurface(mSurface, target, transform, 1.0f, EA::WebKit::CompositeSourceOver, EA::WebKit::ClampToEdge, filters);
        }
    }
}

void TileEA::resize(const IntSize& newSize)
{
    if (mSurface)
    {
        mSurface->SetContentDimensions(newSize.width(), newSize.height());
    }

    mRect = IntRect(mRect.location(), newSize);
    invalidate(mRect);
}

void TiledBackingStoreBackend::paintCheckerPattern(GraphicsContext* context, const FloatRect& target)
{
}

PassRefPtr<Tile> TiledBackingStoreBackend::createTile(TiledBackingStore* backingStore, const Tile::Coordinate& tileCoordinate)
{
    // Looks like the only class that is a client is the Frame. If that changes this code will need to change.
    WebCore::Frame *frame = static_cast<WebCore::Frame*>(backingStore->client());
    EA::WebKit::WebPage *eaPage = EA::WebKit::WebFramePrivate::kit(frame)->page();
    EA::WebKit::View *view = eaPage->view();

    return TileEA::create(backingStore, tileCoordinate, view);
}

void TileEA::saveToPng(const char8_t *filepath) const
{
	int width = 0;
	int height = 0;
	mSurface->GetContentDimensions(&width, &height);

	EA::WebKit::ISurface::SurfaceDescriptor desc = {0};
	mSurface->Lock(&desc);

	cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data((unsigned char*)desc.mData, CAIRO_FORMAT_ARGB32, width, height, desc.mStride);    
	cairo_surface_write_to_png(cairoSurface, filepath);
	cairo_surface_destroy(cairoSurface);

	mSurface->Unlock();
}

}
#endif
