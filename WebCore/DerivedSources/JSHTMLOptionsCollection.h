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

#ifndef JSHTMLOptionsCollection_h
#define JSHTMLOptionsCollection_h

#include "HTMLOptionsCollection.h"
#include "JSDOMBinding.h"
#include "JSHTMLCollection.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSHTMLOptionsCollection : public JSHTMLCollection {
public:
    typedef JSHTMLCollection Base;
    static JSHTMLOptionsCollection* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLOptionsCollection> impl)
    {
        JSHTMLOptionsCollection* ptr = new (NotNull, JSC::allocateCell<JSHTMLOptionsCollection>(globalObject->vm().heap)) JSHTMLOptionsCollection(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static void put(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName, JSC::JSValue, JSC::PutPropertySlot&);
    static void putByIndex(JSC::JSCell*, JSC::ExecState*, unsigned propertyName, JSC::JSValue, bool shouldThrow);
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);

    // Custom attributes
    void setLength(JSC::ExecState*, JSC::JSValue);

    // Custom functions
    JSC::JSValue add(JSC::ExecState*);
    JSC::JSValue remove(JSC::ExecState*);
    HTMLOptionsCollection& impl() const
    {
        return static_cast<HTMLOptionsCollection&>(Base::impl());
    }
protected:
    JSHTMLOptionsCollection(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<HTMLOptionsCollection>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
    void indexSetter(JSC::ExecState*, unsigned index, JSC::JSValue);
};

class JSHTMLOptionsCollectionOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, HTMLOptionsCollection*)
{
    DEFINE_STATIC_LOCAL(JSHTMLOptionsCollectionOwner, jsHTMLOptionsCollectionOwner, ());
    return &jsHTMLOptionsCollectionOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, HTMLOptionsCollection*)
{
    return &world;
}

HTMLOptionsCollection* toHTMLOptionsCollection(JSC::JSValue);

class JSHTMLOptionsCollectionPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSHTMLOptionsCollectionPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSHTMLOptionsCollectionPrototype* ptr = new (NotNull, JSC::allocateCell<JSHTMLOptionsCollectionPrototype>(vm.heap)) JSHTMLOptionsCollectionPrototype(vm, globalObject, structure);
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
    JSHTMLOptionsCollectionPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSHTMLOptionsCollectionConstructor : public DOMConstructorObject {
private:
    JSHTMLOptionsCollectionConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSHTMLOptionsCollectionConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSHTMLOptionsCollectionConstructor* ptr = new (NotNull, JSC::allocateCell<JSHTMLOptionsCollectionConstructor>(vm.heap)) JSHTMLOptionsCollectionConstructor(structure, globalObject);
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

JSC::EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionNamedItem(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionAdd(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsHTMLOptionsCollectionPrototypeFunctionRemove(JSC::ExecState*);
// Attributes

JSC::JSValue jsHTMLOptionsCollectionSelectedIndex(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLOptionsCollectionSelectedIndex(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLOptionsCollectionLength(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSHTMLOptionsCollectionLength(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsHTMLOptionsCollectionConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif
