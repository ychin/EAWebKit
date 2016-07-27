// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/JSPromiseConstructor.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue promiseConstructorTableValues[4] = {
   { "fulfill", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncFulfill), (intptr_t)1 },
   { "resolve", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncResolve), (intptr_t)1 },
   { "reject", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(JSPromiseConstructorFuncReject), (intptr_t)1 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable promiseConstructorTable =
    { 8, 7, promiseConstructorTableValues, 0 };
} // namespace
