#pragma once
#include <EAText/EAText.h>
namespace EA
{
namespace Text
{
	struct CollElement
	{
		uint16_t mL1;
		uint8_t  mL2;
		uint8_t  mL3;
	};

	struct MultiCollEntry
	{
		Char        mChar1;
		CollElement mCE0;
		CollElement mCE1;
	};

	struct MultiCollIndex
	{
		Char    mChar0;
		uint8_t mCount;
	};

	struct CollEntry1
	{
		Char        mChar;
		CollElement mCE0;
	};

	struct CollEntry2
	{
		Char mChar;
		CollElement mCE0;
		CollElement mCE1;
	};

	struct CollEntry3
	{
		Char mChar;
		CollElement mCE0;
		CollElement mCE1;
		CollElement mCE2;
	};

}} // namespace EA::Text
