// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/JSDataViewPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dataViewTableIndex[37] = {
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 6, -1 },
    { -1, -1 },
    { -1, -1 },
    { 13, -1 },
    { 2, -1 },
    { -1, -1 },
    { 10, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 5, 33 },
    { -1, -1 },
    { -1, -1 },
    { 9, -1 },
    { 12, -1 },
    { 3, 34 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, 35 },
    { -1, -1 },
    { 1, 32 },
    { -1, -1 },
    { 11, -1 },
    { -1, -1 },
    { 4, 36 },
    { 7, -1 },
    { 8, -1 },
    { 14, -1 },
    { 15, -1 },
};


static const struct HashTableValue dataViewTableValues[16] = {
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETINT8)
   { "getInt8", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetInt8CodeGenerator), (intptr_t)0 },
 #else
   { "getInt8", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetInt8), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETUINT8)
   { "getUint8", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetUint8CodeGenerator), (intptr_t)0 },
 #else
   { "getUint8", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetUint8), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETINT16)
   { "getInt16", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetInt16CodeGenerator), (intptr_t)0 },
 #else
   { "getInt16", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetInt16), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETUINT16)
   { "getUint16", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetUint16CodeGenerator), (intptr_t)0 },
 #else
   { "getUint16", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetUint16), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETINT32)
   { "getInt32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetInt32CodeGenerator), (intptr_t)0 },
 #else
   { "getInt32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetInt32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETUINT32)
   { "getUint32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetUint32CodeGenerator), (intptr_t)0 },
 #else
   { "getUint32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetUint32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETFLOAT32)
   { "getFloat32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetFloat32CodeGenerator), (intptr_t)0 },
 #else
   { "getFloat32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetFloat32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWGETFLOAT64)
   { "getFloat64", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewGetFloat64CodeGenerator), (intptr_t)0 },
 #else
   { "getFloat64", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncGetFloat64), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETINT8)
   { "setInt8", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetInt8CodeGenerator), (intptr_t)0 },
 #else
   { "setInt8", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetInt8), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETUINT8)
   { "setUint8", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetUint8CodeGenerator), (intptr_t)0 },
 #else
   { "setUint8", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetUint8), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETINT16)
   { "setInt16", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetInt16CodeGenerator), (intptr_t)0 },
 #else
   { "setInt16", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetInt16), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETUINT16)
   { "setUint16", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetUint16CodeGenerator), (intptr_t)0 },
 #else
   { "setUint16", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetUint16), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETINT32)
   { "setInt32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetInt32CodeGenerator), (intptr_t)0 },
 #else
   { "setInt32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetInt32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETUINT32)
   { "setUint32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetUint32CodeGenerator), (intptr_t)0 },
 #else
   { "setUint32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetUint32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETFLOAT32)
   { "setFloat32", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetFloat32CodeGenerator), (intptr_t)0 },
 #else
   { "setFloat32", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetFloat32), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATAVIEWSETFLOAT64)
   { "setFloat64", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dataViewSetFloat64CodeGenerator), (intptr_t)0 },
 #else
   { "setFloat64", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dataViewProtoFuncSetFloat64), (intptr_t)(0) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable dataViewTable =
    { 16, 31, false, dataViewTableValues, 0, dataViewTableIndex };
} // namespace
