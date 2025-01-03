// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/IntlNumberFormatPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex numberFormatPrototypeTableIndex[4] = {
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue numberFormatPrototypeTableValues[2] = {
   { "format", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlNumberFormatPrototypeGetterFormat), (intptr_t)static_cast<NativeFunction>(nullptr) },
 #if JSC_BUILTIN_EXISTS(NUMBERFORMATPROTOTYPERESOLVEDOPTIONS)
   { "resolvedOptions", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberFormatPrototypeResolvedOptionsCodeGenerator), (intptr_t)0 },
 #else
   { "resolvedOptions", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlNumberFormatPrototypeFuncResolvedOptions), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable numberFormatPrototypeTable =
    { 2, 3, true, numberFormatPrototypeTableValues, 0, numberFormatPrototypeTableIndex };
} // namespace
