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

#include "config.h"

#if ENABLE(VIDEO_TRACK)

#include "JSTextTrack.h"

#include "Element.h"
#include "Event.h"
#include "EventListener.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "JSNodeCustom.h"
#include "JSTextTrackCue.h"
#include "JSTextTrackCueList.h"
#include "TextTrack.h"
#include "TextTrackCueList.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSTextTrackTableValues[] =
{
    { "kind", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackKind), (intptr_t)0 },
    { "label", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackLabel), (intptr_t)0 },
    { "language", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackLanguage), (intptr_t)0 },
    { "mode", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackMode), (intptr_t)setJSTextTrackMode },
    { "cues", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackCues), (intptr_t)0 },
    { "activeCues", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackActiveCues), (intptr_t)0 },
    { "oncuechange", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackOncuechange), (intptr_t)setJSTextTrackOncuechange },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTextTrackTable = { 18, 15, JSTextTrackTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSTextTrackConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTextTrackConstructorTable = { 1, 0, JSTextTrackConstructorTableValues, 0 };
const ClassInfo JSTextTrackConstructor::s_info = { "TextTrackConstructor", &Base::s_info, &JSTextTrackConstructorTable, 0, CREATE_METHOD_TABLE(JSTextTrackConstructor) };

JSTextTrackConstructor::JSTextTrackConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTextTrackConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTextTrackPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTextTrackConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTextTrackConstructor, JSDOMWrapper>(exec, JSTextTrackConstructorTable, jsCast<JSTextTrackConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSTextTrackPrototypeTableValues[] =
{
    { "addCue", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionAddCue), (intptr_t)1 },
    { "removeCue", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionRemoveCue), (intptr_t)1 },
    { "addEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionAddEventListener), (intptr_t)2 },
    { "removeEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionRemoveEventListener), (intptr_t)2 },
    { "dispatchEvent", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionDispatchEvent), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTextTrackPrototypeTable = { 16, 15, JSTextTrackPrototypeTableValues, 0 };
const ClassInfo JSTextTrackPrototype::s_info = { "TextTrackPrototype", &Base::s_info, &JSTextTrackPrototypeTable, 0, CREATE_METHOD_TABLE(JSTextTrackPrototype) };

JSObject* JSTextTrackPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTextTrack>(vm, globalObject);
}

bool JSTextTrackPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTextTrackPrototype* thisObject = jsCast<JSTextTrackPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSTextTrackPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSTextTrack::s_info = { "TextTrack", &Base::s_info, &JSTextTrackTable, 0 , CREATE_METHOD_TABLE(JSTextTrack) };

JSTextTrack::JSTextTrack(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TextTrack> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSTextTrack::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSTextTrack::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTextTrackPrototype::create(vm, globalObject, JSTextTrackPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTextTrack::destroy(JSC::JSCell* cell)
{
    JSTextTrack* thisObject = static_cast<JSTextTrack*>(cell);
    thisObject->JSTextTrack::~JSTextTrack();
}

JSTextTrack::~JSTextTrack()
{
    releaseImplIfNotNull();
}

bool JSTextTrack::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTextTrack* thisObject = jsCast<JSTextTrack*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSTextTrack, Base>(exec, JSTextTrackTable, thisObject, propertyName, slot);
}

JSValue jsTextTrackKind(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.kind());
    return result;
}


JSValue jsTextTrackLabel(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.label());
    return result;
}


JSValue jsTextTrackLanguage(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.language());
    return result;
}


JSValue jsTextTrackMode(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.mode());
    return result;
}


JSValue jsTextTrackCues(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.cues()));
    return result;
}


JSValue jsTextTrackActiveCues(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.activeCues()));
    return result;
}


JSValue jsTextTrackOncuechange(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    TextTrack& impl = castedThis->impl();
    if (EventListener* listener = impl.oncuechange()) {
        if (const JSEventListener* jsListener = JSEventListener::cast(listener)) {
            if (JSObject* jsFunction = jsListener->jsFunction(impl.scriptExecutionContext()))
                return jsFunction;
        }
    }
    return jsNull();
}


JSValue jsTextTrackConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTextTrack* domObject = jsCast<JSTextTrack*>(asObject(slotBase));
    return JSTextTrack::getConstructor(exec->vm(), domObject->globalObject());
}

void JSTextTrack::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSTextTrack* thisObject = jsCast<JSTextTrack*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSTextTrack, Base>(exec, propertyName, value, JSTextTrackTable, thisObject, slot);
}

void setJSTextTrackMode(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(thisObject);
    TextTrack& impl = castedThis->impl();
    const String& nativeValue(value.isEmpty() ? String() : value.toString(exec)->value(exec));
    if (exec->hadException())
        return;
    impl.setMode(nativeValue);
}


void setJSTextTrackOncuechange(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(thisObject);
    TextTrack& impl = castedThis->impl();
    impl.setOncuechange(createJSAttributeEventListener(exec, value, thisObject));
}


JSValue JSTextTrack::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTextTrackConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddCue(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTextTrack::info()))
        return throwVMTypeError(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    TextTrack& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    TextTrackCue* cue(toTextTrackCue(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.addCue(cue);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveCue(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTextTrack::info()))
        return throwVMTypeError(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    TextTrack& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    TextTrackCue* cue(toTextTrackCue(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.removeCue(cue, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTextTrack::info()))
        return throwVMTypeError(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    TextTrack& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.addEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTextTrack::info()))
        return throwVMTypeError(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    TextTrack& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.removeEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)).get(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTextTrack::info()))
        return throwVMTypeError(exec);
    JSTextTrack* castedThis = jsCast<JSTextTrack*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    TextTrack& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    Event* evt(toEvent(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = jsBoolean(impl.dispatchEvent(evt, ec));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}

static inline bool isObservable(JSTextTrack* jsTextTrack)
{
    if (jsTextTrack->hasCustomProperties())
        return true;
    if (jsTextTrack->impl().hasEventListeners())
        return true;
    return false;
}

bool JSTextTrackOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSTextTrack* jsTextTrack = jsCast<JSTextTrack*>(handle.get().asCell());
    if (jsTextTrack->impl().isFiringEventListeners())
        return true;
    if (!isObservable(jsTextTrack))
        return false;
    Element* element = jsTextTrack->impl().element();
    if (!element)
        return false;
    void* root = WebCore::root(element);
    return visitor.containsOpaqueRoot(root);
}

void JSTextTrackOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTextTrack* jsTextTrack = jsCast<JSTextTrack*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTextTrack->impl(), jsTextTrack);
    jsTextTrack->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TextTrack* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTextTrack>(exec, impl))
        return result;
    ReportMemoryCost<TextTrack>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSTextTrack>(exec, globalObject, impl);
}

TextTrack* toTextTrack(JSC::JSValue value)
{
    return value.inherits(JSTextTrack::info()) ? &jsCast<JSTextTrack*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(VIDEO_TRACK)
