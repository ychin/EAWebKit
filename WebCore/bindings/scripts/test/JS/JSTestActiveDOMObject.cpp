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
#include "JSTestActiveDOMObject.h"

#include "BindingSecurity.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSNode.h"
#include "TestActiveDOMObject.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSTestActiveDOMObjectTableValues[] =
{
    { "excitingAttr", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestActiveDOMObjectExcitingAttr), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestActiveDOMObjectConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestActiveDOMObjectTable = { 4, 3, JSTestActiveDOMObjectTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSTestActiveDOMObjectConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestActiveDOMObjectConstructorTable = { 1, 0, JSTestActiveDOMObjectConstructorTableValues, 0 };
const ClassInfo JSTestActiveDOMObjectConstructor::s_info = { "TestActiveDOMObjectConstructor", &Base::s_info, &JSTestActiveDOMObjectConstructorTable, 0, CREATE_METHOD_TABLE(JSTestActiveDOMObjectConstructor) };

JSTestActiveDOMObjectConstructor::JSTestActiveDOMObjectConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestActiveDOMObjectConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestActiveDOMObjectPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTestActiveDOMObjectConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestActiveDOMObjectConstructor, JSDOMWrapper>(exec, JSTestActiveDOMObjectConstructorTable, jsCast<JSTestActiveDOMObjectConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSTestActiveDOMObjectPrototypeTableValues[] =
{
    { "excitingFunction", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestActiveDOMObjectPrototypeFunctionExcitingFunction), (intptr_t)1 },
    { "postMessage", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestActiveDOMObjectPrototypeFunctionPostMessage), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestActiveDOMObjectPrototypeTable = { 4, 3, JSTestActiveDOMObjectPrototypeTableValues, 0 };
const ClassInfo JSTestActiveDOMObjectPrototype::s_info = { "TestActiveDOMObjectPrototype", &Base::s_info, &JSTestActiveDOMObjectPrototypeTable, 0, CREATE_METHOD_TABLE(JSTestActiveDOMObjectPrototype) };

JSObject* JSTestActiveDOMObjectPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestActiveDOMObject>(vm, globalObject);
}

bool JSTestActiveDOMObjectPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestActiveDOMObjectPrototype* thisObject = jsCast<JSTestActiveDOMObjectPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSTestActiveDOMObjectPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSTestActiveDOMObject::s_info = { "TestActiveDOMObject", &Base::s_info, &JSTestActiveDOMObjectTable, 0 , CREATE_METHOD_TABLE(JSTestActiveDOMObject) };

JSTestActiveDOMObject::JSTestActiveDOMObject(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestActiveDOMObject> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSTestActiveDOMObject::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSTestActiveDOMObject::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestActiveDOMObjectPrototype::create(vm, globalObject, JSTestActiveDOMObjectPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTestActiveDOMObject::destroy(JSC::JSCell* cell)
{
    JSTestActiveDOMObject* thisObject = static_cast<JSTestActiveDOMObject*>(cell);
    thisObject->JSTestActiveDOMObject::~JSTestActiveDOMObject();
}

JSTestActiveDOMObject::~JSTestActiveDOMObject()
{
    releaseImplIfNotNull();
}

bool JSTestActiveDOMObject::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestActiveDOMObject* thisObject = jsCast<JSTestActiveDOMObject*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSTestActiveDOMObject, Base>(exec, JSTestActiveDOMObjectTable, thisObject, propertyName, slot);
}

JSValue jsTestActiveDOMObjectExcitingAttr(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTestActiveDOMObject* castedThis = jsCast<JSTestActiveDOMObject*>(asObject(slotBase));
    if (!BindingSecurity::shouldAllowAccessToDOMWindow(exec, castedThis->impl()))
        return jsUndefined();
    UNUSED_PARAM(exec);
    TestActiveDOMObject& impl = castedThis->impl();
    JSValue result = jsNumber(impl.excitingAttr());
    return result;
}


JSValue jsTestActiveDOMObjectConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSTestActiveDOMObject* domObject = jsCast<JSTestActiveDOMObject*>(asObject(slotBase));
    if (!BindingSecurity::shouldAllowAccessToDOMWindow(exec, domObject->impl()))
        return jsUndefined();
    return JSTestActiveDOMObject::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSTestActiveDOMObject::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestActiveDOMObjectConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTestActiveDOMObjectPrototypeFunctionExcitingFunction(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestActiveDOMObject::info()))
        return throwVMTypeError(exec);
    JSTestActiveDOMObject* castedThis = jsCast<JSTestActiveDOMObject*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestActiveDOMObject::info());
    if (!BindingSecurity::shouldAllowAccessToDOMWindow(exec, castedThis->impl()))
        return JSValue::encode(jsUndefined());
    TestActiveDOMObject& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    Node* nextChild(toNode(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.excitingFunction(nextChild);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTestActiveDOMObjectPrototypeFunctionPostMessage(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSTestActiveDOMObject::info()))
        return throwVMTypeError(exec);
    JSTestActiveDOMObject* castedThis = jsCast<JSTestActiveDOMObject*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestActiveDOMObject::info());
    TestActiveDOMObject& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    const String& message(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.postMessage(message);
    return JSValue::encode(jsUndefined());
}

static inline bool isObservable(JSTestActiveDOMObject* jsTestActiveDOMObject)
{
    if (jsTestActiveDOMObject->hasCustomProperties())
        return true;
    return false;
}

bool JSTestActiveDOMObjectOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSTestActiveDOMObject* jsTestActiveDOMObject = jsCast<JSTestActiveDOMObject*>(handle.get().asCell());
    if (!isObservable(jsTestActiveDOMObject))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestActiveDOMObjectOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestActiveDOMObject* jsTestActiveDOMObject = jsCast<JSTestActiveDOMObject*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestActiveDOMObject->impl(), jsTestActiveDOMObject);
    jsTestActiveDOMObject->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestActiveDOMObject@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore19TestActiveDOMObjectE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TestActiveDOMObject* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestActiveDOMObject>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestActiveDOMObject@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore19TestActiveDOMObjectE[2];
#if COMPILER(CLANG)
    // If this fails TestActiveDOMObject does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestActiveDOMObject), TestActiveDOMObject_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestActiveDOMObject has subclasses. If TestActiveDOMObject has subclasses that get passed
    // to toJS() we currently require TestActiveDOMObject you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<TestActiveDOMObject>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSTestActiveDOMObject>(exec, globalObject, impl);
}

TestActiveDOMObject* toTestActiveDOMObject(JSC::JSValue value)
{
    return value.inherits(JSTestActiveDOMObject::info()) ? &jsCast<JSTestActiveDOMObject*>(asObject(value))->impl() : 0;
}

}
