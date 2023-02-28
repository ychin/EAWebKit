/////////////////////////////////////////////////////////////////////////////
// TestBreak.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextIterator.h>
#include <EASTL/algorithm.h>
#include <EATest/EATest.h>


using namespace EA::Text;



int TestIterator()
{
	int nErrorCount = 0;

	{   // Test an empty text run.
		const uint32_t  kSize = 10;
		TextRun         trEmpty[kSize];         // There are multiple TextRuns, but they are each empty.
		TextRunIterator tri(trEmpty, kSize);
		TextRunIterator triCopy(trEmpty, kSize);

		EATEST_VERIFY(tri == triCopy);

		EATEST_VERIFY(tri.GetSize() == 0);
		EATEST_VERIFY(tri.Empty());
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(tri.AtEnd());
		EATEST_VERIFY(tri.GetPosition() == 0);
		//EATEST_VERIFY(*tri == kCharInvalid); // This function requires the iterator to be valid.
		EATEST_VERIFY(tri[0] == kCharInvalid);
		EATEST_VERIFY(tri[1] == kCharInvalid);
		EATEST_VERIFY(tri.GetRelativeChar(0) == kCharInvalid);
		EATEST_VERIFY(tri.GetRelativeChar(1) == kCharInvalid);
		EATEST_VERIFY(tri.GetRelativeChar(-1) == kCharInvalid);
		tri.SetPosition(0);
		EATEST_VERIFY(*tri == kCharInvalid);
		tri.SetPosition(1);
		EATEST_VERIFY(*tri == kCharInvalid);

		++tri;
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(tri.AtEnd());

		--tri;
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(tri.AtEnd());

		tri += 7;
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(tri.AtEnd());

		tri -= 7;
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(tri.AtEnd());

		triCopy = tri + 7;
		EATEST_VERIFY(triCopy.AtBegin());
		EATEST_VERIFY(triCopy.AtEnd());
		EATEST_VERIFY(triCopy.GetSize() == 0);
	}


	{   // Test a text run with only one char be array element.
		const uint32_t kSize = 5;
		const Char     pString[kSize] = { 'a', 'b', 'c', 'd', 'e' };
		TextRun        trTest[kSize + 3];
		uint32_t       i;

		for(i = 0; i < kSize; i++)
		{
			trTest[i].mpText     = pString + i;
			trTest[i].mnTextSize = 1;
		}

		TextRunIterator tri(trTest, EAArrayCount(trTest));

		EATEST_VERIFY(tri.GetSize() == kSize);
		EATEST_VERIFY(!tri.Empty());
		EATEST_VERIFY(tri.AtBegin());
		EATEST_VERIFY(!tri.AtEnd());
		EATEST_VERIFY(tri.GetPosition() == 0);
		for(i = 0; i < kSize; i++)
			EATEST_VERIFY(tri[i] == pString[i]);
		EATEST_VERIFY(tri.GetRelativeChar(0) == 'a');
		EATEST_VERIFY(tri.GetRelativeChar(1) == 'b');
		EATEST_VERIFY(tri.GetRelativeChar(-1) == kCharInvalid);
		tri.SetPosition(kSize);
		EATEST_VERIFY(*tri == kCharInvalid);
	}


	{   // Test EASTL iterator functionality of TextRunIterator.

		const uint32_t kSize = 5;
		const Char     pString[kSize] = { 'a', 'b', 'c', 'd', 'e' };
		TextRun        trTest[kSize + 3];
		uint32_t       i;

		for(i = 0; i < kSize; i++)
		{
			trTest[i].mpText     = pString + i;
			trTest[i].mnTextSize = 1;
		}

		TextRunIterator tri(trTest, EAArrayCount(trTest));

		// Test various EASTL algorithms
		TextRunIterator it;

		it = eastl::min_element(tri, tri + kSize);
		EATEST_VERIFY(*it == L'a');

		it = eastl::find(tri, tri + kSize, L'c');
		EATEST_VERIFY(*it == L'c');

		it = eastl::find(tri, tri + kSize, L'q');
		EATEST_VERIFY(*it == kCharInvalid);
		EATEST_VERIFY(it == (tri + kSize));

		it = eastl::lower_bound(tri, tri + kSize, L'd');
		EATEST_VERIFY(*it == L'd');
	}

	return nErrorCount;
}

























