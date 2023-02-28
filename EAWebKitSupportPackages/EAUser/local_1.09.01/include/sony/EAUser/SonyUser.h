#pragma once

#include "IEAUser/IEAUser.h"
#include "EAUser/internal/Config.h"
#include "EASTL/set.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "EASTL/unique_ptr.h"
#include "eathread/eathread_atomic.h"
#include "eathread/eathread_futex.h"
#include "coreallocator/icoreallocator.h"
#include <user_service.h>

#if defined(EA_PLATFORM_BALIN)
#include "np_cppwebapi.h"
#include <thread>
#endif

namespace EA
{
namespace User
{
	class SonyUser;

	EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
	typedef SonyUser SonyUserAccessor;

	class EAUSER_API SonyUser : public IEAUser
	{
	public:
		typedef SceUserServiceUserId UserIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
		SonyUser(SceUserServiceUserId userID, EA::Allocator::ICoreAllocator* allocator, SonyUserAccessor* accessor)
			: SonyUser(userID, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		SonyUser(SceUserServiceUserId userID, EA::Allocator::ICoreAllocator* allocator);
		virtual                                    ~SonyUser();

		virtual const EAUserString8&                GetUserIDAsString() const;
		virtual uint64_t                            GetUserID() const;

		virtual const EAUserString16&               GetGamerTag() const;
		virtual const EAUserString16&               GetDisplayName() const;
		virtual UserOnlineState                     GetOnlineState() const;
		virtual bool                                IsGuest() const;
		virtual bool                                IsSignedIn() const;

		virtual bool                                IsValid() const;

		// Online User functions
		virtual bool								RequestUserRealName(RealNameCallback callback) const;
		virtual void								AbortUserRealNameRequest() const;

		virtual bool                                Equals(const IEAUser* user) const;
		bool                                        operator==(SonyUser const& user);

		static bool									Init();
		static bool									Deinit();
#if defined(EA_PLATFORM_PS4)
		static void									SceNpToolkitCallback(sce::Toolkit::NP::V2::Core::CallbackEvent* event);

		// True if we've added our callback to the Sony toolkit, false otherwise
		static bool									sToolkitCallbackAdded;
#elif defined(EA_PLATFORM_BALIN)
		// This must be called by the game before requesting realName lookups on Balin.
		// This info must be gained during initialization.  See the test TestSingleFloatingRealName
		// in TestControllerUserPairingPolicyLayer.cpp for an example.
		static void 								SetWebApiContexts(sce::Np::CppWebApi::Common::LibContext* libContext, uint32_t userContext);
#endif

		void                                        SetIsSignedInState(bool isSignedIn);
		void                                        SetOnlineState(EA::User::UserOnlineState state);

	private:
		SceUserServiceUserId                        mUserID;
		SceNpAccountId								mAccountID;
		mutable bool                                mIsGuest;
		mutable bool                                mIsSignedIn;
		mutable UserOnlineState                     mOnlineState;
		EAUserString8                               mUserIDString;
		mutable EAUserString16                      mDisplayName;

		SceNpAccountId GetAccountId();

		mutable eastl::string						mRealName;
		mutable RealNameCallback					mRealNameLookUpCallback;

		static eastl::string JoinRealNames(const char* firstName, const char* middleName, const char* lastName);

#if defined(EA_PLATFORM_PS4)
		struct UserInfoRequest
		{
			// This will have the results from the request if the request was successful.
			sce::Toolkit::NP::Core::Response<sce::Toolkit::NP::UserProfile::NpProfiles>* mSonyUserInfo;

			// The SonyUser corresponding to this request.
			const SonyUser* mUser;
		};

		static EA::Thread::Futex					sValidRealNameLookUpsLock;
		static eastl::vector<UserInfoRequest>		sRequests;
#elif defined(EA_PLATFORM_BALIN)
		mutable EA::Thread::Futex					mValidRealNameLookUpsLock;
		mutable eastl::unique_ptr<std::thread>		mRealNameLookupThread;
#endif
	};
} // namespace User
} // namespace EA
