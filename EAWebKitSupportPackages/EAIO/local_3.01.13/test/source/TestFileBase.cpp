/////////////////////////////////////////////////////////////////////////////
// TestFileBase.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/PathString.h>
#include <EAStdC/EAString.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <stdio.h>
#include EA_ASSERT_HEADER



///////////////////////////////////////////////////////////////////////////////
// TestFileBase
//
int TestFileBase()
{
	int  nErrorCount = 0;
	bool bResult;

	// There isn't a lot to do here.

	bResult = EA::IO::IsFilePathSeparator(EA_FILE_PATH_SEPARATOR_8);
	EATEST_VERIFY(bResult);

	bResult = EA::IO::IsFilePathSeparator(EA_FILE_PATH_SEPARATOR_16);
	EATEST_VERIFY(bResult);

	EA::IO::FilePathBuffer8 pathBuffer8  = { 0 };
	EA::IO::FilePathBufferW pathBufferW = { 0 };

	char buffer[32];
	sprintf(buffer, "%s %ls\n", pathBuffer8, pathBufferW);

	return nErrorCount;
}





