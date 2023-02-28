// Automatically generated from ../../runtime/InspectorInstrumentationObject.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex inspectorInstrumentationObjectTableIndex[9] = {
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, 8 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { 2, -1 },
};


static const struct HashTableValue inspectorInstrumentationObjectTableValues[3] = {
 #if JSC_BUILTIN_EXISTS(INSPECTORINSTRUMENTATIONOBJECTLOG)
   { "log", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(inspectorInstrumentationObjectLogCodeGenerator), (intptr_t)1 },
 #else
   { "log", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(inspectorInstrumentationObjectLog), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(INSPECTORINSTRUMENTATIONOBJECTPROMISEFULFILLED)
   { "promiseFulfilled", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(inspectorInstrumentationObjectPromiseFulfilledCodeGenerator), (intptr_t)3 },
 #else
   { "promiseFulfilled", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(inspectorInstrumentationObjectPromiseFulfilled), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(INSPECTORINSTRUMENTATIONOBJECTPROMISEREJECTED)
   { "promiseRejected", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(inspectorInstrumentationObjectPromiseRejectedCodeGenerator), (intptr_t)3 },
 #else
   { "promiseRejected", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(inspectorInstrumentationObjectPromiseRejected), (intptr_t)(3) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable inspectorInstrumentationObjectTable =
    { 3, 7, false, inspectorInstrumentationObjectTableValues, 0, inspectorInstrumentationObjectTableIndex };
} // namespace
