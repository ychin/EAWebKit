/////////////////////////////////////////////////////////////////////////////
// EAIOFileStreamImpl.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
//
// Provides a generic base class for all file stream implemetations
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAIOFILESTREAMIMPL_H
#define EAIO_EAIOFILESTREAMIMPL_H

#include <EAIO/internal/EAIOStreamImpl.h>

namespace EA
{
	namespace IO
	{
		class EAIO_API FileStreamImpl
			: public StreamTmpl<0x34722300> // Random u32 that uniquely identifies the type of stream
		{
		public:
			static constexpr uint32_t kTypeFileStream = kStreamType;

			enum Share
			{
				kShareNone = 0x00,     /// No sharing.
				kShareRead = 0x01,     /// Allow sharing for reading.
				kShareWrite = 0x02,     /// Allow sharing for writing.
				kShareDelete = 0x04      /// Allow sharing for deletion.
			};

			enum UsageHints
			{
				kUsageHintNone = 0x00,
				kUsageHintSequential = 0x01,
				kUsageHintRandom = 0x02,
				kUsageHintAsync = 0x04,
				kUsageHintNoCache = 0x08,
			};

		public:
			using StreamTmpl::StreamTmpl;

		private:
		};

	} // namespace IO

} // namespace EA

#endif  // Header include guard



