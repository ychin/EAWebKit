///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h>
#include <EASystemEventMessageDispatcher/Messages.h>

namespace EA
{
	namespace SEMD
	{ 
		//----------------------------------------
		// Static/global
		EA_ALIGNED(uint64_t, sEASEMDInstanceMemory[(sizeof(SystemEventMessageDispatcher) + sizeof(uint64_t) - 1) / sizeof(uint64_t)], 16); 
		SystemEventMessageDispatcher* SystemEventMessageDispatcher::spInstance = NULL;    

		SystemEventMessageDispatcher* SystemEventMessageDispatcher::GetInstance()
		{
			EA_ASSERT_MSG(spInstance, "The SystemEventMessageDispatcher must be created before retrieving an instance. We do not lazily create the instance here, since it's important that EASEMD creation is explicitly known to game teams rather than created by middleware behind the scenes.");
			return spInstance;
		}

		ISystemEventMessageDispatcher* ISystemEventMessageDispatcher::GetInstance()
		{
			return static_cast<ISystemEventMessageDispatcher*>(SystemEventMessageDispatcher::GetInstance());
		}

		SystemEventMessageDispatcher* SystemEventMessageDispatcher::CreateInstance(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator)
		{
			if(spInstance == NULL)
			{
				spInstance = new(sEASEMDInstanceMemory) SystemEventMessageDispatcher(settings, pAllocator);
			}
			else
			{
				EA_FAIL_MSG("Only one instance of the SystemEventMessageDispatcher should be created.");
			}

			return spInstance;
		}

		ISystemEventMessageDispatcher* ISystemEventMessageDispatcher::CreateInstance(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator)
		{
			return static_cast<ISystemEventMessageDispatcher*>(SystemEventMessageDispatcher::CreateInstance(settings, pAllocator));
		}

		void SystemEventMessageDispatcher::DestroyInstance()
		{
			spInstance->~SystemEventMessageDispatcher();
			spInstance = NULL;
		}

		void ISystemEventMessageDispatcher::DestroyInstance()
		{
			SystemEventMessageDispatcher::DestroyInstance();
		}

		//----------------------------------------
		SystemEventMessageDispatcherSettings::SystemEventMessageDispatcherSettings()
			: useUserService(true)
			, useSystemService(true)
			, useCompanionHttpd(false)
			, useCompanionUtil(false)
		{
		}

		//----------------------------------------
		SystemEventMessageDispatcher::SystemEventMessageDispatcher(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator)
			: mSettings(settings)
			, mpAllocator(pAllocator)
		{
			mMessageServer.SetAllocator(mpAllocator);

			EA_ASSERT_MSG(spInstance == NULL, "Only one instance of the SystemEventMessageDispatcher should be created");

			if( mMessageServer.Init() == false )
			{
				EA_FAIL_MSG("Failed to initialize the message server");
			}
		}

		//----------------------------------------
		SystemEventMessageDispatcher::~SystemEventMessageDispatcher() 
		{
			EA_ASSERT(spInstance);

			// Stop new messages from being inserted and get rid of any messages which might be lingering
			mMessageServer.ProcessQueue();

			if(mMessageServer.Shutdown() == false)
			{
				EA_FAIL_MSG("Failed to shutdown the message server");
			}
		}

		//----------------------------------------
		void SystemEventMessageDispatcher::Tick(EA::SEMD::Group groupMessage)
		{
			// Handle all messages
			mMessageServer.ProcessQueue();
		}

		//----------------------------------------
		void SystemEventMessageDispatcher::AddMessageHandler(EA::Messaging::IHandler* pHandler, Group group, bool refCounted, int nPriority)
		{
			mMessageServer.AddHandler(pHandler, group, refCounted, nPriority);
		}

		//----------------------------------------
		bool SystemEventMessageDispatcher::RemoveMessageHandler(EA::Messaging::IHandler* pHandler, Group group, int nPriority)
		{
			return mMessageServer.RemoveHandler(pHandler, group, nPriority);
		}
	}
}
