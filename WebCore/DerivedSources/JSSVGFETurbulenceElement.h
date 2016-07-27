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

#ifndef JSSVGFETurbulenceElement_h
#define JSSVGFETurbulenceElement_h

#if ENABLE(FILTERS) && ENABLE(SVG)

#include "JSDOMBinding.h"
#include "JSSVGElement.h"
#include "SVGElement.h"
#include "SVGFETurbulenceElement.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSSVGFETurbulenceElement : public JSSVGElement {
public:
    typedef JSSVGElement Base;
    static JSSVGFETurbulenceElement* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGFETurbulenceElement> impl)
    {
        JSSVGFETurbulenceElement* ptr = new (NotNull, JSC::allocateCell<JSSVGFETurbulenceElement>(globalObject->vm().heap)) JSSVGFETurbulenceElement(structure, globalObject, impl);
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
    SVGFETurbulenceElement& impl() const
    {
        return static_cast<SVGFETurbulenceElement&>(Base::impl());
    }
protected:
    JSSVGFETurbulenceElement(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<SVGFETurbulenceElement>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};


class JSSVGFETurbulenceElementPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSSVGFETurbulenceElementPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSSVGFETurbulenceElementPrototype* ptr = new (NotNull, JSC::allocateCell<JSSVGFETurbulenceElementPrototype>(vm.heap)) JSSVGFETurbulenceElementPrototype(vm, globalObject, structure);
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
    JSSVGFETurbulenceElementPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSSVGFETurbulenceElementConstructor : public DOMConstructorObject {
private:
    JSSVGFETurbulenceElementConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSSVGFETurbulenceElementConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSSVGFETurbulenceElementConstructor* ptr = new (NotNull, JSC::allocateCell<JSSVGFETurbulenceElementConstructor>(vm.heap)) JSSVGFETurbulenceElementConstructor(structure, globalObject);
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

JSC::JSValue jsSVGFETurbulenceElementBaseFrequencyX(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementBaseFrequencyY(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementNumOctaves(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSeed(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementStitchTiles(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementType(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#if ENABLE(SVG)
JSC::JSValue jsSVGFETurbulenceElementX(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGFETurbulenceElementY(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGFETurbulenceElementWidth(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGFETurbulenceElementHeight(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGFETurbulenceElementResult(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
JSC::JSValue jsSVGFETurbulenceElementConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
// Constants

JSC::JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_UNKNOWN(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_FRACTALNOISE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_TURBULENCE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_UNKNOWN(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_STITCH(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_NOSTITCH(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(FILTERS) && ENABLE(SVG)

#endif
