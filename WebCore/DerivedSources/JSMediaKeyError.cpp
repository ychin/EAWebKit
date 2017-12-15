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

#if ENABLE(ENCRYPTED_MEDIA) || ENABLE(ENCRYPTED_MEDIA_V2)

#include "JSMediaKeyError.h"

#include "MediaKeyError.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSMediaKeyErrorTableValues[] =
{
    { "code", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorCode), (intptr_t)0 },
#if ENABLE(ENCRYPTED_MEDIA_V2)
    { "systemCode", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorSystemCode), (intptr_t)0 },
#endif
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaKeyErrorTable = { 9, 7, JSMediaKeyErrorTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSMediaKeyErrorConstructorTableValues[] =
{
    { "MEDIA_KEYERR_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_UNKNOWN), (intptr_t)0 },
    { "MEDIA_KEYERR_CLIENT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_CLIENT), (intptr_t)0 },
    { "MEDIA_KEYERR_SERVICE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_SERVICE), (intptr_t)0 },
    { "MEDIA_KEYERR_OUTPUT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_OUTPUT), (intptr_t)0 },
    { "MEDIA_KEYERR_HARDWARECHANGE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_HARDWARECHANGE), (intptr_t)0 },
    { "MEDIA_KEYERR_DOMAIN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_DOMAIN), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaKeyErrorConstructorTable = { 17, 15, JSMediaKeyErrorConstructorTableValues, 0 };

COMPILE_ASSERT(1 == MediaKeyError::MEDIA_KEYERR_UNKNOWN, MediaKeyErrorEnumMEDIA_KEYERR_UNKNOWNIsWrongUseDoNotCheckConstants);
COMPILE_ASSERT(2 == MediaKeyError::MEDIA_KEYERR_CLIENT, MediaKeyErrorEnumMEDIA_KEYERR_CLIENTIsWrongUseDoNotCheckConstants);
COMPILE_ASSERT(3 == MediaKeyError::MEDIA_KEYERR_SERVICE, MediaKeyErrorEnumMEDIA_KEYERR_SERVICEIsWrongUseDoNotCheckConstants);
COMPILE_ASSERT(4 == MediaKeyError::MEDIA_KEYERR_OUTPUT, MediaKeyErrorEnumMEDIA_KEYERR_OUTPUTIsWrongUseDoNotCheckConstants);
COMPILE_ASSERT(5 == MediaKeyError::MEDIA_KEYERR_HARDWARECHANGE, MediaKeyErrorEnumMEDIA_KEYERR_HARDWARECHANGEIsWrongUseDoNotCheckConstants);
COMPILE_ASSERT(6 == MediaKeyError::MEDIA_KEYERR_DOMAIN, MediaKeyErrorEnumMEDIA_KEYERR_DOMAINIsWrongUseDoNotCheckConstants);

const ClassInfo JSMediaKeyErrorConstructor::s_info = { "WebKitMediaKeyErrorConstructor", &Base::s_info, &JSMediaKeyErrorConstructorTable, 0, CREATE_METHOD_TABLE(JSMediaKeyErrorConstructor) };

JSMediaKeyErrorConstructor::JSMediaKeyErrorConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSMediaKeyErrorConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSMediaKeyErrorPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSMediaKeyErrorConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSMediaKeyErrorConstructor, JSDOMWrapper>(exec, JSMediaKeyErrorConstructorTable, jsCast<JSMediaKeyErrorConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSMediaKeyErrorPrototypeTableValues[] =
{
    { "MEDIA_KEYERR_UNKNOWN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_UNKNOWN), (intptr_t)0 },
    { "MEDIA_KEYERR_CLIENT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_CLIENT), (intptr_t)0 },
    { "MEDIA_KEYERR_SERVICE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_SERVICE), (intptr_t)0 },
    { "MEDIA_KEYERR_OUTPUT", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_OUTPUT), (intptr_t)0 },
    { "MEDIA_KEYERR_HARDWARECHANGE", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_HARDWARECHANGE), (intptr_t)0 },
    { "MEDIA_KEYERR_DOMAIN", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsMediaKeyErrorMEDIA_KEYERR_DOMAIN), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSMediaKeyErrorPrototypeTable = { 17, 15, JSMediaKeyErrorPrototypeTableValues, 0 };
const ClassInfo JSMediaKeyErrorPrototype::s_info = { "WebKitMediaKeyErrorPrototype", &Base::s_info, &JSMediaKeyErrorPrototypeTable, 0, CREATE_METHOD_TABLE(JSMediaKeyErrorPrototype) };

JSObject* JSMediaKeyErrorPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSMediaKeyError>(vm, globalObject);
}

bool JSMediaKeyErrorPrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSMediaKeyErrorPrototype* thisObject = jsCast<JSMediaKeyErrorPrototype*>(object);
    return getStaticValueSlot<JSMediaKeyErrorPrototype, JSObject>(exec, JSMediaKeyErrorPrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSMediaKeyError::s_info = { "WebKitMediaKeyError", &Base::s_info, &JSMediaKeyErrorTable, 0 , CREATE_METHOD_TABLE(JSMediaKeyError) };

JSMediaKeyError::JSMediaKeyError(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<MediaKeyError> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSMediaKeyError::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSMediaKeyError::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSMediaKeyErrorPrototype::create(vm, globalObject, JSMediaKeyErrorPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSMediaKeyError::destroy(JSC::JSCell* cell)
{
    JSMediaKeyError* thisObject = static_cast<JSMediaKeyError*>(cell);
    thisObject->JSMediaKeyError::~JSMediaKeyError();
}

JSMediaKeyError::~JSMediaKeyError()
{
    releaseImplIfNotNull();
}

bool JSMediaKeyError::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSMediaKeyError* thisObject = jsCast<JSMediaKeyError*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSMediaKeyError, Base>(exec, JSMediaKeyErrorTable, thisObject, propertyName, slot);
}

JSValue jsMediaKeyErrorCode(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSMediaKeyError* castedThis = jsCast<JSMediaKeyError*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    MediaKeyError& impl = castedThis->impl();
    JSValue result = jsNumber(impl.code());
    return result;
}


#if ENABLE(ENCRYPTED_MEDIA_V2)
JSValue jsMediaKeyErrorSystemCode(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSMediaKeyError* castedThis = jsCast<JSMediaKeyError*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    MediaKeyError& impl = castedThis->impl();
    JSValue result = jsNumber(impl.systemCode());
    return result;
}

#endif

JSValue jsMediaKeyErrorConstructor(ExecState* exec, JSValue slotBase, PropertyName)
{
    JSMediaKeyError* domObject = jsCast<JSMediaKeyError*>(asObject(slotBase));
    return JSMediaKeyError::getConstructor(exec->vm(), domObject->globalObject());
}

JSValue JSMediaKeyError::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSMediaKeyErrorConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

// Constant getters

JSValue jsMediaKeyErrorMEDIA_KEYERR_UNKNOWN(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(1));
}

JSValue jsMediaKeyErrorMEDIA_KEYERR_CLIENT(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(2));
}

JSValue jsMediaKeyErrorMEDIA_KEYERR_SERVICE(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(3));
}

JSValue jsMediaKeyErrorMEDIA_KEYERR_OUTPUT(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(4));
}

JSValue jsMediaKeyErrorMEDIA_KEYERR_HARDWARECHANGE(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(5));
}

JSValue jsMediaKeyErrorMEDIA_KEYERR_DOMAIN(ExecState* exec, JSValue, PropertyName)
{
    UNUSED_PARAM(exec);
    return jsNumber(static_cast<int>(6));
}

static inline bool isObservable(JSMediaKeyError* jsMediaKeyError)
{
    if (jsMediaKeyError->hasCustomProperties())
        return true;
    return false;
}

bool JSMediaKeyErrorOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSMediaKeyError* jsMediaKeyError = jsCast<JSMediaKeyError*>(handle.get().asCell());
    if (!isObservable(jsMediaKeyError))
        return false;
    UNUSED_PARAM(visitor);
    return false;
}

void JSMediaKeyErrorOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSMediaKeyError* jsMediaKeyError = jsCast<JSMediaKeyError*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsMediaKeyError->impl(), jsMediaKeyError);
    jsMediaKeyError->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, MediaKeyError* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSMediaKeyError>(exec, impl))
        return result;
#if COMPILER(CLANG)
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to MediaKeyError.
    COMPILE_ASSERT(!__is_polymorphic(MediaKeyError), MediaKeyError_is_polymorphic_but_idl_claims_not_to_be);
#endif
    ReportMemoryCost<MediaKeyError>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSMediaKeyError>(exec, globalObject, impl);
}

MediaKeyError* toMediaKeyError(JSC::JSValue value)
{
    return value.inherits(JSMediaKeyError::info()) ? &jsCast<JSMediaKeyError*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(ENCRYPTED_MEDIA) || ENABLE(ENCRYPTED_MEDIA_V2)