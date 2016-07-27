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

#include "JSSVGGraphicsElement.h"

#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSSVGAnimatedTransformList.h"
#include "JSSVGElement.h"
#include "JSSVGMatrix.h"
#include "JSSVGRect.h"
#include "SVGElement.h"
#include "SVGGraphicsElement.h"
#include "SVGMatrix.h"
#include "SVGRect.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

#if ENABLE(SVG)
#include "JSSVGStringList.h"
#include "SVGStringList.h"
#endif

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGGraphicsElementTableValues[] =
{
    { "transform", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementTransform), (intptr_t)0 },
    { "nearestViewportElement", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementNearestViewportElement), (intptr_t)0 },
    { "farthestViewportElement", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementFarthestViewportElement), (intptr_t)0 },
#if ENABLE(SVG)
    { "requiredFeatures", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementRequiredFeatures), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "requiredExtensions", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementRequiredExtensions), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "systemLanguage", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementSystemLanguage), (intptr_t)0 },
#endif
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGraphicsElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGraphicsElementTable = { 19, 15, JSSVGGraphicsElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGGraphicsElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGraphicsElementConstructorTable = { 1, 0, JSSVGGraphicsElementConstructorTableValues, 0 };
const ClassInfo JSSVGGraphicsElementConstructor::s_info = { "SVGGraphicsElementConstructor", &Base::s_info, &JSSVGGraphicsElementConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGGraphicsElementConstructor) };

JSSVGGraphicsElementConstructor::JSSVGGraphicsElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGGraphicsElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGGraphicsElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGGraphicsElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGGraphicsElementConstructor, JSDOMWrapper>(exec, JSSVGGraphicsElementConstructorTable, jsCast<JSSVGGraphicsElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGGraphicsElementPrototypeTableValues[] =
{
    { "getBBox", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGGraphicsElementPrototypeFunctionGetBBox), (intptr_t)0 },
    { "getCTM", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGGraphicsElementPrototypeFunctionGetCTM), (intptr_t)0 },
    { "getScreenCTM", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGGraphicsElementPrototypeFunctionGetScreenCTM), (intptr_t)0 },
    { "getTransformToElement", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGGraphicsElementPrototypeFunctionGetTransformToElement), (intptr_t)0 },
#if ENABLE(SVG)
    { "hasExtension", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGGraphicsElementPrototypeFunctionHasExtension), (intptr_t)0 },
#endif
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGraphicsElementPrototypeTable = { 17, 15, JSSVGGraphicsElementPrototypeTableValues, 0 };
const ClassInfo JSSVGGraphicsElementPrototype::s_info = { "SVGGraphicsElementPrototype", &Base::s_info, &JSSVGGraphicsElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGGraphicsElementPrototype) };

JSObject* JSSVGGraphicsElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGGraphicsElement>(vm, globalObject);
}

bool JSSVGGraphicsElementPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGGraphicsElementPrototype* thisObject = jsCast<JSSVGGraphicsElementPrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSSVGGraphicsElementPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSSVGGraphicsElement::s_info = { "SVGGraphicsElement", &Base::s_info, &JSSVGGraphicsElementTable, 0 , CREATE_METHOD_TABLE(JSSVGGraphicsElement) };

JSSVGGraphicsElement::JSSVGGraphicsElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGGraphicsElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

void JSSVGGraphicsElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGGraphicsElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGGraphicsElementPrototype::create(vm, globalObject, JSSVGGraphicsElementPrototype::createStructure(vm, globalObject, JSSVGElementPrototype::self(vm, globalObject)));
}

bool JSSVGGraphicsElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGGraphicsElement* thisObject = jsCast<JSSVGGraphicsElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGGraphicsElement, Base>(exec, JSSVGGraphicsElementTable, thisObject, propertyName, slot);
}

JSValue jsSVGGraphicsElementTransform(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedTransformList> obj = impl.transformAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGGraphicsElementNearestViewportElement(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.nearestViewportElement()));
    return result;
}


JSValue jsSVGGraphicsElementFarthestViewportElement(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.farthestViewportElement()));
    return result;
}


#if ENABLE(SVG)
JSValue jsSVGGraphicsElementRequiredFeatures(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGStaticListPropertyTearOff<SVGStringList>::create(impl, impl.requiredFeatures())));
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGGraphicsElementRequiredExtensions(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGStaticListPropertyTearOff<SVGStringList>::create(impl, impl.requiredExtensions())));
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGGraphicsElementSystemLanguage(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGraphicsElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGStaticListPropertyTearOff<SVGStringList>::create(impl, impl.systemLanguage())));
    return result;
}

#endif

JSValue jsSVGGraphicsElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGraphicsElement* domObject = jsCast<JSSVGGraphicsElement*>(asObject(slotBase));
    return JSSVGGraphicsElement::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGGraphicsElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGGraphicsElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsSVGGraphicsElementPrototypeFunctionGetBBox(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGGraphicsElement::info()))
        return throwVMTypeError(exec);
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGGraphicsElement::info());
    SVGGraphicsElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<FloatRect>::create(impl.getBBox())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGGraphicsElementPrototypeFunctionGetCTM(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGGraphicsElement::info()))
        return throwVMTypeError(exec);
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGGraphicsElement::info());
    SVGGraphicsElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGMatrix>::create(impl.getCTM())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGGraphicsElementPrototypeFunctionGetScreenCTM(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGGraphicsElement::info()))
        return throwVMTypeError(exec);
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGGraphicsElement::info());
    SVGGraphicsElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGMatrix>::create(impl.getScreenCTM())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGGraphicsElementPrototypeFunctionGetTransformToElement(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGGraphicsElement::info()))
        return throwVMTypeError(exec);
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGGraphicsElement::info());
    SVGGraphicsElement& impl = castedThis->impl();
    ExceptionCode ec = 0;
    SVGElement* element(toSVGElement(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGMatrix>::create(impl.getTransformToElement(element, ec))));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}

#if ENABLE(SVG)
EncodedJSValue JSC_HOST_CALL jsSVGGraphicsElementPrototypeFunctionHasExtension(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGGraphicsElement::info()))
        return throwVMTypeError(exec);
    JSSVGGraphicsElement* castedThis = jsCast<JSSVGGraphicsElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGGraphicsElement::info());
    SVGGraphicsElement& impl = castedThis->impl();
    const String& extension(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = jsBoolean(impl.hasExtension(extension));
    return JSValue::encode(result);
}

#endif


}

#endif // ENABLE(SVG)
