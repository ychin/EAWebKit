// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/JSPromisePrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex promisePrototypeTableIndex[4] = {
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue promisePrototypeTableValues[2] = {
 #if JSC_BUILTIN_EXISTS(PROMISEPROTOTYPETHEN)
   { "then", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promisePrototypeThenCodeGenerator), (intptr_t)2 },
 #else
   { "then", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromisePrototypeFuncThen), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(PROMISEPROTOTYPECATCH)
   { "catch", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promisePrototypeCatchCodeGenerator), (intptr_t)1 },
 #else
   { "catch", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromisePrototypeFuncCatch), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable promisePrototypeTable =
    { 2, 3, false, promisePrototypeTableValues, 0, promisePrototypeTableIndex };
} // namespace
