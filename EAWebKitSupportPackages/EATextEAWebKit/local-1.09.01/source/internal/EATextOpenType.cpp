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
// EATextOpenType.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/internal/EATextOpenType.h>
#include <EAText/internal/EAStackAllocatorNew.h>
#include <EAText/internal/EATextCoreAllocatorNew.h>
#include <EAText/internal/EATextCoreAllocatorStack.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/EAStreamAdapter.h>
#include <EASTL/fixed_vector.h>
#include <string.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{



typedef eastl::fixed_vector<uint16_t, 1024, true> GlyphBuffer;
typedef eastl::fixed_vector<uint16_t, 1024, true> OffsetBuffer;


// kOTFTableTypeNames
//
// This must match the order of values in enum OTFTableType.
//
const char* kOTFTableTypeNames[kOTFTableTypeCount] = 
{
    "head", "name", "GDEF", "GPOS", "GSUB"
};


static inline uint16_t OTFFromBigEndianUint16(uint16_t n16)
{
    #if defined(EA_SYSTEM_LITTLE_ENDIAN)
        n16 = (uint16_t) ((n16 >> 8) | (n16 << 8));
    #endif

    return n16;
}


static inline uint32_t OTFFromBigEndianUint32(uint32_t n32)
{
    #if defined(EA_SYSTEM_LITTLE_ENDIAN)
        n32 = ((n32 >> 24)               |
              ((n32 << 24) & 0xff000000) |
              ((n32 <<  8) & 0x00ff0000) |
              ((n32 >>  8) & 0x0000ff00)); 
    #endif

    return n32;
}



///////////////////////////////////////////////////////////////////////////////
// OTF
//
OTF::OTF(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mbLoaded(false),
    mpFontStream(NULL),
    mStackAlloctor(),
    mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
    mLastError(kOTFErrorNone)
{
    // We can safely do these memsets because all these structs are PODs.
    memset(&mOffsetTable, 0, sizeof(mOffsetTable));
    memset(mTableDirectory, 0, sizeof(mTableDirectory));
    memset(&mHead, 0, sizeof(mHead));
    memset(&mGdef, 0, sizeof(mGdef));
    memset(&mGsub, 0, sizeof(mGsub));
    memset(&mGpos, 0, sizeof(mGpos));
}


///////////////////////////////////////////////////////////////////////////////
// ~OTF
//
OTF::~OTF()
{
    Unload();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void OTF::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;
}


///////////////////////////////////////////////////////////////////////////////
// Load
//
bool OTF::Load(IO::IStream* pFontStream)
{
    if(!mbLoaded)
    {
        // Regardless of the success of this function, we set mbIsLoaded to true.
        mbLoaded = true;

        mStackAlloctor.Init(NULL, 512, Text::StackAllocatorCoreAllocate, Text::StackAllocatorCoreFree, mpCoreAllocator);

        EA_ASSERT(pFontStream);
        mpFontStream = pFontStream;
        mpFontStream->AddRef();

        if(ReadTableDirectory())
        {
            ReadHead();
            ReadName();
            ReadGdef();
            ReadGsub();
            ReadGpos();

            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Unload
//
bool OTF::Unload()
{
    if(mpFontStream)
    {
        mpFontStream->Release();
        mpFontStream = NULL;
    }

    // This Reset call will free all the allocations for all the table structs.
    mStackAlloctor.Reset();

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// IsLoaded
//
bool OTF::IsLoaded() const
{
    return mbLoaded;
}


///////////////////////////////////////////////////////////////////////////////
// IsScriptSupported
//
bool OTF::IsScriptSupported(const char* pScript, const char* pLanguage) const
{
    if(mbLoaded)
    {
        const OTFTagStruct tagScript(pScript); // tagScript is a uint32_t version of the name, such as 'arab'.

        if((tagScript == kOTFTag_arab) || (tagScript == kOTFTag_deva))
        {
            // Arabic text layout requires OpenType glyph substitution. You cannot 
            // in any practicality draw Arabic text correctly without substitution
            // information. We assume that if any substitution records exist for 
            // Arabic script then Arabic script is supported. Ditto for Devanagari.
            const OTFTagStruct tagLanguage(pLanguage ? OTFTagStruct(pLanguage) : kOTFTag_0000);

            const OTFLangSys* const pLangSys = mGsub.mScriptList.GetLangSys(tagScript, tagLanguage, true);

            return (pLangSys != NULL);
        }

        // Other scripts don't actually require OpenType information to work. 
        // So we return true regardless of the presence and nature of any 
        // OpenType information in this font.
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadUint16
//
bool OTF::ReadUint16(uint16_t& value)
{
    return IO::ReadUint16(mpFontStream, value, IO::kEndianBig);
}


///////////////////////////////////////////////////////////////////////////////
// ReadUint16
//
bool OTF::ReadUint16(uint16_t* pValueArray, IO::size_type count)
{
    return IO::ReadUint16(mpFontStream, pValueArray, count, IO::kEndianBig);
}


///////////////////////////////////////////////////////////////////////////////
// ReadUint32
//
bool OTF::ReadUint32(uint32_t& value)
{
    return IO::ReadUint32(mpFontStream, value, IO::kEndianBig);
}


///////////////////////////////////////////////////////////////////////////////
// ReadUint32
//
bool OTF::ReadUint32(uint32_t* pValueArray, IO::size_type count)
{
    return IO::ReadUint32(mpFontStream, pValueArray, count, IO::kEndianBig);
}


///////////////////////////////////////////////////////////////////////////////
// GetTableDirectoryEntry
//
OTFTableDirectoryEntry* OTF::GetTableDirectoryEntry(const char* pType)
{
    // To consider: There ought to be a faster way to do this.
    const OTFTagStruct t(pType);

    for(size_t i = 0; i < kOTFTableTypeCount; i++)
    {
        if(mTableDirectory[i].mTag == t)
            return &mTableDirectory[i];
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// ReadTableDirectory
//
bool OTF::ReadTableDirectory()
{
    mpFontStream->SetPosition(0);

    IO::size_type count = mpFontStream->Read(&mOffsetTable, sizeof(mOffsetTable));

    if(count == sizeof(mOffsetTable))
    {
        mOffsetTable.mSfntVersion.mValue32 = OTFFromBigEndianUint32(mOffsetTable.mSfntVersion.mValue32);
        //EA_ASSERT(mOffsetTable.mSfntVersion.mValue32 == 0x00010000);

        if(mOffsetTable.mSfntVersion.mValue32 == 0x00010000)
        {
            mOffsetTable.mTableCount = OTFFromBigEndianUint16(mOffsetTable.mTableCount);
            memset(mTableDirectory, 0, sizeof(mTableDirectory));

            // Go through all tables directory entries and collect the ones we are interested in.
            for(size_t i = 0; i < mOffsetTable.mTableCount; i++)
            {
                OTFTableDirectoryEntry temp;
                mpFontStream->Read(&temp, sizeof(temp));
                temp.mTag.mOTFTag = OTFFromBigEndianUint32(temp.mTag.mOTFTag);

                for(size_t k = 0; k < kOTFTableTypeCount; ++k)
                {
                    if(temp.mTag == kOTFTableTypeNames[k]) // kOTFTableTypeNames is an array of "head", "name", "GPOS", etc.
                    {
                        temp.mCheckSum = OTFFromBigEndianUint32(temp.mCheckSum);
                        temp.mOffset   = OTFFromBigEndianUint32(temp.mOffset);
                        temp.mLength   = OTFFromBigEndianUint32(temp.mLength);

                        mTableDirectory[k] = temp;
                        break;
                    }
                }
            }

            EA_ASSERT(GetTableDirectoryEntry("head"));
            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadHead
//
bool OTF::ReadHead()
{
    const OTFTableDirectoryEntry& teHead = mTableDirectory[kOTFTableTypeHead];

    EA_ASSERT(teHead.mOffset != 0); // Assert that it is present.
    mpFontStream->SetPosition(teHead.mOffset);
    mpFontStream->Read(&mHead, sizeof(OTFHead));

    mHead.mMagicNumber = OTFFromBigEndianUint32(mHead.mMagicNumber);
    EA_ASSERT(mHead.mMagicNumber == 0x5F0F3CF5);
    if(mHead.mMagicNumber == 0x5F0F3CF5)
    {
        // To consider: Byte-swap the struct values. However, these values may not be important to us.
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadName
//
bool OTF::ReadName()
{
    // Currently disabled because we don't use the 'name' record.
    //const OTFTableDirectoryEntry& teName = mTableDirectory[kOTFTableTypeName];
    //EA_ASSERT(teName.mOffset != 0); // Assert that it is present.
    //
    //memset(&mName, 0, sizeof(mName));
    //mpFontStream->SetPosition(teName.mOffset);
    //mpFontStream->Read(&mName, sizeof(OTFName));

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadGdef
//
// See http://www.microsoft.com/typography/otspec/gdef.htm
//
bool OTF::ReadGdef()
{
    const OTFTableDirectoryEntry& teGdef = mTableDirectory[kOTFTableTypeGdef];

    if(teGdef.mOffset) // If this table is present...
    {
        mpFontStream->SetPosition(teGdef.mOffset);
        mpFontStream->Read(&mGdef, sizeof(OTFGdef)); // This is too large of a read.

        if(mGdef.mOffsetGlyphClassDef) // This will usually be valid for complex scripts.
        {
            mGdef.mOffsetGlyphClassDef = OTFFromBigEndianUint16(mGdef.mOffsetGlyphClassDef);
            ReadClassDef(mGdef.mGlyphClassDef, teGdef.mOffset + mGdef.mOffsetGlyphClassDef);
        }

        if(mGdef.mOffsetAttachList) // This isn't very often used.
        {
            mGdef.mOffsetAttachList = OTFFromBigEndianUint16(mGdef.mOffsetAttachList);
            mpFontStream->SetPosition(teGdef.mOffset + mGdef.mOffsetAttachList);

            // We read the coverage offset here, but we don't read mCoverage from that offset until later below.
            // The reason for this delay is that it causes less disk head thrashing.
            ReadUint16(mGdef.mAttachList.mCoverageOffset);

            ReadUint16(mGdef.mAttachList.mGlyphCount);
            EA_ASSERT(mGdef.mAttachList.mGlyphCount < 8192); // Sanity check.

            // Need to read mGdef.mAttachList.mGlyphCount offsets. 
            // Offsets are relative to the beginning of the attach list table (i.e. mGdef.mOffsetAttachList).
            OffsetBuffer offsetBuffer(mGdef.mAttachList.mGlyphCount, 0);
            ReadUint16(&offsetBuffer[0], mGdef.mAttachList.mGlyphCount);

            mGdef.mAttachList.mpAttachPointArray = 
                    EATEXT_SA_NEW_ARRAY(OTFAttachPoint, &mStackAlloctor, mGdef.mAttachList.mGlyphCount);

            for(uint32_t i = 0; i < mGdef.mAttachList.mGlyphCount; i++)
            {
                mpFontStream->SetPosition(teGdef.mOffset + mGdef.mOffsetAttachList + offsetBuffer[i]);

                ReadUint16(mGdef.mAttachList.mpAttachPointArray[i].mPointIndexCount);
                EA_ASSERT(mGdef.mAttachList.mpAttachPointArray[i].mPointIndexCount < 8192); // Sanity check.
                
                mGdef.mAttachList.mpAttachPointArray[i].mPointIndexArray = 
                        EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, mGdef.mAttachList.mpAttachPointArray[i].mPointIndexCount);

                ReadUint16(mGdef.mAttachList.mpAttachPointArray[i].mPointIndexArray, 
                            mGdef.mAttachList.mpAttachPointArray[i].mPointIndexCount);
            }

            // We delay this read in order to reduce disk head thrashing. This Coverage data is often after the above data in the file anyway.
            ReadCoverage(mGdef.mAttachList.mCoverage, teGdef.mOffset + mGdef.mOffsetAttachList + mGdef.mAttachList.mCoverageOffset);
        }

        if(mGdef.mOffsetLigCaretList) // This isn't very often used.
        {
            // For now we don't support the ligature caret list.
            //mGdef.mOffsetLigCaretList = OTFFromBigEndianUint16(mGdef.mOffsetLigCaretList);
            memset(&mGdef.mLigatureCaretList, 0, sizeof(mGdef.mLigatureCaretList));
        }

        if(mGdef.mOffsetMarkAttachClassDef) // This isn't very often used.
        {
            mGdef.mOffsetMarkAttachClassDef = OTFFromBigEndianUint16(mGdef.mOffsetMarkAttachClassDef);
            ReadClassDef(mGdef.mMarkAttachClassDef, teGdef.mOffset + mGdef.mOffsetMarkAttachClassDef);
        }

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadClassDef
//
// Reads a generic ClassDef.
//
// See http://www.microsoft.com/typography/otspec/chapter2.htm
// A font developer can assign any glyph to any class, each identified with 
// an integer called a class value. A Class Definition table (ClassDef) groups
// glyph indices by class, beginning with Class 1, then Class 2, and so on. 
// All glyphs not assigned to a class fall into Class 0. Within a given class 
// definition table, each glyph in the font belongs to exactly one class.
// 
bool OTF::ReadClassDef(OTFClassDef& classDef, uint32_t fileOffset)
{
    mpFontStream->SetPosition(fileOffset);

    // Read the class format.
    ReadUint16(classDef.mClassFormat); // Usually the format will be 2.

    // Read the class def data.
    if(classDef.mClassFormat == 1)
    {
        ReadUint16(classDef.mFormat.mFormat1.mStartGlyph);
        ReadUint16(classDef.mFormat.mFormat1.mGlyphCount);
        EA_ASSERT(classDef.mFormat.mFormat1.mGlyphCount < 8192); // Sanity check.

        classDef.mFormat.mFormat1.mpClassValueArray = 
                EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, classDef.mFormat.mFormat1.mGlyphCount);

        ReadUint16(classDef.mFormat.mFormat1.mpClassValueArray, 
                    classDef.mFormat.mFormat1.mGlyphCount);
    }
    else if(classDef.mClassFormat == 2)
    {
        ReadUint16(classDef.mFormat.mFormat2.mClassRangeCount);
        EA_ASSERT(classDef.mFormat.mFormat2.mClassRangeCount < 8192); // Sanity check.

        classDef.mFormat.mFormat2.mpClassRangeRecordArray = 
                EATEXT_SA_NEW_ARRAY(OTFClassRangeRecord, &mStackAlloctor, classDef.mFormat.mFormat2.mClassRangeCount);

        for(uint32_t i = 0; i < classDef.mFormat.mFormat2.mClassRangeCount; i++)
        {
            ReadUint16(classDef.mFormat.mFormat2.mpClassRangeRecordArray[i].mStart);
            ReadUint16(classDef.mFormat.mFormat2.mpClassRangeRecordArray[i].mLast);
            ReadUint16(classDef.mFormat.mFormat2.mpClassRangeRecordArray[i].mClass);

            EA_ASSERT(classDef.mFormat.mFormat2.mpClassRangeRecordArray[i].mStart <= classDef.mFormat.mFormat2.mpClassRangeRecordArray[i].mLast); // Sanity check.
            // For the gdef record, mClass must always be of enum OTFGlyphClass. And also, the Gsub and Gpos lookup classes also must be of enum OTFGlyphClass.
            // However, we are a generic function here and we don't know if we are being used to read one of these types of class defs.
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadCoverage
//
// Reads a generic OTFCoverage record.
//
// See http://www.microsoft.com/typography/otspec/chapter2.htm
//
bool OTF::ReadCoverage(OTFCoverage& coverage, uint32_t fileOffset)
{
    mpFontStream->SetPosition(fileOffset);

    // Read the class format.
    ReadUint16(coverage.mCoverageFormat);
    //EA_ASSERT((coverage.mCoverageFormat == 1) || (coverage.mCoverageFormat == 2));

    // Read the count.
    ReadUint16(coverage.mCount);
    
    // Read the coverage data.
    if(coverage.mCoverageFormat == 1)
    {
        coverage.mTable.mpGlyphIdArray = 
                EATEXT_SA_NEW_ARRAY(OTFGlyphId, &mStackAlloctor, coverage.mCount);

        ReadUint16(coverage.mTable.mpGlyphIdArray, coverage.mCount);
    }
    else if(coverage.mCoverageFormat == 2)
    {
        coverage.mTable.mpRangeRecordArray =
                EATEXT_SA_NEW_ARRAY(OTFRangeRecord, &mStackAlloctor, coverage.mCount);

        for(uint32_t i = 0; i < coverage.mCount; i++)
        {
            ReadUint16(coverage.mTable.mpRangeRecordArray[i].mStart);
            ReadUint16(coverage.mTable.mpRangeRecordArray[i].mLast);
            ReadUint16(coverage.mTable.mpRangeRecordArray[i].mStartCoverageIndex);
        }
    }
    else
    {
       // EA_ASSERT(false);
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadGsub
//
// See http://www.microsoft.com/typography/otspec/gsub.htm
//
bool OTF::ReadGsub()
{
    const OTFTableDirectoryEntry& teGsub = mTableDirectory[kOTFTableTypeGsub];

    if(teGsub.mOffset) // If this table is present (and it usually is for complex scripts such as Arabic and Devanagari)...
    {
        mpFontStream->SetPosition(teGsub.mOffset);
        mpFontStream->Read(&mGsub, sizeof(OTFGsub));  // This is too large of a read.

        if(mGsub.mOffsetScriptList) // This will usually be valid for complex scripts.
        {
            mGsub.mOffsetScriptList = OTFFromBigEndianUint16(mGsub.mOffsetScriptList);
            ReadScriptList(mGsub.mScriptList, teGsub.mOffset + mGsub.mOffsetScriptList);
        }

        if(mGsub.mOffsetFeatureList) // This will usually be valid for complex scripts.
        {
            mGsub.mOffsetFeatureList = OTFFromBigEndianUint16(mGsub.mOffsetFeatureList);
            ReadFeatureList(mGsub.mFeatureList, teGsub.mOffset + mGsub.mOffsetFeatureList);
        }

        if(mGsub.mOffsetLookupList) // This will usually be valid for complex scripts.
        {
            mGsub.mOffsetLookupList = OTFFromBigEndianUint16(mGsub.mOffsetLookupList);
            ReadLookupList(mGsub.mLookupList, teGsub.mOffset + mGsub.mOffsetLookupList, kOTFTableTypeGsub);
        }

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadScriptList
//
// See http://www.microsoft.com/typography/otspec/chapter2.htm
//
bool OTF::ReadScriptList(OTFScriptList& scriptList, uint32_t fileOffset)
{
    uint32_t i, j;

    mpFontStream->SetPosition(fileOffset);

    // Read the script list count.
    ReadUint16(scriptList.mScriptCount);
    EA_ASSERT(scriptList.mScriptCount < 100); // Sanity check. For most fonts this number is less than five.

    // Read the script list.
    scriptList.mpScriptArray =
            EATEXT_SA_NEW_ARRAY(OTFScript, &mStackAlloctor, scriptList.mScriptCount);

    // Note that we write this separate loops instead of hierarchical loops. The reason for this 
    // is that the latter would trigger a lot of disk head thrashing while the current method
    // reads things in linear streams.

    for(i = 0; i < scriptList.mScriptCount; i++)
    {
        ReadUint32(scriptList.mpScriptArray[i].mScriptTag); // mScriptTag will be something like 'arab'.
        ReadUint16(scriptList.mpScriptArray[i].mScriptTableOffset);
    }

    for(i = 0; i < scriptList.mScriptCount; i++)
    {
        mpFontStream->SetPosition(fileOffset + scriptList.mpScriptArray[i].mScriptTableOffset);

        // We read the DefaultLangSys offset here, but we don't read mDefaultLangSys until below, 
        // in order to keep the file reading most efficient.
        ReadUint16(scriptList.mpScriptArray[i].mDefaultLangSysOffset);

        ReadUint16(scriptList.mpScriptArray[i].mLangSysRecordCount);
        EA_ASSERT(scriptList.mpScriptArray[i].mLangSysRecordCount < 200); // Sanity check.

        scriptList.mpScriptArray[i].mpLangSysRecordArray =
                EATEXT_SA_NEW_ARRAY(OTFLangSysRecord, &mStackAlloctor, scriptList.mpScriptArray[i].mLangSysRecordCount);

        for(j = 0; j < scriptList.mpScriptArray[i].mLangSysRecordCount; j++)
        {
            ReadUint32(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSysTag);
            ReadUint16(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSysOffset);
        }

        for(j = 0; j < scriptList.mpScriptArray[i].mLangSysRecordCount; j++)
        {
            // To do: Merge the code here with the equivalent code a little ways further below.
            mpFontStream->SetPosition(fileOffset + scriptList.mpScriptArray[i].mScriptTableOffset + scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSysOffset);

            ReadUint16(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mLookupOrder);
            EA_ASSERT(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mLookupOrder == 0); // Sanity check.

            ReadUint16(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mReqFeatureIndex); // A mReqFeatureIndex of 0xffff means 'no required features'.

            ReadUint16(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mFeatureCount);
            EA_ASSERT(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mFeatureCount < 100); // Sanity check.

            scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mpFeatureIndexArray = 
                    EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mFeatureCount);

            ReadUint16(scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mpFeatureIndexArray,
                        scriptList.mpScriptArray[i].mpLangSysRecordArray[j].mLangSys.mFeatureCount); // This will often result in an array of 0, 1, 2, etc.
        }

        // To do: Merge the code here with the equivalent code a little ways above.
        if(scriptList.mpScriptArray[i].mDefaultLangSysOffset)
        {
            mpFontStream->SetPosition(fileOffset + scriptList.mpScriptArray[i].mScriptTableOffset + scriptList.mpScriptArray[i].mDefaultLangSysOffset);
            
            ReadUint16(scriptList.mpScriptArray[i].mDefaultLangSys.mLookupOrder);
            ReadUint16(scriptList.mpScriptArray[i].mDefaultLangSys.mReqFeatureIndex);
            ReadUint16(scriptList.mpScriptArray[i].mDefaultLangSys.mFeatureCount);
            EA_ASSERT(scriptList.mpScriptArray[i].mDefaultLangSys.mFeatureCount < 1000); // Sanity check.

            scriptList.mpScriptArray[i].mDefaultLangSys.mpFeatureIndexArray =
                    EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, scriptList.mpScriptArray[i].mDefaultLangSys.mFeatureCount);

            ReadUint16(scriptList.mpScriptArray[i].mDefaultLangSys.mpFeatureIndexArray, 
                        scriptList.mpScriptArray[i].mDefaultLangSys.mFeatureCount);
        }
        else
            memset(&scriptList.mpScriptArray[i].mDefaultLangSys, 0, sizeof(scriptList.mpScriptArray[i].mDefaultLangSys));
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadFeatureList
//
// See http://www.microsoft.com/typography/otspec/chapter2.htm
//
bool OTF::ReadFeatureList(OTFFeatureList& featureList, uint32_t fileOffset)
{
    uint32_t i;

    mpFontStream->SetPosition(fileOffset);

    // Read the script list count.
    ReadUint16(featureList.mFeatureCount);
    EA_ASSERT(featureList.mFeatureCount < 500); // Sanity check.

    // Read the feature list.
    featureList.mpFeatureArray =
            EATEXT_SA_NEW_ARRAY(OTFFeature, &mStackAlloctor, featureList.mFeatureCount);

    // Note that we write this separate loops instead of hierarchical loops. The reason for this 
    // is that the latter would trigger a lot of disk head thrashing while the current method
    // reads things in linear streams.

    for(i = 0; i < featureList.mFeatureCount; i++)
    {
        ReadUint32(featureList.mpFeatureArray[i].mFeatureTag);    // This will be a value like (for Arabic) 'isol', 'init', medi', 'fina', 'liga', 'rlig'.
        ReadUint16(featureList.mpFeatureArray[i].mFeatureTableOffset);
    }

    for(i = 0; i < featureList.mFeatureCount; i++)
    {
        mpFontStream->SetPosition(fileOffset + featureList.mpFeatureArray[i].mFeatureTableOffset);

        ReadUint16(featureList.mpFeatureArray[i].mFeatureParams);
        EA_ASSERT(featureList.mpFeatureArray[i].mFeatureParams == 0); // Sanity check.

        ReadUint16(featureList.mpFeatureArray[i].mLookupListIndexCount);
        EA_ASSERT(featureList.mFeatureCount < 500); // Sanity check.

        featureList.mpFeatureArray[i].mpLookupListIndexArray = 
                EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, featureList.mpFeatureArray[i].mLookupListIndexCount);

        ReadUint16(featureList.mpFeatureArray[i].mpLookupListIndexArray, 
                    featureList.mpFeatureArray[i].mLookupListIndexCount);
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadLookupList
//
// See http://www.microsoft.com/typography/otspec/chapter2.htm
//
bool OTF::ReadLookupList(OTFLookupList& lookupList, uint32_t fileOffset, OTFTableType tableType)
{
    uint32_t i, j;

    mpFontStream->SetPosition(fileOffset);

    // Read the lookup list count.
    ReadUint16(lookupList.mLookupCount);
    EA_ASSERT(lookupList.mLookupCount < 1000); // Sanity check.

    // Read the feature list.
    lookupList.mpLookupArray =
            EATEXT_SA_NEW_ARRAY(OTFLookup, &mStackAlloctor, lookupList.mLookupCount);

    // Note that we write this separate loops instead of hierarchical loops. The reason for this 
    // is that the latter would trigger a lot of disk head thrashing while the current method
    // reads things in linear streams.

    for(i = 0; i < lookupList.mLookupCount; i++)
        ReadUint16(lookupList.mpLookupArray[i].mOffset);

    for(i = 0; i < lookupList.mLookupCount; i++)
    {
        mpFontStream->SetPosition(fileOffset + lookupList.mpLookupArray[i].mOffset);

        ReadUint16(lookupList.mpLookupArray[i].mLookupType);
        EA_ASSERT(lookupList.mpLookupArray[i].mLookupType < 15); // Sanity check.

        ReadUint16(lookupList.mpLookupArray[i].mLookupFlags);

        ReadUint16(lookupList.mpLookupArray[i].mSubTableOffsetCount);
        EA_ASSERT(lookupList.mpLookupArray[i].mSubTableOffsetCount < 8192); // Sanity check.

        lookupList.mpLookupArray[i].mpSubTableOffsetArray = 
                EATEXT_SA_NEW_ARRAY(OTFOffset, &mStackAlloctor, lookupList.mpLookupArray[i].mSubTableOffsetCount);

        ReadUint16(lookupList.mpLookupArray[i].mpSubTableOffsetArray, 
                    lookupList.mpLookupArray[i].mSubTableOffsetCount);
    }

    for(i = 0; i < lookupList.mLookupCount; i++)
    {
        if(tableType == kOTFTableTypeGsub)
        {
            lookupList.mpLookupArray[i].mSubTable.mpGsubArray = 
                    EATEXT_SA_NEW_ARRAY(OTFLookupSubTableGsub, &mStackAlloctor, lookupList.mpLookupArray[i].mSubTableOffsetCount);

            for(j = 0; j < lookupList.mpLookupArray[i].mSubTableOffsetCount; j++)
            {
                const uint32_t substitutionTableOffset = fileOffset + lookupList.mpLookupArray[i].mOffset + lookupList.mpLookupArray[i].mpSubTableOffsetArray[j];

                mpFontStream->SetPosition(substitutionTableOffset);

                ReadUint16(lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mFormat);
                EA_ASSERT(lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mFormat <= 3); // Sanity check.

                ReadUint16(lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mCoverageOffset);

                // How we read mpGsubArray depends on mLookupType and mFormat. The mpGsubArray is a union of 13 structs
                // and so we make a separate function for dealing choosing the right one of those 13 from mLookupType and mFormat and reading it.
                ReadGsubArrayEntry(substitutionTableOffset, lookupList.mpLookupArray[i].mLookupType, 
                            lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mFormat, lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j]);
            }

            for(j = 0; j < lookupList.mpLookupArray[i].mSubTableOffsetCount; j++)
            {
                const uint32_t coveragePosition = fileOffset + lookupList.mpLookupArray[i].mOffset + 
                                                  lookupList.mpLookupArray[i].mpSubTableOffsetArray[j] + 
                                                  lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mCoverageOffset;

                ReadCoverage(lookupList.mpLookupArray[i].mSubTable.mpGsubArray[j].mCoverage, coveragePosition);
            }
        }
        else // kOTFTableTypeGpos
        {
            //lookupList.mpLookupArray[i].mSubTable.mpGposArray = 
            //        EATEXT_SA_NEW_ARRAY(OTFLookupSubTableGpos, &mStackAlloctor, lookupList.mpLookupArray[i].mSubTableOffsetCount);
            // To do.
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadGsubArrayEntry
//
bool OTF::ReadGsubArrayEntry(uint32_t substitutionTableOffset, uint32_t nType, uint32_t nFormat, OTFLookupSubTableGsub& gsubEntry)
{
    // Upon entering this function, we've already read the gsubEntry.mFormat and mCoverageOffset fields.
    // We need to read the mLookup union here. The file stream pointer should be at the position
    // after mCoverageOffset in the OpenType record.
    uint32_t i, j;

    switch(nType)
    {
        case 1: // Single Substitution Subtable
        {
            if(nFormat == 1) // SingleSubstFormat1
            {
                // This format is rarely used.
                ReadInt16(mpFontStream, gsubEntry.mLookup.mSingle1.mDeltaGlyphId);
            }
            else // SingleSubstFormat2
            {
                // This format is commonly used with complex scripts.
                ReadUint16(gsubEntry.mLookup.mSingle2.mSubstitutionCount);
                EA_ASSERT(gsubEntry.mLookup.mSingle2.mSubstitutionCount < 10000); // Sanity check.
                // To do: Assert that mSubstitutionCount is the same size as the corresponding coverage table.

                gsubEntry.mLookup.mSingle2.mpSubstitutionArray = 
                        EATEXT_SA_NEW_ARRAY(OTFGlyphId, &mStackAlloctor, gsubEntry.mLookup.mSingle2.mSubstitutionCount);

                ReadUint16(gsubEntry.mLookup.mSingle2.mpSubstitutionArray, 
                            gsubEntry.mLookup.mSingle2.mSubstitutionCount);
            }

            break;
        }

        case 2: // Multiple Substitution Subtable (this is for glyph decomposition)
        {
            // This format is commonly used with complex scripts.
            ReadUint16(gsubEntry.mLookup.mMultiple1.mSequenceCount);
            EA_ASSERT(gsubEntry.mLookup.mMultiple1.mSequenceCount < 500); // Sanity check.

            gsubEntry.mLookup.mMultiple1.mpSequenceArray = 
                    EATEXT_SA_NEW_ARRAY(OTFSequence, &mStackAlloctor, gsubEntry.mLookup.mMultiple1.mSequenceCount);

            for(i = 0; i < gsubEntry.mLookup.mMultiple1.mSequenceCount; i++)
                ReadUint16(gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mOffset);

            for(i = 0; i < gsubEntry.mLookup.mMultiple1.mSequenceCount; i++)
            {
                mpFontStream->SetPosition(substitutionTableOffset + gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mOffset);

                ReadUint16(gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mSubstitutionCount);
                EA_ASSERT(gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mSubstitutionCount < 50); // Sanity check.

                gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mpSubstitutionArray = 
                        EATEXT_SA_NEW_ARRAY(OTFGlyphId, &mStackAlloctor, gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mSubstitutionCount);

                ReadUint16(gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mpSubstitutionArray, 
                            gsubEntry.mLookup.mMultiple1.mpSequenceArray[i].mSubstitutionCount);
            }

            break;
        }

        case 3: // Alternate Substitution Subtable
        {
            // This format is rarely used.
            // We intentionally don't read this record, as it is something that is simply not 
            // commonly used in the real world. If you want to have alternative glyph support, 
            // you can implement this or ask Paul Pedriana to implement it.
            gsubEntry.mLookup.mAlternate1.mAlternateSetCount = 0;
            gsubEntry.mLookup.mAlternate1.mpAlternateSetArray = NULL;

            break;
        }

        case 4: // Ligature Substitution Subtable
        {
            // This format is commonly used.

            ReadUint16(gsubEntry.mLookup.mLigature1.mLigatureSetCount);
            EA_ASSERT(gsubEntry.mLookup.mLigature1.mLigatureSetCount < 500); // Sanity check.

            gsubEntry.mLookup.mLigature1.mpLigatureSetArray = 
                    EATEXT_SA_NEW_ARRAY(OTFLigatureSet, &mStackAlloctor, gsubEntry.mLookup.mLigature1.mLigatureSetCount);

            for(i = 0; i < gsubEntry.mLookup.mLigature1.mLigatureSetCount; i++)
                ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mOffset);

            for(i = 0; i < gsubEntry.mLookup.mLigature1.mLigatureSetCount; i++)
            {
                mpFontStream->SetPosition(substitutionTableOffset + gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mOffset);

                ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mLigatureCount);
                EA_ASSERT(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mLigatureCount < 500); // Sanity check.

                gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray = 
                        EATEXT_SA_NEW_ARRAY(OTFLigature, &mStackAlloctor, gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mLigatureCount);

                for(j = 0; j < gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mLigatureCount; j++)
                    ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mOffset);

                for(j = 0; j < gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mLigatureCount; j++)
                {
                    const uint32_t ligatureOffset = substitutionTableOffset + gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mOffset + 
                                                        gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mOffset;
                    mpFontStream->SetPosition(ligatureOffset);

                    ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mGlyph);
                    ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mComponentCount);
                    EA_ASSERT(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mComponentCount < 500); // Sanity check.

                    // The mComponentCount variable refers to the total number of glyphs that get converted to a single ligature.
                    // The first glyph to be converted is specified in the coverage table, and the additional glyph(s) are 
                    // specified here. So if you have two glyphs that get converted to one, mComponentCount will be 2, 
                    // the first will be in the coverage array, and the second will be here. So mComponentCount is one greater than 
                    // the glyph count stored here.
                    const uint32_t arraySize = gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mComponentCount - 1;

                    gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mpComponentArray = 
                            EATEXT_SA_NEW_ARRAY(OTFGlyphId, &mStackAlloctor, arraySize);

                    ReadUint16(gsubEntry.mLookup.mLigature1.mpLigatureSetArray[i].mpLigatureArray[j].mpComponentArray, arraySize);
                }
            }

            break;
        }

        case 5: // Contextual Substitution Subtable
        {
            if(nFormat == 1) // ContextSubstFormat1
            {
                // We'll make the effort to support this when we see a font that uses this. Contact Paul Pedriana.
                EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: ContextSubstFormat1 not yet supported.");
                memset(&gsubEntry.mLookup.mContext1, 0, sizeof(gsubEntry.mLookup.mContext1));
            }
            else if(nFormat == 2) // ContextSubstFormat2
            {
                // We read the ClassDef offset here, we'll read the actual mClassDef below, for best performance.
                OTFOffset offsetClassDef;
                ReadUint16(offsetClassDef);

                ReadUint16(gsubEntry.mLookup.mContext2.mClassSetCount);
                EA_ASSERT(gsubEntry.mLookup.mContext2.mClassSetCount < 200); // Sanity check.

                gsubEntry.mLookup.mContext2.mpClassSetArray = 
                        EATEXT_SA_NEW_ARRAY(OTFClassSet, &mStackAlloctor, gsubEntry.mLookup.mContext2.mClassSetCount);

                for(i = 0; i < gsubEntry.mLookup.mContext2.mClassSetCount; i++) // Some of the resulting mOffsets here may be NULL. See the standard.
                    ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mOffset);

                for(i = 0; i < gsubEntry.mLookup.mContext2.mClassSetCount; i++)
                {
                    // "If no contexts begin with a particular class (that is, if a SubClassSet 
                    // contains no SubClassRule tables), then the offset to that particular 
                    // SubClassSet in the SubClassSet array will be set to NULL (0)."

                    if(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mOffset)
                    {
                        mpFontStream->SetPosition(substitutionTableOffset + gsubEntry.mLookup.mContext2.mpClassSetArray[i].mOffset);

                        ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount);
                        EA_ASSERT(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount < 200); // Sanity check.

                        gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray = 
                                EATEXT_SA_NEW_ARRAY(OTFClassRule, &mStackAlloctor, gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount);

                        for(j = 0; j < gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount; j++)
                            ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mOffset);

                        for(j = 0; j < gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount; j++)
                        {
                            const uint32_t classRuleOffset = substitutionTableOffset + gsubEntry.mLookup.mContext2.mpClassSetArray[i].mOffset + 
                                                                gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mOffset;

                            mpFontStream->SetPosition(classRuleOffset);

                            ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mGlyphCount);
                            EA_ASSERT(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mGlyphCount < 2000); // Sanity check.

                            gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mpClassArray = 
                                    EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mGlyphCount - 1);

                            ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mSubstCount);
                            EA_ASSERT(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mSubstCount < 2000); // Sanity check.

                            gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mpLookupRecordArray = 
                                    EATEXT_SA_NEW_ARRAY(OTFLookupRecord, &mStackAlloctor, gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mSubstCount);

                            // We read (mGlypyCount - 1) glyph ids, because that's how the standard works.
                            ReadUint16(gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mpClassArray, 
                                        gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mGlyphCount - 1);

                            // We do something that is, strictly speaking, questionable. We read an array of OTFLookupRecords 
                            // in a single read, as a OTFLookupRecord record has merely two uint32_t members.
                            EA_COMPILETIME_ASSERT(sizeof(OTFLookupRecord) == (2 * sizeof(uint16_t)));
                            ReadUint16((uint16_t*)(void*)&gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mpLookupRecordArray[0],
                                        gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray[j].mSubstCount * 2);
                        }
                    }
                    else
                    {
                        gsubEntry.mLookup.mContext2.mpClassSetArray[i].mClassRuleCount  = 0;
                        gsubEntry.mLookup.mContext2.mpClassSetArray[i].mpClassRuleArray = NULL;
                    }
                }

                // Here we read the ClassDef for which we got the offset earlier
                ReadClassDef(gsubEntry.mLookup.mContext2.mClassDef, substitutionTableOffset + offsetClassDef);
            }
            else // ContextSubstFormat3
            {
                // We'll make the effort to support this when we see a font that uses this. Contact Paul Pedriana.
                // EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: ContextSubstFormat3 not yet supported.");
                memset(&gsubEntry.mLookup.mContext3, 0, sizeof(gsubEntry.mLookup.mContext3));
            }

            break;
        }

        case 6: // Chaining Contextual Substitution Subtable
        {
            if(nFormat == 1) // ChainContextSubstFormat1
            {
                // We'll make the effort to support this when we see a font that uses this. Contact Paul Pedriana.
                EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: ChainContextSubstFormat1 not yet supported.");
                memset(&gsubEntry.mLookup.mChainContext1, 0, sizeof(gsubEntry.mLookup.mChainContext1));
            }
            else if(nFormat == 2) // ChainContextSubstFormat2
            {
                // We read the ClassDef offset here, we'll read the actual mBacktrackClassDef below, for best performance.
                OTFOffset offsetBacktrackClassDef;
                ReadUint16(offsetBacktrackClassDef);

                OTFOffset inputClassDef;
                ReadUint16(inputClassDef);

                OTFOffset lookaheadClassDef;
                ReadUint16(lookaheadClassDef);

                // Rad the ChainClassSetArray
                ReadUint16(gsubEntry.mLookup.mChainContext2.mChainClassSetCount);

                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray = 
                        EATEXT_SA_NEW_ARRAY(OTFChainClassSet, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mChainClassSetCount);

                for(i = 0; i < gsubEntry.mLookup.mChainContext2.mChainClassSetCount; i++)
                    ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mOffset);

                for(i = 0; i < gsubEntry.mLookup.mChainContext2.mChainClassSetCount; i++)
                {
                    if(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mOffset)
                    {
                        mpFontStream->SetPosition(substitutionTableOffset + gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mOffset);

                        ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount);
                        EA_ASSERT(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount < 200); // Sanity check.

                        gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray = 
                                EATEXT_SA_NEW_ARRAY(OTFChainClassRule, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount);

                        for(j = 0; j < gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount; j++)
                            ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mOffset);

                        for(j = 0; j < gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount; j++)
                        {
                            const uint32_t chainClassRuleOffset = substitutionTableOffset + gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mOffset + 
                                                                    gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mOffset;

                            mpFontStream->SetPosition(chainClassRuleOffset);

                            // Read BacktrackGlyphArray
                            ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mBacktrackGlyphCount);
                            EA_ASSERT(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mBacktrackGlyphCount < 32); // Sanity check.

                            if(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mBacktrackGlyphCount)
                            {
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpBacktrackGlyphArray = 
                                        EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mBacktrackGlyphCount);

                                ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpBacktrackGlyphArray, 
                                            gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mBacktrackGlyphCount);
                            }
                            else
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpBacktrackGlyphArray = NULL;

                            // Read InputGlyphArray
                            ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mInputGlyphCount);
                            EA_ASSERT(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mInputGlyphCount < 32); // Sanity check.

                            // Note that we allocate and read (mInputGlyphCount - 1) glyphIds. 
                            if(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mInputGlyphCount > 1)
                            {
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpInputGlyphArray = 
                                        EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mInputGlyphCount - 1);

                                ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpInputGlyphArray, 
                                            gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mInputGlyphCount - 1);
                            }
                            else
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpInputGlyphArray = NULL;

                            // Read LookaheadGlyphArray
                            ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mLookaheadGlyphCount);

                            if(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mLookaheadGlyphCount)
                            {
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpLookaheadGlyphArray = 
                                        EATEXT_SA_NEW_ARRAY(uint16_t, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mLookaheadGlyphCount);

                                ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpLookaheadGlyphArray, 
                                            gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mLookaheadGlyphCount);
                            }
                            else
                                gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpLookaheadGlyphArray = NULL;

                            // Read LookupRecordArray
                            ReadUint16(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mSubstCount);
                            EA_ASSERT(gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mSubstCount < 500); // Sanity check.

                            gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpLookupRecordArray = 
                                    EATEXT_SA_NEW_ARRAY(OTFLookupRecord, &mStackAlloctor, gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mSubstCount);

                            // We do something that is, strictly speaking, questionable. We read an array of OTFLookupRecords 
                            // in a single read, as a OTFLookupRecord record has merely two uint32_t members.
                            EA_COMPILETIME_ASSERT(sizeof(OTFLookupRecord) == (2 * sizeof(uint16_t)));
                            ReadUint16((uint16_t*)(void*)&gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mpLookupRecordArray[0],
                                        gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray[j].mSubstCount * 2);
                        }
                    }
                    else
                    {
                        gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mChainClassRuleCount = 0;
                        gsubEntry.mLookup.mChainContext2.mpChainClassSetArray[i].mpChainClassRuleArray = NULL;
                    }
                }

                // Here we read the ClassDef for which we got the offset earlier
                if(offsetBacktrackClassDef)
                    ReadClassDef(gsubEntry.mLookup.mChainContext2.mBacktrackClassDef, substitutionTableOffset + offsetBacktrackClassDef);
                else
                    memset(&gsubEntry.mLookup.mChainContext2.mBacktrackClassDef, 0, sizeof(gsubEntry.mLookup.mChainContext2.mBacktrackClassDef));

                if(inputClassDef)
                    ReadClassDef(gsubEntry.mLookup.mChainContext2.mInputClassDef, substitutionTableOffset + inputClassDef);
                else
                    memset(&gsubEntry.mLookup.mChainContext2.mInputClassDef, 0, sizeof(gsubEntry.mLookup.mChainContext2.mInputClassDef));

                if(lookaheadClassDef)
                    ReadClassDef(gsubEntry.mLookup.mChainContext2.mLookaheadClassDef, substitutionTableOffset + lookaheadClassDef);
                else
                    memset(&gsubEntry.mLookup.mChainContext2.mLookaheadClassDef, 0, sizeof(gsubEntry.mLookup.mChainContext2.mLookaheadClassDef));
            }
            else // ChainContextSubstFormat3
            {
                // We'll make the effort to support this when we see a font that uses this. Contact Paul Pedriana.
                // EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: ChainContextSubstFormat3 not yet supported.");
                memset(&gsubEntry.mLookup.mChainContext3, 0, sizeof(gsubEntry.mLookup.mChainContext3));
            }

            break;
        }

        case 7: // Extension Substitution
        {
            // This format is rarely used. I (Paul Pedriana) have not seen it used on any font.
            // Its purpose is to support fonts files with so much information that the 16 bit offsets aren't enough.
            // To do: Make this fail optional. Sometimes we just set EAText to load all fonts in the 
            // system directory, and some huge fonts may in fact have this option even though we don't use that font.
            // EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: Extension Substitution not yet supported.");
            memset(&gsubEntry.mLookup.mExtension1, 0, sizeof(gsubEntry.mLookup.mExtension1));
            break;
        }

        case 8: // Reverse Chaining Contextual Single Substitution Subtable
        {
            // This format is rarely used. I (Paul Pedriana) have not seen this format used on any font.
            // To do: Make this fail optional. Sometimes we just set EAText to load all fonts in the 
            // system directory, and some huge fonts may in fact have this option even though we don't use that font.
            // EA_FAIL_MESSAGE("EAText::OTF::ReadGsubArrayEntry: Reverse Substitution not yet supported.");
            memset(&gsubEntry.mLookup.mReverseChain1, 0, sizeof(gsubEntry.mLookup.mReverseChain1));
            break;
        }
    }

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// ReadGpos
//
// See http://www.microsoft.com/typography/otspec/gpos.htm
//
// You need GPOS information if you want to read Devanagari (Hindi) script.
// You usually don't need it for other scripts, including complex scripts such as Arabic and Thai.
// However, one would think that Thai should be using GPOS information to position its ornaments.
// But you can achieve the same thing in Thai with contextual glyph substitution, or by knowing
// that in fact Windows Thai fonts all have the same substitution rules and so you don't really 
// need OpenType information to read the rules dynamically.
//
bool OTF::ReadGpos()
{
    const OTFTableDirectoryEntry& teGpos = mTableDirectory[kOTFTableTypeGpos];

    if(teGpos.mOffset) // If this table is present...
    {
        // To do.

        //mpFontStream->SetPosition(teGpos.mOffset);
        //mpFontStream->Read(&mGpos, sizeof(OTFGpos));
    }

    return false;
}



///////////////////////////////////////////////////////////////////////////////
// GetGlyphClass
//
uint32_t OTFClassDef::GetGlyphClass(OTFGlyphId glyphId) const
{
    if(mClassFormat == 2)
    {
        for(uint32_t i = 0; i < mFormat.mFormat2.mClassRangeCount; i++)
        {
            if((glyphId >= mFormat.mFormat2.mpClassRangeRecordArray[i].mStart) &&
               (glyphId <= mFormat.mFormat2.mpClassRangeRecordArray[i].mLast))
            {
                return mFormat.mFormat2.mpClassRangeRecordArray[i].mClass;
            }
        }
    }
    else // mClassFormat == 1
    {
        const uint32_t i = (glyphId - mFormat.mFormat1.mStartGlyph);

        if(i < mFormat.mFormat1.mGlyphCount) // 'i' is unsigned, so we don't need to check for (i < 0).
            return mFormat.mFormat1.mpClassValueArray[i];
    }

    return 0; // The default class is zero.
}


///////////////////////////////////////////////////////////////////////////////
// GetCoverageIndex
//
int32_t OTFCoverage::GetCoverageIndex(OTFGlyphId glyphId) const
{
    if(mCoverageFormat == 2)
    {
        for(uint32_t i = 0; i < mCount; i++)
        {
            if((mTable.mpRangeRecordArray[i].mStart <= glyphId) &&
               (mTable.mpRangeRecordArray[i].mLast  >= glyphId))
            {
                return (mTable.mpRangeRecordArray[i].mStartCoverageIndex + (glyphId - mTable.mpRangeRecordArray[i].mStart));
            }
        }
    }
    else // mCoverageFormat == 1
    {
        for(uint32_t i = 0; i < mCount; i++)
        {
            if(mTable.mpGlyphIdArray[i] == glyphId)
                return i;
        }
    }

    return -1;
}


///////////////////////////////////////////////////////////////////////////////
// GetLangSys
//
const OTFLangSys* OTFScriptList::GetLangSys(const OTFTagStruct& script, const OTFTagStruct& language, 
                                             bool bRequireScriptMatch) const
{
    const OTFScript* pScriptDefault = NULL;

    for(uint32_t i = 0; i < mScriptCount; i++)
    {
        const OTFScript& scriptCurrent = mpScriptArray[i];

        if(mpScriptArray[i].mScriptTag == kOTFTag_DFLT) // Default
            pScriptDefault = &scriptCurrent;

        if(mpScriptArray[i].mScriptTag == script) // If there is a match to what the user requested...
        {
            if(language != 0) // If a specific language is requested...
            {
                for(uint32_t j = 0; j < scriptCurrent.mLangSysRecordCount; j++)
                {
                    if(scriptCurrent.mpLangSysRecordArray[j].mLangSysTag == language)
                        return &scriptCurrent.mpLangSysRecordArray[j].mLangSys;
                }
            }

            return &scriptCurrent.mDefaultLangSys;
        }
    }

    if(!bRequireScriptMatch) // If it's OK to return a default LangSys when the specifically requested one was not found...
    {
        if(!pScriptDefault)
            pScriptDefault = &mpScriptArray[0];

        if(language != 0) // If a specific language is requested...
        {
            for(uint32_t j = 0; j < pScriptDefault->mLangSysRecordCount; j++)
            {
                if(pScriptDefault->mpLangSysRecordArray[j].mLangSysTag == language)
                    return &pScriptDefault->mpLangSysRecordArray[j].mLangSys;
            }
        }

        return &pScriptDefault->mDefaultLangSys;
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Empty
//
bool OTFGsubGpos::Empty() const
{
    return (mFeatureList.mFeatureCount == 0);
}


///////////////////////////////////////////////////////////////////////////////
// BuildFeatureLookup
//
void OTFGsubGpos::BuildFeatureLookup(const OTFTagStruct& script, const OTFTagStruct& language,
                                     FeatureLookupArray& featureLookupArray) const
{
    const OTFLangSys* const pLangSys = mScriptList.GetLangSys(script, language, false); // 'false' means to return the default LangSys if the specific one isn't present.

    // The loops below may look like an O(n^3) operation, but in practice it is actually
    // an (n^2) operation, because the 'k' loop will only ever get run for a single
    // 'j' loop. Since i,j,k are relatively small numbers, the performance here isn't
    // very bad. But it isn't trivial either and so it's best if the results of this
    // map lookup be cached and not rebuilt every time a run of glyphs are operated on.

    // featureLookupArray.size will almost always be in the range of [1,10]
    for(eastl_size_t i = 0, iEnd = featureLookupArray.size(); i < iEnd; i++) // For each requested feature...
    {
        FeatureLookup& featureLookup = featureLookupArray[i];

        // pLangSys->mFeatureCount will almost always be in the range of [1,10]
        for(size_t j = 0; j < pLangSys->mFeatureCount; j++) // For each available feature...
        {
            const uint32_t    featureIndex = pLangSys->mpFeatureIndexArray[j];
            const OTFFeature& feature      = mFeatureList.mpFeatureArray[featureIndex];

            if(feature.mFeatureTag == featureLookup.mFeatureTag)
            {
                featureLookup.mLookupPtrArray.clear();

                // feature.mLookupListIndexCount will usually be in the range of [1,30]
                for(size_t k = 0; k < feature.mLookupListIndexCount; k++) // For each lookup set for the feature...
                {
                    const uint32_t lookupIndex = feature.mpLookupListIndexArray[k];

                    featureLookup.mLookupPtrArray.push_back(&mLookupList.mpLookupArray[lookupIndex]);
                }

                break;
            }
        }
    }
}






} // namespace Text

} // namespace EA



















