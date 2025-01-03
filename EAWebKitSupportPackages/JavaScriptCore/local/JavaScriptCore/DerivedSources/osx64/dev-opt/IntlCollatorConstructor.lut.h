// Automatically generated from ../../runtime/IntlCollatorConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex collatorConstructorTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue collatorConstructorTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(COLLATORCONSTRUCTORSUPPORTEDLOCALESOF)
   { "supportedLocalesOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(collatorConstructorSupportedLocalesOfCodeGenerator), (intptr_t)1 },
 #else
   { "supportedLocalesOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlCollatorConstructorFuncSupportedLocalesOf), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable collatorConstructorTable =
    { 1, 1, false, collatorConstructorTableValues, 0, collatorConstructorTableIndex };
} // namespace
