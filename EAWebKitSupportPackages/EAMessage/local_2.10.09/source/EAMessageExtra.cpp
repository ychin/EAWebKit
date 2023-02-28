/////////////////////////////////////////////////////////////////////////////
// EAMessageExtra.cpp
//
// Copyright (c) 2013, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Electronic Arts
//
/////////////////////////////////////////////////////////////////////////////


#include <EAMessage/internal/Config.h>
#include <EAMessage/EAMessageExtra.h>


namespace EA{

namespace Messaging{


QueuedHandler::QueuedHandler(EA::Allocator::ICoreAllocator* pCoreAllocator, bool threadSafe)
  : mMessageQueue(pCoreAllocator, threadSafe)
  , mpHandler(NULL)
  , mpHandlerFunction(NULL)
  , mpHandlerFunctionContext(NULL)
{
}


void QueuedHandler::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mMessageQueue.SetAllocator(pCoreAllocator);
}


void QueuedHandler::SetHandler(IHandler* pHandler)
{
	mpHandler = pHandler;
}


void QueuedHandler::SetHandlerFunction(HandlerFunction pHandlerFunction, void* pHandlerFunctionContext)
{
	mpHandlerFunction        = pHandlerFunction;
	mpHandlerFunctionContext = pHandlerFunctionContext;
}


bool QueuedHandler::HandleMessage(MessageId messageId, void* pMessage)
{
	if(mpHandler)
		return mpHandler->HandleMessage(messageId, pMessage);
	else if(mpHandlerFunction)
		return mpHandlerFunction(messageId, pMessage, mpHandlerFunctionContext);

	// We don't do anything with the message, and the return value is irrelevant.
	return false;
}


bool QueuedHandler::HandleQueuedMessage(MessageId messageId, EA::Messaging::IMessageRC* pMessageRC, int priority)
{
	// To consider: Do we want our own thread-safety here? mMessageQueue is already thread-safe by default.

	pMessageRC->AddRef(); // Release will be called during ProcessQueue.
	mMessageQueue.InsertMessage(messageId, pMessageRC, NULL, priority);

	// The return value is irrelevant.
	return false;
}


size_t QueuedHandler::ProcessQueue()
{
	// To consider: Do we want our own thread-safety here? mMessageQueue is already thread-safe by default.

	size_t nCount = 0;
	EA::Messaging::MessageQueue::Message message;

	while(mMessageQueue.GetNextMessage(message))
	{
		nCount++;
		HandleMessage(message.mId, message.mpMessageRC);
		message.mpMessageRC->Release();
	}

	return nCount;
}



} // namespace messaging

} // namespace EA











