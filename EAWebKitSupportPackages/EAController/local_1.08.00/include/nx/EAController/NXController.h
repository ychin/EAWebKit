// Copyright Electronic Arts. All rights reserved.

#pragma once

#include <nn/hid/hid_Npad.h>

#include <coreallocator/icoreallocator.h>
#include <EABase/eabase.h>
#include <EAController/internal/Config.h>
#include <eathread/eathread_atomic.h>
#include <IEAController/IEAController.h>

namespace EA
{
namespace Controller
{

class NXController;

EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
typedef NXController NXControllerAccessor;

class EACONTROLLER_API NXController : public IEAController
{
public:
	typedef nn::hid::NpadIdType ControllerIdType;

	EA_DISABLE_VC_WARNING(4996);
	EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
	EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
	NXController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator, NXControllerAccessor* accessor)
		: NXController(id, allocator) {}
	EA_RESTORE_VC_WARNING();
	EA_RESTORE_CLANG_WARNING();

	NXController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator);

	virtual ~NXController() = default;

	virtual const EAControllerString8& GetControllerIDAsString() const;
	virtual uint64_t GetControllerID() const;

 private:
	nn::hid::NpadIdType             mNpadId;
	EAControllerString8             mControllerIDString;
};

}
}
