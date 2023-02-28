///////////////////////////////////////////////////////////////////////////////
// EASymbolUtil.cpp
//
// Copyright (c) 2003-2005 Electronic Arts Inc.
// Created by Paul Pedriana
// Symbol quote/unquote/remove/collapse/sanitize is based on work by Avery Lee.
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EACallstack/EASymbolUtil.h>
#include <EASTL/fixed_substring.h>
#include <EASTL/fixed_string.h>
#include <EACallstack/internal/DemangleGCC.h>
#include <assert.h>
#include <EAStdC/EAString.h>
#include <eathread/eathread_futex.h>

#if defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <Windows.h>

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // DbgHelp and psapi are supported with desktop libraries only.
		#include <DbgHelp.h>
		#ifdef _MSC_VER
			#pragma comment(lib, "dbghelp.lib")
			#pragma comment(lib, "psapi.lib")
		#endif
	#endif
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

namespace EA
{
	namespace Callstack
	{
		EACALLSTACK_API bool IsCIdentifierChar(int c)
		{
			return  (c >= '0' && c <= '9') ||
					(c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					(c == '_');
		}


		EACALLSTACK_API eastl::string& QuoteString(eastl::string& s, bool bUnilateral)
		{
			if(bUnilateral || (s.length() < 2) || (s[0] != '"') || (s[s.length() - 1] != '"'))
			{
				s.insert(s.begin(), '"');
				s += '"';
			}
			return s;
		}

		EACALLSTACK_API FixedString8& QuoteString(FixedString8& s, bool bUnilateral)
		{
			if(bUnilateral || (s.length() < 2) || (s[0] != '"') || (s[s.length() - 1] != '"'))
			{
				s.insert(s.begin(), '"');
				s += '"';
			}
			return s;
		}


		EACALLSTACK_API eastl::string& UnquoteString(eastl::string& s)
		{
			if((s.length() >= 2) && (s[0] == '"') && (s[s.length() - 1] == '"'))
			{
				s.erase(s.begin());
				s.erase(s.length() - 1, 1);
			}
			return s;
		}

		EACALLSTACK_API FixedString8& UnquoteString(FixedString8& s)
		{
			if((s.length() >= 2) && (s[0] == '"') && (s[s.length() - 1] == '"'))
			{
				s.erase(s.begin());
				s.erase(s.length() - 1, 1);
			}
			return s;
		}


		EACALLSTACK_API eastl::string& RemoveSubstring(eastl::string& s, const char* pSubstring)
		{
			const eastl_size_t substringLength = (eastl_size_t)strlen(pSubstring);
			eastl_size_t pos = 0;

			while((pos = s.find(pSubstring, pos, substringLength)) != eastl::string::npos)
				s.erase(pos, substringLength);

			return s;
		}

		EACALLSTACK_API FixedString8& RemoveSubstring(FixedString8& s, const char* pSubstring)
		{
			const eastl_size_t substringLength = (eastl_size_t)strlen(pSubstring);
			eastl_size_t pos = 0;

			while((pos = s.find(pSubstring, pos, substringLength)) != FixedString8::npos)
				s.erase(pos, substringLength);

			return s;
		}


		EACALLSTACK_API eastl::string& CollapseStringSpace(eastl::string& s)
		{
			// This could be implemented by a faster algorithm, but speed 
			// isn't a primary concern of ours at this time.
			eastl_size_t pos = 0;

			while((pos = s.find("  ", pos)) != eastl::string::npos)
				s.erase(pos, 1);

			return s;
		}

		EACALLSTACK_API FixedString8& CollapseStringSpace(FixedString8& s)
		{
			// This could be implemented by a faster algorithm, but speed 
			// isn't a primary concern of ours at this time.
			eastl_size_t pos = 0;

			while((pos = s.find("  ", pos)) != FixedString8::npos)
				s.erase(pos, 1);

			return s;
		}


		EACALLSTACK_API eastl::string& RemoveCIdentifier(eastl::string& s, const char* p)
		{
			const eastl::string  match(p);
			eastl_size_t         pos(0), len(0), matchsize(match.size());

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not a 
				// standalone keyword but just text after some other name.
				if(pos > 0 && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				// If the match is followed by C identifiers, then it is not a 
				// standalone keyword but just text before some other name.
				if(((pos + matchsize) < len) && IsCIdentifierChar(s[pos + matchsize]))
				{
					pos++;
					continue;
				}

				s.erase(pos, matchsize);
			}

			return s;
		}

		EACALLSTACK_API FixedString8& RemoveCIdentifier(FixedString8& s, const char* p)
		{
			const FixedString8  match(p);
			eastl_size_t        pos(0), len(0), matchsize(match.size());

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not a 
				// standalone keyword but just text after some other name.
				if(pos > 0 && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				// If the match is followed by C identifiers, then it is not a 
				// standalone keyword but just text before some other name.
				if(((pos + matchsize) < len) && IsCIdentifierChar(s[pos + matchsize]))
				{
					pos++;
					continue;
				}

				s.erase(pos, matchsize);
			}

			return s;
		}


		EACALLSTACK_API void CollapseTemplate(eastl::string& s, const char* t)
		{
			const eastl::string match(eastl::string(t) + "<");
			eastl_size_t        pos(0), len(0);

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not 
				// the template we are looking for but just text after some other name.
				if((pos > 0) && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				int nestlevel = 1;
				const eastl_size_t start = pos;
				  
				pos += match.size();

				while(pos < len)
				{
					const char c = s[pos++];

					if(c == '<')
						++nestlevel;
					else if(c == '>')
					{
						if(--nestlevel == 0)
							break;
					}
				}

				if(pos < len)
				{
					s.replace(start, pos - start, "*");
					pos++;
				}
			}
		}

		EACALLSTACK_API void CollapseTemplate(FixedString8& s, const char* t)
		{
			const FixedString8 match(FixedString8(t) + "<");
			eastl_size_t       pos(0), len(0);

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not 
				// the template we are looking for but just text after some other name.
				if((pos > 0) && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				int nestlevel = 1;
				const eastl_size_t start = pos;
				  
				pos += match.size();

				while(pos < len)
				{
					const char c = s[pos++];

					if(c == '<')
						++nestlevel;
					else if(c == '>')
					{
						if(--nestlevel == 0)
							break;
					}
				}

				if(pos < len)
				{
					s.replace(start, pos - start, "*");
					pos++;
				}
			}
		}


		EACALLSTACK_API void CollapseTemplateArg(eastl::string& s, const char* t)
		{
			const eastl::string match(eastl::string(t) + "<");
			eastl_size_t        pos(0), len(0);

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not 
				// the template we are looking for but just text after some other name.
				if((pos > 0) && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				pos += match.size();

				const eastl_size_t start = pos;
				int nestlevel = 1;

				while(pos < len)
				{
					const char c = s[pos];

					if(c == '<')
						++nestlevel;
					else if(c == '>')
					{
						if(--nestlevel == 0)
							break;
					}

					++pos;
				}

				if(pos < len)
				{
					s.replace(start, pos - start, "*");
					pos++;
				}
			}
		}

		EACALLSTACK_API void CollapseTemplateArg(FixedString8& s, const char* t)
		{
			const FixedString8 match(FixedString8(t) + "<");
			eastl_size_t       pos(0), len(0);

			while((pos = s.find(match, pos)) < (len = s.length()))
			{
				// If the match is preceded by C identifiers, then it is not 
				// the template we are looking for but just text after some other name.
				if((pos > 0) && IsCIdentifierChar(s[pos - 1]))
				{
					pos++;
					continue;
				}

				pos += match.size();

				const eastl_size_t start = pos;
				int nestlevel = 1;

				while(pos < len)
				{
					const char c = s[pos];

					if(c == '<')
						++nestlevel;
					else if(c == '>')
					{
						if(--nestlevel == 0)
							break;
					}

					++pos;
				}

				if(pos < len)
				{
					s.replace(start, pos - start, "*");
					pos++;
				}
			}
		}


		EACALLSTACK_API void RemoveArguments(eastl::string& s, bool bKeepParentheses)
		{
			// Find the first '(' char. Then find the matching end ')' char.
			// Ignore embedded "()" pairs in-between these.

			eastl_size_t pos = s.find('(');

			if(pos != eastl::string::npos)
			{
				const eastl_size_t end = s.length();
				eastl_size_t i;
				eastl_size_t depth = 0;

				for(i = pos; i < end; i++)
				{
					if(s[i] == '(') // If this is the first or an embedded '('...
						++depth;
					else if(s[i] == ')')
					{
						if(--depth == 0) // If this is the matching ')'...
						{
							++i;
							break;
						}
					}
				}

				if(bKeepParentheses)
				{
					++pos;
					if(depth == 0) // If there was a matching ')'...
						--i;
				}

				s.erase(pos, i - pos);
			}
		}

		EACALLSTACK_API void RemoveArguments(FixedString8& s, bool bKeepParentheses)
		{
			// Find the first '(' char. Then find the matching end ')' char.
			// Ignore embedded "()" pairs in-between these.

			eastl_size_t pos = s.find('(');

			if(pos != FixedString8::npos)
			{
				const eastl_size_t end = s.length();
				eastl_size_t i;
				eastl_size_t depth = 0;

				for(i = pos; i < end; i++)
				{
					if(s[i] == '(') // If this is the first or an embedded '('...
						++depth;
					else if(s[i] == ')')
					{
						if(--depth == 0) // If this is the matching ')'...
						{
							++i;
							break;
						}
					}
				}

				if(bKeepParentheses)
				{
					++pos;
					if(depth == 0) // If there was a matching ')'...
						--i;
				}

				s.erase(pos, i - pos);
			}
		}


		EACALLSTACK_API void RemoveScoping(eastl::string& s, int retainCount)
		{
			(void)s;
			(void)retainCount;

			//To do. 
			//const eastl_size_t pos = s.rfind("::", eastl::string::npos);
			//
			//while(retainCount > 0)
			//{
			//}
		}

		EACALLSTACK_API void RemoveScoping(FixedString8& s, int retainCount)
		{
			(void)s;
			(void)retainCount;

			//To do. 
			//const eastl_size_t pos = s.rfind("::", FixedString8::npos);
			//
			//while(retainCount > 0)
			//{
			//}
		}


		EACALLSTACK_API bool IsSymbolUseful(const eastl::string& symbol)
		{
			// To do: Add GCC symbol support to this. GCC doesn't use terms  
			// like __ehhandler, but has its own terms that we want to deal with.

			if(symbol.find("`vftable'") != eastl::string::npos)
				return false;

			if(symbol.compare(0, 11, "__ehhandler") == 0)
				return false;

			return true;
		}

		EACALLSTACK_API bool IsSymbolUseful(const FixedString8& symbol)
		{
			// To do: Add GCC symbol support to this. GCC doesn't use terms  
			// like __ehhandler, but has its own terms that we want to deal with.

			if(symbol.find("`vftable'") != FixedString8::npos)
				return false;

			if(symbol.compare(0, 11, "__ehhandler") == 0)
				return false;

			return true;
		}

		// Map files generated by DRM technology 
		//  example from a map file:
		//  000c:0048eea0       LSI85555__worldlocaltransformutilsentities.cpp~?onInit@LocalToWorldTransformEntity@fb@@UEAAXAEAVEntityInitInfo@2@@Z~129 00000001432d3ea0     KUNDEN.obj
		//

		// DETAILS from  Gabler, Christopher
		//
		//Callstack:
		//          fifa15.exe!LSI478736__profiler.cpp~MY_?StartFrame@Profiler@GameFrameWork@@YAXW4CategoryEnum@pvProfiler@@@Z~809 ()  Unknown
		//
		//	The label that you can see above called “Label_130279” does not come from Fifa itself. So in case your callstack (can happen in some cases) shows any “Label_xxxx” just scroll up in disassembly and locate the first “LSIxxxxx__....” label that comes from the game. This will indicate where it crashed.
		//
		//	More information regarding the Fifa game code labels format:
		//	LSI478746__profiler.cpp~MY_?StartFrame@Profiler@GameFrameWork@@YAXW4CategoryEnum@pvProfiler@@@Z~809:
		//
		//	LSIxxxxx: Can be ignored, it is an internal number used to make labels unique
		//	profiler.cpp: Source code name
		//	MY_: Can be ignored, this prefix comes from the internal labeling system
		//	StartFrame@Profiler@GameFramework@@YAXW4CategoryEnum@pvProfiler@@Z: This is the class and method information from the game
		//	809: This is the source code line

		
		// "The label that you can see above called “Label_130279” does not come from Fifa itself. "
		EACALLSTACK_API bool IsDRMInternalLabel(const char* mangledName)
		{
			// ideally if the name is mangled this check will never give us false positives with real normal function names (starting with '@' or '?')

			return EA::StdC::Strncmp(mangledName, "Label_", EA::StdC::Strlen("Label_")) == 0;
		}

#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)


		static char* StripDRMEncodingIfFound(char* pOutBuffer)
		{
			char* rtnBuffer = pOutBuffer;
			if (EA::StdC::Strncmp(pOutBuffer, "MY_", EA::StdC::Strlen("MY_")) == 0 ||
				EA::StdC::Strncmp(pOutBuffer, "LSI", EA::StdC::Strlen("LSI")) == 0)
			{
				// strip beginning till we reach a real symbol prefix
				// http://en.wikipedia.org/wiki/Visual_C%2B%2B_name_mangling
				while (	pOutBuffer[0] != '\0' && 
						pOutBuffer[0] != '?' && 
						!(pOutBuffer[0] == '@' && pOutBuffer[1] == '?'))
				{
					pOutBuffer++;
				}
	
				// this will be the new start
				rtnBuffer = pOutBuffer;

				// remove the "~1234" at the end otherwise the UnDecorateSymbolName will not work
				while (pOutBuffer[0] != '\0')
				{
					if (pOutBuffer[0] == '~')
					{
						pOutBuffer[0] = '\0'; // terminated !
						break;
					}
					pOutBuffer++;
				}
			}


			return rtnBuffer;
		}
#endif

		EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, char* buffer, size_t bufferCapacity, CompilerType ct)
		{
			buffer[0] = 0;

			if(ct == kCompilerTypeMSVC)
			{
				#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

					static const int MAX_FULL_SYMBOL_NAME = 2048;
					// create a copy so we can strip the DRM symbols
					char     symbolStripped[MAX_FULL_SYMBOL_NAME + 1];
					EA::StdC::Strncpy(symbolStripped, pSymbol, MAX_FULL_SYMBOL_NAME);
					symbolStripped[MAX_FULL_SYMBOL_NAME] = '\0'; // for safety
					pSymbol = StripDRMEncodingIfFound(symbolStripped);

					// Use the MS DbgHelp UndecorateSymbolName function to do the work for us.
					DWORD count;
					{
						// All DbgHelp functions are not thread safe and must be have all accesses synchronized
						// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681400(v=vs.85).aspx
						static EA::Thread::Futex sDbgFutex;
						EA::Thread::AutoFutex autoFutex(sDbgFutex);
						count = UnDecorateSymbolName(pSymbol, buffer, (DWORD)bufferCapacity, UNDNAME_NO_THISTYPE | UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MEMBER_TYPE);
					}

					if(count == 0)
						buffer[0] = 0; // Must re-clear the buffer.
					return (size_t)count;
				#else
					// We don't currently have code to manually demangle VC++ symbols, though such code  
					// could be implemented, as the format is reasonably well documented online.
				#endif
			}
			else if((ct == kCompilerTypeGCC) || 
					(ct == kCompilerTypeSN)  ||
					(ct == kCompilerTypeEDG)) 
			{
				#if EACALLSTACK_GCC_DEMANGLE_ENABLED
					return Demangle::UnmangleSymbolGCC(pSymbol, buffer, bufferCapacity);
				#endif
			}

			strncpy(buffer, pSymbol, bufferCapacity);
			buffer[bufferCapacity - 1] = 0;

			return strlen(buffer);
		}


		EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, eastl::string& s, CompilerType ct)
		{
			char         buffer[1024];
			const size_t requiredStrlen = UnmangleSymbol(pSymbol, buffer, sizeof(buffer), ct);

			s = buffer; // To do: retry with more capacity if needed.

			return requiredStrlen;
		}


		EACALLSTACK_API size_t UnmangleSymbol(const char* pSymbol, FixedString8& s, CompilerType ct)
		{
			char         buffer[1024];
			const size_t requiredStrlen = UnmangleSymbol(pSymbol, buffer, sizeof(buffer), ct);

			s = buffer; // To do: retry with more capacity if needed.

			return requiredStrlen;
		}


		EACALLSTACK_API eastl::string& SanitizeSymbol(eastl::string& s, CompilerType /*ct*/)
		{
			// To do: Add GCC symbol support to this. GCC doesn't use terms  
			// like __fastcall, but has its own terms that we want to deal with.

			// Remove calling conventions
			RemoveCIdentifier(s, "__cdecl");
			RemoveCIdentifier(s, "__stdcall");
			RemoveCIdentifier(s, "__fastcall");
			RemoveCIdentifier(s, "__thiscall");
			RemoveCIdentifier(s, "class");
			RemoveCIdentifier(s, "struct");
			RemoveCIdentifier(s, "enum");

			// Remove various specific known templates
			CollapseTemplate(s, "allocator");
			CollapseTemplate(s, "_Select1st");
			CollapseTemplate(s, "_Nonconst_traits");

			// Remove namespace prefixes
			RemoveSubstring(s, "_STL::");
			RemoveSubstring(s, "std::");
			RemoveSubstring(s, "eastl::");
			RemoveSubstring(s, "EA::");

			// Clear extraneous string space.
			s.trim();
			CollapseStringSpace(s);

			return s;
		}

		EACALLSTACK_API FixedString8& SanitizeSymbol(FixedString8& s, CompilerType /*ct*/)
		{
			// To do: Add GCC symbol support to this. GCC doesn't use terms  
			// like __fastcall, but has its own terms that we want to deal with.

			// Remove calling conventions
			RemoveCIdentifier(s, "__cdecl");
			RemoveCIdentifier(s, "__stdcall");
			RemoveCIdentifier(s, "__fastcall");
			RemoveCIdentifier(s, "__thiscall");
			RemoveCIdentifier(s, "class");
			RemoveCIdentifier(s, "struct");
			RemoveCIdentifier(s, "enum");

			// Remove various specific known templates
			CollapseTemplate(s, "allocator");
			CollapseTemplate(s, "_Select1st");
			CollapseTemplate(s, "_Nonconst_traits");

			// Remove namespace prefixes
			RemoveSubstring(s, "_STL::");
			RemoveSubstring(s, "std::");
			RemoveSubstring(s, "eastl::");
			RemoveSubstring(s, "EA::");

			// Clear extraneous string space.
			s.trim();
			CollapseStringSpace(s);

			return s;
		}

	} // namespace Callstack

} // namespace EA





