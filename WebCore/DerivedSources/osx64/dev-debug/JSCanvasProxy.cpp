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

#if ENABLE(CANVAS_PROXY)

#include "JSCanvasProxy.h"

#include "CanvasProxy.h"
#include "JSDOMBinding.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsCanvasProxyConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSCanvasProxyPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSCanvasProxyPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSCanvasProxyPrototype* ptr = new (NotNull, JSC::allocateCell<JSCanvasProxyPrototype>(vm.heap)) JSCanvasProxyPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSCanvasProxyPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSCanvasProxyConstructor : public DOMConstructorObject {
private:
    JSCanvasProxyConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSCanvasProxyConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSCanvasProxyConstructor* ptr = new (NotNull, JSC::allocateCell<JSCanvasProxyConstructor>(vm.heap)) JSCanvasProxyConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

const ClassInfo JSCanvasProxyConstructor::s_info = { "CanvasProxyConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSCanvasProxyConstructor) };

JSCanvasProxyConstructor::JSCanvasProxyConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSCanvasProxyConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSCanvasProxy::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("CanvasProxy"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSCanvasProxyPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCanvasProxyConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSCanvasProxyPrototype::s_info = { "CanvasProxyPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSCanvasProxyPrototype) };

void JSCanvasProxyPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSCanvasProxyPrototypeTableValues, *this);
}

const ClassInfo JSCanvasProxy::s_info = { "CanvasProxy", &Base::s_info, 0, CREATE_METHOD_TABLE(JSCanvasProxy) };

JSCanvasProxy::JSCanvasProxy(Structure* structure, JSDOMGlobalObject* globalObject, Ref<CanvasProxy>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSCanvasProxy::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSCanvasProxyPrototype::create(vm, globalObject, JSCanvasProxyPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSCanvasProxy::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSCanvasProxy>(vm, globalObject);
}

void JSCanvasProxy::destroy(JSC::JSCell* cell)
{
    JSCanvasProxy* thisObject = static_cast<JSCanvasProxy*>(cell);
    thisObject->JSCanvasProxy::~JSCanvasProxy();
}

JSCanvasProxy::~JSCanvasProxy()
{
    releaseImpl();
}

EncodedJSValue jsCanvasProxyConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSCanvasProxyPrototype* domObject = jsDynamicCast<JSCanvasProxyPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSCanvasProxy::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSCanvasProxy::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSCanvasProxyConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

bool JSCanvasProxyOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSCanvasProxyOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsCanvasProxy = jsCast<JSCanvasProxy*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsCanvasProxy->impl(), jsCanvasProxy);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7CanvasProxy@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore11CanvasProxyE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, CanvasProxy* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSCanvasProxy>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7CanvasProxy@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore11CanvasProxyE[2];
#if COMPILER(CLANG)
    // If this fails CanvasProxy does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(CanvasProxy), CanvasProxy_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // CanvasProxy has subclasses. If CanvasProxy has subclasses that get passed
    // to toJS() we currently require CanvasProxy you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSCanvasProxy>(globalObject, impl);
}

CanvasProxy* JSCanvasProxy::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSCanvasProxy*>(value))
        return &wrapper->impl();
    return nullptr;
}

}

#endif // ENABLE(CANVAS_PROXY)