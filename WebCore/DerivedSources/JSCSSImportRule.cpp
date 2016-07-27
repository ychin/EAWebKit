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
#include "JSCSSImportRule.h"

#include "CSSImportRule.h"
#include "CSSStyleSheet.h"
#include "JSCSSStyleSheet.h"
#include "JSMediaList.h"
#include "MediaList.h"
#include "URL.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSCSSImportRuleTableValues[] =
{
    { "href", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCSSImportRuleHref), (intptr_t)0 },
    { "media", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCSSImportRuleMedia), (intptr_t)0 },
    { "styleSheet", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCSSImportRuleStyleSheet), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCSSImportRuleConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCSSImportRuleTable = { 8, 7, JSCSSImportRuleTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSCSSImportRuleConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCSSImportRuleConstructorTable = { 1, 0, JSCSSImportRuleConstructorTableValues, 0 };
const ClassInfo JSCSSImportRuleConstructor::s_info = { "CSSImportRuleConstructor", &Base::s_info, &JSCSSImportRuleConstructorTable, 0, CREATE_METHOD_TABLE(JSCSSImportRuleConstructor) };

JSCSSImportRuleConstructor::JSCSSImportRuleConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSCSSImportRuleConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSCSSImportRulePrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSCSSImportRuleConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCSSImportRuleConstructor, JSDOMWrapper>(exec, JSCSSImportRuleConstructorTable, jsCast<JSCSSImportRuleConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSCSSImportRulePrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSCSSImportRulePrototypeTable = { 1, 0, JSCSSImportRulePrototypeTableValues, 0 };
const ClassInfo JSCSSImportRulePrototype::s_info = { "CSSImportRulePrototype", &Base::s_info, &JSCSSImportRulePrototypeTable, 0, CREATE_METHOD_TABLE(JSCSSImportRulePrototype) };

JSObject* JSCSSImportRulePrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSCSSImportRule>(vm, globalObject);
}

const ClassInfo JSCSSImportRule::s_info = { "CSSImportRule", &Base::s_info, &JSCSSImportRuleTable, 0 , CREATE_METHOD_TABLE(JSCSSImportRule) };

JSCSSImportRule::JSCSSImportRule(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<CSSImportRule> impl)
    : JSCSSRule(structure, globalObject, impl)
{
}

void JSCSSImportRule::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSCSSImportRule::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSCSSImportRulePrototype::create(vm, globalObject, JSCSSImportRulePrototype::createStructure(vm, globalObject, JSCSSRulePrototype::self(vm, globalObject)));
}

bool JSCSSImportRule::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSCSSImportRule* thisObject = jsCast<JSCSSImportRule*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSCSSImportRule, Base>(exec, JSCSSImportRuleTable, thisObject, propertyName, slot);
}

JSValue jsCSSImportRuleHref(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSCSSImportRule* castedThis = jsCast<JSCSSImportRule*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    CSSImportRule& impl = castedThis->impl();
    JSValue result = jsStringOrNull(exec, impl.href());
    return result;
}


JSValue jsCSSImportRuleMedia(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSCSSImportRule* castedThis = jsCast<JSCSSImportRule*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    CSSImportRule& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.media()));
    return result;
}


JSValue jsCSSImportRuleStyleSheet(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSCSSImportRule* castedThis = jsCast<JSCSSImportRule*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    CSSImportRule& impl = castedThis->impl();
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl.styleSheet()));
    return result;
}


JSValue jsCSSImportRuleConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSCSSImportRule* domObject = jsCast<JSCSSImportRule*>(asObject(slotBase));
    return JSCSSImportRule::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSCSSImportRule::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSCSSImportRuleConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}


}
