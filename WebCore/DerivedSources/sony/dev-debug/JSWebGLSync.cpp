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

#if ENABLE(WEBGL)

#include "JSWebGLSync.h"

#include "JSDOMBinding.h"
#include "WebGLSync.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsWebGLSyncConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSWebGLSyncPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSWebGLSyncPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSWebGLSyncPrototype* ptr = new (NotNull, JSC::allocateCell<JSWebGLSyncPrototype>(vm.heap)) JSWebGLSyncPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSWebGLSyncPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSWebGLSyncConstructor : public DOMConstructorObject {
private:
    JSWebGLSyncConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSWebGLSyncConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSWebGLSyncConstructor* ptr = new (NotNull, JSC::allocateCell<JSWebGLSyncConstructor>(vm.heap)) JSWebGLSyncConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

const ClassInfo JSWebGLSyncConstructor::s_info = { "WebGLSyncConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebGLSyncConstructor) };

JSWebGLSyncConstructor::JSWebGLSyncConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSWebGLSyncConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSWebGLSync::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("WebGLSync"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSWebGLSyncPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebGLSyncConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSWebGLSyncPrototype::s_info = { "WebGLSyncPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebGLSyncPrototype) };

void JSWebGLSyncPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSWebGLSyncPrototypeTableValues, *this);
}

const ClassInfo JSWebGLSync::s_info = { "WebGLSync", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebGLSync) };

JSWebGLSync::JSWebGLSync(Structure* structure, JSDOMGlobalObject* globalObject, Ref<WebGLSync>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSWebGLSync::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSWebGLSyncPrototype::create(vm, globalObject, JSWebGLSyncPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSWebGLSync::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSWebGLSync>(vm, globalObject);
}

void JSWebGLSync::destroy(JSC::JSCell* cell)
{
    JSWebGLSync* thisObject = static_cast<JSWebGLSync*>(cell);
    thisObject->JSWebGLSync::~JSWebGLSync();
}

JSWebGLSync::~JSWebGLSync()
{
    releaseImpl();
}

EncodedJSValue jsWebGLSyncConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSWebGLSyncPrototype* domObject = jsDynamicCast<JSWebGLSyncPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSWebGLSync::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSWebGLSync::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSWebGLSyncConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

bool JSWebGLSyncOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSWebGLSyncOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsWebGLSync = jsCast<JSWebGLSync*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsWebGLSync->impl(), jsWebGLSync);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7WebGLSync@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore9WebGLSyncE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, WebGLSync* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSWebGLSync>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7WebGLSync@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore9WebGLSyncE[2];
#if COMPILER(CLANG)
    // If this fails WebGLSync does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(WebGLSync), WebGLSync_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // WebGLSync has subclasses. If WebGLSync has subclasses that get passed
    // to toJS() we currently require WebGLSync you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSWebGLSync>(globalObject, impl);
}

WebGLSync* JSWebGLSync::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSWebGLSync*>(value))
        return &wrapper->impl();
    return nullptr;
}

}

#endif // ENABLE(WEBGL)
