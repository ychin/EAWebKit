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

#include "JSOESElementIndexUint.h"

#include "JSDOMBinding.h"
#include "OESElementIndexUint.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

class JSOESElementIndexUintPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSOESElementIndexUintPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSOESElementIndexUintPrototype* ptr = new (NotNull, JSC::allocateCell<JSOESElementIndexUintPrototype>(vm.heap)) JSOESElementIndexUintPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSOESElementIndexUintPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

/* Hash table for prototype */

static const HashTableValue JSOESElementIndexUintPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

const ClassInfo JSOESElementIndexUintPrototype::s_info = { "OESElementIndexUintPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSOESElementIndexUintPrototype) };

void JSOESElementIndexUintPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSOESElementIndexUintPrototypeTableValues, *this);
}

const ClassInfo JSOESElementIndexUint::s_info = { "OESElementIndexUint", &Base::s_info, 0, CREATE_METHOD_TABLE(JSOESElementIndexUint) };

JSOESElementIndexUint::JSOESElementIndexUint(Structure* structure, JSDOMGlobalObject* globalObject, Ref<OESElementIndexUint>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSOESElementIndexUint::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSOESElementIndexUintPrototype::create(vm, globalObject, JSOESElementIndexUintPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSOESElementIndexUint::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSOESElementIndexUint>(vm, globalObject);
}

void JSOESElementIndexUint::destroy(JSC::JSCell* cell)
{
    JSOESElementIndexUint* thisObject = static_cast<JSOESElementIndexUint*>(cell);
    thisObject->JSOESElementIndexUint::~JSOESElementIndexUint();
}

JSOESElementIndexUint::~JSOESElementIndexUint()
{
    releaseImpl();
}

bool JSOESElementIndexUintOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    auto* jsOESElementIndexUint = jsCast<JSOESElementIndexUint*>(handle.slot()->asCell());
    WebGLRenderingContextBase* root = WTF::getPtr(jsOESElementIndexUint->impl().context());
    return visitor.containsOpaqueRoot(root);
}

void JSOESElementIndexUintOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsOESElementIndexUint = jsCast<JSOESElementIndexUint*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsOESElementIndexUint->impl(), jsOESElementIndexUint);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7OESElementIndexUint@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore19OESElementIndexUintE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, OESElementIndexUint* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSOESElementIndexUint>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7OESElementIndexUint@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore19OESElementIndexUintE[2];
#if COMPILER(CLANG)
    // If this fails OESElementIndexUint does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(OESElementIndexUint), OESElementIndexUint_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // OESElementIndexUint has subclasses. If OESElementIndexUint has subclasses that get passed
    // to toJS() we currently require OESElementIndexUint you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSOESElementIndexUint>(globalObject, impl);
}

OESElementIndexUint* JSOESElementIndexUint::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSOESElementIndexUint*>(value))
        return &wrapper->impl();
    return nullptr;
}

}

#endif // ENABLE(WEBGL)
