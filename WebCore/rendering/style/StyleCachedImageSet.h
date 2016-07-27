/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef StyleCachedImageSet_h
#define StyleCachedImageSet_h

#if ENABLE(CSS_IMAGE_SET)

#include "CachedImageClient.h"
#include "CachedResourceHandle.h"
#include "LayoutSize.h"
#include "StyleImage.h"

namespace WebCore {

class CachedImage;
class CSSImageSetValue;

// This class keeps one cached image and has access to a set of alternatives.

class StyleCachedImageSet FINAL : public StyleImage, private CachedImageClient {
    WTF_MAKE_FAST_ALLOCATED;
public:
    static PassRefPtr<StyleCachedImageSet> create(CachedImage* image, float imageScaleFactor, CSSImageSetValue* value)
    {
        return adoptRef(new StyleCachedImageSet(image, imageScaleFactor, value));
    }
    virtual ~StyleCachedImageSet();

    virtual CachedImage* cachedImage() const OVERRIDE { return m_bestFitImage.get(); }

    void clearImageSetValue() { m_imageSetValue = nullptr; }

private:
    virtual PassRefPtr<CSSValue> cssValue() const OVERRIDE;

    // FIXME: This is used by StyleImage for equality comparison, but this implementation
    // only looks at the image from the set that we have loaded. I'm not sure if that is
    // meaningful enough or not.
    virtual WrappedImagePtr data() const OVERRIDE { return m_bestFitImage.get(); }

    virtual bool canRender(const RenderObject*, float multiplier) const OVERRIDE;
    virtual bool isLoaded() const OVERRIDE;
    virtual bool errorOccurred() const OVERRIDE;
    virtual LayoutSize imageSize(const RenderElement*, float multiplier) const OVERRIDE;
    virtual bool imageHasRelativeWidth() const OVERRIDE;
    virtual bool imageHasRelativeHeight() const OVERRIDE;
    virtual void computeIntrinsicDimensions(const RenderElement*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) OVERRIDE;
    virtual bool usesImageContainerSize() const OVERRIDE;
    virtual void setContainerSizeForRenderer(const RenderElement*, const IntSize&, float) OVERRIDE;
    virtual void addClient(RenderElement*) OVERRIDE;
    virtual void removeClient(RenderElement*) OVERRIDE;
    virtual PassRefPtr<Image> image(RenderElement*, const IntSize&) const OVERRIDE;
    virtual float imageScaleFactor() const OVERRIDE { return m_imageScaleFactor; }
    virtual bool knownToBeOpaque(const RenderElement*) const OVERRIDE;

    StyleCachedImageSet(CachedImage*, float imageScaleFactor, CSSImageSetValue*);

    CachedResourceHandle<CachedImage> m_bestFitImage;
    float m_imageScaleFactor;
    CSSImageSetValue* m_imageSetValue; // Not retained; it owns us.
};

} // namespace WebCore

#endif // ENABLE(CSS_IMAGE_SET)

#endif // StyleCachedImageSet_h
