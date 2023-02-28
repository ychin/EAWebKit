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
#include "JSMallocStatistics.h"

#include "JSDOMBinding.h"
#include "MallocStatistics.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsMallocStatisticsReservedVMBytes(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMallocStatisticsCommittedVMBytes(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsMallocStatisticsFreeListBytes(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSMallocStatisticsPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSMallocStatisticsPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSMallocStatisticsPrototype* ptr = new (NotNull, JSC::allocateCell<JSMallocStatisticsPrototype>(vm.heap)) JSMallocStatisticsPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSMallocStatisticsPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

/* Hash table for prototype */

static const HashTableValue JSMallocStatisticsPrototypeTableValues[] =
{
    { "reservedVMBytes", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMallocStatisticsReservedVMBytes), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "committedVMBytes", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMallocStatisticsCommittedVMBytes), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "freeListBytes", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMallocStatisticsFreeListBytes), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSMallocStatisticsPrototype::s_info = { "MallocStatisticsPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSMallocStatisticsPrototype) };

void JSMallocStatisticsPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSMallocStatisticsPrototypeTableValues, *this);
}

const ClassInfo JSMallocStatistics::s_info = { "MallocStatistics", &Base::s_info, 0, CREATE_METHOD_TABLE(JSMallocStatistics) };

JSMallocStatistics::JSMallocStatistics(Structure* structure, JSDOMGlobalObject* globalObject, Ref<MallocStatistics>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSMallocStatistics::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSMallocStatisticsPrototype::create(vm, globalObject, JSMallocStatisticsPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSMallocStatistics::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSMallocStatistics>(vm, globalObject);
}

void JSMallocStatistics::destroy(JSC::JSCell* cell)
{
    JSMallocStatistics* thisObject = static_cast<JSMallocStatistics*>(cell);
    thisObject->JSMallocStatistics::~JSMallocStatistics();
}

JSMallocStatistics::~JSMallocStatistics()
{
    releaseImpl();
}

EncodedJSValue jsMallocStatisticsReservedVMBytes(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMallocStatistics* castedThis = jsDynamicCast<JSMallocStatistics*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMallocStatisticsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MallocStatistics", "reservedVMBytes");
        return throwGetterTypeError(*exec, "MallocStatistics", "reservedVMBytes");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.reservedVMBytes());
    return JSValue::encode(result);
}


EncodedJSValue jsMallocStatisticsCommittedVMBytes(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMallocStatistics* castedThis = jsDynamicCast<JSMallocStatistics*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMallocStatisticsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MallocStatistics", "committedVMBytes");
        return throwGetterTypeError(*exec, "MallocStatistics", "committedVMBytes");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.committedVMBytes());
    return JSValue::encode(result);
}


EncodedJSValue jsMallocStatisticsFreeListBytes(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSMallocStatistics* castedThis = jsDynamicCast<JSMallocStatistics*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSMallocStatisticsPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "MallocStatistics", "freeListBytes");
        return throwGetterTypeError(*exec, "MallocStatistics", "freeListBytes");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.freeListBytes());
    return JSValue::encode(result);
}


bool JSMallocStatisticsOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSMallocStatisticsOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsMallocStatistics = jsCast<JSMallocStatistics*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsMallocStatistics->impl(), jsMallocStatistics);
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, MallocStatistics* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSMallocStatistics>(globalObject, impl))
        return result;
#if COMPILER(CLANG)
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to MallocStatistics.
    COMPILE_ASSERT(!__is_polymorphic(MallocStatistics), MallocStatistics_is_polymorphic_but_idl_claims_not_to_be);
#endif
    return createNewWrapper<JSMallocStatistics>(globalObject, impl);
}

MallocStatistics* JSMallocStatistics::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSMallocStatistics*>(value))
        return &wrapper->impl();
    return nullptr;
}

}
