/////////////////////////////////////////////////////////////////////////////
// TestFontFusion.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextFont.h>
#include <EAText/EATextFontServer.h>
#include <EAStdC/EAStopwatch.h>
#include <eathread/eathread.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileStream.h>
#include <MemoryMan/MemoryMan.h>
#include <EATest/EATest.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

EA_DISABLE_VC_WARNING(4464) // relative include path contains '..' (compiling source file W : \dev\packages\EAText\dev\test\source\TestFreeType.cpp)
#include <ft2build.h>
#include FT_FREETYPE_H
EA_RESTORE_VC_WARNING()

using namespace EA::Text;


int TestFreeType()
{
	int nErrorCount = 0;

	// This code is based on the FreeType example1.c file that comes with FreeType.
	FilePathChar pFontDirectory[EA::IO::kMaxPathLength];
		
	GetApplicationFontDirectory(pFontDirectory, EA::IO::kMaxPathLength);
	EA::StdC::Strcat(pFontDirectory, EATEXT_FILE_PATH_CHAR("micross.ttf"));

	EA::IO::FileStream fileStream(pFontDirectory);

	if(fileStream.Open(EA::IO::kAccessFlagRead))
	{
		const size_t nFileSize = (size_t)fileStream.GetSize();

		uint8_t* const pData = EA_NEW("EATextTest/FileStream") uint8_t[nFileSize];

		if(fileStream.Read(pData, nFileSize) == nFileSize)
		{
			FT_Library    library;
			FT_Face       face;
			FT_GlyphSlot  slot;
			FT_Matrix     matrix;                 // transformation matrix
			FT_Vector     pen;                    // untransformed origin
			FT_Error      error;
			char*         text;
			double        angle;
			int           target_height;
			int           n, num_chars;
			#define       WIDTH   640
			#define       HEIGHT  480

			text          = const_cast<char*>("Hello world");
			num_chars     = (int)strlen( text );
			angle         = ( 25.0 / 360 ) * 3.14159 * 2;      // use 25 degrees
			target_height = HEIGHT;

			error = FT_Init_FreeType( &library );              // initialize library
			// error handling omitted

			error = FT_New_Memory_Face( library, (FT_Byte*)pData, (FT_Long)nFileSize, 0, &face ); // create face object
			// error handling omitted

			// use 50pt at 100dpi
			error = FT_Set_Char_Size( face, 50 * 64, 0, 100, 0 ); // set character size
			// error handling omitted

			slot = face->glyph;

			// set up matrix
			matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
			matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
			matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
			matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

			// the pen position in 26.6 cartesian space coordinates;
			// start at (300,200) relative to the upper left corner
			pen.x = 300 * 64;
			pen.y = ( target_height - 200 ) * 64;

			for ( n = 0; n < num_chars; n++ )
			{
				// set transformation
				FT_Set_Transform( face, &matrix, &pen );

				// load glyph image into the slot (erase previous one)
				error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
				if ( error )
					continue;                 // ignore errors

				// now, draw to our target surface (convert position)
				// draw_bitmap( &slot->bitmap, slot->bitmap_left, target_height - slot->bitmap_top );

				// increment pen position
				pen.x += slot->advance.x;
				pen.y += slot->advance.y;
			}


			// Small benchmark
			FT_Set_Transform(face, NULL, NULL);
			FT_Set_Char_Size(face, 16 * 64, 0, 96, 0);  // 16 point size at 96 dpi.

			const int threadPriority = EA::Thread::GetThreadPriority();
			EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault + 2);
			EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsCycles, true);

			for(int i = 0; i < 64; i++)
			{
				FT_Load_Glyph(face, i, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL);
				FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			}

			stopwatch.Stop();
			EA::Thread::SetThreadPriority(threadPriority);
			EA::UnitTest::ReportVerbosity(1, "FreeType mini glyph render benchmark cycles: %I64u\n", stopwatch.GetElapsedTime());


			// Shutdown the font
			FT_Done_Face    ( face );
			FT_Done_FreeType( library );
		}

		delete[] pData;
	}

	return nErrorCount;
}
