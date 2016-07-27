/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
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
#include "Pasteboard.h"

#include "DocumentFragment.h"
#include "Editor.h"
#include "Frame.h"
#include "Image.h"
#include "markup.h"
#include "RenderImage.h"
#include "NotImplemented.h"
#include <WebFrame.h>
#include <WebFrame_p.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
namespace WebCore {

PassOwnPtr<Pasteboard> Pasteboard::createForCopyAndPaste()
{
	OwnPtr<Pasteboard> pasteboard = adoptPtr(new Pasteboard);
	return pasteboard.release();
}

PassOwnPtr<Pasteboard> Pasteboard::createPrivate()
{
	//No "Private pasteboard" concept.
	return createForCopyAndPaste();
} 

Pasteboard::Pasteboard()
{
}

void Pasteboard::writeSelection(Range& /*selectedRange*/, bool /*canSmartCopyOrDelete*/, Frame& frame, ShouldSerializeSelectedTextForClipboard)
{
    String text = frame.editor().selectedText();

    //EAWebKitTODO: CannotSmartReplace seems safer, but maybe it's wrong - in any case currently ignored in writePlainText
    return writePlainText(text, CannotSmartReplace);
}

bool Pasteboard::canSmartReplace()
{
    return false;
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame& frame, Range& context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    return 0;
}

void Pasteboard::read(PasteboardPlainText& text)
{
    //EAWebKitTODO - in testing we may notice a problem because we're no longer specifying a view or user data when reading plain text
    using namespace EA::WebKit;
    if(EAWebKitClient* const pClient = GetEAWebKitClient()) 
    {
        ClipboardEventInfo cei;
        cei.mpView = nullptr;
        cei.mpUserData = nullptr;
        cei.mType = kClipBoardEventReadPlainText;
        pClient->ClipboardEvent(cei); 
        if(!GetFixedString(cei.mText)->empty())
            text.text = WTF::String(GetFixedString(cei.mText)->c_str());
    }
}

void Pasteboard::read(PasteboardWebContentReader&)
{
	notImplemented();
}

void Pasteboard::writePlainText(const String& text, SmartReplaceOption)
{
    using namespace EA::WebKit;
    if(EAWebKitClient* const pClient = GetEAWebKitClient()) 
    {
        ClipboardEventInfo cei;
        cei.mpView = NULL;      // We don't have any view info at this level.
        cei.mpUserData = NULL;  // Since we have no view, no user data.
        cei.mType = kClipBoardEventWritePlainText;
        GetFixedString(cei.mText)->assign(text.characters(), text.length());
        pClient->ClipboardEvent(cei); 
    }
}

void Pasteboard::writeImage(Element& element, const URL&, const String&)
{
    ASSERT(element.renderer() && element.renderer()->isImage());
}

bool Pasteboard::hasData()
{
	return false;
}

Vector<String> Pasteboard::types() 
{
	return Vector<String>();
}

String Pasteboard::readString(const String& type)
{
	return "";
}

bool Pasteboard::writeString(const String& /*type*/, const String& data)
{
	writePlainText(data, CannotSmartReplace);
	return true;
}

void Pasteboard::clear()
{
	
}

//EAWebKitTODO - the no param version has no implementation, but maybe this one should have one
void Pasteboard::clear(const String& type)
{

}

Vector<String> Pasteboard::readFilenames()
{
	Vector<String> fileNames;
	notImplemented();
	return fileNames;
}

#if ENABLE(DRAG_SUPPORT)
PassOwnPtr<Pasteboard> Pasteboard::createForDragAndDrop()
{
	notImplemented();
	return adoptPtr(new Pasteboard());
}

// static
PassOwnPtr<Pasteboard> Pasteboard::createForDragAndDrop(const DragData& dragData)
{
	notImplemented();
	return adoptPtr(new Pasteboard());
}

void Pasteboard::setDragImage(DragImageRef, const IntPoint&)
{
	
}

#endif

void Pasteboard::write(const PasteboardURL& pasteboardURL)
{

}

void Pasteboard::writePasteboard(const Pasteboard& sourcePasteboard)
{
	notImplemented();
}
}
