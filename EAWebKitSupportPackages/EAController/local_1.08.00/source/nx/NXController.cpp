#include <EAController/NXController.h>

namespace EA
{
namespace Controller
{
NXController::NXController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator)
	: IEAController(CT_GAMEPAD, allocator)
	, mNpadId(id)
	, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
{
	mControllerIDString.append_convert(eastl::to_string(id).c_str());
}

const EAControllerString8& NXController::GetControllerIDAsString() const
{
	return mControllerIDString;
}

uint64_t NXController::GetControllerID() const
{
	return static_cast<uint64_t>(mNpadId);
}

}
}
