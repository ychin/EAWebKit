// Copyright Electronic Arts. All rights reserved.

#include <ggp/profile.h>
#include <ggp/status.h>

#include <IEAController/IEAController.h>
#include <EAUser/StadiaUser.h>

#include <EAAssert/eaassert.h>
#include <EASTL/string.h>
#include <EAStdC/EAString.h>

namespace EA
{
namespace User
{

StadiaUser::StadiaUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
	: IEAUser(allocator)
	, mUserID(userID)
	, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
	, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] Display Name String", allocator))
{
	GgpProfile profile;
	GgpFuture result = GgpGetProfile(mUserID);
	GgpStatus status;
	bool ret = GgpFutureGetResult(result, &profile, sizeof(GgpProfile), &status);
	EA_ASSERT_FORMATTED(ret, ("Unable to get player profile from GGP - error %d", status.status_code));
	EA_UNUSED(ret);
	EA_UNUSED(result);
	EA_UNUSED(status);

	mUserIDString.append_convert(eastl::to_string(mUserID).c_str());
	mDisplayName.assign_convert(profile.stadia_name, EA::StdC::Strlen(profile.stadia_name));
}

StadiaUser::~StadiaUser() = default;

const EAUserString8& StadiaUser::GetUserIDAsString() const
{
	return mUserIDString;
}

EAUserID StadiaUser::GetUserID() const
{
	return mUserID;
}

const EAUserString16& StadiaUser::GetGamerTag() const
{
	return mDisplayName;
}

const EAUserString16& StadiaUser::GetDisplayName() const
{
	return mDisplayName;
}

UserOnlineState StadiaUser::GetOnlineState() const
{
	return USER_STATE_ONLINE;
}

bool StadiaUser::IsGuest() const
{
	return false;
}

bool StadiaUser::IsSignedIn() const
{
	return true;
}

bool StadiaUser::IsValid() const
{
	return true;
}

bool StadiaUser::Equals(const IEAUser* other) const
{
	if (!other)
	{
		return false;
	}

	return *this == *static_cast<const StadiaUser*>(other);
}

bool StadiaUser::operator==(const StadiaUser& other) const
{
	return mUserID == other.mUserID;
}

void StadiaUser::SetOnlineState(UserOnlineState state)
{
	// currently there's no need to force a user's online state one way or
	// the other on Stadia, so this function is unimplemented
	EA_ASSERT_MESSAGE(false, "unable to set user online state on Stadia");
}

}
}
