#pragma once

#include <EAController/internal/Config.h>
#include <IEAController/IEAController.h>
#include <EASTL/string.h>
#include <eathread/eathread_atomic.h>
#include <coreallocator/icoreallocator.h>

#include <user_service.h>
#include <pad.h>

namespace EA
{
namespace Controller
{
	class SonyController;

	EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
	typedef SonyController SonyControllerAccessor;

	class EACONTROLLER_API SonyController : public IEAController
	{
	public:
		typedef int ControllerIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
		SonyController(EA::Allocator::ICoreAllocator* allocator, SonyControllerAccessor* accessor)
			: SonyController(allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		SonyController(EA::Allocator::ICoreAllocator* allocator);

		virtual ~SonyController();

		int32_t Open(SceUserServiceUserId userID);
		int32_t Close();

		// Unique identifier assigned by platform
		virtual const EAControllerString8& GetControllerIDAsString() const;

		uint64_t GetControllerID() const;
		bool IsConnected() const;
		int32_t SetLightBarColor(uint8_t r, uint8_t g, uint8_t b);

		EA_DEPRECATED_MESSAGE("Do not use this function outside of InputMan and EACUP. It's a temporary hack to get EACUP to pair wheels correctly.")
		void SetWheel();
		EA_DEPRECATED_MESSAGE("Do not use this function outside of InputMan and EACUP. It's a temporary hack to get EACUP to pair wheels correctly.")
		bool IsWheel() const { return mWheel; }
		void SetConnected(bool state);

	private:
		uint64_t							mControllerHandle;
		EAControllerString8					mControllerIDString;
		bool								mConnected;
		bool								mWheel;
	};
	
}} // namespace EA::User
