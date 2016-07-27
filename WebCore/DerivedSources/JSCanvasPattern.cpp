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
#include "JSCanvasPattern.h"

#include "CanvasPattern.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSCanvasPatternTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCanvasPatternConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCanvasPatternTable = { 2, 1, JSCanvasPatternTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSCanvasPatternConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCanvasPatternConstructorTable = { 1, 0, JSCanvasPatternConstructorTableValues, 0 };
const ClassInfo JSCanvasPatternConstructor::s_info = { "CanvasPatternConstructor", &Base::s_info, &JSCanvasPatternConstructorTable, 0, CREATE_METHOD_TABLE(JSCanvasPatternConstructor) };

JSCanvasPatternConstructor::JSCanvasPatternConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSCanvasPatternConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSCanvasPatternPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSCanvasPatternConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCanvasPatternConstructor, JSDOMWrapper>(exec, JSCanvasPatternConstructorTable, jsCast<JSCanvasPatternConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSCanvasPatternPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCanvasPatternPrototypeTable = { 1, 0, JSCanvasPatternPrototypeTableValues, 0 };
const ClassInfo JSCanvasPatternPrototype::s_info = { "CanvasPatternPrototype", &Base::s_info, &JSCanvasPatternPrototypeTable, 0, CREATE_METHOD_TABLE(JSCanvasPatternPrototype) };

JSObject* JSCanvasPatternPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSCanvasPattern>(vm, globalObject);
}

const ClassInfo JSCanvasPattern::s_info = { "CanvasPattern", &Base::s_info, &JSCanvasPatternTable, 0 , CREATE_METHOD_TABLE(JSCanvasPattern) };

JSCanvasPattern::JSCanvasPattern(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<CanvasPattern> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSCanvasPattern::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSCanvasPattern::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSCanvasPatternPrototype::create(vm, globalObject, JSCanvasPatternPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSCanvasPattern::destroy(JSC::JSCell* cell)
{
    JSCanvasPattern* thisObject = static_cast<JSCanvasPattern*>(cell);
    thisObject->JSCanvasPattern::~JSCanvasPattern();
}

JSCanvasPattern::~JSCanvasPattern()
{
    releaseImplIfNotNull();
}

bool JSCanvasPattern::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSCanvasPattern* thisObject = jsCast<JSCanvasPattern*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSCanvasPattern, Base>(exec, JSCanvasPatternTable, thisObject, propertyName, slot);
}

JSValue jsCanvasPatternConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSCanvasPattern* domObject = jsCast<JSCanvasPattern*>(asObject(slotBase));
    return JSCanvasPattern::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSCanvasPattern::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSCanvasPatternConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static inline bool isObservable(JSCanvasPattern* jsCanvasPattern)
{
    if (jsCanvasPattern->hasCustomProperties())
        return true;
    return false;
}

bool JSCanvasPatternOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSCanvasPattern* jsCanvasPattern = jsCast<JSCanvasPattern*>(handle.get().asCell());
    if (!isObservable(jsCanvasPattern))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSCanvasPatternOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSCanvasPattern* jsCanvasPattern = jsCast<JSCanvasPattern*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsCanvasPattern->impl(), jsCanvasPattern);
    jsCanvasPattern->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, CanvasPattern* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSCanvasPattern>(exec, impl))
        return result;
#if COMPILER(CLANG)
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to CanvasPattern.
    COMPILE_ASSERT(!__is_polymorphic(CanvasPattern), CanvasPattern_is_polymorphic_but_idl_claims_not_to_be);
#endif
    ReportMemoryCost<CanvasPattern>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSCanvasPattern>(exec, globalObject, impl);
}

CanvasPattern* toCanvasPattern(JSC::JSValue value)
{
    return value.inherits(JSCanvasPattern::info()) ? &jsCast<JSCanvasPattern*>(asObject(value))->impl() : 0;
}

}
