/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2011, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * All rights reserved.
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

#ifndef EditorClientEA_h
#define EditorClientEA_h

#include "EditorClient.h"
#include <wtf/RefCounted.h>

#include <wtf/Forward.h>

namespace EA
{
namespace WebKit
{
class WebPage;
}
}

namespace WebCore {

class EditorClientEA : public EditorClient {
public:
	EditorClientEA(EA::WebKit::WebPage* page);
    
    virtual void pageDestroyed() OVERRIDE;
    
    virtual bool shouldDeleteRange(Range*) OVERRIDE;
    virtual bool smartInsertDeleteEnabled() OVERRIDE; 
    virtual bool isSelectTrailingWhitespaceEnabled() OVERRIDE; 
    virtual bool isContinuousSpellCheckingEnabled() OVERRIDE;
    virtual void toggleContinuousSpellChecking() OVERRIDE;
    virtual bool isGrammarCheckingEnabled() OVERRIDE;
    virtual void toggleGrammarChecking() OVERRIDE;
    virtual int spellCheckerDocumentTag() OVERRIDE;

    virtual bool shouldBeginEditing(Range*) OVERRIDE;
    virtual bool shouldEndEditing(Range*) OVERRIDE;
    virtual bool shouldInsertNode(Node*, Range*, EditorInsertAction) OVERRIDE;
    virtual bool shouldInsertText(const String&, Range*, EditorInsertAction) OVERRIDE;
    virtual bool shouldChangeSelectedRange(Range* fromRange, Range* toRange, EAffinity, bool stillSelecting) OVERRIDE;

    virtual bool shouldApplyStyle(StylePropertySet*, Range*) OVERRIDE;

    virtual bool shouldMoveRangeAfterDelete(Range*, Range*) OVERRIDE;

    virtual void didBeginEditing() OVERRIDE;
    virtual void respondToChangedContents() OVERRIDE;
    virtual void respondToChangedSelection(Frame*) OVERRIDE;
    virtual void didEndEditing() OVERRIDE;
    virtual void willWriteSelectionToPasteboard(Range*) OVERRIDE;
    virtual void didWriteSelectionToPasteboard() OVERRIDE;
    virtual void getClientPasteboardDataForRange(Range*, Vector<String>& pasteboardTypes, Vector<RefPtr<WebCore::SharedBuffer> >& pasteboardData) OVERRIDE;
    
    virtual void registerUndoStep(PassRefPtr<UndoStep>) OVERRIDE;
    virtual void registerRedoStep(PassRefPtr<UndoStep>) OVERRIDE;
    virtual void clearUndoRedoOperations() OVERRIDE;

    virtual bool canCopyCut(Frame*, bool defaultValue) const OVERRIDE;
    virtual bool canPaste(Frame*, bool defaultValue) const OVERRIDE;
    virtual bool canUndo() const OVERRIDE;
    virtual bool canRedo() const OVERRIDE;
    
    virtual void undo() OVERRIDE;
    virtual void redo() OVERRIDE;

    virtual void handleKeyboardEvent(KeyboardEvent*) OVERRIDE;
    virtual void handleInputMethodKeydown(KeyboardEvent*) OVERRIDE;

    virtual void textFieldDidBeginEditing(Element*) OVERRIDE;
    virtual void textFieldDidEndEditing(Element*) OVERRIDE;
    virtual void textDidChangeInTextField(Element*) OVERRIDE;
    virtual bool doTextFieldCommandFromEvent(Element*, KeyboardEvent*) OVERRIDE;
    virtual void textWillBeDeletedInTextField(Element*) OVERRIDE;
    virtual void textDidChangeInTextArea(Element*) OVERRIDE;

    virtual void updateSpellingUIWithGrammarString(const String&, const GrammarDetail&) OVERRIDE;
    virtual void updateSpellingUIWithMisspelledWord(const String&) OVERRIDE;
    virtual void showSpellingUI(bool show) OVERRIDE;
    virtual bool spellingUIIsShowing() OVERRIDE;
    virtual void willSetInputMethodState() OVERRIDE;
    virtual void setInputMethodState(bool enabled) OVERRIDE;
    virtual TextCheckerClient* textChecker() { return NULL; }

    virtual bool supportsGlobalSelection() OVERRIDE;

    bool isEditing() const;

    static bool dumpEditingCallbacks;
    static bool acceptsEditing;

private:
	EA::WebKit::WebPage* m_page;
    bool m_editing;
};

}

#endif

// vim: ts=4 sw=4 et
