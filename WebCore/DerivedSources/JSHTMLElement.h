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

#ifndef JSHTMLElement_h
#define JSHTMLElement_h

#include "HTMLElement.h"
#include "JSDOMBinding.h"
#include "JSElement.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSHTMLElement : public JSElement {
public:
    typedef JSElement Base;
    static JSHTMLElement* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLElement> impl)
    {
        JSHTMLElement* ptr = new (NotNull, JSC::allocateCell<JSHTMLElement>(globalObject->vm().heap)) JSHTMLElement(structure, globalObject, impl);
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

    JSC::JSScope* pushEventHandlerScope(JSC::ExecState*, JSC::JSScope*) const;

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    HTMLElement& impl() const
    {
        return static_cast<HTMLElement&>(Base::impl());
    }
protected:
    JSHTMLElement(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<HTMLElement>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};

HTMLElement* toHTMLElement(JSC::JSValue);

class JSHTMLElementPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSHTMLElementPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSHTMLElementPrototype* ptr = new (NotNull, JSC::allocateCell<JSHTMLElementPrototype>(vm.heap)) JSHTMLElementPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSHTMLElementPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSHTMLElementConstructor : public DOMConstructorObject {
private:
    JSHTMLElementConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSHTMLElementConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSHTMLElementConstructor* ptr = new (NotNull, JSC::allocateCell<JSHTMLElementConstructor>(vm.heap)) JSHTMLElementConstructor(structure, globalObject);
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

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsHTMLElementPrototypeFunctionInsertAdjacentElement(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsHTMLElementPrototypeFunctionInsertAdjacentHTML(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsHTMLElementPrototypeFunctionInsertAdjacentText(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsHTMLElementPrototypeFunctionClick(JSC::ExecState*);
// Attributes

JSC::JSValue jsHTMLElementTitle(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementTitle(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementLang(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementLang(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementTranslate(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementTranslate(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementDir(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementDir(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementTabIndex(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementTabIndex(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementDraggable(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementDraggable(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementWebkitdropzone(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementWebkitdropzone(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementHidden(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementHidden(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementAccessKey(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementAccessKey(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementInnerHTML(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementInnerHTML(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementInnerText(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementInnerText(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementOuterHTML(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementOuterHTML(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementOuterText(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementOuterText(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementChildren(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsHTMLElementContentEditable(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementContentEditable(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementIsContentEditable(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsHTMLElementSpellcheck(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLElementSpellcheck(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLElementConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif
