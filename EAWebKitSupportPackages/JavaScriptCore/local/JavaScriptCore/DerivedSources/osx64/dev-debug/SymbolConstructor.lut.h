// Automatically generated from ../../runtime/SymbolConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex symbolConstructorTableIndex[4] = {
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
};


static const struct HashTableValue symbolConstructorTableValues[2] = {
 #if JSC_BUILTIN_EXISTS(SYMBOLCONSTRUCTORFOR)
   { "for", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(symbolConstructorForCodeGenerator), (intptr_t)1 },
 #else
   { "for", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(symbolConstructorFor), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(SYMBOLCONSTRUCTORKEYFOR)
   { "keyFor", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(symbolConstructorKeyForCodeGenerator), (intptr_t)1 },
 #else
   { "keyFor", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(symbolConstructorKeyFor), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable symbolConstructorTable =
    { 2, 3, false, symbolConstructorTableValues, 0, symbolConstructorTableIndex };
} // namespace
