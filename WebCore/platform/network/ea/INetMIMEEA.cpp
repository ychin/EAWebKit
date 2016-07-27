/*
Copyright (C) 2005, 2009, 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// INetMIMEEA.cpp
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#include "config.h"
#include "INetMIMEEA.h"

#include <internal/include/EAWebKitString.h>
#include <stddef.h>  
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>


namespace
{

char8_t Tolower(char8_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char8_t)(c + ('a' - 'A'));
    return c;
}

char16_t Tolower(char16_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char16_t)(c + ('a' - 'A'));
    return c;
}


    // Not sure why we were not using EAHash FNV1 before, but since we already
    // have our own in this file, I'm writing this one so we don't have to copy
    // the type string
    uint32_t FNVHash8Lower( const char8_t* pData, const char8_t* pDataEnd )
    {
        uint32_t nInitialValue = 2166136261U;

        for (; *pData && pData != pDataEnd; ++pData)
            nInitialValue = (nInitialValue * 16777619) ^ Tolower(*pData);

        return nInitialValue;
    }

    uint32_t FNVHash16(const char16_t* pData)
    {
        uint32_t nInitialValue = 2166136261U;
        const uint16_t* pData16 = (const uint16_t*)pData;

        while(*pData16)
            nInitialValue = (nInitialValue * 16777619) ^ *pData16++;

        return nInitialValue;
    }
}



namespace EA
{

namespace WebKit
{


///////////////////////////////////////////////////////////////////////////////
// MIMEStringToMIMETypes
//
bool MIMEStringToMIMETypes(const char8_t* pHeaderTypeString, MIMEType& mimeType, MIMEType& mimeSubType, uint32_t nMaxLength)
{
    const char8_t* pCurrentPosition = pHeaderTypeString;
    const char8_t* pEndPosition     = nMaxLength == UINT32_MAX ? (char8_t*)(pCurrentPosition + 0xffffffff) : pCurrentPosition + nMaxLength;

    mimeType = mimeSubType = kMIMETypeUnknown;

    while((pCurrentPosition < pEndPosition) && *pCurrentPosition)
    {
        if(*pCurrentPosition == '/')
        {
            mimeType = MIMETypeStringToMIMEType(pHeaderTypeString, true, (uint32_t)(uintptr_t)(pCurrentPosition - pHeaderTypeString));
            
            const char8_t* pNewCurrentPosition = ++pCurrentPosition;

            while((pNewCurrentPosition < pEndPosition) && *pNewCurrentPosition)
                ++pNewCurrentPosition;

            mimeSubType = MIMETypeStringToMIMEType(pCurrentPosition, false, (uint32_t)(uintptr_t)(pNewCurrentPosition - pCurrentPosition));

            return (mimeType != kMIMETypeUnknown) && (mimeSubType != kMIMETypeUnknown);
        }

        pCurrentPosition++;
    }

    // There is no '/' in the string, and thus we can only come up with a type, but not subtype.
    mimeType = MIMETypeStringToMIMEType(pHeaderTypeString, true, (uint32_t)(uintptr_t)(pCurrentPosition - pHeaderTypeString));

    return (mimeType != kMIMETypeUnknown) && (mimeSubType != kMIMETypeUnknown);
}




///////////////////////////////////////////////////////////////////////////////
// MIMETypeStringToMIMEType
//
MIMEType MIMETypeStringToMIMEType(const char8_t* pMIMETypeString, bool bTypeOrSubType, uint32_t nMaxLength)
{
    const char8_t* pTypeStrEnd = pMIMETypeString + nMaxLength;

    // MIME Types
    if(bTypeOrSubType) // If user is asking for type...
    {
        uint32_t hashVal = FNVHash8Lower ( pMIMETypeString, pTypeStrEnd );

        switch ( hashVal )
        {
            case 0xb12bfa38: // "text"              // These constant hex values were generated by the FNV1_32Bit VBScript macro that  
                return kMIMETypeText;               // Paul Pedriana made for DevStudio. Compilers don't like calculating compile-time
            case 0x9e7c75bf: // "multipart"         // hashes from strings, as they treat strings as addresses. 
                return kMIMETypeMultipart;
            case 0xdbfffa2a: // "message"
                return kMIMETypeMessage;
            case 0x527ffacd: // "application"
                return kMIMETypeApplication;
            case 0x9e0b246c: // "image"
                return kMIMETypeImage;
            case 0x3fc40859: // "audio"
                return kMIMETypeAudio;
            case 0xb1cc1af6: // "video"
                return kMIMETypeVideo;
            case 0x2954e734: // "model"
                return kMIMETypeModel;
        }
        return kMIMETypeUnknown;
    }

    uint32_t hashVal = FNVHash8Lower ( pMIMETypeString, pTypeStrEnd );
    switch( hashVal )
    {
        // Text Subtypes
        case 0x0dd5cf93: // "plain"
            return kMIMESubTypeTextPlain;

        case 0x7cf2ac0e: // "richtext"
            return kMIMESubTypeTextRichtext;

        case 0x80ffa9f7: // "tab-separated-values"
            return kMIMESubTypeTextTabSeparatedValues;

        case 0xdd6233d6: // "html"
        case 0x278ab99e: // "htm"
            return kMIMESubTypeTextHTML;

        case 0x2ce51530: // "sgml"
            return kMIMESubTypeTextSGML;

        case 0x0f9793fe: // "uri-list"
            return kMIMESubTypeTextURIList;

        case 0x0e3280e9: // "rfc822-headers"
            return kMIMESubTypeTextRFC822Headers;

        case 0x1e639c34: // "xml"
            return kMIMESubTypeTextXML;

        case 0x2b72bb3f: // "rtf"
            return kMIMESubTypeTextRTF;

        case 0x2c978db6: // "css"
            return kMIMESubTypeTextCSS;

        case 0xfc19f3e8: // "javascript"
            return kMIMESubtypeTextJavascript;

        // Multipart Subtypes
        case 0xbac63b5a: // "mixed"
            return kMIMESubTypeMultipartMixed;

        case 0x52c9da9e: // "alternative"
            return kMIMESubTypeMultipartAlternative;

        case 0xcb0e0d45: // "digest"
            return kMIMESubTypeMultipartDigest;

        case 0x604d8fc8: // "parallel"
            return kMIMESubTypeMultipartParallel;

        case 0x88d95348: // "appledouble"
            return kMIMESubTypeMultipartAppleDouble;

        case 0x575d15bd: // "header-set"
            return kMIMESubTypeMultipartHeaderSet;

        case 0x1c099b80: // "form-data"
            return kMIMESubTypeMultipartFormData;

        case 0x4a0b828e:  // "related"
            return kMIMESubTypeMultipartRelated;

        case 0x38f65389: // "signed"
            return kMIMESubTypeMultipartSigned;

        case 0x5b865ed7: // "encrypted"
            return kMIMESubTypeMultipartEncrypted;

        case 0x37ff7999: // "byteranges"
            return kMIMESubTypeMultipartByteRanges;


        // Message Subtypes
        case 0x2550ebdc: // "rfc822"
            return kMIMESubTypeMessageRFC822;

        case 0xac75ecec: // "partial"
            return kMIMESubTypeMessagePartial;

        case 0xdb3cc501: // "external-body"
            return kMIMESubTypeMessageExternalBody;

        case 0x35943f2c: // "news"
            return kMIMESubTypeMessageNews;

        case 0xc6620ff5: // "http"
            return kMIMESubTypeMessageHTTP;

        case 0x390d80e9: // "s-http"
            return kMIMESubTypeMessageSHTTP;


        // Application Subtypes
        case 0x7db6b69b: // "mathematica"
            return kMIMESubTypeApplicationMathematica;

        case 0xb4829bebl: // "vnd.ms-excel"
            return kMIMESubTypeApplicationMSExcel;

        case 0xaf4060dc: // "vnd.ms-word"
            return kMIMESubTypeApplicationMSWord;

        case 0x9dd66363: // "octet-stream"
            return kMIMESubTypeApplicationOctetStream;

        case 0x477764fd: // "pdf"
            return kMIMESubTypeApplicationPDF;

        case 0xbd85000a: // "postscript"
            return kMIMESubTypeApplicationPostScript;

        case 0x5831000d: // "x-tar"
            return kMIMESubTypeApplicationXTar;

        case 0x3caa495a: // "x-www-form-urlencoded"
            return kMIMESubTypeApplicationXWWWFormURLEncoded;

        case 0xdca43ae7: // "x-www-local-exec"
            return kMIMESubTypeApplicationXWWWLocalExec;

        case 0x465dcb52: // "zip"
            return kMIMESubTypeApplicationZip;


        // Image Subtypes
        case 0xe88db35f: // "jpeg"
            return kMIMESubTypeImageJPEG;

        case 0x1a8ce475: // "gif"
            return kMIMESubTypeImageGIF;

        case 0x386cbfc3: // "tga"
            return kMIMESubTypeImageTGA;

        case 0xa736778c: // "tiff"
            return kMIMESubTypeImageTIFF;

        case 0x41775b4e: // "png"
            return kMIMESubTypeImagePNG;

        case 0x3872cfb2: // "rgb"
            return kMIMESubTypeImageRGB;

        // Audio Subtypes
        case 0xc718e10d: // "basic"
            return kMIMESubTypeAudioBasic;

        case 0x883807a2: // "x-wav"
            return kMIMESubTypeAudioXWav;


        // Video Subtypes
        case 0x9ec736df: // "x-mpeg"
            return kMIMESubTypeVideoMPEG;

        case 0x14f8d92a: // "x-quicktime"
            return kMIMESubTypeVideoQuicktime;

        case 0xe480a535: // "x-msvideo"
            return kMIMESubTypeVideoXMSVideo;


        // Model Subtypes
        case 0xe919578e: // "vrml"
            return kMIMESubTypeVRML;

        case 0xae6c4306: // "mesh"
            return kMIMESubTypeMESH;
    }

    return kMIMETypeUnknown;
}


///////////////////////////////////////////////////////////////////////////////
// MimeTypeToString
//
const char* MimeTypeToString(MIMEType mimeType)
{
    switch((int)mimeType)
    {
        // Types
        case kMIMETypeText:
            return "text";
        case kMIMETypeMultipart:
            return "multipart";
        case kMIMETypeMessage:
            return "message";
        case kMIMETypeApplication:
            return "application";
        case kMIMETypeImage:
            return "image";
        case kMIMETypeAudio:
            return "audio";
        case kMIMETypeVideo:
            return "video";
        case kMIMETypeModel:
            return "model";

        // Text subtypes
        case kMIMESubTypeTextPlain:
            return "plain";
        case kMIMESubTypeTextRichtext:
            return "richtext";
        case kMIMESubTypeTextTabSeparatedValues:
            return "tab-separated-values";
        case kMIMESubTypeTextHTML:
            return "html";
        case kMIMESubTypeTextXML:
            return "xml";
        case kMIMESubTypeTextRTF:
            return "rtf";
        case kMIMESubTypeTextCSS:
            return "css";
        case kMIMESubtypeTextJavascript:
            return "javascript";


        // Multipart Subtypes
        case kMIMESubTypeMultipartMixed:
            return "mixed";
        case kMIMESubTypeMultipartDigest:
            return "digest";
        case kMIMESubTypeMultipartFormData:
            return "form-data";
        case kMIMESubTypeMultipartRelated:
            return "related";
        case kMIMESubTypeMultipartSigned:
            return "signed";
        case kMIMESubTypeMultipartEncrypted:
            return "encrypted";


        // Message Subtypes
        case kMIMESubTypeMessageHTTP:
            return "http";
        case kMIMESubTypeMessageSHTTP:
            return "s-http";

        // Application Subtypes
        case kMIMESubTypeApplicationOctetStream:
            return "octet-stream";
        case kMIMESubTypeApplicationPDF:
            return "pdf";
        case kMIMESubTypeApplicationZip:
            return "zip";

        // Image Subtypes
        case kMIMESubTypeImageJPEG:
            return "jpeg";
        case kMIMESubTypeImageGIF:
            return "gif";
        case kMIMESubTypeImageTGA:
            return "tga";
        case kMIMESubTypeImageTIFF:
            return "tiff";
        case kMIMESubTypeImagePNG:
            return "png";

        // Audio Subtypes
        case kMIMESubTypeAudioXWav:
            return "x-wav";

        // Video Subtypes
        case kMIMESubTypeVideoMPEG:
            return "x-mpeg";
        case kMIMESubTypeVideoQuicktime:
            return "x-quicktime";
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// MimeTypeToString
//
int MimeTypesToString(MIMEType mimeType, MIMEType mimeSubType, char* pString, size_t stringCapacity)
{
    const char* pMIMEType = MimeTypeToString(mimeType);

    if(pMIMEType)
    {
        const char* pMIMESubType = MimeTypeToString(mimeSubType);

        if(pMIMESubType)
        {
            const size_t strlenSum = strlen(pMIMEType) + 1 + strlen(pMIMESubType);

            if((strlenSum + 1) < stringCapacity)
            {
                strcpy(pString, pMIMEType);
                strcat(pString, "/");
                strcat(pString, pMIMESubType);
            }

            return (int)(unsigned)strlenSum;
        }
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// MIMETypesToFileExtension
//
int MIMETypesToFileExtension(MIMEType mimeType, MIMEType mimeSubType, char16_t* pFileExtension, size_t fileExtensionCapacity)
{
    const char16_t* pExtensionTemp = NULL;

    switch ((int)mimeType)
    {
        case kMIMETypeText:
        {
            switch ((int)mimeSubType)
            {
                case kMIMESubTypeTextPlain:
					pExtensionTemp = EA_CHAR16(".txt");
                    break;
                case kMIMESubTypeTextHTML:
                case kMIMESubTypeTextSGML:
                case kMIMESubTypeTextXML:
                    pExtensionTemp = EA_CHAR16(".html");
                    break;
                case kMIMESubTypeTextRTF:
                    pExtensionTemp = EA_CHAR16(".rtf");
                    break;
                case kMIMESubTypeTextCSS:
                    pExtensionTemp = EA_CHAR16(".css");
                    break;
                case kMIMESubtypeTextJavascript:
                    pExtensionTemp = EA_CHAR16(".js");
                    break;
                default:
                    break; // We simply have no answer for other subtypes.
            }
            break;
        }

        case kMIMETypeApplication:
        {
            switch ((int)mimeSubType)
            {
                case kMIMESubTypeApplicationMacBinhex40:
                    pExtensionTemp = EA_CHAR16(".bin");
                    break;
                case kMIMESubTypeApplicationMSExcel:
                    pExtensionTemp = EA_CHAR16(".xls");
                    break;
                case kMIMESubTypeApplicationMSWord:
                    pExtensionTemp = EA_CHAR16(".doc");
                    break;
                case kMIMESubTypeApplicationPDF:
                    pExtensionTemp = EA_CHAR16(".pdf");
                    break;
                case kMIMESubTypeApplicationXTar:
                    pExtensionTemp = EA_CHAR16(".tar");
                    break;
                case kMIMESubTypeApplicationXWWWLocalExec:
                    pExtensionTemp = EA_CHAR16(".exe");
                    break;
                case kMIMESubTypeApplicationZip:
                    pExtensionTemp = EA_CHAR16(".zip");
                    break;
                default:
                    break; // We simply have no answer for other subtypes.
            }
            break;
        }

        case kMIMETypeImage:
        {
            switch ((int)mimeSubType)
            {
                case kMIMESubTypeImageJPEG:
                    pExtensionTemp = EA_CHAR16(".jpeg");
                    break;
                case kMIMESubTypeImageGIF:
                    pExtensionTemp = EA_CHAR16(".gif");
                    break;
                case kMIMESubTypeImageTGA:
                    pExtensionTemp = EA_CHAR16(".tga");
                    break;
                case kMIMESubTypeImageTIFF:
                    pExtensionTemp = EA_CHAR16(".tiff");
                    break;
                case kMIMESubTypeImagePNG:
                    pExtensionTemp = EA_CHAR16(".png");
                    break;
                case kMIMESubTypeImageXBMP:
                    pExtensionTemp = EA_CHAR16(".bmp");
                    break;
                default:
                    break; // We simply have no answer for other subtypes.
            }

            break;  
        }

        case kMIMETypeAudio:
        {
            switch ((int)mimeSubType)
            {
                case kMIMESubTypeAudioXWav:
                    pExtensionTemp = EA_CHAR16(".wav");
                    break;
                default:
                    break; // We simply have no answer for other subtypes.
            }
            break;
        }

        case kMIMETypeVideo:
        {
            switch ((int)mimeSubType)
            {
                case kMIMESubTypeVideoMPEG:
                    pExtensionTemp = EA_CHAR16(".mpg");
                    break;
                case kMIMESubTypeVideoQuicktime:
                    pExtensionTemp = EA_CHAR16(".qts");
                    break;
                case kMIMESubTypeVideoXMSVideo:
                    pExtensionTemp = EA_CHAR16(".avi");
                    break;
                case kMIMESubTypeVideoXMadcow:
                    pExtensionTemp = EA_CHAR16(".tgq");
                    break;
                default:
                    break; // We simply have no answer for other subtypes.
            }
            break;
        }

        default:
            break; // We simply have no answer for other types.
    }

    if(pExtensionTemp)
    {
        if(fileExtensionCapacity > 0)
        {
            EA::Internal::Strncpy(pFileExtension, pExtensionTemp, fileExtensionCapacity);
            pFileExtension[fileExtensionCapacity - 1] = 0;
        }

        return (int)(unsigned)EA::Internal::Strlen(pExtensionTemp);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// FileExtensionToMIMETypes
//
bool FileExtensionToMIMETypes(const char16_t* pFileExtension, MIMEType& mimeType, MIMEType& mimeSubType)
{
    mimeType    = kMIMETypeUnknown;
    mimeSubType = kMIMESubTypeUnknown;

    if(*pFileExtension == '.')
        pFileExtension++;

    char16_t extensionLowerCase[32];
    EA::Internal::Strncpy(extensionLowerCase, pFileExtension, 32);
    extensionLowerCase[31] = 0;

    switch(FNVHash16(extensionLowerCase))
    {
        case 0xdd6233d6: // "html"
            mimeType    = kMIMETypeText;
            mimeSubType = kMIMESubTypeTextHTML;
            break;

        case 0x2b6cab5f: // "txt"
            mimeType    = kMIMETypeText;
            mimeSubType = kMIMESubTypeTextPlain;
            break;

        case 0x2c978db6: // "css"
            mimeType    = kMIMETypeText;
            mimeSubType = kMIMESubTypeTextCSS;
            break;

        case 0x67771f5c: // "js"
            mimeType    = kMIMETypeText;
            mimeSubType = kMIMESubtypeTextJavascript;
            break;


        case 0x3f8662ea: // "jpg"
        case 0xe88db35f: // "jpeg"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImageJPEG;
            break;

        case 0x1a8ce475: // "gif"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImageGIF;
            break;

        case 0x386cbfc3: // "tga"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImageTGA;
            break;

        case 0xa736778c: // "tiff"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImageTIFF;
            break;

        case 0x41775b4e: // "png"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImagePNG;
            break;

        case 0x2299bcee: // "bmp"
            mimeType    = kMIMETypeImage;
            mimeSubType = kMIMESubTypeImageXBMP;
            break;

        default:
            break;
    }

    return (mimeType != kMIMETypeUnknown);
}




} // namespace Internet

} // namespace EA














