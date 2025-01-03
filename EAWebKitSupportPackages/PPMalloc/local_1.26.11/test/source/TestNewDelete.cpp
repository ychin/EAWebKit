///////////////////////////////////////////////////////////////////////////////
// TestNewDelete
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <PPMallocTest/PPMallocTest.h>


// This is disabled because it requires new/delete operators to be compiled
// into the PPMalloc runtime. But we don't do that and so don't currently have
// a practical way to test them. However, we would like to move EANewDelete 
// into a .inl file that can be #included by user code such as main.cpp. 
// That would allow this test to be implemented cleanly as well.

int TestNewDelete()
{
	return 0;
}















