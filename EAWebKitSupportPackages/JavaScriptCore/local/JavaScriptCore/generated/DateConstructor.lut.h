// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/DateConstructor.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue dateConstructorTableValues[4] = {
   { "parse", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateParse), (intptr_t)1 },
   { "UTC", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateUTC), (intptr_t)7 },
   { "now", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateNow), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable dateConstructorTable =
    { 9, 7, dateConstructorTableValues, 0 };
} // namespace
