/*
 * Copyright (C) 2006 Lars Knoll <lars@trolltech.com>
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
 

#include "config.h"
#include "TextBreakIterator.h"
#include "TextEncoding.h"
#include "TextBoundaries.h"
#include <wtf/atomics.h>
#include <wtf/MainThread.h>

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebkit_p.h>
#include <internal/include/EAWebKitAssert.h>

// Most of these APIs from webcore are not documented at all. The comments below come after reading the ICU based 
// implementation of these functions.

namespace WebCore 
{

static_assert(EA::WebKit::TextBreakDone == WebCore::TextBreakDone, "TextBreakDone mismatch !");

class TextBreakIterator
{
public:
	void* mpBreakIterator;
    void* mUCharBuffer;

    TextBreakIterator()
        : mpBreakIterator(NULL)
        , mUCharBuffer(NULL)
    {

    }

    ~TextBreakIterator()
    {
        if (mUCharBuffer)
        {
            delete[] mUCharBuffer;
            mUCharBuffer = NULL;
        }
    }
};

static TextBreakIterator swordBreakIterator;
static TextBreakIterator sSentenceBreakIterator;
static TextBreakIterator sCursorMovementIterator;

// WebCore expects only one iterator to exist at a time. If it wants to change the text for the iterator, this function is called with the new text run info.
// The implication is that we only need to maintain one word break iterator.
TextBreakIterator* wordBreakIterator(const UChar* pText, int length)
{
	swordBreakIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->WordBreakIterator((EA::WebKit::Char*) pText, length);
	return &swordBreakIterator;
}

// WebCore expects only one iterator to exist at a time. If it wants to change the text for the iterator, this function is called with the new text run info.
// The implication is that we only need to maintain one sentence break iterator.
TextBreakIterator* sentenceBreakIterator(const UChar* pText, int length)
{
	sSentenceBreakIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->SentenceBreakIterator((EA::WebKit::Char *) pText, length);
	return &sSentenceBreakIterator;
}

// This should set the iterator to the first break index that is found in the text run it is working on. 
int textBreakFirst(TextBreakIterator* pIterator)
{
	return EA::WebKit::GetTextSystem()->TextBreakFirst(pIterator->mpBreakIterator);   
}

// This should set the iterator to the index immediately beyond the last usable index. Much like a standard iterator where the end()
// is one past the last usable value.
int textBreakLast(TextBreakIterator* pIterator)
{
	return EA::WebKit::GetTextSystem()->TextBreakLast(pIterator->mpBreakIterator);   
}

// This should return the next iterator value. end() (One beyond usable index) is a legit return value much like standard iterator.
// If you have returned end() previously, any attempt to move pass it should return WebCore::TextBreakDone.
int textBreakNext(TextBreakIterator* pIterator)
{
	return EA::WebKit::GetTextSystem()->TextBreakNext(pIterator->mpBreakIterator);
}	

// This should return the previous iterator value including the first value. It returns WebCore::TextBreakDone if you try to go before the first usable index.
int textBreakPrevious(TextBreakIterator* pIterator)
{
	return EA::WebKit::GetTextSystem()->TextBreakPrevious(pIterator->mpBreakIterator);
}

// Return the most recently returned position by one of other iterators
int textBreakCurrent(TextBreakIterator* pIterator)
{
	return EA::WebKit::GetTextSystem()->TextBreakCurrent(pIterator->mpBreakIterator);
}

// Return the break position before "position". If none exists, return WebCore::TextBreakDone
int textBreakPreceding(TextBreakIterator* pIterator, int position)
{
	return EA::WebKit::GetTextSystem()->TextBreakPreceding(pIterator->mpBreakIterator, position);
}

// Return the break position after "position". If none exists, return WebCore::TextBreakDone.
int textBreakFollowing(TextBreakIterator* pIterator, int position)
{
	return EA::WebKit::GetTextSystem()->TextBreakFollowing(pIterator->mpBreakIterator, position);
}

// Returns true if the specified position is a boundary position.
// As a side effect, leaves the iterator pointing to the first boundary position at or after "offset".
bool isTextBreak(TextBreakIterator* pIterator, int position)
{
	return EA::WebKit::GetTextSystem()->IsTextBreak(pIterator->mpBreakIterator, position); 
}

bool isWordTextBreak(TextBreakIterator* pIterator)
{

	//EAWebKitTODO - https://bugs.webkit.org/show_bug.cgi?id=81136
	return true; 
}

TextBreakIterator* acquireLineBreakIterator(const UChar* pText, int length, const AtomicString& locale, const UChar* priorContext, unsigned priorContextLength)
{ 
    (void)locale;
    
    TextBreakIterator *newIterator = new TextBreakIterator();

    UChar* totalString = const_cast<UChar*>(pText);
    int totalLength = length;
    if (priorContextLength > 0)
    {
        totalLength += priorContextLength;

        UChar* totalString = new UChar[totalLength];

        for (int i = 0; i <priorContextLength; ++i)
        {
            totalString[i] = priorContext[i];
        }

        for (int i = 0; i <length; ++i)
        {
            totalString[i + priorContextLength] = pText[i];
        }

        newIterator->mUCharBuffer = totalString;
    }
   
	newIterator->mpBreakIterator = EA::WebKit::GetTextSystem()->AcquireLineBreakIterator((EA::WebKit::Char *)totalString, totalLength);
	return newIterator;
}

void releaseLineBreakIterator(TextBreakIterator* pIterator)
{
	EA::WebKit::GetTextSystem()->ReleaseLineBreakIterator(pIterator->mpBreakIterator);
	delete pIterator;
}

//EAWebKitToDo: It is not entirely clear if this code is doing the right thing. Also, because our text implementation keeps a shared iterator anyway.
static TextBreakIterator* nonSharedCharacterBreakIterator; 
NonSharedCharacterBreakIterator::NonSharedCharacterBreakIterator(const UChar* buffer, int length)
{
	EAW_ASSERT_MSG(WTF::isMainThread(),"We don't support multiple threads for this function at the moment");

	m_iterator = nonSharedCharacterBreakIterator;
	bool createdIterator = m_iterator && WTF::weakCompareAndSwap(reinterpret_cast<void**>(&nonSharedCharacterBreakIterator), m_iterator, 0);
	if (!createdIterator)
		m_iterator = new TextBreakIterator();
	
	m_iterator->mpBreakIterator = EA::WebKit::GetTextSystem()->CharacterBreakIterator((EA::WebKit::Char *) buffer, length); 
}

NonSharedCharacterBreakIterator::~NonSharedCharacterBreakIterator()
{
	if (!WTF::weakCompareAndSwap(reinterpret_cast<void**>(&nonSharedCharacterBreakIterator), 0, m_iterator))
		delete m_iterator;
}


TextBreakIterator* cursorMovementIterator(const UChar* string, int length)
{
	sCursorMovementIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->CursorBreakIterator((EA::WebKit::Char *) string, length);
	return &sCursorMovementIterator;
}

bool detectTextEncoding(const char*, size_t, const char*, WebCore::TextEncoding* detectedEncoding)
{
	*detectedEncoding = WebCore::TextEncoding();
	return false;
}

int findNextWordFromIndex(UChar const* buffer, int len, int position, bool forward)
{
	// We want to return the position that indicates start of the next word and not the end of the current word here.
	// If you do a simple GetNextBreak() on a word iterator, you may end up with the end of the current word.
	return EA::WebKit::GetTextSystem()->FindNextWordFromIndex((EA::WebKit::Char*) buffer, len, position, forward);
}

void findWordBoundary(UChar const* buffer, int len, int position, int* start, int* end)
{
	// A simple test for this function is to double click a word in rendered text and it should end up being selected.
	EA::WebKit::GetTextSystem()->FindWordBoundary((EA::WebKit::Char*) buffer, len, position, start, end);
}



}