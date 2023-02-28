/////////////////////////////////////////////////////////////////////////////
// FnEncode.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Created by Alex Liberman and Talin
//
// Character transcoding function for filenames.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include EA_ASSERT_HEADER


namespace EA
{
	namespace IO
	{
		EAIO_API uint32_t ConvertPath(Path::PathString8& dstPath8, const char16_t* pSrcPath16)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char*)NULL, pSrcPath16, 0);
			dstPath8.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath8[0], pSrcPath16, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}

		EAIO_API uint32_t ConvertPath(Path::PathString16& dstPath16, const char* pSrcPath8)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char16_t*)NULL, pSrcPath8, 0);
			dstPath16.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath16[0], pSrcPath8, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}


		EAIO_API uint32_t ConvertPath(Path::PathString8& dstPath8, const char32_t* pSrcPath32)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char*)NULL, pSrcPath32, 0);
			dstPath8.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath8[0], pSrcPath32, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}

		EAIO_API uint32_t ConvertPath(Path::PathString32& dstPath32, const char* pSrcPath8)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char32_t*)NULL, pSrcPath8, 0);
			dstPath32.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath32[0], pSrcPath8, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}


		EAIO_API uint32_t ConvertPath(Path::PathString16& dstPath16, const char32_t* pSrcPath32)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char16_t*)NULL, pSrcPath32, 0);
			dstPath16.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath16[0], pSrcPath32, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}

		EAIO_API uint32_t ConvertPath(Path::PathString32& dstPath32, const char16_t* pSrcPath16)
		{
			const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char32_t*)NULL, pSrcPath16, 0);
			dstPath32.resize(nCharsNeeded);
			return (uint32_t)EA::StdC::Strlcpy(&dstPath32[0], pSrcPath16, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
		}


		#if EA_WCHAR_UNIQUE
			EAIO_API uint32_t ConvertPath(Path::PathString8& dstPath8, const wchar_t* pSrcPathW)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char*)NULL, pSrcPathW, 0);
				dstPath8.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPath8[0], pSrcPathW, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}

			EAIO_API uint32_t ConvertPath(Path::PathString16& dstPath16, const wchar_t* pSrcPathW)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char16_t*)NULL, pSrcPathW, 0);
				dstPath16.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPath16[0], pSrcPathW, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}

			EAIO_API uint32_t ConvertPath(Path::PathString32& dstPath32, const wchar_t* pSrcPathW)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((char32_t*)NULL, pSrcPathW, 0);
				dstPath32.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPath32[0], pSrcPathW, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}

			EAIO_API uint32_t ConvertPath(Path::PathStringW& dstPathW, const char* pSrcPath8)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((wchar_t*)NULL, pSrcPath8, 0);
				dstPathW.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPathW[0], pSrcPath8, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}

			EAIO_API uint32_t ConvertPath(Path::PathStringW& dstPathW, const char16_t* pSrcPath16)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((wchar_t*)NULL, pSrcPath16, 0);
				dstPathW.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPathW[0], pSrcPath16, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}
			
			EAIO_API uint32_t ConvertPath(Path::PathStringW& dstPathW, const char32_t* pSrcPath32)
			{
				const uint32_t nCharsNeeded = (uint32_t)EA::StdC::Strlcpy((wchar_t*)NULL, pSrcPath32, 0);
				dstPathW.resize(nCharsNeeded);
				return (uint32_t)EA::StdC::Strlcpy(&dstPathW[0], pSrcPath32, nCharsNeeded + 1); // (EASTL factors-in a hidden + 1 for NULL terminator)
			}
		#endif




		EAIO_API bool StrEq(const char16_t* str1, const char16_t* str2)
		{
			while (*str1 == *str2)
			{
				if (*str1 == 0)
					return true;
				str1++;
				str2++;
			}
			return false;
		}

		EAIO_API bool StrEq(const char32_t* str1, const char32_t* str2)
		{
			while (*str1 == *str2)
			{
				if (*str1 == 0)
					return true;
				str1++;
				str2++;
			}
			return false;
		}


		#if EA_WCHAR_UNIQUE
			EAIO_API bool StrEq(const wchar_t* str1, const wchar_t* str2)
			{
				while (*str1 == *str2)
				{
					if (*str1 == 0)
						return true;
					str1++;
					str2++;
				}
				return false;
			}
		#endif





		///////////////////////////////////////////////////////////////
		// Deprecated -- Use the above functions and EAStdC instead. //
		///////////////////////////////////////////////////////////////

		EAIO_API size_t   StrlcpyUTF16ToUTF8(char* pDest, size_t nDestLength, const char16_t* pSrc, size_t nSrcLength)
			{ return EA::StdC::Strlcpy(pDest, pSrc, nDestLength, nSrcLength); }
		EAIO_API size_t   StrlcpyUTF8ToUTF16(char16_t* pDest, size_t nDestLength, const char* pSrc, size_t nSrcLength)
			{ return EA::StdC::Strlcpy(pDest, pSrc, nDestLength, nSrcLength); }
		EAIO_API uint32_t ConvertPathUTF8ToUTF16(Path::PathString16& dstPath16, const char* pSrcPath8)
			{ return ConvertPath(dstPath16, pSrcPath8); }
		EAIO_API uint32_t ConvertPathUTF16ToUTF8(Path::PathString8& dstPath8, const char16_t* pSrcPath16)
			{ return ConvertPath(dstPath8, pSrcPath16); }
		EAIO_API bool     StrEq16(const char16_t* str1, const char16_t* str2)
			{ return StrEq(str1, str2); }
		EAIO_API size_t   EAIOStrlen8(const char* str)
			{ return EA::StdC::Strlen(str); }
		EAIO_API size_t   EAIOStrlen16(const char16_t* str)
			{ return EA::StdC::Strlen(str); }
		EAIO_API size_t   EAIOStrlcpy8(char* pDestination, const char* pSource, size_t nDestCapacity)
			{ return EA::StdC::Strlcpy(pDestination, pSource, nDestCapacity); }
		EAIO_API size_t   EAIOStrlcpy16(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity)
			{ return EA::StdC::Strlcpy(pDestination, pSource, nDestCapacity); }
		EAIO_API size_t   EAIOStrlcat16(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity)
			{ return EA::StdC::Strlcpy(pDestination, pSource, nDestCapacity); }

	} // namespace IO

} // namespace EA
