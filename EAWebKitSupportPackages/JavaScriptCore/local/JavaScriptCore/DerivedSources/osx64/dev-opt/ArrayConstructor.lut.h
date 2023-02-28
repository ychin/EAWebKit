// Automatically generated from ../../runtime/ArrayConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex arrayConstructorTableIndex[8] = {
    { -1, -1 },
    { 0, -1 },
    { 2, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
};


static const struct HashTableValue arrayConstructorTableValues[3] = {
 #if JSC_BUILTIN_EXISTS(ARRAYCONSTRUCTORISARRAY)
   { "isArray", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(arrayConstructorIsArrayCodeGenerator), (intptr_t)1 },
 #else
   { "isArray", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(arrayConstructorIsArray), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(ARRAYCONSTRUCTOROF)
   { "of", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(arrayConstructorOfCodeGenerator), (intptr_t)0 },
 #else
   { "of", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(arrayConstructorOf), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(ARRAYCONSTRUCTORFROM)
   { "from", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(arrayConstructorFromCodeGenerator), (intptr_t)0 },
 #else
   { "from", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(arrayConstructorFrom), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable arrayConstructorTable =
    { 3, 7, false, arrayConstructorTableValues, 0, arrayConstructorTableIndex };
} // namespace
