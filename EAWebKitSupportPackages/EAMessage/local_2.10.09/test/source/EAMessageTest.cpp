/////////////////////////////////////////////////////////////////////////////
// EAMessageTest.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include <EAMessage/internal/Config.h>
#include <EAMessage/EAMessage.h>
#include <EAMessage/EAMessageExtra.h>
EA_DISABLE_VC_WARNING(4464) // relative include path contains '..'
#include <EAMessage/../../extras/EAMessagePool.h>
EA_RESTORE_VC_WARNING()
#include <eathread/eathread_futex.h>
#include <eathread/eathread.h>
#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocator.inl>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EAStdC/EASprintf.h>
#include <EATest/EATest.h>
#include <eathread/eathread_thread.h>
#include <EAMain/EAEntryPointMain.inl>
#include <EAAssert/eaassert.h>
#include <stdio.h>
#include <new>


///////////////////////////////////
// Disabled until we get EACallback support in EAStdC completed. 
#if EA_MESSAGE_DEFER_ENABLED 
	#define EA_MESSAGE_DEFER_ENABLED 0
#endif

#if EA_MESSAGE_DEFER_ENABLED // && (EASTDC_VERSION_N >= ?)
	#include <EAStdC/EACallback.h>
	#include <EAMessage/EAMessageDefer.h>
#endif
///////////////////////////////////

EA_DISABLE_VC_WARNING(4355) // 'this' : used in base member initializer list


using namespace EA::Messaging;
using namespace EA::Thread;


const size_t kMaxMessageCount = 32768;
Futex        gMutex;


///////////////////////////////////////////////////////////////////////////////
// Required by EASTL.
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t*  pFormat, va_list arguments)
{
	return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
	return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

#if (EASTDC_VERSION_N >= 10600)
	int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}
#endif

#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
	int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}
#endif

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Rand
//
EA::UnitTest::RandGenT<eastl_size_t> gRandInstance(12345);



///////////////////////////////////////////////////////////////////////////////
// CustomCoreAllocator
//
class CustomCoreAllocator : public EA::Allocator::ICoreAllocator 
{
public:
	CustomCoreAllocator()
	  : mAllocationCount(0), mFreeCount(0)
	{ }

	~CustomCoreAllocator()
	{ }

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/)
	{
		mAllocationCount++;
		void* pMemory = malloc(size);
		return pMemory;
	}

	void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/, 
				unsigned /*align*/, unsigned /*alignOffset*/)
	{
		mAllocationCount++;
		void* pMemory = malloc(size);
		return pMemory;
	}

	void Free(void* block, size_t /*size*/)
	{
		mFreeCount++;
		free(block);
	}

	void Reset()
	{
		mAllocationCount = 0;
		mFreeCount = 0;
	}

public:
	int32_t mAllocationCount;
	int32_t mFreeCount;
};

CustomCoreAllocator gCustomCoreAllocator;



///////////////////////////////////////////////////////////////////////////////
// MessageHistoryEntry
//
struct MessageHistoryEntry
{
	MessageId mId;
	IHandler* mpHandler;

	MessageHistoryEntry(MessageId id, IHandler* pHandler) : mId(id), mpHandler(pHandler) {}
};

eastl::vector<MessageHistoryEntry> gMessageHistoryEntryArray;



///////////////////////////////////////////////////////////////////////////////
// VerifyReceivedMessages
//
static int VerifyReceivedMessages(const MessageId* pMessageIdArrayExpected, MessageId* pMessageIdArrayActual, bool bOrderSensitive = true)
{
	int nErrorCount = 0;

	if(bOrderSensitive)
	{
		while(*pMessageIdArrayExpected)
		{
			if(!*pMessageIdArrayActual)
			{
				nErrorCount++;
				break;
			}

			EATEST_VERIFY(*pMessageIdArrayExpected == *pMessageIdArrayActual);

			pMessageIdArrayExpected++;
			pMessageIdArrayActual++;
		}
	}
	else
	{
		MessageId* const pMessageIdArrayActualEnd = eastl::find(pMessageIdArrayActual, pMessageIdArrayActual + kMaxMessageCount, (MessageId)0);

		while(*pMessageIdArrayExpected)
		{
			MessageId* const pResult = eastl::find(pMessageIdArrayActual, pMessageIdArrayActualEnd, *pMessageIdArrayExpected);

			EATEST_VERIFY(pResult != pMessageIdArrayActualEnd);
			if(pResult != pMessageIdArrayActualEnd)
				*pResult = kMessageIdNone;

			pMessageIdArrayExpected++;
		}
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// Widget
//
struct Widget
{
	Widget() : mHandlerMemberFunction(), mnReceivedCount(0)
	{
		mHandlerMemberFunction.Assign(this, &Widget::HandleMessage);
		memset(mReceivedMessages, 0, sizeof(mReceivedMessages));
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		if((mnReceivedCount + 1) < kMaxMessageCount)
			mReceivedMessages[mnReceivedCount++] = id;
		return true;
	}

	HandlerMemberFunction<Widget> mHandlerMemberFunction;
	MessageId mReceivedMessages[kMaxMessageCount];
	size_t    mnReceivedCount;
};


///////////////////////////////////////////////////////////////////////////////
// Badger
//
struct Badger : public IHandler
{
	Badger() : mnReceivedCount(0), mnPriority(0)
	{
		memset(mReceivedMessages, 0, sizeof(mReceivedMessages));
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		if((mnReceivedCount + 1) < kMaxMessageCount)
			mReceivedMessages[mnReceivedCount++] = id;
		gMutex.Lock();
		gMessageHistoryEntryArray.push_back(MessageHistoryEntry(id, this));
		gMutex.Unlock();
		return true;
	}

	MessageId mReceivedMessages[kMaxMessageCount];
	size_t    mnReceivedCount;
	int       mnPriority;
};


///////////////////////////////////////////////////////////////////////////////
// Dagger
//
struct Dagger : public IHandlerRC
{
	Dagger() : mnRefCount(0), mnReceivedCount(0), mnPriority(0)
	{
		memset(mReceivedMessages, 0, sizeof(mReceivedMessages));
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		if((mnReceivedCount + 1) < kMaxMessageCount)
			mReceivedMessages[mnReceivedCount++] = id;
		gMutex.Lock();
		gMessageHistoryEntryArray.push_back(MessageHistoryEntry(id, this));
		gMutex.Unlock();
		return true;
	}

	int AddRef()
	{
		return mnRefCount.Increment();
	}

	int Release()
	{
		int32_t rc = mnRefCount.Decrement();
		if(rc)
			return rc;
		mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
		EA_DELETE this;         // is called while in destructor. This can happen
		return 0;               // if the destructor does things like debug traces.
	}

	AtomicInt32 mnRefCount;
	MessageId   mReceivedMessages[kMaxMessageCount];
	size_t      mnReceivedCount;
	int         mnPriority;
};


///////////////////////////////////////////////////////////////////////////////
// Diddle
//
struct Diddle : public IHandlerRC
{
	Diddle() : mnRefCount(0), mnReceivedCount(0), mnPriority(0)
	{
		memset(mReceivedMessages, 0, sizeof(mReceivedMessages));
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		//MessageBasicRC<1>* pMBRC = (MessageBasicRC<1>*)pMessage;
		// We don't need to do anything with pMBRC.

		if((mnReceivedCount + 1) < kMaxMessageCount)
			mReceivedMessages[mnReceivedCount++] = id;
		gMutex.Lock();
		gMessageHistoryEntryArray.push_back(MessageHistoryEntry(id, this));
		gMutex.Unlock();
		return true;
	}

	int AddRef()
	{
		return mnRefCount.Increment();
	}

	int Release()
	{
		int32_t rc = mnRefCount.Decrement();
		if(rc)
			return rc;
		mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
		EA_DELETE this;         // is called while in destructor. This can happen
		return 0;               // if the destructor does things like debug traces.
	}

	AtomicInt32 mnRefCount;
	MessageId   mReceivedMessages[kMaxMessageCount];
	size_t      mnReceivedCount;
	int         mnPriority;
};


///////////////////////////////////////////////////////////////////////////////
// CountingQueuedHandler used for TestQueuedHandler
//
class TestQueuedHandlerMessage : public  EA::Messaging::IMessageRC
{
public:
	int   AddRef() 
	{ 
		return mnRefCount.Increment();
	}
	int   Release()
	{
		int32_t rc = mnRefCount.Decrement();
		if (rc)
			return rc;
		mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
		EA_DELETE this;         // is called while in destructor. This can happen
		return 0;               // if the destructor does things like debug traces.
	}

	static  TestQueuedHandlerMessage* Create()
	{
		return EA_NEW("TestQueuedHandler Message") TestQueuedHandlerMessage();
	}

	static const MessageId kMessageId = 314;
private:
	TestQueuedHandlerMessage() : mnRefCount(0) {}
	AtomicInt32 mnRefCount;
};

struct CountingQueuedHandler : public QueuedHandler
{
	CountingQueuedHandler(EA::Allocator::ICoreAllocator* allocator) : QueuedHandler(allocator,true), mnRefCount(0), mnReceivedCount(0)
	{
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		if (id == TestQueuedHandlerMessage::kMessageId)
		{
			mnReceivedCount++;
		}

		return true;
	}

	int AddRef()
	{
		return mnRefCount.Increment();
	}

	int Release()
	{
		int32_t rc = mnRefCount.Decrement();
		if (rc)
			return rc;
		mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
		EA_DELETE this;         // is called while in destructor. This can happen
		return 0;               // if the destructor does things like debug traces.
	}

	AtomicInt32 mnRefCount;
	AtomicInt32 mnReceivedCount;
};

struct CountingHandler : public IHandler
{
	CountingHandler() : mnRefCount(0), mnReceivedCount(0)
	{
	}

	bool HandleMessage(MessageId id, void* /*pMessage*/)
	{
		if (id == TestQueuedHandlerMessage::kMessageId)
		{
			mnReceivedCount++;
		}

		return true;
	}

	int AddRef()
	{
		return mnRefCount.Increment();
	}

	int Release()
	{
		int32_t rc = mnRefCount.Decrement();
		if (rc)
			return rc;
		mnRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
		EA_DELETE this;         // is called while in destructor. This can happen
		return 0;               // if the destructor does things like debug traces.
	}

	AtomicInt32 mnRefCount;
	AtomicInt32 mnReceivedCount;
};

///////////////////////////////////////////////////////////////////////////////
// TestMessageQueue
//
static int TestMessageQueue()
{
	int nErrorCount = 0;

	{
		const size_t kArraySize = 1000;
		MessageQueue messageQueue(&gCustomCoreAllocator);
		size_t i;
		MessageBasicRC<1>* pMBRC[kArraySize];
		IHandlerRC* pHandlerRC = (IHandlerRC*)(uintptr_t)0xfefefefe; // Dummy value which we won't use.

		for(i = 0; i < kArraySize; i++)
		{
			pMBRC[i] = EA_NEW("TestMessageQueue/MessageBasicRC") MessageBasicRC<1>;
			pMBRC[i]->AddRef();
			pMBRC[i]->SetUint32(0, (uint32_t)i);
			EATEST_VERIFY(pMBRC[i]->GetUint32(0) == i);
		}
		eastl::random_shuffle(pMBRC, pMBRC + kArraySize, gRandInstance);

		for(i = 0; i < kArraySize; i++)
			messageQueue.InsertMessage((MessageId)i, pMBRC[i], pHandlerRC, (int)pMBRC[i]->GetUint32(0));

		// Verify that the messages are ordered.
		MessageQueue::Queue* const pQueue = messageQueue.GetQueue();

		for(i = 0; i < kArraySize; i++)
		{
			EATEST_VERIFY(!pQueue->empty());
			if(!pQueue->empty())
			{
				const MessageQueue::Message& message = pQueue->front();
				
				EATEST_VERIFY(message.mpHandlerRC == pHandlerRC);

				MessageBasicRC<1>* pMBRCTemp = (MessageBasicRC<1>*)message.mpMessageRC;
				EATEST_VERIFY(pMBRCTemp->GetUint32(0) == ((kArraySize - 1) - i));
				EATEST_VERIFY(message.mnPriority == (int)((kArraySize - 1) - i));

				pQueue->pop_front();
			}
		}

		for(i = 0; i < kArraySize; i++)
			EATEST_VERIFY(pMBRC[i]->Release() == 0);
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestMessageSend
//
static int TestMessageSend()
{
	int nErrorCount = 0;

	{   // Test the registration and unregistation of handlers.
		size_t i;

		gMessageHistoryEntryArray.clear();

		const MessageId messageIdArray[5] = { 1, 2, 3, 4, 0 };
		AutoHandler     autoHandler;

		Server* pServer = EA_NEW("TestMessageSend/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		Dagger* pDagger = EA_NEW("TestMessageSend/Dagger") Dagger;
		pDagger->AddRef();

		// The following should (intentionally) have no effect.
		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		autoHandler.AddHandler(pServer, pDagger, messageIdArray, EAArrayCount(messageIdArray) - 1, true);

		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		EATEST_VERIFY(autoHandler.RemoveHandler());

		// The following should (intentionally) have no effect.
		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		nErrorCount += VerifyReceivedMessages(messageIdArray, pDagger->mReceivedMessages);

		EATEST_VERIFY(pDagger->Release() == 0);
		delete pServer;

		EATEST_VERIFY(gCustomCoreAllocator.mAllocationCount > 0);
		EATEST_VERIFY(gCustomCoreAllocator.mAllocationCount == gCustomCoreAllocator.mFreeCount);

		gCustomCoreAllocator.Reset();
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestMessageSendPriority
//
// Tests the handler priority mechanism of message sending.
//
static int TestMessageSendPriority()
{
	int nErrorCount = 0;
	eastl_size_t i;

	gMessageHistoryEntryArray.clear();

	const MessageId messageIdArray[3] = { 1, 2, 0 };

	Server* pServer = EA_NEW("TestMessageSendPriority/Server") EA::Messaging::Server(&gCustomCoreAllocator);
	Badger* pBadgerArray[32];

	for(i = 0; i < 32; i++)
	{
		pBadgerArray[i] = EA_NEW("TestMessageSendPriority/Badger") Badger;
		pBadgerArray[i]->mnPriority = (int)i;
	}

	// Jumble the ordering of the badger array.
	eastl::random_shuffle(pBadgerArray, pBadgerArray + 32, gRandInstance);

	for(i = 0; messageIdArray[i]; i++)
	{
		for(size_t j = 0; j < 32; j++)
			pServer->AddHandler(pBadgerArray[j], messageIdArray[i], false, pBadgerArray[j]->mnPriority);
	}

	for(i = 0; messageIdArray[i]; i++)
		pServer->MessageSend(messageIdArray[i], NULL);

	for(i = 0; messageIdArray[i]; i++)
	{
		for(size_t j = 0; j < 32; j++)
			pServer->RemoveHandler(pBadgerArray[j], messageIdArray[i], pBadgerArray[j]->mnPriority);
	}

	// Verify that messages were received in order of highest to lowest priority
	eastl_size_t k = 0;

	for(i = 0; messageIdArray[i]; i++)
	{
		for(eastl_size_t j = 0; j < 32; j++, k++)
		{
			EATEST_VERIFY(k < gMessageHistoryEntryArray.size());

			if(k < gMessageHistoryEntryArray.size())
			{
				// We should be able to predict which entry is next.
				EATEST_VERIFY(gMessageHistoryEntryArray[k].mId == messageIdArray[i]);

				Badger* const pBadger = (Badger*)gMessageHistoryEntryArray[k].mpHandler;

				EATEST_VERIFY(pBadger && (pBadger->mnPriority == (int32_t)(uint32_t)(31 - j)));
			}
		}
	}

	for(i = 0; i < 32; i++)
		delete pBadgerArray[i];

	delete pServer;

	EATEST_VERIFY(gCustomCoreAllocator.mAllocationCount > 0);
	EATEST_VERIFY(gCustomCoreAllocator.mAllocationCount == gCustomCoreAllocator.mFreeCount);

	gCustomCoreAllocator.Reset();

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestMessagePost
//
static int TestMessagePost()
{
	int nErrorCount = 0;

	{   // Basic message posting
		size_t i;

		gMessageHistoryEntryArray.clear();

		const MessageId messageIdArray[5] = { 1, 2, 3, 4, 0 };
		AutoHandler     autoHandler;

		Server* pServer = EA_NEW("TestMessagePost/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		Dagger* pDagger = EA_NEW("TestMessagePost/Dagger") Dagger;
		pDagger->AddRef();

		// The following should (intentionally) have no effect.
		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		autoHandler.AddHandler(pServer, pDagger, messageIdArray, EAArrayCount(messageIdArray) - 1, true);

		for(i = 0; messageIdArray[i]; i++)
		{
			MessageBasicRC<1>* pMBRC = EA_NEW("TestMessagePost/MessageBasicRC") MessageBasicRC<1>;
			pMBRC->AddRef();
			pServer->MessagePost(messageIdArray[i], pMBRC);
			pMBRC->Release();
		}

		pServer->ProcessQueue();

		EATEST_VERIFY(autoHandler.RemoveHandler());

		// The following should (intentionally) have no effect.
		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		nErrorCount += VerifyReceivedMessages(messageIdArray, pDagger->mReceivedMessages);

		EATEST_VERIFY(pDagger->Release() == 0);

		delete pServer;
	}

	return nErrorCount;
}




class MultiHandler
{
public:
	MultiHandler() : mHandleMessage3(this, &MultiHandler::HandleMessage3), 
					 mHandleMessage4(this, &MultiHandler::HandleMessage4),
					 mbMessage3Received(false),
					 mbMessage4Received(false)
	{
		GetServer()->AddHandler(&mHandleMessage3, 0x33333333, false);
		GetServer()->AddHandler(&mHandleMessage4, 0x44444444, false);
	}

   ~MultiHandler()
	{
		GetServer()->RemoveHandler(&mHandleMessage3, 0x33333333);
		GetServer()->RemoveHandler(&mHandleMessage4, 0x44444444);
	}

	bool HandleMessage3(MessageId id, void* /*pMsg*/)
	{
		mbMessage3Received = (id == 0x33333333);
		return true;
	}

	bool HandleMessage4(MessageId id, void* /*pMsg*/)
	{
		mbMessage4Received = (id == 0x44444444);
		return true;
	}

protected:
	Handler<MultiHandler> mHandleMessage3;
	Handler<MultiHandler> mHandleMessage4;

public:
	bool mbMessage3Received; // Used to verify the test succeeded.
	bool mbMessage4Received;
};


///////////////////////////////////////////////////////////////////////////////
// TestMessageExtra
//
// Tests functionality from EAMessageExtra.
//
static int TestMessageExtra()
{
	int nErrorCount = 0;

	{ // Handler
		Server* pServer  = EA_NEW("TestMessageExtra/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		SetServer(pServer);

		{
			MultiHandler multiHandler;
			pServer->MessageSend(0x33333333, NULL);
			pServer->MessageSend(0x44444444, NULL);
			EATEST_VERIFY(multiHandler.mbMessage3Received && multiHandler.mbMessage4Received);
		}

		delete pServer;
	}

	{   // HandlerMemberFunction
		size_t i;

		gMessageHistoryEntryArray.clear();

		const MessageId messageIdArray[3] = { 1, 2, 0 };

		Server* pServer  = EA_NEW("TestMessageExtra/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		Widget* pWidget1 = EA_NEW("TestMessageExtra/Widget") Widget;
		Widget* pWidget2 = EA_NEW("TestMessageExtra/Widget") Widget;

		for(i = 0; messageIdArray[i]; i++)
		{
			pServer->AddHandlerFunction(pWidget1->mHandlerMemberFunction, &pWidget1->mHandlerMemberFunction, messageIdArray[i], false);
			pServer->AddHandlerFunction(pWidget2->mHandlerMemberFunction, &pWidget2->mHandlerMemberFunction, messageIdArray[i], false);
		}

		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		for(i = 0; messageIdArray[i]; i++)
		{
			pServer->RemoveHandlerFunction(pWidget1->mHandlerMemberFunction, messageIdArray[i]);
			pServer->RemoveHandlerFunction(pWidget2->mHandlerMemberFunction, messageIdArray[i]);
		}

		nErrorCount += VerifyReceivedMessages(messageIdArray, pWidget1->mReceivedMessages);
		nErrorCount += VerifyReceivedMessages(messageIdArray, pWidget2->mReceivedMessages);

		delete pWidget1;
		delete pWidget2;
		delete pServer;
	}


	{   // AutoHandler
		size_t i;

		gMessageHistoryEntryArray.clear();

		const MessageId messageIdArray[5] = { 1, 2, 3, 4, 0 };
		AutoHandler     autoHandler;

		Server* pServer = EA_NEW("TestMessageExtra/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		Badger* pBadger = EA_NEW("TestMessageExtra/Badger") Badger;

		autoHandler.AddHandler(pServer, pBadger, messageIdArray, EAArrayCount(messageIdArray) - 1, false);

		for(i = 0; messageIdArray[i]; i++)
			pServer->MessageSend(messageIdArray[i], NULL);

		EATEST_VERIFY(autoHandler.RemoveHandler());

		nErrorCount += VerifyReceivedMessages(messageIdArray, pBadger->mReceivedMessages);

		delete pBadger;
		delete pServer;
	}

	{   // MessageBasic
		const size_t kCount = 4;
		MessageBasic<kCount> messageBasic;

		// Get/SetUint32
		for(size_t i = 0; i < kCount; i++)
			messageBasic.SetUint32(i, 0x11111111);
		for(size_t i = 0; i < kCount; i++)
			EATEST_VERIFY(messageBasic.GetUint32(i) == 0x11111111);

		// Get/SetUint64
		for(size_t i = 0; i < kCount; i++)
			messageBasic.SetUint64(i, UINT64_C(0x2222222222222222));
		for(size_t i = 0; i < kCount; i++)
			EATEST_VERIFY(messageBasic.GetUint64(i) == UINT64_C(0x2222222222222222));

		// Get/SetFloat
		for(size_t i = 0; i < kCount; i++)
			messageBasic.SetFloat(i, 1.1f);
		for(size_t i = 0; i < kCount; i++)
			EATEST_VERIFY(messageBasic.GetFloat(i) == 1.1f);

		// Get/SetDouble
		for(size_t i = 0; i < kCount; i++)
			messageBasic.SetDouble(i, 2.2);
		for(size_t i = 0; i < kCount; i++)
			EATEST_VERIFY(messageBasic.GetDouble(i) == 2.2);

		// Get/SetVoidPtr
		for(size_t i = 0; i < kCount; i++)
			messageBasic.SetVoidPtr(i, (void*)(uintptr_t)0x33333333);
		for(size_t i = 0; i < kCount; i++)
			EATEST_VERIFY(messageBasic.GetVoidPtr(i) == (void*)(uintptr_t)0x33333333);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestMessageDefer
//
static int TestMessageDefer()
{
	int nErrorCount = 0;

	#if EA_MESSAGE_DEFER_ENABLED
		gMessageHistoryEntryArray.clear();

		const MessageId messageIdArray[5] = { 1, 2, 3, 4, 0 };
		DeferredMessage dm;

		Server* const pServer = EA_NEW("TestMessageDefer/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		Diddle* const pDiddle = EA_NEW("TestMessageDefer/Diddle") Diddle;
		pDiddle->AddRef();

		for(int i = 0; messageIdArray[i]; i++)
		{
			MessageBasicRC<1>* pMBRC = EA_NEW("MessageBasicRC") MessageBasicRC<1>;

			pMBRC->AddRef();
			EATEST_VERIFY(dm.MessagePost((i + 1) * 200, messageIdArray[i], pMBRC, pDiddle, pServer));
			pMBRC->Release();
		}

		// We need to wait for a while.
		const uint32_t nEndTime = EA::CallbackSystem::GetTime() + 8000;

		do{
			RandomSleep(200, 400);
			pServer->ProcessQueue();
			EA::CallbackSystem::Update();
		} while(EA::CallbackSystem::GetTime() < nEndTime);

		// Verify that things went OK
		nErrorCount += VerifyReceivedMessages(messageIdArray, pDiddle->mReceivedMessages, false);

		// Shut down.
		pDiddle->Release();
		delete pServer; // As of this writing, the DeferredMessage will crash if the server is deleted before all DeferredMessages are received. 

	#endif

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestThreadSafeCallback
//
static int TestThreadSafeCallback()
{
	int nErrorCount = 0;

	#if EA_MESSAGE_DEFER_ENABLED
		///    void SomeCallbackFunction(void* pContext, uint32_t absTime, uint32_t deltaTime);
		///
		///    ThreadSafeCallback* pThreadSafeCallback = EA_NEW("TestThreadSafeCallback/ThreadSafeCallback") ThreadSafeCallback;
		///    pThreadSafeCallback->AddRef();  // AddRef for our own ownership.
		///    pThreadSafeCallback->Start(1000, true, SomeCallbackFunction, NULL, pServer);
		///    ...
		///    pThreadSafeCallback->Release(); // Release when we are done with it.
	#endif

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestCallbackMessage
//
static int TestCallbackMessage()
{
	int nErrorCount = 0;

	#if EA_MESSAGE_DEFER_ENABLED
		gMessageHistoryEntryArray.clear();

		Server* const pServer = EA_NEW("TestCallbackMessage/Server") EA::Messaging::Server(&gCustomCoreAllocator);
		pServer->Init();

		Dagger* const pDagger = EA_NEW("TestCallbackMessage/Dagger") Dagger;
		pDagger->AddRef();

		pServer->AddHandler(pDagger, 0x12345678, true);

		CallbackMessage callbackMessage;
		callbackMessage.Start(1000, false, kDeliveryPost, 0x12345678, NULL, pServer);
		
		// We wait for a while.
		const uint32_t nEndTime = EA::CallbackSystem::GetTime() + 8000;

		do{
			RandomSleep(200, 400);
			pServer->ProcessQueue();
			EA::CallbackSystem::Update();
		} while(EA::CallbackSystem::GetTime() < nEndTime);

		callbackMessage.Stop();

		// Check the message history array.
		EATEST_VERIFY(gMessageHistoryEntryArray.size() >= 5); // We aren't precise about the expected value, but expect at least 5.

		for(eastl_size_t i = 0, iEnd = gMessageHistoryEntryArray.size(); i < iEnd; i++)
		{
			const MessageHistoryEntry& mhe = gMessageHistoryEntryArray[i];

			EATEST_VERIFY(mhe.mId == 0x12345678);
			EATEST_VERIFY(mhe.mpHandler == pDagger);
		}

		// Shut down.
		pServer->RemoveHandler(pDagger, 0x12345678);
		pDagger->Release();
		pServer->Shutdown();

		delete pServer;

	#endif

	return nErrorCount;
}




const uint32_t kMsgTouchdownScored = 0x00112233;

struct TouchdownScored : public EA::Messaging::PostedMessageBase
{
	TouchdownScored(int points = 0) : mPoints(points){}
	TouchdownScored(const TouchdownScored& ts) : mPoints(ts.mPoints) {}
	int mPoints;
};


///////////////////////////////////////////////////////////////////////////////
// TestPostedMessagePool
//
static int TestPostedMessagePool()
{
	int nErrorCount = 0;

	Server* pServer = EA_NEW("TestPostedMessagePool/Server") EA::Messaging::Server(&gCustomCoreAllocator);

	{
		#if defined(_WIN32) && defined(PostMessage)
			#undef PostMessage
		#endif

		// PostedMessagePool(EA::Messaging::Server* mpMessageServer = NULL, EA::Allocator::ICoreAllocator* mpAllocator = NULL);
		EA::Messaging::PostedMessagePool pmp(pServer, &gCustomCoreAllocator);

		/// Example usage with PostedMessagePool. The msg is copied as part of the posting.
		TouchdownScored msg(7);
		pmp.PostMessage(kMsgTouchdownScored, &msg);

		// template <typename U> U* CreateMessage(const U* pU = NULL);
		TouchdownScored* pMsg = pmp.CreateMessage<TouchdownScored>();
		pMsg->mPoints = 7;
		pServer->MessagePost(kMsgTouchdownScored, pMsg);

		// template <typename U> U* PostMessage(MessageId id, const U* pU, int nPriority = kPriorityNormal, IHandlerRC* pHandlerRC = NULL);
		TouchdownScored* pResult = pmp.PostMessage(kMsgTouchdownScored, pMsg);
		EATEST_VERIFY(pResult != pMsg); // Verify that a new message was created which is a copy of pMsg.

		// What we need to do here is test receiving the messages.

		// We don't call this, as posted messages are ref-counted and will be Released by their owners (in this case the Server).
		// pmp.DestroyMessage(pMsg);
		// pmp.DestroyMessage(pResult);
	}

	delete pServer; // This will Release any posted messages that are still queued.

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestAsType
//
static int TestAsType()
{
	int nErrorCount = 0;

	// Disabled because we are probably not going to use this TypeId system.

	/*
	struct HandlerTest : public IHandler
	{    
		bool HandleMessage(MessageId, void*) { return true; }
	};

	struct HandlerRCTest : public IHandlerRC
	{
		HandlerRCTest() : mRefCount(0) { }
	
		bool HandleMessage(MessageId, void*) { return true; }
		int  AddRef() { return mRefCount.Increment(); }
		int  Release() { int32_t rc = mRefCount.Decrement(); if(rc) return rc; delete this; return 0; }
	
		AtomicInt32 mRefCount;
	};

	struct QueuedHandlerTest : public IQueuedHandler
	{
		bool HandleMessage(MessageId/, void*) { return true; }
		bool HandleQueuedMessage(MessageId/, EA::Messaging::IMessageRC*, int) { return true; }
	};

	HandlerTest       handlerTest;
	HandlerRCTest     handlerRCTest;
	QueuedHandlerTest queuedHandlerTest;

	IHandler*       pIHandler;
	IHandlerRC*     pIHandlerRC;
	IQueuedHandler* pIQueuedHandler;

	// IHandler
	pIHandler = type_cast<IHandler*>(&handlerTest);
	EATEST_VERIFY(pIHandler == &handlerTest);

	pIHandler = type_cast<IHandlerRC*>(&handlerTest);
	EATEST_VERIFY(pIHandler == NULL);

	pIHandler = type_cast<IQueuedHandler*>(&handlerTest);
	EATEST_VERIFY(pIHandler == NULL);

	// IHandlerRC
	pIHandler = type_cast<IHandler*>(&handlerRCTest);
	EATEST_VERIFY(pIHandler == &handlerRCTest);

	pIHandlerRC = type_cast<IHandlerRC*>(&handlerRCTest);
	EATEST_VERIFY(pIHandlerRC == &handlerRCTest);

	pIQueuedHandler = type_cast<IQueuedHandler*>(&handlerRCTest);
	EATEST_VERIFY(pIQueuedHandler == NULL);

	// IQueuedHandler
	pIHandler = type_cast<IHandler*>(&queuedHandlerTest);
	EATEST_VERIFY(pIHandler == &queuedHandlerTest);

	pIHandlerRC = type_cast<IHandlerRC*>(&queuedHandlerTest);
	EATEST_VERIFY(pIHandlerRC == NULL);

	pIQueuedHandler = type_cast<IQueuedHandler*>(&queuedHandlerTest);
	EATEST_VERIFY(pIQueuedHandler == &queuedHandlerTest);
	*/

	return nErrorCount;
}

struct TestQueuedHandlerHelperContext
{
	CountingQueuedHandler* mpQueuedHandler;
	size_t mNumMessages;
};

intptr_t TestQueuedHandlerHelper(void* context)
{
	TestQueuedHandlerHelperContext* pTestContext = (TestQueuedHandlerHelperContext*)context;
	CountingQueuedHandler* pQueuedHandler = pTestContext->mpQueuedHandler;
	const size_t kNumMessages = pTestContext->mNumMessages;

	EA::Messaging::Server* pMS = EA::Messaging::GetServer();
	EA_ASSERT(pMS);

	for (size_t i = 0; i < kNumMessages; ++i)
	{
		pMS->MessagePost(TestQueuedHandlerMessage::kMessageId, TestQueuedHandlerMessage::Create());
		pQueuedHandler->ProcessQueue();
		EA::Thread::ThreadSleep();
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// TestQueuedHandler
//
static int TestQueuedHandler()
{
	int nErrorCount = 0;

	{
		struct QueuedHandlerSubclass : public QueuedHandler
		{
			QueuedHandlerSubclass(EA::Allocator::ICoreAllocator* pAllocator)
				: QueuedHandler(pAllocator){}

			bool HandleMessage(MessageId messageId, void* pMessage)
				{ mLastMessageId = messageId; mpLastMessage = pMessage; return false; }

			MessageId mLastMessageId;
			void*     mpLastMessage;
		};

		QueuedHandlerSubclass qhs(&gCustomCoreAllocator);
		Server                server(&gCustomCoreAllocator);

		server.AddQueuedHandler(&qhs, 0x00000001, false);

		// Test MessageSend
		server.MessageSend(0x00000001, &server, NULL);
		EATEST_VERIFY(qhs.mLastMessageId == 0x00000001);
		EATEST_VERIFY(qhs.mpLastMessage == &server);
		EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 0); // Nothing should be queued, as this is a sent (and not posted) message.
		qhs.mLastMessageId = 0x00000000;

		// Test MessagePost
		MessageBasicRC<1>* pMBRC = new MessageBasicRC<1>;
		pMBRC->SetUint32(0, 0x12345678);
		server.MessagePost(0x00000001, pMBRC);
		EATEST_VERIFY(qhs.mLastMessageId == 0x00000000); // It should not have been received.
		EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 1);
		if(qhs.GetMessageQueue()->GetMessageCount()) // This should always be true.
		{
			MessageQueue::Message mqm;
			bool result = qhs.GetMessageQueue()->GetNextMessage(mqm);

			EATEST_VERIFY(result);
			EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 0); // Assert that the queue is now empty.
			EATEST_VERIFY(static_cast<MessageBasicRC<1>*>(mqm.mpMessageRC)->GetUint32(0) == 0x12345678);

			// Since we are directly manipulating the MessageQueue in this test case, we need to manually 
			// Release Message mpMessageRC.
			mqm.mpMessageRC->Release();
		}
	}

	{
		struct HandlerSubclass : public IHandler
		{
			bool HandleMessage(MessageId messageId, void* pMessage)
				{ mLastMessageId = messageId; mpLastMessage = pMessage; return false; }

			MessageId mLastMessageId;
			void*     mpLastMessage;
		};

		QueuedHandler   qhs(&gCustomCoreAllocator);
		HandlerSubclass hs;
		Server          server(&gCustomCoreAllocator);

		qhs.SetHandler(&hs);
		server.AddQueuedHandler(&qhs, 0x00000001, false);

		// Test MessageSend
		server.MessageSend(0x00000001, &server, NULL);
		EATEST_VERIFY(hs.mLastMessageId == 0x00000001);
		EATEST_VERIFY(hs.mpLastMessage == &server);
		EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 0); // Nothing should be queued, as this is a sent (and not posted) message.
		hs.mLastMessageId = 0x00000000;

		// Test MessagePost
		MessageBasicRC<1>* pMBRC = new MessageBasicRC<1>;
		pMBRC->SetUint32(0, 0x12345678);
		server.MessagePost(0x00000001, pMBRC);
		EATEST_VERIFY(hs.mLastMessageId == 0x00000000); // It should not have been received.
		EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 1);
		if(qhs.GetMessageQueue()->GetMessageCount()) // This should always be true.
		{
			qhs.ProcessQueue(); // This should result in messages being recieved by hs (through qhs).
			EATEST_VERIFY(qhs.GetMessageQueue()->GetMessageCount() == 0); // It should now be empty.
			EATEST_VERIFY(hs.mLastMessageId == 0x00000001); // It should have been received.
			EATEST_VERIFY(hs.mpLastMessage == pMBRC);
		}
	}

	// Creates two handlers (IHandler and a QueuedHandler) and one thread.
	// The thread will process the queued handler and POST messages which both handlers
	// are registered for. We expect the number of times the handler's HandleMessage() function
	// is invoked to be equal to the number of messages sent.
	{
		const size_t kNumTestMessages = 1000;
		CountingHandler ch;
		CountingQueuedHandler cqh(&gCustomCoreAllocator);

		TestQueuedHandlerHelperContext helperContext;
		helperContext.mNumMessages = kNumTestMessages;
		helperContext.mpQueuedHandler = &cqh;
		
		Server server(&gCustomCoreAllocator);
		server.AddHandler(&ch, TestQueuedHandlerMessage::kMessageId, false);
		server.AddQueuedHandler(&cqh, TestQueuedHandlerMessage::kMessageId, false);
		EA::Messaging::SetServer(&server);

		EA::Thread::Thread queuedMessageHandlerThread;
		queuedMessageHandlerThread.Begin(TestQueuedHandlerHelper, &helperContext);
		while (queuedMessageHandlerThread.GetStatus() == EA::Thread::Thread::kStatusRunning)
		{
			server.ProcessQueue();
			EA::Thread::ThreadSleep();
		}
		queuedMessageHandlerThread.WaitForEnd();
		server.ProcessQueue();
		cqh.ProcessQueue();

		EATEST_VERIFY_F(kNumTestMessages == ch.mnReceivedCount,  "IHandler MessageCount:      expected(%" PRIu64 ") != actual(%d)", kNumTestMessages, ch.mnReceivedCount.GetValue());
		EATEST_VERIFY_F(kNumTestMessages == cqh.mnReceivedCount, "QueuedHandler MessageCount: expected(%" PRIu64 ") != actual(%d)", kNumTestMessages, cqh.mnReceivedCount.GetValue());

		// cleanup
		EA::Messaging::SetServer(nullptr);
		gCustomCoreAllocator.Reset();
	}
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// EAMain
//
int EAMain(int argc, char** argv)
{
	int nErrorCount = 0;

	EA::EAMain::PlatformStartup();

	#if EA_MESSAGE_DEFER_ENABLED
		EA::StdC::CallbackManager callbackManager;
		callbackManager.Init();
	#endif

	// Add the tests
	EA::UnitTest::TestApplication testSuite("EAMessage Unit Tests", argc, argv);

	testSuite.AddTest("AsType",              TestAsType);
	testSuite.AddTest("MessageQueue",        TestMessageQueue);
	testSuite.AddTest("MessageSend",         TestMessageSend);
	testSuite.AddTest("MessageSendPriority", TestMessageSendPriority);
	testSuite.AddTest("MessagePost",         TestMessagePost);
	testSuite.AddTest("MessageExtra",        TestMessageExtra);
	testSuite.AddTest("MessageDefer",        TestMessageDefer);
	testSuite.AddTest("ThreadSafeCallback",  TestThreadSafeCallback);
	testSuite.AddTest("CallbackMessage",     TestCallbackMessage);
	testSuite.AddTest("PostedMessagePool",   TestPostedMessagePool);

	testSuite.AddTest("QueuedHandler",       TestQueuedHandler);

	nErrorCount = testSuite.Run();

	#if EA_MESSAGE_DEFER_ENABLED
		callbackManager.Shutdown();
	#endif

	EA::EAMain::PlatformShutdown(nErrorCount);

	return nErrorCount;
}

EA_RESTORE_VC_WARNING() // C4355 - 'this' : used in base member initializer list
