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

#if ENABLE(MEDIA_STREAM)

#include "JSMediaStreamTrackEvent.h"

#include "JSDictionary.h"
#include "JSMediaStreamTrack.h"
#include "MediaStreamTrack.h"
#include "MediaStreamTrackEvent.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSMediaStreamTrackEventTableValues[] =
{
    { "track", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaStreamTrackEventTrack), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaStreamTrackEventConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaStreamTrackEventTable = { 4, 3, JSMediaStreamTrackEventTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSMediaStreamTrackEventConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaStreamTrackEventConstructorTable = { 1, 0, JSMediaStreamTrackEventConstructorTableValues, 0 };
EncodedJSValue JSC_HOST_CALL JSMediaStreamTrackEventConstructor::constructJSMediaStreamTrackEvent(ExecState* exec)
{
    JSMediaStreamTrackEventConstructor* jsConstructor = jsCast<JSMediaStreamTrackEventConstructor*>(exec->callee());

    ScriptExecutionContext* executionContext = jsConstructor->scriptExecutionContext();
    if (!executionContext)
        return throwVMError(exec, createReferenceError(exec, "Constructor associated execution context is unavailable"));

    AtomicString eventType = exec->argument(0).toString(exec)->value(exec);
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    MediaStreamTrackEventInit eventInit;

    JSValue initializerValue = exec->argument(1);
    if (!initializerValue.isUndefinedOrNull()) {
        // Given the above test, this will always yield an object.
        JSObject* initializerObject = initializerValue.toObject(exec);

        // Create the dictionary wrapper from the initializer object.
        JSDictionary dictionary(exec, initializerObject);

        // Attempt to fill in the EventInit.
        if (!fillMediaStreamTrackEventInit(eventInit, dictionary))
            return JSValue::encode(jsUndefined());
    }

    RefPtr<MediaStreamTrackEvent> event = MediaStreamTrackEvent::create(eventType, eventInit);
    return JSValue::encode(toJS(exec, jsConstructor->globalObject(), event.get()));
}

bool fillMediaStreamTrackEventInit(MediaStreamTrackEventInit& eventInit, JSDictionary& dictionary)
{
    if (!fillEventInit(eventInit, dictionary))
        return false;

    if (!dictionary.tryGetProperty("track", eventInit.track))
        return false;
    return true;
}

const ClassInfo JSMediaStreamTrackEventConstructor::s_info = { "MediaStreamTrackEventConstructor", &Base::s_info, &JSMediaStreamTrackEventConstructorTable, 0, CREATE_METHOD_TABLE(JSMediaStreamTrackEventConstructor) };

JSMediaStreamTrackEventConstructor::JSMediaStreamTrackEventConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSMediaStreamTrackEventConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSMediaStreamTrackEventPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(1), ReadOnly | DontDelete | DontEnum);
}

bool JSMediaStreamTrackEventConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSMediaStreamTrackEventConstructor, JSDOMWrapper>(exec, JSMediaStreamTrackEventConstructorTable, jsCast<JSMediaStreamTrackEventConstructor*>(object), propertyName, slot);
}

ConstructType JSMediaStreamTrackEventConstructor::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructJSMediaStreamTrackEvent;
    return ConstructTypeHost;
}

/* Hash table for prototype */

static const HashTableValue JSMediaStreamTrackEventPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaStreamTrackEventPrototypeTable = { 1, 0, JSMediaStreamTrackEventPrototypeTableValues, 0 };
const ClassInfo JSMediaStreamTrackEventPrototype::s_info = { "MediaStreamTrackEventPrototype", &Base::s_info, &JSMediaStreamTrackEventPrototypeTable, 0, CREATE_METHOD_TABLE(JSMediaStreamTrackEventPrototype) };

JSObject* JSMediaStreamTrackEventPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSMediaStreamTrackEvent>(vm, globalObject);
}

const ClassInfo JSMediaStreamTrackEvent::s_info = { "MediaStreamTrackEvent", &Base::s_info, &JSMediaStreamTrackEventTable, 0 , CREATE_METHOD_TABLE(JSMediaStreamTrackEvent) };

JSMediaStreamTrackEvent::JSMediaStreamTrackEvent(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<MediaStreamTrackEvent> impl)
    : JSEvent(structure, globalObject, impl)
{
}

void JSMediaStreamTrackEvent::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSMediaStreamTrackEvent::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSMediaStreamTrackEventPrototype::create(vm, globalObject, JSMediaStreamTrackEventPrototype::createStructure(vm, globalObject, JSEventPrototype::self(vm, globalObject)));
}

bool JSMediaStreamTrackEvent::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSMediaStreamTrackEvent* thisObject = jsCast<JSMediaStreamTrackEvent*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSMediaStreamTrackEvent, Base>(exec, JSMediaStreamTrackEventTable, thisObject, propertyName, slot);
}

JSValue jsMediaStreamTrackEventTrack(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSMediaStreamTrackEvent* castedThis = jsCast<JSMediaStreamTrackEvent*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    MediaStreamTrackEvent& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.track()));
    return result;
}


JSValue jsMediaStreamTrackEventConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSMediaStreamTrackEvent* domObject = jsCast<JSMediaStreamTrackEvent*>(asObject(slotBase));
    return JSMediaStreamTrackEvent::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSMediaStreamTrackEvent::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSMediaStreamTrackEventConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(MEDIA_STREAM)
