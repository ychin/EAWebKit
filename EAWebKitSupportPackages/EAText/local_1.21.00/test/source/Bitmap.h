/////////////////////////////////////////////////////////////////////////////
// Bitmap.h
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// By Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "EABase/eabase.h"


namespace EA
{
	namespace Font
	{

		class Bitmap
		{
		public:
			Bitmap(int w = 0, int h = 0);
		   ~Bitmap();

			bool      Init(int w, int h);
			bool      Shutdown();

			int       GetWidth() const;
			int       GetHeight() const;
			int       GetStride() const;
			uint32_t* GetData() const;

			uint32_t  GetPixel(int x, int y) const;
			uint32_t  GetPixelClamped(int x, int y, uint32_t cOutOfBounds) const;
			uint32_t  GetPixelInterpolatedFloat(float x, float y, uint32_t cOutOfBounds) const;
			void      SetPixel(int x, int y, uint32_t c);
			void      SetPixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b);

			void      Blit(const uint32_t* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY);
			void      Blit(const Bitmap* pSourceBitmap, int destX, int destY);
			void      Fill(uint32_t c);
			void      FillRect(uint32_t c, int x, int y, int width, int height);
			void      TransferFrom(Bitmap* pSourceBitmap);
			void      Swap(Bitmap* pSourceBitmap);

		public:
			uint32_t* mpData;       /// ARGB (A = high byte, B = low byte).
			int       mnWidth;      /// Bitmap row width in pixels.
			int       mnHeight;     /// Bitmap column height in pixels.
			int       mnStride;     /// Bitmap row width in bytes.
		};


	} // namespace Font

} // namespace EA















