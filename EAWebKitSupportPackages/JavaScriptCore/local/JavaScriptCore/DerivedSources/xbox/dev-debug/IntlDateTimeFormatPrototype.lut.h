// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/IntlDateTimeFormatPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dateTimeFormatPrototypeTableIndex[4] = {
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue dateTimeFormatPrototypeTableValues[2] = {
   { "format", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlDateTimeFormatPrototypeGetterFormat), (intptr_t)static_cast<NativeFunction>(nullptr) },
 #if JSC_BUILTIN_EXISTS(DATETIMEFORMATPROTOTYPERESOLVEDOPTIONS)
   { "resolvedOptions", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateTimeFormatPrototypeResolvedOptionsCodeGenerator), (intptr_t)0 },
 #else
   { "resolvedOptions", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlDateTimeFormatPrototypeFuncResolvedOptions), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable dateTimeFormatPrototypeTable =
    { 2, 3, true, dateTimeFormatPrototypeTableValues, 0, dateTimeFormatPrototypeTableIndex };
} // namespace
