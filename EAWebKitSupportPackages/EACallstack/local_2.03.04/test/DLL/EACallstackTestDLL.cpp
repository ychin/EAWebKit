///////////////////////////////////////////////////////////////////////////////
// EACallstackTestDLL.cpp
//
// Copyright (c) 2007 Electronic Arts Inc.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "EACallstackTestDLL.h"

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


EACALLSTACKTESTDLL_API void CallFunction(FunctionType pFunction)
{
	pFunction();
}










