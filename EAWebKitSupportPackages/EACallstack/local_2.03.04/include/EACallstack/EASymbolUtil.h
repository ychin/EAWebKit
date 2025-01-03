/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_EASYMBOLUTIL_H
#define EACALLSTACK_EASYMBOLUTIL_H


#include <EABase/eabase.h>
#include <EACallstack/internal/Config.h>
#include <EACallstack/EAAddressRep.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <stddef.h>
#include <string.h>


namespace EA
{
namespace Callstack
{

	/// CompilerType
	///
	/// Identifies the compiler that created a given set of symbols.
	///
	enum CompilerType
	{
		kCompilerTypeNone,
		kCompilerTypeMSVC,          // Microsoft VC++
		kCompilerTypeGCC,           // GNU C/C++ and clang.
		kCompilerTypeSN,            // Sony gaming platforms.
		kCompilerTypeEDG           // This is a front-end to a compiler, and not the full compiler itself.
	};


	/// IsCIdentifierChar
	///
	/// Returns true if the given char is a valid C/C++ char for use in
	/// a C identifier (e.g. variable name, class name). C doesn't allow
	/// the use of a digit as the first character of an identifier, but
	/// that distinction doesn't matter here for our uses.
	///
	EACALLSTACK_API bool IsCIdentifierChar(int c);


	/// QuoteString
	///
	/// Puts quotes around a string, possibly skipping the operation if
	/// there are already quotes around the string.
	/// If bUnilateral is true, quotes are placed no matter what.
	/// If bUnilateral is false, quotes are placed only if they aren't
	/// already present. A string that has a quote at one end but not
	/// the other is treated as if it were completely unquoted.
	///
	EACALLSTACK_API eastl::string& QuoteString(eastl::string& s, bool bUnilateral = true);
	EACALLSTACK_API FixedString8&  QuoteString(FixedString8& s, bool bUnilateral = true);


	/// UnquoteString
	///
	/// Removes quotes from a string.
	/// A string that has a quote at one end but not the other is treated
	/// as if it were completely unquoted and thus no operation is done.
	///
	EACALLSTACK_API eastl::string& UnquoteString(eastl::string& s);
	EACALLSTACK_API FixedString8&  UnquoteString(FixedString8& s);


	/// RemoveSubstring
	///
	/// Removes instances of the given supplied substring.
	///
	EACALLSTACK_API eastl::string& RemoveSubstring(eastl::string& s, const char* pSubstring);
	EACALLSTACK_API FixedString8&  RemoveSubstring(FixedString8&  s, const char* pSubstring);


	/// CollapseStringSpace
	///
	/// Converts all spaces of more than one space to be just one space.
	///
	/// Example usage:
	///     string s("hello          world     hello");
	///     CollapseStringSpace(s);
	///     s is now equal to "hello world hello"
	///
	EACALLSTACK_API eastl::string& CollapseStringSpace(eastl::string& s);
	EACALLSTACK_API FixedString8&  CollapseStringSpace(FixedString8& s);


	/// RemoveCIdentifier
	///
	/// Removes any instances of the given p string from the input string s.
	///
	/// Example usage:
	///     string s("template class integer class float classical");
	///     RemoveCIdentifier(s, "class");
	///     s is now equal to "template  integer  float classical"
	///
	EACALLSTACK_API eastl::string& RemoveCIdentifier(eastl::string& s, const char* p);
	EACALLSTACK_API FixedString8&  RemoveCIdentifier(FixedString8&  s, const char* p);


	/// CollapseTemplate
	///
	/// For any instances of the given template in the string, this function
	/// replaces the template with simply '*'.
	///
	/// Example usage:
	///     string s("hello widget< int, bucket<char, char> > world");
	///     CollapseTemplate(s, "bucket");
	///     s is now equal to "hello widget< int, * > world"
	///
	EACALLSTACK_API void CollapseTemplate(eastl::string& s, const char* t);
	EACALLSTACK_API void CollapseTemplate(FixedString8& s, const char* t);


	/// CollapseTemplateArg
	///
	/// For any instances of the given template in the string, this function
	/// replaces the template parameters to simply '*'.
	///
	/// Example usage:
	///     string s("hello widget< int, bucket<char, char> > world");
	///     CollapseTemplateArg(s, "widget");
	///     s is now equal to "hello widget<*> world"
	///
	EACALLSTACK_API void CollapseTemplateArg(eastl::string& s, const char* t);
	EACALLSTACK_API void CollapseTemplateArg(FixedString8&  s, const char* t);


	/// RemoveArguments
	///
	/// Removes the arguments portion of a function declaration altogether.
	/// A string that has a '(' but no associated ')' is assumed to have
	/// a matching ')' at the end of the string.
	///
	/// Example usage:
	///     string s("void SomeFunction(int x, string<char>& y)");
	///     RemoveArguments(s, false);
	///     s is now equal to "void SomeFunction"
	///
	EACALLSTACK_API void RemoveArguments(eastl::string& s, bool bKeepParentheses = false);
	EACALLSTACK_API void RemoveArguments(FixedString8&  s, bool bKeepParentheses = false);


	/// RemoveScoping
	///
	/// Removes leading namespaces and class scoping from a symbol.
	/// Let's you specify how many leading scopes you want to retain.
	///
	/// Give that namespace syntax is the same as class scoping syntax,
	/// (i.e. the :: delimiter) this function has little choice but to
	/// treat them the same. This we call this function RemoveScoping
	/// instead of something like RemoveNamespaces.
	///
	/// Example usage:
	///     string s("void abc::def::ghi::jkl::SomeFunction()");
	///     RemoveScoping(s, 1);
	///     s is now equal to "jkl::SomeFunction()"
	///
	EACALLSTACK_API void RemoveScoping(eastl::string& s, int retainCount = 0);
	EACALLSTACK_API void RemoveScoping(FixedString8&  s, int retainCount = 0);


	/// IsSymbolUseful
	///
	/// Returns true if the symbol is deemed useful.
	/// A useful symbol is one that refers to a user-created variable or function,
	/// as opposed to compiler-generated symbols.
	///
	EACALLSTACK_API bool IsSymbolUseful(const eastl::string& symbol, CompilerType ct = kCompilerTypeNone);
	EACALLSTACK_API bool IsSymbolUseful(const FixedString8&  symbol, CompilerType ct = kCompilerTypeNone);


	/// UnmangleSymbol
	///
	/// Converts a C++ 'mangled' symbol name to a readable name.
	///
	/// You will recall that the C++ languauge differs from C in that C++ symbols
	/// (e.g. function names, variable names) are stored in debug information
	/// (e.g. map files) with their names mangled. This is due to C++ concepts
	/// such as overloading and namespace. For example a C++ member function in
	/// the EA::Allocator namespace such as this:
	///     void GeneralAllocator::AdjustHighFence()
	/// Gets mangled by the VC++ compiler to this:
	///     ?AdjustHighFence@GeneralAllocator@Allocator@EA@@IAEXXZ
	///
	/// Returns the required strlen of the result, which will be >= to bufferCapacity
	/// if there wasn't enough space. The returned buffer will always be 0-terminated
	/// and will be written up to the capacity. bufferCapacity must be > 0.
	///
	/// To consider: Add options to this function specifying the extent of demangling.
	///
	EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, char* buffer, size_t bufferCapacity, CompilerType ct = kCompilerTypeNone);

	EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, eastl::string& s, CompilerType ct = kCompilerTypeNone);
	EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, FixedString8&  s, CompilerType ct = kCompilerTypeNone);


	/// IsDRMInternalLabel
	///
	/// simple utiltiy to determine if symbol is simply an internal label created by DRM technology
	EACALLSTACK_API bool IsDRMInternalLabel(const char* mangledName);


	/// SanitizeSymbol
	///
	/// Sanitizes the given symbol, removing things considered extraneous
	/// to the user understanding of the symnbol.
	///
	EACALLSTACK_API eastl::string& SanitizeSymbol(eastl::string& s, CompilerType ct = kCompilerTypeNone);
	EACALLSTACK_API FixedString8&  SanitizeSymbol(FixedString8&  s, CompilerType ct = kCompilerTypeNone);


} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_EASYMBOLUTIL_H
