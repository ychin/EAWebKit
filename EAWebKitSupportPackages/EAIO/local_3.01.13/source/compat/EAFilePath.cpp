/////////////////////////////////////////////////////////////////////////////
// EAFilePath.cpp
// 
// Copyright (c) 2003, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Note: This module is "out of favor". The EAIO/PathString module is the
//       intended simplified successor to this module. 
//
/////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <EAIO/compat/EAFilePath.h>
#include <EAIO/EAFileUtil.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <EAStdC/EAMemory.h>
#include EA_ASSERT_HEADER

#ifdef EA_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <direct.h>
#endif

#if defined(_MSC_VER)
	#pragma warning(disable: 6385)  // Invalid data: accessing 'comp1', the readable size is '20' bytes, but '4194248' bytes might be read: Lines: 504, 505, 507, 508, 510, 512, 514
#endif


#if !defined(EAIO_FILEPATH_ENABLED) || !EAIO_FILEPATH_ENABLED
	#error EAIO_FILEPATH_ENABLED needs to be set to 1 in config.h
#endif


#define LOCAL_MIN(x,y) ((x) < (y) ? (x) : (y))
#define LOCAL_MAX(x,y) ((x) > (y) ? (x) : (y))


namespace EA
{

namespace IO
{

// Forward declarations
EAIO_API char16_t* FullPath(char16_t* pPathResult, const char16_t* pPath, size_t /*nMaxLength*/, FileSystem fileSystemType);


FilePath::FilePath()
{
	mPath[0] = 0;
}


FilePath::FilePath(const char16_t* pPath)
{
	mPath[0] = 0;
	SetPath(pPath);
}


FilePath::FilePath(const char16_t* pDrive, const char16_t* pDirectory, const char16_t* pFileName, const char16_t* pExtension)
{
	mPath[0] = 0;
	SetPath(pDrive, pDirectory, pFileName, pExtension);
}


const char16_t* FilePath::GetPath() const
{
	return mPath;
}


const char16_t* FilePath::GetFileNameAndExtension() const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return pFile;
}


const char16_t* FilePath::GetExtension() const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return pExt;
}


size_t FilePath::GetDrive(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(mPath, pDir, dst, capacity);
}


size_t FilePath::GetDirectory(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(pDir, pFile, dst, capacity);
}


size_t FilePath::GetFileName(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(pFile, pExt, dst, capacity);
}


size_t FilePath::GetExtension(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(pExt, NULL, dst, capacity);
}


size_t FilePath::GetPath(char16_t *dst, size_t capacity) const
{
	return SafeSubstringCopy(mPath, NULL, dst, capacity);
}


size_t FilePath::GetFileNameAndExtension(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(pFile, NULL, dst, capacity);
}


size_t FilePath::GetDriveAndDirectory(char16_t *dst, size_t capacity) const
{
	const char16_t *pDir = NULL, *pFile = NULL, *pExt = NULL;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return SafeSubstringCopy(mPath, pFile, dst, capacity);
}


void FilePath::TrimToDriveAndDirectory()
{
	char16_t *dir = NULL, *dirEnd = NULL, *ext = NULL;
	SplitPathPtrs(mPath, dir, dirEnd, ext);
	*dirEnd = 0;
}


void FilePath::TrimToRoot()
{
	 char16_t *dir = NULL, *dirEnd = NULL, *ext = NULL;
	 SplitPathPtrs(mPath, dir, dirEnd, ext);

	 if (IsFilePathSeparator(*dir))
		  ++dir;
	 *dir = 0;
}


bool FilePath::RemoveLastDirectory()
{
	char16_t *dir, *dirEnd, *ext;
	SplitPathPtrs(mPath, dir, dirEnd, ext);

	if (dir != dirEnd)
	{
		char16_t *keepEnd = dirEnd;

		// We should delete a trailing backslash.
		if (keepEnd[-1] == EA_FILE_PATH_SEPARATOR_16  || keepEnd[-1] == EA_FILE_PATH_SEPARATOR_ALT_16)
			--keepEnd;

		// Find the next trailing backslash from the end and delete everything
		// after it in the directory.
		while(keepEnd > dir)
		{
			--keepEnd;

			if (*keepEnd == EA_FILE_PATH_SEPARATOR_16 || *keepEnd == EA_FILE_PATH_SEPARATOR_ALT_16)
			{
				const char16_t emptyString[1] = { 0 }; // This is faster and more portable than using EAText16("").
				return SafeReplace(keepEnd+1, dirEnd, emptyString, NULL, 0, 0);
			}
		}

		const char16_t emptyString[1] = { 0 }; // This is faster and more portable than using EAText16("").
		return SafeReplace(dir, dirEnd, emptyString, NULL, 0, 0);
	}

	return false;
}


bool FilePath::AppendDirectory(const char16_t *dir)
{
	char16_t *driveEnd, *file, *ext;
	SplitPathPtrs(mPath, driveEnd, file, ext);

	return SafeReplace(file, file, dir, NULL, 0, dir != file ? EA_FILE_PATH_SEPARATOR_16 : (char16_t)0);
}


bool FilePath::SetDrive(const char16_t* pDrive, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDriveStart, kPosDirectoryStart, pDrive, bInputIsPath);
}


bool FilePath::SetDirectory(const char16_t* pDirectory, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDirectoryStart, kPosFileNameStart, pDirectory, bInputIsPath);
}


bool FilePath::SetFileName(const char16_t* pFileName, bool bInputIsPath)
{
	return ReplacePathComponent(kPosFileNameStart, kPosExtensionStart, pFileName, bInputIsPath);
}


bool FilePath::SetExtension(const char16_t* pExtension, bool bInputIsPath)
{
	return ReplacePathComponent(kPosExtensionStart, kPosEnd, pExtension, bInputIsPath);
}


bool FilePath::SetDriveAndDirectory(const char16_t* pDriveAndDirectory, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDriveStart, kPosFileNameStart, pDriveAndDirectory, bInputIsPath);
}


bool FilePath::SetFileNameAndExtension(const char16_t* pFileNameAndExtension, bool bInputIsPath)
{
	return ReplacePathComponent(kPosFileNameStart, kPosEnd, pFileNameAndExtension, bInputIsPath);
}


bool FilePath::SetPath(const char16_t* pPath)
{
	// It is possible that the input path is a relative path. 
	// If so, then FullPath function will resolve it.
	char16_t pBaseDirectoryEmpty(0); // This tells FullPath that we don't want it guessing some default directory.
	FullPath(mPath, pPath, &pBaseDirectoryEmpty);
	return true;
}


bool FilePath::SetPath(const char16_t* pDrive, const char16_t* pDirectoryOnly, const char16_t* pFileNameOnly, const char16_t* pExtension)
{
	if (!SetDrive(pDrive)             ||
		!SetDirectory(pDirectoryOnly) ||
		!SetFileName(pFileNameOnly)   ||
		!SetExtension(pExtension))
	{
		Clear();
		return false;
	}

	return true;
}


void FilePath::Clear()
{
	mPath[0] = 0;
}


int FilePath::CompareSubstrings(const char16_t *pStart1, const char16_t *pEnd1, const char16_t *pStart2, const char16_t *pEnd2, FileSystem fileSystemType)
{
	const ptrdiff_t len1 = pEnd1 - pStart1;
	const ptrdiff_t len2 = pEnd2 - pStart2;

	ptrdiff_t len = LOCAL_MAX(len1, len2);

	if (!len)
		return 0;

	char16_t c1, c2;
	char16_t s1, s2;

	if(((unsigned)fileSystemType - (unsigned)kFileSystemFAT) <= (unsigned)(kFileSystemNTFS - kFileSystemFAT)) // If a Windows-based file system...
	{
		s1 = EA_FILE_PATH_SEPARATOR_16;
		s2 = EA_FILE_PATH_SEPARATOR_ALT_16;
	}
	else
		s1 = s2 = EA_FILE_PATH_SEPARATOR_16;

	// For the purposes of comparison, we convert all separators to -1. This
	// ensures that both regular and alt separators are considered equal and
	// forces directories to sort to the top.

	if(IsFileSystemCaseSensitive(fileSystemType))
	{
		do
		{
			c1 = *pStart1++;
			c2 = *pStart2++;

			while(c1 != c2)
			{
				if (c1 == s1 || c1 == s2)
				{
					c1 = (char16_t)-1;
					continue;
				}

				if (c2 == s1 || c2 == s2)
				{
					c2 = (char16_t)-1;
					continue;
				}

				return (c2-c1);
			}
		} while(--len);
	}
	else
	{
		do
		{
			c1 = EA::StdC::Tolower(*pStart1++);
			c2 = EA::StdC::Tolower(*pStart2++);

			while(c1 != c2)
			{
				if (c1 == s1 || c1 == s2)
				{
					c1 = (char16_t)-1;
					continue;
				}

				if (c2 == s1 || c2 == s2)
				{
					c2 = (char16_t)-1;
					continue;
				}

				return (c2-c1);
			}
		} while(--len);
	}

	return 0;
}


int FilePath::CompareSubstrings(PathPos start, PathPos end, const char16_t* p1, const char16_t* p2, FileSystem fileSystemType, bool bComponentwise)
{
	const char16_t* comp1[kPosCount] = { p1 };
	const char16_t* comp2[kPosCount] = { p2 };

	SplitPathPtrs(p1, comp1[kPosDirectoryStart], comp1[kPosFileNameStart], comp1[kPosExtensionStart]);
	SplitPathPtrs(p2, comp2[kPosDirectoryStart], comp2[kPosFileNameStart], comp2[kPosExtensionStart]);

	if (bComponentwise)
	{
		while(start < end)
		{
			const int result = CompareSubstrings(comp1[start], comp1[start + 1], comp2[start], comp2[start + 1], fileSystemType);
			if(result)
				return result;
		}

		return 0;
	}

	return CompareSubstrings(comp1[start], comp1[end], comp2[start], comp2[end], fileSystemType);
}


int FilePath::CompareDrive(const FilePath& path, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosDriveStart, kPosDirectoryStart, mPath, path.mPath, fileSystemType, false);
}


int FilePath::CompareDirectory(const FilePath& path, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosDirectoryStart, kPosFileNameStart, mPath, path.mPath, fileSystemType, false);
}


int FilePath::CompareFileName(const FilePath& path, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosFileNameStart, kPosExtensionStart, mPath, path.mPath, fileSystemType, false);
}


int FilePath::CompareExtension(const FilePath& path, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosExtensionStart, kPosEnd, mPath, path.mPath, fileSystemType, false);
}

int FilePath::ComparePath(const FilePath& path, bool bComponentwise, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosDriveStart, kPosEnd, mPath, path.mPath, fileSystemType, bComponentwise);
}


int FilePath::CompareDriveAndDirectory(const FilePath& path, bool bComponentwise, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosDriveStart, kPosFileNameStart, mPath, path.mPath, fileSystemType, bComponentwise);
}

int FilePath::CompareFileNameAndExtension(const FilePath& path, bool bComponentwise, FileSystem fileSystemType) const
{
	return CompareSubstrings(kPosFileNameStart, kPosEnd, mPath, path.mPath, fileSystemType, bComponentwise);
}

size_t FilePath::SafeSubstringCopy(const char16_t* begin, const char16_t* end, char16_t* dst, size_t capacity)
{
	if (!end)
		end = begin + EA::StdC::Strlen(begin);

	const size_t nRequiredStrlen = (size_t)(end - begin);

	if (nRequiredStrlen < capacity) // If there is enough space...
	{
		EA::StdC::Memcpy(dst, begin, nRequiredStrlen * sizeof(char16_t));
		dst[nRequiredStrlen] = 0;
	}

	return nRequiredStrlen;
}


bool FilePath::SafeReplace(char16_t* replaceBegin, char16_t* replaceEnd, const char16_t* srcBegin, const char16_t* srcEnd, char16_t prefixChar, char16_t suffixChar)
{
	if (!replaceEnd)
		replaceEnd = replaceBegin + EA::StdC::Strlen(replaceBegin);

	if (!srcEnd)
		srcEnd = srcBegin + EA::StdC::Strlen(srcBegin);

	const size_t srcLen  = (size_t)(srcEnd - srcBegin);
	const size_t headLen = (size_t)(replaceBegin - mPath);
	const size_t tailLen = EA::StdC::Strlen(replaceEnd);

	// check for missing prefix/postfix chars
	if (srcLen)
	{
		if (srcBegin[0] == prefixChar)
			prefixChar = 0;

		if (srcEnd[-1] == suffixChar)
			suffixChar = 0;
	}

	size_t extraLen = 0;

	if (prefixChar)
		++extraLen;

	if (suffixChar)
		++extraLen;

	// check for buffer overflow and bail if so
	if (headLen + srcLen + tailLen + extraLen + 1 > sizeof mPath / sizeof mPath[0])          // '+1' for null terminator.
		return false;

	// move tail and insert
	EA::StdC::Memmove(replaceBegin + srcLen + extraLen, replaceEnd, (tailLen + 1) * sizeof(char16_t));

	if (prefixChar)
	{
		replaceBegin[0] = prefixChar;
		EA::StdC::Memcpy(replaceBegin+1, srcBegin, srcLen * sizeof(char16_t));
	}
	else
		EA::StdC::Memcpy(replaceBegin, srcBegin, srcLen * sizeof(char16_t));

	if (suffixChar)
		replaceBegin[srcLen + extraLen - 1] = suffixChar;
	return true;
}


bool FilePath::ReplacePathComponent(PathPos start, PathPos end, const char16_t* pSrc, bool bInputIsPath)
{
	const char16_t* pSrcBegin = pSrc;
	const char16_t* pSrcEnd   = NULL;

	if (bInputIsPath)
	{
		const char16_t* srcComponents[kPosCount] = { pSrc };
		SplitPathPtrs(pSrc, srcComponents[kPosDirectoryStart], srcComponents[kPosFileNameStart], srcComponents[kPosExtensionStart]);
		pSrcBegin = srcComponents[start];
		pSrcEnd   = srcComponents[end];
	}

	char16_t* dstComponents[kPosCount + 1] = { mPath };
	SplitPathPtrs(mPath, dstComponents[kPosDirectoryStart], dstComponents[kPosFileNameStart], dstComponents[kPosExtensionStart]);

	// Directories must be postfixed with a backslash, and extensions prefixed
	// with a period, if the string is non-empty.
	char16_t prefixChar = 0;
	char16_t suffixChar = 0;

	if (pSrcEnd != pSrcBegin)
	{
		if (start == kPosExtensionStart)
			prefixChar = EA_CHAR16('.');

		if (end == kPosFileNameStart)
			suffixChar = EA_FILE_PATH_SEPARATOR_16;
	}

	return SafeReplace(dstComponents[start], dstComponents[end], pSrcBegin, pSrcEnd, prefixChar, suffixChar);
}




///////////////8-bit
FilePath::FilePath(const char* pPath)
{
	mPath[0] = 0;
	SetPath(pPath);
}


FilePath::FilePath(const char* pDrive, const char* pDirectory, const char* pFileName, const char* pExtension)
{
	mPath[0] = 0;
	SetPath(pDrive, pDirectory, pFileName, pExtension);
}

size_t FilePath::GetDrive(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, mPath, capacity, pDir - mPath);
}


size_t FilePath::GetDirectory(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, pDir, capacity, pFile - pDir);
}


size_t FilePath::GetFileName(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, pFile, capacity, pExt - pFile);
}


size_t FilePath::GetExtension(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, pExt, capacity);
}


size_t FilePath::GetPath(char* dst, size_t capacity) const
{
	return EA::StdC::Strlcpy(dst, mPath, capacity);
}


size_t FilePath::GetFileNameAndExtension(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, pFile, capacity);
}


size_t FilePath::GetDriveAndDirectory(char* dst, size_t capacity) const
{
	const char16_t* pDir, *pFile, *pExt;
	SplitPathPtrs(mPath, pDir, pFile, pExt);
	return EA::StdC::Strlcpy(dst, mPath, capacity, pFile - mPath);
}

bool FilePath::AppendDirectory(const char* dir8)
{
	char16_t dir[kMaxDirectoryNameLength];
	EA::StdC::Strlcpy(dir, dir8, kMaxDirectoryNameLength);

	char16_t* driveEnd, *file, *ext;
	SplitPathPtrs(mPath, driveEnd, file, ext);

	return SafeReplace(file, file, dir, NULL, 0, dir != file ? EA_FILE_PATH_SEPARATOR_16 : (char16_t)0);
}

bool FilePath::SetDrive(const char* pDrive, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDriveStart, kPosDirectoryStart, pDrive, bInputIsPath);
}


bool FilePath::SetDirectory(const char* pDirectory, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDirectoryStart, kPosFileNameStart, pDirectory, bInputIsPath);
}


bool FilePath::SetFileName(const char* pFileName, bool bInputIsPath)
{
	return ReplacePathComponent(kPosFileNameStart, kPosExtensionStart, pFileName, bInputIsPath);
}


bool FilePath::SetExtension(const char* pExtension, bool bInputIsPath)
{
	return ReplacePathComponent(kPosExtensionStart, kPosEnd, pExtension, bInputIsPath);
}


bool FilePath::SetDriveAndDirectory(const char* pDriveAndDirectory, bool bInputIsPath)
{
	return ReplacePathComponent(kPosDriveStart, kPosFileNameStart, pDriveAndDirectory, bInputIsPath);
}


bool FilePath::SetFileNameAndExtension(const char* pFileNameAndExtension, bool bInputIsPath)
{
	return ReplacePathComponent(kPosFileNameStart, kPosEnd, pFileNameAndExtension, bInputIsPath);
}


bool FilePath::SetPath(const char* pPath8)
{
	char16_t pPath[kMaxPathLength];
	EA::StdC::Strlcpy(pPath, pPath8, kMaxPathLength);

	// It is possible that the input path is a relative path. 
	// If so, then ResolvePath function will resolve it.
	char16_t pBaseDirectoryEmpty(0); // This tells FullPath that we don't want it guessing some default directory.
	FullPath(mPath, pPath, &pBaseDirectoryEmpty); // used to use ResolvePath 
	return true;
}

bool FilePath::SetPath(const char* pDrive, const char* pDirectoryOnly, const char* pFileNameOnly, const char* pExtension)
{
	if (!SetDrive(pDrive)             ||
		!SetDirectory(pDirectoryOnly) ||
		!SetFileName(pFileNameOnly)   ||
		!SetExtension(pExtension))
	{
		Clear();
		return false;
	}

	return true;
}

bool FilePath::ReplacePathComponent(PathPos start, PathPos end, const char* pSrc8, bool bInputIsPath)
{
	char16_t pSrc[kMaxPathLength];
	EA::StdC::Strlcpy(pSrc, pSrc8, kMaxPathLength);

	const char16_t* pSrcBegin = pSrc;
	const char16_t* pSrcEnd   = NULL;

	if (bInputIsPath)
	{
		const char16_t* srcComponents[kPosCount] = { pSrc };
		SplitPathPtrs(pSrc, srcComponents[kPosDirectoryStart], srcComponents[kPosFileNameStart], srcComponents[kPosExtensionStart]);
		pSrcBegin = srcComponents[start];
		pSrcEnd   = srcComponents[end];
	}

	char16_t* dstComponents[kPosCount+1] = { mPath };
	SplitPathPtrs(mPath, dstComponents[kPosDirectoryStart], dstComponents[kPosFileNameStart], dstComponents[kPosExtensionStart]);

	// Directories must be postfixed with a backslash, and extensions prefixed
	// with a period, if the string is non-empty.
	char16_t prefixChar = 0;
	char16_t suffixChar = 0;

	if (pSrcEnd != pSrcBegin)
	{
		if (start == kPosExtensionStart)
			prefixChar = EA_CHAR16('.');

		if (end == kPosFileNameStart)
			suffixChar = EA_FILE_PATH_SEPARATOR_16;
	}

	return SafeReplace(dstComponents[start], dstComponents[end], pSrcBegin, pSrcEnd, prefixChar, suffixChar);
}


////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Standalone functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GetHasTrailingPathSeparator
//
EAIO_API bool GetHasTrailingPathSeparator(const char16_t* pDirectory, size_t nDirectoryLength)
{
	if(nDirectoryLength == kLengthNull)
		nDirectoryLength = EA::StdC::Strlen(pDirectory);
	return (nDirectoryLength && IsFilePathSeparator(pDirectory[nDirectoryLength - 1]));
}


///////////////////////////////////////////////////////////////////////////////
// GetHasTrailingPathSeparator
//
EAIO_API bool GetHasTrailingPathSeparator(const char* pDirectory, size_t nDirectoryLength)
{
	if(nDirectoryLength == kLengthNull)
		nDirectoryLength = EA::StdC::Strlen(pDirectory);
	return (nDirectoryLength && IsFilePathSeparator(pDirectory[nDirectoryLength - 1]));
}


///////////////////////////////////////////////////////////////////////////////
// EnsureTrailingPathSeparator
//
EAIO_API bool EnsureTrailingPathSeparator(char16_t* pDirectory, size_t nDirectoryLength)
{
	if(nDirectoryLength == kLengthNull)
		nDirectoryLength = EA::StdC::Strlen(pDirectory);
	
	if(!nDirectoryLength || !IsFilePathSeparator(pDirectory[nDirectoryLength - 1]))
	{
		pDirectory[nDirectoryLength + 0] = EA_FILE_PATH_SEPARATOR_16;
		pDirectory[nDirectoryLength + 1] = 0;
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// EnsureTrailingPathSeparator
//
EAIO_API bool EnsureTrailingPathSeparator(eastl::string16& sDirectory)
{
	if(sDirectory.empty() || !IsFilePathSeparator(sDirectory[sDirectory.length() - 1])) // Could also use *rbegin(), but our string class may not have such a thing.
	{
		sDirectory.push_back(EA_FILE_PATH_SEPARATOR_16);
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// RemoveTrailingPathSeparator
//
EAIO_API bool RemoveTrailingPathSeparator(char16_t* pDirectory, size_t nDirectoryLength)
{
	if(nDirectoryLength == kLengthNull)
		nDirectoryLength = EA::StdC::Strlen(pDirectory);

	// To do: This function will not work properly if the user passes in
	// an incomplete UNC-style path in the form of simply "\\".
	if(nDirectoryLength && IsFilePathSeparator(pDirectory[nDirectoryLength - 1]))
	{
		pDirectory[nDirectoryLength - 1] = 0;
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// RemoveTrailingPathSeparator
//
EAIO_API bool RemoveTrailingPathSeparator(char* pDirectory, size_t nDirectoryLength)
{
	if(nDirectoryLength == kLengthNull)
		nDirectoryLength = EA::StdC::Strlen(pDirectory);

	// To do: This function will not work properly if the user passes in
	// an incomplete UNC-style path in the form of simply "\\".
	if(nDirectoryLength && IsFilePathSeparator(pDirectory[nDirectoryLength - 1]))
	{
		pDirectory[nDirectoryLength - 1] = 0;
		return true;
	}

	return false;
}


EAIO_API size_t ConcatenatePathComponents(char16_t* pDestination, const char16_t* pPrefix, const char16_t* pSuffix)
{
	char16_t*             pDestinationCurrent = pDestination;
	const char16_t* const pPrefixSaved = pPrefix;

	// Possibly copy pPrefix to the destination
	if(pPrefix)
	{
		while((*pDestinationCurrent++ = *pPrefix++) != 0)
			{ } // Do nothing.
		pDestinationCurrent--; // Move pDestinationCurrent back to terminating zero.
	}

	// Possibly append a path separator. Don't add it if the prefix was absent, 
	// as that would mean appending it to nothing, whereas we only want to append
	// to a true path prefix.
	if(pPrefixSaved && *pPrefixSaved && ((pDestinationCurrent == pDestination) || !IsFilePathSeparator(pDestinationCurrent[-1])))
		*pDestinationCurrent++ = EA_FILE_PATH_SEPARATOR_16;

	// Possibly append the suffix.
	if(pSuffix)
	{
		while((*pDestinationCurrent++ = *pSuffix++) != 0)
			{ } // Do nothing.
		--pDestinationCurrent;
	}
	else
		*pDestinationCurrent = 0;

	return (size_t)(pDestinationCurrent - pDestination);
}




///////////////////////////////////////////////////////////////////////////////
// StripTrailingPathComponent
//
// Results by example:
//     Path             PathComponent      Result            Notes
//     --------------------------------------------------------------
//     /a/b/c/d         Drive              /a/b/c/d
//                      Directory                           Perhaps this should be /a/b/c/d because strictly speaking d is a file name and not a directory name.
//                      DirectoryAlone     /a/b/c/          Perhaps this should be /a/b/c/d
//                      File               /a/b/c/
//                      FileAlone          /a/b/c/
//                      FileExtension      /a/b/c/d
//
//     /a/b/c/d/        Drive              /a/b/c/d/
//                      Directory            
//                      DirectoryAlone     /a/b/c/
//                      File               /a/b/c/d/
//                      FileAlone          /a/b/c/d/
//                      FileExtension      /a/b/c/d/
//
//     /a/b/c/d.e       Drive              /a/b/c/d.e
//                      Directory          /a/b/c/d.e
//                      DirectoryAlone     /a/b/c/d.e
//                      File               /a/b/c/
//                      FileAlone          /a/b/c/d.e
//                      FileExtension      /a/b/c/d
//
//     c:/a/b/c/d.e     Drive              c:/a/b/c/d.e
//                      Directory          c:/a/b/c/d.e
//                      DirectoryAlone     c:/a/b/c/d.e
//                      File               c:/a/b/c/
//                      FileAlone          c:/a/b/c/c.e
//                      FileExtension      c:/a/b/c/d
//
//     c:/              Drive                 
//                      Directory          c:/
//                      DirectoryAlone     c:/
//                      File               c:/
//                      FileAlone          c:/
//                      FileExtension      c:/
//
EAIO_API size_t StripTrailingPathComponent(char16_t* pPath, PathComponent pathComponent)
{
	switch (pathComponent)
	{
		case kPathComponentFile:
		case kPathComponentFileAlone:
		case kPathComponentDirectory:
		case kPathComponentDirectoryAlone:
		{
			// Conveniently, the file and directory stripping logic 
			// are nearly equivalent. For stripping a file, we simply 
			// search for the last directory separator and return 
			// whatever is after it.
			char16_t* pCurrent     = pPath;
			char16_t* pSeparator1 = pPath - 1;
			char16_t* pSeparator2 = pSeparator1;

			while(*pCurrent)
			{
				if(IsFilePathSeparator(*pCurrent))
				{
					pSeparator1 = pSeparator2; // Save it for future reference.
					pSeparator2 = pCurrent;
				}
				++pCurrent;
			}

			if((pathComponent < kPathComponentFile) && ((pSeparator2 + 1) == pCurrent)) // If it's a directory component and if the last character was a separator
				pSeparator2 = pSeparator1; // Go back to the previous separator. We have a directory like this: /a/b/c/d/ and need to return /a/b/c/

			*++pSeparator2 = 0;
			return (size_t)(pSeparator2 - pPath);
		}

		case kPathComponentFileExtension:
		{
			// We need to find the last '.' char of the last file name.
			// But we need to take care that we don't just search for the 
			// last '.' in the string, because that '.' might be part of 
			// a directory in the middle of the path.
			char16_t* pCurrent          = pPath;
			char16_t* pFinalSeparator = pPath - 1;
			char16_t* pFinalDot         = pPath - 1;

			while(*pCurrent)
			{
				if(IsFilePathSeparator(*pCurrent))
					pFinalSeparator = pCurrent;
				else if(*pCurrent == (char16_t)'.')
					pFinalDot = pCurrent;
				++pCurrent;
			}

			if(pFinalDot > pFinalSeparator) // If there was a '.' and it was part of a file name and not a directory...
			{
				*pFinalDot = 0;
				return (size_t)(pFinalDot - pPath);
			}
			return (size_t)(pCurrent - pPath); // No change occured.
		}

		case kPathComponentDrive:
		{
			// The standard path format is <volume>:/<directory>/<directory>/...
			// whereby <volume>: is not necessarily present, depending on the 
			// platform (some platforms don't have drives or volumes) and 
			// depending on the path (some path strings are relative paths or
			// standalone file names).
			// Here we strip the <volume>: part of the string if it is at the 
			// end of the path (i.e. if it is the only thing in the path).
			char16_t* pCurrent = pPath;

			while(*pCurrent && !IsFilePathSeparator(*pCurrent))
			{
				if(*pCurrent == (char16_t)':')
				{
					// If there is nothing after the colon or there is only a 
					// path separator after the colon, then strip the path.
					if(!pCurrent[1] || (IsFilePathSeparator(pCurrent[1]) && !pCurrent[2])) 
					{
						*pPath = 0;
						return 0;
					}
				}

				++pCurrent;
			}

			return (size_t)(pCurrent - pPath); // No change occured.
		}

		case kPathComponentNone:
		default:
			break;
	}

	// The pathComponent type is not supported or not possible, so we don't strip anything.
	return EA::StdC::Strlen(pPath);
}



///////////////////////////////////////////////////////////////////////////////
// SplitPathPtrs
//
// Purpose: Splits a path into its components.
//
// Example:
//     my_junk_drive:/home/stuff/bar/some misc junk.vcproj
//     <---drive----><----- dir-----><----file----><-ext->
//
// Pointers are returned to the start of each component. All pointers are
// valid and will always point somewhere in the string, although some may be
// equal to others or pointing to the null terminator, indicating an absent
// component.
//
// This function expects paths to be in a format compatible with the
// operating system being run. It is an error, for example, if we
// are running Unix and the path being passed in has DOS driver letters
// in it.
//
// UNC paths (\\server\share) are detected and moved into the drive section
// on all platforms.
//
EAIO_API void SplitPathPtrs(char16_t *pPath, char16_t *&pDirStart, char16_t *&pFileStart, char16_t *&pExtStart)
{
	const char16_t *dir = NULL, *file = NULL, *ext = NULL;

	SplitPathPtrs(pPath, dir, file, ext);

	pDirStart  = const_cast<char16_t *>(dir);
	pFileStart = const_cast<char16_t *>(file);
	pExtStart  = const_cast<char16_t *>(ext);
}

EAIO_API void SplitPathPtrs(const char16_t *pPath, const char16_t *&pDirStart, const char16_t *&pFileStart, const char16_t *&pExtStart)
{
	// Scan path for first and last separators and last period after last
	// separator.
	const char16_t *pFirstSeparator = NULL;
	const char16_t *pLastSeparator  = NULL;
	const char16_t *pLastPeriod     = NULL;
	const char16_t *s = pPath;

	while(char16_t c = *s)
	{
		if (IsFilePathSeparator(c))
		{
			if (!pFirstSeparator)
					pFirstSeparator = s;

			pLastSeparator = s;
			pLastPeriod = NULL;
		}

		if (c == EA_CHAR16('.'))
			pLastPeriod = s;

		++s;
	}

	// The directory starts immediately after the drive separator; if there is
	// no drive separator, then the directory starts at the path start. The
	// exception is if the path starts with something of the form:
	//
	//         \\server\share
	//
	// which indicates a UNC path. Only backslashes can be used for this, and
	// both the server and share must exist -- \\server alone isn't a valid
	// filesystem path.

	pDirStart = pPath;
	if (pPath[0] == EA_CHAR16('\\') && pPath[1] == EA_CHAR16('\\'))
	{
		int backslashes = 0;

		s = pPath + 2;

		while(char16_t c = *s)
		{
			if (c == EA_CHAR16('\\'))
			{
				++backslashes;
				if (backslashes == 2)
					break;
			}
			else if (IsFilePathSeparator(c))
				break;

			++s;
		}

		pDirStart = s;

		if (pLastSeparator && pLastSeparator < s)
			pLastSeparator = *s ? s : NULL;

		if (pLastPeriod && pLastPeriod < s)
			pLastPeriod = NULL;

	}
	else if (pFirstSeparator && *pFirstSeparator == EA_FILE_PATH_DRIVE_SEPARATOR_16)
		pDirStart = pFirstSeparator+1;

	// The filename starts immediately after the last separator; if there is
	// none then it starts after the drive prefix.
	pFileStart = pDirStart;
	if (pLastSeparator)
		pFileStart = pLastSeparator+1;

	// The extension starts at the first period after the last separator.
	if (pLastPeriod)
		pExtStart = pLastPeriod;
	else
	{
		// No extension, so use the null terminator. Note that we return a
		// pointer to the null terminator and not a null pointer.
		pExtStart = s;
		while(*pExtStart)
			++pExtStart;
	}
}

EAIO_API void SplitPathPtrs(char* pPath, char*& pDirStart, char*& pFileStart, char*& pExtStart)
{
	const char* dir, *file, *ext;

	SplitPathPtrs(pPath, dir, file, ext);

	pDirStart  = const_cast<char* >(dir);
	pFileStart = const_cast<char* >(file);
	pExtStart  = const_cast<char* >(ext);
}


EAIO_API void SplitPathPtrs(const char* pPath, const char*& pDirStart, const char*& pFileStart, const char*& pExtStart)
{
	// Scan path for first and last separators and last period after last
	// separator.
	const char* pFirstSeparator = NULL;
	const char* pLastSeparator  = NULL;
	const char* pLastPeriod      = NULL;
	const char* s = pPath;

	while(char c = *s)
	{
		if (IsFilePathSeparator(c))
		{
			if (!pFirstSeparator)
					pFirstSeparator = s;

			pLastSeparator = s;
			pLastPeriod = NULL;
		}

		if (c == ('.'))
			pLastPeriod = s;

		++s;
	}

	// The directory starts immediately after the drive separator; if there is
	// no drive separator, then the directory starts at the path start. The
	// exception is if the path starts with something of the form:
	//
	//         \\server\share
	//
	// which indicates a UNC path. Only backslashes can be used for this, and
	// both the server and share must exist -- \\server alone isn't a valid
	// filesystem path.

	pDirStart = pPath;
	if (pPath[0] == ('\\') && pPath[1] == ('\\'))
	{
		int backslashes = 0;

		s = pPath + 2;

		while(char c = *s)
		{
			if (c == ('\\'))
			{
				++backslashes;
				if (backslashes == 2)
					break;
			}
			else if (IsFilePathSeparator(c))
				break;

			++s;
		}

		pDirStart = s;

		if (pLastSeparator && pLastSeparator < s)
			pLastSeparator = *s ? s : NULL;

		if (pLastPeriod && pLastPeriod < s)
			pLastPeriod = NULL;

	}
	else if (pFirstSeparator && *pFirstSeparator == kFilePathDriveSeparator8)
		pDirStart = pFirstSeparator+1;

	// The filename starts immediately after the last separator; if there is
	// none then it starts after the drive prefix.
	pFileStart = pDirStart;
	if (pLastSeparator)
		pFileStart = pLastSeparator+1;

	// The extension starts at the first period after the last separator.
	if (pLastPeriod)
		pExtStart = pLastPeriod;
	else
	{
		// No extension, so use the null terminator. Note that we return a
		// pointer to the null terminator and not a null pointer.
		pExtStart = s;
		while(*pExtStart)
			++pExtStart;
	}
}




///////////////////////////////////////////////////////////////////////////////
// SplitPath
//
// Purpose:
//     Splits a file path into its individual components
//
// Entry:
//     pPath       - pointer to path name to be parsed.
//     pDrive      - pointer to buffer for drive component, if any.
//     pDirectory  - pointer to buffer for subdirectory component, if any.
//     pFileName   - pointer to buffer for file base name component, if any.
//     pExtension  - pointer to buffer for file name extension component, if any.
//
// Exit:
//     pDrive      - pointer to drive string. Includes ':' if a drive was given.
//     pDirectory  - pointer to subdirectory string. Includes leading and trailing '/' or '\', if any.
//     pFileName   - pointer to file base name.
//     pExtension  - pointer to file extension, if any. Includes leading '.'.
//
// This function expects paths to be in a format compatible with the
// operating system being run. It is an error, for example, if we
// are running Unix and the path being passed in has DOS driver letters
// in it.
//
///////////////////////////////////////////////////////////////////////////
//
// We assume that the path argument has the following form, where any
// or all of the components may be missing.
//     <drive><dir><fname><ext>
// and each of the components has the following expected form(s)
//
// Drive:
//     0 to kMaxDriveLength-1 characters, the last of which, if any, is a ':'
//     On platforms that don't have drive letters, like Unix and BeOS,
//     the driver will be an empty string.
//
// Directory:
//     0 to kMaxDirectoryLength-1 characters in the form of an absolute path
//     (leading '/' or '\') or relative path, the last of which, if
//     any, must be a '/' or '\'.  E.g -
//     absolute path:
//          \top\next\last\ ; or
//          /top/next/last/
//     relative path:
//          top\next\last\  ; or
//          top/next/last/
//     Mixed use of '/' and '\' within a path is also tolerated
//
// FileName:
//     0 to kMaxFileNameLength - 1 characters.
//
// Extension:
//     0 to kMaxExtensionLength - 1 characters where, if any, the first must be a '.'.
//
///////////////////////////////////////////////////////////////////////////
//
EAIO_API void SplitPath(const char16_t* pPath, char16_t* pDrive, char16_t* pDirectory, char16_t* pFileName, char16_t* pExtension, FileSystem /*fileSystemType*/)
{
	const char16_t *pDirStart, *pFileNameStart, *pExtStart;

	SplitPathPtrs(pPath, pDirStart, pFileNameStart, pExtStart);

	const size_t driveLen = LOCAL_MIN(kMaxDriveLength, pDirStart - pPath + 1);
	const size_t dirLen   = LOCAL_MIN(kMaxDirectoryLength, pFileNameStart - pDirStart + 1);
	const size_t nameLen  = LOCAL_MIN(kMaxFileNameLength, pExtStart - pFileNameStart + 1);

	if (pDrive)
		EA::StdC::Strlcpy(pDrive, pPath, driveLen);

	if (pDirectory) 
		EA::StdC::Strlcpy(pDirectory, pDirStart, dirLen);

	if (pFileName)
		EA::StdC::Strlcpy(pFileName, pFileNameStart, nameLen);

	if (pExtension)
		EA::StdC::Strlcpy(pExtension, pExtStart, kMaxExtensionLength);
}



///////////////////////////////////////////////////////////////////////////////
// SplitDirectory
//
EAIO_API size_t SplitDirectory(const char16_t* pDirectory, char16_t pDirectoryNames[][kMaxPathLength], size_t nDirectoryNamesCount, FileSystem /*fileSystemType*/)
{
	const size_t nDirectoryPathLength(EA::StdC::Strlen(pDirectory));

	if(nDirectoryPathLength > 0)
	{
		size_t nPreviousSeparatorPosition(nDirectoryPathLength - 1); //The last character in the directory path
		size_t nPosition(nDirectoryPathLength - 1);
		size_t nDirectoryIndex(0);
		size_t nCharCount;

		if(IsFilePathSeparator(pDirectory[nPosition]))
			nPosition--; // We ignore the last path separator while starting the loop below.

		if(!pDirectoryNames)
			nDirectoryNamesCount = (size_t)-1; // Put no limit on the count.

		while(((intptr_t)nPosition >= 0) && (nDirectoryNamesCount > (nDirectoryIndex + 1)))
		{
			const bool bIsFilePathSeparator = IsFilePathSeparator(pDirectory[nPosition]);

			if(bIsFilePathSeparator || (nPosition == 0))
			{
				if(bIsFilePathSeparator)
				{
					nCharCount = nPreviousSeparatorPosition - nPosition;
					if(pDirectoryNames)
						EA::StdC::Strncpy(pDirectoryNames[nDirectoryIndex], pDirectory + nPosition + 1, nCharCount);
					nPreviousSeparatorPosition = nPosition;
				}
				else
				{
					nCharCount = nPreviousSeparatorPosition;
					if(pDirectoryNames)
						EA::StdC::Strncpy(pDirectoryNames[nDirectoryIndex], pDirectory, nCharCount);
					nPreviousSeparatorPosition = 0;
				}

				if((nCharCount > 0) && (nCharCount < (size_t)kMaxDirectoryLength))
				{
					if(pDirectoryNames)
					{
						if(IsFilePathSeparator(pDirectoryNames[nDirectoryIndex][nCharCount - 1]))
							pDirectoryNames[nDirectoryIndex][nCharCount] = 0;
						else
						{
							pDirectoryNames[nDirectoryIndex][nCharCount]     = EA_FILE_PATH_SEPARATOR_16;
							pDirectoryNames[nDirectoryIndex][nCharCount + 1] = 0;
						}
					}
				}
				else{               // The directory string is invalid. It has an empty directory (e.g. C:/SomeDir//AnotherDir/)
					goto Abort;     // or it has a directory with too many characters.
				}
				nDirectoryIndex++;
			}
			nPosition--;
		}

		if((nPreviousSeparatorPosition > 0) && (nDirectoryNamesCount > nDirectoryIndex))
		{
			nCharCount = nPreviousSeparatorPosition;

			if((nCharCount > 0) && (nCharCount < (size_t)kMaxDirectoryLength))
			{
				if(pDirectoryNames)
				{
					EA::StdC::Strncpy(pDirectoryNames[nDirectoryIndex], pDirectory, nCharCount);

					if(IsFilePathSeparator(pDirectoryNames[nDirectoryIndex][nCharCount-1]))
						pDirectoryNames[nDirectoryIndex][nCharCount] = 0;
					else
					{
						pDirectoryNames[nDirectoryIndex][nCharCount]      = EA_FILE_PATH_SEPARATOR_16;
						pDirectoryNames[nDirectoryIndex][nCharCount + 1] = 0;
					}
				}
				nDirectoryIndex++;
			}
			else{               // The directory string is invalid. It has an empty directory (e.g. C:/SomeDir//AnotherDir/)
				goto Abort;     // or it has a directory with too many characters.
			}
		}

		return nDirectoryIndex;
	}

	Abort:
	return 0;
}


EAIO_API bool SplitDirectory(const char16_t* pDirectoryInput, char16_t* pFirstPart, char16_t* pLastPart)
{
	char16_t pDirectoryNames[2][kMaxPathLength] = { { 0 }, { 0 } };
	size_t   nDirectoryCount(2);

	if(SplitDirectory(pDirectoryInput, pDirectoryNames, nDirectoryCount))
	{
		if(pFirstPart)
			EA::StdC::Strlcpy(pFirstPart, pDirectoryNames[1], kMaxDirectoryLength);

		if(pLastPart)
			EA::StdC::Strlcpy(pLastPart,  pDirectoryNames[0], kMaxDirectoryLength);

		return true;
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// MakePath
//
EAIO_API void MakePath(char16_t* pPath, const char16_t* pDrive, const char16_t* pDirectoryOnly, 
				const char16_t* pFileNameOnly, const char16_t* pExtension, FileSystem /*fileSystemType*/)
{
	const char16_t* pCurrent;

	// Copy drive if present.
	if(pDrive && *pDrive)
	{
		*pPath++ = *pDrive;
		*pPath++ = EA_FILE_PATH_DRIVE_SEPARATOR_16;
	}

	// Copy directory.
	pCurrent = pDirectoryOnly;
	if(pCurrent && *pCurrent)
	{
		do{
			*pPath++ = *pCurrent++;
		} while(*pCurrent);

		if(!IsFilePathSeparator(*(pCurrent - 1)))
			*pPath++ = EA_FILE_PATH_SEPARATOR_16;
	}

	// Copy file name.
	pCurrent = pFileNameOnly;
	if(pCurrent)
	{
		while(*pCurrent)
			*pPath++ = *pCurrent++;
	}

	// Copy extension.
	if(pExtension)
	{
		pCurrent = (char16_t*)pExtension;

		if(*pCurrent && (*pCurrent != char16_t('.')))
			*pPath++ = char16_t('.');

		while((*pPath++ = *pCurrent++) != 0)    // This (empty) loop copies pExtension and the null
		{}                                      // terminator at the end of it.
	}
	else
		*pPath = 0;
}



///////////////////////////////////////////////////////////////////////////////
// FullPath
//
EAIO_API char16_t* FullPath(char16_t* pPathResult, const char16_t* pPath, size_t /*nMaxLength*/, FileSystem fileSystemType)
{
	const int  kBasePathLength = (kMaxPathLength * 2) + 1;
	char16_t   pBaseDirectory[kBasePathLength];
	char16_t*  pSource = NULL;
	char16_t*  pDestination = NULL;

	pBaseDirectory[0] = 0;

	// What we do here is fill in the base path with the current working directory.
	if(((unsigned)fileSystemType - (unsigned)kFileSystemFAT) <= (unsigned)(kFileSystemNTFS - kFileSystemFAT)) // If Windows-based
	{
		// Check for UNC name, only append drive name if not.
		pSource = (char16_t*)pPath;

		const bool bUNC = pSource[0] && pSource[1] && IsFilePathSeparator(pSource[0]) && IsFilePathSeparator(pSource[1]);

		if(bUNC)
		{
			pDestination   = pBaseDirectory;
		   *pDestination++ = EA_FILE_PATH_SEPARATOR_16;
		   *pDestination++ = EA_FILE_PATH_SEPARATOR_16;
		   *pDestination++ = 0;
			pSource += 2;
		}
		else // Else conventional drive-based path.
		{
			int nDriveLetter, nDrive = -1; // A drive of 0 means 'a', 1 means 'b', etc.

			// Check for a drive name. If not provided, get the current drive. 
			// Then point pSource at the first character in pPath after the drive name, if any.
			if(pSource[0] && pSource[1] && (pSource[1] == EA_FILE_PATH_DRIVE_SEPARATOR_16) && (((unsigned)(EA::StdC::Tolower(pSource[0]) - 'a')) <= (unsigned)('z' - 'a')))
			{
				nDriveLetter = EA::StdC::Tolower(pSource[0]);
				nDrive       = (nDriveLetter - 'a');
				pSource     += 2;
			}
			else
			{
				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
					nDrive = (int)(_getdrive() - 1); // Get current drive. _getdrive returns 1 for 'a', 2 for 'b', etc.
				#endif
			}

			if (IsFilePathSeparator(pSource[0]) && nDrive >= 0) // If path is absolute (e.g. begins with C:\ or just \"), store the drivename.
			{
				if((unsigned)nDrive < 26)
					nDriveLetter = (int)('a' + nDrive);
				else
					nDriveLetter = (int)'c';

				pDestination   = pBaseDirectory;
			   *pDestination++ = (char16_t)nDriveLetter;
			   *pDestination++ = EA_FILE_PATH_DRIVE_SEPARATOR_16;
			   *pDestination++ = EA_FILE_PATH_SEPARATOR_16;
			   *pDestination++ = 0;
				pSource++; // Move the source ptr past the slash in "C:\", for example.
			}
			else // else path is relative. Input path is of form "..\.." or
			{
				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
				// if (nDrive >= 0) // Note: This line has been disabled because it prevents UNC paths from working. It is not clear why this line exists, but perhaps there was a good reason.
					if (_wgetdcwd( nDrive + 1, reinterpret_cast<wchar_t *>(pBaseDirectory), kBasePathLength ) == NULL)
						Directory::GetCurrentWorkingDirectory( pBaseDirectory );
				#else
					Directory::GetCurrentWorkingDirectory( pBaseDirectory );
				#endif

				const size_t nBasePathLength = EA::StdC::Strlen( pBaseDirectory );
				pDestination = &pBaseDirectory[ nBasePathLength ];

				if(!IsFilePathSeparator(pDestination[-1]))  // If directory doesn't end in a path separator... append one
				{
					*pDestination++ = EA_FILE_PATH_SEPARATOR_16; 
					*pDestination++ = 0;  // Make sure we remain null-terminated.
				}
			}
		}
	}
	else // Unix and similar.
	{
		pSource = (char16_t*)pPath;
		Directory::GetCurrentWorkingDirectory( pBaseDirectory );

		const size_t nBasePathLength = EA::StdC::Strlen(pBaseDirectory);
		pDestination = &pBaseDirectory[nBasePathLength];

		if(!IsFilePathSeparator(pDestination[-1]))  // If directory doesn't end in path separator... append one
		{
			*pDestination++ = EA_FILE_PATH_SEPARATOR_16;
			*pDestination++ = 0; // Make sure we remain null-terminated.
		}
	}

	// Now call the base function to finish the work.
	return FullPath(pPathResult, pSource, pBaseDirectory, fileSystemType);
}


///////////////////////////////////////////////////////////////////////////////
// FullPath
//
// Fixes a path that is implicitly relative and/or has specific relative path specifiers.
// Returns the input buffer or NULL upon failure.
// Destination pointer must point to enough space to hold a maximum possible length value.
//
EAIO_API char16_t* FullPath(char16_t* pPathResult, const char16_t* pPath, const char16_t* pBaseDirectory, FileSystem fileSystemType)
{
	if(!pPath)
	{
		EA_FAIL_MESSAGE("FullPath: Null pPath argument.");
		return NULL;
	}

	if(!pBaseDirectory)
		return FullPath(pPathResult, pPath, kMaxPathLength, fileSystemType);

	const char16_t* src;
	const char16_t* srcLimit;
		  char16_t* dst       = pPathResult;
		  char16_t* dstLimit  = pPathResult + kMaxPathLength;
		  char16_t  c;

	// If pBaseDirectory is empty (but non-NULL), we assume that any
	// existing drive part of pPath is the base directory instead.
	src = pBaseDirectory;
	srcLimit = src + EA::StdC::Strlen(src);
	if (src == srcLimit)
	{
		const char16_t *pPathDir = NULL, *pPathFile = NULL, *pPathExt = NULL;
		SplitPathPtrs(pPath, pPathDir, pPathFile, pPathExt);

		// Move a leading separator on the directory to the trailing end of
		// the drive path, if it exists (convention says that paths should
		// end in a separator).
		if (IsFilePathSeparator(*pPathDir))
			++pPathDir;

		// Set the base directory to the drive portion of pPath.
		src = pPath;
		srcLimit = pPathDir;

		// Advance pPath to the start of the relative path.
		pPath = pPathDir;
	}

	// Copy the base directory.
	if (src != srcLimit)
	{
		while(src != srcLimit)
		{
			c = *src++;

			// fix path separators
			if (c == EA_FILE_PATH_SEPARATOR_ALT_16)
				c = EA_FILE_PATH_SEPARATOR_16;

			if (dst == dstLimit)
			{
				EA_FAIL_MESSAGE("FullPath: Destination limit reached.");
				dstLimit[-1] = 0;
				return NULL;
			}

			*dst++ = c;
		}

		// Add path separator if necessary.
		if (!IsFilePathSeparator(dst[-1]))
		{
			if (dst == dstLimit)
			{
				EA_FAIL_MESSAGE("FullPath: Destination limit reached.");
				dstLimit[-1] = 0;
				return NULL;
			}
			*dst++ = EA_FILE_PATH_SEPARATOR_16;

		}
	}

	// Make sure we have room for a null terminator.
	if (dst == dstLimit)
	{
		EA_FAIL_MESSAGE("FullPath: Destination limit reached.");
		dstLimit[-1] = 0;
		return NULL;
	}
	*dst = 0;

	// Determine root of base path. We use this to keep /.. from backing into
	// the share of a UNC path.
	const char16_t *pRootEnd = NULL, *pFileNameStart = NULL, *pExtStart = NULL;

	SplitPathPtrs(pPathResult, pRootEnd, pFileNameStart, pExtStart);

	// If there is a separator after the drive, protect it so we don't accidentally
	// change an absolute path to a drive-relative one.
	if (IsFilePathSeparator(*pRootEnd))
		++pRootEnd;

	// If a base drive (Windows concept only) was copied over, we assume that it is 
	// safe to rip off redundant parent components at the start of the path. Otherwise, 
	// we assume that the path is relative to an unknown base, and must not remove 
	// those components.
	#if defined(EA_FILE_PATH_SEPARATOR_TYPE_UNIX) && EA_FILE_PATH_SEPARATOR_TYPE_UNIX
	bool preserveLeadingRelativeSpecifiersAtStart = false;
	#else
	bool preserveLeadingRelativeSpecifiersAtStart = (pRootEnd == pPathResult);
	#endif
	bool preserveLeadingRelativeSpecifiers = preserveLeadingRelativeSpecifiersAtStart;

	// Copy components from path, applying the following transformations:
	//
	// ./       -> deleted
	// xxx/../  -> deleted
	// //       -> /

	src = pPath;

	bool bComponentStart = true;
	do
	{
		c = *src++;

		// eat redundant separators
		if (IsFilePathSeparator(c))
		{
			if (bComponentStart)
				continue;

			bComponentStart = true;

			// fix path separators
			if (c == EA_FILE_PATH_SEPARATOR_ALT_16)
				c = EA_FILE_PATH_SEPARATOR_16;
		}
		else
		{
			// look for . and ..
			if (c != '.')
			{
				preserveLeadingRelativeSpecifiers = false;
			}
			else if (bComponentStart)
			{
				char16_t d = src[0];

				if (!d || IsFilePathSeparator(d))
				{
					// We have a '.' component, which is ordinarily a no-op.
					// However, to avoid producing unexpected results like
					// an empty path, we only remove it if it is preceded
					// by a path separator.
					if ((dst > pPathResult) && IsFilePathSeparator(dst[-1]))
						continue;
				}
				else if (d == '.' && (!src[1] || IsFilePathSeparator(src[1])))
				{
					if (!preserveLeadingRelativeSpecifiers)
					{
						// Remove last component and then delete.
						char16_t *dstAfterDelete = dst;
						int separatorsEaten = 0;

						while(dstAfterDelete > pRootEnd)
						{
							if (IsFilePathSeparator(dstAfterDelete[-1]))
							{
								if (++separatorsEaten > 1)
									break;
							}

							--dstAfterDelete;
						}

						// Make sure we aren't backing up over .., as that isn't allowed.
						if (   dstAfterDelete[0] != '.'
							|| dstAfterDelete[1] != '.'
							|| (dstAfterDelete[2] && !IsFilePathSeparator(dstAfterDelete[2])))
						{
							dst = dstAfterDelete;

							// If we backed all the way to the start, reset the "preserve" flag.
							if (dst == pRootEnd)
							{
								preserveLeadingRelativeSpecifiers = preserveLeadingRelativeSpecifiersAtStart;
							}

							++src;
							continue;
						}
					}
				}
			}

			// Copy this component.
			bComponentStart = false;
		}

		// copy character
		if (dst == dstLimit) {
			EA_FAIL_MESSAGE("FullPath: Destination limit reached.");
			dstLimit[-1] = 0;
			return NULL;
		}
		*dst++ = c;
	} while(c);

	return pPathResult;
}


///////////////////////////////////////////////////////////////////////////////
// IsPathRooted
//
// Returns true if the input path begins at the root and has no relative 
// path specifiers internally. Path patterns are considered full paths.
//
// The following are rooted paths:
//    (Win32)       C:\Temp\SomeFile.txt
//    (Win32)       C:\Temp\                                // This comment here to appease GCC regarding trailing \ chars.
//    (Win32)       C:\Temp\*.txt
//    (Win32)       C:\Temp\..\Temp\                        // This comment here to appease GCC regarding trailing \ chars.
//    (Win32)       \\SomeServer\Temp\../Temp/*.txt
//    (Win32)       \\SomeServer\Temp\*.txt
//    (Unix)		/Temp/SomeFile.txt
//    (Unix)		/Temp/									// This comment here to appease GCC regarding trailing \ chars.
//    (Unix)		/Temp/*.txt
//    (Unix)		/Temp\..\Temp\							// This comment here to appease GCC regarding trailing \ chars.
//
// The following are not rooted paths:
//    (Win32)       Temp\SomeFile.txt
//    (Win32)       ..\Temp\SomeFile.txt
//    (Unix)		Temp/SomeFile.txt
//    (Unix)		../Temp/SomeFile.txt
//
EAIO_API bool IsPathRooted(const char16_t* pPath, FileSystem fileSystemType)
{
	if((unsigned)(fileSystemType - kFileSystemFAT) <= (unsigned)(kFileSystemNTFS - kFileSystemFAT)) // If Microsoft-derived...
	{
		const char16_t pDrivePattern[3] = { ':',  '\\', 0 }; // Drives must have '\' after the colon and not '/'.

		if(EA::StdC::Isalpha(pPath[0]) && EA::StdC::Strstr(pPath, pDrivePattern))  // Test for "C:\" or "GAME:\", for example.
			return true;

		if((pPath[0] == '\\') && (pPath[1] == '\\')) // If we pPath is a UNC path...
			return true;
	}
	else if(pPath[0] == EA_FILE_PATH_SEPARATOR_16) // If the path begins with a root directory...
	{
		return true;
	}
	else if(fileSystemType == kFileSystemUDFX)
	{
		const char16_t pDrivePattern[3] = { ':',  EA_FILE_PATH_SEPARATOR_16, 0 };
		
		if(EA::StdC::Isalpha(pPath[0]) && EA::StdC::Strstr(pPath, pDrivePattern))
		{
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// IsPathRelative
//
// Returns true if the input path has any relative path specifiers.
//
// The following have relative components:
//    (Win32) C:\Temp\..\Temp\                      // Win32
//    (Win32) \\SomeServer\Temp\../Temp/*.txt
//    (Win32) ..\Temp\SomeFile.txt
//    (Unix)  /Temp\..\Temp\                        // Win32
//    (Unix)  ../Temp/SomeFile.txt
//
EAIO_API bool IsPathRelative(const char16_t* pPath, FileSystem fileSystemType)
{
	if((unsigned)(fileSystemType - kFileSystemFAT) <= (unsigned)(kFileSystemNTFS - kFileSystemFAT)) // If Windows-derived...
	{
		const char16_t pattern[] = { EA_FILE_PATH_SEPARATOR_ALT_16, '.', 0 };

		if (EA::StdC::Strstr(pPath, pattern) != 0)
			return true;
	}

	const char16_t pattern[] = { EA_FILE_PATH_SEPARATOR_16, '.', 0 };  // "/." also covers "/.."
	if (EA::StdC::Strstr(pPath, pattern) != 0)
		return true;

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// IsFullPath
//
// Returns true if the input path begins at the root and has no relative 
// path specifiers internally. Path patterns are considered full paths.
//
// The following are full paths:
//    (Win32)       C:\Temp\SomeFile.txt
//    (Win32)       C:\Temp\                            // This comment here to appease GCC regarding trailing \ chars.
//    (Win32)       C:\Temp\*.txt
//    (Win32)       \\SomeServer\Temp\*.txt
//    (Unix)		/Temp/SomeFile.txt
//    (Unix)		/Temp/
//    (Unix)		/Temp/*.txt
//
// The following are not full paths:
//    (Win32)       Temp\SomeFile.txt
//    (Win32)       C:\Temp\..\Temp\                    // This comment here to appease GCC regarding trailing \ chars.
//    (Win32)       \\SomeServer\Temp\../Temp/*.txt
//    (Win32)       ..\Temp\SomeFile.txt
//    (Unix)		Temp/SomeFile.txt
//    (Unix)		/Temp\..\Temp\                      // This comment here to appease GCC regarding trailing \ chars.
//    (Unix)		../Temp/SomeFile.txt
//
EAIO_API bool IsFullPath(const char16_t* pPath, FileSystem fileSystemType)
{
	return IsPathRooted(pPath, fileSystemType) && !IsPathRelative(pPath, fileSystemType);
}


///////////////////////////////////////////////////////////////////////////////
// ResolvePath
//
// Resolves a relative path into an absolute path using a base path.
// If the input path is already absolute, it simply returns a copy of the input argument.
// @note This does the same thing as FullPath except when the input pPath argument is already absolute.
// Destination pointer must point to enough space to hold a maximum possible length value.
//
EAIO_API char16_t* ResolvePath(char16_t* pPathResult, const char16_t* pPath, const char16_t* pBaseDirectory, FileSystem fileSystemType)
{
	bool isRooted   = IsPathRooted  (pPath, fileSystemType);
	bool isRelative = IsPathRelative(pPath, fileSystemType);

	if (isRooted && !isRelative)
	{
		// Just copy the string
		if(EA::StdC::Strlen(pPath) < (kMaxPathLength - 1))
		{
			EA::StdC::Strlcpy(pPathResult, pPath, kMaxPathLength);
			return pPathResult;
		}

		EA_FAIL_MESSAGE("ResolvePath: Destination limit reached.");
		return NULL; // Ran out of space.
	}

	// Don't graft pPath on to base directory if we're already rooted.
	return FullPath(pPathResult, pPath, isRooted ? (char16_t*) "\0\0" : pBaseDirectory, fileSystemType); // OK if pBaseDirectory is NULL.
}


///////////////////////////////////////////////////////////////////////////////
// ComparePaths
//
// Return value is the same as with the standard C strcmp (string comparison) function.
// Takes into account things like case sensitivity, relative paths, Win32 aliases, and Win32 short file names.
//
EAIO_API int ComparePaths(const char16_t* pPath1, const char16_t* pPath2, FileSystem fileSystemType)
{
	// Todo: Consider checking for either file being a Win32 short file name or being a Win32 shortcut.
	// #if defined(EA_PLATFORM_WINDOWS)
	//     if(((unsigned)fileSystemType - (unsigned)kFileSystemFAT) <= (unsigned)(kFileSystemNTFS - kFileSystemFAT)) // If Windows-based...
	//     {
	//         // Check for short file names or Win32 shortcuts
	//     }
	// #endif

	char16_t path1Temp[kMaxPathLength];
	char16_t path2Temp[kMaxPathLength];
  
	ResolvePath(path1Temp, pPath1, NULL);
	ResolvePath(path2Temp, pPath2, NULL);

	if(!IsFileSystemCaseSensitive(fileSystemType))
	{
		EA::StdC::Strlwr(path1Temp);
		EA::StdC::Strlwr(path2Temp);
	}

	return EA::StdC::Strcmp(path1Temp, path2Temp);
}



///////////////////////////////////////////////////////////////////////////////
// GetFileSystem
//
EAIO_API FileSystem GetFileSystem(const char16_t* pPath)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		char16_t pDrive[kMaxDriveLength];
		wchar_t  pVolumeName[MAX_PATH + 1] = { 0 };
		DWORD    nVolumeNameSize(MAX_PATH + 1);
		DWORD    nMaximumComponentLength(0);
		DWORD    nFileSystemFlags(0);
		wchar_t  pFileSystemType[MAX_PATH + 1] = { 0 };
		DWORD    nFileSystemTypeSize(MAX_PATH + 1);

		SplitPath(pPath, pDrive, NULL, NULL, NULL);

		// This call will be slow (~1 second) if the input server cannot be found.
		if(GetVolumeInformationW( reinterpret_cast<LPCWSTR>(pDrive), pVolumeName, nVolumeNameSize,
								  NULL, &nMaximumComponentLength, &nFileSystemFlags, 
								  pFileSystemType, nFileSystemTypeSize))
		{
			if(EA::StdC::Stricmp(pVolumeName, L"NTFS") == 0)
				return kFileSystemNTFS;

			if(EA::StdC::Stricmp(pVolumeName, L"FAT32") == 0)
				return kFileSystemFAT32;
		}

		return kFileSystemFAT;

	#elif defined(EA_PLATFORM_UNIX)

		(void)pPath; // Prevent possible compiler warnings related to disuse.
		return kFileSystemUnix; // This isn't necessarily correct, but it will do for now.

	#else

		(void)pPath; // Prevent possible compiler warnings related to disuse.
		return kFileSystemUnix;

	#endif
}


///////////////////////////////////////////////////////////////////////////////
// IsFileSystemCaseSensitive
//
EAIO_API bool IsFileSystemCaseSensitive(FileSystem fileSystemType)
{
	return !((fileSystemType == kFileSystemWindows) || 
			 (fileSystemType == kFileSystemFAT)     || 
			 (fileSystemType == kFileSystemFAT32)   || 
			 (fileSystemType == kFileSystemNTFS));
}




///////////////////////////////////////////////////////////////////////////////
// GetNextPathComponent
//
// Returns a string (sCurrentComponent) of the characters from current position up to but not including the next path separator char.
// If there is no '/' (or platform-appropriate path separator), returns the rest of the string.
// Sets nCurrentPosition to be one past the '/' char or one past the last char in the string.
// May possibly return an empty string, in the case of two successive '/' chars.
// Returns true if there were any charaters left to read.
// Returns false if the input position is last the end of the string.
//
typedef eastl::fixed_string<char16_t, kMaxPathLength, true> PathString;

static bool GetNextPathComponent(const PathString& sInputPath, PathString& sCurrentComponent, eastl_size_t& nCurrentPosition)
{
	const eastl_size_t nInputPathLength = sInputPath.length();

	if(nCurrentPosition < nInputPathLength)
	{
		eastl_size_t i;

		for(i = nCurrentPosition; i < nInputPathLength; i++)
		{
			if(IsFilePathSeparator(sInputPath[i]))
			{
				sCurrentComponent.assign(sInputPath, nCurrentPosition, i - nCurrentPosition);
				nCurrentPosition = i+1;
				return true;
			}
		}

		sCurrentComponent.assign(sInputPath, nCurrentPosition, i-nCurrentPosition);
		nCurrentPosition = i;
		return true;
   }

   return false;
}


namespace EAFilePathLocal
{
	///////////////////////////////////////////////////////////////////////////////
	// Reserved Windows path names
	//
	// These reserved words (alone or with any extension) cannot be used 
	// as the name of a file, directory, server, or volume. We define these
	// outside #ifdef EA_PLATFORM_WINDOWS because this function is meant to 
	// be able to test Win32 paths while running under Unix, etc.

	static const char16_t kCON[]   = { 'c', 'o', 'n', 0 };
	static const char16_t kPRN[]   = { 'p', 'r', 'n', 0 };
	static const char16_t kAUX[]   = { 'a', 'u', 'x', 0 };
	static const char16_t kCLOCK[] = { 'c', 'l', 'o', 'c', 'k', '$', 0 };
	static const char16_t kNUL[]   = { 'n', 'u', 'l', 0 };
	static const char16_t kCOM1[]  = { 'c', 'o', 'm', '1', 0 };
	static const char16_t kCOM2[]  = { 'c', 'o', 'm', '2', 0 };
	static const char16_t kCOM3[]  = { 'c', 'o', 'm', '3', 0 };
	static const char16_t kCOM4[]  = { 'c', 'o', 'm', '4', 0 };
	static const char16_t kCOM5[]  = { 'c', 'o', 'm', '5', 0 };
	static const char16_t kCOM6[]  = { 'c', 'o', 'm', '6', 0 };
	static const char16_t kCOM7[]  = { 'c', 'o', 'm', '7', 0 };
	static const char16_t kCOM8[]  = { 'c', 'o', 'm', '8', 0 };
	static const char16_t kCOM9[]  = { 'c', 'o', 'm', '9', 0 };
	static const char16_t kLPT1[]  = { 'l', 'p', 't', '1', 0 };
	static const char16_t kLPT2[]  = { 'l', 'p', 't', '2', 0 };
	static const char16_t kLPT3[]  = { 'l', 'p', 't', '3', 0 };
	static const char16_t kLPT4[]  = { 'l', 'p', 't', '4', 0 };
	static const char16_t kLPT5[]  = { 'l', 'p', 't', '5', 0 };
	static const char16_t kLPT6[]  = { 'l', 'p', 't', '6', 0 };
	static const char16_t kLPT7[]  = { 'l', 'p', 't', '7', 0 };
	static const char16_t kLPT8[]  = { 'l', 'p', 't', '8', 0 };
	static const char16_t kLPT9[]  = { 'l', 'p', 't', '9', 0 };

	static const char16_t* kReservedNamesWin32[] =
	{
		kCON,  kPRN,  kAUX,  kCLOCK, kNUL,
		kCOM1, kCOM2, kCOM3, kCOM4,  kCOM5,
		kCOM6, kCOM7, kCOM8, kCOM9,  kLPT1,
		kLPT2, kLPT3, kLPT4, kLPT5,  kLPT6,
		kLPT7, kLPT8, kLPT9,
	};
}


///////////////////////////////////////////////////////////////////////////////
// IsPathValid
//
// Here are some helper URLs from Microsoft, though they are actually not complete:
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_longfile.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/storage_7mn9.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/volref_6wfi.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore98/HTML/_core_device_names_for_windows.asp
//
// Recognized file system types include:
//     Unix:      UNIX, 
//     Macintosh: HFS, 
//     Windows:   FAT (same as VFAT for Windows 95 and later), HPFS, NTFS 
//     CDROM:     UDF, CDFS (Windows), Joliet, Rockridge (Unix compatible)

EAIO_API bool IsPathValid(const char16_t* pPath, FileSystem fileSystemType)
{
	bool           bReturnValue = false;
	PathString     sPath(pPath);
	const char16_t kReservedCharactersWin32[] = { '<', '>', ':', '\"', '|', '*', '?', 0 }; // We don't include '/' and '\' because we deal with them separately.

	// Do path length tests
	#if defined(EA_PLATFORM_WINDOWS)
		if(sPath.length() < 3)                  // A length less than 3 chars cannot be a full path, as Windows requires at least a drive or UNC prefix.
			return false;

		if(sPath.find(EA_CHAR16("\\\\?\\UNC\\")) == 0)    // Under Windows, if a path is prefixed with `\\?\UNC\` then it has no length limit.
			sPath.erase(2, 6);                            // Convert (e.g.) `\\?\UNC\server\volume\Temp\temp.txt` to `\\server\volume\Temp\temp.txt`.
		else if(sPath.find(EA_CHAR16("\\\\?\\")) == 0)    // Under Windows, if a path is prefixed with `\\?\` then it has no length limit.
			sPath.erase(0, 4);                            // Convert (e.g.) `\\?\C:\Temp\temp.txt` to `C:\Temp\temp.txt`.
		else if(sPath.length() > kMaxPathLength)
			return false;
	#else
		if(sPath.length() < 1)                  // We treat a path of "/" as valid, as it is a valid directory path.
			return false;

		if(sPath.length() > (eastl_size_t)kMaxPathLength)     // Strictly speaking, some Unix systems allow more than this many characters.
			return false;
	#endif


	// Break up the path into separate components.
	// We'll analyze them below.

	PathString sDrive    ((eastl_size_t)kMaxDriveLength     - 1, 0);
	PathString sDirectory((eastl_size_t)kMaxDirectoryLength - 1, 0);
	PathString sFileName ((eastl_size_t)kMaxFileNameLength  - 1, 0);
	PathString sExtension((eastl_size_t)kMaxExtensionLength - 1, 0);

	SplitPath(sPath.c_str(), const_cast<char16_t*>(sDrive.data()), 
							 const_cast<char16_t*>(sDirectory.data()), 
							 const_cast<char16_t*>(sFileName.data()),
							 const_cast<char16_t*>(sExtension.data()),
							 fileSystemType);

	sDrive    .resize((eastl_size_t)EA::StdC::Strlen(sDrive.c_str()));
	sDirectory.resize((eastl_size_t)EA::StdC::Strlen(sDirectory.c_str()));
	sFileName .resize((eastl_size_t)EA::StdC::Strlen(sFileName.c_str()));
	sExtension.resize((eastl_size_t)EA::StdC::Strlen(sExtension.c_str()));


	// Do validation per file system
	if((fileSystemType == kFileSystemWindows) || 
	   (fileSystemType == kFileSystemFAT)     ||
	   (fileSystemType == kFileSystemFAT32)   ||
	   (fileSystemType == kFileSystemNTFS))
	{ 
		// Set the return value to true until proven false for the rest of this section.
		eastl_size_t nPosition = 0;
		bReturnValue = true;

		//Go through each directory and the file name in the path and see if 
		//it has any of the above reserved names. Note that directory names
		//can't be "com1.txt" any more than file names can't be "com1.txt".
		const char16_t kBackSlashBackSlash[] = { '\\', '\\', 0 };

		if(sPath.find(kBackSlashBackSlash) == 0) // If it is a UNC path... (\\<server>\<volume>\<dir path>). '/' is not accepted as part of the UNC path prefix.
		{
			const eastl_size_t nPosition2 = 1;
			const eastl_size_t nPosition3 = sPath.find('\\', 2);
			const eastl_size_t nPosition4 = sPath.find('\\', nPosition3 + 1); // We know from earlier that there are at least 4 '\' chars.

			if((nPosition3 > (nPosition2 + 1)) && (nPosition4 > (nPosition3 + 1)))
			{
				const PathString sServer(sPath, nPosition2 + 1, nPosition3 - nPosition2);
				const PathString sVolume(sPath, nPosition3 + 1, nPosition4 - nPosition3);

				// We're a little conservative with the server and volume name checks here.
				// Their limitations don't appear to be well documented, so we err on acceptance.
				if(sServer.find_first_of(kReservedCharactersWin32) != PathString::npos)
					bReturnValue = false;
				else if(sVolume.find_first_of(kReservedCharactersWin32) != PathString::npos)
					bReturnValue = false;
				else
				{
					PathString sServerLowerCaseMinusExtension(sServer);
					PathString sVolumeLowerCaseMinusExtension(sVolume);

					sServerLowerCaseMinusExtension.make_lower();
					const eastl_size_t nServerDotPosition(sServerLowerCaseMinusExtension.find('.'));    
					if(nServerDotPosition != PathString::npos)
						sServerLowerCaseMinusExtension.erase(nServerDotPosition);

					sVolumeLowerCaseMinusExtension.make_lower();
					const eastl_size_t nVolumeDotPosition(sVolumeLowerCaseMinusExtension.find('.'));    
					if(nVolumeDotPosition != PathString::npos)
						sVolumeLowerCaseMinusExtension.erase(nVolumeDotPosition);

					for(size_t i(0); i < sizeof(EAFilePathLocal::kReservedNamesWin32) / sizeof(EAFilePathLocal::kReservedNamesWin32[0]); i++)
					{
						if(sServerLowerCaseMinusExtension == EAFilePathLocal::kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}

						if(sVolumeLowerCaseMinusExtension == EAFilePathLocal::kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}
					}
					nPosition = nPosition4 + 1;
				}
			}
			else
				bReturnValue = false;
		}
		else
		{
			if((sPath.length() >= 3) && 
			   (EA::StdC::Isalpha(sPath[0])) &&
			   (sPath[1] == ':')     &&
			   (sPath[2] == '\\'))       // '/' (as opposed to '\') is allowed as directory separators but not after "C:". That's Microsoft for ya.
			{
				nPosition = 3;
			}
			else
				bReturnValue = false;
		}

		if(bReturnValue) // If the code directly above found everything OK so far...
		{
			PathString sCurrentComponent;

			while(bReturnValue && GetNextPathComponent(sPath, sCurrentComponent, nPosition))
			{
				if(bReturnValue)
				{
					if(sCurrentComponent.empty() || (sCurrentComponent.length() > (eastl_size_t)kMaxFileNameLength))
					{
						bReturnValue = false;
						break;
					}
				}

				// Do reserved word tests
				if(bReturnValue)
				{
					PathString sCurrentComponentLowerCaseMinusExtension(sCurrentComponent);

					sCurrentComponentLowerCaseMinusExtension.make_lower();
					const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));

					if(nDotPosition != PathString::npos)
						sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

					for(size_t i(0); i < sizeof(EAFilePathLocal::kReservedNamesWin32) / sizeof(EAFilePathLocal::kReservedNamesWin32[0]); i++)
					{
						if(sCurrentComponentLowerCaseMinusExtension == EAFilePathLocal::kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}
					}

					if(bReturnValue)
					{
						// Components that consist of just '.' and/or ' ' chars are illegal.
						// The Win32 documentation doesn't specifically state that these combinations
						// are illegal, but in practice they are. Thus, "...", "    ", and ". ." are
						// all invalid.
						char16_t kDotSpace[] = { '.', ' ', 0 };

						if(sCurrentComponent.find_first_not_of(kDotSpace) == PathString::npos)
							bReturnValue = false;
					}
				}

				// Do reserved character tests
				if(bReturnValue)
				{
					// Windows documentation says that chars 0-31 are invalid, but curiously, if you
					// type such characters into the Windows 2000 file browser for a name, it accepts them.
					// Don't be fooled. It actually isn't accepting them, but is instead converting them
					// to alternative Unicode equivalents for the same glyphs before accepting them. 

					for(char16_t c(0); c <= 31; c++)
					{
						if(sCurrentComponent.find(c) != PathString::npos)
							bReturnValue = false;
					}

					if(bReturnValue)
					{
						if(sCurrentComponent.find_first_of(kReservedCharactersWin32) != PathString::npos)
							bReturnValue = false;
					}
				}
			}
		}
	}
	else
	{
		EA_FAIL_MESSAGE("IsPathValid: Not implemented for the given file system.");
		return true; // Play it safe and return true. If we were using this function to implement in a security-conscious way, we might instead choose to return false.
	}

   return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MakePathValid
//
// Strips invalid characters from the input path or file name to make it 
// valid. The result path argument can be the same string as the input.
// Destination pointer must point to enough space to hold a maximum possible length value.
//
EAIO_API bool MakePathValid(const char16_t* /*pPath*/, char16_t* /*pPathResult*/, FileSystem /*fileSystemType*/)
{
	// Todo: Complete this.
	// Contact Paul Pedriana if and when you need this.
	return false;
}



///////////////////////////////////////////////////////////////////////////////
// IsValidFileNameChar
//
// Returns true if the input character can always be used in a path file name.
// You cannot use this function one-by-one on a file name to determine if the file
// name is valid, as there are special combination cases that make this so.
//
EAIO_API bool IsValidFileNameChar(char16_t c, FileSystem fileSystemType)
{
	switch (fileSystemType)
	{
		case kFileSystemUnix:       // General Unix paths
		case kFileSystemISO9660:
		case kFileSystemJoliet:
		case kFileSystemUDF:
		case kFileSystemNone:
			return (c != '/');

		case kFileSystemWindows:
		case kFileSystemFAT:
		case kFileSystemFATX:
		case kFileSystemFAT32:
		case kFileSystemNTFS:
		case kFileSystemUDFX:
		default:
			return ((c != '<') && (c != '>') && (c != ':') && (c != '\\') && 
					(c != '/') && (c != '\"') && (c != '|') && (c != '*') && (c != '?'));
	}

	// return true;  // The above switch handles all possibilities.
}


///////////////////////////////////////////////////////////////////////////////
// IsFileNameValid
//
// Returns true if the input file name is valid for the file system.
//
EAIO_API bool IsFileNameValid(const char16_t* pName, FileSystem fileSystemType)
{
	PathString sName(pName);

	if(sName.length() < (eastl_size_t)kMaxFileNameLength)
	{
		//Check for bad characters. This is an OK technique even for multi-byte locales.
		for(eastl_size_t i(0), iEnd(sName.length()); i < iEnd; i++)
		{
			const char16_t c = sName[i];  //We don't have to worry about multi-byte text because we use UTF8 which is not hostile to what we do here.

			if(!IsValidFileNameChar(c))
				return false;
		}

		// Do reserved word tests
		if((fileSystemType == kFileSystemWindows) || 
		   (fileSystemType == kFileSystemFAT)     ||
		   (fileSystemType == kFileSystemFAT32)   ||
		   (fileSystemType == kFileSystemNTFS))
		{ 
			PathString sCurrentComponentLowerCaseMinusExtension(sName);
			sCurrentComponentLowerCaseMinusExtension.make_lower();

			// Remove anything past a dot because that's insignificant to Windows when testing reserved words.
			const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));
			if(nDotPosition != PathString::npos)
				sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

			// Also trim off any spaces at the end because those are insignificant to Windows when testing reserved words.
			sCurrentComponentLowerCaseMinusExtension.rtrim();

			for(eastl_size_t i(0); i < (sizeof(EAFilePathLocal::kReservedNamesWin32) / sizeof(EAFilePathLocal::kReservedNamesWin32[0])); i++)
			{
				if(sCurrentComponentLowerCaseMinusExtension == EAFilePathLocal::kReservedNamesWin32[i])
					return false;
			}
		}

		// Components that consist of just '.' and/or ' ' chars are illegal.
		// The Win32 documentation doesn't specifically state that these combinations
		// are illegal, but in practice they are. Thus, "...", "   ", and ". ." are
		// all invalid.
		const char16_t pDotSpace[] = { '.', ' ', 0 };

		if(sName.find_first_not_of(pDotSpace) == PathString::npos)
			return false;

		return true;
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// MakeFileNameValid
//
// Fixes an improper file name by removing invalid characters as necessary.
// Returns true if the resulting file name (altered or otherwise) is valid for 
// the file system.
//
EAIO_API bool MakeFileNameValid(const char16_t* pName, char16_t* pNameResult, FileSystem /*fileSystemType*/)
{
	PathString sNameResultTemp;

	// Remove bad characters. This is an OK technique even for multi-byte locales. 
	while(*pName)
	{
		if(IsValidFileNameChar(*pName))
			sNameResultTemp += *pName;
		++pName;
	}

	// Do a length check. Do so intentionally after the character check because 
	// it will potentially preserve more valid characters.
	if(sNameResultTemp.length() > (eastl_size_t)kMaxFileNameLength - 1)
		sNameResultTemp.resize((eastl_size_t)kMaxFileNameLength - 1);

	EA::StdC::Strlcpy(pNameResult, sNameResultTemp.c_str(), kMaxFileNameLength);

	return (sNameResultTemp.length() > 0);
}


///////////////////////////////////////////////////////////////////////////////
// IsValidDirectoryNameChar
//
// Returns true if the input character can always be used in a path directory.
//
EAIO_API bool IsValidDirectoryNameChar(char16_t c, FileSystem fileSystemType)
{
	return IsValidFileNameChar(c, fileSystemType); // This works for all currently supported platforms.
}



///////////////////////////////////////////////////////////////////////////////
// IsDirectoryNameValid
//
// Returns true if the input directory name is valid for the file system.
//
EAIO_API bool IsDirectoryNameValid(const char16_t* pName, FileSystem fileSystemType)
{
	return IsFileNameValid(pName, fileSystemType); // This works for all currently supported platforms.
}



///////////////////////////////////////////////////////////////////////////////
// MakeDirectoryNameValid
//
// Fixes an improper directory name by removing invalid characters as necessary.
// Returns true if the resulting directory name (altered or otherwise) is valid for the file system.
// Destination pointer must point to enough space to hold a maximum possible length value.
//
EAIO_API bool MakeDirectoryNameValid(const char16_t* pName, char16_t* pNameResult, FileSystem fileSystemType)
{
	return MakeFileNameValid(pName, pNameResult, fileSystemType); // This works for all currently supported platforms.
}



///////////////////////////////////////////////////////////////////////////////
// CanonicalizeDirectory
//
// void TestCanonicalizeDirectory() {
//     char16_t buffer[32];
//     
//     CanonicalizeDirectory("/a/b/c/d/", buffer, '/');
//     EA_ASSERT(!Strcmp16(buffer, "/a/b/c/d/");
// 
//     CanonicalizeDirectory("/a/b/c/d", buffer, '/');
//     EA_ASSERT(!Strcmp16(buffer, "/a/b/c/d/");
// 
//     CanonicalizeDirectory("\\a\\b\\c\\d", buffer, '/');
//     EA_ASSERT(!Strcmp16(buffer, "/a/b/c/d/");
// 
//     CanonicalizeDirectory("\\a\\b\\c\\d\\", buffer, '\\');
//     EA_ASSERT(!Strcmp16(buffer, "\a\b\c\d\");
// 
//     CanonicalizeDirectory("/a/b/c/d", buffer, '/');
//     EA_ASSERT(!Strcmp16(buffer, "\a\b\c\d\");
// 
//     CanonicalizeDirectory("/", buffer, '\\');
//     EA_ASSERT(!Strcmp16(buffer, "\\");
//
//     CanonicalizeDirectory("", buffer, '/');
//     EA_ASSERT(!Strcmp16(buffer, "/");
// }
EAIO_API char16_t* CanonicalizeDirectory(const char16_t* pDirectory, char16_t* pDirectoryResult, char16_t pathSeparator)
{
	const char16_t* pCurrentSource = pDirectory;
		  char16_t* pCurrentDest   = pDirectoryResult;

	while(*pCurrentSource)
	{
		if(IsFilePathSeparator(*pCurrentSource) && (*pCurrentSource != EA_FILE_PATH_DRIVE_SEPARATOR_16))
			*pCurrentDest =  pathSeparator;
		else
			*pCurrentDest = *pCurrentSource;
		++pCurrentSource;
		++pCurrentDest;
	}

	// Question: What should we do about the case of an input directory of "c:"? 
	// IsFilePathSeparator currently interprets the ':' as a directory separator,
	// which may well be what we want as-is.
	if((pCurrentSource == pDirectory) || !IsFilePathSeparator(pCurrentSource[-1])) // If the input directory was empty or didn't end in a separator...
		*pCurrentDest++ = pathSeparator;
	*pCurrentDest = 0;

	return pDirectoryResult;
}


///////////////////////////////////////////////////////////////////////////////
// IsSubdirectory
//
// Returns true if the input pPathSubdirectory is a child directory of the
// input pPathDirectory. Both are expected to be full and valid paths.
//
EAIO_API bool IsSubdirectory(const char16_t* pDirectory, const char16_t* pSubdirectory, FileSystem fileSystemType)
{
	bool bReturnValue(true); // True until proven false.

	if(*pDirectory) //If the input directory is empty, then the subdirectory always matches. Also, this check helps prevent out-of-bounds errors below.
	{
		const bool bCaseSensitiveFileSystem(IsFileSystemCaseSensitive(fileSystemType));

		while(*pDirectory)
		{
			if(*pSubdirectory)
			{
				if(IsFilePathSeparator(*pDirectory))
				{
					if(!IsFilePathSeparator(*pSubdirectory))
					{  // We have a mismatch between the directories.
						bReturnValue = false;
						break;
					}
				}
				else
				{
					if(bCaseSensitiveFileSystem)
					{
						if(*pSubdirectory != *pDirectory)
						{  
							bReturnValue = false;
							break; // We have a mismatch between the directories.
						}
					}
					else
					{
						if(EA::StdC::Tolower(*pSubdirectory) != EA::StdC::Tolower(*pDirectory))
						{  
							bReturnValue = false;
							break; // We have a mismatch between the directories.
						}
					}
				}
			}
			else // Else the subdirectory was shorter than the parent directory.
			{
				bReturnValue = false;
				break;
			}

			++pDirectory;
			++pSubdirectory;
		}

		if(bReturnValue)
		{
			// In this case the subdirectory matches the parent directory 
			// for all characters of the parent directory. For the child to 
			// be a subdirectory of the parent, either the parent must end
			// in a path separator or the next character in the child path
			// must be a path separator or nothing. Note that all of the 
			// pointer dereferences below should be safe from out-of-bounds
			// errors due to the logic above.
			bReturnValue = IsFilePathSeparator(pDirectory[-1]) || !*pSubdirectory || IsFilePathSeparator(*pSubdirectory);
		}
	}

	return bReturnValue;
}



} // namespace IO


} // namespace EA



// For unity build friendliness, undef all local #defines.
#undef LOCAL_MIN
#undef LOCAL_MAX
