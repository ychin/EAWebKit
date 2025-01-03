// Automatically generated from ../../runtime/IntlDateTimeFormatConstructor.cpp using ../../create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dateTimeFormatConstructorTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const struct HashTableValue dateTimeFormatConstructorTableValues[1] = {
 #if JSC_BUILTIN_EXISTS(DATETIMEFORMATCONSTRUCTORSUPPORTEDLOCALESOF)
   { "supportedLocalesOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateTimeFormatConstructorSupportedLocalesOfCodeGenerator), (intptr_t)1 },
 #else
   { "supportedLocalesOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(IntlDateTimeFormatConstructorFuncSupportedLocalesOf), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable dateTimeFormatConstructorTable =
    { 1, 1, false, dateTimeFormatConstructorTableValues, 0, dateTimeFormatConstructorTableIndex };
} // namespace
