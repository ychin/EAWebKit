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

#if ENABLE(WIRELESS_PLAYBACK_TARGET)

#include "JSWebKitPlaybackTargetAvailabilityEvent.h"

#include "JSDOMBinding.h"
#include "JSDictionary.h"
#include "URL.h"
#include "WebKitPlaybackTargetAvailabilityEvent.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsWebKitPlaybackTargetAvailabilityEventAvailability(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsWebKitPlaybackTargetAvailabilityEventConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSWebKitPlaybackTargetAvailabilityEventPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSWebKitPlaybackTargetAvailabilityEventPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSWebKitPlaybackTargetAvailabilityEventPrototype* ptr = new (NotNull, JSC::allocateCell<JSWebKitPlaybackTargetAvailabilityEventPrototype>(vm.heap)) JSWebKitPlaybackTargetAvailabilityEventPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSWebKitPlaybackTargetAvailabilityEventPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSWebKitPlaybackTargetAvailabilityEventConstructor : public DOMConstructorObject {
private:
    JSWebKitPlaybackTargetAvailabilityEventConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSWebKitPlaybackTargetAvailabilityEventConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSWebKitPlaybackTargetAvailabilityEventConstructor* ptr = new (NotNull, JSC::allocateCell<JSWebKitPlaybackTargetAvailabilityEventConstructor>(vm.heap)) JSWebKitPlaybackTargetAvailabilityEventConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
protected:
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSWebKitPlaybackTargetAvailabilityEvent(JSC::ExecState*);
    static JSC::ConstructType getConstructData(JSC::JSCell*, JSC::ConstructData&);
};

EncodedJSValue JSC_HOST_CALL JSWebKitPlaybackTargetAvailabilityEventConstructor::constructJSWebKitPlaybackTargetAvailabilityEvent(ExecState* exec)
{
    auto* jsConstructor = jsCast<JSWebKitPlaybackTargetAvailabilityEventConstructor*>(exec->callee());

    ScriptExecutionContext* executionContext = jsConstructor->scriptExecutionContext();
    if (!executionContext)
        return throwVMError(exec, createReferenceError(exec, "Constructor associated execution context is unavailable"));

    AtomicString eventType = exec->argument(0).toString(exec)->toAtomicString(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());

    WebKitPlaybackTargetAvailabilityEventInit eventInit;

    JSValue initializerValue = exec->argument(1);
    if (!initializerValue.isUndefinedOrNull()) {
        // Given the above test, this will always yield an object.
        JSObject* initializerObject = initializerValue.toObject(exec);

        // Create the dictionary wrapper from the initializer object.
        JSDictionary dictionary(exec, initializerObject);

        // Attempt to fill in the EventInit.
        if (!fillWebKitPlaybackTargetAvailabilityEventInit(eventInit, dictionary))
            return JSValue::encode(jsUndefined());
    }

    RefPtr<WebKitPlaybackTargetAvailabilityEvent> event = WebKitPlaybackTargetAvailabilityEvent::create(eventType, eventInit);
    return JSValue::encode(toJS(exec, jsConstructor->globalObject(), event.get()));
}

bool fillWebKitPlaybackTargetAvailabilityEventInit(WebKitPlaybackTargetAvailabilityEventInit& eventInit, JSDictionary& dictionary)
{
    if (!fillEventInit(eventInit, dictionary))
        return false;

    if (!dictionary.tryGetProperty("availability", eventInit.availability))
        return false;
    return true;
}

const ClassInfo JSWebKitPlaybackTargetAvailabilityEventConstructor::s_info = { "WebKitPlaybackTargetAvailabilityEventConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebKitPlaybackTargetAvailabilityEventConstructor) };

JSWebKitPlaybackTargetAvailabilityEventConstructor::JSWebKitPlaybackTargetAvailabilityEventConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSWebKitPlaybackTargetAvailabilityEventConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSWebKitPlaybackTargetAvailabilityEvent::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("WebKitPlaybackTargetAvailabilityEvent"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(1), ReadOnly | DontEnum);
}

ConstructType JSWebKitPlaybackTargetAvailabilityEventConstructor::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructJSWebKitPlaybackTargetAvailabilityEvent;
    return ConstructTypeHost;
}

/* Hash table for prototype */

static const HashTableValue JSWebKitPlaybackTargetAvailabilityEventPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitPlaybackTargetAvailabilityEventConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "availability", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitPlaybackTargetAvailabilityEventAvailability), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSWebKitPlaybackTargetAvailabilityEventPrototype::s_info = { "WebKitPlaybackTargetAvailabilityEventPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebKitPlaybackTargetAvailabilityEventPrototype) };

void JSWebKitPlaybackTargetAvailabilityEventPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSWebKitPlaybackTargetAvailabilityEventPrototypeTableValues, *this);
}

const ClassInfo JSWebKitPlaybackTargetAvailabilityEvent::s_info = { "WebKitPlaybackTargetAvailabilityEvent", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebKitPlaybackTargetAvailabilityEvent) };

JSWebKitPlaybackTargetAvailabilityEvent::JSWebKitPlaybackTargetAvailabilityEvent(Structure* structure, JSDOMGlobalObject* globalObject, Ref<WebKitPlaybackTargetAvailabilityEvent>&& impl)
    : JSEvent(structure, globalObject, WTF::move(impl))
{
}

JSObject* JSWebKitPlaybackTargetAvailabilityEvent::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSWebKitPlaybackTargetAvailabilityEventPrototype::create(vm, globalObject, JSWebKitPlaybackTargetAvailabilityEventPrototype::createStructure(vm, globalObject, JSEvent::getPrototype(vm, globalObject)));
}

JSObject* JSWebKitPlaybackTargetAvailabilityEvent::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSWebKitPlaybackTargetAvailabilityEvent>(vm, globalObject);
}

EncodedJSValue jsWebKitPlaybackTargetAvailabilityEventAvailability(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSWebKitPlaybackTargetAvailabilityEvent* castedThis = jsDynamicCast<JSWebKitPlaybackTargetAvailabilityEvent*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSWebKitPlaybackTargetAvailabilityEventPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "WebKitPlaybackTargetAvailabilityEvent", "availability");
        return throwGetterTypeError(*exec, "WebKitPlaybackTargetAvailabilityEvent", "availability");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.availability());
    return JSValue::encode(result);
}


EncodedJSValue jsWebKitPlaybackTargetAvailabilityEventConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSWebKitPlaybackTargetAvailabilityEventPrototype* domObject = jsDynamicCast<JSWebKitPlaybackTargetAvailabilityEventPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSWebKitPlaybackTargetAvailabilityEvent::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSWebKitPlaybackTargetAvailabilityEvent::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSWebKitPlaybackTargetAvailabilityEventConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(WIRELESS_PLAYBACK_TARGET)