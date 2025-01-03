///////////////////////////////////////////////////////////////////////////////
// PathString.cpp
// 
// Copyright (c) 2006, Electronic Arts. All Rights Reserved.
// Written and maintained by Talin and Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <coreallocator/icoreallocator_interface.h>
#include EA_ASSERT_HEADER


namespace EA
{
namespace IO
{
namespace Path
{


namespace
{
	template<typename char_type>
	struct PathCharTraits
	{
		static const char_type PathSeparator = (char_type)EA_FILE_PATH_SEPARATOR_8;
		static const char_type AltPathSeparator = (char_type)EA_FILE_PATH_SEPARATOR_ALT_8;
		static const char_type DriveSeparator = (char_type)EA_FILE_PATH_DRIVE_SEPARATOR_8;
	};

	template<typename T>
	struct PathTraits
	{
		typedef T type;
		typedef typename type::base_type base_type;
		typedef typename base_type::value_type char_type;
		static const void *kEndAuto;
	};

	template<> const void *PathTraits<PathString8>::kEndAuto = NULL;
	template<> const void *PathTraits<PathString16>::kEndAuto = NULL;
	template<> const void *PathTraits<PathString32>::kEndAuto = NULL;


	////////////////////////
	// IsDirectorySeparator
	////////////////////////

	// To do: Make it so there are only one of these.

	template<typename T>
	inline bool IsDirectorySeparator(T c)
	{
		return (c == PathCharTraits<T>::PathSeparator) || (c == PathCharTraits<T>::AltPathSeparator);
	}


	////////////////////////
	// HasDrivePrefix
	////////////////////////

	template<typename T>
	bool HasDrivePrefix(typename T::const_iterator first, typename T::const_iterator last)
	{
		typedef typename PathTraits<T>::char_type char_type;
		for (; first < last; ++first)
		{
			if (*first == PathCharTraits<char_type>::DriveSeparator)
				return true;
			if (IsDirectorySeparator(*first))
				return false;
		}
		return false;
	}


	////////////////////////
	// HasUNCPrefix
	////////////////////////

	// To do: Make this a template.

	template<typename T>
	bool HasUNCPrefix(typename T::const_iterator first, typename T::const_iterator last)
	{
		// UNC paths specifically use '\' and not '/'.
		return (((first + 2) <= last) && (first[0] == '\\') && (first[1] == '\\'));
	}
} // anonymous



//////////////////////////////////////////////////////////////////////////
// Compare
//////////////////////////////////////////////////////////////////////////

template<typename T>
static int CompareImpl(typename T::const_iterator a, typename T::const_iterator a_end,
					   typename T::const_iterator b, typename T::const_iterator b_end,
					   CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	typedef typename PathTraits<T>::char_type char_type;

	EA_ASSERT(a && b);

	if(a_end == PathTraits<T>::kEndAuto)
		a_end = StrEnd(a);

	if(b_end == PathTraits<T>::kEndAuto)
		b_end = StrEnd(b);

	for(;;)
	{
		char_type A = *a;
		char_type B = *b;

		if(IsDirectorySeparator(A))
			A = PathCharTraits<char_type>::PathSeparator; // Normalize it, in case the platform has multiple possible directory separator chars.
		else if(caseSensitivity == kCaseInsensitive)
			A = EA::StdC::Tolower(A);     // It isn't Unicode-correct to simply lower-case individual characters. We really need to lower-case strings.

		if(IsDirectorySeparator(B))
			B = PathCharTraits<char_type>::PathSeparator;
		else if(caseSensitivity == kCaseInsensitive)
			B = EA::StdC::Tolower(B);

		if(a >= a_end)      // If reached the end of the a string...
		{
			if(b >= b_end)  // If also reached the end of the b string...
				return 0;   // The strings are equal

			// a ended but b did not.
			if(bDirectoriesFirst && IsDirectorySeparator(B))
				return 1;
			else
				return -1;
		} 
		else if(b >= b_end) // if b ended but a did not.
		{
			if(bDirectoriesFirst && IsDirectorySeparator(A))
				return -1;
			else
				return 1;
		}
		else if(A != B)   // If a and b differ at this position...
		{
			if(IsDirectorySeparator(A)) // If a ended but b did not...
				return bDirectoriesFirst ? -1 : 1;
			if(IsDirectorySeparator(B)) // If b ended but a did not...
				return bDirectoriesFirst ? 1 : -1;

			return (A < B) ? -1 : 1;
		}

		++a;
		++b;
	}
}

EAIO_API int Compare(PathString8::const_iterator a, PathString8::const_iterator a_end,
					 PathString8::const_iterator b, PathString8::const_iterator b_end,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl<PathString8>(a, a_end, b, b_end, caseSensitivity, bDirectoriesFirst);
}


EAIO_API int Compare(PathString16::const_iterator a, PathString16::const_iterator a_end,
					 PathString16::const_iterator b, PathString16::const_iterator b_end,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl<PathString16>(a, a_end, b, b_end, caseSensitivity, bDirectoriesFirst);
}


EAIO_API int Compare(PathString32::const_iterator a, PathString32::const_iterator a_end,
					 PathString32::const_iterator b, PathString32::const_iterator b_end,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl<PathString32>(a, a_end, b, b_end, caseSensitivity, bDirectoriesFirst);
}




//////////////////////////////////////////////////////////////////////////
// FindComponentFwd
//////////////////////////////////////////////////////////////////////////

template<typename T>
static typename T::iterator FindComponentFwdImpl(
	typename T::iterator first,
	typename T::iterator last)
{
	typedef typename PathTraits<T>::char_type char_type;
	EA_ASSERT(first);
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	// Skip over initial UNC separators
	if(HasUNCPrefix<T>(first, last))
		first += 2;

	// Then, skip over any non-separator chars (if drive separator encountered,
	// then break)
	while(first < last && !IsDirectorySeparator(*first))
	{
		if(*first == PathCharTraits<char_type>::DriveSeparator)
		{ 
			first++; 
			break; 
		}
		++first;
	}

	// Skip over trailing separator
	if((first < last) && IsDirectorySeparator(*first)) 
		++first;

	return first;
}


EAIO_API PathString8::iterator FindComponentFwd(
	PathString8::iterator first,
	PathString8::iterator last)
{
	return FindComponentFwdImpl<PathString8>(first, last);
}


EAIO_API PathString16::iterator FindComponentFwd(
	PathString16::iterator first,
	PathString16::iterator last)
{
	return FindComponentFwdImpl<PathString16>(first, last);
}


EAIO_API PathString32::iterator FindComponentFwd(
	PathString32::iterator first,
	PathString32::iterator last)
{
	return FindComponentFwdImpl<PathString32>(first, last);
}




//////////////////////////////////////////////////////////////////////////
// FindComponentFwd
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator FindComponentFwd(
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	return FindComponentFwd(const_cast<PathString8::iterator>(first), 
							const_cast<PathString8::iterator>(last));
}

EAIO_API PathString16::const_iterator FindComponentFwd(
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	return FindComponentFwd(const_cast<PathString16::iterator>(first), 
							const_cast<PathString16::iterator>(last));
}

EAIO_API PathString32::const_iterator FindComponentFwd(
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	return FindComponentFwd(const_cast<PathString32::iterator>(first), 
							const_cast<PathString32::iterator>(last));
}




//////////////////////////////////////////////////////////////////////////
// FindComponentRvs
//////////////////////////////////////////////////////////////////////////

template<typename T>
static typename T::iterator FindComponentRvsImpl(
	typename T::iterator first,
	typename T::iterator last)
{
	typedef typename PathTraits<T>::char_type char_type;
	EA_ASSERT(first);

	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);
	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	// Skip over any file path separators
	if((last > first) && IsDirectorySeparator(last[-1])) 
		--last;

	// Skip over drive separator
	if((last > first) && (last[-1] == PathCharTraits<char_type>::DriveSeparator)) 
		--last;

	// Skip over any non-file-path separators
	while((last > first) && !IsDirectorySeparator(last[-1]) && (last[-1] != PathCharTraits<char_type>::DriveSeparator)) 
		--last;

	// Skip over UNC prefix.
	if((last == first + 2) && IsDirectorySeparator(first[0]) && IsDirectorySeparator(first[1]))
		last = first;

	return last;
}


EAIO_API PathString8::iterator FindComponentRvs(
	PathString8::iterator first,
	PathString8::iterator last)
{
	return FindComponentRvsImpl<PathString8>(first, last);
}


EAIO_API PathString16::iterator FindComponentRvs(
	PathString16::iterator first,
	PathString16::iterator last)
{
	return FindComponentRvsImpl<PathString16>(first, last);
}


EAIO_API PathString32::iterator FindComponentRvs(
	PathString32::iterator first,
	PathString32::iterator last)
{
	return FindComponentRvsImpl<PathString32>(first, last);
}



//////////////////////////////////////////////////////////////////////////
// FindComponentRvs
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator FindComponentRvs(
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	return FindComponentRvs(const_cast<PathString8::iterator>(first), 
							const_cast<PathString8::iterator>(last));
}

EAIO_API PathString16::const_iterator FindComponentRvs(
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	return FindComponentRvs(const_cast<PathString16::iterator>(first), 
							const_cast<PathString16::iterator>(last));
}

EAIO_API PathString32::const_iterator FindComponentRvs(
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	return FindComponentRvs(const_cast<PathString32::iterator>(first), 
							const_cast<PathString32::iterator>(last));
}




//////////////////////////////////////////////////////////////////////////
// GetPathComponentStart
//////////////////////////////////////////////////////////////////////////

template<typename T>
static typename T::iterator GetPathComponentStartImpl(
	typename T::iterator first,
	typename T::iterator last,
	int32_t nIndex)
{
	EA_ASSERT(first);
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	if(nIndex >= 0)
	{
		// Positive index - search forward
		while(first < last && nIndex > 0)
		{
			first = FindComponentFwd(first, last);
			nIndex--;
		}
		return first;
	} 
	else
	{
		while(last > first && nIndex < 0)
		{
			last = FindComponentRvs(first, last);
			nIndex++;
		}
		return last;
	}
}


EAIO_API PathString8::iterator GetPathComponentStart(
	PathString8::iterator first,
	PathString8::iterator last,
	int32_t nIndex)
{
	return GetPathComponentStartImpl<PathString8>(first, last, nIndex);
}


EAIO_API PathString16::iterator GetPathComponentStart(
	PathString16::iterator first,
	PathString16::iterator last,
	int32_t nIndex)
{
	return GetPathComponentStartImpl<PathString16>(first, last, nIndex);
}


EAIO_API PathString32::iterator GetPathComponentStart(
	PathString32::iterator first,
	PathString32::iterator last,
	int32_t nIndex)
{
	return GetPathComponentStartImpl<PathString32>(first, last, nIndex);
}


//////////////////////////////////////////////////////////////////////////
// GetPathComponentEnd
//////////////////////////////////////////////////////////////////////////

template<typename T>
static typename T::iterator GetPathComponentEndImpl(
	typename T::iterator first,
	typename T::iterator last,
	int32_t nIndex)
{
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	if(nIndex >= 0)
	{
		// Positive index - search forward
		nIndex++;
		typename T::iterator result = first;
		while((result < last) && (nIndex > 0))
		{
			result = FindComponentFwd(result, last);
			nIndex--;
		}
		// back up over the trailing separator
		if((nIndex == 0) && (result > first) && IsDirectorySeparator(result[-1])) 
			--result;
		return result;
	}
	else
	{
		nIndex++;
		typename T::iterator result = last;
		while(result > first && nIndex < 0)
		{
			result = FindComponentRvs(first, result);
			nIndex++;
		}
		if((result > first) && IsDirectorySeparator(result[-1])) 
			--result;
		return result;
	}
}


EAIO_API PathString8::iterator GetPathComponentEnd(
	PathString8::iterator first,
	PathString8::iterator last,
	int32_t nIndex)
{
	return GetPathComponentEndImpl<PathString8>(first, last, nIndex);
}


EAIO_API PathString16::iterator GetPathComponentEnd(
	PathString16::iterator first,
	PathString16::iterator last,
	int32_t nIndex)
{
	return GetPathComponentEndImpl<PathString16>(first, last, nIndex);
}


EAIO_API PathString32::iterator GetPathComponentEnd(
	PathString32::iterator first,
	PathString32::iterator last,
	int32_t nIndex)
{
	return GetPathComponentEndImpl<PathString32>(first, last, nIndex);
}




//////////////////////////////////////////////////////////////////////////
// GetFileName
//////////////////////////////////////////////////////////////////////////

template<typename T>
static typename T::iterator GetFileNameImpl(
	typename T::const_iterator first,
	typename T::const_iterator last)
{
	typedef typename PathTraits<T>::char_type char_type;

	EA_ASSERT(first);
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	// If it's a trailing separator, then there's no file name
	if((first < last) && IsDirectorySeparator(last[-1])) 
		return const_cast<typename T::iterator>(last);

	typename T::iterator fname = const_cast<typename T::iterator>(last);

	// Skip over any non-file-path separators
	while((fname > first)
		&& !IsDirectorySeparator(fname[-1])
		&& (fname[-1] != PathCharTraits<char_type>::DriveSeparator))
	{
		--fname;
	}

	// A UNC machine name is not, I am afraid, a file name
	if((fname == first + 2) && HasUNCPrefix<T>(first, last))
		return const_cast<typename T::iterator>(last);

	// Neither is a drive name
	//if(fname == first && HasDrivePrefix(first, last))
	//    return last;

	return fname;
}


EAIO_API PathString8::iterator GetFileName(
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	return GetFileNameImpl<PathString8>(first, last);
}


EAIO_API PathString16::iterator GetFileName(
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	return GetFileNameImpl<PathString16>(first, last);
}


EAIO_API PathString32::iterator GetFileName(
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	return GetFileNameImpl<PathString32>(first, last);
}



//////////////////////////////////////////////////////////////////////////
// GetFileName
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator GetFileName(const PathString8& path)
{
	return GetFileName(const_cast<PathString8&>(path).begin(),
					   const_cast<PathString8&>(path).end());
}

EAIO_API PathString16::const_iterator GetFileName(const PathString16& path)
{
	return GetFileName(const_cast<PathString16&>(path).begin(),
					   const_cast<PathString16&>(path).end());
}

EAIO_API PathString32::const_iterator GetFileName(const PathString32& path)
{
	return GetFileName(const_cast<PathString32&>(path).begin(),
					   const_cast<PathString32&>(path).end());
}




//////////////////////////////////////////////////////////////////////////
// GetFileExtension
//////////////////////////////////////////////////////////////////////////


template<typename T>
static typename T::iterator GetFileExtensionImpl(
	typename T::const_iterator first,
	typename T::const_iterator last)
{
	typedef typename PathTraits<T>::char_type char_type;
	EA_ASSERT(first);
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	// If it's a trailing separator, then there's no file name
	if(first < last && IsDirectorySeparator(last[-1]))
		return const_cast<typename T::iterator>(last);

	// If it has a UNC prefix, it needs to be treated specially
	if(HasUNCPrefix<T>(first, last))
		first = FindComponentFwd(first, last);

	for(typename T::iterator it = const_cast<typename T::iterator>(last)-1; it >= first; --it)
	{
		if(IsDirectorySeparator(*it) || *it == PathCharTraits<char_type>::DriveSeparator)
			break;
		if(*it == '.')
			return it;
	}

	return const_cast<typename T::iterator>(last);
}


EAIO_API PathString8::iterator GetFileExtension(
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	return GetFileExtensionImpl<PathString8>(first, last);
}


EAIO_API PathString16::iterator GetFileExtension(
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	return GetFileExtensionImpl<PathString16>(first, last);
}

EAIO_API PathString32::iterator GetFileExtension(
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	return GetFileExtensionImpl<PathString32>(first, last);
}



//////////////////////////////////////////////////////////////////////////
// GetFileExtension
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator GetFileExtension(const PathString8& path)
{
	return GetFileExtension(const_cast<PathString8&>(path).begin(),
							const_cast<PathString8&>(path).end());
}

EAIO_API PathString16::const_iterator GetFileExtension(const PathString16& path)
{
	return GetFileExtension(const_cast<PathString16&>(path).begin(),
							const_cast<PathString16&>(path).end());
}

EAIO_API PathString32::const_iterator GetFileExtension(const PathString32& path)
{
	return GetFileExtension(const_cast<PathString32&>(path).begin(),
							const_cast<PathString32&>(path).end());
}




//////////////////////////////////////////////////////////////////////////
// GetLocalRoot
//////////////////////////////////////////////////////////////////////////

template<typename T>
typename T::iterator GetLocalRootImpl(typename T::iterator first, typename T::iterator last)
{
	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	if(HasDrivePrefix<T>(first, last))
		return first + 2;

	if(HasUNCPrefix<T>(first, last))
		return GetPathComponentStart(first, last, 1);

	return first;
}

EAIO_API PathString8::iterator GetLocalRoot(
	PathString8::iterator first,
	PathString8::iterator last)
{
	return GetLocalRootImpl<PathString8>(first, last);
}

EAIO_API PathString16::iterator GetLocalRoot(
	PathString16::iterator first,
	PathString16::iterator last)
{
	return GetLocalRootImpl<PathString16>(first, last);
}

EAIO_API PathString32::iterator GetLocalRoot(
	PathString32::iterator first,
	PathString32::iterator last)
{
	return GetLocalRootImpl<PathString32>(first, last);
}


//////////////////////////////////////////////////////////////////////////
// GetLocalRoot
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator GetLocalRoot(const PathString8& path)
{
	return GetLocalRoot(const_cast<PathString8&>(path).begin(),
						const_cast<PathString8&>(path).end());
}

EAIO_API PathString16::const_iterator GetLocalRoot(const PathString16& path)
{
	return GetLocalRoot(const_cast<PathString16&>(path).begin(),
						const_cast<PathString16&>(path).end());
}

EAIO_API PathString32::const_iterator GetLocalRoot(const PathString32& path)
{
	return GetLocalRoot(const_cast<PathString32&>(path).begin(),
						const_cast<PathString32&>(path).end());
}



//////////////////////////////////////////////////////////////////////////
// Append
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Append(
	PathString8& dst,
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	EA_ASSERT(first);
	if(last == kEndAuto8)
		last = StrEnd(first);

	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	if(first == last)
		return dst;

	if(IsRelative(first, last))
	{
		if(!dst.empty()) // It makes little sense to append a '/' char to an empty dst, as an empty dst means there is no dst and the appended directory is all there is.
			EnsureTrailingSeparator(dst);
	}
	else
		dst.clear();

	dst.append(first, last);
	return dst;
}


EAIO_API PathString16& Append(
	PathString16& dst,
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	EA_ASSERT(first);
	if(last == kEndAuto16)
		last = StrEnd(first);

	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	if(first == last)
		return dst;

	if(IsRelative(first, last))
	{
		if(!dst.empty()) // It makes little sense to append a '/' char to an empty dst, as an empty dst means there is no dst and the appended directory is all there is.
			EnsureTrailingSeparator(dst);
	}
	else
		dst.clear();

	dst.append(first, last);
	return dst;
}


EAIO_API PathString32& Append(
	PathString32& dst,
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	EA_ASSERT(first);
	if(last == kEndAuto32)
		last = StrEnd(first);

	EA_ASSERT(last);
	EA_ASSERT(first <= last);

	if(first == last)
		return dst;

	if(IsRelative(first, last))
	{
		if(!dst.empty()) // It makes little sense to append a '/' char to an empty dst, as an empty dst means there is no dst and the appended directory is all there is.
			EnsureTrailingSeparator(dst);
	}
	else
		dst.clear();

	dst.append(first, last);
	return dst;
}



//////////////////////////////////////////////////////////////////////////
// Append
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Append(PathString8& dst, const PathString8& suffix)
{
	return Append(dst, suffix.begin(), suffix.end());
}

EAIO_API PathString16& Append(PathString16& dst, const PathString16& suffix)
{
	return Append(dst, suffix.begin(), suffix.end());
}

EAIO_API PathString32& Append(PathString32& dst, const PathString32& suffix)
{
	return Append(dst, suffix.begin(), suffix.end());
}




//////////////////////////////////////////////////////////////////////////
// Join
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Join(
	PathString8& dst,
	PathString8::const_iterator first,
	PathString8::const_iterator last)
{
	Append(dst, first, last);
	return Normalize(dst);
}

EAIO_API PathString16& Join(
	PathString16& dst,
	PathString16::const_iterator first,
	PathString16::const_iterator last)
{
	Append(dst, first, last);
	return Normalize(dst);
}

EAIO_API PathString32& Join(
	PathString32& dst,
	PathString32::const_iterator first,
	PathString32::const_iterator last)
{
	Append(dst, first, last);
	return Normalize(dst);
}




//////////////////////////////////////////////////////////////////////////
// Join
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Join(PathString8& dst, const PathString8& suffix)
{
	return Join(dst, suffix.begin(), suffix.end());
}

EAIO_API PathString16& Join(PathString16& dst, const PathString16& suffix)
{
	return Join(dst, suffix.begin(), suffix.end());
}

EAIO_API PathString32& Join(PathString32& dst, const PathString32& suffix)
{
	return Join(dst, suffix.begin(), suffix.end());
}



//////////////////////////////////////////////////////////////////////////
// Split
//////////////////////////////////////////////////////////////////////////

EAIO_API void Split(const PathString8& path, PathString8* pDrive, PathString8* pDirectory, 
					PathString8* pFileName, PathString8* pFileExtension)
{
	PathString8::const_iterator psDirectory     = GetLocalRoot(path);
	PathString8::const_iterator psFileName      = GetFileName(path);
	PathString8::const_iterator psFileExtension = GetFileExtension(path);

	if(pDrive)
		pDrive->assign(path.c_str(), psDirectory);
	if(pDirectory)
		pDirectory->assign(psDirectory, psFileName);
	if(pFileName)
		pFileName->assign(psFileName, psFileExtension);
	if(pFileExtension)
		pFileExtension->assign(psFileExtension, path.c_str() + path.length());
}

EAIO_API void Split(const PathString16& path, PathString16* pDrive, PathString16* pDirectory, 
					PathString16* pFileName, PathString16* pFileExtension)
{
	PathString16::const_iterator psDirectory     = GetLocalRoot(path);
	PathString16::const_iterator psFileName      = GetFileName(path);
	PathString16::const_iterator psFileExtension = GetFileExtension(path);

	if(pDrive)
		pDrive->assign(path.c_str(), psDirectory);
	if(pDirectory)
		pDirectory->assign(psDirectory, psFileName);
	if(pFileName)
		pFileName->assign(psFileName, psFileExtension);
	if(pFileExtension)
		pFileExtension->assign(psFileExtension, path.c_str() + path.length());
}

EAIO_API void Split(const PathString32& path, PathString32* pDrive, PathString32* pDirectory, 
					PathString32* pFileName, PathString32* pFileExtension)
{
	PathString32::const_iterator psDirectory     = GetLocalRoot(path);
	PathString32::const_iterator psFileName      = GetFileName(path);
	PathString32::const_iterator psFileExtension = GetFileExtension(path);

	if(pDrive)
		pDrive->assign(path.c_str(), psDirectory);
	if(pDirectory)
		pDirectory->assign(psDirectory, psFileName);
	if(pFileName)
		pFileName->assign(psFileName, psFileExtension);
	if(pFileExtension)
		pFileExtension->assign(psFileExtension, path.c_str() + path.length());
}




//////////////////////////////////////////////////////////////////////////
// Split
//////////////////////////////////////////////////////////////////////////

EAIO_API void Split(PathString8::const_iterator  path, PathString8::const_iterator&  pDrive, PathString8::const_iterator&  pDirectory, 
					PathString8::const_iterator&  pFileName, PathString8::const_iterator&  pFileExtension)
{
	pDrive         = path;
	pDirectory     = GetLocalRoot(path);
	pFileName      = GetFileName(path);
	pFileExtension = GetFileExtension(path);
}

EAIO_API void Split(PathString16::const_iterator path, PathString16::const_iterator& pDrive, PathString16::const_iterator& pDirectory, 
					PathString16::const_iterator& pFileName, PathString16::const_iterator& pFileExtension)
{
	pDrive         = path;
	pDirectory     = GetLocalRoot(path);
	pFileName      = GetFileName(path);
	pFileExtension = GetFileExtension(path);
}

EAIO_API void Split(PathString32::const_iterator path, PathString32::const_iterator& pDrive, PathString32::const_iterator& pDirectory, 
					PathString32::const_iterator& pFileName, PathString32::const_iterator& pFileExtension)
{
	pDrive         = path;
	pDirectory     = GetLocalRoot(path);
	pFileName      = GetFileName(path);
	pFileExtension = GetFileExtension(path);
}




//////////////////////////////////////////////////////////////////////////
// TruncateComponent
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8::const_iterator TruncateComponent(PathString8& path, int nIndex){
	PathString8::iterator it = GetPathComponentStart(path.begin(), path.end(), nIndex);
	return path.erase(it, path.end());
}

EAIO_API PathString16::const_iterator TruncateComponent(PathString16& path, int nIndex){
	PathString16::iterator it = GetPathComponentStart(path.begin(), path.end(), nIndex);
	return path.erase(it, path.end());
}

EAIO_API PathString32::const_iterator TruncateComponent(PathString32& path, int nIndex){
	PathString32::iterator it = GetPathComponentStart(path.begin(), path.end(), nIndex);
	return path.erase(it, path.end());
}




//////////////////////////////////////////////////////////////////////////
// Canonicalize
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Canonicalize(PathString8& path, char separator){
	for(PathString8::iterator it = path.begin(); it != path.end(); ++it){
		if((*it == '/') || (*it =='\\')) // This may do the wrong thing with Unix-like paths.
			*it = separator;
	}
	return path;
}

EAIO_API PathString16& Canonicalize(PathString16& path, char16_t separator){
	for(PathString16::iterator it = path.begin(); it != path.end(); ++it){
		if((*it == '/') || (*it =='\\')) // This may do the wrong thing with Unix-like paths.
			*it = separator;
	}
	return path;
}

EAIO_API PathString32& Canonicalize(PathString32& path, char32_t separator){
	for(PathString32::iterator it = path.begin(); it != path.end(); ++it){
		if((*it == '/') || (*it =='\\')) // This may do the wrong thing with Unix-like paths.
			*it = separator;
	}
	return path;
}



//////////////////////////////////////////////////////////////////////////
// PathStringNormalize
//////////////////////////////////////////////////////////////////////////


template<typename T>
static T& PathStringNormalizeImpl(T& path, bool bCanonicalize)
{
	typedef typename PathTraits<T>::char_type char_type;

	typename T::iterator first = path.begin();
	typename T::iterator last  = path.end();
	typename T::iterator out   = first;

	bool bHasNonLocalPart = HasUNCPrefix<T>(first, last) || HasDrivePrefix<T>(first, last);

	// Copy initial UNC path
	while((first < last) && (*first == '\\'))
	{
		if(bCanonicalize)
			*out++ = EA_FILE_PATH_SEPARATOR_8;
		else
			*out++ = *first;
		++first;
	}

	// Because simplification never increases the size of the path, we can simply
	// write the buffer in-place and adjust the size at the end.
	while(first < last)
	{
		if((first + 1 < last) && (first[0] == '.') && IsDirectorySeparator(first[1]))
		{
			first += 2;

			// Skip over doubled separator
			while(first < last && IsDirectorySeparator(*first)) 
				++first;
			continue;
		}
		else if(first + 2 < last
			&& first[0] == '.'
			&& first[1] == '.'
			&& IsDirectorySeparator(first[2])
			&& out > path.begin())
		{
			// strip and that directory isn't a ".."; otherwise, copy the ".." literally 
			// (i.e. leave it in the string.)
			typename T::iterator prev = FindComponentRvs(path.begin(), out);

			if((prev > path.begin() || !bHasNonLocalPart) && 
				!((prev + 2 < last) && (prev[0] == '.') && (prev[1] == '.') && IsDirectorySeparator(prev[2])))
			{
				out = prev;
				first += 3;

				// Skip over doubled separator
				while(first < last && IsDirectorySeparator(*first)) 
					++first;
				continue;
			}
		}

		// Copy next component
		while(first < last)
		{
			char_type c = *first++;

			if(IsDirectorySeparator(c))
			{
				// Use the canonical separator, not the one in the string
				if(bCanonicalize)
					*out++ = PathCharTraits<char_type>::PathSeparator;
				else
					*out++ = c;

				// Skip over doubled separator
				while(first < last && IsDirectorySeparator(*first))
					++first;
				break;
			} 
			else if(c == PathCharTraits<char_type>::DriveSeparator)
			{
				// Drive separator. Break here if the next char is not a dir sep.
				*out++ = c;
				if(first >= last || !IsDirectorySeparator(*out))
					break;
			} 
			else{
				*out++ = c;
			}
		}
	}

	path.erase(out, last);
	return path;
}


static PathString8& PathStringNormalize(PathString8& path, bool bCanonicalize)
{
	return PathStringNormalizeImpl<PathString8>(path, bCanonicalize);
}


static PathString16& PathStringNormalize(PathString16& path, bool bCanonicalize)
{
	return PathStringNormalizeImpl<PathString16>(path, bCanonicalize);
}


static PathString32& PathStringNormalize(PathString32& path, bool bCanonicalize)
{
	return PathStringNormalizeImpl<PathString32>(path, bCanonicalize);
}


//////////////////////////////////////////////////////////////////////////
// Simplify
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Simplify(PathString8& path)
{
	return PathStringNormalize(path, false);
}

EAIO_API PathString16& Simplify(PathString16& path)
{
	return PathStringNormalize(path, false);
}

EAIO_API PathString32& Simplify(PathString32& path)
{
	return PathStringNormalize(path, false);
}




//////////////////////////////////////////////////////////////////////////
// Normalize
//////////////////////////////////////////////////////////////////////////

EAIO_API PathString8& Normalize(PathString8& path)
{
	return PathStringNormalize(path, true);
}

EAIO_API PathString16& Normalize(PathString16& path)
{
	return PathStringNormalize(path, true);
}

EAIO_API PathString32& Normalize(PathString32& path)
{
	return PathStringNormalize(path, true);
}




//////////////////////////////////////////////////////////////////////////
// IsRelative
//////////////////////////////////////////////////////////////////////////

template<typename T>
static bool IsRelativeImpl(typename T::const_iterator first, typename T::const_iterator last)
{
	EA_ASSERT(first);

	if(last == PathTraits<T>::kEndAuto) 
		last = StrEnd(first);

	if(first >= last) // If it's empty
		return true;

	if(HasDrivePrefix<T>(first, last)) // If begins with "C:\"
		return false;

	if((last > first) && IsDirectorySeparator(*first)) // If begins with \ or / (then it is absolute, either as a Unix rooted path or as a Microsoft UNC path)
		return false;

	return true;
}

EAIO_API bool IsRelative(PathString8::const_iterator first, PathString8::const_iterator last)
{
	return IsRelativeImpl<PathString8>(first, last);
}

EAIO_API bool IsRelative(PathString16::const_iterator first, PathString16::const_iterator last)
{
	return IsRelativeImpl<PathString16>(first, last);
}

EAIO_API bool IsRelative(PathString32::const_iterator first, PathString32::const_iterator last)
{
	return IsRelativeImpl<PathString32>(first, last);
}



//////////////////////////////////////////////////////////////////////////
// IsRelative
//////////////////////////////////////////////////////////////////////////

EAIO_API bool IsRelative(const PathString8& path)
{
	return IsRelative(path.begin(), path.end());
}

EAIO_API bool IsRelative(const PathString16& path)
{
	return IsRelative(path.begin(), path.end());
}

EAIO_API bool IsRelative(const PathString32& path)
{
	return IsRelative(path.begin(), path.end());
}




//////////////////////////////////////////////////////////////////////////
// Compare
//////////////////////////////////////////////////////////////////////////

template<typename T>
static int CompareImpl(const T& a, const T& b, CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	// first and last refer to the start and end of an individual name in the path.
	typename T::const_iterator  a_first = a.begin(), a_last;
	typename T::const_iterator  b_first = b.begin(), b_last;

	// Compare each component one at a time, until we find a name that doesn't match.
	while((a_first < a.end()) && (b_first < b.end()))
	{
		a_last = FindComponentFwd(a_first, a.end());
		b_last = FindComponentFwd(b_first, b.end());

		int result = Compare(a_first, a_last, b_first, b_last, caseSensitivity, bDirectoriesFirst);
		if(result)
			return result;

		a_first = a_last;
		b_first = b_last;
	}

	return 0;
}


EAIO_API int Compare(const PathString8& a, const PathString8& b,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl(a, b, caseSensitivity, bDirectoriesFirst);
}


EAIO_API int Compare(const PathString16& a, const PathString16& b,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl(a, b, caseSensitivity, bDirectoriesFirst);
}


EAIO_API int Compare(const PathString32& a, const PathString32& b,
					 CaseSensitivity caseSensitivity, bool bDirectoriesFirst)
{
	return CompareImpl(a, b, caseSensitivity, bDirectoriesFirst);
}



//////////////////////////////////////////////////////////////////////////
// ComputeRelative
//////////////////////////////////////////////////////////////////////////

template<typename T>
static T& ComputeRelativeImpl(T& result, const T& source, const T& target)
{
	// first and last refer to the start and end of an individual name
	// in the path.
	typename T::const_iterator  source_first = source.begin(), source_last;
	typename T::const_iterator  target_first = target.begin(), target_last;

	// Compare each component one at a time, until we find a name that
	// doesn't match.
	while(source_first < source.end() && target_first < target.end())
	{
		source_last = FindComponentFwd(source_first, source.end());
		target_last = FindComponentFwd(target_first, target.end());

		int cResult = Compare(source_first, source_last,
							  target_first, target_last);
		if(cResult)
			break;

		source_first = source_last;
		target_first = target_last;
	}

	// Clear the result buffer
	result.clear();

	// If both were equal, then we're done
	if(source_first == source.end() && target_first == target.end())
		return result;

	// If dest is not relative, then just use that
	if(!IsRelative(target_first))
	{
		result = target_first;
		Canonicalize(result);
		return result;
	}

	// Otherwise, add a "../" for each dir remaining in source
	typedef typename PathTraits<T>::char_type char_type;
	const char_type parentSequence[4] = { char_type('.'), char_type('.'), PathCharTraits<char_type>::PathSeparator, 0 };

	while(source_first < source.end())
	{
		source_last = FindComponentFwd(source_first, source.end());
		result += parentSequence;
		source_first = source_last;
	}

	return Join(result, target_first, target.end());
}


EAIO_API PathString8& ComputeRelative(PathString8& result, const PathString8& source, const PathString8& target)
{
	return ComputeRelativeImpl(result, source, target);
}


EAIO_API PathString16& ComputeRelative(
	PathString16& result,
	const PathString16& source,
	const PathString16& target)
{
	return ComputeRelativeImpl(result, source, target);
}


EAIO_API PathString32& ComputeRelative(
	PathString32& result,
	const PathString32& source,
	const PathString32& target)
{
	return ComputeRelativeImpl(result, source, target);
}



//////////////////////////////////////////////////////////////////////////
// IsSubdirectory
//////////////////////////////////////////////////////////////////////////

template<typename T>
static bool IsSubdirectoryImpl(const T& dir, const T& sub)
{
	// first and last refer to the start and end of an individual name
	// in the path.
	typename T::const_iterator  dir_first = dir.begin(), dir_last;
	typename T::const_iterator  sub_first = sub.begin(), sub_last;

	// Compare each component one at a time, until we find a name that
	// doesn't match.
	while(dir_first < dir.end() && sub_first < sub.end())
	{
		dir_last = FindComponentFwd(dir_first, dir.end());
		sub_last = FindComponentFwd(sub_first, sub.end());

		int result = Compare(
			dir_first, dir_last,
			sub_first, sub_last);

		if(result)
			break;

		dir_first = dir_last;
		sub_first = sub_last;
	}

	return dir_first >= dir.end();
}

EAIO_API bool IsSubdirectory(
	const PathString8& dir,
	const PathString8& sub)
{
	return IsSubdirectoryImpl(dir, sub);
}

EAIO_API bool IsSubdirectory(
	const PathString16& dir,
	const PathString16& sub)
{
	return IsSubdirectoryImpl(dir, sub);
}


EAIO_API bool IsSubdirectory(
	const PathString32& dir,
	const PathString32& sub)
{
	return IsSubdirectoryImpl(dir, sub);
}




//////////////////////////////////////////////////////////////////////////
// GetHasTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static bool GetHasTrailingSeparatorImpl(const T& path)
{
	return (!path.empty() && IsDirectorySeparator(path.back()));
}

EAIO_API bool GetHasTrailingSeparator(const PathString8& path)
{
	return GetHasTrailingSeparatorImpl(path);
}

EAIO_API bool GetHasTrailingSeparator(const PathString16& path)
{
	return GetHasTrailingSeparatorImpl(path);
}

EAIO_API bool GetHasTrailingSeparator(const PathString32& path)
{
	return GetHasTrailingSeparatorImpl(path);
}



//////////////////////////////////////////////////////////////////////////
// GetHasTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static bool GetHasTrailingSeparatorImpl(T path, size_t nLength)
{
	if (nLength == kLengthNull)
		nLength = EA::StdC::Strlen(path);

	return ((nLength  != 0) && IsDirectorySeparator(*(path + nLength - 1)));
}


EAIO_API bool GetHasTrailingSeparator(PathString8::const_iterator path, size_t nLength)
{
	return GetHasTrailingSeparatorImpl(path, nLength);
}


EAIO_API bool GetHasTrailingSeparator(PathString16::const_iterator path, size_t nLength)
{
	return GetHasTrailingSeparatorImpl(path, nLength);
}


EAIO_API bool GetHasTrailingSeparator(PathString32::const_iterator path, size_t nLength)
{
	return GetHasTrailingSeparatorImpl(path, nLength);
}



//////////////////////////////////////////////////////////////////////////
// EnsureTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static T& EnsureTrailingSeparatorImpl(T& path)
{
	typedef typename PathTraits<T>::char_type char_type;

	// To consider: should we add to an empty path?
	if(/*!path.empty() &&*/ !GetHasTrailingSeparator(path))
		path.push_back(PathCharTraits<char_type>::PathSeparator);
	return path;
}

EAIO_API PathString8& EnsureTrailingSeparator(PathString8& path)
{
	return EnsureTrailingSeparatorImpl(path);
}


EAIO_API PathString16& EnsureTrailingSeparator(PathString16& path)
{
	return EnsureTrailingSeparatorImpl(path);
}


EAIO_API PathString32& EnsureTrailingSeparator(PathString32& path)
{
	return EnsureTrailingSeparatorImpl(path);
}




//////////////////////////////////////////////////////////////////////////
// EnsureTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static bool EnsureTrailingSeparatorImpl(T* pPath, size_t dirCapacity)
{
	// To consider: should we add to an empty path?
	const size_t n = EA::StdC::Strlen(pPath);

	if (!EA::IO::Path::GetHasTrailingSeparator(pPath, (uint32_t)n))
	{
		if ((n + 2) <= dirCapacity)
		{
			pPath[n] = PathCharTraits<T>::PathSeparator;
			pPath[n + 1] = 0;

			return true;
		}
	}

	return false;
}

EAIO_API bool EnsureTrailingSeparator(char* pPath, size_t dirCapacity)
{
	return EnsureTrailingSeparatorImpl(pPath, dirCapacity);
}


EAIO_API bool EnsureTrailingSeparator(char16_t* pPath, size_t dirCapacity)
{
	return EnsureTrailingSeparatorImpl(pPath, dirCapacity);
}


EAIO_API bool EnsureTrailingSeparator(char32_t* pPath, size_t dirCapacity)
{
	return EnsureTrailingSeparatorImpl(pPath, dirCapacity);
}



//////////////////////////////////////////////////////////////////////////
// StripTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static T& StripTrailingSeparatorImpl(T& path)
{
	if (GetHasTrailingSeparator(path))
		path.pop_back();
	return path;
}

EAIO_API PathString8& StripTrailingSeparator(PathString8& path)
{
	return StripTrailingSeparatorImpl(path);
}


EAIO_API PathString16& StripTrailingSeparator(PathString16& path)
{
	return StripTrailingSeparatorImpl(path);

}


EAIO_API PathString32& StripTrailingSeparator(PathString32& path)
{
	return StripTrailingSeparatorImpl(path);

}



//////////////////////////////////////////////////////////////////////////
// StripTrailingSeparator
//////////////////////////////////////////////////////////////////////////

template<typename T>
static void StripTrailingSeparatorImpl(T* pPath, size_t nLength)
{
	if (nLength == kLengthNull)
		nLength = EA::StdC::Strlen(pPath);
	
	if(nLength > 0)
	{
		if (pPath[nLength-1] == PathCharTraits<T>::PathSeparator || pPath[nLength-1] == PathCharTraits<T>::AltPathSeparator)
			pPath[nLength-1] = 0;
	}
}

EAIO_API void StripTrailingSeparator(char* pPath, size_t nLength)
{
	StripTrailingSeparatorImpl(pPath, nLength);
}

EAIO_API void StripTrailingSeparator(char16_t* pPath, size_t nLength)
{
	StripTrailingSeparatorImpl(pPath, nLength);
}

EAIO_API void StripTrailingSeparator(char32_t* pPath, size_t nLength)
{
	StripTrailingSeparatorImpl(pPath, nLength);
}


#if EA_WCHAR_UNIQUE
	EAIO_API PathStringW::iterator FindComponentFwd(PathStringW::iterator first,
													 PathStringW::iterator last)
	{
		// The PS3 compiler warned about PathCharTraits<wchar_t>::DriveSeparator being unused.  So, this EA_UNUSED statement
		// has been added to this somewhat arbitrary location in the code.
		EA_UNUSED(PathCharTraits<wchar_t>::DriveSeparator);

		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(FindComponentFwd(PathString16::iterator(first), PathString16::iterator(last)));
		#else
			return PathStringW::iterator(FindComponentFwd(PathString32::iterator(first), PathString32::iterator(last)));
		#endif
	}
	
	EAIO_API PathStringW::const_iterator FindComponentFwd(PathStringW::const_iterator first,
														  PathStringW::const_iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::const_iterator(FindComponentFwd(PathString16::const_iterator(first), PathString16::const_iterator(last)));
		#else
			return PathStringW::const_iterator(FindComponentFwd(PathString32::const_iterator(first), PathString32::const_iterator(last)));
		#endif
	}

	EAIO_API PathStringW::iterator FindComponentRvs(PathStringW::iterator first,
													 PathStringW::iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(FindComponentRvs(PathString16::iterator(first), PathString16::iterator(last)));
		#else
			return PathStringW::iterator(FindComponentRvs(PathString32::iterator(first), PathString32::iterator(last)));
		#endif
	}

	EAIO_API PathStringW::const_iterator FindComponentRvs(PathStringW::const_iterator first,
														  PathStringW::const_iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::const_iterator(FindComponentRvs(PathString16::const_iterator(first), PathString16::const_iterator(last)));
		#else
			return PathStringW::const_iterator(FindComponentRvs(PathString32::const_iterator(first), PathString32::const_iterator(last)));
		#endif
	}

	EAIO_API PathStringW::iterator GetPathComponentStart(PathStringW::iterator first,
															   PathStringW::iterator last,
															   int32_t nIndex)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(GetPathComponentStart(PathString16::iterator(first), PathString16::iterator(last), nIndex));
		#else
			return PathStringW::iterator(GetPathComponentStart(PathString32::iterator(first), PathString32::iterator(last), nIndex));
		#endif
	}

	EAIO_API PathStringW::iterator GetPathComponentEnd(PathStringW::iterator first,
													   PathStringW::iterator last,
													   int32_t nIndex)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(GetPathComponentEnd(PathString16::iterator(first), PathString16::iterator(last), nIndex));
		#else
			return PathStringW::iterator(GetPathComponentEnd(PathString32::iterator(first), PathString32::iterator(last), nIndex));
		#endif
	}

	EAIO_API PathStringW::const_iterator GetFileName(const PathStringW& path)
	{
		return GetFileName(path.begin(), path.end());
	}

	EAIO_API PathStringW::iterator GetFileName(PathStringW::const_iterator first,
											   PathStringW::const_iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(GetFileName(PathString16::const_iterator(first), PathString16::const_iterator(last)));
		#else
			return PathStringW::iterator(GetFileName(PathString32::const_iterator(first), PathString32::const_iterator(last)));
		#endif
	}

	EAIO_API PathStringW::const_iterator GetFileExtension(const PathStringW &path)
	{
		return GetFileExtension(path.begin(), path.end());
	}

	EAIO_API PathStringW::iterator GetFileExtension(PathStringW::const_iterator first,
													PathStringW::const_iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(GetFileExtension(PathString16::const_iterator(first), PathString16::const_iterator(last)));
		#else
			return PathStringW::iterator(GetFileExtension(PathString32::const_iterator(first), PathString32::const_iterator(last)));
		#endif
	}

	EAIO_API PathStringW::const_iterator GetLocalRoot(const PathStringW& path)
	{
		return GetLocalRoot(const_cast<PathStringW::iterator>(path.begin()), const_cast<PathStringW::iterator>(path.end()));
	}

	EAIO_API PathStringW::iterator GetLocalRoot(PathStringW::iterator first,
												PathStringW::iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return PathStringW::iterator(GetLocalRoot(PathString16::iterator(first), PathString16::iterator(last)));
		#else
			return PathStringW::iterator(GetLocalRoot(PathString32::iterator(first), PathString32::iterator(last)));
		#endif
	}

	EAIO_API PathStringW& Append(PathStringW &dst,
								 PathStringW::const_iterator first,
								 PathStringW::const_iterator last)
	{
		EA_ASSERT(first);
		if(last == kEndAutoW)
			last = StrEnd(first);

		EA_ASSERT(last);
		EA_ASSERT(first <= last);

		if(first == last)
			return dst;

		if(IsRelative(first, last))
		{
			if(!dst.empty()) // It makes little sense to append a '/' char to an empty dst, as an empty dst means there is no dst and the appended directory is all there is.
				EnsureTrailingSeparator(dst);
		}
		else
			dst.clear();

		dst.append(first, last);
		return dst;
	}

	EAIO_API PathStringW& Append(PathStringW &dst, const PathStringW& suffix)
	{
		return Append(dst, suffix.begin(), suffix.end());
	}

	EAIO_API PathStringW& Join(PathStringW &dst,
							   PathStringW::const_iterator suffix_first,
							   PathStringW::const_iterator suffix_last)
	{
		Append(dst, suffix_first, suffix_last);
		return Normalize(dst);
	}

	EAIO_API PathStringW& Join(PathStringW &dst, const PathStringW& suffix)
	{
		return Join(dst, suffix.begin(), suffix.end());
	}

	EAIO_API void Split(const PathStringW& path, PathStringW* pDrive, PathStringW* pDirectory, PathStringW* pFileName, PathStringW* pFileExtension)
	{
		PathStringW::const_iterator psDirectory     = GetLocalRoot(path);
		PathStringW::const_iterator psFileName      = GetFileName(path);
		PathStringW::const_iterator psFileExtension = GetFileExtension(path);

		if(pDrive)
			pDrive->assign(path.c_str(), psDirectory);
		if(pDirectory)
			pDirectory->assign(psDirectory, psFileName);
		if(pFileName)
			pFileName->assign(psFileName, psFileExtension);
		if(pFileExtension)
			pFileExtension->assign(psFileExtension, path.c_str() + path.length());
	}

	EAIO_API void Split(PathStringW::const_iterator path, PathStringW::const_iterator& pDrive, PathStringW::const_iterator& pDirectory, PathStringW::const_iterator& pFileName, PathStringW::const_iterator& pFileExtension)
	{
		pDrive         = path;
		pDirectory     = GetLocalRoot(path);
		pFileName      = GetFileName(path);
		pFileExtension = GetFileExtension(path);
	}

	EAIO_API PathStringW::const_iterator TruncateComponent(PathStringW& path, int nIndex)
	{
		PathStringW::iterator it = GetPathComponentStart(path.begin(), path.end(), nIndex);
		return path.erase(it, path.end());
	}

	EAIO_API PathStringW& Normalize(PathStringW& path)
	{
		#if (EA_WCHAR_SIZE == 2)
			PathString16 convertedPath;
		#else
			PathString32 convertedPath;
		#endif

		ConvertPath(convertedPath, &path[0]);
		path = PathStringW(PathStringNormalize(convertedPath, true));
		return path;
	}

	EAIO_API PathStringW& Simplify(PathStringW& path)
	{
		#if (EA_WCHAR_SIZE == 2)
			PathString16 convertedPath;
		#else
			PathString32 convertedPath;
		#endif

		ConvertPath(convertedPath, &path[0]);
		path = PathStringW(PathStringNormalize(convertedPath, true));
		return path;
	}

	EAIO_API PathStringW& Canonicalize(PathStringW& path, wchar_t separator)
	{
		for(PathStringW::iterator it = path.begin(); it != path.end(); ++it){
			if((*it == '/') || (*it =='\\')) // This may do the wrong thing with Unix-like paths.
				*it = separator;
		}
		return path;
	}

	EAIO_API bool IsRelative(PathStringW::const_iterator first,
							 PathStringW::const_iterator last)
	{
		#if (EA_WCHAR_SIZE == 2)
			return IsRelative(PathString16::const_iterator(first), PathString16::const_iterator(last));
		#else
			return IsRelative(PathString32::const_iterator(first), PathString32::const_iterator(last));
		#endif
	}

	EAIO_API bool IsRelative(const PathStringW& path)
	{
		return IsRelative(path.begin(), path.end());
	}

	EAIO_API int Compare(const PathStringW& a, const PathStringW& b, 
						 CaseSensitivity caseSensitivity, 
						 bool bDirectoriesFirst)
	{
		return Compare(a.begin(), a.end(), b.begin(), b.end(), caseSensitivity, bDirectoriesFirst);
	}

	EAIO_API int Compare(PathStringW::const_iterator a, PathStringW::const_iterator a_end,
						 PathStringW::const_iterator b, PathStringW::const_iterator b_end, 
						 CaseSensitivity caseSensitivity, 
						 bool bDirectoriesFirst)
	{
		#if (EA_WCHAR_SIZE == 2)
			return Compare(PathString16::const_iterator(a), PathString16::const_iterator(a_end),
							PathString16::const_iterator(b), PathString16::const_iterator(b_end),
							caseSensitivity,
							bDirectoriesFirst);
		#else
			return Compare(PathString32::const_iterator(a), PathString32::const_iterator(a_end),
							PathString32::const_iterator(b), PathString32::const_iterator(b_end),
							caseSensitivity,
							bDirectoriesFirst);
		#endif
	}

	EAIO_API PathStringW& ComputeRelative(PathStringW& result,
										   const PathStringW& src,
										   const PathStringW& target)
	{
		#if (EA_WCHAR_SIZE == 2)
			PathString16 convertedSrc;
			PathString16 convertedTarget;
			PathString16 convertedResult;
		#else
			PathString32 convertedSrc;
			PathString32 convertedTarget;
			PathString32 convertedResult;
		#endif
		ConvertPath(convertedSrc, &src[0]);
		ConvertPath(convertedTarget, &target[0]);
		ComputeRelative(convertedResult, convertedSrc, convertedTarget);
		result = PathStringW(convertedResult);
		return result;
	}

	EAIO_API bool IsSubdirectory(const PathStringW& dir, const PathStringW& subdir)
	{
		#if (EA_WCHAR_SIZE == 2)
			PathString16 convertedDir;
			PathString16 convertedSubdir;
		#else
			PathString32 convertedDir;
			PathString32 convertedSubdir;
		#endif
		ConvertPath(convertedDir, &dir[0]);
		ConvertPath(convertedSubdir, &subdir[0]);
		return IsSubdirectory(convertedDir, convertedSubdir);
	}

	EAIO_API bool GetHasTrailingSeparator(const PathStringW& path)
	{
		return (!path.empty() && IsDirectorySeparator(path.back()));
	}

	EAIO_API bool GetHasTrailingSeparator(PathStringW::const_iterator path, size_t nLength)
	{
		if (nLength == kLengthNull)
			nLength = EA::StdC::Strlen(path);

		return ((nLength != 0) && IsDirectorySeparator(*(path + nLength - 1)));
	}

	EAIO_API PathStringW& EnsureTrailingSeparator(PathStringW& path)
	{
		// To consider: should we add to an empty path?
		if(/*!path.empty() &&*/ !GetHasTrailingSeparator(path))
			path.push_back(EA_FILE_PATH_SEPARATOR_W);
		return path;
	}

	EAIO_API bool EnsureTrailingSeparator(wchar_t* pPath, size_t dirCapacity)
	{
		#if (EA_WCHAR_SIZE == 2)
			return EnsureTrailingSeparator(reinterpret_cast<char16_t*>(pPath), dirCapacity);
		#else
			return EnsureTrailingSeparator(reinterpret_cast<char32_t*>(pPath), dirCapacity);
		#endif
	}

	EAIO_API PathStringW& StripTrailingSeparator(PathStringW& path)
	{
		if (GetHasTrailingSeparator(path))
			path.pop_back();

		return path;
	}

	EAIO_API void StripTrailingSeparator(wchar_t* pPath, size_t nLength)
	{
		#if (EA_WCHAR_SIZE == 2)
			StripTrailingSeparator(reinterpret_cast<char16_t*>(pPath), nLength);
		#else
			StripTrailingSeparator(reinterpret_cast<char32_t*>(pPath), nLength);
		#endif
	}
#endif




} // namespace Path

} // namespace IO

} // namespace EA












