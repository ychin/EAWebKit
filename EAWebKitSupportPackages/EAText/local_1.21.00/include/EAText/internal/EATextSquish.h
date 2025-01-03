///////////////////////////////////////////////////////////////////////////////
// EATextSquish.h
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Sumedha Kshirsagar, Paul Pedriana, EARS
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSQUISH_H
#define EATEXT_INTERNAL_EATEXTSQUISH_H


#include <EABase/eabase.h>


namespace EATextSquish
{
	void Compress(const uint8_t* pSourceARGB, void* pDestination, uint32_t nSourceStride, uint32_t nImageSize);
}

#endif // Header include guard





