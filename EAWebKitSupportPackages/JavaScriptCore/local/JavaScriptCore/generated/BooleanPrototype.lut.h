// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/BooleanPrototype.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue booleanPrototypeTableValues[3] = {
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(booleanProtoFuncToString), (intptr_t)0 },
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(booleanProtoFuncValueOf), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable booleanPrototypeTable =
    { 4, 3, booleanPrototypeTableValues, 0 };
} // namespace
