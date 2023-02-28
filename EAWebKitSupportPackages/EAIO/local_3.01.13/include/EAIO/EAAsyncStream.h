/////////////////////////////////////////////////////////////////////////////
// EAAsyncStream.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAASYNCSTREAM_H
#define EAIO_EAASYNCSTREAM_H

#include <EAIO/EAStream.h>
#ifdef EA_ISTREAM_HAS_ASYNC
#include <EAIO/EAAsyncExecutor.h>
#include <EAIO/internal/EAIOEARefCount.h>
#include <EASTL/functional.h>
#include <EASTL/fixed_function.h>

namespace EA
{
namespace IO
{
// Forward declarations
class AsyncExecutor;
class IStream;

/////////////////////////////////////////////////////////////////////////////////////////////////
class EAIO_API AsyncStreamIoRequest : public AsyncIoRequest
{
public:
	AsyncStreamIoRequest(IStream& stream, const AsyncIoFunctionCallback& func) 
		: AsyncIoRequest(0, 0)
		, mStream(&stream)
		, mFunc(func)
	{}

	virtual AsyncIoMode GetAsyncIoMode() const override
	{
		return AsyncIoMode::Blocking;
	}

	virtual void Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override final
	{
		if (mFunc)
			mFunc(result, errorCode, bytesTransferred);
	}

protected:
	EAIOAutoRefCount<IStream> mStream;
	AsyncIoFunctionCallback mFunc;
};

void EAIO_API AsyncCancel(IStream* stream, AsyncRequestId requestId);

} // namespace IO
} // namespace EA

#endif
#endif // Header include guard














