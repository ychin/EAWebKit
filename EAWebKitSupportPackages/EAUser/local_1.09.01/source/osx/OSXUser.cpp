#include <EAUser/internal/Config.h>
#include <IEAUser/IEAUser.h>
#include <EAUser/OSXUser.h>
#include <EAStdC/EAString.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>
#include <IEAController/IEAController.h>

namespace EA
{
namespace User
{
	OSXUser::OSXUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator), mUserID(userID), mOnlineState(EA::User::USER_STATE_UNKNOWN) 
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] GameTag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
	{
		EA_ASSERT(mpAllocator);

		char buffer[32];
		EA::StdC::U64toa(mUserID, buffer, 10);

		mUserIDString = buffer;
		mGamerTag.append_convert(buffer, EA::StdC::Strlen(buffer));
		mDisplayName.append_convert(buffer, EA::StdC::Strlen(buffer));
	}

	OSXUser::~OSXUser() = default;

	const EAUserString8& OSXUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	uint64_t OSXUser::GetUserID() const
	{
		return mUserID;
	}

	bool OSXUser::IsValid() const
	{
		// There is currently no way for a PC User to be invalidated
		return true;
	}

	void OSXUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	const EAUserString16& OSXUser::GetGamerTag() const
	{
		return mGamerTag;
	}

	const EAUserString16& OSXUser::GetDisplayName() const
	{
		return mDisplayName;
	}

	UserOnlineState OSXUser::GetOnlineState() const
	{           
		return USER_STATE_UNKNOWN;
	}

	bool OSXUser::IsGuest() const
	{
		return false;
	}

	bool OSXUser::IsSignedIn() const
	{
		return false;
	};

	bool OSXUser::Equals(const IEAUser* user) const
	{
		const OSXUser* OSXUser = static_cast<const class OSXUser*>(user);

		if(OSXUser == NULL)
		{
			return false;
		}

		return mUserID == OSXUser->mUserID;
	}

	bool OSXUser::operator==(OSXUser const& user)
	{
		return mUserID == user.mUserID;
	}

}} //namespace EA::User
