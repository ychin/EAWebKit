// Automatically generated from /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/runtime/RegExpConstructor.cpp using /home/abaldeva/WebKit-r157437/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue regExpConstructorTableValues[22] = {
   { "input", None, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorInput), (intptr_t)setRegExpConstructorInput },
   { "$_", DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorInput), (intptr_t)setRegExpConstructorInput },
   { "multiline", None, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorMultiline), (intptr_t)setRegExpConstructorMultiline },
   { "$*", DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorMultiline), (intptr_t)setRegExpConstructorMultiline },
   { "lastMatch", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLastMatch), (intptr_t)0 },
   { "$&", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLastMatch), (intptr_t)0 },
   { "lastParen", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLastParen), (intptr_t)0 },
   { "$+", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLastParen), (intptr_t)0 },
   { "leftContext", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLeftContext), (intptr_t)0 },
   { "$`", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorLeftContext), (intptr_t)0 },
   { "rightContext", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorRightContext), (intptr_t)0 },
   { "$'", DontDelete|ReadOnly|DontEnum, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorRightContext), (intptr_t)0 },
   { "$1", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar1), (intptr_t)0 },
   { "$2", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar2), (intptr_t)0 },
   { "$3", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar3), (intptr_t)0 },
   { "$4", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar4), (intptr_t)0 },
   { "$5", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar5), (intptr_t)0 },
   { "$6", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar6), (intptr_t)0 },
   { "$7", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar7), (intptr_t)0 },
   { "$8", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar8), (intptr_t)0 },
   { "$9", DontDelete|ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(regExpConstructorDollar9), (intptr_t)0 },
   { 0, 0, NoIntrinsic, 0, 0 }
};

extern const struct HashTable regExpConstructorTable =
    { 65, 63, regExpConstructorTableValues, 0 };
} // namespace
