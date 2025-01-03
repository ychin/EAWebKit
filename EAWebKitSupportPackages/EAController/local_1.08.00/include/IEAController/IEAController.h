#pragma once

#include <IEAController/Config.h>

#include <coreallocator/icoreallocator.h>
#include <EAAssert/eaassert.h>
#include <EASTL/string.h>
#include <EASTL/fixed_list.h>
#include <EASTL/intrusive_ptr.h>
#include <EASTL/core_allocator_adapter.h>
#include <eathread/eathread_atomic.h>

namespace EA
{
	namespace User
	{
		// Forward declaration of IEAUser class
		class IEAUser;
		typedef eastl::intrusive_ptr<const IEAUser> IEAUserSmartPtr;
	}

	namespace Controller
	{
		typedef eastl::basic_string<char8_t,  EA::Allocator::EASTLICoreAllocator>  EAControllerString8;

		enum ControllerType
		{
			CT_UNSUPPORTED = 0,
			CT_GAMEPAD,
			CT_XINPUT_GAMEPAD,
			CT_DINPUT,
			CT_CAMERA_BODY, 
			CT_NAVIGATION, // Devices such as a Windows Smart Glass device
			CT_TOUCHSCREEN,
			CT_KEYBOARD,
			CT_MOUSE,

			CT_LIST_END
		};

		static const char* kControllerTypeStringArray[] = 
		{
			"CT_UNSUPPORTED",
			"CT_GAMEPAD",
			"CT_XINPUT_GAMEPAD",
			"CT_DINPUT",
			"CT_CAMERA_BODY",
			"CT_NAVIGATION", // Devices such as a Windows Smart Glass device
			"CT_TOUCHSCREEN",
			"CT_KEYBOARD",
			"CT_MOUSE",

			"CT_LIST_END"
		};	
		EA_COMPILETIME_ASSERT(EAArrayCount(kControllerTypeStringArray) == (CT_LIST_END + 1)); // + 1 since the ControllerType is 0 based

		// Platform agnostic controller class. This class gives game teams a pointer to 
		// a controller object which can be uniquely identified across the system
		// Each platform will have their own CapilanoController/KettleController implementation
		// which can platform specifics in platform specific code paths
		class EACONTROLLER_API IEAController
		{
		protected:
			IEAController(ControllerType type, EA::Allocator::ICoreAllocator* allocator);
			IEAController() = default;

		public:
			virtual ~IEAController() = default;

			// Unique identifier assigned by platform
			// For Kettle this is the controller's device handle. This package
			// is responsible for opening devices and closing them
			virtual const EAControllerString8& GetControllerIDAsString() const = 0;
			virtual uint64_t GetControllerID() const = 0;

			ControllerType GetType() const;
			const EA::User::IEAUser* GetPairedUser() const;

			void SetPairedUser(const EA::User::IEAUser* pUser);
			void SetPairedUser(const EA::User::IEAUserSmartPtr& pUser) { SetPairedUser(pUser.get()); }
			void SetType(ControllerType type);
			IEAController* GetControllerInstance() { return this; }

			//This ontended to be checked  guard against unsupported downcasts to real controller implementations in the case where the controller is simulated
			virtual bool IsSimulated() const { return false; };

			int AddRef() const;
			int Release() const;

		protected:
			EA::Allocator::ICoreAllocator*  mAllocator;
			const EA::User::IEAUser*        mPairedUser;
			ControllerType                  mType;
			mutable EA::Thread::AtomicInt32 mRefCount;
		};

		EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
		typedef IEAController IEAControllerAccessor;
		typedef eastl::intrusive_ptr<const IEAController> IEAControllerSmartPtr;

		typedef eastl::fixed_list<const EA::Controller::IEAController*, 8, true, EA::Allocator::EASTLICoreAllocator> ControllerList;
		typedef eastl::fixed_list<IEAControllerSmartPtr, 8, true, EA::Allocator::EASTLICoreAllocator> SmartControllerList;
	} // namespace Controller
} // namespace EA
