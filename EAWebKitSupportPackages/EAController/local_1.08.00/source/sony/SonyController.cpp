#include "IEAController/IEAController.h"
#include "EAController/SonyController.h"
#include "EAStdC/EAMemory.h"
#include "EAStdC/EAString.h"
#include "EASTL/string.h"
#include "EAAssert/eaassert.h"
#include "coreallocator/icoreallocator.h"

#include <user_service.h>
#include <scebase.h>

namespace EA
{
namespace Controller
{
	SonyController::SonyController(EA::Allocator::ICoreAllocator* allocator)
		: IEAController(CT_UNSUPPORTED, allocator)
		, mControllerHandle((uint64_t)-1)
		, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] ControllerID String", allocator))
		, mWheel(false)
	{
		char buffer[32];
		EA::StdC::U64toa(mControllerHandle, buffer, 10);

		mControllerIDString = buffer;
	}

	SonyController::~SonyController()
	{
		if (mControllerHandle != -1)
		{
			Close();
		}
	}

	int32_t SonyController::Open(SceUserServiceUserId userID)
	{
		int result = scePadOpen(userID, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
		if(result > 0) {
			mControllerHandle = result;
			char buffer[32];
			EA::StdC::U64toa(mControllerHandle, buffer, 10);

			mControllerIDString = buffer;
			mType = CT_GAMEPAD;

			ScePadControllerInformation info;

			// use a different status variable so we can still report the controller
			// handle/error value for scePadOpen
			int status = scePadGetControllerInformation(mControllerHandle, &info);
			if (status == SCE_OK)
			{
				mConnected = info.connected;
			}
		}

		return result;
	}

	int32_t SonyController::Close()
	{
		if (mWheel)
			return 0;
		int result = scePadClose(mControllerHandle);

		if(result > SCE_OK)
		{
			mControllerHandle = -1;
		}

		return result;
	}

	// Unique identifier assigned by platform
	const EAControllerString8& SonyController::GetControllerIDAsString() const
	{
		return mControllerIDString;
	}

	uint64_t SonyController::GetControllerID() const
	{
		return mControllerHandle;
	}

	bool SonyController::IsConnected() const
	{
		return mConnected;
	}

	void SonyController::SetConnected(bool state)
	{
		mConnected = state;
	}

	void SonyController::SetWheel()
	{
		mType = CT_GAMEPAD;
		mWheel = true;
		mConnected = true;
	}

	int32_t SonyController::SetLightBarColor(uint8_t r, uint8_t g, uint8_t b)
	{
		if (mWheel)
			return 0;
		ScePadLightBarParam lightBarParam;
		EA::StdC::Memset8(&lightBarParam, 0, sizeof(lightBarParam));
		lightBarParam.r = r;
		lightBarParam.g = g;
		lightBarParam.b = b;

		return scePadSetLightBar(mControllerHandle, &lightBarParam);
	}

}} //namespace EA::Controller
