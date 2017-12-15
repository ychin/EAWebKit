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
#include "FilterOperation.h"

#if USE(TEXTURE_MAPPER)

namespace EA
{
namespace WebKit
{
	class View;
	class ISurface;
	class IHardwareRenderer;
    struct FilterInfo;
    class Filters;
}
}

namespace WebCore {

	
class TextureMapperEA : public TextureMapper {
	WTF_MAKE_FAST_ALLOCATED;
public:
	friend class BitmapTextureEA;
	static PassOwnPtr<TextureMapper> create(EA::WebKit::View* view) { return adoptPtr(new TextureMapperEA(view)); }

	// TextureMapper implementation
	virtual void drawBorder(const Color&, float borderWidth, const FloatRect&, const TransformationMatrix&) OVERRIDE;
	virtual void drawNumber(int number, const Color&, const FloatPoint&, const TransformationMatrix&) OVERRIDE;
	virtual void drawTexture(const BitmapTexture&, const FloatRect& targetRect, const TransformationMatrix&, float opacity, unsigned exposedEdges) OVERRIDE;
	virtual void drawSolidColor(const FloatRect&, const TransformationMatrix&, const Color&) OVERRIDE;
	virtual void beginClip(const TransformationMatrix&, const FloatRect&) OVERRIDE;
	virtual void bindSurface(BitmapTexture* surface) OVERRIDE;
	virtual void endClip() OVERRIDE; 
	virtual IntRect clipBounds() OVERRIDE;
	virtual IntSize maxTextureSize() const;
	virtual PassRefPtr<BitmapTexture> createTexture() OVERRIDE;
	virtual void beginPainting(PaintFlags = 0) OVERRIDE;
	virtual void endPainting() OVERRIDE;

#if ENABLE(CSS_FILTERS)
    void setupFilterInfo(const FilterOperation& source, EA::WebKit::FilterInfo *target);
#endif

private:
	struct ClipState {
		IntRect scissorBox;
		int stencilIndex;
		ClipState(const IntRect& scissors = IntRect(), int stencil = 1)
			: scissorBox(scissors)
			, stencilIndex(stencil)
		{ }
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

};



class BitmapTextureEA : public BitmapTexture {
	friend class TextureMapperEA;
public:
	//static PassRefPtr<BitmapTexture> create() { return adoptRef(new BitmapTextureEA); }
	virtual IntSize size() const OVERRIDE;
	virtual bool isValid() const OVERRIDE;
	virtual bool canReuseWith(const IntSize& contentsSize, Flags = 0) OVERRIDE;
	virtual void didReset() OVERRIDE;
	virtual void updateContents(Image*, const IntRect&, const IntPoint&, UpdateContentsFlag) OVERRIDE;
	virtual void updateContents(TextureMapper*, GraphicsLayer*, const IntRect& target, const IntPoint& offset, UpdateContentsFlag) OVERRIDE;
	virtual void updateContents(const void*, const IntRect& target, const IntPoint& sourceOffset, int bytesPerLine, UpdateContentsFlag) OVERRIDE;
#if ENABLE(CSS_FILTERS)
	virtual PassRefPtr<BitmapTexture> applyFilters(TextureMapper*, const FilterOperations&) OVERRIDE;
	EA::WebKit::Filters *GetFilters(void) const {return mFilterList;}
#endif
	void bind(TextureMapperEA*);
	void initializeStencil();
	EA::WebKit::ISurface *GetSurface(void) const {return mSurface;}
	void saveToPng(const char8_t *filepath) const;

private:
	BitmapTextureEA(EA::WebKit::ISurface *surface, EA::WebKit::View* view);
	~BitmapTextureEA(void);

	GraphicsContext* graphicsContext() { return mContext ? mContext.get() : 0 ; }

	EA::WebKit::View* mView;
	EA::WebKit::ISurface *mSurface;
	EA::WebKit::IHardwareRenderer* mRenderer;

	RefPtr<cairo_surface_t> mCairoSurface;
	RefPtr<cairo_t> mCairoContext;
	OwnPtr<GraphicsContext> mContext;

	TextureMapperEA::ClipStack m_clipStack;
	bool m_shouldClear;
	bool m_boundFirstTime;
	bool m_clearedStencil;

#if ENABLE(CSS_FILTERS)
	EA::WebKit::Filters *mFilterList;
#endif
	
};


}
#endif // USE(TEXTURE_MAPPER)
#endif //TEXTUREMAPPER_EA_H

