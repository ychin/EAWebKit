#pragma once

#include "IEAController/IEAController.h"
#include "eathread/eathread_atomic.h"
#include "coreallocator/icoreallocator.h"

#ifdef __OBJC__
@class GCController;
#else
class GCController;
#endif

namespace EA
{
namespace Controller
{
	class iOSController;

	EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
	typedef iOSController iOSControllerAccessor;

	class iOSController : public IEAController
	{
	public:
		typedef GCController* ControllerIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
		EACONTROLLER_API iOSController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator, iOSControllerAccessor* accessor)
			: iOSController(controller, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EACONTROLLER_API iOSController(ControllerIdType controller, EA::Allocator::ICoreAllocator* allocator);
		EACONTROLLER_API virtual ~iOSController() = default;

		EACONTROLLER_API virtual const EAControllerString8& GetControllerIDAsString() const;
		EACONTROLLER_API virtual uint64_t GetControllerID() const;

		EACONTROLLER_API GCController* GetFirstPartyObject() const;

	private:
		uint64_t                        mControllerID;  // Stores the GCController pointer
		EAControllerString8             mControllerIDString;
	};
}
}
