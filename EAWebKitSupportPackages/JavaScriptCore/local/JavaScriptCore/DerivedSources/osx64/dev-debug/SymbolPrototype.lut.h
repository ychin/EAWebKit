// Automatically generated from ../../runtime/SymbolPrototype.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex symbolPrototypeTableIndex[4] = {
    { 0, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
};


static const struct HashTableValue symbolPrototypeTableValues[2] = {
 #if JSC_BUILTIN_EXISTS(SYMBOLPROTOTYPETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(symbolPrototypeToStringCodeGenerator), (intptr_t)0 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(symbolProtoFuncToString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(SYMBOLPROTOTYPEVALUEOF)
   { "valueOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(symbolPrototypeValueOfCodeGenerator), (intptr_t)0 },
 #else
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(symbolProtoFuncValueOf), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable symbolPrototypeTable =
    { 2, 3, false, symbolPrototypeTableValues, 0, symbolPrototypeTableIndex };
} // namespace
