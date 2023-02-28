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
#include "JSWebKitCSSFilterValue.h"

#include "JSDOMBinding.h"
#include "WebKitCSSFilterValue.h"
#include <runtime/PropertyNameArray.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Attributes

JSC::EncodedJSValue jsWebKitCSSFilterValueOperationType(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsWebKitCSSFilterValueConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSWebKitCSSFilterValuePrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSWebKitCSSFilterValuePrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSWebKitCSSFilterValuePrototype* ptr = new (NotNull, JSC::allocateCell<JSWebKitCSSFilterValuePrototype>(vm.heap)) JSWebKitCSSFilterValuePrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSWebKitCSSFilterValuePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSWebKitCSSFilterValueConstructor : public DOMConstructorObject {
private:
    JSWebKitCSSFilterValueConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSWebKitCSSFilterValueConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSWebKitCSSFilterValueConstructor* ptr = new (NotNull, JSC::allocateCell<JSWebKitCSSFilterValueConstructor>(vm.heap)) JSWebKitCSSFilterValueConstructor(structure, globalObject);
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

static const struct CompactHashIndex JSWebKitCSSFilterValueTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const HashTableValue JSWebKitCSSFilterValueTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitCSSFilterValueConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

static const HashTable JSWebKitCSSFilterValueTable = { 1, 1, true, JSWebKitCSSFilterValueTableValues, 0, JSWebKitCSSFilterValueTableIndex };
/* Hash table for constructor */

static const HashTableValue JSWebKitCSSFilterValueConstructorTableValues[] =
{
    { "CSS_FILTER_REFERENCE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(1), (intptr_t) (0) },
    { "CSS_FILTER_GRAYSCALE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(2), (intptr_t) (0) },
    { "CSS_FILTER_SEPIA", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(3), (intptr_t) (0) },
    { "CSS_FILTER_SATURATE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(4), (intptr_t) (0) },
    { "CSS_FILTER_HUE_ROTATE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(5), (intptr_t) (0) },
    { "CSS_FILTER_INVERT", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(6), (intptr_t) (0) },
    { "CSS_FILTER_OPACITY", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(7), (intptr_t) (0) },
    { "CSS_FILTER_BRIGHTNESS", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(8), (intptr_t) (0) },
    { "CSS_FILTER_CONTRAST", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(9), (intptr_t) (0) },
    { "CSS_FILTER_BLUR", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(10), (intptr_t) (0) },
    { "CSS_FILTER_DROP_SHADOW", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(11), (intptr_t) (0) },
};

const ClassInfo JSWebKitCSSFilterValueConstructor::s_info = { "WebKitCSSFilterValueConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebKitCSSFilterValueConstructor) };

JSWebKitCSSFilterValueConstructor::JSWebKitCSSFilterValueConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSWebKitCSSFilterValueConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSWebKitCSSFilterValue::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("WebKitCSSFilterValue"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
    reifyStaticProperties(vm, JSWebKitCSSFilterValueConstructorTableValues, *this);
}

/* Hash table for prototype */

static const HashTableValue JSWebKitCSSFilterValuePrototypeTableValues[] =
{
    { "operationType", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsWebKitCSSFilterValueOperationType), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "CSS_FILTER_REFERENCE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(1), (intptr_t) (0) },
    { "CSS_FILTER_GRAYSCALE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(2), (intptr_t) (0) },
    { "CSS_FILTER_SEPIA", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(3), (intptr_t) (0) },
    { "CSS_FILTER_SATURATE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(4), (intptr_t) (0) },
    { "CSS_FILTER_HUE_ROTATE", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(5), (intptr_t) (0) },
    { "CSS_FILTER_INVERT", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(6), (intptr_t) (0) },
    { "CSS_FILTER_OPACITY", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(7), (intptr_t) (0) },
    { "CSS_FILTER_BRIGHTNESS", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(8), (intptr_t) (0) },
    { "CSS_FILTER_CONTRAST", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(9), (intptr_t) (0) },
    { "CSS_FILTER_BLUR", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(10), (intptr_t) (0) },
    { "CSS_FILTER_DROP_SHADOW", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, (intptr_t)(11), (intptr_t) (0) },
};

const ClassInfo JSWebKitCSSFilterValuePrototype::s_info = { "WebKitCSSFilterValuePrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWebKitCSSFilterValuePrototype) };

void JSWebKitCSSFilterValuePrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSWebKitCSSFilterValuePrototypeTableValues, *this);
}

const ClassInfo JSWebKitCSSFilterValue::s_info = { "WebKitCSSFilterValue", &Base::s_info, &JSWebKitCSSFilterValueTable, CREATE_METHOD_TABLE(JSWebKitCSSFilterValue) };

JSWebKitCSSFilterValue::JSWebKitCSSFilterValue(Structure* structure, JSDOMGlobalObject* globalObject, Ref<WebKitCSSFilterValue>&& impl)
    : JSCSSValueList(structure, globalObject, WTF::move(impl))
{
}

JSObject* JSWebKitCSSFilterValue::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSWebKitCSSFilterValuePrototype::create(vm, globalObject, JSWebKitCSSFilterValuePrototype::createStructure(vm, globalObject, JSCSSValueList::getPrototype(vm, globalObject)));
}

JSObject* JSWebKitCSSFilterValue::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSWebKitCSSFilterValue>(vm, globalObject);
}

bool JSWebKitCSSFilterValue::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSWebKitCSSFilterValue*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    const HashTableValue* entry = getStaticValueSlotEntryWithoutCaching<JSWebKitCSSFilterValue>(exec, propertyName);
    if (entry) {
        slot.setCacheableCustom(thisObject, entry->attributes(), entry->propertyGetter());
        return true;
    }
    Optional<uint32_t> optionalIndex = parseIndex(propertyName);
    if (optionalIndex && optionalIndex.value() < thisObject->impl().length()) {
        unsigned index = optionalIndex.value();
        unsigned attributes = DontDelete | ReadOnly;
        slot.setValue(thisObject, attributes, toJS(exec, thisObject->globalObject(), thisObject->impl().item(index)));
        return true;
    }
    return getStaticValueSlot<JSWebKitCSSFilterValue, Base>(exec, JSWebKitCSSFilterValueTable, thisObject, propertyName, slot);
}

bool JSWebKitCSSFilterValue::getOwnPropertySlotByIndex(JSObject* object, ExecState* exec, unsigned index, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSWebKitCSSFilterValue*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (index < thisObject->impl().length()) {
        unsigned attributes = DontDelete | ReadOnly;
        slot.setValue(thisObject, attributes, toJS(exec, thisObject->globalObject(), thisObject->impl().item(index)));
        return true;
    }
    return Base::getOwnPropertySlotByIndex(thisObject, exec, index, slot);
}

EncodedJSValue jsWebKitCSSFilterValueOperationType(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSWebKitCSSFilterValue* castedThis = jsDynamicCast<JSWebKitCSSFilterValue*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSWebKitCSSFilterValuePrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "WebKitCSSFilterValue", "operationType");
        return throwGetterTypeError(*exec, "WebKitCSSFilterValue", "operationType");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsNumber(impl.operationType());
    return JSValue::encode(result);
}


EncodedJSValue jsWebKitCSSFilterValueConstructor(ExecState* exec, JSObject*, EncodedJSValue thisValue, PropertyName)
{
    JSWebKitCSSFilterValue* domObject = jsDynamicCast<JSWebKitCSSFilterValue*>(JSValue::decode(thisValue));
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSWebKitCSSFilterValue::getConstructor(exec->vm(), domObject->globalObject()));
}

void JSWebKitCSSFilterValue::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    auto* thisObject = jsCast<JSWebKitCSSFilterValue*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    for (unsigned i = 0, count = thisObject->impl().length(); i < count; ++i)
        propertyNames.add(Identifier::from(exec, i));
    Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

JSValue JSWebKitCSSFilterValue::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSWebKitCSSFilterValueConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}
