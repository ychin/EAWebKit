#pragma once

#include <IEAController/IEAController.h>
#include <coreallocator/icoreallocator.h>

#include "Windows.Xbox.Input.h"

namespace EA
{
namespace Controller
{
	class CapilanoController;

	EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
	typedef CapilanoController CapilanoControllerAccessor;

	class CapilanoController : public IEAController
	{
	public:
		typedef Windows::Xbox::Input::IController^ ControllerIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
		EACONTROLLER_API CapilanoController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator, CapilanoControllerAccessor* accessor)
			: CapilanoController(controller, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EACONTROLLER_API CapilanoController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator);
		EACONTROLLER_API virtual ~CapilanoController() = default;

		// Unique identifier assigned by platform
		EACONTROLLER_API virtual const EAControllerString8& GetControllerIDAsString() const;
		EACONTROLLER_API virtual uint64_t GetControllerID() const;

		EACONTROLLER_API Windows::Xbox::Input::IController^ GetFirstPartyObject() const;
		EACONTROLLER_API Windows::Xbox::Input::GamepadButtons GetButtons() const;
		EACONTROLLER_API void SetFirstPartyObject(Windows::Xbox::Input::IController^ controller);

	private:
		Windows::Xbox::Input::IController^ mController;
		uint64_t                           mControllerID;
		EAControllerString8                mControllerIDString;
	};
}} // namespace EA::User
