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

#if ENABLE(WEB_AUDIO)

#include "JSPeriodicWave.h"

#include "PeriodicWave.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSPeriodicWaveTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsPeriodicWaveConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSPeriodicWaveTable = { 2, 1, JSPeriodicWaveTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSPeriodicWaveConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSPeriodicWaveConstructorTable = { 1, 0, JSPeriodicWaveConstructorTableValues, 0 };
const ClassInfo JSPeriodicWaveConstructor::s_info = { "PeriodicWaveConstructor", &Base::s_info, &JSPeriodicWaveConstructorTable, 0, CREATE_METHOD_TABLE(JSPeriodicWaveConstructor) };

JSPeriodicWaveConstructor::JSPeriodicWaveConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSPeriodicWaveConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSPeriodicWavePrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSPeriodicWaveConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSPeriodicWaveConstructor, JSDOMWrapper>(exec, JSPeriodicWaveConstructorTable, jsCast<JSPeriodicWaveConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSPeriodicWavePrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSPeriodicWavePrototypeTable = { 1, 0, JSPeriodicWavePrototypeTableValues, 0 };
const ClassInfo JSPeriodicWavePrototype::s_info = { "PeriodicWavePrototype", &Base::s_info, &JSPeriodicWavePrototypeTable, 0, CREATE_METHOD_TABLE(JSPeriodicWavePrototype) };

JSObject* JSPeriodicWavePrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSPeriodicWave>(vm, globalObject);
}

const ClassInfo JSPeriodicWave::s_info = { "PeriodicWave", &Base::s_info, &JSPeriodicWaveTable, 0 , CREATE_METHOD_TABLE(JSPeriodicWave) };

JSPeriodicWave::JSPeriodicWave(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<PeriodicWave> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSPeriodicWave::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSPeriodicWave::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSPeriodicWavePrototype::create(vm, globalObject, JSPeriodicWavePrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSPeriodicWave::destroy(JSC::JSCell* cell)
{
    JSPeriodicWave* thisObject = static_cast<JSPeriodicWave*>(cell);
    thisObject->JSPeriodicWave::~JSPeriodicWave();
}

JSPeriodicWave::~JSPeriodicWave()
{
    releaseImplIfNotNull();
}

bool JSPeriodicWave::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSPeriodicWave* thisObject = jsCast<JSPeriodicWave*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSPeriodicWave, Base>(exec, JSPeriodicWaveTable, thisObject, propertyName, slot);
}

JSValue jsPeriodicWaveConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSPeriodicWave* domObject = jsCast<JSPeriodicWave*>(asObject(slotBase));
    return JSPeriodicWave::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSPeriodicWave::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSPeriodicWaveConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static inline bool isObservable(JSPeriodicWave* jsPeriodicWave)
{
    if (jsPeriodicWave->hasCustomProperties())
        return true;
    return false;
}

bool JSPeriodicWaveOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSPeriodicWave* jsPeriodicWave = jsCast<JSPeriodicWave*>(handle.get().asCell());
    if (!isObservable(jsPeriodicWave))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSPeriodicWaveOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSPeriodicWave* jsPeriodicWave = jsCast<JSPeriodicWave*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsPeriodicWave->impl(), jsPeriodicWave);
    jsPeriodicWave->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, PeriodicWave* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSPeriodicWave>(exec, impl))
        return result;
#if COMPILER(CLANG)
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to PeriodicWave.
    COMPILE_ASSERT(!__is_polymorphic(PeriodicWave), PeriodicWave_is_polymorphic_but_idl_claims_not_to_be);
#endif
    ReportMemoryCost<PeriodicWave>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSPeriodicWave>(exec, globalObject, impl);
}

PeriodicWave* toPeriodicWave(JSC::JSValue value)
{
    return value.inherits(JSPeriodicWave::info()) ? &jsCast<JSPeriodicWave*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(WEB_AUDIO)
