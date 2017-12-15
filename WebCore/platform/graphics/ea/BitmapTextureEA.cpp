/*
Copyright (C) 2012, 2013, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

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

#if USE(TEXTURE_MAPPER)
// portions of the code are based on the BitmapTextureGL implementation
#include "BitmapTextureEA.h"
#include "GraphicsLayer.h"
#include <internal/include/EAWebKit_p.h>	//for GetParameters
#include <EAWebKit/EAWebKitView.h>

namespace WebCore {

void BitmapTextureEA::bind(TextureMapperEA* )
{
	mRenderer->SetRenderTarget( static_cast<BitmapTextureEA*>(this)->GetSurface());
	if(m_boundFirstTime || m_shouldClear)
	{
		m_clipStack.reset(IntRect(IntPoint::zero(), m_contentSize));
		m_clipStack.applyIfNeeded(mRenderer);

		mRenderer->Clear(EA::WebKit::ClearColorBit,0,0.0f,0);

		m_boundFirstTime = false;
		m_shouldClear = false;
	}

	m_clipStack.apply(mRenderer);
}
void BitmapTextureEA::updateContents(const void* data, const IntRect& targetRect, const IntPoint& sourceOffset, int bytesPerLine, UpdateContentsFlag)
{
	//currently does not seem to be getting hit
#if PLATFORM(CAIRO)
	RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data(static_cast<unsigned char*>(data()),
		CAIRO_FORMAT_ARGB32,
		targetRect.width(), targetRect.height(),
		bytesPerLine));
	m_image->context()->platformContext()->drawSurfaceToContext(surface.get(), targetRect,
		IntRect(sourceOffset, targetRect.size()), m_image->context());
#else
	ASSERT_NOT_REACHED();
	UNUSED_PARAM(data);
	UNUSED_PARAM(targetRect);
	UNUSED_PARAM(sourceOffset);
	UNUSED_PARAM(bytesPerLine);
#endif
}

void BitmapTextureEA::updateContents(TextureMapper*, GraphicsLayer* sourceLayer, const IntRect& targetRect, const IntPoint& sourceOffset, UpdateContentsFlag)
{
	//This is being called internally and it renders the actual contents in the graphics context
	EA::WebKit::ISurface::SurfaceDescriptor descriptor = {0};
	mSurface->Lock(&descriptor);
		
	IntSize surfaceSize = size() ;

	mCairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)descriptor.mData, CAIRO_FORMAT_ARGB32, surfaceSize.width(), surfaceSize.height(), descriptor.mStride));    
	mCairoContext = adoptRef(cairo_create(mCairoSurface.get()));

	mContext = std::make_unique<GraphicsContext>(mCairoContext.get());

	mContext->clearRect(targetRect);

	IntRect sourceRect(targetRect);
	sourceRect.setLocation(sourceOffset);
	mContext->save();
	mContext->clip(targetRect);
	mContext->translate(targetRect.x() - sourceOffset.x(), targetRect.y() - sourceOffset.y());
	sourceLayer->paintGraphicsLayerContents(*(mContext.get()), sourceRect);
	mContext->restore();

	mSurface->Unlock();
}

void BitmapTextureEA::updateContents(Image* image, const IntRect& targetRect, const IntPoint& offset, UpdateContentsFlag)
{
	//special case for drawing images
	EA::WebKit::ISurface::SurfaceDescriptor descriptor = {0};
	mSurface->Lock(&descriptor);

	IntSize surfaceSize = size() ;

	mCairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)descriptor.mData, CAIRO_FORMAT_ARGB32, surfaceSize.width(), surfaceSize.height(), descriptor.mStride));    
	mCairoContext = adoptRef(cairo_create(mCairoSurface.get()));

	mContext = std::make_unique<GraphicsContext>(mCairoContext.get());
	
	mContext->drawImage(image, ColorSpaceDeviceRGB, targetRect, IntRect(offset, targetRect.size()), ImagePaintingOptions(CompositeCopy));

	mSurface->Unlock();
}

BitmapTextureEA::BitmapTextureEA(EA::WebKit::ISurface *surface, EA::WebKit::View* view)
	: mView(view)
	, mSurface(surface)
	, mRenderer(view->GetHardwareRenderer())
	, mCairoSurface(NULL)
	, mCairoContext(NULL)
	, m_shouldClear(true)
	, m_boundFirstTime(true)
	, m_clearedStencil(false)
    , mFilterList(NULL)
{
	mView->AddTexture(this);
}

BitmapTextureEA::~BitmapTextureEA(void) 
{
	if (mSurface)
	{
		mSurface->Release();
		mSurface = NULL;
	}
    
    if (mFilterList)
    {
        delete mFilterList;
        mFilterList = NULL;
    }

	mView->RemoveTexture(this);
}

IntSize BitmapTextureEA::size(void) const
{
	// Looks like this allows the platform owner to return size greater than the actual content size. For example, next power of two.
	// We don't need that at the moment so we simply return content size. Same as ImageBuffer and our last port.

	int width = 0;
	int height = 0;
	mSurface->GetContentDimensions(&width, &height);

	return IntSize(width, height);
}

bool BitmapTextureEA::isValid(void) const 
{
	// This means didReset was called with a valid size so the texture implementation should be valid at this point
	return mSurface && m_contentSize.width() != 0 && m_contentSize.height() != 0; 
}

bool BitmapTextureEA::canReuseWith(const IntSize& contentsSize, Flags /* = 0 */)
{
	// We ignore flags as all our surfaces have alpha channel.
	return contentsSize == m_contentSize;
}
void BitmapTextureEA::didReset()
{
	m_shouldClear = true;
		
	int width = 0;
	int height = 0;
	mSurface->GetContentDimensions(&width, &height);
	if (m_contentSize.width() != width || m_contentSize.height() != height)
	{
		mSurface->SetContentDimensions(m_contentSize.width(), m_contentSize.height());
	}
}

void BitmapTextureEA::saveToPng(const char8_t *filepath) const
{
	if(mCairoSurface)
		cairo_surface_write_to_png(mCairoSurface.get(), filepath);
}

static void setupFilterInfo(const FilterOperation& source, EA::WebKit::FilterInfo *target)
{
	EA::WebKit::FilterType filterType = EA::WebKit::FilterTypeNone;
	float amount = 0.0f;
	IntPoint location;
	uint32_t color = 0;

	switch (source.type())
	{
	case FilterOperation::GRAYSCALE:
		filterType = EA::WebKit::FilterTypeGrayscale;
		amount = (static_cast<const BasicColorMatrixFilterOperation&>(source)).amount();
		break;
	case FilterOperation::SEPIA:
		filterType = EA::WebKit::FilterTypeSepia;
		amount = (static_cast<const BasicColorMatrixFilterOperation&>(source)).amount();
		break;
	case FilterOperation::SATURATE:
		filterType = EA::WebKit::FilterTypeSaturate;
		amount = (static_cast<const BasicColorMatrixFilterOperation&>(source)).amount();
		break;
	case FilterOperation::HUE_ROTATE:
		filterType = EA::WebKit::FilterTypeHueRotate;
		amount = (static_cast<const BasicColorMatrixFilterOperation&>(source)).amount();
		break;
	case FilterOperation::INVERT:
		filterType = EA::WebKit::FilterTypeInvert;
		amount = (static_cast<const BasicComponentTransferFilterOperation&>(source)).amount();
		break;
	case FilterOperation::OPACITY:
		filterType = EA::WebKit::FilterTypeOpacity;
		amount = (static_cast<const BasicComponentTransferFilterOperation&>(source)).amount();
		break;
	case FilterOperation::BRIGHTNESS:
		filterType = EA::WebKit::FilterTypeBrightness;
		amount = (static_cast<const BasicComponentTransferFilterOperation&>(source)).amount();
		break;
	case FilterOperation::CONTRAST:
		filterType = EA::WebKit::FilterTypeContrast;
		amount = (static_cast<const BasicComponentTransferFilterOperation&>(source)).amount();
		break;
	case FilterOperation::BLUR:
		filterType = EA::WebKit::FilterTypeBlur;
		amount = (static_cast<const BlurFilterOperation&>(source)).stdDeviation().value();
		break;
	case FilterOperation::DROP_SHADOW:
		filterType = EA::WebKit::FilterTypeDropShadow;
		amount = (static_cast<const DropShadowFilterOperation&>(source)).stdDeviation();  //blur size
		color = premultipliedARGBFromColor((static_cast<const DropShadowFilterOperation&>(source)).color());
		location = (static_cast<const DropShadowFilterOperation&>(source)).location();
		break;
	case FilterOperation::NONE:
		filterType = EA::WebKit::FilterTypeNone;
		break;
	default:
		EAW_ASSERT_FORMATTED(false, "Unsupported Filter type - %d", source.type());
	}
	target->filterType = filterType;
	target->amount = amount;
	target->color = color;
	target->location = location;
}

PassRefPtr<BitmapTexture> BitmapTextureEA::applyFilters(TextureMapper* textureMapper, const FilterOperations& filters)
{   
    // early out if filters aren't being done in hardware OR if there are no filters
    if (!EA::WebKit::GetParameters().mDoCssFiltersInHardware || (filters.size() == 0))
    {
        return this;
    }

    //do we need to make a new filter list?
    if (mFilterList == NULL)
    {
        mFilterList = new EA::WebKit::Filters();
    }
    else
    {
        mFilterList->Reset();     //reset the list if we are re-using it from a previous call
    }

    EAW_ASSERT_MSG(filters.size() < mFilterList->CountMax(),"Not enough space in mFilterList to hold all filters.");
    // early out if the assert would have failed, "Not enough space in mFilterList to hold all filters".
    if (filters.size() > mFilterList->CountMax())
    {
        return this;
    }

    for (size_t i = 0; (i < filters.size()) && (i < mFilterList->CountMax()); ++i)
    {
        RefPtr<FilterOperation> filter = filters.operations()[i];   
        EA::WebKit::FilterInfo info;
        setupFilterInfo(*filter, &info);
        mFilterList->Add(info);
	}

    return this;
}


void BitmapTextureEA::initializeStencil()
{
	if(m_clearedStencil)
		return;

	mRenderer->Clear(EA::WebKit::ClearStencilBit,0,0.0,0);
	m_clearedStencil = true; 
}



}
#endif
