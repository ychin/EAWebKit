#pragma once

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4275)  // non dll-interface class used as base for dll-interface class.
#endif

#include "IEAController/IEAController.h"
#include "EAController/internal/Config.h"
#include "EASTL/string.h"
#include "EASTL/list.h"
#include "eathread/eathread_atomic.h"
#include "coreallocator/icoreallocator.h"

#if defined(_MSC_VER)
	#pragma warning(push, 0)
#endif
#include <Windows.h>
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

namespace EA
{
namespace Controller
{
	class PCController;

	EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
	typedef PCController PCControllerAccessor;

	class PCController : public IEAController
	{
	public:
		typedef uint64_t ControllerIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
		EACONTROLLER_API PCController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator, PCControllerAccessor* accessor)
			: PCController(userID, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EACONTROLLER_API PCController(ControllerIdType userID, EA::Allocator::ICoreAllocator* allocator);
		EACONTROLLER_API virtual ~PCController();

		EACONTROLLER_API virtual const EAControllerString8& GetControllerIDAsString() const;
		EACONTROLLER_API virtual uint64_t GetControllerID() const;

	private:
		uint64_t                        mControllerID;
		EAControllerString8             mControllerIDString;
	};
}} // namespace EA::Controller

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
