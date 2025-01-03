///////////////////////////////////////////////////////////////////////////////
// FnMatch.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Maintained by Paul Pedriana.
//
// Implements a file path match much like the Unix fnmatch function.
///////////////////////////////////////////////////////////////////////////////

#include <EAIO/EAFileBase.h>
#include <EAIO/internal/Config.h>
#include <EAIO/FnMatch.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>


namespace EA
{

namespace IO
{

namespace // anonymous
{


///////////////////////////////////////////////////////////////////////////////
// RangeMatch
//
// Helper function for FnMatch.
//
const char* RangeMatch(const char* pPattern, char cSeparator, char test, int fnMatchFlags)
{
	using namespace EA::StdC;

	int     ok;
	char c, c2;
	int     negate = (*pPattern == '!' || *pPattern == '^');

	if(negate)
		++pPattern;

	if(fnMatchFlags & kFNMCaseFold)
		test = Tolower(test);

	for(ok = 0; (c = *pPattern++) != ']'; ) 
	{
		if((c == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
			c = *pPattern++;

		if(c == '\0')
			return NULL;

		if(fnMatchFlags & kFNMCaseFold)
			c = Tolower(c);

		if((*pPattern == '-') && ((c2 = pPattern[1]) != '\0') && (c2 != ']')) 
		{
			pPattern += 2;

			if((c2 == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
				c2 = *pPattern++;

			if(c2 == '\0')
				return NULL;

			if(fnMatchFlags & kFNMCaseFold)
				c2 = Tolower(c2);

			if((uint8_t)c    <= (uint8_t)test &&
			   (uint8_t)test <= (uint8_t)c2)
			{
				ok = 1;
			}
		} 
		else if(c == test)
			ok = 1;
	}

	return (ok == negate) ? NULL : pPattern;
}


const char16_t* RangeMatch(const char16_t* pPattern, char16_t cSeparator, char16_t test, int fnMatchFlags)
{
	using namespace EA::StdC;

	int      ok;
	char16_t c, c2;
	int      negate = (*pPattern == '!' || *pPattern == '^');

	if(negate)
		++pPattern;

	if(fnMatchFlags & kFNMCaseFold)
		test = Tolower(test);

	for(ok = 0; (c = *pPattern++) != ']'; ) 
	{
		if((c == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
			c = *pPattern++;

		if(c == '\0')
			return NULL;

		if(fnMatchFlags & kFNMCaseFold)
			c = Tolower(c);

		if((*pPattern == '-') && ((c2 = pPattern[1]) != '\0') && (c2 != ']')) 
		{
			pPattern += 2;

			if((c2 == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
				c2 = *pPattern++;

			if(c2 == '\0')
				return NULL;

			if(fnMatchFlags & kFNMCaseFold)
				c2 = Tolower(c2);

			if((char16_t)c    <= (char16_t)test &&
			   (char16_t)test <= (char16_t)c2)
			{
				ok = 1;
			}
		} 
		else if(c == test)
			ok = 1;
	}

	return (ok == negate) ? NULL : pPattern;
}


const char32_t* RangeMatch(const char32_t* pPattern, char32_t cSeparator, char32_t test, int fnMatchFlags)
{
	using namespace EA::StdC;

	int      ok;
	char32_t c, c2;
	int      negate = (*pPattern == '!' || *pPattern == '^');

	if(negate)
		++pPattern;

	if(fnMatchFlags & kFNMCaseFold)
		test = Tolower(test);

	for(ok = 0; (c = *pPattern++) != ']'; ) 
	{
		if((c == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
			c = *pPattern++;

		if(c == '\0')
			return NULL;

		if(fnMatchFlags & kFNMCaseFold)
			c = Tolower(c);

		if((*pPattern == '-') && ((c2 = pPattern[1]) != '\0') && (c2 != ']')) 
		{
			pPattern += 2;

			if((c2 == '\\') && (cSeparator != '\\') && ((fnMatchFlags & kFNMNoEscape) == 0))
				c2 = *pPattern++;

			if(c2 == '\0')
				return NULL;

			if(fnMatchFlags & kFNMCaseFold)
				c2 = Tolower(c2);

			if((char32_t)c    <= (char32_t)test &&
			   (char32_t)test <= (char32_t)c2)
			{
				ok = 1;
			}
		} 
		else if(c == test)
			ok = 1;
	}

	return (ok == negate) ? NULL : pPattern;
}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// FnMatch
//
EAIO_API bool FnMatch(const char* EA_RESTRICT pPattern, const char* EA_RESTRICT pString, int fnMatchFlags)
{
	using namespace EA::StdC;

	#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS)
		if((fnMatchFlags & kFNMUnixPath) == 0)
			fnMatchFlags |= kFNMDosPath;
	#else
		if((fnMatchFlags & kFNMDosPath) == 0)
			fnMatchFlags |= kFNMUnixPath;
	#endif

	const char* const pStringBegin = pString;
	const char        cSeparator = (fnMatchFlags & kFNMDosPath) ? '\\' : '/';
	char              test;

	for(;;)
	{
		char c = *pPattern++;

		switch (c)
		{
			case '\0':
				if((fnMatchFlags & kFNMLeadingDir) && (*pString == cSeparator))
					return true;

				return (*pString == '\0') ? true : false;

			case '?':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((*pString == '.') && 
					(fnMatchFlags & kFNMPeriod) &&
					((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				++pString;
				break;

			case '*':
				c = *pPattern;

				while(c == '*')
					c = *++pPattern;

				if((*pString == '.') && 
				   (fnMatchFlags & kFNMPeriod) &&
				   ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				if(c == '\0')
				{
					if(fnMatchFlags & kFNMPathname)
						return (fnMatchFlags & kFNMLeadingDir) || !Strchr(pString, cSeparator);
					else
						return true;
				}

				else if((c == cSeparator) && (fnMatchFlags & kFNMPathname))
				{
					pString = Strchr(pString, cSeparator);

					if(pString == NULL)
						return false;
					break;
				}

				while((test = *pString) != '\0') 
				{
					if(FnMatch(pPattern, pString, (fnMatchFlags & ~kFNMPeriod)))
						return true;

					if((test == cSeparator) && (fnMatchFlags & kFNMPathname))
						break;

					++pString;
				}

				return false;

			case '[':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((pPattern = RangeMatch(pPattern, cSeparator, *pString, fnMatchFlags)) == NULL)
					return false;

				++pString;
				break;

			case '\\':
				if(cSeparator != '\\')  // '\' is always treated as a file path separator when using DOS paths.
				{
					if((fnMatchFlags & kFNMNoEscape) == 0)
					{
						if((c = *pPattern++) == '\0')
						{
							c = '\\';
							--pPattern;
						}
					}
				}
				// Fall through

			default:
				if(c == *pString)
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMCaseFold) && (Tolower(c) == Tolower(*pString)))
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMPrefixDir) && 
						 (*pString == '\0') &&
						 (((c == cSeparator) && (pString != pStringBegin)) || ((pString == (pStringBegin + 1)) && (*pStringBegin == cSeparator))))
				{
					return true;
				}
				else
					return false;

				pString++;
				break;

		} // switch()

	} // for()

} // FnMatch


///////////////////////////////////////////////////////////////////////////////
// FnMatch
//
EAIO_API bool FnMatch(const char16_t* EA_RESTRICT pPattern, const char16_t* EA_RESTRICT pString, int fnMatchFlags)
{
	using namespace EA::StdC;

	#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS)
		if((fnMatchFlags & kFNMUnixPath) == 0)
			fnMatchFlags |= kFNMDosPath;
	#else
		if((fnMatchFlags & kFNMDosPath) == 0)
			fnMatchFlags |= kFNMUnixPath;
	#endif

	const char16_t* const pStringBegin = pString;
	const char16_t        cSeparator = (fnMatchFlags & kFNMDosPath) ? '\\' : '/';
	char16_t              test;

	for(;;)
	{
		char16_t c = *pPattern++;

		switch (c)
		{
			case '\0':
				if((fnMatchFlags & kFNMLeadingDir) && (*pString == cSeparator))
					return true;

				return (*pString == '\0') ? true : false;

			case '?':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((*pString == '.') && 
					(fnMatchFlags & kFNMPeriod) &&
					((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				++pString;
				break;

			case '*':
				c = *pPattern;

				while(c == '*')
					c = *++pPattern;

				if((*pString == '.') && 
				   (fnMatchFlags & kFNMPeriod) &&
				   ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				if(c == '\0')
				{
					if(fnMatchFlags & kFNMPathname)
						return (fnMatchFlags & kFNMLeadingDir) || !Strchr(pString, cSeparator);
					else
						return true;
				}

				else if((c == cSeparator) && (fnMatchFlags & kFNMPathname))
				{
					pString = Strchr(pString, cSeparator);

					if(pString == NULL)
						return false;
					break;
				}

				while((test = *pString) != '\0') 
				{
					if(FnMatch(pPattern, pString, (fnMatchFlags & ~kFNMPeriod)))
						return true;

					if((test == cSeparator) && (fnMatchFlags & kFNMPathname))
						break;

					++pString;
				}

				return false;

			case '[':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((pPattern = RangeMatch(pPattern, cSeparator, *pString, fnMatchFlags)) == NULL)
					return false;

				++pString;
				break;

			case '\\':
				if(cSeparator != '\\')  // '\' is always treated as a file path separator when using DOS paths.
				{
					if((fnMatchFlags & kFNMNoEscape) == 0)
					{
						if((c = *pPattern++) == '\0')
						{
							c = '\\';
							--pPattern;
						}
					}
				}
				// Fall through

			default:
				if(c == *pString)
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMCaseFold) && (Tolower(c) == Tolower(*pString)))
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMPrefixDir) && 
						 (*pString == '\0') &&
						 (((c == cSeparator) && (pString != pStringBegin)) || ((pString == (pStringBegin + 1)) && (*pStringBegin == cSeparator))))
				{
					return true;
				}
				else
					return false;

				pString++;
				break;

		} // switch()

	} // for()

} // FnMatch


///////////////////////////////////////////////////////////////////////////////
// FnMatch
//
EAIO_API bool FnMatch(const char32_t* EA_RESTRICT pPattern, const char32_t* EA_RESTRICT pString, int fnMatchFlags)
{
	using namespace EA::StdC;

	#if defined(EA_FILE_PATH_SEPARATOR_TYPE_WINDOWS)
		if((fnMatchFlags & kFNMUnixPath) == 0)
			fnMatchFlags |= kFNMDosPath;
	#else
		if((fnMatchFlags & kFNMDosPath) == 0)
			fnMatchFlags |= kFNMUnixPath;
	#endif

	const char32_t* const pStringBegin = pString;
	const char32_t        cSeparator = (fnMatchFlags & kFNMDosPath) ? '\\' : '/';
	char32_t              test;

	for(;;)
	{
		char32_t c = *pPattern++;

		switch (c)
		{
			case '\0':
				if((fnMatchFlags & kFNMLeadingDir) && (*pString == cSeparator))
					return true;

				return (*pString == '\0') ? true : false;

			case '?':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((*pString == '.') && 
					(fnMatchFlags & kFNMPeriod) &&
					((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				++pString;
				break;

			case '*':
				c = *pPattern;

				while(c == '*')
					c = *++pPattern;

				if((*pString == '.') && 
				   (fnMatchFlags & kFNMPeriod) &&
				   ((pString == pStringBegin) || ((fnMatchFlags & kFNMPathname) && (pString[-1] == cSeparator))))
				{
					return false;
				}

				if(c == '\0')
				{
					if(fnMatchFlags & kFNMPathname)
						return (fnMatchFlags & kFNMLeadingDir) || !Strchr(pString, cSeparator);
					else
						return true;
				}

				else if((c == cSeparator) && (fnMatchFlags & kFNMPathname))
				{
					pString = Strchr(pString, cSeparator);

					if(pString == NULL)
						return false;
					break;
				}

				while((test = *pString) != '\0') 
				{
					if(FnMatch(pPattern, pString, (fnMatchFlags & ~kFNMPeriod)))
						return true;

					if((test == cSeparator) && (fnMatchFlags & kFNMPathname))
						break;

					++pString;
				}

				return false;

			case '[':
				if(*pString == '\0')
					return false;

				if((*pString == cSeparator) && (fnMatchFlags & kFNMPathname))
					return false;

				if((pPattern = RangeMatch(pPattern, cSeparator, *pString, fnMatchFlags)) == NULL)
					return false;

				++pString;
				break;

			case '\\':
				if(cSeparator != '\\')  // '\' is always treated as a file path separator when using DOS paths.
				{
					if((fnMatchFlags & kFNMNoEscape) == 0)
					{
						if((c = *pPattern++) == '\0')
						{
							c = '\\';
							--pPattern;
						}
					}
				}
				// Fall through

			default:
				if(c == *pString)
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMCaseFold) && (Tolower(c) == Tolower(*pString)))
				{
					// Empty
				}
				else if((fnMatchFlags & kFNMPrefixDir) && 
						 (*pString == '\0') &&
						 (((c == cSeparator) && (pString != pStringBegin)) || ((pString == (pStringBegin + 1)) && (*pStringBegin == cSeparator))))
				{
					return true;
				}
				else
					return false;

				pString++;
				break;

		} // switch()

	} // for()

} // FnMatch



} // namespace IO

} // namespace EA













