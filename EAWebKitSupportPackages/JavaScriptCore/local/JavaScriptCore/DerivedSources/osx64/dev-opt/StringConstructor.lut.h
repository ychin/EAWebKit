// Automatically generated from ../../runtime/StringConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex stringConstructorTableIndex[8] = {
    { -1, -1 },
    { 2, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue stringConstructorTableValues[3] = {
 #if JSC_BUILTIN_EXISTS(STRINGCONSTRUCTORFROMCHARCODE)
   { "fromCharCode", ((DontEnum|Function) & ~Function) | Builtin, FromCharCodeIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(stringConstructorFromCharCodeCodeGenerator), (intptr_t)1 },
 #else
   { "fromCharCode", DontEnum|Function, FromCharCodeIntrinsic, (intptr_t)static_cast<NativeFunction>(stringFromCharCode), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(STRINGCONSTRUCTORFROMCODEPOINT)
   { "fromCodePoint", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(stringConstructorFromCodePointCodeGenerator), (intptr_t)1 },
 #else
   { "fromCodePoint", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(stringFromCodePoint), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(STRINGCONSTRUCTORRAW)
   { "raw", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(stringConstructorRawCodeGenerator), (intptr_t)1 },
 #else
   { "raw", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(stringRaw), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable stringConstructorTable =
    { 3, 7, false, stringConstructorTableValues, 0, stringConstructorTableIndex };
} // namespace
