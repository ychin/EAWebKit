/*
Copyright (C) 2012 Electronic Arts, Inc.  All rights reserved.

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

#ifndef ZLIB_VERSION_H
#define ZLIB_VERSION_H

#if defined _MSC_VER
#pragma once
#endif

// Define the major, minor and patch versions.
// This information is updated with each release.

//! This define indicates the major version number for the bigfile package.
//! \sa ZLIB_VERSION
#define ZLIB_VERSION_MAJOR   1
//! This define indicates the minor version number for the bigfile package.
//! \sa ZLIB_VERSION
#define ZLIB_VERSION_MINOR   2
//! This define indicates the patch version number for the bigfile package.
//! \sa ZLIB_VERSION
#define ZLIB_VERSION_PATCH   7

/*!
 * This is a utility macro that users may use to create a single version number
 * that can be compared against ZLIB_VERSION.
 *
 * For example:
 *
 * \code
 *
 * #if ZLIB_VERSION > ZLIB_CREATE_VERSION_NUMBER( 1, 1, 0 )
 * printf("Bigfile version is greater than 1.1.0.\n");
 * #endif
 *
 * \endcode
 */
#define ZLIB_CREATE_VERSION_NUMBER( major_ver, minor_ver, patch_ver ) \
    ((major_ver) * 1000000 + (minor_ver) * 1000 + (patch_ver))

/*!
 * This macro is an aggregate of the major, minor and patch version numbers.
 * \sa ZLIB_CREATE_VERSION_NUMBER
 */
#define ZLIB_VERSION \
    ZLIB_CREATE_VERSION_NUMBER( ZLIB_VERSION_MAJOR, ZLIB_VERSION_MINOR, ZLIB_VERSION_PATCH )

#endif // ZLIB_VERSION_H

