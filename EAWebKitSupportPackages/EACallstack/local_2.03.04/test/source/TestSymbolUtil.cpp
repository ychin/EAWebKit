///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif

struct GCCDemangleTest
{
	const char* mpMangled;
	const char* mpDemangled;
	const char* mpDemangledBrief;
};

#if EACALLSTACK_GCC_DEMANGLE_ENABLED
// These test cases are taken from:
//     http://www.codesourcery.com/public/cxx-abi/abi-examples.html#mangling
//     http://sourceware.org/ml/src-cvs/2008-q3/msg00017.html
//     
const GCCDemangleTest gGCCDemangleTestArray[] = 
{
	{ "St9bad_alloc", "std::bad_alloc", "std::bad_alloc" },
	{ "_ZN1f1fE", "f::f", "f::f" },
	{ "_Z1fv", "f()", "f" },
	{ "_Z1fi", "f(int)", "f" },
	{ "_Z3foo3bar", "foo(bar)", "foo" },
	{ "_Zrm1XS_", "operator%(X, X)", "operator%" },
	{ "_ZplR1XS0_", "operator+(X&, X&)", "operator+" },
	{ "_ZlsRK1XS1_", "operator<<(X const&, X const&)", "operator<<" },
	{ "_ZN3FooIA4_iE3barE", "Foo<int [4]>::bar", "Foo<int [4]>::bar" },
	{ "_Z1fIiEvi", "void f<int>(int)", "f<int>" },
	{ "_Z5firstI3DuoEvS0_", "void first<Duo>(Duo)", "first<Duo>" },
	{ "_Z5firstI3DuoEvT_", "void first<Duo>(Duo)", "first<Duo>" },
	{ "_Z3fooIiFvdEiEvv", "void foo<int, void (double), int>()", "foo<int, void ()(double), int>" },
	{ "_ZN1N1fE", "N::f", "N::f" },
	{ "_ZN6System5Sound4beepEv", "System::Sound::beep()", "System::Sound::beep" },
	{ "_ZN5Arena5levelE", "Arena::level", "Arena::level" },
	{ "_ZN5StackIiiE5levelE", "Stack<int, int>::level", "Stack<int, int>::level" },
	{ "_Z1fI1XEvPVN1AIT_E1TE", "void f<X>(A<X>::T volatile*)", "f<X>" },
	{ "_ZngILi42EEvN1AIXplT_Li2EEE1TE", "void operator-<42>(A<(42) + (2)>::T)", "operator-<42>" },
	{ "_Z4makeI7FactoryiET_IT0_Ev", "Factory<int> make<Factory, int>()", "make<Factory, int>" },
	{ "_Z3foo5Hello5WorldS0_S_", "foo(Hello, World, World, Hello)", "foo" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "foo(intAB::**)"
	{ "_Z3fooPM2ABi", "foo(int AB::**)", "foo" },
	#endif

	{ "_ZlsRSoRKSs", "operator<<(std::ostream&, std::string const&)", "operator<<" },
	{ "_ZTI7a_class", "typeinfo for a_class", "typeinfo for a_class" },
	{ "U4_farrVKPi", "int* const volatile restrict _far", "int* const volatile restrict _far" },
	{ "_Z3fooILi2EEvRAplT_Li1E_i", "void foo<2>(int (&) [(2) + (1)])", "foo<2>" },
	{ "_Z1fM1AKFvvE", "f(void (A::*)() const)", "f" },
	{ "_Z3fooc", "foo(char)", "foo" },
	{ "2CBIL_Z3foocEE", "CB<foo(char)>", "CB<foo(char)>" },
	{ "2CBIL_Z7IsEmptyEE", "CB<IsEmpty>", "CB<IsEmpty>" },
	{ "_ZZN1N1fEiE1p", "N::f(int)::p", "N::f(int)::p" },
	{ "_ZZN1N1fEiEs", "N::f(int)::string literal", "N::f(int)::string literal" },
	{ "_Z1fPFvvEM1SFvvE", "f(void (*)(), void (S::*)())", "f" },
	{ "_ZN1N1TIiiE2mfES0_IddE", "N::T<int, int>::mf(N::T<double, double>)", "N::T<int, int>::mf" },
	{ "_ZSt5state", "std::state", "std::state" },
	{ "_ZNSt3_In4wardE", "std::_In::ward", "std::_In::ward" },
	{ "_Z1fKPFiiE", "f(int (* const)(int))", "f" },
	{ "_Z1fAszL_ZZNK1N1A1fEvE3foo_0E_i", "f(int [sizeof (N::A::f() const::foo)])", "f" },
	{ "_Z1fA37_iPS_", "f(int [37], int (*) [37])", "f" },
	{ "_Z1fM1AFivEPS0_", "f(int (A::*)(), int (*)())", "f" },

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_Z1fPFPA1_ivE", "f(int (* (*)()) [1])", "f" },
	#else
	{ "_Z1fPFPA1_ivE", "f(int (*(*)()) [1])", "f" },
	#endif

	{ "_Z1fPKM1AFivE", "f(int (A::*)() const*)", "f" },

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_Z1jM1AFivEPS1_", "j(int (A::*)(), int (A::*)()*)", "j" },
	#else
	{ "_Z1jM1AFivEPS1_", "j(int (A::*)(), int (A::**)())", "j" },
	#endif

	{ "_Z1sPA37_iPS0_", "s(int (*) [37], int (**) [37])", "s" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "foo(int [] [30])"
	{ "_Z3fooA30_A_i", "foo(int [30][])", "foo" },
	#endif

	{ "_Z3kooPA28_A30_i", "koo(int (*) [28][30])", "koo" },
	{ "_ZlsRKU3fooU4bart1XS0_", "operator<<(X bart foo const&, X bart)", "operator<<" },
	{ "_ZlsRKU3fooU4bart1XS2_", "operator<<(X bart foo const&, X bart foo const)", "operator<<" },
	{ "_Z1fM1AKFivE", "f(int (A::*)() const)", "f" },
	{ "_Z3absILi11EEvv", "void abs<11>()", "abs<11>" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "A<float>::operator float<int>()"
	{ "_ZN1AIfEcvT_IiEEv", "A<float>::operator int<int>()", "A<float>::operator int<int>" },
	#endif

	{ "_ZN12libcw_app_ct10add_optionIS_EEvMT_FvPKcES3_cS3_S3_", "void libcw_app_ct::add_option<libcw_app_ct>(void (libcw_app_ct::*)(char const*), char const*, char, char const*, char const*)", "libcw_app_ct::add_option<libcw_app_ct>" },
	{ "_ZGVN5libcw24_GLOBAL__N_cbll.cc0ZhUKa23compiler_bug_workaroundISt6vectorINS_13omanip_id_tctINS_5debug32memblk_types_manipulator_data_ctEEESaIS6_EEE3idsE", "guard variable for libcw::(anonymous namespace)::compiler_bug_workaround<std::vector<libcw::omanip_id_tct<libcw::debug::memblk_types_manipulator_data_ct>, std::allocator<libcw::omanip_id_tct<libcw::debug::memblk_types_manipulator_data_ct> > > >::ids", "guard variable for libcw::(anonymous namespace)::compiler_bug_workaround<std::vector<libcw::omanip_id_tct<libcw::debug::memblk_types_manipulator_data_ct>, std::allocator<libcw::omanip_id_tct<libcw::debug::memblk_types_manipulator_data_ct> > > >::ids" },
	{ "_ZN5libcw5debug13cwprint_usingINS_9_private_12GlobalObjectEEENS0_17cwprint_using_tctIT_EERKS5_MS5_KFvRSt7ostreamE", "libcw::debug::cwprint_using_tct<libcw::_private_::GlobalObject> libcw::debug::cwprint_using<libcw::_private_::GlobalObject>(libcw::_private_::GlobalObject const&, void (libcw::_private_::GlobalObject::*)(std::ostream&) const)", "libcw::debug::cwprint_using<libcw::_private_::GlobalObject>" },
	{ "_ZNKSt14priority_queueIP27timer_event_request_base_ctSt5dequeIS1_SaIS1_EE13timer_greaterE3topEv", "std::priority_queue<timer_event_request_base_ct*, std::deque<timer_event_request_base_ct*, std::allocator<timer_event_request_base_ct*> >, timer_greater>::top() const", "std::priority_queue<timer_event_request_base_ct*, std::deque<timer_event_request_base_ct*, std::allocator<timer_event_request_base_ct*> >, timer_greater>::top" },
	{ "_ZNKSt15_Deque_iteratorIP15memory_block_stRKS1_PS2_EeqERKS5_", "std::_Deque_iterator<memory_block_st*, memory_block_st* const&, memory_block_st* const*>::operator==(std::_Deque_iterator<memory_block_st*, memory_block_st* const&, memory_block_st* const*> const&) const", "std::_Deque_iterator<memory_block_st*, memory_block_st* const&, memory_block_st* const*>::operator==" },
	{ "_ZNKSt17__normal_iteratorIPK6optionSt6vectorIS0_SaIS0_EEEmiERKS6_", "std::__normal_iterator<option const*, std::vector<option, std::allocator<option> > >::operator-(std::__normal_iterator<option const*, std::vector<option, std::allocator<option> > > const&) const", "std::__normal_iterator<option const*, std::vector<option, std::allocator<option> > >::operator-" },
	{ "_ZNSbIcSt11char_traitsIcEN5libcw5debug27no_alloc_checking_allocatorEE12_S_constructIPcEES6_T_S7_RKS3_", "char* std::basic_string<char, std::char_traits<char>, libcw::debug::no_alloc_checking_allocator>::_S_construct<char*>(char*, char*, libcw::debug::no_alloc_checking_allocator const&)", "std::basic_string<char, std::char_traits<char>, libcw::debug::no_alloc_checking_allocator>::_S_construct<char*>" },

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_Z1fI1APS0_PKS0_EvT_T0_T1_PA4_S3_M1CS8_", "void f<A, A*, A const*>(A, A*, A const*, A const* (*) [4], A const* (*C::*) [4])", "f<A, A*, A const*>" },
	#else
	{ "_Z1fI1APS0_PKS0_EvT_T0_T1_PA4_S3_M1CS8_", "void f<A, A*, A const*>(A, A*, A const*, A const* (*) [4], A const* (* C::*) [4])", "f<A, A*, A const*>" },
	#endif

	{ "_Z3fooiPiPS_PS0_PS1_PS2_PS3_PS4_PS5_PS6_PS7_PS8_PS9_PSA_PSB_PSC_", "foo(int, int*, int**, int***, int****, int*****, int******, int*******, int********, int*********, int**********, int***********, int************, int*************, int**************, int***************)", "foo" },
	{ "_ZSt1BISt1DIP1ARKS2_PS3_ES0_IS2_RS2_PS2_ES2_ET0_T_SB_SA_PT1_", "std::D<A*, A*&, A**> std::B<std::D<A*, A* const&, A* const*>, std::D<A*, A*&, A**>, A*>(std::D<A*, A* const&, A* const*>, std::D<A*, A* const&, A* const*>, std::D<A*, A*&, A**>, A**)", "std::B<std::D<A*, A* const&, A* const*>, std::D<A*, A*&, A**>, A*>" },
	{ "_X11TransParseAddress", "_X11TransParseAddress", "_X11TransParseAddress" },
	{ "_ZNSt13_Alloc_traitsISbIcSt18string_char_traitsIcEN5libcw5debug9_private_17allocator_adaptorIcSt24__default_alloc_templateILb0ELi327664EELb1EEEENS5_IS9_S7_Lb1EEEE15_S_instancelessE", "std::_Alloc_traits<std::basic_string<char, std::string_char_traits<char>, libcw::debug::_private_::allocator_adaptor<char, std::__default_alloc_template<false, 327664>, true> >, libcw::debug::_private_::allocator_adaptor<std::basic_string<char, std::string_char_traits<char>, libcw::debug::_private_::allocator_adaptor<char, std::__default_alloc_template<false, 327664>, true> >, std::__default_alloc_template<false, 327664>, true> >::_S_instanceless", "std::_Alloc_traits<std::basic_string<char, std::string_char_traits<char>, libcw::debug::_private_::allocator_adaptor<char, std::__default_alloc_template<false, 327664>, true> >, libcw::debug::_private_::allocator_adaptor<std::basic_string<char, std::string_char_traits<char>, libcw::debug::_private_::allocator_adaptor<char, std::__default_alloc_template<false, 327664>, true> >, std::__default_alloc_template<false, 327664>, true> >::_S_instanceless" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "_GLOBAL__I__Z2fnv"
	{ "_GLOBAL__I__Z2fnv", "global constructors keyed to _Z2fnv", "global constructors keyed to _Z2fnv" },
	#endif

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "r(int (G::*)(), int (G::*)() const, G, int (H::*)(), int (G::*)(), what<G const>, what2<what>, int ()() const)"
	{ "_Z1rM1GFivEMS_KFivES_M1HFivES1_4whatIKS_E5what2IS8_ES3_", "r(int (G::*)(), int (G::*)() const, G, int (H::*)(), int (G::*)(), what<G const>, what2<G const>, int (G::*)() const)", "r" },
	#endif

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_Z10hairyfunc5PFPFilEPcE", "hairyfunc5(int (* (*)(char*))(long))", "hairyfunc5" },
	#else
	{ "_Z10hairyfunc5PFPFilEPcE", "hairyfunc5(int (*(*)(char*))(long))", "hairyfunc5" },
	#endif

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "_Z1fILi1ELc120EEv1AIXplT_cviLd810000000000000000703DAD7A370C5EEE"
	{ "_Z1fILi1ELc120EEv1AIXplT_cviLd810000000000000000703DAD7A370C5EEE", "void f<1, (char)120>(A<(1) + ((int)((double)[810000000000000000703DAD7A370C5]))>)", "f<1, (char)120>" },
	#endif

	// Disabled because there are various valid system representations of this.
	//#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	//{ "_Z1fILi1EEv1AIXplT_cvingLf3f800000EEE", "void f<1>(A<(1) + ((int)(-(0x1.000000p+0f)))>)", "f<1>" },
	//#else
	//{ "_Z1fILi1EEv1AIXplT_cvingLf3f800000EEE", "void f<1>(A<(1) + ((int)(-((float)[3f800000])))>)", "f<1>" },
	//#endif

	{ "_ZNK11__gnu_debug16_Error_formatter14_M_format_wordImEEvPciPKcT_", "void __gnu_debug::_Error_formatter::_M_format_word<unsigned long>(char*, int, char const*, unsigned long) const", "__gnu_debug::_Error_formatter::_M_format_word<unsigned long>" },
	{ "_ZSt18uninitialized_copyIN9__gnu_cxx17__normal_iteratorIPSt4pairISsPFbP6sqlitePPcEESt6vectorIS9_SaIS9_EEEESE_ET0_T_SG_SF_", "__gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > > std::uninitialized_copy<__gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > >, __gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > > >(__gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > >, __gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > >, __gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > >)", "std::uninitialized_copy<__gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > >, __gnu_cxx::__normal_iterator<std::pair<std::string, bool (*)(sqlite*, char**)>*, std::vector<std::pair<std::string, bool (*)(sqlite*, char**)>, std::allocator<std::pair<std::string, bool (*)(sqlite*, char**)> > > > >" },
	{ "_Z1fP1cIPFiiEE", "f(c<int (*)(int)>*)", "f" },
	{ "_Z4dep9ILi3EEvP3fooIXgtT_Li2EEE", "void dep9<3>(foo<((3) > (2))>*)", "dep9<3>" },

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_ZStltI9file_pathSsEbRKSt4pairIT_T0_ES6_", "bool std::operator<<file_path, std::string>(std::pair<file_path, std::string> const&, std::pair<file_path, std::string> const&)", "std::operator< <file_path, std::string>" },
	#else
	{ "_ZStltI9file_pathSsEbRKSt4pairIT_T0_ES6_", "bool std::operator< <file_path, std::string>(std::pair<file_path, std::string> const&, std::pair<file_path, std::string> const&)", "std::operator< <file_path, std::string>" },
	#endif

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "hairyfunc(int (* const (X::*)(long)) [3] const* (* restrict (* volatile* (Y::*)(int))(char*)) [2] const)"
	{ "_Z9hairyfuncM1YKFPVPFrPA2_PM1XKFKPA3_ilEPcEiE", "hairyfunc(int (* const (X::** (* restrict (* volatile* (Y::*)(int) const)(char*)) [2])(long) const) [3])", "hairyfunc" },
	#endif

	{ "_Z1fILin1EEvv", "void f<-1>()", "f<-1>" },

	#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM has a different-but-valid interpretation than libstdc++.
	{ "_ZNSdD0Ev", "std::iostream::~std::iostream()", "std::iostream::~std::iostream()" },
	#else
	{ "_ZNSdD0Ev", "std::basic_iostream<char, std::char_traits<char> >::~basic_iostream()", "std::basic_iostream<char, std::char_traits<char> >::~basic_iostream" },
	#endif

	{ "_ZNK15nsBaseHashtableI15nsUint32HashKey8nsCOMPtrI4IFooEPS2_E13EnumerateReadEPF15PLDHashOperatorRKjS4_PvES9_", "nsBaseHashtable<nsUint32HashKey, nsCOMPtr<IFoo>, IFoo*>::EnumerateRead(PLDHashOperator (*)(unsigned int const&, IFoo*, void*), void*) const", "nsBaseHashtable<nsUint32HashKey, nsCOMPtr<IFoo>, IFoo*>::EnumerateRead" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "int (*C::f<int>())() const"
	{ "_ZNK1C1fIiEEPFivEv", "int (*C::f<int>() const)()", "C::f<int>" },
	#endif

	{ "_ZZ3BBdI3FooEvvENK3Fob3FabEv", "void BBd<Foo>()::Fob::Fab() const", "void BBd<Foo>()::Fob::Fab" },
	{ "_ZZZ3BBdI3FooEvvENK3Fob3FabEvENK3Gob3GabEv", "void BBd<Foo>()::Fob::Fab() const::Gob::Gab() const", "void BBd<Foo>()::Fob::Fab() const::Gob::Gab" },
	{ "_ZNK5boost6spirit5matchI13rcs_deltatextEcvMNS0_4impl5dummyEFvvEEv", "boost::spirit::match<rcs_deltatext>::operator void (boost::spirit::impl::dummy::*)()() const", "boost::spirit::match<rcs_deltatext>::operator void (boost::spirit::impl::dummy::*)()" },

	#if !EACALLSTACK_LLVM_DEMANGLE_AVAILABLE // LLVM currently mis-prints this as "void foo<int const [6]>(int const const [6] [9], int const const restrict (* volatile restrict) [9][6])"
	{ "_Z3fooIA6_KiEvA9_KT_rVPrS4_", "void foo<int const [6]>(int const [9][6], int restrict const (* volatile restrict) [9][6])", "foo<int const [6]>" },
	#endif

	{ "_Z3fooIA3_iEvRKT_", "void foo<int [3]>(int const (&) [3])", "foo<int [3]>" },
	{ "_Z3fooIPA3_iEvRKT_", "void foo<int (*) [3]>(int (* const&) [3])", "foo<int (*) [3]>" },

  //{ "_ZN13PatternDriver23StringScalarDeleteValueC1ERKNS_25ConflateStringScalarValueERKNS_25AbstractStringScalarValueERKNS_12TemplateEnumINS_12pdcomplementELZNS_16complement_namesEELZNS_14COMPLEMENTENUMEEEE", "_ZN13PatternDriver23StringScalarDeleteValueC1ERKNS_25ConflateStringScalarValueERKNS_25AbstractStringScalarValueERKNS_12TemplateEnumINS_12pdcomplementELZNS_16complement_namesEELZNS_14COMPLEMENTENUMEEEE", "PatternDriver::StringScalarDeleteValue::StringScalarDeleteValue" },

};
#endif



///////////////////////////////////////////////////////////////////////////////
// TestDemangleGCC
//
// Applies to SN/EDG as well as general GCC.
//
static int TestDemangleGCC()
{
	using namespace EA::Callstack;

	int nErrorCount = 0;

	#if EACALLSTACK_GCC_DEMANGLE_ENABLED
		char buffer[2048];

		for(size_t i = 0, iEnd = EAArrayCount(gGCCDemangleTestArray); i != iEnd; ++i)
		{
			size_t len = UnmangleSymbol(gGCCDemangleTestArray[i].mpMangled, buffer, sizeof(buffer), kCompilerTypeGCC);

			// If using the LLVM pathway (and we know the code behaves consistently because we have the code here), then we can rely on our testing.
			// Currently the LLVM code relies on C++ RTTI in order to report full types (otherwise partial but still useful types).
			// Note that this RTTI check assumes that the LLVM code in cxx_demangle.cpp has been compiled the same.
			#if EACALLSTACK_LLVM_DEMANGLE_AVAILABLE && !defined(EA_COMPILER_NO_RTTI) 
				bool bStrlenMatches = (len == strlen(gGCCDemangleTestArray[i].mpDemangled));
				bool bStringMatches = bStrlenMatches && (strcmp(buffer, gGCCDemangleTestArray[i].mpDemangled) == 0);

				EATEST_VERIFY_F(bStringMatches, "TestDemangleGCC failure.\n   Expected: %s\n   Actual:   %s\n", gGCCDemangleTestArray[i].mpDemangled, buffer);
			#else
				EATEST_VERIFY((len > 0) && (len == strlen(buffer)));
			#endif
		}
	#endif

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestSymbolUtil
//
int TestSymbolUtil()
{
	EA::UnitTest::ReportVerbosity(1, "\nTestSymbolUtil\n");

	int nErrorCount = 0;

	nErrorCount += TestDemangleGCC();

	return nErrorCount;
}


TEST(EACallstackTest, TestSymbolUtil)
{
	int nErrorCount = TestSymbolUtil();

	EXPECT_EQ(nErrorCount, 0);
}


EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
