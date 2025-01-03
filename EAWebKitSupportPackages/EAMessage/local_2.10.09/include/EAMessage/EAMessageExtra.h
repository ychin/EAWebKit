/////////////////////////////////////////////////////////////////////////////
// EAMessageExtra.h
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAMESSAGE_EAMESSAGEEXTRA_H
#define EAMESSAGE_EAMESSAGEEXTRA_H


#include <EABase/eabase.h>
#include <EAMessage/EAMessage.h>
#include <eathread/eathread_atomic.h>
#include <stddef.h>


#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
	#pragma warning(disable: 4275)  // non dll-interface class used as base for dll-interface class.
#endif


namespace EA
{
	namespace Messaging
	{
		/// QueuedHandler
		///
		/// Implements a handler which maintains a private queue. 
		/// This is a utility class which implements the IQueuedHandler interface.
		/// QueuedHandler can be registered like a regular IHandler but messages can be only posted to its
		/// queue and not sent to it, because message queues can hold only ref-counted (IMessageRC) messages.
		/// Messages sent to a QueuedHandler as opposed to posted are not queued but delivered as usual
		/// via the HandleMessage function.
		///
		///    QueuedHandler myQueuedHandler(pCoreAllocator);
		///    myQueuedHandler.SetHandler(pSomeHandler); // This is optional
		///
		///    gpServer->AddQueuedHandler(&myQueuedHandler, 0x12345678, false); }
		///
		///    MessageBasicRC<1>* pMessage = new MessageBasicRC<1>;
		///    pServer->MessagePost(0x12345678, pMessage);
		///
		///    // Later in handler's thread:
		///    myQueuedHandler.ProcessQueue();          // If pSomeHandler was associated with myQueuedHandler like in the example above then the messages will be sent to it here.
		///        or
		///    myQueuedHandler.GetMessageQueue()->...;  // Directly read the queue.
		///
		class EAMESSAGE_API QueuedHandler : public EA::Messaging::IQueuedHandler
		{
		public:     
			QueuedHandler(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL, bool threadSafe = true);

			/// SetAllocator
			/// Sets the memory allocator to use with this class (in particular mQueue).
			/// This function must be called before any messages are handled by the instance.
			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			/// SetHandler
			/// Allows you to set a handler to pass messages onto. This is optional, as you can
			/// alternatively subclass QueuedHandler and intercept the messages directly.
			void SetHandler(IHandler* pHandler);

			/// SetHandlerFunction
			/// Same as SetHandler but lets you set a C function.
			void SetHandlerFunction(HandlerFunction pHandlerFunction, void* pHandlerFunctionContext);

			/// Inherited from IHandler.
			/// Calls mpHandlerRC if it's non-NULL, else call mpHandlerFunction if it's non-NULL.
			bool HandleMessage(MessageId messageId, void* pMessage);

			/// Passes on the message to our mpHandlerFunction if there is one.
			bool HandleQueuedMessage(MessageId messageId, EA::Messaging::IMessageRC* pMessageRC, int priority);

			/// ProcessQueue
			/// Does one cycle of queue processing. 
			/// Messages are delivered to the HandleMessage function.
			/// Returns the number of messages processed.
			/// This function would typically be called by the message recipient, possibly in its own thread.
			size_t ProcessQueue();
	
			/// ProcessQueue
			///
			/// Does multiple cycles of queue processing.
			/// Messages are delivered to the HandleMessage function.
			/// Returns the number of messages which were processed.
			/// This function is useful for the case where you want to process the queue
			/// repeatedly in one shot. This can be useful because sometimes the delivery
			/// of a queued message results in the queuing of additional messages and 
			/// the user may want to make sure that the additional messages are processed
			/// as well as the original ones.
			/// This function would typically be called by the message recipient, possibly in its own thread.
			///
			/// Example usage:
			///    mHandler.ProcessQueue(5, 10, 1000);
			///
			/// Disabled until it might be needed.
			/// size_t ProcessQueue(size_t nMinCycleCount, size_t nMaxCycleCount = (size_t)-1, size_t nMaxTimeMilliseconds = (size_t)-1);

			/// GetMessageQueue
			/// If you don't want to call ProcessQueue and have it push the messages onto a handler, 
			/// you can call manually pull the messages from the queue via the MessageQueue.
			MessageQueue* GetMessageQueue()
				{ return &mMessageQueue; }

		protected:
			QueuedHandler(const QueuedHandler&) : IQueuedHandler() {}
			void operator=(const QueuedHandler&){}

		protected:
			MessageQueue    mMessageQueue;              /// Queue of all messages delivered to this handler.
			IHandler*       mpHandler;                  /// 
			HandlerFunction mpHandlerFunction;          /// 
			void*           mpHandlerFunctionContext;   /// 
		};



		/// Handler
		///
		/// This templated class allows you to have a class which recieves messages to 
		/// a user-defined member function as opposed to a function named
		/// HandleMessage. This may be useful for the case that you have a class
		/// which you want to handle multiple messages, each via a different 
		/// function. It's also useful for that case that you don't want your 
		/// class to inherit from an IHandler.
		///
		/// Example usage:
		///    struct MultiHandler{
		///       MultiHandler() : mHandleMessage3(this, &MultiHandler::HandleMessage3), 
		///                        mHandleMessage4(this, &MultiHandler::HandleMessage4)
		///       {
		///           pMessageManager->AddHandler(&mHandleMessage3, 0x33333333);
		///           pMessageManager->AddHandler(&mHandleMessage4, 0x44444444);
		///       }
		///
		///       ~MultiHandler()
		///       {
		///           pMessageManager->RemoveHandler(&mHandleMessage3, 0x33333333);
		///           pMessageManager->RemoveHandler(&mHandleMessage4, 0x44444444);
		///       }
		///
		///       bool HandleMessage3(MessageId id, void* pMsg);
		///       bool HandleMessage4(MessageId id, void* pMsg);
		///
		///      protected:
		///       Handler<MultiHandler> mHandleMessage3;
		///       Handler<MultiHandler> mHandleMessage4;
		///    };
		///
		template <typename T>
		class Handler : public EA::Messaging::IHandler
		{
		public:
			typedef bool (T::*MemberFunction)(EA::Messaging::MessageId messageId, void* pMessage);

			Handler(T* pT = NULL, MemberFunction pMemberFunction = NULL)
				: EA::Messaging::IHandler(), mParent(pT), mMemberFunction(pMemberFunction) { }

			void Assign(T* pT, MemberFunction pMemberFunction)
			{ 
				mParent = pT; 
				mMemberFunction = pMemberFunction; 
			}

			virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
			{
				return (mParent->*(mMemberFunction))(messageId, pMessage);
			}

		protected :
			T*             mParent;
			MemberFunction mMemberFunction;     
		};



		/// HandlerMemberFunction
		///
		/// This is an adapter class that allows a class member function to be a message
		/// handler without being associated with an Handler interface and without 
		/// necessarily being a virtual function. This class works via the Server 
		/// AddHandler function:
		///     void AddHandler(HandlerFunction, void* pHandlerFunctionContext, MessageId messageId, bool bRefCounted, int nPriority);
		/// The first argument to AddHandler must be this class and the second argument 
		/// must be the address of this class. See the example below.
		///
		/// Example usage:
		///    struct Widget{
		///       bool HandleMessage(MessageId id, void* pMsg);
		///       HandlerMemberFunction<Widget> mHandlerMemberFunction;
		///       Widget() : mHandlerMemberFunction(this, &HandleMessage){}
		///    };
		///
		///    Widget* pWidget = new Widget;
		///    pMessageManager->AddHandlerFunction(pWidget->mHandlerMemberFunction, &pWidget1->mHandlerMemberFunction, 0x12345678);
		///    pMessageManager->RemoveHandlerFunction(pWidget->mHandlerMemberFunction, 0x12345678);
		///
		template <typename T>
		class HandlerMemberFunction
		{
		public:
			typedef bool (T::*MemberFunction)(MessageId messageId, void* pMessage);
			  
			HandlerMemberFunction(T* pT = NULL, MemberFunction pMemberFunction = NULL)
				: mpT(pT), mpMemberFunction(pMemberFunction) { }

			void Assign(T* pT, MemberFunction pMemberFunction)
				{ mpT = pT; mpMemberFunction = pMemberFunction; }

			static bool HandleMessage(MessageId messageId, void* pMessage, void* pContext)
			{
				HandlerMemberFunction* pHMF = (HandlerMemberFunction*)pContext;
				return ((pHMF->mpT)->*(pHMF->mpMemberFunction))(messageId, pMessage);
			}

			operator HandlerFunction()
				{ return HandleMessage; }

		protected :
			T*             mpT;
			MemberFunction mpMemberFunction;     
		};



		/// AutoHandler
		///
		/// This class is useful for helping provide error free notification additions and removals in 
		/// the case where an entity needs to manage large numbers of notifications. History has shown 
		/// that code that registers for large numbers of notifications starts to get bogged down in 
		/// code to add and remove notifications. This class reduces both the amount of code that needs
		/// to be written and reduces the chance of error due to mismatched handler addition/removal. 
		///
		/// Note that AutoHandler uses a reference to the id array and not a copy of the id array.
		/// This is done like so in order to prevent memory allocations from occurring. However, it 
		/// results in the user having to make the id array available at the time of the AutoHandler
		/// destruction. 
		///
		/// Example Usage:
		///     const uint32_t ids[] = { 0x11111111, 0x22222222, 0x33333333 };
		///
		///     class Widget : public Handler{
		///         AutoHandler mAutoHandler;
		///
		///         Widget(){
		///             mAutoHandler.AddHandler(pMessageServer, this, ids, 3);
		///         }
		///
		///         int HandleMessage(MessageId messageId, void* pMessage);
		///     };
		///     
		class AutoHandler
		{
		public:
			AutoHandler()
				: mpServer(NULL), mpHandler(NULL), mpIdArray(NULL), mnIdArrayCount(0), mnPriority(0) {}

		   ~AutoHandler()
			{
				if(mpServer)
					RemoveHandler();
			}

			void AddHandler(Server* pServer, IHandler* pHandler, 
							const MessageId* pIdArray, size_t nIdArrayCount, 
							bool bRefCounted, int nPriority = kPriorityNormal)
			{
				// EA_ASSERT((mnIdArrayCount == 0) && (mpIdArray == NULL));
				mpServer       = pServer;
				mpHandler      = pHandler;
				mpIdArray      = pIdArray;
				mnIdArrayCount = nIdArrayCount;
				mnPriority     = nPriority;

				if(pServer && pHandler && mpIdArray)
					Messaging::AddHandler(pServer, pHandler, pIdArray, nIdArrayCount, bRefCounted, nPriority);
			}

			bool RemoveHandler()
			{
				bool bResult = false;

				// EA_ASSERT(mpHandler && mpIdArray);

				if(mpServer)
				{
					bResult = Messaging::RemoveHandler(mpServer, mpHandler, mpIdArray, mnIdArrayCount, mnPriority);
					mpServer = NULL;
				}
				return bResult;
			}

		protected:
			Server*          mpServer;      /// Stores the server, but is also used to tell if we've unregistered.
			IHandler*        mpHandler;
			const MessageId* mpIdArray;
			size_t           mnIdArrayCount;
			int              mnPriority;
		};


		/// class MessageRC
		///
		/// Implements a thread-safe, reference-counted message class suitable
		/// for use as a base class for custom message class implementations.
		///
		/// Note that this class uses 'delete this' in its Release method.
		/// This results in global 'delete' being called on the class and thus
		/// means that the class must be allocated with global 'new'. However,
		/// you can avoid this limitation via at least one of the following:
		///     - Make a subclass of MessageRC that overrides Release.
		///     - Make a copy of MessageRC that works differently.
		///     - Using the ZoneObject mixin from the MemoryMan package 
		///       or something similar (which stores an allocator).
		///
		class MessageRC : public IMessageRC
		{
		public:
			MessageRC() : mRefCount(0) { };
			virtual ~MessageRC() { };

			virtual int AddRef()
			{
				return static_cast<int>(mRefCount.Increment());
			}

			virtual int Release()
			{
				int32_t nRefCount = mRefCount.Decrement();
				if(nRefCount) 
					return nRefCount;
				mRefCount.SetValue(1); // Prevent double destroy if AddRef/Release
				delete this;           // is called while in destructor. This can happen
				return 0;              // if the destructor does things like debug traces.
			}

		protected:
			EA::Thread::AtomicInt32 mRefCount;
		};


		/// class MessageBasic
		///
		/// Provides a simple message class which stores arbitrary basic data.
		/// You can use this class for simple messages that pass basic data
		/// to recipients. In the past we've found that a surprisingly large
		/// percentage of messages can be done this way.
		///
		/// Example usage:
		///    MessageBasic<3> message;
		///    message.SetUint32(1, 0x12345678);
		///    uint32_t val = message.GetUint32(1);
		///
		/// Example usage:
		///   In this case we have a subclass that does something smart in 
		///   its destructor with owned data:
		///
		///   struct Subclass : public MessageBasic<1>{
		///      Subclass(Blah* pBlah) { SetVoid(0, pBlah); }
		///     ~Subclass()            { Blah* pBlah = GetVoid(0); delete pBlah; } 
		///   };
		///
		template <size_t N>
		class MessageBasic
		{
		public:
			enum { count = N };

			union Data // This union is like a very small 'Variant' implementation.
			{
				uint32_t mUint32;
				uint64_t mUint64;
				float    mFloat;
				double   mDouble;
				void*    mpVoid;
			};

			Data      mData[N];
			MessageId mId;

			MessageBasic(MessageId id = 0) : mId(id) { }

			uint32_t GetUint32(size_t i) const       { return mData[i].mUint32; }
			void     SetUint32(size_t i, uint32_t n) { mData[i].mUint32 = n;    }

			uint64_t GetUint64(size_t i) const       { return mData[i].mUint64; }
			void     SetUint64(size_t i, uint64_t n) { mData[i].mUint64 = n;    }

			float    GetFloat(size_t i) const        { return mData[i].mFloat;  }
			void     SetFloat(size_t i, float f)     { mData[i].mFloat = f;     }

			double   GetDouble(size_t i) const       { return mData[i].mDouble; }
			void     SetDouble(size_t i, double d)   { mData[i].mDouble = d;    }

			void*    GetVoidPtr(size_t i) const      { return mData[i].mpVoid; }
			void     SetVoidPtr(size_t i, void* p)   { mData[i].mpVoid = p;    }
		};


		/// class MessageBasicRC
		///
		/// This is a version of MessageBasic which implements reference counting.
		/// This class can be used for Server::MessagePost, as that function 
		/// uses IMessageRC (IMessage, Reference Counted).
		///
		/// Instances of this class need to be created via some kind of heap and 
		/// cannot be created on the stack. This is because their lifetime is 
		/// necessarily indeterminate. However, such objects can be located in a 
		/// special nonfragmenting fixed size heap in order to optimize their allocation.
		///
		/// Example usage:
		///    MessageBasicRC<1>* pMBRC = new MessageBasicRC<1>;
		///    pServer->MessagePost(0x12345678, pMBRC);
		///
		template <size_t N>
		class MessageBasicRC : public MessageBasic<N>, public MessageRC
		{
		public:
			MessageBasicRC(MessageId id = 0)
				:  MessageBasic<N>(id)
				{ }
		};

	} // namespace Messaging


} // namespace EA


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // EAMESSAGE_EAMESSAGEEXTRA_H







