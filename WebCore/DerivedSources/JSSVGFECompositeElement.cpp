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

#if ENABLE(FILTERS) && ENABLE(SVG)

#include "JSSVGFECompositeElement.h"

#include "JSSVGAnimatedEnumeration.h"
#include "JSSVGAnimatedNumber.h"
#include "JSSVGAnimatedString.h"
#include "SVGFECompositeElement.h"
#include <wtf/GetPtr.h>

#if ENABLE(SVG)
#include "JSSVGAnimatedLength.h"
#endif

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGFECompositeElementTableValues[] =
{
    { "in1", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementIn1), (intptr_t)0 },
    { "in2", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementIn2), (intptr_t)0 },
    { "operator", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementOperator), (intptr_t)0 },
    { "k1", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementK1), (intptr_t)0 },
    { "k2", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementK2), (intptr_t)0 },
    { "k3", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementK3), (intptr_t)0 },
    { "k4", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementK4), (intptr_t)0 },
#if ENABLE(SVG)
    { "x", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementX), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "y", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementY), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "width", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementWidth), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "height", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementHeight), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "result", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementResult), (intptr_t)0 },
#endif
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGFECompositeElementTable = { 33, 31, JSSVGFECompositeElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGFECompositeElementConstructorTableValues[] =
{
    { "SVG_FECOMPOSITE_OPERATOR_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_UNKNOWN), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_OVER", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OVER), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_IN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_IN), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_OUT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OUT), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_ATOP", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ATOP), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_XOR", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_XOR), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_ARITHMETIC", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ARITHMETIC), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGFECompositeElementConstructorTable = { 16, 15, JSSVGFECompositeElementConstructorTableValues, 0 };
const ClassInfo JSSVGFECompositeElementConstructor::s_info = { "SVGFECompositeElementConstructor", &Base::s_info, &JSSVGFECompositeElementConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGFECompositeElementConstructor) };

JSSVGFECompositeElementConstructor::JSSVGFECompositeElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGFECompositeElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGFECompositeElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGFECompositeElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGFECompositeElementConstructor, JSDOMWrapper>(exec, JSSVGFECompositeElementConstructorTable, jsCast<JSSVGFECompositeElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGFECompositeElementPrototypeTableValues[] =
{
    { "SVG_FECOMPOSITE_OPERATOR_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_UNKNOWN), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_OVER", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OVER), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_IN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_IN), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_OUT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OUT), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_ATOP", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ATOP), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_XOR", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_XOR), (intptr_t)0 },
    { "SVG_FECOMPOSITE_OPERATOR_ARITHMETIC", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ARITHMETIC), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGFECompositeElementPrototypeTable = { 16, 15, JSSVGFECompositeElementPrototypeTableValues, 0 };
const ClassInfo JSSVGFECompositeElementPrototype::s_info = { "SVGFECompositeElementPrototype", &Base::s_info, &JSSVGFECompositeElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGFECompositeElementPrototype) };

JSObject* JSSVGFECompositeElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGFECompositeElement>(vm, globalObject);
}

bool JSSVGFECompositeElementPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGFECompositeElementPrototype* thisObject = jsCast<JSSVGFECompositeElementPrototype*>(object);
    return getStaticValueSlot<JSSVGFECompositeElementPrototype, JSObject>(exec, JSSVGFECompositeElementPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSSVGFECompositeElement::s_info = { "SVGFECompositeElement", &Base::s_info, &JSSVGFECompositeElementTable, 0 , CREATE_METHOD_TABLE(JSSVGFECompositeElement) };

JSSVGFECompositeElement::JSSVGFECompositeElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGFECompositeElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

void JSSVGFECompositeElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGFECompositeElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGFECompositeElementPrototype::create(vm, globalObject, JSSVGFECompositeElementPrototype::createStructure(vm, globalObject, JSSVGElementPrototype::self(vm, globalObject)));
}

bool JSSVGFECompositeElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGFECompositeElement* thisObject = jsCast<JSSVGFECompositeElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGFECompositeElement, Base>(exec, JSSVGFECompositeElementTable, thisObject, propertyName, slot);
}

JSValue jsSVGFECompositeElementIn1(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedString> obj = impl.in1Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementIn2(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedString> obj = impl.in2Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementOperator(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedEnumeration> obj = impl.svgOperatorAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementK1(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedNumber> obj = impl.k1Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementK2(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedNumber> obj = impl.k2Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementK3(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedNumber> obj = impl.k3Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGFECompositeElementK4(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedNumber> obj = impl.k4Animated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


#if ENABLE(SVG)
JSValue jsSVGFECompositeElementX(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.xAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGFECompositeElementY(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.yAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGFECompositeElementWidth(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.widthAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGFECompositeElementHeight(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.heightAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGFECompositeElementResult(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* castedThis = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGFECompositeElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedString> obj = impl.resultAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

JSValue jsSVGFECompositeElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGFECompositeElement* domObject = jsCast<JSSVGFECompositeElement*>(asObject(slotBase));
    return JSSVGFECompositeElement::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGFECompositeElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGFECompositeElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

// Constant getters

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_UNKNOWN(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(0));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OVER(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(1));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_IN(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(2));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_OUT(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(3));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ATOP(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(4));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_XOR(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(5));
}

JSValue jsSVGFECompositeElementSVG_FECOMPOSITE_OPERATOR_ARITHMETIC(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(6));
}


}

#endif // ENABLE(FILTERS) && ENABLE(SVG)
