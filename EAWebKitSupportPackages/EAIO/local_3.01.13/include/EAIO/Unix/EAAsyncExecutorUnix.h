/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorUnix.h
//
// Unix specific definitions for the executor
/////////////////////////////////////////////////////////////////////////////

#ifndef EAIO_EAASYNCEXECUTOR_UNIX_H
#define EAIO_EAASYNCEXECUTOR_UNIX_H

namespace EA
{
	namespace IO
	{
		struct AsyncExecutorAsyncOp;

		enum PlatformSpecificType
		{
			kDefault = 0,
		};

		void BindToUnixExecutor(AsyncExecutor& asyncExecutor, int fd);
		void UnbindFromUnixExecutor(AsyncExecutor& asyncExecutor, int fd);

		bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
			AsyncIoRequest* request, int fd, size_type position, void* destination, size_type size);

		bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op, 
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, 
			AsyncIoRequest* request, int fd, size_type position, const void* source, size_type size);

		bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred);

		void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op);
	} // namespace IO

} // namespace EA

#endif  // Header include guard
