// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/NumberPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex numberPrototypeTableIndex[17] = {
    { -1, -1 },
    { -1, -1 },
    { 2, 16 },
    { 5, -1 },
    { 4, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 3, -1 },
};


static const struct HashTableValue numberPrototypeTableValues[6] = {
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeToStringCodeGenerator), (intptr_t)1 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToString), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPETOLOCALESTRING)
   { "toLocaleString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeToLocaleStringCodeGenerator), (intptr_t)0 },
 #else
   { "toLocaleString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToLocaleString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPEVALUEOF)
   { "valueOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeValueOfCodeGenerator), (intptr_t)0 },
 #else
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncValueOf), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPETOFIXED)
   { "toFixed", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeToFixedCodeGenerator), (intptr_t)1 },
 #else
   { "toFixed", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToFixed), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPETOEXPONENTIAL)
   { "toExponential", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeToExponentialCodeGenerator), (intptr_t)1 },
 #else
   { "toExponential", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToExponential), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(NUMBERPROTOTYPETOPRECISION)
   { "toPrecision", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(numberPrototypeToPrecisionCodeGenerator), (intptr_t)1 },
 #else
   { "toPrecision", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToPrecision), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable numberPrototypeTable =
    { 6, 15, false, numberPrototypeTableValues, 0, numberPrototypeTableIndex };
} // namespace
