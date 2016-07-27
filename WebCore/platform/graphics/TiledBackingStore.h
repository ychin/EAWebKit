/*
 Copyright (C) 2010-2012 Nokia Corporation and/or its subsidiary(-ies)
 Copyright (C) 2012, 2014 Electronic Arts, Inc. All rights reserved.

 
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

#ifndef TiledBackingStore_h
#define TiledBackingStore_h

#if USE(TILED_BACKING_STORE)

#include "FloatPoint.h"
#include "IntPoint.h"
#include "IntRect.h"
#include "Tile.h"
#include "TiledBackingStoreBackend.h"
#include "Timer.h"
#include <wtf/Assertions.h>
#include <wtf/HashMap.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class GraphicsContext;
class TiledBackingStore;
class TiledBackingStoreClient;

class TiledBackingStore {
    WTF_MAKE_NONCOPYABLE(TiledBackingStore); WTF_MAKE_FAST_ALLOCATED;
public:
    TiledBackingStore(TiledBackingStoreClient*, PassOwnPtr<TiledBackingStoreBackend> = TiledBackingStoreBackend::create());
    ~TiledBackingStore();

    //+EAWebKitChange
    //3/22/2012 - 
	// Instead of relying on a timer, we move the logic to create tiles as required for the content and rendering of the content in those tiles to this function. 
	// This can be called by the Paint() function of the EA::WebKit::View which allows a more predictable call location. 
    void updateTilesIfNeeded(void);  
    //-EAWebKitChange

    TiledBackingStoreClient* client() { return m_client; }

    // Used when class methods cannot be called asynchronously by client.
    // Updates of tiles are committed as soon as all the events in event queue have been processed.
    void setCommitTileUpdatesOnIdleEventLoop(bool enable) { m_commitTileUpdatesOnIdleEventLoop = enable; }

    void setTrajectoryVector(const FloatPoint&);
    void coverWithTilesIfNeeded();

    float contentsScale() { return m_contentsScale; }
    void setContentsScale(float);

	//+EAWebKitChange
	//5/1/2014
	//used during pinch/zoom operation. We don't support it.
	//-EAWebKitChange
	bool contentsFrozen() const { return m_contentsFrozen; }
    void setContentsFrozen(bool);

	//+EAWebKitChange
	//5/1/2014
	// render the render tree into tile back buffers. This is called from the updateTilesIfNeeded that we have added. The render tree is rendered inside the GraphicsContext of the tile.
	// The GraphicsContext renders to the surface it is encapsulating which may not necessarily be something on the screen (could be an intermediate buffer or a GPU texture).
	//-EAWebKitChange
	void updateTileBuffers();

	//+EAWebKitChange
	//5/1/2014
	// Invalidate the dirtyRect so that tiled backing store will ask its tiles to render the content in this dirtyRect. dirtyRect is specified in content coordinates. As a general rule, the rendering system works in 
	// content coordinates rather than screen/window coordinates (which actually makes sense). The window rect coordinates are only useful at the client-application interaction level.
	//-EAWebKitChange
	void invalidate(const IntRect& dirtyRect); 

	//+EAWebKitChange
	//5/1/2014
	// The tile buffers have the content that is already rendered. This API is simply asking the content rect to be painted in the provided GraphicsContext. In the simplest term, it is a memcpy from tiles to the GraphicsContext.
	// So depending upon whether you are using CPU/GPU, this could mean different thing.
	// On the CPU path, it current copies the dirty rect back on to the main surface.
	//-EAWebKitChange
    void paint(GraphicsContext*, const IntRect&);

    IntSize tileSize() { return m_tileSize; }
    void setTileSize(const IntSize&);

	//+EAWebKitChange
	//5/1/2014
	//These are usually no-ops for us as we don't support content scaling at the moment. 
	//-EAWebKitChange
	IntRect mapToContents(const IntRect&) const;
    IntRect mapFromContents(const IntRect&) const;

    IntRect tileRectForCoordinate(const Tile::Coordinate&) const;
    Tile::Coordinate tileCoordinateForPoint(const IntPoint&) const;
    double tileDistance(const IntRect& viewport, const Tile::Coordinate&) const;

    IntRect coverRect() const { return m_coverRect; }
    bool visibleAreaIsCovered() const;
    void removeAllNonVisibleTiles();

	//+EAWebKitChange
	//5/1/2014
	//seems still unused - https://bugs.webkit.org/show_bug.cgi?id=78809 
	//-EAWebKitChange
	void setSupportsAlpha(bool);

private:
    void startTileBufferUpdateTimer(); 
    void startBackingStoreUpdateTimer(double = 0);

    void tileBufferUpdateTimerFired(Timer<TiledBackingStore>*);
    void backingStoreUpdateTimerFired(Timer<TiledBackingStore>*);

    void createTiles();
    void computeCoverAndKeepRect(const IntRect& visibleRect, IntRect& coverRect, IntRect& keepRect) const;

    bool isBackingStoreUpdatesSuspended() const;
    bool isTileBufferUpdatesSuspended() const;

    void commitScaleChange();

    bool resizeEdgeTiles();
    void setCoverRect(const IntRect& rect) { m_coverRect = rect; }
    void setKeepRect(const IntRect&);

    PassRefPtr<Tile> tileAt(const Tile::Coordinate&) const;
    void setTile(const Tile::Coordinate& coordinate, PassRefPtr<Tile> tile);
    void removeTile(const Tile::Coordinate& coordinate);

    IntRect visibleRect() const;

    float coverageRatio(const IntRect&) const;
    void adjustForContentsRect(IntRect&) const;

    void paintCheckerPattern(GraphicsContext*, const IntRect&, const Tile::Coordinate&);

private:
    TiledBackingStoreClient* m_client;
    OwnPtr<TiledBackingStoreBackend> m_backend;

    typedef HashMap<Tile::Coordinate, RefPtr<Tile> > TileMap;
    TileMap m_tiles;

    Timer<TiledBackingStore> m_tileBufferUpdateTimer;
    Timer<TiledBackingStore> m_backingStoreUpdateTimer;

    IntSize m_tileSize;
    float m_coverAreaMultiplier;

    FloatPoint m_trajectoryVector;
    FloatPoint m_pendingTrajectoryVector;
    IntRect m_visibleRect;

    IntRect m_coverRect;
    IntRect m_keepRect;
    IntRect m_rect;

    float m_contentsScale;
    float m_pendingScale;

    bool m_commitTileUpdatesOnIdleEventLoop;
    bool m_contentsFrozen;
    bool m_supportsAlpha;
    bool m_pendingTileCreation;
	//+EAWebKitChange
	//3/22/2012
	bool m_useTimers;
	bool m_tilesNeedUpdate;
	bool m_tilesNeedCreation;
	//-EAWebKitChange
    friend class Tile;
};

}

#endif
#endif
