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

#ifndef JSSVGViewSpec_h
#define JSSVGViewSpec_h

#if ENABLE(SVG)

#include "JSDOMBinding.h"
#include "SVGElement.h"
#include "SVGViewSpec.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSSVGViewSpec : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSSVGViewSpec* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGViewSpec> impl)
    {
        JSSVGViewSpec* ptr = new (NotNull, JSC::allocateCell<JSSVGViewSpec>(globalObject->vm().heap)) JSSVGViewSpec(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static void put(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName, JSC::JSValue, JSC::PutPropertySlot&);
    static void destroy(JSC::JSCell*);
    ~JSSVGViewSpec();
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    SVGViewSpec& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    SVGViewSpec* m_impl;
protected:
    JSSVGViewSpec(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<SVGViewSpec>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};

class JSSVGViewSpecOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, SVGViewSpec*)
{
    DEFINE_STATIC_LOCAL(JSSVGViewSpecOwner, jsSVGViewSpecOwner, ());
    return &jsSVGViewSpecOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, SVGViewSpec*)
{
    return &world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, SVGViewSpec*);
SVGViewSpec* toSVGViewSpec(JSC::JSValue);

class JSSVGViewSpecPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSSVGViewSpecPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSSVGViewSpecPrototype* ptr = new (NotNull, JSC::allocateCell<JSSVGViewSpecPrototype>(vm.heap)) JSSVGViewSpecPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSSVGViewSpecPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = Base::StructureFlags;
};

class JSSVGViewSpecConstructor : public DOMConstructorObject {
private:
    JSSVGViewSpecConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSSVGViewSpecConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSSVGViewSpecConstructor* ptr = new (NotNull, JSC::allocateCell<JSSVGViewSpecConstructor>(vm.heap)) JSSVGViewSpecConstructor(structure, globalObject);
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

JSC::JSValue jsSVGViewSpecTransform(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecViewTarget(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecViewBoxString(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecPreserveAspectRatioString(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecTransformString(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecViewTargetString(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsSVGViewSpecZoomAndPan(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSSVGViewSpecZoomAndPan(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
#if ENABLE(SVG)
JSC::JSValue jsSVGViewSpecViewBox(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
#if ENABLE(SVG)
JSC::JSValue jsSVGViewSpecPreserveAspectRatio(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
#endif
JSC::JSValue jsSVGViewSpecConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(SVG)

#endif
