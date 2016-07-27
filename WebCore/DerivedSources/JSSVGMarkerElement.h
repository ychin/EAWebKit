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

#ifndef JSSVGMarkerElement_h
#define JSSVGMarkerElement_h

#if ENABLE(SVG)

#include "JSDOMBinding.h"
#include "JSSVGElement.h"
#include "SVGElement.h"
#include "SVGMarkerElement.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSSVGMarkerElement : public JSSVGElement {
public:
    typedef JSSVGElement Base;
    static JSSVGMarkerElement* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGMarkerElement> impl)
    {
        JSSVGMarkerElement* ptr = new (NotNull, JSC::allocateCell<JSSVGMarkerElement>(globalObject->vm().heap)) JSSVGMarkerElement(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    SVGMarkerElement& impl() const
    {
        return static_cast<SVGMarkerElement&>(Base::impl());
    }
protected:
    JSSVGMarkerElement(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<SVGMarkerElement>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};


class JSSVGMarkerElementPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSSVGMarkerElementPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSSVGMarkerElementPrototype* ptr = new (NotNull, JSC::allocateCell<JSSVGMarkerElementPrototype>(vm.heap)) JSSVGMarkerElementPrototype(vm, globalObject, structure);
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
    JSSVGMarkerElementPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSSVGMarkerElementConstructor : public DOMConstructorObject {
private:
    JSSVGMarkerElementConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSSVGMarkerElementConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSSVGMarkerElementConstructor* ptr = new (NotNull, JSC::allocateCell<JSSVGMarkerElementConstructor>(vm.heap)) JSSVGMarkerElementConstructor(structure, globalObject);
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

JSC::EncodedJSValue JSC_HOST_CALL jsSVGMarkerElementPrototypeFunctionSetOrientToAuto(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsSVGMarkerElementPrototypeFunctionSetOrientToAngle(JSC::ExecState*);
// Attributes

JSC::JSValue jsSVGMarkerElementRefX(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementRefY(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementMarkerUnits(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementMarkerWidth(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementMarkerHeight(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementOrientType(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementOrientAngle(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#if ENABLE(SVG)
JSC::JSValue jsSVGMarkerElementExternalResourcesRequired(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGMarkerElementViewBox(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGMarkerElementPreserveAspectRatio(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
JSC::JSValue jsSVGMarkerElementConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
// Constants

JSC::JSValue jsSVGMarkerElementSVG_MARKERUNITS_UNKNOWN(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementSVG_MARKERUNITS_USERSPACEONUSE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementSVG_MARKERUNITS_STROKEWIDTH(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementSVG_MARKER_ORIENT_UNKNOWN(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementSVG_MARKER_ORIENT_AUTO(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGMarkerElementSVG_MARKER_ORIENT_ANGLE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(SVG)

#endif
