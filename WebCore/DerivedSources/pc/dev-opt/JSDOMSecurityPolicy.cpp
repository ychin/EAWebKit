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

#if ENABLE(CSP_NEXT)

#include "JSDOMSecurityPolicy.h"

#include "DOMSecurityPolicy.h"
#include "DOMStringList.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSDOMStringList.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsConnectionTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFontFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFormAction(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFrameFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsImageFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsMediaFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsObjectFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsPluginType(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsScriptFrom(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsStyleFrom(JSC::ExecState*);

// Attributes

JSC::EncodedJSValue jsDOMSecurityPolicyAllowsEval(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDOMSecurityPolicyAllowsInlineScript(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDOMSecurityPolicyAllowsInlineStyle(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDOMSecurityPolicyIsActive(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDOMSecurityPolicyReportURIs(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDOMSecurityPolicyConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSDOMSecurityPolicyPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSDOMSecurityPolicyPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDOMSecurityPolicyPrototype* ptr = new (NotNull, JSC::allocateCell<JSDOMSecurityPolicyPrototype>(vm.heap)) JSDOMSecurityPolicyPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSDOMSecurityPolicyPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSDOMSecurityPolicyConstructor : public DOMConstructorObject {
private:
    JSDOMSecurityPolicyConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSDOMSecurityPolicyConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSDOMSecurityPolicyConstructor* ptr = new (NotNull, JSC::allocateCell<JSDOMSecurityPolicyConstructor>(vm.heap)) JSDOMSecurityPolicyConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

const ClassInfo JSDOMSecurityPolicyConstructor::s_info = { "SecurityPolicyConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDOMSecurityPolicyConstructor) };

JSDOMSecurityPolicyConstructor::JSDOMSecurityPolicyConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSDOMSecurityPolicyConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSDOMSecurityPolicy::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("SecurityPolicy"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSDOMSecurityPolicyPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "allowsEval", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyAllowsEval), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "allowsInlineScript", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyAllowsInlineScript), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "allowsInlineStyle", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyAllowsInlineStyle), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "isActive", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyIsActive), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "reportURIs", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMSecurityPolicyReportURIs), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "allowsConnectionTo", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsConnectionTo), (intptr_t) (1) },
    { "allowsFontFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsFontFrom), (intptr_t) (1) },
    { "allowsFormAction", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsFormAction), (intptr_t) (1) },
    { "allowsFrameFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsFrameFrom), (intptr_t) (1) },
    { "allowsImageFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsImageFrom), (intptr_t) (1) },
    { "allowsMediaFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsMediaFrom), (intptr_t) (1) },
    { "allowsObjectFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsObjectFrom), (intptr_t) (1) },
    { "allowsPluginType", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsPluginType), (intptr_t) (1) },
    { "allowsScriptFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsScriptFrom), (intptr_t) (1) },
    { "allowsStyleFrom", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDOMSecurityPolicyPrototypeFunctionAllowsStyleFrom), (intptr_t) (1) },
};

const ClassInfo JSDOMSecurityPolicyPrototype::s_info = { "SecurityPolicyPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDOMSecurityPolicyPrototype) };

void JSDOMSecurityPolicyPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSDOMSecurityPolicyPrototypeTableValues, *this);
}

const ClassInfo JSDOMSecurityPolicy::s_info = { "SecurityPolicy", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDOMSecurityPolicy) };

JSDOMSecurityPolicy::JSDOMSecurityPolicy(Structure* structure, JSDOMGlobalObject* globalObject, Ref<DOMSecurityPolicy>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSDOMSecurityPolicy::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSDOMSecurityPolicyPrototype::create(vm, globalObject, JSDOMSecurityPolicyPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSDOMSecurityPolicy::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSDOMSecurityPolicy>(vm, globalObject);
}

void JSDOMSecurityPolicy::destroy(JSC::JSCell* cell)
{
    JSDOMSecurityPolicy* thisObject = static_cast<JSDOMSecurityPolicy*>(cell);
    thisObject->JSDOMSecurityPolicy::~JSDOMSecurityPolicy();
}

JSDOMSecurityPolicy::~JSDOMSecurityPolicy()
{
    releaseImpl();
}

EncodedJSValue jsDOMSecurityPolicyAllowsEval(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDOMSecurityPolicyPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DOMSecurityPolicy", "allowsEval");
        return throwGetterTypeError(*exec, "DOMSecurityPolicy", "allowsEval");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.allowsEval());
    return JSValue::encode(result);
}


EncodedJSValue jsDOMSecurityPolicyAllowsInlineScript(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDOMSecurityPolicyPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DOMSecurityPolicy", "allowsInlineScript");
        return throwGetterTypeError(*exec, "DOMSecurityPolicy", "allowsInlineScript");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.allowsInlineScript());
    return JSValue::encode(result);
}


EncodedJSValue jsDOMSecurityPolicyAllowsInlineStyle(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDOMSecurityPolicyPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DOMSecurityPolicy", "allowsInlineStyle");
        return throwGetterTypeError(*exec, "DOMSecurityPolicy", "allowsInlineStyle");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.allowsInlineStyle());
    return JSValue::encode(result);
}


EncodedJSValue jsDOMSecurityPolicyIsActive(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDOMSecurityPolicyPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DOMSecurityPolicy", "isActive");
        return throwGetterTypeError(*exec, "DOMSecurityPolicy", "isActive");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.isActive());
    return JSValue::encode(result);
}


EncodedJSValue jsDOMSecurityPolicyReportURIs(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDOMSecurityPolicyPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DOMSecurityPolicy", "reportURIs");
        return throwGetterTypeError(*exec, "DOMSecurityPolicy", "reportURIs");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.reportURIs()));
    return JSValue::encode(result);
}


EncodedJSValue jsDOMSecurityPolicyConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSDOMSecurityPolicyPrototype* domObject = jsDynamicCast<JSDOMSecurityPolicyPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSDOMSecurityPolicy::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSDOMSecurityPolicy::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSDOMSecurityPolicyConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsConnectionTo(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsConnectionTo");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsConnectionTo(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFontFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsFontFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsFontFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFormAction(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsFormAction");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsFormAction(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsFrameFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsFrameFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsFrameFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsImageFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsImageFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsImageFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsMediaFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsMediaFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsMediaFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsObjectFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsObjectFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsObjectFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsPluginType(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsPluginType");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String type = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsPluginType(type));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsScriptFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsScriptFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsScriptFrom(url));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDOMSecurityPolicyPrototypeFunctionAllowsStyleFrom(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDOMSecurityPolicy* castedThis = jsDynamicCast<JSDOMSecurityPolicy*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DOMSecurityPolicy", "allowsStyleFrom");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDOMSecurityPolicy::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String url = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.allowsStyleFrom(url));
    return JSValue::encode(result);
}

bool JSDOMSecurityPolicyOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSDOMSecurityPolicyOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsDOMSecurityPolicy = jsCast<JSDOMSecurityPolicy*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsDOMSecurityPolicy->impl(), jsDOMSecurityPolicy);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7DOMSecurityPolicy@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore17DOMSecurityPolicyE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, DOMSecurityPolicy* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSDOMSecurityPolicy>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7DOMSecurityPolicy@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore17DOMSecurityPolicyE[2];
#if COMPILER(CLANG)
    // If this fails DOMSecurityPolicy does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(DOMSecurityPolicy), DOMSecurityPolicy_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // DOMSecurityPolicy has subclasses. If DOMSecurityPolicy has subclasses that get passed
    // to toJS() we currently require DOMSecurityPolicy you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSDOMSecurityPolicy>(globalObject, impl);
}

DOMSecurityPolicy* JSDOMSecurityPolicy::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSDOMSecurityPolicy*>(value))
        return &wrapper->impl();
    return nullptr;
}

}

#endif // ENABLE(CSP_NEXT)
