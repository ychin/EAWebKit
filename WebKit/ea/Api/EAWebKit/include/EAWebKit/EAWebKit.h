/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKit.h
// By Paul Pedriana
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKIT_H
#define EAWEBKIT_EAWEBKIT_H

#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebKitSystem.h>
#include <EAWebKit/EAWebKitDll.h>
namespace EA
{
namespace WebKit
{
struct PlatformSocketAPI;

enum WebKitStatus
{
    kWebKitStatusInactive,
    kWebKitStatusInitializing,
    kWebKitStatusActive,
    kWebKitStatusShuttingDown
};

struct RAMCacheInfo
{
	// Having a larger RAM cache allows for better performance (like when scrolling or switching pages)
	/* abaldeva
	In order to understand this API, it is important to understand terminology associated with it. 
	Resource - An asset like image, script etc.
	Resource memory - Divided in 3 parts
		Source size - This is the size of your resource which is often much smaller for an encoded resource such as jpg/png image. This is the data downloaded from the server.
		Decoded size - This is the size of your resource when decoded so that it can be used by the render engine. For images, it would be fully decoded ARGB pixels. This is the heaviest part of the memory.
		Overhead size - The overhead needed by the Cache system. Reasonably small but grows with number of resources in use.
	Live resource - A resource that is currently referenced on a page. In WebCore terms, it currently has clients.
	Dead resource - A resource that is currently not referenced on a page. In WebCore terms, it currently has no clients. However, the resource can become a live resource again due to user action such as pressing the back button to go to a previous page/screen. Note that an off-viewport image is not a dead resource.
	Cache capacity - Total memory size serving as a goal to for the cache system. It is very important to understand that this is not an absolute limit. While it may sound odd, it is easy to understand. If you set the cache to 2 MB but have images worth 10 MB source data on your page, those images will still be downloaded and cause the cache system to report 10 MB memory usage. And any image that is drawn on the screen will cause the memory usage to grow even further. However, you can use RAMCacheInfo structure to control the pruning of the decoded data. Because once the image is drawn on the screen, the decoded data is no longer required. However, if a subsequent user action causes the image to be redrawn (for example, scrolling), if the decoded data is not in the cached memory, it will have to be decoded again and redrawn. So you’d want to tune your cache to provide a proper balance between memory and performance. This also depends on your page layout.
	Dead capacity - The upper limit of memory size devoted to keeping dead resources in memory. It may not always be a best choice to keep this capacity to 0. If your UI layout causes user to frequently move between different screens, you may want to keep dead capacity at a reasonable level in order to make the experience smoother. If the dead capacity is set to 0, the source data of the resource is deleted as well. This will causes the resource to be re-downloaded in case it is needed.
	
	From WebCore - MemoryCache class
	//  - minDeadBytes: The maximum number of bytes that dead resources should consume when the cache is under pressure.
	//  - maxDeadBytes: The maximum number of bytes that dead resources should consume when the cache is not under pressure.
	//  - totalBytes: The maximum number of bytes that the cache should consume overall.
	So sounds like the members are not named intuitively. minDeadBytes should be thought as mMaxDeadBytesUnderPressure and maxDeadBytes should be thought as
	mMaxDeadBytesNoPressure
	*/

    uint32_t    mTotalBytes;    // Total number of bytes to be used for the RAM cache.  
    uint32_t    mMaxDeadBytes;  // Max dead resources allowed to be stored in the cache.
	double		mDeadDecodedDataDeletionInterval; // In seconds. 0.0 by default (which means never). The interval value for cache can be used judiciously. If you set it say 30 seconds, you can keep some dead data and allow the user to switch between pages very quickly. But once you see that the user is not switching frequently(30 seconds have elapsed), you claim that memory automatically. The regular prune process does not claim the dead data if not overflowing the dead capacity preference. 
	// mDeadDecodedDataDeletionInterval is useless if you set mMaxDeadBytes to 0 as dead data is claimed right away.

	RAMCacheInfo()
		: mTotalBytes(8 * 1024 * 1024)
        , mMaxDeadBytes(8/4 * 1024 * 1024)
		, mDeadDecodedDataDeletionInterval(0.0)
	{
	}

	RAMCacheInfo(uint32_t totalBytes, uint32_t maxDeadBytes, double deadDecodedDataDeletionInterval)
        : mTotalBytes(totalBytes)
        , mMaxDeadBytes(maxDeadBytes)
		, mDeadDecodedDataDeletionInterval(deadDecodedDataDeletionInterval)
	{
	}
};

struct RAMCacheUsageInfo
{
	uint32_t mImagesBytes;			// Current RAM Cache memory used for images
	uint32_t mCssStyleSheetsBytes;	// Current RAM Cache memory used for css style sheets
	uint32_t mScriptsBytes;			// Current RAM Cache memory used for scripts
	uint32_t mFontsBytes;			// Current RAM Cache memory used for fonts. This is not same as cached glyphs that the text system may keep. 

	RAMCacheUsageInfo()
		: mImagesBytes(0)
		, mCssStyleSheetsBytes(0)
		, mScriptsBytes(0)
		, mFontsBytes(0)
	{

	}

};

enum MemoryCacheClearFlags
{
    MemoryCacheClearScriptBit       = 1 << 0, // Clear compiled Script code that holds references to many other elements (meaning lot of memory can't be freed until this is cleared).
    MemoryCacheClearResourcesBit    = 1 << 1, // Clear resources such as images, css etc. 
    //...
    // Any future entries go here
    MemoryCacheClearAll		        = -1      // Clear everything.
};

struct DiskCacheInfo
{
	uint32_t		mDiskCacheSize;				// In bytes. No new files are cached once this limit is reached.
	uint32_t		mMaxNumberOfOpenFiles;      // Max number of file handles the disk cache system can open concurrently. This can help application manage the file handles available by OS. You want this number to be a minimum of 3 (1 FileCache.ini, 1 cache file write operation and 1 cache file read operation). Something like 16/24 will be a more reasonable number performance wise. Note that the handles are not reserved in advance so a higher max number does not impose any penalty.   
	const utf8_t*	mDiskCacheDirectory;		// Full/Relative file path to writable directory. If relative, a valid full path should be returned when calling FileSystem::GetBaseDirectory(). SetDiskCacheUsage copies this string.

	DiskCacheInfo()
		: mDiskCacheSize(256 * 1024 * 1024) // Max size that the file cache can use
		, mMaxNumberOfOpenFiles(24)         // Max number of files that can stay open
		, mDiskCacheDirectory(0)            // Path to the file cache
	{
	}

	DiskCacheInfo(uint32_t diskCacheSize, const utf8_t* cacheDiskDirectory, uint32_t maxNumberOfOpenFiles = 24)
		: mDiskCacheSize(diskCacheSize)
		, mMaxNumberOfOpenFiles(maxNumberOfOpenFiles)
		, mDiskCacheDirectory(cacheDiskDirectory)
	{
	}

};

struct DiskCacheUsageInfo
{
	uint32_t		mCurrentNumberOfCachedFiles;    // Current number of files that are cached in the cache directory
	uint32_t		mCurrentNumberOfOpenFiles;		// Current number of files that are open 

	DiskCacheUsageInfo()
		: mCurrentNumberOfCachedFiles(0)
		, mCurrentNumberOfOpenFiles(0)
	{

	}
};
struct CookieInfo
{
	uint32_t		mMaxIndividualCookieSize;	// Should usually be at least 4096. The usable space is mMaxIndividualCookieSize-1.
	uint32_t		mDiskCookieStorageSize;		// Should usually be at least 32768.
	uint16_t		mMaxCookieCount;			// Max number of concurrent cookies. Should usually be at least 16. Set to zero to clear and disable cookies.
	const utf8_t*	mCookieFilePath;			// Full/Relative file path to writable directory. If relative, a valid full path should be returned when calling FileSystem::GetBaseDirectory(). SetCookieUsage copies this string.      

	CookieInfo()
		: mMaxIndividualCookieSize(4096)
		, mDiskCookieStorageSize(32768)
		, mMaxCookieCount(256)
		, mCookieFilePath(0)

	{

	}
	CookieInfo(const utf8_t* cookieFilePath, uint32_t maxIndividualCookieSize, uint32_t diskCookieStorageSize, uint16_t maxCookieCount)
		: mMaxIndividualCookieSize(maxIndividualCookieSize)
		, mDiskCookieStorageSize(diskCookieStorageSize)
		, mMaxCookieCount(maxCookieCount)
		, mCookieFilePath(cookieFilePath)
	{

	}
};

const size_t kFontNameCapacity = 32;
struct Parameters
{
	const char8_t*      mpLocale;                           // Defaults to NULL, which means "en-us". The SetParameters function copies this string, mpLocale doesn't need to persist. Currently unused.
	const char8_t*		mpAcceptLanguageHttpHeaderValue;    // Defaults to NULL, which means the "Accept-Language" header is not sent. If present, this is the "value" part of the header. This is used when making an http(s) request.The SetParameters function copies this string, mpAcceptLanguage doesn't need to persist.
	const char8_t*      mpApplicationName;                  // Defaults to NULL, which means "EAWebKit". The SetParameters function copies this string, mpApplicationName doesn't need to persist.

	// Transport settings:
	const char8_t*      mpUserAgent;                    // Defaults to NULL, which means "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/602.1 (KHTML, like Gecko) WebKit/188436 EAWebKit/"<EAWebKit Version>. The SetParameters function copies this string, mpUserAgent doesn't need to persist. See http://www.useragentstring.com/
	uint32_t            mMaxTransportJobs;              // Defaults to 16. Specifies maximum number of concurrent transport jobs (e.g. HTTP requests).
    uint32_t            mMaxParallelConnectionsPerHost; // Defaults to 6. Specifies the number of parallel connections maintained to host.
	uint32_t			mHttpRequestResponseBufferSize; // Defaults to 4096. Number of bytes that a HTTP request/response handle has for transaction with server. This is only for request/response headers and does not put any limit on the actual resource size(say a css file).
	uint32_t            mPageTimeoutSeconds;            // Defaults to 30 seconds. Page load timeout, in seconds. This timeout is applied to each individual resource. Your page does not have to finish loading within 30 seconds but this timeout is maximum time allowed between consecutive bytes received from the server (until the resource is fully loaded). 
	bool				mHttpPipeliningEnabled;		    // Defaults to false. If enabled, the current implementation increases load times with slightly lower memory usage.
	bool                mVerifySSLCert;                 // Defaults to true. If true then we do SSL/TLS peer verification via security certificates. You should set this to false only if debugging non-shipping builds. Was called mVerifyPeersEnabled in 1.x.
	bool				mTickHttpManagerFromNetConnIdle;// Defaults to true (existing behavior). If true, NetConnIdle call in DirtySDK updates HttpManager used by this library. This results in slightly improved performance. The reason to turn it false would be if you are ticking EAWebKit and DirtySDK on different threads.
	int32_t				mHttpManagerLogLevel;			// Defaults to 1.
	int32_t				mEAWebkitLogLevel;              //Defaults to 1.
	//+ Start of WebCore::Settings section (these values get transfered down to the WebCore Settings but only for a new webframe)
	// Font settings
	uint32_t   mSystemFontSize;            // Defaults to 13                 
	uint32_t   mDefaultFontSize;           // Defaults to 16.
	uint32_t   mDefaultMonospaceFontSize;  // Defaults to 13.
	uint32_t   mMinimumFontSize;           // Defaults to 1.
	uint32_t   mMinimumLogicalFontSize;    // Defaults to 8.
	char16_t   mFontFamilyStandard[kFontNameCapacity];    // Defaults to "Times New Roman". This will be the font used if none of the fonts specified for the text could be found or no font was specified for the text. This should be set to a font you have installed with the FontServer.
	char16_t   mFontFamilySerif[kFontNameCapacity];       // Defaults to "Times New Roman".
	char16_t   mFontFamilySansSerif[kFontNameCapacity];   // Defaults to "Arial".
	char16_t   mFontFamilyMonospace[kFontNameCapacity];   // Defaults to "Courier New".
	char16_t   mFontFamilyCursive[kFontNameCapacity];     // Defaults to "Comic Sans MS".
	char16_t   mFontFamilyFantasy[kFontNameCapacity];     // Defaults to "Comic Sans MS".
    char16_t   mSystemFont[kFontNameCapacity];            // Defaults to 0. This is the font that appears in form elements such as listboxes.
	bool       mSystemFontItalic;                         // Defaults to false. Italic if true. 
	bool       mSystemFontBold;                           // Defaults to false. Bold if true.
    uint16_t   mPasswordMaskCharacter;                    // Defaults to the bullet character.  
	//- End of WebCore::Settings section 

    //Remote web inspector settings
    uint16_t   mRemoteWebInspectorPort;                   // Defaults to 0 (disabled)
	const char8_t*      mpRemoteWebInspectorIp;           // Defaults to 0.0.0.0 listening on all ip

	//Note by Arpit Baldeva: JavaScriptStackSize defaults to 256 KB. The core Webkit trunk allocates 0.5 MB by default (well, they don't allocate but assume that the platform has on-demand commit capability) at the time of writing. This is not suitable for consoles with limited amount of memory and without on-demand commit capability.
	// The user can tweak this size and may be get around by using a smaller size that fits their need. If the size is too small, some JavaScript code may fail to execute. You will see a DebugLog similar to following.
	// EAWebKit: JS Error - undefined (0): RangeError: Maximum call stack size exceeded.
	uint32_t    mJavaScriptStackSize;
	uint32_t	mJavaScriptHeapWatermark; // PC Only(Ignored on Consoles as default value is sufficient). The watermark at which JSCore engine starts to recollect blocks. Higher watermark means more speed of execution. 1 MB by default. 

	// Font smoothing size
	uint32_t    mSmoothFontSize;                // Default to 18 css pixels.  If 0, all font sizes are smooth (anti-aliased), including bold and italic. Results can vary depending on font family and sizes (better in general for larger fonts).
	uint32_t    mFontFilterColorIntensity;      // Default is 48.  0-255 range.  If mEnableFontAlphaFilter is true, it will filter any pen color that has a color channel <= to this intentsity value.  It is to avoid filtering bright colors (e.g white) which can sometimes look faded if alpha filtered.
	bool        mEnableFontAlphaFilter;         // Default is false.  If set, it will lower the alpha values of glyphs (mostly used to counter blur from smoothing) if the pen color channels are under mFontFilterColorIntensity value.

	bool mDefaultToolTipEnabled;            // Default to true.  Allows the display of the default tool tip text. 

	
    bool mReportJSExceptionCallstacks;			// Defaults to false.
    bool mDoCssFiltersInHardware;               // Defaults to false, to enable you must implement a hardware shader for each of EA::WebKit::FilterType*
	bool mEnableProfiling;						// Disabled by default. If enabled, EAWebKitClient::ViewProcessStatus is called with profiling info.
	bool mEnableImageCompression;               // Defaults to false.  If enabled, it will compress decoded images saving runtime memory at the expense of decoding processing.
	bool mIgnoreGammaAndColorProfile;			// Defaults to false. If enabled, the gamma correction and color profile for images is skipped resulting in faster decoding.
	bool mEnableLowQualitySamplingDuringScaling;// Default to true. If enabled, the image is sampled at low quality before it comes to rest. The image remains at low quality for at least 50 ms after scaling is done. Useful for good performance if your page has heavy zoom/pan usage(for example, maps.google.com) 
    bool mEnableRoundedCornerClip;              // Enables detailed rounded corner clipping. This can however impact performance so off by default. 
	bool mEnableWebWorkers;						// Defaults to true. Web workers create an OS level thread which might not be desired in some scenarios. This option allows disabling web workers at library initialization. Note that the JavaScript based checks for web worker support will still succeed however the failure will be apparent. 
    Parameters();
private:
	// Disallow compiler generated copy ctor. The embedder should take a reference using GetParameters().
	Parameters(const Parameters&);
};


// These are optional settings for various buttons, scroll bars and selections colors
// Note that most of these do NOT have a direct mapping in terms of CSS properties as WebKit expects them to specified by the OS. Thus, these
// cannot be overriden by user supplied style sheet.
// In general, the users should try to use CSS styling mechanism to the extent possible for making WebKit rendered content similar to the 
// rest of the app in terms of look and feel.
struct ThemeParameters
{
	// Selection colors (e.g highlight text for cut&paste)
	uint32_t mColorActiveSelectionBack;       
	uint32_t mColorActiveSelectionFore;
	uint32_t mColorInactiveSelectionBack;
	uint32_t mColorInactiveSelectionFore;

	// List/Combo/Select dropdown box 
	uint32_t mColorActiveListBoxSelectionBack;   // Default Background color for the selected item in a List/Combo/Select dropdown box       
	uint32_t mColorActiveListBoxSelectionFore;   // Default color for the selected text in a List/Combo/Select dropdown box 
	uint32_t mColorInactiveListBoxSelectionBack; // Default Background color for the unselected item in a List/Combo/Select dropdown box
	uint32_t mColorInactiveListBoxSelectionFore; // Default color for the unselected text in a List/Combo/Select dropdown box

	// Buttons/Text Fields/Radio/Check Mark etc.
	uint32_t mBoxEnabledFillColor;   // For the inside of radio and check boxes when enabled.
	uint32_t mBoxDisabledFillColor;  // For the inside of radio and check boxes when not enabled.
	uint32_t mCheckMarkColor;        // Used for radio and check marks
	uint32_t mHoverHighlightColor;   // Used for hover highlights
	uint32_t mRadioButtonSize;       // Radio button size
	uint32_t mCheckBoxSize;          // Check mark size.  In 1.x we used the same size for radio and checkbox.
	uint32_t mTextFieldBorderWidth;  // Border for text field boxes. Setting to 2 will give a more 3D feel. 
	uint32_t mMenuListBorderWidth;   // Border for menu list boxes. Setting to 2 will give a more 3D feel. 

	// ScrollBar 
	// ScollBar can be completely customized using CSS.

    // Custom focus ring color
	uint32_t mCustomFocusRingColor;     // optional custom color for the focus ring. (0 by default which disables using a custom color). 
	bool     mEnableFocusRingDraw;      // Enables the focus ring draw (true by default) 

	ThemeParameters();
};


}
}

namespace EA
{
namespace WebKit
{
class Allocator;
class FileSystem;
class EAWebKitClient;
class View;
class EASTLFixedString16Wrapper;
class EASTLFixedString8Wrapper;
class EASTLHeaderMapWrapper;
class TransportHandler;
class TransportInfo;
class SocketTransportHandler;
class JavascriptValue;
class ITextSystem;
class IThreadSystem;

enum CookieFlags //Used in conjunction with cookie callbacks
{
	Secure		= 1 << 0,
	Session		= 1 << 1,
	HttpOnly	= 1 << 2
};

struct CookieEx //Would like to call it Cookie or CookieInfo but they are already taken.
{
	const char16_t* pName;
	const char16_t* pValue;
	const char16_t* pUrl;
	int64_t expires; //in seconds
	uint32_t flags; //Secure/Session/HttpOnly

	CookieEx()
		: pName(0), pValue(0), pUrl(0), expires(-1),flags(0)
	{

	}
};

// Callbacks
typedef double	    (*EAWebKitTimerCallback)();
typedef double	    (*EAWebKitMonotonicTimerCallback)();
typedef void*   	(*EAWebKitStackBaseCallback)();
typedef bool		(*EAWebKitCryptographicallyRandomValueCallback)(unsigned char *buffer, size_t length);  // Returns true if no error, else false
typedef void		(*EAWebKitGetCookiesCallback)(const char16_t* pUrl,EASTLFixedString16Wrapper& result,uint32_t flags);
typedef bool		(*EAWebKitSetCookieCallback)(const CookieEx& cookie);

struct AppCallbacks
{
EAWebKitTimerCallback							timer;
EAWebKitMonotonicTimerCallback					monotonicTimer;
EAWebKitStackBaseCallback						stackBase;
EAWebKitCryptographicallyRandomValueCallback	cryptRandomValue;
EAWebKitGetCookiesCallback						getCookies;
EAWebKitSetCookieCallback						setCookie;
};

struct AppSystems
{
EA::WebKit::Allocator* mAllocator;
EA::WebKit::FileSystem* mFileSystem;
EA::WebKit::ITextSystem* mTextSystem;
EA::WebKit::IThreadSystem* mThreadSystem;
EA::WebKit::EAWebKitClient* mEAWebkitClient;
};

class EAWebKitLib
{
public:
	// APIs related to the library setup/tear down
	// 12/05/2011 - The Init function now accepts the callbacks and system overrides as part of it. 
	virtual bool Init(AppCallbacks* appCallbacks = NULL, AppSystems* appSystems = NULL);
	virtual void Shutdown();

	// Added 04/10/2013
	// Call tick repeatedly on the thread where you run EAWebKit. You do NOT have to limit the tick to 60 FPS. Ideally, tick as 
	// frequently as your application can handle. But at bare minimum, call tick at least 60 FPS. This tick processes global functions of
	// EAWebKit that are not bound to a specific view.
	virtual void Tick();

	// Accessors
	virtual EAWebKitClient* GetEAWebKitClient(const View* pView = NULL);
	virtual Allocator*		GetAllocator();
	virtual FileSystem*		GetFileSystem();
	virtual ITextSystem*	GetTextSystem();
	virtual IThreadSystem*	GetThreadSystem();

	// APIs related to EA::WebKit::View
	virtual View*	CreateView();
	virtual void	DestroyView(View* pView);
	
	// APIs related to EAWebKit run time preferences
	virtual void				SetParameters(const Parameters& params);
	virtual Parameters&			GetParameters();
	virtual void				SetRAMCacheUsage(const RAMCacheInfo& ramCacheInfo);
    virtual void				GetRAMCacheUsage(RAMCacheUsageInfo& ramCacheUsageInfo);
   	virtual bool				SetDiskCacheUsage(const DiskCacheInfo& diskCacheInfo);
	virtual void				GetDiskCacheUsage(DiskCacheUsageInfo& diskCacheUsageInfo);
	virtual void				SetCookieUsage(const CookieInfo& cookieInfo);
	virtual void				SetThemeParameters(const ThemeParameters& themeParams);
	virtual ThemeParameters&	GetThemeParameters();

	// APIs related to Domain filtering system. Useful if you don't have full control over content pages(For example, a third party payment processor site)
	virtual void AddAllowedDomainInfo(const char8_t* allowedDomain, const char8_t* excludedPaths = 0);
	virtual bool CanNavigateToURL(const char8_t* url);
	
	// APIs related to Transport handlers
	virtual void              AddTransportHandler(TransportHandler* pTH, const char16_t* pScheme);
	virtual void              RemoveTransportHandler(TransportHandler* pTH, const char16_t* pScheme);
	virtual TransportHandler* GetTransportHandler(const char16_t* pScheme);

	//APIs related to Socket transport (For Web Socket support)
	virtual void					AddSocketTransportHandler(SocketTransportHandler* pSTH);
	virtual void					RemoveSocketTransportHandler(SocketTransportHandler* pSTH);
	virtual SocketTransportHandler* GetSocketTransportHandler();

    // APIs related to Javascript
    virtual JavascriptValue *CreateJavascriptValue(View *view);
    virtual void DestroyJavascriptValue(JavascriptValue *v);
    virtual JavascriptValue *CreateJavascriptValueArray(View *view, size_t count);
    virtual void DestroyJavascriptValueArray(JavascriptValue *args);

	// Misc. Runtime APIs
	virtual void ClearMemoryCache(MemoryCacheClearFlags flags = MemoryCacheClearAll);
    virtual void TriggerGarbageCollectFromScript();   //The least impactful way to try free up some memory.
	virtual void NetworkStateChanged(bool isOnline); //Call this to indicate any change in network status. 
	virtual void RegisterURLSchemeAsCORSEnabled(const char16_t* pScheme);
    virtual bool IsURLSchemeCORSEnabled(const char16_t* pScheme);

	// Add a user style sheet that affects all rendered pages.
	// Example 1 - Adding ":link, :visited { text-decoration: line-through ! important; }" will line-through all the links on all pages even if styled by the page.
	// Example 2 - Adding ":link, :visited { text-decoration: line-through;}" will line-through all the links that are not styled on the pages.
    virtual void AddUserStyleSheet(const EA::WebKit::View* pView, const char8_t* userStyleSheet);



	// APIs related to mostly debugging
	virtual const char8_t*	GetVersion();

	//
	// Add newer APIs above this.
	//


	////////////////////////////////////////////////////////////////////////

	//
	//
	// Normally, an application does not need following two unless they want to manage cookie storage themselves. 
	// For example, by combining cookies from other browsers.
	
	// An application can pass cookie text(in the standard http headers format without the Set-Cookie/Set-Cookie2 key). These cookies will we considered trusted
	// cookies so no check against a Url will be made.
	virtual bool		AddCookie(const char8_t* pHeaderValue);
	// An application can pass in the buffer and read the cookies data.
	// If a 0 is passed for buffer, the return value indicates the number of cookies that can be read.
	// For a non null buffer, the return value indicates the number of cookies that can are read.
	// A typical usage will be 
	// numCookies = ReadCookies(0);
	// allocate rawCookieData using numCookies and CookieInfo::mMaxCookieCount
    // numCookiesRead = ReadCookies(rawCookieData);
	virtual uint16_t	ReadCookies(char8_t** rawCookieData);
	//
	//
	////////////////////////////////////////////////////////////////////////

	
	
	
	
	
	
	
	
	
	
	
	////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//
	// Following APIs are not expected to be called by the users of EAWebKit. They exist for various reasons. 
	// Some of them are used by EAWebKit internally.
	//
	//
	//
	// Following would load SSL certificates on the Dll side transport handler. If your application supplies 
	// its own transport handler, you need to load the certificate on the application side.
	virtual int32_t LoadSSLCertificate(const uint8_t *pCACert, int32_t iCertSize);
	virtual void	ClearSSLCertificates();
	virtual void	SetPlatformSocketAPI(const EA::WebKit::PlatformSocketAPI& platformSocketAPI);

	virtual uint32_t	SetTextFromHeaderMapWrapper(const EASTLHeaderMapWrapper& headerMapWrapper, char8_t* pHeaderMapText, uint32_t textCapacity);
	virtual bool		SetHeaderMapWrapperFromText(const char8_t* pHeaderMapText, uint32_t textSize, EASTLHeaderMapWrapper& headerMapWrapper,  bool bExpectFirstCommandLine, bool bClearMap);
	virtual double		GetTime();


	virtual void ReattachCookies(TransportInfo* pTInfo);
	virtual void RemoveCookies();

	virtual ~EAWebKitLib()
	{
	}
};

}
}



#endif // EAWEBKIT_EAWEBKIT_H
