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
#include "JSHTMLLinkElement.h"

#include "HTMLLinkElement.h"
#include "HTMLNames.h"
#include "JSStyleSheet.h"
#include "StyleSheet.h"
#include "URL.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSHTMLLinkElementTableValues[] =
{
    { "disabled", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementDisabled), (intptr_t)setJSHTMLLinkElementDisabled },
    { "charset", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementCharset), (intptr_t)setJSHTMLLinkElementCharset },
    { "href", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementHref), (intptr_t)setJSHTMLLinkElementHref },
    { "hreflang", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementHreflang), (intptr_t)setJSHTMLLinkElementHreflang },
    { "media", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementMedia), (intptr_t)setJSHTMLLinkElementMedia },
    { "rel", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementRel), (intptr_t)setJSHTMLLinkElementRel },
    { "rev", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementRev), (intptr_t)setJSHTMLLinkElementRev },
    { "sizes", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementSizes), (intptr_t)setJSHTMLLinkElementSizes },
    { "target", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementTarget), (intptr_t)setJSHTMLLinkElementTarget },
    { "type", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementType), (intptr_t)setJSHTMLLinkElementType },
    { "sheet", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementSheet), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLLinkElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLLinkElementTable = { 33, 31, JSHTMLLinkElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSHTMLLinkElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLLinkElementConstructorTable = { 1, 0, JSHTMLLinkElementConstructorTableValues, 0 };
const ClassInfo JSHTMLLinkElementConstructor::s_info = { "HTMLLinkElementConstructor", &Base::s_info, &JSHTMLLinkElementConstructorTable, 0, CREATE_METHOD_TABLE(JSHTMLLinkElementConstructor) };

JSHTMLLinkElementConstructor::JSHTMLLinkElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSHTMLLinkElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSHTMLLinkElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSHTMLLinkElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLLinkElementConstructor, JSDOMWrapper>(exec, JSHTMLLinkElementConstructorTable, jsCast<JSHTMLLinkElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLLinkElementPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLLinkElementPrototypeTable = { 1, 0, JSHTMLLinkElementPrototypeTableValues, 0 };
const ClassInfo JSHTMLLinkElementPrototype::s_info = { "HTMLLinkElementPrototype", &Base::s_info, &JSHTMLLinkElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSHTMLLinkElementPrototype) };

JSObject* JSHTMLLinkElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLLinkElement>(vm, globalObject);
}

const ClassInfo JSHTMLLinkElement::s_info = { "HTMLLinkElement", &Base::s_info, &JSHTMLLinkElementTable, 0 , CREATE_METHOD_TABLE(JSHTMLLinkElement) };

JSHTMLLinkElement::JSHTMLLinkElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLLinkElement> impl)
    : JSHTMLElement(structure, globalObject, impl)
{
}

void JSHTMLLinkElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSHTMLLinkElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSHTMLLinkElementPrototype::create(vm, globalObject, JSHTMLLinkElementPrototype::createStructure(vm, globalObject, JSHTMLElementPrototype::self(vm, globalObject)));
}

bool JSHTMLLinkElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLLinkElement* thisObject = jsCast<JSHTMLLinkElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSHTMLLinkElement, Base>(exec, JSHTMLLinkElementTable, thisObject, propertyName, slot);
}

JSValue jsHTMLLinkElementDisabled(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.fastHasAttribute(WebCore::HTMLNames::disabledAttr));
    return result;
}


JSValue jsHTMLLinkElementCharset(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::charsetAttr));
    return result;
}


JSValue jsHTMLLinkElementHref(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.getURLAttribute(WebCore::HTMLNames::hrefAttr));
    return result;
}


JSValue jsHTMLLinkElementHreflang(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::hreflangAttr));
    return result;
}


JSValue jsHTMLLinkElementMedia(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::mediaAttr));
    return result;
}


JSValue jsHTMLLinkElementRel(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::relAttr));
    return result;
}


JSValue jsHTMLLinkElementRev(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::revAttr));
    return result;
}


JSValue jsHTMLLinkElementSizes(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    return castedThis->sizes(exec);
}


JSValue jsHTMLLinkElementTarget(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::targetAttr));
    return result;
}


JSValue jsHTMLLinkElementType(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.fastGetAttribute(WebCore::HTMLNames::typeAttr));
    return result;
}


JSValue jsHTMLLinkElementSheet(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLLinkElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.sheet()));
    return result;
}


JSValue jsHTMLLinkElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLLinkElement* domObject = jsCast<JSHTMLLinkElement*>(asObject(slotBase));
    return JSHTMLLinkElement::getConstructor(exec->vm(), domObject->globalObject());
}

void JSHTMLLinkElement::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSHTMLLinkElement* thisObject = jsCast<JSHTMLLinkElement*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSHTMLLinkElement, Base>(exec, propertyName, value, JSHTMLLinkElementTable, thisObject, slot);
}

void setJSHTMLLinkElementDisabled(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    bool nativeValue(value.toBoolean(exec));
    if (exec->hadException())
        return;
    impl.setBooleanAttribute(WebCore::HTMLNames::disabledAttr, nativeValue);
}


void setJSHTMLLinkElementCharset(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::charsetAttr, nativeValue);
}


void setJSHTMLLinkElementHref(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::hrefAttr, nativeValue);
}


void setJSHTMLLinkElementHreflang(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::hreflangAttr, nativeValue);
}


void setJSHTMLLinkElementMedia(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::mediaAttr, nativeValue);
}


void setJSHTMLLinkElementRel(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::relAttr, nativeValue);
}


void setJSHTMLLinkElementRev(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::revAttr, nativeValue);
}


void setJSHTMLLinkElementSizes(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    jsCast<JSHTMLLinkElement*>(thisObject)->setSizes(exec, value);
}


void setJSHTMLLinkElementTarget(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::targetAttr, nativeValue);
}


void setJSHTMLLinkElementType(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLLinkElement* castedThis = jsCast<JSHTMLLinkElement*>(thisObject);
    HTMLLinkElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::typeAttr, nativeValue);
}


JSValue JSHTMLLinkElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSHTMLLinkElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}
