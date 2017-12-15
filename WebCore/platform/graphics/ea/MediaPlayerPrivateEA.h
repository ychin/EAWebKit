/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2011, 2012, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

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

#ifndef MediaPlayerPrivateEA_h
#define MediaPlayerPrivateEA_h

#include "MediaPlayerPrivate.h"
#include <EAWebKit/EAWebKitClient.h>
#include "MediaPlayer.h"

namespace EA
{
    namespace WebKit
    {
        class View;        
    }
}

namespace WebCore
{

class IntRect;
class IntSize;




class MediaPlayerPrivateEA : public MediaPlayerPrivateInterface
{
public:
	explicit MediaPlayerPrivateEA(MediaPlayer* player);
    ~MediaPlayerPrivateEA();

    // For setting up the engine
    static void registerMediaEngine(MediaEngineRegistrar);
    static void getSupportedTypes(HashSet<String> &supported);
	static MediaPlayer::SupportsType supportsType(const MediaEngineSupportParameters& parameters);

    // For playing the media
    virtual void load(const String& url) override;
    virtual void cancelLoad() override;
    virtual void play() override; 
    virtual void pause() override;    
    virtual bool supportsFullscreen() const override;
    virtual FloatSize naturalSize() const override;
    virtual bool hasVideo() const override;
    virtual bool hasAudio() const override;
    virtual void setVisible(bool) override;
    virtual float duration() const override;
    virtual float currentTime() const override;
    virtual void seek(float time) override;
    virtual bool seeking() const override;
    virtual void setRate(float) override;
    virtual bool paused() const override;
    virtual void setVolume(float) override;
    virtual bool supportsMuting() const override;
    virtual void setMuted(bool) override;
    virtual MediaPlayer::NetworkState networkState() const override; 
    virtual MediaPlayer::ReadyState readyState() const override; 
    virtual float maxTimeSeekable() const override;
    virtual std::unique_ptr<PlatformTimeRanges> buffered() const override; 
    virtual void setSize(const IntSize& ) override;
    virtual void paint(GraphicsContext*, const FloatRect&) override;

    // Ignore these for now as unclear if needed.
    virtual void prepareToPlay() override { }
    virtual void setPreservesPitch(bool) override { }
    virtual bool hasClosedCaptions() const override { return false; }    
    virtual void setClosedCaptionsVisible(bool) override { }
    
    void UpdateTimerFired();
    virtual bool didLoadingProgress() const override;

private: 
    static EA::WebKit::MediaUpdateInfo& GetMediaUpdateInfo(void);
    static void Finalize(void);
    void ClientUpdate(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    bool ClientUpdateAndReturnBool(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    float ClientUpdateAndReturnTime(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    EA::WebKit::MediaUpdateInfo::MediaState ClientMediaState(void) const;
    void ClientUpdateStates(void);


    MediaPlayer*                mpWebCorePlayer;
    EA::WebKit::View*           mpEAWebKitView;
    int                         mHandleID;          // Unique handle ID
    bool                        mIsVisible;         // Store to avoid repetitive calls to client
    bool                        mIsPlaying;         // Store if play was sent
    bool                        mIsFinished;        // Media finished playing.    
    bool                        mIsLooping;         // Store if looping set
    bool                        mIsVideo;           // Set if using the video tag.
    IntSize                     mSize;              // The movie rect size.
    FloatSize                   mNaturalSize;       // The natural movie size (used for aspect ratio) 
    IntRect                     mMovieRect;         // The last movie position used to detect changes for client notification.
    IntRect                     mWindowRect;        // The last movie window position (clip rect) used to detect changes for client notification.    
    MediaPlayer::ReadyState     mReadyState;        // Current ready state.    
    MediaPlayer::NetworkState   mNetworkState;      // Current network state.
    EA::WebKit::MediaUpdateInfo::MediaState mMediaState; // Current media state.    
    Timer mUpdateTimer;       // Used to control the repaint and check on status.

    mutable unsigned int mBytesLoadedAtLastDidLoadingProgress;

    static int sIdGenerator;                        // Used to assign unique handle ID's to each movie instance.
    static int sRefCount;                           // For auto cleanup
    static EA::WebKit::MediaUpdateInfo* spMediaUpdateInfo;    // A shared media info struct for callback updates 
};


} // WebCore


#endif // MediaPlayerPrivateEA_h
