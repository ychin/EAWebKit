// Automatically generated from ../../runtime/DateConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dateConstructorTableIndex[9] = {
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, 8 },
    { -1, -1 },
    { -1, -1 },
    { 2, -1 },
};


static const struct HashTableValue dateConstructorTableValues[3] = {
 #if JSC_BUILTIN_EXISTS(DATECONSTRUCTORPARSE)
   { "parse", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateConstructorParseCodeGenerator), (intptr_t)1 },
 #else
   { "parse", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateParse), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATECONSTRUCTORUTC)
   { "UTC", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateConstructorUTCCodeGenerator), (intptr_t)7 },
 #else
   { "UTC", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateUTC), (intptr_t)(7) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATECONSTRUCTORNOW)
   { "now", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateConstructorNowCodeGenerator), (intptr_t)0 },
 #else
   { "now", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateNow), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable dateConstructorTable =
    { 3, 7, false, dateConstructorTableValues, 0, dateConstructorTableIndex };
} // namespace
