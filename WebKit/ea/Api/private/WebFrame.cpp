/*
    Copyright (C) 2008,2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.
	Copyright (C) 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc. All rights reserved.

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
#include "WebFrame.h"

#include "APICast.h"
#include "BridgeJSC.h"
#include "CallFrame.h"
#include "Document.h"
#include "DocumentLoader.h"
#include "DragData.h"
#include "Element.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoaderClientEA.h"
#include "FrameSelection.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "Chrome.h"
#include "FrameLoadRequest.h"
#include "GCController.h"
#include "GraphicsContext.h"
#include "HTMLMetaElement.h"
#include "HitTestResult.h"
#include "HTTPParsers.h"
#include "IconDatabase.h"
#include "InspectorController.h"
#include "JavaScript.h"
#include "JSDOMBinding.h"
#include "JSDOMWindowBase.h"
#include "JSLock.h"
#include "JSObject.h"
#include "JSRetainPtr.h"
#include "OpaqueJSString.h"
#include "NetworkingContext.h"
#include "NodeList.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PlatformWheelEvent.h"
#include "PrintContext.h"
#include "PutPropertySlot.h"
#include "RenderLayer.h"
#include "RenderTreeAsText.h"
#include "RenderView.h"
#include "ResourceRequest.h"
#include "ScriptController.h"
#include "ScriptSourceCode.h"
#include "ScriptValue.h"
#include "Scrollbar.h"
#include "Settings.h"
#include "SubstituteData.h"
#include "SVGSMILElement.h"
#include "TiledBackingStore.h"
#include "htmlediting.h"
#include "markup.h"

#include "WebFrame_p.h"
#include "WebPage.h"
#include "WebPage_p.h"
#include "runtime_object.h"
#include "runtime_root.h"
#include "interpreter.h"
#if USE(ACCELERATED_COMPOSITING)
#include "texmap/TextureMapper.h"
#include "texmap/TextureMapperPlatformLayer.h"
#include "TextureMapperLayerClientEA.h"
#endif

#include <wtf/HashMap.h>
#include "ea_utility.h"
#include "ea_instance.h"
#include <EAWebKit/EAWebKit.h>
#include <EAWebkit/EAWebkitClient.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKit_p.h>
#include "PageClientEA.h"
#include "ChromeClientEA.h"

#include "ScrollTypes.h"
#include "RefPtrCairo.h"

#include "PlatformContextCairo.h"

namespace EA
{
namespace WebKit
{
    const bool kZoomTextOnly = false;   // QT had this as a user option, not sure we would even need it.  
    
    WebFrameData::WebFrameData(WebCore::Page* parentPage, WebCore::Frame* parentFrame,
                             WebCore::HTMLFrameOwnerElement* ownerFrameElement,
                             const WTF::String& frameName)
    : name(frameName)
    , ownerElement(ownerFrameElement)
    , page(parentPage)
    , allowsScrolling(true)
    , marginWidth(0)
    , marginHeight(0)
{
	using namespace WebCore;
	// mainframe is already created in WebCore::Page, just use it.
	if (!parentFrame || !ownerElement) {
		frame = &parentPage->mainFrame();
		frameLoaderClient = static_cast<FrameLoaderClientEA*>(&frame->loader().client());
	} else {
		frameLoaderClient = new FrameLoaderClientEA();
		frame = Frame::create(page, ownerElement, frameLoaderClient);
	}

	// FIXME: All of the below should probably be moved over into WebCore
	frame->tree().setName(name);
	if (parentFrame)
		parentFrame->tree().appendChild(frame);
}


void WebFramePrivate::init(WebFrame* wframe, WebFrameData* frameData)
{
	webFrame = wframe;
	
	allowsScrolling = frameData->allowsScrolling;
	marginWidth = frameData->marginWidth;
	marginHeight = frameData->marginHeight;
	frame = frameData->frame.get();
	frameLoaderClient = frameData->frameLoaderClient;
	frameLoaderClient->setFrame(wframe, frame);

	frame->init();
}

void WebFramePrivate::setPage(WebPage* newPage)
{
// 	if (page == newPage)
// 		return;
	EAW_ASSERT_MSG(!page, "Not expecting it to be called multiple times");
	page = newPage;
}

WebCore::Scrollbar* WebFramePrivate::horizontalScrollBar() const
{
	if (!frame->view())
		return 0;
	return frame->view()->horizontalScrollbar();
}

WebCore::Scrollbar* WebFramePrivate::verticalScrollBar() const
{
	if (!frame->view())
		return 0;
	return frame->view()->verticalScrollbar();
}

void WebFramePrivate::urlChanged()
{
	url = frame->document()->url();
}

WebFrame::WebFrame(WebPage *parent, WebFrameData *frameData)
: d(new WebFramePrivate)
{
	d->page = parent;
	d->init(this, frameData);

	if (!frameData->url.isEmpty()) 
	{
		WebCore::ResourceRequest request(frameData->url, frameData->referrer);
		d->frame->loader().load(WebCore::FrameLoadRequest(d->frame, request));
	}
}

WebFrame::WebFrame(WebFrame *parent, WebFrameData *frameData)
: d(new WebFramePrivate)
{
	d->page = parent->d->page;
	// abaldeva: Qt knows parentFrame through their framework. Also, in Qt, the parent of a WebFrame does not need to be WebFrame, 
	// it can be a WebPage. We set it explicitly here.
	d->parentWebFrame = parent;
	d->init(this, frameData);
}

WebFrame::~WebFrame()
{
	if (d->frame)
    {
		static_cast<WebCore::FrameLoaderClientEA&>(d->frame->loader().client()).m_webFrame = 0;
    }

    if (d->mHorizontalScroll)
    {
        d->mHorizontalScroll->Release();
    }

    if (d->mVerticalScroll)
    {
        d->mVerticalScroll->Release();
    }

    if (d->mScrollCorner)
    {
        d->mScrollCorner->Release();
    }

    delete d;
}


void WebFrame::addToJavaScriptWindowObject(const char8_t *name, IJSBoundObject *obj)
{
    WebCore::JSDOMWindow* window = toJSDOMWindow(d->frame, WebCore::mainThreadNormalWorld());
    JSC::Bindings::RootObject* root = d->frame->script().bindingRootObject();;

    if (!window) 
	{
        EAW_ASSERT_MSG(false, "Could not get window object");
		return;
    }

	if (!root) 
	{
		EAW_ASSERT_MSG(false, "Could not get root object");
		return;
	}

	JSC::ExecState* exec = window->globalExec();
	JSC::JSLockHolder lock(exec);
    
	JSC::JSObject* runtimeObject = JSC::Bindings::EAInstance::create(obj, root)->createRuntimeObject(exec);
	JSC::Identifier ident(&exec->vm(), name);
    JSC::PutPropertySlot slot;
    window->methodTable()->put(window, exec, ident, runtimeObject, slot);
}

void WebFrame::GetJavaScriptCallstack(eastl::vector<eastl::string8> *namesOut, eastl::vector<eastl::string8> *argsOut, eastl::vector<int> *linesOut, eastl::vector<eastl::string8> *urlsOut)
{
    WebCore::JSDOMWindow* window = toJSDOMWindow(d->frame, WebCore::mainThreadNormalWorld());
    JSC::ExecState *exec = window->globalExec();

	JSC::JSLockHolder lock(exec);
	//EAWebKitTODO: exec->interpreter()->GetCallstack(exec, namesOut, argsOut, linesOut, urlsOut);
}

WTF::String WebFrame::toHtml() const
{
	if (!d->frame->document())
		return WTF::String();
	return WebCore::createMarkup(*(d->frame->document()));
}

WTF::String WebFrame::toPlainText() const
{
	if (d->frame->view() && d->frame->view()->layoutPending())
		d->frame->view()->layout();

	WebCore::Element *documentElement = d->frame->document()->documentElement();
	if (documentElement)
		return documentElement->innerText();
	return WTF::String();
}

WTF::String WebFrame::renderTreeDump() const
{
	if (d->frame->view() && d->frame->view()->layoutPending())
		d->frame->view()->layout();

	return WebCore::externalRepresentation(d->frame);
}

WTF::String WebFrame::title() const
{
	if (d->frame->document())
		return d->frame->loader().documentLoader()->title().string();
	return WTF::String();
}

static inline void clearCoreFrame(WebCore::Frame* frame)
{
	WebCore::DocumentLoader* documentLoader = frame->loader().activeDocumentLoader();
	EAW_ASSERT(documentLoader);
	if(documentLoader)
	{
		documentLoader->writer().begin();
		documentLoader->writer().end();
	}
}

static inline bool isCoreFrameClear(WebCore::Frame* frame)
{
	return frame->document()->url().isEmpty();
}


void WebFrame::setUrl(const WebCore::URL& url)
{
    clearCoreFrame(d->frame);
    d->url = url;
    load(url);
}

WebCore::URL WebFrame::url() const
{
	return d->url;
}

WebCore::URL WebFrame::requestedUrl() const
{
	return d->frameLoaderClient->lastRequestedUrl();
}

WebCore::URL WebFrame::baseUrl() const
{
	if (isCoreFrameClear(d->frame))
		return d->url;

	return d->frame->document()->baseURL();
}

WebPage* WebFrame::page() const
{
	return d->page;
}

//The view remains the same until enough data has arrived to display the new url.
void WebFrame::load(const WebCore::URL& url)
{
	if (d->parentFrame())
		d->page->d->insideOpenCall = true;

	d->frame->loader().load(WebCore::FrameLoadRequest(d->frame,WebCore::ResourceRequest(url)));

	if (d->parentFrame())
		d->page->d->insideOpenCall = false;
}

void WebFrame::setHtml(const char8_t* html, size_t length, const WebCore::URL& baseUrl)
{
	setContent(html,length,"text/html; charset=utf-8",baseUrl);
}

void WebFrame::setContent(const char8_t* data, size_t length, const char8_t* pMediaType, const WebCore::URL& baseUrl)
{
	WebCore::ResourceRequest request(baseUrl);
	WTF::RefPtr<WebCore::SharedBuffer> buffer = WebCore::SharedBuffer::create(data, length);
	WTF::String actualMimeType;
	WTF::String encoding;
	WTF::String mediaType(pMediaType);
	if (mediaType.isEmpty())
		actualMimeType = "text/html";
	else {
		actualMimeType = WebCore::extractMIMETypeFromMediaType(mediaType);
		encoding = WebCore::extractCharsetFromMediaType(mediaType);
	}
	WebCore::SubstituteData substituteData(buffer, actualMimeType, encoding, WebCore::URL());
	d->frame->loader().load(WebCore::FrameLoadRequest(d->frame,request,substituteData));
}






WebFrame *WebFrame::parentFrame() const
{
    return d->parentFrame();
}

void WebFrame::setParentFrame(WebFrame* wFrame)
{
	d->setParentFrame(wFrame);
}
EA::WebKit::ScrollbarPolicy WebFrame::scrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation) const
{
	if (orientation == EA::WebKit::ScrollbarHorizontal)
        return d->horizontalScrollBarPolicy;
    return d->verticalScrollBarPolicy;
}

void WebFrame::setScrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation, EA::WebKit::ScrollbarPolicy policy)
{
	COMPILE_ASSERT((int)WebCore::ScrollbarAuto == (int)EA::WebKit::ScrollbarAuto, WebCoreAndEAWebKitScrollBarEnumsShouldMatch);
	COMPILE_ASSERT((int)WebCore::ScrollbarAlwaysOff == (int)EA::WebKit::ScrollbarAlwaysOff,WebCoreAndEAWebKitScrollBarEnumsShouldMatch);
    COMPILE_ASSERT((int)WebCore::ScrollbarAlwaysOn == (int)EA::WebKit::ScrollbarAlwaysOn,WebCoreAndEAWebKitScrollBarEnumsShouldMatch);

	if (orientation == EA::WebKit::ScrollbarHorizontal) {
        d->horizontalScrollBarPolicy = policy;
        if (d->frame->view()) {
			d->frame->view()->setHorizontalScrollbarMode((WebCore::ScrollbarMode)policy, policy != EA::WebKit::ScrollbarAuto /* lock */);
            d->frame->view()->updateCanHaveScrollbars();
        }
    } else {
        d->verticalScrollBarPolicy = policy;
        if (d->frame->view()) {
            d->frame->view()->setVerticalScrollbarMode((WebCore::ScrollbarMode)policy, policy != EA::WebKit::ScrollbarAuto /* lock */);
            d->frame->view()->updateCanHaveScrollbars();
        }
    }
}

void WebFrame::setScrollBarValue(EA::WebKit::ScrollbarOrientation orientation, int value)
{
	COMPILE_ASSERT((int)WebCore::HorizontalScrollbar	== (int)EA::WebKit::ScrollbarHorizontal,	ScrollbarOrientationEnumOutOfSync);
	COMPILE_ASSERT((int)WebCore::VerticalScrollbar		== (int)EA::WebKit::ScrollbarVertical,		ScrollbarOrientationEnumOutOfSync);

	WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb) {
        if (value < 0)
            value = 0;
        else if (value > scrollBarMaximum(orientation))
            value = scrollBarMaximum(orientation);
        sb->scrollableArea()->scrollToOffsetWithoutAnimation(orientation == EA::WebKit::ScrollbarHorizontal ? WebCore::HorizontalScrollbar : WebCore::VerticalScrollbar, value);
    }
}

int WebFrame::scrollBarValue(EA::WebKit::ScrollbarOrientation orientation) const
{
	WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->value();
    return 0;
}

int WebFrame::scrollBarMaximum(EA::WebKit::ScrollbarOrientation orientation) const
{
    WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->maximum();
    return 0;
}

int WebFrame::scrollBarMinimum(EA::WebKit::ScrollbarOrientation orientation) const
{
    (void)orientation;
    return 0;
}

WebCore::IntRect WebFrame::scrollBarGeometry(EA::WebKit::ScrollbarOrientation orientation) const
{
    WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->frameRect();
	return WebCore::IntRect();
}

void WebFrame::scroll(int dx, int dy)
{
    if (!d->frame->view())
        return;

    d->frame->view()->scrollBy(IntSize(dx, dy));
}

WebCore::IntPoint WebFrame::scrollPosition() const
{
    if (!d->frame->view())
		return WebCore::IntPoint(0,0);

	WebCore::IntSize ofs = d->frame->view()->scrollOffset();
	return WebCore::IntPoint(ofs.width(), ofs.height());
}

void WebFrame::setScrollPosition(const WebCore::IntPoint &pos)
{
	WebCore::IntPoint current = scrollPosition();
    int dx = pos.x() - current.x();
    int dy = pos.y() - current.y();
    scroll(dx, dy);
}

void WebFrame::scrollToAnchor(const WTF::String& anchor)
{
    WebCore::FrameView *view = d->frame->view();
    if (view)
        view->scrollToAnchor(anchor);
}

void WebFrame::ClearDisplay(const WebCore::Color &color)
{
    d->mClearDisplaySurface = true;
    d->mClearDisplayColor = color;
}

void WebFrame::renderNonTiled(EA::WebKit::IHardwareRenderer* renderer, ISurface *surface, const eastl::vector<WebCore::IntRect> &dirtyRegions) 
{
	EAW_ASSERT_MSG(!d->page->view()->IsUsingTiledBackingStore(), "non tiled rendering path called but using tiled backing store");
	if(d->page->view()->IsUsingTiledBackingStore())
		return;

	if (d->mClearDisplaySurface)
    {
		NOTIFY_PROCESS_STATUS(kVProcessTypeClearSurface, EA::WebKit::kVProcessStatusStarted, d->page->view());
        EA::WebKit::ClearSurfaceToColor(surface,d->mClearDisplayColor);
        d->mClearDisplaySurface = false;
		NOTIFY_PROCESS_STATUS(kVProcessTypeClearSurface, EA::WebKit::kVProcessStatusEnded, d->page->view());
    }

    WebCore::FrameView *view = d->frame->view();
	NOTIFY_PROCESS_STATUS(kVProcessTypeDirtyRectsRender, EA::WebKit::kVProcessStatusStarted, d->page->view());
	for (unsigned i = 0; i < dirtyRegions.size(); ++i) 
	{
        ISurface::SurfaceDescriptor surfaceDescriptor = {0};
        
        IntRect eaRect(dirtyRegions[i]);
        surface->Lock(&surfaceDescriptor, &eaRect);
        
		RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, eaRect.mSize.mWidth, eaRect.mSize.mHeight, surfaceDescriptor.mStride));    
        RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));
        
		WebCore::GraphicsContext graphicsContext(cairoContext.get());

        if (!graphicsContext.paintingDisabled() || graphicsContext.updatingControlTints()) 
        {
            const WebCore::IntRect &dirty = dirtyRegions[i];
			graphicsContext.translate(-dirty.x(), -dirty.y());// Translate the context so the drawing starts at the origin of the locked portion.
            view->paint(&graphicsContext, dirty);// Paint contents and scroll bars. Some ports call paintContents and then painScrollbars separately.
        }

		EAW_ASSERT_FORMATTED(cairo_status(cairoContext.get()) == CAIRO_STATUS_SUCCESS, "cairo failed to paint (for example, OOM) - %d",cairo_status(cairoContext.get()));

        if(d->page->view()->ShouldDrawDebugVisuals())
		{
			NOTIFY_PROCESS_STATUS(kVProcessTypeDrawDebug, EA::WebKit::kVProcessStatusStarted, d->page->view());
			graphicsContext.save();
			
			graphicsContext.setStrokeStyle(WebCore::SolidStroke);
			graphicsContext.setStrokeColor(WebCore::Color(1.0f,0.0f,0.0f,1.0f), WebCore::ColorSpaceDeviceRGB);
			graphicsContext.strokeRect(WebCore::FloatRect(eaRect.mLocation.mX,eaRect.mLocation.mY,eaRect.mSize.mWidth,eaRect.mSize.mHeight),2.0f); 

			graphicsContext.restore();
			NOTIFY_PROCESS_STATUS(kVProcessTypeDrawDebug, EA::WebKit::kVProcessStatusEnded, d->page->view());
		}

        surface->Unlock();
    }
	NOTIFY_PROCESS_STATUS(kVProcessTypeDirtyRectsRender, EA::WebKit::kVProcessStatusEnded, d->page->view());

	NOTIFY_PROCESS_STATUS(kVProcessTypeRenderCompLayers, EA::WebKit::kVProcessStatusStarted, d->page->view());
	renderCompositedLayers(renderer, surface);
	NOTIFY_PROCESS_STATUS(kVProcessTypeRenderCompLayers, EA::WebKit::kVProcessStatusEnded, d->page->view());

	drawHighlightedNodeFromInspector(surface);
}


void WebFrame::renderTiled(EA::WebKit::IHardwareRenderer* renderer, ISurface* surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
#if USE(TILED_BACKING_STORE)
	EAW_ASSERT_MSG(d->page->view()->IsUsingTiledBackingStore(), "tiled rendering path called but not using tiled backing store");
	
	if(!d->page->view()->IsUsingTiledBackingStore())
		return;

	if(!d->page->view()->HardwareAccelerated())//Don't need to copy the tiled rects to main surface in GPU compositing path
	{
		for (unsigned i = 0; i < dirtyRegions.size(); ++i) 
		{
			ISurface::SurfaceDescriptor surfaceDescriptor = {0};

			// Only lock the portion of the surface that is being rendered to.
			IntRect eaRect(dirtyRegions[i]);
			surface->Lock(&surfaceDescriptor, &eaRect);

			RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, eaRect.mSize.mWidth, eaRect.mSize.mHeight, surfaceDescriptor.mStride));    
			RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

			WebCore::GraphicsContext graphicsContext(cairoContext.get());
			int scrollX = d->frame->view()->scrollX();
			int scrollY = d->frame->view()->scrollY();

			if (!graphicsContext.paintingDisabled() || graphicsContext.updatingControlTints()) 
			{
				const WebCore::IntRect& dirty = dirtyRegions[i];
				graphicsContext.translate(-dirty.x(), -dirty.y());// Translate the context so the dirty region is the origin.
				graphicsContext.translate(-scrollX, -scrollY); // This is simply written so due to the way TileCairo is implemented 

				WebCore::IntRect dirtyRectContents(scrollX+dirty.x(), scrollY+dirty.y(), dirty.width(),dirty.height());
				d->frame->tiledBackingStore()->paint(&graphicsContext, dirtyRectContents);

				graphicsContext.translate(scrollX, scrollY); // Scroll bars stay at place so translate back the scroll offset
				d->frame->view()->paintScrollbars(&graphicsContext, dirty); //scroll bars check if the dirty rect intersect them so we don't explicitly do it outside (well, at least on CPU path for now)
			}

			EAW_ASSERT_FORMATTED(cairo_status(cairoContext.get()) == CAIRO_STATUS_SUCCESS, "cairo failed to paint (for example, OOM) - %d",cairo_status(cairoContext.get()));
			surface->Unlock();
		}
	}
	renderCompositedLayers(renderer, surface);


	if(d->page->view()->HardwareAccelerated())
	{
		NOTIFY_PROCESS_STATUS(kVProcessTypePaintScrollbars, EA::WebKit::kVProcessStatusStarted, d->page->view());
		// Paint/render scrollbars.
		renderScrollbar(d->horizontalScrollBar(), renderer, &d->mHorizontalScroll, dirtyRegions);
		renderScrollbar(d->verticalScrollBar(), renderer, &d->mVerticalScroll, dirtyRegions);
		renderScrollCorner(d->frame->view(), renderer, &d->mScrollCorner, dirtyRegions);
		NOTIFY_PROCESS_STATUS(kVProcessTypePaintScrollbars, EA::WebKit::kVProcessStatusEnded, d->page->view());

	}


	drawHighlightedNodeFromInspector(surface);
#endif
}

void WebFrame::renderCompositedLayers(EA::WebKit::IHardwareRenderer* renderer, ISurface* surface)
{
#if USE(ACCELERATED_COMPOSITING)
	WebCore::Page* page = d->frame->page();
	if (!page)
		return;

	// If using renderer, composite the main layer to GPU. For CPU path, the surface passed to this function already has the main layer and compositing
	// happens on top of the graphics context created for this surface.
	if(renderer)
	{
		if(d->page->view()->IsUsingTiledBackingStore())
		{
			NOTIFY_PROCESS_STATUS(kVProcessTypePaintTilesGPU, EA::WebKit::kVProcessStatusStarted, d->page->view());
			int scrollX = d->frame->view()->scrollX();
			int scrollY = d->frame->view()->scrollY();

			WebCore::IntRect fullscreen(IntPoint(0, 0), WebCore::IntSize(d->page->view()->GetSize()));
			WebCore::IntRect scrolledScreen(IntPoint(scrollX, scrollY), fullscreen.size());
			EA::WebKit::TransformationMatrix identity;

			// Render the tiled backing store.
			EA::WebKit::FloatRect clipRectFloat(fullscreen.x(), fullscreen.y(), fullscreen.width(), fullscreen.height());
			EA::WebKit::IntRect clipRectInt(fullscreen.x(), fullscreen.y(), fullscreen.width(), fullscreen.height());

			//If you don't clip, tiles may overflow beyond the View bounds on scroll
			if(renderer->UseCustomClip())
				renderer->BeginClip(identity,clipRectFloat);
			else
				renderer->ScissorClip(clipRectInt); 
			
			RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data(NULL, CAIRO_FORMAT_ARGB32, 0, 0, 0)) ;
			RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

			WebCore::GraphicsContext graphicsContext(cairoContext.get());
			graphicsContext.translate(-scrollX, -scrollY);
			d->frame->tiledBackingStore()->paint(&graphicsContext/*context*/, scrolledScreen);
			
			if(renderer->UseCustomClip())
				renderer->EndClip();

			
			NOTIFY_PROCESS_STATUS(kVProcessTypePaintTilesGPU, EA::WebKit::kVProcessStatusEnded, d->page->view());
		}
		else
		{
			EA::WebKit::TransformationMatrix identity;
			EA::WebKit::FloatRect fullScreen(0.0f, 0.0f, d->page->view()->GetSize().mWidth, d->page->view()->GetSize().mHeight);
            EA::WebKit::Filters filters;
			renderer->RenderSurface(d->page->view()->GetDisplaySurface(), fullScreen, identity, 1.0f, EA::WebKit::CompositeSourceOver, EA::WebKit::ClampToEdge, filters);
		}
	}

	// Composite the auxiliary layers if we have them
	if (WebCore::TextureMapperLayerClientEA* client = static_cast<WebCore::ChromeClientEA&>(page->chrome().client()).m_textureMapperLayerClient.get())
	{
		WebCore::IntRect fullscreen(IntPoint(0, 0), WebCore::IntSize(d->page->view()->GetSize()));
		if(renderer)
		{
			// GPU compositing
			// Create a null context for the texture mapper
			RefPtr<cairo_surface_t> cairoSurface =adoptRef(cairo_image_surface_create_for_data(NULL, CAIRO_FORMAT_ARGB32, 0, 0, 0));
			RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

			WebCore::GraphicsContext graphicsContext(cairoContext.get());

			client->syncLayers();
			client->renderCompositedLayers(&graphicsContext, fullscreen);
		}
		else
		{
			//CPU compositing
			int width = 0;
			int height = 0;
			surface->GetContentDimensions(&width, &height);

			ISurface::SurfaceDescriptor surfaceDescriptor = {0};
			surface->Lock(&surfaceDescriptor);

			RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, width, height, surfaceDescriptor.mStride));    
			RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

			WebCore::GraphicsContext graphicsContext(cairoContext.get());

			client->syncLayers();
			client->renderCompositedLayers(&graphicsContext, fullscreen);

			surface->Unlock();
		}
	}
#endif
}
void WebFrame::renderScrollHelper(WebCore::Scrollbar *bar, WebCore::FrameView *view, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
	EAW_ASSERT_MSG(!(bar && view),"Only expecting either bar or view");
	EAW_ASSERT_MSG((bar || view),"Expecting at least one of the bar or view");

	bool needsPaint = false;
	if (!*surface)
	{
		*surface = renderer->CreateSurface(EA::WebKit::SurfaceTypeOverlay);
		needsPaint = true;
	}

	WebCore::IntRect boundsRect = view ? view->scrollCornerRect() : bar->frameRect();

	for (size_t i = 0; i < dirtyRegions.size(); ++i)
	{
		if (dirtyRegions[i].intersects(boundsRect))
		{
			needsPaint = true;
			break;
		}
	}

	if (needsPaint)
	{
		(*surface)->SetContentDimensions(boundsRect.width(), boundsRect.height());

		ISurface::SurfaceDescriptor desc;
		(*surface)->Lock(&desc);

		RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)desc.mData, CAIRO_FORMAT_ARGB32, boundsRect.width(), boundsRect.height(), desc.mStride));    
		RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

		WebCore::GraphicsContext graphicsContext(cairoContext.get());
		graphicsContext.translate(-boundsRect.x(), -boundsRect.y()); 
		if(view)
			view->paintScrollCorner(&graphicsContext, boundsRect);
		else
			bar->paint(&graphicsContext, boundsRect);

		(*surface)->Unlock();
	}

	EA::WebKit::TransformationMatrix identity;
	EA::WebKit::FloatRect boundsRectFloat(boundsRect.x(), boundsRect.y(), boundsRect.width(), boundsRect.height());
    EA::WebKit::Filters filters;
	renderer->RenderSurface(*surface, boundsRectFloat, identity, 1.0f, EA::WebKit::CompositeSourceOver, EA::WebKit::ClampToEdge, filters);
}
void WebFrame::renderScrollCorner(WebCore::FrameView *view, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
	if (view && view->scrollbarCornerPresent())
	{
		renderScrollHelper(0, view, renderer,surface,dirtyRegions);
	}	
}

void WebFrame::renderScrollbar(WebCore::Scrollbar *bar, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
	if (bar)
	{
		renderScrollHelper(bar, 0, renderer,surface,dirtyRegions);
	}
}

void WebFrame::drawHighlightedNodeFromInspector(EA::WebKit::ISurface* surface)
{
#if ENABLE(INSPECTOR)
	WebCore::Frame* coreFrame = d->core(this);
	if (coreFrame->page()->inspectorController()->highlightedNode()) 
	{
		int width = 0;
		int height = 0;
		surface->GetContentDimensions(&width, &height);
		
        ISurface::SurfaceDescriptor surfaceDescriptor = {0};
		surface->Lock(&surfaceDescriptor);

		RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, width, height, surfaceDescriptor.mStride));    
		RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));
		
		WebCore::GraphicsContext graphicsContext(cairoContext.get());
		coreFrame->page()->inspectorController()->drawHighlight(graphicsContext);

		surface->Unlock();
	}
#endif
}

void WebFrame::setTextSizeMultiplier(float factor)
{
    // Note: QT sets the mZoomTextEnabled param here but we  want to leave it up to the global params
    //EA::WebKit::Parameters &params = EA::WebKit::GetParameters();
    //params.mZoomTextEnabled = true;
	d->frame->setPageAndTextZoomFactors(1, factor);
}

float WebFrame::textSizeMultiplier() const
{
    return  kZoomTextOnly ? d->frame->textZoomFactor() : d->frame->pageZoomFactor();
}

void WebFrame::setZoomFactor(float factor)
{
    if (kZoomTextOnly)
		d->frame->setTextZoomFactor(factor);
	else
		d->frame->setPageZoomFactor(factor);
}

float WebFrame::zoomFactor() const
{
	return kZoomTextOnly ? d->frame->textZoomFactor() : d->frame->pageZoomFactor();
}

bool WebFrame::hasFocus() const
{
    WebCore::Frame* ff = d->frame->page()->focusController().focusedFrame();
    return ff && WebFramePrivate::kit(ff) == this;
}

void WebFrame::setFocus()
{
    WebFramePrivate::core(this)->page()->focusController().setFocusedFrame(WebFramePrivate::core(this));
}

/*!
    Returns the position of the frame relative to it's parent frame.
*/
WebCore::IntPoint WebFrame::pos() const
{
    if (!d->frame->view())
		return WebCore::IntPoint();

    return d->frame->view()->frameRect().location();
}

/*!
    Return the geometry of the frame relative to it's parent frame.
*/
WebCore::IntRect WebFrame::geometry() const
{
    if (!d->frame->view())
        return WebCore::IntRect();
    return d->frame->view()->frameRect();
}

/*!
    the size of the contents in this frame
*/
WebCore::IntSize WebFrame::contentsSize() const
{
	WebCore::FrameView *view = d->frame->view();
    if (!view)
        return WebCore::IntSize();
    return view->contentsSize();
}

/*!
    Performs a hit test on the frame contents at the given position \a pos and returns the hit test result.
*/
WebHitTestResult WebFrame::hitTestContent(const WebCore::IntPoint &pos, bool excludeScrollbarFromHitTest) const
{
    if (!d->frame->view() || !d->frame->contentRenderer())
        return WebHitTestResult();

	WebCore::HitTestResult result = d->frame->eventHandler().hitTestResultAtPoint(d->frame->view()->windowToContents(pos));

    // Added exclude scrollbar bool to allow inspector to launch even if the cursor is over a scrollbar.
    if ((excludeScrollbarFromHitTest) && (result.scrollbar()))
        return WebHitTestResult();

    return WebHitTestResult(new WebHitTestResultPrivate(result));
}



bool WebFrame::evaluateJavaScript(const WTF::String& scriptSource, JavascriptValue *resultOut)
{
    WebCore::ScriptController& scriptController = d->frame->script();
	WebCore::ScriptValue sv = scriptController.executeScript(WebCore::ScriptSourceCode(scriptSource));

	if (!sv.hasNoValue()) 
	{
		if (resultOut) 
		{
			JSC::JSValue v = sv.jsValue();
			JSC::ExecState *exec = scriptController.globalObject(WebCore::mainThreadNormalWorld())->globalExec(); 
			resultOut->SetExec(exec);
			JSC::Bindings::JStoEA(exec, v, resultOut);
		}

		return true;
	}

    return false;
}

JSC::ExecState* WebFrame::GetExec(void) 
{
    WebCore::ScriptController& scriptController = d->frame->script();
    return scriptController.globalObject(WebCore::mainThreadNormalWorld())->globalExec();
}

WebFrame::LoadState WebFrame::GetLoadState() const
{
	return d->mLoadState;
}

void WebFrame::SetLoadState(LoadState loadState)
{
	d->mLoadState = loadState;
}

WebCore::Frame* WebFramePrivate::core(const WebFrame* webFrame)
{
	return webFrame->d->frame;
}

WebFrame* WebFramePrivate::kit(const WebCore::Frame* coreFrame)
{
	if(coreFrame->loader().networkingContext())
		return static_cast<WebFrame*>(coreFrame->loader().networkingContext()->originatingObject());
	return NULL;
}

WebFrame* WebFramePrivate::kit(const WebCore::MainFrame& coreFrame)
{
	if(coreFrame.loader().networkingContext())
		return static_cast<WebFrame*>(coreFrame.loader().networkingContext()->originatingObject());
	return NULL;
}

WebHitTestResult::WebHitTestResult(WebHitTestResultPrivate *priv)
: d(priv)
{
}
WebHitTestResultPrivate::WebHitTestResultPrivate(const WebCore::HitTestResult &hitTest)
: isContentEditable(false)
, isContentSelected(false)
, isScrollBar(false)
{
	if (!hitTest.innerNode())
		return;
	// Leaving some example code here for now. We may end up resurrecting it through some other work we have in the 
	// dev plan.
#if 0
	pos = hitTest.point();
	WebCore::TextDirection dir;
	title = hitTest.title(dir);
	linkText = hitTest.textContent();
	linkUrl = hitTest.absoluteLinkURL();
	linkTitle = hitTest.titleDisplayString();
	alternateText = hitTest.altDisplayString();
	imageUrl = hitTest.absoluteImageURL();
#endif
	innerNode = hitTest.innerNode();
	innerNonSharedNode = hitTest.innerNonSharedNode();

	isContentEditable = hitTest.isContentEditable();
	isContentSelected = hitTest.isSelected();
	isScrollBar = hitTest.scrollbar();

	if (innerNonSharedNode && innerNonSharedNode->document().frame())
		frame = WebFramePrivate::kit(innerNonSharedNode->document().frame());

}







WebHitTestResult::WebHitTestResult()
: d(0)
{
}

WebHitTestResult::WebHitTestResult(const WebHitTestResult &other)
: d(0)
{
	if (other.d)
		d = new WebHitTestResultPrivate(*other.d);
}

WebHitTestResult &WebHitTestResult::operator=(const WebHitTestResult &other)
{
	if (this != &other) {
		if (other.d) {
			if (!d)
				d = new WebHitTestResultPrivate;
			*d = *other.d;
		} else {
			delete d;
			d = 0;
		}
	}
	return *this;
}

WebHitTestResult::~WebHitTestResult()
{
	delete d;
}

bool WebHitTestResult::isNull() const
{
	return !d;
}

WebFrame *WebHitTestResult::frame() const
{
	if (!d)
		return 0;
	return d->frame;
}

WebCore::Node* WebHitTestResult::node() const
{
	if (!d)
		return 0;
	return d->innerNode.get();
}

}
}

