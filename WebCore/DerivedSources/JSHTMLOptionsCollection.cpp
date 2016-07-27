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
#include "JSHTMLOptionsCollection.h"

#include "Element.h"
#include "ExceptionCode.h"
#include "HTMLOptionsCollection.h"
#include "JSDOMBinding.h"
#include "JSNode.h"
#include "JSNodeCustom.h"
#include "Node.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSHTMLOptionsCollectionTableValues[] =
{
    { "selectedIndex", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLOptionsCollectionSelectedIndex), (intptr_t)setJSHTMLOptionsCollectionSelectedIndex },
    { "length", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLOptionsCollectionLength), (intptr_t)setJSHTMLOptionsCollectionLength },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLOptionsCollectionConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLOptionsCollectionTable = { 9, 7, JSHTMLOptionsCollectionTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSHTMLOptionsCollectionConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLOptionsCollectionConstructorTable = { 1, 0, JSHTMLOptionsCollectionConstructorTableValues, 0 };
const ClassInfo JSHTMLOptionsCollectionConstructor::s_info = { "HTMLOptionsCollectionConstructor", &Base::s_info, &JSHTMLOptionsCollectionConstructorTable, 0, CREATE_METHOD_TABLE(JSHTMLOptionsCollectionConstructor) };

JSHTMLOptionsCollectionConstructor::JSHTMLOptionsCollectionConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSHTMLOptionsCollectionConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSHTMLOptionsCollectionPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSHTMLOptionsCollectionConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLOptionsCollectionConstructor, JSDOMWrapper>(exec, JSHTMLOptionsCollectionConstructorTable, jsCast<JSHTMLOptionsCollectionConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLOptionsCollectionPrototypeTableValues[] =
{
    { "namedItem", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLOptionsCollectionPrototypeFunctionNamedItem), (intptr_t)0 },
    { "add", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLOptionsCollectionPrototypeFunctionAdd), (intptr_t)0 },
    { "remove", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsHTMLOptionsCollectionPrototypeFunctionRemove), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSHTMLOptionsCollectionPrototypeTable = { 8, 7, JSHTMLOptionsCollectionPrototypeTableValues, 0 };
const ClassInfo JSHTMLOptionsCollectionPrototype::s_info = { "HTMLOptionsCollectionPrototype", &Base::s_info, &JSHTMLOptionsCollectionPrototypeTable, 0, CREATE_METHOD_TABLE(JSHTMLOptionsCollectionPrototype) };

JSObject* JSHTMLOptionsCollectionPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLOptionsCollection>(vm, globalObject);
}

bool JSHTMLOptionsCollectionPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLOptionsCollectionPrototype* thisObject = jsCast<JSHTMLOptionsCollectionPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSHTMLOptionsCollectionPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSHTMLOptionsCollection::s_info = { "HTMLOptionsCollection", &Base::s_info, &JSHTMLOptionsCollectionTable, 0 , CREATE_METHOD_TABLE(JSHTMLOptionsCollection) };

JSHTMLOptionsCollection::JSHTMLOptionsCollection(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLOptionsCollection> impl)
    : JSHTMLCollection(structure, globalObject, impl)
{
}

void JSHTMLOptionsCollection::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSHTMLOptionsCollection::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSHTMLOptionsCollectionPrototype::create(vm, globalObject, JSHTMLOptionsCollectionPrototype::createStructure(vm, globalObject, JSHTMLCollectionPrototype::self(vm, globalObject)));
}

bool JSHTMLOptionsCollection::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSHTMLOptionsCollection* thisObject = jsCast<JSHTMLOptionsCollection*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    JSValue proto = thisObject->prototype();
    if (proto.isObject() && jsCast<JSObject*>(asObject(proto))->hasProperty(exec, propertyName))
        return false;

    return getStaticValueSlot<JSHTMLOptionsCollection, Base>(exec, JSHTMLOptionsCollectionTable, thisObject, propertyName, slot);
}

JSValue jsHTMLOptionsCollectionSelectedIndex(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLOptionsCollection& impl = castedThis->impl();
    JSValue result = jsNumber(impl.selectedIndex());
    return result;
}


JSValue jsHTMLOptionsCollectionLength(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLOptionsCollection& impl = castedThis->impl();
    JSValue result = jsNumber(impl.length());
    return result;
}


JSValue jsHTMLOptionsCollectionConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSHTMLOptionsCollection* domObject = jsCast<JSHTMLOptionsCollection*>(asObject(slotBase));
    return JSHTMLOptionsCollection::getConstructor(exec->vm(), domObject->globalObject());
}

void JSHTMLOptionsCollection::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSHTMLOptionsCollection* thisObject = jsCast<JSHTMLOptionsCollection*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    unsigned index = propertyName.asIndex();
    if (index != PropertyName::NotAnIndex) {
        thisObject->indexSetter(exec, index, value);
        return;
    }
    lookupPut<JSHTMLOptionsCollection, Base>(exec, propertyName, value, JSHTMLOptionsCollectionTable, thisObject, slot);
}

void JSHTMLOptionsCollection::putByIndex(JSCell* cell, ExecState* exec, unsigned index, JSValue value, bool shouldThrow)
{
    JSHTMLOptionsCollection* thisObject = jsCast<JSHTMLOptionsCollection*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index <= MAX_ARRAY_INDEX) {
        UNUSED_PARAM(shouldThrow);
        thisObject->indexSetter(exec, index, value);
        return;
    }
    Base::putByIndex(cell, exec, index, value, shouldThrow);
}

void setJSHTMLOptionsCollectionSelectedIndex(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(thisObject);
    HTMLOptionsCollection& impl = castedThis->impl();
    int nativeValue(toInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setSelectedIndex(nativeValue);
}


void setJSHTMLOptionsCollectionLength(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    jsCast<JSHTMLOptionsCollection*>(thisObject)->setLength(exec, value);
}


JSValue JSHTMLOptionsCollection::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSHTMLOptionsCollectionConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionNamedItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLOptionsCollection::info()))
        return throwVMTypeError(exec);
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLOptionsCollection::info());
    HTMLOptionsCollection& impl = castedThis->impl();
    const String& name(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.namedItem(name)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionAdd(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLOptionsCollection::info()))
        return throwVMTypeError(exec);
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLOptionsCollection::info());
    return JSValue::encode(castedThis->add(exec));
}

EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionRemove(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSHTMLOptionsCollection::info()))
        return throwVMTypeError(exec);
    JSHTMLOptionsCollection* castedThis = jsCast<JSHTMLOptionsCollection*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSHTMLOptionsCollection::info());
    return JSValue::encode(castedThis->remove(exec));
}

static inline bool isObservable(JSHTMLOptionsCollection* jsHTMLOptionsCollection)
{
    if (jsHTMLOptionsCollection->hasCustomProperties())
        return true;
    return false;
}

bool JSHTMLOptionsCollectionOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSHTMLOptionsCollection* jsHTMLOptionsCollection = jsCast<JSHTMLOptionsCollection*>(handle.get().asCell());
    if (!isObservable(jsHTMLOptionsCollection))
        return false;
    void* root = WebCore::root(jsHTMLOptionsCollection->impl().ownerNode());
    return visitor.containsOpaqueRoot(root);
}

void JSHTMLOptionsCollectionOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSHTMLOptionsCollection* jsHTMLOptionsCollection = jsCast<JSHTMLOptionsCollection*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsHTMLOptionsCollection->impl(), jsHTMLOptionsCollection);
    jsHTMLOptionsCollection->releaseImpl();
}

HTMLOptionsCollection* toHTMLOptionsCollection(JSC::JSValue value)
{
    return value.inherits(JSHTMLOptionsCollection::info()) ? &jsCast<JSHTMLOptionsCollection*>(asObject(value))->impl() : 0;
}

}
