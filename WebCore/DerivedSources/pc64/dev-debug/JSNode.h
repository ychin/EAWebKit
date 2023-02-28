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

#ifndef JSNode_h
#define JSNode_h

#include "JSDOMWrapper.h"
#include "Node.h"
#include <wtf/NeverDestroyed.h>

namespace WebCore {

class WEBCORE_EXPORT JSNode : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSNode* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, Ref<Node>&& impl)
    {
        JSNode* ptr = new (NotNull, JSC::allocateCell<JSNode>(globalObject->vm().heap)) JSNode(structure, globalObject, WTF::move(impl));
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static JSC::JSObject* getPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static Node* toWrapped(JSC::JSValue);
    static void destroy(JSC::JSCell*);
    ~JSNode();

protected:
    static const JSC::ClassInfo s_info;
public:
    static const JSC::ClassInfo* info() { return &s_info; }

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::JSType(JSNodeType), StructureFlags), info());
    }

    JSC::JSScope* pushEventHandlerScope(JSC::ExecState*, JSC::JSScope*) const;

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    static void visitChildren(JSCell*, JSC::SlotVisitor&);
    void visitAdditionalChildren(JSC::SlotVisitor&);


    // Custom functions
    JSC::JSValue insertBefore(JSC::ExecState*);
    JSC::JSValue replaceChild(JSC::ExecState*);
    JSC::JSValue removeChild(JSC::ExecState*);
    JSC::JSValue appendChild(JSC::ExecState*);
    Node& impl() const { return *m_impl; }
    void releaseImpl() { std::exchange(m_impl, nullptr)->deref(); }

private:
    Node* m_impl;
protected:
    JSNode(JSC::Structure*, JSDOMGlobalObject*, Ref<Node>&&);

    void finishCreation(JSC::VM& vm)
    {
        Base::finishCreation(vm);
        ASSERT(inherits(info()));
    }

};

class JSNodeOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, Node*)
{
    static NeverDestroyed<JSNodeOwner> owner;
    return &owner.get();
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, Node*);
inline JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, Node& impl) { return toJS(exec, globalObject, &impl); }
JSC::JSValue toJSNewlyCreated(JSC::ExecState*, JSDOMGlobalObject*, Node*);


} // namespace WebCore

#endif
#include "JSNodeCustom.h"
