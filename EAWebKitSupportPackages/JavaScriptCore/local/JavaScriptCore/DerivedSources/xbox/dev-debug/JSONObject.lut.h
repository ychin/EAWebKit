// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/JSONObject.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex jsonTableIndex[4] = {
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue jsonTableValues[2] = {
 #if JSC_BUILTIN_EXISTS(JSONPARSE)
   { "parse", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(jsonParseCodeGenerator), (intptr_t)2 },
 #else
   { "parse", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSONProtoFuncParse), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(JSONSTRINGIFY)
   { "stringify", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(jsonStringifyCodeGenerator), (intptr_t)3 },
 #else
   { "stringify", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSONProtoFuncStringify), (intptr_t)(3) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable jsonTable =
    { 2, 3, false, jsonTableValues, 0, jsonTableIndex };
} // namespace
