/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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
#include "MediaPlayerPrivateEA.h"
#include "TimeRanges.h"
#include "RenderVideo.h"
#include "FrameView.h"
#include "RefPtrCairo.h"
#include "HTMLMediaElement.h"
#include "MediaControls.h"
#include <WebFrame.h>
#include <WebFrame_p.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <internal/include/EAWebKitAssert.h>
#include "PlatformContextCairo.h"
#include <cairo/cairo.h>
using namespace EA::WebKit;

namespace WebCore {

int MediaPlayerPrivateEA::sIdGenerator = 0;    
int MediaPlayerPrivateEA::sRefCount = 0;
MediaUpdateInfo* MediaPlayerPrivateEA::spMediaUpdateInfo = NULL;



// Helper functions for common calls to client.
static void SetUpClientUpdate(MediaUpdateInfo& info, MediaUpdateInfo::UpdateType type, int handleID, EA::WebKit::View* pView, bool isVideo)
{
    info.mUpdateType = type; 
    info.mHandleID = handleID;
    info.mpView = pView;
    info.mpUserData = pView ? pView->GetUserData() : NULL;
    info.mIsMovie = isVideo;

    // Clean up the return values
    info.mReturnTime = 0.f;
    info.mReturnBool = false;
    info.mReturnBytes = 0;
    info.mReturnNaturalSize.mWidth = 0;
    info.mReturnNaturalSize.mHeight = 0;
    info.mReturnMediaState = MediaUpdateInfo::kIdle;
    info.mReturnData = NULL;
}

static void CallClientUpdate(MediaUpdateInfo& info)
{
    if (EAWebKitClient* const pClient = GetEAWebKitClient(info.mpView)) 
         pClient->MediaUpdate(info);         
}

static EA::WebKit::View* GetEAWebKitMediaView(MediaPlayer* pPlayer)
{
    EA::WebKit::View* pView = NULL;

    if (pPlayer)
    {
        FrameView* pFV = pPlayer->frameView();
        if (pFV)
        {    
            const Frame& pFrame = pFV->frame(); 
            const EA::WebKit::WebFrame* pWebFrame = EA::WebKit::WebFramePrivate::kit(&pFrame);
            if (pWebFrame)
            {
                pView = pWebFrame->page()->view();
            }
        }
    
        // If we fail to get the view from the player (might not be set yet by WebKit), we can try to get it from the document directly.
        if (!pView)
        {
            MediaPlayerClient* pMPClient = pPlayer->mediaPlayerClient();
            if ( (pMPClient) && (pMPClient->mediaPlayerOwningDocument()) )
            {
                Document* pDoc = pMPClient->mediaPlayerOwningDocument();
                const Frame* pFrame = pDoc->frame();
                const EA::WebKit::WebFrame* pWebFrame = EA::WebKit::WebFramePrivate::kit(pFrame);
                if (pWebFrame)
                {
                    pView = pWebFrame->page()->view();
                }
            }
        }
    }
    return pView;
}

//--- MediaPlayerPrivateEA Class ---
MediaPlayerPrivateEA::MediaPlayerPrivateEA(MediaPlayer* player)
    : mpWebCorePlayer(player)
    , mpEAWebKitView(NULL)
    , mHandleID(++sIdGenerator)
    , mIsVisible(false)
    , mIsPlaying(false)
    , mIsFinished(false)
    , mIsLooping(false)
    , mIsVideo(false)    
    , mSize()
    , mNaturalSize()
    , mMovieRect()
    , mWindowRect()
    , mReadyState(MediaPlayer::HaveNothing)
    , mNetworkState(MediaPlayer::Empty)
    , mMediaState(EA::WebKit::MediaUpdateInfo::kIdle)
    , mUpdateTimer(this, &MediaPlayerPrivateEA::UpdateTimerFired)
    , mBytesLoadedAtLastDidLoadingProgress(0)
{
    sRefCount++;    // The refcount is used to auto finalize.
    EAW_ASSERT(player);

    // Find out if this media is video.
    const HTMLMediaElement* element = static_cast<HTMLMediaElement*>(player->mediaPlayerClient());
    mIsVideo = element->isVideo();
}

MediaPlayerPrivateEA::~MediaPlayerPrivateEA()
{
    ClientUpdate(MediaUpdateInfo::kDestroy);
    
    mUpdateTimer.stop();
    
    sRefCount--;
    if (!sRefCount)
        Finalize();         
}

MediaUpdateInfo& MediaPlayerPrivateEA::GetMediaUpdateInfo(void)
{
    if (!spMediaUpdateInfo)
    {
        spMediaUpdateInfo = new MediaUpdateInfo;
        EAW_ASSERT(spMediaUpdateInfo);
    }
    return *spMediaUpdateInfo;
}

void MediaPlayerPrivateEA::Finalize(void)
{
    if (spMediaUpdateInfo)
    {
        delete spMediaUpdateInfo;
        spMediaUpdateInfo = NULL;
    }
}

void MediaPlayerPrivateEA::ClientUpdate(MediaUpdateInfo::UpdateType type) const
{
    MediaUpdateInfo& info = GetMediaUpdateInfo();
    
    EA::WebKit::View* pView = mpEAWebKitView;
    // If we don't yet have a view, try to get one now.
    if(!pView)
        pView = GetEAWebKitMediaView(mpWebCorePlayer);
    
    SetUpClientUpdate(info, type, mHandleID, pView, mIsVideo);    
    CallClientUpdate(info); 
}

float MediaPlayerPrivateEA::ClientUpdateAndReturnTime(MediaUpdateInfo::UpdateType type) const
{
    ClientUpdate(type);
    return GetMediaUpdateInfo().mReturnTime;
}

bool MediaPlayerPrivateEA::ClientUpdateAndReturnBool(MediaUpdateInfo::UpdateType type) const
{
    ClientUpdate(type);
    return GetMediaUpdateInfo().mReturnBool;
}

void MediaPlayerPrivateEA::registerMediaEngine(MediaEngineRegistrar registrar)
{
    registrar(create, getSupportedTypes, supportsType, 0, 0, 0);
}

PassOwnPtr<MediaPlayerPrivateInterface> MediaPlayerPrivateEA::create(MediaPlayer* player)
{
    return adoptPtr(new MediaPlayerPrivateEA(player));
}

void MediaPlayerPrivateEA::getSupportedTypes(HashSet<String> &supported)
{
    // Ignore for now as does not seem to be needed to be set. Could tie in with supported EA mime types.
}

MediaPlayer::SupportsType MediaPlayerPrivateEA::supportsType(const String& mime, const String& codec, const URL& url)
{
    MediaUpdateInfo& info = GetMediaUpdateInfo();
    
    // We don't have access to the tag info here but can look at the mime type to eval if we have a video.
    bool isVideo = mime.contains("video");
    SetUpClientUpdate(info, MediaUpdateInfo::kGetIfMimeSupported, 0, 0, isVideo);  
    GetFixedString(info.mMime)->assign(mime.characters(), mime.length());
    GetFixedString(info.mCodec)->assign(codec.characters(), codec.length());
    CallClientUpdate(info);
    if (info.mReturnBool)
    {
        return MediaPlayer::IsSupported;
    }
    else
    {
        return MediaPlayer::IsNotSupported;
    }
}

void MediaPlayerPrivateEA::load(const String& url)
{
    if (!mpEAWebKitView)
        mpEAWebKitView = GetEAWebKitMediaView(mpWebCorePlayer);

    // Note: We get the autoplay and loop flags from the element directly.
    HTMLMediaElement* element = static_cast<HTMLMediaElement*>(mpWebCorePlayer->mediaPlayerClient());
    mIsLooping = element->loop();
   
    MediaUpdateInfo& info = GetMediaUpdateInfo();  
    info.mIsLooping = mIsLooping; 
	WebCore::URL kurl(WebCore::URL(), url);
	String escapedUrl = kurl.protocolIsInHTTPFamily() ? url : decodeURLEscapeSequences(url);

    GetFixedString(info.mURI)->assign(escapedUrl.characters(), escapedUrl.length());
    ClientUpdate(MediaUpdateInfo::kLoad);
    
    // Set info up to play
    setMuted(mpWebCorePlayer->muted());
    setVolume(mpWebCorePlayer->volume());
    
    if(mNetworkState != MediaPlayer::Loading)
    {
        mNetworkState = MediaPlayer::Loading;
        mpWebCorePlayer->networkStateChanged();
    }
    
    if(mReadyState != MediaPlayer::HaveNothing)
    {
       mReadyState = MediaPlayer::HaveNothing; 
       mpWebCorePlayer->readyStateChanged(); 
    }
    
    bool autoPlay = element->autoplay();
    if(autoPlay)
        mpWebCorePlayer->play();            // This just calls our play.

    ClientUpdateStates();                   // This will update the pause/play state right away.

    // Set up an update timer to control the repaint and playback.
    if (mUpdateTimer.isActive())
        mUpdateTimer.stop();
    const double kUpdateTime = 1.0/60.0;      // We want to update at 60hz so we keep up with the render calls. 
    mUpdateTimer.startRepeating(kUpdateTime);
}

void MediaPlayerPrivateEA::cancelLoad()
{
    ClientUpdate(MediaUpdateInfo::kCancelLoad);
    ClientUpdateStates();
}

void MediaPlayerPrivateEA::play() 
{
    if(!mIsPlaying)
    {
        mMediaState = MediaUpdateInfo::kPlaying;
        mIsFinished = false;    
        mIsPlaying = true;
        mMovieRect.setWidth(0);      // Invalidate. This is used to detect window size changes.
        mWindowRect.setWidth(0);
    }
    
    GetMediaUpdateInfo().mIsLooping = mIsLooping;
    ClientUpdate(MediaUpdateInfo::kPlay);
}

void MediaPlayerPrivateEA::pause()  
{
    ClientUpdate(MediaUpdateInfo::kPause);
}

bool MediaPlayerPrivateEA::supportsFullscreen() const
{
    return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetSupportsFullScreen); 
}

IntSize MediaPlayerPrivateEA::naturalSize() const
{
     return mNaturalSize;
}

bool MediaPlayerPrivateEA::hasVideo() const
{
     return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetHasVideo);   
}

bool MediaPlayerPrivateEA::hasAudio() const
{
     return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetHasAudio);   
}

void MediaPlayerPrivateEA::setVisible(bool visible)
{
    // Filter out identical calls.
    if(mIsVisible == visible)
        return;

    if(!mpEAWebKitView)
        mpEAWebKitView = GetEAWebKitMediaView(mpWebCorePlayer);

    mIsVisible = visible;
    
    GetMediaUpdateInfo().mIsVisible = visible;
    ClientUpdate(MediaUpdateInfo::kVisible);  
}

PassRefPtr<TimeRanges> MediaPlayerPrivateEA::buffered() const
{
    float min = ClientUpdateAndReturnTime(MediaUpdateInfo::kGetMinBufferedTime);   
    float max = ClientUpdateAndReturnTime(MediaUpdateInfo::kGetMaxBufferedTime);   
    RefPtr<TimeRanges> buffered = TimeRanges::create(min,max);
    return buffered.release();
}

float MediaPlayerPrivateEA::duration()const
{
    float duration = ClientUpdateAndReturnTime(MediaUpdateInfo::kGetDuration);   
    return duration;   
}

float MediaPlayerPrivateEA::currentTime() const
{
    // If the movie finished, we want to show that the full duration in the progress bar (versus back to 0).
   if(mIsFinished)
        return ClientUpdateAndReturnTime(MediaUpdateInfo::kGetDuration); 
   else
        return ClientUpdateAndReturnTime(MediaUpdateInfo::kGetCurrentTime);       
}

void MediaPlayerPrivateEA::seek(float time)
{
    GetMediaUpdateInfo().mSeek = time;
    ClientUpdate(MediaUpdateInfo::kSeek); 
}

bool MediaPlayerPrivateEA::seeking() const
{
    return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetIsSeeking);       
}

float MediaPlayerPrivateEA::startTime() const
{ 
    return ClientUpdateAndReturnTime(MediaUpdateInfo::kGetStartTime);       
}

double MediaPlayerPrivateEA::initialTime() const
{ 
    return (double) ClientUpdateAndReturnTime(MediaUpdateInfo::kGetInitialTime);           
}

void MediaPlayerPrivateEA::setRate(float rate)
{
    GetMediaUpdateInfo().mRate = rate;
    ClientUpdate(MediaUpdateInfo::kRate);          
}

bool MediaPlayerPrivateEA::paused() const
{ 
    return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetIsPaused);         
}

void MediaPlayerPrivateEA::setVolume(float volume)
{
    GetMediaUpdateInfo().mVolume = volume;
    ClientUpdate(MediaUpdateInfo::kVolume);        
}

bool MediaPlayerPrivateEA::supportsMuting() const
{
    return ClientUpdateAndReturnBool(MediaUpdateInfo::kGetIfSupportsMuting);    
}

void MediaPlayerPrivateEA::setMuted(bool mute)
{
   GetMediaUpdateInfo().mIsMuted = mute;
   ClientUpdate(MediaUpdateInfo::kMute); 
}

MediaPlayer::NetworkState MediaPlayerPrivateEA::networkState() const
{
    return mNetworkState;
}

MediaPlayer::ReadyState MediaPlayerPrivateEA::readyState() const
{
    return mReadyState;
}

float MediaPlayerPrivateEA::maxTimeSeekable() const
{
    return ClientUpdateAndReturnTime(MediaUpdateInfo::kGetMaxTimeSeekable);
}

void MediaPlayerPrivateEA::setSize(const IntSize& size) 
{
    // Filter out duplicate calls.
    if (mSize == size)
        return;
    mSize = size;
}

void MediaPlayerPrivateEA::paint(GraphicsContext* context, const IntRect& r)
{
    if (!context)
        return;

    // Can get a NULL platform context so need to verify.  Seems that UpdateControlTints does what is called a "fake" paint
    // with a null platform context just to get an invalidate.
    PlatformContextCairo* pPlatformContext = context->platformContext();
    if (!pPlatformContext)
        return;

    cairo_t* cr = context->platformContext()->cr();
    if (!cr)
        return;

    MediaUpdateInfo& info = GetMediaUpdateInfo();
    const FrameView* pFV = mpWebCorePlayer->frameView();
    if (!pFV)
        return;

    double x = (double) r.x();
    double y = (double) r.y();
    double w = (double) r.width();
    double h = (double) r.height();
	
	// following is overly complex. It needs to be understood why movie API needs these odd calculations. I arrived at them by mostly creating sample pages to render movie in iframes and scroll extensively. 
	// The graphics context can easily draw the passed in rect (r) without much of these calculations. 
	IntPoint location = pFV->location(); //zero for main frame. Some offset for the iframe. 
	IntSize scrollOffset = pFV->scrollOffset();//scroll offset for the current frame.
	
	ScrollView* pParent = pFV->parent();
	while(pParent)
	{
		scrollOffset +=pParent->scrollOffset(); //take into account scroll offset of any parent frame
		pParent = pParent->parent();
	}
	IntPoint totalOffset(location.x()-scrollOffset.width(),location.y()-scrollOffset.height());
    
	const IntRect rect((int)x + totalOffset.x(), (int) y + totalOffset.y(), (int) w, (int) h);

    // The intersection of frameView contents and the movie is used as clip rect for we just want to know what part of the movie is visible.
    IntRect clip = pFV->windowClipRect(true);
	clip.intersect(rect);
    
    // Find controls intersection
    HTMLMediaElement* element = static_cast<HTMLMediaElement*>(mpWebCorePlayer->mediaPlayerClient());
    if(element && element->controls())
    {
        MediaControls* pControls = element->mediaControls();    
        bool hideControls = pControls->shouldHideControls(); 
        if (!hideControls)
        {
            const int kControlHeight = 16;  // EAWebKitTODO: Consider finding a way to extract this info directly from the controls or pass as a theme param.
            
            LayoutRect boundingRect = pControls->boundingBox();
            x = (double) boundingRect.x();
            y = (double) boundingRect.y();
            w = (double) boundingRect.width();
            h = (double) (boundingRect.height() - kControlHeight);
            
			const IntRect ctrlRect((int) x + totalOffset.x(), (int) y + totalOffset.y(), (int) w, (int) h);

            clip.intersect(ctrlRect);

        }
    }

    if ((rect != mMovieRect) || (clip != mWindowRect))
    {
        mMovieRect = rect;      // Store copy locally to detect changes.
        mWindowRect = clip;

        info.mMovieRect = rect;
        info.mWindowRect = clip;
        ClientUpdate(MediaUpdateInfo::kWindowSize);
    }    
 
    ClientUpdate(MediaUpdateInfo::kPaint);
    if (info.mReturnData)
    {
        // Draw surface to view
#ifndef NDEBUG
        static bool sAssertChecked = false;
        if(!sAssertChecked)
        {
            EAW_ASSERT(!info.mReturnData);   
            sAssertChecked = true;
        }
#endif
        context->save(); 
        RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*) info.mReturnData, CAIRO_FORMAT_ARGB32, w, h, w * sizeof(uint32_t)));
        EAW_ASSERT(cairo_surface_status(cairoSurface.get()) == CAIRO_STATUS_SUCCESS);
        cairo_set_source_surface(cr, cairoSurface.get(), rect.x(), rect.y()); 
        cairo_paint(cr);
        context->restore(); 
    }
    else
    {
        // Draw a default black background.
        context->save(); 
        context->setStrokeStyle(NoStroke);
        context->setFillColor(Color::black, ColorSpaceDeviceRGB);
        context->drawRect(r);    
        context->restore();
    }
}

EA::WebKit::MediaUpdateInfo::MediaState MediaPlayerPrivateEA::ClientMediaState(void) const
{
  ClientUpdate(MediaUpdateInfo::kGetMediaState); 
  return GetMediaUpdateInfo().mReturnMediaState;
}


void MediaPlayerPrivateEA::ClientUpdateStates(void)
{
    mMediaState = ClientMediaState();
    
    MediaPlayer::NetworkState nState = mNetworkState;
    MediaPlayer::ReadyState rState = mReadyState;
    
    switch(mMediaState)
    {
         case MediaUpdateInfo::kHaveMetaData:
            nState = MediaPlayer::Loading;
            rState = MediaPlayer::HaveMetadata;        
            break;
    
         case MediaUpdateInfo::kPaused:
             // Do nothing
             break;

        case MediaUpdateInfo::kFinished:
        case MediaUpdateInfo::kPlaying:
            nState = MediaPlayer::Loading;
            rState = MediaPlayer::HaveEnoughData;        
            break;

        case MediaUpdateInfo::kError:
            nState = MediaPlayer::DecodeError;
            rState = MediaPlayer::HaveNothing;        
            break;
        
        case MediaUpdateInfo::kIdle:
        default:
            nState = MediaPlayer::Idle;
            rState = MediaPlayer::HaveNothing;        
            break;
    }
    
    if (mReadyState != rState)
    {
        mReadyState = rState;
        mpWebCorePlayer->readyStateChanged();
    }

    if (mNetworkState != nState)
    {
        mNetworkState = nState;
        mpWebCorePlayer->networkStateChanged();
    }
}

void MediaPlayerPrivateEA::UpdateTimerFired(Timer<MediaPlayerPrivateEA>*)
{
    // We can use this to control the movie playback and make sure the media player is getting all the info it needs.
    // Currently we just feed it repaint and a pause. This seems to work good enough for basic playback control but
    // we might have to revise this if further synchronization is needed between a movie player and the internal media player system.
    ClientUpdateStates();   

    // Get the natural size here so that we control when it changes (so it does not occur in the paint).
    ClientUpdate(MediaUpdateInfo::kGetNaturalSize);  
    if (mNaturalSize != GetMediaUpdateInfo().mReturnNaturalSize)
    {
        mNaturalSize = GetMediaUpdateInfo().mReturnNaturalSize;  
        mpWebCorePlayer->sizeChanged(); 
    }

    if (mMediaState == MediaUpdateInfo::kFinished)
    {
        mIsFinished = true;
        if(mIsPlaying)    
        {
            mIsPlaying = false;     // This will trigger a play request when calling the play button.

            // Pause if done playing so that the play arrow button shows up for possible replay.
            HTMLMediaElement* element = static_cast<HTMLMediaElement*>(mpWebCorePlayer->mediaPlayerClient());
            element->pause();
        }
    }

    // The movie paint needs to be ticked so that the controls get repainted and the view updated.  
    mpWebCorePlayer->repaint();   
}

bool MediaPlayerPrivateEA::didLoadingProgress() const
{
    ClientUpdate(MediaUpdateInfo::kGetBytesLoaded);
    unsigned int bytesLoaded = GetMediaUpdateInfo().mReturnBytes;

    bool didLoadingProgress = (bytesLoaded != mBytesLoadedAtLastDidLoadingProgress);
    mBytesLoadedAtLastDidLoadingProgress = bytesLoaded;
    return didLoadingProgress;
}

}

