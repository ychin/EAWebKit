/////////////////////////////////////////////////////////////////////////////
// EAFileStreamUnix.h
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAFILESTREAM_UNIX_H
#define EAIO_EAFILESTREAM_UNIX_H


#include <EAIO/internal/EAIOFileStreamImpl.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/PathString.h>
#include <stddef.h>
#include <jni.h>



namespace EA
{
	namespace IO
	{
		/*
		 * The FileStream implementation for Android supports regular files on
		 * the filesystem as well as files (i.e. assets) embedded in the
		 * applications .apk (application package) file.
		 *
		 * Before loading an asset from a .apk file it is important to
		 * initialize EAIO from Java.  Doing so provides EAIO with a
		 * reference to the Java AssetManager as well as the JVM itself.
		 *
		 * You will need to startup EAIO and pass the AssetManager:
		 *
		 *   import com.ea.EAIO.EAIO;
		 *
		 *   ...
		 *
		 *
		 *   EAIO.Startup(getAssets());
		 * 
		 * When the application is exitting you should also call shutdown to free up
		 * references to Java objects held by the filesystem.
		 *
		 *   EAIO.Shutdown();
		 *
		 * This .apk support allows files to be read from a .apk package.  Due
		 * to the limited API exposed by the Android API's, this system only
		 * implements a subset of the normal driver features.  For example it
		 * isn't possible to write into the .apk package.  Also, seeking within
		 * a file is simulated by skipping forward through the stream.  A seek
		 * backwards in the stream actually causes the file to be reopened to
		 * reset the position and then skipped forward until the requested
		 * position.  Therefore users should attempt to only read sequentially
		 * through a file or expect to see performance penalties.
		 *
		 */
		class EAIO_API FileStream : public FileStreamImpl
		{
		public:
			FileStream(const char* pPath8 = NULL);
			FileStream(const char16_t* pPath16);
			FileStream(const char32_t* pPath32);
			#if EA_WCHAR_UNIQUE
				FileStream(const wchar_t* pPathW);
			#endif

			// FileStream
			// Does not copy information related to an open file, such as the file handle.
			FileStream(const FileStream& fs);

			virtual ~FileStream();

			// operator=
			// Does not copy information related to an open file, such as the file handle.
			FileStream& operator=(const FileStream& fs);

			virtual void      SetPath(const char* pPath8);
			virtual void      SetPath(const char16_t* pPath16);
			virtual void      SetPath(const char32_t* pPath32);
			#if EA_WCHAR_UNIQUE
				virtual void  SetPath(const wchar_t* pPathW);
			#endif

			virtual size_t    GetPath(char* pPath8, size_t nPathCapacity);
			virtual size_t    GetPath(char16_t* pPath16, size_t nPathCapacity);
			virtual size_t    GetPath(char32_t* pPath32, size_t nPathCapacity);
			#if EA_WCHAR_UNIQUE
				virtual size_t  GetPath(wchar_t* pPathW, size_t nPathCapacity);
			#endif

			virtual bool      Open(int nAccessFlags = kAccessFlagRead, int nCreationDisposition = kCDDefault, int nSharing = kShareRead, int nUsageHints = kUsageHintNone); 
			virtual bool      Close();
			virtual int       GetAccessFlags() const;
			virtual int       GetState() const;

			// Note: GetSize may return -1 for files that are stored with compression inside a .apk file.
			virtual size_type GetSize() const;
			virtual bool      SetSize(size_type size);

			virtual off_type  GetPosition(PositionType positionType = kPositionTypeBegin) const;
			// Note: SetPosition may be very inefficient for files inside a .apk file.
			virtual bool      SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

			virtual size_type GetAvailable() const;

			virtual size_type Read(void* pData, size_type nSize);
			virtual bool      Write(const void* pData, size_type nSize);
			virtual bool      Flush();

		protected:
			typedef EA::IO::Path::PathString8 PathString8;

			enum InternalStreamType
			{
				IST_Native,
				IST_AppBundle,
			};

			jobject            mJavaInputStream;        /// Reference to Java FileChannel used to read assets object
			mutable size_type  mJavaFileSize;           /// Size of the Java file opened.    
			off_type           mJavaFilePosition;       /// Current read position within the file.
			int                mnFileHandle;
			PathString8        mPath8;                  /// Path for the file.
			int                mnAccessFlags;           /// See enum AccessFlags.
			int                mnCD;                    /// See enum CD (creation disposition).
			int                mnSharing;               /// See enum Share.
			int                mnUsageHints;            /// See enum UsageHints.
			mutable int        mnLastError;             /// Used for error reporting.
			InternalStreamType mStreamType;             /// Used internally to control how stream is accessed.

		}; // class FileStream

	} // namespace IO

} // namespace EA

#endif  // include guard











