// Copyright Electronic Arts. All rights reserved.

#pragma once

#include <coreallocator/icoreallocator.h>
#include <EABase/eabase.h>
#include <EAController/internal/Config.h>
#include <IEAController/IEAController.h>

#if !defined (EA_PLATFORM_SONY)

#include <GameInput.h>
#include <wrl/client.h>

namespace EA
{
namespace Controller
{

class GDKController;

EA_DEPRECATED_MESSAGE("The controller accessor classes have been deprecated in favour of accessing controllers directly. Please remove references to this type, either by changing to plain controllers or removing them entirely (if you are creating them).")
typedef GDKController GDKControllerAccessor;

class EACONTROLLER_API GDKController : public IEAController
{
public:
	typedef IGameInputDevice* ControllerIdType;

	EA_DISABLE_VC_WARNING(4996);
	EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
	EA_DEPRECATED_MESSAGE("The controller accessor constructors for controller classes have been deprecated. Remove the accessor argument from your controller constructor calls.")
	GDKController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator, GDKControllerAccessor* accessor)
		: GDKController(id, allocator) {}
	EA_RESTORE_VC_WARNING();
	EA_RESTORE_CLANG_WARNING();

	GDKController(ControllerIdType id, EA::Allocator::ICoreAllocator* allocator);
	virtual ~GDKController() = default;

	virtual const EAControllerString8& GetControllerIDAsString() const;
	virtual uint64_t GetControllerID() const;

	ControllerIdType GetFirstPartyObject() const;

private:
	Microsoft::WRL::ComPtr<IGameInputDevice> mDevice;
	EAControllerString8 mControllerIDString;
	uint64_t mControllerID;
};

}
}
#endif
