/*
 * Copyright (C) 2009 Jian Li <jianli@chromium.org>
 * Copyright (C) 2012 Patrick Gansterer <paroga@paroga.com>
 * Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.
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
#include "ThreadSpecific.h"

#include "StdLibExtras.h"
#include "ThreadingPrimitives.h"
#include <wtf/DoublyLinkedList.h>
#include <wtf/NeverDestroyed.h>

namespace WTF {

static DoublyLinkedList<PlatformThreadSpecificKey>& destructorsList()
{
    static DoublyLinkedList<PlatformThreadSpecificKey> staticList;
    return staticList;
}

static Mutex& destructorsMutex()
{
    static NeverDestroyed<Mutex *> spdestructorsMutex = new Mutex();
	return *spdestructorsMutex;
}

class PlatformThreadSpecificKey : public DoublyLinkedListNode<PlatformThreadSpecificKey> {
public:
    friend class DoublyLinkedListNode<PlatformThreadSpecificKey>;

    PlatformThreadSpecificKey(void (*destructor)(void *))
        : m_destructor(destructor)
    {
        m_tlsKey = EA::WebKit::GetThreadSystem()->CreateAThreadLocalStorage();
    }

    ~PlatformThreadSpecificKey()
    {
		EA::WebKit::GetThreadSystem()->DestroyAThreadLocalStorage(m_tlsKey);
    }

	void setValue(void* data) { m_tlsKey->SetValue(data); }
    void* value() { return m_tlsKey->GetValue(); }

    void callDestructor()
    {
       if (void* data = value())
            m_destructor(data);
    }

private:
    void (*m_destructor)(void *);
    EA::WebKit::IThreadLocalStorage* m_tlsKey;
    PlatformThreadSpecificKey* m_prev;
    PlatformThreadSpecificKey* m_next;
};


void threadSpecificKeyCreate(ThreadSpecificKey* key, void (*destructor)(void *))
{
    *key = static_cast<PlatformThreadSpecificKey*>(fastMalloc(sizeof(PlatformThreadSpecificKey)));
    new (*key) PlatformThreadSpecificKey(destructor);

    MutexLocker locker(destructorsMutex());
    destructorsList().push(*key);
}

void threadSpecificKeyDelete(ThreadSpecificKey key)
{
    MutexLocker locker(destructorsMutex());
    destructorsList().remove(key);
    key->~PlatformThreadSpecificKey();
    fastFree(key);
}

void threadSpecificSet(ThreadSpecificKey key, void* data)
{
    key->setValue(data);
}

void* threadSpecificGet(ThreadSpecificKey key)
{
    return key->value();
}

//void ThreadSpecificThreadExit()
//{
//
//}

} // namespace WTF
