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

#ifndef TEXTUREMAPPER_EA_H
#define TEXTUREMAPPER_EA_H

#include "TextureMapper.h"

#if USE(TEXTURE_MAPPER)

namespace EA
{
namespace WebKit
{
	class View;
	class IHardwareRenderer;
}
}

namespace WebCore {

	
class TextureMapperEA : public TextureMapper {
	WTF_MAKE_FAST_ALLOCATED;
public:

	friend class BitmapTextureEA;
	static std::unique_ptr<TextureMapper> create(EA::WebKit::View* view)
	{
		std::unique_ptr<TextureMapper> tm(new TextureMapperEA(view));
		return tm;
	}

	// TextureMapper implementation
	virtual void drawBorder(const Color&, float borderWidth, const FloatRect&, const TransformationMatrix&) override;
	virtual void drawNumber(int number, const Color&, const FloatPoint&, const TransformationMatrix&) override;
	virtual void drawTexture(const BitmapTexture&, const FloatRect& targetRect, const TransformationMatrix&, float opacity, unsigned exposedEdges) override;
	virtual void drawSolidColor(const FloatRect&, const TransformationMatrix&, const Color&) override;
	virtual void beginClip(const TransformationMatrix&, const FloatRect&) override;
	virtual void bindSurface(BitmapTexture* surface) override;
	virtual void endClip() override; 
	virtual IntRect clipBounds() override;
	virtual IntSize maxTextureSize() const override;
	virtual PassRefPtr<BitmapTexture> createTexture(EA::WebKit::SurfaceType type, const void* data = 0, size_t length = 0) override;
	virtual void beginPainting(PaintFlags = 0) override;
	virtual void endPainting() override;

private:
	struct ClipState {
		IntRect scissorBox;
		int stencilIndex;
		ClipState(const IntRect& scissors = IntRect(), int stencil = 1)
			: scissorBox(scissors)
			, stencilIndex(stencil)
		{ }

		bool equals(const ClipState& other)
		{
			return scissorBox == other.scissorBox && stencilIndex == other.stencilIndex;
		}
	};

	class ClipStack {
	public:
		ClipStack()
			: clipStateDirty(false)
		{ }
		
		void push();
		void pop();
		void apply(EA::WebKit::IHardwareRenderer*);
		void applyIfNeeded(EA::WebKit::IHardwareRenderer*);
		inline ClipState& current() { return clipState; }
		void reset(const IntRect&);
		void intersect(const IntRect&);
		void setStencilIndex(int);
		inline int getStencilIndex() const
		{
			return clipState.stencilIndex;
		}
		inline bool isCurrentScissorBoxEmpty() const
		{
			return clipState.scissorBox.isEmpty();
		}

	private:
		ClipState clipState;
		ClipState lastApplied;
		Vector<ClipState> clipStack;
		bool clipStateDirty;
		IntSize size;
	};
	 
	bool beginScissorClip(const TransformationMatrix&, const FloatRect&);
	void bindDefaultSurface();
	void initializeStencil();
	ClipStack& clipStack();

	TextureMapperEA(EA::WebKit::View* view);
	RefPtr<BitmapTexture> m_currentSurface; //Either null or points to the texture where rendering is currently bound
	EA::WebKit::View* mView;
	EA::WebKit::IHardwareRenderer* mRenderer;
	ClipStack m_clipStack;
	bool m_didModifyStencil;
	bool m_useCustomClip;
};
}
#endif // USE(TEXTURE_MAPPER)
#endif // TEXTUREMAPPER_EA_H

