// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/IntlNumberFormatConstructor.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex numberFormatConstructorTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue numberFormatConstructorTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(NUMBERFORMATCONSTRUCTORSUPPORTEDLOCALESOF)
   { "supportedLocalesOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberFormatConstructorSupportedLocalesOfCodeGenerator), (intptr_t)1 },
 #else
   { "supportedLocalesOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlNumberFormatConstructorFuncSupportedLocalesOf), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable numberFormatConstructorTable =
    { 1, 1, false, numberFormatConstructorTableValues, 0, numberFormatConstructorTableIndex };
} // namespace
