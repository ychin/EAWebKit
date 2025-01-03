/////////////////////////////////////////////////////////////////////////////
// EAStreamRwfilesys.h
//
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTREAMRWFILESYS_H
#define EASTREAMRWFILESYS_H


#include <EAIO/EAFileBase.h>
#include <EAIO/EAStream.h>
#include <rw/core/filesys/ifilestream.h>
#include <stddef.h>
#include <stdio.h>


namespace EA
{
	namespace IO
	{
		class RwfilesysStream : public IStream
		{
		public:
			enum { kTypeRwfilesysStream = 0x0642a9c4 };

		public:
			RwfilesysStream(const char* pPath8 = NULL);
			RwfilesysStream(const char16_t* pPath16);

			// RwfilesysStream
			// Does not copy information related to an open file, such as the file handle.
			RwfilesysStream(const RwfilesysStream& fs);

			virtual ~RwfilesysStream();

			// operator=
			// Does not copy information related to an open file, such as the file handle.
			RwfilesysStream& operator=(const RwfilesysStream& fs);

			virtual int       AddRef();
			virtual int       Release();

			virtual void      SetPath(const char* pPath8);
			virtual void      SetPath(const char16_t* pPath16);
			virtual size_t    GetPath(char* pPath8, size_t nPathLength);
			virtual size_t    GetPath(char16_t* pPath16, size_t nPathLength);

			virtual bool      Open(int nAccessFlags = kAccessFlagRead, int nCreationDisposition = kCDDefault); 

			virtual bool      Close();
			virtual uint32_t  GetType() const { return kTypeRwfilesysStream; }
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
			rw::core::filesys::DefaultFileStream mFileStream;                    /// 
			char                              mPath8[EA::IO::kMaxPathLength]; /// Path for the file.
			int                                  mnRefCount;                     /// Reference count, which may or may not be in use.
			int                                  mnAccessFlags;                  /// See enum AccessFlags.
			int                                  mnCD;                           /// See enum CD (creation disposition).
			mutable int                          mnLastError;                    /// Used for error reporting.

		}; // class RwfilesysStream

	} // namespace IO

} // namespace EA


#endif // Header include guard










