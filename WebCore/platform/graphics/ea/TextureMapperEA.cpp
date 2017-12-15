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
// portions of the code are based on the TextureMapperGL implementation
#include "TextureMapperEA.h"
#include "BitmapTextureEA.h"
#include "FloatQuad.h"
#include "NotImplemented.h" 
#include <EAWebKit/EAWebKitView.h>

// ENABLE_SCISSOR_CLIP is expected to remain 1 all the time as scissors based clipping is much more efficient compared to stencil based
// clipping. The reason for this define is to test stencil clipping path exclusively by turning it off as most of test pages we have get optimized
// by scissor clipping path. 
#define ENABLE_SCISSOR_CLIP 1

namespace WebCore {

IntSize TextureMapperEA::maxTextureSize() const
{
	int32_t maxTextureSize = mRenderer->MaxTextureSize();
	return IntSize(maxTextureSize, maxTextureSize);
}

PassRefPtr<BitmapTexture> TextureMapperEA::createTexture(EA::WebKit::SurfaceType type, const void* data /* = 0 */, size_t length /* = 0 */)
{
	EA::WebKit::ISurface *surface = mRenderer->CreateSurface(type, data, length);
	return adoptRef(new BitmapTextureEA(surface,mView));
}

bool TextureMapperEA::beginScissorClip(const TransformationMatrix& modelViewMatrix, const FloatRect& targetRect)
{
	// 3D transforms are currently not supported in scissor clipping
	// resulting in cropped surfaces when z>0.
	if (!modelViewMatrix.isAffine())
		return false;

	FloatQuad quad = modelViewMatrix.projectQuad(targetRect);
	IntRect rect = quad.enclosingBoundingBox();

	// Only use scissors on rectilinear clips.
	if (!quad.isRectilinear() || rect.isEmpty())
		return false;

	clipStack().intersect(rect);
	clipStack().applyIfNeeded(mRenderer);
	return true;
}

void TextureMapperEA::beginClip(const TransformationMatrix& transform, const FloatRect& rect)
{
	if(m_useCustomClip)
	{
		EA::WebKit::TransformationMatrix eaMatrix(transform);
		EA::WebKit::FloatRect eaTarget(rect);
		mRenderer->BeginClip(eaMatrix,eaTarget);
	}
	else
	{
		clipStack().push();
			 
#if ENABLE_SCISSOR_CLIP
		if(beginScissorClip(transform, rect))
			return;
#endif
		initializeStencil();

		int stencilIndex = clipStack().getStencilIndex();
		EA::WebKit::TransformationMatrix eaMatrix(transform);
		EA::WebKit::FloatRect eaTarget(rect);
		mRenderer->DrawStencil(eaMatrix, eaTarget, stencilIndex);

		clipStack().setStencilIndex(stencilIndex<<1);
		clipStack().applyIfNeeded(mRenderer);
	}
}

void TextureMapperEA::endClip()
{
	if(m_useCustomClip)
	{
		mRenderer->EndClip();
	}
	else
	{
		clipStack().pop();
		clipStack().applyIfNeeded(mRenderer);
	}
}

IntRect TextureMapperEA::clipBounds()
{
	if(m_useCustomClip)
		return mRenderer->CurrentClipBound();
	else
		return clipStack().current().scissorBox;
}

void TextureMapperEA::bindDefaultSurface()
{
	mRenderer->SetRenderTarget(0);
		
	m_clipStack.apply(mRenderer);
	m_currentSurface = nullptr;
}

void TextureMapperEA::bindSurface(BitmapTexture* texture)
{
	if (!texture) {
		bindDefaultSurface();
		return;
	}

	static_cast<BitmapTextureEA*>(texture)->bind(this);
	m_currentSurface = texture;
}

void TextureMapperEA::drawTexture(const BitmapTexture& texture, const FloatRect& targetRect, const TransformationMatrix& matrix, float opacity, unsigned /* exposedEdges */)
{
	if (!texture.isValid()) 
	{
		EAW_ASSERT_MSG(false,"Invalid texture!");
		return;
	}
		
	if (clipStack().isCurrentScissorBoxEmpty())
		return;
		
	const BitmapTextureEA& textureEA = static_cast<const BitmapTextureEA&>(texture);
    EA::WebKit::Filters filters;

    if (textureEA.GetFilters() != NULL)
    {
        filters = *textureEA.GetFilters();
    }

	EA::WebKit::TransformationMatrix eaMatrix(matrix);
	EA::WebKit::FloatRect eaTarget(targetRect);

	EAW_ASSERT_MSG(targetRect.size().width() == texture.size().width() && targetRect.size().height() == texture.size().height(), "We expect to draw full texture always !");
	mRenderer->RenderSurface(textureEA.GetSurface(), 
							eaTarget, 
							eaMatrix, 
							opacity, 
							isInMaskMode() ? EA::WebKit::CompositeDestinationIn : EA::WebKit::CompositeSourceOver, 
							wrapMode() == WebCore::TextureMapper::RepeatWrap ? EA::WebKit::Repeat : EA::WebKit::ClampToEdge,
							filters);
}

void TextureMapperEA::drawSolidColor(const FloatRect& rect, const TransformationMatrix& matrix, const Color& color)
{
	EA::WebKit::TransformationMatrix eaMatrix(matrix);
	EA::WebKit::FloatRect eaTarget(rect);
	mRenderer->FillColor(premultipliedARGBFromColor(color), eaTarget, eaMatrix,isInMaskMode() ? EA::WebKit::CompositeDestinationIn : EA::WebKit::CompositeSourceOver);
}

void TextureMapperEA::drawBorder(const Color& color, float /* borderWidth */, const FloatRect& rect, const TransformationMatrix& matrix)
{
	if (clipStack().isCurrentScissorBoxEmpty())
		return;
		
	EA::WebKit::TransformationMatrix eaMatrix(matrix);
	EA::WebKit::FloatRect eaTarget(rect);
	mRenderer->DrawOutline(premultipliedARGBFromColor(color), eaTarget, eaMatrix);
}

void TextureMapperEA::drawNumber(int /* number */, const Color&, const FloatPoint&, const TransformationMatrix&)
{
	notImplemented();//Need to implement this for debug info
}

TextureMapperEA::TextureMapperEA(EA::WebKit::View* view)
    : TextureMapper()
	, mView(view)
	, mRenderer(view->GetHardwareRenderer())
	, m_didModifyStencil(false)
	, m_useCustomClip(mRenderer->UseCustomClip())
{ }


void TextureMapperEA::ClipStack::reset(const IntRect& rect)
{
	clipStack.clear();
	size = rect.size();
		
	clipState = TextureMapperEA::ClipState(rect);
	lastApplied = TextureMapperEA::ClipState();
	clipStateDirty = true;
}

void TextureMapperEA::ClipStack::intersect(const IntRect& rect)
{
	clipState.scissorBox.intersect(rect);
	clipStateDirty = true;
}

void TextureMapperEA::ClipStack::setStencilIndex(int stencilIndex)
{
	clipState.stencilIndex = stencilIndex;
	clipStateDirty = true;
}

void TextureMapperEA::ClipStack::push()
{
	clipStack.append(clipState);
	clipStateDirty = true;
}

void TextureMapperEA::ClipStack::pop()
{
	if (clipStack.isEmpty())
		return;
	clipState = clipStack.last();
	clipStack.removeLast();
	clipStateDirty = true;
}
void TextureMapperEA::ClipStack::apply(EA::WebKit::IHardwareRenderer* renderer)
{
	if(renderer->UseCustomClip())
		return;
	
	if (clipState.scissorBox.isEmpty())
		return;

	if(!lastApplied.equals(clipState))
	{
#if ENABLE_SCISSOR_CLIP	
		EA::WebKit::IntRect scissorRect(clipState.scissorBox.x(), clipState.scissorBox.y(),
			clipState.scissorBox.width(), clipState.scissorBox.height());

		renderer->ScissorClip(scissorRect);
#endif

		renderer->ClipAgainstStencil(clipState.stencilIndex);

		lastApplied = clipState;
	}

}

void TextureMapperEA::ClipStack::applyIfNeeded(EA::WebKit::IHardwareRenderer* renderer)
{
	if(renderer->UseCustomClip())
		return;
	
	if (!clipStateDirty)
		return;

	clipStateDirty = false;
	apply(renderer);
}

void TextureMapperEA::initializeStencil()
{
	if (m_currentSurface) 
	{
		static_cast<BitmapTextureEA*>(m_currentSurface.get())->initializeStencil();
		return;
	}

	if (m_didModifyStencil) //once per frame
		return;

	mRenderer->Clear(EA::WebKit::ClearStencilBit, 0, 0.0f,0);
		
	m_didModifyStencil = true;
}

TextureMapperEA::ClipStack& TextureMapperEA::clipStack()
{
	return m_currentSurface ? static_cast<BitmapTextureEA*>(m_currentSurface.get())->m_clipStack : m_clipStack;
}

void TextureMapperEA::beginPainting(PaintFlags flags)
{
	m_didModifyStencil = false;
	m_clipStack.reset(IntRect(IntPoint(0, 0), WebCore::IntSize(mView->GetSize())));
	// Following binding is not necessary as the m_rootTextureMapperLayer->paint() right after this binds the default surface.
	// bindSurface(0); 
}

void TextureMapperEA::endPainting()
{
	if(m_didModifyStencil)
	{
		mRenderer->Clear(EA::WebKit::ClearStencilBit, 0, 0.0f,1);
	}
}

}
#endif
