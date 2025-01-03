// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/JSPromiseConstructor.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex promiseConstructorTableIndex[9] = {
    { 2, -1 },
    { -1, -1 },
    { 0, 8 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 3, -1 },
};


static const struct HashTableValue promiseConstructorTableValues[4] = {
 #if JSC_BUILTIN_EXISTS(PROMISECONSTRUCTORRESOLVE)
   { "resolve", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promiseConstructorResolveCodeGenerator), (intptr_t)1 },
 #else
   { "resolve", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncResolve), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(PROMISECONSTRUCTORREJECT)
   { "reject", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promiseConstructorRejectCodeGenerator), (intptr_t)1 },
 #else
   { "reject", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncReject), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(PROMISECONSTRUCTORRACE)
   { "race", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promiseConstructorRaceCodeGenerator), (intptr_t)1 },
 #else
   { "race", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncRace), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(PROMISECONSTRUCTORALL)
   { "all", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(promiseConstructorAllCodeGenerator), (intptr_t)1 },
 #else
   { "all", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncAll), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable promiseConstructorTable =
    { 4, 7, false, promiseConstructorTableValues, 0, promiseConstructorTableIndex };
} // namespace
