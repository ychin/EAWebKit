// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/NumberConstructor.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue numberConstructorTableValues[6] = {
   { "NaN", DontEnum|DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(numberConstructorNaNValue), (intptr_t)0 },
   { "NEGATIVE_INFINITY", DontEnum|DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(numberConstructorNegInfinity), (intptr_t)0 },
   { "POSITIVE_INFINITY", DontEnum|DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(numberConstructorPosInfinity), (intptr_t)0 },
   { "MAX_VALUE", DontEnum|DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(numberConstructorMaxValue), (intptr_t)0 },
   { "MIN_VALUE", DontEnum|DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(numberConstructorMinValue), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable numberConstructorTable =
    { 16, 15, numberConstructorTableValues, 0 };
} // namespace
