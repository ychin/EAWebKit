/////////////////////////////////////////////////////////////////////////////
// EAMessageDefer.h
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAMESSAGE_EAMESSAGEDEFER_H
#define EAMESSAGE_EAMESSAGEDEFER_H


#include <EAMessage/internal/Config.h>
#include <EABase/eabase.h>
#include <EAMessage/EAMessage.h>
#include <EAMessage/EAMessageExtra.h>
#include <EAStdC/EACallback.h>
#include <eathread/eathread_atomic.h>
#include <stddef.h>



namespace EA
{
	namespace Messaging
	{
		// Forward Declarations
		class IMessageRC;
		class IHandler;
		class Server;


		/// DeferredMessage
		///
		/// Implements a message that is sent after a certain amount of time.
		/// This is like a posted message but the delivery doesn't just occur
		/// upon the next message management cycle but instead after a certain
		/// amount of time or events (e.g. vblanks).
		///
		/// Example usage:
		///    DeferredMessage dM;
		///    SomeMessage* pMessage = new SomeMessage;
		///    dm.MessagePost(1000, 0x12345678, pMessage, NULL, pServer);
		///
		class DeferredMessage
		{
		public:
			/// MessagePost
			///
			/// This function is guaranteed to succeed as long as a valid Server
			/// is supplied or available. If successful, pMessageRC and pHandlerRC
			/// (if non-NULL) will be AddRefd once each; the Release of each will 
			/// occur upon delivery of the message.
			///
			/// Currently this is a member function and not a static member function,
			/// even though there is no member data. We want to reserve the right to 
			/// have member data in the future.
			///
			/// To consider: Allow the ability to specify sync or async callback operation.
			///
			bool MessagePost(uint32_t nTimeDeltaMS, MessageId id, IMessageRC* pMessageRC, IHandlerRC* pHandlerRC = NULL, Server* pServer = NULL);
		};



		/// ThreadSafeCallback
		///
		/// Implements a callback that is done via EA::Callback but the callback
		/// occurs in the main thread instead of the Callback thread. A primary use
		/// of this is to allow a system to receive callbacks in a thread-safe way
		/// because Callback may occur outside the main thread but the recipient
		/// wants the callback to occur in the main thread.
		///
		/// Since this class works by transferring information from one thread to
		/// another, the precision of this callback is necessarily lower than
		/// when using EA::Callback directly.
		///
		/// This class is a subclass of IHandlerRC because this class needs to be a
		/// message handler itself and because the asynchronous nature of this 
		/// functionality requires reference counting in order to work as desired. 
		///
		/// Example usage:
		///    void SomeCallbackFunction(void* pContext, uint32_t absTime, uint32_t deltaTime);
		///
		///    ThreadSafeCallback* pThreadSafeCallback = new ThreadSafeCallback;
		///    pThreadSafeCallback->AddRef();  // AddRef for our own ownership.
		///    pThreadSafeCallback->Start(1000, true, SomeCallbackFunction, NULL, pServer);
		///    ...
		///    pThreadSafeCallback->Release(); // Release when we are done with it.
		///
		class ThreadSafeCallback : public IHandlerRC
		{
		public:
			ThreadSafeCallback();
			virtual ~ThreadSafeCallback();

			bool Start(uint32_t nPeriodMS, bool bOneShot, EA::Callback::CallbackFunctionType pFunction, void* pContext = NULL, Server* pServer = NULL);
			bool Stop();

			// IHandlerRC functions
			bool HandleMessage(MessageId messageId, void* pMessage);
			int  AddRef();
			int  Release();

		protected:
			friend void ThreadSafeCallbackImp(void*, uint32_t, uint32_t);

			EA::Thread::AtomicInt32                  mnRefCount; /// Refcounting in a multithreaded callback system requires thread-safety.
			Server*                                  mpServer;
			EA::StdC::Callback                       mCallback;
			EA::StdC::Callback::CallbackFunctionType mpCallbackFunction;
			void*                                    mpCallbackFunctionContext;
		};



		/// CallbackMessage
		///
		/// Implements a Callback (EA::Callback) that sends or posts a callback event 
		/// to a given recipient instead of a calling a member function. A primary use
		/// of this is to allow a system to receive Callbacks via messaging instead of
		/// via a specific member function. This allows some systems to be written in
		/// a more generic way and have reduced dependencies, assuming those systems
		/// are already messaging-savvy.
		///
		/// If the delivery method is kDeliverySend, then the send occurs directly from
		/// a Callback and thus might not occur within the main thread. If the delivery
		/// method is kDeliveryPost, the deliver occurs via the processing of the server
		/// message queue, which usually happens in the main thread but is dependent on
		/// the use of the Server.
		///
		/// To consider:
		///    Allow for this CallbackMessage object to be refcounted by the Callback system.
		///
		/// Example usage:
		///    CallbackMessage callbackMessage;
		///    callbackMessage.Start(1000, false, kDeliveryPost, 0x12345678, NULL, pServer);
		///    ...
		///    callbackMessage.Stop();
		///
		class CallbackMessage
		{
		public:
			/// CallbackMessageData
			/// callbackMessageData.GetUint32(0) is the absolute time.
			/// callbackMessageData.GetUint32(1) is the delta time.
			typedef MessageBasicRC<2> CallbackMessageData; 

			CallbackMessage();
		   ~CallbackMessage();

			bool Start(uint32_t nPeriodMS, bool bOneShot, Delivery delivery, MessageId id, IHandlerRC* pHandlerRC = NULL, Server* pServer = NULL);
			bool Stop();

		protected:
			friend void CallbackMessageImp(void* pContext, uint32_t nTime, uint32_t nTimeDelta);

			Callback    mCallback;
			Delivery    mDelivery;
			MessageId   mId;
			IHandlerRC* mpHandlerRC;
			Server*     mpServer;
		};


	} // namespace Messaging


} // namespace EA


#endif // EAMESSAGE_EAMESSAGEDEFER_H

















