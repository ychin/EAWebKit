/*
Copyright (C) 2010, 2011 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitUtilStreamDecompressor.h
//
//
// By Arpit Baldeva 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file (and cpp implementation) define an abstraction for the stream decompression. It also implements a decompressor that is capable of 
// decompressing a stream compressed using "DEFLATE" algorithm. 

// All 3 common decompression specifications are supported.
// zlib			-	http://tools.ietf.org/html/rfc1950 (Content-Encoding: deflate)
// Raw Deflate	-	http://tools.ietf.org/html/rfc1951 (Content-Encoding: deflate)
// gzip			-	http://tools.ietf.org/html/rfc1952 (Content-Encoding: gzip)
///////////////////////////////////////////////////////////////////////////////

#ifndef STREAMDECOMPRESSOREA_H
#define STREAMDECOMPRESSOREA_H

#ifndef ENABLE_PAYLOAD_DECOMPRESSION
	#define ENABLE_PAYLOAD_DECOMPRESSION 1
#endif


#if (ENABLE_PAYLOAD_DECOMPRESSION)

namespace EA
{
namespace WebKit
{
//This is the callback for the application when data is decompressed. 
typedef void (*DecompressedDataCallback)(void* userData, uint8_t* decompressedData, uint32_t decompressedDataLength);

class IStreamDecompressor
{
public:
	IStreamDecompressor(){}
	virtual ~IStreamDecompressor();

	//Return value indicates the status. Need to have proper error codes.
	//For now, passing the zlib error code would be sufficient as it is the only library that we use.
	virtual int32_t Init();
	
	//A positive return value indicates the amount of bytes decompressed in the buffer.
	//A negative return value indicates an error.
	//If no decompression buffer is passed, the implementation should allocate one as required.
	//Note that the user don't need to worry about the capacity of the decompression buffer but ideally it should be at least as large as the source buffer.
	//As a result, callback must be provided to consume data when available. This is simply because the Decompress routine may do multiple passes if the
	//decompression buffer is not large enough. Do not rely on the decompression buffer at the end of the routine.
	//Our implementation below is tested with the decompression buffer as small as the source buffer.
	virtual int32_t Decompress(uint8_t* sourceStream, uint32_t sourceLength, uint8_t* decompressionBuffer = 0, uint32_t decompressionBufferCapacity = 0) = 0;
	//Set a callback to receive the data when decompressed.
	virtual void	SetDecompressedDataCallback(DecompressedDataCallback callback, void* userData){};
private:
	IStreamDecompressor(const IStreamDecompressor& streamDecompressor);
	IStreamDecompressor& operator=(const IStreamDecompressor& streamDecompressor);
};
}
}

typedef struct z_stream_s z_stream;

namespace EA
{
namespace WebKit
{
enum eStreamType
{
	eStreamTypeNone,
	eStreamTypeZLib,
	eStreamTypeRaw,
	eStreamTypeGZip
};

//This stream decompressor is able to decompress a stream compressed using "DEFLATE" algorithm. It can be any of the three common streams
//used in Http 1.1. 
class DeflateStreamDecompressor : public IStreamDecompressor
{
public:
	DeflateStreamDecompressor(eStreamType streamType);
	virtual			~DeflateStreamDecompressor();
	virtual int32_t	Init();
	
	//If no decompression buffer is passed, the implementation would allocate one. This would be kept alive as long as it is suitable.
	virtual int32_t Decompress(uint8_t* sourceStream, uint32_t sourceLength, uint8_t* decompressionBuffer = 0, uint32_t decompressionBufferCapacity = 0);

	virtual void	SetDecompressedDataCallback(DecompressedDataCallback callback, void* userData);

private:
	void UnInit();
	void AllocateDecompressionBuffer( uint32_t sourceLength );
	int32_t ProcessGZipHeader(uint8_t*& sourceStream, uint32_t& sourceLength);
	
	z_stream*										mZStream;
	DecompressedDataCallback						mDecompressedDataCallback;
	uint8_t*										mDecompressionBuffer;
	size_t											mDecompressionBufferCapacity;
	eStreamType										mStreamType;
	void*											mUserData;
	bool											mInitialized;
	bool											mRawDeflateStream;
	bool											mCheckForErrorCondition;
};

}
}
#endif //ENABLE_PAYLOAD_DECOMPRESSION
#endif //Header include guard