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

#include "JSVideoTrackList.h"

#include "Element.h"
#include "Event.h"
#include "EventListener.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "JSNodeCustom.h"
#include "JSVideoTrack.h"
#include "VideoTrack.h"
#include "VideoTrackList.h"
#include <runtime/Error.h>
#include <runtime/PropertyNameArray.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSVideoTrackListTableValues[] =
{
    { "length", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsVideoTrackListLength), (intptr_t)0 },
    { "onchange", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsVideoTrackListOnchange), (intptr_t)setJSVideoTrackListOnchange },
    { "onaddtrack", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsVideoTrackListOnaddtrack), (intptr_t)setJSVideoTrackListOnaddtrack },
    { "onremovetrack", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsVideoTrackListOnremovetrack), (intptr_t)setJSVideoTrackListOnremovetrack },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSVideoTrackListTable = { 9, 7, JSVideoTrackListTableValues, 0 };
/* Hash table for prototype */

static const HashTableValue JSVideoTrackListPrototypeTableValues[] =
{
    { "item", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsVideoTrackListPrototypeFunctionItem), (intptr_t)1 },
    { "getTrackById", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsVideoTrackListPrototypeFunctionGetTrackById), (intptr_t)1 },
    { "addEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsVideoTrackListPrototypeFunctionAddEventListener), (intptr_t)2 },
    { "removeEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsVideoTrackListPrototypeFunctionRemoveEventListener), (intptr_t)2 },
    { "dispatchEvent", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsVideoTrackListPrototypeFunctionDispatchEvent), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSVideoTrackListPrototypeTable = { 16, 15, JSVideoTrackListPrototypeTableValues, 0 };
const ClassInfo JSVideoTrackListPrototype::s_info = { "VideoTrackListPrototype", &Base::s_info, &JSVideoTrackListPrototypeTable, 0, CREATE_METHOD_TABLE(JSVideoTrackListPrototype) };

JSObject* JSVideoTrackListPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSVideoTrackList>(vm, globalObject);
}

bool JSVideoTrackListPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSVideoTrackListPrototype* thisObject = jsCast<JSVideoTrackListPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSVideoTrackListPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSVideoTrackList::s_info = { "VideoTrackList", &Base::s_info, &JSVideoTrackListTable, 0 , CREATE_METHOD_TABLE(JSVideoTrackList) };

JSVideoTrackList::JSVideoTrackList(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<VideoTrackList> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSVideoTrackList::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSVideoTrackList::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSVideoTrackListPrototype::create(vm, globalObject, JSVideoTrackListPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSVideoTrackList::destroy(JSC::JSCell* cell)
{
    JSVideoTrackList* thisObject = static_cast<JSVideoTrackList*>(cell);
    thisObject->JSVideoTrackList::~JSVideoTrackList();
}

JSVideoTrackList::~JSVideoTrackList()
{
    releaseImplIfNotNull();
}

bool JSVideoTrackList::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSVideoTrackList* thisObject = jsCast<JSVideoTrackList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    const HashEntry* entry = getStaticValueSlotEntryWithoutCaching<JSVideoTrackList>(exec, propertyName);
    if (entry) {
        slot.setCustom(thisObject, entry->attributes(), entry->propertyGetter());
        return true;
    }
    unsigned index = propertyName.asIndex();
    if (index != PropertyName::NotAnIndex && index < thisObject->impl().length()) {
        unsigned attributes = DontDelete | ReadOnly;
        slot.setCustomIndex(thisObject, attributes, index, indexGetter);
        return true;
    }
    return getStaticValueSlot<JSVideoTrackList, Base>(exec, JSVideoTrackListTable, thisObject, propertyName, slot);
}

bool JSVideoTrackList::getOwnPropertySlotByIndex(JSObject* object, ExecState* exec, unsigned index, PropertySlot& slot)
{
    JSVideoTrackList* thisObject = jsCast<JSVideoTrackList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index < thisObject->impl().length()) {
        unsigned attributes = DontDelete | ReadOnly;
        slot.setCustomIndex(thisObject, attributes, index, thisObject->indexGetter);
        return true;
    }
    return Base::getOwnPropertySlotByIndex(thisObject, exec, index, slot);
}

JSValue jsVideoTrackListLength(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    VideoTrackList& impl = castedThis->impl();
    JSValue result = jsNumber(impl.length());
    return result;
}


JSValue jsVideoTrackListOnchange(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    VideoTrackList& impl = castedThis->impl();
    if (EventListener* listener = impl.onchange()) {
        if (const JSEventListener* jsListener = JSEventListener::cast(listener)) {
            if (JSObject* jsFunction = jsListener->jsFunction(impl.scriptExecutionContext()))
                return jsFunction;
        }
    }
    return jsNull();
}


JSValue jsVideoTrackListOnaddtrack(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    VideoTrackList& impl = castedThis->impl();
    if (EventListener* listener = impl.onaddtrack()) {
        if (const JSEventListener* jsListener = JSEventListener::cast(listener)) {
            if (JSObject* jsFunction = jsListener->jsFunction(impl.scriptExecutionContext()))
                return jsFunction;
        }
    }
    return jsNull();
}


JSValue jsVideoTrackListOnremovetrack(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    VideoTrackList& impl = castedThis->impl();
    if (EventListener* listener = impl.onremovetrack()) {
        if (const JSEventListener* jsListener = JSEventListener::cast(listener)) {
            if (JSObject* jsFunction = jsListener->jsFunction(impl.scriptExecutionContext()))
                return jsFunction;
        }
    }
    return jsNull();
}


void JSVideoTrackList::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSVideoTrackList* thisObject = jsCast<JSVideoTrackList*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSVideoTrackList, Base>(exec, propertyName, value, JSVideoTrackListTable, thisObject, slot);
}

void setJSVideoTrackListOnchange(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(thisObject);
    VideoTrackList& impl = castedThis->impl();
    impl.setOnchange(createJSAttributeEventListener(exec, value, thisObject));
}


void setJSVideoTrackListOnaddtrack(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(thisObject);
    VideoTrackList& impl = castedThis->impl();
    impl.setOnaddtrack(createJSAttributeEventListener(exec, value, thisObject));
}


void setJSVideoTrackListOnremovetrack(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(thisObject);
    VideoTrackList& impl = castedThis->impl();
    impl.setOnremovetrack(createJSAttributeEventListener(exec, value, thisObject));
}


void JSVideoTrackList::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    JSVideoTrackList* thisObject = jsCast<JSVideoTrackList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    for (unsigned i = 0, count = thisObject->impl().length(); i < count; ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

EncodedJSValue JSC_HOST_CALL jsVideoTrackListPrototypeFunctionItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSVideoTrackList::info()))
        return throwVMTypeError(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSVideoTrackList::info());
    VideoTrackList& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.item(index)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsVideoTrackListPrototypeFunctionGetTrackById(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSVideoTrackList::info()))
        return throwVMTypeError(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSVideoTrackList::info());
    VideoTrackList& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    const String& id(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.getTrackById(id)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsVideoTrackListPrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSVideoTrackList::info()))
        return throwVMTypeError(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSVideoTrackList::info());
    VideoTrackList& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.addEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsVideoTrackListPrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSVideoTrackList::info()))
        return throwVMTypeError(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSVideoTrackList::info());
    VideoTrackList& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.removeEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)).get(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsVideoTrackListPrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSVideoTrackList::info()))
        return throwVMTypeError(exec);
    JSVideoTrackList* castedThis = jsCast<JSVideoTrackList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSVideoTrackList::info());
    VideoTrackList& impl = castedThis->impl();
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


JSValue JSVideoTrackList::indexGetter(ExecState* exec, JSValue slotBase, unsigned index)
{
    JSVideoTrackList* thisObj = jsCast<JSVideoTrackList*>(asObject(slotBase));
    ASSERT_GC_OBJECT_INHERITS(thisObj, info());
    return toJS(exec, thisObj->globalObject(), thisObj->impl().item(index));
}

static inline bool isObservable(JSVideoTrackList* jsVideoTrackList)
{
    if (jsVideoTrackList->hasCustomProperties())
        return true;
    if (jsVideoTrackList->impl().hasEventListeners())
        return true;
    return false;
}

bool JSVideoTrackListOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSVideoTrackList* jsVideoTrackList = jsCast<JSVideoTrackList*>(handle.get().asCell());
    if (jsVideoTrackList->impl().isFiringEventListeners())
        return true;
    if (!isObservable(jsVideoTrackList))
        return false;
    Element* element = jsVideoTrackList->impl().element();
    if (!element)
        return false;
    void* root = WebCore::root(element);
    return visitor.containsOpaqueRoot(root);
}

void JSVideoTrackListOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSVideoTrackList* jsVideoTrackList = jsCast<JSVideoTrackList*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsVideoTrackList->impl(), jsVideoTrackList);
    jsVideoTrackList->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7VideoTrackList@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore14VideoTrackListE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, VideoTrackList* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSVideoTrackList>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7VideoTrackList@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore14VideoTrackListE[2];
#if COMPILER(CLANG)
    // If this fails VideoTrackList does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(VideoTrackList), VideoTrackList_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // VideoTrackList has subclasses. If VideoTrackList has subclasses that get passed
    // to toJS() we currently require VideoTrackList you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<VideoTrackList>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSVideoTrackList>(exec, globalObject, impl);
}

VideoTrackList* toVideoTrackList(JSC::JSValue value)
{
    return value.inherits(JSVideoTrackList::info()) ? &jsCast<JSVideoTrackList*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(VIDEO_TRACK)
