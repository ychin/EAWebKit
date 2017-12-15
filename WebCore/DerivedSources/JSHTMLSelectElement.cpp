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
#include "JSHTMLSelectElement.h"

#include "ExceptionCode.h"
#include "HTMLCollection.h"
#include "HTMLFormElement.h"
#include "HTMLNames.h"
#include "HTMLOptionsCollection.h"
#include "HTMLSelectElement.h"
#include "JSDOMBinding.h"
#include "JSHTMLCollection.h"
#include "JSHTMLElement.h"
#include "JSHTMLFormElement.h"
#include "JSHTMLOptionsCollection.h"
#include "JSNode.h"
#include "JSNodeList.h"
#include "JSValidityState.h"
#include "NameNodeList.h"
#include "Node.h"
#include "NodeList.h"
#include "URL.h"
#include "ValidityState.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <runtime/PropertyNameArray.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSHTMLSelectElementTableValues[] =
{
    { "autofocus", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementAutofocus), (intptr_t)setJSHTMLSelectElementAutofocus },
    { "disabled", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementDisabled), (intptr_t)setJSHTMLSelectElementDisabled },
    { "form", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementForm), (intptr_t)0 },
    { "multiple", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementMultiple), (intptr_t)setJSHTMLSelectElementMultiple },
    { "name", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementName), (intptr_t)setJSHTMLSelectElementName },
    { "required", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementRequired), (intptr_t)setJSHTMLSelectElementRequired },
    { "size", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementSize), (intptr_t)setJSHTMLSelectElementSize },
    { "type", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementType), (intptr_t)0 },
    { "options", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementOptions), (intptr_t)0 },
    { "length", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementLength), (intptr_t)setJSHTMLSelectElementLength },
    { "selectedOptions", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementSelectedOptions), (intptr_t)0 },
    { "selectedIndex", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementSelectedIndex), (intptr_t)setJSHTMLSelectElementSelectedIndex },
    { "value", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementValue), (intptr_t)setJSHTMLSelectElementValue },
    { "willValidate", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementWillValidate), (intptr_t)0 },
    { "validity", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementValidity), (intptr_t)0 },
    { "validationMessage", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementValidationMessage), (intptr_t)0 },
    { "labels", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementLabels), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLSelectElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLSelectElementTable = { 67, 63, JSHTMLSelectElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSHTMLSelectElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLSelectElementConstructorTable = { 1, 0, JSHTMLSelectElementConstructorTableValues, 0 };
const ClassInfo JSHTMLSelectElementConstructor::s_info = { "HTMLSelectElementConstructor", &Base::s_info, &JSHTMLSelectElementConstructorTable, 0, CREATE_METHOD_TABLE(JSHTMLSelectElementConstructor) };

JSHTMLSelectElementConstructor::JSHTMLSelectElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSHTMLSelectElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSHTMLSelectElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSHTMLSelectElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLSelectElementConstructor, JSDOMWrapper>(exec, JSHTMLSelectElementConstructorTable, jsCast<JSHTMLSelectElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLSelectElementPrototypeTableValues[] =
{
    { "item", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionItem), (intptr_t)1 },
    { "namedItem", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionNamedItem), (intptr_t)0 },
    { "add", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionAdd), (intptr_t)0 },
    { "remove", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionRemove), (intptr_t)0 },
    { "checkValidity", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionCheckValidity), (intptr_t)0 },
    { "setCustomValidity", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLSelectElementPrototypeFunctionSetCustomValidity), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLSelectElementPrototypeTable = { 16, 15, JSHTMLSelectElementPrototypeTableValues, 0 };
const ClassInfo JSHTMLSelectElementPrototype::s_info = { "HTMLSelectElementPrototype", &Base::s_info, &JSHTMLSelectElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSHTMLSelectElementPrototype) };

JSObject* JSHTMLSelectElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLSelectElement>(vm, globalObject);
}

bool JSHTMLSelectElementPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLSelectElementPrototype* thisObject = jsCast<JSHTMLSelectElementPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSHTMLSelectElementPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSHTMLSelectElement::s_info = { "HTMLSelectElement", &Base::s_info, &JSHTMLSelectElementTable, 0 , CREATE_METHOD_TABLE(JSHTMLSelectElement) };

JSHTMLSelectElement::JSHTMLSelectElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLSelectElement> impl)
    : JSHTMLElement(structure, globalObject, impl)
{
}

void JSHTMLSelectElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSHTMLSelectElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSHTMLSelectElementPrototype::create(vm, globalObject, JSHTMLSelectElementPrototype::createStructure(vm, globalObject, JSHTMLElementPrototype::self(vm, globalObject)));
}

bool JSHTMLSelectElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLSelectElement* thisObject = jsCast<JSHTMLSelectElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    const HashEntry* entry = getStaticValueSlotEntryWithoutCaching<JSHTMLSelectElement>(exec, propertyName);
    if (entry) {
        slot.setCustom(thisObject, entry->attributes(), entry->propertyGetter());
        return true;
    }
    unsigned index = propertyName.asIndex();
    if (index != PropertyName::NotAnIndex && index < thisObject->impl().length()) {
        unsigned attributes = DontDelete;
        slot.setCustomIndex(thisObject, attributes, index, indexGetter);
        return true;
    }
    return getStaticValueSlot<JSHTMLSelectElement, Base>(exec, JSHTMLSelectElementTable, thisObject, propertyName, slot);
}

bool JSHTMLSelectElement::getOwnPropertySlotByIndex(JSObject* object, ExecState* exec, unsigned index, PropertySlot& slot)
{
    JSHTMLSelectElement* thisObject = jsCast<JSHTMLSelectElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index < thisObject->impl().length()) {
        unsigned attributes = DontDelete;
        slot.setCustomIndex(thisObject, attributes, index, thisObject->indexGetter);
        return true;
    }
    return Base::getOwnPropertySlotByIndex(thisObject, exec, index, slot);
}

JSValue jsHTMLSelectElementAutofocus(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.fastHasAttribute(WebCore::HTMLNames::autofocusAttr));
    return result;
}


JSValue jsHTMLSelectElementDisabled(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.fastHasAttribute(WebCore::HTMLNames::disabledAttr));
    return result;
}


JSValue jsHTMLSelectElementForm(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.form()));
    return result;
}


JSValue jsHTMLSelectElementMultiple(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.multiple());
    return result;
}


JSValue jsHTMLSelectElementName(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.getNameAttribute());
    return result;
}


JSValue jsHTMLSelectElementRequired(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.fastHasAttribute(WebCore::HTMLNames::requiredAttr));
    return result;
}


JSValue jsHTMLSelectElementSize(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.size());
    return result;
}


JSValue jsHTMLSelectElementType(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.type());
    return result;
}


JSValue jsHTMLSelectElementOptions(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.options()));
    return result;
}


JSValue jsHTMLSelectElementLength(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.length());
    return result;
}


JSValue jsHTMLSelectElementSelectedOptions(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.selectedOptions()));
    return result;
}


JSValue jsHTMLSelectElementSelectedIndex(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.selectedIndex());
    return result;
}


JSValue jsHTMLSelectElementValue(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.value());
    return result;
}


JSValue jsHTMLSelectElementWillValidate(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.willValidate());
    return result;
}


JSValue jsHTMLSelectElementValidity(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.validity()));
    return result;
}


JSValue jsHTMLSelectElementValidationMessage(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.validationMessage());
    return result;
}


JSValue jsHTMLSelectElementLabels(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLSelectElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.labels()));
    return result;
}


JSValue jsHTMLSelectElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLSelectElement* domObject = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    return JSHTMLSelectElement::getConstructor(exec->vm(), domObject->globalObject());
}

void JSHTMLSelectElement::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSHTMLSelectElement* thisObject = jsCast<JSHTMLSelectElement*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    unsigned index = propertyName.asIndex();
    if (index != PropertyName::NotAnIndex) {
        thisObject->indexSetter(exec, index, value);
        return;
    }
    lookupPut<JSHTMLSelectElement, Base>(exec, propertyName, value, JSHTMLSelectElementTable, thisObject, slot);
}

void JSHTMLSelectElement::putByIndex(JSCell* cell, ExecState* exec, unsigned index, JSValue value, bool shouldThrow)
{
    JSHTMLSelectElement* thisObject = jsCast<JSHTMLSelectElement*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index <= MAX_ARRAY_INDEX) {
        UNUSED_PARAM(shouldThrow);
        thisObject->indexSetter(exec, index, value);
        return;
    }
    Base::putByIndex(cell, exec, index, value, shouldThrow);
}

void setJSHTMLSelectElementAutofocus(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    bool nativeValue(value.toBoolean(exec));
    if (exec->hadException())
        return;
    impl.setBooleanAttribute(WebCore::HTMLNames::autofocusAttr, nativeValue);
}


void setJSHTMLSelectElementDisabled(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    bool nativeValue(value.toBoolean(exec));
    if (exec->hadException())
        return;
    impl.setBooleanAttribute(WebCore::HTMLNames::disabledAttr, nativeValue);
}


void setJSHTMLSelectElementMultiple(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    bool nativeValue(value.toBoolean(exec));
    if (exec->hadException())
        return;
    impl.setMultiple(nativeValue);
}


void setJSHTMLSelectElementName(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setAttribute(WebCore::HTMLNames::nameAttr, nativeValue);
}


void setJSHTMLSelectElementRequired(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    bool nativeValue(value.toBoolean(exec));
    if (exec->hadException())
        return;
    impl.setBooleanAttribute(WebCore::HTMLNames::requiredAttr, nativeValue);
}


void setJSHTMLSelectElementSize(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    int nativeValue(toInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setSize(nativeValue);
}


void setJSHTMLSelectElementLength(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    ExceptionCode ec = 0;
    unsigned nativeValue(toUInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setLength(nativeValue, ec);
    setDOMException(exec, ec);
}


void setJSHTMLSelectElementSelectedIndex(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    int nativeValue(toInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setSelectedIndex(nativeValue);
}


void setJSHTMLSelectElementValue(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(thisObject);
    HTMLSelectElement& impl = castedThis->impl();
    const String& nativeValue(valueToStringWithNullCheck(exec, value));
    if (exec->hadException())
        return;
    impl.setValue(nativeValue);
}


void JSHTMLSelectElement::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    JSHTMLSelectElement* thisObject = jsCast<JSHTMLSelectElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    for (unsigned i = 0, count = thisObject->impl().length(); i < count; ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

JSValue JSHTMLSelectElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSHTMLSelectElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    HTMLSelectElement& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.item(index)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionNamedItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    HTMLSelectElement& impl = castedThis->impl();
    const String& name(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.namedItem(name)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionAdd(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    HTMLSelectElement& impl = castedThis->impl();
    ExceptionCode ec = 0;
    HTMLElement* element(toHTMLElement(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    HTMLElement* before(toHTMLElement(exec->argument(1)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.add(element, before, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionRemove(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    return JSValue::encode(castedThis->remove(exec));
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionCheckValidity(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    HTMLSelectElement& impl = castedThis->impl();

    JSC::JSValue result = jsBoolean(impl.checkValidity());
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsHTMLSelectElementPrototypeFunctionSetCustomValidity(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLSelectElement::info()))
        return throwVMTypeError(exec);
    JSHTMLSelectElement* castedThis = jsCast<JSHTMLSelectElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLSelectElement::info());
    HTMLSelectElement& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    const String& error(valueToStringWithUndefinedOrNullCheck(exec, exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.setCustomValidity(error);
    return JSValue::encode(jsUndefined());
}


JSValue JSHTMLSelectElement::indexGetter(ExecState* exec, JSValue slotBase, unsigned index)
{
    JSHTMLSelectElement* thisObj = jsCast<JSHTMLSelectElement*>(asObject(slotBase));
    ASSERT_GC_OBJECT_INHERITS(thisObj, info());
    return toJS(exec, thisObj->globalObject(), thisObj->impl().item(index));
}


}