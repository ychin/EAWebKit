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
#include "JSSVGPathSegArcAbs.h"

#include "JSDOMBinding.h"
#include "SVGPathSegArcAbs.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsSVGPathSegArcAbsX(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsX(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsY(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsY(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsR1(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsR1(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsR2(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsR2(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsAngle(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsAngle(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsLargeArcFlag(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsLargeArcFlag(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsSweepFlag(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSSVGPathSegArcAbsSweepFlag(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsSVGPathSegArcAbsConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSSVGPathSegArcAbsPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSSVGPathSegArcAbsPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSSVGPathSegArcAbsPrototype* ptr = new (NotNull, JSC::allocateCell<JSSVGPathSegArcAbsPrototype>(vm.heap)) JSSVGPathSegArcAbsPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSSVGPathSegArcAbsPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSSVGPathSegArcAbsConstructor : public DOMConstructorObject {
private:
    JSSVGPathSegArcAbsConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSSVGPathSegArcAbsConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSSVGPathSegArcAbsConstructor* ptr = new (NotNull, JSC::allocateCell<JSSVGPathSegArcAbsConstructor>(vm.heap)) JSSVGPathSegArcAbsConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

const ClassInfo JSSVGPathSegArcAbsConstructor::s_info = { "SVGPathSegArcAbsConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGPathSegArcAbsConstructor) };

JSSVGPathSegArcAbsConstructor::JSSVGPathSegArcAbsConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGPathSegArcAbsConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGPathSegArcAbs::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("SVGPathSegArcAbs"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSSVGPathSegArcAbsPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "x", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsX), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsX) },
    { "y", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsY), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsY) },
    { "r1", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsR1), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsR1) },
    { "r2", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsR2), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsR2) },
    { "angle", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsAngle), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsAngle) },
    { "largeArcFlag", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsLargeArcFlag), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsLargeArcFlag) },
    { "sweepFlag", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegArcAbsSweepFlag), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSSVGPathSegArcAbsSweepFlag) },
};

const ClassInfo JSSVGPathSegArcAbsPrototype::s_info = { "SVGPathSegArcAbsPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGPathSegArcAbsPrototype) };

void JSSVGPathSegArcAbsPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSSVGPathSegArcAbsPrototypeTableValues, *this);
}

const ClassInfo JSSVGPathSegArcAbs::s_info = { "SVGPathSegArcAbs", &Base::s_info, 0, CREATE_METHOD_TABLE(JSSVGPathSegArcAbs) };

JSSVGPathSegArcAbs::JSSVGPathSegArcAbs(Structure* structure, JSDOMGlobalObject* globalObject, Ref<SVGPathSegArcAbs>&& impl)
    : JSSVGPathSeg(structure, globalObject, WTF::move(impl))
{
}

JSObject* JSSVGPathSegArcAbs::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGPathSegArcAbsPrototype::create(vm, globalObject, JSSVGPathSegArcAbsPrototype::createStructure(vm, globalObject, JSSVGPathSeg::getPrototype(vm, globalObject)));
}

JSObject* JSSVGPathSegArcAbs::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGPathSegArcAbs>(vm, globalObject);
}

EncodedJSValue jsSVGPathSegArcAbsX(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "x");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "x");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.x());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsY(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "y");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "y");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.y());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsR1(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "r1");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "r1");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.r1());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsR2(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "r2");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "r2");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.r2());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsAngle(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "angle");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "angle");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.angle());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsLargeArcFlag(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "largeArcFlag");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "largeArcFlag");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.largeArcFlag());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsSweepFlag(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "SVGPathSegArcAbs", "sweepFlag");
        return throwGetterTypeError(*exec, "SVGPathSegArcAbs", "sweepFlag");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.sweepFlag());
    return JSValue::encode(result);
}


EncodedJSValue jsSVGPathSegArcAbsConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSSVGPathSegArcAbsPrototype* domObject = jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSSVGPathSegArcAbs::getConstructor(exec->vm(), domObject->globalObject()));
}

void setJSSVGPathSegArcAbsX(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "x");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "x");
        return;
    }
    auto& impl = castedThis->impl();
    float nativeValue = value.toFloat(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setX(nativeValue);
}


void setJSSVGPathSegArcAbsY(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "y");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "y");
        return;
    }
    auto& impl = castedThis->impl();
    float nativeValue = value.toFloat(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setY(nativeValue);
}


void setJSSVGPathSegArcAbsR1(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "r1");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "r1");
        return;
    }
    auto& impl = castedThis->impl();
    float nativeValue = value.toFloat(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setR1(nativeValue);
}


void setJSSVGPathSegArcAbsR2(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "r2");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "r2");
        return;
    }
    auto& impl = castedThis->impl();
    float nativeValue = value.toFloat(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setR2(nativeValue);
}


void setJSSVGPathSegArcAbsAngle(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "angle");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "angle");
        return;
    }
    auto& impl = castedThis->impl();
    float nativeValue = value.toFloat(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setAngle(nativeValue);
}


void setJSSVGPathSegArcAbsLargeArcFlag(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "largeArcFlag");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "largeArcFlag");
        return;
    }
    auto& impl = castedThis->impl();
    bool nativeValue = value.toBoolean(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setLargeArcFlag(nativeValue);
}


void setJSSVGPathSegArcAbsSweepFlag(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSSVGPathSegArcAbs* castedThis = jsDynamicCast<JSSVGPathSegArcAbs*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSSVGPathSegArcAbsPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "SVGPathSegArcAbs", "sweepFlag");
        else
            throwSetterTypeError(*exec, "SVGPathSegArcAbs", "sweepFlag");
        return;
    }
    auto& impl = castedThis->impl();
    bool nativeValue = value.toBoolean(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setSweepFlag(nativeValue);
}


JSValue JSSVGPathSegArcAbs::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGPathSegArcAbsConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}