///////////////////////////////////////////////////////////////////////////////
// EAText.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EAText.h>
#include <EAStdC/EAString.h>
#include <EAText/EATextLayout.h>
#include <EAText/EATextOutlineFont.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAText/internal/EATextCollation.h>

#include <EASTL/hash_map.h>
#include <eathread/eathread_futex.h>

#include EA_ASSERT_HEADER

#if EATEXT_LAYOUT_ENABLED
	#include <EAText/EATextLayout.h>
#endif

EA_DISABLE_VC_WARNING(4464) // relative include path contains '..' (compiling source file W : \dev\packages\EAText\dev\test\source\TestFreeType.cpp)
#include <ft2build.h>
#include FT_FREETYPE_H

#if (((FREETYPE_MAJOR * 100) + FREETYPE_MINOR) >= 209) // FreeType 2.9 changed the include directory structure.
	#include <freetype/internal/internal.h>
	#include <freetype/internal/ftobjs.h>
#elif (((FREETYPE_MAJOR * 100) + FREETYPE_MINOR) >= 205) // FreeType 2.5 changed the include directory structure.
	#include <internal/internal.h>
	#include <internal/ftobjs.h>
#else
	#include <freetype/internal/internal.h>
	#include <freetype/internal/ftobjs.h>
#endif
EA_RESTORE_VC_WARNING()


namespace EA
{
namespace Text
{


// Global variable declarations.
EA::Allocator::ICoreAllocator* gpCoreAllocator = NULL;

AssertionFailureFunction gpAssertionFailureFunction        = NULL;
void*                    gpAssertionFailureFunctionContext = NULL;


#if (EATEXT_CHAR_TYPE_SIZE == 2) && !defined(EA_CHAR16)
	// This is a temporary solution that is not ideal but allows to stick 
	// with 2 bytes char16_t on compiler where wchar_t is 4 bytes. This solution
	// is necessary only for the case of a non-conforming C++11 compiler that
	// doesn't fully support C++11 char16_t while the EAText Char type is set
	// to be char16_t. 
	const char16_t* TextWPtrToText16Ptr(const wchar_t* pTextW)
	{
		typedef eastl::hash_map<const wchar_t*, eastl::string16> String16Cache;

		static String16Cache     mapString16;
		static EA::Thread::Futex mapLock;
		EA::Thread::AutoFutex    autoFutex(mapLock);

		String16Cache::iterator itFound = mapString16.find(pTextW); // Note that this is finding by pointer equivalence adn not content equivalence. However, string literals normally are made equivalent by the linker, and so this shouldn't be the efficiency concern it may seem.

		if(itFound == mapString16.end())
		{
			eastl::string16 dest;
			EA::StdC::Strlcpy(dest, pTextW);

			String16Cache::insert_return_type result = mapString16.insert(eastl::make_pair(pTextW, dest));
			itFound = result.first;
		}

		return itFound->second.c_str();
	}

#elif (EATEXT_CHAR_TYPE_SIZE == 4) && !defined(EA_CHAR32)
	// See above comments, which apply here similarly.
	const char32_t* TextWPtrToText32Ptr(const wchar_t* pTextW)
	{
		typedef eastl::hash_map<const wchar_t*, eastl::string32> String32Cache;

		static String32Cache     mapString32;
		static EA::Thread::Futex mapLock;
		EA::Thread::AutoFutex    autoFutex(mapLock);

		String32Cache::iterator itFound = mapString32.find(pTextW); // Note that this is finding by pointer equivalence adn not content equivalence. However, string literals normally are made equivalent by the linker, and so this shouldn't be the efficiency concern it may seem.

		if(itFound == mapString32.end())
		{
			eastl::string32 dest;
			EA::StdC::Strlcpy(dest, pTextW);

			String32Cache::insert_return_type result = mapString32.insert(eastl::make_pair(pTextW, dest));
			itFound = result.first;
		}

		return itFound->second.c_str();
	}
#endif


// Each independent user of FreeType works through an FT_Library instance.
// We have a problem in that if the user uses EAText from multiple threads 
// and shuts down and NULLs gFTLibrary from one thread but is still accessing
// it from another thread, we could have a problem. The only fix is to do an
// atomic read and write to gFTLibrary in all places it's used, or else use 
// a mutex. On the other hand, if the application is accessing gFTLibrary from
// multiple threads while any of them could be deleting gFTLibrary then we have
// a bigger problem that thread safety can't solve. So as of this writing we 
// are not protecting gFTLibrary with thread synchronization primitive beyond
// the C++ volatile keyword.
volatile FT_Library gFTLibrary = NULL;



void* FTAlloc(FT_Memory /*memory*/, long n)
{
	if(!gpCoreAllocator)
		gpCoreAllocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();

	void* p = gpCoreAllocator->Alloc((size_t)n, EATEXT_ALLOC_PREFIX "FreeType", 0);

	if(p) // The FreeType documentation says that this function should zero the memory.
		memset(p, 0, (size_t)n);

	return p;
}


void FTFree(FT_Memory /*memory*/, void* p)
{
	gpCoreAllocator->Free(p);
}

void* FTRealloc(FT_Memory memory, long oldSize, long n, void* p)
{
	// For the C realloc function, ANSI C requires:
	//  (1) realloc(NULL, newsize) is equivalent to malloc(newsize).
	//  (2) realloc(p, 0) is equivalent to free(p) (except that NULL is returned).
	//  (3) if the realloc() fails, the object pointed to by p is left unchanged.
	//
	// What is not made clear is what the resulting behaviour of realloc(NULL, 0)
	// should be. We will take cases (1) and (2) above and assume we should return NULL.

	void* pReturnValue;

	if(p)
	{
		if(n > 0)
		{
			pReturnValue = FTAlloc(memory, n);

			if(pReturnValue)
			{
				const long memcpySize = (n > oldSize) ? oldSize : n;
				memcpy(pReturnValue, p, (size_t)memcpySize);
				FTFree(memory, p);
			}
		}
		else // In this case the standard calls for the same behaviour as free().                
			pReturnValue = NULL;
	}
	else // In this case the standard calls for the same behaviour as malloc().            
		pReturnValue = FTAlloc(memory, n);
		
	return pReturnValue;
}


// This function is the same as FT_Init_FreeType, except it allows the user to pass in 
// memory allocation functions. Curiously, FreeType doesn't let the user override the 
// default memory allocation functions but rather expects the user to replace the global
// FreeType memory allocation functions, when makes you wonder why FreeType's Library
// object goes through the trouble of having a memory object, since you're stuck using
// the global functions anyway behind it.

FT_EXPORT_DEF(FT_Error)
FT_Init_FreeType_2(FT_Library* alibrary, FT_MemoryRec_* memoryRec)
{
	FT_Error   error;
	FT_Memory  memory;

	memory = (FT_Memory)memoryRec->alloc(memoryRec, sizeof(*memory));
	if(memory)
		*memory = *memoryRec;
	else
	{
		//FT_ERROR(( "FT_Init_FreeType: cannot find memory manager\n" ));
		return FT_Err_Unimplemented_Feature;
	}

	error = FT_New_Library(memory, alibrary);
	if(error)
		memory->free(memory, memory);
	else
	{
		(*alibrary)->version_major = FREETYPE_MAJOR;
		(*alibrary)->version_minor = FREETYPE_MINOR;
		(*alibrary)->version_patch = FREETYPE_PATCH;

		FT_Add_Default_Modules(*alibrary);
	}

	return error;
}


// This function is like FT_Done_FreeType except it uses the user-supplied allocator to
// free the FT_Memory object instead of using a global allocator like FT_Done_FreeType does.

FT_EXPORT_DEF(FT_Error)
FT_Done_FreeType_2(FT_Library  library)
{
	if(library)
	{
		FT_Memory  memory = library->memory;
		FT_Done_Library(library);
		memory->free(memory, memory);
	}

	return FT_Err_Ok;
}



Allocator::ICoreAllocator* GetAllocator()
{
	return gpCoreAllocator;
}


void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
	gpCoreAllocator = pCoreAllocator;
}


EATEXT_API void SetAssertionFailureFunction(AssertionFailureFunction pAssertionFailureFunction, void* pContext)
{
	gpAssertionFailureFunction        = pAssertionFailureFunction;
	gpAssertionFailureFunctionContext = pContext;
}


EATEXT_API void AssertionFailure(const char* pExpression)
{
	if(gpAssertionFailureFunction)
		gpAssertionFailureFunction(pExpression, gpAssertionFailureFunctionContext);
}


EATEXT_API void Init()
{
	if(gFTLibrary == NULL)
	{
		FT_MemoryRec_ memory = { gpCoreAllocator, FTAlloc, FTFree, FTRealloc };
		FT_Error error = FT_Init_FreeType_2((FT_Library*)&gFTLibrary, &memory); // Question: does casting gFTLibrary to non-volatile create a C/C++ 'strict aliasing' problem?
		EA_ASSERT(error == 0); (void)error;

		// FT_Library_SetLcdFilter specifies how FreeType balances the weights of the colors generated during sub-pixel rendering.
		#if defined(FT_CONFIG_OPTION_SUBPIXEL_RENDERING)
			FT_Library_SetLcdFilter(gFTLibrary, FT_LCD_FILTER_DEFAULT);
		#endif
	}

	EATextCollationInit();
}


EATEXT_API void Shutdown()
{
	if(gFTLibrary != NULL)
	{
		FT_Done_FreeType_2(gFTLibrary);
		gFTLibrary = NULL;
	}

	#if EATEXT_EFFECTS_ENABLED
		OutlineFont::ResetEffectBitmaps();
	#endif

	#if EATEXT_FONT_SERVER_ENABLED
		EA::Text::ShutdownFontServer();
	#endif

	#if EATEXT_LAYOUT_ENABLED
		EA::Text::ShutdownLayout();
	#endif

	#if EATEXT_STYLE_MANAGER_ENABLED
		EA::Text::ShutdownStyleManager();
	#endif

	EATextCollationShutdown();
}


EATEXT_API bool operator<(const Point& a, const Point& b)
{
	// Comparisons go haywire if any values are NANs.
	// EA_ASSERT(!IsNAN(a.mX) && !IsNAN(b.mX) && !IsNAN(a.mY) && !IsNAN(b.mY));

	// We treat a point as an arbitrary array of two floats and don't 
	// treat them as spatial entities. All operator < cares about for STL
	// is that there be a consistent ordering (e.g. a < b, b < c, thus a < c).

	return (a.mX == b.mX) ? (a.mY < b.mY) : (a.mX < b.mX);
}


EATEXT_API bool operator<(const Rectangle& a, const Rectangle& b)
{
	// We treat a rectangle as an arbitrary array of four floats and don't 
	// treat them as spatial entities. All operator < cares about for STL
	// is that there be a consistent ordering (e.g. a < b, b < c, thus a < c).

	if(a.mLeft != b.mLeft)
		return (a.mLeft < b.mLeft);
	if(a.mTop != b.mTop)
		return (a.mTop < b.mTop);
	if(a.mRight != b.mRight)
		return (a.mRight < b.mRight);
	return (a.mBottom < b.mBottom);
}


} // namespace Text

} // namespace EA





#if EATEXT_FONT_SERVER_ENABLED
	#include <EAText/EATextFontServer.h>
	#include <coreallocator/icoreallocatormacros.h>

	namespace EA
	{
		namespace Text
		{
			static FontServer* gpFontServer = NULL;
			static FontServer* gpFontServerAllocated = NULL;

			EATEXT_API FontServer* GetFontServer(bool bCreateIfNotFound)
			{
				if(!gpFontServer && bCreateIfNotFound)
				{
					if(!gpFontServerAllocated)  
					{
						if(!gpCoreAllocator)
							gpCoreAllocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();

						gpFontServerAllocated = CORE_NEW(gpCoreAllocator, EATEXT_ALLOC_PREFIX "FontServer", 0) FontServer;
					}

					gpFontServer = gpFontServerAllocated;
					gpFontServer->Init();
				}

				return gpFontServer;
			}


			EATEXT_API FontServer* SetFontServer(FontServer* pFontServer)
			{
				FontServer* const pSaved = gpFontServer;

				if(pFontServer != gpFontServer)
				{
					gpFontServer = pFontServer;

					if(pSaved == gpFontServerAllocated && gpFontServerAllocated != NULL)
					{
						gpFontServerAllocated->Shutdown();
						CORE_DELETE(gpCoreAllocator, gpFontServerAllocated);
						gpFontServerAllocated = NULL;
					}
				}

				return pSaved;
			}

			// Shuts down and possibly destroys the currently registered FontServer.
			EATEXT_API void ShutdownFontServer()
			{
				// Shutdown/destroy the font server only if it's the one we allocated.
				if(gpFontServerAllocated)
				{
					if(gpFontServer == gpFontServerAllocated)
						gpFontServer = NULL;

					gpFontServerAllocated->Shutdown();

					CORE_DELETE(gpCoreAllocator, gpFontServerAllocated);
					gpFontServerAllocated = NULL;
				}
				else
				{
					// To consider: Call Shutdown on the global font server that the user created and registered.
				}
			}

			// The following auto-destroyer code is deprecated. Instead the EAText::Shutdown 
			// functionality should be used.
			struct FontServerDestroyer
			{
				~FontServerDestroyer()
					{ ShutdownFontServer(); }
			};

			static FontServerDestroyer sFontServerDestroyer;

		} // namespace Text

	} // namespace EA

#endif // #if EATEXT_FONT_SERVER_ENABLED



#if EATEXT_LAYOUT_ENABLED

	///////////////////////////////////////////////////////////////////////////////
	// GetDefaultLayout / SetDefaultLayout
	//
	namespace EA
	{
		namespace Text
		{
			static const Layout* gpDefaultLayout = NULL;

			EATEXT_API const Layout* GetDefaultLayout(bool bCreateIfNotFound)
			{
				static const Layout sDefaultLayout;

				if(!gpDefaultLayout && bCreateIfNotFound)
					gpDefaultLayout = &sDefaultLayout;
				return gpDefaultLayout;
			}

			EATEXT_API const Layout* SetDefaultLayout(const Layout* pLayout)
			{
				const Layout* const pSaved = gpDefaultLayout;
				gpDefaultLayout = pLayout;
				return pSaved;
			}

		} // namespace Text

	} // namespace EA

#endif



#if EATEXT_STYLE_MANAGER_ENABLED
	#include <EAText/EATextStyle.h>

	namespace EA
	{
		namespace Text
		{
			///////////////////////////////////////////////////////////////////////////////
			// GetStyleManager / SetStyleManager
			//
			static StyleManager* gpStyleManager = NULL;

			// This is merely an accessor for a local static sStyleManager.
			static StyleManager* GetDefaultStyleManager()
			{
				// To consider: Make this be like the FontServer case above and be dynamically allocated.
				static StyleManager sStyleManager; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				return &sStyleManager;
			}

			EATEXT_API StyleManager* GetStyleManager(bool bCreateIfNotFound)
			{
				if(!gpStyleManager && bCreateIfNotFound)
					gpStyleManager = GetDefaultStyleManager();
				return gpStyleManager;
			}

			EATEXT_API StyleManager* SetStyleManager(StyleManager* pStyleManager)
			{
				StyleManager* const pSaved = gpStyleManager;
				gpStyleManager = pStyleManager;
				return pSaved;
			}

			// Shuts down and destroys the currently registered StyleManager.
			EATEXT_API void ShutdownStyleManager()
			{
				StyleManager* pDefaultStyleManager = GetDefaultStyleManager();
				if(pDefaultStyleManager) // Should always be true, as things currently are.
					pDefaultStyleManager->Shutdown();

				// To consider: Call Shutdown on the global style manager that the user created and registered.
				//StyleManager* pStyleManager = GetStyleManager(false);
				//if(pStyleManager && (pStyleManager != pDefaultStyleManager)) // If valid and not already handled above...
				//    pStyleManager->Shutdown();
			}

		} // namespace Text

	} // namespace EA


#endif // EATEXT_STYLE_MANAGER_ENABLED
