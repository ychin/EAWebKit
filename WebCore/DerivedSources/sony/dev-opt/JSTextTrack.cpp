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

#if ENABLE(VIDEO_TRACK)

#include "JSTextTrack.h"

#include "Element.h"
#include "Event.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "JSNodeCustom.h"
#include "JSTextTrackCue.h"
#include "JSTextTrackCueList.h"
#include "TextTrack.h"
#include "TextTrackCueList.h"
#include "TextTrackMediaSource.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

#if ENABLE(MEDIA_SOURCE) && ENABLE(VIDEO_TRACK)
#include "JSSourceBuffer.h"
#include "SourceBuffer.h"
#endif

#if ENABLE(WEBVTT_REGIONS)
#include "JSVTTRegion.h"
#include "JSVTTRegionList.h"
#include "VTTRegionList.h"
#endif

using namespace JSC;

namespace WebCore {

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddCue(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveCue(JSC::ExecState*);
#if ENABLE(WEBVTT_REGIONS)
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddRegion(JSC::ExecState*);
#endif
#if ENABLE(WEBVTT_REGIONS)
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveRegion(JSC::ExecState*);
#endif
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddEventListener(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveEventListener(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionDispatchEvent(JSC::ExecState*);

// Attributes

JSC::EncodedJSValue jsTextTrackId(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTextTrackKind(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSTextTrackKind(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsTextTrackLabel(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTextTrackLanguage(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSTextTrackLanguage(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsTextTrackInBandMetadataTrackDispatchType(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTextTrackMode(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSTextTrackMode(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsTextTrackCues(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTextTrackActiveCues(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTextTrackOncuechange(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
void setJSTextTrackOncuechange(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
#if ENABLE(WEBVTT_REGIONS)
JSC::EncodedJSValue jsTextTrackRegions(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
#endif
#if ENABLE(MEDIA_SOURCE) && ENABLE(VIDEO_TRACK)
JSC::EncodedJSValue jsTextTrackSourceBuffer(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
#endif
JSC::EncodedJSValue jsTextTrackConstructor(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSTextTrackPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSTextTrackPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTextTrackPrototype* ptr = new (NotNull, JSC::allocateCell<JSTextTrackPrototype>(vm.heap)) JSTextTrackPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTextTrackPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

class JSTextTrackConstructor : public DOMConstructorObject {
private:
    JSTextTrackConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSTextTrackConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSTextTrackConstructor* ptr = new (NotNull, JSC::allocateCell<JSTextTrackConstructor>(vm.heap)) JSTextTrackConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
};

/* Hash table */

static const struct CompactHashIndex JSTextTrackTableIndex[4] = {
    { 0, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
};


static const HashTableValue JSTextTrackTableValues[] =
{
    { "kind", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackKind), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTextTrackKind) },
    { "language", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackLanguage), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTextTrackLanguage) },
};

static const HashTable JSTextTrackTable = { 2, 3, true, JSTextTrackTableValues, 0, JSTextTrackTableIndex };
const ClassInfo JSTextTrackConstructor::s_info = { "TextTrackConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSTextTrackConstructor) };

JSTextTrackConstructor::JSTextTrackConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTextTrackConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTextTrack::getPrototype(vm, globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("TextTrack"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

/* Hash table for prototype */

static const HashTableValue JSTextTrackPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "id", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackId), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "label", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackLabel), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "inBandMetadataTrackDispatchType", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackInBandMetadataTrackDispatchType), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "mode", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackMode), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTextTrackMode) },
    { "cues", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackCues), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "activeCues", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackActiveCues), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "oncuechange", DontDelete | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackOncuechange), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTextTrackOncuechange) },
#if ENABLE(WEBVTT_REGIONS)
    { "regions", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackRegions), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
#else
    { 0, 0, NoIntrinsic, 0, 0 },
#endif
#if ENABLE(MEDIA_SOURCE) && ENABLE(VIDEO_TRACK)
    { "sourceBuffer", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTextTrackSourceBuffer), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
#else
    { 0, 0, NoIntrinsic, 0, 0 },
#endif
    { "addCue", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionAddCue), (intptr_t) (1) },
    { "removeCue", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionRemoveCue), (intptr_t) (1) },
#if ENABLE(WEBVTT_REGIONS)
    { "addRegion", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionAddRegion), (intptr_t) (1) },
#else
    { 0, 0, NoIntrinsic, 0, 0 },
#endif
#if ENABLE(WEBVTT_REGIONS)
    { "removeRegion", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionRemoveRegion), (intptr_t) (1) },
#else
    { 0, 0, NoIntrinsic, 0, 0 },
#endif
    { "addEventListener", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionAddEventListener), (intptr_t) (2) },
    { "removeEventListener", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionRemoveEventListener), (intptr_t) (2) },
    { "dispatchEvent", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTextTrackPrototypeFunctionDispatchEvent), (intptr_t) (1) },
};

const ClassInfo JSTextTrackPrototype::s_info = { "TextTrackPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSTextTrackPrototype) };

void JSTextTrackPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTextTrackPrototypeTableValues, *this);
}

const ClassInfo JSTextTrack::s_info = { "TextTrack", &Base::s_info, &JSTextTrackTable, CREATE_METHOD_TABLE(JSTextTrack) };

JSTextTrack::JSTextTrack(Structure* structure, JSDOMGlobalObject* globalObject, Ref<TextTrack>&& impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(&impl.leakRef())
{
}

JSObject* JSTextTrack::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTextTrackPrototype::create(vm, globalObject, JSTextTrackPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSTextTrack::getPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTextTrack>(vm, globalObject);
}

void JSTextTrack::destroy(JSC::JSCell* cell)
{
    JSTextTrack* thisObject = static_cast<JSTextTrack*>(cell);
    thisObject->JSTextTrack::~JSTextTrack();
}

JSTextTrack::~JSTextTrack()
{
    releaseImpl();
}

bool JSTextTrack::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSTextTrack*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSTextTrack, Base>(exec, JSTextTrackTable, thisObject, propertyName, slot);
}

EncodedJSValue jsTextTrackId(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "id");
        return throwGetterTypeError(*exec, "TextTrack", "id");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.id());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackKind(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSTextTrack*>(slotBase);
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.kind());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackLabel(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "label");
        return throwGetterTypeError(*exec, "TextTrack", "label");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.label());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackLanguage(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSTextTrack*>(slotBase);
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.language());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackInBandMetadataTrackDispatchType(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "inBandMetadataTrackDispatchType");
        return throwGetterTypeError(*exec, "TextTrack", "inBandMetadataTrackDispatchType");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.inBandMetadataTrackDispatchType());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackMode(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "mode");
        return throwGetterTypeError(*exec, "TextTrack", "mode");
    }
    auto& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.mode());
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackCues(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "cues");
        return throwGetterTypeError(*exec, "TextTrack", "cues");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.cues()));
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackActiveCues(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "activeCues");
        return throwGetterTypeError(*exec, "TextTrack", "activeCues");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.activeCues()));
    return JSValue::encode(result);
}


EncodedJSValue jsTextTrackOncuechange(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "oncuechange");
        return throwGetterTypeError(*exec, "TextTrack", "oncuechange");
    }
    UNUSED_PARAM(exec);
    return JSValue::encode(eventHandlerAttribute(castedThis->impl(), eventNames().cuechangeEvent));
}


#if ENABLE(WEBVTT_REGIONS)
EncodedJSValue jsTextTrackRegions(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "regions");
        return throwGetterTypeError(*exec, "TextTrack", "regions");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.regions()));
    return JSValue::encode(result);
}

#endif

#if ENABLE(MEDIA_SOURCE) && ENABLE(VIDEO_TRACK)
EncodedJSValue jsTextTrackSourceBuffer(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TextTrack", "sourceBuffer");
        return throwGetterTypeError(*exec, "TextTrack", "sourceBuffer");
    }
    auto& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(TextTrackMediaSource::sourceBuffer(&impl)));
    return JSValue::encode(result);
}

#endif

EncodedJSValue jsTextTrackConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSTextTrackPrototype* domObject = jsDynamicCast<JSTextTrackPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSTextTrack::getConstructor(exec->vm(), domObject->globalObject()));
}

void setJSTextTrackKind(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSTextTrack*>(baseObject);
    UNUSED_PARAM(thisValue);
    UNUSED_PARAM(exec);
    castedThis->setKind(exec, value);
}


void setJSTextTrackLanguage(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    UNUSED_PARAM(thisValue);
    auto* castedThis = jsCast<JSTextTrack*>(baseObject);
    UNUSED_PARAM(thisValue);
    UNUSED_PARAM(exec);
    castedThis->setLanguage(exec, value);
}


void setJSTextTrackMode(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "TextTrack", "mode");
        else
            throwSetterTypeError(*exec, "TextTrack", "mode");
        return;
    }
    auto& impl = castedThis->impl();
    String nativeValue = value.toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return;
    if (nativeValue != "disabled" && nativeValue != "hidden" && nativeValue != "showing")
        return;
    impl.setMode(nativeValue);
}


void setJSTextTrackOncuechange(ExecState* exec, JSObject* baseObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(baseObject);
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTextTrackPrototype*>(JSValue::decode(thisValue)))
            reportDeprecatedSetterError(*exec, "TextTrack", "oncuechange");
        else
            throwSetterTypeError(*exec, "TextTrack", "oncuechange");
        return;
    }
    setEventHandlerAttribute(*exec, *castedThis, castedThis->impl(), eventNames().cuechangeEvent, value);
}


JSValue JSTextTrack::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTextTrackConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddCue(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "addCue");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    TextTrackCue* cue = JSTextTrackCue::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.addCue(cue, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveCue(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "removeCue");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    TextTrackCue* cue = JSTextTrackCue::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.removeCue(cue, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

#if ENABLE(WEBVTT_REGIONS)
EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddRegion(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "addRegion");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    VTTRegion* region = JSVTTRegion::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.addRegion(region);
    return JSValue::encode(jsUndefined());
}

#endif

#if ENABLE(WEBVTT_REGIONS)
EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveRegion(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "removeRegion");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    VTTRegion* region = JSVTTRegion::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    impl.removeRegion(region, ec);
    setDOMException(exec, ec);
    return JSValue::encode(jsUndefined());
}

#endif

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "addEventListener");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (UNLIKELY(!listener.isObject()))
        return JSValue::encode(jsUndefined());
    impl.addEventListener(exec->argument(0).toString(exec)->toAtomicString(exec), createJSEventListenerForAdd(*exec, *asObject(listener), *castedThis), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "removeEventListener");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    JSValue listener = exec->argument(1);
    if (UNLIKELY(!listener.isObject()))
        return JSValue::encode(jsUndefined());
    impl.removeEventListener(exec->argument(0).toString(exec)->toAtomicString(exec), createJSEventListenerForRemove(*exec, *asObject(listener), *castedThis).ptr(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsTextTrackPrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTextTrack* castedThis = jsDynamicCast<JSTextTrack*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TextTrack", "dispatchEvent");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTextTrack::info());
    auto& impl = castedThis->impl();
    if (UNLIKELY(exec->argumentCount() < 1))
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    Event* event = JSEvent::toWrapped(exec->argument(0));
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());
    JSValue result = jsBoolean(impl.dispatchEvent(event, ec));

    setDOMException(exec, ec);
    return JSValue::encode(result);
}

void JSTextTrack::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    auto* thisObject = jsCast<JSTextTrack*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);
    thisObject->impl().visitJSEventListeners(visitor);
    thisObject->visitAdditionalChildren(visitor);
}

bool JSTextTrackOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    auto* jsTextTrack = jsCast<JSTextTrack*>(handle.slot()->asCell());
    if (jsTextTrack->impl().isFiringEventListeners())
        return true;
    Element* element = WTF::getPtr(jsTextTrack->impl().element());
    if (!element)
        return false;
    void* root = WebCore::root(element);
    return visitor.containsOpaqueRoot(root);
}

void JSTextTrackOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTextTrack = jsCast<JSTextTrack*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTextTrack->impl(), jsTextTrack);
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, TextTrack* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTextTrack>(globalObject, impl))
        return result;
    return createNewWrapper<JSTextTrack>(globalObject, impl);
}

TextTrack* JSTextTrack::toWrapped(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTextTrack*>(value))
        return &wrapper->impl();
    return nullptr;
}

}

#endif // ENABLE(VIDEO_TRACK)