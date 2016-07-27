// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/RegExpObject.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue regExpTableValues[5] = {
   { "global", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectGlobal), (intptr_t)0 },
   { "ignoreCase", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectIgnoreCase), (intptr_t)0 },
   { "multiline", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectMultiline), (intptr_t)0 },
   { "source", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpObjectSource), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable regExpTable =
    { 9, 7, regExpTableValues, 0 };
} // namespace
