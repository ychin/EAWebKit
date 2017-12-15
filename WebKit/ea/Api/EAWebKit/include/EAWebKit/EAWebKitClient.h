/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAWebKitClient.h
// By Arpit Baldeva (Refactored Paul Pedriana's code in EAWebKitView.h to a new header)
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITCLIENT_H
#define EAWEBKIT_EAWEBKITCLIENT_H

#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebKitLocalizedStringsEnum.h>
#include <EAWebKit/EAWebkitSTLWrapper.h>
#include <EAWebKit/EAWebkitSystem.h>

namespace EA
{
namespace WebKit
{

class View;
// LocalizedStringInfo
// This allows the user to provide localized strings.
// If any of mString8 or mString16 returns a valid string, that will be used.
// If using mString8, it is supposed to be UTF-8 encoded.
// If none of the mString8/mString16 is valid, default value is used (on Windows only).
struct LocalizedStringInfo
{
	LocalizedStringType		mStringId;// EAWebKit sets the mStringId value, and the app fills in either the 
	char16_t				mString16[256];
	utf8_t					mString8[256];     // mString8 or mString16 string with the localized version of that string.
};

enum CursorType
{
	kCursorIdPointer,
	kCursorIdCross,
	kCursorIdHand,
	kCursorIdIBeam,
	kCursorIdWait,
	kCursorIdHelp,
	kCursorIdEastResize,
	kCursorIdNorthResize,
	kCursorIdNorthEastResize,
	kCursorIdNorthWestResize,
	kCursorIdSouthResize,
	kCursorIdSouthEastResize,
	kCursorIdSouthWestResize,
	kCursorIdWestResize,
	kCursorIdNorthSouthResize,
	kCursorIdEastWestResize,
	kCursorIdNorthEastSouthWestResize,
	kCursorIdNorthWestSouthEastResize,
	kCursorIdColumnResize,
	kCursorIdRowResize,
	kCursorIdMiddlePanning,
	kCursorIdEastPanning,
	kCursorIdNorthPanning,
	kCursorIdNorthEastPanning,
	kCursorIdNorthWestPanning,
	kCursorIdSouthPanning,
	kCursorIdSouthEastPanning,
	kCursorIdSouthWestPanning,
	kCursorIdWestPanning,
	kCursorIdMove,
	kCursorIdVerticalText,
	kCursorIdCell,
	kCursorIdContextMenu,
	kCursorIdAlias,
	kCursorIdProgress,
	kCursorIdNoDrop,
	kCursorIdCopy,
	kCursorIdNone,
	kCursorIdNotAllowed,
	kCursorIdZoomIn,
	kCursorIdZoomOut,
	kCursorIdGrab,
	kCursorIdGrabbing,
	kCursorIdCustom,
	kCursorIdCount  // Always last
};
// CursorChangeInfo
// The user is required to respond to this message if the user wants 
// cursors to change appropriately within the view.
// The implementation of a cursor/mouse must be handled by the application
// and is not implemented by EAWebKit.
struct CursorChangeInfo
{
    View*       mpView;
	void*		mpUserData;
    CursorType  mType;
    CursorChangeInfo(View* pView, void* pUserData, CursorType type) 
		: mpView(pView)
		, mpUserData(pUserData)
		, mType(type) 
	{
	} 
};

// CursorMovedInfo
// When using controller, the location of the cursor can change in response to the button presses. 
// We communicate it separately from CursorChangeInfo since that one is only triggered for changing type of the cursor.
struct CursorMovedInfo
{
	View*       mpView;
	void*		mpUserData;
	int32_t		mX;
	int32_t		mY;
	CursorMovedInfo(View* pView, void* pUserData, int32_t x, int32_t y) 
		: mpView(pView)
		, mpUserData(pUserData)
		, mX(x)
		, mY(y)
	{
	} 
};

enum LoadEventType
{
	kLETNone,
	
	kLETLoadStarted,            
	kLETLoadCommited,
	kLETVisualLayOutAvailable,
	kLETLoadProgressUpdate,    
	kLETLoadCompletedWithoutErrors,          
	kLETLoadCompletedWithErrors,          
	kLETOnLoadEventsHandled,
	kLETJSWindowObjectCleared,

	kLETUrlChanged,
	
	kLETRedirectionReceived,
	kLETResourceResponseReceived,       
	kLETResourceLoadError		

/*
	kLETNone 
		- None.
	kLETLoadStarted 
		- The Webkit has sent a request to the server for a page load. 
	kLETLoadCommited
		- The first chunk of data from the server is available. If the difference between kLETLoadStarted and kLETLoadCommited is high,
		that indicates a server with high latency(which could be due to number of reasons).
	kLETVisualLayOutAvailable
		- For a new page load, something is about to appear on the screen for the first time.
	kLETLoadProgressUpdate
		- Estimated fraction [0.0 .. 1.0] of page load completion. Could be helpful in implementing a loading bar.
	kLETLoadCompletedWithoutErrors
		- The page load completed without any errors.
	kLETLoadCompletedWithErrors
		- The page load completed but with errors. Errors could be fatal or non fatal depending on the application.
	kLETOnLoadEventsHandled
		- The onload events have been handled.
	kLETJSWindowObjectCleared
		- The JavaScript Window Object cleared.
	kLETUrlChanged
		- The URL changed (For example, due to named anchors)
	kLETRedirectionReceived
		- The server has redirected a resource request. LoadInfo::mResourceURL is the original Url, LoadInfo::mStatusCode is redirection code and LoadInfo::mRedirectedURL is the new
		redirected Url. 
	kLETResourceResponseReceived
		- The server has responded to a resource request. You can use LoadInfo::mStatusCode and LoadInfo::mResourceURL to take 
		appropriate action.
	kLETResourceLoadError
		- There is an error in loading a resource. You can use LoadInfo::mLoadErrorType and LoadInfo::mResourceURL to figure out details. 
		Note that getting a 404 from server is not a load error. When a 404 is received, the server still sends the notification message.
		The load errors are identified by LoadErrorType enum below.
*/

};

enum LoadErrorType
{
	kLETNoError								= 0,
	kLETLoadCancelled						= -1,
	kLETCannotShowMIMEType					= -2,
	kLETBlocked								= -3,	//Blocked due to any reason, say using restricted port or not authorized.
	kLETCannotShowURL						= -4,
	kLETFrameLoadInterruptedByPolicyChange	= -5,
	kLETContentNotFoundError				= -6, // similar to HTTP 404 but for plugins only
	KLETPluginWillHandleLoadError			= -7,
	kLETTransport							= -8, //Failed somewhere in transport.
	kLETBadServerResponse					= -9
};

// Sent out for various resources that a page downloads (For example, page HTML/images/css on a given URL are separate resources).
struct LoadInfo
{
	View*						mpView;         
	void*						mpUserData;
	EASTLFixedString16Wrapper   mResourceURL;			// The URL associated with this particular resource. 
	EASTLFixedString16Wrapper   mRedirectedURL;			// The redirected URL when originally fetching mResourceURL.  Only set for kLETRedirectionReceived event. 
	double						mProgressEstimation;	// [0.0 ... 1.0]
	LoadEventType				mLoadEventType;			// The LoadEventType that triggered this update.
	LoadErrorType				mLoadErrorType;			// The LoadErrorType if any. Only set for kLETResourceLoadError event.
	int32_t						mStatusCode;			// HTTP Status Code received from server. Only set for kLETResourceResponseReceived and kLETRedirectionReceived event.
	LoadInfo(View* pView, void* pUserData, LoadEventType let);
};

// Used to notify the application of any error in the Network/Transport layer. Note that this has nothing to do with HTTP return codes
// and their interpretation. For example, HTTP 404 does not mean a network layer error. An example of network layer error is a possible 
// timeout(May be because Internet is not connected or the server itself is down) or DNS lookup failure etc.
enum NetworkErrorType
{
	kNetworkErrorUnknown = 0,
	kNetworkErrorTimeOut,
	kNetworkErrorSSLCert,
	kNetworkErrorSocketLevel
};

struct NetworkErrorInfo
{
	View*				mpView;
	void*				mpUserData;
	const char*			mErrorMessage; //It is not kept around so copy it if you need it.
	NetworkErrorType	mNetworkErrorType;
	int32_t				mNetworkErrorCode;//This is the code that we get from DirtySDK. This would depend on the NetworkErrorType. We just pass it along.
    const char*	        mURI;
	NetworkErrorInfo(NetworkErrorType networkErrorType = kNetworkErrorUnknown)
		: mpView(0)
		, mpUserData(0)
		, mErrorMessage(0)
		, mNetworkErrorType(networkErrorType)
		, mNetworkErrorCode(0)
        , mURI(0)
	{

	}
};

// DebugLogInfo
// Type flags for the log info so it can be filtered if needed
enum DebugLogType
{
	kDebugLogGlobal,
	kDebugLogJavascript,
	kDebugLogAssertion,
	kDebugLogNetwork,
	kDebugLogGraphics,
	kDebugLogMemory,
	kDebugLogWebCore,
	kDebugLogCount,
};

struct DebugLogInfo
{
    // NOTE: For kDebugLogAssertion mpView and mpUserData are NULL.
	View*		mpView;
	void*		mpUserData;
	const char* mpLogText;
	DebugLogType mType;     // Type of debugLog Info.  This can be used to filter or redirect certain logs.
};


// CreateViewInfo structure is used for creating a new view (something equivalent of a new "tab" in the browser) in response to a javascript window.open or
// a user clicks on a link <a href="newpage.htm" target="_blank">New Page</a>. 

// An application can handle it in two ways. 
// 1. Preferred way - The application should simply create a new view (mpCreatedView) and return true. The management of this View and its relation with old View is EAWebKit's responsibility.
// This is how WebCore wants it. However, the display of this View is application's responsibility.
// Note that you should NOT set the URL on the newly created View. EAWebKit will manage that.

// 2. Less preferred but a workaround way - The application may not be in a position to handle multiple view display. In that case, the application may want to do something with 
// the URL (For example, open it in an external browser). Obviously, in such scenario EAWebKit will not be able to maintain any relationship between Views.

struct CreateViewInfo
{
	enum EventType
	{
		kEventNone,
		kEventWindowOpen,	// The notification is sent as a result of window.open(...)
		kEventTargetBlank,	// The notification is coming from <a target="_blank"/>
		kEventInspectorOpen // The notification is sent by the inspector
	};
	View*		mpView;			// The associated View.
	void*		mpUserData;
	View*		mpCreatedView;	// Application needs to create this View and set the pointer.

	EventType	mEventType;
	// Parameters from javascript for the creation of window associated with this View
	uint16_t	mLeft;
	uint16_t	mTop;
	uint16_t	mWidth;
	uint16_t	mHeight;
	bool		mResizable;
	bool		mScrollBars;


	// Please read the comments above. The application needs to use this information only in case if it does not want to 
	// display multiple views and instead implement a workaround.
	const char16_t* mpURLToOpen;      

	CreateViewInfo()
		: mpView(NULL), mpUserData(NULL), mpCreatedView(NULL)
		, mEventType(CreateViewInfo::kEventNone)
		, mLeft(0), mTop(0), mWidth(EA::WebKit::kViewWidthDefault), mHeight(EA::WebKit::kViewHeightDefault)
		, mResizable(true), mScrollBars(true)
		, mpURLToOpen(0)
	{

	}
};

struct DestroyViewInfo
{
	enum EventType
	{
		kEventNone,
		kEventWindowClose,	// The notification is sent as a result of window.close().
		kEventInspectorClose // The notification is sent by the inspector close.
	};
	
	View *mpView;
    void *mpUserData;
    
	EventType	mEventType;

    DestroyViewInfo(void)
    : mpView(NULL)
    , mpUserData(NULL)
	, mEventType(DestroyViewInfo::kEventNone)
    {
        // Do nothing.
    }
};

struct ViewUpdateInfo 
{
    View *mpView;
	void *mpUserData;

    const IntRect *mDirtyRegions;
    int mDirtyRegionCount;

    enum ViewUpdateStage 
    {
        Unknown,
        Begin,
        End,
    };
    ViewUpdateStage mStage;
};

// LinkNotificationInfo
// This is one of the complex callbacks to the application as the execution depends on the type of the LinkNotification.
// This is called from the multiple places in EAWebKit library. 
struct LinkNotificationInfo
{
	enum LinkNotificationType
	{
		kLinkNavigationNone,
		kLinkNavigationBlock,		// If the notification is of this type, you may block navigation by setting mBlockNavigation to true.
		kLinkNavigationModify,		// If the notification is of this type, you may modify navigation by setting mModifiedURI and mModifiedHeaderMap.
		kLinkNavigationFiltered	    // If the notification is of this type, it is read only. This only happens if you set up the library to do domain filtering.
	};

	View* mpView;                 
	void* mpUserData;
	
	EASTLHeaderMapWrapper		mOriginalHeaderMap;	
	EASTLHeaderMapWrapper		mModifiedHeaderMap;
	
	EASTLFixedString16Wrapper	mOriginalURI;           
	EASTLFixedString16Wrapper	mModifiedURI;		
		

	LinkNotificationType		mLinkNotificationType;

	bool						mBlockNavigation;	// Simply set this to true if you are interested in handling navigation yourself.

	bool						mUsePlatformHttp;	// Simply set this to true if you are interested in using platform http layer for this request.

	LinkNotificationInfo()
		: mpView(NULL)
		, mpUserData(NULL)
		, mLinkNotificationType(kLinkNavigationNone)
		, mBlockNavigation(false)
		, mUsePlatformHttp(false)
	{

	}

};


// NavigationUpdateInfo      
// To notify when the status of navigation buttons(Back/Forward) etc. are changed.
struct NavigationActionsInfo
{
	View*							mpView;                 
	void*							mpUserData;
	bool							mCanGoBack;				// The user can use this to enable/disable a visual indicator.
	bool							mCanGoForward;			// The user can use this to enable/disable a visual indicator.
	bool							mCanStop;				// The user can use this to enable/disable a visual indicator.
	bool							mCanReload;				// The user can use this to enable/disable a visual indicator.

	NavigationActionsInfo()
		: mpView(NULL)
		, mpUserData(NULL)
		, mCanGoBack(false)
		, mCanGoForward(false)
		, mCanStop(false)
		, mCanReload(false)
	{

	}
};



// Profiling support - This is mostly for EAWebKit developers but could be useful for integrators.
// VProcessType - Various types of processes/functions to profile
enum VProcessType
{
	kVProcessTypeNone = -1,                 // 
	kVProcessTypeUser1,						// Generic for internal EAWebkit debugging
	kVProcessTypeUser2,						// Generic for internal EAWebkit debugging
	kVProcessTypeUser3,						// Generic for internal EAWebkit debugging

	kVProcessTypeLibTick,					// Tick the library
	kVProcessTypeViewTick,					// Main view update tick
	kVProcessTypeTransportTick,				// Tick the network update
	kVProcessTypeTHJobs,                    // Main job loop for resource handler
	kVProcessTypeTransportJob,              // Single job loop tracking using the transport system
	kVProcessTypeFileCacheJob,              // Single job loop tracking using the file cache system

    kVProcessTypePaint,						// Total Paint for either software/hardware(might not include some special canvas draw calls)
    kVProcessTypeSyncLayers,
    kVProcessTypePaintTilesCPU,				// Paint the content into the tiles which still happens on CPU
    kVProcessTypeHardwareRender,
    kVProcessTypePaintTilesGPU,				// Render the tiles using Hardware renderer/GPU
    kVProcessTypePaintTextureMapper,
    kVProcessTypePaintScrollbars,
    kVProcessTypeLayout,
    kVProcessTypeBeginViewUpdate,
	kVProcessTypeEndViewUpdate,
    kVProcessTypeAnimation,
	kVProcessTypePaintOverlays,
	kVProcessTypeFrameRender,
	kVProcessTypeClearSurface,
	kVProcessTypeDirtyRectsRender,
	kVProcessTypeDrawDebug,
	kVProcessTypeRenderCompLayers,
	kVProcessTypePruneMemCache,
	kVProcessTypeDispatchFunctions,
	kVProcessTypeTickDownload,
	kVProcessTypeFireTimer,

	//****************************************************************//
	//Add any new process types above this line//
	//****************************************************************//
	kVProcessTypeLast						// Keep this at the end for array size
};

// VProcessStatus - Various states a process can go through. For most processes, it would be start and end.
// However, jobs can have more states. To keep it less complicated, we include those states here.
// The queued states tell exactly how long it took for the previous state to finish. However, this does not mean that all the time was spent 
// inside the state machine. This is because a state change can cause some other code to execute and delay the exact change of state. This actually
// works well because this is the type of behavior we are interested in investigating anyway (Which states are taking longer to change and why). 
enum VProcessStatus
{
	//Common process states
	kVProcessStatusNone,
	kVProcessStatusStarted,					// Start of a process or function
	kVProcessStatusEnded,					// End of a process or function

	//Uncommon process states(These are actually specific to Transport jobs)
	kVProcessStatusQueuedToInit,			// This is when the job is queued for Init state in the code.
	kVProcessStatusQueuedToConnection,		// This is when the Init state is finished and the job is queued for the Connection state.
	kVProcessStatusQueuedToTransfer,		// This is when the Connection state is finished and the job is queued for the Transfer state. 
	kVProcessStatusQueuedToDisconnect,      // This is when the Transfer state is finished and the job is queued for the Disconnect state. 
	kVProcessStatusQueuedToShutdown,		// This is when the Disconnect state is finished and the job is queued for the Shutdown state. 
	kVProcessStatusQueuedToRemove			// This is when the Shutdown state is finished and the job is queued for the Remove state. 
};

// ViewProcessInfo
// This is mostly to give user insight of when certain key processes are started and stopped.
// It can be used for profiling for example by timing the start and end status.
// Things like URI and job information are kept in this structure to make things less complicated.
struct ViewProcessInfo
{
	View*                               mpView;             // The associated View.
	void*								mpUserData;

	// Variables
	VProcessType						mProcessType;
	VProcessStatus						mProcessStatus;
	double								mStartTime;			// This is a user controlled workspace clock for timing
	double								mIntermediateTime;	// This is a user controlled workspace clock for timing
	const EASTLFixedString16Wrapper*	mURI;				// The URL associated with the process, if any.
	int									mSize;				// Various usage but mostly return size info
	int									mJobId;				// Job Id of this process, if any.						
	// Constructors
	ViewProcessInfo();
	ViewProcessInfo(VProcessType,VProcessStatus, View* pView = NULL);
	void ResetTime();
};

struct ProcessEventsInfo
{
    View *mpView;       // The view being debugged. Note: This is _not_ the inspector view.
    void *mpUserData;

    View *mpInspectorView;  // The view the inspector is being displayed in.
    void *mpInspectorUserData;
};

// Note by Arpit Baldeva: Use this function to notify an event change in state of a process found in the global array that keeps track of the 
// predefined processes. This is what you would want to use most of the times.
void NOTIFY_PROCESS_STATUS(VProcessType processType, VProcessStatus processStatus, EA::WebKit::View *pView = NULL);
// Note by Arpit Baldeva: Use this function to notify an event change associated with jobs. Each job has a ViewProcessInfo attached with it.
void NOTIFY_PROCESS_STATUS(ViewProcessInfo& process,  VProcessStatus processStatus);

// TextInputStateInfo
// Used to indicate that the TextInput state has changed, such as when a 
// TextInput form control is gains or loses focus activation.
enum KeyboardType
{
	kDefaultKeyBoard = 0,			//eaw-kb-default
	kNumericKeyBoard,				//eaw-kb-numeric
	kJapaneseFullKeyBoard,			//eaw-kb-japanese-full
	kJapaneseHiraganaKeyBoard,		//eaw-kb-japanese-hiragana
	kJapaneseKatakanaKeyBoard,		//eaw-kb-japanese-katakana
	kRussianFullKeyBoard,			//eaw-kb-russian
	kKoreanFullKeyBoard,			//eaw-kb-korean
	kTraditionalChineseFullKeyBoard,//eaw-kb-traditional-chinese
	kSimplifiedChineseKeyBoard,		//eaw-kb-simplified-chinese
	kLatinFullKeyBoard,				//eaw-kb-latin-full
	kURLKeyBoard,					//eaw-kb-url
	kPasswordKeyBoard,				//eaw-kb-password
	kEmailKeyBoard,					//eaw-kb-email
	kGamerTagKeyBoard,				//eaw-kb-gamertag
	kPhoneKeyBoard,					//eaw-kb-phone
	kIPAddressKeyBoard,				//eaw-kb-ipaddress



	//Fill in more keyboard types as needed above this.
	kCountKeyBoardTypes
};

enum InputType //HTML5 text related input types. This is newer than KeyboardType enum above. There is no one to one mapping between two enums.
{
	kInputTypeNone,
	kInputTypeEmail,
	kInputTypeNumber,
	kInputTypeSearch,
	kInputTypeTelephone,
	kInputTypeUrl,
	kInputTypePassword
};

struct TextInputStateInfo
{
	View*           mpView;             // The associated View.
	void*			mpUserData;
	bool            mIsActivated;       // If keyboard input is active = true
	InputType		mInputType;			// This maps to the HTML5 supported input types such as <input type="email" name="email"> 
	TextInputStateInfo() 
		: mpView(0)
		, mpUserData(0)
		, mIsActivated(false)
		, mInputType(EA::WebKit::kInputTypeNone)
	{
	}
};

// ClipboardEventInfo for passing clipboard strings to and from EAWebKit (supported with ctrl-v,ctrl-x,ctrl-c).
// Need to be in a focused node to copy from EAWebKit. Pasting is only allowed inside an editable node (like inside a text input box).
enum ClipBoardEventType
{
    kClipBoardEventNone,                // 
    kClipBoardEventReadPlainText,       // Read from the application clipboard.
    kClipBoardEventWritePlainText       // Write to the application clipboard.
};

struct ClipboardEventInfo
{
	View*                        mpView;        // The associated View when available.  Can be NULL.
	void*		                 mpUserData;    // Returned user data if the view is found.  Can be NULL.
	EASTLFixedString16Wrapper	 mText;         // Used to be written or read depending on the ClipBoardEventType.
	ClipBoardEventType	         mType;         // Event type.  
};


struct ReportJSCallstackInfo
{
    ReportJSCallstackInfo(void)
    : mDepth(0)
    , mMessage(NULL)
    , mNames(NULL)
    , mUrls(NULL)
    , mLines(NULL)
    , mColumn(NULL)
    {
        // Do nothing.
    }
    
    int mDepth;         // Depth of the callstack, there's one entry per level in the below arrays.
    // Don't hold onto these.
    char8_t *mMessage;  // Error message that comes with this callstack
    char8_t **mNames;   // Array of function names.
    char8_t **mUrls;    // Array of source urls for the functions.
    uint32_t *mLines;   // Array of line numbers.
    uint32_t *mColumn;  // Array of column numbers.
};

enum JSMessageBoxType
{
    kJSMessageBoxAlert = 1,   // Just an 'OK' box, no return result.
    kJSMessageBoxConfirm,     // Choose between two options, mConfirm set to true or false.
    kJSMessageBoxPrompt,      // Prompt for a string. mPromptResponse set to the result. mConfirm set to false if cancelled.
};

struct JSMessageBoxInfo
{
    View *mpView;
    void *mpUserData;

	EASTLFixedString16Wrapper mMessage;
	JSMessageBoxType mType;

    EASTLFixedString16Wrapper mPromptDefault;
    EASTLFixedString16Wrapper mPromptResponse;
	bool mConfirm;

};

// Media info: For movies and audio files 
struct MediaUpdateInfo
{
    enum UpdateType
    {
        kNone,                  // Unknown type, never should be set.
        kLoad,                  // Request to load the file mURI and start media decoding. 
        kCancelLoad,            // Cancel load.
        kDestroy,               // End of playback and removal of player. End of any further notifications.       
        kPlay,                  // Start the playback.
        kPause,                 // Pause.   
        kSeek,                  // Seek to time location passed in mSeek.
        kRate,                  // Media playback rate passed in mRate.
        kMute,                  // Mute status set in mIsMuted.
        kVolume,                // Volume set in mVolume.
        kVisible,               // Visible status is returned in mIsVisible.
        kWindowSize,            // The movie location or size changed.  Window is returned in mWindowRect (= clip rect) and the movie location and size in mMediaRect
        kPaint,                 // For draw of a movie inside EAWebKit.  


        kGetIfMimeSupported,    // Set mReturnBool if the passed mime type in mMime is supported.  
        kGetSupportsFullScreen, // Set mReturnBool if full screen is supported.    
        kGetHasVideo,           // Set mReturnBool if the file has video/movie.    
        kGetHasAudio,           // Set mReturnBool if the file has audio.
        kGetIsPaused,           // Set mReturnBool if paused.
        kGetIsSeeking,          // Set mReturnBool if seeking.
        kGetIfSupportsMuting,   // Set mReturnBool if supports muting.
        kGetBytesLoaded,        // Set mReturnBytes with the number of bytes that have been loaded so far. 
        kGetTotalBytes,         // Set mReturnBytes with the total bytes to be loaded.  
        kGetNaturalSize,        // Set mReturnNaturalSize with the natural movie size. This is used take the aspect ratio of the movie in account.           
        kGetMediaState,         // Set mMediaState with the current state.           
        kGetDuration,           // Set mReturnTime with the media duration time (in seconds).
        kGetCurrentTime,        // Set mReturnTime with the media current play time.   
        kGetStartTime,          // Set mReturnTime with the media start time.
        kGetInitialTime,        // Set mReturnTime with the media initial time.
        kGetMinBufferedTime,    // Set mReturnTime with the start of the media buffered time.
        kGetMaxBufferedTime,    // Set mReturnTime with the end of the media buffered time.
        kGetMaxTimeSeekable,    // Set mReturnTime with the max time that is seekable.   
    };

    enum MediaState  { kIdle, kHaveMetaData, kPlaying, kPaused, kFinished, kError };

    // Set by EAWebKit 
    View*                       mpView;                     // Associated view (not always set like for kGetIfMimeSupported).
    void*                       mpUserData;                 // User data from the associated view (not always set like for kGetIfMimeSupported).
    UpdateType                  mUpdateType;                // Type of media update. If the update is of type kGet..., it is asking for a return value.    
    float                       mRate;                      // For kRate: media playback rate
    float                       mSeek;                      // For kSeek: new time to seek to.
    float                       mVolume;                    // For kVolume: current volume level set.
	bool                        mIsMovie;                   // True if using the video tag.  False if audio.  
	bool                        mIsVisible;                 // For kVisible: visible status.
	bool                        mIsMuted;                   // For kMute: set if muted.
    bool                        mIsLooping;                 // For kPlay: set if looping.
    IntRect                     mMovieRect;                 // For kPaint: Unclipped movie location in view space (it could be start outside of the view if scrolled so needs the mClip to determine the displayed region).
    IntRect                     mWindowRect;                // For KPaint: clip region of the movie within the view space (this is the area of the movie that is visible in the view).
    EASTLFixedString16Wrapper	mMime;                      // For kGetIsMimeTypeSupported: sets the mime type to be verified (by mReturIfMimeSupported).
    EASTLFixedString16Wrapper	mCodec;                     // For kGetIsMimeTypeSupported: sets the codec type to be verified if "codecs" was set in the type (by mReturIfMimeSupported).  
    EASTLFixedString16Wrapper	mURI;                       // For kLoad: if kLoad is set, the requested media URL will be set here.   
    int                         mHandleID;                  // Unique ID for media update (set to 0 when kGetIfMimeSupported).

    // Set by the user if requested by a kGet... type.
	IntSize                     mReturnNaturalSize;         // Return the natural size of the movie.    
	float                       mReturnTime;                // Return time requested.
    unsigned int                mReturnBytes;               // Return the bytes requested.
    MediaState                  mReturnMediaState;          // Return the current media state
    void*                       mReturnData;                // Optional - return the movie surface data for drawing inside EAWebKit with a kPaint call. It needs to have the passed mRect width and height.
	bool                        mReturnBool;                // Return if request is true or false.
};


// This is a very critical notification and all the clients should handle it. This notification would be fired if something really unusual happens on the page. Most of the time, you would 
// want to destroy the view (though not in the notification itself) and return the application to safety but it is really up to the client on how to handle this situation(since they have the complete context
// of the page rendered by the view).
// This notification differs from the usual NetworkError/LoadError and DebugLog notification. Most of the times, those errors might be handled by the application/page that is aware of the context (For example, a missing image that may not be important). 
// This notification is designed to put a mechanism in place which does the basic analysis of the view state and fires notification for events that seem really bad regardless of the page context. An unresponsive page stuck in executing a script is one such
// example.

struct WatchDogNotificationInfo
{
	enum WatchDogNotificationType
	{
		kNone,
		kScriptRunningTooLong,		// Notification is sent if a script is running for more than 10 seconds. Currently, timeout is not configurable but can be easily increased by the client by simply not interrupting script. Another notification will be sent after 10 seconds interval. 
		kUnknownTransportHandler,	// Notification is sent if EAWebKit does not understand what to do with a protocol. Considered pretty severe as the impact could be very bad in terms of end user experience. 
		kXSSDetected,				// Notification is send when webcore has detected an XSS attack. The application should most likely exit the EAWebKit flow in this scenario.
		kClientAppFail				// This is a special notification type which can be used by the middleware user's of EAWebKit library to communicate any specific issue to the app. EAWebKit does not use it internally. Do NOT use it for trivial issues.
		// More to come...
	};
	
	View* mpView;                 
	void* mpUserData;
	
	WatchDogNotificationType mWatchDogNotificationType;

	bool mInterruptScript;		//Only valid if kScriptRunningTooLong notification type.
	WatchDogNotificationInfo()
		: mpView(NULL)
		, mpUserData(NULL)
		, mWatchDogNotificationType(kNone)
		, mInterruptScript(true) // By default, we want to interrupt script when it exceeds timeout
	{

	}
};
// The user can provide an instance of this interface to the EAWebKit library. You can think of this instance as a delegate to which EAWebKit
// refers when it needs to interact with the application.
class EAWebKitClient
{
public:
	virtual ~EAWebKitClient() { }

	virtual void GetLocalizedString     (LocalizedStringInfo&)          {   }
	virtual void LoadUpdate				(LoadInfo&)						{	}
	virtual void DebugLog        		(DebugLogInfo&)         		= 0;
	virtual void NetworkError			(NetworkErrorInfo&)				{	}

    // Called for javascript window.open and <a> target="_blank"</a>. The application is responsible for creating the View.
    // Also called when the inspector is opened.
	virtual void CreateView				(CreateViewInfo&)				{	}

    // Called when EAWebkit is done with the specified view. 
    virtual void DestroyView            (DestroyViewInfo&)              {   }
    
	virtual void ViewUpdate             (ViewUpdateInfo&)               {   } // Called only in the software rendering path. In hardware rendering mode, IHardwareRenderer::BeginPainting()/EndPainting() serves similar purpose.
	virtual void CursorChanged          (CursorChangeInfo&)             {   }
	virtual void CursorMoved			(CursorMovedInfo&)				{   }
	virtual void LinkNotification    	(LinkNotificationInfo&) 		{	}   // Called for each link in the page.
	virtual void NavigationActionsUpdate(NavigationActionsInfo&)		{	}
	virtual void ViewProcessStatus		(ViewProcessInfo&)				{	}
	virtual void TextInputState			(TextInputStateInfo&)			{	}
    virtual void ClipboardEvent         (ClipboardEventInfo&)           {   }

    // This function is called so that your application can continue to process events when a
    // Javascript breakpoint has been set. The breakpoint will block on the thread, and continually
    // call this function until javascript execution is resumed.
    // At the VERY LEAST you will need to continue ticking, rendering, and passing input events to
    // the inspector's view in this function.
    virtual void ProcessEvents          (ProcessEventsInfo&)            {   }

    virtual void ReportJSCallstack      (ReportJSCallstackInfo&)        {   }
    virtual void JSMessageBox           (JSMessageBoxInfo&)             {   }
    virtual void MediaUpdate            (MediaUpdateInfo&)              {   }
	virtual void WatchDogNotification	(WatchDogNotificationInfo&)		{	}

};

void			SetEAWebKitClient(EAWebKitClient* pClient);
EAWebKitClient* GetEAWebKitClient(const EA::WebKit::View* pView = NULL);

}
}
#endif
