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

#ifndef JSHTMLSpanElement_h
#define JSHTMLSpanElement_h

#include "HTMLSpanElement.h"
#include "JSHTMLElement.h"

namespace WebCore {

class JSHTMLSpanElement : public JSHTMLElement {
public:
    typedef JSHTMLElement Base;
    static JSHTMLSpanElement* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, Ref<HTMLSpanElement>&& impl)
    {
        JSHTMLSpanElement* ptr = new (NotNull, JSC::allocateCell<JSHTMLSpanElement>(globalObject->vm().heap)) JSHTMLSpanElement(structure, globalObject, WTF::move(impl));
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static JSC::JSObject* getPrototype(JSC::VM&, JSC::JSGlobalObject*);

    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::JSType(JSElementType), StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    HTMLSpanElement& impl() const
    {
        return static_cast<HTMLSpanElement&>(Base::impl());
    }
protected:
    JSHTMLSpanElement(JSC::Structure*, JSDOMGlobalObject*, Ref<HTMLSpanElement>&&);

    void finishCreation(JSC::VM& vm)
    {
        Base::finishCreation(vm);
        ASSERT(inherits(info()));
    }

};



} // namespace WebCore

#endif
