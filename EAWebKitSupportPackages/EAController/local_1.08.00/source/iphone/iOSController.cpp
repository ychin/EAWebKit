#include <EAController/iOSController.h>
#include <EAStdC/EAString.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>

namespace EA
{
	namespace Controller
	{
		iOSController::iOSController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator)
			: IEAController(CT_GAMEPAD, allocator)
			, mControllerID((uint64_t) controller)
			, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
		{
			char buffer[32];
			EA::StdC::U64toa(mControllerID, buffer, 10);

			mControllerIDString = buffer;
		}

		// Unique identifier assigned by platform
		const EAControllerString8& iOSController::GetControllerIDAsString() const
		{
			return mControllerIDString;
		}

		uint64_t iOSController::GetControllerID() const
		{
			return mControllerID;
		}

		GCController* iOSController::GetFirstPartyObject() const
		{
			return (GCController*) mControllerID;
		}
	}
} //namespace EA::Controller
