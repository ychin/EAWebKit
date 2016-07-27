/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies)
 * Copyright (C) 2014 Electronic Arts, Inc. All rights reserved.
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
#include "TextureMapperLayerClientEA.h"

//So WebCore talks to the ChromeClient(EA) which delegates all the functionality here.

#if USE(ACCELERATED_COMPOSITING)

#include "FrameView.h"
#include "GraphicsLayerTextureMapper.h"
#include "WebFrame_p.h"
#include "WebPage_p.h"
#include "TextureMapperLayer.h"
#include "GraphicsContext.h"
#include "PlatformContextCairo.h"
#include "TextureMapperEA.h"

using namespace WebCore;

TextureMapperLayerClientEA::TextureMapperLayerClientEA(EA::WebKit::WebFrame* frame)
    : m_frame(frame)
    , m_rootTextureMapperLayer(0)
	, m_needSync(false)
{
}

TextureMapperLayerClientEA::~TextureMapperLayerClientEA()
{
    m_rootTextureMapperLayer = 0;
}

void TextureMapperLayerClientEA::syncRootLayer()
{
    m_rootGraphicsLayer->flushCompositingStateForThisLayerOnly();
}

void TextureMapperLayerClientEA::markForSync(bool)
{
    m_needSync = true;
}

TextureMapperLayer* TextureMapperLayerClientEA::rootLayer()
{
    return toTextureMapperLayer(m_rootGraphicsLayer.get());
}

void TextureMapperLayerClientEA::setRootGraphicsLayer(GraphicsLayer* layer)
{
    if (layer) {
        m_rootGraphicsLayer = GraphicsLayer::create(0, 0);
        m_rootTextureMapperLayer = toTextureMapperLayer(m_rootGraphicsLayer.get());
        m_rootGraphicsLayer->addChild(layer);
        m_rootGraphicsLayer->setDrawsContent(false);
        m_rootGraphicsLayer->setMasksToBounds(false);
        m_rootGraphicsLayer->setSize(IntSize(1, 1));
        
        if (m_frame->page()->view()->HardwareAccelerated())
			m_textureMapper = TextureMapperEA::create(m_frame->page()->view());
		else
			m_textureMapper = TextureMapper::create(TextureMapper::SoftwareMode);

        m_rootTextureMapperLayer->setTextureMapper(m_textureMapper.get());
        syncRootLayer();
    } else {
        m_rootGraphicsLayer = nullptr;
        m_rootTextureMapperLayer = 0;
    }
}

void TextureMapperLayerClientEA::syncLayers()
{
    if(!m_needSync)
		return;

	if (m_rootGraphicsLayer)
        syncRootLayer();

    m_frame->d->frame->view()->flushCompositingStateIncludingSubframes();

	if(m_rootGraphicsLayer)
		m_needSync = rootLayer()->descendantsOrSelfHaveRunningAnimations();
}

void TextureMapperLayerClientEA::renderCompositedLayers(GraphicsContext* context, const IntRect& clip)
{
	// we would want clip to be bounding region of all dirty rects and not the entire view port.

	if (!m_rootTextureMapperLayer || !m_textureMapper)
        return;

	//Use of this GC is in the TextureMapper implementation layer (so like TexttureMapperImageBuffer/TextureMapperEA). It is not used
	//elsewhere.
    m_textureMapper->setGraphicsContext(context); 
    // GraphicsContext::imageInterpolationQuality is always InterpolationDefault here, do we want to do something
	// interesting here?
    m_textureMapper->setImageInterpolationQuality(context->imageInterpolationQuality());
    m_textureMapper->setTextDrawingMode(context->textDrawingMode());
	//view port size is missing now.
	const WebCore::TransformationMatrix matrix(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	PlatformGraphicsContext* platformContext = context->platformContext();

    //if (m_rootGraphicsLayer->opacity() != painter->opacity() || m_rootGraphicsLayer->transform() != matrix) {
        m_rootGraphicsLayer->setOpacity(platformContext->globalAlpha());
        m_rootGraphicsLayer->setTransform(matrix);
        m_rootGraphicsLayer->flushCompositingStateForThisLayerOnly();
    //}
    m_textureMapper->beginPainting();
    m_textureMapper->beginClip(matrix, clip);
    m_rootTextureMapperLayer->paint();
    m_fpsCounter.updateFPSAndDisplay(m_textureMapper.get(), IntPoint::zero(), matrix);
    m_textureMapper->endClip();
    m_textureMapper->endPainting();

}

#endif
