// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/BooleanPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex booleanPrototypeTableIndex[4] = {
    { 0, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
};


static const struct HashTableValue booleanPrototypeTableValues[2] = {
 #if JSC_BUILTIN_EXISTS(BOOLEANPROTOTYPETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(booleanPrototypeToStringCodeGenerator), (intptr_t)0 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(booleanProtoFuncToString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(BOOLEANPROTOTYPEVALUEOF)
   { "valueOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(booleanPrototypeValueOfCodeGenerator), (intptr_t)0 },
 #else
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(booleanProtoFuncValueOf), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable booleanPrototypeTable =
    { 2, 3, false, booleanPrototypeTableValues, 0, booleanPrototypeTableIndex };
} // namespace
