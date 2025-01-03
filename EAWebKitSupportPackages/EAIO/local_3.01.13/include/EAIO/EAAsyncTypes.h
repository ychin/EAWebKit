/////////////////////////////////////////////////////////////////////////////
// EAAsyncTypes.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAASYNCTYPES_H
#define EAIO_EAASYNCTYPES_H

#ifndef INCLUDED_eabase_H
	#include <EABase/eabase.h>
#endif

#include <EAIO/internal/Config.h>

namespace EA
{
	namespace IO
	{
		// Forward declarations
		class AsyncExecutor;

		/// AsyncResult
		/// The result of a request
		enum class AsyncResult : uint8_t
		{
			Success,
			Error,
			Canceled,
			Retry,
		};

		/// AsyncErrorCode
		/// Generic method of passing around error codes 
		class AsyncErrorCode
		{
		public:
			using value_type = int;

			AsyncErrorCode() {}
			AsyncErrorCode(value_type value) : mValue(value) {}

			value_type GetValue() const { return mValue; }

		private:
			value_type mValue{ 0 };
		};

		/// AsyncRequestId
		/// Unique identifier assigned to any requested with an id.  This can later be used to cancel a request
		using AsyncRequestId = uint64_t;

		/// kInvalidAsyncRequestId
		static constexpr AsyncRequestId kInvalidAsyncRequestId = 0;

	} // namespace IO

} // namespace EA

#endif // Header include guard














