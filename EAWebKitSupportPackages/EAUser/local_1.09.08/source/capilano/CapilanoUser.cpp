#include "IEAUser/IEAUser.h"
#include "EAUser/CapilanoUser.h"
#include "EABase/eabase.h"
#include "EASTL/set.h"
#include "EASTL/string.h"
#include "EASTL/internal/config.h"
#include "EAStdC/EAString.h"
#include "coreallocator/icoreallocator.h"
#include "EAAssert/eaassert.h"
#include "IEAController/IEAController.h"


// From UserManager
#include <ppltasks.h>
#include <robuffer.h>
#include <wrl/client.h>

using namespace Concurrency;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Presence;
using namespace Microsoft::Xbox::Services::Social;
using namespace Windows::Foundation;
using namespace Windows::Xbox::ApplicationModel::Core;
using namespace Windows::Xbox::System;

namespace EA
{
namespace User
{
	CapilanoUser::CapilanoUser(UserIdType user, EA::Allocator::ICoreAllocator* allocator)
		: IEAUser(allocator)
		, mIsGuest(false), mIsSignedIn(false)
		, mOnlineState(EA::User::USER_STATE_UNKNOWN), mUserID(0), mUserXboxID()
		, mUserIDString(EA::Allocator::EASTLICoreAllocator("[EAUser] User ID String", allocator))
		, mGamerTag(EA::Allocator::EASTLICoreAllocator("[EAUser] Gamer Tag String", allocator))
		, mDisplayName(EA::Allocator::EASTLICoreAllocator("[EAUser] DisplayName String", allocator))
	{
		EA_ASSERT(mpAllocator);
		SetUser(user);
	}

	CapilanoUser::~CapilanoUser()
	{
		AbortUserRealNameRequest();
		AbortGamerpicRequest();
	}

	Windows::Xbox::System::User^ CapilanoUser::GetFirstPartyObject() const
	{
		return mUser;
	}

	const EAUserString8& CapilanoUser::GetUserIDAsString() const
	{
		return mUserIDString;
	}

	uint64_t CapilanoUser::GetUserID() const
	{
		return mUserID;
	}

	Platform::String^ CapilanoUser::GetUserXboxID() const
	{
		return mUserXboxID;
	}

	bool CapilanoUser::IsValid() const
	{
		bool valid = false;
		try
		{
			// This function reports SignedIn Users and Sponsored Users as valid. Unknown Users (not signed in and not sponsored) as invalid.
			valid = mUser->IsSignedIn;
		}
		catch(Platform::Exception^ e)
		{
			
		}
		return valid;
	}

	const EAUserString16& CapilanoUser::GetGamerTag() const
	{
		return mGamerTag;
	}

	const EAUserString16& CapilanoUser::GetDisplayName() const
	{
		return mDisplayName;
	}

	UserOnlineState CapilanoUser::GetOnlineState() const
	{
		return mOnlineState;
	}

	bool CapilanoUser::IsGuest() const
	{
		return mIsGuest;
	}

	bool CapilanoUser::IsSignedIn() const
	{
		return mIsSignedIn;
	}

	void CapilanoUser::SetOnlineState(EA::User::UserOnlineState state)
	{
		mOnlineState = state;
	}

	void CapilanoUser::SetUser(Windows::Xbox::System::User^ user)
	{
		if(user)
		{
			char buffer[32];
			EA::StdC::U32toa(user->Id, buffer, 10);

			mUserIDString = buffer;
			mUserID = user->Id;
			mUserXboxID = user->XboxUserId;

            mGamerTag = reinterpret_cast<const char16_t*>(user->DisplayInfo->Gamertag->Data());
            mDisplayName = reinterpret_cast<const char16_t*>(user->DisplayInfo->GameDisplayName->Data());
			mIsGuest = user->IsGuest;
			mIsSignedIn= user->IsSignedIn;

			switch(user->OnlineState)
			{
				case Windows::Xbox::System::UserOnlineState::Online:
				{
					mOnlineState = USER_STATE_ONLINE;
					break;
				}
				case Windows::Xbox::System::UserOnlineState::Offline:
				{
					mOnlineState = USER_STATE_OFFLINE;
					break;
				}
				default:
				case Windows::Xbox::System::UserOnlineState::Unknown:
				{
					mOnlineState = USER_STATE_UNKNOWN;
					break;
				}
			}

			mUser = user;
		}
		else
		{
			mUser = nullptr;
		}
	}

	bool CapilanoUser::Equals(const IEAUser* user) const
	{
		try
		{
			if(user == NULL)
			{
				return false;
			}

			const CapilanoUser* capsUser = static_cast<const CapilanoUser*>(user);
			return mUserID == capsUser->GetUserID();
		}
		catch(Platform::Exception^ e)
		{
		}

		return false;
	}

	bool CapilanoUser::operator==(CapilanoUser const& user)
	{
		try
		{
			return mUserID == user.GetUserID();
		}
		catch(Platform::Exception^ e)
		{
		}

		return false;
	}

	bool CapilanoUser::operator==(Windows::Xbox::System::User^ user)
	{
		try
		{
			return mUserID == user->Id;
		}
		catch(Platform::Exception^ e)
		{
		}

		return false;
	}

	bool CapilanoUser::RequestUserRealName(RealNameCallback callback) const
	{
		{
			EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);

			// We only allow one outstanding lookup per user.  A previous outstanding lookup
			// should be aborted before starting a new one.
			EA_ASSERT(mRealNameLookUpCallback == nullptr);
			if (mRealNameLookUpCallback != nullptr)
				return false;
			mRealNameLookUpCallback = callback;
		}
		
		wchar_t xuidBuffer[256];
		_ui64tow(mUserID, xuidBuffer, 10);
		::Platform::String^ xuidString = mUserXboxID;

		XboxLiveContext^ xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(GetFirstPartyObject());
		auto pAsyncOp = xboxLiveContext->ProfileService->GetUserProfileAsync(xuidString);

		create_task(pAsyncOp).then([=](task<XboxUserProfile^> resultTask)
		{
			try
			{
				RealNameCallback currentCallback;
					
				{
					EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);
					currentCallback = mRealNameLookUpCallback;
					mRealNameLookUpCallback = nullptr;
				}

				// This ALWAYS need to happen so we catch any exceptions thrown by GetUserProfileAsync
				XboxUserProfile^ result = resultTask.get();
				::Platform::String^ displayName = result->GameDisplayName;

				if (!currentCallback || displayName == nullptr || displayName->IsEmpty())
					return;

				EAUserString8 dest(EA::Allocator::EASTLICoreAllocator("[EAUser] CapilanoControllerEntry", mpAllocator, EA::Allocator::MEM_TEMP));
				const char16_t* src = reinterpret_cast<const char16_t*>(displayName->Data());
				EA::StdC::Strlcpy(dest, src);
				currentCallback(this, dest.c_str());
			}
			catch (::Platform::Exception^)
			{
			}
		});

		return true;
	}

	void CapilanoUser::AbortUserRealNameRequest() const
	{
		EA::Thread::AutoFutex lockScope(mValidRealNameLookUpsLock);
		mRealNameLookUpCallback = nullptr;
	}

	static uint8_t* getBufferBytes(Windows::Storage::Streams::IBuffer^ buffer)
	{
		if (buffer == nullptr)
			return nullptr;

		using namespace Windows::Storage::Streams;
		IUnknown* unknown = reinterpret_cast<IUnknown*>(buffer);
		Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;

		HRESULT hr = unknown->QueryInterface(_uuidof(IBufferByteAccess), &bufferByteAccess);

		if (FAILED(hr))
			return nullptr;

		byte* bytes = nullptr;
		bufferByteAccess->Buffer(&bytes);
		return bytes;
	}

	bool CapilanoUser::RequestGamerpic(GamerPicCallback callback) const
	{
		Windows::Xbox::System::User^ user = GetFirstPartyObject();
		if (user == nullptr)
		{
			EA_ASSERT(false, "Trying to get a gamerpic from a null WinRT user!");
			return false;
		}

		{
			EA::Thread::AutoFutex lockScope(mGamerpicLock);

			// We don't allow multiple outstanding lookups per user.
			EA_ASSERT(mGamerPicCallback == nullptr);
			if (mGamerPicCallback != nullptr)
				return false;
			mGamerPicCallback = callback;

			mGamerpicApiBuffer = nullptr;
			mGamerpicBuffer = nullptr;
			mGamerpicBufSize = 0;
		}

		// first get the size
		mGamerpicApiBuffer = ref new Windows::Storage::Streams::Buffer(16);
		mGamerpicApiBuffer->Length = 0;
		auto getSizeOp = user->DisplayInfo->GetGamerPictureAsync(Windows::Xbox::System::UserPictureSize::Small, mGamerpicApiBuffer);
		getSizeOp->Completed = ref new AsyncOperationCompletedHandler<GetPictureResult^>(
			[this, user](IAsyncOperation<GetPictureResult^>^ result,
				Windows::Foundation::AsyncStatus status)
		{
			if (status == Windows::Foundation::AsyncStatus::Completed)
			{
				GetPictureResult^ sizeResult = nullptr;
				try
				{
					sizeResult = result->GetResults();
				}
				catch (...)
				{
					return;
				}

				mGamerpicApiBuffer = ref new Windows::Storage::Streams::Buffer(sizeResult->RequiredBufferSize);
				auto op = user->DisplayInfo->GetGamerPictureAsync(Windows::Xbox::System::UserPictureSize::Small, mGamerpicApiBuffer);
				op->Completed = ref new AsyncOperationCompletedHandler<GetPictureResult^>(
					[this](IAsyncOperation<GetPictureResult^>^ result,
						Windows::Foundation::AsyncStatus status)
					{
						if (status == Windows::Foundation::AsyncStatus::Completed)
						{
							GetPictureResult^ pictureResult = nullptr;
							try
							{
								pictureResult = result->GetResults();
							}
							catch (...)
							{
								return;
							}

							Windows::Foundation::HResult hRes = pictureResult->Result;
							if (hRes.Value == 0) // Successfully retrieved gamerpic
							{
								GamerPicCallback currentCallback;

								{
									EA::Thread::AutoFutex lockScope(mGamerpicLock);

									uint8_t* buffer = nullptr;
									buffer = getBufferBytes(mGamerpicApiBuffer);

									currentCallback = mGamerPicCallback;
									mGamerPicCallback = nullptr;

									mGamerpicBufSize = (uint32_t)mGamerpicApiBuffer->Length;
									mGamerpicBuffer = buffer;
								}

								if (currentCallback)
									currentCallback(this, mGamerpicBuffer, mGamerpicBufSize);
							}
						}
						else
						{
							mGamerpicBuffer = nullptr;
							mGamerpicBufSize = 0;
						}
					});
			}
			else
			{ 
				mGamerpicBuffer = nullptr;
				mGamerpicBufSize = 0;
			}
		});
		return true;
	}

	void CapilanoUser::AbortGamerpicRequest() const
	{
		EA::Thread::AutoFutex lockScope(mGamerpicLock);
		mGamerPicCallback = nullptr;
	}
}} //namespace EA::User
