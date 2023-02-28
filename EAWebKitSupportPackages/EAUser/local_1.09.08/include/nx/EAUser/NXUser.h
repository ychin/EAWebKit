// Copyright Electronic Arts. All rights reserved.

#pragma once

#include <nn/account/account_ApiForApplications.h>

#include "EAUser/internal/Config.h"
#include "eathread/eathread_atomic.h"
#include "IEAUser/IEAUser.h"

namespace EA
{
	namespace User
	{
		class NXUser;

		EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
		typedef NXUser NXUserAccessor;

		class EAUSER_API NXUser : public IEAUser
		{
		public:
			typedef nn::account::UserHandle UserIdType;

			EA_DISABLE_VC_WARNING(4996);
			EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
			EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
			NXUser(UserIdType userHandle, EA::Allocator::ICoreAllocator* allocator, NXUserAccessor* accessor)
				: NXUser(userHandle, allocator) {}
			EA_RESTORE_VC_WARNING();
			EA_RESTORE_CLANG_WARNING();

			NXUser(UserIdType userHandle, EA::Allocator::ICoreAllocator* allocator);

			virtual                                    ~NXUser();

			virtual const EAUserString8&                GetUserIDAsString() const;
			virtual EAUserID                            GetUserID() const;

			virtual const EAUserString16&               GetGamerTag() const;
			virtual const EAUserString16&               GetDisplayName() const;
			virtual UserOnlineState                     GetOnlineState() const;
			virtual void                                SetOnlineState(UserOnlineState);
			virtual bool                                IsGuest() const;
			virtual bool                                IsSignedIn() const;

			virtual bool                                IsValid() const;

			virtual bool                                Equals(const IEAUser* user) const;
			bool                                        operator==(NXUser const& user) const;
			nn::account::UserHandle                     GetNNUserHandle() const;
			nn::account::Uid                            GetNNUid() const;

		private:
			void                                        Initialize(bool guest);

			nn::account::UserHandle                     mhUser;
			nn::account::Uid                            mUserID;

			bool                                        mbIsGuest;

			// NX doesn't provide much support for 'guest' users, in that they
			// don't have a unique Uid to distinguish them from one another.
			// So to let us tell the difference, we keep a guest ID here,
			// which is only relevant for users with 'mIsGuest == true'.
			//
			// The NN Uid for guest accounts should be nn::account::InvalidUid,
			// as it's the only Uid which is guaranteed to not correspond to a
			// valid user.
			uint32_t                                    mGuestID;
			EAUserString8                               mUserIDString;
			EAUserString16                              mDisplayName;
		};
	}
}
