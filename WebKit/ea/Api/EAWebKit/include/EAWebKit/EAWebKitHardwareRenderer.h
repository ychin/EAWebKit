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

#ifndef EAWEBKIT_HARDWARE_RENDERER_H
#define EAWEBKIT_HARDWARE_RENDERER_H

#include <EAWebKit/EAWebkitSystem.h>

namespace EA { namespace WebKit 
{
    class ISurface;

	enum CompositOperator
	{
		CompositeSourceOver, 
		CompositeDestinationIn
	};

	enum TextureWrapMode 
	{
		ClampToEdge,
		Repeat
	};

	enum ClearFlags
	{
		ClearColorBit		= 1 << 0,
		ClearDepthBit		= 1 << 1, // Currently unused
		ClearStencilBit		= 1 << 2  // Only used if interface implements the scissor+stencil based clipping. Not used if using custom clipping. See below for more details.
	};

	// SurfaceType is exposed to help integrators implement any possible optimization. If the surface type is either Tile or Texture, the allocation and lock/unlock operations on it are within
	// View::Paint().
	enum SurfaceType
	{
		SurfaceTypeMain,
		SurfaceTypeTile,
		SurfaceTypeTexture,
		SurfaceTypeOverlay
	};

	// The css filter operation to apply on the surface/texture 
	enum FilterType 
	{
		FilterTypeNone,
        FilterTypeGrayscale,
		FilterTypeSepia,
		FilterTypeSaturate,
		FilterTypeHueRotate,
		FilterTypeInvert,
		FilterTypeOpacity,
		FilterTypeBrightness,
		FilterTypeContrast,
		FilterTypeBlur,
		FilterTypeDropShadow,
	};
	
	// The parameters for the filter operation. Not all fields may be required for all operations. 
	struct FilterInfo
	{
		FilterType filterType;
		float amount;
        uint32_t color;
        IntPoint location;
		
        FilterInfo()
			: filterType(EA::WebKit::FilterType::FilterTypeNone)
			, amount(0.0f)
            , color(0)
            , location(0,0)
		{

		}
	};

    class Filters
    {
    public:
        Filters()
            : mIndex(0)
        {

        }

        FilterInfo Get(uint32_t index) const { if (mIndex < kMaxFilters) { return mFilterList[index]; } return FilterInfo(); }
        void Add(FilterInfo info) { if (mIndex < kMaxFilters) {mFilterList[mIndex] = info;  mIndex++;} }
        void Reset() {mIndex = 0;}  //doesn't actually clear the data but sets the count back to 0
        uint32_t Count() const { return mIndex; }
        uint32_t CountMax() const { return kMaxFilters; }
    
    private:
        static const uint32_t kMaxFilters = 16;
        uint32_t mIndex;
        FilterInfo mFilterList[kMaxFilters];
    };

    // Implement this class and pass it through the view's initialization
    // parameters to enable GPU compositing.
    // You will also need to implement a version of the ISurface class that can work with the 
    // renderer.

    class IHardwareRenderer
    {
    public:
        virtual ~IHardwareRenderer(void) {}

        // You should return a new instance of your Hardware ISurface implementation.
        // Note that there is no 'DestroySurface' call, so it's up to the ISurface to nuke itself
        // when ISurface::Release is called.
        virtual EA::WebKit::ISurface *CreateSurface(EA::WebKit::SurfaceType surfaceType) = 0;

        // If target is null, render to screen(back buffer)
        // For non null target, render to the target surface(texture) 
        virtual void SetRenderTarget(EA::WebKit::ISurface *target) = 0;

		// Render the surface on to currently bound render target.
		// target specifies the location and size of the surface to be rendered. We always draw full texture so target.size is same as surface.size. 
		// matrix specifies any additional translation and rotation of the surface to be rendered in the containing view coordinates. The translation
		// may cause the part of/full surface to get clipped.
        virtual void RenderSurface(EA::WebKit::ISurface *surface, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, 
									float opacity, EA::WebKit::CompositOperator op, EA::WebKit::TextureWrapMode wrap, Filters &filters) = 0;
		// Fill the color on to the currently bound render target. Note that the color value has alpha pre-multiplied.
		virtual void FillColor(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, EA::WebKit::CompositOperator op) = 0;
		// Draw the outline on to the currently bound render target. Note that the color value has alpha pre-multiplied.
		// The outline API is used only for debugging at the time of writing (08/05/2014).
		virtual void DrawOutline(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix) = 0;

		// Use this to optimize your texture size. If a composited layer is bigger than this size, the layer is broken down into
		// the tiles of this size.
		virtual int32_t MaxTextureSize(void) = 0;

		// Clear the buffers indicated by the flags. 
		virtual void Clear(ClearFlags flags, uint32_t premultiplied_rgba32, float z, uint32_t stencil) = 0;

		//+ Clipping API
		// The clipping API can be implemented one of the two ways.
		// 1. You can take advantage of the internal clipping state management which significantly reduces the complexity. Note that clipping
		// calls are bound to the current render target which can be back buffer or a texture. This needs to be handled appropriately.
		// The internally managed clipping is based on a combination of scissors and stencil buffer. The IHardwareRenderer implementation needs
		// to simply translate these APIs to corresponding GPU calls.
		
		// Clip the rectangular region specified by the rect. The rect is guaranteed to be axis aligned so no check it necessary.
		virtual void ScissorClip(EA::WebKit::IntRect axisAlignedRect) = 0;
		// Draw at the currently bound stencil buffer at index specified by stencilIndex. This will serve as the region to clip against. 
		virtual void DrawStencil(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target, uint32_t stencilIndex) = 0;
		// Actually clip against the previously drawn stencil. Clip against all bit planes that were drawn previously.
		// Note that this call just prepares the stencilIndex to be used. If there is not DrawStencil call as a follow up, the buffer at
		// the new stencilIndex is not used at all(meaning stencil based clipping is not kicked in/required). 
		virtual void ClipAgainstStencil(uint32_t stencilIndex) = 0;

		// 2. If you'd rather implement clipping yourself, indicate so by returning true for UseCustomClip API.
		// Then you implement BeginClip/EndClip/CurrentClipBound. Note that these clip APIs should take into account currently bound
		// render target.
		virtual bool UseCustomClip() = 0;
		virtual void BeginClip(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target) = 0;
		virtual void EndClip(void) = 0;
		virtual EA::WebKit::IntRect CurrentClipBound() = 0;

		//-ClippingAPI

		// These are here in case you need to be notified that the renderer is about to start or stop painting.
		virtual void BeginPainting(void) {}
		virtual void EndPainting(void) {}

    };
}}

#endif
