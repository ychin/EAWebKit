// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/NumberPrototype.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue numberPrototypeTableValues[7] = {
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToString), (intptr_t)1 },
   { "toLocaleString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToLocaleString), (intptr_t)0 },
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncValueOf), (intptr_t)0 },
   { "toFixed", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToFixed), (intptr_t)1 },
   { "toExponential", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToExponential), (intptr_t)1 },
   { "toPrecision", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(numberProtoFuncToPrecision), (intptr_t)1 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable numberPrototypeTable =
    { 17, 15, numberPrototypeTableValues, 0 };
} // namespace
