/*
Copyright (C) 2006, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextOpenType.h
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements reading of OpenType tables from OpenType/TrueType fonts.
//
// The OpenType standard is reasonably well documented at:
//     The OpenType Font File
//     http://www.microsoft.com/typography/otspec/otff.htm
// 
// OpenType processing is heavily structure-based and array-based. There are 
// many structures defined here, and these come verbatim from the structs 
// defined in the OpenType standard. How OpenType works isn't very complicated
// but the tables of tables of tables of tables can be confusing.
//
// The code used to read OpenType records here may seem complicated at first,
// but I can assure you that it is complicated more because of the nature of
// the OpenType file format than due to the code here being bad. In fact the 
// code here is at least as clean and readable as any equivalent code you 
// will find from the Internet or commercial sources. And also, due to the
// nature of the OpenType format, libraries that read OpenType data tend to
// be constructed somewhat similarly. 
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTOPENTYPE_H
#define EATEXT_INTERNAL_EATEXTOPENTYPE_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAIO/EAStream.h>
#include <PPMalloc/EAStackAllocator.h>
#include <EASTL/fixed_vector.h>


namespace EA
{

namespace Allocator
{
    class ICoreAllocator;
}

namespace Text
{


// OpenType standard data type definitions
typedef  uint8_t OTFByte;          /// 8-bit unsigned integer.
typedef   int8_t OTFChar;          /// 8-bit signed integer.
typedef uint16_t OTFUShort;        /// 16-bit unsigned integer.
typedef  int16_t OTFShort;         /// 16-bit signed integer.
typedef uint32_t OTFULong;         /// 32-bit unsigned integer.
typedef  int32_t OTFLong;          /// 32-bit signed integer.
typedef  int32_t OTFFixed;         /// 32-bit signed fixed-point number (16.16).
typedef  int32_t OTFFunit;         /// Smallest measurable distance in the em space.
typedef  int16_t OTFFWord;         /// 16-bit signed integer (SHORT) that describes a quantity in FUnits.
typedef uint16_t OTFUFWord;        /// 16-bit unsigned integer (USHORT) that describes a quantity in FUnits.
typedef  int16_t OTFF2Dot14;       /// 16-bit signed fixed number with the low 14 bits of fraction (2.14).
typedef  int64_t OTFLongDateTime;  /// Date represented in number of seconds since 12:00 midnight, January 1, 1904. The value is represented as a signed 64-bit integer.
typedef uint32_t OTFTag;           /// Array of four uint8s (length = 32 bits) used to identify a script, language system, feature, or baseline
typedef uint16_t OTFGlyphId;       /// Glyph index number, same as uint16(length = 16 bits)
typedef uint16_t OTFOffset;        /// Offset to a table, same as uint16 (length = 16 bits), NULL offset = 0x0000
#define OTFNull  0                 /// The term Null is used in the OpenType standard to refer to zero values.


/// OTFFixedPoint
///
/// Data structure corresponding to the OTF Fixed type (our OTFFixed typedef).
///
struct EATEXT_API OTFFixedPoint
{
    union
    {
        int32_t mValue32;

        union
        {
            int16_t mHigh16;
            int16_t mLow16;
        };
    };
};


// Common OTFTag types
//
#if 1 // defined(EA_SYSTEM_BIG_ENDIAN)
    // Scripts 
    // See http://www.microsoft.com/typography/otspec/scripttags.htm
    const OTFTag kOTFTag_0000 = 0x00000000;     // Empty, unspecified script.
    const OTFTag kOTFTag_arab = 0x61726162;     // Arabic script
    const OTFTag kOTFTag_deva = 0x64657661;     // Devanagari (Hindi) script
    const OTFTag kOTFTag_thai = 0x74686169;     // Thai script
    const OTFTag kOTFTag_DFLT = 0x44464C54;     // Default script

    // GSub types
    // See http://www.microsoft.com/typography/otfntdev/arabicot/features.htm
    const OTFTag kOTFTag_ccmp = 0x63636d70;     // Character composition/decomposition substitution
    const OTFTag kOTFTag_isol = 0x69736f6c;     // Isolated character form substitution
    const OTFTag kOTFTag_fina = 0x66696e61;     // Final character form substitution
    const OTFTag kOTFTag_medi = 0x6d656469;     // Medial character form substitution
    const OTFTag kOTFTag_init = 0x696e6974;     // Initial character form substitution
    const OTFTag kOTFTag_rlig = 0x726c6967;     // Required ligature substitution
    const OTFTag kOTFTag_calt = 0x63616c74;     // Connection form substitution
    const OTFTag kOTFTag_liga = 0x6c696761;     // Standard ligature substitution
    const OTFTag kOTFTag_dlig = 0x646c6967;     // Discretionary ligature substitution
    const OTFTag kOTFTag_cswh = 0x63737768;     // Contextual swashes
    const OTFTag kOTFTag_mset = 0x6d736574;     // Mark positioning via substitution

    // GPos types
    // See http://www.microsoft.com/typography/otfntdev/arabicot/features.htm
    const OTFTag kOTFTag_curs = 0x63757273;     // Cursive positioning
    const OTFTag kOTFTag_kern = 0x6b65726e;     // Pair kerning
    const OTFTag kOTFTag_mark = 0x6d61726b;     // Mark to base positioning
    const OTFTag kOTFTag_mkmk = 0x6d6b6d6b;     // Mark to mark positioning
#else
    const OTFTag kOTFTag_0000 = 0x00000000;     // Empty, unspecified script.
    const OTFTag kOTFTag_arab = 0x61726162;     // Arabic script
    const OTFTag kOTFTag_deva = 0x64657661;     // Devanagari (Hindi) script
    const OTFTag kOTFTag_thai = 0x74686169;     // Thai script
    const OTFTag kOTFTag_DFLT = 0x44464C54;     // Default script

    const OTFTag kOTFTag_ccmp = 0x706d6363;     // Character composition/decomposition substitution
    const OTFTag kOTFTag_isol = 0x6c6f7369;     // Isolated character form substitution
    const OTFTag kOTFTag_fina = 0x616e6966;     // Final character form substitution
    const OTFTag kOTFTag_medi = 0x6964656d;     // Medial character form substitution
    const OTFTag kOTFTag_init = 0x74696e69;     // Initial character form substitution
    const OTFTag kOTFTag_rlig = 0x67696c72;     // Required ligature substitution
    const OTFTag kOTFTag_calt = 0x746c6163;     // Connection form substitution
    const OTFTag kOTFTag_liga = 0x6167696c;     // Standard ligature substitution
    const OTFTag kOTFTag_dlig = 0x67696c64;     // Discretionary ligature substitution
    const OTFTag kOTFTag_cswh = 0x68777363;     // Contextual swashes
    const OTFTag kOTFTag_mset = 0x7465736d;     // Mark positioning via substitution

    const OTFTag kOTFTag_curs = 0x73727563;     // Cursive positioning
    const OTFTag kOTFTag_kern = 0x6e72656b;     // Pair kerning
    const OTFTag kOTFTag_mark = 0x6b72616d;     // Mark to base positioning
    const OTFTag kOTFTag_mkmk = 0x6b6d6b6d;     // Mark to mark positioning
#endif


/// OTFTagStruct
///
struct EATEXT_API OTFTagStruct
{
    uint32_t mOTFTag;

    OTFTagStruct(uint32_t otfTag = 0)
        { mOTFTag = otfTag; }

    OTFTagStruct(const char* p)
    {
        mOTFTag = (unsigned)(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        // Checking version:
        // mOTFTag = (unsigned)((p[0] << 24) | (!p[1] ? 0 : ((p[1] << 16) | (!p[2] ? 0 : (p[2] << 8) | p[3]))));
    }

    //operator uint32_t() const // Disabled because it can cause compiler conversion confusion.
    //    { return mOTFTag; }

    uint32_t AsUint32() const
        { return mOTFTag; }

    const char* AsChar() const
        { return (char*)(void*)&mOTFTag; }
};

inline bool operator==(const OTFTagStruct t1, const OTFTagStruct t2)
{
    return (t1.mOTFTag == t2.mOTFTag);
}

inline bool operator==(const OTFTagStruct t, const char* p)
{
    return (t == OTFTagStruct(p));
}

inline bool operator==(const char* p, const OTFTagStruct t)
{
    return (t == OTFTagStruct(p));
}

inline bool operator==(const OTFTagStruct t, OTFTag otfTag)
{
    return (t.mOTFTag == otfTag);
}

inline bool operator!=(const OTFTagStruct t, OTFTag otfTag)
{
    return (t.mOTFTag != otfTag);
}

// OTFTagStructArray
//
// Defines a fixed-size array of UTFTags
typedef eastl::fixed_vector<OTFTagStruct, 16, true> OTFTagStructArray16;




/// OTFError
///
/// Defines errors resulting from problems reading OpenType data.
///
enum OTFError
{
    kOTFErrorNone,
    kOTFErrorMemory,
    kOTFErrorFile,
    kOTFErrorTable,
    kOTFErrorHead,
    kOTFErrorName,
    kOTFErrorCmap,
    kOTFErrorGdef,
    kOTFErrorGpos,
    kOTFErrorGsub
};


/// OTFTableType
///
enum OTFTableType
{
    kOTFTableTypeHead,
    kOTFTableTypeName,
    kOTFTableTypeGdef,
    kOTFTableTypeGpos,
    kOTFTableTypeGsub,
    kOTFTableTypeCount  // Count of table types.
};


///////////////////////////////////////////////////////////////////////////////
// 'head' record
//
// http://www.microsoft.com/typography/otspec/head.htm
///////////////////////////////////////////////////////////////////////////////

/// OTFHead
///
/// See the OpenType standard regarding the 'head' record.
///
struct EATEXT_API OTFHead
{
    OTFFixedPoint mTableVersionNumber;
    OTFFixedPoint mFontRevision;        // Windows uses the version string (id 5) in the 'name' table instead of this.
    uint32_t      mCheckSumAdjustment;
    uint32_t      mMagicNumber;
    uint16_t      mFlags;
    uint16_t      mUnitsPerEm;
    uint32_t      mReservedPadding;     // Not defined in the standard; allows us to memcpy structs.
    int64_t       mCreationTime;
    int64_t       mModificationTime;
    int16_t       mXMin;
    int16_t       mYMin;
    int16_t       mXMax;
    int16_t       mYMax;
    uint16_t      mMacStyle;
    uint16_t      mLowestRecPPEM;
    int16_t       mFontDirectionHint;
    int16_t       mIndexToLocFormat;
    int16_t       mGlyphDataFormat;
};



///////////////////////////////////////////////////////////////////////////////
// 'name' record
//
// http://www.microsoft.com/typography/otspec/name.htm
///////////////////////////////////////////////////////////////////////////////

const size_t kOTFNameCount = 21;

struct EATEXT_API OTFNameRecord
{
    uint16_t mPlatformId;
    uint16_t mEncodingId;
    uint16_t mLanguageId;
    uint16_t mNameId;
    uint16_t mLength;
    uint16_t mOffset;
    uint8_t* mpName;    // Dynamically allocated. Will be NULL unless user wants it loaded.
};

struct EATEXT_API OTFName
{
    uint16_t      mFormat;
    uint16_t      mCount;               // Should be <= kOTFNameCount.
    uint16_t      mStringOffset;
    OTFNameRecord mNameRecord[kOTFNameCount];
};



///////////////////////////////////////////////////////////////////////////////
// 'cmap' record
//
// http://www.microsoft.com/typography/otspec/cmap.htm
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// Format 0 (bytes)

struct EATEXT_API OTFCharMap0
{
    uint16_t mFormat;
    uint16_t mLength;
    uint16_t mLanguage;
    uint8_t  mGlyphIdArray[256];
};

/////////////////////////////////////////////
// Format 2 (UCS2 Unicode)

struct EATEXT_API OTFCmapSubHeader
{
    uint16_t mFirstCode;
    uint16_t mEntryCount;
    int16_t  mIdDelta;
    uint16_t mIdRangeOffset;
};

struct EATEXT_API OTFCharMap2
{
    uint16_t          mFormat;
    uint16_t          mLength;
    uint16_t          mLanguage;
    uint16_t          mSubHeaderKeys[256];
    int16_t           mSubHeaderCount;
    OTFCmapSubHeader* mpSubHeaderArray;
    uint16_t          mGlyphIndexCount;
    OTFGlyphId*       mpGlyphIndexArray;
};

/////////////////////////////////////////////
// Format 4 (UCS2 Unicode)

struct EATEXT_API OTFCmapSegument
{
    uint16_t mEndCount;
    uint16_t mStartCount;
    int16_t  mIdDelta;
    uint16_t mIdRangeOffset;
};

struct EATEXT_API OTFCharMap4
{
    uint16_t          mFormat;
    uint16_t          mLength;
    uint16_t          mLanguage;
    uint16_t          mSegCountX2;
    uint16_t          mSearchRange;
    uint16_t          mEntrySelector;
    uint16_t          mRangeShift;
    OTFCmapSegument*  mpSegmentArray;
    uint16_t          mGlyphCount;
    OTFGlyphId*       mpGlyphIdArray;
};

/////////////////////////////////////////////
// Format 6

struct EATEXT_API OTFCharMap6
{
    uint16_t    mFormat;
    uint16_t    mLength;
    uint16_t    mLanguage;
    uint16_t    mFirstCode;
    uint16_t    mGlyphCount; // Called "entryCount" in the standard.
    OTFGlyphId* mpGlyphIdArray;
};

/////////////////////////////////////////////
// Format 8 (UCS2 and UCS4 Unicode)

struct EATEXT_API OTFCmapGroup
{
    uint32_t mStartCharCode;
    uint32_t mEndCharCode;
    uint32_t mStartGlyphID;
};

struct EATEXT_API OTFCharMap8
{
    uint16_t      mFormat;
    uint16_t      mReserved;
    uint32_t      mLength;
    uint32_t      mLanguage;
    uint8_t       mbIs32[8192];  // Bit array
    uint32_t      mGroupCount;
    OTFCmapGroup* mpGroupArray;
};

/////////////////////////////////////////////
// Format 10 (UCS4 Unicode) (Not supported by Microsoft)

struct EATEXT_API OTFCharMap10
{
    uint16_t    mFormat;
    uint16_t    mReserved;
    uint32_t    mLength;
    uint32_t    mLanguage;
    uint32_t    mStartCharCode;
    uint32_t    mGlyphCount;
    uint16_t*   mpGlyphIdArray;
};


/////////////////////////////////////////////
// Format 12 (UCS4 Unicode)

struct EATEXT_API OTFCharMap12
{
    uint16_t      mFormat;
    uint16_t      mReserved;
    uint32_t      mLength;
    uint32_t      mLanguage;
    uint32_t      mGroupCount;
    OTFCmapGroup* mpGroupArray;
};


struct EATEXT_API OTFEncodingRecord
{
    uint16_t mPlatformId;
    uint16_t mEncodingId;    // Platform-specific encoding ID. 1 => UCS2 Unicode.
    uint32_t mMapOffset;     // Byte offset from beginning of table to the subtable for this encoding.

    union 
    {
        OTFCharMap0*  mpMap0;
        OTFCharMap2*  mpMap2;
        OTFCharMap4*  mpMap4;
        OTFCharMap6*  mpMap6;
        OTFCharMap8*  mpMap8;
        OTFCharMap12* mpMap12;
    } mMap;
};

struct EATEXT_API OTFCmap
{
    uint16_t           mVersion;
    uint16_t           mEncodingRecordCount;
    OTFEncodingRecord* mpEncodingRecordArray;
    uint32_t           mGlyphIdMax;
    uint16_t*          mpEncodingArray; // Map of UCS2 to GlyphId.
    uint16_t*          mpDecodingArray; // Map of GlyphId to UCS2.
};





///////////////////////////////////////////////////////////////////////////////
// Shared structs
///////////////////////////////////////////////////////////////////////////////

/// OTFGlyphClass
///
/// This enum defines predefined class values used by the Gdef, Gpos, and Gsub
/// records. 
///
enum OTFGlyphClass
{
    kOTFGlyphClassOther     = 0,
    kOTFGlyphClassBase      = 1,
    kOTFGlyphClassLigature  = 2,
    kOTFGlyphClassMark      = 3,
    kOTFGlyphClassComponent = 4,
    kOTFGlyphClassCount     = 5 //Count of enumerated glyph classes.
};

struct EATEXT_API OTFRangeRecord
{
    OTFGlyphId mStart;
    OTFGlyphId mLast;
    uint16_t   mStartCoverageIndex; // I think this refers to the count of glyphs in all range records before this, but the OpenType Standard isn't clear at all about it.
};


// Each subtable (except an Extension LookupType subtable) in a lookup references 
// a Coverage table (Coverage), which specifies all the glyphs affected 
// by a substitution or positioning operation described in the subtable. 
// The GSUB, GPOS, and GDEF tables rely on this notion of coverage. 
// If a glyph does not appear in a Coverage table, the client can skip 
// that subtable and move immediately to the next subtable.
//
struct EATEXT_API OTFCoverage
{
    uint16_t mCoverageFormat;  // Either 1 or 2
    uint16_t mCount;

    union {
        OTFGlyphId*     mpGlyphIdArray;         // Individual glyphs, sorted from low to high.
        OTFRangeRecord* mpRangeRecordArray;     // Spans of glyphs, sorted from low to high.
    } mTable;

    // Returns the location of the glyphId if it is present in the Coverage. 
    // Returns -1 if not covered. This function can thus be used to test of a 
    // glyph is represented in an OTFCoverage.
    int32_t GetCoverageIndex(OTFGlyphId glyphId) const;
};

struct EATEXT_API OTFDeviceTable
{
    OTFOffset mOffset;
    uint32_t  mStartSize;
    uint32_t  mEndSize;
    uint32_t  mDeltaFormat;
    char*     mpDeltaValueArray;
};

struct EATEXT_API OTFClassRangeRecord // To consider: Pack this struct into 32 bits instead of 64 bits. Need to make sure it's feasible first.
{
    OTFGlyphId mStart;  // First GlyphID in the range
    OTFGlyphId mLast;   // Last GlyphID in the range
    uint16_t   mClass;  // Applied to all glyphs in the range. mClass is an arbitrary number that is referenced by other OpenType data. However, for the gdef record the class values are not arbitrary but are always of enum OTFGlyphClass. And GSUB and GPOS lookups refer to these same values.
};

struct EATEXT_API OTFClassDefFormat1
{
    OTFGlyphId mStartGlyph;         // First GlyphID of the ClassValueArray.
    uint16_t   mGlyphCount;         // Size of the ClassValueArray.
    uint16_t*  mpClassValueArray;   // Array of Class Values-one per GlyphID.
};

struct EATEXT_API OTFClassDefFormat2
{
    uint16_t             mClassRangeCount;          // Number of RangeRecords
    OTFClassRangeRecord* mpClassRangeRecordArray;   // Array of glyph ranges-ordered by Start GlyphID
};

struct EATEXT_API OTFClassDef // See http://www.microsoft.com/typography/otspec/chapter2.htm
{
    uint16_t mClassFormat; // Either 1 or 2

    union
    {
        OTFClassDefFormat1 mFormat1;
        OTFClassDefFormat2 mFormat2;
    } mFormat;

    uint32_t GetGlyphClass(OTFGlyphId glyphId) const;
};



///////////////////////////////////////////////////////////////////////////////
// 'gdef' record
//
// http://www.microsoft.com/typography/otspec/gdef.htm
// The Glyph Definition (GDEF) table contains the following tables:
//
//    1 The GlyphClassDef table classifies the different types of glyphs in the font.
//    2 The AttachmentList table identifies all attachment points on the glyphs, 
//      which streamlines data access and bitmap caching.
//    3 The LigatureCaretList table contains positioning data for ligature carets, 
//      which the text-processing client uses on screen to select and highlight 
//      the individual components of a ligature glyph.
//    4 The MarkAttachClassDef table classifies mark glyphs, to help group 
//      together marks that are positioned similarly. 
//
// Both the GSUB and GPOS tables reference the GDEF table information to supplement 
// their own data for substituting and positioning glyphs. Even so, a GDEF table is 
// optional for a font, included at the discretion of the font developer. Without a 
// GDEF table, however, the text-processing client may have to define and maintain 
// the GDEF information on its own when substituting and positioning glyphs. 
///////////////////////////////////////////////////////////////////////////////

struct EATEXT_API OTFAttachPoint
{
    uint16_t  mPointIndexCount; // Number of attachment points on this glyph
    uint16_t* mPointIndexArray; // Array of contour point indices, in increasing numerical order.
};

struct EATEXT_API OTFAttachList
{
    OTFOffset       mCoverageOffset;    // Offset to OTFCoverage, relative to OTFAttachList position. 
    OTFCoverage     mCoverage;          // 
    uint16_t        mGlyphCount;        // Number of glyphs with attachment points.
    OTFAttachPoint* mpAttachPointArray; // Array of offsets to AttachPoint tables-from beginning of AttachList table-in Coverage Index order
};

struct EATEXT_API OTFCaretValue
{
    OTFOffset mOffset;
    uint16_t  mCaretValueFormat; // 1, 2, or 3

    union
    {
        union
        {
            int16_t mCoordinate; // X or Y value, in design units
        } mFormat1;

        union
        {
            uint16_t mCaretValuePoint; // Contour point index on glyph
        } mFormat2;

        union
        {
            int16_t        mCoordinate; // X or Y value, in design units
            OTFDeviceTable mDeviceTable;
        } mFormat3;
    } mFormat;
};


struct EATEXT_API OTFLigatureGlyph
{
    OTFOffset      mOffset;
    uint16_t       mCaretValueCount;
    OTFCaretValue* mpCaretValueArray;
};


struct EATEXT_API OTFLigatureCaretList
{
    OTFCoverage       mCoverage;            // Offset to Coverage table - from beginning of LigCaretList table
    uint16_t          mLigatureGlyphCount;  // Number of ligature glyphs
    OTFLigatureGlyph* mpLigatureGlyphArray;
};


struct EATEXT_API OTFGdef
{
    OTFFixedPoint        mVersion;
    OTFOffset            mOffsetGlyphClassDef;          // Offset relative to GDEF header (this struct) in the file.
    OTFOffset            mOffsetAttachList;
    OTFOffset            mOffsetLigCaretList;
    OTFOffset            mOffsetMarkAttachClassDef;

    OTFClassDef          mGlyphClassDef;                // This OTFClassDef uses classes that aren't arbitrary but are of type OTFGlyphClass.
    OTFAttachList        mAttachList;
    OTFLigatureCaretList mLigatureCaretList;
    OTFClassDef          mMarkAttachClassDef;
};



///////////////////////////////////////////////////////////////////////////////
// ScriptList
// FeatureList
// LookupList
///////////////////////////////////////////////////////////////////////////////
//
// The GSUB and GPOS structure hierarchy on disk.
//   ScriptList
//     ScriptRecord[]
//       ScriptTag
//     Script[]
//       DefaultLangSys
//       LangSysRecord[]
//         LangSysTag
//       LangSys[]
//         LookupOrder
//         ReqFeatureIndex
//         FeatureIndex[]
//  FeatureList
//    FeatureRecord[]
//      FeatureTag
//    Feature[]
//      FeatureParams
//      LookupListIndex[]
//  LookupList
//    LookupOffset[]
//    Lookup[]
//      LookupType
//      LookupFlag
//      SubTableOffset[]
//      SubTable.Gsub[] or SubTable.Gpos[]
//
///////////////////////////////////////////////////////////////////////////////

struct EATEXT_API OTFLangSys
{
    OTFOffset  mLookupOrder;        // = NULL (reserved for an offset to a reordering table).
    uint16_t   mReqFeatureIndex;    // Index of a feature required for this language system- if no required features = 0xFFFF.
    uint16_t   mFeatureCount;       // Number of FeatureIndex values for this language system-excludes the required feature.
    uint16_t*  mpFeatureIndexArray; // Array of indices into the FeatureList-in arbitrary order.
};

struct EATEXT_API OTFLangSysRecord
{
    OTFTag     mLangSysTag;      // 4-byte LangSysTag identifier.
    OTFOffset  mLangSysOffset;   // Offset to LangSys table-from beginning of Script table.
    OTFLangSys mLangSys;         // The LangSys table.
};

struct EATEXT_API OTFScript
{
    OTFTag            mScriptTag;                   // 4-byte ScriptTag identifier.
    OTFOffset         mScriptTableOffset;           // Offset to Script table-from beginning of ScriptList.
    OTFOffset         mDefaultLangSysOffset;        // Offset to DefaultLangSys table-from beginning of Script table-may be NULL.
    OTFLangSys        mDefaultLangSys;              // Default LangSys.
    uint16_t          mLangSysRecordCount;          // Number of LangSysRecords for this script-excluding the DefaultLangSys.
    OTFLangSysRecord* mpLangSysRecordArray;         // Array of LangSysRecords-listed alphabetically by LangSysTag.
};

struct EATEXT_API OTFScriptList
{
    OTFOffset  mOffset;             // Offset to this ScriptList from beginning of parent record (e.g. GSUB or GPOS).
    uint16_t   mScriptCount;        //
    OTFScript* mpScriptArray;       //

    const OTFLangSys* GetLangSys(const OTFTagStruct& script, const OTFTagStruct& language, bool bRequireScriptMatch) const;
};

struct EATEXT_API OTFFeature
{
    OTFTag     mFeatureTag;             // 4-byte feature identification tag.
    OTFOffset  mFeatureTableOffset;     // Offset to Feature table-from beginning of FeatureList.
    OTFOffset  mFeatureParams;          // = NULL (reserved for offset to FeatureParams).
    uint16_t   mLookupListIndexCount;   // Number of LookupList indices for this feature.
    uint16_t*  mpLookupListIndexArray;  // Array of LookupList indices for this feature -zero-based (first lookup is LookupListIndex = 0).
};

struct EATEXT_API OTFFeatureList
{
    OTFOffset   mOffset;            // Offset to this FeatureList from beginning of parent record (e.g. GSUB or GPOS).
    uint16_t    mFeatureCount;      // Number of FeatureRecords in this table.
    OTFFeature* mpFeatureArray;     // Array of FeatureRecords-zero-based (first feature has FeatureIndex = 0)-listed alphabetically by FeatureTag.
};

enum OTFLookupFlagBit
{
    kOTFLookupFlagRightToLeft        = 0x0001,
    kOTFLookupFlagIgnoreBaseGlyphs   = 0x0002,
    kOTFLookupFlagIgnoreLigatures    = 0x0004,
    kOTFLookupFlagIgnoreMarks        = 0x0008,
    kOTFLookupFlagReserved           = 0x00f0,
    kOTFLookupFlagMarkAttachmentType = 0xff00
};

const int OTFLookupFlagIgnoreMask = (kOTFLookupFlagIgnoreBaseGlyphs | kOTFLookupFlagIgnoreLigatures | kOTFLookupFlagIgnoreMarks);


///////////////////////////////////////////////////////////////////////////////
// 'gsub' record
//
// http://www.microsoft.com/typography/otspec/gsub.htm
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
//
struct EATEXT_API OTFLookupSubTableGsub;
struct EATEXT_API OTFLookupSubTableGpos;

struct EATEXT_API OTFLookup
{
    // To consider: It might be useful to put a OTFTableType mTableType variable here, so we know if this applies to Gpos or Gsub.
    OTFOffset  mOffset;
    uint16_t   mLookupType;             // Different enumerations for GSUB and GPOS. GSUB uses values of 1-8 (see http://www.microsoft.com/typography/otspec/gsub.htm).
    uint16_t   mLookupFlags;            // Lookup qualifiers. See OTFLookupFlagBit. e.g. kOTFLookupFlagIgnoreBaseGlyphs
    uint16_t   mSubTableOffsetCount;    // Number of SubTables for this lookup.
    OTFOffset* mpSubTableOffsetArray;   // Array of offsets to SubTables-from beginning of Lookup table.

    union
    {
        OTFLookupSubTableGsub* mpGsubArray;
        OTFLookupSubTableGpos* mpGposArray;
    } mSubTable;
};

typedef eastl::fixed_vector<OTFLookup*, 16> OTFLookupPtrArray16;


struct EATEXT_API OTFLookupList
{
    OTFOffset  mOffset;
    uint16_t   mLookupCount;    // Number of lookups in this table.
    OTFLookup* mpLookupArray;   // Array of offsets to Lookup tables-from beginning of LookupList -zero based (first lookup is Lookup index = 0).
};

struct EATEXT_API OTFLookupRecord
{
    uint16_t mSequenceIndex;    // Index into current glyph sequence-first glyph = 0.
    uint16_t mLookupListIndex;  // Lookup to apply to that position-zero-based.
};

struct EATEXT_API OTFRule
{
    OTFOffset        mOffset;
    uint32_t         mGlyphCount;
    OTFGlyphId*      mpGlyphIdArray;
    uint32_t         mLookupRecordCount;
    OTFLookupRecord* mpLookupRecord;
};

struct EATEXT_API OTFRuleSet
{
    OTFOffset mOffset;
    uint32_t  mRuleCount;
    OTFRule*  mpRuleArray;
};

struct EATEXT_API OTFClassRule
{
    OTFOffset        mOffset;
    uint16_t         mGlyphCount;           // Total number of classes specified for the context in the rule-includes the first class.
    uint16_t*        mpClassArray;          // [GlyphCount - 1] Array of classes-beginning with the second class-to be matched to the input glyph class sequence.
    uint16_t         mSubstCount;           // Number of SubstLookupRecords (OTFLookupRecords).
    OTFLookupRecord* mpLookupRecordArray;   // Array of SubstLookupRecords (OTFLookupRecords) (in design order).
};

struct EATEXT_API OTFClassSet
{
    OTFOffset     mOffset;
    uint16_t      mClassRuleCount;
    OTFClassRule* mpClassRuleArray;
};

struct EATEXT_API OTFChainRule
{
    OTFOffset        mOffset;
    uint16_t         mBacktrackGlyphCount;
    OTFGlyphId*      mpBacktrackGlyphArray;
    uint16_t         mInputGlyphCount;
    OTFGlyphId*      mpInputGlyphArray;
    uint16_t         mLookaheadGlyphCount;
    OTFGlyphId*      mpLookaheadGlyphArray;
    uint16_t         mLookupRecordCount;
    OTFLookupRecord* mpLookupRecordArray;
};

struct EATEXT_API OTFChainRuleSet
{
    OTFOffset     mOffset;
    uint16_t      mChainRuleCount;
    OTFChainRule* mpChainRuleArray;
};

struct EATEXT_API OTFChainClassRule
{
    OTFOffset        mOffset;                   // Offset of this struct relative to position of parent struct.
    uint16_t         mBacktrackGlyphCount;      // Total number of glyphs in the backtrack sequence (number of glyphs to be matched before the first glyph).
    uint16_t*        mpBacktrackGlyphArray;     // Array of backtracking classes(to be matched before the input sequence).
    uint16_t         mInputGlyphCount;          // Total number of classes in the input sequence (includes the first class).
    uint16_t*        mpInputGlyphArray;         // [InputGlyphCount - 1] Array of input classes(start with second class; to be matched with the input glyph sequence).
    uint16_t         mLookaheadGlyphCount;      // Total number of classes in the look ahead sequence (number of classes to be matched after the input sequence).
    uint16_t*        mpLookaheadGlyphArray;     // Array of lookahead classes(to be matched after the input sequence).
    uint16_t         mSubstCount;               // Number of SubstLookupRecords (OTFLookupRecords).
    OTFLookupRecord* mpLookupRecordArray;       // Array of SubstLookupRecords (OTFLookupRecords) (in design order).
};

struct EATEXT_API OTFChainClassSet
{
    OTFOffset          mOffset;
    uint16_t           mChainClassRuleCount;    // Number of ChainSubClassRule tables
    OTFChainClassRule* mpChainClassRuleArray;
};


struct EATEXT_API OTFContext1
{
    uint16_t    mRuleSetCount;
    OTFRuleSet* mpRuleSetArray;
};

struct EATEXT_API OTFContext2
{
    OTFClassDef  mClassDef;
    uint16_t     mClassSetCount;    // Number of SubClassSet tables.
    OTFClassSet* mpClassSetArray;
};

struct EATEXT_API OTFContext3
{
    uint16_t         mGlyphCount;
    OTFCoverage*     mpGlyphArray;          // Holds a GlyphId array.
    uint16_t         mLookupRecordCount;
    OTFLookupRecord* mpLookupRecordArray;
};

struct EATEXT_API OTFChainContext1
{
    uint16_t         mChainRuleSetCount;
    OTFChainRuleSet* mpChainRuleSetArray;
};

struct EATEXT_API OTFChainContext2
{
    OTFClassDef       mBacktrackClassDef;
    OTFClassDef       mInputClassDef;
    OTFClassDef       mLookaheadClassDef;
    uint16_t          mChainClassSetCount;
    OTFChainClassSet* mpChainClassSetArray;
};

struct EATEXT_API OTFChainContext3
{
    uint16_t         mBacktrackGlyphCount;
    OTFCoverage*     mpBacktrackGlyphArray; // Holds a GlyphId array.
    uint16_t         mInputGlyphCount;
    OTFCoverage*     mpInputGlyphArray;     // Holds a GlyphId array.
    uint16_t         mLookaheadGlyphCount;
    OTFCoverage*     mpLookAheadGlyphArray; // Holds a GlyphId array.
    uint16_t         mLookupRecordCount;
    OTFLookupRecord* mpLookupRecordArray;
};

// FeatureLookup is a data structure compiled from mScriptList, mFeatureList, mLookupList.
struct FeatureLookup
{
    OTFTagStruct        mFeatureTag;        // 
    uint32_t            mGlyphFlags;        // Only glyphs that have these flags are considered for the given feature. The meaning of flags depends on the script. For example, in Arabic the flags denote glyphs as initial, medial, final, or isolated. And you only want to apply the 'init' substitution feature to glyphs flagged as initial.
    OTFLookupPtrArray16 mLookupPtrArray;    // 
};

typedef eastl::fixed_vector<FeatureLookup, 10, true> FeatureLookupArray;

struct EATEXT_API OTFGsubGpos
{
    OTFFixedPoint  mVersion;
    OTFOffset      mOffsetScriptList;          // Offset relative to GPOS header (this struct) in the file.
    OTFOffset      mOffsetFeatureList;
    OTFOffset      mOffsetLookupList;

    OTFScriptList  mScriptList;
    OTFFeatureList mFeatureList;
    OTFLookupList  mLookupList;

    bool Empty() const;

    // For a given script/language/feature list, BuildFeatureLookup builds a list of available
    // features and the associated lookup array for each feature. It allows for faster 
    // gsub and gpos lookups at runtime. The order of elements in FeatureLookupArray is significant
    // as OpenType features need to be applied in a specific order (e.g. substitution before ligation).
    void BuildFeatureLookup(const OTFTagStruct& script, const OTFTagStruct& language, FeatureLookupArray& featureLookupArray) const;
};


///////////////////////////////////////////////////
// OTFGsubSingle1
//
// For every glyph in the coverage, the subsituted glyph is at a fixed offset.
// This format is uncommon.
//
struct EATEXT_API OTFGsubSingle1
{
    int16_t mDeltaGlyphId;
};

///////////////////////////////////////////////////
// OTFGsubSingle2
//
// For every glyph in the coverage, the substitute glyph is identified by a table lookup.
// The substitution array must contain the same number of glyph indices as the Coverage table. 
// This format is common.
//
struct EATEXT_API OTFGsubSingle2
{
    uint16_t    mSubstitutionCount;
    OTFGlyphId* mpSubstitutionArray;
};

///////////////////////////////////////////////////
// OTFGsubMultiple1
//
// A Multiple Substitution (MultipleSubst) subtable replaces a single glyph with 
// more than one glyph, as when multiple glyphs replace a single ligature. 
// This is the reverse of forming a ligature; it is decomposing a ligature. 
//
struct EATEXT_API OTFSequence
{
    OTFOffset   mOffset;                // offset to Sequence table-from beginning of Substitution table-ordered by Coverage Index.
    uint16_t    mSubstitutionCount;
    OTFGlyphId* mpSubstitutionArray;
};

struct EATEXT_API OTFGsubMultiple1
{
    uint16_t     mSequenceCount;        // Number of Sequence table offsets in the Sequence array.
    OTFSequence* mpSequenceArray;
};


///////////////////////////////////////////////////
// OTFGsubAlternate1
//
// This is used to provide arbitrary alternative forms of glyphs.
// It allows a single font to have two or more different looks.
// This feature is not commonly used, as there are higher level mechanisms
// which do this kind of thing better (e.g. HTML/CSS).
//
struct EATEXT_API OTFAlternateSet
{
    OTFOffset   mOffset;
    uint16_t    mAlternateCount;
    OTFGlyphId* mpAlternateArray;
};

struct EATEXT_API OTFGsubAlternate1
{
    uint16_t         mAlternateSetCount;
    OTFAlternateSet* mpAlternateSetArray;
};


///////////////////////////////////////////////////
// OTFGsubLigature1
//
struct EATEXT_API OTFLigature
{
    OTFOffset   mOffset;                    // Offset of this struct relative to parent struct.
    OTFGlyphId  mGlyph;                     // GlyphID of ligature to substitute. Note that OpenType supports N to 1 glyph conversion but not N to M glyph conversion.
    uint16_t    mComponentCount;            // Number of components in the ligature.
    OTFGlyphId* mpComponentArray;           // [mComponentCount - 1] Array of component GlyphIDs-start with the second component-ordered in writing direction.
};

struct EATEXT_API OTFLigatureSet
{
    OTFOffset    mOffset;                   // Offset of this struct relative to parent struct.
    uint16_t     mLigatureCount;            // Number of Ligature tables.
    OTFLigature* mpLigatureArray;           // Ligature tables.
};

struct EATEXT_API OTFGsubLigature1
{
    uint16_t        mLigatureSetCount;      // Number of sets of ligatures.
    OTFLigatureSet* mpLigatureSetArray;     // The ligature sets.
};


typedef OTFContext1      OTFGsubContext1;
typedef OTFContext2      OTFGsubContext2;
typedef OTFContext3      OTFGsubContext3;
typedef OTFChainContext1 OTFGsubChainContext1;
typedef OTFChainContext2 OTFGsubChainContext2;
typedef OTFChainContext3 OTFGsubChainContext3;

struct EATEXT_API OTFGsubExtension1
{
    uint16_t               mExtensionLookupType;        // Lookup type of subtable referenced by ExtensionOffset (i.e. the extension subtable).
    uint32_t               mExtensionOffset;            // (32 bit) Offset to the extension subtable, of lookup type ExtensionLookupType, relative to the start of the ExtensionSubstFormat1 subtable.
    OTFLookupSubTableGsub* mpExtensionSubTableArray;
};

struct EATEXT_API OTFGsubReverseChain1
{
    uint16_t     mBacktrackGlyphCount;
    OTFCoverage* mpBacktrackGlyphArray; // Holds a GlyphId array.
    uint16_t     mLookaheadGlyphCount;
    OTFCoverage* mpLookaheadGlyphArray; // Holds a GlyphId array.
    uint16_t     mSubstitutionCount;
    OTFGlyphId*  mpSubstitutionArray;   // Holds a GlyphId array.
};

struct EATEXT_API OTFLookupSubTableGsub
{
    uint16_t    mFormat;         // Format identifier-format. A value from 1 to 3. See the comments below.
    OTFOffset   mCoverageOffset; // Offset of Coverage table.
    OTFCoverage mCoverage;       // Coverage table.

    union
    {
        // LookupType 1 (single glyph converted to another glyph)
        OTFGsubSingle1 mSingle1;        // Format 1 (mFormat == 1)
        OTFGsubSingle2 mSingle2;        // Format 2

        // LookupType 2 (single glyph expanded to multiple glyphs)
        OTFGsubMultiple1 mMultiple1;

        // LookupType 3 (arbitrary alternative representations of a glyph)
        OTFGsubAlternate1 mAlternate1;

        // LookupType 4 (multiple glyphs condensed to a single glyph)
        OTFGsubLigature1 mLigature1;

        // LookupType 5 (substitution of glyphs based on their context)
        OTFGsubContext1 mContext1;
        OTFGsubContext2 mContext2; // This is the most commonly used of the three formats.
        OTFGsubContext3 mContext3;

        // LookupType 6 (substitution of glyphs based on their context)
        OTFGsubChainContext1 mChainContext1;
        OTFGsubChainContext2 mChainContext2; // This is the most commonly used of the three formats.
        OTFGsubChainContext3 mChainContext3;

        // LookupType 7 (rarely used)
        OTFGsubExtension1 mExtension1;

        // LookupType 8 (rarely used)
        OTFGsubReverseChain1 mReverseChain1;
    }mLookup;
};

struct EATEXT_API OTFGsub : public OTFGsubGpos
{
    // Empty. We inherit everything from our parent.
};




///////////////////////////////////////////////////////////////////////////////
// 'GPOS' record
//
// http://www.microsoft.com/typography/otspec/gpos.htm
///////////////////////////////////////////////////////////////////////////////

enum OTFValueFormat
{
    kOTFValueFormatXPlacement       = 0x0001,
    kOTFValueFormatYPlacement       = 0x0002,
    kOTFValueFormatXAdvance         = 0x0004,
    kOTFValueFormatYAdvance         = 0x0008,
    kOTFValueFormatXPlacementDevice = 0x0010,
    kOTFValueFormatYPlacementDevice = 0x0020,
    kOTFValueFormatXAdvanceDevice   = 0x0040,
    kOTFValueFormatYAdvanceDevice   = 0x0080
};

struct EATEXT_API OTFValueRecord
{
    int32_t        mXPlacement;
    int32_t        mYPlacement;
    int32_t        mXAdvance;
    int32_t        mYAdvance;
    OTFDeviceTable mXPlacementDevice;
    OTFDeviceTable mYPlacementDevice;
    OTFDeviceTable mXAdvanceDevice;
    OTFDeviceTable mYAdvanceDevice;
};

struct EATEXT_API OTFAnchor
{
    OTFOffset mOffset;
    uint16_t  mAnchorFormat;
    int32_t   mXCoordinate;
    int32_t   mYCoordinate;

    union
    {
        union // Anchor format 1
        {
            uint16_t mAnchorPoint;
        } mAnchor1;

        union // Anchor format 2
        {
            OTFDeviceTable mXDeviceTable;
            OTFDeviceTable mYDeviceTable;
        } mAnchor2;
    } mAnchor;
};

struct EATEXT_API OTFMarkRecord
{
    uint16_t    mClass;
    OTFAnchor   mMarkAnchor;
};

struct EATEXT_API OTFMarkArray
{
    OTFOffset      mOffset;
    uint16_t       mMarkRecordCount;
    OTFMarkRecord* mpMarkRecordArray;
};

/////////////////////////////////////////////////
// OTFGposSingle1

struct EATEXT_API OTFGposSingle1
{
    uint16_t       mValueFormat;
    OTFValueRecord mValueRecord;
};

/////////////////////////////////////////////////
// OTFGposSingle2

struct EATEXT_API OTFGposSingle2
{
    uint16_t        mValueFormat;
    uint16_t        mValueCount;
    OTFValueRecord* mpValueArray;
};

/////////////////////////////////////////////////
// OTFGposPair1

struct EATEXT_API OTFPairValueRecord
{
    OTFGlyphId     mSecondGlyph;
    OTFValueRecord mValueRecord1;
    OTFValueRecord mValueRecord2;
};

struct EATEXT_API OTFPairSet
{
    OTFOffset           mOffset;
    uint16_t            mPairValueRecordCount;
    OTFPairValueRecord* mpPairValueRecordArray;
};

struct EATEXT_API OTFGposPair1
{
    uint16_t    mValueFormat1;
    uint16_t    mValueFormat2;
    uint16_t    mPairSetCount;
    OTFPairSet* mpPairSetArray;
};

/////////////////////////////////////////////////
// OTFGposPair2

struct EATEXT_API OTFClass2Record
{
    OTFValueRecord mValueRecord1;
    OTFValueRecord mValueRecord2;
};

struct EATEXT_API OTFClass1Record
{
    OTFClass2Record* mpClass2RecordArray;
};

struct EATEXT_API OTFGposPair2
{
    uint16_t         mValueFormat1;
    uint16_t         mValueFormat2;
    OTFClassDef      mClassDef1;
    OTFClassDef      mClassDef2;
    uint16_t         mClass1RecordCount;
    OTFClass1Record* mpClass1RecordArray;
    uint16_t         mClass2RecordCount;
};

/////////////////////////////////////////////////
// OTFGposCursive1

struct EATEXT_API OTFEntryExitRecord
{
    OTFAnchor mEntryAnchor;
    OTFAnchor mExitAnchor;
};

struct EATEXT_API OTFGposCursive1
{
    uint16_t            mEntryExitRecordCount;
    OTFEntryExitRecord* mpEntryExitRecordArray;
};

/////////////////////////////////////////////////
// OTFGposMarkBase1

struct EATEXT_API OTFAnchorRecord
{
    OTFAnchor* mpAnchorArray;
};

struct EATEXT_API OTFAnchorArray
{
    OTFOffset        mOffset;
    uint16_t         mAnchorRecordCount;
    OTFAnchorRecord* mpAnchorRecordArray;
};

struct EATEXT_API OTFGposMarkBase1
{
    OTFCoverage    mBaseCoverage;
    uint16_t       mClassCount;
    OTFMarkArray   mMarkArray;
    OTFAnchorArray mBaseArray;
};

/////////////////////////////////////////////////
// OTFGposMarkLig1

struct EATEXT_API OTFComponentRecord
{
    OTFAnchor* mpAnchorArray;
};

struct EATEXT_API OTFLigatureAttach
{
    OTFOffset           mOffset;
    uint16_t            mComponentRecordCount;
    OTFComponentRecord* mpComponentRecordArray;
};

struct EATEXT_API OTFLigatureArray
{
    OTFOffset          mOffset;
    uint16_t           mLigatureAttachCount;
    OTFLigatureAttach* mpLigatureAttachArray;
};

struct EATEXT_API OTFGposMarkLig1
{
    OTFCoverage      mLigatureCoverage;
    uint16_t         mClassCount;
    OTFMarkArray     mMarkArray;
    OTFLigatureArray mLigatureArray;
};

/////////////////////////////////////////////////
// OTFGposMarkMark1

struct EATEXT_API OTFGposMarkMark1
{
    OTFCoverage    mMark2Coverage;
    uint16_t       mClassCount;
    OTFMarkArray   mMark1Array;
    OTFAnchorArray mMark2Array;
};


typedef OTFContext1      OTFGposContext1;
typedef OTFContext2      OTFGposContext2;
typedef OTFContext3      OTFGposContext3;
typedef OTFChainContext1 OTFGposChainContext1;
typedef OTFChainContext2 OTFGposChainContext2;
typedef OTFChainContext3 OTFGposChainContext3;

struct EATEXT_API OTFGposExtension1
{
    uint16_t               mExtensionLookupType;
    uint16_t               mExtensionOffset;
    OTFLookupSubTableGpos* mpExtensionSubtable;
};

struct EATEXT_API OTFLookupSubTableGpos
{
    uint16_t    mFormat;
    OTFCoverage mCoverage;

    union
    {
        // LookupType 1
        OTFGposSingle1 mSingle1;
        OTFGposSingle2 mSingle2;

        // LookupType 2
        OTFGposPair1 mPair1;
        OTFGposPair2 mPair2;

        // LookupType 3
        OTFGposCursive1 mCursive1;

        // LookupType 4
        OTFGposMarkBase1 mMarkBase1;

        // LookupType 5
        OTFGposMarkLig1 mMarkLig1;

        // LookupType 6
        OTFGposMarkMark1 mMarkMark1;

        // LookupType 7
        OTFGposContext1 mContext1;
        OTFGposContext2 mContext2;
        OTFGposContext3 mContext3;

        // LookupType 8
        OTFGposChainContext1 mChainContext1;
        OTFGposChainContext2 mChainContext2;
        OTFGposChainContext3 mChainContext3;

        // LookupType 9
        OTFGposExtension1 mExtension1;
    } mLookup;
};

typedef OTFGsubGpos OTFGpos;


/// OTFOffsetTable
///
/// Every TrueType font begins with this.
/// 
struct EATEXT_API OTFOffsetTable
{
    OTFFixedPoint mSfntVersion; // This will be 0x00010000 for version 1.0.
    uint16_t      mTableCount;
    uint16_t      mSearchRange;
    uint16_t      mEnterSelector;
    uint16_t      mRangeShift;
};

struct EATEXT_API OTFTableDirectoryEntry
{
    OTFTagStruct mTag;
    uint32_t     mCheckSum;
    uint32_t     mOffset;
    uint32_t     mLength;
};


///////////////////////////////////////////////////////////////////////////////
// OTF
//
// This is the primary OpenType reading class.
//
class EATEXT_API OTF
{
public:
    OTFOffsetTable          mOffsetTable;
    OTFTableDirectoryEntry  mTableDirectory[kOTFTableTypeCount];
    OTFHead                 mHead;
    OTFGdef                 mGdef;
    OTFGsub                 mGsub;
    OTFGpos                 mGpos;

public:
    OTF(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
   ~OTF();

    void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);
    bool Load(IO::IStream* pFontStream);
    bool Unload();
    bool IsLoaded() const;
    bool IsScriptSupported(const char* pScript, const char* pLanguage = NULL) const;

protected:
    OTFTableDirectoryEntry* GetTableDirectoryEntry(const char* pType);

    bool ReadUint16(uint16_t& value);
    bool ReadUint16(uint16_t* pValueArray, IO::size_type count);

    bool ReadUint32(uint32_t& value);
    bool ReadUint32(uint32_t* pValueArray, IO::size_type count);

    bool ReadTableDirectory();
    bool ReadHead();
    bool ReadName();
    bool ReadGdef();
    bool ReadGsub();
    bool ReadGpos();

    bool ReadClassDef(OTFClassDef& classDef, uint32_t fileOffset);
    bool ReadCoverage(OTFCoverage& coverage, uint32_t fileOffset);
    bool ReadScriptList(OTFScriptList& scriptList, uint32_t fileOffset);
    bool ReadFeatureList(OTFFeatureList& featureList, uint32_t fileOffset);
    bool ReadLookupList(OTFLookupList& lookupList, uint32_t fileOffset, OTFTableType tableType);
    bool ReadGsubArrayEntry(uint32_t substitutionTableOffset, uint32_t nType, uint32_t nFormat, OTFLookupSubTableGsub& gsubEntry);

protected:
    bool                          mbLoaded;
    IO::IStream*                  mpFontStream;
    Allocator::StackAllocator     mStackAlloctor;
    Allocator::ICoreAllocator*    mpCoreAllocator;      // This is used to supply the core memory for mStackAllocator. We don't usually allocate from mpAllocator directly.
    OTFError                      mLastError;
};




} // namespace Text

} // namespace EA


#endif // Header include guard



















