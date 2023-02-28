// Automatically generated from ../../runtime/IntlCollatorPrototype.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex collatorPrototypeTableIndex[4] = {
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue collatorPrototypeTableValues[2] = {
   { "compare", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlCollatorPrototypeGetterCompare), (intptr_t)static_cast<NativeFunction>(nullptr) },
 #if JSC_BUILTIN_EXISTS(COLLATORPROTOTYPERESOLVEDOPTIONS)
   { "resolvedOptions", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(collatorPrototypeResolvedOptionsCodeGenerator), (intptr_t)0 },
 #else
   { "resolvedOptions", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlCollatorPrototypeFuncResolvedOptions), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable collatorPrototypeTable =
    { 2, 3, true, collatorPrototypeTableValues, 0, collatorPrototypeTableIndex };
} // namespace
