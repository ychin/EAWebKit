/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2014 Electronic Arts, Inc. All rights reserved.
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

#ifndef EAWEBKIT_EA_CLASS_H
#define EAWEBKIT_EA_CLASS_H

#include "BridgeJSC.h"

namespace JSC { namespace Bindings {

// This is the blueprint of the object bound to the page. This is invoked to check the validity of 
// requested field/method for each access (read/write) of the bound object in the JavaScript land.
// If a field/method is not valid, the js value is turned into jsUndefined.

class EAClass : public Class
{
WTF_MAKE_NONCOPYABLE(EAClass);
protected:
    EAClass(void) {}

public:
    static EAClass* classForInstance(const Instance* inst);
    static void Finalize(void);

    virtual ~EAClass(void);

	virtual Method* methodNamed(PropertyName, Instance*) const OVERRIDE;
	virtual Field* fieldNamed(PropertyName, Instance*) const OVERRIDE;

private:
	mutable HashMap<RefPtr<StringImpl>, OwnPtr<Method>> mMethods;
	mutable HashMap<RefPtr<StringImpl>, OwnPtr<Field>> mFields;
};

}}

#endif
