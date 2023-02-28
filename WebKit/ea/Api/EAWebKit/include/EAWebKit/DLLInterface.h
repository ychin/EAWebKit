/*
Copyright (C) 2010, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
its contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
///////////////////////////////////////////////////////////////////////////////
// DLLInterface.h
// By Paul Pedriana/Arpit Baldeva
// Maintained by EAWebKit Team
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////


/******************************************************************************
    DLLInterface.h

    The following macros provide pseudo-platform-independent support for 
    defining a dynamically loaded library.


    PLATFORM_DLL_MODULE(name, attribute, major, minor)
    Provide the name of the module, any attributes, major and minor version info. 
    Name is a user-defined 0-terminated string literal of 27 chars or less. 
    Attributes, major, and minor are user-defined integer constants.
    Note that the interface allows defining independent libraries within the 
    module, via PLATFORM_DLL_LIB (below).


    PLATFORM_DLL_START
    Provide/map a custom start function to be called when the dll is started.
    Function is of the form: extern "C" int StartFunctionName(void);
    The following return values are defined:

        PLATFORM_DLL_START_SUCCESS
        Return this from the DLL start function if the DLL should remain loaded, 
        as is usually the case.

        PLATFORM_DLL_START_FAILURE
        Return this from the DLL start function if the DLL should be immediately
        unloaded, which would be the case if the entire processing of the DLL 
        completed in the start function. 


    PLATFORM_DLL_STOP
    Provide/map a custom stop function to be called when the dll is stopped.
    Function is of the form: extern "C" int StopFunctionName(void);
    The following return values are defined:

        PLATFORM_DLL_STOP_SUCCESS
        Return this from the DLL stop function if it succeeded, as is usually 
        the case.

        PLATFORM_DLL_STOP_FAILURE
        Return this from the DLL stop function if it failed.


    PLATFORM_DLL_LIB(libname, attr)
    Defines a library within the module along with any startup attributes.
    libname is a C identifier and its name is exported by the module and thus
    may not be the same as any other symbol within the module.
    The following attributes, which may be or-d together, are defined:

        PLATFORM_DLL_LIB_ATTR_REGISTER
        Automatically register the library to the system.

        PLATFORM_DLL_LIB_ATTR_OVERRIDE
        Overwrite any resident library of the same name and version.

        PLATFORM_DLL_LIB_ATTR_NOLINK
        Undocumented.

        PLATFORM_DLL_LIB_ATTR_DEPENDENT_LOAD
        Load other dlls that reference this library even if it is not loaded.


	PLATFORM_DLL_EXPORT_INTERFACE
	Define the interface used to export symbols from the library.

	PLATFORM_DLL_IMPORT_INTERFACE
	Define the interface used to import symbols from the library.
		
	PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype)
	Declare the function to be exported from the library.
	The function is any global public extern "C" function the user has compiled into the module. The rettype is
	the return type of the function. interfacetype defines the interface of the function (import or export). 

	PLATFORM_DLL_EXPORT_FUNC(funcname, libname)
    Export a function from the library.
    The function is any global public extern "C" function the user has compiled into the module.
    The library is a library declared with PLATFORM_DLL_DECLARE.


    PLATFORM_DLL_EXPORT_VAR(variable, libname)
    Export a variable from the library.
    The variable is any global public variable the user has compiled into the module.
    The library is a library declared with PLATFORM_DLL_DECLARE.

******************************************************************************/


#ifndef DllInterface_h
#define DllInterface_h

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0 
#endif

#if   defined(__ORBIS__) || defined (__PROSPERO__)
#define PLATFORM_DLL_MODULE(name,attr,major,minor) 

#if defined(__cplusplus)
#define PLATFORM_DLL_START(funcname) extern "C" int module_start(size_t args, const void* argp){return funcname(args,argp);}	
#else
#define PLATFORM_DLL_START(funcname) int module_start(size_t args, const void* argp){return funcname(args,argp);}
#endif

#define PLATFORM_DLL_START_SUCCESS                      TRUE
#define PLATFORM_DLL_START_FAILURE                      FALSE

#if defined(__cplusplus)
#define PLATFORM_DLL_STOP(funcname)	extern "C" int module_stop(size_t args, const void* argp){return funcname(args,argp);}	
#else
#define PLATFORM_DLL_STOP(funcname)	int module_stop(size_t args, const void* argp){return funcname(args,argp);}		
#endif
#define PLATFORM_DLL_STOP_SUCCESS                       TRUE
#define PLATFORM_DLL_STOP_FAILURE                       FALSE

#define PLATFORM_DLL_LIB(libname,attr)
#define PLATFORM_DLL_LIB_ATTR_REGISTER					0x0001
#define PLATFORM_DLL_LIB_ATTR_OVERRIDE					0x0002
#define PLATFORM_DLL_LIB_ATTR_DEPENDENT_LOAD			0x0008

#define PLATFORM_DLL_EXPORT_INTERFACE __declspec(dllexport)
#define PLATFORM_DLL_IMPORT_INTERFACE __declspec(dllimport)

#if defined(__cplusplus)
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) extern "C" interfacetype rettype funcname(); 
#else
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) interfacetype rettype funcname(); 
#endif

#define PLATFORM_DLL_EXPORT_FUNC(funcname, libname)
#define PLATFORM_DLL_EXPORT_VAR(variable, libname)   


#elif defined(_WIN32) || defined(_WIN64) 

#define PLATFORM_DLL_MODULE(name,attr,major,minor) 

#if defined(__cplusplus)
#define PLATFORM_DLL_START(funcname) extern "C" int platform_dll_start_func(void){return funcname(0,0);}	
#else
#define PLATFORM_DLL_START(funcname) int platform_dll_start_func(void){return funcname(0,0);}	
#endif
#define PLATFORM_DLL_START_SUCCESS                      TRUE
#define PLATFORM_DLL_START_FAILURE                      FALSE

#if defined(__cplusplus)
#define PLATFORM_DLL_STOP(funcname)	extern "C" int platform_dll_stop_func(void){return funcname(0,0);}	
#else
#define PLATFORM_DLL_STOP(funcname)	int platform_dll_stop_func(void){return funcname(0,0);}	
#endif
#define PLATFORM_DLL_STOP_SUCCESS                       TRUE
#define PLATFORM_DLL_STOP_FAILURE                       FALSE

#define PLATFORM_DLL_LIB(libname,attr)
#define PLATFORM_DLL_LIB_ATTR_REGISTER					0x0001
#define PLATFORM_DLL_LIB_ATTR_OVERRIDE					0x0002
#define PLATFORM_DLL_LIB_ATTR_DEPENDENT_LOAD			0x0008

#define PLATFORM_DLL_EXPORT_INTERFACE 
#define PLATFORM_DLL_IMPORT_INTERFACE 

#if defined(__cplusplus)
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) 
#else
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) 
#endif

#define PLATFORM_DLL_EXPORT_FUNC(funcname, libname)
#define PLATFORM_DLL_EXPORT_VAR(variable, libname)   

#elif defined(__APPLE__) 

#define PLATFORM_DLL_MODULE(name,attr,major,minor) 

#if defined(__cplusplus)
#define PLATFORM_DLL_START(funcname) extern "C" void __attribute__ ((constructor)) platform_dll_start_func(void){funcname(0,0);}	
#else
#define PLATFORM_DLL_START(funcname) void __attribute__ ((constructor)) platform_dll_start_func(void){funcname(0,0);}	
#endif
#define PLATFORM_DLL_START_SUCCESS                      1
#define PLATFORM_DLL_START_FAILURE                      0

#if defined(__cplusplus)
#define PLATFORM_DLL_STOP(funcname)	extern "C" void __attribute__ ((destructor)) platform_dll_stop_func(void){funcname(0,0);}	
#else
#define PLATFORM_DLL_STOP(funcname)	void __attribute__ ((destructor)) platform_dll_stop_func(void){funcname(0,0);}	
#endif
#define PLATFORM_DLL_STOP_SUCCESS                       1
#define PLATFORM_DLL_STOP_FAILURE                       0

#define PLATFORM_DLL_LIB(libname,attr)
#define PLATFORM_DLL_LIB_ATTR_REGISTER					0x0001
#define PLATFORM_DLL_LIB_ATTR_OVERRIDE					0x0002
#define PLATFORM_DLL_LIB_ATTR_DEPENDENT_LOAD			0x0008

#define PLATFORM_DLL_EXPORT_INTERFACE __attribute__ ((visibility ("default")))
#define PLATFORM_DLL_IMPORT_INTERFACE __attribute__ ((visibility ("default")))

#if defined(__cplusplus)
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) extern "C" interfacetype rettype funcname(); 
#else
#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) interfacetype rettype funcname(); 
#endif

#define PLATFORM_DLL_EXPORT_FUNC(funcname, libname)
#define PLATFORM_DLL_EXPORT_VAR(variable, libname)  

#endif 
#endif // DllInterface_h
