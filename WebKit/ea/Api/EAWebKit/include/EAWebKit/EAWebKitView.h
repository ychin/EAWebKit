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

#ifndef EAWEBKIT_EAWEBKITVIEW_H
#define EAWEBKIT_EAWEBKITVIEW_H

#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebKitSystem.h>
#include <EAWebKit/EAWebkitSTLWrapper.h>
#include <EAWebKit/EAWebKitInput.h>
#include <EAWebKit/EAWebKitSurface.h>
#include <EAWebKit/EAWebkitJavascriptValue.h>
#include <EAWebKit/EAWebKitHardwareRenderer.h>

namespace JSC
{
class ExecState;
}

namespace WebCore
{
class Frame;
class Node;
class TileEA;
class BitmapTextureEA;
}

namespace EA { namespace WebKit
{
class WebPage;
class ViewNavigationDelegate;
class EAWebKitClient;
class WebFrame;

//
// ViewParameters - These are passed to the view upon initialization.
//
struct ViewParameters
{
	ISurface*			mDisplaySurface;        // Defaults to NULL. If no surface is provided, a default display surface will be created(if not using tiled backing storage). If a display surface is provided along with a hardware renderer, they should be compatible.
	IHardwareRenderer*	mHardwareRenderer;		// Defaults to NULL. If provided, this renderer will enable hardware rendering for this view.
	void*				mpUserData;				// Defaults to NULL. This will be passed back to the user along with the View* when applicable.
	EAWebKitClient*		mEAWebKitClient;		// Defaults to NULL. If provided, the notification callback will use this client instead of the global EAWebKitClient to notify the app about View specific events. It is this client's responsibility to forward the notification to the application specific client if it does NOT handle a notification.
	int					mWidth;					// Defaults to kViewWidthDefault
	int					mHeight;                // Defaults to kViewHeightDefault
	uint32_t			mBackgroundColor;       // Color the texture should be set to (behind the webpage). May be transparent.
	int					mTileSize;				// Defaults to 512. Only used if using tiled backing store. Otherwise, ignored. 
	bool				mUseTiledBackingStore;	// Defaults to true. Only used if the hardware renderer is provided. Otherwise, ignored.
	bool				mRemoveNonVisibleTiles;	// Defaults to true. Only used if using tiled backing store. Otherwise, ignored. Keeping a non visible tile around may improve scrolling performance in some cases at the expense of additional memory usage.
	bool				mbTabKeyFocusCycle;     // Defaults to true.
	bool				mAllowJSTextInputStateNotificationOnConsole;	// Defaults to true. When this flag is set, the TextInputState event is fired when calling say .focus() from the page on an element. This allows the client to open a software keyboard automatically without needing the user to click. 
																		// The reason to turn it to false will be to allow the user a better view of the page content without popping in keyboard automatically.
																		// This flag default to true which changes the existing behavior at the introduction of this flag. However, that seems the right thing to do as it probably is the expected behavior of a new user of this library.
     bool                mDisableCaretOnConsole;   // Defaults to false. Set to true if you don't want the caret to display/blink in a text input box (e.g. the virtual keyboard has its own blinking cursor).  
    ViewParameters(void);
};

inline ViewParameters::ViewParameters(void)
: mDisplaySurface(NULL), mHardwareRenderer(NULL), mpUserData(NULL), mEAWebKitClient(NULL) 
, mWidth(kViewWidthDefault), mHeight(kViewHeightDefault), mBackgroundColor(0xffffffff) 
, mTileSize(512), mUseTiledBackingStore(false), mRemoveNonVisibleTiles(false)
, mbTabKeyFocusCycle(true), mAllowJSTextInputStateNotificationOnConsole(true), mDisableCaretOnConsole(false)
{
}

// The virtual members of this class represent the API that can be called by the Application.
// Non virtual members are for internal usage.
class View
{
public:
	explicit View(void);
    virtual ~View(void);
	
	//
	// Setup/Teardown
	//
	virtual bool InitView(const ViewParameters& vp);
	virtual void ShutdownView(void);
	
    //
	// Runtime
	//
	virtual void Paint();

    //
    // URI Navigation
    //
    virtual void SetURI(const char16_t* url); 
    virtual void SetURI(const char8_t *url);
    virtual const char16_t* GetURI(void) const;

    // These two functions internally cache the result that is returned.
    // If you plan on copying it, or using it later, make sure to do so
    // before another call to GetEncodedURI.
    //
    // This function is useful for comparing the string you pass to SetURI
    // with the string returned from GetURI.
    // For Example: result = Compare(GetURI(), GetEncodedURI(stringPassedToSetURI))
	// The reason for doing comparison this way is because GetURI call returns encoded URI of the URI passed to
	// it(for example, white spaces are encoded) and a straight up strcmp would fail.
    virtual const char16_t* GetEncodedURI(const char16_t *url);
    virtual const char16_t* GetEncodedURI(const char8_t *url);
    

    virtual void CancelLoad(void);
    virtual void GoBack(void);
    virtual void GoForward(void);
    virtual void Refresh(void);

    //
    // Setting the web page/element content from application 
    //
	virtual void SetHtml(const char8_t* html, size_t length, const char8_t* baseUrl = "about:blank");
	virtual void SetContent(const char8_t* data, size_t length, const char8_t* mediaType, const char8_t* baseUrl = "about:blank");// example of mediaType or Content-Type is "text/html; charset=utf-8"

    //
    // JavaScript
    //
    virtual bool EvaluateJavaScript(const char8_t *source, JavascriptValue *resultOut = NULL);
    virtual bool EvaluateJavaScript(const char16_t *source, JavascriptValue *resultOut = NULL);
    virtual void BindJavaScriptObject(const char8_t *name, IJSBoundObject *obj);

    //
    // Input Events
    //
    virtual void OnMouseMoveEvent(const MouseMoveEvent& ev);
    virtual void OnMouseButtonEvent(const MouseButtonEvent& ev);
    virtual void OnMouseWheelEvent(const MouseWheelEvent& ev);
    virtual void OnKeyboardEvent(const KeyboardEvent& ev);
    virtual void OnFocusChangeEvent(bool hasFocus);

    //
	// Input Fields - Useful on platforms with virtual keyboards.
	//
	virtual void		SetTextIntoSelectedInput(const char16_t* contentTextBuffer, bool textWasAccepted = true); 
	virtual uint32_t	GetTextFromSelectedInput(char16_t* contentTextBuffer, const uint32_t maxcontentTextBufferLength, char16_t* titleTextBuffer = 0, const uint32_t maxTitleTextBufferLength = 0 );

	//
	// Navigation using a controller
	//
	virtual bool JumpToNearestElement(EA::WebKit::JumpDirection direction);
	virtual bool Click();
	virtual bool IsEmulatingConsoleOnPC() const;
	virtual void SetEmulatingConsoleOnPC(bool emulatingConsoleOnPC);
	virtual void GetCursorPosition(int& x, int& y) const; 
	
	
	//
    // Miscellaneous
    //
	virtual void* GetUserData(void) const;
    virtual ISurface *GetDisplaySurface(void);
    virtual bool HardwareAccelerated(void);
    virtual const char16_t* Title(void) const;
    virtual IntSize GetSize(void) const;
    virtual void SetSize(IntSize size);
    virtual void SaveSurfacePNG(const char8_t *filepath);
    virtual void ShowInspector(bool state);
    virtual void SetZoomFactor(float factor);   // 1.0f is the default. Note that using multiple values can have an an impact on the glyph cache capacity.
    virtual float ZoomFactor(void) const; 
	// Turn visual debug draw on/off. For simplicity, turn everything on/off together.
	// Hardware rendering - Red color - Dirty rects // tiled layer: orange // normal layer: green // masking layer: pale blue // container: yellow
	// Software rendering - Red color - Dirty rects
	// In hardware rendering, memory usage may slightly increase because of the additional surfaces being created for drawing texture boundaries.
	virtual void SetDrawDebugVisuals(bool on);   
    virtual void ForceInvalidateFullView(); // This call is normally not needed but can be used to manually force the view to be repainted.  


    //
    // Internal Functions - These should not be called outside the DLL (and therefore should not be virtual)
    //
	void triggerPageAction(WebAction action);
	void AddDirtyRegion(const IntRect &region);
    JSC::ExecState* GetJSExecState(void);
    void RebindJSBoundObjects(void);
	EAWebKitClient* GetClient(void)const;
    uint32_t GetBackgroundColor(void) const;
    ISurface* CreateOverlaySurface(int x, int y, int width, int height);    
    void DestroyOverlaySurface(ISurface* pOverlaySurface);
    void SetOverlayInputClient(IOverlayInputClient* pClient);
	IOverlayInputClient* GetOverlayInputClient() const;
    WebPage* Page(void) const;
    void SetPage(WebPage* page);
    void PaintOverlays(void);
    

	void SetCursorPosition(int x, int y);
	void MoveMouseCursorToNode(WebCore::Node* node, bool scrollIfNecessary = true);
	bool AllowJSTextInputStateNotificationOnConsole(void) const;
	
	void SetBeingDebugged(bool state);

	WebCore::Frame* GetFrame(bool focusFrame = false) const;
    void ScheduleAnimation(void);
    void SetToolTip(const char16_t* tip);
	bool IsUsingTiledBackingStore() const;
	bool IsUsingCPUCompositing() const; // Only exists for initial testing (It is not optimized at all and is slower than regular flow at the moment)
	IHardwareRenderer *GetHardwareRenderer(void);
	bool ShouldRemoveNonVisibleTiles() const;
	bool ShouldDrawDebugVisuals() const;

	void AddTile(WebCore::TileEA* tile);
	void RemoveTile(WebCore::TileEA* tile);

	void AddTexture(WebCore::BitmapTextureEA* texture);
	void RemoveTexture(WebCore::BitmapTextureEA* texture);

    bool IsCaretDisabledOnConsole(void) const;   


private:
	void SaveSurfacePNGHelper(const char8_t *filepath, EA::WebKit::ISurface* surface);
	friend class WebPage;
	friend class ViewNavigationDelegate;

    struct ViewPrivate* d;
};

}}

#endif // EAWEBKIT_EAWEBKITVIEW_H
