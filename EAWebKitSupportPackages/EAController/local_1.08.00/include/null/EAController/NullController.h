#pragma once

#include "IEAController/IEAController.h"
#include "EAController/internal/Config.h"
#include "EASTL/string.h"
#include "EASTL/list.h"
#include "eathread/eathread_atomic.h"
#include "coreallocator/icoreallocator.h"

namespace EA
{
namespace Controller
{
	class NullController;

	EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
	typedef NullController NullControllerAccessor;

	class NullController : public IEAController
	{
	public:
		typedef uint64_t ControllerIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
		EACONTROLLER_API NullController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator, NullControllerAccessor* accessor)
			: NullController(userID, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EACONTROLLER_API NullController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator);
		EACONTROLLER_API virtual ~NullController() = default;

		EACONTROLLER_API virtual const EAControllerString8& GetControllerIDAsString() const;
		EACONTROLLER_API virtual uint64_t GetControllerID() const;

	private:
		uint64_t                        mControllerID;
		EAControllerString8             mControllerIDString;
	};
	
}} // namespace EA::Controller
