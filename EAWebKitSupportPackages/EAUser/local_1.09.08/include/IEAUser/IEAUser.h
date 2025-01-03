#pragma once

#include <EABase/eabase.h>
#include <EABase/int128.h>

#include <IEAUser/Config.h>
#include <IEAController/IEAController.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAAssert/eaassert.h>
#include <EASTL/string.h>
#include <EASTL/fixed_list.h>
#include <EASTL/intrusive_ptr.h>
#include <EASTL/core_allocator_adapter.h>
#include <eathread/eathread_atomic.h>

#if defined(EA_PLATFORM_SONY)
#if defined(EA_PLATFORM_PS4)
#include <np_toolkit2.h>
#endif
#include <np/np_common.h>
#endif

namespace EA
{
	namespace Controller
	{
		// TODO(lmitchell): deprecate this stuff. We can just give out an eastl::list of controllers...
		class IEAControllerEntry
		{
		public:
			IEAControllerEntry(EA::Controller::SmartControllerList::const_iterator iter) : mControllerIterator(iter) { }
			EAUSER_API const EA::Controller::IEAController* GetController() { return mControllerIterator->get(); }
			EA::Controller::SmartControllerList::const_iterator mControllerIterator;
		};
	}

	namespace User
	{
		// TODO(lmitchell): deprecate this.
		// This enum probably doesn't belong here. But I'm not sure where to put it instead.
		//
		// This was originally added because XB1 provided a 'User::OnlineState' property,
		// which would lead one to believe that users could be online or offline on an
		// individual basis.
		//
		// However, that property was static, and simply represented the *console*
		// connection state, returning the same value for all users. On Sony, this was
		// implemented in a way that matched that, just checking the console online
		// status.
		//
		// So this never has any valid reason to be different per-user, and is not at all
		// a property of the user. EA::User::GetOnlineState() should probably be removed,
		// as I don't believe it's used anywhere but EACUP, which can be refactored fairly
		// simply to avoid the need for it.
		enum UserOnlineState
		{
			USER_STATE_UNKNOWN = 0x0,
			USER_STATE_ONLINE  = 0x1,
			USER_STATE_OFFLINE = 0x2,
			USER_STATE_MAX
		};

		typedef eastl::basic_string<char8_t,  EA::Allocator::EASTLICoreAllocator>  EAUserString8;
		typedef eastl::basic_string<char16_t, EA::Allocator::EASTLICoreAllocator>  EAUserString16;

		#if defined(EA_PLATFORM_NX)
		typedef uint128_t EAUserID;
		#else
		typedef uint64_t EAUserID;
		#endif

		//using uint = unsigned int;
		//using u64 = uint64_t;

#if defined(EA_PLATFORM_SONY)
		using EAUserOnlineID = SceNpAccountId;
#else
		using EAUserOnlineID = uint64_t;
#endif

		class IEAUser;

		// This typedef replaces the FastDelegate-based one from the interface to UserManager. 
		// typedef FastDelegate<void(uint, const char*)> LookupUserRealNameCallback;
		using RealNameCallback = eastl::function<void(const IEAUser *, const char*)>;
		
		using GamerPicCallback = eastl::function<void(const IEAUser *, const uint8_t* buffer, uint32_t size)>;


		// Platform agnostic user class. This class gives game teams a pointer to 
		// a user object which can be uniquely identified across the system.
		// Each platform will have their own CapilanoUser/KettleUser implementation
		// which can platform specifics in platform specific code paths
		class EAUSER_API IEAUser
		{
		public:
			IEAUser(EA::Allocator::ICoreAllocator* allocator)
				: mpAllocator(allocator)
				, mControllerList(EA::Allocator::EASTLICoreAllocator("[EAUser] Controller List", allocator))
				, mnRefCount(0)
			{}

			IEAUser(const IEAUser& other) = delete;

			virtual ~IEAUser() = default;

			// Unique identifier assigned by platform
			// Caps LiveIdentity is represented here
			// Kettle's userId is stringified here from uint32_t
			virtual const EAUserString8&                GetUserIDAsString() const = 0;
			virtual EAUserID                            GetUserID() const = 0;

			virtual const EAUserString16&               GetGamerTag() const = 0;
			virtual const EAUserString16&               GetDisplayName() const = 0;

			// Returns the online state for services such as Xbox Live or PS4's NP services
			virtual UserOnlineState                     GetOnlineState() const = 0;
			virtual bool                                IsGuest() const = 0;

			// Is the user signed in on the system (this does not correspond to online play)
			virtual bool                                IsSignedIn() const = 0;

			EA::Controller::IEAControllerEntry*         GetFirstController() const
			{
				EA::Controller::SmartControllerList::const_iterator controllerIterator = mControllerList.begin();
				EA::Controller::IEAControllerEntry* pEntry = NULL;

				if (mControllerList.validate_iterator(controllerIterator) && controllerIterator != mControllerList.end())
				{
					pEntry = new((EA::Controller::IEAControllerEntry*) mpAllocator->Alloc(sizeof(EA::Controller::IEAControllerEntry), "[EAUser] IEAControllerEntry", EA::Allocator::MEM_TEMP, 16)) EA::Controller::IEAControllerEntry(controllerIterator);
				}

				return pEntry;
			}

			EA::Controller::IEAControllerEntry* GetNextController(EA::Controller::IEAControllerEntry* entry) const
			{
				EA::Controller::IEAControllerEntry* pNXEntry = (EA::Controller::IEAControllerEntry*) entry;
				++pNXEntry->mControllerIterator;

				if (mControllerList.validate_iterator(pNXEntry->mControllerIterator) && pNXEntry->mControllerIterator != mControllerList.end())
				{
					return pNXEntry;
				}
				else
				{
					pNXEntry->~IEAControllerEntry();
					mpAllocator->Free(pNXEntry);
					return NULL;
				}
			}

			void FinishGetController(EA::Controller::IEAControllerEntry* entry) const
			{
				if (entry != NULL)
				{
					EA::Controller::IEAControllerEntry* pNXEntry = (EA::Controller::IEAControllerEntry*) entry;
					pNXEntry->~IEAControllerEntry();
					mpAllocator->Free(pNXEntry);
				}
			}

			virtual bool                                IsValid() const = 0;

			// Online user functions.
			// These are only implemented on Sony and MS platforms currently, so
			// we provide default no-op implementations here for other platforms.
			virtual bool								RequestUserRealName(RealNameCallback) const { return false; }
			virtual void								AbortUserRealNameRequest() const {}

			virtual bool								RequestGamerpic(GamerPicCallback) const { return false; }
			virtual void								AbortGamerpicRequest() const {}

			virtual bool                                Equals(const IEAUser* user) const = 0;

			virtual void                                SetOnlineState(UserOnlineState state) = 0;

			int AddRef() const
			{ return mnRefCount.Increment(); }

			int Release() const
			{
				int32_t rc = mnRefCount.Decrement();
				if(rc)
				{
					return rc;
				}

				EA_ASSERT(mpAllocator);
				// Prevent double destroy if AddRef/Release is called while in destructor. This can happen
				// if the destructor does things like debug traces.
				mnRefCount.SetValue(1); 

				this->~IEAUser();
				mpAllocator->Free((void*)this);

				return 0;
			}


			IEAUser* GetUserInstance() { return this; }
			void SetUserInstance(IEAUser*) {}
			void DestroyThis() {}

			void AddController(const EA::Controller::IEAController* pController);
			void RemoveController(const EA::Controller::IEAController* pController);
			void AddController(EA::Controller::IEAControllerSmartPtr pController);
			void RemoveController(EA::Controller::IEAControllerSmartPtr pController);

		protected:
			EA::Allocator::ICoreAllocator*              mpAllocator;
			EA::Controller::SmartControllerList         mControllerList;

			// Required to be mutable because AddRef/Release are marked const.
			mutable EA::Thread::AtomicInt32             mnRefCount;
		};


		EA_DEPRECATED_MESSAGE("The user accessor classes have been deprecated in favour of accessing users directly. Please remove references to this type, either by changing to plain users or removing them entirely (if you are creating them).")
		typedef IEAUser IEAUserAccessor;

		typedef eastl::intrusive_ptr<const IEAUser> IEAUserSmartPtr;
		typedef eastl::fixed_list<const EA::User::IEAUser*, 8, true, EA::Allocator::EASTLICoreAllocator> UserList;
		typedef eastl::fixed_list<IEAUserSmartPtr, 8, true, EA::Allocator::EASTLICoreAllocator> SmartUserList;

	}// namespace User
} // namespace EA
