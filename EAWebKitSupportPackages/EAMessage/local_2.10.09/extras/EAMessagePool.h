/////////////////////////////////////////////////////////////////////////////
// EAMessagePool.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This file implements a pool for postable messages. 
//
// It allows the user to easily do asynchronous message posting without
// having to directly worry about memory management, but still do memory
// management in a conservative controlled way. With this module you can
// write simple code such as this:
//     TouchdownScored msg(7);
//     pPostedMessagePool->MessagePost(kMsgTouchdownScored, &msg);
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAMESSAGE_EAMESSAGEPOOL_H
#define EAMESSAGE_EAMESSAGEPOOL_H


#include <EAMessage/EAMessage.h>
#include <eathread/eathread.h>
#include <coreallocator/icoreallocator_interface.h>
#include <new>


namespace EA
{
	namespace Messaging
	{

		/// PostedMessageBase
		///
		/// Provides a base class which a posted message can inherit from and which
		/// can come from a memory pool specified by an ICoreAllocator. This class
		/// is not required for usage with MessageServer nor PostedMessagePool,
		/// but is a potentially convenient option. 
		///
		/// Example declaration:
		///     const uint32_t kMsgTouchdownScored;
		///
		///     struct TouchdownScored : public PostedMessageBase {
		///         TouchdownScored(int points) : mPoints(points){}
		///         int mPoints;
		///     };
		///
		/// Example usage with PostedMessagePool. The msg is copied as part of the posting.
		///     TouchdownScored msg(7);
		///     pPostedMessagePool->MessagePost(kMsgTouchdownScored, &msg);
		///
		/// Example posting directly via MessageServer. The pMsg is explicitly created by the user.
		///     TouchdownScored* pMsg = pPostedMessagePool->MessageCreate<TouchdownScored>();
		///     pMsg->mPoints = 7;
		///     pMessageServer->MessagePost(kMsgTouchdownScored, pMsg);
		///
		/// Example usage directly and only with MessageServer:
		///     TouchdownScored* pMsg = pAllocator->Alloc(sizeof(TouchdownScored));
		///     pMsg = new(pMsg) TouchdownScored(7); // Placement new
		///     pMessageServer->MessagePost(kMsgTouchdownScored, pMsg);
		///
		class PostedMessageBase : public EA::Messaging::IMessageRC
		{
		public:
			PostedMessageBase(EA::Allocator::ICoreAllocator* pAllocator = NULL) 
				: mRefCount(0), mpAllocator(pAllocator) { }

			int AddRef()
			{
				return (int)mRefCount.Increment();
			}

			int Release()
			{
				const int nRefCount = (int)mRefCount.Decrement();
				if(nRefCount) 
					return nRefCount;

				this->~PostedMessageBase();
				mpAllocator->Free(this);
				return 0;
			}

			void SetAllocator(EA::Allocator::ICoreAllocator* pAllocator)
			{
				mpAllocator = pAllocator;
			}

		protected:
			EA::Thread::AtomicInt32        mRefCount;
			EA::Allocator::ICoreAllocator* mpAllocator;
		};



		/// PostedMessagePool
		///
		/// Provides a posted message memory pool whereby the user can post somewhat
		/// arbitrary messages in a fire-and-forget fashion. The user provides a
		/// memory pool and uses message structs that inherit from PostedMessageBase
		/// or an equivalent user-defined class.
		///
		/// Note that you can have multiple simultaneous instances of PostedMessagePool
		/// which work with different pools (allocators) and/or different message servers. 
		///
		/// Example declaration:
		///     PostedMessagePool postedMessagePool(pMessageServer, pAllocator);
		///
		/// Example usage with PostedMessagePool. The msg is copied as part of the posting.
		///     TouchdownScored msg(7);
		///     postedMessagePool.MessagePost(kMsgTouchdownScored, &msg);
		///
		/// Example posting whereby the message is explicitly created by the user.
		///     TouchdownScored* pMsg = pPostedMessagePool->MessageCreate<TouchdownScored>();
		///     pMsg->mPoints = 7;
		///     postedMessagePool.MessagePost(kMsgTouchdownScored, pMsg); // Note that we directly use the message server.
		///
		class PostedMessagePool
		{
		public:
			PostedMessagePool(EA::Messaging::Server* mpMessageServer = NULL, 
							  EA::Allocator::ICoreAllocator* mpAllocator = NULL) : 
				mpMessageServer(mpMessageServer), 
				mpAllocator(mpAllocator)
			{
			}


			/// MessageCreate
			///
			/// Creates a message via the memory pool specified by our ICoreAllocator.
			/// You can use this to directly create a message or you can use this 
			/// indirectly by calling the MessagePost function with a local copy of 
			/// the message you want to send. See MessagePost for more.
			/// The templated type U must inherit from PostedMessageBase or it must
			/// at least have a SetAllocator member function.
			///
			/// Example usage where the message is not ref-counted (e.g. posted to message server):
			///     SomeMessageType msg(111, 222);
			///     SomeMessageType* pMsg = pPostedMessagePool->MessageCreate(&msg);
			///     . . .
			///     pPostedMessagePool->MessageDestroy(pMsg);
			/// 
			/// Example usage where the message is not ref-counted (e.g. posted to message server):
			///     SomeMessageType* pMsg = pPostedMessagePool->MessageCreate<SomeMessageType>();
			///     . . .
			///     pPostedMessagePool->MessageDestroy(pMsg);
			/// 
			/// Example usage where message is ref-counted:
			///     SomeMessageType* pMsg = pPostedMessagePool->MessageCreate<SomeMessageType>();
			///     pServer->MessagePost(kMsgTouchdownScored, pMsg);
			///     // No need to delete or release the message here; pServer has the only ref-count on it.
			/// 
			template <typename U>
			U* MessageCreate(const U* pU = NULL)
			{
				// To consider: Support alignment specification for the allocation of U via the EAAlignOf(U) macro from EABase.
				void* const pMemory = mpAllocator->Alloc(sizeof(U), "PostedMessagePool", 0);

				if(pMemory)
				{
					U* const pNewU = (pU ? new(pMemory) U(*pU) : new(pMemory) U); // Placement operator new using pMemory.
					pNewU->SetAllocator(mpAllocator);
					return pNewU;
				}

				return NULL;
			}

			// Deprecated due to Microsoft collisions, but retained for backwards compatibility:
			template <typename U>
			U* CreateMessage(const U* pU = NULL) { return MessageCreate<U>(pU); }



			/// MessageDestroy
			///
			/// This function complements the functionality of MessageCreate.
			/// Has the same effect as 'delete pU' except that it uses the assigned allocator.
			///
			/// Note that if you call MessageCreate with a refcounted message class and hand the 
			/// resulting instance over to somebody that (e.g. message Server) refcounts messages,
			/// you want to use AddRef/Release to manage the lifetime of the message and don't want
			/// to directly call MessageDestroy on such messages.
			///
			template <typename U>
			void MessageDestroy(U* pU)
			{
				pU->~U();
				mpAllocator->Free(pU);
			}

			// Deprecated due to Microsoft collisions, but retained for backwards compatibility:
			template <typename U>
			U* DestroyMessage(const U* pU = NULL) { MessageDestroy<U>(pU); }


			/// MessagePost
			///
			/// Posts a message that is copied from a user message (pU). 
			/// The pU value can be NULL, in which case a new default instance is made.
			/// The user message must inherit from PostedMessageBase or be a class that has
			/// equivalent functionality.
			/// The user message pU is not saved by this function, as this function copies
			/// the message. The memory for it must be managed by the user.
			/// Additionally, when the message is ultimately received by the user it must be 
			/// manually freed. The MessageDestroy function can do that.
			///
			/// Example usage:
			///     SomeMessageType msg(111, 222);
			///     pPostedMessagePool->MessagePost(0x12345678, &msg);
			///
			template <typename U>
			U* MessagePost(MessageId id, const U* pU, int nPriority = kPriorityNormal, IHandlerRC* pHandlerRC = NULL)
			{
				U* const pUNew = MessageCreate(pU);

				if(pUNew)
				{
					mpMessageServer->MessagePost(id, pUNew, nPriority, pHandlerRC);
					return pUNew;
				}

				return NULL;
			}

			// Deprecated due to Microsoft collisions, but retained for backwards compatibility:
			template <typename U>
			U* PostMessage(MessageId id, const U* pU, int nPriority = kPriorityNormal, IHandlerRC* pHandlerRC = NULL) { return MessagePost<U>(id, pU, nPriority, pHandlerRC); }

		protected:
			EA::Messaging::Server*         mpMessageServer;
			EA::Allocator::ICoreAllocator* mpAllocator;
		};

	} // namespace Messaging

} // namespace EA



#endif // Header include guard.



