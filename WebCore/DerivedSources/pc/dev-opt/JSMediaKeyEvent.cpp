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

#if ENABLE(ENCRYPTED_MEDIA)

#include "JSMediaKeyEvent.h"

#include "JSDOMBinding.h"
#include "JSDictionary.h"
#include "JSMediaKeyError.h"
#include "MediaKeyError.h"
#include "MediaKeyEvent.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsMediaKeyEventKeySystem(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventSessionId(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventInitData(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventMessage(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventDefaultURL(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventErrorCode(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventSystemCode(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMediaKeyEventConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSMediaKeyEventPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSMediaKeyEventPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSMediaKeyEventPrototype* ptr = new (NotNull, JSC::allocateCell<JSMediaKeyEventPrototype>(vm.heap)) JSMediaKeyEventPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSMediaKeyEventPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSMediaKeyEventConstructor : public DOMConstructorObject {
private:
    JSMediaKeyEventConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSMediaKeyEventConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSMediaKeyEventConstructor* ptr = new (NotNull, JSC::allocateCell<JSMediaKeyEventConstructor>(vm.heap)) JSMediaKeyEventConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
protected:
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSMediaKeyEvent(JSC::ExecState*);
    static JSC::ConstructType getConstructData(JSC::JSCell*, JSC::ConstructData&);
};

EncodedJSValue JSC_HOST_CALL JSMediaKeyEventConstructor::constructJSMediaKeyEvent(ExecState* exec)
{
    auto* jsConstructor = jsCast<JSMediaKeyEventConstructor*>(exec->callee());

    ScriptExecutionContext* executionContext = jsConstructor->scriptExecutionContext();
    if (!executionContext)
        return throwVMError(exec, createReferenceError(exec, "Constructor associated execution context is unavailable"));

    AtomicString eventType = exec->argument(0).toString(exec)->toAtomicString(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());

    MediaKeyEventInit eventInit;

    JSValue initializerValue = exec->argument(1);
    if (!initializerValue.isUndefinedOrNull()) {
        // Given the above test, this will always yield an object.
        JSObject* initializerObject = initializerValue.toObject(exec);

        // Create the dictionary wrapper from the initializer object.
        JSDictionary dictionary(exec, initializerObject);

        // Attempt to fill in the EventInit.
        if (!fillMediaKeyEventInit(eventInit, dictionary))
            return JSValue::encode(jsUndefined());
    }

    RefPtr<MediaKeyEvent> event = MediaKeyEvent::create(eventType, eventInit);
    return JSValue::encode(toJS(exec, jsConstructor->globalObject(), event.get()));
}

bool fillMediaKeyEventInit(MediaKeyEventInit& eventInit, JSDictionary& dictionary)
{
    if (!fillEventInit(eventInit, dictionary))
        return false;

    if (!dictionary.tryGetProperty("keySystem", eventInit.keySystem))
        return false;
    if (!dictionary.tryGetProperty("sessionId", eventInit.sessionId))
        return false;
    if (!dictionary.tryGetProperty("initData", eventInit.initData))
        return false;
    if (!dictionary.tryGetProperty("message", eventInit.message))
        return false;
    if (!dictionary.tryGetProperty("defaultURL", eventInit.defaultURL))
        return false;
    if (!dictionary.tryGetProperty("errorCode", eventInit.errorCode))
        return false;
    if (!dictionary.tryGetProperty("systemCode", eventInit.systemCode))
        return false;
    return true;
}

const ClassInfo JSMediaKeyEventConstructor::s_info = { "MediaKeyEventConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSMediaKeyEventConstructor) };

JSMediaKeyEventConstructor::JSMediaKeyEventConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSMediaKeyEventConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSMediaKeyEvent::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("MediaKeyEvent"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(1), ReadOnly | DontEnum);
}

ConstructType JSMediaKeyEventConstructor::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructJSMediaKeyEvent;
    return ConstructTypeHost;
}

/* Hash table for prototype */

static const HashTableValue JSMediaKeyEventPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "keySystem", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventKeySystem), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "sessionId", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventSessionId), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "initData", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventInitData), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "message", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventMessage), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "defaultURL", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventDefaultURL), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "errorCode", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventErrorCode), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "systemCode", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyEventSystemCode), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSMediaKeyEventPrototype::s_info = { "MediaKeyEventPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSMediaKeyEventPrototype) };

void JSMediaKeyEventPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSMediaKeyEventPrototypeTableValues, *this);
}

const ClassInfo JSMediaKeyEvent::s_info = { "MediaKeyEvent", &Base::s_info, 0, CREATE_METHOD_TABLE(JSMediaKeyEvent) };

JSMediaKeyEvent::JSMediaKeyEvent(Structure* structure, JSDOMGlobalObject* globalObject, Ref<MediaKeyEvent>&& impl)
    : JSEvent(structure, globalObject, WTF::move(impl))
{
}

JSObject* JSMediaKeyEvent::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSMediaKeyEventPrototype::create(vm, globalObject, JSMediaKeyEventPrototype::createStructure(vm, globalObject, JSEvent::getPrototype(vm, globalObject)));
}

JSObject* JSMediaKeyEvent::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSMediaKeyEvent>(vm, globalObject);
}

EncodedJSValue jsMediaKeyEventKeySystem(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "keySystem");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "keySystem");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.keySystem());
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventSessionId(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "sessionId");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "sessionId");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.sessionId());
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventInitData(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "initData");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "initData");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.initData()));
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventMessage(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "message");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "message");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.message()));
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventDefaultURL(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "defaultURL");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "defaultURL");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.defaultURL());
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventErrorCode(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "errorCode");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "errorCode");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.errorCode()));
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventSystemCode(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMediaKeyEvent* castedThis = jsDynamicCast<JSMediaKeyEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMediaKeyEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MediaKeyEvent", "systemCode");
        return throwGetterTypeError(*exec, "MediaKeyEvent", "systemCode");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.systemCode());
    return JSValue::encode(result);
}


EncodedJSValue jsMediaKeyEventConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSMediaKeyEventPrototype* domObject = jsDynamicCast<JSMediaKeyEventPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSMediaKeyEvent::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSMediaKeyEvent::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSMediaKeyEventConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(ENCRYPTED_MEDIA)
