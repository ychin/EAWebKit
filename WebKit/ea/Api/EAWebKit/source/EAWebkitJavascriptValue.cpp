/*
Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#include <EAWebkit/EAWebkitJavascriptValue.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <internal/include/EAWebKitString.h>
#include <internal/include/EAWebKitNewDelete.h>
#include <internal/include/EAWebKitFPUPrecision.h>
#include <internal/include/EAWebKit_p.h>
#include <EAAssert/eaassert.h>

#include <eastl/fixed_vector.h>

#include <JSValue.h>
#include <JSFunction.h>
#include <JSDOMBinding.h>
#include <Protect.h>
#include <ObjectConstructor.h>

// The JavaScriptValue caches the underlying JSValue instance rather than creating an appropriate C++ type when instantiated. This is different from other platform 
// implementations. 

namespace EA { namespace WebKit 
{
typedef eastl::fixed_vector<JSC::JSValue, 8> JavascriptArgumentList;

//EAWebKitTODO: Setting FPU preicision might be a little overzealous in the API below.

namespace 
{
// These APIs directly expose the encapsulated JSObject within JSValue.  
// EAWebKitTODO: Probably all the API needs to be written in terms of JSValue methods.

JSC::JSArray *ValueToArray(JSC::JSValue* jsValue) 
{
    EAW_ASSERT_MSG(jsValue->isObject(), "Incoming value is not an object");

    JSC::JSObject* jsObject = jsValue->getObject();
    EAW_ASSERT_MSG(jsObject->inherits(JSC::JSArray::info()), "Incoming value is not an array");

    return static_cast<JSC::JSArray*>(jsObject);
}

JSC::JSObject *ValueToObject(JSC::JSValue* jsValue) 
{
    EAW_ASSERT_MSG(jsValue->isObject(),"Incoming value is not an object");
    return jsValue->getObject();
}

JSC::JSFunction *ValueToFunction(JSC::JSValue* jsValue)
{
    EAW_ASSERT_MSG(jsValue->isObject(),"Incoming value is not a function (In JavaScript, a function is an object as well"); 

    JSC::JSObject *jsObject = jsValue->getObject();
    EAW_ASSERT_MSG(jsObject->inherits(JSC::JSFunction::info()),"Incoming value is not a function");

    return static_cast<JSC::JSFunction*>(jsObject);
}
}


EA_COMPILETIME_ASSERT(JavascriptValue::kJSValueSize == sizeof(JSC::JSValue));

EA_COMPILETIME_ASSERT(8 == EA_ALIGN_OF(JSC::JSValue));


void JavascriptValue::SetNull(void) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::JSValue jsVal = JSC::jsNull();
    Assign(&jsVal);
}

void JavascriptValue::SetUndefined(void) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::JSValue jsVal = JSC::jsUndefined();
    Assign(&jsVal);
}

// Difference between as* and to* API in JSValue is that no type conversions are performed when calling as*. For example, calling asBoolean on number 12 will return false (and assert)
// whereas calling toBoolean on number 12 will return true.
void JavascriptValue::SetNumberValue(double v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 

    JSC::JSValue jsVal = JSC::jsNumber(v);
    Assign(&jsVal);
}

void JavascriptValue::SetNumberValue(int v)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::JSValue jsVal = JSC::jsNumber(v);
    Assign(&jsVal);
}
    
double JavascriptValue::GetNumberValue(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
    EAW_ASSERT_MSG(GetImpl()->isNumber(),"Trying to read number value from a JavaScriptValue which is not a number");
    return GetImpl()->asNumber();
}

bool JavascriptValue::IsInt32(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK();
	
    return  GetImpl()->isInt32();
}

int JavascriptValue::GetInt32(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK();
	
	EAW_ASSERT_MSG(GetImpl()->isInt32(),"Trying to read int value from a JavaScriptValue which is not an int");
	return  GetImpl()->asInt32();
}

bool JavascriptValue::IsDouble(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK();
	
	return  GetImpl()->isDouble();
}

double JavascriptValue::GetDouble(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK();
	
	EAW_ASSERT_MSG(GetImpl()->isDouble(),"Trying to read double value from a JavaScriptValue which is not a double");
	return  GetImpl()->asDouble();
}

void JavascriptValue::SetBooleanValue(bool v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::JSValue jsVal = JSC::jsBoolean(v);
    Assign(&jsVal);
}

bool JavascriptValue::GetBooleanValue(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	EAW_ASSERT_MSG(GetImpl()->isBoolean(),"Trying to read boolean value from a JavaScriptValue which is not a boolean");
	return  GetImpl()->asBoolean();
}

void JavascriptValue::SetStringValue(const char16_t *v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	EAW_ASSERT_MSG(v && v[0], "String is NULL.");
    
	WTF::String s(v);
    JSC::JSValue jsVal = JSC::jsString(mExecState, s);
    Assign(&jsVal);
}

const char16_t *JavascriptValue::GetStringValue(size_t *pLengthOut) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	EAW_ASSERT_MSG(GetImpl()->isString(),"Trying to read characters from a JavaScriptValue which is not a string");

	WTF::String s =  GetImpl()->getString(mExecState);
    if (pLengthOut) 
        *pLengthOut = s.length();
    return s.characters();
}

void JavascriptValue::SetArrayType(size_t arraySize) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
    JSC::JSValue jsVal(JSC::constructEmptyArray(mExecState, 0, arraySize));
    Assign(&jsVal);
}

void JavascriptValue::SetArrayValue(size_t index, const JavascriptValue &v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
	
    JSC::JSArray *jsArray = ValueToArray(GetImpl());
    jsArray->putDirectIndex(mExecState, index, *(v.GetImpl()));
}

void JavascriptValue::SetArrayValues(const JavascriptValue *array, size_t count, size_t startIndex)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
	
	EAW_ASSERT_MSG(array,"Passing a null array!");

    JSC::JSArray *jsArray = ValueToArray(GetImpl());
    for (size_t i = 0; i < count; ++i)
    {   
        jsArray->putDirectIndex(mExecState, startIndex + i, *(array[i].GetImpl()));
    }
}

void JavascriptValue::PushArrayValue(const JavascriptValue &v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
	
    JSC::JSArray *jsArray = ValueToArray(GetImpl());
    jsArray->push(mExecState, *(v.GetImpl()));
}

void JavascriptValue::PopArrayValue(JavascriptValue *pValueOut) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 

    JSC::JSArray *jsArray = ValueToArray(GetImpl());
    JSC::JSValue back = jsArray->pop(mExecState);

    if (pValueOut)
        *pValueOut = JavascriptValue(&back, mExecState);
}

void JavascriptValue::GetArrayValue(size_t index, JavascriptValue* pValueOut) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 

    JSC::JSArray *jsArray = ValueToArray(GetImpl());
	JSC::JSValue jsValue = jsArray->getIndex(mExecState,index);
	if (pValueOut)
		*pValueOut = JavascriptValue(&jsValue, mExecState);
}

size_t JavascriptValue::GetArrayLength(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
	
	JSC::JSArray *jsArray = ValueToArray(GetImpl());
    return jsArray->length();
}

void JavascriptValue::SetObjectType() 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
    JSC::JSValue jsVal(JSC::constructEmptyObject(mExecState));
    Assign(&jsVal);
}

void JavascriptValue::SetProperty(const char16_t *key, const JavascriptValue &v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
	
	JSC::JSObject *jsObject = ValueToObject(GetImpl());
    JSC::JSValue sourceJSValue = *(v.GetImpl());

    JSC::Identifier id(&(mExecState->vm()), key, EA::Internal::Strlen(key));
    JSC::PutPropertySlot slot;
    JSC::JSObject::put(jsObject,mExecState, id, sourceJSValue, slot);
}

void JavascriptValue::GetProperty(const char16_t *key, JavascriptValue* pValueOut) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::JSObject *jsObject = ValueToObject(GetImpl());
    JSC::Identifier id(&(mExecState->vm()), key, EA::Internal::Strlen(key));
    JSC::JSValue jsValue = jsObject->get(mExecState, id);
    if (pValueOut)
        *pValueOut = JavascriptValue(&jsValue, mExecState);
}

JavascriptValue::PropertyIterator* JavascriptValue::First(void) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 

    JSC::JSObject *jsObject = ValueToObject(GetImpl()); 

    if (!mIterator) 
        mIterator = new JavascriptValue::PropertyIterator(this);  

    JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mIterator->mNames);
    JSC::JSObject::getPropertyNames(jsObject,mExecState, *names,JSC::ExcludeDontEnumProperties);

    if (!names->size()) 
        return NULL;

    mIterator->mIterator = (void*)names->begin();
    return mIterator;
}

JavascriptValue::PropertyIterator* JavascriptValue::GetNext(void) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	if (!mIterator) 
        return NULL;

    JSC::PropertyNameArray::const_iterator iProperty = reinterpret_cast<JSC::PropertyNameArray::const_iterator>(mIterator->mIterator);
    if (!iProperty) 
        return NULL;

    ++iProperty;
        
    JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mIterator->mNames);
    if (iProperty == names->end()) 
    {
        mIterator->mIterator = NULL;
        return NULL;
    }

    mIterator->mIterator = (void*)iProperty;
    return mIterator;
}

bool JavascriptValue::Call(JavascriptValue *args, size_t argCount, JavascriptValue *pValueOut)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    
	JSC::MarkedArgumentBuffer jsArgs;

    if (args)
    {
        for (size_t i = 0; i < argCount; ++i)
        {
            jsArgs.append(*(args[i].GetImpl()));
        }
    }
        
    JSC::JSFunction *jsFunction = ValueToFunction(GetImpl());
    JSC::CallData callData;
    JSC::CallType callType = JSC::JSFunction::getCallData(jsFunction,callData);
    if (callType != JSC::CallTypeNone)
    {
        JSC::JSValue jsvFunction = *GetImpl();
        JSC::ExecState *exec = mExecState;
        JSC::JSValue result = JSC::call(exec, jsvFunction, callType, callData, jsvFunction, jsArgs);

        if (!exec->hadException())
        {
            if (pValueOut)
            {
                pValueOut->SetExec(mExecState);
                pValueOut->Assign(&result);
            }

            return true;
        }
        else
        {
            WebCore::reportException(exec, exec->exception());
        }
    }

    if (pValueOut)
    {
        pValueOut->SetUndefined();
    }

    return false;
}

JavascriptValue::JavascriptValue(JSC::JSValue *jsValue, JSC::ExecState *execState)
: mExecState(execState)
, mIterator(NULL) 
{
    new (mJSValue) JSC::JSValue();
    Assign(jsValue);
}

JavascriptValue::JavascriptValue(void)
: mExecState(NULL)
, mIterator(NULL) 
{
    new (mJSValue) JSC::JSValue();
}

JavascriptValue::JavascriptValue(const JavascriptValue &source) 
{
    mIterator = NULL;
    new (mJSValue) JSC::JSValue();
    Copy(source);
}

void JavascriptValue::Destruct(void) 
{
    if (mIterator) 
    {
        delete mIterator;
        mIterator = NULL;
    }
        
    JSC::gcUnprotect(*GetImpl());//The user does not need the underlying JSValue so unprotect it from garbage collection.
	GetImpl()->~JSValue();
}

void JavascriptValue::Copy(const JavascriptValue &source) 
{
    Assign(source.GetImpl());
    mExecState = source.mExecState;
}

void JavascriptValue::Assign(const JSC::JSValue* jsValue) 
{
    JSC::JSValue* thisValue = GetImpl();

    JSC::gcUnprotect(*thisValue);//Unprotect the previously held value so that it can be garbage collected
    *thisValue = *jsValue;
    JSC::gcProtect(*thisValue);//protect the new value from garbage collection
}

JavascriptValueType::JavascriptValueType JavascriptValue::Type(void) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK();  
    
	JSC::JSValue jsValue = *GetImpl();
    if (jsValue.isNumber()) 
    {
        return JavascriptValueType::Number;
    }
    else if (jsValue.isBoolean()) 
    {
        return JavascriptValueType::Boolean;
    }
    else if (jsValue.isString()) 
    {
        return JavascriptValueType::StringType;
    }
    else if (jsValue.isObject()) 
    {
        JSC::JSObject *jsObject = jsValue.getObject();
        if (jsObject->inherits(JSC::JSArray::info())) 
        {
            return JavascriptValueType::Array;
        }
        else if (jsObject->inherits(JSC::JSFunction::info()))
        {
            return JavascriptValueType::Function;
        }
        else 
        {
            return JavascriptValueType::Object;
        }
    }
    else if (jsValue.isUndefined()) 
    {
        return JavascriptValueType::Undefined;
    }
    else if (jsValue.isNull()) 
    {
        return JavascriptValueType::Null;
    }
    else 
    {
        return JavascriptValueType::Unknown;
    }
}

void JavascriptValue::SetExec(JSC::ExecState *exec) 
{
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 
    mExecState = exec;
}

EA_COMPILETIME_ASSERT(JavascriptValue::PropertyIterator::kPropertyNameArraySize >= sizeof(JSC::PropertyNameArray));
EA_COMPILETIME_ASSERT(EA_PLATFORM_PTR_SIZE == EA_ALIGN_OF(JSC::PropertyNameArray));

JavascriptValue::PropertyIterator::PropertyIterator(JavascriptValue *owner) 
{
    mObject = owner;
    mIterator = NULL;

    new (mNames) JSC::PropertyNameArray(owner->mExecState);
}

JavascriptValue::PropertyIterator::~PropertyIterator(void) 
{
    JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mNames);
    names->~PropertyNameArray();
    mIterator = NULL;
}

void JavascriptValue::PropertyIterator::GetValue(JavascriptValue* pValueOut) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK(); 
	
	JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mNames);
    JSC::PropertyNameArray::const_iterator iProperty = reinterpret_cast<JSC::PropertyNameArray::const_iterator>(mIterator);
    EAW_ASSERT(iProperty != names->end());
    (void) names;

    JSC::JSObject *jsObject = ValueToObject(mObject->GetImpl());
    JSC::JSValue jsValue = jsObject->get(mObject->mExecState, *iProperty);
    *pValueOut = JavascriptValue(&jsValue, mObject->mExecState);
}

const char16_t *JavascriptValue::PropertyIterator::GetKey(size_t *pLengthOut) const 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	EAWWBKIT_INIT_CHECK();
	
	JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mNames);
    JSC::PropertyNameArray::const_iterator iProperty = reinterpret_cast<JSC::PropertyNameArray::const_iterator>(mIterator);
    EAW_ASSERT(iProperty != names->end());
    (void) names;

    *pLengthOut = iProperty->length();
    return iProperty->characters();
}

void JavascriptValue::PropertyIterator::SetValue(const JavascriptValue &v) 
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
    EAWEBKIT_THREAD_CHECK();
    EAWWBKIT_INIT_CHECK(); 

    JSC::PropertyNameArray *names = reinterpret_cast<JSC::PropertyNameArray*>((void*)mNames);
    JSC::PropertyNameArray::const_iterator iProperty = reinterpret_cast<JSC::PropertyNameArray::const_iterator>(mIterator);
    EAW_ASSERT(iProperty != names->end());
    (void) names;
    JSC::JSObject *jsObject = ValueToObject(mObject->GetImpl());
    JSC::ExecState *exec = mObject->mExecState;

    JSC::PutPropertySlot slot;
    JSC::JSObject::put(jsObject,exec, *iProperty, *(v.GetImpl()), slot);
}
}}
