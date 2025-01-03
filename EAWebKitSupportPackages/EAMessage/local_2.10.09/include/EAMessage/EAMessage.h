/////////////////////////////////////////////////////////////////////////////
// EAMessage.h
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana, Maxis
/////////////////////////////////////////////////////////////////////////////


#ifndef EAMESSAGE_EAMESSAGE_H
#define EAMESSAGE_EAMESSAGE_H


#include <EABase/eabase.h>
#include <EAMessage/internal/Config.h>
#include <eathread/eathread_futex.h>
#include <eathread/eathread_atomic.h>
#include <eathread/eathread_rwmutex.h>
#include <EASTL/hash_map.h>
#include <EASTL/list.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/core_allocator_adapter.h>
#include <coreallocator/icoreallocator_interface.h>
#include <stddef.h>


#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
	#pragma warning(disable: 4275)  // non dll-interface class used as base for dll-interface class.
#endif


/// namespace EA
/// The standard Electronic Arts namespace.
namespace EA
{
	/// namespace Messaging
	/// Messaging namespace, used for client/server messaging or event propogation.
	namespace Messaging
	{
		// Forward declarations
		class IMessageRC;
		class IHandler;
		class IHandlerRC;
		class IQueuedHandler;


		/// MessageId
		typedef uint32_t MessageId;


		// Constants
		const MessageId kMessageIdNone    = 0x00000000;    /// Refers specifically to no messages. See class and function documentation for applicability.
		const MessageId kMessageIdAll     = 0xffffffff;    /// Refers specifically to all messages. See class and function documentation for applicability.
		const MessageId kMessageIdAddRef  = 0x00000001;    /// Sent to a subscriber to notify it that its reference count has been incremented. A subscriber which implements reference counting would respond by incrementing its own reference count.
		const MessageId kMessageIdRelease = 0x00000002;    /// Sent to a subscriber to notify it that its reference count has been decremented. A subscriber which implements reference counting would respond by decrementing its own reference count.

		/// TypeId
		/// Used at runtime to identify different kinds of handlers.
		// typedef uint32_t TypeId; Disabled because we are probably not going to use this TypeId system.
		

		/// enum Priority
		/// This enumeration defines messaging system priorities. In practice, priorities are implemented
		/// simply as signed integers whereby higher values denote higher priority. As such, this enumeration
		/// is more of a placeholder than a set of values that must be adhered to.
		enum Priority
		{
			kPriorityAny    = -9999,    /// This is a special value used to mean to match any priority during a RemoveHandler call.
			kPriorityLow    = -1000,
			kPriorityNormal =     0,
			kPriorityHigh   =  1000
		};


		/// enum Delivery
		/// Defines how messages are delivered.
		enum Delivery
		{
			kDeliverySend,  /// The message is delivered immediately.
			kDeliveryPost   /// The message is put into a queue for later delivery.
		};


		/// IMessageRC
		///
		/// Provides an interface for reference counted messages. Only queue (delayed)
		/// messages need to be IMessageRC objects. Regular sent messages can be of any
		/// type of object, including IMessageRC objects.
		///
		/// Instances of this interface need to be created via some kind of heap and 
		/// cannot be created on the stack. This is because their lifetime is 
		/// necessarily indeterminate. However, such objects can be located in a special
		/// nonfragmenting fixed size heap in order to optimize their allocation.
		///
		class IMessageRC
		{
		public:
			virtual ~IMessageRC() { }

			virtual int AddRef() = 0;
			virtual int Release() = 0;
		};


		/// IHandler
		///
		/// Defines a base class that can process messages. A user would register
		/// such a class with a an entity which generates or distributes messages;
		/// Normally this is the Server class. However, IHandler instances can be 
		/// used outside of the Server class, including the case whereby some piece
		/// of code simply directly calls the HandleMessage function of an IHandler.
		///
		/// The return value usage of the IHandler::HandleMessage function is user-defined.
		/// The Server class does not use this return value, but other users of the 
		/// IHandler may do so. The return value is used in some systems to indicate
		/// whether the handler handled the message or not.
		///
		/// Note that the HandleMessage function doesn't have a mechanism to identify 
		/// the message sender nor other information such as a timestamp. The reason
		/// for this is that history has shown that 95+% of the time the message 
		/// recipient either doesn't care about who sent the message, or always knows
		/// who sent it because there is only one sender. Thus sender identification is 
		/// something that should be added to the Message subclass on an as-needed basis.
		/// As for time stamps and other similar metadata, history has shown that this 
		/// is best handled in the message server in an application-specific way, as there
		/// is no single type of metadata that all users want or need.
		///
		/// Example usage:
		///    class Widget : public Messaging::IHandler{
		///        bool HandleMessage(MessageId id, void* pMsg);
		///    };
		///
		///    Widget* pWidget = new Widget;
		///    pMessageServer->AddHandler(pWidget, 0x12345678);
		///    pMessageServer->RemoveHandler(pWidget, 0x12345678);
		///
		class EAMESSAGE_API IHandler
		{
		public:
		  //enum { kTypeId = 0x0ff551a4 };

			virtual ~IHandler() { }

			virtual bool  HandleMessage(MessageId messageId, void* pMessage) = 0;
		  //virtual void* AsType(TypeId typeId);
		};


		/// IHandlerRC
		///
		/// A ref-counted IHandler. Used for safely managing handler lifetimes when conditions call for it.
		/// 
		/// Example usage:
		///     struct HandlerRC : public IHandlerRC
		///     {
		///         HandlerRC() : mRefCount(0) { }
		/// 
		///         bool HandleMessage(MessageId id, void* pMessage) { ...}
		///         int  AddRef() { return mRefCount.Increment(); }
		///         int  Release() { int32_t rc = mRefCount.Decrement(); if(rc) return rc; delete this; return 0; }
		/// 
		///         AtomicInt32 mRefCount;
		///     };
		///
		///     HandlerRC* pHandlerRC = new HandlerRC;
		///     pServer->AddHandler(pHandlerRC, 0x11223344, true); // Calls AddRef. Release will be called upon RemoveHandler.
		///
		class EAMESSAGE_API IHandlerRC : public IHandler
		{
		public:
		  //enum { kTypeId = 0x0ff55c4f };

			virtual int   AddRef() = 0;
			virtual int   Release() = 0;
		  //virtual void* AsType(TypeId typeId);
		};


		/// IQueuedHandler
		///
		/// Base class for having posted messages queued for another thread to read.
		/// This allows you to recieve messages in a private queue that gets processed in your
		/// thread instead of the main thread.
		/// See the QueuedHandler class for a concrete default implementation.
		/// 
		class EAMESSAGE_API IQueuedHandler : public IHandler
		{
		public:
		  //enum { kTypeId = 0x0ff55cb9 };

			virtual ~IQueuedHandler() { }

			// This is inherited from IHandler and is called from the Server when a message is sent.
			virtual bool HandleMessage(MessageId messageId, void* pMessage) = 0;

			/// This is called from the Server when a message is posted. It's called from the thread
			/// that Server::MessagePost was called. 
			/// It's intended that the implementation store the message in a queue of its own which 
			/// is later processed by a different thread, such as a thread that the QueuedHandler is 
			/// associated with. HandQueuedMessage may choose to signal a condition variable or set a 
			/// flag that causes the thread to process the queue. 
			/// The return value is user-defined. The Server class does not use this return value, but other 
			/// users of the IQueuedHandler/IHandler may do so. The return value is used in some systems to 
			/// indicate whether the handler handled the message or not.
			virtual bool HandleQueuedMessage(MessageId messageId, EA::Messaging::IMessageRC* pMessageRC, int priority) = 0;

		  //virtual void* AsType(TypeId typeId);
		};



		/// type_cast
		///
		/// A template function to handle dynamic casting.
		/// No reference counting operations are done as part of the casting.
		///
		/// Example usage:
		///    IHandlerRC* pHandlerRC = type_cast<IHandlerRC>(pHandler)
		///
		//template <typename ToClassPtr, typename FromClass>
		//ToClassPtr type_cast(const FromClass* pCastable)
		//{
		//    return pCastable ? static_cast<ToClassPtr>(const_cast<FromClass*>(pCastable)->AsType(ToClassPtr(NULL)->kTypeId)) : NULL;
		//}



		/// HandlerFunction
		///
		/// Defines a standalone message processing callback function. A user would 
		/// register such a function with an entity which generates or distributes messages.
		///
		/// The return value usage of the HandlerFunction is user-defined.
		/// The Server class does not use this return value, but other users of the 
		/// HandlerFunction may do so. The return value is used in some systems to 
		/// indicate whether the handler handled the message or not.
		///
		/// Example usage:
		///    bool HandleMessage(MessageId id, void* pMsg, void* pContext);
		///
		///    pMessageServer->AddHandler(HandleMessage, 0x12345678, NULL);
		///    pMessageServer->RemoveHandler(HandleMessage, 0x12345678, NULL);
		///
		typedef bool (*HandlerFunction)(MessageId messageId, void* pMessage, void* pContext);



		/// MessageQueue
		///
		/// This class implements a queue of messages sorted by priority.
		/// This is the foundation of a message server implementation, 
		/// but can also be used for similar message queueing uses.
		///
		/// This class has no thread-safety built-in. Users of this class must
		/// therefore implement thread-safe access to this container if that is 
		/// a requirement. Similarly, this class has no message reference counting 
		/// functionality built in. Users of this class must implement referenced 
		/// counting as per their requirements. Both of these design aspects of 
		/// this class are there for performance reasons.
		///
		class EAMESSAGE_API MessageQueue
		{
		public:
			// Currently we use an STL container for storage of queued messages.
			// We may consider switching this to a custom container type that has
			// more tightly controlled memory behaviour. Either way, it's probably 
			// best if we allocate a small non-fragmenting pool of memory to the 
			// message queue top allocate nodes from.
			struct Message
			{
				MessageId   mId; 
				IMessageRC* mpMessageRC;
				IHandlerRC* mpHandlerRC; 
				int         mnPriority;

				#if EAMESSAGE_C_HANDLER_ENABLED
					HandlerFunction mpHandlerFunction;
					void*           mpHandlerFunctionContext;
				#endif
			};

			typedef eastl::list<Message, EA::Allocator::EASTLICoreAllocator> Queue;

		public:
			MessageQueue(bool threadSafe = true);
			MessageQueue(EA::Allocator::ICoreAllocator* pCoreAllocator, bool threadSafe = true);
		   ~MessageQueue();


			/// SetAllocator
			/// Sets the memory allocator to use with this class (in particular mQueue).
			/// This function must be called before any messages are handled by the instance.
			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);


			/// GetMessageCount
			///
			/// Returns the size of the message queue. 
			///
			size_t GetMessageCount() const;


			/// GetNextMessage
			///
			/// Returns the next message in the queue while removing the 
			/// message from the queue. This function does not AddRef nor Release
			/// the returned Message. However, the user of this queue is expected
			/// to AddRef and Release messages upon adding them to and removing
			/// them from the queue.
			///
			bool GetNextMessage(Message& message);


			/// InsertMessage
			///
			/// Inserts the given message with the given priority into the queue. 
			/// If other messages exist in the queue with an equivalent priority, 
			/// the newly inserted message is placed after the others. This function
			/// does not AddRef nor Release the pMessage argument. However, the user 
			/// of this queue is expected to AddRef and Release messages upon adding 
			/// them to and removing them from the queue.
			///
			void InsertMessage(MessageId id, IMessageRC* pMessage, IHandlerRC* pHandler = NULL, int nPriority = kPriorityNormal);
			void InsertMessage(MessageId id, IMessageRC* pMessage, HandlerFunction pHandlerFunction = NULL, void* pHandlerFunctionContext = NULL, int nPriority = kPriorityNormal);
			void InsertMessage(const Message& message);


			/// GetQueue
			///
			/// Returns the Queue object. This is an advanced function which
			/// allows for the implementation of fancy searching and for queue pruning
			/// among other purposes. The user of this queue is expected to AddRef and 
			/// Release messages upon adding them to and removing them from the queue.
			/// If thread safety is enabled then the user is expected to handle that 
			/// manually as well.
			///
			Queue* GetQueue()
				{ return &mQueue; }


			/// Lock
			///
			/// Locks the MessageQueue for thead-safe access over a user-specified period of time.
			/// This is useful for doing multiple actions with the server without being interrupted.
			/// Returns the number of locks after the operation has completed. 
			///
			int Lock(bool bLock);


			/// Options
			///
			/// Specifies user-configurable options.
			///
			enum Options
			{
				kOptionNone,            /// Refers to no options.
				kOptionThreadSafe,      /// Enabled by default. Enables thread safety.
				kOptionCount            /// Count of options.
			};


			/// GetOption
			///
			/// The input 'option' is an int instead of enum Option because having it 
			/// as an int allows subclasses to extend the range of options. If the 
			/// input option is an undefined option, the return value is zero.
			///
			/// Example usage:
			///    bool threadSafe = pMessageQueue->GetOption(MessageQueue::kOptionThreadSafe);
			///
			int GetOption(int option) const;


			/// SetOption
			///
			/// The input 'option' is an int instead of enum Option because having it 
			/// as an int allows subclasses to extend the range of options. If the 
			/// input option is an undefined option, the call has no effect.
			/// All options should be set before first use of the MessageQueue.
			///
			/// Example usage:
			///    pMessageQueue->SetOption(MessageQueue::kOptionThreadSafe, true);
			///
			void SetOption(int option, int value);

		protected:
			MessageQueue(const MessageQueue&){}
			void operator=(const MessageQueue&){}

		protected:
			bool                  mbThreadSafetyEnabled;    /// If true then we use mQueueMutex. Enabled by default.
			Queue                 mQueue;                   /// Implements a message queue.
			mutable Thread::Futex mQueueMutex;              /// 
		};


		/// class Server
		///
		/// Standard message server. This class implements a server which allows the 
		/// registration of handlers for messages and the sending and posting of messages
		/// to those handlers. 
		///
		/// Re-entrancy safety allows the message server to post messages and add/remove
		/// handlers while in the middle of posting messages to handlers. This is rather
		/// important in systems with complex message handling usage patterns.
		///
		/// To consider: Perhaps the Server should be a ref-countable object. Without it
		/// there is the possibility of code using a Server after it has been destroyed.
		///
		class EAMESSAGE_API Server
		{
		public:
			/// Server
			/// 
			/// Constructs an instance of a Server. This function doesn't do anything more
			/// than set member data to a known empty state. You must call the Init function
			/// separately in order to prepare the Server for use; see Init for information
			/// about this.
			///
			Server();
			Server(EA::Allocator::ICoreAllocator* pCoreAllocator);
			Server(const Server&);
			Server& operator=(const Server&);

			/// ~Server
			///
			/// Destructs an instance of a Server. You must call the Shutdown function 
			/// before destructing an instance of Server. This is good practice and eliminates
			/// the possibiliy that a specific implementation of this class falls prey to 
			/// chicken-and-egg dependency problems. 
			///
			EAM_VIRTUAL ~Server();


			/// Init
			///
			/// Initalizes an instance of this class. 
			///
			EAM_VIRTUAL bool Init();


			/// Shutdown
			///
			/// Shuts down an instance of this class. The instance will be restored to 
			/// its state prior to initialization. All notification registrations will
			/// be unregistered and 
			///
			EAM_VIRTUAL bool Shutdown();

			/// SetAllocator
			/// Sets the memory allocator to use with this class.
			/// This allocator is used to allocate mHandlerInfoMap elements
			/// This function must be called right after Init().
			EAM_VIRTUAL void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			/// Options
			///
			/// Specifies user-configurable options.
			///
			enum Options
			{
				kOptionNone,            /// Refers to no options.
				kOptionThreadSafe,      /// Enabled by default. Enables thread safety. Thread safety allows the server to be used from multiple threads concurrently.
				kOptionReentrancy,      /// Enabled by default. Enables reentrancy. When the server is re-entrant-safe, users can call back into the message server while the message server is in the process of directly or indirectly calling the user.
				kOptionQueueProcessing, /// Enabled by default. This option specifies whether queued messages are processed. You can use this option to temporarily delay processing. Multiple calls to disable processing must be matched with multiple calls to enable processing for processing to be resumed.
				kOptionClearQueue,      /// Enabled by default. This option forces the clearing of the posted message queue on DoProcessing. Otherwise, only messages in the queue before DoProcessing are processed.
				kOptionRefCount,        /// Enabled by default. Enables the ability to do reference counting of posted messages and subscribers Doesn't mean that reference counting is forced on all clients; they still have a choice.
				kOptionTrace,           /// Enabled by default. Enables the propogation of traces or assertion failures in debug builds.
				kOptionCount            /// Count of options.
			};


			/// GetOption
			///
			/// The input 'option' is an int instead of enum Option because having it 
			/// as an int allows subclasses to extend the range of options. If the 
			/// input option is an undefined option, the return value is zero.
			///
			/// Example usage:
			///    bool threadSafe = pServer->GetOption(Server::kOptionThreadSafe);
			///
			EAM_VIRTUAL int GetOption(int option) const;


			/// SetOption
			///
			/// The input 'option' is an int instead of enum Option because having it 
			/// as an int allows subclasses to extend the range of options. If the 
			/// input option is an undefined option, the call has no effect.
			/// All options should be set before first use of the Server.
			///
			/// Example usage:
			///    pServer->SetOption(Server::kOptionThreadSafe, true);
			///
			EAM_VIRTUAL void SetOption(int option, int value);


			/// MessageSend
			///
			/// Immediately delivers the given message to subscribers.
			/// The message can be any block of data. The format of that data is defined
			/// by the sender and all messages of the same MessageId must have the same
			/// format.
			///
			/// Any MessageId is valid except for kMessageIdNone, kMessageIdAddRef, and
			/// kMessageIdRelease. A value of kMessageIdAll will result in the message
			/// being sent only to those who have registered for kMessageIdAll.
			///
			/// Note that the message system is reentrant. Thus it is possible that while
			/// a message is being sent, one of the recipients may turn around and send
			/// additional messages do other message system activity while processing
			/// a given message. As a result, you will want to take care to make sure that
			/// you account for this if you need to. 
			///
			/// If pHandler is non-NULL, the message is delivered to the given handler alone.
			/// In this case, the IHandler need not be a handler that has subscribed to 
			/// messages but can be any arbitrary IHandler object.
			///
			/// Example usage:
			///    MessageBlah message;
			///    pServer->MessageSend(0x12345678, &message);
			///
			/// Example usage:
			///    MessageBlah message;
			///    pServer->MessageSend(0x12345678, &message, pSomeHandler);
			///
			EAM_VIRTUAL void MessageSend(MessageId id, void* pMessage, IHandler* pHandler = NULL);


			/// MessagePost
			///
			/// Puts the given message into a queue for delivery the next time the queue is processed.
			/// Among other things, usage of posted messages removes the need for clients to be 
			/// reentrant and thread-safe, but has its downsides as well. Nevertheless, this is 
			/// critical for the communication between threads in systems such as audio.
			///
			/// Messages that are posted are based on the IMessageRC interface. The reason for this
			/// is that IMessageRC provides for reference counting which is used to allow the message
			/// to live for an unspecified time while in the queue. For performance reasons it is 
			/// advised that IMessageRC-based objects avoid using the general heap and instead come
			/// from a pool of objects which are given back to the pool via IMessageRC::Release().
			///
			/// If the input pHandler is NULL, the message is delivered to all subscribers, 
			/// else it is delivered only to the given message handler.
			/// 
			/// The priority parameter allows the message to be moved higher in the queue. By default
			/// messages go at the back of the queue. But if a message is of higher priority than 
			/// any  messages at the back of the queue, it is put ahead of them. 99% of the time
			/// you want to use normal priority. The only times you want to use higher priority
			/// messages is when you have out-of-band items that are generally independent from 
			/// other items and preservation of exact ordering is not important.
			///
			/// The recipient list for a posted message is evaluated at the time the message will be 
			/// delivered and not at the time of the MessagePost call. 
			///
			/// Any MessageId is valid except for kMessageIdNone, kMessageIdAddRef, and
			/// kMessageIdRelease. A value of kMessageIdAll will result in the message
			/// being sent only to those who have registered for kMessageIdAll.
			///
			/// Example usage:
			///    #include <EAMessage/EAMessageExtra.h>
			///    struct MyMessage : public EA::Message::MessageRC { int value; };
			///    MyMessage* pMyMessage = new MyMessage;
			///    pServer->MessagePost(0x12345678, pMyMessage); // The Server will AddRef pMyMessage immediately, and then Release it after the message is later delivered.
			///
			EAM_VIRTUAL void MessagePost(MessageId id, IMessageRC* pMessage, int nPriority = kPriorityNormal, IHandlerRC* pHandlerRC = NULL);

			
			/// MessagePostFunction
			///
			/// Similar to MessagePost for IHandlerRC, except it uses a standalone function as the override destination.
			///
			#if EAMESSAGE_C_HANDLER_ENABLED
				EAM_VIRTUAL void MessagePostFunction(MessageId id, IMessageRC* pMessage, int nPriority = kPriorityNormal, 
													 HandlerFunction pHandlerFunction = NULL, void* pHandlerFunctionContext = NULL);
			#endif


			/// AddHandler
			///
			/// Adds the given message handler to the subscription list for the given message id.
			/// A given handler can be registered more than once; this may be useful for the registation
			/// of a handler for both low and high priorities.
			///
			/// If the bRefCounted argument is true, the IHandler input is assumed to be an IHandlerRC
			/// object and is reference counted by the message server via the IHandlerRC interface, 
			/// which is an extension of the IHandler interface. Reference counting is useful in preventing
			/// problems which occur with highly reentrant usage patterns.
			///
			/// The priority argument specifies whether the given handler is put ahead or behind other 
			/// handlers in the subscriber list. A handler with a high priority receives messages before
			/// handlers with lower priorities.
			///
			/// Any MessageId is valid except for kMessageIdNone, kMessageIdAddRef, and
			/// kMessageIdRelease. A value of kMessageIdAll is a special case which will 
			/// result in the handler receiving all messages that are sent and posted via
			/// the Server. The priority value will uses as with regular messages. 
			///
			/// Example usage:
			///    class Widget : public IHandler {
			///        Widget(){ pServer->AddHandler(this, 0x12345678, false); }
			///       ~Widget(){ pServer->RemoveHandler(this); }
			///        bool HandleMessage(MessageId id, void* pMsg);
			///    };
			///
			EAM_VIRTUAL void AddHandler(IHandler* pHandler, MessageId messageId, bool bRefCounted, int nPriority = kPriorityNormal);


			/// AddQueuedHandler
			///
			/// Similar to AddHandler, except posted messages get delivered to the IQueuedHandler::HandleQueuedMessage function.
			/// You can use an instance of IQueuedHandler (or its subclass QueuedHandler) to recieve messages into a private
			/// queue, usually for the purpose of processing them in another thread.
			/// If you call AddQueuedHandler then your pHandler will recieve posted messages via its HandleQueuedMessage function,
			/// and will recieve sent messages directly via its HandleMessage function. You probably don't want to call AddHandler
			/// for an iHandler/MessageId pair that you're already calling AddQueuedHandler for.
			///
			/// Example usage:
			///    QueuedHandler myQueuedHandler(pCoreAllocator); // Can optionally associate an IHandler with this.
			///
			///    pServer->AddQueuedHandler(&myQueuedHandler, 0x12345678, false); }
			///
			///    MessageBasicRC<1>* pMessage = new MessageBasicRC<1>;
			///    pServer->MessagePost(0x12345678, pMessage);
			///    myQueuedHandler.ProcessQueue();          // If an IHandler was associated with myQueuedHandler then the messages will be sent to it here.
			///        or
			///    myQueuedHandler.GetMessageQueue()->...;  // Directly read the queue. Make sure to Release the Message if you directly remove messages from the queue.
			///
			EAM_VIRTUAL void AddQueuedHandler(IQueuedHandler* pHandler, MessageId messageId, bool bRefCounted, int nPriority = kPriorityNormal);


			/// AddHandlerFunction
			///
			/// Adds the given message handling function to the subscription list for the given message id.
			/// It is similar to the IHandler-based AddHandler function in all respects except for the
			/// case of reference counting. In this case, if bRefCounted is true then reference counting
			/// is implemented via the kMessageIdAddRef and kMessageIdRelease messages sent to the function
			/// by the server.
			///
			/// The pHandlerFunctionContext is a pointer to user-defined data (or is NULL) which gets passed
			/// to the HandlerFunction when it is called. This allows the user to attach contextual data to
			/// the callback function which the function can use to interpret how to operate.
			///
			/// Example usage:
			///    bool HandleMessage(MessageId id, void* pMsg, void* pContext);
			///    pServer->AddHandler(HandleMessage, 0x12345678, NULL);
			///
			#if EAMESSAGE_C_HANDLER_ENABLED
				EAM_VIRTUAL void AddHandlerFunction(HandlerFunction pHandlerFunction, void* pHandlerFunctionContext, 
													MessageId messageId, bool bRefCounted, int nPriority = kPriorityNormal);
			#endif


			/// RemoveHandler
			///
			/// Removes the given handler for the given message Id. A handler is removed only once for 
			/// a given call to RemoveHandler in the case that the handler was added more than once.
			///
			/// The priority parameter is used to match a handler to a given priority specified in 
			/// in AddHandler. If no priority is matched exactly, then any priority match is used.
			/// This allows you to remove a handler multiple times without having to know or care
			/// about any special priorities it was registered with.
			///
			/// Example usage:
			///    class Widget : public IHandler{
			///        Widget(){ pServer->AddHandler(this, 0x12345678); }
			///       ~Widget(){ pServer->RemoveHandler(this);          }
			///        bool HandleMessage(MessageId id, void* pMsg);
			///    };
			///
			EAM_VIRTUAL bool RemoveHandler(IHandler* pHandler, MessageId messageId, int nPriority = kPriorityAny);


			/// RemoveQueuedHandler
			///
			/// Removes the handler previously added via AddQueuedHandler.
			///
			/// Example usage:
			///    QueuedHandler myQueuedHandler(pCoreAllocator); // Can optionally associate an IHandler with this.
			///
			///    pServer->AddQueuedHandler(&myQueuedHandler, 0x12345678, false); }
			///    pServer->RemoveQueuedHandler(&myQueuedHandler, 0x12345678); }
			///
			EAM_VIRTUAL bool RemoveQueuedHandler(IQueuedHandler* pHandler, MessageId messageId, int nPriority = kPriorityAny);


			/// RemoveHandlerFunction
			///
			/// This function is just like RemoveHandler for IHandler except it works for a HandlerFunction.
			///
			/// Example usage:
			///    bool HandleMessage(MessageId id, void* pMsg, void* pContext);
			///    pServer->RemoveHandler(HandleMessage, 0x12345678, NULL);
			///
			#if EAMESSAGE_C_HANDLER_ENABLED
				EAM_VIRTUAL bool RemoveHandlerFunction(HandlerFunction pHandlerFunction, MessageId messageId, int nPriority = kPriorityAny);
			#endif


			/// IsHandlerRegistered
			///
			/// Returns true if the given handler is registered by AddHandler to handle the
			/// given message id. If the message id is kMessageIdNone, then this function 
			/// returns true if the handler is registered to handle any message ids.
			///
			bool IsHandlerRegistered(IHandler* pHandler, MessageId id = kMessageIdNone) const;


			/// ProcessQueue
			///
			/// Does one cycle of queue processing. 
			/// Returns the number of messages processed.
			///
			/// If kOptionQueueProcessing is disabled then the queue is not processed 
			/// and the return value is zero.
			///
			/// If the kOptionClearQueue option is enabled then the queue will be processed 
			/// until no messages remain, including messages posted while processing 
			/// queued messages. If kOptionClearQueue is disabled, then the only messages
			/// sent will be those present in the queue upon the start of the processsing.
			///
			EAM_VIRTUAL size_t ProcessQueue();


			/// ProcessQueue
			///
			/// Does multiple cycles of queue processing.
			/// Returns the number of messages which were processed.
			/// This function is useful for the case where you want to process the queue
			/// repeatedly in one shot. This can be useful because sometimes the delivery
			/// of a queued message results in the queuing of additional messages and 
			/// the user may want to make sure that the additional messages are processed
			/// as well as the original ones.
			///
			/// Example usage:
			///    pServer->ProcessQueue(5, 10, 1000);
			///
			EAM_VIRTUAL size_t ProcessQueue(size_t nMinCycleCount, size_t nMaxCycleCount = (size_t)-1, size_t nMaxTimeMilliseconds = (size_t)-1);


			/// GetMessageQueue
			///
			/// Returns a copy of the message queue to the caller. This function may seem a little
			/// overly powerful in that it lets the caller inspect and modify the message queue, 
			/// but a MessageQueue is a first class item and not a strictly internal implementation.
			/// Additionally, history has shown that there are times when systems truly need to 
			/// do smart inspection of message queues in order to do the right thing in the most
			/// efficient way.
			///
			/// In a multithreaded environment the caller needs to Lock the Server while the 
			/// MessageQueue is being used. 
			///
			EAM_VIRTUAL MessageQueue* GetMessageQueue()
				{ return &mMessageQueue; }


			/// ReserveMessageIdCapacity
			///
			/// Allows you to reserve memory for management of MessageId handler registration. 
			/// AddHandler potentially results in an increase in the size of the memory pool used
			/// to manage registerd handlers. This function allows you to tell the Server up front
			/// how many message ids you expect to be using, so it can reserve that amount of memory
			/// right away, as opposed to repeatedly reallocating it at runtime as handler regisration
			/// increases. This function can be called at any time, but it normally would be called 
			/// before an increase in handler registration occurs, such as on startup.
			///
			EAM_VIRTUAL void ReserveMessageIdCapacity(size_t messageIdCount);


			/// Lock
			///
			/// Locks the message server for thead-safe access over a user-specified period of time.
			/// This is useful for doing multiple actions with the server without being interrupted.
			/// 
			/// Returns the number of locks after the operation has completed.
			///
			EAM_VIRTUAL int Lock(bool bLock);


		protected:
			// struct / typedef / enum
			struct HandlerInfo
			{
				IHandler* mpHandler;
				int       mnPriority;
				bool      mbRefCounted;

				#if EAMESSAGE_C_HANDLER_ENABLED
					HandlerFunction mpHandlerFunction;
					void*           mpHandlerFunctionContext;
				#endif

				void AddRef();
				void Release();
			};

			/// HandlerInfoArray
			/// Used for temporary lists.
			typedef eastl::fixed_vector<HandlerInfo, 32, true, EA::Allocator::EASTLICoreAllocator> HandlerInfoArray;

			/// HandlerInfoList
			/// Used for registered message lists.
			typedef eastl::list<HandlerInfo, EA::Allocator::EASTLICoreAllocator> HandlerInfoList;

			/// HandlerInfoMap
			/// Used for registered message lists.
			typedef eastl::hash_map<MessageId, HandlerInfoList*, eastl::hash<MessageId>, eastl::equal_to<MessageId>, EA::Allocator::EASTLICoreAllocator> HandlerInfoMap;

		protected:
			// Functions
			EAM_VIRTUAL void AddHandlerInternal(MessageId messageId, HandlerInfoMap& handlerInfoMap, EA::Thread::Futex& mapMutex, 
												IHandler* pHandler, HandlerFunction pHandlerFunction, void* pHandlerFunctionContext, bool bRefCounted, int nPriority);
			EAM_VIRTUAL bool RemoveHandlerInternal(MessageId messageId, HandlerInfoMap& handlerInfoMap, Thread::Futex& mapMutex, int nPriority, void* pHandler);
			EAM_VIRTUAL bool IsHandlerRegisteredInternal(MessageId id, const HandlerInfoMap& handlerInfoMap, Thread::Futex& mapMutex, IHandler* pHandler) const;
			EAM_VIRTUAL void MessageSendInternal(MessageId id, HandlerInfoMap& handlerInfoMap, Thread::Futex& mapMutex, void* pMessage) const;

			void ClearHandlerInfoMap(HandlerInfoMap& handlerInfoMap);
			void PopulateHandleInfoArrayForMessage(MessageId id, const HandlerInfoMap& handlerInfoMap, HandlerInfoArray& handlerInfoArray);

		protected:
			// Data
			EA::Thread::AtomicUint64       mnQueueProcessedMessageCount;    /// Number of messages processed from the queue. Assists in mesage delivery.
			EA::Allocator::ICoreAllocator* mpCoreAllocator;                 /// Allocator for our containers.
			MessageQueue                   mMessageQueue;                   /// A queue of messages delivered via PostMessage.
			Thread::Futex                  mMessageQueueMutex;              /// Mutex specifically for mMessageQueue.
			HandlerInfoMap                 mHandlerInfoMap;                 /// Used for mapping message types to recipients.
			mutable Thread::Futex          mHandlerInfoMapMutex;            /// Mutex specifically for mHandlerInfoMap.
			HandlerInfoMap                 mQueuedHandlerInfoMap;           /// Used for registed QueuedHandlers, which are different from normal Handlers in that they maintain their own private receive queue.
			mutable Thread::Futex          mQueuedHandlerInfoMapMutex;      /// Spin lock for access to the handler info map
			bool                           mbThreadSafetyEnabled;           /// If false then we don't lock mutexes.
			bool                           mbReentrantSafetyEnabled;        /// If false then we don't worry about being called back while we are delivering messages.
			bool                           mbQueueProcessingEnabled;        /// 
			bool                           mbClearQueueEnabled;             /// 
			bool                           mbRefCountEnabled;               /// 
			bool                           mbTraceEnabled;                  /// 
		}; // class Server


		/// GetServer
		/// Retrieves the default global Server object.
		EAMESSAGE_API Server* GetServer();


		/// SetServer
		/// Sets the default global Server object.
		/// Returns the old Server object.
		/// The old Server is not deleted, as this function is merely an accessor modifier.
		EAMESSAGE_API Server* SetServer(Server* pServer);




		/// AddHandler
		///
		/// This is a helper function which is the same as Server::AddHandler except 
		/// that it allows the user to provide an array of messages to subscribe to.
		///
		/// Example usage:
		///    const MessageId idArray[4] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444 };
		///
		///    void SomeClass::Init() {
		///        AddHandler(pServer, this, idArray, 4, true);
		///    }
		///
		inline void AddHandler(Server* pServer, IHandler* pHandler, const MessageId* pMessageIdArray, 
								size_t messageIdArrayCount, bool bRefCounted, int nPriority = kPriorityNormal)
		{
			for(size_t i = 0; i < messageIdArrayCount; ++i)
				pServer->AddHandler(pHandler, pMessageIdArray[i], bRefCounted, nPriority);
		}


		/// RemoveHandler
		///
		/// This is a helper function which is the same as Server::RemoveHandler except 
		/// that it allows the user to provide an array of messages to unsubscribe to.
		///
		/// Example usage:
		///    const MessageId idArray[4] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444 };
		///
		///    void SomeClass::Shutdown() {
		///        RemoveHandler(pServer, this, idArray, 4, true);
		///    }
		///
		inline bool RemoveHandler(Server* pServer, IHandler* pHandler, const MessageId* pMessageIdArray, 
									size_t messageIdArrayCount, int nPriority = kPriorityNormal)
		{
			bool bReturnValue = true;
			for(size_t i = 0; i < messageIdArrayCount; ++i)
			{
				if(!pServer->RemoveHandler(pHandler, pMessageIdArray[i], nPriority))
					bReturnValue = false;
			}
			return bReturnValue;
		}


		/// message_cast<>
		///
		/// Template for casting a void pointer to an EA::Messaging::IMessageRC
		/// instance back to its original object type.
		///
		/// Example usage:
		///    \code
		///    bool HandleMessage(Messaging::MessageId messageId, void* pMessage)
		///    {
		///        MyMessageRC* const pMessageRC = message_cast<MyMessageRC*>(pMessage);
		///        ...
		///    }
		///    \endcode
		///
		template <typename MessagePtr>
		inline MessagePtr message_cast(void* pMessage)
		{
			return pMessage ? static_cast<MessagePtr>(static_cast<IMessageRC*>(pMessage)) : NULL;
		}


	} // namespace Messaging

} // namespace EA






/*
namespace EA
{
	namespace Messaging
	{
		
		inline void* IHandler::AsType(TypeId typeId)
		{
			if(typeId == IHandler::kTypeId)
				return static_cast<IHandler*>(this);
			return NULL;
		}

		inline void* IHandlerRC::AsType(TypeId typeId)
		{
			if(typeId == IHandlerRC::kTypeId)
				return static_cast<IHandlerRC*>(this);
			return IHandler::AsType(typeId);
		}

		inline void* IQueuedHandler::AsType(TypeId typeId)
		{
			if(typeId == IQueuedHandler::kTypeId)
				return static_cast<IQueuedHandler*>(this);
			return IHandler::AsType(typeId);
		}


	} // namespace Messaging

} // namespace EA
*/


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard
















