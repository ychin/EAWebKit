///////////////////////////////////////////////////////////////////////////////
// EATextOutlineFont.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// FreeType Settings
// 
//     Setting                                      Explanation
//     ------------------------------------------------------------------------
//     FT_CONFIG_OPTION_SUBPIXEL_RENDERING          Needed if you want to do LCD-specific smoothing. Note that there are Microsoft patent issues related to this.
//     TT_CONFIG_OPTION_BYTECODE_INTERPRETER        Needed if you want to use hinting present in TrueType-based fonts.
//     TT_CONFIG_OPTION_UNPATENTED_HINTING          Needed if you want to draw certain unusual Asian fonts but you have TT_CONFIG_OPTION_BYTECODE_INTERPRETER disabled. 
//
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextScript.h>
#include <EAText/EATextFontServer.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAEndian.h>
#include <EAIO/EAStreamFixedMemory.h>
#include <coreallocator/icoreallocatormacros.h>
#include <math.h>
#include EA_ASSERT_HEADER

#if EATEXT_EFFECTS_ENABLED
	#include <EAText/EATextEffects.h>
#endif

EA_DISABLE_VC_WARNING(4464) // relative include path contains '..' (compiling source file W : \dev\packages\EAText\dev\test\source\TestFreeType.cpp)
#include <ft2build.h>

#include FT_FREETYPE_H        // #include <freetype/freetype.h>
#include FT_SIZES_H           // #include <freetype/ftsizes.h>
#include FT_SFNT_NAMES_H      // #include <freetype/ftsnames.h>
#include FT_TRUETYPE_IDS_H    // #include <freetype/ttnameid.h>
#include FT_TRUETYPE_TABLES_H // #include <freetype/tttables.h>
EA_RESTORE_VC_WARNING()


#define FTFontUnitsToFloat(x) (FFFixed26ToFloat(FT_MulFix((x), static_cast<FT_Int32>(mpFaceData->mpFTFace->size->metrics.x_scale))))

#define FFFloatToFixed16(x) ((int32_t)((x) * 65536.f))
#define FFFloatToFixed26(x) ((int32_t)((x) * 64.f))

#define FFFixed16ToFloat(x) (((float)(x) * (1.f / 65536.f))) // 1/65536 optimizes to an exact floating point constant, as 65536 is a power of 2.
#define FFFixed26ToFloat(x) (((float)(x) * (1.f / 64.f)))    // 1/64 optimizes to an exact floating point constant, as 65536 is a power of 2.


namespace EA
{

namespace Text
{


extern volatile FT_Library gFTLibrary;

static unsigned long FT_Stream_Io(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count)
{
	IO::IStream* const pStream = (IO::IStream*)stream->descriptor.pointer;

	if(pStream->SetPosition((IO::off_type)offset) && count)  // count is zero when FreeType wants to simply seek to a position.
	{
		const uint32_t nActualSize = (uint32_t)pStream->Read(buffer, (IO::size_type)count);

		if(nActualSize == (uint32_t)count)
			return nActualSize;
	}

	return 0;  // FreeType doesn't document what an error return value should be.
}

static void FT_Stream_Close(FT_Stream /*stream*/)
{
	// We don't close the stream here, because we manage stream 
	// lifetime at a higher level. We do this because two fonts 
	// could be sharing the same stream and we don't want one
	// font to close the stream of another font.
}



///////////////////////////////////////////////////////////////////////////////
// FaceData
///////////////////////////////////////////////////////////////////////////////

#if EATEXT_THREAD_SAFETY_ENABLED
	EA::Thread::Futex FaceData::mMutex;
#endif


FaceData::FaceData(Allocator::ICoreAllocator* pCoreAllocator)
  :
	mpFTFace(NULL),
	mpFTStream(),
	mSupportedChars(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "FaceData/SupportedChars", pCoreAllocator)),

	mpCoreAllocator(pCoreAllocator),
	mRefCount(0)
{
}


FaceData::~FaceData()
{
	if(mpFTFace)
	{
		// EA_ASSERT(gFTLibrary != NULL); // To consider: Enable this. It might be impractical to enable it due to application requirements that some fonts be freed after FreeType is shut down.
		if(gFTLibrary) // If the application hasn't shut down Freetype already...
			FT_Done_Face(mpFTFace);
		mpFTFace = NULL;
	}

	if(mpFTStream)
	{
		mpCoreAllocator->Free(mpFTStream);
		mpFTStream = NULL;
	}
}


int FaceData::AddRef()
{
	EA_ASSERT(mRefCount < 5000); // Sanity check.
	return ++mRefCount;
}


int FaceData::Release()
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		int32_t rc = mRefCount.Decrement();
		if(rc)
			return rc;
	#else
		if(mRefCount > 1)
			return --mRefCount;
	#endif

	this->~FaceData();
	mpCoreAllocator->Free(this);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// OutlineFont
///////////////////////////////////////////////////////////////////////////////

OutlineFont::OutlineFont(Allocator::ICoreAllocator* pCoreAllocator)
  : Font(pCoreAllocator),
	mFontDescription(),
	mFontMetrics(),
	mGlyphMetricsMap(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "OutlineFont/GlyphMetricsMap", mpCoreAllocator)),
	mGlyphBitmap(),
	mnRenderCount(0),
	mDPI(EATEXT_DPI),
	mScreenColorType(kSCT_Unknown),
	mbEffectsInitialized(false),
	mbFontMetricsCalculated(false),
	mbEnableHinting(true),
	mbUseAutoHinting(false),
	mbOpened(false),
	mbFontHasUsefulHinting(true),
	mpEffectsProcessor(NULL),
	mpFaceData(NULL)
{
	#if EATEXT_OPENTYPE_ENABLED
		mbOTFEnabled = false;
		mOTF.SetAllocator(mpCoreAllocator);
	#endif

	mpFTSize  = NULL;
}


OutlineFont::OutlineFont(const OutlineFont& outlineFont)
  : Font(outlineFont)
{
	operator=(outlineFont);
}


OutlineFont& OutlineFont::operator=(const OutlineFont&)
{
	EA_FAIL(); // This is not yet supported.
	return *this;
}


OutlineFont::~OutlineFont()
{
	OutlineFont::Close(); // Call OutlineFont::Close instead of just Close because Close may be a virtual function, and this is a destructor and vtables don't work in destructors.
}


void OutlineFont::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
	Font::SetAllocator(pCoreAllocator);

	mGlyphMetricsMap.get_allocator().set_allocator(pCoreAllocator);

	#if EATEXT_OPENTYPE_ENABLED
		mOTF.SetAllocator(pCoreAllocator);
	#endif
}


void OutlineFont::SetFaceData(FaceData* pFaceData)
{
	if(mpFaceData != pFaceData)
	{
		if(pFaceData)
			pFaceData->AddRef();

		if(mpFaceData)
			mpFaceData->Release();

		mpFaceData = pFaceData;
	}
}


FaceData* OutlineFont::CreateFaceData(Allocator::ICoreAllocator* pAllocator, IO::IStream* pStream, 
												 const void* pSourceData, uint32_t nSourceSize, int nFaceIndex)
{
	FaceData* pFaceData = (FaceData*)pAllocator->Alloc(sizeof(FaceData), EATEXT_ALLOC_PREFIX "FaceData", 0);

	if(pFaceData)
	{
		new(pFaceData) FaceData(pAllocator);
		pFaceData->AddRef();

		FT_Error nFTError = 0;

		EA_ASSERT(gFTLibrary != NULL);
		if(pStream && gFTLibrary) // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.
		{
			if(!pFaceData->mpFTStream)
				pFaceData->mpFTStream = (FT_Stream)pAllocator->Alloc(sizeof(struct FT_StreamRec_), EATEXT_ALLOC_PREFIX "FT_Stream", 0);

			if(pFaceData->mpFTStream)
			{
				memset(pFaceData->mpFTStream, 0, sizeof(struct FT_StreamRec_));

				pFaceData->mpFTStream->base               = NULL;
				pFaceData->mpFTStream->size               = (unsigned long)pStream->GetSize();
				pFaceData->mpFTStream->pos                = 0;
				pFaceData->mpFTStream->descriptor.pointer = pStream;
				pFaceData->mpFTStream->pathname.pointer   = NULL; // It could be useful to set this to the file name void the IStream if it's a stream.
				pFaceData->mpFTStream->read               = FT_Stream_Io;
				pFaceData->mpFTStream->close              = FT_Stream_Close;
				pFaceData->mpFTStream->memory             = NULL;
				pFaceData->mpFTStream->cursor             = NULL;
				pFaceData->mpFTStream->limit              = NULL;

				FT_Open_Args openArgs;

				// Disabled until we are sure we want it:
				// https://lists.nongnu.org/archive/html/freetype/2010-01/msg00005.html
				FT_Parameter params[] = { 
					{ FT_PARAM_TAG_IGNORE_PREFERRED_FAMILY,    0 },   // FT_PARAM_TAG_IGNORE_PREFERRED_FAMILY requires FreeType 2.4.2 or later.
					{ FT_PARAM_TAG_IGNORE_PREFERRED_SUBFAMILY, 0 } 
				};

				openArgs.flags       = FT_OPEN_STREAM;
				openArgs.memory_base = NULL;
				openArgs.memory_size = 0;
				openArgs.pathname    = NULL;
				openArgs.stream      = pFaceData->mpFTStream;
				openArgs.driver      = NULL;    // Let FreeType find what kind of font file this is, though it ought to be TrueType.
				openArgs.num_params  = EAArrayCount(params);
				openArgs.params      = params;

				nFTError = FT_Open_Face(gFTLibrary, &openArgs, nFaceIndex, &pFaceData->mpFTFace);
			}
			else
				nFTError = FT_Err_Out_Of_Memory; // The logic here is a little simpler if we assume this is a FreeType error instead of our own memory error.
		}
		else
		{
			#ifdef EA_DEBUG // We won't be using mpFTStream, so zero it in debug builds so it doesn't confuse us while looking at it in a debugger.
				memset(&pFaceData->mpFTStream, 0, sizeof(pFaceData->mpFTStream));
			#endif

			nFTError = FT_New_Memory_Face(gFTLibrary, (const FT_Byte*)pSourceData, nSourceSize, nFaceIndex, &pFaceData->mpFTFace);
		}

		// If this fails, did you forget to call EA::Text::Init() on startup?
		EA_ASSERT((nFTError == 0) && pFaceData->mpFTFace);
		if(nFTError)
		{
			pFaceData->Release();
			pFaceData = NULL;
		}
		else
		{
			// Set up mSupportedChars.
			FaceData::Bitvector& supportedChars = pFaceData->mSupportedChars;

			FT_Face& face = pFaceData->mpFTFace;

			FT_UInt      glyphIndex;
			eastl_size_t ch = (eastl_size_t)FT_Get_First_Char(face, &glyphIndex);

			while (glyphIndex)
			{
				supportedChars.set(ch, true);
				ch = (eastl_size_t)FT_Get_Next_Char(face, (FT_ULong)ch, &glyphIndex);
			}
		}
	}

	return pFaceData;
}


bool OutlineFont::Open(IO::IStream* pStream, int nFaceIndex)
{
	// We don't lock any mutex here because we assume that the owner of the font has not given 
	// an unopened font to anybody else and thus is handling any thread safety.

	bool bReturnValue;

	if(mbOpened) // If already opened...
		bReturnValue = true;
	else
	{
		bReturnValue = false;

		EA_ASSERT(pStream && pStream->GetAccessFlags());
		if(pStream && pStream->GetAccessFlags()) // If there is a stream to open and if the stream is open...
		{
			// mpFaceData will be NULL only if the user is creating a private or otherwise 
			// standalone font as opposed to creating a font managed by the FontServer. Using the 
			// FontServer is more efficient, as it allows for sharing between multiple representations
			// of a given face.

			if(!mpFaceData)
			{
				mpFaceData = CreateFaceData(mpCoreAllocator, pStream, NULL, 0, nFaceIndex); // This will AddRef the return value for the caller.
				EA_ASSERT(mpFaceData != NULL);
			}

			if(mpFaceData)
			{
				#if EATEXT_THREAD_SAFETY_ENABLED
					EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex); // Lock the mutex for the remainder of this block.
				#endif

				bReturnValue = OpenInternal(nFaceIndex);
				EA_ASSERT(bReturnValue);

				if(bReturnValue)
				{
					mpStream = pStream;
					mpStream->AddRef(); // AddRef it for our own uses.
				}

				#if EATEXT_OPENTYPE_ENABLED
					// Currently we unilaterally load the OTF (OpenType) information for the Font.
					// We ignore the mOTF.Load() return value, as a failed load doesn't mean we 
					// are inoperable; it may simply mean that the font doesn't have OpenType information.
					// OpenType is really needed only for Arabic and Hindi (Devenagari). OpenType is 
					// also useful for Thai, as it allows Thai decoration glyphs to be positioned 
					// precisely. However, EAText has basic Thai decoration positioning functionality
					// in the absence of OpenType and which is good enough in practice for most uses.
					if(bReturnValue && mbOTFEnabled)
						mOTF.Load(mpStream);
				#endif
			}
		}

		if(!bReturnValue)
			Close();

		mbOpened = bReturnValue;
	}

	return bReturnValue;
}


bool OutlineFont::Open(const void* pSourceData, uint32_t nSourceSize, int nFaceIndex)
{
	// We don't lock any mutex here because we assume that the owner of the font has not given 
	// an unopened font to anybody else and thus is handling any thread safety.

	bool bReturnValue;

	if(mbOpened) // If already opened...
		bReturnValue = true;
	else
	{
		bReturnValue = false;

		EA_ASSERT(pSourceData != NULL);
		if(pSourceData) // If there is source to open...
		{
			// mpFaceData will be NULL only if the user is creating a private or otherwise 
			// standalone font as opposed to creating a font managed by the FontServer. Using the 
			// FontServer is more efficient, as it allows for sharing between multiple representations
			// of a given face.

			if(!mpFaceData)
			{
				mpFaceData = CreateFaceData(mpCoreAllocator, NULL, pSourceData, nSourceSize, nFaceIndex); // This will AddRef the return value for the caller.
				EA_ASSERT(mpFaceData != NULL);
			}

			if(mpFaceData)
			{
				#if EATEXT_THREAD_SAFETY_ENABLED
					EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex); // Lock the mutex for the remainder of this block.
				#endif

				bReturnValue = OpenInternal(nFaceIndex);
				EA_ASSERT(bReturnValue);

				// Do don't do anything additional with pSourceData. 
				// The user is expected to keep it around for the duration of the font.

				// To consider: Enable this code. Doing so will enable OpenType support for memory-based fonts.
				//#if EATEXT_OPENTYPE_ENABLED
				//    if(bReturnValue && mbOTFEnabled)
				//    {
				//        EA::IO::FixedMemoryStream stream(pSourceData, nSourceSize);
				//        stream.AddRef();
				//        mOTF.Load(&stream);
				//    }
				//#endif
			}
		}

		if(!bReturnValue)
			Close();

		mbOpened = bReturnValue;
	}

	return bReturnValue;
}


void OutlineFont::SetOption(int32_t option, int32_t value)
{
	// We don't lock any mutex because these options must be used before the font is opened.

	switch (option)
	{
		#if EATEXT_OPENTYPE_ENABLED
		case kOptionOpenTypeFeatures:
			mbOTFEnabled = (value != 0);
			break;
		#endif

		case kOptionDPI:
			EA_ASSERT(value > 0);
			mDPI = (int16_t)value;
			break;

		case kOptionEnableHinting:
			// Note that the font might lack hinting or have useless hinting, in which case this option has no effect.
			mbEnableHinting = (value != 0);
			break;

		case kOptionUseAutoHinting:
			mbUseAutoHinting = (value != 0);
			break;

		case kOptionSubPixelAA:
			mScreenColorType = (ScreenColorType)value;
			break;

		//case kOptionEnableDirectGlyphCacheUse:
		//    // Doesn't apply to OutlineFonts
		//    break;
	}
}



// FreeType's FTFace has a family_name field that it fills in, but that field is the 
// PostScript family name and not the traditional family name. And these two are often 
// different, especially for some font collections like the Adobe collection. 
// For example, there are two font files named HelveticaLTStd-BlkCond.ttf and HelveticaLTStd-Blk.ttf.
// These files have traditional family names of "Helvetica Lt Std Blk Cond" and "Helvetica Std Blk",
// respectively. That allows us (and HTML/CSS) to recognize them as different fonts. But FreeType
// reads the PoscriptName from these and reports them both as having a family name of "Helvetica Lt Std,"
// and puts the other information in the font_style string field. This way of doing it works OK for 
// art applications like Adobe Photoshop and can work OK for operating system font systems that are
// aware of this kind of family name string / style string breakdown. But HTML CSS doesn't support 
// arbitrary style strings and so cannot be compatible with this, and instead effectively require 
// the family name to include all the information needed to identify the font, aside from italic/oblique
// properties. So what we do is ignore the FreeType font_family and read the traditional family name field instead.
// This situation has been confirmed by the FreeType development team.
// To consider: revise EAText FontDescription and FontStyle to include fields for the PostScript family
// name and style, in case we have uses that need to identify fonts this way as opposed to the traditional way.
// For more info on CSS font-family and font-style, see http://www.w3schools.com/cssref/pr_font_font-family.asp
// and http://www.w3schools.com/cssref/pr_font_font-style.asp
// For more info on TrueType and OpenType font naming, see http://www.microsoft.com/typography/otspec/name.htm

// The format of name.string depends on name.platform_id and name.encoding_id, due to the 
// fact that TrueType was invented before Unicode became significant. Theoretically we could
// have to handle hundreds of combinations of encoding.
// platform_id is always 0 (Unicode), 1 (Macintosh), or 3 (Windows).
// encoding_id is usually Unicode for modern fonts, but we should be careful to watch out for the fact that it's either 16 or 32 bit Unicode.
// The name_id field identifies what language the name is in. This is scary because it allows things to have different names depending on language. We ignore it and hope it's always English.
union String8_16_32
{
	char8_t  mString8[32];
	char16_t mString16[32];
	char32_t mString32[32];
};

static bool GetFreeTypeFontTraditionalFamilyName(FT_Face pFace, Char* familyName, size_t familyNameCapacity)
{
	FT_UInt     nameCount = FT_Get_Sfnt_Name_Count(pFace);
	FT_SfntName name;
	bool        bFound = false;
	bool        bBestMatchFound = false;
	#define     TT_PLATFORM_NONE 9999
	FT_UShort   currentBestPlatformId = TT_PLATFORM_NONE;
	FT_UInt     currentBestNameStrlen = 0;

	memset(&name, 0, sizeof(name));

	for(FT_UInt i = 0; (i < nameCount) && !bBestMatchFound; i++)
	{
		// FT_SfntName::string is not null-terminated, and the application must deallocate it when done with it.
		FT_Error nFTError = FT_Get_Sfnt_Name(pFace, i, &name);

		if(nFTError == 0)
		{
			if(name.name_id == TT_NAME_ID_FONT_FAMILY)
			{
				String8_16_32 string8_16_32;
				int           charSize  = 0;
				bool          bigEndian = false;

				// It's theoretically possible that name.string is encoded in something other than Unicode.
				// Such encodings are archaic and would create an explosion of code here in order to support.
				// So we suppport only ASCII and Unicode names.

				switch (name.platform_id)
				{
					case TT_PLATFORM_APPLE_UNICODE:
						bigEndian = true;

						if(name.encoding_id < TT_APPLE_ID_UNICODE_32)
							charSize = 2;
						else if(name.encoding_id == TT_APPLE_ID_UNICODE_32)
							charSize = 4;
						//else unsupported

						break;

					case TT_PLATFORM_MACINTOSH:
						// Until shown otherwise, it seems that all the TT_MAC_ID_XXX types are single byte. 
						// However, they are in different encodings which aren't UTF8. We ignore this for now 
						// and assume that only the single byte ASCII portion of UTF8 is used (which is almost always true for Western fonts).
						charSize = 1;
						break;

					case TT_PLATFORM_MICROSOFT:
						bigEndian = true;

						if(name.encoding_id == TT_MS_ID_UNICODE_CS)
							charSize = 2;
						else if(name.encoding_id == TT_MS_ID_UCS_4)
							charSize = 4;
						//else the type (e.g. TT_MS_ID_BIG_5) isn't supported, because it's not Unicode.

						break;

					default:
						// We ignore other types.
						// We may want to put an assert failure here, but the problem is that we really need to delay such a 
						// failure until after we've looped hrough all the name records.
						break;
				}

				if(charSize && ((name.string_len + charSize) <= sizeof(string8_16_32))) // If there is enough space for name.string plus a terminating 0 char...
				{
					const FT_UInt strLen = (name.string_len / charSize);

					// We have a little logic here to try to tell if the current name is 'better' than any previous one we have.
					if((name.platform_id == TT_PLATFORM_NONE) ||                                                                // If this is the first candidate we are evaluating...
						((name.platform_id == TT_PLATFORM_MICROSOFT) && (currentBestPlatformId != TT_PLATFORM_MICROSOFT)) ||     // If this is the first Microsoft candidate we are evalulating...
						(strLen > currentBestNameStrlen))                                                                        // If all else is equal, and this string is longer than a previous candidate's string...
					{
						currentBestPlatformId = name.platform_id;
						currentBestNameStrlen = strLen;

						EA::StdC::Memcpy(string8_16_32.mString8, name.string, name.string_len);

						if(charSize == 1)
						{
							string8_16_32.mString8[name.string_len / charSize] = 0;
							EA::StdC::Strlcpy(familyName, string8_16_32.mString8, familyNameCapacity);
						}
						else if(charSize == 2)
						{
							if(bigEndian)
							{
								for(FT_UInt c = 0; c < strLen; c++)
									string8_16_32.mString16[c] = EA::StdC::FromBigEndian((uint16_t)string8_16_32.mString16[c]);
							}

							string8_16_32.mString16[strLen] = 0;
							EA::StdC::Strlcpy(familyName, string8_16_32.mString16, familyNameCapacity);
						}
						else // if(charSize == 4)
						{
							if(bigEndian)
							{
								for(FT_UInt c = 0; c < strLen; c++)
									string8_16_32.mString32[c] = EA::StdC::FromBigEndian((uint32_t)string8_16_32.mString32[c]);
							}

							string8_16_32.mString32[strLen] = 0;
							EA::StdC::Strlcpy(familyName, string8_16_32.mString32, familyNameCapacity);
						}

						if((name.platform_id == TT_PLATFORM_MICROSOFT) && (charSize >= 2)) // Names for the Microsoft platform id have the full traditional name that we are looking for,
							bBestMatchFound = true; // We found an ideal candidate.        // whereas names for other platforms (e.g. Mac) don't necessarily have it.
					}

					bFound = true; // We found possible candidate.
				}
			}

			// It appears that we don't need to do the following, as the Face itself does so in its destructor.
			// pFace->memory->free(pFace->memory, name.string);
		}
	}

	return bFound;
}



bool OutlineFont::OpenInternal(int nFaceIndex)
{
	bool bReturnValue = false;

	#if EATEXT_WEAK_HINTING_DETECTION_ENABLED
		const int   nameFieldsToCheck[] = { 0 /*TT_NAME_ID_COPYRIGHT*/, 8 /*TT_NAME_ID_MANUFACTURER*/ };
		const char* namesToCheck[] = { "adobe", "monotype" /* possibly add others */ }; // This is a list of any string we might find in these fields which is a giveaway that the font doesn't have useful hinting. For example, Adobe fonts seem to never have useful hinting.
	#endif

	// This is an internal function, so we assume the mutex is locked.
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData && mpFaceData->mMutex.HasLock());
	#endif

	EA_ASSERT(gFTLibrary != NULL);
	if(mpFaceData->mpFTFace && gFTLibrary) // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.
	{
		// mFontDescription.mFamily
		if(!GetFreeTypeFontTraditionalFamilyName(mpFaceData->mpFTFace, mFontDescription.mFamily, kFamilyNameCapacity))
		{
			if(mpFaceData->mpFTFace->family_name)
				EA::StdC::Strlcpy(mFontDescription.mFamily, mpFaceData->mpFTFace->family_name, kFamilyNameCapacity);  // It's not a critical problem if there wasn't enough capacity for the full string.
			else
				EA::StdC::Strlcpy(mFontDescription.mFamily, "UnknownFamily", kFamilyNameCapacity); // Don't check the return value because this will always succeed. And even if it didn't it wouldn't be a big deal.
		}

		// mFontDescription.mfSize
		mFontDescription.mfSize = 0.f;  // Don't set this here. User sets it separately.

		// mFontDescription.mStyle
		if(mpFaceData->mpFTFace->style_flags & FT_STYLE_FLAG_ITALIC)
			mFontDescription.mStyle = kStyleItalic;
		else if(mpFaceData->mpFTFace->family_name && EA::StdC::Stristr(mpFaceData->mpFTFace->family_name, " italic"))
			mFontDescription.mStyle = kStyleItalic;
		else if(mpFaceData->mpFTFace->style_name && EA::StdC::Stristr(mpFaceData->mpFTFace->style_name, "italic"))
			mFontDescription.mStyle = kStyleItalic;
		else if(mpFaceData->mpFTFace->style_name && EA::StdC::Stristr(mpFaceData->mpFTFace->style_name, "oblique"))
			mFontDescription.mStyle = kStyleOblique;
		else
			mFontDescription.mStyle = kStyleNormal;

		// mFontDescription.mWeight
		if(mpFaceData->mpFTFace->style_flags & FT_STYLE_FLAG_BOLD)
			mFontDescription.mfWeight = kWeightBold;
		else if(mpFaceData->mpFTFace->family_name && EA::StdC::Stristr(mpFaceData->mpFTFace->family_name, " bold"))
			mFontDescription.mfWeight = kWeightBold;
		else if(mpFaceData->mpFTFace->style_name && EA::StdC::Stristr(mpFaceData->mpFTFace->style_name, "bold"))
			mFontDescription.mfWeight = kWeightBold;
		else
			mFontDescription.mfWeight = kWeightNormal;

		// mFontDescription.mfStretch
		mFontDescription.mfStretch = 1.f;

		// mFontDescription.mPitch
		if(mpFaceData->mpFTFace->face_flags & FT_FACE_FLAG_FIXED_WIDTH)
			mFontDescription.mPitch = kPitchFixed;
		else
			mFontDescription.mPitch = kPitchVariable;

		// mFontDescription.mVariant
		// It appears that we need to read lower level font information to tell if the face is kVariantSmallCaps.
		mFontDescription.mVariant = kVariantNormal;

		// mFontDescription.mSmooth
		// This is disabled, as it is already the default value for mFontDescription and because the user 
		// may have already changed this to be a different value before calling this Open function.
		// mFontDescription.mSmooth = kSmoothNone;  // Don't need to set this here. User sets normally it separately.


		// Misc Setup
		//////////////////////////////////////////////////

		// Set up mbFontHasUsefulHinting
		mbFontHasUsefulHinting = true; // True until possibly proven false below.

		#if EATEXT_WEAK_HINTING_DETECTION_ENABLED
			for(size_t i = 0; i < EAArrayCount(nameFieldsToCheck) && mbFontHasUsefulHinting; i++)
			{
				FT_SfntName name;

				if(FT_Get_Sfnt_Name(mpFaceData->mpFTFace, nameFieldsToCheck[i], &name) == FT_Err_Ok)
				{
					// We copy a string that is non zero-terminated into buffer so we can do a EA::StdC::Stristr call on it.
					const unsigned kBufferSize = 256;
					char buffer[kBufferSize] = {};
                    const unsigned capacity = (name.string_len < kBufferSize) ? name.string_len : kBufferSize;
                    EA::StdC::Strncat(buffer, (char*)name.string, capacity);

					for(size_t n = 0; (n < EAArrayCount(namesToCheck)) && mbFontHasUsefulHinting; n++)
					{
						if(EA::StdC::Stristr(buffer, namesToCheck[n]))
							mbFontHasUsefulHinting = false;
					}
				}
			}
		#endif

		// Set up mSupportedScriptSet.
		SetupSupportedScriptSet();

		// Set the replacement glyph for the font.
		Char c = (Char)EA_TEXT_REPLACEMENT_CHAR;
		mReplacementGlyphId = (GlyphId)FT_Get_Char_Index(mpFaceData->mpFTFace, c); // FreeType returns 0 for missing glyph. Which is a problem since it doesn't tell you if there was an error.

		// Add a kGlyphIdZeroWidth glyph.
		GlyphMetrics gmZeroWidth;
		memset(&gmZeroWidth, 0, sizeof(gmZeroWidth));
		mGlyphMetricsMap.insert(GlyphMetricsMap::value_type(kGlyphIdZeroWidth, gmZeroWidth));

		// We don't currently use the face index. But somewhere in this class we need to be able to load it.
		EA_UNUSED(nFaceIndex);

		bReturnValue = true;
	}

	return bReturnValue;
}


bool OutlineFont::Close()
{
	// Should we attempt to lock the mutex here? Normally we shouldn't be here
	// unless the font is no longer being used. We probably don't want to support
	// fonts that can be arbitrarily closed by one thread while possibly used 
	// by another thread.

	#if EATEXT_EFFECTS_ENABLED
		if(mpEffectsProcessor)
		{
			CORE_DELETE(mpCoreAllocator, mpEffectsProcessor);
			mpEffectsProcessor = NULL;
		}
	#endif

	if(mpFTSize)
	{
		// EA_ASSERT(gFTLibrary != NULL); // To consider: Enable this. It might be impractical to enable it due to application requirements that some fonts be freed after FreeType is shut down.
		if(gFTLibrary)
			FT_Done_Size(mpFTSize); // This will remove it from the FTFace if it is active.
		mpFTSize = NULL;
	}


	if(mpFaceData)
	{
		mpFaceData->Release();
		mpFaceData = NULL;
	}

	if(mpStream)
	{
		mpStream->Release();
		mpStream = NULL;
	}

	mnRenderCount = 0;

	return true;
}


FontType OutlineFont::GetFontType()
{
	return (mbOpened ? kFontTypeOutline : kFontTypeUnknown);
}


OutlineFont::BitmapFormat OutlineFont::GetBitmapFormat() const
{
	if(mFontDescription.mEffect == kEffectNone)
		return (mFontDescription.mSmooth == kSmoothEnabled) ? kBFGrayscale : kBFMonochrome;

	// As of this writing, effects are always done as ARGB and not RGBA or something else.
	return kBFARGB;
}


bool OutlineFont::GetFontDescription(FontDescription& fontDescription)
{
	// Disabled because this class' data is considered const at runtime:
	//#if EATEXT_THREAD_SAFETY_ENABLED
	//    EA_ASSERT(mpFaceData);
	//    EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	//#endif

	if(GetFontType() != kFontTypeUnknown)
	{
		fontDescription = mFontDescription;
		return true;
	}

	return false;
}


bool OutlineFont::GetFontMetrics(FontMetrics& fontMetrics)
{
	if(GetFontType() != kFontTypeUnknown)
	{
		#if EATEXT_EFFECTS_ENABLED
			if(mFontDescription.mEffect && !mbFontMetricsCalculated)
			{
				#if EATEXT_THREAD_SAFETY_ENABLED
					EA_ASSERT(mpFaceData);
					EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
				#endif

				if(!mbEffectsInitialized)
					InitEffectsProcessor();

				mpEffectsProcessor->AdjustFontMetrics(mFontMetrics);
				mbFontMetricsCalculated = true;
			}
		#endif

		fontMetrics = mFontMetrics;
		return true;
	}
	return false;
}


bool OutlineFont::GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics)
{
	bool bReturnValue = true;

	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData);
		EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	#endif

	const GlyphMetricsMap::const_iterator it = mGlyphMetricsMap.find(glyphId);

	if(it != mGlyphMetricsMap.end()) // If found...
	{
		const GlyphMetrics& glyphMetricsTemp = (*it).second;
		glyphMetrics = glyphMetricsTemp;
	}
	else
	{
		// If the following assert fails then Open wasn't successfully executed or SetTransform wasn't successfully executed after Open.
		EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace && (mFontMetrics.mfSize > 0));

		mnRenderCount++;

		if(mFontDescription.mEffect)
		{
			#if EATEXT_EFFECTS_ENABLED
				// The first time we try to render an effect, we initialize the EffectsProcessor.
				if(!mbEffectsInitialized)
					InitEffectsProcessor();

				EffectsProcessor::BeginEffectBitmapsUse(this, mpCoreAllocator);
				mpEffectsProcessor->Execute(this, glyphId);
				EffectsProcessor::EndEffectBitmapsUse(this);             // We don't need the bitmap data at all, so we can immediately tell the effects processor we are done with it.

				const GlyphMetrics& glyphMetricsTemp = mpEffectsProcessor->GetEffectsState()->mGlyphMetrics;
				const GlyphMetricsMap::value_type mapEntry(glyphId, glyphMetricsTemp);
				mGlyphMetricsMap.insert(mapEntry);
				glyphMetrics = glyphMetricsTemp;
			#else
				EA_FAIL_MESSAGE("OutlineFont::GetGlyphMetrics: EAText Effects are requested but disabled.");
				bReturnValue = false;
			#endif
		}
		else
		{
			EA_ASSERT(gFTLibrary != NULL);
			if(gFTLibrary) // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.
			{
				FT_Error nFTError = FT_Activate_Size(mpFTSize);
				EA_ASSERT(nFTError == 0);
				EA_UNUSED(nFTError);

				// FreeType is a little confusing about load flags, load target flags and render flags. 
				// You pass FT_LOAD and FT_LOAD_TARGET flags to FT_Load_Glyph, and you pass 
				// FT_RENDER_MODE flags to FT_Render_Glyph. Its a little confusing because their 
				// names overlap in ways that make it easy to get confused about the effect of each.
				//
				// For FreeType, we take 'smoothing enabled' to mean hinting disabled. 

				int32_t loadFlags       = 0;        // http://refspecs.linux-foundation.org/freetype/freetype-doc-2.1.10/docs/reference/ft2-base_interface.html#FT_LOAD_XXX
				int32_t loadTargetFlags = 0;        // One of FT_LOAD_TARGET_NORMAL, FT_LOAD_TARGET_LIGHT, FT_LOAD_TARGET_MONO, FT_LOAD_TARGET_LCD, FT_LOAD_TARGET_LCD_V. Don't want FT_LOAD_TARGET_MONO, as that targets the hinting algorithm at a true monochrome video screen and is not the same as rendering in mono.

				if(mbUseAutoHinting)
					loadFlags |= FT_LOAD_FORCE_AUTOHINT;
				else if(!mbEnableHinting || !mbFontHasUsefulHinting)
					loadFlags |= FT_LOAD_NO_HINTING;

				if((mScreenColorType == kSCT_LCD_H_RGB) || (mScreenColorType == kSCT_LCD_H_BGR))
				{
					loadFlags       |=  FT_LOAD_NO_BITMAP;       // Disable bitmaps for LCD rendering, as it results in some font sizes/glyphs being sharp but not others. Windows ClearType also disables bitmaps.
					loadFlags       |=  FT_LOAD_FORCE_AUTOHINT;  // Glyphs look better when using auto-hinting instead of built-in hinting for the case of LCD sub-pixel antialiasing.
					loadFlags       &= ~FT_LOAD_NO_HINTING;
					loadTargetFlags |=  FT_LOAD_TARGET_LCD;
				}
				else if((mScreenColorType == kSCT_LCD_V_RGB) || (mScreenColorType == kSCT_LCD_V_BGR))
				{
					loadFlags       |=  FT_LOAD_NO_BITMAP;   // Disable bitmaps for LCD rendering, as it results in some font sizes/glyphs being sharp but not others. Windows ClearType also disables bitmaps.
					loadFlags       |=  FT_LOAD_FORCE_AUTOHINT;  // Glyphs look better when using auto-hinting instead of built-in hinting for the case of LCD sub-pixel antialiasing.
					loadFlags       &= ~FT_LOAD_NO_HINTING;
					loadTargetFlags |=  FT_LOAD_TARGET_LCD_V;
				}
				else
					loadTargetFlags = FT_LOAD_TARGET_NORMAL;

				nFTError = FT_Load_Glyph(mpFaceData->mpFTFace, glyphId, loadFlags | loadTargetFlags);
				EA_ASSERT(nFTError == 0);

				if(nFTError == 0)
				{
					FT_Render_Mode renderMode; // FT_RENDER_MODE_NORMAL, FT_RENDER_MODE_LIGHT, FT_RENDER_MODE_MONO, FT_RENDER_MODE_LCD, FT_RENDER_MODE_LCD_V. http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Render_Mode

					if((mScreenColorType == kSCT_LCD_H_RGB) || (mScreenColorType == kSCT_LCD_H_BGR))
						renderMode = FT_RENDER_MODE_LCD;    // Results in a bitmap of bytes that is three times wider than normal, as there is one byte each for R, G, B. Each byte is the opacity of that color.
					else if((mScreenColorType == kSCT_LCD_V_RGB) || (mScreenColorType == kSCT_LCD_V_BGR))
						renderMode = FT_RENDER_MODE_LCD_V;  // Results in a bitmap of bytes that is three times taller than normal, as there is one byte each for R, G, B. Each byte is the opacity of that color.
					else if((mFontDescription.mSmooth == kSmoothEnabled) || (mFontDescription.mEffect != kEffectNone))
						renderMode = FT_RENDER_MODE_NORMAL; // Results in a bitmap of bytes where each byte is the opacity (and thus gray level) of the pixel.
					else
						renderMode = FT_RENDER_MODE_MONO;   // Results in a bitmap of bits where each bit represents the opacity of the pixel.

					nFTError = FT_Render_Glyph(mpFaceData->mpFTFace->glyph, renderMode);
					EA_ASSERT(nFTError == 0);

					if(nFTError == 0)
					{
						if(renderMode == FT_RENDER_MODE_LCD) // In this case FreeType renders a glyph that is 3x wide in its metrics.
						{
							EA_ASSERT((mpFaceData->mpFTFace->glyph->bitmap.width % 3) == 0);
							mpFaceData->mpFTFace->glyph->bitmap.width /= 3;
						}
						else if(renderMode == FT_RENDER_MODE_LCD_V) // In this case FreeType renders a glyph that is 3x tall in its metrics.
						{
							EA_ASSERT((mpFaceData->mpFTFace->glyph->bitmap.rows % 3) == 0);
							mpFaceData->mpFTFace->glyph->bitmap.rows /= 3;
						}

						GetCurrentGlyphMetrics(glyphMetrics); // Get glyph metrics which we will return to the user.

						const GlyphMetricsMap::value_type mapEntry(glyphId, glyphMetrics); // Cache the glyph metrics.
						mGlyphMetricsMap.insert(mapEntry);
					}
				}

				bReturnValue = (nFTError == 0);
			}
			else
				bReturnValue = false;
		}
	}

	return bReturnValue;
}


void OutlineFont::GetCurrentGlyphMetrics(GlyphMetrics& glyphMetrics)
{
	// This is an internal function, so we assume the mutex is locked.
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData && mpFaceData->mMutex.HasLock());
	#endif

	EA_ASSERT(gFTLibrary != NULL);
	if(gFTLibrary) // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.
	{
		EA_ASSERT(mpFaceData->mpFTFace && mpFaceData->mpFTFace->glyph);

		FT_GlyphSlot pGlyphSlot = mpFaceData->mpFTFace->glyph;

		glyphMetrics.mfSizeX     = (float)pGlyphSlot->bitmap.width;
		glyphMetrics.mfSizeY     = (float)pGlyphSlot->bitmap.rows;
		glyphMetrics.mfHBearingX = (float)pGlyphSlot->bitmap_left;
		glyphMetrics.mfHBearingY = (float)pGlyphSlot->bitmap_top;
		glyphMetrics.mfHAdvanceX = FFFixed26ToFloat(pGlyphSlot->advance.x); // + 0.5f;  // Add 0.5f in order to cause integer rounding to work right. 

		#if EATEXT_VERTICAL_ENABLED
			// To consider: Implement this. In practice nobody is needing this, as vertical text is all but dead on computers.
		#endif
	}
}


uint32_t OutlineFont::GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
									bool bUseReplacementGlyph, const uint32_t nGlyphIdStride, bool bWriteInvalidGlyphs)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData);
		EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	#endif

	EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace);  // If this fails then Open wasn't successfully executed.
	if(!gFTLibrary || !mpFaceData->mpFTFace)
		return 0;

	GlyphId     glyphId;    // Scratch memory in case pGlyphIdArray is NULL.
	const Char* pChar       = pCharArray;
	const Char* pCharEnd    = pCharArray + nCharArrayCount;
	GlyphId*    pGlyphId    = pGlyphIdArray ? pGlyphIdArray : &glyphId;
	uint32_t    glyphCount  = 0;

	while(pChar < pCharEnd)
	{
		// FreeType returns 0 for a non-existing glyph. Which is a problem since it doesn't tell you if there was an error.
		const GlyphId g = (GlyphId)FT_Get_Char_Index(mpFaceData->mpFTFace, *pChar);

		// Currently there is no known way to tell if FT_Get_Char_Index has failed and is returning the 0 glyph index due to an error or because it's the first glyph.
		bool bGlyphOK = (g != 0); // We can only hope that there isn't some real character at position zero.

		if(bGlyphOK) // The large majority of the time this will be true.
			*pGlyphId = g;
		else
		{
			bGlyphOK = true;

			if(IsCharZeroWidth(*pChar))
				*pGlyphId = kGlyphIdZeroWidth;
			else if(bUseReplacementGlyph)
				*pGlyphId = mReplacementGlyphId;
			else if(bWriteInvalidGlyphs)
				*pGlyphId = kGlyphIdInvalid;
			else
				bGlyphOK = false;
		}

		if(bGlyphOK)
		{
			glyphCount++;

			if(pGlyphIdArray)
				pGlyphId = (GlyphId*)((char*)pGlyphId + nGlyphIdStride);
		}

		pChar++;
	}

	return glyphCount;
}


// Problem: We don't have a means whereby 
uint32_t OutlineFont::GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
									bool bUseReplacementGlyph, const uint32_t nGlyphIdStride, bool bWriteInvalidGlyphs)
{
	// Disabled because this class' data is considered const at runtime:
	//#if EATEXT_THREAD_SAFETY_ENABLED
	//    EA_ASSERT(mpFaceData);
	//    EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	//#endif

	EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace);  // If this fails then Open wasn't successfully executed.
	if(!gFTLibrary || !mpFaceData->mpFTFace)
		return 0;

	GlyphId        glyphId;    // Scratch memory in case pGlyphIdArray is NULL.
	const char8_t* pChar       = pCharArray;
	const char8_t* pCharEnd    = pCharArray + nCharArrayCount;
	GlyphId*       pGlyphId    = pGlyphIdArray ? pGlyphIdArray : &glyphId;
	uint32_t       glyphCount  = 0;

	while(pChar < pCharEnd)
	{
			// FreeType returns 0 for a non-existing glyph. Which is a problem since it doesn't tell you if there was an error.
		const GlyphId g = (GlyphId)FT_Get_Char_Index(mpFaceData->mpFTFace, (uint8_t)*pChar);

		// Currently there is no known way to tell if FT_Get_Char_Index has failed and is returning the 0 glyph index due to an error or because it's the first glyph.
		bool bGlyphOK = (g != 0); // We can only hope that there isn't some real character at position zero.

		if(bGlyphOK) // The large majority of the time this will be true.
			*pGlyphId = g;
		else
		{
			bGlyphOK = true;

			if(IsCharZeroWidth((uint8_t)*pChar))
				*pGlyphId = kGlyphIdZeroWidth;
			else if(bUseReplacementGlyph)
				*pGlyphId = mReplacementGlyphId;
			else if(bWriteInvalidGlyphs)
				*pGlyphId = kGlyphIdInvalid;
			else
				bGlyphOK = false;
		}

		if(bGlyphOK)
		{
			glyphCount++;

			if(pGlyphIdArray)
				pGlyphId = (GlyphId*)((char*)pGlyphId + nGlyphIdStride);
		}

		pChar++;
	}

	return glyphCount;
}


bool OutlineFont::IsCharSupported(Char c, Script /*script*/)
{
	// Disabled because this class' data is considered const at runtime once Open has successfully executed:
	//#if EATEXT_THREAD_SAFETY_ENABLED
	//    EA_ASSERT(mpFaceData);
	//    EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	//#endif

	return mpFaceData != nullptr && mpFaceData->mSupportedChars.test((eastl_size_t)c, false);

	/* Reference implementation which is slower:
	EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace);  // If this fails then Open wasn't successfully executed.
	if(!gFTLibrary || !mpFaceData->mpFTFace)
		return false;
	// FreeType returns 0 for a non-existing glyph. Which is a problem since it doesn't tell you if there was an error.
	return FT_Get_Char_Index(mpFaceData->mpFTFace, c) != 0;
	*/
}


bool OutlineFont::GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool /*bHorizontalLayout*/)
{
	bool bReturnValue = false;

	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData);
		EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	#endif

	EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace);  // If this fails then Open wasn't successfully executed.
	if(!gFTLibrary || !mpFaceData->mpFTFace)
		return false;

	#if (EATEXT_KERNING_SUPPORT > 0)
		if((mpFaceData->mpFTFace->face_flags & FT_FACE_FLAG_KERNING) && (mFontMetrics.mPitch != kPitchFixed))
		{
			FT_Error nFTError = FT_Activate_Size(mpFTSize);
			EA_ASSERT(nFTError == 0);
			EA_UNUSED(nFTError);

			if(EA_UNLIKELY(direction % 2)) // If RTL...
				eastl::swap(g1, g2);

			FT_Vector ftVector;
			nFTError = FT_Get_Kerning(mpFaceData->mpFTFace, g1, g2, FT_KERNING_DEFAULT, &ftVector);

			if(nFTError == 0) // If the kerning exists...
			{
				// http://freetype.sourceforge.net/freetype2/docs/tutorial/step2.html
				// The default value is FT_KERNING_DEFAULT which has value 0. It corresponds to kerning distances expressed in 26.6 grid-fitted pixels 
				// (which means that the values are multiples of 64). For scalable formats, this means that the design kerning distance is scaled, then rounded.
 
				kerning.mfKernX = FFFixed26ToFloat(ftVector.x);
				kerning.mfKernY = FFFixed26ToFloat(ftVector.y);
				bReturnValue = true;
			}
		}
	#endif

	if(!bReturnValue)
	{
		kerning.mfKernX = 0.f;
		kerning.mfKernY = 0.f;
	}

	return bReturnValue;
}


float OutlineFont::GetSize() const
{
	return mFontDescription.mfSize;
}


bool OutlineFont::SetTransform(float f00, float f01, float f10, float f11)
{
	// It's hard to say if this function should be thread-safe. If two threads call this function
	// at the same time, putting a mutex here doesn't really help the problem of differing transforms.
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData);
		EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	#endif

	EA_ASSERT(gFTLibrary && mpFaceData->mpFTFace);

	if(gFTLibrary && mpFaceData->mpFTFace)
	{
		FT_Error nFTError = FT_Activate_Size(mpFTSize);
		EA_ASSERT(nFTError == 0);
		EA_UNUSED(nFTError);

		FT_Matrix ftMatrix;

		ftMatrix.xx = FFFloatToFixed16(f00);
		ftMatrix.xy = FFFloatToFixed16(f01);
		ftMatrix.yx = FFFloatToFixed16(f10);
		ftMatrix.yy = FFFloatToFixed16(f11);

		FT_Set_Transform(mpFaceData->mpFTFace, &ftMatrix, NULL);

		return true;
	}

	return false;
}


bool OutlineFont::SetTransform(float fSize, float /*fAngle*/, float /*fSkew*/)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA_ASSERT(mpFaceData);
		EA::Thread::AutoFutex autoMutex(mpFaceData->mMutex);
	#endif

	bool bResult = false;

	EA_ASSERT(gFTLibrary); // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.

	if(gFTLibrary)
	{
		FT_Error nFTError;

		if(!mpFTSize)
		{
			nFTError = FT_New_Size(mpFaceData->mpFTFace, &mpFTSize);
			EA_ASSERT(nFTError == 0);
			EA_UNUSED(nFTError);
		}

		nFTError = FT_Activate_Size(mpFTSize);
		EA_ASSERT(nFTError == 0);

		nFTError = FT_Set_Char_Size(mpFaceData->mpFTFace, 0, FFFloatToFixed26(fSize), mDPI, mDPI);
		EA_ASSERT(nFTError == 0);

		TT_OS2* os2Table = (TT_OS2*)FT_Get_Sfnt_Table(mpFaceData->mpFTFace, ft_sfnt_os2);

		// With FreeType, the concept size and transform are different things. 
		// The font transform doesn't specify the font size, it modifies it. 

		// Read FontMetrics
		mFontMetrics.mfSize                   = 0.f; // This is to be set below.
		mFontMetrics.mPitch                   = mFontDescription.mPitch;
		mFontMetrics.mfHAdvanceXMax           = FTFontUnitsToFloat(mpFaceData->mpFTFace->max_advance_width);
		mFontMetrics.mfAscent                 = FTFontUnitsToFloat(mpFaceData->mpFTFace->ascender);
		mFontMetrics.mfDescent                = FTFontUnitsToFloat(mpFaceData->mpFTFace->descender);
		mFontMetrics.mfLeading                = FTFontUnitsToFloat(mpFaceData->mpFTFace->height - mpFaceData->mpFTFace->ascender + mpFaceData->mpFTFace->descender);
		mFontMetrics.mfLineHeight             = FTFontUnitsToFloat(mpFaceData->mpFTFace->height);

		// Here we perform a simple heuristic to try and account for fonts which have been incorrectly created.
		// Specifically in practice most fonts we find have a leading value which makes up < 5% of the line height.
		// As such, we take the, perhaps too generous, approach that if a font's leading is > 10% of the line height,
		// we try to look at the font's OS/2 table for Windows clipping bounds for the font. These bounds will guarantee a 
		// font will fit (it is calculated as the max height of all ANSI characters in the font) but should only be used in
		// general for bounds checking or default line spacing according to the open type specification (even though this field
		// has actually been abused by font creators and application writers historically). If the OS/2 does not exist or is invalid
		// we simply assume the font creator mistakenly calculated the font's ascent by including the in the font's leading
		// instead. Here we correct that assumed error by adding the leading to the ascent and assuming a leading of 0 (which 
		// is in practice a very common leading value)
		if((mFontMetrics.mfLeading / mFontMetrics.mfLineHeight) > 0.1f)
		{
			if (os2Table->version != 0xFFFF) // Freetype marks invalid/missing tables with this version number
			{
				mFontMetrics.mfAscent = FTFontUnitsToFloat(os2Table->usWinAscent);
				mFontMetrics.mfDescent = -FTFontUnitsToFloat(os2Table->usWinDescent);
				mFontMetrics.mfLeading = 0;
			}
			else
			{
				mFontMetrics.mfAscent += mFontMetrics.mfLeading;
				mFontMetrics.mfLeading = 0;
			}
		}
			
		mFontMetrics.mfBaseline               = mFontMetrics.mfLeading + mFontMetrics.mfAscent;
		if(((int)mFontMetrics.mfBaseline != mFontMetrics.mfBaseline) || ((int)mFontMetrics.mfDescent != mFontMetrics.mfDescent)) // If either the baseline or descent has a fractional value, we bump up the line height by one. This is because in practice the rendered line heights will in fact be of this larger line height size.
			mFontMetrics.mfLineHeight += 1.f;
		mFontMetrics.mfXHeight                = mFontMetrics.mfAscent / 2;  // This is incorrect. To do: Find a better way to get the x-height. This information should be stored within the TrueType font.
		mFontMetrics.mfCapsHeight             = mFontMetrics.mfAscent;      // This is not strictly correct. 
		mFontMetrics.mfUnderlinePosition      = FTFontUnitsToFloat(mpFaceData->mpFTFace->underline_position);
		mFontMetrics.mfUnderlineThickness     = FTFontUnitsToFloat(mpFaceData->mpFTFace->underline_thickness);
		mFontMetrics.mfLinethroughPosition    = mFontMetrics.mfXHeight;
		mFontMetrics.mfLinethroughThickness   = mFontMetrics.mfUnderlineThickness;
		mFontMetrics.mfOverlinePosition       = mFontMetrics.mfCapsHeight + 1 + (mFontMetrics.mfUnderlineThickness / 2);
		mFontMetrics.mfOverlineThickness      = mFontMetrics.mfUnderlineThickness;

		// Assert that the underline position is a somewhat sane value.
		EA_ASSERT((mFontMetrics.mfUnderlinePosition > -mFontMetrics.mfLineHeight) && (mFontMetrics.mfUnderlinePosition <= mFontMetrics.mfLineHeight));

		if(mpFaceData->mpFTFace->max_advance_height > 0)
			mFontMetrics.mfVAdvanceYMax = FTFontUnitsToFloat(mpFaceData->mpFTFace->max_advance_height);
		else
			mFontMetrics.mfVAdvanceYMax = mFontMetrics.mfLineHeight;

		// If using custom effects, we delay the metrics calculation until needed,
		// as we need to run the custom effect program in order to know the metrics.
		mbFontMetricsCalculated = (mFontDescription.mEffect == 0);
	 
		bResult = true;
	}

	if(bResult)
	{
		mFontDescription.mfSize = fSize;
		mFontMetrics.mfSize     = fSize;

		if(mFontMetrics.mfUnderlineThickness == 0.f)
			mFontMetrics.mfUnderlineThickness = ceilf(fSize / 16);

		if(mFontMetrics.mfLinethroughThickness == 0.f)
			mFontMetrics.mfLinethroughThickness = ceilf(fSize / 16);

		if(mFontMetrics.mfOverlineThickness == 0.f)
			mFontMetrics.mfOverlineThickness = ceilf(fSize / 16);
	}

	return bResult;
}


void OutlineFont::SetSmoothing(Smooth smooth)
{
	EA_ASSERT_MESSAGE(mnRenderCount == 0, "OutlineFont::SetSmoothing: Should not set smoothing after glyphs have been rendered. Use a different font.");

	// Find a way to assert that we haven't started rendering characters yet.
	// If we have started rendering characters, the result is that glyph metrics might become inaccurate.
	mFontDescription.mSmooth = smooth;
}


void OutlineFont::SetEffect(uint32_t effect, float fParamX, float fParamY, Color BaseColor, Color EffectColor, Color HighlightColor)
{
	mFontDescription.mEffect          = effect;
	mFontDescription.mfEffectX        = fParamX;
	mFontDescription.mfEffectY        = fParamY;
	mFontDescription.mEffectBaseColor = BaseColor;
	mFontDescription.mEffectColor     = EffectColor;
	mFontDescription.mHighLightColor  = HighlightColor;

	#if EATEXT_EFFECTS_ENABLED
		if(effect)
		{
			if(!mpEffectsProcessor)
			{
				mpEffectsProcessor = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "EffectsProcessor", 0) EffectsProcessor(mpCoreAllocator);
				EA_ASSERT(mpEffectsProcessor);
				mbFontMetricsCalculated = false;
			}
		}
	#endif
}


void OutlineFont::ResetEffectBitmaps()
{
	EffectsProcessor::ResetEffectBitmaps();
}


void OutlineFont::InitEffectsProcessor()
{
	#if EATEXT_EFFECTS_ENABLED
		EA_ASSERT(mpEffectsProcessor);

		mbEffectsInitialized = true;

		// Possibly create an effects processor.
		if(!mpEffectsProcessor)
		{
			mpEffectsProcessor = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "EffectsProcessor", 0) EffectsProcessor(mpCoreAllocator);
			EA_ASSERT(mpEffectsProcessor);
			mbFontMetricsCalculated = false;
		}

		const EIWord* pInstructionList = NULL;
		uint32_t      instructionCount = 0;
		bool          bResult = false;

		// Provide emulation support for the deprecated enumerated effects types. 
		// It is expected that the new EATextEffects be used instead, as it is more flexible and powerful.
		switch(mFontDescription.mEffect)
		{
			case kEffectOutline:
			{
				EffectsState es;
				es.SetGlyphColor(mFontDescription.mEffectColor);
				es.SetGlyphBrush(kBSRound, 1 + ((int32_t)mFontDescription.mfEffectX * 2), .5, 1.f);
				es.DrawGlyphSmearOutline();
				es.Merge();
				es.Clear();
				es.SetGlyphColor(mFontDescription.mEffectBaseColor);
				es.DrawGlyph();
				es.Merge();
				es.SetGlyphHSpace(1, 0);
				pInstructionList = es.mInstructionList;
				instructionCount = es.mInstructionListSize;
				bResult = true;
				break;
			}

			case kEffectShadow:
			{ 
				EffectsState es;
				es.SetGlyphColor(mFontDescription.mEffectColor);
				es.SetGlyphOffset((int32_t)mFontDescription.mfEffectX, (int32_t)mFontDescription.mfEffectY);
				es.DrawGlyph();
				es.ApplyBoxBlur(2, 1.1f, 2);
				es.Merge();
				es.Clear();
				es.SetGlyphColor(mFontDescription.mEffectBaseColor);
				es.SetGlyphOffset(0, 0);
				es.DrawGlyph();
				es.Merge();
				es.SetGlyphHSpace(1, 0);
				pInstructionList = es.mInstructionList;
				instructionCount = es.mInstructionListSize;
				bResult = true;
				break;
			}

			case kEffectRaised:
			case kEffectDepressed:
				// The previous versiosn of these were so lame that nobody used them.
				// So we just ignore them here.
				break;

			default:
				// Effects are identified by id and are registered with the FontServer.
				// Here we retrieve the effect definition from the FontServer and give 
				// it to the EffectProcessor.
				FontServer* const pFontServer = EA::Text::GetFontServer(false);
				EA_ASSERT(pFontServer); // If this fails then most likely you forgot to call SetFontServer with your FontServer.

				bResult = pFontServer->GetEffect(mFontDescription.mEffect, pInstructionList, instructionCount);
				EA_ASSERT(bResult); // If this fails then most likely you forgot to add the effect to the FontServer or you forgot to call SetFontServer with your FontServer.
				break;
		}


		if(bResult)
		{
			EffectsState* const pEffectsState = mpEffectsProcessor->GetEffectsState();
			pEffectsState->AppendInstructionList(pInstructionList, instructionCount);
		}
	#endif
}


// Enabling this define draws a rectangular outline around all glyphs rendered by FreeType.
// #define EATEXT_DRAW_DIAGNOSTIC_GLYPH_RECTANGLES

#ifdef EATEXT_DRAW_DIAGNOSTIC_GLYPH_RECTANGLES
static inline void PutPixel(FT_Bitmap* bitmap, int x, int y)
{
	int pitch = bitmap->pitch;
	unsigned char* buffer = bitmap->buffer;
	char pixel_mode = bitmap->pixel_mode;
	if (pixel_mode == FT_PIXEL_MODE_MONO)
	{
		buffer[y*pitch + x / 8] |= 0x80 >> (x&7);
	}
	else if (pixel_mode == FT_PIXEL_MODE_GRAY)
	{
		buffer[y*pitch + x] = 0x80; // Draw a half intensity pixel to make it easier to distinguish from text (normally 100% black)
	}
	else
	{
		EA_FAIL_MESSAGE("Unsupported pixel format encountered.");
	}
}
#endif

bool OutlineFont::RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId glyphId, uint32_t /*renderFlags*/, float fXFraction, float fYFraction)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		// We have to hold this lock while the bitmap is outstanding to prevent
		// conflicts on the shared glyph buffer.
		EA_ASSERT(mpFaceData);
		mpFaceData->mMutex.Lock();
	#endif

	EA_ASSERT(gFTLibrary); // gFTLibrary is initialized in the EA::Text::Init function, which the app must call on startup.
	EA_ASSERT(mpFaceData->mpFTFace && pGlyphBitmap && (mFontMetrics.mfSize > 0));  // If this fails then Open wasn't successfully executed or SetTransform wasn't successfully executed after Open.

	if(mpFaceData->mpFTFace)
	{
		if(EA_UNLIKELY(glyphId == kGlyphIdZeroWidth))
		{
			mGlyphBitmap.mnWidth       = 0;
			mGlyphBitmap.mnHeight      = 0;
			mGlyphBitmap.mnStride      = 0;
			mGlyphBitmap.mBitmapFormat = (mFontDescription.mSmooth == kSmoothEnabled) ? kBFGrayscale : kBFMonochrome; // Actually the format here is irrelevant.
			mGlyphBitmap.mpData        = 0;
			mGlyphBitmap.mGlyphMetrics = mGlyphMetricsMap[kGlyphIdZeroWidth];

		   *pGlyphBitmap = &mGlyphBitmap;

			// Note that we are leaving here with the mutex still locked. 
			// It will be unlocked in DoneGlyphBitmap().
			return true;
		}
		else
		{
			if(mFontDescription.mEffect)
			{
				#if EATEXT_EFFECTS_ENABLED
					// The first time we try to render an effect, we initialize the EffectsProcessor.
					if(!mbEffectsInitialized)
						InitEffectsProcessor();

					EffectsProcessor::BeginEffectBitmapsUse(this, mpCoreAllocator);
					mpEffectsProcessor->Execute(this, glyphId);

					// We don't call mpEffectsProcessor->EndBitmapUse until the DoneGlyphBitmap function. This is because 
					// in this function we hand over the bitmap bits to the caller and the caller notifies us that he's done
					// with the data by calling DoneGlyphBitmap. We then in turn call mpEffectsProcessor->EndBitmapUse.

					EffectsState* const pEffectsState = mpEffectsProcessor->GetEffectsState();
					const GlyphMetrics& glyphMetrics = pEffectsState->mGlyphMetrics;

					mGlyphBitmap.mGlyphMetrics = glyphMetrics;
					mGlyphBitmap.mnWidth       = (uint32_t)pEffectsState->mActualGlyphRight  - pEffectsState->mActualGlyphLeft;
					mGlyphBitmap.mnHeight      = (uint32_t)pEffectsState->mActualGlyphBottom - pEffectsState->mActualGlyphTop;
					mGlyphBitmap.mnStride      = (uint32_t)pEffectsState->mEffectsBitmaps.mBaseImage.mnStride;
					mGlyphBitmap.mpData        = pEffectsState->mEffectsBitmaps.mBaseImage.GetPixelPtr(pEffectsState->mActualGlyphLeft, pEffectsState->mActualGlyphTop);
					mGlyphBitmap.mBitmapFormat = kBFARGB; // As of this writing, effects are always done as ARGB and not RGBA or something else.

					if(mGlyphMetricsMap.find(glyphId) == mGlyphMetricsMap.end()) // If the glyph metrics aren't already cached...
					{
						const GlyphMetricsMap::value_type mapEntry(glyphId, glyphMetrics);
						mGlyphMetricsMap.insert(mapEntry);
					}

					*pGlyphBitmap = &mGlyphBitmap;

					return true;
				#endif
			}
			else
			{
				int8_t xFraction, yFraction; // These are values from -64 to +63; +16 means a quarter pixel adjustment rightward or downward.

				if(fXFraction)
					xFraction = (int8_t)(fXFraction * 64.f);
				else
					xFraction = 0;

				if(fYFraction)
					yFraction = (int8_t)(fYFraction * 64.f);
				else
					yFraction = 0;

				EA_UNUSED(xFraction); // These variables are not currently used by this code, 
				EA_UNUSED(yFraction); // but we need to make 'unused variable' warnings go away.

				FT_Error nFTError = FT_Activate_Size(mpFTSize);
				EA_ASSERT(nFTError == 0);
				EA_UNUSED(nFTError);

				// FreeType is a little confusing about load flags, load target flags and render flags. 
				// You pass FT_LOAD and FT_LOAD_TARGET flags to FT_Load_Glyph, and you pass 
				// FT_RENDER_MODE flags to FT_Render_Glyph. Its a little confusing because their 
				// names overlap in ways that make it easy to get confused about the effect of each.
				//
				// For FreeType, we take 'smoothing enabled' to mean hinting disabled. 

				int32_t loadFlags       = 0;        // http://refspecs.linux-foundation.org/freetype/freetype-doc-2.1.10/docs/reference/ft2-base_interface.html#FT_LOAD_XXX
				int32_t loadTargetFlags = 0;        // One of FT_LOAD_TARGET_NORMAL, FT_LOAD_TARGET_LIGHT, FT_LOAD_TARGET_MONO, FT_LOAD_TARGET_LCD, FT_LOAD_TARGET_LCD_V. Don't want FT_LOAD_TARGET_MONO, as that targets the hinting algorithm at a true monochrome video screen and is not the same as rendering in mono.

				if(mbUseAutoHinting)
					loadFlags |= FT_LOAD_FORCE_AUTOHINT;
				else if(!mbEnableHinting || !mbFontHasUsefulHinting)
					loadFlags |= FT_LOAD_NO_HINTING;

				if((mScreenColorType == kSCT_LCD_H_RGB) || (mScreenColorType == kSCT_LCD_H_BGR))
				{
					loadFlags       |=  FT_LOAD_NO_BITMAP;       // Disable bitmaps for LCD rendering, as it results in some font sizes/glyphs being sharp but not others. Windows ClearType also disables bitmaps.
					loadFlags       |=  FT_LOAD_FORCE_AUTOHINT;  // Glyphs look better when using auto-hinting instead of built-in hinting for the case of LCD sub-pixel antialiasing.
					loadFlags       &= ~FT_LOAD_NO_HINTING;
					loadTargetFlags |=  FT_LOAD_TARGET_LCD;
				}
				else if((mScreenColorType == kSCT_LCD_V_RGB) || (mScreenColorType == kSCT_LCD_V_BGR))
				{
					loadFlags       |=  FT_LOAD_NO_BITMAP;   // Disable bitmaps for LCD rendering, as it results in some font sizes/glyphs being sharp but not others. Windows ClearType also disables bitmaps.
					loadFlags       |=  FT_LOAD_FORCE_AUTOHINT;  // Glyphs look better when using auto-hinting instead of built-in hinting for the case of LCD sub-pixel antialiasing.
					loadFlags       &= ~FT_LOAD_NO_HINTING;
					loadTargetFlags |=  FT_LOAD_TARGET_LCD_V;
				}
				else
					loadTargetFlags = FT_LOAD_TARGET_NORMAL;

				nFTError = FT_Load_Glyph(mpFaceData->mpFTFace, glyphId, loadFlags | loadTargetFlags);
				EA_ASSERT(nFTError == 0);

				if(nFTError == 0)
				{
					FT_Render_Mode renderMode; // FT_RENDER_MODE_NORMAL, FT_RENDER_MODE_LIGHT, FT_RENDER_MODE_MONO, FT_RENDER_MODE_LCD

					if((mScreenColorType == kSCT_LCD_H_RGB) || (mScreenColorType == kSCT_LCD_H_BGR))
						renderMode = FT_RENDER_MODE_LCD;    // Results in a bitmap of bytes that is three times wider than normal, as there is one byte each for R, G, B. Each byte is the opacity of that color.
					else if((mScreenColorType == kSCT_LCD_V_RGB) || (mScreenColorType == kSCT_LCD_V_BGR))
						renderMode = FT_RENDER_MODE_LCD_V;  // Results in a bitmap of bytes that is three times taller than normal, as there is one byte each for R, G, B. Each byte is the opacity of that color.
					else if((mFontDescription.mSmooth == kSmoothEnabled) || (mFontDescription.mEffect != kEffectNone))
						renderMode = FT_RENDER_MODE_NORMAL; // Results in a bitmap of bytes where each byte is the opacity (and thus gray level) of the pixel.
					else
						renderMode = FT_RENDER_MODE_MONO;   // Results in a bitmap of bits where each bit represents the opacity of the pixel.

					nFTError = FT_Render_Glyph(mpFaceData->mpFTFace->glyph, renderMode);
					EA_ASSERT(nFTError == 0);

					#ifdef EATEXT_DRAW_DIAGNOSTIC_GLYPH_RECTANGLES
						FT_Bitmap *bitmap = &mpFaceData->mpFTFace->glyph->bitmap;
						int width = bitmap->width;
						int rows = bitmap->rows;
						for (int x = 0; x < width; x++)
						{
							PutPixel(bitmap, x, 0);
							PutPixel(bitmap, x, rows - 1);
						}
						for (int y = 0; y < rows; y++)
						{
							PutPixel(bitmap, 0, y);
							PutPixel(bitmap, width-1, y);
						}
					#endif

					if(renderMode == FT_RENDER_MODE_LCD) // In this case FreeType renders a glyph that is 3x wide in its metrics.
					{
						EA_ASSERT((mpFaceData->mpFTFace->glyph->bitmap.width % 3) == 0);
						mpFaceData->mpFTFace->glyph->bitmap.width /= 3;
					}
					else if(renderMode == FT_RENDER_MODE_LCD_V) // In this case FreeType renders a glyph that is 3x tall in its metrics.
					{
						EA_ASSERT((mpFaceData->mpFTFace->glyph->bitmap.rows % 3) == 0);
						mpFaceData->mpFTFace->glyph->bitmap.rows /= 3;
					}
				}

				if(nFTError == 0) // If the render succeeded...
				{
					++mnRenderCount;

					// Since we only ever have one outstanding render at a time, we store the results
					// in a member variable. Then we simply pass the address of that member variable 
					// back to the user.
					FT_GlyphSlot pGlyphSlot = mpFaceData->mpFTFace->glyph;

					mGlyphBitmap.mnWidth        = (uint32_t)pGlyphSlot->bitmap.width;
					mGlyphBitmap.mnHeight       = (uint32_t)pGlyphSlot->bitmap.rows;
					mGlyphBitmap.mnStride       = (uint32_t)pGlyphSlot->bitmap.pitch; // According to FreeType documentation, pitch can be negative, though we've never seen it to be negative and the FreeType source code appears in at least one place to assume it's positive.
					mGlyphBitmap.mpData         = pGlyphSlot->bitmap.buffer;

					if(pGlyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
						mGlyphBitmap.mBitmapFormat = kBFMonochrome;
					else if(pGlyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_LCD)
						mGlyphBitmap.mBitmapFormat = kBFRGBHSubpixelAA;
					else if(pGlyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_LCD_V)
						mGlyphBitmap.mBitmapFormat = kBFRGBVSubpixelAA;
					else
						mGlyphBitmap.mBitmapFormat = kBFGrayscale;
				}

				GetCurrentGlyphMetrics(mGlyphBitmap.mGlyphMetrics);

				if(mGlyphMetricsMap.find(glyphId) == mGlyphMetricsMap.end()) // If the glyph metrics aren't already cached...
				{
					const GlyphMetricsMap::value_type mapEntry(glyphId, mGlyphBitmap.mGlyphMetrics);
					mGlyphMetricsMap.insert(mapEntry);
				}

				*pGlyphBitmap = &mGlyphBitmap;

				#if EATEXT_GLYPH_PIXEL_FIXING_ENABLED
					// The following letters of the following fonts have a pixel that is 1 when it should 
					// instead be 0, in kBFMonochrome mode only.
					//    Arial 8: C K 
					//    Arial 11: y & 
					//    Verdana 8: 0 2
					//    Verdana 9: X 6 8
					//    Tahoma 8: 0
					//    Tahoma 9: e 0
					//    Tahoma 10: e 0
					//    Microsoft Sans Serif 10: e q 8 p d 
					//    Trebuchet MS 9: 8 
					//    Trebuchet MS 11: 8 

					// if(mGlyphBitmap.mBitmapFormat == kBFMonochrome)
					// {
					//     // To do:
					//     // If this glyph refers to one of the font/char combimations above then 
					//     // manually fix the glyph by turning off the bad pixel. It would be fastest
					//     // if this was done via a small pre-calculated data stucture that was a 
					//     // hash_map of font name + style + char to the pixel index which is broken.
					// }
				#endif

				// Note that we are leaving here with the mutex still locked. 
				// It will be unlocked in DoneGlyphBitmap().
				return true;
			}
		}
	}

	#if EATEXT_THREAD_SAFETY_ENABLED
		// We unlock the mutex only if the return value is false. Otherwise it will be unlocked in the DoneGlyphBitmap function.
		mpFaceData->mMutex.Unlock();
	#endif

	return false;
}


void OutlineFont::DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		// Note that the mutex is locked on entry to this function from a previous
		// call to RenderGlyphBitmap().
		EA_ASSERT(mpFaceData && mpFaceData->mMutex.HasLock());
	#endif

	#if EATEXT_EFFECTS_ENABLED
		if(EffectsProcessor::IsEffectBitmapsInUse(this)) // We need to check this because DoneGlyphBitmap might be called in cases where BeginGlyphBitmap 
			EffectsProcessor::EndEffectBitmapsUse(this); // was called but mpEffectsProcessor->BeginBitmapUse() wasn't called.
	#endif


	// FreeType has an FT_Done_Glyph() function, but we don't need to use it unless
	// we are working with a standalone glyph that isn't attached to the face in
	// what FreeType calls the face's "glyph slot." The face automatically manages
	// the memory for this glyph. It might be useful to have a way to force the face
	// to free that memory anyway, such as with the rendering of huge-sized glyphs.

	// Note: if we provide the option to force-free the glyph, we need to make sure 
	// we have thread-safety code here for this function.
	EA_UNUSED(pGlyphBitmap);


	#if EATEXT_THREAD_SAFETY_ENABLED
		// Matches the Lock from RenderGlyphBitmap.
		mpFaceData->mMutex.Unlock();
	#endif
}


OTF* OutlineFont::GetOTF()
{
	#if EATEXT_OPENTYPE_ENABLED
		return &mOTF;
	#else
		return NULL;
	#endif
}




// Undefine values defined earlier.
#undef FFFloatToFixed16
#undef FFFloatToFixed26
#undef FFFixed16ToFloat
#undef FFFixed26ToFloat


} // namespace Text

} // namespace EA











