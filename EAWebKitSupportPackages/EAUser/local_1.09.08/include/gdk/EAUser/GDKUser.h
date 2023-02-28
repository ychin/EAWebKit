// Copyright Electronic Arts. All rights reserved.

#pragma once

#include "EAUser/internal/Config.h"
#include "eathread/eathread_atomic.h"
#include "eathread/eathread_futex.h"
#include <EASTL/unique_ptr.h>
#include "IEAUser/IEAUser.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <XUser.h>
#include <XStore.h>
#include <xsapi-c/services_c.h>

namespace EA
{
	namespace User
	{
		class GDKUser;

		EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
		typedef GDKUser GDKUserAccessor;

		class EAUSER_API GDKUser : public IEAUser
		{
		public:
			typedef XUserHandle UserIdType;

			EA_DISABLE_VC_WARNING(4996);
			EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
			EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
			GDKUser(XUserHandle user, EA::Allocator::ICoreAllocator* allocator, GDKUserAccessor* accessor)
				: GDKUser(user, allocator) {}
			EA_RESTORE_VC_WARNING();
			EA_RESTORE_CLANG_WARNING();

			// Note that this expects an XLocalUserId rather than an XUID,
			// since we still need to be able to create accounts for users
			// that aren't on XBox Live. For users with an XUID, the XUID
			// will be retrieved from XUser and returned via GetUserID().
			// For guest users, an invalid ID will be returned from
			// GetUserID().
			GDKUser(XUserHandle user, EA::Allocator::ICoreAllocator* allocator);
			virtual                                    ~GDKUser();

			virtual const EAUserString8&                GetUserIDAsString() const;
			virtual EAUserID                            GetUserID() const;

			virtual const EAUserString16&               GetGamerTag() const;
			virtual const EAUserString16&               GetDisplayName() const;
			virtual UserOnlineState                     GetOnlineState() const;
			virtual bool                                IsGuest() const;
			virtual bool                                IsSignedIn() const;
			virtual void                                SetOnlineState(EA::User::UserOnlineState state);

			virtual bool                                IsValid() const;

			virtual bool								RequestUserRealName(RealNameCallback callback) const;
			virtual void								AbortUserRealNameRequest() const;

			virtual bool								RequestGamerpic(GamerPicCallback callback) const;
			virtual void								AbortGamerpicRequest() const;
			virtual bool                                Equals(const IEAUser* user) const;
			bool                                        operator==(GDKUser const& user) const;

			XUserLocalId                                GetLocalID() const;
			XUserHandle                                 GetHandle() const;

			XStoreContextHandle							GetStoreContext() const;
			XblContextHandle							GetXboxLiveContext() const;

		private:
			void                                        Initialize(bool guest);

			bool                                        mIsGuest;
			XUserLocalId                                mLocalID;
			uint64_t                                    mXUID;
			XUserHandle                                 mHandle;
			EAUserString8                               mUserIDString;
			EAUserString16                              mDisplayName;
			EA::Controller::ControllerList              mControllerList;

            XStoreContextHandle                         mStoreContext;

			// Some of these things need to be mutable in order to support lazy initialization
			// of these systems.
            mutable XblContextHandle                    mXboxLiveContext;

            mutable eastl::string						mRealName;
			mutable eastl::unique_ptr<XAsyncBlock>		mRealNameAsync;
			mutable RealNameCallback					mRealNameLookUpCallback;
			mutable EA::Thread::Futex					mValidRealNameLookUpsLock;

			mutable uint32_t							mGamerpicDataSize = 0;
			mutable eastl::unique_ptr<uint8_t>			mGamerpicData;
			mutable eastl::unique_ptr<XAsyncBlock>		mGamerpicAsync;
			mutable GamerPicCallback					mGamerPicCallback;
			mutable EA::Thread::Futex                   mGamerpicLock;
    	};
	}
}
