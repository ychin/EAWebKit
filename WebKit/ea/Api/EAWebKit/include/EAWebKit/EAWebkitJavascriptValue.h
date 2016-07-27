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

///////////////////////////////////////////////////////////////////////////////
// EAWebkitJavaScriptValue.h
// By David Siems
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITJAVASCRIPTVALUE_H
#define EAWEBKIT_EAWEBKITJAVASCRIPTVALUE_H

#include <EAWebkit/EAWebkitSTLWrapper.h>

namespace JSC
{
class ExecState;
class JSValue;
}

namespace EA { namespace WebKit {
    class JavascriptValue;
    class View;
/*
The documentation below is perhaps easier to understand in the presence of the sample HTML below and a very simple bound object implementation.
Sample HTML
<html>
<head>
<script type="text/javascript">
function friendsCountAvailable() {
document.getElementById("friendCount").innerHTML = EAWebKitTest.FriendsCount;
}
function clickCB() {
EAWebKitTest.QueryFriends("Adam");
}
</script>
</head>
<body>
<p><b>Test Page Info:</b> A quick set up to test various JavaScript binding mechanism. This page is not intended for QA testers. </p>
<button type="button" onclick="clickCB()">Find my friends!</button>
<br />
<p> Friend count: </p><div id="friendCount"></div>
</body>
</html>

class EAWebKitTestJavascriptBoundObject : public EA::WebKit::IJSBoundObject
{
public:
EAWebKitTestJavascriptBoundObject()
{
mFriendsCount = 101; // Some non-zero value
}
virtual bool hasMethod(const char *name)
{
if (strcmp(name, "QueryFriends") == 0) 
return true;

return false;
}

virtual bool hasProperty(const char *name)
{
if (strcmp(name, "FriendsCount") == 0) 
return true;

return false;
}

virtual void setProperty(const char *name, const EA::WebKit::JavascriptValue &value)
{
if (strcmp(name, "FriendsCount") == 0) 
{
EA_ASSERT_MSG(value.Type() == EA::WebKit::JavascriptValueType::Number, "Type mismatch");
mFriendsCount = value.GetNumberValue();

}

};

virtual void getProperty(const char *name, EA::WebKit::JavascriptValue *resultOut)
{
if (strcmp(name, "FriendsCount") == 0) 
{
resultOut->SetNumberValue(mFriendsCount);
}

}
virtual bool invokeMethod(const char *name, EA::WebKit::JavascriptValue *args, unsigned argCount, EA::WebKit::JavascriptValue *resultOut)
{ 
using namespace EA::WebKit;

if (strcmp(name, "QueryFriends") == 0)
{
mInvokedValid = true;

mInvokedValid = mInvokedValid && argCount == 1;
mInvokedValid = mInvokedValid && args[0].Type() == JavascriptValueType::StringType;

EA_ASSERT_MSG(mInvokedValid,"Something went wrong when invoking the method");

eastl::string16 script(L"friendsCountAvailable();");
mEAWebKitView->EvaluateJavaScript(script.data(), NULL);

return true;
}
return false;
}

bool mInvokedValid;
int32_t mFriendsCount;
};

*/
// Class for binding C++ objects to Javascript.
// The object bounds to main frame using the EAWebKitView::BindJavaScriptObject API call.
// If you want to use this object in an iframe, use parent.<bound_object_name> pattern.
// If a bound property/method does not exist in the object, the js value falls back to 'undefined'.
class IJSBoundObject 
{
public:
    virtual ~IJSBoundObject(void) {}

    // If your C++ object wants to handle the method called 'name', return true otherwise false. This method is called when the
	// JavaScript tries to access the method named 'name' on your bound object for the first time. The results are cached afterwards. 
	virtual bool hasMethod(const char *name) { return false; }
    // Call a method on the object. For example, calling EAWebKitTest.QueryFriends("Adam"); in JavaScript will trigger
	// invokeMethod call with name being 'QueryFriends' and args being the arguments to the function in the JavaScript.
	// Return true to indicate success. Return the results in resultOut if your JavaScript is expecting a result.
	// Returning false will cause an error to be thrown. (I.E. method not found)
    virtual bool invokeMethod(const char *name, JavascriptValue *args, unsigned argCount, EA::WebKit::JavascriptValue *resultOut) { return false; }

	// Return true if the bound object behaves like a function. I.E. var returnValue = EAPlayAudio('welcome');
	virtual bool isFunction(void) { return false; }

	// Invoke the object like it is a function.
	// Returning false will trigger a generic error.        
	virtual bool invoke(const JavascriptValue *args, const unsigned argCount, JavascriptValue *resultOut) { return false; }
	
	/*****************************************************************
	/
	// The property means the members that are not methods. It'd have been better to call it field instead of property.
	/
	******************************************************************/
	
	// In JS, if you want a statement like EAWebKitTest.FriendsCount = 11; to succeed, return true
	// when your bound object is queried for 'FriendsCount' property. If you return false, the assignment
	// or even a simple access like console.log(EAWebKitTest.FriendsCount) won't succeed.
    virtual bool hasProperty(const char *name) { return false; }
        
	// In JS, when a property is accessed, for example, console.log(EAWebKitTest.FriendsCount), getProperty
	// is called with the 'name' of the property. resultOut is expected to be filled appropriately by C++ code.
	virtual void getProperty(const char *name, JavascriptValue *resultOut) {}

	// In JS, when a property is set, for example, EAWebKitTest.FriendsCount = 11; setProperty is called 
	// with the 'name' of the property and the JavaScriptValue. The C++ code can read the value out of the
	// passed in parameter to appropriately set the game side state.
    virtual void setProperty(const char *name, const JavascriptValue &value) {}

    // Default tostring value.
    virtual const char8_t *stringValue(void) { return ""; }

	// Object property iteration. This will be called for a for-in loop on JS side for this object.
	class PropertyIterator {
	public:
		PropertyIterator(){}
		virtual ~PropertyIterator(void){}

		virtual const char8_t* GetKey() const { return NULL;};
		//virtual void GetValue(JavascriptValue *pValueOut) const {}; - Not necessary yet but may be added in future if required.
	private:
		PropertyIterator(const PropertyIterator& rhs);              // Simply declared, No copy supported
		PropertyIterator& operator =(const PropertyIterator& rhs);  // Simply declared, No assignment supported

	};
		
	virtual PropertyIterator* First(void){ return NULL;}
	virtual PropertyIterator* GetNext(void){return NULL;}
};

namespace JavascriptValueType {
    enum JavascriptValueType 
    {
        Null,
        Undefined,
        Number,
        Boolean,
        StringType,
        Array,
        Object,
        Function,
        Unknown
    };
}

// Wraps JSValue in JavaScript land to the C++ land.
// Note that JavascriptValues are usually passed by value, this means
// that if you want to change a value inside an array or object you
// need to explicitly call Set, instead of just changing the value
// returned from Get. Objects and Arrays are always by reference.
//
// These values are protected from garbage collection, so it's safe
// to hold on to them across frames.
class JavascriptValue 
{
public:
    
    virtual JavascriptValueType::JavascriptValueType Type(void) const;

    virtual void SetNull(void);
    virtual void SetUndefined(void);

    virtual void SetNumberValue(double v);
    virtual void SetNumberValue(int v);
    virtual double GetNumberValue(void) const;  // Converts to double if the JSvalue is a number (int or double)

    virtual bool IsInt32(void) const;
    virtual int GetInt32(void) const;           // Asserts if not an int.
        
    virtual bool IsDouble(void) const;
    virtual double GetDouble(void) const;       // Asserts if not a double.

    virtual void SetBooleanValue(bool v);
    virtual bool GetBooleanValue(void) const;

    virtual void SetStringValue(const char16_t *v);
    virtual const char16_t *GetStringValue(size_t *pLengthOut) const;

    // Arrays
    virtual void SetArrayType(size_t arraySize = 0);
    virtual void SetArrayValue(size_t index, const JavascriptValue &v);
    virtual void SetArrayValues(const JavascriptValue *array, size_t count, size_t startIndex = 0);
    virtual void PushArrayValue(const JavascriptValue &v);
    virtual void PopArrayValue(JavascriptValue *pValueOut);
    virtual void GetArrayValue(size_t index, JavascriptValue *pValueOut) const;
    virtual size_t GetArrayLength(void) const;

    // Objects
    virtual void SetObjectType(void);
    virtual void SetProperty(const char16_t *key, const JavascriptValue &v);
    virtual void GetProperty(const char16_t *key, JavascriptValue *pValueOut) const;

	// Functions - Call this value as function
	virtual bool Call(JavascriptValue *args, size_t argCount, JavascriptValue *pValueOut = NULL);

	// Object property iteration
    class PropertyIterator {
        friend class JavascriptValue;
    public:
        virtual const char16_t* GetKey(size_t *pLengthOut) const;
        virtual void GetValue(JavascriptValue *pValueOut) const;
        virtual void SetValue(const JavascriptValue &v);

		static const uint32_t kPropertyNameArraySize = 96; //EAWebKitTODO: Test the sizeof(JSC::PropertyNameArray) on all platforms and evaluate this number
	private:
        PropertyIterator(JavascriptValue *owner);
        PropertyIterator(const PropertyIterator& rhs);              // Simply declared, No copy supported
        PropertyIterator& operator =(const PropertyIterator& rhs);  // Simply declared, No assignment supported

        virtual ~PropertyIterator(void);

		EA_ALIGN(EA_PLATFORM_PTR_SIZE) char mNames[kPropertyNameArraySize];          
        void* mIterator;
        class JavascriptValue *mObject;
    };

    virtual PropertyIterator* First(void);
    virtual PropertyIterator* GetNext(void);

    inline JSC::JSValue* GetImpl(void) const
	{
		return (JSC::JSValue*) &mJSValue;
	}

    void SetExec(JSC::ExecState* exec);
    void Assign(const JSC::JSValue* jsValue);

	// Internal constructors, may only used inside DLL.
	JavascriptValue(JSC::JSValue* jsValue, JSC::ExecState* execState);
	JavascriptValue(void);

	// Copying - Javascript values need to be protected using the 
	// JSC::gcProtect API so that they don't get garbage collected
	// out from under the user.
	JavascriptValue(const JavascriptValue &source);

	void operator=(const JavascriptValue &source) {
		Copy(source);
	}

	virtual ~JavascriptValue(void) {
		Destruct();
	}
	static const int kJSValueSize = 8;
private:
    friend class PropertyIterator;

    virtual void Destruct(void);
    virtual void Copy(const JavascriptValue &source);

    EA_ALIGN(8) char mJSValue[kJSValueSize];
    JSC::ExecState* mExecState;
    PropertyIterator* mIterator;
};
}}

#endif
