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

#ifndef JSHTMLMeterElement_h
#define JSHTMLMeterElement_h

#if ENABLE(METER_ELEMENT)

#include "HTMLMeterElement.h"
#include "JSDOMBinding.h"
#include "JSHTMLElement.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSHTMLMeterElement : public JSHTMLElement {
public:
    typedef JSHTMLElement Base;
    static JSHTMLMeterElement* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLMeterElement> impl)
    {
        JSHTMLMeterElement* ptr = new (NotNull, JSC::allocateCell<JSHTMLMeterElement>(globalObject->vm().heap)) JSHTMLMeterElement(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static void put(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName, JSC::JSValue, JSC::PutPropertySlot&);
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    HTMLMeterElement& impl() const
    {
        return static_cast<HTMLMeterElement&>(Base::impl());
    }
protected:
    JSHTMLMeterElement(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<HTMLMeterElement>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};


class JSHTMLMeterElementPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSHTMLMeterElementPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSHTMLMeterElementPrototype* ptr = new (NotNull, JSC::allocateCell<JSHTMLMeterElementPrototype>(vm.heap)) JSHTMLMeterElementPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSHTMLMeterElementPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = Base::StructureFlags;
};

class JSHTMLMeterElementConstructor : public DOMConstructorObject {
private:
    JSHTMLMeterElementConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSHTMLMeterElementConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSHTMLMeterElementConstructor* ptr = new (NotNull, JSC::allocateCell<JSHTMLMeterElementConstructor>(vm.heap)) JSHTMLMeterElementConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::ImplementsHasInstance | DOMConstructorObject::StructureFlags;
};

// Attributes

JSC::JSValue jsHTMLMeterElementValue(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementValue(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementMin(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementMin(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementMax(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementMax(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementLow(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementLow(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementHigh(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementHigh(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementOptimum(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLMeterElementOptimum(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLMeterElementLabels(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsHTMLMeterElementConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(METER_ELEMENT)

#endif