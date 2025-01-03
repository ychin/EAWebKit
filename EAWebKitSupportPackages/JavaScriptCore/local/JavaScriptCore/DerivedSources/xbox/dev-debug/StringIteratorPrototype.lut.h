// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/StringIteratorPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex stringIteratorPrototypeTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue stringIteratorPrototypeTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(STRINGITERATORPROTOTYPENEXT)
   { "next", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(stringIteratorPrototypeNextCodeGenerator), (intptr_t)0 },
 #else
   { "next", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(stringIteratorPrototypeFuncNext), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable stringIteratorPrototypeTable =
    { 1, 1, false, stringIteratorPrototypeTableValues, 0, stringIteratorPrototypeTableIndex };
} // namespace
