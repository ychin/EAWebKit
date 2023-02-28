#pragma once

#include <IEAUser/IEAUser.h>
#include <EAUser/internal/Config.h>
#include <EASTL/string.h>
#include <EASTL/list.h>
#include <eathread/eathread_atomic.h>
#include <coreallocator/icoreallocator_interface.h>

EA_DISABLE_VC_WARNING(4275) // non-dll-interface class used as base for DLL-interface classkey 'identifier'

namespace EA
{
namespace User
{
	class PCUser;

	EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
	typedef PCUser PCUserAccessor;

	class PCUser : public IEAUser
	{
	public:
		// Origin does not have any concept of a 'guest' user, but we still need
		// some way to represent those here. So instead of using an OriginUserT
		// as the 'userID' here, we use the UserIndex from Origin (range 0..3).
		// This lets us create users based on the index, then determine whether
		// they are guests by whether or not Origin knows about a user at that
		// index.
		//
		// When Origin is not enabled, this can be basically anything - EACUP
		// conventionally creates users with auto-incremented IDs starting at
		// zero, but there's nothing in EAUser that mandates that.
		typedef uint64_t UserIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
		EAUSER_API PCUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator, PCUserAccessor* accessor)
			: PCUser(userID, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EAUSER_API PCUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator);
		EAUSER_API virtual                                     ~PCUser();

		EAUSER_API virtual const EAUserString8&                GetUserIDAsString() const;
		EAUSER_API virtual uint64_t                            GetUserID() const;

		EAUSER_API virtual const EAUserString16&               GetGamerTag() const;
		EAUSER_API virtual const EAUserString16&               GetDisplayName() const;
		EAUSER_API virtual UserOnlineState                     GetOnlineState() const;
		EAUSER_API virtual bool                                IsGuest() const;
		EAUSER_API virtual bool                                IsSignedIn() const;
		EAUSER_API void                                        SetOnlineState(UserOnlineState state);

		EAUSER_API virtual bool                                IsValid() const;

		EAUSER_API virtual bool                                Equals(const IEAUser* user) const;
		EAUSER_API bool                                        operator==(PCUser const& user) const;

		// When Origin is enabled, we track the index of the user and their
		// ID separately, since we need to know which user ID is where.
		// When Origin isn't enabled, we just use indices for the ID,
		// so we can just return the ID here.
		#if defined(EAUSER_ORIGIN_ENABLED)
		int                                                    GetIndex() const { return mOriginIndex; }
		#else
		int                                                    GetIndex() const { return static_cast<int>(mUserID); }
		#endif

	private:
		uint64_t                                               mUserID;

		EA::User::UserOnlineState                              mOnlineState;
		EAUserString8                                          mUserIDString;
		EAUserString16                                         mGamerTag;
		EAUserString16                                         mDisplayName;

	#if defined(EAUSER_ORIGIN_ENABLED)
		int                                                    mOriginIndex;
		bool                                                   mbIsGuest;
	#endif
	};
} // namespace User
} // namespace EA

EA_RESTORE_VC_WARNING() // for push done above in EA_DISABLE_VC_WARNING(4275)
