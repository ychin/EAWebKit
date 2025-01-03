/*
 Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
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
#include "TextureMapper.h"

#include "BitmapTexturePool.h"
#include "FilterOperations.h"
#include "GraphicsLayer.h"
#include "Timer.h"
#include <wtf/CurrentTime.h>

#if USE(TEXTURE_MAPPER)

namespace WebCore {

PassRefPtr<BitmapTexture> TextureMapper::acquireTextureFromPool(const IntSize& size, const BitmapTexture::Flags flags)
{
    RefPtr<BitmapTexture> selectedTexture = m_texturePool->acquireTexture(size);
    selectedTexture->reset(size, flags);
    return selectedTexture.release();
}

std::unique_ptr<TextureMapper> TextureMapper::create()
{
    return platformCreateAccelerated();
}

TextureMapper::TextureMapper()
    : m_context(0)
//+EAWebKitChange
//10/06/2015 construct the m_texturePool too
#if PLATFORM(EA)    
	, m_texturePool(std::make_unique<BitmapTexturePool>(this))
#endif
//-EAWebKitChange
    , m_interpolationQuality(InterpolationDefault)
    , m_textDrawingMode(TextModeFill)
    , m_isMaskMode(false)
    , m_wrapMode(StretchWrap)
{ }

TextureMapper::~TextureMapper()
{ }

} // namespace

#endif
