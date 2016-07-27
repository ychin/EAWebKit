/*
Copyright (C) 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
#ifndef TILE_EA_H
#define TILE_EA_H

#if USE(TILED_BACKING_STORE)

#include "IntPoint.h"
#include "IntRect.h"
#include "Tile.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace EA { namespace WebKit {
    class View;
	class IHardwareRenderer;
    class ISurface;
}}

namespace WebCore 
{
    class TiledBackingStore;

    // This is our Tiled Backing Store implementation. It just uses Cairo to paint into the tiles, and then
    // it uses our IHardwareRenderer interface to send the tiles to the GPU. The WebCore TiledBackingStore class
    // does most of the heavy lifting.
    class TileEA : public Tile 
    {
    public:
        typedef IntPoint Coordinate;

        static PassRefPtr<Tile> create(TiledBackingStore* backingStore, const Coordinate& tileCoordinate, EA::WebKit::View *view) 
		{ 
			return adoptRef(new TileEA(backingStore, tileCoordinate, view)); 
		}
        virtual ~TileEA(void);

        virtual bool isDirty() const OVERRIDE;
        virtual void invalidate(const IntRect&)OVERRIDE;
        virtual Vector<IntRect> updateBackBuffer(void) OVERRIDE;
        virtual void swapBackBufferToFront(void) OVERRIDE;
        virtual bool isReadyToPaint(void) const OVERRIDE;
        virtual void paint(GraphicsContext*, const IntRect&) OVERRIDE;

        virtual const Tile::Coordinate& coordinate(void) const OVERRIDE { return mCoordinate; }
        virtual const IntRect& rect(void) const OVERRIDE{ return mRect; }
        virtual void resize(const WebCore::IntSize&) OVERRIDE;

		EA::WebKit::ISurface* GetSurface() const
		{
			return mSurface;
		}

		void saveToPng(const char8_t *filepath) const;

    private:
        TileEA(TiledBackingStore*, const Coordinate&, EA::WebKit::View*);

        TiledBackingStore* mBackingStore;
        Coordinate mCoordinate;
        IntRect mRect;
        IntRect mDirtyRect;
        EA::WebKit::ISurface *mSurface;
        EA::WebKit::IHardwareRenderer *mRenderer;
		EA::WebKit::View* mView;
    };
}
#endif

#endif
