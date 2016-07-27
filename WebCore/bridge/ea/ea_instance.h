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

#ifndef EAWEBKIT_EA_INSTANCE_H
#define EAWEBKIT_EA_INSTANCE_H

#include "BridgeJSC.h"
#include "runtime_root.h"
#include "ea_class.h"
#include "ea_utility.h"
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_vector.h>

namespace EA
{
namespace WebKit
{
class IJSBoundObject;
}
}

namespace JSC { namespace Bindings {

// The EAInstance acts as the glue between the object in the JavaScript land and native land. It executes runtime method calls etc.
// The field read/write are done via a different path though. 

class EAInstance : public Instance {
	WTF_MAKE_NONCOPYABLE(EAInstance);
public:
    static PassRefPtr<EAInstance> create(EA::WebKit::IJSBoundObject* object, PassRefPtr<RootObject> rootObject)
    {
        return adoptRef(new EAInstance(object, rootObject));
    }

    virtual ~EAInstance(void);

    virtual Class *getClass() const OVERRIDE;

	//virtual bool setValueOfUndefinedField(ExecState*, PropertyName, JSValue) { return false; }
	
	virtual JSValue getMethod(ExecState*, PropertyName) OVERRIDE;
	virtual JSValue invokeMethod(ExecState*, RuntimeMethod*) OVERRIDE;
	
	virtual bool supportsInvokeDefaultMethod() const OVERRIDE;
	virtual JSValue invokeDefaultMethod(ExecState*) OVERRIDE;

	//EAWebKitTODO: Implement these to support bound object as constructor. 
	//virtual bool supportsConstruct() const { return false; }
	//virtual JSValue invokeConstruct(ExecState*, const ArgList&) { return JSValue(); }
	
	//EAWebKitTODO: We currently don't support methods, only fields.
	virtual void getPropertyNames(ExecState*, PropertyNameArray&) OVERRIDE;
	
    virtual JSValue defaultValue(ExecState*, PreferredPrimitiveType) const OVERRIDE;

	virtual JSValue valueOf(ExecState*) const OVERRIDE;

	//EAWebKitTODO: Implement these to support dynamic object member addition.
	//virtual bool getOwnPropertySlot(JSObject*, ExecState*, PropertyName, PropertySlot&) { return false; }
	//virtual void put(JSObject*, ExecState*, PropertyName, JSValue, PutPropertySlot&) { }

    EA::WebKit::IJSBoundObject* getBoundObject(void) const { return mBoundObject; }

private:
	// Invoke the method identified by the name. If name is NULL, invoke self as a function.
	JSValue invokeMethodPriv(ExecState*, const char8_t* methodName);
	
	EAInstance(EA::WebKit::IJSBoundObject*, PassRefPtr<RootObject>);
    mutable EAClass *mClass;
    EA::WebKit::IJSBoundObject *mBoundObject;
};

}}

#endif
