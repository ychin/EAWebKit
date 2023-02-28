// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/ReflectObject.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex reflectObjectTableIndex[35] = {
    { 9, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 8, -1 },
    { 5, 34 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 3, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 2, 32 },
    { 0, 33 },
    { -1, -1 },
    { 4, -1 },
    { 6, -1 },
    { 7, -1 },
};


static const struct HashTableValue reflectObjectTableValues[10] = {
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTAPPLY)
   { "apply", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectApplyCodeGenerator), (intptr_t)3 },
 #else
   { "apply", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectApply), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTDEFINEPROPERTY)
   { "defineProperty", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectDefinePropertyCodeGenerator), (intptr_t)3 },
 #else
   { "defineProperty", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectDefineProperty), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTDELETEPROPERTY)
   { "deleteProperty", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectDeletePropertyCodeGenerator), (intptr_t)2 },
 #else
   { "deleteProperty", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectDeleteProperty), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTENUMERATE)
   { "enumerate", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectEnumerateCodeGenerator), (intptr_t)1 },
 #else
   { "enumerate", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectEnumerate), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTGETPROTOTYPEOF)
   { "getPrototypeOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectGetPrototypeOfCodeGenerator), (intptr_t)1 },
 #else
   { "getPrototypeOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectGetPrototypeOf), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTHAS)
   { "has", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectHasCodeGenerator), (intptr_t)2 },
 #else
   { "has", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectHas), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTISEXTENSIBLE)
   { "isExtensible", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectIsExtensibleCodeGenerator), (intptr_t)1 },
 #else
   { "isExtensible", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectIsExtensible), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTOWNKEYS)
   { "ownKeys", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectOwnKeysCodeGenerator), (intptr_t)1 },
 #else
   { "ownKeys", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectOwnKeys), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTPREVENTEXTENSIONS)
   { "preventExtensions", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectPreventExtensionsCodeGenerator), (intptr_t)1 },
 #else
   { "preventExtensions", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectPreventExtensions), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REFLECTOBJECTSETPROTOTYPEOF)
   { "setPrototypeOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(reflectObjectSetPrototypeOfCodeGenerator), (intptr_t)2 },
 #else
   { "setPrototypeOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(reflectObjectSetPrototypeOf), (intptr_t)(2) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable reflectObjectTable =
    { 10, 31, false, reflectObjectTableValues, 0, reflectObjectTableIndex };
} // namespace
