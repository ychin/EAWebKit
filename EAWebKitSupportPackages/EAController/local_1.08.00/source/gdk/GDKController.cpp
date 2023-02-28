#include <EAController/GDKController.h>

#if !defined(EA_PLATFORM_SONY)
namespace EA
{
namespace Controller
{
GDKController::GDKController(ControllerIdType device, EA::Allocator::ICoreAllocator* allocator)
    : IEAController(CT_GAMEPAD, allocator)
    , mDevice(device)
    , mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
	, mControllerID(reinterpret_cast<uint64_t>(device))
{
	mControllerIDString.append_sprintf("%x", device);
}

const EAControllerString8& GDKController::GetControllerIDAsString() const
{
	return mControllerIDString;
}

uint64_t GDKController::GetControllerID() const
{
	return mControllerID;
}

GDKController::ControllerIdType GDKController::GetFirstPartyObject() const
{
	return mDevice.Get();
}
}
}
#endif
