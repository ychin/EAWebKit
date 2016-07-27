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

#if ENABLE(SVG)

#include "JSSVGSVGElement.h"

#include "Element.h"
#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSElement.h"
#include "JSNodeList.h"
#include "JSSVGAngle.h"
#include "JSSVGAnimatedLength.h"
#include "JSSVGElement.h"
#include "JSSVGLength.h"
#include "JSSVGMatrix.h"
#include "JSSVGNumber.h"
#include "JSSVGPoint.h"
#include "JSSVGRect.h"
#include "JSSVGTransform.h"
#include "JSSVGViewSpec.h"
#include "NameNodeList.h"
#include "NodeList.h"
#include "SVGAngle.h"
#include "SVGLength.h"
#include "SVGMatrix.h"
#include "SVGPoint.h"
#include "SVGRect.h"
#include "SVGSVGElement.h"
#include "SVGStaticPropertyTearOff.h"
#include "SVGTransform.h"
#include "SVGViewSpec.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

#if ENABLE(SVG)
#include "JSSVGAnimatedBoolean.h"
#include "JSSVGAnimatedPreserveAspectRatio.h"
#include "JSSVGAnimatedRect.h"
#endif

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSSVGSVGElementTableValues[] =
{
    { "x", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementX), (intptr_t)0 },
    { "y", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementY), (intptr_t)0 },
    { "width", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementWidth), (intptr_t)0 },
    { "height", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementHeight), (intptr_t)0 },
    { "contentScriptType", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementContentScriptType), (intptr_t)setJSSVGSVGElementContentScriptType },
    { "contentStyleType", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementContentStyleType), (intptr_t)setJSSVGSVGElementContentStyleType },
    { "viewport", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementViewport), (intptr_t)0 },
    { "pixelUnitToMillimeterX", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementPixelUnitToMillimeterX), (intptr_t)0 },
    { "pixelUnitToMillimeterY", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementPixelUnitToMillimeterY), (intptr_t)0 },
    { "screenPixelToMillimeterX", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementScreenPixelToMillimeterX), (intptr_t)0 },
    { "screenPixelToMillimeterY", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementScreenPixelToMillimeterY), (intptr_t)0 },
    { "useCurrentView", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementUseCurrentView), (intptr_t)0 },
    { "currentView", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementCurrentView), (intptr_t)0 },
    { "currentScale", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementCurrentScale), (intptr_t)setJSSVGSVGElementCurrentScale },
    { "currentTranslate", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementCurrentTranslate), (intptr_t)0 },
#if ENABLE(SVG)
    { "externalResourcesRequired", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementExternalResourcesRequired), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "viewBox", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementViewBox), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "preserveAspectRatio", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementPreserveAspectRatio), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "zoomAndPan", DontDelete, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementZoomAndPan), (intptr_t)setJSSVGSVGElementZoomAndPan },
#endif
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGSVGElementTable = { 66, 63, JSSVGSVGElementTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSSVGSVGElementConstructorTableValues[] =
{
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_UNKNOWN), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_DISABLE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_DISABLE), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_MAGNIFY", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_MAGNIFY), (intptr_t)0 },
#endif
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGSVGElementConstructorTable = { 9, 7, JSSVGSVGElementConstructorTableValues, 0 };

#if ENABLE(SVG)
COMPILE_ASSERT(0 == SVGSVGElement::SVG_ZOOMANDPAN_UNKNOWN, SVGSVGElementEnumSVG_ZOOMANDPAN_UNKNOWNIsWrongUseDoNotCheckConstants);
#endif
#if ENABLE(SVG)
COMPILE_ASSERT(1 == SVGSVGElement::SVG_ZOOMANDPAN_DISABLE, SVGSVGElementEnumSVG_ZOOMANDPAN_DISABLEIsWrongUseDoNotCheckConstants);
#endif
#if ENABLE(SVG)
COMPILE_ASSERT(2 == SVGSVGElement::SVG_ZOOMANDPAN_MAGNIFY, SVGSVGElementEnumSVG_ZOOMANDPAN_MAGNIFYIsWrongUseDoNotCheckConstants);
#endif

const ClassInfo JSSVGSVGElementConstructor::s_info = { "SVGSVGElementConstructor", &Base::s_info, &JSSVGSVGElementConstructorTable, 0, CREATE_METHOD_TABLE(JSSVGSVGElementConstructor) };

JSSVGSVGElementConstructor::JSSVGSVGElementConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSSVGSVGElementConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSSVGSVGElementPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSSVGSVGElementConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGSVGElementConstructor, JSDOMWrapper>(exec, JSSVGSVGElementConstructorTable, jsCast<JSSVGSVGElementConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGSVGElementPrototypeTableValues[] =
{
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_UNKNOWN), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_DISABLE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_DISABLE), (intptr_t)0 },
#endif
#if ENABLE(SVG)
    { "SVG_ZOOMANDPAN_MAGNIFY", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGSVGElementSVG_ZOOMANDPAN_MAGNIFY), (intptr_t)0 },
#endif
    { "suspendRedraw", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionSuspendRedraw), (intptr_t)0 },
    { "unsuspendRedraw", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionUnsuspendRedraw), (intptr_t)0 },
    { "unsuspendRedrawAll", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionUnsuspendRedrawAll), (intptr_t)0 },
    { "forceRedraw", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionForceRedraw), (intptr_t)0 },
    { "pauseAnimations", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionPauseAnimations), (intptr_t)0 },
    { "unpauseAnimations", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionUnpauseAnimations), (intptr_t)0 },
    { "animationsPaused", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionAnimationsPaused), (intptr_t)0 },
    { "getCurrentTime", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionGetCurrentTime), (intptr_t)0 },
    { "setCurrentTime", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionSetCurrentTime), (intptr_t)0 },
    { "getIntersectionList", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionGetIntersectionList), (intptr_t)0 },
    { "getEnclosureList", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionGetEnclosureList), (intptr_t)0 },
    { "checkIntersection", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCheckIntersection), (intptr_t)0 },
    { "checkEnclosure", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCheckEnclosure), (intptr_t)0 },
    { "deselectAll", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionDeselectAll), (intptr_t)0 },
    { "createSVGNumber", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGNumber), (intptr_t)0 },
    { "createSVGLength", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGLength), (intptr_t)0 },
    { "createSVGAngle", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGAngle), (intptr_t)0 },
    { "createSVGPoint", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGPoint), (intptr_t)0 },
    { "createSVGMatrix", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGMatrix), (intptr_t)0 },
    { "createSVGRect", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGRect), (intptr_t)0 },
    { "createSVGTransform", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGTransform), (intptr_t)0 },
    { "createSVGTransformFromMatrix", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionCreateSVGTransformFromMatrix), (intptr_t)0 },
    { "getElementById", DontDelete | JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsSVGSVGElementPrototypeFunctionGetElementById), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSSVGSVGElementPrototypeTable = { 69, 63, JSSVGSVGElementPrototypeTableValues, 0 };
const ClassInfo JSSVGSVGElementPrototype::s_info = { "SVGSVGElementPrototype", &Base::s_info, &JSSVGSVGElementPrototypeTable, 0, CREATE_METHOD_TABLE(JSSVGSVGElementPrototype) };

JSObject* JSSVGSVGElementPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGSVGElement>(vm, globalObject);
}

bool JSSVGSVGElementPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGSVGElementPrototype* thisObject = jsCast<JSSVGSVGElementPrototype*>(object);
    return getStaticPropertySlot<JSSVGSVGElementPrototype, JSObject>(exec, JSSVGSVGElementPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSSVGSVGElement::s_info = { "SVGSVGElement", &Base::s_info, &JSSVGSVGElementTable, 0 , CREATE_METHOD_TABLE(JSSVGSVGElement) };

JSSVGSVGElement::JSSVGSVGElement(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGSVGElement> impl)
    : JSSVGGraphicsElement(structure, globalObject, impl)
{
}

void JSSVGSVGElement::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSSVGSVGElement::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSSVGSVGElementPrototype::create(vm, globalObject, JSSVGSVGElementPrototype::createStructure(vm, globalObject, JSSVGGraphicsElementPrototype::self(vm, globalObject)));
}

bool JSSVGSVGElement::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSSVGSVGElement* thisObject = jsCast<JSSVGSVGElement*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSSVGSVGElement, Base>(exec, JSSVGSVGElementTable, thisObject, propertyName, slot);
}

JSValue jsSVGSVGElementX(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.xAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGSVGElementY(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.yAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGSVGElementWidth(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.widthAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGSVGElementHeight(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedLength> obj = impl.heightAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}


JSValue jsSVGSVGElementContentScriptType(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.contentScriptType());
    return result;
}


JSValue jsSVGSVGElementContentStyleType(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.contentStyleType());
    return result;
}


JSValue jsSVGSVGElementViewport(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<FloatRect>::create(impl.viewport())));
    return result;
}


JSValue jsSVGSVGElementPixelUnitToMillimeterX(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.pixelUnitToMillimeterX());
    return result;
}


JSValue jsSVGSVGElementPixelUnitToMillimeterY(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.pixelUnitToMillimeterY());
    return result;
}


JSValue jsSVGSVGElementScreenPixelToMillimeterX(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.screenPixelToMillimeterX());
    return result;
}


JSValue jsSVGSVGElementScreenPixelToMillimeterY(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.screenPixelToMillimeterY());
    return result;
}


JSValue jsSVGSVGElementUseCurrentView(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsBoolean(impl.useCurrentView());
    return result;
}


JSValue jsSVGSVGElementCurrentView(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.currentView()));
    return result;
}


JSValue jsSVGSVGElementCurrentScale(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.currentScale());
    return result;
}


JSValue jsSVGSVGElementCurrentTranslate(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGStaticPropertyTearOff<SVGSVGElement, SVGPoint>::create(impl, impl.currentTranslate(), &SVGSVGElement::updateCurrentTranslate)));
    return result;
}


#if ENABLE(SVG)
JSValue jsSVGSVGElementExternalResourcesRequired(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedBoolean> obj = impl.externalResourcesRequiredAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGSVGElementViewBox(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedRect> obj = impl.viewBoxAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGSVGElementPreserveAspectRatio(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    RefPtr<SVGAnimatedPreserveAspectRatio> obj = impl.preserveAspectRatioAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get());
    return result;
}

#endif

#if ENABLE(SVG)
JSValue jsSVGSVGElementZoomAndPan(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGSVGElement& impl = castedThis->impl();
    JSValue result = jsNumber(impl.zoomAndPan());
    return result;
}

#endif

JSValue jsSVGSVGElementConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSSVGSVGElement* domObject = jsCast<JSSVGSVGElement*>(asObject(slotBase));
    return JSSVGSVGElement::getConstructor(exec->vm(), domObject->globalObject());
}

void JSSVGSVGElement::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    JSSVGSVGElement* thisObject = jsCast<JSSVGSVGElement*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    lookupPut<JSSVGSVGElement, Base>(exec, propertyName, value, JSSVGSVGElementTable, thisObject, slot);
}

void setJSSVGSVGElementContentScriptType(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(thisObject);
    SVGSVGElement& impl = castedThis->impl();
    const String& nativeValue(value.isEmpty() ? String() : value.toString(exec)->value(exec));
    if (exec->hadException())
        return;
    impl.setContentScriptType(nativeValue);
}


void setJSSVGSVGElementContentStyleType(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(thisObject);
    SVGSVGElement& impl = castedThis->impl();
    const String& nativeValue(value.isEmpty() ? String() : value.toString(exec)->value(exec));
    if (exec->hadException())
        return;
    impl.setContentStyleType(nativeValue);
}


void setJSSVGSVGElementCurrentScale(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(thisObject);
    SVGSVGElement& impl = castedThis->impl();
    float nativeValue(value.toFloat(exec));
    if (exec->hadException())
        return;
    impl.setCurrentScale(nativeValue);
}


#if ENABLE(SVG)
void setJSSVGSVGElementZoomAndPan(ExecState* exec, JSObject* thisObject, JSValue value)
{
    UNUSED_PARAM(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(thisObject);
    SVGSVGElement& impl = castedThis->impl();
    unsigned short nativeValue(toUInt32(exec, value, NormalConversion));
    if (exec->hadException())
        return;
    impl.setZoomAndPan(nativeValue);
}

#endif

JSValue JSSVGSVGElement::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGSVGElementConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionSuspendRedraw(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    unsigned maxWaitMilliseconds(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = jsNumber(impl.suspendRedraw(maxWaitMilliseconds));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionUnsuspendRedraw(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    unsigned suspendHandleId(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.unsuspendRedraw(suspendHandleId);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionUnsuspendRedrawAll(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    impl.unsuspendRedrawAll();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionForceRedraw(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    impl.forceRedraw();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionPauseAnimations(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    impl.pauseAnimations();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionUnpauseAnimations(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    impl.unpauseAnimations();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionAnimationsPaused(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = jsBoolean(impl.animationsPaused());
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionGetCurrentTime(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = jsNumber(impl.getCurrentTime());
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionSetCurrentTime(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    float seconds(exec->argument(0).toFloat(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl.setCurrentTime(seconds);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionGetIntersectionList(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    SVGPropertyTearOff<FloatRect>* rect(toSVGRect(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!rect) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }
    SVGElement* referenceElement(toSVGElement(exec->argument(1)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.getIntersectionList(rect->propertyReference(), referenceElement)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionGetEnclosureList(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    SVGPropertyTearOff<FloatRect>* rect(toSVGRect(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!rect) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }
    SVGElement* referenceElement(toSVGElement(exec->argument(1)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.getEnclosureList(rect->propertyReference(), referenceElement)));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCheckIntersection(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    SVGElement* element(toSVGElement(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    SVGPropertyTearOff<FloatRect>* rect(toSVGRect(exec->argument(1)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!rect) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }

    JSC::JSValue result = jsBoolean(impl.checkIntersection(element, rect->propertyReference()));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCheckEnclosure(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    SVGElement* element(toSVGElement(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    SVGPropertyTearOff<FloatRect>* rect(toSVGRect(exec->argument(1)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!rect) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }

    JSC::JSValue result = jsBoolean(impl.checkEnclosure(element, rect->propertyReference()));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionDeselectAll(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    impl.deselectAll();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGNumber(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<float>::create(impl.createSVGNumber())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGLength(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGLength>::create(impl.createSVGLength())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGAngle(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGAngle>::create(impl.createSVGAngle())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGPoint(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGPoint>::create(impl.createSVGPoint())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGMatrix(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGMatrix>::create(impl.createSVGMatrix())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGRect(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<FloatRect>::create(impl.createSVGRect())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGTransform(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGTransform>::create(impl.createSVGTransform())));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionCreateSVGTransformFromMatrix(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    SVGPropertyTearOff<SVGMatrix>* matrix(toSVGMatrix(exec->argument(0)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (!matrix) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(SVGPropertyTearOff<SVGTransform>::create(impl.createSVGTransformFromMatrix(matrix->propertyReference()))));
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL jsSVGSVGElementPrototypeFunctionGetElementById(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(JSSVGSVGElement::info()))
        return throwVMTypeError(exec);
    JSSVGSVGElement* castedThis = jsCast<JSSVGSVGElement*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSSVGSVGElement::info());
    SVGSVGElement& impl = castedThis->impl();
    const String& elementId(exec->argument(0).isEmpty() ? String() : exec->argument(0).toString(exec)->value(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.getElementById(elementId)));
    return JSValue::encode(result);
}

// Constant getters

#if ENABLE(SVG)
JSValue jsSVGSVGElementSVG_ZOOMANDPAN_UNKNOWN(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(0));
}

#endif
#if ENABLE(SVG)
JSValue jsSVGSVGElementSVG_ZOOMANDPAN_DISABLE(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(1));
}

#endif
#if ENABLE(SVG)
JSValue jsSVGSVGElementSVG_ZOOMANDPAN_MAGNIFY(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(2));
}

#endif

}

#endif // ENABLE(SVG)
