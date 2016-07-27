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
#include "JSTestEventTarget.h"

#include "Event.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "JSNode.h"
#include "Node.h"
#include "TestEventTarget.h"
#include "wtf/text/AtomicString.h"
#include <runtime/Error.h>
#include <runtime/PropertyNameArray.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSTestEventTargetTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestEventTargetConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestEventTargetTable = { 2, 1, JSTestEventTargetTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSTestEventTargetConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestEventTargetConstructorTable = { 1, 0, JSTestEventTargetConstructorTableValues, 0 };
const ClassInfo JSTestEventTargetConstructor::s_info = { "TestEventTargetConstructor", &Base::s_info, &JSTestEventTargetConstructorTable, 0, CREATE_METHOD_TABLE(JSTestEventTargetConstructor) };

JSTestEventTargetConstructor::JSTestEventTargetConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestEventTargetConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestEventTargetPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTestEventTargetConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestEventTargetConstructor, JSDOMWrapper>(exec, JSTestEventTargetConstructorTable, jsCast<JSTestEventTargetConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSTestEventTargetPrototypeTableValues[] =
{
    { "item", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestEventTargetPrototypeFunctionItem), (intptr_t)1 },
    { "addEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestEventTargetPrototypeFunctionAddEventListener), (intptr_t)2 },
    { "removeEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestEventTargetPrototypeFunctionRemoveEventListener), (intptr_t)2 },
    { "dispatchEvent", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestEventTargetPrototypeFunctionDispatchEvent), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestEventTargetPrototypeTable = { 8, 7, JSTestEventTargetPrototypeTableValues, 0 };
const ClassInfo JSTestEventTargetPrototype::s_info = { "TestEventTargetPrototype", &Base::s_info, &JSTestEventTargetPrototypeTable, 0, CREATE_METHOD_TABLE(JSTestEventTargetPrototype) };

JSObject* JSTestEventTargetPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestEventTarget>(vm, globalObject);
}

bool JSTestEventTargetPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestEventTargetPrototype* thisObject = jsCast<JSTestEventTargetPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSTestEventTargetPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSTestEventTarget::s_info = { "TestEventTarget", &Base::s_info, &JSTestEventTargetTable, 0 , CREATE_METHOD_TABLE(JSTestEventTarget) };

JSTestEventTarget::JSTestEventTarget(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestEventTarget> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSTestEventTarget::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSTestEventTarget::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestEventTargetPrototype::create(vm, globalObject, JSTestEventTargetPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTestEventTarget::destroy(JSC::JSCell* cell)
{
    JSTestEventTarget* thisObject = static_cast<JSTestEventTarget*>(cell);
    thisObject->JSTestEventTarget::~JSTestEventTarget();
}

JSTestEventTarget::~JSTestEventTarget()
{
    releaseImplIfNotNull();
}

bool JSTestEventTarget::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestEventTarget* thisObject = jsCast<JSTestEventTarget*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    const HashEntry* entry = getStaticValueSlotEntryWithoutCaching<JSTestEventTarget>(exec, propertyName);
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
    if (canGetItemsForName(exec, &thisObject->impl(), propertyName)) {
        slot.setCustom(thisObject, ReadOnly | DontDelete | DontEnum, thisObject->nameGetter);
        return true;
    }
    return getStaticValueSlot<JSTestEventTarget, Base>(exec, JSTestEventTargetTable, thisObject, propertyName, slot);
}

bool JSTestEventTarget::getOwnPropertySlotByIndex(JSObject* object, ExecState* exec, unsigned index, PropertySlot& slot)
{
    JSTestEventTarget* thisObject = jsCast<JSTestEventTarget*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index < thisObject->impl().length()) {
        unsigned attributes = DontDelete | ReadOnly;
        slot.setCustomIndex(thisObject, attributes, index, thisObject->indexGetter);
        return true;
    }
    PropertyName propertyName = Identifier::from(exec, index);
    if (canGetItemsForName(exec, &thisObject->impl(), propertyName)) {
        slot.setCustom(thisObject, ReadOnly | DontDelete | DontEnum, thisObject->nameGetter);
        return true;
    }
    return Base::getOwnPropertySlotByIndex(thisObject, exec, index, slot);
}

JSValue jsTestEventTargetConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTestEventTarget* domObject = jsCast<JSTestEventTarget*>(asObject(slotBase));
    return JSTestEventTarget::getConstructor(exec->vm(), domObject->globalObject());
}

void JSTestEventTarget::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    JSTestEventTarget* thisObject = jsCast<JSTestEventTarget*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    for (unsigned i = 0, count = thisObject->impl().length(); i < count; ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

JSValue JSTestEventTarget::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestEventTargetConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestEventTarget::info()))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestEventTarget::info());
    TestEventTarget& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    int index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (index < 0) {
        setDOMException(exec, INDEX_SIZE_ERR);
        return JSValue::encode(jsUndefined());
    }
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.item(index)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestEventTarget::info()))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestEventTarget::info());
    TestEventTarget& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.addEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestEventTarget::info()))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestEventTarget::info());
    TestEventTarget& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.removeEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)).get(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestEventTarget::info()))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestEventTarget::info());
    TestEventTarget& impl = castedThis->impl();
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

void JSTestEventTarget::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    JSTestEventTarget* thisObject = jsCast<JSTestEventTarget*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    COMPILE_ASSERT(StructureFlags & OverridesVisitChildren, OverridesVisitChildrenWithoutSettingFlag);
    ASSERT(thisObject->structure()->typeInfo().overridesVisitChildren());
    Base::visitChildren(thisObject, visitor);
    thisObject->impl().visitJSEventListeners(visitor);
}


JSValue JSTestEventTarget::indexGetter(ExecState* exec, JSValue slotBase, unsigned index)
{
    JSTestEventTarget* thisObj = jsCast<JSTestEventTarget*>(asObject(slotBase));
    ASSERT_GC_OBJECT_INHERITS(thisObj, info());
    return toJS(exec, thisObj->globalObject(), thisObj->impl().item(index));
}

static inline bool isObservable(JSTestEventTarget* jsTestEventTarget)
{
    if (jsTestEventTarget->hasCustomProperties())
        return true;
    if (jsTestEventTarget->impl().hasEventListeners())
        return true;
    return false;
}

bool JSTestEventTargetOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSTestEventTarget* jsTestEventTarget = jsCast<JSTestEventTarget*>(handle.get().asCell());
    if (jsTestEventTarget->impl().isFiringEventListeners())
        return true;
    if (!isObservable(jsTestEventTarget))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestEventTargetOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestEventTarget* jsTestEventTarget = jsCast<JSTestEventTarget*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestEventTarget->impl(), jsTestEventTarget);
    jsTestEventTarget->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestEventTarget@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore15TestEventTargetE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TestEventTarget* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestEventTarget>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestEventTarget@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore15TestEventTargetE[2];
#if COMPILER(CLANG)
    // If this fails TestEventTarget does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestEventTarget), TestEventTarget_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestEventTarget has subclasses. If TestEventTarget has subclasses that get passed
    // to toJS() we currently require TestEventTarget you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<TestEventTarget>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSTestEventTarget>(exec, globalObject, impl);
}

TestEventTarget* toTestEventTarget(JSC::JSValue value)
{
    return value.inherits(JSTestEventTarget::info()) ? &jsCast<JSTestEventTarget*>(asObject(value))->impl() : 0;
}

}
