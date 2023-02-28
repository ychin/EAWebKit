/////////////////////////////////////////////////////////////////////////////
// IEAControllerUserPairing.h
//
// Copyright (c) 2009, Electronic Arts Inc. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <IEASystemEventMessageDispatcher/Config.h>
#include <coreallocator/icoreallocator.h>
#include <EAMessage/EAMessage.h>

#if defined(EA_DLL) && defined(_MSC_VER)
	#define ISEMD_API __declspec(dllexport)
#else
	// We are not making a DLL, so this macro
	// should define to nothing in this case.
	#define ISEMD_API
#endif

namespace EA
{
namespace SEMD
{  
	#if defined(_MSC_VER) && (_MSC_VER <= 1600) // if > VS2010
		enum Group;       // kGroupAll            = 0xFFFFFFF,          // Used to access all System Messages
	#else
		enum Group : int; // kGroupAll            = 0xFFFFFFF,          // Used to access all System Messages
	#endif

	struct SystemEventMessageDispatcherSettings;

	// ----------------------------------------
	// SystemEventMessageDispatcher allows game teams to a add message handlers for events the 
	// SystemEventMessageDispatcher listens for. These messages will be queued and sent to game 
	// teams when the Tick() method is called.
	class ISystemEventMessageDispatcher
	{
	public:
		virtual ~ISystemEventMessageDispatcher() {}

		virtual void Tick(Group messageGroup = (Group)0xFFFFFFF) = 0;

		virtual void AddMessageHandler(EA::Messaging::IHandler* pHandler, Group group, bool refCounted, int nPriority = EA::Messaging::kPriorityNormal) = 0;

		virtual bool RemoveMessageHandler(EA::Messaging::IHandler* pHandler, Group group, int nPriority = EA::Messaging::kPriorityAny) = 0;

		virtual const SystemEventMessageDispatcherSettings& GetSettings() const = 0;

		virtual void SetSettings(const SystemEventMessageDispatcherSettings& settings, bool canTurnOffSettings = false) = 0;

		virtual bool HasTicked() = 0;

		static ISEMD_API ISystemEventMessageDispatcher* CreateInstance(const SystemEventMessageDispatcherSettings& settings, EA::Allocator::ICoreAllocator* pAllocator);
		static ISEMD_API void DestroyInstance();
		static ISEMD_API ISystemEventMessageDispatcher* GetInstance();
	};

}} // namespace EA::Pairing
