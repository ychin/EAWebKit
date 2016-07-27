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

#include "JSSVGGElement.h"

#include "SVGGElement.h"
#include <wtf/GetPtr.h>

#if ENABLE(SVG)
#include "JSSVGAnimatedBoolean.h"
#endif

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGGElementTableValues[] =
{
#if ENABLE(SVG)
    { "externalResourcesRequired", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGElementExternalResourcesRequired), (intptr_t)0 },
#endif
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGGElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGElementTable = { 5, 3, JSSVGGElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGGElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGElementConstructorTable = { 1, 0, JSSVGGElementConstructorTableValues, 0 };
const ClassInfo JSSVGGElementConstructor::s_info = { "SVGGElementConstructor", &Base::s_info, &JSSVGGElementConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGGElementConstructor) };

JSSVGGElementConstructor::JSSVGGElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGGElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGGElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGGElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGGElementConstructor, JSDOMWrapper>(exec, JSSVGGElementConstructorTable, jsCast<JSSVGGElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGGElementPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGGElementPrototypeTable = { 1, 0, JSSVGGElementPrototypeTableValues, 0 };
const ClassInfo JSSVGGElementPrototype::s_info = { "SVGGElementPrototype", &Base::s_info, &JSSVGGElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGGElementPrototype) };

JSObject* JSSVGGElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGGElement>(vm, globalObject);
}

const ClassInfo JSSVGGElement::s_info = { "SVGGElement", &Base::s_info, &JSSVGGElementTable, 0 , CREATE_METHOD_TABLE(JSSVGGElement) };

JSSVGGElement::JSSVGGElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGGElement> impl)
    : JSSVGGraphicsElement(structure, globalObject, impl)
{
}

void JSSVGGElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGGElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGGElementPrototype::create(vm, globalObject, JSSVGGElementPrototype::createStructure(vm, globalObject, JSSVGGraphicsElementPrototype::self(vm, globalObject)));
}

bool JSSVGGElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGGElement* thisObject = jsCast<JSSVGGElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGGElement, Base>(exec, JSSVGGElementTable, thisObject, propertyName, slot);
}

#if ENABLE(SVG)
JSValue jsSVGGElementExternalResourcesRequired(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGElement* castedThis = jsCast<JSSVGGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedBoolean> obj = impl.externalResourcesRequiredAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

JSValue jsSVGGElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGGElement* domObject = jsCast<JSSVGGElement*>(asObject(slotBase));
    return JSSVGGElement::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGGElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGGElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(SVG)
