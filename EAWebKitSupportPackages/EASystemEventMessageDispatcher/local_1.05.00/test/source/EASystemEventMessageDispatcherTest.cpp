#include <EAAssert/eaassert.h>
#include <EAMain/EAEntryPointMain.inl>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAMemory.h>
#include <IEASystemEventMessageDispatcher/ISystemEventMessageDispatcher.h>
#include <EASystemEventMessageDispatcher/Messages.h>
#include <EATest/EATest.h>
#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocator.inl>

#ifdef EA_PLATFORM_SONY

#include <app_content.h>

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
	#include <companion_httpd.h>
	#include <companion_util.h>
#endif

#include <libsysmodule.h>
#include <scebase_common.h>
#include <system_service.h>
#include <user_service.h>  
#include <sdk_version.h>

// Set default heap size to unlimited as we want to be able to allocate more than 256KB
size_t sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT;
unsigned int sceLibcHeapExtendedAlloc = 1; // Enable

#define HANDLE_ERROR_CASE(x) case x: EA_FAIL_FORMATTED( (#x " (0x%08X)\n", error) );

//----------------------------------------
void HandleSysModuleError(int error)
{
	EA_UNUSED(error);

#ifdef EA_DEBUG
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

#ifdef EA_DEBUG
	switch(error)
	{
	HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_INTERNAL)
	HANDLE_ERROR_CASE(SCE_USER_SERVICE_ERROR_NOT_INITIALIZED)
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

#ifdef EA_DEBUG
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

//----------------------------------------

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)

void HandleCompanionHttpdError(int error)
{
	EA_UNUSED(error);

#ifdef EA_DEBUG
	switch(error)
	{
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_UNKNOWN)
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_FATAL)
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NOMEM)
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_INVALID_PARAM)
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_INVALID_OPERATION)
	HANDLE_ERROR_CASE(SCE_COMPANION_HTTPD_ERROR_NOT_INITIALIZED)
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

#ifdef EA_DEBUG
	switch(error)
	{
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_UNKNOWN)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_FATAL)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_NOMEM)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_INVALID_PARAM)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_INVALID_OPERATION)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_NOT_INITIALIZED)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_ALREADY_INITIALIZED)
	HANDLE_ERROR_CASE(SCE_COMPANION_UTIL_ERROR_NO_EVENT)
			
	default:                                                        EA_FAIL_FORMATTED( ("Unhandled error (0x%08X)\n", error) );
	}
#endif
}
#endif

#undef HANDLE_ERROR_CASE

#endif

int nErrorCount;


struct UserServiceHandler : EA::Messaging::IHandler
{
	virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
	{
		EA_ASSERT(messageId == EA::SEMD::kGroupUserService);

		EA::SEMD::Message* pSEMDMessage = reinterpret_cast<EA::SEMD::Message*>(pMessage);
		EA_UNUSED(pSEMDMessage); // to avoid warnings on platforms which do not use this

#       ifdef EA_PLATFORM_SONY

		const SceUserServiceEvent* pEvent = pSEMDMessage->GetData<const SceUserServiceEvent*>();
				
		switch(pEvent->eventType)
		{
			case SCE_USER_SERVICE_EVENT_TYPE_LOGIN:
			{
				char userName[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
				int ret = sceUserServiceGetUserName( pEvent->userId, userName, sizeof(userName));
				EA_UNUSED(ret);
				EA_ASSERT(ret == SCE_OK);                
				EA::EAMain::Report("USER ADDED: %s (%" PRId64 ")\n", userName, pEvent->userId);
				return true;
			}

			case SCE_USER_SERVICE_EVENT_TYPE_LOGOUT:
			{
				EA::EAMain::Report("USER REMOVED: (%" PRId64 ")\n", pEvent->userId);
				return true;
			}

			default:
			{
				EA_FAIL_FORMATTED( ("We recieved an unhandled event: %d", pEvent->eventType) );
				return false;
			}
		}

		EA_FAIL(); // If this hit then some case in the switch(pUserServiceEvent->eventType) is not properly returning
		return false;

#       else

		// This platform has no appropriate handler 
		EA_FAIL();
		return false;

#       endif
	}
};


struct SystemServiceHandler : EA::Messaging::IHandler
{
	virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
	{
		EA_ASSERT(messageId == EA::SEMD::kGroupSystemService);

		EA::SEMD::Message* pSEMDMessage = reinterpret_cast<EA::SEMD::Message*>(pMessage);
		EA_UNUSED(pSEMDMessage); // to avoid warnings on platforms which do not use this

#       ifdef EA_PLATFORM_SONY

		const SceSystemServiceEvent* pEvent = pSEMDMessage->GetData<const SceSystemServiceEvent*>();
				
		switch(pEvent->eventType)
		{
			case SCE_SYSTEM_SERVICE_EVENT_ON_RESUME:
			{
				EA::EAMain::Report("APPLICATION RESUMED\n");
				return true;
			}

			case SCE_SYSTEM_SERVICE_EVENT_ENTITLEMENT_UPDATE:
			{
				EA::EAMain::Report("ENTITLEMENT UPDATE\n");
				return true;
			}

			default:
			{
				EA_FAIL_FORMATTED( ("We recieved an unhandled event: %d", pEvent->eventType) );
				return false;
			}
		}

		EA_FAIL(); // If this hit then some case in the switch(pUserServiceEvent->eventType) is not properly returning
		return false;

#       else

		// This platform has no appropriate handler 
		EA_FAIL();
		return false;

#       endif
	}
};


struct CompanionUtilHandler : EA::Messaging::IHandler
{
	virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
	{
		EA_ASSERT(messageId == EA::SEMD::kGroupCompanionUtil);

		EA::SEMD::Message* pSEMDMessage = reinterpret_cast<EA::SEMD::Message*>(pMessage);
		EA_UNUSED(pSEMDMessage); // to avoid warnings on platforms which do not use this

#       if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)

		const SceCompanionUtilEvent* pEvent = pSEMDMessage->GetData<const SceCompanionUtilEvent*>();
				
		switch(pEvent->event)
		{
			default:
			{
				EA_FAIL_FORMATTED( ("We recieved an unhandled event: %d", pEvent->event) );
				return false;
			}
		}

		EA_FAIL(); // If this hit then some case in the switch(pUserServiceEvent->eventType) is not properly returning
		return false;

#       else

		// This platform has no appropriate handler 
		EA_FAIL();
		return false;

#       endif
	}
};


struct CompanionHttpdHandler : EA::Messaging::IHandler
{
	virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
	{
		EA_ASSERT(messageId == EA::SEMD::kGroupCompanionHttpd);

		EA::SEMD::Message* pSEMDMessage = reinterpret_cast<EA::SEMD::Message*>(pMessage);
		EA_UNUSED(pSEMDMessage); // to avoid warnings on platforms which do not use this

#       if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)

		const SceCompanionHttpdEvent* pEvent = pSEMDMessage->GetData<const SceCompanionHttpdEvent*>();
				
		switch(pEvent->event)
		{
			default:
			{
				EA_FAIL_FORMATTED( ("We recieved an unhandled event: %d", pEvent->event) );
				return false;
			}
		}

		EA_FAIL(); // If this hit then some case in the switch(pUserServiceEvent->eventType) is not properly returning
		return false;

#       else

		// This platform has no appropriate handler 
		EA_FAIL();
		return false;

#       endif
	}
};

#ifdef EA_PLATFORM_SONY

//---------------------------------------
// Globals
#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
static void* g_pCompanionHttpdWorkMemory = 0;
static void* g_pCompanionUtilWorkMemory = 0;
#endif

//---------------------------------------
bool InitializeKettle(const EA::SEMD::SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator)
{
	// Initialize the User Service
	{
		int32_t ret = sceUserServiceInitialize(NULL);
		if(ret != SCE_OK)
		{
			HandleUserServiceError(ret);
			return false;
		}
	}
			
	// No need to initialize the System Service

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
	// Initialize the companion httpd
	{
		int32_t	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT);
		if (ret != SCE_OK)
		{
			EA_FAIL();
			return ret;
		}

		// First provide mount data
		SceAppContentInitParam initparam;
		SceAppContentBootParam bootparam;
		memset(&initparam, 0x00, sizeof(initparam));
		memset(&bootparam, 0x00, sizeof(bootparam));
		ret = sceAppContentInitialize(&initparam, &bootparam);
		if (ret != SCE_OK)
		{
			EA_FAIL();
			return ret;
		}

		SceAppContentMountPoint mount;
		memset(&mount, 0x00, sizeof(mount));

		ret = sceAppContentTemporaryDataMount2(SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_FORMAT, &mount);

		if (ret != SCE_OK)
		{
			EA_FAIL();
			return ret;
		}
		
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_COMPANION_HTTPD);
		if(ret != SCE_OK)
		{
			HandleSysModuleError(ret);
			return false;
		}

		SceCompanionHttpdOptParam2 option;
		ret = sceCompanionHttpdOptParamInitialize2(&option);
		if(ret != SCE_OK)
		{
			HandleCompanionHttpdError(ret);
			return false;
		}

		g_pCompanionHttpdWorkMemory = pAllocator->Alloc( SCE_COMPANION_HTTPD_DATALENGTH_MIN_WORK_HEAPMEMORY_SIZE, "Companion httpd work memory", EA::Allocator::MEM_PERM );
		if(g_pCompanionHttpdWorkMemory == 0)
		{
			EA_FAIL_MSG("No memory to allocate the companion httpd work memory");
			return false;
		}

		option.workMemory               = g_pCompanionHttpdWorkMemory;
		option.workMemorySize           = SCE_COMPANION_HTTPD_DATALENGTH_MIN_WORK_HEAPMEMORY_SIZE; 
		option.workThreadStackSize		= SCE_COMPANION_HTTPD_DATALENGTH_MIN_WORK_STACKMEMORY_SIZE;
		option.port						= 13000;
		option.screenOrientation		= SCE_COMPANION_HTTPD_ORIENTATION_PORTRAIT_LANDSCAPE;

		option.transceiverThreadCount	= 4;
		option.transceiverStackSize		= SCE_COMPANION_HTTPD_DATALENGTH_MIN_TRANSCEIVER_STACKMEMORY_SIZE;
		option.workDirectory			= mount.data;

		ret = sceCompanionHttpdInitialize2(&option);
		if(ret != SCE_OK)
		{
			HandleCompanionUtilError(ret);
			return false;    
		}
	}

	// Initialize the companion util
	{
		int32_t ret = sceSysmoduleLoadModule(SCE_SYSMODULE_COMPANION_UTIL);
		if(ret != SCE_OK)
		{
			HandleSysModuleError(ret);
			return false;
		}

		SceCompanionUtilOptParam option;
		ret = sceCompanionUtilOptParamInitialize(&option);
		if(ret != SCE_OK)
		{
			HandleCompanionUtilError(ret);
			return false;
		}

		g_pCompanionUtilWorkMemory = pAllocator->Alloc( SCE_COMPANION_UTIL_DATALENGTH_MIN_WORKMEMORY_SIZE, "Companion util work memory", EA::Allocator::MEM_PERM );
		if(g_pCompanionUtilWorkMemory == 0)
		{
			EA_FAIL_MSG("No memory to allocate the companion util work memory");
			return false;
		}

		option.workMemory = g_pCompanionUtilWorkMemory;
		option.workMemorySize = SCE_COMPANION_UTIL_DATALENGTH_MIN_WORKMEMORY_SIZE; 

		ret = sceCompanionUtilInitialize(&option);
		if(ret != SCE_OK)
		{
			HandleCompanionUtilError(ret);
			return false;    
		}
	}
#endif

	return true;
}

//---------------------------------------
bool ShutdownKettle(const EA::SEMD::SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator)
{
	// Shutdown the user service
	{
		int32_t ret = sceUserServiceTerminate();
		if(ret != SCE_OK)
		{
			HandleUserServiceError(ret);
			return false;
		}
	}

#if EA_IS_ENABLED(EASEMD_USE_COMPANION_LIBS)
	// Shutdown the companion httpd
	{
		int32_t ret = sceCompanionHttpdUnregisterRequestCallback();
		if (ret != SCE_OK)
		{
			HandleCompanionHttpdError(ret);
			return false;
		}

		ret = sceCompanionHttpdTerminate();
		if (ret != SCE_OK)
		{
			HandleCompanionHttpdError(ret);
			return false;
		}

		if( g_pCompanionHttpdWorkMemory != 0 )
		{
			pAllocator->Free( g_pCompanionHttpdWorkMemory );
			g_pCompanionHttpdWorkMemory = 0;
		}

		ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_COMPANION_HTTPD);
		if(ret != SCE_OK)
		{
			HandleSysModuleError(ret);
			return false;
		}
	}

	// Shutdown the companion util
	{
		int32_t ret = sceCompanionUtilTerminate();
		if(ret != SCE_OK)
		{
			HandleCompanionUtilError(ret);
			return false;
		}

		if( g_pCompanionUtilWorkMemory != 0  )
		{
			pAllocator->Free( g_pCompanionUtilWorkMemory );
			g_pCompanionUtilWorkMemory = 0;
		}

		ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_COMPANION_UTIL);
		if(ret != SCE_OK)
		{
			HandleSysModuleError(ret);
			return false;
		}
	}
#endif

	return true;
}

#endif

/// TestISystemEventMessageDispatcher
/// 
/// Just references ISystemEventMessageDispatcher to check for syntax errors (as it is only an interface)
int TestISystemEventMessageDispatcher()
{
	EA::SEMD::ISystemEventMessageDispatcher* pSEMD = NULL;
	EA_UNUSED(pSEMD);

	return EA::UnitTest::kTestResultOK; //Always passes for now
}

//---------------------------------------
int EAMain(int argc, char** argv)
{
	nErrorCount = 0;

	EA::EAMain::PlatformStartup();   

	EA::Allocator::ICoreAllocator* pAllocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();

	EA::UnitTest::TestApplication testSuite("IEASystemEventMessageDispatcher Unit Tests", argc, argv);
	testSuite.AddTest("Interface", TestISystemEventMessageDispatcher);
	nErrorCount = testSuite.Run();

	EA::SEMD::SystemEventMessageDispatcherSettings settings;
	
#ifdef EA_PLATFORM_SONY
	InitializeKettle(settings, pAllocator);
#endif

	// Initialize the SystemEventMessageDispatcher
	EA::SEMD::ISystemEventMessageDispatcher* pSystemEventMessageDispatcher = EA::SEMD::ISystemEventMessageDispatcher::CreateInstance(settings, pAllocator);

	// Test changing the settings
	EA::SEMD::SystemEventMessageDispatcherSettings originalSettings = pSystemEventMessageDispatcher->GetSettings();
	EA::SEMD::SystemEventMessageDispatcherSettings newSettings = originalSettings;
	EATEST_VERIFY_MSG(EA::StdC::Memcmp(&originalSettings, &settings, sizeof(EA::SEMD::SystemEventMessageDispatcherSettings)) == 0, "GetSettings() did not return the expected settings");

	newSettings.useCompanionUtil = true; // turn on
	newSettings.useUserService = false;  // turn off

	// without passing "canTurnOffSettings == true" we should only turn on the CompanionUtil and not turn off user service
	pSystemEventMessageDispatcher->SetSettings(newSettings);
	settings = pSystemEventMessageDispatcher->GetSettings();
	EATEST_VERIFY_MSG(settings.useCompanionUtil, "SetSettings() failed to enable the companionUtil service");
	EATEST_VERIFY_MSG(settings.useUserService, "SetSettings() should have failed to disable the User Service");

	pSystemEventMessageDispatcher->SetSettings(newSettings, true);
	settings = pSystemEventMessageDispatcher->GetSettings();
	EATEST_VERIFY_MSG(settings.useUserService == false, "SetSettings() did not turn off the expected services");
	EATEST_VERIFY_MSG(EA::StdC::Memcmp(&newSettings, &settings, sizeof(EA::SEMD::SystemEventMessageDispatcherSettings)) == 0, "SetSettings() did not turn off the expected services");

	// Set settings back to what they were
	pSystemEventMessageDispatcher->SetSettings(originalSettings, true);

	// -----
	// Add our event handlers
	UserServiceHandler userServiceHandler;
	pSystemEventMessageDispatcher->AddMessageHandler(&userServiceHandler, EA::SEMD::kGroupUserService, false, 0);
	
	SystemServiceHandler systemServiceHandler;
	pSystemEventMessageDispatcher->AddMessageHandler(&systemServiceHandler, EA::SEMD::kGroupSystemService, false, 0);

	CompanionUtilHandler companionUtilHandler;
	pSystemEventMessageDispatcher->AddMessageHandler(&companionUtilHandler, EA::SEMD::kGroupCompanionUtil, false, 0);

	CompanionUtilHandler companionHttpdHandler;
	pSystemEventMessageDispatcher->AddMessageHandler(&companionHttpdHandler, EA::SEMD::kGroupCompanionHttpd, false, 0);


	EA::EAMain::Report("\n\n::::EASystemEventMessageDispatcher Test::::\n");

	const size_t testRunTime = 60; // seconds
	EA::EAMain::Report("::::Running for %d second(s)::::\n\n", testRunTime);


	EA::StdC::Stopwatch stopwatch;
	stopwatch.SetUnits(EA::StdC::Stopwatch::kUnitsSeconds);
	stopwatch.Start();

	while(stopwatch.GetElapsedTime() < testRunTime)
	{
		pSystemEventMessageDispatcher->Tick();
	}

	stopwatch.Stop();
	EA::EAMain::Report("\n\n::::Shutting test down...::::\n");
   
#   ifdef EA_PLATFORM_SONY
		ShutdownKettle(settings, pAllocator);
#   endif

	EA::SEMD::ISystemEventMessageDispatcher::DestroyInstance();

	EA::EAMain::Report("\n\n::::Finished!::::\n");
	EA::EAMain::PlatformShutdown(nErrorCount);

	return nErrorCount;
}
