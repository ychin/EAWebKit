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

#if ENABLE(SVG)

#include "JSSVGAnimatedLengthList.h"

#include "JSSVGLengthList.h"
#include "SVGLengthList.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGAnimatedLengthListTableValues[] =
{
    { "baseVal", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedLengthListBaseVal), (intptr_t)0 },
    { "animVal", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedLengthListAnimVal), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGAnimatedLengthListConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedLengthListTable = { 8, 7, JSSVGAnimatedLengthListTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGAnimatedLengthListConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedLengthListConstructorTable = { 1, 0, JSSVGAnimatedLengthListConstructorTableValues, 0 };
const ClassInfo JSSVGAnimatedLengthListConstructor::s_info = { "SVGAnimatedLengthListConstructor", &Base::s_info, &JSSVGAnimatedLengthListConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGAnimatedLengthListConstructor) };

JSSVGAnimatedLengthListConstructor::JSSVGAnimatedLengthListConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGAnimatedLengthListConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGAnimatedLengthListPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGAnimatedLengthListConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGAnimatedLengthListConstructor, JSDOMWrapper>(exec, JSSVGAnimatedLengthListConstructorTable, jsCast<JSSVGAnimatedLengthListConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGAnimatedLengthListPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGAnimatedLengthListPrototypeTable = { 1, 0, JSSVGAnimatedLengthListPrototypeTableValues, 0 };
const ClassInfo JSSVGAnimatedLengthListPrototype::s_info = { "SVGAnimatedLengthListPrototype", &Base::s_info, &JSSVGAnimatedLengthListPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGAnimatedLengthListPrototype) };

JSObject* JSSVGAnimatedLengthListPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGAnimatedLengthList>(vm, globalObject);
}

const ClassInfo JSSVGAnimatedLengthList::s_info = { "SVGAnimatedLengthList", &Base::s_info, &JSSVGAnimatedLengthListTable, 0 , CREATE_METHOD_TABLE(JSSVGAnimatedLengthList) };

JSSVGAnimatedLengthList::JSSVGAnimatedLengthList(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGAnimatedLengthList> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSSVGAnimatedLengthList::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGAnimatedLengthList::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGAnimatedLengthListPrototype::create(vm, globalObject, JSSVGAnimatedLengthListPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSSVGAnimatedLengthList::destroy(JSC::JSCell* cell)
{
    JSSVGAnimatedLengthList* thisObject = static_cast<JSSVGAnimatedLengthList*>(cell);
    thisObject->JSSVGAnimatedLengthList::~JSSVGAnimatedLengthList();
}

JSSVGAnimatedLengthList::~JSSVGAnimatedLengthList()
{
    releaseImplIfNotNull();
}

bool JSSVGAnimatedLengthList::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGAnimatedLengthList* thisObject = jsCast<JSSVGAnimatedLengthList*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGAnimatedLengthList, Base>(exec, JSSVGAnimatedLengthListTable, thisObject, propertyName, slot);
}

JSValue jsSVGAnimatedLengthListBaseVal(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedLengthList* castedThis = jsCast<JSSVGAnimatedLengthList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGAnimatedLengthList& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(static_cast<SVGListPropertyTearOff<SVGLengthList>*>(impl.baseVal())));
    return result;
}


JSValue jsSVGAnimatedLengthListAnimVal(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedLengthList* castedThis = jsCast<JSSVGAnimatedLengthList*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGAnimatedLengthList& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(static_cast<SVGListPropertyTearOff<SVGLengthList>*>(impl.animVal())));
    return result;
}


JSValue jsSVGAnimatedLengthListConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGAnimatedLengthList* domObject = jsCast<JSSVGAnimatedLengthList*>(asObject(slotBase));
    return JSSVGAnimatedLengthList::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGAnimatedLengthList::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGAnimatedLengthListConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static inline bool isObservable(JSSVGAnimatedLengthList* jsSVGAnimatedLengthList)
{
    if (jsSVGAnimatedLengthList->hasCustomProperties())
        return true;
    return false;
}

bool JSSVGAnimatedLengthListOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSSVGAnimatedLengthList* jsSVGAnimatedLengthList = jsCast<JSSVGAnimatedLengthList*>(handle.get().asCell());
    if (!isObservable(jsSVGAnimatedLengthList))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSSVGAnimatedLengthListOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSSVGAnimatedLengthList* jsSVGAnimatedLengthList = jsCast<JSSVGAnimatedLengthList*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsSVGAnimatedLengthList->impl(), jsSVGAnimatedLengthList);
    jsSVGAnimatedLengthList->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, SVGAnimatedLengthList* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSSVGAnimatedLengthList>(exec, impl))
        return result;
    ReportMemoryCost<SVGAnimatedLengthList>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSSVGAnimatedLengthList>(exec, globalObject, impl);
}

SVGAnimatedLengthList* toSVGAnimatedLengthList(JSC::JSValue value)
{
    return value.inherits(JSSVGAnimatedLengthList::info()) ? &jsCast<JSSVGAnimatedLengthList*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(SVG)
