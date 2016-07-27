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

#if ENABLE(INDEXED_DATABASE)

#include "JSIDBCursor.h"

#include "ExceptionCode.h"
#include "IDBAny.h"
#include "IDBCursor.h"
#include "IDBRequest.h"
#include "JSDOMBinding.h"
#include "JSIDBAny.h"
#include "JSIDBRequest.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSIDBCursorTableValues[] =
{
    { "source", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsIDBCursorSource), (intptr_t)0 },
    { "direction", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsIDBCursorDirection), (intptr_t)0 },
    { "key", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsIDBCursorKey), (intptr_t)0 },
    { "primaryKey", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsIDBCursorPrimaryKey), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsIDBCursorConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSIDBCursorTable = { 16, 15, JSIDBCursorTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSIDBCursorConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSIDBCursorConstructorTable = { 1, 0, JSIDBCursorConstructorTableValues, 0 };
static const HashTable& getJSIDBCursorConstructorTable(ExecState* exec)
{
    return getHashTableForGlobalData(exec->vm(), JSIDBCursorConstructorTable);
}

const ClassInfo JSIDBCursorConstructor::s_info = { "IDBCursorConstructor", &Base::s_info, 0, getJSIDBCursorConstructorTable, CREATE_METHOD_TABLE(JSIDBCursorConstructor) };

JSIDBCursorConstructor::JSIDBCursorConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSIDBCursorConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSIDBCursorPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSIDBCursorConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSIDBCursorConstructor, JSDOMWrapper>(exec, getJSIDBCursorConstructorTable(exec), jsCast<JSIDBCursorConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSIDBCursorPrototypeTableValues[] =
{
    { "update", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsIDBCursorPrototypeFunctionUpdate), (intptr_t)1 },
    { "advance", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsIDBCursorPrototypeFunctionAdvance), (intptr_t)1 },
    { "continue", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsIDBCursorPrototypeFunctionContinue), (intptr_t)0 },
    { "delete", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsIDBCursorPrototypeFunctionDelete), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSIDBCursorPrototypeTable = { 9, 7, JSIDBCursorPrototypeTableValues, 0 };
static const HashTable& getJSIDBCursorPrototypeTable(ExecState* exec)
{
    return getHashTableForGlobalData(exec->vm(), JSIDBCursorPrototypeTable);
}

const ClassInfo JSIDBCursorPrototype::s_info = { "IDBCursorPrototype", &Base::s_info, 0, getJSIDBCursorPrototypeTable, CREATE_METHOD_TABLE(JSIDBCursorPrototype) };

JSObject* JSIDBCursorPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSIDBCursor>(vm, globalObject);
}

bool JSIDBCursorPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSIDBCursorPrototype* thisObject = jsCast<JSIDBCursorPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, getJSIDBCursorPrototypeTable(exec), thisObject, propertyName, slot);
}

static const HashTable& getJSIDBCursorTable(ExecState* exec)
{
    return getHashTableForGlobalData(exec->vm(), JSIDBCursorTable);
}

const ClassInfo JSIDBCursor::s_info = { "IDBCursor", &Base::s_info, 0, getJSIDBCursorTable , CREATE_METHOD_TABLE(JSIDBCursor) };

JSIDBCursor::JSIDBCursor(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<IDBCursor> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSIDBCursor::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSIDBCursor::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSIDBCursorPrototype::create(vm, globalObject, JSIDBCursorPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSIDBCursor::destroy(JSC::JSCell* cell)
{
    JSIDBCursor* thisObject = static_cast<JSIDBCursor*>(cell);
    thisObject->JSIDBCursor::~JSIDBCursor();
}

JSIDBCursor::~JSIDBCursor()
{
    releaseImplIfNotNull();
}

bool JSIDBCursor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSIDBCursor* thisObject = jsCast<JSIDBCursor*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSIDBCursor, Base>(exec, getJSIDBCursorTable(exec), thisObject, propertyName, slot);
}

JSValue jsIDBCursorSource(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    IDBCursor& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.source()));
    return result;
}


JSValue jsIDBCursorDirection(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    IDBCursor& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.direction());
    return result;
}


JSValue jsIDBCursorKey(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    IDBCursor& impl = castedThis->impl();
    JSValue result = (impl.key().hasNoValue() ? jsNull() : impl.key().jsValue());
    return result;
}


JSValue jsIDBCursorPrimaryKey(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    IDBCursor& impl = castedThis->impl();
    JSValue result = (impl.primaryKey().hasNoValue() ? jsNull() : impl.primaryKey().jsValue());
    return result;
}


JSValue jsIDBCursorConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSIDBCursor* domObject = jsCast<JSIDBCursor*>(asObject(slotBase));
    return JSIDBCursor::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSIDBCursor::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSIDBCursorConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsIDBCursorPrototypeFunctionUpdate(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSIDBCursor::info()))
        return throwVMTypeError(exec);
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSIDBCursor::info());
    IDBCursor& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    ScriptValue value(exec->vm(), exec->argument(0));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.update(exec, value, ec)));
    setDOMException(exec, ec);
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsIDBCursorPrototypeFunctionAdvance(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSIDBCursor::info()))
        return throwVMTypeError(exec);
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSIDBCursor::info());
    IDBCursor& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned count(toUInt32(exec, exec->argument(0), EnforceRange));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.advance(count, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsIDBCursorPrototypeFunctionContinue(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSIDBCursor::info()))
        return throwVMTypeError(exec);
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSIDBCursor::info());
    IDBCursor& impl = castedThis->impl();
    ExceptionCode ec = 0;
    ScriptExecutionContext* scriptContext = jsCast<JSDOMGlobalObject*>(exec->lexicalGlobalObject())->scriptExecutionContext();
    if (!scriptContext)
        return JSValue::encode(jsUndefined());

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 0) {
        impl.continueFunction(scriptContext, ec);
        setDOMException(exec, ec);
        return JSValue::encode(jsUndefined());
    }

    ScriptValue key(exec->vm(), exec->argument(0));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.continueFunction(scriptContext, key, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsIDBCursorPrototypeFunctionDelete(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSIDBCursor::info()))
        return throwVMTypeError(exec);
    JSIDBCursor* castedThis = jsCast<JSIDBCursor*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSIDBCursor::info());
    IDBCursor& impl = castedThis->impl();
    ExceptionCode ec = 0;
    ScriptExecutionContext* scriptContext = jsCast<JSDOMGlobalObject*>(exec->lexicalGlobalObject())->scriptExecutionContext();
    if (!scriptContext)
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.deleteFunction(scriptContext, ec)));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}

static inline bool isObservable(JSIDBCursor* jsIDBCursor)
{
    if (jsIDBCursor->hasCustomProperties())
        return true;
    return false;
}

bool JSIDBCursorOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSIDBCursor* jsIDBCursor = jsCast<JSIDBCursor*>(handle.get().asCell());
    if (!isObservable(jsIDBCursor))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSIDBCursorOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSIDBCursor* jsIDBCursor = jsCast<JSIDBCursor*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsIDBCursor->impl(), jsIDBCursor);
    jsIDBCursor->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7IDBCursor@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore9IDBCursorE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, IDBCursor* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSIDBCursor>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7IDBCursor@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore9IDBCursorE[2];
#if COMPILER(CLANG)
    // If this fails IDBCursor does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(IDBCursor), IDBCursor_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // IDBCursor has subclasses. If IDBCursor has subclasses that get passed
    // to toJS() we currently require IDBCursor you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<IDBCursor>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSIDBCursor>(exec, globalObject, impl);
}

IDBCursor* toIDBCursor(JSC::JSValue value)
{
    return value.inherits(JSIDBCursor::info()) ? &jsCast<JSIDBCursor*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(INDEXED_DATABASE)
