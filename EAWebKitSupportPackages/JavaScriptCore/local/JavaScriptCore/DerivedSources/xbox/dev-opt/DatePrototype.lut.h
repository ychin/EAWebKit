// Automatically generated from /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/runtime/DatePrototype.cpp using /cygdrive/c/EAOS/EAOS/EAWebKit/DL/EAWebKit/dev/EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "JSCBuiltins.h"

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dateTableIndex[134] = {
    { 42, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 5, -1 },
    { 30, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 7, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 4, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 0, 130 },
    { 26, -1 },
    { 11, 133 },
    { -1, -1 },
    { 21, 131 },
    { -1, -1 },
    { 1, -1 },
    { -1, -1 },
    { 27, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 44, -1 },
    { -1, -1 },
    { 34, -1 },
    { -1, -1 },
    { 36, -1 },
    { 10, 128 },
    { 24, -1 },
    { -1, -1 },
    { 18, -1 },
    { -1, -1 },
    { 13, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 25, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 3, -1 },
    { -1, -1 },
    { 19, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 16, -1 },
    { -1, -1 },
    { -1, -1 },
    { 17, -1 },
    { 39, -1 },
    { -1, -1 },
    { -1, -1 },
    { 20, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 6, -1 },
    { -1, -1 },
    { -1, -1 },
    { 2, 129 },
    { -1, -1 },
    { 33, -1 },
    { 12, -1 },
    { -1, -1 },
    { 9, -1 },
    { 43, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 37, -1 },
    { 28, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 22, -1 },
    { 15, 132 },
    { 8, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 45, -1 },
    { -1, -1 },
    { -1, -1 },
    { 40, -1 },
    { -1, -1 },
    { 41, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 23, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 14, -1 },
    { 29, -1 },
    { 31, -1 },
    { 32, -1 },
    { 35, -1 },
    { 38, -1 },
};


static const struct HashTableValue dateTableValues[46] = {
 #if JSC_BUILTIN_EXISTS(DATETOSTRING)
   { "toString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToStringCodeGenerator), (intptr_t)0 },
 #else
   { "toString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOISOSTRING)
   { "toISOString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToISOStringCodeGenerator), (intptr_t)0 },
 #else
   { "toISOString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToISOString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOUTCSTRING)
   { "toUTCString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToUTCStringCodeGenerator), (intptr_t)0 },
 #else
   { "toUTCString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToUTCString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETODATESTRING)
   { "toDateString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToDateStringCodeGenerator), (intptr_t)0 },
 #else
   { "toDateString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToDateString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOTIMESTRING)
   { "toTimeString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToTimeStringCodeGenerator), (intptr_t)0 },
 #else
   { "toTimeString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToTimeString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOLOCALESTRING)
   { "toLocaleString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToLocaleStringCodeGenerator), (intptr_t)0 },
 #else
   { "toLocaleString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToLocaleString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOLOCALEDATESTRING)
   { "toLocaleDateString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToLocaleDateStringCodeGenerator), (intptr_t)0 },
 #else
   { "toLocaleDateString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToLocaleDateString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOLOCALETIMESTRING)
   { "toLocaleTimeString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToLocaleTimeStringCodeGenerator), (intptr_t)0 },
 #else
   { "toLocaleTimeString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToLocaleTimeString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEVALUEOF)
   { "valueOf", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateValueOfCodeGenerator), (intptr_t)0 },
 #else
   { "valueOf", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetTime), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETTIME)
   { "getTime", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetTimeCodeGenerator), (intptr_t)0 },
 #else
   { "getTime", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetTime), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETFULLYEAR)
   { "getFullYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetFullYearCodeGenerator), (intptr_t)0 },
 #else
   { "getFullYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetFullYear), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCFULLYEAR)
   { "getUTCFullYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCFullYearCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCFullYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCFullYear), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOGMTSTRING)
   { "toGMTString", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToGMTStringCodeGenerator), (intptr_t)0 },
 #else
   { "toGMTString", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToGMTString), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETMONTH)
   { "getMonth", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetMonthCodeGenerator), (intptr_t)0 },
 #else
   { "getMonth", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetMonth), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCMONTH)
   { "getUTCMonth", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCMonthCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCMonth", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCMonth), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETDATE)
   { "getDate", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetDateCodeGenerator), (intptr_t)0 },
 #else
   { "getDate", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetDate), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCDATE)
   { "getUTCDate", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCDateCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCDate", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCDate), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETDAY)
   { "getDay", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetDayCodeGenerator), (intptr_t)0 },
 #else
   { "getDay", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetDay), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCDAY)
   { "getUTCDay", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCDayCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCDay", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCDay), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETHOURS)
   { "getHours", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetHoursCodeGenerator), (intptr_t)0 },
 #else
   { "getHours", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetHours), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCHOURS)
   { "getUTCHours", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCHoursCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCHours", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCHours), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETMINUTES)
   { "getMinutes", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetMinutesCodeGenerator), (intptr_t)0 },
 #else
   { "getMinutes", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetMinutes), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCMINUTES)
   { "getUTCMinutes", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCMinutesCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCMinutes", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCMinutes), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETSECONDS)
   { "getSeconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetSecondsCodeGenerator), (intptr_t)0 },
 #else
   { "getSeconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetSeconds), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCSECONDS)
   { "getUTCSeconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCSecondsCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCSeconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCSeconds), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETMILLISECONDS)
   { "getMilliseconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetMillisecondsCodeGenerator), (intptr_t)0 },
 #else
   { "getMilliseconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetMilliSeconds), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETUTCMILLISECONDS)
   { "getUTCMilliseconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetUTCMillisecondsCodeGenerator), (intptr_t)0 },
 #else
   { "getUTCMilliseconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetUTCMilliseconds), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETTIMEZONEOFFSET)
   { "getTimezoneOffset", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetTimezoneOffsetCodeGenerator), (intptr_t)0 },
 #else
   { "getTimezoneOffset", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetTimezoneOffset), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETTIME)
   { "setTime", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetTimeCodeGenerator), (intptr_t)1 },
 #else
   { "setTime", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetTime), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETMILLISECONDS)
   { "setMilliseconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetMillisecondsCodeGenerator), (intptr_t)1 },
 #else
   { "setMilliseconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetMilliSeconds), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCMILLISECONDS)
   { "setUTCMilliseconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCMillisecondsCodeGenerator), (intptr_t)1 },
 #else
   { "setUTCMilliseconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCMilliseconds), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETSECONDS)
   { "setSeconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetSecondsCodeGenerator), (intptr_t)2 },
 #else
   { "setSeconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetSeconds), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCSECONDS)
   { "setUTCSeconds", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCSecondsCodeGenerator), (intptr_t)2 },
 #else
   { "setUTCSeconds", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCSeconds), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETMINUTES)
   { "setMinutes", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetMinutesCodeGenerator), (intptr_t)3 },
 #else
   { "setMinutes", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetMinutes), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCMINUTES)
   { "setUTCMinutes", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCMinutesCodeGenerator), (intptr_t)3 },
 #else
   { "setUTCMinutes", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCMinutes), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETHOURS)
   { "setHours", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetHoursCodeGenerator), (intptr_t)4 },
 #else
   { "setHours", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetHours), (intptr_t)(4) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCHOURS)
   { "setUTCHours", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCHoursCodeGenerator), (intptr_t)4 },
 #else
   { "setUTCHours", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCHours), (intptr_t)(4) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETDATE)
   { "setDate", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetDateCodeGenerator), (intptr_t)1 },
 #else
   { "setDate", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetDate), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCDATE)
   { "setUTCDate", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCDateCodeGenerator), (intptr_t)1 },
 #else
   { "setUTCDate", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCDate), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETMONTH)
   { "setMonth", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetMonthCodeGenerator), (intptr_t)2 },
 #else
   { "setMonth", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetMonth), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCMONTH)
   { "setUTCMonth", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCMonthCodeGenerator), (intptr_t)2 },
 #else
   { "setUTCMonth", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCMonth), (intptr_t)(2) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETFULLYEAR)
   { "setFullYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetFullYearCodeGenerator), (intptr_t)3 },
 #else
   { "setFullYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetFullYear), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETUTCFULLYEAR)
   { "setUTCFullYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetUTCFullYearCodeGenerator), (intptr_t)3 },
 #else
   { "setUTCFullYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetUTCFullYear), (intptr_t)(3) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATESETYEAR)
   { "setYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateSetYearCodeGenerator), (intptr_t)1 },
 #else
   { "setYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncSetYear), (intptr_t)(1) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATEGETYEAR)
   { "getYear", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateGetYearCodeGenerator), (intptr_t)0 },
 #else
   { "getYear", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncGetYear), (intptr_t)(0) },
 #endif
 #if JSC_BUILTIN_EXISTS(DATETOJSON)
   { "toJSON", ((DontEnum|Function) & ~Function) | Builtin, NoIntrinsic, (intptr_t)static_cast<BuiltinGenerator>(dateToJSONCodeGenerator), (intptr_t)1 },
 #else
   { "toJSON", DontEnum|Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(dateProtoFuncToJSON), (intptr_t)(1) },
 #endif
};

JS_EXPORT_PRIVATE extern const struct HashTable dateTable =
    { 46, 127, false, dateTableValues, 0, dateTableIndex };
} // namespace
