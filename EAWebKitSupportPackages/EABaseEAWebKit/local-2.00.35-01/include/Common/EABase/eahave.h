/*
Copyright (C) 2011-2013 Electronic Arts, Inc.  All rights reserved.

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
    This file's functionality is preliminary and won't be considered stable until 
    a future EABase version.
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
    This header identifies if the given facilities are available in the 
    standard build environment the current compiler/linker/standard library/
    operating system combination. This file may in some cases #include standard
    headers in order to make availability determinations, such as to check 
    compiler or SDK version numbers. However, it cannot be perfect.
    This header does not identify compiler features, as those are defined in 
    eacompiler.h and eacompilertraits.h. Rather this header is about library support.
    This header does not identify platform or library conventions either, such
    as whether the file paths use \ or / for directory separators.

    We provide three types of HAVE features here:

        - EA_HAVE_XXX_FEATURE - Have compiler feature. 
          Identifies if the compiler has or lacks some feature in the 
          current build. Sometimes you need to check to see if the 
          compiler is running in some mode in able to write portable code
          against it. For example, some compilers (e.g. VC++) have a 
          mode in which all language extensions are disabled. If you want
          to write code that works with that but still uses the extensions
          when available then you can check #if defined(EA_HAVE_EXTENSIONS_FEATURE).
          Features can be forcibly cancelled via EA_NO_HAVE_XXX_FEATURE.
          EA_NO_HAVE is useful for a build system or user to override the 
          defaults because it happens to know better.

        - EA_HAVE_XXX_H - Have header file information. 
          Identifies if a given header file is available to the current 
          compile configuration. For example, some compilers provide a 
          malloc.h header, while others don't. For the former we define 
          EA_HAVE_MALLOC_H, while for the latter it remains undefined.
          If a header is missing then it may still be that the functions
          the header usually declares are declared in some other header.
          EA_HAVE_XXX does not include the possibility that our own code 
          provides versions of these headers, and in fact a purpose of 
          EA_HAVE_XXX is to decide if we should be using our own because
          the system doesn't provide one.
          Header availability can be forcibly cancelled via EA_NO_HAVE_XXX_H.
          EA_NO_HAVE is useful for a build system or user to override the 
          defaults because it happens to know better.

        - EA_HAVE_XXX_DECL - Have function declaration information. 
          Identifies if a given function declaration is provided by 
          the current compile configuration. For example, some compiler
          standard libraries declare a wcslen function, while others
          don't. For the former we define EA_HAVE_WCSLEN_DECL, while for
          the latter it remains undefined. If a declaration of a function
          is missing then we assume the implementation is missing as well.
          EA_HAVE_XXX_DECL does not include the possibility that our 
          own code provides versions of these declarations, and in fact a 
          purpose of EA_HAVE_XXX_DECL is to decide if we should be using 
          our own because the system doesn't provide one.
          Declaration availability can be forcibly cancelled via EA_NO_HAVE_XXX_DECL.
          EA_NO_HAVE is useful for a build system or user to override the 
          defaults because it happens to know better.

        - EA_HAVE_XXX_IMPL - Have function implementation information. 
          Identifies if a given function implementation is provided by
          the current compile and link configuration. For example, it's
          commonly the case that console platforms (e.g. Playstation)
          declare a getenv function but don't provide a linkable implementation.
          In this case the user needs to provide such a function manually
          as part of the link. If the implementation is available then
          we define EA_HAVE_GETENV_IMPL, otherwise it remains undefined.
          Beware that sometimes a function may not seem to be present in 
          the Standard Library but in reality you need to link some auxiliary
          provided library for it. An example of this is the Unix real-time
          functions such as clock_gettime.
          EA_HAVE_XXX_IMPL does not include the possibility that our 
          own code provides versions of these implementations, and in fact a 
          purpose of EA_HAVE_XXX_IMPL is to decide if we should be using 
          our own because the system doesn't provide one.
          Implementation availability can be forcibly cancelled via EA_NO_HAVE_XXX_IMPL.
          EA_NO_HAVE is useful for a build system or user to override the 
          defaults because it happens to know better.

    It's not practical to define EA_HAVE macros for every possible header,
    declaration, and implementation, and so the user must simply know that
    some headers, declarations, and implementations tend to require EA_HAVE
    checking. Nearly every C Standard Library we've seen has a <string.h> 
    header, a strlen declaration, and a linkable strlen implementation, 
    so there's no need to provide EA_HAVE support for this. On the other hand
    it's commonly the case that the C Standard Library doesn't have a malloc.h
    header or an inet_ntop declaration.

---------------------------------------------------------------------------*/


#ifndef INCLUDED_eahave_H
#define INCLUDED_eahave_H


#include <EABase/eabase.h>


/* EA_HAVE_XXX_FEATURE */

#if !defined(EA_HAVE_EXTENSIONS_FEATURE) && !defined(EA_NO_HAVE_EXTENSIONS_FEATURE)
    /* To do: Find a way to detect this automatically for various compilers. */
        #define EA_HAVE_EXTENSIONS_FEATURE 1
#endif


/* EA_HAVE_XXX_LIBRARY */

#if !defined(EA_HAVE_DINKUMWARE_LIBRARY) && !defined(EA_NO_HAVE_DINKUMWARE_LIBRARY)
    #if defined(_YVALS) /* If using the Dinkumware Standard library... */
        #define EA_HAVE_DINKUMWARE_LIBRARY 1
    #else
        #define EA_NO_HAVE_DINKUMWARE_LIBRARY 1
    #endif
#endif

#if !defined(EA_HAVE_LIBSTDCPP_LIBRARY) && !defined(EA_NO_HAVE_LIBSTDCPP_LIBRARY)
    #if defined(__GLIBCXX__) /* If using libstdc++ ... */
        #define EA_HAVE_LIBSTDCPP_LIBRARY 1
    #else
        #define EA_NO_HAVE_LIBSTDCPP_LIBRARY 1
    #endif
#endif


/* EA_HAVE_XXX_H */

#if !defined(EA_HAVE_SYS_TYPES_H) && !defined(EA_NO_HAVE_SYS_TYPES_H)
        #define EA_HAVE_SYS_TYPES_H 1     /* <sys/types.h> */
#endif

#if !defined(EA_HAVE_IO_H) && !defined(EA_NO_HAVE_IO_H)
    #if (defined(EA_PLATFORM_DESKTOP) || defined(CS_UNDEFINED_STRING)) && !defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_IO_H 1            /* <io.h> */
    #else
        #define EA_NO_HAVE_IO_H 1
    #endif
#endif

#if !defined(EA_HAVE_INTTYPES_H) && !defined(EA_NO_HAVE_INTTYPES_H)
        #define EA_NO_HAVE_INTTYPES_H 1
#endif

#if !defined(EA_HAVE_UNISTD_H) && !defined(EA_NO_HAVE_UNISTD_H)
        #define EA_NO_HAVE_UNISTD_H 1
#endif

#if !defined(EA_HAVE_SYS_TIME_H) && !defined(EA_NO_HAVE_SYS_TIME_H)
        #define EA_NO_HAVE_SYS_TIME_H 1
#endif

#if !defined(EA_HAVE_SYS_PTRACE_H) && !defined(EA_NO_HAVE_SYS_PTRACE_H)
        #define EA_NO_HAVE_SYS_PTRACE_H 1
#endif

#if !defined(EA_HAVE_SYS_STAT_H) && !defined(EA_NO_HAVE_SYS_STAT_H)
    #if (defined(CS_UNDEFINED_STRING) && !(defined(CS_UNDEFINED_STRING) && defined(EA_PLATFORM_CONSOLE))) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_SYS_STAT_H 1 /* declares the stat struct and function */
    #else
        #define EA_NO_HAVE_SYS_STAT_H 1
    #endif
#endif

#if !defined(EA_HAVE_LOCALE_H) && !defined(EA_NO_HAVE_LOCALE_H)
        #define EA_HAVE_LOCALE_H 1
#endif

#if !defined(EA_HAVE_DIRENT_H) && !defined(EA_NO_HAVE_DIRENT_H)
        #define EA_NO_HAVE_DIRENT_H 1
#endif

#if !defined(EA_HAVE_SIGNAL_H) && !defined(EA_NO_HAVE_SIGNAL_H)
    #if 1  /* So far all our platforms have signal.h, though they don't all have equal implementations. */
        #define EA_HAVE_SIGNAL_H 1    /* <signal.h> */
    #else
        #define EA_NO_HAVE_SIGNAL_H 1
    #endif
#endif

#if !defined(EA_HAVE_PTHREAD_H) && !defined(EA_NO_HAVE_PTHREAD_H)
        #define EA_NO_HAVE_PTHREAD_H 1
#endif

#if !defined(EA_HAVE_WCHAR_H) && !defined(EA_NO_HAVE_WCHAR_H)
        #define EA_NO_HAVE_WCHAR_H 1
#endif

#if !defined(EA_HAVE_MALLOC_H) && !defined(EA_NO_HAVE_MALLOC_H)
    #if defined(_MSC_VER) || defined(__MINGW32__)
        #define EA_HAVE_MALLOC_H 1
    #else
        #define EA_NO_HAVE_MALLOC_H 1
    #endif
#endif

#if !defined(EA_HAVE_ALLOCA_H) && !defined(EA_NO_HAVE_ALLOCA_H)
    #if (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(CS_UNDEFINED_STRING) || defined(__EDG__)
        #define EA_HAVE_ALLOCA_H 1
    #else
        #define EA_NO_HAVE_ALLOCA_H 1
    #endif
#endif

#if !defined(EA_HAVE_EXECINFO_H) && !defined(EA_NO_HAVE_EXECINFO_H)
    #if defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_EXECINFO_H 1 /* Identifies if <execinfo.h> is available. */
    #else
        #define EA_NO_HAVE_EXECINFO_H 1
    #endif
#endif

#if !defined(EA_HAVE_SEMAPHORE_H) && !defined(EA_NO_HAVE_SEMAPHORE_H)
        #define EA_NO_HAVE_SEMAPHORE_H 1
#endif

#if !defined(EA_HAVE_DIRENT_H) && !defined(EA_NO_HAVE_DIRENT_H)
        #define EA_NO_HAVE_DIRENT_H 1
#endif






/* EA_HAVE_XXX_DECL */

#if !defined(EA_HAVE_mkstemps_DECL) && !defined(EA_NO_HAVE_mkstemps_DECL)
    #if defined(EA_PLATFORM_APPLE) || defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_mkstemps_DECL 1
    #else
        #define EA_NO_HAVE_mkstemps_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_gettimeofday_DECL) && !defined(EA_NO_HAVE_gettimeofday_DECL)
    #if defined(EA_PLATFORM_POSIX) /* Posix means Linux, Unix, and Macintosh OSX, among others (including Linux-based mobile platforms). */
        #define EA_HAVE_gettimeofday_DECL 1
    #else
        #define EA_NO_HAVE_gettimeofday_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_strcasecmp_DECL) && !defined(EA_NO_HAVE_strcasecmp_DECL)
        #define EA_HAVE_stricmp_DECL  1
        #define EA_HAVE_strnicmp_DECL 1
#endif

#if !defined(EA_HAVE_mmap_DECL) && !defined(EA_NO_HAVE_mmap_DECL)
    #if defined(EA_PLATFORM_POSIX)
        #define EA_HAVE_mmap_DECL 1 /* mmap functionality varies significantly between systems. */
    #else
        #define EA_NO_HAVE_mmap_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_fopen_DECL) && !defined(EA_NO_HAVE_fopen_DECL)
        #define EA_HAVE_fopen_DECL 1 /* C FILE functionality such as fopen */
#endif

#if !defined(EA_HAVE_ISNAN) && !defined(EA_NO_HAVE_ISNAN)
        #define EA_HAVE_ISNAN(x)  _isnan(x)          /* declared in <math.h> */
        #define EA_HAVE_ISINF(x)  !_finite(x)
#endif

#if !defined(EA_HAVE_itoa_DECL) && !defined(EA_NO_HAVE_itoa_DECL)
    #if defined(EA_COMPILER_MSVC) && !defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_itoa_DECL 1
    #else
        #define EA_NO_HAVE_itoa_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_nanosleep_DECL) && !defined(EA_NO_HAVE_nanosleep_DECL)
    #if (defined(CS_UNDEFINED_STRING) && !defined(CS_UNDEFINED_STRING)) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING) || defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_nanosleep_DECL 1
    #else
        #define EA_NO_HAVE_nanosleep_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_utime_DECL) && !defined(EA_NO_HAVE_utime_DECL)
        #define EA_HAVE_utime_DECL _utime
#endif

#if !defined(EA_HAVE_ftruncate_DECL) && !defined(EA_NO_HAVE_ftruncate_DECL)
    #if !defined(CS_UNDEFINED_STRING) && !defined(__MINGW32__)
        #define EA_HAVE_ftruncate_DECL 1
    #else
        #define EA_NO_HAVE_ftruncate_DECL 1
    #endif
#endif

#if !defined(EA_HAVE_localtime_DECL) && !defined(EA_NO_HAVE_localtime_DECL)
        #define EA_HAVE_localtime_DECL 1
#endif

#if !defined(EA_HAVE_pthread_getattr_np_DECL) && !defined(EA_NO_HAVE_pthread_getattr_np_DECL)
        #define EA_NO_HAVE_pthread_getattr_np_DECL 1
#endif



/* EA_HAVE_XXX_IMPL*/

#if !defined(EA_HAVE_WCHAR_IMPL) && !defined(EA_NO_HAVE_WCHAR_IMPL)
    #if defined(EA_PLATFORM_DESKTOP) 
        #define EA_HAVE_WCHAR_IMPL 1      /* Specifies if wchar_t string functions are provided, such as wcslen, wprintf, etc. */
    #else
        #define EA_NO_HAVE_WCHAR_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_getenv_IMPL) && !defined(EA_NO_HAVE_getenv_IMPL)
    #if defined(EA_PLATFORM_DESKTOP) || defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_getenv_IMPL 1
        #define EA_HAVE_putenv_IMPL 1
        #define EA_HAVE_unsetenv_IMPL 1
    #else
        #define EA_NO_HAVE_getenv_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_time_IMPL) && !defined(EA_NO_HAVE_time_IMPL)
        #define EA_HAVE_time_IMPL 1
        #define EA_HAVE_clock_IMPL 1
#endif

#if !defined(EA_HAVE_fopen_IMPL) && !defined(EA_NO_HAVE_fopen_IMPL)
        #define EA_HAVE_fopen_IMPL 1  /* C FILE functionality such as fopen */
#endif

#if !defined(EA_HAVE_inet_ntop_IMPL) && !defined(EA_NO_HAVE_inet_ntop_IMPL)
        #define EA_HAVE_inet_ntop_IMPL 1
        #define EA_HAVE_inet_pton_IMPL 1
#endif

#if !defined(EA_HAVE_clock_gettime_IMPL) && !defined(EA_NO_HAVE_clock_gettime_IMPL)
    #if defined(CS_UNDEFINED_STRING) || defined(__CYGWIN__) || (_POSIX_TIMERS > 0) || (defined(EA_PLATFORM_POSIX) && defined(_YVALS) /*Dinkumware*/)
        #define EA_HAVE_clock_gettime_IMPL 1 /* You need to link the 'rt' library to get this */
    #else
        #define EA_NO_HAVE_clock_gettime_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_getcwd_IMPL) && !defined(EA_NO_HAVE_getcwd_IMPL)
    #if (defined(EA_PLATFORM_DESKTOP) || defined(CS_UNDEFINED_STRING)) && !defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_getcwd_IMPL 1
    #else
        #define EA_NO_HAVE_getcwd_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_tmpnam_IMPL) && !defined(EA_NO_HAVE_tmpnam_IMPL)
    #if (defined(EA_PLATFORM_DESKTOP) || defined(CS_UNDEFINED_STRING)) && !defined(CS_UNDEFINED_STRING)
        #define EA_HAVE_tmpnam_IMPL 1
    #else
        #define EA_NO_HAVE_tmpnam_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_nullptr_IMPL) && !defined(EA_NO_HAVE_nullptr_IMPL)
    #if (defined(_MSC_VER) && (_MSC_VER >= 1600)) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)) || defined(__clang__) /* VS2010+, GCC 4.6+, clang */
        /* Note that <EABase/nullptr.h> implements a portable nullptr implementation. */
        #define EA_HAVE_nullptr_IMPL 1 /* C++ nullptr is a built-in type and doesn't come from any header file */
    #else
        #define EA_NO_HAVE_nullptr_IMPL 1
    #endif
#endif

#if !defined(EA_HAVE_std_terminate) && !defined(EA_NO_HAVE_std_terminate)
        #define EA_HAVE_std_terminate 1 /* iOS doesn't appear to provide an implementation for std::terminate under the armv6 target. */
#endif


/* Implementations that all platforms seem to have: */
/*
    alloca
    malloc
    calloc
    strtoll
    strtoull
    vsprintf
    vsnprintf
*/

/* Implementations that we don't care about: */
/*
    bcopy   -- Just use memmove or some customized equivalent. bcopy offers no practical benefit.
    strlcpy -- So few platforms have this built-in that we get no benefit from using it. Use EA::StdC::Strlcpy instead.
    strlcat -- "
*/



/*-----------------------------------------------------------------------------
    EABASE_USER_HAVE_HEADER
    
    This allows the user to define a header file to be #included after the 
    eahave.h's contents are compiled. A primary use of this is to override
    the contents of this header file. You can define the overhead header 
    file name in-code or define it globally as part of your build file.
    
    Example usage:
       #define EABASE_USER_HAVE_HEADER "MyHaveOverrides.h" 
       #include <EABase/eahave.h>
---------------------------------------------------------------------------*/

#ifdef EABASE_USER_HAVE_HEADER
    #include EABASE_USER_HAVE_HEADER
#endif


#endif /* Header include guard */



