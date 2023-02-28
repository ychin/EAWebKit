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
#include "JSDataTransfer.h"

#include "DataTransfer.h"
#include "ExceptionCode.h"
#include "FileList.h"
#include "JSDOMBinding.h"
#include "JSElement.h"
#include "JSFileList.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

#if ENABLE(DATA_TRANSFER_ITEMS)
#include "DataTransferItemList.h"
#include "JSDataTransferItemList.h"
#endif

using namespace JSC;

namespace WebCore {

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionClearData(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionGetData(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionSetData(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionSetDragImage(JSC::ExecState*);

// Attributes

JSC::EncodedJSValue jsDataTransferDropEffect(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSDataTransferDropEffect(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsDataTransferEffectAllowed(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSDataTransferEffectAllowed(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsDataTransferTypes(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsDataTransferFiles(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
#if ENABLE(DATA_TRANSFER_ITEMS)
JSC::EncodedJSValue jsDataTransferItems(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
#endif
JSC::EncodedJSValue jsDataTransferConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSDataTransferPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSDataTransferPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDataTransferPrototype* ptr = new (NotNull, JSC::allocateCell<JSDataTransferPrototype>(vm.heap)) JSDataTransferPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSDataTransferPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSDataTransferConstructor : public DOMConstructorObject {
private:
    JSDataTransferConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSDataTransferConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSDataTransferConstructor* ptr = new (NotNull, JSC::allocateCell<JSDataTransferConstructor>(vm.heap)) JSDataTransferConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

/* Hash table */

static const struct CompactHashIndex JSDataTransferTableIndex[2] = {
    { 0, -1 },
    { -1, -1 },
};


static const HashTableValue JSDataTransferTableValues[] =
{
    { "types", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferTypes), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

static const HashTable JSDataTransferTable = { 1, 1, true, JSDataTransferTableValues, 0, JSDataTransferTableIndex };
const ClassInfo JSDataTransferConstructor::s_info = { "DataTransferConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDataTransferConstructor) };

JSDataTransferConstructor::JSDataTransferConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSDataTransferConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSDataTransfer::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("DataTransfer"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSDataTransferPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "dropEffect", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferDropEffect), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSDataTransferDropEffect) },
    { "effectAllowed", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferEffectAllowed), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSDataTransferEffectAllowed) },
    { "files", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferFiles), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
#if ENABLE(DATA_TRANSFER_ITEMS)
    { "items", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsDataTransferItems), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
#else
    { 0, 0, NoIntrinsic, 0, 0 },
#endif
    { "clearData", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDataTransferPrototypeFunctionClearData), (intptr_t) (0) },
    { "getData", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDataTransferPrototypeFunctionGetData), (intptr_t) (1) },
    { "setData", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDataTransferPrototypeFunctionSetData), (intptr_t) (2) },
    { "setDragImage", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsDataTransferPrototypeFunctionSetDragImage), (intptr_t) (3) },
};

const ClassInfo JSDataTransferPrototype::s_info = { "DataTransferPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSDataTransferPrototype) };

void JSDataTransferPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSDataTransferPrototypeTableValues, *this);
}

const ClassInfo JSDataTransfer::s_info = { "DataTransfer", &Base::s_info, &JSDataTransferTable, CREATE_METHOD_TABLE(JSDataTransfer) };

JSDataTransfer::JSDataTransfer(Structure* structure, JSDOMGlobalObject* globalObject, Ref<DataTransfer>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSDataTransfer::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSDataTransferPrototype::create(vm, globalObject, JSDataTransferPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSDataTransfer::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSDataTransfer>(vm, globalObject);
}

void JSDataTransfer::destroy(JSC::JSCell* cell)
{
    JSDataTransfer* thisObject = static_cast<JSDataTransfer*>(cell);
    thisObject->JSDataTransfer::~JSDataTransfer();
}

JSDataTransfer::~JSDataTransfer()
{
    releaseImpl();
}

bool JSDataTransfer::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSDataTransfer*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSDataTransfer, Base>(exec, JSDataTransferTable, thisObject, propertyName, slot);
}

EncodedJSValue jsDataTransferDropEffect(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DataTransfer", "dropEffect");
        return throwGetterTypeError(*exec, "DataTransfer", "dropEffect");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.dropEffect());
    return JSValue::encode(result);
}


EncodedJSValue jsDataTransferEffectAllowed(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DataTransfer", "effectAllowed");
        return throwGetterTypeError(*exec, "DataTransfer", "effectAllowed");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.effectAllowed());
    return JSValue::encode(result);
}


EncodedJSValue jsDataTransferTypes(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSDataTransfer*>(slotBase);
    return JSValue::encode(castedThis->types(exec));
}


EncodedJSValue jsDataTransferFiles(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DataTransfer", "files");
        return throwGetterTypeError(*exec, "DataTransfer", "files");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.files()));
    return JSValue::encode(result);
}


#if ENABLE(DATA_TRANSFER_ITEMS)
EncodedJSValue jsDataTransferItems(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "DataTransfer", "items");
        return throwGetterTypeError(*exec, "DataTransfer", "items");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.items()));
    return JSValue::encode(result);
}

#endif

EncodedJSValue jsDataTransferConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSDataTransferPrototype* domObject = jsDynamicCast<JSDataTransferPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSDataTransfer::getConstructor(exec->vm(), domObject->globalObject()));
}

void setJSDataTransferDropEffect(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "DataTransfer", "dropEffect");
        else
            throwSetterTypeError(*exec, "DataTransfer", "dropEffect");
        return;
    }
    auto& impl = castedThis->impl();
    String nativeValue = value.toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setDropEffect(nativeValue);
}


void setJSDataTransferEffectAllowed(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSDataTransferPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "DataTransfer", "effectAllowed");
        else
            throwSetterTypeError(*exec, "DataTransfer", "effectAllowed");
        return;
    }
    auto& impl = castedThis->impl();
    String nativeValue = value.toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    impl.setEffectAllowed(nativeValue);
}


JSValue JSDataTransfer::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSDataTransferConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionClearData(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DataTransfer", "clearData");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDataTransfer::info());
    auto& impl = castedThis->impl();

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 0) {
        impl.clearData();
        return JSValue::encode(jsUndefined());
    }

    String type = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.clearData(type);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionGetData(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DataTransfer", "getData");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDataTransfer::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String type = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsStringWithCache(exec, impl.getData(type));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionSetData(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DataTransfer", "setData");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDataTransfer::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 2))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    String type = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    String data = exec->argument(1).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.setData(type, data);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsDataTransferPrototypeFunctionSetDragImage(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSDataTransfer* castedThis = jsDynamicCast<JSDataTransfer*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "DataTransfer", "setDragImage");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSDataTransfer::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 3))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    Element* image = JSElement::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    int x = toInt32(exec, exec->argument(1), NormalConversion);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    int y = toInt32(exec, exec->argument(2), NormalConversion);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.setDragImage(image, x, y);
    return JSValue::encode(jsUndefined());
}

bool JSDataTransferOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSDataTransferOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsDataTransfer = jsCast<JSDataTransfer*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsDataTransfer->impl(), jsDataTransfer);
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, DataTransfer* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSDataTransfer>(globalObject, impl))
        return result;
    return createNewWrapper<JSDataTransfer>(globalObject, impl);
}

DataTransfer* JSDataTransfer::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSDataTransfer*>(value))
        return &wrapper->impl();
    return nullptr;
}

}