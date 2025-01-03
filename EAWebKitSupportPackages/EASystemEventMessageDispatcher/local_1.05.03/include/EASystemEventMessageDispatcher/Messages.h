///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <coreallocator/icoreallocator.h>
#include <EAMessage/EAMessage.h>
#include <EASystemEventMessageDispatcher/internal/Config.h>
#include <EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h>
#include <eathread/eathread_atomic.h>
			   
namespace EA
{
	namespace SEMD
	{
		//--------------------------------------------
		class Message : public EA::Messaging::IMessageRC
		{
		public:
			
			Message(EA::Allocator::ICoreAllocator* pAllocator, void* pData) : mnRefCount(0), mpAllocator(pAllocator), mpData(pData) 
			{
			}
			
			int AddRef() 
			{ 
				return mnRefCount.Increment(); 
			}

			int Release()
			{
				int32_t rc = mnRefCount.Decrement();
				if(rc)
				{
					return rc;
				}
				
				this->~Message();
				mpAllocator->Free(this);

				return 0;                
			}
			
			template <typename T> T GetData()
			{ 
			   return reinterpret_cast<T>(mpData); 
			}

			template <typename T> const T GetData() const
			{ 
				return reinterpret_cast<T>(mpData); 
			}

		private:
			
			Message(const Message&);
			Message& operator=(const Message&);
						

			EA::Thread::AtomicInt32             mnRefCount;
			EA::Allocator::ICoreAllocator*      mpAllocator;
			void*                               mpData;
		};
	}
}
