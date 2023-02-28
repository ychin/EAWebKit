/*
 * Copyright (C) 2006 Lars Knoll <lars@trolltech.com>
 * Copyright (C) 2011, 2012, 2015, 2017 Electronic Arts, Inc. All rights reserved.
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
#include <string>
#include "TextBreakIterator.h"
#include "TextEncoding.h"
#include "TextBoundaries.h"
#include <wtf/Atomics.h>
#include <wtf/MainThread.h>

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebKit_p.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKitString.h>


// Most of these APIs from webcore are not documented at all. The comments below come after reading the ICU based 
// implementation of these functions.

namespace WebCore 
{

static_assert(EA::WebKit::TextBreakDone == WebCore::TextBreakDone, "TextBreakDone mismatch !");

class TextBreakIterator
{
public:
	void* mpBreakIterator;
	eastl::string16 mBreakIteratorText;
};

static TextBreakIterator swordBreakIterator;
static TextBreakIterator sSentenceBreakIterator;
static TextBreakIterator sCursorMovementIterator;


// WebCore expects only one iterator to exist at a time. If it wants to change the text for the iterator, this function is called with the new text run info.
// The implication is that we only need to maintain one word break iterator.
TextBreakIterator* wordBreakIterator(StringView string)
{
	swordBreakIterator.mBreakIteratorText.assign(string.upconvertedCharacters(), string.length());
	swordBreakIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->WordBreakIterator((EA::WebKit::Char *)swordBreakIterator.mBreakIteratorText.c_str(), swordBreakIterator.mBreakIteratorText.length());
	return &swordBreakIterator;
}

// WebCore expects only one iterator to exist at a time. If it wants to change the text for the iterator, this function is called with the new text run info.
// The implication is that we only need to maintain one sentence break iterator.
TextBreakIterator* sentenceBreakIterator(StringView string)
{
	sSentenceBreakIterator.mBreakIteratorText.assign(string.upconvertedCharacters(), string.length());
	sSentenceBreakIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->SentenceBreakIterator((EA::WebKit::Char *) sSentenceBreakIterator.mBreakIteratorText.c_str(), sSentenceBreakIterator.mBreakIteratorText.length());
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

TextBreakIterator* acquireLineBreakIterator(StringView string, const AtomicString& locale, const UChar* priorContext, unsigned priorContextLength, LineBreakIteratorMode mode, bool isCJK)
{
    //Ignoring mode and isCJK - Chinese/Japanese/Korean
    (void)locale;

    TextBreakIterator *newIterator = new TextBreakIterator();
    if (!newIterator)
       return nullptr;
        
    if (priorContextLength > 0)
    {
		StringView::UpconvertedCharacters stringU = string.upconvertedCharacters();
        const UChar* stringUChar = stringU.get();
        int stringLength = std::char_traits<UChar>::length(stringUChar);
        int totalLength = stringLength + priorContextLength;
        UChar* totalString = new UChar[totalLength];

        for (int i = 0; i < priorContextLength; ++i)
        {
            totalString[i] = priorContext[i];
        }

        for (int i = 0; i < stringLength; ++i)
        {
            totalString[i + priorContextLength] = stringUChar[i];
        }
        newIterator->mBreakIteratorText.assign(totalString, totalLength);
        delete[] totalString;
    }
    else
    {
        newIterator->mBreakIteratorText.assign(string.upconvertedCharacters().get(), string.length());
    }

	newIterator->mpBreakIterator = EA::WebKit::GetTextSystem()->AcquireLineBreakIterator((EA::WebKit::Char *)newIterator->mBreakIteratorText.c_str(), newIterator->mBreakIteratorText.length());
	return newIterator;
}

void releaseLineBreakIterator(TextBreakIterator* pIterator)
{
	EA::WebKit::GetTextSystem()->ReleaseLineBreakIterator(pIterator->mpBreakIterator);

	pIterator->mBreakIteratorText.clear();

	delete pIterator;
}

// Recognize BCP47 compliant primary language values of 'zh', 'ja', 'ko'
// (in any combination of case), optionally followed by subtags. Don't
// recognize 3-letter variants 'chi'/'zho', 'jpn', or 'kor' since BCP47
// requires use of shortest language tag.
bool isCJKLocale(const AtomicString& locale)
{
	size_t length = locale.length();
	if (length < 2)
		return false;
	auto c1 = locale[0];
	auto c2 = locale[1];
	auto c3 = length == 2 ? 0 : locale[2];
	if (!c3 || c3 == '-' || c3 == '_' || c3 == '@') {
		if (c1 == 'z' || c1 == 'Z')
			return c2 == 'h' || c2 == 'H';
		if (c1 == 'j' || c1 == 'J')
			return c2 == 'a' || c2 == 'A';
		if (c1 == 'k' || c1 == 'K')
			return c2 == 'o' || c2 == 'O';
	}
	return false;
}

//EAWebKitToDo: It is not entirely clear if this code is doing the right thing. Also, because our text implementation keeps a shared iterator anyway.
static TextBreakIterator* nonSharedCharacterBreakIterator; 
NonSharedCharacterBreakIterator::NonSharedCharacterBreakIterator(StringView string)
{
	EAW_ASSERT_MSG(WTF::isMainThread(),"We don't support multiple threads for this function at the moment");

	m_iterator = nonSharedCharacterBreakIterator;
	bool createdIterator = m_iterator && WTF::weakCompareAndSwap(reinterpret_cast<void**>(&nonSharedCharacterBreakIterator), m_iterator, 0);
	if (!createdIterator)
		m_iterator = new TextBreakIterator();

	m_iterator->mBreakIteratorText.assign(string.upconvertedCharacters(), string.length());
	m_iterator->mpBreakIterator = EA::WebKit::GetTextSystem()->CharacterBreakIterator((EA::WebKit::Char*)m_iterator->mBreakIteratorText.c_str(), m_iterator->mBreakIteratorText.length());
}

NonSharedCharacterBreakIterator::~NonSharedCharacterBreakIterator()
{
	m_iterator->mBreakIteratorText.clear();
	if (!WTF::weakCompareAndSwap(reinterpret_cast<void**>(&nonSharedCharacterBreakIterator), 0, m_iterator))
		delete m_iterator;
}


TextBreakIterator* cursorMovementIterator(const UChar* string, int length)
{
	sCursorMovementIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->CursorBreakIterator((EA::WebKit::Char *) string, length);
	return &sCursorMovementIterator;
}

TextBreakIterator* cursorMovementIterator(StringView string)
{
	sCursorMovementIterator.mBreakIteratorText.assign(string.upconvertedCharacters(), string.length());
	sCursorMovementIterator.mpBreakIterator = EA::WebKit::GetTextSystem()->CursorBreakIterator((EA::WebKit::Char *) sCursorMovementIterator.mBreakIteratorText.c_str(), sCursorMovementIterator.mBreakIteratorText.length());
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

int findNextWordFromIndex(StringView text, int position, bool forward)
{
    return findNextWordFromIndex(text.upconvertedCharacters(), text.length(), position, forward);
}

void findWordBoundary(StringView text, int position, int* start, int* end)
{
    findWordBoundary(text.upconvertedCharacters(), text.length(), position, start, end);
}

void findEndWordBoundary(StringView text, int position, int* end)
{
	int start = 0;
	findWordBoundary(text.upconvertedCharacters(), text.length(), position, &start, end);
}

unsigned numGraphemeClusters(const String& s)
{
	unsigned stringLength = s.length();

	if (!stringLength)
		return 0;

	// The only Latin-1 Extended Grapheme Cluster is CR LF
	if (s.is8Bit() && !s.contains('\r'))
		return stringLength;

	NonSharedCharacterBreakIterator it(s);
	if (!it)
		return stringLength;

	unsigned num = 0;
	while (textBreakNext(it) != TextBreakDone)
		++num;
	return num;
}

unsigned numCharactersInGraphemeClusters(const StringView& s, unsigned numGraphemeClusters)
{
	unsigned stringLength = s.length();

	if (!stringLength)
		return 0;

	// The only Latin-1 Extended Grapheme Cluster is CR LF
	if (s.is8Bit() && !s.contains('\r'))
		return std::min(stringLength, numGraphemeClusters);

	NonSharedCharacterBreakIterator it(s);
	if (!it)
		return std::min(stringLength, numGraphemeClusters);

	for (unsigned i = 0; i < numGraphemeClusters; ++i) {
		if (textBreakNext(it) == TextBreakDone)
			return stringLength;
	}
	return textBreakCurrent(it);
}


}
