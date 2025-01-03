// Copyright Electronic Arts. All rights reserved.

#include <IEAUser/IEAUser.h>
#include "EAUser/internal/Config.h"

#include <eathread/eathread_atomic.h>
#include <coreallocator/icoreallocator.h>

#include <ggp/profile.h>

namespace EA
{
	namespace User
	{
		class EAUSER_API StadiaUser : public IEAUser
		{
		public:
			friend class StadiaUserAccessor;
			typedef GgpPlayerId UserIdType;

			StadiaUser(UserIdType userHandle, EA::Allocator::ICoreAllocator* allocator);
			virtual                                    ~StadiaUser();

			virtual const EAUserString8&                GetUserIDAsString() const;
			virtual EAUserID                            GetUserID() const;

			virtual const EAUserString16&               GetGamerTag() const;
			virtual const EAUserString16&               GetDisplayName() const;
			virtual UserOnlineState                     GetOnlineState() const;
			virtual void                                SetOnlineState(UserOnlineState state);
			virtual bool                                IsGuest() const;
			virtual bool                                IsSignedIn() const;
			virtual bool                                IsValid() const;

			virtual bool                                Equals(const IEAUser* user) const;
			bool                                        operator==(const StadiaUser& user) const;

		private:
			GgpPlayerId                                 mUserID;
			bool                                        mbIsGuest;

			EAUserString8                               mUserIDString;
			EAUserString16                              mDisplayName;
		};
	}
}
