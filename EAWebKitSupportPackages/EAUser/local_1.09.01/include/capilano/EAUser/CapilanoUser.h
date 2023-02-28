#pragma once

#include "IEAUser/IEAUser.h"
#include "EAUser/internal/Config.h"
#include "EASTL/string.h"
#include "EASTL/list.h"
#include "eathread/eathread_atomic.h"
#include "eathread/eathread_futex.h"
#include "coreallocator/icoreallocator.h"

EA_DISABLE_ALL_VC_WARNINGS()
	EA_DISABLE_VC_WARNING(4946) // vccorlib.h reinterpret_cast used between related classes
	EA_DISABLE_VC_WARNING(4251) // 'x' needs to have dll-interface to be used by clients of class 'y'.
	#include "Windows.Xbox.System.h"
	EA_RESTORE_VC_WARNING()
EA_RESTORE_ALL_VC_WARNINGS()

namespace EA
{
namespace User
{
	class CapilanoUser;

	EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
	typedef CapilanoUser CapilanoUserAccessor;

	class CapilanoUser : public IEAUser
	{
	public:
		typedef Windows::Xbox::System::User^ UserIdType;

		EA_DISABLE_VC_WARNING(4996);
		EA_DISABLE_CLANG_WARNING(-Wdeprecated-declarations);
		EA_DEPRECATED_MESSAGE("The user accessor constructors for user classes have been deprecated. Remove the accessor argument from your user constructor calls.")
		EAUSER_API CapilanoUser(UserIdType user, EA::Allocator::ICoreAllocator* allocator, CapilanoUser* accessor)
			: CapilanoUser(user, allocator) {}
		EA_RESTORE_VC_WARNING();
		EA_RESTORE_CLANG_WARNING();

		EAUSER_API CapilanoUser(UserIdType user, EA::Allocator::ICoreAllocator* allocator);
		EAUSER_API virtual										~CapilanoUser();

		EAUSER_API virtual const EAUserString8&					GetUserIDAsString() const;
		EAUSER_API virtual uint64_t								GetUserID() const;

		EAUSER_API virtual const EAUserString16&				GetGamerTag() const;
		EAUSER_API virtual const EAUserString16&				GetDisplayName() const;
		EAUSER_API virtual UserOnlineState						GetOnlineState() const;
		EAUSER_API virtual bool									IsGuest() const;
		EAUSER_API virtual bool									IsSignedIn() const;

		EAUSER_API virtual bool									IsValid() const;

		// Online User functions
		EAUSER_API virtual bool									RequestUserRealName(RealNameCallback callback) const;
		EAUSER_API virtual void									AbortUserRealNameRequest() const;

		EAUSER_API virtual bool									RequestGamerpic(GamerPicCallback callback) const;
		EAUSER_API virtual void									AbortGamerpicRequest() const;

		EAUSER_API virtual bool									Equals(const IEAUser* user) const;

		EAUSER_API bool											operator==(CapilanoUser const& user);
		EAUSER_API bool											operator==(Windows::Xbox::System::User^ user);

		EAUSER_API Windows::Xbox::System::User^					GetFirstPartyObject() const;
		EAUSER_API Platform::String^							GetUserXboxID() const;

		EAUSER_API void											SetOnlineState(UserOnlineState state);
		EAUSER_API void											SetUser(Windows::Xbox::System::User^ user);

	private:
		Windows::Xbox::System::User^							mUser;
		mutable bool											mIsGuest;
		mutable bool											mIsSignedIn;
		UserOnlineState											mOnlineState;
		uint64_t												mUserID;
		Platform::String^										mUserXboxID;
		EAUserString8											mUserIDString;
		EAUserString16											mGamerTag;
		EAUserString16											mDisplayName;

		mutable EA::Thread::Futex								mValidRealNameLookUpsLock;
		mutable RealNameCallback								mRealNameLookUpCallback;

		mutable uint32_t										mGamerpicBufSize = 0;
		mutable uint8_t*										mGamerpicBuffer = nullptr;
		mutable Windows::Storage::Streams::IBuffer^				mGamerpicApiBuffer;
		mutable GamerPicCallback								mGamerPicCallback;
		mutable EA::Thread::Futex								mGamerpicLock;
	};
} // namespace User
} // namespace EA
