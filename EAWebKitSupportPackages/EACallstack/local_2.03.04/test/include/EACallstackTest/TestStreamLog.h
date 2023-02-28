/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef TESTSTREAMLOG_H
#define TESTSTREAMLOG_H

#include <EACallstack/internal/Config.h>
#include <EAIO/EAStream.h>
#include <EATest/EATest.h>

namespace EA
{
	namespace IO
	{
		/// class StreamLog
		///
		/// Implements a fixed-size write-only stream which traces-out via the EA_LOG macro
		/// and thus allows for logging to an IStream.
		///
		/// This class is not inherently thread-safe. As a result, thread-safe usage
		/// between multiple threads requires higher level coordination, such as a mutex.
		///
		class StreamLog : public EA::IO::IStream
		{
		public:
			enum { kTypeStreamLog = 0x32ea45ac };

			StreamLog()
				: mnRefCount(0)
			{
			}

			int AddRef()
			{
				return ++mnRefCount;
			}

			int Release()
			{
				if (mnRefCount > 1)
					return --mnRefCount;

				delete this;

				return 0;
			}

			// IStream
			uint32_t    GetType() const { return kTypeStreamLog; }
			int         GetAccessFlags() const { return kAccessFlagWrite; }
			int         GetState() const { return 0; }
			bool        Close() { return true; }

			size_type   GetSize() const { return 0; }
			bool        SetSize(size_type) { return true; }

			off_type    GetPosition(PositionType) const { return 0; }
			bool        SetPosition(off_type, PositionType){ return true; }

			size_type   GetAvailable() const { return 0; }
			size_type   Read(void*, size_type) { return 0; }

			bool        Flush() { return true; }
			bool        Write(const void* pData, size_type nSize)
			{
				mLine.assign((const char*)pData, (eastl_size_t)nSize);
				EA::UnitTest::Report("%s", mLine.c_str());
				return true;
			}

		protected:
			typedef     eastl::fixed_string<char, 256, true> LineType;

			LineType    mLine;
			int         mnRefCount;

		}; // class StreamLog

	}
}

#endif // TESTSTREAMLOG_H
