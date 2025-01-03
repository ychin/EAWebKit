// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/ArrayIteratorPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex arrayIteratorPrototypeTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue arrayIteratorPrototypeTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(ARRAYITERATORPROTOTYPENEXT)
   { "next", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(arrayIteratorPrototypeNextCodeGenerator), (intptr_t)0 },
 #else
   { "next", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(arrayIteratorProtoFuncNext), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable arrayIteratorPrototypeTable =
    { 1, 1, false, arrayIteratorPrototypeTableValues, 0, arrayIteratorPrototypeTableIndex };
} // namespace
