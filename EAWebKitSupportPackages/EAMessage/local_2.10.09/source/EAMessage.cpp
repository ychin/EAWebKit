/////////////////////////////////////////////////////////////////////////////
// EAMessage.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis
//
/////////////////////////////////////////////////////////////////////////////


#include <EAMessage/internal/Config.h>
#include <EAMessage/EAMessage.h>
#include <EASTL/fixed_vector.h>
#include <new>
#include EA_ASSERT_HEADER

#if EAMESSAGE_TRACE_ENABLED
	#include <EATrace/EATrace.h>
#endif

#if defined(EA_TRACE_ENABLED) && (EA_TRACE_ENABLED != 0)
	// #include <EATrace/EATrace.h>
	// This file uses EATrace, but we don't have build file support for it yet since the build file
	// needs to be updated to read a global configuration option which brings in a dependency on 
	// the EATrace package. For the time being we disable EATrace usage in this file.
	#undef  EA_TRACE_ENABLED
	#define EA_TRACE_ENABLED 0
#endif


// Include headers for GetMillisecondTime function
#if defined(EA_PLATFORM_MICROSOFT)
	#pragma warning(push, 0)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	#pragma warning(pop)
#else
	#include <time.h>
#endif



namespace EA{

namespace Messaging{


inline size_t GetMillisecondTime()
{
	#if defined(EA_PLATFORM_MICROSOFT) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		const ULONGLONG ticks = GetTickCount64();
		return static_cast<size_t>(ticks);
	#elif defined(EA_PLATFORM_WINDOWS)
		return (size_t)GetTickCount(); 
	#else
		return (unsigned)(clock() * UINT64_C(1000) / CLOCKS_PER_SEC);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// MessageQueue
///////////////////////////////////////////////////////////////////////////////

MessageQueue::MessageQueue(bool threadSafe)
  : mbThreadSafetyEnabled(threadSafe),
	mQueue(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Queue", EAMESSAGE_GET_DEFAULT_ALLOCATOR())),
	mQueueMutex()
{
}


MessageQueue::MessageQueue(EA::Allocator::ICoreAllocator* pCoreAllocator, bool threadSafe)
  : mbThreadSafetyEnabled(threadSafe),
	mQueue(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Queue", pCoreAllocator)),
	mQueueMutex()
{
}


MessageQueue::~MessageQueue()
{
}


void MessageQueue::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mQueue.get_allocator().set_allocator(pCoreAllocator);

	// Could alternatively use:
	// mQueue.set_allocator(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Queue", pCoreAllocator));
}


int MessageQueue::GetOption(int option) const
{
	switch (option)
	{
		case kOptionThreadSafe:
			return mbThreadSafetyEnabled  ? 1 : 0;
	}

	return 0;
}


void MessageQueue::SetOption(int option, int value)
{
	switch (option)
	{
		case kOptionThreadSafe:
			mbThreadSafetyEnabled = (value != 0);
			break;
	}
}


int MessageQueue::Lock(bool bLock)
{
	if(bLock)
		mQueueMutex.Lock();
	else 
		mQueueMutex.Unlock();

	return mQueueMutex.GetLockCount();
}


size_t MessageQueue::GetMessageCount() const
{
	if(mbThreadSafetyEnabled)
		mQueueMutex.Lock();

	size_t queueSize = mQueue.size();

	if(mbThreadSafetyEnabled)
		mQueueMutex.Unlock();

	return queueSize;
}


bool MessageQueue::GetNextMessage(Message& message)
{
	bool returnValue;

	if(mbThreadSafetyEnabled)
		mQueueMutex.Lock();

	if(!mQueue.empty())
	{
		message = mQueue.front();
		mQueue.pop_front();
		returnValue = true;
	}
	else
		returnValue = false;

	if(mbThreadSafetyEnabled)
		mQueueMutex.Unlock();

	return returnValue;
}


void MessageQueue::InsertMessage(const Message& message)
{
	if(mbThreadSafetyEnabled)
		mQueueMutex.Lock();

	// We do a quick check for the message being added to the back of the queue.
	// 90+% of the time we'll be adding to the back of the queue.
	if(mQueue.empty() || (mQueue.back().mnPriority >= message.mnPriority))
		mQueue.push_back(message);
	else if((mQueue.front().mnPriority < message.mnPriority))
		mQueue.push_front(message);
	else
	{
		// In this case we must manually search the queue for the appropriate location to insert.
		// We can implement a binary search here or we can implement a linear search. For entirely
		// random data, a binary search will be the best choice. But it turns out that message 
		// queues here aren't random. 90% of messages are of kPriorityNormal and will be lumped 
		// together in the middle of the queue. So we optimize for that situation.
		if(message.mnPriority <= kPriorityNormal) // If the input priority is low...we start searching from the back.
		{
			for(Queue::reverse_iterator it(mQueue.rbegin()); it != mQueue.rend(); ++it)
			{
				const Message& messageTemp = *it;

				if(message.mnPriority <= messageTemp.mnPriority)
				{
					mQueue.insert(it.base(), message);
					goto done;
				}
			
			}
			mQueue.push_front(message);
		}
		else
		{
			for(Queue::iterator it(mQueue.begin()); it != mQueue.end(); ++it)
			{
				const Message& messageTemp = *it;

				if(message.mnPriority > messageTemp.mnPriority)
				{
					mQueue.insert(it, message);
					goto done;
				}
			}
			mQueue.push_back(message);
		}
	}

	done:
	if(mbThreadSafetyEnabled)
		mQueueMutex.Unlock();
}


void MessageQueue::InsertMessage(MessageId id, IMessageRC* pMessageRC, IHandlerRC* pHandlerRC, int nPriority)
{
	// No mutex lock because the InsertMessage call below handles it.
	Message message;

	message.mId         = id;
	message.mpMessageRC = pMessageRC;
	message.mpHandlerRC = pHandlerRC;
	message.mnPriority  = nPriority;

	#if EAMESSAGE_C_HANDLER_ENABLED
		message.mpHandlerFunction        = NULL;
		message.mpHandlerFunctionContext = NULL;
	#endif

	InsertMessage(message);
}


#if EAMESSAGE_C_HANDLER_ENABLED
	void MessageQueue::InsertMessage(MessageId id, IMessageRC* pMessageRC, HandlerFunction pHandlerFunction, 
										void* pHandlerFunctionContext, int nPriority)
	{
		// No mutex lock because the InsertMessage call below handles it.
		Message message;

		message.mId                      = id;
		message.mpMessageRC              = pMessageRC;
		message.mpHandlerRC              = NULL;
		message.mnPriority               = nPriority;

		message.mpHandlerFunction        = pHandlerFunction;
		message.mpHandlerFunctionContext = pHandlerFunctionContext;

		InsertMessage(message);
	}
#endif



///////////////////////////////////////////////////////////////////////////////
// Server
///////////////////////////////////////////////////////////////////////////////

static Server* gpServer = NULL;

EAMESSAGE_API Server* GetServer()
{
	// Possibly assert if not present.
	return gpServer;
}

EAMESSAGE_API Server* SetServer(Server* pServer)
{   
	Server* const pServerSaved = gpServer;
	gpServer = pServer;
	return pServerSaved;
}



void Server::HandlerInfo::AddRef()
{
	#if EAMESSAGE_C_HANDLER_ENABLED
		if(mpHandler)
			static_cast<IHandlerRC*>(mpHandler)->AddRef();
		else
			mpHandlerFunction(kMessageIdAddRef, NULL, mpHandlerFunctionContext);
	#else
		static_cast<IHandlerRC*>(mpHandler)->AddRef();
	#endif
}


void Server::HandlerInfo::Release()
{
	#if EAMESSAGE_C_HANDLER_ENABLED
		if(mpHandler)
			static_cast<IHandlerRC*>(mpHandler)->Release();
		else
			mpHandlerFunction(kMessageIdRelease, NULL, mpHandlerFunctionContext);
	#else
		static_cast<IHandlerRC*>(mpHandler)->Release();
	#endif
}


Server::Server()
 :  mnQueueProcessedMessageCount(0),
	mpCoreAllocator(NULL),
	mMessageQueue(false),   // We'll handle the thread safety ourselves.
	mMessageQueueMutex(),
	mHandlerInfoMap(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Map", mpCoreAllocator)),
	mHandlerInfoMapMutex(),
	mQueuedHandlerInfoMap(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Map", mpCoreAllocator)),
	mQueuedHandlerInfoMapMutex(),
	mbThreadSafetyEnabled(true),
	mbReentrantSafetyEnabled(true),
	mbQueueProcessingEnabled(true),
	mbClearQueueEnabled(true),
	mbRefCountEnabled(true),
	mbTraceEnabled(true)
{
	// Empty
}

Server::Server(EA::Allocator::ICoreAllocator* pCoreAllocator)
 :  mnQueueProcessedMessageCount(0),
	mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EAMESSAGE_GET_DEFAULT_ALLOCATOR()),
	mMessageQueue(mpCoreAllocator, false), // We'll handle the thread safety ourselves.
	mMessageQueueMutex(),
	mHandlerInfoMap(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Map", mpCoreAllocator)),
	mHandlerInfoMapMutex(),
	mQueuedHandlerInfoMap(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "Map", mpCoreAllocator)),
	mQueuedHandlerInfoMapMutex(),
	mbThreadSafetyEnabled(true),
	mbReentrantSafetyEnabled(true),
	mbQueueProcessingEnabled(true),
	mbClearQueueEnabled(true),
	mbRefCountEnabled(true),
	mbTraceEnabled(true)
{
	// Empty
}

void Server::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	mHandlerInfoMap.get_allocator().set_allocator(pCoreAllocator);
	mQueuedHandlerInfoMap.get_allocator().set_allocator(pCoreAllocator);
	mMessageQueue.SetAllocator(pCoreAllocator);
}

Server::~Server()
{
	Shutdown();
}


bool Server::Init()
{
	// Nothing to do for now.
	return true;
}

bool Server::Shutdown()
{
	{
		if(mbThreadSafetyEnabled)
			mHandlerInfoMapMutex.Lock();

		// We clear and delete all the lists in the mHandlerInfoMap.
		ClearHandlerInfoMap(mHandlerInfoMap);

		if(mbThreadSafetyEnabled)
			mHandlerInfoMapMutex.Unlock();
	}


	// We clear and delete all the lists in the mQueuedHandlerInfoMap.
	{
		if(mbThreadSafetyEnabled)
			mQueuedHandlerInfoMapMutex.Lock();

		ClearHandlerInfoMap(mQueuedHandlerInfoMap);

		if(mbThreadSafetyEnabled)
			mQueuedHandlerInfoMapMutex.Unlock();
	}


	// We're still holding a reference to each of the messages still in the queue. Release them.
	{
		if(mbThreadSafetyEnabled)
			mMessageQueueMutex.Lock();

		MessageQueue::Queue* const pQueue = mMessageQueue.GetQueue();

		for(MessageQueue::Queue::iterator it = pQueue->begin(), itEnd = pQueue->end(); it != itEnd; ++it)
		{
			MessageQueue::Message& message = *it;

			if(message.mpMessageRC)
			{
				message.mpMessageRC->Release();
				message.mpMessageRC = NULL;
			}

			if(message.mpHandlerRC)
			{
				message.mpHandlerRC->Release();
				message.mpHandlerRC = NULL;
			}
		}

		pQueue->clear();

		if(mbThreadSafetyEnabled)
			mMessageQueueMutex.Unlock();
	}

	return true;
}


void Server::ClearHandlerInfoMap(HandlerInfoMap& handlerInfoMap)
{
	// We assume the given mutex has already been locked during this operation.

	while(!handlerInfoMap.empty()) // We do this technique because we don't want to have a re-entrancy problem.
	{
		HandlerInfoMap::iterator itMap = handlerInfoMap.begin();
		HandlerInfoList* const pList = (*itMap).second;
		handlerInfoMap.erase(itMap);

		if(pList)
		{
			if(!pList->empty())
			{
				#if EAMESSAGE_TRACE_ENABLED
					if(mbTraceEnabled)
					{
						const MessageId id = (*itMap).first;
						EA_TRACE_FORMATTED(("Messaging::Server::Shutdown: Handler(s) still registered for id 0x%08x\n", id));
					}
				#endif

				while(!pList->empty()) // We do this technique because we don't want to have a re-entrancy problem.
				{
					HandlerInfo handlerInfo = pList->front(); // Intentionally make a copy.
					pList->pop_front();

					#if EAMESSAGE_TRACE_ENABLED
						if(mbTraceEnabled)
						{
							#if EAMESSAGE_C_HANDLER_ENABLED
								EA_TRACE_FORMATTED(("   %08p\n", (void*)handlerInfo.mpHandler ? (void*)handlerInfo.mpHandler : (void*)handlerInfo.mpHandlerFunction));
							#else
								EA_TRACE_FORMATTED(("   %08p\n", (void*)handlerInfo.mpHandler));
							#endif
						}
					#endif

					if(mbRefCountEnabled && handlerInfo.mbRefCounted)
						handlerInfo.Release();
				}
			}

			// Do a placement delete and memory free.
			pList->~HandlerInfoList();
			mpCoreAllocator->Free(pList);
		}
	}
}


int Server::GetOption(int option) const
{
	// Consider switching options to be an array instead of individual variables.
	switch (option)
	{
		case kOptionThreadSafe:
			return mbThreadSafetyEnabled  ? 1 : 0;
		case kOptionReentrancy:
			return mbReentrantSafetyEnabled  ? 1 : 0;
		case kOptionQueueProcessing:
			return mbQueueProcessingEnabled  ? 1 : 0;
		case kOptionClearQueue:
			return mbClearQueueEnabled  ? 1 : 0;
		case kOptionRefCount:
			return mbRefCountEnabled  ? 1 : 0;
		case kOptionTrace:
			return mbTraceEnabled  ? 1 : 0;
	}

	return 0;
}


void Server::SetOption(int option, int value)
{
	// Consider switching options to be an array instead of individual variables.
	//EA_ASSERT_MESSAGE(!mbTraceEnabled || mHandlerInfoMap.empty(), "Server::SetOption: Should only be called before the server is first used.");
	EA_ASSERT(!mbTraceEnabled || mHandlerInfoMap.empty()); // Server::SetOption: Should only be called before the server is first used.

	switch (option)
	{
		case kOptionThreadSafe:
			mbThreadSafetyEnabled = (value != 0);
			break;
		case kOptionReentrancy:
			mbReentrantSafetyEnabled = (value != 0);
			break;
		case kOptionQueueProcessing:
			mbQueueProcessingEnabled = (value != 0);
			break;
		case kOptionClearQueue:
			mbClearQueueEnabled = (value != 0);
			break;
		case kOptionRefCount:
			mbRefCountEnabled = (value != 0);
			break;
		case kOptionTrace:
			mbTraceEnabled = (value != 0);
			break;
	}
}


void Server::MessageSendInternal(MessageId id, HandlerInfoMap& handlerInfoMap, Thread::Futex& mapMutex, void* pMessage) const
{
	// We declare a container of handlers on the stack. This container cannot
	// be a member variable, as this function is re-entrant and multiple
	// such containers could be in use at the same time.
	HandlerInfoArray handlerInfoArray;
	EA_ASSERT(mpCoreAllocator != NULL); // User must have set an allocator.
	handlerInfoArray.set_overflow_allocator(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "HandlerInfoArray", mpCoreAllocator));

	if(mbThreadSafetyEnabled)
		mapMutex.Lock();

	// First we look for any users listening to kMessageIdAll.
	// After we do this, then we'll look for users listening to 'id'.
	MessageId listenerListId = kMessageIdAll;

	BeginProcessing:
	HandlerInfoMap::iterator itMap = handlerInfoMap.find(listenerListId);

	if(itMap != handlerInfoMap.end())
	{
		HandlerInfoList* const pList = (*itMap).second;

		for(HandlerInfoList::iterator itList = pList->begin(); itList != pList->end(); ++itList)
		{
			HandlerInfo& handlerInfo = *itList;

			// If we are reentrant-safe or thread-safe, then we need to make a small temporary list
			// of handlers. The reason for this is that if we didn't do this then we would have 
			// serious problems, with the problem being different for reentrancy and thread safety.
			//    - Reentrancy: If we called the handler directly here and then the 
			//      handler called AddHandler, our data structure would be changed
			//      while we are working on it. This is bad.
			//    - Thread safety: If we call out to an external handler while our mutex
			//      is locked, we have a potential for a thread deadlock.
			if(mbReentrantSafetyEnabled || mbThreadSafetyEnabled)
			{
				handlerInfoArray.push_back(handlerInfo);

				if(mbRefCountEnabled && handlerInfo.mbRefCounted)
					handlerInfo.AddRef();
			}
			else // Else we send the message right away.
			{
				#if EAMESSAGE_C_HANDLER_ENABLED
					if(handlerInfo.mpHandler)
						handlerInfo.mpHandler->HandleMessage(id, pMessage);
					else
						handlerInfo.mpHandlerFunction(id, pMessage, handlerInfo.mpHandlerFunctionContext);
				#else
					handlerInfo.mpHandler->HandleMessage(id, pMessage);
				#endif
			}
		}
	}

	// Now possibly go back and check for listeners listening for 'id'.
	// Note that if 'id' happens to be kMessageIdAll, this code does the right thing.
	if(listenerListId != id)
	{
		listenerListId = id;
		goto BeginProcessing;
	}

	// It's important that we unlock our mutex before calling external code below.
	if(mbThreadSafetyEnabled)
		mapMutex.Unlock();

	// Iterate the array and call the handlers. 
	// If re-entrancy and thread-safety are disabled then the handlerInfoArray will be empty because messages 
	// were handled directly above instead of put into handlerInfoArray.
	for(HandlerInfoArray::iterator it = handlerInfoArray.begin(), itEnd = handlerInfoArray.end(); it != itEnd; ++it)
	{
		HandlerInfo& handlerInfo = *it;

		#if EAMESSAGE_C_HANDLER_ENABLED
			if(handlerInfo.mpHandler)
				handlerInfo.mpHandler->HandleMessage(id, pMessage);
			else
				handlerInfo.mpHandlerFunction(id, pMessage, handlerInfo.mpHandlerFunctionContext);
		#else
			handlerInfo.mpHandler->HandleMessage(id, pMessage);
		#endif

		if(mbRefCountEnabled && handlerInfo.mbRefCounted)
			handlerInfo.Release();
	}
}


void Server::MessageSend(MessageId id, void* pMessage, IHandler* pHandler)
{
	if(EA_UNLIKELY(pHandler))
		pHandler->HandleMessage(id, pMessage);
	else
	{
		MessageSendInternal(id, mQueuedHandlerInfoMap, mQueuedHandlerInfoMapMutex, pMessage);
		MessageSendInternal(id, mHandlerInfoMap,       mHandlerInfoMapMutex,       pMessage);
	}
}


void Server::PopulateHandleInfoArrayForMessage(EA::Messaging::MessageId id, const EA::Messaging::Server::HandlerInfoMap& handlerInfoMap, HandlerInfoArray& handlerInfoArray)
{
	HandlerInfoMap::const_iterator itMap = handlerInfoMap.find(id);

	if(itMap != handlerInfoMap.end())
	{
		HandlerInfoList* const pList = itMap->second;

		if(pList) // Should always be true.
		{
			for(HandlerInfoList::iterator itList = pList->begin(); itList != pList->end(); ++itList)
			{
				HandlerInfo& handlerInfo = *itList;

				if(mbReentrantSafetyEnabled || mbThreadSafetyEnabled)
				{
					handlerInfoArray.push_back(handlerInfo);

					if(mbRefCountEnabled && handlerInfo.mbRefCounted)
						handlerInfo.AddRef();
				}
			}
		}
	}
}


void Server::MessagePost(MessageId id, IMessageRC* pMessage, int nPriority, IHandlerRC* pHandlerRC)
{
	if(mbRefCountEnabled)
	{
		if(pMessage)
			pMessage->AddRef();
		if(pHandlerRC)
			pHandlerRC->AddRef();
	}

	if(!pHandlerRC) // If it's not directed to a particular handler...
	{
		// Post the message to any IQueuedHandlers in mQueuedHandlerInfoMap.
		// In this case we don't insert the message into mMessageQueue for later delivery, but rather
		// we immediately deliver to any IQueuedHandlers. We do this immediately because otherwise 
		// there would be a delay between this function and them recieving the message due to having
		// to wait in two queues instead of one. 
		// We create a temporary array of HandlerInfo because we need to protect against problems that
		// can occur with re-entrancy and thread-safety.

		HandlerInfoArray handlerInfoArray;
		EA_ASSERT(mpCoreAllocator != NULL); // User must have set an allocator.
		handlerInfoArray.set_overflow_allocator(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "HandlerInfoArray", mpCoreAllocator));

		{
			if(mbThreadSafetyEnabled)
				mQueuedHandlerInfoMapMutex.Lock();

			PopulateHandleInfoArrayForMessage(kMessageIdAll, mQueuedHandlerInfoMap, handlerInfoArray);
			PopulateHandleInfoArrayForMessage(id, mQueuedHandlerInfoMap, handlerInfoArray);

			if(mbThreadSafetyEnabled)
				mQueuedHandlerInfoMapMutex.Unlock();
		}

		// Iterate the array and call the handlers.
		for(HandlerInfoArray::iterator it = handlerInfoArray.begin(), itEnd = handlerInfoArray.end(); it != itEnd; ++it)
		{
			EA::Messaging::Server::HandlerInfo& handlerInfo = *it;
			IQueuedHandler* pQueuedHandler = static_cast<IQueuedHandler*>(handlerInfo.mpHandler);
			pQueuedHandler->HandleQueuedMessage(id, pMessage, nPriority);

			if(mbRefCountEnabled && handlerInfo.mbRefCounted)
				handlerInfo.Release(); // This matches the AddRef from PopulateHandleInfoArrayForMessage.
		}
	}

	if(mbThreadSafetyEnabled)
		mMessageQueueMutex.Lock();

	mMessageQueue.InsertMessage(id, pMessage, pHandlerRC, nPriority);

	if(mbThreadSafetyEnabled)
		mMessageQueueMutex.Unlock();
}


#if EAMESSAGE_C_HANDLER_ENABLED
	void Server::MessagePostFunction(MessageId id, IMessageRC* pMessage, int nPriority, 
									  HandlerFunction pHandlerFunction, void* pHandlerFunctionContext)
	{
		if(mbRefCountEnabled)
		{
			if(pMessage)
				pMessage->AddRef();
			if(pHandlerFunction)
				pHandlerFunction(kMessageIdAddRef, NULL, pHandlerFunctionContext);
		}

		if(mbThreadSafetyEnabled)
			mMessageQueueMutex.Lock();

		mMessageQueue.InsertMessage(id, pMessage, pHandlerFunction, pHandlerFunctionContext, nPriority);

		if(mbThreadSafetyEnabled)
			mMessageQueueMutex.Unlock();
	}
#endif


void Server::AddHandlerInternal(MessageId messageId, HandlerInfoMap& handlerInfoMap, EA::Thread::Futex& mapMutex, 
								IHandler* pHandler, HandlerFunction pHandlerFunction, void* pHandlerFunctionContext, bool bRefCounted, int nPriority)
{
	if(pHandler || pHandlerFunction)
	{
		HandlerInfo handlerInfo;

		handlerInfo.mpHandler    = pHandler;
		handlerInfo.mnPriority   = nPriority;
		handlerInfo.mbRefCounted = bRefCounted;

		#if EAMESSAGE_C_HANDLER_ENABLED
			handlerInfo.mpHandlerFunction        = pHandlerFunction;
			handlerInfo.mpHandlerFunctionContext = pHandlerFunctionContext;
		#else
			EA_UNUSED(pHandlerFunction);
			EA_UNUSED(pHandlerFunctionContext);
		#endif

		if(mbRefCountEnabled && bRefCounted)
			handlerInfo.AddRef();

		if(mbThreadSafetyEnabled)
			mapMutex.Lock();

		HandlerInfoMap::iterator it = handlerInfoMap.find(messageId);

		if(it == handlerInfoMap.end())
		{
			EA_ASSERT(mpCoreAllocator != NULL); // User must have set an allocator.

			// Do a placement construction of pList.
			HandlerInfoList* const pList = (HandlerInfoList*)mpCoreAllocator->Alloc(sizeof(HandlerInfoList), EAMESSAGE_ALLOC_PREFIX "List", 0);
			new(pList) HandlerInfoList(EA::Allocator::EASTLICoreAllocator(EAMESSAGE_ALLOC_PREFIX "List", mpCoreAllocator));

			pList->push_back(handlerInfo);

			HandlerInfoMap::value_type mapEntry(messageId, pList);
			handlerInfoMap.insert(mapEntry);
		}
		else
		{
			HandlerInfoList* const pList = (*it).second;
			EA_ASSERT(!mbTraceEnabled || pList);

			HandlerInfoList::iterator itList;

			for(itList = pList->begin(); itList != pList->end(); ++itList)
			{
				const HandlerInfo& handlerInfoTemp = *itList;

				if(handlerInfo.mnPriority >= handlerInfoTemp.mnPriority)
				{
					pList->insert(itList, handlerInfo);
					break;
				}
			}

			if(itList == pList->end()) // If the sorted insertion didn't occur above...
				pList->insert(itList, handlerInfo);

			EA_ASSERT(!mbTraceEnabled || (pList->size() < 512)); // This is just a sanity check to make sure something isn't spiraling out of control.
		}

		if(mbThreadSafetyEnabled)
			mapMutex.Unlock();
	}
}


void Server::AddHandler(IHandler* pHandler, MessageId messageId, bool bRefCounted, int nPriority)
{
	AddHandlerInternal(messageId, mHandlerInfoMap, mHandlerInfoMapMutex, pHandler, NULL, NULL, bRefCounted, nPriority);
}


void Server::AddQueuedHandler(IQueuedHandler* pHandler, MessageId messageId, bool bRefCounted, int nPriority)
{
	AddHandlerInternal(messageId, mQueuedHandlerInfoMap, mQueuedHandlerInfoMapMutex, pHandler, NULL, NULL, bRefCounted, nPriority);
}


#if EAMESSAGE_C_HANDLER_ENABLED
	void Server::AddHandlerFunction(HandlerFunction pHandlerFunction, void* pHandlerFunctionContext, 
									 MessageId messageId, bool bRefCounted, int nPriority)
	{
		AddHandlerInternal(messageId, mHandlerInfoMap, mHandlerInfoMapMutex, NULL, pHandlerFunction, pHandlerFunctionContext, bRefCounted, nPriority);
	}
#endif


bool Server::RemoveHandlerInternal(MessageId messageId, HandlerInfoMap& handlerInfoMap, EA::Thread::Futex& mapMutex, int nPriority, void* pHandler)
{
	bool bReturnValue = false;

	if(mbThreadSafetyEnabled)
		mapMutex.Lock();

	HandlerInfoMap::iterator itMap = handlerInfoMap.find(messageId);

	// Possibly enable this:
	// EA_ASSERT(itMap != handlerInfoMap.end()); // ("Server::RemoveHandler called for unregistered object %p (id 0x%08x).\n", pHandler, messageId)

	if(itMap != handlerInfoMap.end())
	{
		HandlerInfoList* const pList = (*itMap).second;
		HandlerInfoList::iterator itList;

		for(itList = pList->begin(); itList != pList->end(); ++itList)
		{
			HandlerInfo& handlerInfo = *itList;

			#if EAMESSAGE_C_HANDLER_ENABLED
			 if((handlerInfo.mpHandler == pHandler) || ((void*)handlerInfo.mpHandlerFunction == pHandler))
			#else
			 if(handlerInfo.mpHandler == pHandler)
			#endif
			{
				if((nPriority == kPriorityAny) || (handlerInfo.mnPriority == nPriority))
				{
					if(mbRefCountEnabled && handlerInfo.mbRefCounted)
						handlerInfo.Release();
					bReturnValue = true;
					break;
				}
			}
		}

		if(itList != pList->end()) // If the handler was found...
		{
			pList->erase(itList);

			if(pList->empty()) // If there are no more notifications for this type...
			{
				EA_ASSERT(mpCoreAllocator != NULL);

				// Do a placement delete and memory free.
				pList->~HandlerInfoList();
				mpCoreAllocator->Free(pList);

				handlerInfoMap.erase(messageId);
			}
		}
	}

	if(mbThreadSafetyEnabled)
		mapMutex.Unlock();

	return bReturnValue;
}


bool Server::RemoveHandler(IHandler* pHandler, MessageId messageId, int nPriority)
{
	return RemoveHandlerInternal(messageId, mHandlerInfoMap, mHandlerInfoMapMutex, nPriority, pHandler);
}


bool Server::RemoveQueuedHandler(IQueuedHandler* pHandler, MessageId messageId, int nPriority)
{
	return RemoveHandlerInternal(messageId, mQueuedHandlerInfoMap, mQueuedHandlerInfoMapMutex, nPriority, pHandler);
}


#if EAMESSAGE_C_HANDLER_ENABLED
	bool Server::RemoveHandlerFunction(HandlerFunction pHandlerFunction, MessageId messageId, int nPriority)
	{
		return RemoveHandlerInternal(messageId, mHandlerInfoMap, mHandlerInfoMapMutex, nPriority, (void*)pHandlerFunction);
	}
#endif


bool Server::IsHandlerRegisteredInternal(MessageId id, const HandlerInfoMap& handlerInfoMap, Thread::Futex& mapMutex, IHandler* pHandler) const
{
	bool bReturnValue = false;

	if(mbThreadSafetyEnabled)
		mapMutex.Lock();

	if(id == kMessageIdNone)
	{
		// As it currently stands, we have no O(1) means of looking up the handler.
		// We need to walk the entire HandlerInfoMap to find it. 
		for(HandlerInfoMap::const_iterator it = handlerInfoMap.begin(), itEnd = handlerInfoMap.end(); it != itEnd; ++it)
		{
			const MessageId idCurrent = (*it).first;

			if((idCurrent != kMessageIdNone) && IsHandlerRegistered(pHandler, idCurrent))
			{
				bReturnValue = true;
				break;
			}
		}
	}
	else
	{
		HandlerInfoMap::const_iterator itMap = handlerInfoMap.find(id);

		if(itMap != handlerInfoMap.end())
		{
			HandlerInfoList* const pList = (*itMap).second;

			for(HandlerInfoList::const_iterator itList = pList->begin(); itList != pList->end(); ++itList)
			{
				const HandlerInfo& handlerInfo = *itList;

				if(handlerInfo.mpHandler == pHandler)
				{
					bReturnValue = true;
					break;
				}
			}
		}
	}

	if(mbThreadSafetyEnabled)
		mapMutex.Unlock();

	return bReturnValue;
}


bool Server::IsHandlerRegistered(IHandler* pHandler, MessageId id) const
{
	return IsHandlerRegisteredInternal(id, mHandlerInfoMap, mHandlerInfoMapMutex, pHandler);
}


size_t Server::ProcessQueue()
{
	bool       bResult = false;
	size_t     nCount = 0;                                      // The number of messages from the queue that we processed.
	uint64_t   nLastMessageIndex = 0;                           // This is one-past the last message we will process during this run.
	const bool bThreadSafetyEnabled = mbThreadSafetyEnabled;    // Make a temporary in case the member value changes while we are executing. 

	MessageQueue::Message message = { 
		kMessageIdNone,
		NULL,
		NULL,
		kPriorityNormal
	  #if EAMESSAGE_C_HANDLER_ENABLED
	   ,NULL,
		NULL
	  #endif
	};

	// We have this 'do' loop like this because we are making sure the critical section
	// is unlocked when we call MessageSend. This greatly reduces potential deadlocks.
	do
	{
		if(bThreadSafetyEnabled)
			mMessageQueueMutex.Lock();

		if(nCount == 0) // If this is the first time through the loop... get the value for nLastMessageIndex.
		{
			// Get a count of the number of messages we expect to process to clear the queue.
			nLastMessageIndex = (mnQueueProcessedMessageCount + mMessageQueue.GetMessageCount());
		}

		bResult = mMessageQueue.GetNextMessage(message);   // Returns false if the queue was empty and thus there is no message.

		if(bResult)
		{
			++mnQueueProcessedMessageCount;
			++nCount;
		}

		if(bThreadSafetyEnabled)
		{
			// EA_ASSERT_MESSAGE(!mbTraceEnabled || (mMessageQueueMutex.GetLockCount() == 1), ("Messaging::Server: Queue mutex needs to be unlocked before proceeding.\n"));
			EA_ASSERT(!mbTraceEnabled || (mMessageQueueMutex.GetLockCount() == 1)); // Messaging::Server: Queue mutex needs to be unlocked before proceeding.
			mMessageQueueMutex.Unlock();
		}

		// It's very important that we have no member mutexes locked by this thread.
		// Otherwise the opportunity for deadlock exists. Always keep mutexes unlocked
		// when calling external code.
		if(bResult)
		{
			if(message.mpHandlerRC) // If we are targeting a specific handler...
			{
				message.mpHandlerRC->HandleMessage(message.mId, message.mpMessageRC);
				if(mbRefCountEnabled)
					message.mpHandlerRC->Release();
			}
			#if EAMESSAGE_C_HANDLER_ENABLED
			else if(message.mpHandlerFunction)
			{
				message.mpHandlerFunction(message.mId, message.mpMessageRC, message.mpHandlerFunctionContext);
				if(mbRefCountEnabled)
					message.mpHandlerFunction(kMessageIdRelease, NULL, message.mpHandlerFunctionContext);
			}
			#endif
			else
				MessageSendInternal(message.mId, mHandlerInfoMap, mHandlerInfoMapMutex, message.mpMessageRC);

			if(message.mpMessageRC && mbRefCountEnabled)
				message.mpMessageRC->Release();
		}

	} while(bResult && (mbClearQueueEnabled || (mnQueueProcessedMessageCount.GetValue() < nLastMessageIndex)));

   return nCount;
}


size_t Server::ProcessQueue(size_t nMinCycleCount, size_t nMaxCycleCount, size_t nMaxTimeMilliseconds)
{
	size_t nCycleCount(0);
	size_t nMessageCount(0);

	if(mbQueueProcessingEnabled)
	{
		const size_t timeEnd = GetMillisecondTime() + nMaxTimeMilliseconds;

		while((nCycleCount < nMaxCycleCount) && mMessageQueue.GetMessageCount())
		{
			nMessageCount += ProcessQueue();

			if((nCycleCount > nMinCycleCount) && (GetMillisecondTime() >= timeEnd))
				break;
			nCycleCount++;
		}
	}

	return nMessageCount;
}


void Server::ReserveMessageIdCapacity(size_t messageIdCount)
{
	// First get the ideal bucket count for the messageIdCount and the actual bucket count.
	// Increase the bucket count if idealBucketCount > currentBucketCount. 
	const eastl_size_t idealBucketCount   = mHandlerInfoMap.rehash_policy().GetBucketCount((uint32_t)messageIdCount);
	const eastl_size_t currentBucketCount = mHandlerInfoMap.bucket_count();

	if(idealBucketCount > currentBucketCount)
		mHandlerInfoMap.rehash(idealBucketCount);
}


int Server::Lock(bool bLock)
{
	if(mbThreadSafetyEnabled)
	{
		if(bLock)
		{
			mHandlerInfoMapMutex.Lock();
			mMessageQueueMutex.Lock();
		}
		else
		{
			mHandlerInfoMapMutex.Unlock();
			mMessageQueueMutex.Unlock();
		}

		return mMessageQueueMutex.GetLockCount();
	}

	return 0;
}



} // namespace messaging

} // namespace EA




#if EAMESSAGE_DLL // If this library is built to be a DLL... provide our own default new/delete-based ICoreAllocator.

	namespace EA
	{
		namespace IO
		{
			class CoreAllocatorMalloc : public EA::Allocator::ICoreAllocator
			{
			public:
				void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/)
					{ return new char[size]; }

				void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/, unsigned int /*align*/, unsigned int /*alignOffset*/ = 0)
					{ return new char[size]; }

				void  Free(void* p, size_t /*size*/ = 0)
					{ delete[] (char*)p; }
			};

			CoreAllocatorMalloc gCoreAllocatorMalloc;
		}


		namespace Allocator
		{
			ICoreAllocator* ICoreAllocator::GetDefaultAllocator()
			{
				return &IO::gCoreAllocatorMalloc;
			}
		}

	} // namespace EA

#endif







