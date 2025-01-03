#pragma once

#include "IEAUser/IEAUser.h"
#include <EAUser/internal/Config.h>
#include <EASTL/string.h>
#include <EASTL/list.h>
#include <eathread/eathread_atomic.h>
#include <coreallocator/icoreallocator_interface.h>

namespace EA
{
namespace User
{
	class OSXUser;

	EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
	typedef OSXUser OSXUserAccessor;

	class OSXUser : public IEAUser
	{      
	public:
		typedef uint64_t UserIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EAUSER_API OSXUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator, OSXUserAccessor* accessor)
			: OSXUser(userID, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EAUSER_API OSXUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator);
		EAUSER_API virtual                                     ~OSXUser();

		EAUSER_API virtual const EAUserString8&                GetUserIDAsString() const;
		EAUSER_API virtual uint64_t                            GetUserID() const;

		EAUSER_API virtual const EAUserString16&               GetGamerTag() const;
		EAUSER_API virtual const EAUserString16&               GetDisplayName() const;
		EAUSER_API virtual UserOnlineState                     GetOnlineState() const;
		EAUSER_API virtual void                                SetOnlineState(UserOnlineState state);
		EAUSER_API virtual bool                                IsGuest() const;
		EAUSER_API virtual bool                                IsSignedIn() const;

		EAUSER_API virtual bool                                IsValid() const;

		EAUSER_API virtual bool                                Equals(const IEAUser* user) const;

		EAUSER_API bool                                        operator==(OSXUser const& user);

	private:
		uint64_t                                               mUserID;

		EA::User::UserOnlineState                              mOnlineState;
		EAUserString8                                          mUserIDString;
		EAUserString16                                         mGamerTag;
		EAUserString16                                         mDisplayName;
	};
} // namespace User
} // namespace EA
