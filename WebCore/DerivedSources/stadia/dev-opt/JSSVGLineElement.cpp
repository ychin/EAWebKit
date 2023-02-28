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
#include "JSSVGLineElement.h"

#include "JSDOMBinding.h"
#include "JSSVGAnimatedBoolean.h"
#include "JSSVGAnimatedLength.h"
#include "SVGLineElement.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsSVGLineElementX1(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsSVGLineElementY1(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsSVGLineElementX2(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsSVGLineElementY2(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsSVGLineElementExternalResourcesRequired(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsSVGLineElementConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSSVGLineElementPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSSVGLineElementPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSSVGLineElementPrototype* ptr = new (NotNull, JSC::allocateCell<JSSVGLineElementPrototype>(vm.heap)) JSSVGLineElementPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSSVGLineElementPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSSVGLineElementConstructor : public DOMConstructorObject {
private:
    JSSVGLineElementConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSSVGLineElementConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSSVGLineElementConstructor* ptr = new (NotNull, JSC::allocateCell<JSSVGLineElementConstructor>(vm.heap)) JSSVGLineElementConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

const ClassInfo JSSVGLineElementConstructor::s_info = { "SVGLineElementConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGLineElementConstructor) };

JSSVGLineElementConstructor::JSSVGLineElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGLineElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGLineElement::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("SVGLineElement"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSSVGLineElementPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "x1", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementX1), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "y1", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementY1), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "x2", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementX2), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "y2", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementY2), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "externalResourcesRequired", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGLineElementExternalResourcesRequired), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSSVGLineElementPrototype::s_info = { "SVGLineElementPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGLineElementPrototype) };

void JSSVGLineElementPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSSVGLineElementPrototypeTableValues, *this);
}

const ClassInfo JSSVGLineElement::s_info = { "SVGLineElement", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGLineElement) };

JSSVGLineElement::JSSVGLineElement(Structure* structure, JSDOMGlobalObject* globalObject, Ref<SVGLineElement>&& impl)
    : JSSVGGraphicsElement(structure, globalObject, WTF::move(impl))
{
}

JSObject* JSSVGLineElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGLineElementPrototype::create(vm, globalObject, JSSVGLineElementPrototype::createStructure(vm, globalObject, JSSVGGraphicsElement::getPrototype(vm, globalObject)));
}

JSObject* JSSVGLineElement::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGLineElement>(vm, globalObject);
}

EncodedJSValue jsSVGLineElementX1(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGLineElement* castedThis = jsDynamicCast<JSSVGLineElement*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGLineElementPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGLineElement", "x1");
        return throwGetterTypeError(*exec, "SVGLineElement", "x1");
    }
    auto& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.x1Animated();
    JSValue result = toJS(exec, castedThis->globalObject(), obj.get());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGLineElementY1(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGLineElement* castedThis = jsDynamicCast<JSSVGLineElement*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGLineElementPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGLineElement", "y1");
        return throwGetterTypeError(*exec, "SVGLineElement", "y1");
    }
    auto& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.y1Animated();
    JSValue result = toJS(exec, castedThis->globalObject(), obj.get());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGLineElementX2(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGLineElement* castedThis = jsDynamicCast<JSSVGLineElement*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGLineElementPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGLineElement", "x2");
        return throwGetterTypeError(*exec, "SVGLineElement", "x2");
    }
    auto& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.x2Animated();
    JSValue result = toJS(exec, castedThis->globalObject(), obj.get());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGLineElementY2(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGLineElement* castedThis = jsDynamicCast<JSSVGLineElement*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGLineElementPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGLineElement", "y2");
        return throwGetterTypeError(*exec, "SVGLineElement", "y2");
    }
    auto& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.y2Animated();
    JSValue result = toJS(exec, castedThis->globalObject(), obj.get());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGLineElementExternalResourcesRequired(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGLineElement* castedThis = jsDynamicCast<JSSVGLineElement*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGLineElementPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGLineElement", "externalResourcesRequired");
        return throwGetterTypeError(*exec, "SVGLineElement", "externalResourcesRequired");
    }
    auto& impl = castedThis->impl();
    RefPtr<SVGAnimatedBoolean> obj = impl.externalResourcesRequiredAnimated();
    JSValue result = toJS(exec, castedThis->globalObject(), obj.get());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGLineElementConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSSVGLineElementPrototype* domObject = jsDynamicCast<JSSVGLineElementPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSSVGLineElement::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSSVGLineElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGLineElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}
