/////////////////////////////////////////////////////////////////////////////
// Bitmap.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// By Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "Bitmap.h"
#include <stdlib.h>
#include <EASTL/algorithm.h>
#include <math.h>
#include <assert.h>



namespace EA
{

namespace Font
{


Bitmap::Bitmap(int w, int h)
  : mpData(NULL), mnWidth(w), mnHeight(h), mnStride(w)
{
	Init(w, h);
}

Bitmap::~Bitmap()
{
	Shutdown();
}

bool Bitmap::Init(int w, int h)
{
	Shutdown();

	if((w > 0) && (h > 0))
	{
		mpData   = new uint32_t[w * h];
		mnWidth  = w;
		mnHeight = h;
		mnStride = w * sizeof(uint32_t);

		Fill(0);
	}

	return true;
}

bool Bitmap::Shutdown()
{
	delete[] mpData;
	mpData  = NULL;
	mnWidth = 0;
	mnHeight = 0;
	mnStride = 0;

	return true;
}

int Bitmap::GetWidth() const
{
	return mnWidth;
}

int Bitmap::GetHeight() const
{
	return mnHeight;
}

int Bitmap::GetStride() const
{
	return mnStride;
}

uint32_t* Bitmap::GetData() const
{
	return mpData;
}

uint32_t Bitmap::GetPixel(int x, int y) const
{
	assert(x > 0);
	assert(y > 0);
	assert(x < mnWidth);
	assert(y < mnHeight);

	return mpData[(y * mnWidth) + x];
}

uint32_t Bitmap::GetPixelClamped(int x, int y, uint32_t cOutOfBounds) const
{
	if((x < 0) || (y < 0) || (x >= mnWidth) || (y >= mnHeight))
		return cOutOfBounds;
	
	return mpData[(y * mnWidth) + x];
}


///////////////////////////////////////////////////////////////////////////////
// GetPixelInterpolatedFloat
//
// Example command line for test of unstretching for testing this function:
//    -Source:"C:\windows\fonts\arial.ttf" -Output:C:\temp\arial.bmpFont -Size:16 -Color:0,0,0 -Smoothing:yes -UnstretchW:.75 -CharFile:"C:\Temp\ASCII Characters.txt"
//
uint32_t Bitmap::GetPixelInterpolatedFloat(float x, float y, uint32_t cOutOfBounds) const
{
	// We implement bilinear filtering here. This is just an averaging of the ARGB value in 
	// a way that takes into account fractional pixel contribution.
	// We don't need this function to be fast, so the code is written for clarity more
	// than it is written for execution speed.

	int   xFloor    = (int)floorf(x);
	int   yFloor    = (int)floorf(y);
	float xRatio    = x - xFloor;
	float yRatio    = y - yFloor;
	float xInvRatio = 1 - xRatio;
	float yInvRatio = 1 - yRatio;

	uint32_t c00 = GetPixelClamped(xFloor    , yFloor    , cOutOfBounds);
	uint32_t c01 = GetPixelClamped(xFloor    , yFloor + 1, cOutOfBounds);
	uint32_t c10 = GetPixelClamped(xFloor + 1, yFloor    , cOutOfBounds);
	uint32_t c11 = GetPixelClamped(xFloor + 1, yFloor + 1, cOutOfBounds);

	const uint8_t srcColorsUint8[4][4] =
	{
		// A                     R                     G                    B
		{ (uint8_t)(c00 >> 24), (uint8_t)(c00 >> 16), (uint8_t)(c00 >> 8), (uint8_t)(c00 >> 0) }, // c00
		{ (uint8_t)(c01 >> 24), (uint8_t)(c01 >> 16), (uint8_t)(c01 >> 8), (uint8_t)(c01 >> 0) }, // c01
		{ (uint8_t)(c10 >> 24), (uint8_t)(c10 >> 16), (uint8_t)(c10 >> 8), (uint8_t)(c10 >> 0) }, // c10
		{ (uint8_t)(c11 >> 24), (uint8_t)(c11 >> 16), (uint8_t)(c11 >> 8), (uint8_t)(c11 >> 0) }  // c11
	};

	const float srcColorsFloat[4][4] =
	{
		// A R G B
		{ srcColorsUint8[0][0] / 255.f, srcColorsUint8[0][1] / 255.f, srcColorsUint8[0][2] / 255.f, srcColorsUint8[0][3] / 255.f }, // c00
		{ srcColorsUint8[1][0] / 255.f, srcColorsUint8[1][1] / 255.f, srcColorsUint8[1][2] / 255.f, srcColorsUint8[1][3] / 255.f }, // c01
		{ srcColorsUint8[2][0] / 255.f, srcColorsUint8[2][1] / 255.f, srcColorsUint8[2][2] / 255.f, srcColorsUint8[2][3] / 255.f }, // c10
		{ srcColorsUint8[3][0] / 255.f, srcColorsUint8[3][1] / 255.f, srcColorsUint8[3][2] / 255.f, srcColorsUint8[3][3] / 255.f }  // c11
	};

	float dstChannelsFloat[4] = 
	{
		((srcColorsFloat[0][0] * xInvRatio  + srcColorsFloat[2][0] * xRatio) * yInvRatio) +     // A
		((srcColorsFloat[1][0] * xInvRatio  + srcColorsFloat[3][0] * xRatio) * yRatio),

		((srcColorsFloat[0][1] * xInvRatio  + srcColorsFloat[2][1] * xRatio) * yInvRatio) +     // R
		((srcColorsFloat[1][1] * xInvRatio  + srcColorsFloat[3][1] * xRatio) * yRatio),

		((srcColorsFloat[0][2] * xInvRatio  + srcColorsFloat[2][2] * xRatio) * yInvRatio) +     // G
		((srcColorsFloat[1][2] * xInvRatio  + srcColorsFloat[3][2] * xRatio) * yRatio),

		((srcColorsFloat[0][3] * xInvRatio  + srcColorsFloat[2][3] * xRatio) * yInvRatio) +     // B
		((srcColorsFloat[1][3] * xInvRatio  + srcColorsFloat[3][3] * xRatio) * yRatio)
	};

	// We want to make sure no roundoff error occurs which puts a value over 1.0
	float dstChannelsFloatClamped[4] = 
	{
		((dstChannelsFloat[0] <= 1.f) ? dstChannelsFloat[0] : 1.f),     // A
		((dstChannelsFloat[1] <= 1.f) ? dstChannelsFloat[1] : 1.f),     // R
		((dstChannelsFloat[2] <= 1.f) ? dstChannelsFloat[2] : 1.f),     // G
		((dstChannelsFloat[3] <= 1.f) ? dstChannelsFloat[3] : 1.f)      // B
	};

	const uint8_t destChannelsUint8[4] =
	{
		(uint8_t)(dstChannelsFloatClamped[0] * 255.f),    // A
		(uint8_t)(dstChannelsFloatClamped[1] * 255.f),    // R
		(uint8_t)(dstChannelsFloatClamped[2] * 255.f),    // G
		(uint8_t)(dstChannelsFloatClamped[3] * 255.f)     // B
	};

	const uint32_t dstColor = (destChannelsUint8[0] << 24) | (destChannelsUint8[1] << 16) | (destChannelsUint8[2] << 8) | destChannelsUint8[3];

	return dstColor;
}


void Bitmap::SetPixel(int x, int y, uint32_t c)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < mnWidth);
	assert(y < mnHeight);

	mpData[(y * mnWidth) + x] = c;
}

void Bitmap::SetPixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < mnWidth);
	assert(y < mnHeight);

	uint32_t c = (a << 24) | (r << 16) | (g << 8) | (b);
	mpData[(y * mnWidth) + x] = c;
}

void Bitmap::Blit(const uint32_t* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY)
{
	assert(sourceX >= 0);
	assert(sourceY >= 0);
	assert(destX >= 0);
	assert(destY >= 0);
	assert((destX + width) <= mnWidth);
	assert((destY + height) <= mnHeight);

	for(int sx = sourceX, sxEnd = sourceX + width, dx = destX; sx < sxEnd; sx++, dx++)
	{
		for(int sy = sourceY, syEnd = sourceY + height, dy = destY; sy < syEnd; sy++, dy++)
		{
			uint32_t c = pSourceBitmap[(sy * (sourceStride / sizeof(uint32_t))) + sx];
			mpData[(dy * (mnStride / sizeof(uint32_t))) + dx] = c;
		}
	}
}

void Bitmap::Blit(const Bitmap* pSourceBitmap, int destX, int destY)
{
	Blit(pSourceBitmap->mpData, 0, 0, pSourceBitmap->mnWidth, pSourceBitmap->mnHeight, pSourceBitmap->mnStride, destX, destY);
}


void Bitmap::Fill(uint32_t c)
{
	for(int y = 0; y < mnHeight; y++)
	{
		for(int x = 0; x < mnWidth; x++)
			mpData[(y * (mnStride / sizeof(uint32_t))) + x] = c;
	}
}


void Bitmap::FillRect(uint32_t c, int x, int y, int width, int height)
{
	assert(x >= 0);
	assert(y >= 0);
	assert((x + width) <= mnWidth);
	assert((y + height) <= mnHeight);

	for(int dx = x; dx < x + width; dx++)
	{
		for(int dy = y; dy < y + height; dy++)
			mpData[(dy * (mnStride / sizeof(uint32_t))) + dx] = c;
	}
}


void Bitmap::TransferFrom(Bitmap* pSourceBitmap)
{
	Shutdown();

	mpData   = pSourceBitmap->mpData;
	mnWidth  = pSourceBitmap->mnWidth;
	mnHeight = pSourceBitmap->mnHeight;
	mnStride = pSourceBitmap->mnStride;

	pSourceBitmap->mpData   = NULL;
	pSourceBitmap->mnWidth  = 0;
	pSourceBitmap->mnHeight = 0;
	pSourceBitmap->mnStride = 0;
}


void Bitmap::Swap(Bitmap* pSourceBitmap)
{
	eastl::swap(mpData,   pSourceBitmap->mpData);
	eastl::swap(mnWidth,  pSourceBitmap->mnWidth);
	eastl::swap(mnHeight, pSourceBitmap->mnHeight);
	eastl::swap(mnStride, pSourceBitmap->mnStride);
}



} // namespace Font

} // namespace EA
















