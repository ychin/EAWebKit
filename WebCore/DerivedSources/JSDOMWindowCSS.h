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

#ifndef JSDOMWindowCSS_h
#define JSDOMWindowCSS_h

#if ENABLE(CSS3_CONDITIONAL_RULES)

#include "DOMWindowCSS.h"
#include "JSDOMBinding.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSDOMWindowCSS : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSDOMWindowCSS* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<DOMWindowCSS> impl)
    {
        JSDOMWindowCSS* ptr = new (NotNull, JSC::allocateCell<JSDOMWindowCSS>(globalObject->vm().heap)) JSDOMWindowCSS(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static void destroy(JSC::JSCell*);
    ~JSDOMWindowCSS();
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    DOMWindowCSS& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    DOMWindowCSS* m_impl;
protected:
    JSDOMWindowCSS(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<DOMWindowCSS>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = Base::StructureFlags;
};

class JSDOMWindowCSSOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, DOMWindowCSS*)
{
    DEFINE_STATIC_LOCAL(JSDOMWindowCSSOwner, jsDOMWindowCSSOwner, ());
    return &jsDOMWindowCSSOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, DOMWindowCSS*)
{
    return &world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, DOMWindowCSS*);
DOMWindowCSS* toDOMWindowCSS(JSC::JSValue);

class JSDOMWindowCSSPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSDOMWindowCSSPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDOMWindowCSSPrototype* ptr = new (NotNull, JSC::allocateCell<JSDOMWindowCSSPrototype>(vm.heap)) JSDOMWindowCSSPrototype(vm, globalObject, structure);
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
    JSDOMWindowCSSPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsDOMWindowCSSPrototypeFunctionSupports(JSC::ExecState*);

} // namespace WebCore

#endif // ENABLE(CSS3_CONDITIONAL_RULES)

#endif
