// Automatically generated from ../../runtime/JSGlobalObject.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex globalObjectTableIndex[34] = {
    { -1, -1 },
    { 8, -1 },
    { 6, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 4, 33 },
    { -1, -1 },
    { -1, -1 },
    { 2, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, 32 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 7, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 3, -1 },
    { 5, -1 },
};


static const struct HashTableValue globalObjectTableValues[9] = {
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTPARSEFLOAT)
   { "parseFloat", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectParseFloatCodeGenerator), (intptr_t)1 },
 #else
   { "parseFloat", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncParseFloat), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTISNAN)
   { "isNaN", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectIsNaNCodeGenerator), (intptr_t)1 },
 #else
   { "isNaN", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncIsNaN), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTISFINITE)
   { "isFinite", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectIsFiniteCodeGenerator), (intptr_t)1 },
 #else
   { "isFinite", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncIsFinite), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTESCAPE)
   { "escape", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectEscapeCodeGenerator), (intptr_t)1 },
 #else
   { "escape", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEscape), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTUNESCAPE)
   { "unescape", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectUnescapeCodeGenerator), (intptr_t)1 },
 #else
   { "unescape", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncUnescape), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTDECODEURI)
   { "decodeURI", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectDecodeURICodeGenerator), (intptr_t)1 },
 #else
   { "decodeURI", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncDecodeURI), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTDECODEURICOMPONENT)
   { "decodeURIComponent", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectDecodeURIComponentCodeGenerator), (intptr_t)1 },
 #else
   { "decodeURIComponent", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncDecodeURIComponent), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTENCODEURI)
   { "encodeURI", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectEncodeURICodeGenerator), (intptr_t)1 },
 #else
   { "encodeURI", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEncodeURI), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(GLOBALOBJECTENCODEURICOMPONENT)
   { "encodeURIComponent", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(globalObjectEncodeURIComponentCodeGenerator), (intptr_t)1 },
 #else
   { "encodeURIComponent", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEncodeURIComponent), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable globalObjectTable =
    { 9, 31, false, globalObjectTableValues, 0, globalObjectTableIndex };
} // namespace
