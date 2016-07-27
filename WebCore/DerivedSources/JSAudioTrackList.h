/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

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

#ifndef JSAudioTrackList_h
#define JSAudioTrackList_h

#if ENABLE(VIDEO_TRACK)

#include "AudioTrackList.h"
#include "JSDOMBinding.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSAudioTrackList : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSAudioTrackList* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<AudioTrackList> impl)
    {
        JSAudioTrackList* ptr = new (NotNull, JSC::allocateCell<JSAudioTrackList>(globalObject->vm().heap)) JSAudioTrackList(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static bool getOwnPropertySlotByIndex(JSC::JSObject*, JSC::ExecState*, unsigned propertyName, JSC::PropertySlot&);
    static void put(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName, JSC::JSValue, JSC::PutPropertySlot&);
    static void destroy(JSC::JSCell*);
    ~JSAudioTrackList();
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static void getOwnPropertyNames(JSC::JSObject*, JSC::ExecState*, JSC::PropertyNameArray&, JSC::EnumerationMode mode = JSC::ExcludeDontEnumProperties);
    static void visitChildren(JSCell*, JSC::SlotVisitor&);

    AudioTrackList& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    AudioTrackList* m_impl;
protected:
    JSAudioTrackList(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<AudioTrackList>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetPropertyNames | JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | JSC::OverridesVisitChildren | Base::StructureFlags;
    static JSC::JSValue indexGetter(JSC::ExecState*, JSC::JSValue, unsigned);
};

class JSAudioTrackListOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, AudioTrackList*)
{
    DEFINE_STATIC_LOCAL(JSAudioTrackListOwner, jsAudioTrackListOwner, ());
    return &jsAudioTrackListOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, AudioTrackList*)
{
    return &world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, AudioTrackList*);
AudioTrackList* toAudioTrackList(JSC::JSValue);

class JSAudioTrackListPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSAudioTrackListPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSAudioTrackListPrototype* ptr = new (NotNull, JSC::allocateCell<JSAudioTrackListPrototype>(vm.heap)) JSAudioTrackListPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSAudioTrackListPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::OverridesVisitChildren | Base::StructureFlags;
};

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsAudioTrackListPrototypeFunctionItem(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsAudioTrackListPrototypeFunctionGetTrackById(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsAudioTrackListPrototypeFunctionAddEventListener(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsAudioTrackListPrototypeFunctionRemoveEventListener(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsAudioTrackListPrototypeFunctionDispatchEvent(JSC::ExecState*);
// Attributes

JSC::JSValue jsAudioTrackListLength(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsAudioTrackListOnchange(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSAudioTrackListOnchange(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsAudioTrackListOnaddtrack(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSAudioTrackListOnaddtrack(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsAudioTrackListOnremovetrack(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSAudioTrackListOnremovetrack(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);

} // namespace WebCore

#endif // ENABLE(VIDEO_TRACK)

#endif
