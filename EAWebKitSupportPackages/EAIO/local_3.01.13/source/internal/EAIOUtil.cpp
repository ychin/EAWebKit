///////////////////////////////////////////////////////////////////////////////
// EAIOUtil.cpp
//
// Copyright (c) 2012, Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/internal/EAIOUtil.h>

#if defined(EA_PROCESSOR_X86_64) && defined(_MSC_VER)
	#pragma warning(push, 0)
	#include <Windows.h>
	#pragma warning(pop)
#endif


namespace EA
{
	namespace IO
	{
		namespace Internal
		{
			#if defined(EA_PROCESSOR_X86) && defined(_MSC_VER) && defined(EA_PLATFORM_WINDOWS)
				bool PointerIsFromStack(void* p)
				{
					void* stackLow, *stackHigh;

					_asm {
						mov eax,  FS:[8] 
						mov stackLow,  eax
						mov eax,  FS:[4] 
						mov stackHigh, eax
					}

					return ((uintptr_t)p > ((uintptr_t)stackLow - 4096)) && ((uintptr_t)p < (uintptr_t)stackHigh);
				}

			#elif defined(EA_PROCESSOR_X86_64) && defined(_MSC_VER) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

				bool PointerIsFromStack(void* p)
				{
					CONTEXT context;
					context.ContextFlags = CONTEXT_ALL; // Actually we should need only CONTEXT_INTEGER, so let's test that next chance we get.
					RtlCaptureContext(&context);

					return ((uintptr_t)p > (context.Rsp - 4096)) && ((uintptr_t)p < (context.Rsp + 8192));
				}

			#elif defined(__GNUC__) // This may not work for all GCC variants. Works for Linux, OS X, iOS, and general ARM GCC.
				bool PointerIsFromStack(void* p)
				{
					uintptr_t pStack = (uintptr_t)__builtin_frame_address(0);

					return ((uintptr_t)p > (pStack - 4096)) && ((uintptr_t)p < (pStack + 8192));
				}

			#else
				bool PointerIsFromStack(void*)
				{
					return false;
				}
			#endif

		} // namespace Internal

	}   // namespace IO

}   // namespace EA


