// (c) 2008 Criterion Software Limited and its licensors. All Rights Reserved

/**
\file
Define the major, minor and patch versions.
*/

#ifndef COREALLOCATOR_VERSION_H
#define COREALLOCATOR_VERSION_H

#if defined _MSC_VER
#pragma once
#endif

// This information is updated with each release.

//! This define indicates the major version number for the coreallocator package.
#define COREALLOCATOR_VERSION_MAJOR   1

//! This define indicates the minor version number for the coreallocator package.
#define COREALLOCATOR_VERSION_MINOR   4

//! This define indicates the patch version number for the coreallocator package.
#define COREALLOCATOR_VERSION_PATCH   4

/*!
 * This is a utility macro that users may use to create a single version number
 * that can be compared against COREALLOCATOR_VERSION.
 *
 * For example:
 *
 * \code
 *
 * #if COREALLOCATOR_VERSION > COREALLOCATOR_CREATE_VERSION_NUMBER( 1, 1, 0 )
 * printf("Coreallocator version is greater than 1.1.0.\n");
 * #endif
 *
 * \endcode
 */
#define COREALLOCATOR_CREATE_VERSION_NUMBER( major_ver, minor_ver, patch_ver ) \
	((major_ver) * 1000000 + (minor_ver) * 1000 + (patch_ver))

/*!
 * This macro is an aggregate of the major, minor and patch version numbers.
 * \sa COREALLOCATOR_CREATE_VERSION_NUMBER
 */
#define COREALLOCATOR_VERSION \
	COREALLOCATOR_CREATE_VERSION_NUMBER( COREALLOCATOR_VERSION_MAJOR, COREALLOCATOR_VERSION_MINOR, COREALLOCATOR_VERSION_PATCH )

#endif // COREALLOCATOR_VERSION_H
