#include <EAUser/internal/Config.h>
#include <IEAUser/IEAUser.h>
#include <EAUser/NullUser.h>
#include <EAStdC/EAString.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>
#include <IEAController/IEAController.h>

namespace EA
{
namespace User
{
	NullUser::NullUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator)
		, mUserID(userID), mOnlineState(EA::User::USER_STATE_UNKNOWN)
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] GameTag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
	{
		EA_ASSERT(mpAllocator);

		char8_t buffer[32];
		EA::StdC::U64toa(mUserID, buffer, 10);

		mUserIDString = buffer;
		mGamerTag = (char16_t*) L"Null User";
		mDisplayName = (char16_t*) L"Null User";;
	}

	NullUser::~NullUser() = default;

	const EAUserString8& NullUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	uint64_t NullUser::GetUserID() const
	{
		return mUserID;
	}

	bool NullUser::IsValid() const
	{
		return true;
	}

	void NullUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	const EAUserString16& NullUser::GetGamerTag() const
	{
		return mGamerTag;
	}

	const EAUserString16& NullUser::GetDisplayName() const
	{
		return mDisplayName;
	}

	UserOnlineState NullUser::GetOnlineState() const
	{           
		return USER_STATE_UNKNOWN;
	}

	bool NullUser::IsGuest() const
	{
		return false;
	}

	bool NullUser::IsSignedIn() const
	{
		return false;
	};

	bool NullUser::Equals(const IEAUser* user) const
	{
		const NullUser* nullUser = static_cast<const EA::User::NullUser*>(user);

		if(nullUser == NULL)
		{
			return false;
		}

		return mUserID == nullUser->mUserID;
	}

	bool NullUser::operator==(NullUser const& user)
	{
		return mUserID == user.mUserID;
	}
}} //namespace EA::User
