/*
 * Copyright (C) 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "DragData.h"

#include "Document.h"
#include "DocumentFragment.h"
#include "markup.h"
#include "Range.h"

namespace WebCore {

bool DragData::canSmartReplace() const
{
    return false;
}

bool DragData::containsColor() const
{
    if (!m_platformDragData)
        return false;
    return false; 
}

bool DragData::containsFiles() const
{
    if (!m_platformDragData)
        return false;

	return false;
}

void DragData::asFilenames(Vector<String>& result) const
{
    if (!m_platformDragData)
        return;
}

bool DragData::containsPlainText() const
{
    if (!m_platformDragData)
        return false;
    
	return false; 
}

String DragData::asPlainText(Frame *frame) const
{
    if (!m_platformDragData)
        return String();
    // FIXME: Should handle rich text here
    return asURL(frame, DoNotConvertFilenames, 0);
}

Color DragData::asColor() const
{
    if (!m_platformDragData)
        return Color();
    return Color();
}

bool DragData::containsCompatibleContent() const
{
    if (!m_platformDragData)
        return false;
    return false;
}

bool DragData::containsURL(Frame*, FilenameConversionPolicy filenamePolicy) const
{
    // FIXME: Use filenamePolicy.
    if (!m_platformDragData)
        return false;
    return false;
}

String DragData::asURL(Frame*, FilenameConversionPolicy filenamePolicy, String*) const
{
    // FIXME: Use filenamePolicy.
    if (!m_platformDragData)
        return String();

	return String();
}

PassRefPtr<DocumentFragment> DragData::asFragment(Frame*, Range& context, bool allowPlainText, bool& chosePlainText) const
{
    return 0;
}

unsigned DragData::numberOfFiles() const
{
	return 0;
}

}

