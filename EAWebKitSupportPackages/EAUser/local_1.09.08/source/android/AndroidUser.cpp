#include "IEAUser/IEAUser.h"
#include "EAUser/AndroidUser.h"
#include "EABase/eabase.h"
#include "EASTL/string.h"
#include "EASTL/internal/config.h"
#include "EAStdC/EAString.h"
#include "coreallocator/icoreallocator.h"
#include "EAAssert/eaassert.h"
#include "IEAController/IEAController.h"

namespace EA
{
namespace User
{
	AndroidUser::AndroidUser(UserIdType id, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator), mIsGuest(false), mIsSignedIn(false)
		, mOnlineState(EA::User::USER_STATE_UNKNOWN), mUserID(id)
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] User ID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] Gamer Tag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
	{
		EA_ASSERT(mpAllocator);

		char8_t buffer[32];
		EA::StdC::U64toa(mUserID, buffer, 10);

		mUserIDString = buffer;
		mGamerTag.sprintf((char16_t*)L"Android User %d", mUserID);
		mDisplayName.sprintf((char16_t*)L"Android User %d", mUserID);
	}

	AndroidUser::~AndroidUser() = default;

	const EAUserString8& AndroidUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	uint64_t AndroidUser::GetUserID() const
	{
		return mUserID;
	}

	bool AndroidUser::IsValid() const
	{
		return false;
	}

	const EAUserString16& AndroidUser::GetGamerTag() const
	{
		return mGamerTag;
	}

	const EAUserString16& AndroidUser::GetDisplayName() const
	{
		return mDisplayName;
	}

	UserOnlineState AndroidUser::GetOnlineState() const
	{
		return mOnlineState;
	}

	bool AndroidUser::IsGuest() const
	{
		return mIsGuest;
	}

	bool AndroidUser::IsSignedIn() const
	{
		return mIsSignedIn;
	}

	void AndroidUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	bool AndroidUser::Equals(const IEAUser* user) const
	{
		return mUserID == user->GetUserID();
	}

	bool AndroidUser::operator==(AndroidUser const& user)
	{
		return mUserID == user.mUserID;
	}
}} //namespace EA::User
