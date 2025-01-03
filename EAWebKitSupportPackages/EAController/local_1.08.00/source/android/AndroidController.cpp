#include "IEAController/IEAController.h"
#include "EAController/AndroidController.h"
#include "EAStdC/EAString.h"
#include "EASTL/string.h"
#include "coreallocator/icoreallocator.h"
#include "EAAssert/eaassert.h"

namespace EA
{
namespace Controller
{
	AndroidController::AndroidController(ControllerIdType controllerID, EA::Allocator::ICoreAllocator* allocator)
		: IEAController(CT_UNSUPPORTED, allocator)
		, mControllerID(controllerID)
		, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
	{
		char buffer[32];
		EA::StdC::U64toa(mControllerID, buffer, 10);

		mControllerIDString = buffer;
	}

	// Unique identifier assigned by platform
	const EAControllerString8& AndroidController::GetControllerIDAsString() const
	{
		return mControllerIDString;
	}

	uint64_t AndroidController::GetControllerID() const
	{
		return mControllerID;
	}

}} //namespace EA::Controller

