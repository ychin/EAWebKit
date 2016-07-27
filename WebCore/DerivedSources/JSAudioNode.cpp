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

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "JSAudioNode.h"

#include "AudioContext.h"
#include "AudioNode.h"
#include "Event.h"
#include "ExceptionCode.h"
#include "JSAudioContext.h"
#include "JSAudioNode.h"
#include "JSAudioParam.h"
#include "JSDOMBinding.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSAudioNodeTableValues[] =
{
    { "context", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeContext), (intptr_t)0 },
    { "numberOfInputs", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeNumberOfInputs), (intptr_t)0 },
    { "numberOfOutputs", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeNumberOfOutputs), (intptr_t)0 },
    { "channelCount", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeChannelCount), (intptr_t)setJSAudioNodeChannelCount },
    { "channelCountMode", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeChannelCountMode), (intptr_t)setJSAudioNodeChannelCountMode },
    { "channelInterpretation", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeChannelInterpretation), (intptr_t)setJSAudioNodeChannelInterpretation },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsAudioNodeConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSAudioNodeTable = { 16, 15, JSAudioNodeTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSAudioNodeConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSAudioNodeConstructorTable = { 1, 0, JSAudioNodeConstructorTableValues, 0 };
const ClassInfo JSAudioNodeConstructor::s_info = { "AudioNodeConstructor", &Base::s_info, &JSAudioNodeConstructorTable, 0, CREATE_METHOD_TABLE(JSAudioNodeConstructor) };

JSAudioNodeConstructor::JSAudioNodeConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSAudioNodeConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSAudioNodePrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSAudioNodeConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSAudioNodeConstructor, JSDOMWrapper>(exec, JSAudioNodeConstructorTable, jsCast<JSAudioNodeConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSAudioNodePrototypeTableValues[] =
{
    { "connect", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsAudioNodePrototypeFunctionConnect), (intptr_t)1 },
    { "disconnect", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsAudioNodePrototypeFunctionDisconnect), (intptr_t)0 },
    { "addEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsAudioNodePrototypeFunctionAddEventListener), (intptr_t)2 },
    { "removeEventListener", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsAudioNodePrototypeFunctionRemoveEventListener), (intptr_t)2 },
    { "dispatchEvent", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsAudioNodePrototypeFunctionDispatchEvent), (intptr_t)1 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSAudioNodePrototypeTable = { 17, 15, JSAudioNodePrototypeTableValues, 0 };
const ClassInfo JSAudioNodePrototype::s_info = { "AudioNodePrototype", &Base::s_info, &JSAudioNodePrototypeTable, 0, CREATE_METHOD_TABLE(JSAudioNodePrototype) };

JSObject* JSAudioNodePrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSAudioNode>(vm, globalObject);
}

bool JSAudioNodePrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSAudioNodePrototype* thisObject = jsCast<JSAudioNodePrototype*>(object);
    return getStaticFunctionSlot<JSObject>(exec, JSAudioNodePrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSAudioNode::s_info = { "AudioNode", &Base::s_info, &JSAudioNodeTable, 0 , CREATE_METHOD_TABLE(JSAudioNode) };

JSAudioNode::JSAudioNode(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<AudioNode> impl)
    : JSEventTarget(structure, globalObject, impl)
{
}

void JSAudioNode::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSAudioNode::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSAudioNodePrototype::create(vm, globalObject, JSAudioNodePrototype::createStructure(vm, globalObject, JSEventTargetPrototype::self(vm, globalObject)));
}

bool JSAudioNode::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSAudioNode* thisObject = jsCast<JSAudioNode*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSAudioNode, Base>(exec, JSAudioNodeTable, thisObject, propertyName, slot);
}

JSValue jsAudioNodeContext(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.context()));
    return result;
}


JSValue jsAudioNodeNumberOfInputs(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = jsNumber(impl.numberOfInputs());
    return result;
}


JSValue jsAudioNodeNumberOfOutputs(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = jsNumber(impl.numberOfOutputs());
    return result;
}


JSValue jsAudioNodeChannelCount(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = jsNumber(impl.channelCount());
    return result;
}


JSValue jsAudioNodeChannelCountMode(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.channelCountMode());
    return result;
}


JSValue jsAudioNodeChannelInterpretation(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    AudioNode& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.channelInterpretation());
    return result;
}


JSValue jsAudioNodeConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSAudioNode* domObject = jsCast<JSAudioNode*>(asObject(slotBase));
    return JSAudioNode::getConstructor(exec->vm(), domObject->globalObject());
}

void JSAudioNode::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSAudioNode* thisObject = jsCast<JSAudioNode*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSAudioNode, Base>(exec, propertyName, value, JSAudioNodeTable, thisObject, slot);
}

void setJSAudioNodeChannelCount(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(thisObject);
    AudioNode& impl = castedThis->impl();
    ExceptionCode ec = 0;
    unsigned nativeValue(toUInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setChannelCount(nativeValue, ec);
    setDOMException(exec, ec);
}


void setJSAudioNodeChannelCountMode(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(thisObject);
    AudioNode& impl = castedThis->impl();
    ExceptionCode ec = 0;
    const String& nativeValue(value.isEmpty() ? String() : value.toString(exec)->value(exec));
    if (exec->hadException())
        return;
    impl.setChannelCountMode(nativeValue, ec);
    setDOMException(exec, ec);
}


void setJSAudioNodeChannelInterpretation(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(thisObject);
    AudioNode& impl = castedThis->impl();
    ExceptionCode ec = 0;
    const String& nativeValue(value.isEmpty() ? String() : value.toString(exec)->value(exec));
    if (exec->hadException())
        return;
    impl.setChannelInterpretation(nativeValue, ec);
    setDOMException(exec, ec);
}


JSValue JSAudioNode::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSAudioNodeConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionConnect1(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    AudioNode* destination(toAudioNode(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    unsigned output(toUInt32(exec, exec->argument(1), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    unsigned input(toUInt32(exec, exec->argument(2), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.connect(destination, output, input, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

static EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionConnect2(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    AudioParam* destination(toAudioParam(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    unsigned output(toUInt32(exec, exec->argument(1), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.connect(destination, output, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionConnect(ExecState* exec)
{
    size_t argsCount = exec->argumentCount();
    JSValue arg0(exec->argument(0));
    if ((argsCount == 1 && (arg0.isNull() || (arg0.isObject() && asObject(arg0)->inherits(JSAudioNode::info())))) || (argsCount == 2 && (arg0.isNull() || (arg0.isObject() && asObject(arg0)->inherits(JSAudioNode::info())))) || (argsCount == 3 && (arg0.isNull() || (arg0.isObject() && asObject(arg0)->inherits(JSAudioNode::info())))))
        return jsAudioNodePrototypeFunctionConnect1(exec);
    if ((argsCount == 1 && (arg0.isNull() || (arg0.isObject() && asObject(arg0)->inherits(JSAudioParam::info())))) || (argsCount == 2 && (arg0.isNull() || (arg0.isObject() && asObject(arg0)->inherits(JSAudioParam::info())))))
        return jsAudioNodePrototypeFunctionConnect2(exec);
    if (argsCount < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    return throwVMTypeError(exec);
}

EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionDisconnect(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    ExceptionCode ec = 0;
    unsigned output(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.disconnect(output, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.addEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (!listener.isObject())
        return JSValue::encode(jsUndefined());
    impl.removeEventListener(exec->argument(0).toString(exec)->value(exec), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)).get(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsAudioNodePrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSAudioNode::info()))
        return throwVMTypeError(exec);
    JSAudioNode* castedThis = jsCast<JSAudioNode*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSAudioNode::info());
    AudioNode& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    Event* event(toEvent(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = jsBoolean(impl.dispatchEvent(event, ec));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}

void JSAudioNode::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    JSAudioNode* thisObject = jsCast<JSAudioNode*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    COMPILE_ASSERT(StructureFlags & OverridesVisitChildren, OverridesVisitChildrenWithoutSettingFlag);
    ASSERT(thisObject->structure()->typeInfo().overridesVisitChildren());
    Base::visitChildren(thisObject, visitor);
    thisObject->impl().visitJSEventListeners(visitor);
}

static inline bool isObservable(JSAudioNode* jsAudioNode)
{
    if (jsAudioNode->hasCustomProperties())
        return true;
    if (jsAudioNode->impl().hasEventListeners())
        return true;
    return false;
}

bool JSAudioNodeOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSAudioNode* jsAudioNode = jsCast<JSAudioNode*>(handle.get().asCell());
    if (jsAudioNode->impl().isFiringEventListeners())
        return true;
    if (!isObservable(jsAudioNode))
        return false;
    AudioNode* root = &jsAudioNode->impl();
    return visitor.containsOpaqueRoot(root);
}

void JSAudioNodeOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSAudioNode* jsAudioNode = jsCast<JSAudioNode*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsAudioNode->impl(), jsAudioNode);
    jsAudioNode->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7AudioNode@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore9AudioNodeE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, AudioNode* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSAudioNode>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7AudioNode@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore9AudioNodeE[2];
#if COMPILER(CLANG)
    // If this fails AudioNode does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(AudioNode), AudioNode_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // AudioNode has subclasses. If AudioNode has subclasses that get passed
    // to toJS() we currently require AudioNode you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<AudioNode>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSAudioNode>(exec, globalObject, impl);
}

AudioNode* toAudioNode(JSC::JSValue value)
{
    return value.inherits(JSAudioNode::info()) ? &jsCast<JSAudioNode*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(WEB_AUDIO)
