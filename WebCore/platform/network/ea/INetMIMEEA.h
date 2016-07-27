/*
Copyright (C) 2005,2009, 2010, 2011 Electronic Arts, Inc.  All rights reserved.

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
// INetMIMEEA.h
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////



#ifndef INetMIMEEA_H
#define INetMIMEEA_H


#include <EABase/eabase.h>

namespace EA
{
    namespace WebKit
    {


        ///////////////////////////////////////////////////////////////////////////////
        /// MIMEType
        ///
        /// MIME types and subtypes as defined by RFC1521 and RFC1522, and later by 
        /// RFC2045 and RFC2046. We do not list all subtypes here, because many of them 
        /// will never be of any use to us. For a complete listing of the official 
        /// types, see 'ftp.isi.edu/in-notes/iana/assignments/media-types/media-types'
        /// What's cool about this FTP site is that it gives a simple example of a file
        /// containing an example of each subtype. Types listed below that aren't in 
        /// the official list are 'non-standard' types, and they begin with 'X'.
        ///
        enum MIMEType
        {
            kMIMETypeUnknown = 0,
            kMIMESubTypeUnknown = 0,

            // Standard MIME types
            kMIMETypeText,
            kMIMETypeMultipart,
            kMIMETypeMessage,
            kMIMETypeApplication,
            kMIMETypeImage,
            kMIMETypeAudio,
            kMIMETypeVideo,
            kMIMETypeModel,

            // Text subtypes
            kMIMESubTypeTextPlain,                      // Plain old text.
            kMIMESubTypeTextRichtext,                   // Not same as RTF.
            kMIMESubTypeTextTabSeparatedValues,
            kMIMESubTypeTextHTML,                       // Hyper Text Markup Language
            kMIMESubTypeTextSGML,                       // Standard Generalized Markup Language
            kMIMESubTypeTextURIList,
            kMIMESubTypeTextRFC822Headers,              // Mail headers
            kMIMESubTypeTextXML,                        // Extensible Markup Language
            kMIMESubTypeTextRTF,                        // Microsoft's Rich Text Format
            kMIMESubTypeTextCSS,                        // Cascading style sheets
            kMIMESubtypeTextJavascript,                 // Javascript .js files.

            // Multipart subtypes
            kMIMESubTypeMultipartMixed,
            kMIMESubTypeMultipartAlternative,
            kMIMESubTypeMultipartDigest,
            kMIMESubTypeMultipartParallel,
            kMIMESubTypeMultipartAppleDouble,
            kMIMESubTypeMultipartHeaderSet,
            kMIMESubTypeMultipartFormData,
            kMIMESubTypeMultipartRelated,
            kMIMESubTypeMultipartSigned,
            kMIMESubTypeMultipartEncrypted,
            kMIMESubTypeMultipartByteRanges,

            // Message subtypes
            kMIMESubTypeMessageRFC822,                  // Mail body
            kMIMESubTypeMessagePartial,
            kMIMESubTypeMessageExternalBody,
            kMIMESubTypeMessageNews,
            kMIMESubTypeMessageHTTP,                    // HyperText Tranport Protocol
            kMIMESubTypeMessageSHTTP,                   // Secure HTTP

            // Application subtypes
            kMIMESubTypeApplicationCommonGround,
            kMIMESubTypeApplicationMacBinhex40,
            kMIMESubTypeApplicationMathematica,
            kMIMESubTypeApplicationMSExcel,
            kMIMESubTypeApplicationMSWord,
            kMIMESubTypeApplicationOctetStream,
            kMIMESubTypeApplicationPDF,
            kMIMESubTypeApplicationPostScript,
            kMIMESubTypeApplicationRTF,
            kMIMESubTypeApplicationSGML,
            kMIMESubTypeApplicationXML,
            kMIMESubTypeApplicationXTar,                // Unix tar.
            kMIMESubTypeApplicationXWWWFormURLEncoded,
            kMIMESubTypeApplicationXWWWLocalExec,
            kMIMESubTypeApplicationZip,                 // PKZip file format

            // Image subtypes
            kMIMESubTypeImageJPEG,                      // Joint Photo Experts Group
            kMIMESubTypeImageGIF,                       // Graphics Interchange Format
            kMIMESubTypeImageTGA,                       // Targa File Format
            kMIMESubTypeImageTIFF,                      // Tagged Image File Format
            kMIMESubTypeImagePNG,                       // Portable Network Graphics
            kMIMESubTypeImageRGB,                       // RGB raw
            kMIMESubTypeImageXPBM,                      // Portable Bit Map
            kMIMESubTypeImageXPPM,                      // Portable Pixel Map
            kMIMESubTypeImageXPICT,                     // Macintosh PICT file
            kMIMESubTypeImageXBMP,                      // Microsfot .bmp file

            //Audio subtypes
            kMIMESubTypeAudioBasic,                     // "8-bit 8 KHz ISDN mu-law encoded audio"
            kMIMESubTypeAudioXAIFF,                     // Audio Interchange File Format
            kMIMESubTypeAudioXWav,                      // Microsoft .wav files
            kMIMESubTypeAudioXMacAudio,                 // Macintosh audio files

            //Video subtypes
            kMIMESubTypeVideoMPEG,                       // Motion Picture Experts Group
            kMIMESubTypeVideoQuicktime,                  // Apple Quicktime
            kMIMESubTypeVideoXMSVideo,                   // AVI movies
            kMIMESubTypeVideoXSGIMovie,
            kMIMESubTypeVideoXSmacker,                   // From RAD gametools
            kMIMESubTypeVideoXBink,                      // From RAD gametools
            kMIMESubTypeVideoXMadcow,                    // Internal EA movie format

            // Model subtypes
            kMIMESubTypeVRML,                            // Virtual Reality Markup Language
            kMIMESubTypeMESH

        }; // enum MIMEType



        /// MIMEStringToMIMETypes
        ///
        /// Given a full MIME content type string (e.g. "text/html"), this function returns type and subtype.
        ///
        bool MIMEStringToMIMETypes(const char8_t* pMIMETypeString, MIMEType& mimeType, MIMEType& mimeSubtype, uint32_t nMaxLength = (uint32_t)-1);


        /// MIMETypeStringToMIMEType
        ///
        /// Given a MIME type (bTypeOrSubType = true) or subtype string (e.g. "html"), returns the enumeration for it.
        ///
        MIMEType MIMETypeStringToMIMEType(const char8_t* pMIMETypeString, bool bTypeOrSubType, uint32_t nMaxLength = (uint32_t)-1);


        /// MimeTypeToString
        ///
        /// Returns MIME type string or NULL if not available.
        ///
        const char* MimeTypeToString(MIMEType mimeType);


        /// MimeTypesToString
        ///
        /// Yields a string such as "text/html". 
        /// Returns desired Strlen of result. Returns 0 if the types are unknown.
        ///
        int MimeTypesToString(MIMEType mimeType, MIMEType mimeSubType, char* pString, size_t stringCapacity);


        /// MIMETypesToFileExtension
        ///
        /// If MIME type string is "text/html", result is ".html". 
        /// Returns desired Strlen of result. Returns 0 if the types are unknown.
        ///
        int MIMETypesToFileExtension(MIMEType mimeType, MIMEType mimeSubtype, char16_t* pFileExtension, size_t fileExtensionCapacity);


        /// FileExtensionToMIMETypes
        ///
        /// If the file extension is ".html", the MIME type and subtype are text and html respectively. 
        ///
        bool FileExtensionToMIMETypes(const char16_t* pFileExtension, MIMEType& mimeType, MIMEType& mimeSubtype);


    } // namespace WebKit

} // namespace EA


#endif // INetMIMEEA_H








