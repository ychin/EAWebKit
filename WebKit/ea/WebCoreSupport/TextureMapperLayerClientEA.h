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
#ifndef TextureMapperLayerClientEA_h
#define TextureMapperLayerClientEA_h

namespace EA
{
namespace WebKit
{
class WebFrame;
}
}

#include "GraphicsLayer.h"
#include "TextureMapper.h"
#include "TextureMapperFPSCounter.h"
#include "Timer.h"
#include <wtf/OwnPtr.h>

namespace WebCore {

class TextureMapperLayer;

#if USE(ACCELERATED_COMPOSITING)
class TextureMapperLayerClientEA {
public:
    TextureMapperLayerClientEA(EA::WebKit::WebFrame*);
    ~TextureMapperLayerClientEA();
    void syncRootLayer();
    TextureMapperLayer* rootLayer();

    void markForSync(bool scheduleSync);

    void setRootGraphicsLayer(GraphicsLayer*);

    void syncLayers();

    void renderCompositedLayers(GraphicsContext*, const IntRect& clip);
private:
    EA::WebKit::WebFrame* m_frame;
    std::unique_ptr<GraphicsLayer> m_rootGraphicsLayer;
    WebCore::TextureMapperLayer* m_rootTextureMapperLayer;
    OwnPtr<WebCore::TextureMapper> m_textureMapper;
    WebCore::TextureMapperFPSCounter m_fpsCounter;
	bool m_needSync;
};
#endif

}

#endif
