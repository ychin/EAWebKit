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

#ifndef JSOscillatorNode_h
#define JSOscillatorNode_h

#if ENABLE(WEB_AUDIO)

#include "JSAudioNode.h"
#include "JSDOMBinding.h"
#include "OscillatorNode.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSOscillatorNode : public JSAudioNode {
public:
    typedef JSAudioNode Base;
    static JSOscillatorNode* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<OscillatorNode> impl)
    {
        JSOscillatorNode* ptr = new (NotNull, JSC::allocateCell<JSOscillatorNode>(globalObject->vm().heap)) JSOscillatorNode(structure, globalObject, impl);
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

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);

    // Custom attributes
    void setType(JSC::ExecState*, JSC::JSValue);
    OscillatorNode& impl() const
    {
        return static_cast<OscillatorNode&>(Base::impl());
    }
protected:
    JSOscillatorNode(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<OscillatorNode>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, OscillatorNode*);

class JSOscillatorNodePrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSOscillatorNodePrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSOscillatorNodePrototype* ptr = new (NotNull, JSC::allocateCell<JSOscillatorNodePrototype>(vm.heap)) JSOscillatorNodePrototype(vm, globalObject, structure);
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
    JSOscillatorNodePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

class JSOscillatorNodeConstructor : public DOMConstructorObject {
private:
    JSOscillatorNodeConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSOscillatorNodeConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSOscillatorNodeConstructor* ptr = new (NotNull, JSC::allocateCell<JSOscillatorNodeConstructor>(vm.heap)) JSOscillatorNodeConstructor(structure, globalObject);
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

JSC::EncodedJSValue JSC_HOST_CALL jsOscillatorNodePrototypeFunctionStart(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsOscillatorNodePrototypeFunctionStop(JSC::ExecState*);
#if ENABLE(LEGACY_WEB_AUDIO)
JSC::EncodedJSValue JSC_HOST_CALL jsOscillatorNodePrototypeFunctionNoteOn(JSC::ExecState*);
#endif
#if ENABLE(LEGACY_WEB_AUDIO)
JSC::EncodedJSValue JSC_HOST_CALL jsOscillatorNodePrototypeFunctionNoteOff(JSC::ExecState*);
#endif
JSC::EncodedJSValue JSC_HOST_CALL jsOscillatorNodePrototypeFunctionSetPeriodicWave(JSC::ExecState*);
// Attributes

JSC::JSValue jsOscillatorNodeType(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSOscillatorNodeType(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsOscillatorNodePlaybackState(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeFrequency(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeDetune(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeOnended(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
void setJSOscillatorNodeOnended(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsOscillatorNodeConstructor(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
// Constants

JSC::JSValue jsOscillatorNodeSINE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeSQUARE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeSAWTOOTH(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeTRIANGLE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeCUSTOM(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeUNSCHEDULED_STATE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeSCHEDULED_STATE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodePLAYING_STATE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);
JSC::JSValue jsOscillatorNodeFINISHED_STATE(JSC::ExecState*, JSC::JSValue, JSC::PropertyName);

} // namespace WebCore

#endif // ENABLE(WEB_AUDIO)

#endif
