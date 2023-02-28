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

#ifndef JSRadioNodeList_h
#define JSRadioNodeList_h

#include "JSNodeList.h"
#include "RadioNodeList.h"

namespace WebCore {

class JSRadioNodeList : public JSNodeList {
public:
    typedef JSNodeList Base;
    static JSRadioNodeList* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, Ref<RadioNodeList>&& impl)
    {
        JSRadioNodeList* ptr = new (NotNull, JSC::allocateCell<JSRadioNodeList>(globalObject->vm().heap)) JSRadioNodeList(structure, globalObject, WTF::move(impl));
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static JSC::JSObject* getPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static bool getOwnPropertySlotByIndex(JSC::JSObject*, JSC::ExecState*, unsigned propertyName, JSC::PropertySlot&);

    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static void getOwnPropertyNames(JSC::JSObject*, JSC::ExecState*, JSC::PropertyNameArray&, JSC::EnumerationMode = JSC::EnumerationMode());
    RadioNodeList& impl() const
    {
        return static_cast<RadioNodeList&>(Base::impl());
    }
public:
    static const unsigned StructureFlags = JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | JSC::OverridesGetOwnPropertySlot | JSC::OverridesGetPropertyNames | Base::StructureFlags;
protected:
    JSRadioNodeList(JSC::Structure*, JSDOMGlobalObject*, Ref<RadioNodeList>&&);

    void finishCreation(JSC::VM& vm)
    {
        Base::finishCreation(vm);
        ASSERT(inherits(info()));
    }

};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, RadioNodeList*);
inline JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, RadioNodeList& impl) { return toJS(exec, globalObject, &impl); }


} // namespace WebCore

#endif
