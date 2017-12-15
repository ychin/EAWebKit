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

#ifndef BITMAP_TEXTURE_EA_H
#define BITMAP_TEXTURE_EA_H

#include "BitmapTexture.h"
#include "TextureMapperEA.h"

#if USE(TEXTURE_MAPPER)

namespace EA
{
namespace WebKit
{
	class View;
	class ISurface;
	class IHardwareRenderer;
    class Filters;
}
}

namespace WebCore {

class BitmapTextureEA : public BitmapTexture {
	friend class TextureMapperEA;
public:
	//static PassRefPtr<BitmapTexture> create() { return adoptRef(new BitmapTextureEA); }
	virtual IntSize size() const override;
	virtual bool isValid() const override;
	virtual bool canReuseWith(const IntSize& contentsSize, Flags = 0) override;
	virtual void didReset() override;
	virtual void updateContents(Image*, const IntRect&, const IntPoint&, UpdateContentsFlag) override;
	virtual void updateContents(TextureMapper*, GraphicsLayer*, const IntRect& target, const IntPoint& offset, UpdateContentsFlag) override;
	virtual void updateContents(const void*, const IntRect& target, const IntPoint& sourceOffset, int bytesPerLine, UpdateContentsFlag) override;

	virtual PassRefPtr<BitmapTexture> applyFilters(TextureMapper*, const FilterOperations&) override;
	EA::WebKit::Filters *GetFilters(void) const {return mFilterList;}

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
	std::unique_ptr<GraphicsContext> mContext;

	TextureMapperEA::ClipStack m_clipStack;
	bool m_shouldClear;
	bool m_boundFirstTime;
	bool m_clearedStencil;

	EA::WebKit::Filters *mFilterList;
	
};


}
#endif // USE(TEXTURE_MAPPER)
#endif //BITMAP_TEXTURE_EA_H

