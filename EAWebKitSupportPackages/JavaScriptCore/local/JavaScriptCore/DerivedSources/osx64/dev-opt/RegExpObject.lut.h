// Automatically generated from JavaScriptCore/runtime/RegExpObject.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex regExpTableIndex[9] = {
    { -1, -1 },
    { -1, -1 },
    { 1, 8 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, -1 },
    { 2, -1 },
    { 3, -1 },
};


static const struct HashTableValue regExpTableValues[4] = {
   { "global", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectGlobal), (intptr_t)static_cast<PutPropertySlot::PutValueFunc>(0) },
   { "ignoreCase", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectIgnoreCase), (intptr_t)static_cast<PutPropertySlot::PutValueFunc>(0) },
   { "multiline", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectMultiline), (intptr_t)static_cast<PutPropertySlot::PutValueFunc>(0) },
   { "source", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectSource), (intptr_t)static_cast<PutPropertySlot::PutValueFunc>(0) },
};

JS_EXPORT_PRIVATE extern const struct HashTable regExpTable =
    { 4, 7, true, regExpTableValues, 0, regExpTableIndex };
} // namespace
