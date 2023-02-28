///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4275)  // non dll-interface class used as base for dll-interface class.
#endif

#include "internal/Config.h"
#include <IEASystemEventMessageDispatcher/ISystemEventMessageDispatcher.h>
#include <coreallocator/icoreallocator.h>
#include <EAAssert/eaassert.h>
#include <EAMessage/EAMessage.h>
#include <EASTL/allocator.h>
#include <EASTL/list.h>
#include <EASystemEventMessageDispatcher/Messages.h>
#include <eathread/eathread_atomic.h>

namespace EA
{
	namespace SEMD
	{
		// ----------------------------------------
		#if defined(_MSC_VER) && (_MSC_VER <= 1600) // if > VS2010
			enum Group
		#else
			enum Group : int
		#endif
		{
			kGroupUserService    = 0x0000001,          // The platform specific data from Message::GetData is of type SceUserServiceEvent 
			kGroupSystemService  = 0x0000002,          // The platform specific data from Message::GetData is of type SceSystemServiceEvent
			kGroupCompanionUtil  = 0x0000004,          // The platform specific data from Message::GetData is of type SceCompanionUtilEvent
			kGroupCompanionHttpd = 0x0000008,          // The platform specific data from Message::GetData is of type SceCompanionHttpdEvent
			kGroupAll            = 0xFFFFFFF,          // Used to access all of the above

			kGroupCount          = 4
		};

		// ----------------------------------------
		struct EASEMD_API SystemEventMessageDispatcherSettings
		{
			SystemEventMessageDispatcherSettings();

			void operator|=(const SystemEventMessageDispatcherSettings& settings)
			{
				// Update this operator if the definition of SystemEventMessageDispatcherSettings has changed
				EA_COMPILETIME_ASSERT(sizeof(SystemEventMessageDispatcherSettings) == 4);

				useUserService    |= settings.useUserService;
				useSystemService  |= settings.useSystemService;
				useCompanionHttpd |= settings.useCompanionHttpd;
				useCompanionUtil  |= settings.useCompanionUtil;
			}

			bool useUserService;        // default = true
			bool useSystemService;      // default = true
			bool useCompanionHttpd;     // default = false
			bool useCompanionUtil;      // default = false
		};

		// ----------------------------------------
		// SystemEventMessageDispatcher allows game teams to a add message handlers for events the 
		// SystemEventMessageDispatcher listens for. These messages will be queued and sent to game 
		// teams when the Tick() method is called.
		class EASEMD_API SystemEventMessageDispatcher : public ISystemEventMessageDispatcher
		{
		private:
			SystemEventMessageDispatcher(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator);
		public:
			~SystemEventMessageDispatcher();

			virtual void Tick(Group messageGroup = kGroupAll);

			virtual void AddMessageHandler(EA::Messaging::IHandler* pHandler, Group group, bool refCounted, int nPriority = EA::Messaging::kPriorityNormal);

			virtual bool RemoveMessageHandler(EA::Messaging::IHandler* pHandler, Group group, int nPriority = EA::Messaging::kPriorityAny);

			virtual const SystemEventMessageDispatcherSettings& GetSettings() const { return mSettings; }

			virtual void SetSettings(const SystemEventMessageDispatcherSettings& settings, bool canTurnOffSettings = false)
			{ 
				if(canTurnOffSettings)
				{
					mSettings = settings; 
				}
				else
				{
					// overloaded |= in SystemEventMessageDispatcherSettings
					mSettings |= settings;
				}
			};

			virtual bool HasTicked() { return !mIsFirstUpdate; }

			static SystemEventMessageDispatcher* CreateInstance(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator);
			static void DestroyInstance();
			static SystemEventMessageDispatcher* GetInstance();

		private:
			
			SystemEventMessageDispatcher(const SystemEventMessageDispatcher&);
			SystemEventMessageDispatcher& operator=(const SystemEventMessageDispatcher&);

			static SystemEventMessageDispatcher*    spInstance;

			SystemEventMessageDispatcherSettings    mSettings;
			uint32_t                                mHandlerCount[kGroupCount];
			EA::Allocator::ICoreAllocator*          mpAllocator;
			EA::Messaging::Server                   mMessageServer;
			bool                                    mIsFirstUpdate;
		};
	}
}

#ifdef _MSC_VER
	#pragma warning(pop)
#endif 
