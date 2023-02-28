// Copyright Electronic Arts. All rights reserved.

#include <nn/account/account_ApiForApplications.h>
#include <nn/account/account_Api.h>
#include <nn/account/account_Result.h>

#include <EAUser/NXUser.h>

#include <coreallocator/icoreallocator.h>
#include <EAAssert/eaassert.h>
#include <EAStdC/EAString.h>
#include <IEAController/IEAController.h>

namespace EA
{
namespace User
{
NXUser::NXUser(nn::account::UserHandle userHandle, EA::Allocator::ICoreAllocator* allocator)
	: IEAUser(allocator)
	, mhUser(userHandle)
	, mbIsGuest(false)
	, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
	, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] Display Name String", allocator))
{
	auto result = nn::account::GetUserId(&mUserID, mhUser);
	Initialize(!result.IsSuccess());
}

void NXUser::Initialize(bool guest)
{
	// There's no good concept of a 'guest' user ID on NX, so we use a static
	// counter here to give them out. This won't persist across reboots, but
	// there should be no way for guests to maintain persistent user information
	// across reboots on NX, so that's fine.
	static uint32_t nGuests = 0;
	mbIsGuest = guest;

	if (IsValid() && !mbIsGuest)
	{
		mUserIDString.append_convert(eastl::to_string(mUserID._data[0]).c_str());
		mUserIDString.append_convert(eastl::to_string(mUserID._data[1]).c_str());

		nn::account::Nickname nickname;
		auto result = nn::account::GetNickname(&nickname, mUserID);
		EA_UNUSED(result);
		EA_ASSERT_MESSAGE(result.IsSuccess(), "unable to get nickname for user");
		mDisplayName.assign_convert(nickname.name, EA::StdC::Strlen(nickname.name));
	}
	else
	{
		uint32_t id = nGuests++;
		mGuestID = id;
		mUserIDString.sprintf("Guest %d", id);
		mDisplayName.sprintf(u"Guest %d", id);
	}
}

NXUser::~NXUser()
{
	if (IsValid() && !mbIsGuest)
	{
		nn::account::CloseUser(mhUser);
	}
}

const EAUserString8& NXUser::GetUserIDAsString() const
{
	return mUserIDString;
}

EAUserID NXUser::GetUserID() const
{
	return EAUserID(mUserID._data[0], mUserID._data[1]);
}

const EAUserString16& NXUser::GetGamerTag() const
{
	return mDisplayName;
}

const EAUserString16& NXUser::GetDisplayName() const
{
	return mDisplayName;
}

UserOnlineState NXUser::GetOnlineState() const
{
	if (mbIsGuest)
	{
		return USER_STATE_OFFLINE;
	}

	bool available;
	auto result = nn::account::IsNetworkServiceAccountAvailable(&available, mhUser);
	EA_ASSERT_MESSAGE(result.IsSuccess(), "failed to check network service account availability");
	EA_UNUSED(result);

	return available ? USER_STATE_ONLINE : USER_STATE_OFFLINE;
}

bool NXUser::IsGuest() const
{
	return mbIsGuest;
}

bool NXUser::IsSignedIn() const
{
	return true;
}

nn::account::UserHandle NXUser::GetNNUserHandle() const
{
	return mhUser;
}

nn::account::Uid NXUser::GetNNUid() const
{
	return mUserID;
}

bool NXUser::IsValid() const
{
	// Yes, this is the documented way to check if a nn::account::Uid is valid.
	return static_cast<bool>(mUserID);
}

bool NXUser::Equals(const IEAUser* other) const
{
	if (!other)
	{
		return false;
	}

	const NXUser* nxUser = static_cast<const NXUser*>(other);
	return *this == *nxUser;
}

bool NXUser::operator==(const NXUser& other) const
{
	// Either this or other may be a guest, with Uid == InvalidUid.
	// So we need to check for that first before comparing the Uids.
	if (mbIsGuest != other.mbIsGuest)
	{
		return false;
	}

	if (mbIsGuest)
	{
		return other.mGuestID == mGuestID;
	}
	else
	{
		return other.mUserID == mUserID;
	}
}

void NXUser::SetOnlineState(EA::User::UserOnlineState state)
{
	// currently there's no need to force a user's online state one way or
	// the other on NX, so this function is unimplemented
	EA_ASSERT_MESSAGE(false, "unable to set user online state on NX");
}

}
}
