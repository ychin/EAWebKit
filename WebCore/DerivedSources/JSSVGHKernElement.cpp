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

#if ENABLE(SVG) && ENABLE(SVG_FONTS)

#include "JSSVGHKernElement.h"

#include "SVGHKernElement.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGHKernElementTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGHKernElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGHKernElementTable = { 2, 1, JSSVGHKernElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGHKernElementConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGHKernElementConstructorTable = { 1, 0, JSSVGHKernElementConstructorTableValues, 0 };
const ClassInfo JSSVGHKernElementConstructor::s_info = { "SVGHKernElementConstructor", &Base::s_info, &JSSVGHKernElementConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGHKernElementConstructor) };

JSSVGHKernElementConstructor::JSSVGHKernElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGHKernElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGHKernElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGHKernElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGHKernElementConstructor, JSDOMWrapper>(exec, JSSVGHKernElementConstructorTable, jsCast<JSSVGHKernElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGHKernElementPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGHKernElementPrototypeTable = { 1, 0, JSSVGHKernElementPrototypeTableValues, 0 };
const ClassInfo JSSVGHKernElementPrototype::s_info = { "SVGHKernElementPrototype", &Base::s_info, &JSSVGHKernElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGHKernElementPrototype) };

JSObject* JSSVGHKernElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGHKernElement>(vm, globalObject);
}

const ClassInfo JSSVGHKernElement::s_info = { "SVGHKernElement", &Base::s_info, &JSSVGHKernElementTable, 0 , CREATE_METHOD_TABLE(JSSVGHKernElement) };

JSSVGHKernElement::JSSVGHKernElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGHKernElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

void JSSVGHKernElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGHKernElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGHKernElementPrototype::create(vm, globalObject, JSSVGHKernElementPrototype::createStructure(vm, globalObject, JSSVGElementPrototype::self(vm, globalObject)));
}

bool JSSVGHKernElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGHKernElement* thisObject = jsCast<JSSVGHKernElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGHKernElement, Base>(exec, JSSVGHKernElementTable, thisObject, propertyName, slot);
}

JSValue jsSVGHKernElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGHKernElement* domObject = jsCast<JSSVGHKernElement*>(asObject(slotBase));
    return JSSVGHKernElement::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSSVGHKernElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGHKernElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(SVG) && ENABLE(SVG_FONTS)