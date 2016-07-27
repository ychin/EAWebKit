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

#if ENABLE(SVG)

#include "JSSVGAnimatedPreserveAspectRatio.h"

#include "JSSVGPreserveAspectRatio.h"
#include "SVGPreserveAspectRatio.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGAnimatedPreserveAspectRatioTableValues[] =
{
    { "baseVal", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedPreserveAspectRatioBaseVal), (intptr_t)0 },
    { "animVal", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedPreserveAspectRatioAnimVal), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedPreserveAspectRatioConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedPreserveAspectRatioTable = { 8, 7, JSSVGAnimatedPreserveAspectRatioTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGAnimatedPreserveAspectRatioConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedPreserveAspectRatioConstructorTable = { 1, 0, JSSVGAnimatedPreserveAspectRatioConstructorTableValues, 0 };
const ClassInfo JSSVGAnimatedPreserveAspectRatioConstructor::s_info = { "SVGAnimatedPreserveAspectRatioConstructor", &Base::s_info, &JSSVGAnimatedPreserveAspectRatioConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGAnimatedPreserveAspectRatioConstructor) };

JSSVGAnimatedPreserveAspectRatioConstructor::JSSVGAnimatedPreserveAspectRatioConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGAnimatedPreserveAspectRatioConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGAnimatedPreserveAspectRatioPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGAnimatedPreserveAspectRatioConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGAnimatedPreserveAspectRatioConstructor, JSDOMWrapper>(exec, JSSVGAnimatedPreserveAspectRatioConstructorTable, jsCast<JSSVGAnimatedPreserveAspectRatioConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGAnimatedPreserveAspectRatioPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedPreserveAspectRatioPrototypeTable = { 1, 0, JSSVGAnimatedPreserveAspectRatioPrototypeTableValues, 0 };
const ClassInfo JSSVGAnimatedPreserveAspectRatioPrototype::s_info = { "SVGAnimatedPreserveAspectRatioPrototype", &Base::s_info, &JSSVGAnimatedPreserveAspectRatioPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGAnimatedPreserveAspectRatioPrototype) };

JSObject* JSSVGAnimatedPreserveAspectRatioPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGAnimatedPreserveAspectRatio>(vm, globalObject);
}

const ClassInfo JSSVGAnimatedPreserveAspectRatio::s_info = { "SVGAnimatedPreserveAspectRatio", &Base::s_info, &JSSVGAnimatedPreserveAspectRatioTable, 0 , CREATE_METHOD_TABLE(JSSVGAnimatedPreserveAspectRatio) };

JSSVGAnimatedPreserveAspectRatio::JSSVGAnimatedPreserveAspectRatio(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGAnimatedPreserveAspectRatio> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSSVGAnimatedPreserveAspectRatio::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGAnimatedPreserveAspectRatio::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGAnimatedPreserveAspectRatioPrototype::create(vm, globalObject, JSSVGAnimatedPreserveAspectRatioPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSSVGAnimatedPreserveAspectRatio::destroy(JSC::JSCell* cell)
{
    JSSVGAnimatedPreserveAspectRatio* thisObject = static_cast<JSSVGAnimatedPreserveAspectRatio*>(cell);
    thisObject->JSSVGAnimatedPreserveAspectRatio::~JSSVGAnimatedPreserveAspectRatio();
}

JSSVGAnimatedPreserveAspectRatio::~JSSVGAnimatedPreserveAspectRatio()
{
    releaseImplIfNotNull();
}

bool JSSVGAnimatedPreserveAspectRatio::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGAnimatedPreserveAspectRatio* thisObject = jsCast<JSSVGAnimatedPreserveAspectRatio*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGAnimatedPreserveAspectRatio, Base>(exec, JSSVGAnimatedPreserveAspectRatioTable, thisObject, propertyName, slot);
}

JSValue jsSVGAnimatedPreserveAspectRatioBaseVal(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedPreserveAspectRatio* castedThis = jsCast<JSSVGAnimatedPreserveAspectRatio*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGAnimatedPreserveAspectRatio& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(static_cast<SVGPropertyTearOff<SVGPreserveAspectRatio>*>(impl.baseVal())));
    return result;
}


JSValue jsSVGAnimatedPreserveAspectRatioAnimVal(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedPreserveAspectRatio* castedThis = jsCast<JSSVGAnimatedPreserveAspectRatio*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGAnimatedPreserveAspectRatio& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(static_cast<SVGPropertyTearOff<SVGPreserveAspectRatio>*>(impl.animVal())));
    return result;
}


JSValue jsSVGAnimatedPreserveAspectRatioConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedPreserveAspectRatio* domObject = jsCast<JSSVGAnimatedPreserveAspectRatio*>(asObject(slotBase));
    return JSSVGAnimatedPreserveAspectRatio::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGAnimatedPreserveAspectRatio::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGAnimatedPreserveAspectRatioConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static inline bool isObservable(JSSVGAnimatedPreserveAspectRatio* jsSVGAnimatedPreserveAspectRatio)
{
    if (jsSVGAnimatedPreserveAspectRatio->hasCustomProperties())
        return true;
    return false;
}

bool JSSVGAnimatedPreserveAspectRatioOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSSVGAnimatedPreserveAspectRatio* jsSVGAnimatedPreserveAspectRatio = jsCast<JSSVGAnimatedPreserveAspectRatio*>(handle.get().asCell());
    if (!isObservable(jsSVGAnimatedPreserveAspectRatio))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSSVGAnimatedPreserveAspectRatioOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSSVGAnimatedPreserveAspectRatio* jsSVGAnimatedPreserveAspectRatio = jsCast<JSSVGAnimatedPreserveAspectRatio*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsSVGAnimatedPreserveAspectRatio->impl(), jsSVGAnimatedPreserveAspectRatio);
    jsSVGAnimatedPreserveAspectRatio->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, SVGAnimatedPreserveAspectRatio* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSSVGAnimatedPreserveAspectRatio>(exec, impl))
        return result;
    ReportMemoryCost<SVGAnimatedPreserveAspectRatio>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSSVGAnimatedPreserveAspectRatio>(exec, globalObject, impl);
}

SVGAnimatedPreserveAspectRatio* toSVGAnimatedPreserveAspectRatio(JSC::JSValue value)
{
    return value.inherits(JSSVGAnimatedPreserveAspectRatio::info()) ? &jsCast<JSSVGAnimatedPreserveAspectRatio*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(SVG)
