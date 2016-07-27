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

#if ENABLE(MEDIA_SOURCE)

#include "JSWebKitSourceBufferList.h"

#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSWebKitSourceBuffer.h"
#include "WebKitSourceBuffer.h"
#include "WebKitSourceBufferList.h"
#include <runtime/Error.h>
#include <runtime/PropertyNameArray.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSWebKitSourceBufferListTableValues[] =
{
    { "length", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitSourceBufferListLength), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitSourceBufferListConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSWebKitSourceBufferListTable = { 5, 3, JSWebKitSourceBufferListTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSWebKitSourceBufferListConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSWebKitSourceBufferListConstructorTable = { 1, 0, JSWebKitSourceBufferListConstructorTableValues, 0 };
const ClassInfo JSWebKitSourceBufferListConstructor::s_info = { "WebKitSourceBufferListConstructor", &Base::s_info, &JSWebKitSourceBufferListConstructorTable, 0, CREATE_METHOD_TABLE(JSWebKitSourceBufferListConstructor) };

JSWebKitSourceBufferListConstructor::JSWebKitSourceBufferListConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSWebKitSourceBufferListConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSWebKitSourceBufferListPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSWebKitSourceBufferListConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSWebKitSourceBufferListConstructor, JSDOMWrapper>(exec, JSWebKitSourceBufferListConstructorTable, jsCast<JSWebKitSourceBufferListConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSWebKitSourceBufferListPrototypeTableValues[] =
{
    { "item", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsWebKitSourceBufferListPrototypeFunctionItem), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSWebKitSourceBufferListPrototypeTable = { 2, 1, JSWebKitSourceBufferListPrototypeTableValues, 0 };
const ClassInfo JSWebKitSourceBufferListPrototype::s_info = { "WebKitSourceBufferListPrototype", &Base::s_info, &JSWebKitSourceBufferListPrototypeTable, 0, CREATE_METHOD_TABLE(JSWebKitSourceBufferListPrototype) };

JSObject* JSWebKitSourceBufferListPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSWebKitSourceBufferList>(vm, globalObject);
}

bool JSWebKitSourceBufferListPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSWebKitSourceBufferListPrototype* thisObject = jsCast<JSWebKitSourceBufferListPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSWebKitSourceBufferListPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSWebKitSourceBufferList::s_info = { "WebKitSourceBufferList", &Base::s_info, &JSWebKitSourceBufferListTable, 0 , CREATE_METHOD_TABLE(JSWebKitSourceBufferList) };

JSWebKitSourceBufferList::JSWebKitSourceBufferList(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<WebKitSourceBufferList> impl)
    : JSEventTarget(structure, globalObject, impl)
{
}

void JSWebKitSourceBufferList::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSWebKitSourceBufferList::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSWebKitSourceBufferListPrototype::create(vm, globalObject, JSWebKitSourceBufferListPrototype::createStructure(vm, globalObject, JSEventTargetPrototype::self(vm, globalObject)));
}

bool JSWebKitSourceBufferList::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSWebKitSourceBufferList* thisObject = jsCast<JSWebKitSourceBufferList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    const HashEntry* entry = getStaticValueSlotEntryWithoutCaching<JSWebKitSourceBufferList>(exec, propertyName);
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
    return getStaticValueSlot<JSWebKitSourceBufferList, Base>(exec, JSWebKitSourceBufferListTable, thisObject, propertyName, slot);
}

bool JSWebKitSourceBufferList::getOwnPropertySlotByIndex(JSObject* object, ExecState* exec, unsigned index, PropertySlot& slot)
{
    JSWebKitSourceBufferList* thisObject = jsCast<JSWebKitSourceBufferList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index < thisObject->impl().length()) {
        unsigned attributes = DontDelete | ReadOnly;
        slot.setCustomIndex(thisObject, attributes, index, thisObject->indexGetter);
        return true;
    }
    return Base::getOwnPropertySlotByIndex(thisObject, exec, index, slot);
}

JSValue jsWebKitSourceBufferListLength(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSWebKitSourceBufferList* castedThis = jsCast<JSWebKitSourceBufferList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    WebKitSourceBufferList& impl = castedThis->impl();
    JSValue result = jsNumber(impl.length());
    return result;
}


JSValue jsWebKitSourceBufferListConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSWebKitSourceBufferList* domObject = jsCast<JSWebKitSourceBufferList*>(asObject(slotBase));
    return JSWebKitSourceBufferList::getConstructor(exec->vm(), domObject->globalObject());
}

void JSWebKitSourceBufferList::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    JSWebKitSourceBufferList* thisObject = jsCast<JSWebKitSourceBufferList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    for (unsigned i = 0, count = thisObject->impl().length(); i < count; ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

JSValue JSWebKitSourceBufferList::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSWebKitSourceBufferListConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsWebKitSourceBufferListPrototypeFunctionItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSWebKitSourceBufferList::info()))
        return throwVMTypeError(exec);
    JSWebKitSourceBufferList* castedThis = jsCast<JSWebKitSourceBufferList*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebKitSourceBufferList::info());
    WebKitSourceBufferList& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.item(index)));
    return JSValue::encode(result);
}

void JSWebKitSourceBufferList::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    JSWebKitSourceBufferList* thisObject = jsCast<JSWebKitSourceBufferList*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    COMPILE_ASSERT(StructureFlags & OverridesVisitChildren, OverridesVisitChildrenWithoutSettingFlag);
    ASSERT(thisObject->structure()->typeInfo().overridesVisitChildren());
    Base::visitChildren(thisObject, visitor);
    thisObject->impl().visitJSEventListeners(visitor);
}


JSValue JSWebKitSourceBufferList::indexGetter(ExecState* exec, JSValue slotBase, unsigned index)
{
    JSWebKitSourceBufferList* thisObj = jsCast<JSWebKitSourceBufferList*>(asObject(slotBase));
    ASSERT_GC_OBJECT_INHERITS(thisObj, info());
    return toJS(exec, thisObj->globalObject(), thisObj->impl().item(index));
}

WebKitSourceBufferList* toWebKitSourceBufferList(JSC::JSValue value)
{
    return value.inherits(JSWebKitSourceBufferList::info()) ? &jsCast<JSWebKitSourceBufferList*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(MEDIA_SOURCE)
