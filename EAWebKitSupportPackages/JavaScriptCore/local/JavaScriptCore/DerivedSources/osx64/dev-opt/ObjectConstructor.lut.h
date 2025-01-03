// Automatically generated from ../../runtime/ObjectConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex objectConstructorTableIndex[34] = {
    { 1, -1 },
    { -1, -1 },
    { 4, -1 },
    { -1, -1 },
    { 8, -1 },
    { 14, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 7, -1 },
    { 11, -1 },
    { 15, -1 },
    { -1, -1 },
    { 10, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 12, -1 },
    { 5, -1 },
    { -1, -1 },
    { -1, -1 },
    { 9, -1 },
    { -1, -1 },
    { -1, -1 },
    { 2, 32 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, -1 },
    { 13, -1 },
    { -1, -1 },
    { 3, 33 },
    { 6, -1 },
};


static const struct HashTableValue objectConstructorTableValues[16] = {
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORGETPROTOTYPEOF)
   { "getPrototypeOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorGetPrototypeOfCodeGenerator), (intptr_t)1 },
 #else
   { "getPrototypeOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorGetPrototypeOf), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORSETPROTOTYPEOF)
   { "setPrototypeOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorSetPrototypeOfCodeGenerator), (intptr_t)2 },
 #else
   { "setPrototypeOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorSetPrototypeOf), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORGETOWNPROPERTYDESCRIPTOR)
   { "getOwnPropertyDescriptor", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorGetOwnPropertyDescriptorCodeGenerator), (intptr_t)2 },
 #else
   { "getOwnPropertyDescriptor", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorGetOwnPropertyDescriptor), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORGETOWNPROPERTYNAMES)
   { "getOwnPropertyNames", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorGetOwnPropertyNamesCodeGenerator), (intptr_t)1 },
 #else
   { "getOwnPropertyNames", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorGetOwnPropertyNames), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORKEYS)
   { "keys", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorKeysCodeGenerator), (intptr_t)1 },
 #else
   { "keys", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorKeys), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORDEFINEPROPERTY)
   { "defineProperty", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorDefinePropertyCodeGenerator), (intptr_t)3 },
 #else
   { "defineProperty", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorDefineProperty), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORDEFINEPROPERTIES)
   { "defineProperties", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorDefinePropertiesCodeGenerator), (intptr_t)2 },
 #else
   { "defineProperties", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorDefineProperties), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORCREATE)
   { "create", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorCreateCodeGenerator), (intptr_t)2 },
 #else
   { "create", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorCreate), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORSEAL)
   { "seal", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorSealCodeGenerator), (intptr_t)1 },
 #else
   { "seal", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorSeal), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORFREEZE)
   { "freeze", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorFreezeCodeGenerator), (intptr_t)1 },
 #else
   { "freeze", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorFreeze), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORPREVENTEXTENSIONS)
   { "preventExtensions", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorPreventExtensionsCodeGenerator), (intptr_t)1 },
 #else
   { "preventExtensions", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorPreventExtensions), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORISSEALED)
   { "isSealed", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorIsSealedCodeGenerator), (intptr_t)1 },
 #else
   { "isSealed", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorIsSealed), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORISFROZEN)
   { "isFrozen", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorIsFrozenCodeGenerator), (intptr_t)1 },
 #else
   { "isFrozen", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorIsFrozen), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORISEXTENSIBLE)
   { "isExtensible", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorIsExtensibleCodeGenerator), (intptr_t)1 },
 #else
   { "isExtensible", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorIsExtensible), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORIS)
   { "is", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorIsCodeGenerator), (intptr_t)2 },
 #else
   { "is", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorIs), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(OBJECTCONSTRUCTORASSIGN)
   { "assign", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(objectConstructorAssignCodeGenerator), (intptr_t)2 },
 #else
   { "assign", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(objectConstructorAssign), (intptr_t)(2) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable objectConstructorTable =
    { 16, 31, false, objectConstructorTableValues, 0, objectConstructorTableIndex };
} // namespace
