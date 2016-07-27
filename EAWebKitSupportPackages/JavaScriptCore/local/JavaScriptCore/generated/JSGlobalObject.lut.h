// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/JSGlobalObject.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue globalObjectTableValues[11] = {
   { "parseInt", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncParseInt), (intptr_t)2 },
   { "parseFloat", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncParseFloat), (intptr_t)1 },
   { "isNaN", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncIsNaN), (intptr_t)1 },
   { "isFinite", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncIsFinite), (intptr_t)1 },
   { "escape", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEscape), (intptr_t)1 },
   { "unescape", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncUnescape), (intptr_t)1 },
   { "decodeURI", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncDecodeURI), (intptr_t)1 },
   { "decodeURIComponent", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncDecodeURIComponent), (intptr_t)1 },
   { "encodeURI", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEncodeURI), (intptr_t)1 },
   { "encodeURIComponent", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(globalFuncEncodeURIComponent), (intptr_t)1 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable globalObjectTable =
    { 34, 31, globalObjectTableValues, 0 };
} // namespace
