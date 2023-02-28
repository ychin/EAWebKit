// Automatically generated from ../../runtime/ErrorPrototype.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex errorPrototypeTableIndex[2] = {
    { 0, -1 },
    { -1, -1 },
};


static const struct HashTableValue errorPrototypeTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(ERRORPROTOTYPETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(errorPrototypeToStringCodeGenerator), (intptr_t)0 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(errorProtoFuncToString), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable errorPrototypeTable =
    { 1, 1, false, errorPrototypeTableValues, 0, errorPrototypeTableIndex };
} // namespace
