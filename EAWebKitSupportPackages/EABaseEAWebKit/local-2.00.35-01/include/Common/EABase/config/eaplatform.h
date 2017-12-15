/*
Copyright (C) 2002-2014 Electronic Arts, Inc.  All rights reserved.

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

 /*-----------------------------------------------------------------------------
 * Currently supported platform identification defines include:
 *    EA_PLATFORM_PS3
 *    EA_PLATFORM_PS3_PPU
 *    EA_PLATFORM_PS3_SPU
 *    EA_PLATFORM_KETTLE
 *    EA_PLATFORM_XENON (a.k.a. XBox 360)
 *    EA_PLATFORM_CAPILANO
 *    EA_PLATFORM_APPLE
 *    EA_PLATFORM_OSX
 *    EA_PLATFORM_LINUX
 *    EA_PLATFORM_FREEBOX
 *    EA_PLATFORM_WINDOWS
 *    EA_PLATFORM_WIN32
 *    EA_PLATFORM_WIN64
 *    EA_PLATFORM_WINCE
 *    EA_PLATFORM_POSIX     (pseudo-platform; may be defined along with another platform like EA_PLATFORM_LINUX, EA_PLATFORM_UNIX, EA_PLATFORM_QNX)
 *    EA_PLATFORM_UNIX      (pseudo-platform; may be defined along with another platform like EA_PLATFORM_LINUX)
 *    EA_PLATFORM_CYGWIN    (pseudo-platform; may be defined along with another platform like EA_PLATFORM_LINUX)
 *    EA_PLATFORM_MINGW     (pseudo-platform; may be defined along with another platform like EA_PLATFORM_WINDOWS)
 *    EA_PLATFORM_MICROSOFT (pseudo-platform; may be defined along with another platform like EA_PLATFORM_WINDOWS)
 *
 *    EA_ABI_ARM_LINUX      (a.k.a. "eabi". for all platforms that use the CodeSourcery GNU/Linux toolchain, like Android/Palm Pre/Bada/Airplay)
 *    EA_ABI_ARM_APPLE      (similar to eabi but not identical)
 *    EA_ABI_ARM_WINCE      (similar to eabi but not identical)
 *
 * Other definitions emanated from this file inclue:
 *    EA_PLATFORM_NAME = <string>
 *    EA_PLATFORM_DESCRIPTION = <string>
 *    EA_PROCESSOR_XXX
 *    EA_MISALIGNED_SUPPORT_LEVEL=0|1|2
 *    EA_SYSTEM_LITTLE_ENDIAN | EA_SYSTEM_BIG_ENDIAN
 *    EA_ASM_STYLE_ATT | EA_ASM_STYLE_INTEL | EA_ASM_STYLE_MOTOROLA
 *    EA_PLATFORM_PTR_SIZE = <integer size in bytes>
 *    EA_PLATFORM_WORD_SIZE = <integer size in bytes>
 *
 *---------------------------------------------------------------------------*/

/*
    EA_PLATFORM_MOBILE
    EA_PLATFORM_MOBILE is a peer to EA_PLATORM_DESKTOP and EA_PLATFORM_CONSOLE. Their definition is qualitative rather
    than quantitative, and refers to the general (usually weaker) capabilities of the machine. Mobile devices have a
    similar set of weaknesses that are useful to generally categorize. The primary motivation is to avoid code that
    tests for multiple mobile platforms on a line and needs to be updated every time we get a new one.
    For example, mobile platforms tend to have weaker ARM processors, don't have full multiple processor support,
    are hand-held, don't have mice (though may have touch screens or basic cursor controls), have writable solid
    state permanent storage. Production user code shouldn't have too many expecations about the meaning of this define.

    EA_PLATFORM_DESKTOP
    This is similar to EA_PLATFORM_MOBILE in its qualitative nature and refers to platforms that are powerful.
    For example, they nearly always have virtual memory, mapped memory, hundreds of GB of writable disk sto rage,
    TCP/IP network connections, mice, keyboards, 512+ MB of RAM, multiprocessing, multiple display support.
    Production user code shouldn't have too many expecations about the meaning of this define.

    EA_PLATFORM_CONSOLE
    This is similar to EA_PLATFORM_MOBILE in its qualitative nature and refers to platforms that are consoles.
    This means platforms that are connected to TVs, are fairly powerful (especially graphics-wise), are tightly
    controlled by vendors, tend not to have mapped memory, tend to have TCP/IP, don't have multiple process support
    though they might have multiple CPUs, support TV output only. Production user code shouldn't have too many
    expecations about the meaning of this define.

*/


#ifndef INCLUDED_eaplatform_H
#define INCLUDED_eaplatform_H


// Cygwin
// This is a pseudo-platform which will be defined along with EA_PLATFORM_LINUX when
// using the Cygwin build environment.
#if defined(__CYGWIN__)
    #define EA_PLATFORM_CYGWIN 1
    #define EA_PLATFORM_DESKTOP 1
#endif

// MinGW
// This is a pseudo-platform which will be defined along with EA_PLATFORM_WINDOWS when
// using the MinGW Windows build environment.
#if defined(__MINGW32__) || defined(__MINGW64__)
    #define EA_PLATFORM_MINGW 1
    #define EA_PLATFORM_DESKTOP 1
#endif

#if defined(CS_UNDEFINED_STRING) || defined(EA_PLATFORM_PS4)
	#undef  EA_PLATFORM_PS4
	#undef  CS_UNDEFINED_STRING
	#define EA_PLATFORM_PS4 1
	#define CS_UNDEFINED_STRING 1
    #define EA_PLATFORM_NAME "PS4"
    #define EA_SYSTEM_LITTLE_ENDIAN 1
    #define EA_PLATFORM_DESCRIPTION "PS4 on x64"
    #define EA_PLATFORM_CONSOLE 1
    #define CS_UNDEFINED_STRING 1
    #define EA_PLATFORM_POSIX 1
    #define EA_POSIX_THREADS_AVAILABLE 1
    #define EA_PROCESSOR_X86_64 1
    #if defined(__GNUC__) || defined(__clang__)
        #define EA_ASM_STYLE_ATT 1
    #endif

#elif defined(CS_UNDEFINED_STRING) || defined(EA_PLATFORM_XBOXONE)
    #undef  EA_PLATFORM_XBOXONE
	#undef  CS_UNDEFINED_STRING
	#define EA_PLATFORM_XBOXONE 1
	#define CS_UNDEFINED_STRING 1
	
	#define EA_PLATFORM_NAME "Xbox One"
    //#define EA_PROCESSOR_X86  Currently our policy is that we don't define this, even though x64 is something of a superset of x86.
    #define EA_PROCESSOR_X86_64 1
    #define EA_SYSTEM_LITTLE_ENDIAN 1
    #define EA_PLATFORM_DESCRIPTION "Xbox One on x64"
    #define EA_ASM_STYLE_INTEL 1
    #define EA_PLATFORM_CONSOLE 1
    #define EA_PLATFORM_MICROSOFT 1

    // WINAPI_FAMILY defines - mirrored from winapifamily.h
    #define EA_WINAPI_FAMILY_APP         1
    #define EA_WINAPI_FAMILY_DESKTOP_APP 2
    #define EA_WINAPI_FAMILY_MOBILE_APP  3
    #define EA_WINAPI_FAMILY_TV_APP      4
    #define EA_WINAPI_FAMILY_TV_TITLE    5
    
    #if defined(WINAPI_FAMILY) 
        #include <winapifamily.h>
        #if WINAPI_FAMILY == WINAPI_FAMILY_TV_TITLE
            #define EA_WINAPI_FAMILY EA_WINAPI_FAMILY_TV_TITLE
        #elif WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
            #define EA_WINAPI_FAMILY EA_WINAPI_FAMILY_DESKTOP_APP
        #else
            #error Unsupported WINAPI_FAMILY
        #endif
    #else
        #error WINAPI_FAMILY should always be defined on Capilano.
    #endif

    // Macro to determine if a partition is enabled.
    #define EA_WINAPI_FAMILY_PARTITION(Partition)	(Partition)

    #if EA_WINAPI_FAMILY == EA_WINAPI_FAMILY_DESKTOP_APP
        #define EA_WINAPI_PARTITION_CORE    1
        #define EA_WINAPI_PARTITION_DESKTOP 1
        #define EA_WINAPI_PARTITION_APP     1
    #elif EA_WINAPI_FAMILY == EA_WINAPI_FAMILY_TV_TITLE
        #define EA_WINAPI_PARTITION_CORE     1
        #define EA_WINAPI_PARTITION_TV_TITLE 1
    #else
        #error Unsupported WINAPI_FAMILY
    #endif
#elif defined(EA_PLATFORM_BSD) || (defined(__BSD__) || defined(__FreeBSD__))
    #undef  EA_PLATFORM_BSD
    #define EA_PLATFORM_BSD 1
    #define CS_UNDEFINED_STRING 1
    #define EA_PLATFORM_POSIX 1     // BSD's posix complaince is not identical to Linux's
    #define EA_PLATFORM_NAME "BSD Unix"
    #if defined(__i386__) || defined(__intel__)
        #define EA_PROCESSOR_X86 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "BSD on x86"
    #elif defined(__x86_64__)
        #define EA_PROCESSOR_X86_64 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "BSD on x86-64"
    #elif defined(__powerpc64__)
        #define EA_PROCESSOR_POWERPC 1
        #define EA_PROCESSOR_POWERPC_64 1
        #define EA_SYSTEM_BIG_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "BSD on PowerPC 64"
    #elif defined(__powerpc__)
        #define EA_PROCESSOR_POWERPC 1
        #define EA_PROCESSOR_POWERPC_32 1
        #define EA_SYSTEM_BIG_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "BSD on PowerPC"
    #else
        #error Unknown processor
        #error Unknown endianness
    #endif
    #if !defined(EA_PLATFORM_FREEBSD) && defined(__FreeBSD__)
        #define EA_PLATFORM_FREEBSD 1 // This is a variation of BSD.
    #endif
    #if defined(__GNUC__)
        #define EA_ASM_STYLE_ATT 1
    #endif
    #define EA_PLATFORM_DESKTOP 1

#else
    #undef  EA_PLATFORM_WINDOWS
    #define EA_PLATFORM_WINDOWS 1
    #define EA_PLATFORM_NAME "Windows"
    #ifdef _WIN64 // VC++ defines both _WIN32 and _WIN64 when compiling for Win64.
        #define EA_PLATFORM_WIN64 1
    #else
        #define EA_PLATFORM_WIN32 1
    #endif
    #if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
        #define EA_PROCESSOR_X86_64 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "Windows on X86-64"
    #elif defined(_M_IX86) || defined(_X86_)
        #define EA_PROCESSOR_X86 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "Windows on X86"
    #elif defined(_M_IA64) || defined(_IA64_)
        #define EA_PROCESSOR_IA64 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "Windows on IA-64"
    #elif defined(_M_ARM)
        #define EA_ABI_ARM_WINCE 1
        #define CS_UNDEFINED_STRING 1
        #define EA_SYSTEM_LITTLE_ENDIAN 1
        #define EA_PLATFORM_DESCRIPTION "Windows on ARM"
    #else //Possibly other Windows CE variants
        #error Unknown processor
        #error Unknown endianness
    #endif
    #if defined(__GNUC__)
        #define EA_ASM_STYLE_ATT 1
    #elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL)
        #define EA_ASM_STYLE_INTEL 1
    #endif
    #define EA_PLATFORM_DESKTOP 1
    #define EA_PLATFORM_MICROSOFT 1

    // WINAPI_FAMILY defines to support Windows 8 Metro Apps - mirroring winapifamily.h in the Windows 8 SDK
    #define EA_WINAPI_PARTITION_DESKTOP   0x00000001
    #define EA_WINAPI_PARTITION_APP       0x00000002
    #define EA_WINAPI_FAMILY_APP          EA_WINAPI_PARTITION_APP
    #define EA_WINAPI_FAMILY_DESKTOP_APP  (EA_WINAPI_PARTITION_DESKTOP | EA_WINAPI_PARTITION_APP)

    #if defined(WINAPI_FAMILY)
        #if defined(_MSC_VER)
            #pragma warning(push, 0)
        #endif
        #include <winapifamily.h>
        #if defined(_MSC_VER)
            #pragma warning(pop)
        #endif
        #if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
            #define EA_WINAPI_FAMILY EA_WINAPI_FAMILY_DESKTOP_APP
        #elif WINAPI_FAMILY == WINAPI_FAMILY_APP
            #define EA_WINAPI_FAMILY EA_WINAPI_FAMILY_APP
        #else
            #error Unsupported WINAPI_FAMILY
        #endif
    #else
        #define EA_WINAPI_FAMILY EA_WINAPI_FAMILY_DESKTOP_APP
    #endif

    #define EA_WINAPI_FAMILY_PARTITION(Partition)   ((EA_WINAPI_FAMILY & Partition) == Partition)

#endif



// EA_PLATFORM_PTR_SIZE
// Platform pointer size; same as sizeof(void*).
// This is not the same as sizeof(int), as int is usually 32 bits on
// even 64 bit platforms.
//
// _WIN64 is defined by Win64 compilers, such as VC++.
// _M_IA64 is defined by VC++ and Intel compilers for IA64 processors.
// __LP64__ is defined by HP compilers for the LP64 standard.
// _LP64 is defined by the GCC and Sun compilers for the LP64 standard.
// __ia64__ is defined by the GCC compiler for IA64 processors.
// __arch64__ is defined by the Sparc compiler for 64 bit processors.
// __mips64__ is defined by the GCC compiler for MIPS processors.
// __powerpc64__ is defined by the GCC compiler for PowerPC processors.
// __64BIT__ is defined by the AIX compiler for 64 bit processors.
// __sizeof_ptr is defined by the ARM compiler (armcc, armcpp).
//
#ifndef EA_PLATFORM_PTR_SIZE
    #if defined(__WORDSIZE) // Defined by some variations of GCC.
        #define EA_PLATFORM_PTR_SIZE ((__WORDSIZE) / 8)
    #elif defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__mips64__) || defined(__64BIT__) || defined(__Ptr_Is_64) || defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
        #define EA_PLATFORM_PTR_SIZE 8
    #elif defined(__CC_ARM) && (__sizeof_ptr == 8)
        #define EA_PLATFORM_PTR_SIZE 8
    #elif defined(__CC_ARM) && (__sizeof_ptr == 8)
        #define EA_PLATFORM_PTR_SIZE 8
    #else
        #define EA_PLATFORM_PTR_SIZE 4
    #endif
#endif



// EA_PLATFORM_WORD_SIZE
// This defines the size of a machine word. This will be the same as
// the size of registers on the machine but not necessarily the same
// as the size of pointers on the machine. A number of 64 bit platforms
// have 64 bit registers but 32 bit pointers.
//
#ifndef EA_PLATFORM_WORD_SIZE
   #if defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING)
      #define EA_PLATFORM_WORD_SIZE 8
   #else
      #define EA_PLATFORM_WORD_SIZE EA_PLATFORM_PTR_SIZE
   #endif
#endif


// EA_MISALIGNED_SUPPORT_LEVEL
// Specifies if the processor can read and write built-in types that aren't
// naturally aligned.
//    0 - not supported. Likely causes an exception.
//    1 - supported but slow.
//    2 - supported and fast.
//
#ifndef EA_MISALIGNED_SUPPORT_LEVEL
    #if defined(EA_PROCESSOR_X64) || defined(EA_PROCESSOR_X86_64)
        #define EA_MISALIGNED_SUPPORT_LEVEL 2
    #else
        #define EA_MISALIGNED_SUPPORT_LEVEL 0
    #endif
#endif

// Macro to determine if a Windows API partition is enabled. Always false on non Microsoft platforms.
#if !defined(EA_WINAPI_FAMILY_PARTITION)
    #define EA_WINAPI_FAMILY_PARTITION(Partition) (0)
#endif


#endif // INCLUDED_eaplatform_H








