#include <EAController/PCController.h>
#include <EAStdC/EAString.h>
#include <EASTL/string.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>

#if !defined(DIRECTINPUT_VERSION) || (DIRECTINPUT_VERSION < 0x0800)
	#undef  DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif
EA_DISABLE_ALL_VC_WARNINGS()
#include <dinput.h>
EA_RESTORE_ALL_VC_WARNINGS()

namespace EA
{
namespace Controller
{
	PCController::PCController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAController(CT_GAMEPAD, allocator)
		, mControllerID(userID)
		, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
	{
		char buffer[32];
		EA::StdC::U64toa(mControllerID, buffer, 10);

		mControllerIDString = buffer;
	}

	PCController::~PCController()
	{
		if (mType == CT_DINPUT)
		{
			IDirectInput8W* pDevice = (IDirectInput8W*)mControllerID;
			pDevice->Release();
		}
	}

	// Unique identifier assigned by platform
	const EAControllerString8& PCController::GetControllerIDAsString() const
	{
		return mControllerIDString;
	}

	uint64_t PCController::GetControllerID() const
	{
		return mControllerID;
	}
}} //namespace EA::Controller
