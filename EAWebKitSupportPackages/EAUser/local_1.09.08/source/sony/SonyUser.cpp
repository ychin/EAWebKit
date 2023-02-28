#include "IEAUser/IEAUser.h"
#include "EAUser/SonyUser.h"
#include "EAStdC/EAString.h"
#include "EASTL/set.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "coreallocator/icoreallocator.h"
#include "EAAssert/eaassert.h"
#include "IEAController/IEAController.h"


#include <scebase.h>
#include <user_service.h>
#include <np/np_common.h>
#include <np/np_error.h>

#if defined(EA_PLATFORM_PS4)
#include <user_service.h>
#include <scebase.h>
#include <user_service.h>
#include <np/np_common.h>
#include <np/np_error.h>
#include <libsysmodule.h>
#include <np_toolkit2.h>
namespace NpToolkit2 = sce::Toolkit::NP::V2;
#endif

#if defined(EA_PLATFORM_PS5)
#include <libsysmodule.h>
#include "np_cppwebapi.h"
using namespace sce::Np::CppWebApi::UserProfile::V1;
using namespace sce::Np::CppWebApi;
#endif

namespace EA
{
namespace User
{
#if defined(EA_PLATFORM_PS4)
	bool SonyUser::sToolkitCallbackAdded = false;
	EA::Thread::Futex SonyUser::sValidRealNameLookUpsLock;
	eastl::vector<SonyUser::UserInfoRequest> SonyUser::sRequests;
#elif defined(EA_PLATFORM_PS5)
	// These are obtained during initialization (which SonyUser can't do), so the game
	// must pass in the values (which are needed for looking up the realname).
	static sce::Np::CppWebApi::Common::LibContext* sCppWebApiLibContext = nullptr;
	static uint32_t sWebApiUserContext = 0;
#endif
	SonyUser::SonyUser(UserIdType userID, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator), mUserID(userID)
		, mIsGuest(false), mIsSignedIn(true), mOnlineState(USER_STATE_UNKNOWN)
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] Display Name String", allocator))
	{
		EA_ASSERT(mpAllocator);

		char buffer[32];
		EA::StdC::U32toa(mUserID, buffer, 10);

		mUserIDString = buffer;

		mAccountID = GetAccountId();

		mDisplayName.reserve(SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1);
		GetDisplayName();
		GetOnlineState();
	}

	SonyUser::~SonyUser() = default;

	bool SonyUser::Init()
	{
#if defined(EA_PLATFORM_PS4)
		// We load these two PRXs here for tests.
		// For games: games will have already initialized first-party libraries, and we'll
		// detect that they are already loaded.  Other required initialization will also
		// have been done, so the call to addCallback() should succeed.
		//
		// For tests, there are two cases:
		// 1. Engine.ControllerUserPairing's test will have loaded the PRXs, performed the
		//    required initialization, and the addCallback() call will succeed, allowing us
		//    to test the realname functionality.
		// 2. The Online extension will init EACUP, which calls this Init() function.
		//    In that case, the PRXs will load, and because the rest of the initialization
		//    doesn't take place, the call to addCallback() below will fail.  However,
		//    this is o.k., since the Online extension tests don't attempt to use the
		//    realname lookup functionality.
		// This is somewhat messy, but it allows both games and the tests to both execute
		// without interfering with each other.

		if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_TOOLKIT2) != SCE_SYSMODULE_LOADED)
		{
			int ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TOOLKIT2);
			if (ret < SCE_OK)
			{
				// The calling function will handle the failure.
				return false;
			}
		}
		if (sceSysmoduleIsLoaded(SCE_SYSMODULE_XML) != SCE_SYSMODULE_LOADED)
		{
			int ret = sceSysmoduleLoadModule(SCE_SYSMODULE_XML);
			if (ret < SCE_OK)
			{
				// The calling function will handle the failure.
				return false;
			}
		}

		EA_ASSERT(!sToolkitCallbackAdded);

		if (!sToolkitCallbackAdded)
		{
			NpToolkit2::Core::Request::AddCallback callbackToAdd;
			callbackToAdd.callback.callback = SceNpToolkitCallback;

			int ret = NpToolkit2::Core::addCallback(callbackToAdd);
			EA_ASSERT(ret != SCE_TOOLKIT_NP_V2_ERROR_MAX_NUM_CALLBACKS_REACHED);
			if (ret < 0)
			{
				// If the toolkit has not been initialized, this addCallback() call with fail.
				// In that case, we silently fail (this will happen in some unit tests that don't
				// initialize the toolkit), but any calls to LookupUserRealName() will assert
				// so game teams are aware they are missing initialization.
				return true;
			}
			else
			{
				sToolkitCallbackAdded = true;
				return true;
			}
		}
		return false;
#else
		return true;
#endif
	}

	bool SonyUser::Deinit()
	{
#if defined(EA_PLATFORM_PS4)
		if (sToolkitCallbackAdded)
		{
			NpToolkit2::Core::Request::RemoveCallback callbackToRemove;
			callbackToRemove.callback.callback = SceNpToolkitCallback;

			int ret = NpToolkit2::Core::removeCallback(callbackToRemove);
			if (ret < 0) {
				return false;
			}
			else
			{
				sToolkitCallbackAdded = false;
			}
		}
#endif
		return true;
	}

	uint64_t SonyUser::GetUserID() const
	{
		return mUserID;
	}

	const EAUserString8& SonyUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	bool SonyUser::IsValid() const
	{
		return mUserID != SCE_USER_SERVICE_USER_ID_INVALID;
	}

	void SonyUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	void SonyUser::SetIsSignedInState(bool isSignedIn)
	{
		mIsSignedIn = isSignedIn;
	}

	const EAUserString16& SonyUser::GetGamerTag() const
	{
		return GetDisplayName();
	}

	const EAUserString16& SonyUser::GetDisplayName() const
	{
		// we may have failed during user creation so we will try again
		if(mDisplayName.empty())
		{
			eastl::string8 tempName;
			int32_t status = 0;
			do
			{
				tempName.resize(tempName.size() + 10);
				status = sceUserServiceGetUserName(mUserID, &tempName[0], tempName.size());
			}while(status == SCE_USER_SERVICE_ERROR_BUFFER_TOO_SHORT);

			if(status != SCE_OK)
			{
				mDisplayName.clear();
			}
			else
			{
				mDisplayName.append_convert(tempName.c_str());
			}
		}

		return mDisplayName;
	}

	UserOnlineState SonyUser::GetOnlineState() const
	{
		SceNpState state;
		int32_t status = sceNpGetState(mUserID, &state);

		if(status != SCE_OK)
		{
			return (mOnlineState = USER_STATE_UNKNOWN);
		}

		switch(state)
		{
			case SCE_NP_STATE_SIGNED_IN:
				return (mOnlineState = USER_STATE_ONLINE);
			case SCE_NP_STATE_SIGNED_OUT:
				return (mOnlineState = USER_STATE_OFFLINE);
			default:
			case SCE_NP_STATE_UNKNOWN:
				return (mOnlineState = USER_STATE_UNKNOWN);
		}
	}

	bool SonyUser::IsGuest() const
	{
		EA_ASSERT_MSG(!mIsGuest, "We should always return false since Sony has no concept of a guest user from an application perspective.");
		return mIsGuest;
	}

	bool SonyUser::IsSignedIn() const
	{
		return mIsSignedIn;
	}

	bool SonyUser::Equals(const IEAUser* user) const
	{
		const SonyUser* kettleUser = static_cast<const SonyUser*>(user);

		if(user == NULL)
		{
			return false;
		}

		return mUserID == kettleUser->mUserID;
	}

	bool SonyUser::operator==(SonyUser const& user)
	{
		return mUserID == user.mUserID;
	}

#if defined(EA_PLATFORM_PS4)

	SceNpAccountId SonyUser::GetAccountId()
	{
		NpToolkit2::UserProfile::Request::GetNpProfiles getNpProfiles;
		int ret = sceNpGetAccountIdA(mUserID, &getNpProfiles.accountIds[0]);
		if (ret < 0)
			return SCE_NP_INVALID_ACCOUNT_ID;
		else
			return getNpProfiles.accountIds[0];
	}

	void SonyUser::SceNpToolkitCallback(NpToolkit2::Core::CallbackEvent* event)
	{
		switch (event->service)
		{
		case NpToolkit2::Core::ServiceType::userProfile:
		{
			if (event->response->getReturnCode() < 0)
				return;

			switch (event->apiCalled)
			{
			case NpToolkit2::Core::FunctionType::userProfileGetNpProfiles:
				{
					// These things need to happen:
					// 1. Find the item in sRequests that correspond to the ID.
					// 2. Get the realname out of the orbisUserInfo.
					// 3. Set the realname for the mUser
					// 4. dealloc the mOrbisUserInfo and set to nullptr.

					EA::Thread::AutoFutex lockScope(sValidRealNameLookUpsLock);

					auto it = sRequests.begin();
					while (it != sRequests.end())
					{
						if (it->mSonyUserInfo == (void*) event->response)
						{
							auto response = it->mSonyUserInfo->get();
							if (response->numNpProfiles > 0)
							{
								// We only ever request one profile per request.
								auto profile = response->npProfiles[0];
								auto realName = profile.personalDetails.realName;

								// Join all three parts of the realName with spaces in between.
								eastl::string fullName = JoinRealNames(
									(realName.firstName[0] != '\0' ? realName.firstName : nullptr),
									(realName.middleName[0] != '\0' ? realName.middleName : nullptr),
									(realName.lastName[0] != '\0' ? realName.lastName : nullptr));

								// Cap the length at the maximum allowed.
								size_t nameBufferSize = 3 * sce::Toolkit::NP::UserProfile::NpProfile::MAX_SIZE_VERIFIED_ACCOUNT_DISPLAY_NAME + 4;
								eastl::string fullNameCapped(fullName, 0, nameBufferSize);

								// Save off the retrieved name and mark the request as done.
								it->mUser->mRealName = fullNameCapped;
								it->mUser->mpAllocator->Free(it->mSonyUserInfo);
								it->mSonyUserInfo = nullptr;

								EA_ASSERT(it->mUser);
								RealNameCallback callback = it->mUser->mRealNameLookUpCallback;
								it->mUser->mRealNameLookUpCallback = nullptr;
								if (callback != nullptr)
									callback(it->mUser, it->mUser->mRealName.c_str());
								it = sRequests.erase(it);
								return;
							}
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		}

		default:
			break;
		}
	}

	bool SonyUser::RequestUserRealName(RealNameCallback callback) const
	{
		EA::Thread::AutoFutex lockScope(sValidRealNameLookUpsLock);

		// It is an error for games to call this method if they have not initialized the toolkit.
		// In that case SonyUser::Init() will silently fail to register the callback, but we
		// assert here to let game teams know that there is missing initialization.
		EA_ASSERT(sToolkitCallbackAdded);

		// We only allow one outstanding lookup per user.  A previous outstanding lookup
		// should be aborted before starting a new one.
		EA_ASSERT(mRealNameLookUpCallback == nullptr);
		if (mRealNameLookUpCallback != nullptr)
			return false;
		mRealNameLookUpCallback = callback;

		NpToolkit2::UserProfile::Request::GetNpProfiles request;
		request.numValidAccountIds = 1;
		request.accountIds[0] = mAccountID;

		UserInfoRequest& requestEntry = sRequests.push_back();

		using ResponseType = sce::Toolkit::NP::Core::Response<sce::Toolkit::NP::UserProfile::NpProfiles>;
		ResponseType* response = new ((ResponseType*) mpAllocator->Alloc(sizeof(ResponseType), "[EAUser] SonyUser", EA::Allocator::MEM_TEMP, 16)) ResponseType;

		requestEntry.mSonyUserInfo = response;
		requestEntry.mUser = this;

		int ret = sce::Toolkit::NP::UserProfile::getNpProfiles(request, requestEntry.mSonyUserInfo);
		if (ret >= 0)
		{
			return true;
		}
		else
		{
			mpAllocator->Free(requestEntry.mSonyUserInfo);
			sRequests.pop_back();
			return false;
		}
	}

	void SonyUser::AbortUserRealNameRequest() const
	{
		EA::Thread::AutoFutex lockScope(sValidRealNameLookUpsLock);
		mRealNameLookUpCallback = nullptr;
	}
#endif

#if defined(EA_PLATFORM_PS5)

	SceNpAccountId SonyUser::GetAccountId()
	{
		SceNpAccountId accountId = SCE_NP_INVALID_ACCOUNT_ID;
		int ret = sceNpGetAccountIdA(mUserID, &accountId);
		if (ret < 0)
			return SCE_NP_INVALID_ACCOUNT_ID;
		else
			return accountId;
	}
	
	bool SonyUser::RequestUserRealName(RealNameCallback callback) const
	{
		EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);

		// We only allow one outstanding lookup per user.  A previous outstanding lookup
		// should be aborted before starting a new one.
		EA_ASSERT(mRealNameLookUpCallback == nullptr);
		if (mRealNameLookUpCallback != nullptr)
			return false;
		mRealNameLookUpCallback = callback;

		EA_ASSERT(sCppWebApiLibContext);
		if (!sCppWebApiLibContext)
			return false;

		auto lambdaForRequest = [](const SonyUser* user, SceNpAccountId accountID) {
			Common::Transaction<Common::IntrusivePtr<GetPublicProfileResponse>> transaction;
			BasicProfileApi::ParameterToGetPublicProfile param;
			Common::IntrusivePtr<PersonalDetail> personalDetail;

			bool requestSucceeded = false;
			do {
				int ret = transaction.start(sCppWebApiLibContext);
				if (ret < 0)
					break;

				eastl::string accountIdAsString = eastl::to_string(accountID);
				ret = param.initialize(sCppWebApiLibContext, accountIdAsString.c_str());
				if (ret < 0) {
					transaction.finish();
					break;
				}

				ret = BasicProfileApi::getPublicProfile(sWebApiUserContext, param, transaction);
				param.terminate();
				if (ret < 0) {
					transaction.finish();
					break;
				}

				Common::IntrusivePtr<GetPublicProfileResponse> respPtr;
				ret = transaction.getResponse(respPtr);
				if (ret != 0)
					break;

				if (!respPtr->personalDetailIsSet())
					break;

				Common::IntrusivePtr<PersonalDetail> personalDetailPtr = respPtr->getPersonalDetail();

				const char *firstName = nullptr;
				const char *middleName = nullptr;
				const char *lastName = nullptr;

				if (personalDetailPtr->firstNameIsSet())
					firstName = personalDetailPtr->getFirstName().c_str();
				if (personalDetailPtr->middleNameIsSet())
					middleName = personalDetailPtr->getMiddleName().c_str();
				if (personalDetailPtr->lastNameIsSet())
					lastName = personalDetailPtr->getLastName().c_str();

				user->mRealName = JoinRealNames(firstName, middleName, lastName);
				requestSucceeded = true;

				transaction.finish();
			} while (false);

			EA::Thread::AutoFutex lockScope(user->mValidRealNameLookUpsLock);
			RealNameCallback callback = user->mRealNameLookUpCallback;
			if (callback != nullptr)
			{
				// If the call failed for any reason, pass nullptr as the pointer to the realname.
				callback(user, requestSucceeded ? user->mRealName.c_str() : nullptr);
			}

			user->mRealNameLookupThread->detach();
			user->mRealNameLookupThread.reset();
		};

		mRealNameLookupThread = eastl::make_unique<std::thread>(lambdaForRequest, this, mAccountID);
		return true;
	}

	void SonyUser::AbortUserRealNameRequest() const
	{
		EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);
		mRealNameLookUpCallback = nullptr;
	}

	void SonyUser::SetWebApiContexts(sce::Np::CppWebApi::Common::LibContext* libContext, uint32_t userContext)
	{
		sCppWebApiLibContext = libContext;
		sWebApiUserContext = userContext;
	}
#endif

	eastl::string SonyUser::JoinRealNames(const char* firstName, const char* middleName, const char* lastName)
	{
		// Join all three parts of the realName with spaces in between.
		eastl::string fullName;
		bool needSpace = false;
		if (firstName) {
			fullName.append(firstName);
			needSpace = true;
		}
		if (middleName) {
			if (needSpace)
				fullName.append(" ");
			fullName.append(middleName);
			needSpace = true;
		}
		if (lastName) {
			if (needSpace)
				fullName.append(" ");
			fullName.append(lastName);
		}

		return fullName;
	}
}} //namespace EA::User
