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

#include "JSSVGPoint.h"

#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSSVGMatrix.h"
#include "JSSVGPoint.h"
#include "SVGPoint.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGPointTableValues[] =
{
    { "x", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPointX), (intptr_t)setJSSVGPointX },
    { "y", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPointY), (intptr_t)setJSSVGPointY },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPointConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGPointTable = { 9, 7, JSSVGPointTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGPointConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGPointConstructorTable = { 1, 0, JSSVGPointConstructorTableValues, 0 };
const ClassInfo JSSVGPointConstructor::s_info = { "SVGPointConstructor", &Base::s_info, &JSSVGPointConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGPointConstructor) };

JSSVGPointConstructor::JSSVGPointConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGPointConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGPointPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGPointConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPointConstructor, JSDOMWrapper>(exec, JSSVGPointConstructorTable, jsCast<JSSVGPointConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGPointPrototypeTableValues[] =
{
    { "matrixTransform", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGPointPrototypeFunctionMatrixTransform), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGPointPrototypeTable = { 2, 1, JSSVGPointPrototypeTableValues, 0 };
const ClassInfo JSSVGPointPrototype::s_info = { "SVGPointPrototype", &Base::s_info, &JSSVGPointPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGPointPrototype) };

JSObject* JSSVGPointPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGPoint>(vm, globalObject);
}

bool JSSVGPointPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGPointPrototype* thisObject = jsCast<JSSVGPointPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSSVGPointPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSSVGPoint::s_info = { "SVGPoint", &Base::s_info, &JSSVGPointTable, 0 , CREATE_METHOD_TABLE(JSSVGPoint) };

JSSVGPoint::JSSVGPoint(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGPropertyTearOff<SVGPoint> > impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSSVGPoint::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGPoint::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGPointPrototype::create(vm, globalObject, JSSVGPointPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSSVGPoint::destroy(JSC::JSCell* cell)
{
    JSSVGPoint* thisObject = static_cast<JSSVGPoint*>(cell);
    thisObject->JSSVGPoint::~JSSVGPoint();
}

JSSVGPoint::~JSSVGPoint()
{
    releaseImplIfNotNull();
}

bool JSSVGPoint::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGPoint* thisObject = jsCast<JSSVGPoint*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGPoint, Base>(exec, JSSVGPointTable, thisObject, propertyName, slot);
}

JSValue jsSVGPointX(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGPoint* castedThis = jsCast<JSSVGPoint*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGPoint& impl = castedThis->impl().propertyReference();
    JSValue result = jsNumber(impl.x());
    return result;
}


JSValue jsSVGPointY(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGPoint* castedThis = jsCast<JSSVGPoint*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGPoint& impl = castedThis->impl().propertyReference();
    JSValue result = jsNumber(impl.y());
    return result;
}


JSValue jsSVGPointConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGPoint* domObject = jsCast<JSSVGPoint*>(asObject(slotBase));
    return JSSVGPoint::getConstructor(exec->vm(), domObject->globalObject());
}

void JSSVGPoint::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSSVGPoint* thisObject = jsCast<JSSVGPoint*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSSVGPoint, Base>(exec, propertyName, value, JSSVGPointTable, thisObject, slot);
}

void setJSSVGPointX(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGPoint* castedThis = jsCast<JSSVGPoint*>(thisObject);
    SVGPropertyTearOff<SVGPoint> & impl = castedThis->impl();
    float nativeValue(value.toFloat(exec));
    if (exec->hadException())
        return;
    if (impl.isReadOnly()) {
        setDOMException(exec, NO_MODIFICATION_ALLOWED_ERR);
        return;
    }
    SVGPoint& podImpl = impl.propertyReference();
    podImpl.setX(nativeValue);
    impl.commitChange();
}


void setJSSVGPointY(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGPoint* castedThis = jsCast<JSSVGPoint*>(thisObject);
    SVGPropertyTearOff<SVGPoint> & impl = castedThis->impl();
    float nativeValue(value.toFloat(exec));
    if (exec->hadException())
        return;
    if (impl.isReadOnly()) {
        setDOMException(exec, NO_MODIFICATION_ALLOWED_ERR);
        return;
    }
    SVGPoint& podImpl = impl.propertyReference();
    podImpl.setY(nativeValue);
    impl.commitChange();
}


JSValue JSSVGPoint::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGPointConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsSVGPointPrototypeFunctionMatrixTransform(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGPoint::info()))
        return throwVMTypeError(exec);
    JSSVGPoint* castedThis = jsCast<JSSVGPoint*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGPoint::info());
    SVGPropertyTearOff<SVGPoint> & impl = castedThis->impl();
    if (impl.isReadOnly()) {
        setDOMException(exec, NO_MODIFICATION_ALLOWED_ERR);
        return JSValue::encode(jsUndefined());
    }
    SVGPoint& podImpl = impl.propertyReference();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    if (exec->argumentCount() > 0 && !exec->argument(0).isUndefinedOrNull() && !exec->argument(0).inherits(JSSVGMatrix::info()))
        return throwVMTypeError(exec);
    SVGPropertyTearOff<SVGMatrix>* matrix(toSVGMatrix(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!matrix) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGPoint>::create(podImpl.matrixTransform(matrix->propertyReference()))));
    return JSValue::encode(result);
}

static inline bool isObservable(JSSVGPoint* jsSVGPoint)
{
    if (jsSVGPoint->hasCustomProperties())
        return true;
    return false;
}

bool JSSVGPointOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSSVGPoint* jsSVGPoint = jsCast<JSSVGPoint*>(handle.get().asCell());
    if (!isObservable(jsSVGPoint))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSSVGPointOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSSVGPoint* jsSVGPoint = jsCast<JSSVGPoint*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsSVGPoint->impl(), jsSVGPoint);
    jsSVGPoint->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, SVGPropertyTearOff<SVGPoint> * impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSSVGPoint, SVGPropertyTearOff<SVGPoint> >(exec, impl))
        return result;
    ReportMemoryCost<SVGPropertyTearOff<SVGPoint> >::reportMemoryCost(exec, impl);
    return createNewWrapper<JSSVGPoint, SVGPropertyTearOff<SVGPoint> >(exec, globalObject, impl);
}

SVGPropertyTearOff<SVGPoint> * toSVGPoint(JSC::JSValue value)
{
    return value.inherits(JSSVGPoint::info()) ? &jsCast<JSSVGPoint*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(SVG)
