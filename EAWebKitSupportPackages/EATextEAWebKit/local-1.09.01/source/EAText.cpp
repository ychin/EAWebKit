/*
Copyright (C) 2004, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EAText.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EAText.h>
#include <EAText/EATextLayout.h>
#include <EAText/EATextOutlineFont.h>
#include <coreallocator/icoreallocator_interface.h>
#include EA_ASSERT_HEADER

#if EATEXT_LAYOUT_ENABLED
    #include <EAText/EATextLayout.h>
#endif

#if EATEXT_USE_FREETYPE
    #include <ft2build.h>
    #include FT_FREETYPE_H  // #include <freetype/freetype.h>

    #include <freetype/internal/internal.h>
    #include <freetype/internal/ftobjs.h>
#endif


namespace EA
{
namespace Text
{


// Global variable declarations.
EA::Allocator::ICoreAllocator* gpCoreAllocator = NULL;

AssertionFailureFunction gpAssertionFailureFunction        = NULL;
void*                    gpAssertionFailureFunctionContext = NULL;

#if EATEXT_USE_FREETYPE
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
#endif



#if EATEXT_USE_FREETYPE 

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
            FT_Done_Memory(memory);
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
#endif



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
    #if EATEXT_USE_FREETYPE
        if(gFTLibrary == NULL)
        {
            FT_MemoryRec_ memory = { gpCoreAllocator, FTAlloc, FTFree, FTRealloc };
            FT_Error error = FT_Init_FreeType_2((FT_Library*)&gFTLibrary, &memory); // Question: does casting gFTLibrary to non-volatile create a C/C++ 'strict aliasing' problem?
            EA_ASSERT(error == 0); (void)error;
        }
    #endif
}


EATEXT_API void Shutdown()
{
    #if EATEXT_USE_FREETYPE
        if(gFTLibrary != NULL)
        {
            FT_Done_FreeType_2(gFTLibrary);
            gFTLibrary = NULL;
        }
    #endif

    #if EATEXT_EFFECTS_ENABLED
        OutlineFont::ResetEffectBitmaps();
    #endif

    #if EATEXT_LAYOUT_ENABLED
        EA::Text::ShutdownLayout();
    #endif
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

            FontServer* GetFontServer(bool bCreateIfNotFound)
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


            FontServer* SetFontServer(FontServer* pFontServer)
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


            struct FontServerDestroyer
            {
                ~FontServerDestroyer()
                {
                    if(gpFontServerAllocated)
                    {
                        if(gpFontServer == gpFontServerAllocated)
                            gpFontServer = NULL;

                        gpFontServerAllocated->Shutdown();
                        CORE_DELETE(gpCoreAllocator, gpFontServerAllocated);
                        gpFontServerAllocated = NULL;
                    }
                }
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

            const Layout* GetDefaultLayout(bool bCreateIfNotFound)
            {
                static const Layout sDefaultLayout;

                if(!gpDefaultLayout && bCreateIfNotFound)
                    gpDefaultLayout = &sDefaultLayout;
                return gpDefaultLayout;
            }

            const Layout* SetDefaultLayout(const Layout* pLayout)
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

            StyleManager* GetStyleManager(bool bCreateIfNotFound)
            {
                if(!gpStyleManager && bCreateIfNotFound)
                {
                    static StyleManager sStyleManager; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
                    gpStyleManager = &sStyleManager;
                }
                return gpStyleManager;
            }

            StyleManager* SetStyleManager(StyleManager* pStyleManager)
            {
                StyleManager* const pSaved = gpStyleManager;
                gpStyleManager = pStyleManager;
                return pSaved;
            }

        } // namespace Text

    } // namespace EA


#endif // EATEXT_STYLE_MANAGER_ENABLED
















