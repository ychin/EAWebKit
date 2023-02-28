/////////////////////////////////////////////////////////////////////////////
// EAFileStreamStdC.h
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAFILESTREAM_STDC_H
#define EAIO_EAFILESTREAM_STDC_H

#include <EAIO/internal/EAIOFileStreamImpl.h>

#ifndef EAIO_EAFILESTREAM_H
	#include <EAIO/EAFileStream.h>
#endif
#ifndef EAIO_EAFILEBASE_H
	#include <EAIO/EAFileBase.h>
#endif
#include <stddef.h>



namespace EA
{
	namespace IO
	{
		class EAIO_API FileStream : public FileStreamImpl
		{

		public:
			FileStream(const char*  pPath8 = NULL);
			FileStream(const char16_t* pPath16);
			FileStream(const char32_t* pPath32);
#if EA_WCHAR_UNIQUE
				FileStream(const wchar_t*  pPathW);
#endif

			// FileStream
			// Does not copy information related to an open file, such as the file handle.
			FileStream(const FileStream& fs);

			virtual ~FileStream();

			// operator=
			// Does not copy information related to an open file, such as the file handle.
			FileStream& operator=(const FileStream& fs);

			virtual void      SetPath(const char*  pPath8);
			virtual void      SetPath(const char16_t* pPath16);
			virtual void      SetPath(const char32_t* pPath32);
#if EA_WCHAR_UNIQUE
				virtual void      SetPath(const wchar_t*  pPathW);
#endif
			virtual size_t    GetPath(char*  pPath8,  size_t nPathLength);
			virtual size_t    GetPath(char16_t* pPath16, size_t nPathLength);
			virtual size_t    GetPath(char32_t* pPath32, size_t nPathLength);
#if EA_WCHAR_UNIQUE
				virtual size_t    GetPath(wchar_t*  pPathW,  size_t nPathLength);
#endif

			virtual bool      Open(int nAccessFlags = kAccessFlagRead, int nCreationDisposition = kCDDefault, int nSharing = kShareRead, int nUsageHints = kUsageHintNone); 
			virtual bool      Close();
			virtual int       GetAccessFlags() const;
			virtual int       GetState() const;

			virtual size_type GetSize() const;
			virtual bool      SetSize(size_type size);

			virtual off_type  GetPosition(PositionType positionType = kPositionTypeBegin) const;
			virtual bool      SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

			virtual size_type GetAvailable() const;

			virtual size_type Read(void* pData, size_type nSize);
			virtual bool      Write(const void* pData, size_type nSize);
			virtual bool      Flush();

		protected:
			int         mnFileHandle;
			char*    mpPath8;                    /// Path for the file.
			int         mnAccessFlags;              /// See enum AccessFlags.
			int         mnCD;                       /// See enum CD (creation disposition).
			int         mnSharing;                  /// See enum Share.
			int         mnUsageHints;               /// See enum UsageHints.
			mutable int mnLastError;                /// Used for error reporting.

		}; // class FileStream

	} // namespace IO

} // namespace EA

#endif  // #ifndef EAIO_EAFILESTREAMS_TDC_H











