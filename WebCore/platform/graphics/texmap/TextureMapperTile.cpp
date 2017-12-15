/*
 Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies)
 Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */
#include "config.h"

#if USE(TEXTURE_MAPPER)
#include "TextureMapperTile.h"

#include "Image.h"
#include "TextureMapper.h"
//+EAWebKitChange
//4/28/2015
#include "GraphicsLayer.h"
#include "RenderLayerBacking.h"
#include "HTMLImageElement.h"
//-EAWebKitChange

#include <wtf/RefPtr.h>

namespace WebCore {

class GraphicsLayer;

void TextureMapperTile::updateContents(TextureMapper* textureMapper, Image* image, const IntRect& dirtyRect, BitmapTexture::UpdateContentsFlag updateContentsFlag)
{
    IntRect targetRect = enclosingIntRect(m_rect);
    targetRect.intersect(dirtyRect);
    if (targetRect.isEmpty())
        return;
    IntPoint sourceOffset = targetRect.location();

    // Normalize sourceRect to the buffer's coordinates.
    sourceOffset.move(-dirtyRect.x(), -dirtyRect.y());

    // Normalize targetRect to the texture's coordinates.
    targetRect.move(-m_rect.x(), -m_rect.y());
    if (!m_texture) {
        //+EAWebKitChange
        //4/28/2015
        m_texture = textureMapper->createTexture(EA::WebKit::SurfaceTypeTexture, 0, 0);
        //-EAWebKitChange
        m_texture->reset(targetRect.size(), image->currentFrameKnownToBeOpaque() ? 0 : BitmapTexture::SupportsAlpha);
    }

    m_texture->updateContents(image, targetRect, sourceOffset, updateContentsFlag);
}

//+EAWebKitChange
//4/28/2015
const WTF::String* imageSrcUrl(const GraphicsLayer* layer)
{

    RenderLayerBacking* backing = static_cast<RenderLayerBacking*>(&(layer->client()));
    Element* element = backing->owningLayer().renderer().element();
    if (element && is<HTMLImageElement>(*element))
    {
        const AtomicString& url = downcast<HTMLImageElement>(element)->imageSourceURL();
        return &url.string();
    }
    return nullptr;
}
//-EAWebKitChange


void TextureMapperTile::updateContents(TextureMapper* textureMapper, GraphicsLayer* sourceLayer, const IntRect& dirtyRect, BitmapTexture::UpdateContentsFlag updateContentsFlag)
{
    IntRect targetRect = enclosingIntRect(m_rect);
    targetRect.intersect(dirtyRect);
    if (targetRect.isEmpty())
        return;
    IntPoint sourceOffset = targetRect.location();

    // Normalize targetRect to the texture's coordinates.
    targetRect.move(-m_rect.x(), -m_rect.y());

    //+EAWebKitChange
    //4/28/2015, 05/08/2015
    bool customTexture = false;
    bool srcChanged = false;
    WTF::String textureName;
    const WTF::String* url = imageSrcUrl(sourceLayer);
    const WTF::String kCustomPrefix = String("eawebkit-custom-texture-"); 

    if(url && url->startsWith(kCustomPrefix))
    {
        customTexture = true;
        textureName = url->substring(kCustomPrefix.length());
        if (textureName != m_customTextureName)
        {
            srcChanged = true;
            m_customTextureName = textureName;
        }
    }    

    if (!m_texture || srcChanged) 
    {
        if(customTexture)
            m_texture = textureMapper->createTexture(EA::WebKit::SurfaceTypeCustom, textureName.characters8(), textureName.length());
        else
            m_texture = textureMapper->createTexture(EA::WebKit::SurfaceTypeTexture);
        m_texture->reset(targetRect.size(), BitmapTexture::SupportsAlpha);
    }
    //-EAWebKitChange

    m_texture->updateContents(textureMapper, sourceLayer, targetRect, sourceOffset, updateContentsFlag);
}

void TextureMapperTile::paint(TextureMapper* textureMapper, const TransformationMatrix& transform, float opacity, const unsigned exposedEdges)
{
    if (texture().get())
        textureMapper->drawTexture(*texture().get(), rect(), transform, opacity, exposedEdges);
}

} // namespace WebCore
#endif 
