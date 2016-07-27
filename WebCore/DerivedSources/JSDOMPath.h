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

#ifndef JSDOMPath_h
#define JSDOMPath_h

#if ENABLE(CANVAS_PATH)

#include "DOMPath.h"
#include "JSDOMBinding.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSDOMPath : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSDOMPath* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<DOMPath> impl)
    {
        JSDOMPath* ptr = new (NotNull, JSC::allocateCell<JSDOMPath>(globalObject->vm().heap)) JSDOMPath(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static void destroy(JSC::JSCell*);
    ~JSDOMPath();
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    DOMPath& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    DOMPath* m_impl;
protected:
    JSDOMPath(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<DOMPath>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};

class JSDOMPathOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, DOMPath*)
{
    DEFINE_STATIC_LOCAL(JSDOMPathOwner, jsDOMPathOwner, ());
    return &jsDOMPathOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, DOMPath*)
{
    return &world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, DOMPath*);
DOMPath* toDOMPath(JSC::JSValue);

class JSDOMPathPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSDOMPathPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDOMPathPrototype* ptr = new (NotNull, JSC::allocateCell<JSDOMPathPrototype>(vm.heap)) JSDOMPathPrototype(vm, globalObject, structure);
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
    JSDOMPathPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSDOMPathConstructor : public DOMConstructorObject {
private:
    JSDOMPathConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSDOMPathConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSDOMPathConstructor* ptr = new (NotNull, JSC::allocateCell<JSDOMPathConstructor>(vm.heap)) JSDOMPathConstructor(structure, globalObject);
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
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSDOMPath(JSC::ExecState*);
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSDOMPath1(JSC::ExecState*);
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSDOMPath2(JSC::ExecState*);
    static JSC::EncodedJSValue JSC_HOST_CALL constructJSDOMPath3(JSC::ExecState*);
    static JSC::ConstructType getConstructData(JSC::JSCell*, JSC::ConstructData&);
};

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionClosePath(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionMoveTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionLineTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionQuadraticCurveTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionBezierCurveTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionArcTo(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionRect(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsDOMPathPrototypeFunctionArc(JSC::ExecState*);
// Attributes

JSC::JSValue jsDOMPathConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(CANVAS_PATH)

#endif
