/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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

#include "ea_class.h"
#include "ea_instance.h"
#include "ea_runtime.h"
#include <EAWebKit/EAWebkitJavascriptValue.h>

namespace JSC { namespace Bindings {

// Each bound instance has a different blueprint. We create one here and keep it around for look up.
typedef HashMap<const Instance*, Class*> InstanceClassMap;
static InstanceClassMap* instanceClassMap = 0;

EAClass* EAClass::classForInstance(const Instance* inst) 
{
    if (!instanceClassMap)
        instanceClassMap = new InstanceClassMap;

    Class* eaClass = instanceClassMap->get(inst);
    if (!eaClass)
	{
        eaClass = new EAClass();
        instanceClassMap->set(inst, eaClass);
    }

    return static_cast<EAClass*>(eaClass);
}

void EAClass::Finalize(void) 
{
    if (instanceClassMap)
	{
        InstanceClassMap::iterator start = instanceClassMap->begin();
        InstanceClassMap::iterator end = instanceClassMap->end();
        for (InstanceClassMap::iterator iObj = start; iObj != end; ++iObj)
            delete iObj->value;

        instanceClassMap->clear();
        delete instanceClassMap;
        instanceClassMap = NULL;
    }
}

EAClass::~EAClass()
{
    mMethods.clear();
    mFields.clear();
}

// Query the bound object if it has the method identified by propertyName. If yes, return it and cache it for future look ups. 
Method* EAClass::methodNamed(PropertyName propertyName, Instance* instance) const
{
    String name(propertyName.publicName());
	
    if (Method* method = mMethods.get(name.impl())) 
        return method;

	CString nameStr = name.ascii();
	const char *pName = nameStr.data(); 

    EA::WebKit::IJSBoundObject *obj = static_cast<const EAInstance*>(instance)->getBoundObject();
    if (obj->hasMethod(pName)) 
    {
        Method* eaMethod = new EAMethod(pName);
        mMethods.set(name.impl(), adoptPtr(eaMethod));
        return eaMethod;
    }

    return 0;
}

// Query the bound object if it has the field identified by propertyName. If yes, return it and cache it for future look ups. 
Field* EAClass::fieldNamed(PropertyName propertyName, Instance* instance) const
{
    String name(propertyName.publicName());
	
    if (Field* aField = mFields.get(name.impl()))
		return aField;
    
	// Turns out that JSC always checks for field first and method next (if the field is not found). So in case of a method call, we'll unnecessarily go
	// through all the fields across the dll boundary. So optimize that look up from O(n) to O(1) where n is the number of fields.
	if (Method* method = mMethods.get(name.impl())) 
		return 0; //not a typo

	CString nameStr = name.ascii();
	const char *pName = nameStr.data();  

    EA::WebKit::IJSBoundObject *obj = static_cast<const EAInstance*>(instance)->getBoundObject();
    if (obj->hasProperty(pName))
    {
        Field* aField = new EAField(pName);
        mFields.set(name.impl(), adoptPtr(aField));
		return aField;
    }

    return 0;
}

}}
