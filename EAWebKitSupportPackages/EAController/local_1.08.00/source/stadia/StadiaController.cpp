// Copyright Electronic Arts. All rights reserved.

#include <EAController/StadiaController.h>

namespace EA
{
namespace Controller
{

StadiaController::StadiaController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator)
	: IEAController(CT_GAMEPAD, allocator)
	, mControllerID(id)
    , mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
{
	mControllerIDString.append_sprintf("%x", id);
}

const EAControllerString8& StadiaController::GetControllerIDAsString() const
{
	return mControllerIDString;
}

uint64_t StadiaController::GetControllerID() const
{
	return mControllerID;
}

}
}
