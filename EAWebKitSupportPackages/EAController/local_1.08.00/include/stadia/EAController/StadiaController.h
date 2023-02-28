// Copyright Electronic Arts. All rights reserved.

#pragma once

#include <EAController/internal/Config.h>
#include <IEAController/IEAController.h>
#include <coreallocator/icoreallocator.h>

#include <ggp/input.h>

namespace EA
{
namespace Controller
{

class EACONTROLLER_API StadiaController : public IEAController
{
public:
	typedef GgpReference ControllerIdType;

	StadiaController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator);

	virtual const EAControllerString8& GetControllerIDAsString() const;
	virtual uint64_t GetControllerID() const;

private:
	GgpReference mControllerID;
	EAControllerString8 mControllerIDString;
};

}
}
