/////////////////////////////////////////////////////////////////////////////
// EAStream.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/EAStream.h>
#ifdef EA_ISTREAM_HAS_ASYNC
#include <EAIO/EAAsyncExecutor.h>
#include <EAIO/EAAsyncStream.h>

namespace EA
{
namespace IO
{
namespace Internal
{

/////////////////////////////////////////////////////////////////////////////////////////////////
/// AsyncReadStreamIoRequest
/// Read a block of data from a IStream
class AsyncReadStreamIoRequest : public AsyncStreamIoRequest
{
public:
	AsyncReadStreamIoRequest(IStream& stream, void* destination, size_type size, const AsyncIoFunctionCallback& func)
		: AsyncStreamIoRequest(stream, func), mDestination(destination), mSize(size) {}
	AsyncReadStreamIoRequest(IStream& stream, size_type position, void* destination, size_type size, const AsyncIoFunctionCallback& func)
		: AsyncStreamIoRequest(stream, func), mDestination(destination), mPosition(position), mSize(size), mHasPosition(true) {}

	virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override;
	virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override;

private:
	void* mDestination;
	size_type mPosition{ 0 };
	size_type mSize;
	bool mHasPosition{ false };
};

bool AsyncReadStreamIoRequest::Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	if (mHasPosition)
	{
		if (!mStream->SetPosition(mPosition, kPositionTypeBegin))
		{
			errorCode = AsyncErrorCode(mStream->GetState());
			result = AsyncResult::Error;
			return true;
		}
	}

	size_type read = mStream->Read(mDestination, mSize);
	if (read == size_type(-1))
	{
		errorCode = AsyncErrorCode(mStream->GetState());
		result = AsyncResult::Error;
		return true;
	}
	bytesTransferred = read;
	result = AsyncResult::Success;
	return true;
}

bool AsyncReadStreamIoRequest::IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	result = AsyncResult::Success;
	bytesTransferred = 0;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/// AsyncWriteStreamIoRequest
/// Read a block of data from a IStream
class AsyncWriteStreamIoRequest : public AsyncStreamIoRequest
{
public:
	AsyncWriteStreamIoRequest(IStream& stream, const void* source, size_type size, AsyncIoFunctionCallback func)
		: AsyncStreamIoRequest(stream, func), mSource(source), mSize(size) {}
	AsyncWriteStreamIoRequest(IStream& stream, size_type position, const void* source, size_type size, AsyncIoFunctionCallback func)
		: AsyncStreamIoRequest(stream, func), mSource(source), mPosition(position), mSize(size), mHasPosition(true) {}

	virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override;
	virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override;

private:
	const void* mSource;
	size_type mPosition{ 0 };
	size_type mSize;
	bool mHasPosition{ false };
};

bool AsyncWriteStreamIoRequest::Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	if (mHasPosition)
	{
		if (!mStream->SetPosition(mPosition, kPositionTypeBegin))
		{
			errorCode = AsyncErrorCode(mStream->GetState());
			result = AsyncResult::Error;
			return true;
		}
	}

	size_type written = mStream->Write(mSource, mSize);
	if (written == size_type(-1))
	{
		errorCode = AsyncErrorCode(mStream->GetState());
		result = AsyncResult::Error;
		return true;
	}
	bytesTransferred = written;
	result = AsyncResult::Success;
	return true;
}

bool AsyncWriteStreamIoRequest::IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	result = AsyncResult::Success;
	bytesTransferred = 0;
	return true;
}

} // namespace Internal

AsyncExecutor* IStream::GetAsyncExecutor()
{
	return AsyncExecutor::StartExecutor(AsyncExecutorType::Sequential);
}

uint32_t IStream::GetAsyncFileAlignment() const
{
	return 1;
}

uint32_t IStream::GetAsyncMemoryAlignment() const
{
	return 1;
}

AsyncRequestId IStream::AsyncRead(AsyncExecutor& exec, void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	return exec.AllocAndPush<Internal::AsyncReadStreamIoRequest>(eastl::ref(*this), destination, size, eastl::cref(func));
}

AsyncRequestId IStream::AsyncRead(AsyncExecutor& exec, size_type position, void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	return exec.AllocAndPush<Internal::AsyncReadStreamIoRequest>(eastl::ref(*this), position, destination, size, eastl::cref(func));
}

AsyncRequestId IStream::AsyncWrite(AsyncExecutor& exec,  const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	return exec.AllocAndPush<Internal::AsyncWriteStreamIoRequest>(eastl::ref(*this), source, size, eastl::cref(func));
}

AsyncRequestId IStream::AsyncWrite(AsyncExecutor& exec, size_type position, const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	return exec.AllocAndPush<Internal::AsyncWriteStreamIoRequest>(eastl::ref(*this), position, source, size, eastl::cref(func));
}

AsyncRequestId IStream::AsyncRead(void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	if (auto exec = GetAsyncExecutor())
		return AsyncRead(*exec, destination, size, func);
	else
		return kInvalidAsyncRequestId;
}

AsyncRequestId IStream::AsyncRead(size_type position, void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	if (auto exec = GetAsyncExecutor())
		return AsyncRead(*exec, position, destination, size, func);
	else
		return kInvalidAsyncRequestId;
}

AsyncRequestId IStream::AsyncWrite(const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	if (auto exec = GetAsyncExecutor())
		return AsyncWrite(*exec, source, size, func);
	else
		return kInvalidAsyncRequestId;
}

AsyncRequestId IStream::AsyncWrite(size_type position, const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	if (auto exec = GetAsyncExecutor())
		return AsyncWrite(*exec, position, source, size, func);
	else
		return kInvalidAsyncRequestId;
}

} // namespace IO
} // namespace EA

#endif

