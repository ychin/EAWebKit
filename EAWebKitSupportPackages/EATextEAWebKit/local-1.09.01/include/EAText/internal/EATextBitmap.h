/*
Copyright (C) 2007, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

/////////////////////////////////////////////////////////////////////////////
// EATextBitmap.h
// By Paul Pedriana - 2007
/////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTBITMAP_H
#define EATEXT_INTERNAL_EATEXTBITMAP_H


#include <EABase/eabase.h>
#include EA_ASSERT_HEADER


#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }


    namespace Text
    {

        ///////////////////////////////////////////////////////////////////////
        // Bitmap32
        //
        // Implements a 32 bpp pixel map. Typically it is ARGB.
        // The coordinate system for the bitmap is assumed to be one with the 
        // origin (0,0) being at the top-left of the image at pixel address 0 and 
        // the end (w,h) being at the bottom-rigth of the image at pixel address
        // (capacity-1).
        //
        class EATEXT_API Bitmap32
        {
        public:
            typedef uint32_t Pixel;

            Bitmap32(Allocator::ICoreAllocator* pCoreAllocator = NULL, int w = 0, int h = 0);
           ~Bitmap32();

            bool   Init(int w, int h);
            void   Shutdown();

            void                       SetAllocator(Allocator::ICoreAllocator* pAllocator);
            Allocator::ICoreAllocator* GetAllocator();

            bool   SetSize(int w, int h);
            void   Acquire(Pixel* pData, int w, int h, int stride);

            int    GetWidth() const;
            int    GetHeight() const;
            int    GetStride() const;
            Pixel* GetData() const;

            Pixel* GetPixelPtr(int x, int y) const;
            Pixel* GetPixelPtrChecked(int x, int y) const;
            Pixel  GetPixel(int x, int y) const;
            Pixel  GetPixelClamped(int x, int y) const;
            Pixel  GetPixelClamped(int x, int y, Pixel cOutOfBounds) const;
            void   SetPixel(int x, int y, Pixel c);
            void   SetPixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b);

            void   Blit(const Pixel* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY);
            void   Blit(const Bitmap32* pSourceBitmap, int destX, int destY);
            void   Clear();
            void   Fill(Pixel c);
            void   TransferFrom(Bitmap32* pSourceBitmap);
            void   Swap(Bitmap32* pSourceBitmap);

            #if EATEXT_DEBUG
                bool WritePPMFile(const char* pPath, Pixel backgroundColor = 0xff000000);
            #endif

        public:
            Pixel* mpData;       /// ARGB (A = high byte, B = low byte).
            int    mnWidth;      /// Bitmap32 row width in pixels.
            int    mnHeight;     /// Bitmap32 column height in pixels.
            int    mnStride;     /// Bitmap32 row width in bytes.
            int    mnCapacity;   /// Number of pixels we can hold, equal to the number of Pixel elements allcoated to mpData.
            Allocator::ICoreAllocator* mpCoreAllocator;
        };


        ///////////////////////////////////////////////////////////////////////
        // Bitmap8
        //
        // Implements an 8 bpp gray map, transparency map, or luminescence map.
        // The coordinate system for the bitmap is assumed to be one with the 
        // origin (0,0) being at the top-left of the image at pixel address 0 and 
        // the end (w,h) being at the bottom-rigth of the image at pixel address
        // (capacity-1).
        //
        class EATEXT_API Bitmap8
        {
        public:
            typedef uint8_t Pixel;

            Bitmap8(Allocator::ICoreAllocator* pCoreAllocator = NULL, int w = 0, int h = 0);
           ~Bitmap8();

            bool   Init(int w, int h);
            void   Shutdown();

            void                       SetAllocator(Allocator::ICoreAllocator* pAllocator);
            Allocator::ICoreAllocator* GetAllocator();

            bool   SetSize(int w, int h);
            void   Acquire(Pixel* pData, int w, int h, int stride);

            int    GetWidth() const;
            int    GetHeight() const;
            int    GetStride() const;
            Pixel* GetData() const;

            Pixel* GetPixelPtr(int x, int y) const;
            Pixel* GetPixelPtrChecked(int x, int y) const;
            Pixel  GetPixel(int x, int y) const;
            Pixel  GetPixelClamped(int x, int y, Pixel cOutOfBounds) const;
            void   SetPixel(int x, int y, Pixel c);

            void   Blit(const Pixel* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY);
            void   Blit1BPP(const uint8_t* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY);
            void   Blit(const Bitmap8* pSourceBitmap, int destX, int destY);
            void   Clear();
            void   Fill(uint8_t c);
            void   CopyFrom(const Bitmap8* pBitmap8);

            #if EATEXT_DEBUG
                bool WritePPMFile(const char* pPath, Pixel backgroundColor = 0x00);
            #endif

        public:
            Pixel* mpData;       /// ARGB (A = high byte, B = low byte).
            int    mnWidth;      /// Bitmap32 row width in pixels.
            int    mnHeight;     /// Bitmap32 column height in pixels.
            int    mnStride;     /// Bitmap32 row width in bytes.
            int    mnCapacity;   /// Number of pixels we can hold, equal to the number of Pixel elements allcoated to mpData.
            Allocator::ICoreAllocator* mpCoreAllocator;
        };


    } // namespace Text

} // namespace EA






///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace Text
    {
        ///////////////////////////////////////////////////////////////////////
        // Bitmap32
        ///////////////////////////////////////////////////////////////////////

        inline Bitmap32::~Bitmap32()
        {
            Shutdown();
        }

        inline void Bitmap32::SetAllocator(Allocator::ICoreAllocator* pAllocator)
        {
            EA_ASSERT(!mpData || (pAllocator == mpCoreAllocator)); // Assert that if the user is changing the allocator then we don't have an outstanding allocation with the previous allocator.
            //if(mpData && (pAllocator == mpCoreAllocator))        // To consider: Enable this.
            //    Shutdown();
            mpCoreAllocator = pAllocator;
        }

        inline Allocator::ICoreAllocator* Bitmap32::GetAllocator()
        {
            return mpCoreAllocator;
        }

        inline int Bitmap32::GetWidth() const
        {
            return mnWidth;
        }


        inline int Bitmap32::GetHeight() const
        {
            return mnHeight;
        }


        inline int Bitmap32::GetStride() const
        {
            return mnStride;
        }


        inline Bitmap32::Pixel* Bitmap32::GetData() const
        {
            return mpData;
        }


        inline Bitmap32::Pixel* Bitmap32::GetPixelPtr(int x, int y) const
        {
            EA_ASSERT((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight));
            return mpData + (y * mnWidth) + x;
        }


        inline Bitmap32::Pixel* Bitmap32::GetPixelPtrChecked(int x, int y) const
        {
            if((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight))
                return mpData + (y * mnWidth) + x;
            return NULL;
        }


        inline Bitmap32::Pixel Bitmap32::GetPixel(int x, int y) const
        {
            const Pixel* const pPixel = GetPixelPtr(x, y);
            return *pPixel;
        }


        inline void Bitmap32::SetPixel(int x, int y, Pixel c)
        {
            Pixel* const pPixel = GetPixelPtr(x, y);
            *pPixel = c;
        }


        inline void Bitmap32::SetPixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
        {
            Pixel* const pPixel = GetPixelPtr(x, y);
            const Pixel c = (Pixel)((a << 24) | (r << 16) | (g << 8) | (b));
            *pPixel = c;
        }


        inline void Bitmap32::Blit(const Bitmap32* pSourceBitmap, int destX, int destY)
        {
            Blit(pSourceBitmap->mpData, 0, 0, pSourceBitmap->mnWidth, pSourceBitmap->mnHeight, pSourceBitmap->mnStride, destX, destY);
        }



        ///////////////////////////////////////////////////////////////////////
        // Bitmap8
        ///////////////////////////////////////////////////////////////////////

        inline Bitmap8::~Bitmap8()
        {
            Shutdown();
        }

        inline void Bitmap8::SetAllocator(Allocator::ICoreAllocator* pAllocator)
        {
            EA_ASSERT(!mpData || (pAllocator == mpCoreAllocator)); // Assert that if the user is changing the allocator then we don't have an outstanding allocation with the previous allocator.
            //if(mpData && (pAllocator == mpCoreAllocator))        // To consider: Enable this.
            //    Shutdown();
            mpCoreAllocator = pAllocator;
        }

        inline Allocator::ICoreAllocator* Bitmap8::GetAllocator()
        {
            return mpCoreAllocator;
        }

        inline int Bitmap8::GetWidth() const
        {
            return mnWidth;
        }


        inline int Bitmap8::GetHeight() const
        {
            return mnHeight;
        }


        inline int Bitmap8::GetStride() const
        {
            return mnStride;
        }


        inline Bitmap8::Pixel* Bitmap8::GetData() const
        {
            return mpData;
        }


        inline Bitmap8::Pixel* Bitmap8::GetPixelPtr(int x, int y) const
        {
            EA_ASSERT((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight));
            return mpData + (y * mnWidth) + x;
        }


        inline Bitmap8::Pixel* Bitmap8::GetPixelPtrChecked(int x, int y) const
        {
            if((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight))
                return mpData + (y * mnWidth) + x;
            return NULL;
        }


        inline Bitmap8::Pixel Bitmap8::GetPixel(int x, int y) const
        {
            const Pixel* const pPixel = GetPixelPtr(x, y);
            return *pPixel;
        }


        inline void Bitmap8::SetPixel(int x, int y, Pixel c)
        {
            Pixel* const pPixel = GetPixelPtr(x, y);
            *pPixel = c;
        }


        inline void Bitmap8::Blit(const Bitmap8* pSourceBitmap, int destX, int destY)
        {
            Blit(pSourceBitmap->mpData, 0, 0, pSourceBitmap->mnWidth, pSourceBitmap->mnHeight, pSourceBitmap->mnStride, destX, destY);
        }


    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_INTERNAL_EATEXTBITMAP_H



