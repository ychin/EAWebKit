
/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"
#include "ea_instance.h"

#include "Error.h"
#include "JSDOMBinding.h"
#include "JSGlobalObject.h"
#include "JSLock.h"
#include "ea_class.h"
#include "ea_runtime.h"
#include "runtime_object.h"

#include <EAWebKit/EAWebkitJavascriptValue.h>
namespace JSC { namespace Bindings {


EAInstance::EAInstance(EA::WebKit::IJSBoundObject *object, PassRefPtr<RootObject> rootObject)
: Instance(rootObject)
{
    mBoundObject = object;
    mClass = NULL;
}

EAInstance::~EAInstance() 
{
    mBoundObject = NULL;
    mClass = NULL;
}

Class *EAInstance::getClass() const
{
    if (!mClass)
        mClass = EAClass::classForInstance(this);
    return mClass; 
}

class EARuntimeMethod : public RuntimeMethod {
public:
    typedef RuntimeMethod Base;

    static EARuntimeMethod* create(ExecState* exec, JSGlobalObject* globalObject, const String& name, Bindings::Method* method)
    {
        // FIXME: deprecatedGetDOMStructure uses the prototype off of the wrong global object
        // We need to pass in the right global object for "i".
        Structure* domStructure = WebCore::deprecatedGetDOMStructure<EARuntimeMethod>(exec);
		EARuntimeMethod* runtimeMethod = new (NotNull, allocateCell<EARuntimeMethod>(*exec->heap())) EARuntimeMethod(globalObject, domStructure, method);
		runtimeMethod->finishCreation(exec->vm(), name);
		return runtimeMethod;
    }

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    DECLARE_INFO;

private:
    EARuntimeMethod(JSGlobalObject* globalObject, Structure* structure, Bindings::Method* method)
        : RuntimeMethod(globalObject, structure, method)
    {
    }

    void finishCreation(VM& vm, const String& name)
    {
		Base::finishCreation(vm, name);
		ASSERT(inherits(info()));
    }
};

const ClassInfo EARuntimeMethod::s_info = { "EARuntimeMethod", &RuntimeMethod::s_info, 0, 0,CREATE_METHOD_TABLE(EARuntimeMethod) };

JSValue EAInstance::getMethod(ExecState* exec, PropertyName propertyName)
{
	Method* method = getClass()->methodNamed(propertyName, this);
	return EARuntimeMethod::create(exec, exec->lexicalGlobalObject(), propertyName.publicName(), method);
}

typedef eastl::fixed_vector<EA::WebKit::JavascriptValue, 8> EAArgumentList;

JSValue EAInstance::invokeMethodPriv(ExecState* exec, const char8_t* methodName)
{
	// Translate all the arguments into C++ JavascriptValue implementation.
	unsigned count = exec->argumentCount();
	EAArgumentList cArgs(count);

	for (unsigned i = 0; i < count; i++)
	{
		cArgs[i].SetExec(exec);
		JStoEA(exec, exec->uncheckedArgument(i), &cArgs[i]); //Because we have already obtained the count and know that index is valid, using uncheckedArgument is faster than argument.
	}

	// Call the bound function.
	bool success = true;

	JSValue undefined(jsUndefined()); //We initialize it to undefined as a JavaScript function with no return value returns undefined.
	EA::WebKit::JavascriptValue result(&undefined, exec);

	{
		// EAWebKitTODO: Its unclear whats happening here. We do it because C implementation does the same thing. In our last port, we had this but that was effectively no-op. I am not sure we gain anything
		// by dropping all the locks when calling the callback.
		JSLock::DropAllLocks dropAllLocks(exec); 
		success = methodName ? mBoundObject->invokeMethod(methodName, cArgs.data(), count, &result) : mBoundObject->invoke(cArgs.data(), count, &result);
	}

	if(success)
		return EAtoJS(exec, result);
	else
	{
		if(methodName)
			exec->vm().throwException(exec, createError(exec, ASCIILiteral("Error invoking method on bound object.")));
		else
			exec->vm().throwException(exec, createError(exec, ASCIILiteral("Error invoking bound object as a function.")));
	}

	return jsUndefined();

}
JSValue EAInstance::invokeMethod(ExecState* exec, RuntimeMethod* runtimeMethod)
{
	if (!asObject(runtimeMethod)->inherits(EARuntimeMethod::info()))
		return exec->vm().throwException(exec, createTypeError(exec, "Wrong method invocation on bound object."));


    if(EAMethod* method = static_cast<EAMethod*>(runtimeMethod->method()))
	{
		return invokeMethodPriv(exec,method->eaName());
	}
	
	return jsUndefined();
}

bool EAInstance::supportsInvokeDefaultMethod() const
{
	return mBoundObject->isFunction();
}

JSValue EAInstance::invokeDefaultMethod(ExecState* exec)
{
    if (!mBoundObject->isFunction())
        return jsUndefined();

	return invokeMethodPriv(exec,NULL);
}

JSValue EAInstance::defaultValue(ExecState* exec, PreferredPrimitiveType hint) const
{
    if (hint == PreferNumber)
    {
        return jsNumber(0);
    }
    
    return valueOf(exec);
}

//EAWebKitTODO: valueOf is different from toString. Thus, the implementation here is wrong.
JSValue EAInstance::valueOf(ExecState* exec) const 
{
    return jsString(exec, mBoundObject->stringValue());
}

void EAInstance::getPropertyNames(ExecState* exec, PropertyNameArray& nameArray)
{
	JSLock::DropAllLocks dropAllLocks(exec);
	EA::WebKit::IJSBoundObject::PropertyIterator* iter = mBoundObject->First();
	while(iter)
	{
		nameArray.add(Identifier(exec,iter->GetKey()));
		iter = mBoundObject->GetNext();
	}
}

}}
