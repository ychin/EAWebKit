// Copyright Electronic Arts. All rights reserved.

#include <EAUser/GDKUser.h>

#include <coreallocator/icoreallocator.h>
#include <EAAssert/eaassert.h>
#include <EAStdC/EAString.h>
#include <IEAController/IEAController.h>

#include <XGameRuntimeFeature.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <XNetworking.h>

#include <xsapi-c/services_c.h>

namespace EA
{
namespace User
{

GDKUser::GDKUser(XUserHandle user, EA::Allocator::ICoreAllocator* allocator)
	: IEAUser(allocator)
	, mIsGuest(false)
	// TODO(lmitchell): MS doesn't provide a 'guaranteed invalid' XUID, or if they do,
	// it's not documented anywhere. If they get back to us and give us something we
	// can safely use here, we should switch to that.
	, mXUID(static_cast<uint64_t>(-1))
	, mHandle(user)
	, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] UserID String", allocator))
	, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] Display Name String", allocator))
	, mStoreContext(nullptr)
	, mXboxLiveContext(nullptr)
{
	if(mHandle == nullptr)
		return;

	EA_MAYBE_UNUSED HRESULT result = XUserGetLocalId(mHandle, &mLocalID);
	EA_ASSERT_MESSAGE(SUCCEEDED(result), ("failed to find local ID for user"));

	result = XUserGetIsGuest(mHandle, &mIsGuest);
	EA_ASSERT_FORMATTED(SUCCEEDED(result), ("failed to check if user %d is guest", mLocalID));

	char gamertag[XUserGamertagComponentClassicMaxBytes];
	size_t length;
	result = XUserGetGamertag(mHandle, XUserGamertagComponent::Classic, XUserGamertagComponentClassicMaxBytes, gamertag, &length);
	EA_ASSERT_FORMATTED(SUCCEEDED(result), ("unable to retrieve gamertag for user %d", mLocalID));
	mDisplayName.append_convert(gamertag);

	// TODO(lmitchell): if we require consent for getting the XUID, does IsGuest() return
	// true or false?...
	if (IsGuest())
	{
		mUserIDString.sprintf("%d", mLocalID);
	}
	else
	{
		result = XUserGetId(mHandle, &mXUID);

		// TODO(lmitchell): figure out what this consent shit means...
		EA_ASSERT_FORMATTED(SUCCEEDED(result), ("user consent via system UI required - unimplemented..."));

		mUserIDString.sprintf("%d", mXUID);

		result = XStoreCreateContext(mHandle, &mStoreContext);
		EA_ASSERT_FORMATTED(SUCCEEDED(result), ("XStoreCreateContext failed: %d", result));
	}
}

GDKUser::~GDKUser()
{
	AbortUserRealNameRequest();
	AbortGamerpicRequest();

	if(mHandle)
	{
		XUserCloseHandle(mHandle);
	}

	if (mStoreContext)
	{
		XStoreCloseContextHandle(mStoreContext);
	}

	if (mXboxLiveContext)
	{
		XblContextCloseHandle(mXboxLiveContext);
	}
}

const EAUserString8& GDKUser::GetUserIDAsString() const
{
	return mUserIDString;
}

EAUserID GDKUser::GetUserID() const
{
	return mXUID;
}

XUserLocalId GDKUser::GetLocalID() const
{
	return mLocalID;
}

XUserHandle GDKUser::GetHandle() const
{
	return mHandle;
}

XStoreContextHandle GDKUser::GetStoreContext() const
{
	return mStoreContext;
}

XblContextHandle GDKUser::GetXboxLiveContext() const
{
	// XblInitialize() needs to be called before XblContextCreateHandle() to succeed.
	// Lazy init mXboxLiveContext here, so it becomes valid once XblInitialize() is called.
	if (!mXboxLiveContext)
	{
		XblContextCreateHandle(mHandle, &mXboxLiveContext);
	}

	EA_ASSERT_FORMATTED(mXboxLiveContext, ("XBoxLive may not have been initialized.  GDKUser: mXboxLiveContext==%d", mXboxLiveContext));
	return mXboxLiveContext;
}

const EAUserString16& GDKUser::GetGamerTag() const
{
	return mDisplayName;
}

const EAUserString16& GDKUser::GetDisplayName() const
{
	return mDisplayName;
}

UserOnlineState GDKUser::GetOnlineState() const
{
	if (!XGameRuntimeIsFeatureAvailable(XGameRuntimeFeature::XNetworking))
	{
		return USER_STATE_UNKNOWN;
	}

#if defined(EA_PLATFORM_GDK)
	XNetworkingConnectivityHint hint;
	XNetworkingGetConnectivityHint(&hint);
	if (hint.connectivityLevel == XNetworkingConnectivityLevelHint::InternetAccess)
	{
		return USER_STATE_ONLINE;
	}
#endif

	return USER_STATE_OFFLINE;
}

bool GDKUser::IsGuest() const
{
	return mIsGuest;
}

bool GDKUser::IsSignedIn() const
{
	if(mHandle == nullptr)
		return false;

	XUserState state;
	EA_MAYBE_UNUSED HRESULT result = XUserGetState(mHandle, &state);
	EA_ASSERT_FORMATTED(SUCCEEDED(result), ("failed to get online state for user %d", mLocalID));

	return state == XUserState::SignedIn;
}

bool GDKUser::IsValid() const
{
	// TODO(lmitchell): MS doesn't provide a 'guaranteed invalid' XUID, or if they do,
	// it's not documented anywhere. If they get back to us and give us something we
	// can safely use here, we should switch to that.
	return mXUID != static_cast<uint64_t>(-1);
}

bool GDKUser::RequestUserRealName(RealNameCallback callback) const
{
	{
		EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);

		// We don't allow multiple outstanding lookups per user.
		EA_ASSERT(mRealNameLookUpCallback == nullptr);
		if (mRealNameLookUpCallback != nullptr)
			return false;
		mRealNameLookUpCallback = callback;
	}

	mRealNameAsync = eastl::make_unique<XAsyncBlock>();
	mRealNameAsync->context = const_cast<GDKUser *>(this);
	mRealNameAsync->queue = nullptr; // use process task queue
	mRealNameAsync->callback = [](XAsyncBlock *async)
	{
		auto user = reinterpret_cast<GDKUser *>(async->context);

		XblUserProfile profile = {};
		HRESULT result = XblProfileGetUserProfileResult(async, &profile);

		{
			EA::Thread::AutoFutex lockScope(user->mValidRealNameLookUpsLock);

			RealNameCallback currentCallback;

			currentCallback = user->mRealNameLookUpCallback;
			user->mRealNameLookUpCallback = nullptr;

			if (SUCCEEDED(result))
			{
				auto gameDisplayName = profile.gameDisplayName;
				user->mRealName = profile.gameDisplayName;

				if (currentCallback)
					currentCallback(user, gameDisplayName);
			}
			else
			{
				// The existing behavior for the gamerpic doesn't have a way to notify of failure.
				// Do we need to modify the callback to note failure?
				user->mRealName = "";
			}

			user->mRealNameAsync.reset();
		}
	};

	XblContextHandle contextHandle = GetXboxLiveContext();

	HRESULT result = XblProfileGetUserProfileAsync(contextHandle, mXUID, mRealNameAsync.get());
	if (result < 0)
		return false;

	return true;
}

void GDKUser::AbortUserRealNameRequest() const
{
	EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);

	// If the AsyncBlock is not nullptr, we should call XAsyncCancel() on it.
	// The GDK callback will fire when we call XAsyncCancel(), so make sure
	// the RealNameLookUp callback is nulled before calling it.

	mRealNameLookUpCallback = nullptr;

	if (mRealNameAsync)
	{
		XAsyncCancel(mRealNameAsync.get());
		mRealNameAsync.release();
	}
}

bool GDKUser::RequestGamerpic(GamerPicCallback callback) const
{
	{
		EA::Thread::AutoFutex lockScope(mGamerpicLock);

		// We don't allow multiple outstanding lookups per user.
		EA_ASSERT(mGamerPicCallback == nullptr);
		if (mGamerPicCallback != nullptr)
			return false;
		mGamerPicCallback = callback;

		mGamerpicData.reset();
		mGamerpicDataSize = 0;
	}

	mGamerpicAsync = eastl::make_unique<XAsyncBlock>();
	mGamerpicAsync->context = const_cast<GDKUser *>(this);
	mGamerpicAsync->queue = nullptr; // use process task queue
	mGamerpicAsync->callback = [](XAsyncBlock *async)
	{
		auto user = reinterpret_cast<GDKUser *>(async->context);
		const size_t c_GamerPicBuffer = 1024 * 16;

		user->mGamerpicData.reset(new uint8_t[c_GamerPicBuffer]);
		size_t bufferFilled = 0;

		HRESULT hr = XUserGetGamerPictureResult(async, c_GamerPicBuffer, user->mGamerpicData.get(), &bufferFilled);

		{
			EA::Thread::AutoFutex lockScope(user->mGamerpicLock);

			GamerPicCallback currentCallback;

			currentCallback = user->mGamerPicCallback;
			user->mGamerPicCallback = nullptr;

			if (SUCCEEDED(hr))
			{
				user->mGamerpicDataSize = (uint32_t)bufferFilled;

				if (currentCallback)
					currentCallback(user, user->mGamerpicData.get(), user->mGamerpicDataSize);
			}
			else
			{
				// The existing behavior for the gamerpic doesn't have a way to notify of failure.
				// Do we need to modify the callback to note failure?
				user->mGamerpicDataSize = 0;
				user->mGamerpicData.reset();
			}

			user->mGamerpicAsync.reset();
		}
	};

	HRESULT result = XUserGetGamerPictureAsync(mHandle, XUserGamerPictureSize::Small, mGamerpicAsync.get());
	if (result < 0)
		return false;

	return true;
}

void GDKUser::AbortGamerpicRequest() const
{
	EA::Thread::AutoFutex lockScope(mGamerpicLock);

	// If the AsyncBlock is not nullptr, we should call XAsyncCancel() on it.
	// The GDK callback will fire when we call XAsyncCancel(), so make sure
	// the GamerPicRequest callback is nulled before calling it.

	mGamerPicCallback = nullptr;

	if (mGamerpicAsync)
	{
		XAsyncCancel(mGamerpicAsync.get());
		mGamerpicAsync.reset();
	}
}

bool GDKUser::Equals(const IEAUser* other) const
{
	if (!other)
	{
		return false;
	}

	const GDKUser* gxdkUser = static_cast<const GDKUser*>(other);
	return *this == *gxdkUser;
}

bool GDKUser::operator==(const GDKUser& other) const
{
	return XUserCompare(mHandle, other.mHandle) == 0;
}

void GDKUser::SetOnlineState(EA::User::UserOnlineState state)
{
	// TODO(lmitchell): is this actually true? I'm pretty sure we don't need it
	// anywhere but capilano anyways... and its usage is a dirty hack in EACUP...
	// so for now, unimplemented.
	EA_ASSERT_MESSAGE(false, "unable to set user online state on GDK");
}

}
}
