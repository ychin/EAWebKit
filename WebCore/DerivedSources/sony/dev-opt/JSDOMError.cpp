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
#include "JSDOMError.h"

#include "DOMError.h"
#include "JSDOMBinding.h"
#include "URL.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsDOMErrorName(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSDOMErrorPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSDOMErrorPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDOMErrorPrototype* ptr = new (NotNull, JSC::allocateCell<JSDOMErrorPrototype>(vm.heap)) JSDOMErrorPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSDOMErrorPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

/* Hash table */

static const struct CompactHashIndex JSDOMErrorTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const HashTableValue JSDOMErrorTableValues[] =
{
    { "name", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDOMErrorName), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

static const HashTable JSDOMErrorTable = { 1, 1, true, JSDOMErrorTableValues, 0, JSDOMErrorTableIndex };
/* Hash table for prototype */

static const HashTableValue JSDOMErrorPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

const ClassInfo JSDOMErrorPrototype::s_info = { "DOMErrorPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDOMErrorPrototype) };

void JSDOMErrorPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSDOMErrorPrototypeTableValues, *this);
}

const ClassInfo JSDOMError::s_info = { "DOMError", &Base::s_info, &JSDOMErrorTable, CREATE_METHOD_TABLE(JSDOMError) };

JSDOMError::JSDOMError(Structure* structure, JSDOMGlobalObject* globalObject, Ref<DOMError>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSDOMError::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSDOMErrorPrototype::create(vm, globalObject, JSDOMErrorPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSDOMError::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSDOMError>(vm, globalObject);
}

void JSDOMError::destroy(JSC::JSCell* cell)
{
    JSDOMError* thisObject = static_cast<JSDOMError*>(cell);
    thisObject->JSDOMError::~JSDOMError();
}

JSDOMError::~JSDOMError()
{
    releaseImpl();
}

bool JSDOMError::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSDOMError*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSDOMError, Base>(exec, JSDOMErrorTable, thisObject, propertyName, slot);
}

EncodedJSValue jsDOMErrorName(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSDOMError*>(slotBase);
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.name());
    return JSValue::encode(result);
}


bool JSDOMErrorOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSDOMErrorOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsDOMError = jsCast<JSDOMError*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsDOMError->impl(), jsDOMError);
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, DOMError* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSDOMError>(globalObject, impl))
        return result;
    return createNewWrapper<JSDOMError>(globalObject, impl);
}

DOMError* JSDOMError::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSDOMError*>(value))
        return &wrapper->impl();
    return nullptr;
}

}