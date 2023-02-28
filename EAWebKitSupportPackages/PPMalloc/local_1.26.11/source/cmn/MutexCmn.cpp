///////////////////////////////////////////////////////////////////////////////
// MutexCmn.cpp
//
// Copyright (c) Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/mutex.h>
#include <eathread/eathread_futex.h>


namespace EA
{
	namespace Allocator
	{
		#if PPM_THREAD_SAFETY_SUPPORTED

			void* PPMMutexCreate(void* pData)
			{
				PPM_CT_ASSERT(sizeof(EA::Thread::Futex) <= kMutexBufferSize * sizeof(uint64_t));
				EA::Thread::Futex* pFutex = new (pData) EA::Thread::Futex;

				return pFutex;
			}

			void PPMMutexDestroy(void* pMutex)
			{
				if (pMutex) 
				{
					EA::Thread::Futex* pFutex = static_cast<EA::Thread::Futex*>(pMutex);
					pFutex->~Futex();
				}
			}

			void PPMMutexLock(void* pMutex)
			{
				if (pMutex) 
				{
					EA::Thread::Futex* pFutex = static_cast<EA::Thread::Futex*>(pMutex);
					pFutex->Lock();
				}
			}

			void PPMMutexUnlock(void* pMutex)
			{
				if (pMutex) 
				{
					EA::Thread::Futex* pFutex = static_cast<EA::Thread::Futex*>(pMutex);
					pFutex->Unlock();
				}
			}

			bool PPMMutexTryLockFor(void* pMutex, int duration)
			{
				if (pMutex)
				{
					const auto absoluteTimeout = EA::Thread::ConvertRelativeTime(static_cast<EA::Thread::ThreadTime>(duration));
					EA::Thread::Futex* pFutex = static_cast<EA::Thread::Futex*>(pMutex);
					return pFutex->Lock(absoluteTimeout) != EA::Thread::Futex::kResultTimeout;
				}

				return false;
			}

			int PPMMutexGetLockCount(const void* pMutex)
			{
				if (pMutex) 
				{
					const EA::Thread::Futex* pFutex = static_cast<const EA::Thread::Futex*>(pMutex);
					return pFutex->GetLockCount();
				}

				return 0;
			}

		#endif // PPM_THREAD_SAFETY_SUPPORTED

	} // namespace Allocator

} // namespace EA
