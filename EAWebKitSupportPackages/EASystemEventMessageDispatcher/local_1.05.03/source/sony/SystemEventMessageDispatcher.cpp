///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h>
#include <EASystemEventMessageDispatcher/Messages.h>
#include <EAStdC/EABitTricks.h>

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
	#include <companion_httpd.h>
	#include <companion_util.h>
#endif
#include <libsysmodule.h>
#include <scebase.h>
#include <system_service.h>
#include <user_service.h>

namespace EA
{
	namespace SEMD
	{ 
#       define HANDLE_ERROR_CASE(x) case x: EA_FAIL_FORMATTED( (#x " (0x%08X)\n", error) ); break;

		//----------------------------------------
		void HandleSysModuleError(int error)
		{
			EA_UNUSED(error);

#if EA_IS_ENABLED(EASEMD_DEBUG)
			switch(error)
			{
			HANDLE_ERROR_CASE(SCE_SYSMODULE_ERROR_INVALID_VALUE)
			HANDLE_ERROR_CASE(SCE_SYSMODULE_ERROR_UNLOADED)
			HANDLE_ERROR_CASE(SCE_SYSMODULE_ERROR_FATAL)
			HANDLE_ERROR_CASE(SCE_SYSMODULE_LOADED)
			
			default:                                                EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
			}
#endif
		}

		//----------------------------------------
		void HandleUserServiceError(int error)
		{
			EA_UNUSED(error);

#if EA_IS_ENABLED(EASEMD_DEBUG)
			switch(error)
			{
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_INTERNAL)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_NO_MEMORY)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_INVALID_ARGUMENT)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTED)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_NO_EVENT)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_NOT_LOGGED_IN)
			HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_BUFFER_TOO_SHORT)
			
			default:                                                EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
			}
#endif
		}

		//----------------------------------------
		void HandleSystemServiceError(int error)
		{
			EA_UNUSED(error);

#if EA_IS_ENABLED(EASEMD_DEBUG)
			switch(error)
			{
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_INTERNAL)
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_UNAVAILABLE)
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_PARAMETER)
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_NO_EVENT)
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_REJECTED)
			HANDLE_ERROR_CASE(SCE_SYSTEM_SERVICE_ERROR_NEED_DISPLAY_SAFE_AREA_SETTINGS)

			default:                                                        EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
			}
#endif
		}

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
			//----------------------------------------
			void HandleCompanionHttpdError(int error)
			{
				EA_UNUSED(error);

	#if EA_IS_ENABLED(EASEMD_DEBUG)
				switch(error)
				{
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_UNKNOWN)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_FATAL)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NOMEM)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_INVALID_PARAM)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_INVALID_OPERATION)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_ALREADY_INITIALIZED)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NO_EVENT)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NOT_GENERATE_RESPONSE)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_ALREADY_STARTED)
				HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NOT_STARTED)
			
				default:                                                        EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
				}
	#endif
			}


			//----------------------------------------
			void HandleCompanionUtilError(int error)
			{
				EA_UNUSED(error);

	#if EA_IS_ENABLED(EASEMD_DEBUG)
				switch(error)
				{
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_UNKNOWN)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_FATAL)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_NOMEM)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_INVALID_PARAM)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_INVALID_OPERATION)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_ALREADY_INITIALIZED)
				HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_NO_EVENT)
			
				default:                                                        EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
				}
	#endif
			}
#endif

#       undef HANDLE_ERROR_CASE


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
			, mIsFirstUpdate(true)
		{
			mMessageServer.SetAllocator(mpAllocator);

			#if !EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
				EA_ASSERT_MSG(mSettings.useCompanionHttpd == false && mSettings.useCompanionUtil == false, "You are trying to enable the companion app libraries, however this library has been built without them enabled. Please rebuild the library with 'EASystemEventMessageDispatcher.IncludeCompanionLibs=true'");
			#endif

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
		void SystemEventMessageDispatcher::Tick(Group messageGroup)
		{
			if(mIsFirstUpdate)
			{
				mIsFirstUpdate = false;
			}

			// Process all user service events
			if((messageGroup & kGroupUserService) && mSettings.useUserService && mHandlerCount[EA::StdC::CountTrailing0Bits((uint32_t)kGroupUserService)])
			{
				int ret = 0;
				SceUserServiceEvent event;
				while((ret = sceUserServiceGetEvent(&event)) != SCE_USER_SERVICE_ERROR_NO_EVENT)
				{
					if(ret == SCE_OK)
					{
						char *pMemory = (char*)mpAllocator->Alloc(sizeof(Message) + sizeof(SceUserServiceEvent), 0, EA::Allocator::MEM_TEMP); 
						void *pMessageMemory = pMemory;
						SceUserServiceEvent *pDataMemory = (SceUserServiceEvent*)(pMemory + sizeof(Message)); 
						memcpy(pDataMemory, &event, sizeof(SceUserServiceEvent));
						Message* pMessage = new(pMessageMemory) Message(mpAllocator, pDataMemory);   
						mMessageServer.MessagePost(kGroupUserService, pMessage);
					}
					else if (ret == SCE_USER_SERVICE_ERROR_NOT_INITIALIZED)
					{
						// We can't handle any messages since the system is no longer initialized so break
						break;
					}
					else
					{
						HandleUserServiceError(ret);
					}                    
				}
			}
			
			// Process all system service events
			if((messageGroup & kGroupSystemService) && mSettings.useSystemService && mHandlerCount[EA::StdC::CountTrailing0Bits((uint32_t)kGroupSystemService)])
			{
				SceSystemServiceStatus status;
				SceSystemServiceEvent event;
				int ret = sceSystemServiceGetStatus(&status);
				if((ret == SCE_OK) && (status.eventNum > 0))
				{
					for(int i = 0; i < status.eventNum; ++i)
					{
						ret = sceSystemServiceReceiveEvent(&event);
						if(ret == SCE_OK)
						{
							char *pMemory = (char*)mpAllocator->Alloc(sizeof(Message) + sizeof(SceSystemServiceEvent), 0, EA::Allocator::MEM_TEMP); 
							void *pMessageMemory = pMemory;
							SceSystemServiceEvent *pDataMemory = (SceSystemServiceEvent*)(pMemory + sizeof(Message)); 
							memcpy(pDataMemory, &event, sizeof(SceSystemServiceEvent));
							Message* pMessage = new(pMessageMemory) Message(mpAllocator, pDataMemory);   
							mMessageServer.MessagePost(kGroupSystemService, pMessage);
						}
						else if(ret != SCE_SYSTEM_SERVICE_ERROR_NO_EVENT)
						{
							HandleSystemServiceError(ret);
						}
					}
				}
			}

			#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
				// Process all companion httpd events
				if((messageGroup & kGroupCompanionHttpd) && mSettings.useCompanionHttpd && mHandlerCount[EA::StdC::CountTrailing0Bits((uint32_t)kGroupCompanionHttpd)])
				{
					SceCompanionHttpdEvent event;
					int32_t ret;

					while((ret = sceCompanionHttpdGetEvent(&event)) != SCE_COMPANION_HTTPD_ERROR_NO_EVENT)
					{
						if(ret == SCE_OK)
						{
							char *pMemory = (char*)mpAllocator->Alloc(sizeof(Message) + sizeof(SceCompanionHttpdEvent), 0, EA::Allocator::MEM_TEMP); 
							void *pMessageMemory = pMemory;
							SceCompanionHttpdEvent *pDataMemory = (SceCompanionHttpdEvent*)(pMemory + sizeof(Message)); 
							memcpy(pDataMemory, &event, sizeof(SceCompanionHttpdEvent));
							Message* pMessage = new(pMessageMemory) Message(mpAllocator, pDataMemory);   
							mMessageServer.MessagePost(kGroupCompanionHttpd, pMessage);    
						}
						else if (ret == SCE_COMPANION_HTTPD_ERROR_NOT_INITIALIZED)
						{
							// We can't handle any messages since the system is no longer initialized so break
							break;
						}
						else
						{
							HandleCompanionHttpdError(ret);
						}
					}
				}   

				// Process all companion util events
				if((messageGroup & kGroupCompanionUtil) && mSettings.useCompanionUtil && mHandlerCount[EA::StdC::CountTrailing0Bits((uint32_t)kGroupCompanionUtil)])
				{
					SceCompanionUtilEvent event;
					int32_t ret;

					while((ret = sceCompanionUtilGetEvent(&event)) != SCE_COMPANION_UTIL_ERROR_NO_EVENT)
					{
						if(ret == SCE_OK)
						{
							char *pMemory = (char*)mpAllocator->Alloc(sizeof(Message) + sizeof(SceCompanionUtilEvent), 0, EA::Allocator::MEM_TEMP); 
							void *pMessageMemory = pMemory;
							SceCompanionUtilEvent *pDataMemory = (SceCompanionUtilEvent*)(pMemory + sizeof(Message)); 
							memcpy(pDataMemory, &event, sizeof(SceCompanionUtilEvent));
							Message* pMessage = new(pMessageMemory) Message(mpAllocator, pDataMemory);   
							mMessageServer.MessagePost(kGroupCompanionUtil, pMessage);
						}
						else if (ret == SCE_COMPANION_UTIL_ERROR_NOT_INITIALIZED)
						{
							// We can't handle any messages since the system is no longer initialized so break
							break;
						}
						else
						{
							HandleCompanionUtilError(ret);
						}
					}
				}
		#endif

			// Handle all messages
			mMessageServer.ProcessQueue();
		}

		//----------------------------------------
		void SystemEventMessageDispatcher::AddMessageHandler(EA::Messaging::IHandler* pHandler, Group group, bool refCounted, int nPriority)
		{
			uint32_t max = 1 << (kGroupCount - 1);
			if(group <= max && EA::StdC::IsPowerOf2(group))
			{
				uint32_t pos = EA::StdC::CountTrailing0Bits((uint32_t)group); 
				mHandlerCount[pos]++;
				mMessageServer.AddHandler(pHandler, group, refCounted, nPriority);
			}
		}

		//----------------------------------------
		bool SystemEventMessageDispatcher::RemoveMessageHandler(EA::Messaging::IHandler* pHandler, Group group, int nPriority)
		{
			uint32_t max = 1 << (kGroupCount - 1);
			if(group <= max && EA::StdC::IsPowerOf2(group))
			{
				uint32_t pos = EA::StdC::CountTrailing0Bits((uint32_t)group); 
				mHandlerCount[pos]--;
				return mMessageServer.RemoveHandler(pHandler, group, nPriority);
			}

			return false;
		}
	}
}
