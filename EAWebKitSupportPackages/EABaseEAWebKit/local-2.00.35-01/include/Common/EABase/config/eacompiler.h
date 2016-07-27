/*
Copyright (C) 2002-2013 Electronic Arts, Inc.  All rights reserved.

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
 * Currently supported defines include:
 *     EA_COMPILER_GNUC
 *     EA_COMPILER_ARM
 *     EA_COMPILER_EDG
 *     EA_COMPILER_SN
 *     EA_COMPILER_MSVC
 *     EA_COMPILER_METROWERKS
 *     EA_COMPILER_INTEL
 *     EA_COMPILER_BORLANDC
 *     EA_COMPILER_IBM
 *     EA_COMPILER_QNX
 *     EA_COMPILER_GREEN_HILLS
 *     EA_COMPILER_CLANG
 *     
 *     EA_COMPILER_VERSION = <integer>
 *     EA_COMPILER_NAME = <string>
 *     EA_COMPILER_STRING = <string>
 *     
 *     EA_COMPILER_NO_STATIC_CONSTANTS
 *     EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
 *     EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
 *     EA_COMPILER_NO_MEMBER_TEMPLATES
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
 *     EA_COMPILER_NO_TEMPLATE_TEMPLATES
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
 *     EA_COMPILER_NO_VOID_RETURNS
 *     EA_COMPILER_NO_COVARIANT_RETURN_TYPE
 *     EA_COMPILER_NO_DEDUCED_TYPENAME
 *     EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
 *     EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
 *     EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
 *     EA_COMPILER_NO_RTTI
 *     EA_COMPILER_NO_EXCEPTIONS
 *     EA_COMPILER_NO_NEW_THROW_SPEC
 *     EA_THROW_SPEC_NEW / EA_THROW_SPEC_DELETE
 *     EA_COMPILER_NO_UNWIND
 *     EA_COMPILER_NO_STANDARD_CPP_LIBRARY
 *     EA_COMPILER_NO_STATIC_VARIABLE_INIT
 *     EA_COMPILER_NO_STATIC_FUNCTION_INIT
 *     EA_COMPILER_NO_VARIADIC_MACROS
 *     EA_COMPILER_NO_RVALUE_REFERENCES
 *     EA_COMPILER_NO_EXTERN_TEMPLATE
 *     EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
 *
 *-----------------------------------------------------------------------------
 *
 * Documentation
 *     EA_COMPILER_NO_STATIC_CONSTANTS
 *         Code such as this is legal, but some compilers fail to compile it:
 *             struct A{ static const a = 1; };
 *
 *     EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
 *         Some compilers fail to allow template specialization, such as with this:
 *             template<class U> void DoSomething(U u);
 *             void DoSomething(int x);
 *
 *     EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
 *         Some compilers fail to allow partial template specialization, such as with this:
 *             template <class T, class Allocator> class vector{ };         // Primary templated class.
 *             template <class Allocator> class vector<bool, Allocator>{ }; // Partially specialized version.
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATES
 *         Some compilers fail to allow member template functions such as this:
 *             struct A{ template<class U> void DoSomething(U u); };
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
 *         Some compilers fail to allow member template specialization, such as with this:
 *             struct A{ 
 *                 template<class U> void DoSomething(U u);
 *                 void DoSomething(int x);
 *             };
 *
 *     EA_COMPILER_NO_TEMPLATE_TEMPLATES
 *         Code such as this is legal:
 *             template<typename T, template<typename> class U>
 *             U<T> SomeFunction(const U<T> x) { return x.DoSomething(); }
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
 *         Some compilers fail to compile templated friends, as with this:
 *             struct A{ template<class U> friend class SomeFriend; };
 *         This is described in the C++ Standard at 14.5.3.
 *
 *     EA_COMPILER_NO_VOID_RETURNS
 *          This is legal C++:
 *              void DoNothing1(){ };
 *              void DoNothing2(){ return DoNothing1(); }
 *
 *     EA_COMPILER_NO_COVARIANT_RETURN_TYPE
 *         See the C++ standard sec 10.3,p5.
 *     
 *     EA_COMPILER_NO_DEDUCED_TYPENAME
 *         Some compilers don't support the use of 'typename' for 
 *         dependent types in deduced contexts, as with this:
 *             template <class T> void Function(T, typename T::type);
 *
 *     EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
 *         Also known as Koenig lookup. Basically, if you have a function
 *         that is a namespace and you call that function without prefixing
 *         it with the namespace the compiler should look at any arguments
 *         you pass to that function call and search their namespace *first* 
 *         to see if the given function exists there.
 *
 *     EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
 *         <exception> is in namespace std. Some std libraries fail to 
 *         put the contents of <exception> in namespace std. The following 
 *         code should normally be legal:
 *             void Function(){ std::terminate(); }
 *
 *     EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
 *         Some compilers fail to execute DoSomething() properly, though they 
 *         succeed in compiling it, as with this:
 *             template <int i>
 *             bool DoSomething(int j){ return i == j; };
 *             DoSomething<1>(2);
 *
 *     EA_COMPILER_NO_EXCEPTIONS
 *         The compiler is configured to disallow the use of try/throw/catch
 *         syntax (often to improve performance). Use of such syntax in this 
 *         case will cause a compilation error. 
 *         
 *     EA_COMPILER_NO_UNWIND
 *         The compiler is configured to allow the use of try/throw/catch 
 *         syntax and behaviour but disables the generation of stack unwinding 
 *         code for responding to exceptions (often to improve performance).
 *
 *---------------------------------------------------------------------------*/

#ifndef INCLUDED_eacompiler_H
#define INCLUDED_eacompiler_H

    #include <EABase/config/eaplatform.h>

    // Note: This is used to generate the EA_COMPILER_STRING macros
    #ifndef INTERNAL_STRINGIZE
        #define INTERNAL_STRINGIZE(x) INTERNAL_PRIMITIVE_STRINGIZE(x)
    #endif
    #ifndef INTERNAL_PRIMITIVE_STRINGIZE
        #define INTERNAL_PRIMITIVE_STRINGIZE(x) #x
    #endif

    // EA_COMPILER_HAS_FEATURE
    #ifndef EA_COMPILER_HAS_FEATURE
        #if defined(__clang__)
            #define EA_COMPILER_HAS_FEATURE(x) __has_feature(x)
        #else
            #define EA_COMPILER_HAS_FEATURE(x) 0
        #endif
    #endif


    // EDG (EDG compiler front-end, used by other compilers such as SN)
    #if defined(__EDG_VERSION__)
        #define EA_COMPILER_EDG 1

        #if defined(_MSC_VER)
            #define EA_COMPILER_EDG_VC_MODE 1
        #endif
        #if defined(__GNUC__)
            #define EA_COMPILER_EDG_GCC_MODE 1
        #endif
    #endif


    // SN


    // Airplay SDK (third party mobile middleware compiler)


    // Green Hills (a.k.a. ghs)
    #if   defined(__ARMCC_VERSION)
        // Note that this refers to the ARM RVCT compiler (armcc or armcpp), but there
        // are other compilers that target ARM processors, such as GCC and Microsoft VC++.
        // If you want to detect compiling for the ARM processor, check for EA_PROCESSOR_ARM
        // being defined.
        // This compiler is also identified by defined(__CC_ARM) || defined(__ARMCC__).
        #define EA_COMPILER_RVCT    1
        #define EA_COMPILER_ARM     1
        #define EA_COMPILER_VERSION __ARMCC_VERSION
        #define EA_COMPILER_NAME    "RVCT"
      //#define EA_COMPILER_STRING (defined below)

    #elif defined(__clang__)
        #define EA_COMPILER_CLANG   1
        #define EA_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
        #define EA_COMPILER_NAME    "clang"
        #define EA_COMPILER_STRING  EA_COMPILER_NAME __clang_version__

    // GCC (a.k.a. GNUC)
    #elif defined(__GNUC__) // GCC compilers exist for many platforms.
        #define EA_COMPILER_GNUC    1
        #define EA_COMPILER_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
        #define EA_COMPILER_NAME    "GCC"
        #define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( __GNUC__ ) "." INTERNAL_STRINGIZE( __GNUC_MINOR__ )

        #if (__GNUC__ == 2) && (__GNUC_MINOR__ < 95) // If GCC < 2.95... 
            #define EA_COMPILER_NO_MEMBER_TEMPLATES 1
        #endif
        #if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 97) // If GCC <= 2.97...
            #define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
        #endif
        #if (__GNUC__ == 3) && ((__GNUC_MINOR__ == 1) || (__GNUC_MINOR__ == 2)) // If GCC 3.1 or 3.2 (but not pre 3.1 or post 3.2)...
            #define EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1
        #endif

    // Borland C++
    #elif defined(__BORLANDC__)
        #define EA_COMPILER_BORLANDC 1
        #define EA_COMPILER_VERSION  __BORLANDC__
        #define EA_COMPILER_NAME     "Borland C"
      //#define EA_COMPILER_STRING (defined below)

        #if (__BORLANDC__ <= 0x0550)      // If Borland C++ Builder 4 and 5...
            #define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
        #endif
        #if (__BORLANDC__ >= 0x561) && (__BORLANDC__ < 0x600)
            #define EA_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1
        #endif


    // Intel C++
    // The Intel Windows compiler masquerades as VC++ and defines _MSC_VER.
    // The Intel compiler is based on the EDG compiler front-end.
    #elif defined(__ICL) || defined(__ICC)
        #define EA_COMPILER_INTEL 1

        // Should we enable the following? We probably should do so since enabling it does a lot more good than harm
        // for users. The Intel Windows compiler does a pretty good job of emulating VC++ and so the user would likely
        // have to handle few special cases where the Intel compiler doesn't emulate VC++ correctly.
        #if defined(_MSC_VER)
            #define EA_COMPILER_MSVC 1
            #define EA_COMPILER_MICROSOFT 1
        #endif

        // Should we enable the following? This isn't as clear because as of this writing we don't know if the Intel 
        // compiler truly emulates GCC well enough that enabling this does more good than harm.
        #if defined(__GNUC__)
            #define EA_COMPILER_GNUC 1
        #endif

        #if defined(__ICL)
            #define EA_COMPILER_VERSION __ICL
        #elif defined(__ICC)
            #define EA_COMPILER_VERSION __ICC
        #endif
        #define EA_COMPILER_NAME "Intel C++"
        #if defined(_MSC_VER)
            #define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", VC++ version " INTERNAL_STRINGIZE( _MSC_VER )
        #elif defined(__GNUC__)
            #define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", GCC version " INTERNAL_STRINGIZE( __GNUC__ )
        #else
            #define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ )
        #endif

    // Metrowerks
    #elif defined(_MSC_VER) && !(defined(CS_UNDEFINED_STRING) && defined(__arm__)) // S3E is a mobile SDK which mistakenly masquerades as VC++ on ARM.
        #define EA_COMPILER_MSVC 1
        #define EA_COMPILER_MICROSOFT 1
        #define EA_COMPILER_VERSION _MSC_VER
        #define EA_COMPILER_NAME "Microsoft Visual C++"
      //#define EA_COMPILER_STRING (defined below)

            #define EA_STANDARD_LIBRARY_MSVC 1
            #define EA_STANDARD_LIBRARY_MICROSOFT 1

        #if (_MSC_VER <= 1200) // If VC6.x and earlier...
            #if (_MSC_VER < 1200)
                #define EA_COMPILER_MSVCOLD 1
            #else
                #define EA_COMPILER_MSVC6 1
            #endif

            #if (_MSC_VER < 1200) // If VC5.x or earlier...
                #define EA_COMPILER_NO_TEMPLATE_SPECIALIZATION 1
            #endif
            #define EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1     // The compiler compiles this OK, but executes it wrong. Fixed in VC7.0
            #define EA_COMPILER_NO_VOID_RETURNS 1                             // The compiler fails to compile such cases. Fixed in VC7.0
            #define EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE 1                  // The compiler fails to compile such cases. Fixed in VC7.0
            #define EA_COMPILER_NO_DEDUCED_TYPENAME 1                         // The compiler fails to compile such cases. Fixed in VC7.0
            #define EA_COMPILER_NO_STATIC_CONSTANTS 1                         // The compiler fails to compile such cases. Fixed in VC7.0
            #define EA_COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
            #define EA_COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
            //#define EA_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?
            //#define EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION 1         // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?

        #elif (_MSC_VER <= 1300) // If VC7.0 and earlier...
            #define EA_COMPILER_MSVC7 1

            #define EA_COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
            #define EA_COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
            #define EA_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1           // This is the case only for VC7.0 and not VC6 or VC7.1+. Fixed in VC7.1
            //#define EA_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC7.0 supports member templates properly 95% of the time. So do we flag the remaining 5%?

        #elif (_MSC_VER < 1400) // If VC7.1... 
            // The VC7.1 and later compiler is fairly close to the C++ standard 
            // and thus has no compiler limitations that we are concerned about.
            #define EA_COMPILER_MSVC7_2003 1
            #define EA_COMPILER_MSVC7_1    1

        #elif (_MSC_VER < 1500) // If VS2005... _MSC_VER of 1400 means VC8 (VS2005)
            #define EA_COMPILER_MSVC8_2005 1
            #define EA_COMPILER_MSVC8_0    1

        #elif (_MSC_VER < 1600) // If VS2008... _MSC_VER of 1500 means VC9 (VS2008)
            #define EA_COMPILER_MSVC9_2008 1
            #define EA_COMPILER_MSVC9_0    1

        #elif (_MSC_VER < 1700) // VS2010       _MSC_VER of 1600 means VC10 (VS2010)
            #define EA_COMPILER_MSVC_2010 1
            #define EA_COMPILER_MSVC10_0  1

        #else // VS2011                         _MSC_VER of 1700 means VC11 (VS2011)
            #define EA_COMPILER_MSVC_2011 1
            #define EA_COMPILER_MSVC11_0  1

        #endif


    // IBM
    #elif defined(__xlC__)
        #define EA_COMPILER_IBM     1
        #define EA_COMPILER_NAME    "IBM XL C"
        #define EA_COMPILER_VERSION __xlC__
        #define EA_COMPILER_STRING "IBM XL C compiler, version " INTERNAL_STRINGIZE( __xlC__ )

    // Unknown
    #else // Else the compiler is unknown

        #define EA_COMPILER_VERSION 0
        #define EA_COMPILER_NAME   "Unknown"

    #endif

    #ifndef EA_COMPILER_STRING
        #define EA_COMPILER_STRING EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(EA_COMPILER_VERSION)
    #endif


    // Deprecated definitions
    // For backwards compatibility, should be supported for at least the life of EABase v2.0.x.
    #ifndef EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
        #define EA_COMPILER_PARTIAL_TEMPLATE_SPECIALIZATION 1
    #endif
    #ifndef EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
        #define EA_COMPILER_TEMPLATE_SPECIALIZATION 1
    #endif
    #ifndef EA_COMPILER_NO_MEMBER_TEMPLATES
        #define EA_COMPILER_MEMBER_TEMPLATES 1
    #endif
    #ifndef EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
        #define EA_COMPILER_MEMBER_TEMPLATE_SPECIALIZATION 1
    #endif



    // EA_COMPILER_NO_RTTI
    //
    // If EA_COMPILER_NO_RTTI is defined, then RTTI (run-time type information)
    // is not available (possibly due to being disabled by the user).
    //
    #if defined(__EDG_VERSION__) && !defined(__RTTI)
        #define EA_COMPILER_NO_RTTI 1
    #elif defined(__clang__) && !EA_COMPILER_HAS_FEATURE(cxx_rtti)
        #define EA_COMPILER_NO_RTTI 1
    #elif defined(__IBMCPP__) && !defined(__RTTI_ALL__)
        #define EA_COMPILER_NO_RTTI 1
    #elif defined(__GXX_ABI_VERSION) && !defined(__GXX_RTTI)
        #define EA_COMPILER_NO_RTTI 1
    #elif defined(_MSC_VER) && !defined(_CPPRTTI)
        #define EA_COMPILER_NO_RTTI 1
    #elif defined(__ARMCC_VERSION) && defined(__TARGET_CPU_MPCORE) && !defined(__RTTI)
        #define EA_COMPILER_NO_RTTI 1
    #endif



    // EA_COMPILER_NO_EXCEPTIONS / EA_COMPILER_NO_UNWIND
    //
    // If EA_COMPILER_NO_EXCEPTIONS is defined, then the compiler is 
    // configured to not recognize C++ exception-handling statements 
    // such as try/catch/throw. Thus, when EA_COMPILER_NO_EXCEPTIONS is 
    // defined, code that attempts to use exception handling statements
    // will usually cause a compilation error. If is often desirable
    // for projects to disable exception handling because exception 
    // handling causes extra code and/or data generation which might
    // not be needed, especially if it is known that exceptions won't
    // be happening. When writing code that is to be portable between
    // systems of which some enable exception handling while others
    // don't, check for EA_COMPILER_NO_EXCEPTIONS being defined. 
    //
    #if !defined(EA_COMPILER_NO_EXCEPTIONS) && !defined(EA_COMPILER_NO_UNWIND)
        #if defined(EA_COMPILER_GNUC) && defined(_NO_EX) // GCC on some platforms (e.g. PS3) defines _NO_EX when exceptions are disabled.
            #define EA_COMPILER_NO_EXCEPTIONS 1

        #elif (defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_INTEL) || defined(EA_COMPILER_RVCT)) && !defined(__EXCEPTIONS) // GCC and most EDG-based compilers define __EXCEPTIONS when exception handling is enabled.
            #define EA_COMPILER_NO_EXCEPTIONS 1

        #elif (defined(EA_COMPILER_BORLAND) || defined(EA_COMPILER_MSVC)) && !defined(_CPPUNWIND)
            #define EA_COMPILER_NO_UNWIND 1

        #endif // EA_COMPILER_NO_EXCEPTIONS / EA_COMPILER_NO_UNWIND
    #endif // !defined(EA_COMPILER_NO_EXCEPTIONS) && !defined(EA_COMPILER_NO_UNWIND)


    // EA_COMPILER_NO_NEW_THROW_SPEC / EA_THROW_SPEC_NEW / EA_THROW_SPEC_DELETE
    //
    // If defined then the compiler's version of operator new is not decorated
    // with a throw specification. This is useful for us to know because we 
    // often want to write our own overloaded operator new implementations.
    // We needs such operator new overrides to be declared identically to the
    // way the compiler is defining operator new itself.
    //
    // Example usage:
    //      void* operator new(std::size_t) EA_THROW_SPEC_NEW(std::bad_alloc);
    //      void* operator new[](std::size_t) EA_THROW_SPEC_NEW(std::bad_alloc);
    //      void* operator new(std::size_t, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE();
    //      void* operator new[](std::size_t, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE();
    //      void  operator delete(void*) EA_THROW_SPEC_DELETE_NONE();
    //      void  operator delete[](void*) EA_THROW_SPEC_DELETE_NONE();
    //      void  operator delete(void*, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE();
    //      void  operator delete[](void*, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE();
    //
    #if defined(EA_COMPILER_NO_EXCEPTIONS) && (!defined(CS_UNDEFINED_STRING) || defined(_MSL_NO_THROW_SPECS)) && !defined(EA_COMPILER_RVCT)
        #define EA_COMPILER_NO_NEW_THROW_SPEC 1

        #define EA_THROW_SPEC_NEW(x)
        #define EA_THROW_SPEC_NEW_NONE()
        #define EA_THROW_SPEC_DELETE_NONE()
    #else
        #define EA_THROW_SPEC_NEW(x)        throw(x)
        #define EA_THROW_SPEC_NEW_NONE()    throw()
        #define EA_THROW_SPEC_DELETE_NONE() throw()
    #endif


    // EA_COMPILER_NO_STANDARD_CPP_LIBRARY
    //
    // If defined, then the compiler doesn't provide a Standard C++ library.
    //


    // EA_COMPILER_NO_STATIC_VARIABLE_INIT
    //
    // If defined, it means that global or static C++ variables will be 
    // constructed. Not all compiler/platorm combinations support this. 
    // User code that needs to be portable must avoid having C++ variables
    // that construct before main. 
    //
    //#if defined(EA_PLATFORM_MOBILE)
    //    #define EA_COMPILER_NO_STATIC_VARIABLE_INIT 1
    //#endif


    // EA_COMPILER_NO_STATIC_FUNCTION_INIT
    //
    // If defined, it means that functions marked as startup functions
    // (e.g. __attribute__((constructor)) in GCC) are supported. It may
    // be that some compiler/platform combinations don't support this.
    //
    //#if defined(XXX) // So far, all compiler/platforms we use support this.
    //    #define EA_COMPILER_NO_STATIC_VARIABLE_INIT 1
    //#endif

    // EA_COMPILER_NO_VARIADIC_MACROS
    // 
    // If defined, the compiler doesn't support C99/C++11 variadic macros.
    // With a variadic macro, you can do this:
    //     #define MY_PRINTF(format, ...) printf(format, __VA_ARGS__)
    //
    #if !defined(EA_COMPILER_NO_VARIADIC_MACROS)
        #if defined(_MSC_VER) && (_MSC_VER < 1500) // If earlier than VS2008..
            #define EA_COMPILER_NO_VARIADIC_MACROS 1
        #elif defined(EA_COMPILER_EDG) // Includes other compilers such as CS_UNDEFINED_STRING, CS_UNDEFINED_STRING, etc.
            // variadic macros are supported
        #elif defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__)) < 401 // If earlier than GCC 4.1..
            #define EA_COMPILER_NO_VARIADIC_MACROS 1
        #endif
    #endif


    // EA_COMPILER_NO_RVALUE_REFERENCES
    // 
    // If defined, the compiler doesn't fully support C++11 rvalue reference semantics.
    // This applies to the compiler only and not the Standard Library in use with the compiler,
    // which is required by the Standard to have some support itself.
    //
    #if !defined(EA_COMPILER_NO_RVALUE_REFERENCES)
        #if defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER >= 1600)              // VS2010+
            // supported
        #elif defined(__cplusplus) && defined(__clang__) && EA_COMPILER_HAS_FEATURE(cxx_rvalue_references)
            // supported
        #elif defined(__cplusplus) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005) && (defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)) // GCC 4.5+
            // supported
        #else
            #define EA_COMPILER_NO_RVALUE_REFERENCES 1
        #endif
    #endif


    // EA_COMPILER_NO_EXTERN_TEMPLATE
    // 
    // If defined, the compiler doesn't support C++11 extern template.
    // With extern templates, you can do this:
    //     extern template void DoSomething(KnownType u);
    //
    #if !defined(EA_COMPILER_NO_EXTERN_TEMPLATE)
        #if defined(__cplusplus) && defined(__clang__) && !defined(CS_UNDEFINED_STRING)            // Clang other than Apple's Clang
            // Extern template is supported.
        #elif defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER >= 1700)            // VC++ >= VS2012...
            // Extern template is supported.
        #elif defined(__cplusplus) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006) && (defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)) // GCC 4.6+
            // Extern template is supported.
        #else
            #define EA_COMPILER_NO_EXTERN_TEMPLATE
        #endif
    #endif


    // EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
    // 
    // If defined, the compiler doesn't support C++11 range-based for loops.
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2930.html
    // You must #include <iterator> for range-based for loops to work.
    // Example usage:
    //    #include <iterator>
    //    #include <vector>
    //    std::vector<float> floatVector;
    //    for(auto& f : floatVector)
    //        f += 1.0;
    //
    #if !defined(EA_COMPILER_NO_RANGE_BASED_FOR_LOOP)
        #if defined(__cplusplus) && (defined(__clang__)  && (EA_COMPILER_VERSION >=  300))  // Clang 3.x+
            // supported
        #elif defined(__cplusplus) && (defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700))  // VC++ >= VS2012...
            // supported
        #elif defined(__cplusplus) && (defined(__GNUC__)  && (EA_COMPILER_VERSION >= 4006) && (defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)))  // GCC 4.6+
            // supported
        #elif defined(__cplusplus) && (__cplusplus >= 201103L)
            // supported
        #else
            #define EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
        #endif
    #endif


#endif // INCLUDED_eacompiler_H





