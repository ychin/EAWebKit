#pragma once

// disable warnings for accessor typedefs
EA_DISABLE_VC_WARNING(4996);
EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);

#if defined(EA_PLATFORM_XBOXONE_XDK)
	#include <EAController/CapilanoController.h>
namespace EA {
	namespace Controller {
		typedef EA::Controller::CapilanoController EAPlatformController;
		typedef EA::Controller::CapilanoControllerAccessor EAPlatformControllerAccessor;
	}
}
#elif defined EA_PLATFORM_MICROSOFT && defined(EA_PLATFORM_GDK)
	#include <EAController/GDKController.h>
namespace EA {
	namespace Controller {
		typedef EA::Controller::GDKController EAPlatformController;
		typedef EA::Controller::GDKControllerAccessor EAPlatformControllerAccessor;
	}
}
#elif defined EA_PLATFORM_WINDOWS
	#include <EAController/PCController.h>
namespace EA {
	namespace Controller {
		typedef EA::Controller::PCController EAPlatformController;
		typedef EA::Controller::PCControllerAccessor EAPlatformControllerAccessor;
	}
}	
#elif defined EA_PLATFORM_PS4
	#include <EAController/KettleController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::KettleController EAPlatformController;
	typedef EA::Controller::KettleControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_SONY
	#include <EAController/SonyController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::SonyController EAPlatformController;
	typedef EA::Controller::SonyControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_ANDROID
	#include <EAController/AndroidController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::AndroidController EAPlatformController;
	typedef EA::Controller::AndroidControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_IPHONE
	#include <EAController/iOSController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::iOSController EAPlatformController;
	typedef EA::Controller::iOSControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_OSX
	#include <EAController/OSXController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::OSXController EAPlatformController;
	typedef EA::Controller::OSXControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_NX
	#include <EAController/NXController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::NXController EAPlatformController;
	typedef EA::Controller::NXControllerAccessor EAPlatformControllerAccessor;
}}
#elif defined EA_PLATFORM_STADIA
	#include <EAController/StadiaController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::StadiaController EAPlatformController;
	typedef EA::Controller::StadiaController EAPlatformControllerAccessor;
}}
#else
	#include <EAController/NullController.h>
namespace EA{ namespace Controller {
	typedef EA::Controller::NullController EAPlatformController;
	typedef EA::Controller::NullControllerAccessor EAPlatformControllerAccessor;
}}

EA_RESTORE_VC_WARNING();
EA_RESTORE_CLANG_WARNING();

#endif
