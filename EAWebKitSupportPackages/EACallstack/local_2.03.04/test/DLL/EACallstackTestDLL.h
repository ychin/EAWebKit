///////////////////////////////////////////////////////////////////////////////
// EACallstackTestDLL.h
//
// Copyright (c) 2007 Electronic Arts Inc.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EACALLSTACKTESTDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EACALLSTACKTESTDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef EACALLSTACKTESTDLL_EXPORTS
	#define EACALLSTACKTESTDLL_API __declspec(dllexport)
#else
	#define EACALLSTACKTESTDLL_API __declspec(dllimport)
#endif


typedef void (*FunctionType)();
typedef void (*CallFunctionType)(FunctionType pFunction);


extern "C" {

/// CallFunction
///
/// This is merely a function which calls the supplied function.
/// This function is of type CallFunctionType, and it calls a 
/// function of type FunctionType
/// 
EACALLSTACKTESTDLL_API void CallFunction(FunctionType pFunction);

}




