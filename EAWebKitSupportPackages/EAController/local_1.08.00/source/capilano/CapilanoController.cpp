#include "IEAController/IEAController.h"
#include "EAController/CapilanoController.h"
#include "EAStdC/EAString.h"
#include "EASTL/string.h"
#include "coreallocator/icoreallocator.h"
#include "EAAssert/eaassert.h"

EA_DISABLE_VC_WARNING(4691) // Official Xbox Wheel docs specifically says to ignore this warning "Windows::Xbox::Input::<windows input type>': type referenced was expected in unreferenced assembly 'Windows.Xbox.Input', type defined in current translation unit used instead"
EA_DISABLE_VC_WARNING(4555) // result of expression not used : vccorlib.h L994

namespace EA
{
namespace Controller
{
	CapilanoController::CapilanoController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator)
		: IEAController(CT_UNSUPPORTED, allocator)
		, mController(controller)
		, mControllerID()
		, mControllerIDString(EA::Allocator::EASTLICoreAllocator("[EAController] Controller ID String", allocator))
	{
		if(controller)
		{
			char buffer[32];
			EA::StdC::U64toa(mController->Id, buffer, 10);

			mControllerIDString = buffer;
			mControllerID = mController->Id;

			Platform::String^ controllerTypeStr = mController->Type;
			if (controllerTypeStr == L"Windows.Xbox.Input.Gamepad"
				|| controllerTypeStr == L"Microsoft.Xbox.Input.Wheel")
			{
				mType = CT_GAMEPAD;
			}
			else
			{
				mType = CT_UNSUPPORTED;
			}
		}
	}

	// Unique identifier assigned by platform
	const EAControllerString8& CapilanoController::GetControllerIDAsString() const
	{
		return mControllerIDString;
	}

	uint64_t CapilanoController::GetControllerID() const
	{
		return mControllerID;
	}

	Windows::Xbox::Input::IController^ CapilanoController::GetFirstPartyObject() const
	{
		return mController;
	}

	EACONTROLLER_API Windows::Xbox::Input::GamepadButtons CapilanoController::GetButtons() const
	{
		Windows::Xbox::Input::GamepadButtons buttons = Windows::Xbox::Input::GamepadButtons::None;
		if (auto gamepad = dynamic_cast<Windows::Xbox::Input::IGamepad^>(mController))
		{
			Windows::Xbox::Input::RawGamepadReading reading = gamepad->GetRawCurrentReading();
			buttons = reading.Buttons;
		} 
		else if (auto wheel = dynamic_cast<Microsoft::Xbox::Input::Wheel^>(mController)) 
		{
			Microsoft::Xbox::Input::IWheelReading^ reading = wheel->GetCurrentReading();
			int buttonMask = 0;
			buttonMask |= reading->IsDPadLeftPressed  ? (int)Windows::Xbox::Input::GamepadButtons::DPadLeft : 0;
			buttonMask |= reading->IsDPadUpPressed    ? (int)Windows::Xbox::Input::GamepadButtons::DPadUp : 0;
			buttonMask |= reading->IsDPadRightPressed ? (int)Windows::Xbox::Input::GamepadButtons::DPadRight : 0;
			buttonMask |= reading->IsDPadDownPressed  ? (int)Windows::Xbox::Input::GamepadButtons::DPadDown : 0;
			buttonMask |= reading->IsXPressed         ? (int)Windows::Xbox::Input::GamepadButtons::X : 0;
			buttonMask |= reading->IsYPressed         ? (int)Windows::Xbox::Input::GamepadButtons::Y : 0;
			buttonMask |= reading->IsBPressed         ? (int)Windows::Xbox::Input::GamepadButtons::B : 0;
			buttonMask |= reading->IsAPressed         ? (int)Windows::Xbox::Input::GamepadButtons::A : 0;
			buttonMask |= reading->IsMenuPressed      ? (int)Windows::Xbox::Input::GamepadButtons::Menu : 0;
			buttonMask |= reading->IsViewPressed      ? (int)Windows::Xbox::Input::GamepadButtons::View : 0;
			buttonMask |= reading->LeftPaddle         ? (int)Windows::Xbox::Input::GamepadButtons::LeftShoulder : 0;
			buttonMask |= reading->RightPaddle        ? (int)Windows::Xbox::Input::GamepadButtons::RightShoulder : 0;
			buttons = (Windows::Xbox::Input::GamepadButtons)buttonMask;
		}
		return buttons;
	}

	void CapilanoController::SetFirstPartyObject(Windows::Xbox::Input::IController^ controller)
	{
		mController = controller;
	}
}} //namespace EA::Controller

EA_RESTORE_VC_WARNING() // C4591
EA_RESTORE_VC_WARNING() // C4555
