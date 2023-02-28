/////////////////////////////////////////////////////////////////////////////
// EAMessageDefer.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis
//
/////////////////////////////////////////////////////////////////////////////


#include <EAMessage/internal/Config.h>

#if EAMESSAGE_DEFER_ENABLED // This #if wraps this entire file

#include <EAMessage/EAMessageDefer.h>
#include <EAMessage/EAMessage.h>
#include <EAMessage/EAMessageExtra.h>
#include <EATrace/EATrace.h>
#include <MemoryMan/MemoryMan.h>
#include <EACOM/EACOM.h>
#include <EACOM/RefCount.h>


namespace EA{

namespace Messaging{

namespace Internal{
	/// DeferredMessageInfo
	///
	/// Implements a deferred message item.
	///
	/// To do: Make a pool of these which are recycled instead of 
	/// making a pool of these objects. Alternatively, make some 
	/// kind of intrusive list of them.
	///
	struct DeferredMessageInfo : public EA::COM::RefCountImpl<EA::COM::IRefCount, EA::COM::RefCountMT>
	{
		EA::Messaging::MessageId   mId;
		EA::Messaging::IMessageRC* mpMessageRC;
		EA::Messaging::IHandlerRC* mpHandlerRC;
		EA::Messaging::Server*     mpServer;
		EA::Callback               mCallback;
	};

} // namespace Internal



// DeferredMessageCallback
//
void DeferredMessageCallback(void* pContext, uint32_t, uint32_t)
{
	Internal::DeferredMessageInfo* const pMessageInfo = (Internal::DeferredMessageInfo*)pContext;

	// We post the message in order to make sure it's delivered by the main thread and not the callback thread.
	if(pMessageInfo->mpServer)
		pMessageInfo->mpServer->MessagePost(pMessageInfo->mId, pMessageInfo->mpMessageRC, kPriorityHigh, pMessageInfo->mpHandlerRC);
	else if(pMessageInfo->mpHandlerRC)
		pMessageInfo->mpHandlerRC->HandleMessage(pMessageInfo->mId, pMessageInfo->mpMessageRC);

	if(pMessageInfo->mpMessageRC)
		pMessageInfo->mpMessageRC->Release();
	if(pMessageInfo->mpHandlerRC)
		pMessageInfo->mpHandlerRC->Release();

	pMessageInfo->Release();
}


// MessagePost
//
bool DeferredMessage::MessagePost(uint32_t nTimeDeltaMS, MessageId id, IMessageRC* pMessageRC, IHandlerRC* pHandlerRC, Server* pServer)
{
	EA_ASSERT(CallbackSystem::IsInitialized());

	// Currently we require a non-NULL server pointer, as there is no mechanism to get a 'default' server.
	// EA_ASSERT_GROUP_FORMATTED("Messaging::DeferredMessage", pServer || pHandlerRC, ("DeferredMessage::MessagePost: NULL server pointer when pHandlerRC is NULL.\n"));

	if(pServer || pHandlerRC)
	{
		if(pMessageRC)
			pMessageRC->AddRef();
		if(pHandlerRC)
			pHandlerRC->AddRef();

		Internal::DeferredMessageInfo* const pMessageInfo = EA_NEW(EAMESSAGE_ALLOC_PREFIX "Defer") Internal::DeferredMessageInfo;

		if(pMessageInfo)
		{
			pMessageInfo->AddRef();
			pMessageInfo->mId = id;
			pMessageInfo->mpMessageRC = pMessageRC;
			pMessageInfo->mpHandlerRC = pHandlerRC;
			pMessageInfo->mpServer = pServer;
			pMessageInfo->mCallback.SetFunctionInfo(DeferredMessageCallback, pMessageInfo, pMessageInfo);
			pMessageInfo->mCallback.SetPeriod(nTimeDeltaMS);
			pMessageInfo->mCallback.SetMode(Callback::kModeAsynch);
			pMessageInfo->mCallback.Start(true); // 'true' here means one-shot and not periodic.

			return true;
		}
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// ThreadSafeCallback
///////////////////////////////////////////////////////////////////////////////

const uint32_t kIdThreadSafeCallback = 0x3278ab2e;


void ThreadSafeCallbackImp(void* pContext, uint32_t nTime, uint32_t nTimeDelta)
{
	ThreadSafeCallback* const pThreadSafeCallback = (ThreadSafeCallback*)pContext;
	MessageBasicRC<2>* const pMessageBasic = EA_NEW(EAMESSAGE_ALLOC_PREFIX "Defer") MessageBasicRC<2>;
	pMessageBasic->AddRef();
	pMessageBasic->SetUint32(0, nTime);
	pMessageBasic->SetUint32(1, nTimeDelta);
	pThreadSafeCallback->mpServer->MessagePost(kIdThreadSafeCallback, pMessageBasic, kPriorityHigh, pThreadSafeCallback);
	pMessageBasic->Release();
}


ThreadSafeCallback::ThreadSafeCallback()
 :  mnRefCount(0),
	mpServer(NULL),
	mCallback(),
	mpCallbackFunction(NULL),
	mpCallbackFunctionContext(NULL)
{
	// Empty
}


ThreadSafeCallback::~ThreadSafeCallback()
{
	Stop(); // Stop any callbacks that may be occurring.
}


bool ThreadSafeCallback::Start(uint32_t nPeriodMS, bool bOneShot, EA::Callback::CallbackFunctionType pFunction, void* pFunctionContext, Server* pServer)
{
	if(pFunction && pServer)
	{
		mpServer                  = pServer;
		mpCallbackFunction        = pFunction;
		mpCallbackFunctionContext = pFunctionContext;

		mCallback.SetFunctionInfo(ThreadSafeCallbackImp, this);
		mCallback.SetPeriod(nPeriodMS);
		mCallback.Start(bOneShot);

		return true;
	}

	return false;
}


bool ThreadSafeCallback::Stop()
{
	mCallback.Stop();
	return true;
}


bool ThreadSafeCallback::HandleMessage(MessageId messageId, void* pMessage)
{
	if((messageId == kIdThreadSafeCallback) && mpCallbackFunction)
	{
		// What we ideally need to do here is fix nTime and nTimeDelta, as they are not accurate.
		MessageBasicRC<2>* const pMessageBasic = (MessageBasicRC<2>*)pMessage;
		const uint32_t nTime      = pMessageBasic->GetUint32(0);
		const uint32_t nTimeDelta = pMessageBasic->GetUint32(1);
		mpCallbackFunction(mpCallbackFunctionContext, nTime, nTimeDelta);
		return true;
	}
	return false;
}


int ThreadSafeCallback::AddRef()
{
	return (int)mnRefCount.Increment(); 
}


int ThreadSafeCallback::Release()
{
	int32_t rc = mnRefCount.Decrement();
	if(rc)
		return rc;
	mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
	EA_DELETE this;         // is called while in destructor. This can happen
	return 0;               // if the destructor does things like debug traces.
}




///////////////////////////////////////////////////////////////////////////////
// CallbackMessage
///////////////////////////////////////////////////////////////////////////////

void CallbackMessageImp(void* pContext, uint32_t nTime, uint32_t nTimeDelta)
{
	CallbackMessage* const pCallbackMessage = (CallbackMessage*)pContext;

	if((pCallbackMessage->mDelivery == kDeliverySend) || !pCallbackMessage->mpServer)
	{
		if(pCallbackMessage->mpHandlerRC)
		{
			CallbackMessage::CallbackMessageData cmd;
			cmd.SetUint32(0, nTime);
			cmd.SetUint32(1, nTimeDelta);
			pCallbackMessage->mpHandlerRC->HandleMessage(pCallbackMessage->mId, &cmd);
		}
	}
	else
	{
		if(pCallbackMessage->mpServer)
		{
			CallbackMessage::CallbackMessageData* const pCMD = EA_NEW(EAMESSAGE_ALLOC_PREFIX "Defer") CallbackMessage::CallbackMessageData;
			pCMD->SetUint32(0, nTime);
			pCMD->SetUint32(1, nTimeDelta);
			pCallbackMessage->mpServer->MessagePost(pCallbackMessage->mId, pCMD, kPriorityHigh, pCallbackMessage->mpHandlerRC);
		}
	}

	if(pCallbackMessage->mpHandlerRC)
		pCallbackMessage->mpHandlerRC->Release();
}


CallbackMessage::CallbackMessage()
  : mCallback(),
	mDelivery(kDeliveryPost),
	mId(0),
	mpHandlerRC(NULL),
	mpServer(NULL)
{
	// Empty
}


CallbackMessage::~CallbackMessage()
{
	Stop();
}


bool CallbackMessage::Start(uint32_t nPeriodMS, bool bOneShot, Delivery delivery, MessageId id, IHandlerRC* pHandlerRC, Server* pServer)
{
	EA_ASSERT(CallbackSystem::IsInitialized());

	// EA_ASSERT_GROUP_FORMATTED("Messaging::CallbackMessage", pServer || pHandlerRC, ("CallbackMessage::Start: NULL server pointer when pHandlerRC is NULL.\n"));
	if(pServer || pHandlerRC)
	{
		if(mpHandlerRC)
			mpHandlerRC->AddRef();

		mDelivery   = delivery;
		mId         = id;
		mpHandlerRC = pHandlerRC;
		mpServer    = pServer;

		mCallback.SetFunctionInfo(CallbackMessageImp, this);
		mCallback.SetPeriod(nPeriodMS);
		mCallback.Start(bOneShot);

		return true;
	}
	return false;
}


bool CallbackMessage::Stop()
{
	mCallback.Stop();
	return true;
}



} // namespace messaging

} // namespace EA


#endif // EAMESSAGE_DEFER_ENABLED








