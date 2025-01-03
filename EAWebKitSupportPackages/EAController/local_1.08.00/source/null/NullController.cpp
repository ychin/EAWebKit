#include <EAController/NullController.h>
#include <EAStdC/EAString.h>
#include <EASTL/string.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>

namespace EA
{
namespace Controller
{
	NullController::NullController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAController(CT_UNSUPPORTED, allocator)
		, mControllerID(userID)
		, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
	{
		char buffer[32];
		EA::StdC::U64toa(mControllerID, buffer, 10);

		mControllerIDString = buffer;
	}

	// Unique identifier assigned by platform
	const EAControllerString8& NullController::GetControllerIDAsString() const
	{
		return mControllerIDString;
	}

	uint64_t NullController::GetControllerID() const
	{
		return mControllerID;
	}
}} //namespace EA::Controller
