#pragma once

#include "IEAUser/IEAUser.h"
#include "EAUser/internal/Config.h"
#include "EASTL/string.h"
#include "EASTL/list.h"
#include "eathread/eathread_atomic.h"
#include "coreallocator/icoreallocator.h"

namespace EA
{
namespace User
{
	class AndroidUser;

	EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
	typedef AndroidUser AndroidUserAccessor;

	class EAUSER_API AndroidUser : public IEAUser
	{
	public:
		typedef uint64_t UserIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
		AndroidUser(UserIdType id, EA::Allocator::ICoreAllocator* allocator, AndroidUserAccessor* accessor)
			: AndroidUser(id, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		AndroidUser(UserIdType id, EA::Allocator::ICoreAllocator* allocator);
		virtual                                     ~AndroidUser();

		virtual const EAUserString8&                GetUserIDAsString() const;
		virtual uint64_t                            GetUserID() const;

		virtual const EAUserString16&               GetGamerTag() const;
		virtual const EAUserString16&               GetDisplayName() const;
		virtual UserOnlineState                     GetOnlineState() const;
		virtual bool                                IsGuest() const;
		virtual bool                                IsSignedIn() const;

		virtual bool                                IsValid() const;

		virtual bool                                Equals(const IEAUser* user) const;

		bool                                        operator==(AndroidUser const& user);

	private:
		void                                        SetOnlineState(EA::User::UserOnlineState state);

		mutable bool                                mIsGuest;
		mutable bool                                mIsSignedIn;
		UserOnlineState                             mOnlineState;
		uint64_t                                    mUserID;
		EAUserString8                               mUserIDString;
		EAUserString16                              mGamerTag;
		EAUserString16                              mDisplayName;
	};
} // namespace User
} // namespace EA
