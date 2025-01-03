///////////////////////////////////////////////////////////////////////////////
// AllocInfo
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Henry Allen
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EA_ALLOCINFO_H
#define EA_ALLOCINFO_H

#include <EABase/eabase.h>
#include <PPMalloc/internal/dllinfo.h>

////////////////////////////////////////////////////////////////////////////////
//EA_ALLOCINFO_ENABLED
// Define EA_ALLOCINFO_ENABLED>0 in order to enable the macros that will pass
// debug parameters. This define should not be used to modify any method
// signatures, object sizes, or anything else that would make code compiled with
// this setting on incompatible with it being off. Therefore it's valid to
// redefine EA_ALLOCINFO_ENABLED as needed in different sections of code
// (or more likely different libraries) within the same build if it's only
// desired to have debug params for some areas.
#if !defined(EA_ALLOCINFO_ENABLED) || (EA_ALLOCINFO_ENABLED < 0)
   #if defined(EA_DEBUG)
	  #define EA_ALLOCINFO_ENABLED (1)
   #else
	  #define EA_ALLOCINFO_ENABLED (0)
   #endif
#endif
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//EA_ALLOCINFO
// Use the EA_ALLOCINFO macro to pass debug params (file, line, and name) to a
// method that accepts an EA::Allocator::AllocInfo parameter. The info will
// automatically be compiled out for non-EA_ALLOCINFO_ENABLED compiles.
//
// Example:
//          myFoo = mAllocator->Alloc(size, flags, EA_ALLOCINFO("Foo"));
//
// If EA_ALLOCINFO_ENABLED, an EA::Allocator::AllocInfo will be initialized with
// "Foo", __LINE__, and __FILE__ and passed to the Alloc call. If
// !EA_ALLOCINFO_ENABLED, EA::Allocator::AllocInfo::NULLREF will instead be
// passed to the Alloc call.
//
// The EA_ALLOCINFO_EXPLICIT macro works like EA_ALLOCINFO except that it uses
// the supplied file and line info instead of the the current file & line. This
// can be useful if you are passsing through to the file and line info from elsewhere.
//
#if EA_ALLOCINFO_ENABLED
	#define EA_ALLOCINFO(_name)                              (EA::Allocator::AllocInfo((_name), __LINE__, __FILE__))
	#define EA_ALLOCINFO_EXPLICIT(_name, _line, _file)       (EA::Allocator::AllocInfo((_name), (_line), (_file)))
#else
	#define EA_ALLOCINFO(_name)                              (EA::Allocator::AllocInfo::NULLREF)
	#define EA_ALLOCINFO_EXPLICIT(_name, _line, _file)       (EA::Allocator::AllocInfo::NULLREF)
#endif
//
////////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace Allocator
	{
		//Debug params that may be included with alloc and other resource aquisition calls.
		// Most users will not need to use objects of this type directly ... use EA_ALLOCINFO macros
		// instead since they will automatically compile out the parameters when !EA_ALLOCINFO_ENABLED
		class PPM_API AllocInfo
		{
			public:
				AllocInfo(const char* const name, const uint32_t lineNum, const char* const fileName) : mName(name), mLineNum(lineNum), mFileName(fileName) {};
				AllocInfo(const AllocInfo& obj) : mName(obj.GetName()), mLineNum(obj.GetLine()), mFileName(obj.GetFileName()) {};

				inline const char*    GetName()     const { return mName; };
				inline       uint32_t GetLine()     const { return mLineNum; };
				inline const char*    GetFileName() const { return mFileName; };
				inline       bool8_t  IsValid()     const { return (this != &NULLREF); };

				//this is public so that we can treat it semantically like an enum or define
				//the main value of this is to prevent un-optimized compiles from having un-due
				//code bloat that would be caused by accessing this via an inline acccessor
				//that didn't actually get inlined
				static const AllocInfo NULLREF;

			private:
				const char* const    mName;
				const uint32_t       mLineNum;
				const char* const    mFileName;

				//private and undefined (members are all const so assignment isn't possible)
				AllocInfo& operator=(const AllocInfo& rhs);
		};

	} //Allocator

} //EA

/******************************************************************************/

#endif    // EA_ALLOCINFO_H

