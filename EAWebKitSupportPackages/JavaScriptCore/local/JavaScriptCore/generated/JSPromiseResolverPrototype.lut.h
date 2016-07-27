// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/JSPromiseResolverPrototype.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue promiseResolverPrototypeTableValues[4] = {
   { "fulfill", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseResolverPrototypeFuncFulfill), (intptr_t)1 },
   { "resolve", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseResolverPrototypeFuncResolve), (intptr_t)1 },
   { "reject", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseResolverPrototypeFuncReject), (intptr_t)1 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable promiseResolverPrototypeTable =
    { 8, 7, promiseResolverPrototypeTableValues, 0 };
} // namespace
