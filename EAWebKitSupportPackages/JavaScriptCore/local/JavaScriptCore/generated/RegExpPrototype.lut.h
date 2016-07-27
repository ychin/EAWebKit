// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/RegExpPrototype.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue regExpPrototypeTableValues[5] = {
   { "compile", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncCompile), (intptr_t)2 },
   { "exec", DontEnum|Function, RegExpExecIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncExec), (intptr_t)1 },
   { "test", DontEnum|Function, RegExpTestIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncTest), (intptr_t)1 },
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(regExpProtoFuncToString), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable regExpPrototypeTable =
    { 9, 7, regExpPrototypeTableValues, 0 };
} // namespace
