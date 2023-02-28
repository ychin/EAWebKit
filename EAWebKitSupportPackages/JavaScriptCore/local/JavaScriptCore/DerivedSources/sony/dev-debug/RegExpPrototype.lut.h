// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/RegExpPrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex regExpPrototypeTableIndex[32] = {
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { 0, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 7, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 6, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 8, -1 },
    { 2, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 5, -1 },
    { -1, -1 },
    { 3, -1 },
    { -1, -1 },
    { 4, -1 },
    { -1, -1 },
};


static const struct HashTableValue regExpPrototypeTableValues[9] = {
 #if JSC_BUILTIN_EXISTS(REGEXPPROTOTYPECOMPILE)
   { "compile", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(regExpPrototypeCompileCodeGenerator), (intptr_t)2 },
 #else
   { "compile", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncCompile), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(REGEXPPROTOTYPEEXEC)
   { "exec", ((DontEnum|Function) & ~Function) | Builtin, RegExpExecIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(regExpPrototypeExecCodeGenerator), (intptr_t)1 },
 #else
   { "exec", DontEnum|Function, RegExpExecIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncExec), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REGEXPPROTOTYPETEST)
   { "test", ((DontEnum|Function) & ~Function) | Builtin, RegExpTestIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(regExpPrototypeTestCodeGenerator), (intptr_t)1 },
 #else
   { "test", DontEnum|Function, RegExpTestIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncTest), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(REGEXPPROTOTYPETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(regExpPrototypeToStringCodeGenerator), (intptr_t)0 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncToString), (intptr_t)(0) },
 #endif
   { "global", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoGetterGlobal), (intptr_t)static_cast<NativeFunction>(nullptr) },
   { "ignoreCase", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoGetterIgnoreCase), (intptr_t)static_cast<NativeFunction>(nullptr) },
   { "multiline", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoGetterMultiline), (intptr_t)static_cast<NativeFunction>(nullptr) },
   { "source", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoGetterSource), (intptr_t)static_cast<NativeFunction>(nullptr) },
   { "flags", DontEnum|Accessor, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoGetterFlags), (intptr_t)static_cast<NativeFunction>(nullptr) },
};

JS_EXPORT_PRIVATE extern const struct HashTable regExpPrototypeTable =
    { 9, 31, true, regExpPrototypeTableValues, 0, regExpPrototypeTableIndex };
} // namespace
