///////////////////////////////////////////////////////////////////////////////
// EAIOUtil.h
//
// Copyright (c) 2012, Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAIOUTIL_H
#define EAIO_EAIOUTIL_H


namespace EA
{
	namespace IO
	{
		namespace Internal
		{
			/// PointerIsFromStack
			///
			/// PointerIsFromStack tells if the given pointer is from the current stack. 
			/// It can't tell if the pointer is from another thread's stack.
			/// It works only on the platforms supported in the implementation below.
			/// It may return false negatives for some platforms, but that's OK.
			/// 
			/// This function is for diagnostic/debugging purposes only.

			bool PointerIsFromStack(void* p);
		}
	}
} 


#endif  // Header include guard
