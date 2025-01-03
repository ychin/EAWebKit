#pragma once
#include <EABase/eabase.h>

EA_DISABLE_VC_WARNING(4996);
EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);

#if defined(EA_PLATFORM_XBOXONE_XDK)
	#include <EAUser/CapilanoUser.h>
namespace EA{ namespace User {
	typedef EA::User::CapilanoUser EAPlatformUser;
	typedef EA::User::CapilanoUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_MICROSOFT && defined(EA_PLATFORM_GDK)
	#include <EAUser/GDKUser.h>
namespace EA{ namespace User {
	typedef EA::User::GDKUser EAPlatformUser;
	typedef EA::User::GDKUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_PS4
	#include <EAUser/KettleUser.h>
namespace EA{ namespace User {
	typedef EA::User::KettleUser EAPlatformUser;
	typedef EA::User::KettleUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_SONY
	#include <EAUser/SonyUser.h>
namespace EA{ namespace User {
	typedef EA::User::SonyUser EAPlatformUser;
	typedef EA::User::SonyUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_WINDOWS
	#include <EAUser/PCUser.h>
namespace EA{ namespace User {
	typedef EA::User::PCUser EAPlatformUser;
	typedef EA::User::PCUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_ANDROID
	#include <EAUser/AndroidUser.h>
namespace EA{ namespace User {
	typedef EA::User::AndroidUser EAPlatformUser;
	typedef EA::User::AndroidUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_IPHONE
	#include <EAUser/iOSUser.h>
namespace EA{ namespace User {
	typedef EA::User::iOSUser EAPlatformUser;
	typedef EA::User::iOSUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_OSX
	#include <EAUser/OSXUser.h>
namespace EA{ namespace User {
	typedef EA::User::OSXUser EAPlatformUser;
	typedef EA::User::OSXUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_NX
	#include <EAUser/NXUser.h>
namespace EA{ namespace User {
	typedef EA::User::NXUser EAPlatformUser;
	typedef EA::User::NXUserAccessor EAPlatformUserAccessor;
}}

#elif defined EA_PLATFORM_STADIA
	#include <EAUser/StadiaUser.h>
namespace EA{ namespace User {
	typedef EA::User::StadiaUser EAPlatformUser;
	typedef EA::User::StadiaUser EAPlatformUserAccessor;
}}

#else
	#include <EAUser/NullUser.h>
namespace EA{ namespace User {
	typedef EA::User::NullUser EAPlatformUser;
	typedef EA::User::NullUserAccessor EAPlatformUserAccessor;
}}
#endif

EA_RESTORE_VC_WARNING();
EA_RESTORE_CLANG_WARNING();
