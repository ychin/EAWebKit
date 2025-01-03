#include <EAUser/internal/Config.h>
#include <IEAUser/IEAUser.h>
#include <EAUser/PCUser.h>
#include <EAStdC/EAString.h>
#include <EAAssert/eaassert.h>
#include <coreallocator/icoreallocator.h>
#include <IEAController/IEAController.h>

#if defined(EAUSER_ORIGIN_ENABLED)
#include <OriginSDK/OriginSDK.h>
#endif

namespace EA
{
namespace User
{
	PCUser::~PCUser() = default;

#if !defined(EAUSER_ORIGIN_ENABLED)

	PCUser::PCUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator)
		, mUserID(userID), mOnlineState(EA::User::USER_STATE_UNKNOWN) 
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] GameTag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
	{
		EA_ASSERT(mpAllocator);

		char buffer[32];
		EA::StdC::U64toa(mUserID, buffer, 10);

		mUserIDString = buffer;
		eastl::string bufferString = buffer;
		mGamerTag.append_convert(bufferString.data(), bufferString.length());
		mDisplayName.append_convert(bufferString.data(), bufferString.length());
	}


	void PCUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	UserOnlineState PCUser::GetOnlineState() const
	{
		return USER_STATE_UNKNOWN;
	}

	bool PCUser::IsGuest() const
	{
		return false;
	}

	bool PCUser::IsSignedIn() const
	{
		return false;
	}

#else

	PCUser::PCUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator)
		, mUserID(0)
		, mOnlineState(EA::User::USER_STATE_UNKNOWN)
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] GameTag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
		, mOriginIndex(static_cast<int>(userID))
		, mbIsGuest(false)
	{
		// Origin expects this to be an index in [0..3]. They may add more users in
		// the future, but if this cast overflowed, something has gone badly wrong.
		EA_ASSERT(mOriginIndex <= INT_MAX);

		// This is an invalid Origin index, so we're definitely a guest user
		if (mOriginIndex > 3)
		{
			mbIsGuest = true;
		}
		else
		{
			OriginProfileT profile;
			OriginHandleT handle;

			// milliseconds - this is just the default value that the Origin docs use
			const int timeout = 10000;

			// Otherwise, it's a valid Origin index, so if we have a valid user
			// signed in at that index, use their profile
			auto err = OriginGetProfileSync(mOriginIndex, timeout, &profile, &handle);
			if (err == ORIGIN_ERROR_INVALID_USER || err == ORIGIN_ERROR_SDK_NOT_INITIALIZED)
			{
				mbIsGuest = true;
			}
			else
			{
				mUserID = profile.UserId;
				mUserIDString.append_convert(eastl::to_string(mUserID).c_str());
				mDisplayName.append_convert(profile.PersonaName);
				mGamerTag.append_convert(profile.PersonaName);

				err = OriginDestroyHandle(handle);
				EA_ASSERT_FORMATTED(err == ORIGIN_SUCCESS || err == ORIGIN_ERROR_SDK_NOT_INITIALIZED, ("failed to destroy Origin handle - error %d", err));
			}
		}

		if (mbIsGuest)
		{
			// 0 is an invalid Origin user ID, so we'll cause errors if we call Origin
			// functions with it (as we should, since that's an issue somewhere in
			// either EAUser or client code).
			mUserID = 0;
			mUserIDString.sprintf("Guest %d", mOriginIndex);
			mDisplayName.sprintf(u"Guest %d", mOriginIndex);
			mGamerTag.sprintf(u"Guest %d", mOriginIndex);
		}
	}

	void PCUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		EA_ASSERT_MESSAGE(false, "unable to set Origin user's online state");
	}

	// We could register for ORIGIN_EVENT_PRESENCE events and avoid polling here
	// by caching the presence somewhere. But for now that doesn't seem
	// necessary.
	UserOnlineState PCUser::GetOnlineState() const
	{
		if (IsGuest())
		{
			return USER_STATE_OFFLINE;
		}

		int8_t online;
		auto err = OriginCheckOnline(&online);
		EA_ASSERT_FORMATTED(err == ORIGIN_SUCCESS, ("failed to check user online status with Origin - %s", OriginGetErrorInfo(err)));

		if (!online)
		{
			return USER_STATE_OFFLINE;
		}

		enumPresence presence;
		err = OriginGetPresenceSync(mUserID, &presence, nullptr, 0, nullptr, 0, nullptr, 0);
		if (err == ORIGIN_ERROR_INVALID_USER)
		{ 
			// This user has been signed out of Origin, so they aren't online any more.
			// The user object should be in the process of being removed from the CUP
			// system, but if anyone still has 
			return USER_STATE_OFFLINE;
		}

		EA_ASSERT_FORMATTED(err == ORIGIN_SUCCESS, ("failed to get user presence from Origin - %s", OriginGetErrorInfo(err)));

		if (presence == ORIGIN_PRESENCE_OFFLINE)
		{
			return USER_STATE_OFFLINE;
		}

		if (presence == ORIGIN_PRESENCE_UNKNOWN)
		{
			return USER_STATE_UNKNOWN;
		}

		// The only two states that are not considered 'online' are OFFLINE
		// and UNKNOWN - everything else is just some variation of 'online'.
		return USER_STATE_ONLINE;
	}

	bool PCUser::IsGuest() const
	{
		return mbIsGuest;
	}

	bool PCUser::IsSignedIn() const
	{
		if (IsGuest())
		{
			return true;
		}

		return OriginGetUser(mOriginIndex) == mUserID;
	}

#endif

	const EAUserString8& PCUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	uint64_t PCUser::GetUserID() const
	{
		return mUserID;
	}

	bool PCUser::IsValid() const
	{
		// There is currently no way for a PC User to be invalidated
		return true;
	}

	const EAUserString16& PCUser::GetGamerTag() const
	{
		return mGamerTag;
	}

	const EAUserString16& PCUser::GetDisplayName() const
	{
		return mDisplayName;
	}

	bool PCUser::Equals(const IEAUser* user) const
	{
		const PCUser* pcUser = static_cast<const PCUser*>(user);

		if (pcUser == NULL)
		{
			return false;
		}

		return *this == *pcUser;
	}

	bool PCUser::operator==(PCUser const& user) const
	{
		if (IsGuest() || user.IsGuest())
		{
			// if both are guests at the same Origin index, we're the same
			return IsGuest() == user.IsGuest()
				&& GetIndex() == user.GetIndex();
		}

		// otherwise, we need to check the actual Origin ID
		return GetUserID() == user.GetUserID();
	}
}} //namespace EA::User
