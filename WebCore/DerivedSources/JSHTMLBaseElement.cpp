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
#include "JSHTMLBaseElement.h"

#include "HTMLBaseElement.h"
#include "HTMLNames.h"
#include "URL.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSHTMLBaseElementTableValues[] =
{
    { "href", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLBaseElementHref), (intptr_t)setJSHTMLBaseElementHref },
    { "target", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLBaseElementTarget), (intptr_t)setJSHTMLBaseElementTarget },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLBaseElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLBaseElementTable = { 8, 7, JSHTMLBaseElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSHTMLBaseElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLBaseElementConstructorTable = { 1, 0, JSHTMLBaseElementConstructorTableValues, 0 };
const ClassInfo JSHTMLBaseElementConstructor::s_info = { "HTMLBaseElementConstructor", &Base::s_info, &JSHTMLBaseElementConstructorTable, 0, CREATE_METHOD_TABLE(JSHTMLBaseElementConstructor) };

JSHTMLBaseElementConstructor::JSHTMLBaseElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSHTMLBaseElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSHTMLBaseElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSHTMLBaseElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLBaseElementConstructor, JSDOMWrapper>(exec, JSHTMLBaseElementConstructorTable, jsCast<JSHTMLBaseElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLBaseElementPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLBaseElementPrototypeTable = { 1, 0, JSHTMLBaseElementPrototypeTableValues, 0 };
const ClassInfo JSHTMLBaseElementPrototype::s_info = { "HTMLBaseElementPrototype", &Base::s_info, &JSHTMLBaseElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSHTMLBaseElementPrototype) };

JSObject* JSHTMLBaseElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLBaseElement>(vm, globalObject);
}

const ClassInfo JSHTMLBaseElement::s_info = { "HTMLBaseElement", &Base::s_info, &JSHTMLBaseElementTable, 0 , CREATE_METHOD_TABLE(JSHTMLBaseElement) };

JSHTMLBaseElement::JSHTMLBaseElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLBaseElement> impl)
    : JSHTMLElement(structure, globalObject, impl)
{
}

void JSHTMLBaseElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSHTMLBaseElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSHTMLBaseElementPrototype::create(vm, globalObject, JSHTMLBaseElementPrototype::createStructure(vm, globalObject, JSHTMLElementPrototype::self(vm, globalObject)));
}

bool JSHTMLBaseElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLBaseElement* thisObject = jsCast<JSHTMLBaseElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSHTMLBaseElement, Base>(exec, JSHTMLBaseElementTable, thisObject, propertyName, slot);
}

JSValue jsHTMLBaseElementHref(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLBaseElement* castedThis = jsCast<JSHTMLBaseElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLBaseElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.href());
    return result;
}


JSValue jsHTMLBaseElementTarget(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLBaseElement* castedThis = jsCast<JSHTMLBaseElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLBaseElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::targetAttr));
    return result;
}


JSValue jsHTMLBaseElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLBaseElement* domObject = jsCast<JSHTMLBaseElement*>(asObject(slotBase));
    return JSHTMLBaseElement::getConstructor(exec->vm(), domObject->globalObject());
}

void JSHTMLBaseElement::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSHTMLBaseElement* thisObject = jsCast<JSHTMLBaseElement*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSHTMLBaseElement, Base>(exec, propertyName, value, JSHTMLBaseElementTable, thisObject, slot);
}

void setJSHTMLBaseElementHref(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLBaseElement* castedThis = jsCast<JSHTMLBaseElement*>(thisObject);
    HTMLBaseElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setHref(nativeValue);
}


void setJSHTMLBaseElementTarget(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLBaseElement* castedThis = jsCast<JSHTMLBaseElement*>(thisObject);
    HTMLBaseElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::targetAttr, nativeValue);
}


JSValue JSHTMLBaseElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSHTMLBaseElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}
