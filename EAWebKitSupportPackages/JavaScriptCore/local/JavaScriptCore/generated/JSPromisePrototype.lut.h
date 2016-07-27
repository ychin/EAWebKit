// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/JSPromisePrototype.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue promisePrototypeTableValues[3] = {
   { "then", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromisePrototypeFuncThen), (intptr_t)0 },
   { "catch", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromisePrototypeFuncCatch), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable promisePrototypeTable =
    { 4, 3, promisePrototypeTableValues, 0 };
} // namespace
