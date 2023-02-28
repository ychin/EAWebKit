/////////////////////////////////////////////////////////////////////////////
// EAStreamAdapter.h
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements adapters for reading and writing basic types in an 
// endian-proper way with the EA::Stream interfaces.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMADAPTER_H)
#define EAIO_EASTREAMADAPTER_H


#ifndef INCLUDED_eabase_H
	#include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#if EAIO_USE_CORESTREAM
	#include <CoreStream/IStream.h>
	#include <CoreStream/IStreamAdapter.h>
#else
	#include <EAIO/EAStream.h>
#endif
#include <string.h>
#include <limits.h>

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4275)  // non dll-interface class used as base for dll-interface class
#endif


/// namespace EA
/// Standard Electronic Arts Framework namespace.
namespace EA
{
	/// namespace IO
	/// The namespace for general IO (input/output) functionality.
	namespace IO
	{
		EAIO_API void EAStreamAdapterAssertionFailure();

		/// CopyStream
		///
		/// This is a generic function for copying a source stream to a destination stream.
		///
		/// If input nSize is less than the input stream size, then nSize bytes are copied.
		/// If input nSize is greater than the input stream size, then only the input stream
		/// size is copied. If input nSize is kLengthNull, then the entire input stream size 
		/// should be copied, whatever it happens to be.
		///
		/// Returns kSizeTypeError if there was an error reading input or writing output. 
		/// Upon error, the input and output stream states are left as they were upon reaching 
		/// the error. The reading from source begins at the current source position, which 
		/// may be different from the beginning of the stream. The writing of the destination 
		/// begins at the current destination position, which may be different from the beginning 
		/// of the stream. In some cases, a given source and/or destination stream might be 
		/// specialized to outperform this function. 
		///
		EAIO_API size_type CopyStream(IStream* pSource, IStream* pDestination, size_type nSize = kLengthNull);



		// Can't use the following as it was intended unless all packages which use 
		// this header recognize EAIO_USE_CORESTREAM and can define it. Until then we
		// simply force EAIO_CORESTREAM_VIRTUAL => virtual. 
		//#if EAIO_USE_CORESTREAM
			#define EAIO_CORESTREAM_VIRTUAL virtual
		//#else
		//    #define EAIO_CORESTREAM_VIRTUAL
		//#endif


		/// StreamAdapter
		///
		/// This class usually makes endian reading/writing easier than using the standalone functions.
		///
		class EAIO_API StreamAdapter
					   #if EAIO_USE_CORESTREAM
					   : public EA::IO::IStreamAdapter
					   #endif
		{
		public:
			StreamAdapter();
			StreamAdapter(EA::IO::IStream* pStream, EA::IO::Endian endianType);
			EAIO_CORESTREAM_VIRTUAL ~StreamAdapter(){}

			EAIO_CORESTREAM_VIRTUAL void SetValid(bool success);
			EAIO_CORESTREAM_VIRTUAL bool IsValid() const;
			EAIO_CORESTREAM_VIRTUAL operator bool() const;
			EAIO_CORESTREAM_VIRTUAL bool operator !() const;

			EAIO_CORESTREAM_VIRTUAL EA::IO::IStream* GetStream() const;
			EAIO_CORESTREAM_VIRTUAL void SetStream(EA::IO::IStream* pStream);

			EAIO_CORESTREAM_VIRTUAL EA::IO::Endian GetEndianType() const;
			EAIO_CORESTREAM_VIRTUAL void SetEndianType(EA::IO::Endian endianType);

			EAIO_CORESTREAM_VIRTUAL void VerifyIO(bool success);

			/// Check if an array size read from a stream is sane, based on whether
			/// enough data remains in the stream. If it's not, then we set the
			/// error code and return false. Generally the reason you'd want to
			/// do this instead of relying on I/O errors alone is so that you don't
			/// try to allocate a 15GB array before entering the read loop.
			EAIO_CORESTREAM_VIRTUAL bool VerifyArraySize(uint32_t elementCount, uint32_t elementSize);

			// basic types
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint8(uint8_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint8(uint8_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint16(uint16_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint16(uint16_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint32(uint32_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint32(uint32_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint64(uint64_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint64(uint64_t v);

			inline EAIO_CORESTREAM_VIRTUAL void ReadInt8(int8_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt8(int8_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt16(int16_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt16(int16_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt32(int32_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt32(int32_t v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt64(int64_t& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt64(int64_t v);

			inline EAIO_CORESTREAM_VIRTUAL void ReadBool8(bool& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteBool8(bool v);

			inline EAIO_CORESTREAM_VIRTUAL void ReadFloat(float& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteFloat(float v);
			inline EAIO_CORESTREAM_VIRTUAL void ReadDouble(double& v);
			inline EAIO_CORESTREAM_VIRTUAL void WriteDouble(double v);

			// basic type array (unstrided)
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint8(uint8_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint8(const uint8_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint16(uint16_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint16(const uint16_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint32(uint32_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint32(const uint32_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadUint64(uint64_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteUint64(const uint64_t* v, uint32_t count);

			inline EAIO_CORESTREAM_VIRTUAL void ReadBool8(bool* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteBool8(const bool* v, uint32_t count);

			inline EAIO_CORESTREAM_VIRTUAL void ReadInt8(int8_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt8(const int8_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt16(int16_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt16(const int16_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt32(int32_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt32(const int32_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadInt64(int64_t* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteInt64(const int64_t* v, uint32_t count);

			inline EAIO_CORESTREAM_VIRTUAL void ReadFloat(float* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteFloat(const float* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void ReadDouble(double* v, uint32_t count);
			inline EAIO_CORESTREAM_VIRTUAL void WriteDouble(const double* v, uint32_t count);

			// Strings

			// The capacity is the space of the input string buffer, and includes space for terminating 0 that is written.
			// Returns the required strlen of the string to read.
			// These functions assume the string is in a <uint32_t size><char[]> format.
			EAIO_CORESTREAM_VIRTUAL size_t ReadString8(char* string, uint32_t capacity);

			template <typename String8>
			void ReadString8(String8& string);

			EAIO_CORESTREAM_VIRTUAL void WriteString8(const char* string, uint32_t length);

			template <typename String8>
			void WriteString8(const String8& string);

			// The capacity is the space of the input string buffer, and includes space for terminating 0 that is written.
			// Returns the required strlen of the string to read.
			EAIO_CORESTREAM_VIRTUAL size_t ReadString16(char16_t* string, uint32_t capacity);
			EAIO_CORESTREAM_VIRTUAL size_t ReadString32(char32_t* string, uint32_t capacity);

			template <typename String16>
			void ReadString16(String16& string);

			template <typename String32>
			void ReadString32(String32& string);

			EAIO_CORESTREAM_VIRTUAL void WriteString16(const char16_t* string, uint32_t length);
			EAIO_CORESTREAM_VIRTUAL void WriteString32(const char32_t* string, uint32_t length);

			template <typename String16>
			void WriteString16(const String16& string);

			template <typename String32>
			void WriteString32(const String32& string);

		protected:
			EA::IO::IStream*  mpStream;
			EA::IO::Endian    mEndianType; 
			bool              mSuccess;
		};



		///////////////////////////////////////////////////////////////////
		// Adapter functions for reading data from streams
		//
		// Example usage:
		//    IStream* pIStream = GetStreamFromSomewhere();
		//
		//    uint16_t nUint16;
		//    ReadUint16(pIStream, nUint16);
		//
		//    uint32_t nUint32(0);
		//    WriteUint32(pIStream, nUint32);
		//
		//    const char16_t line[] = EA_CHAR16("HelloWorld");
		//    ReadLine(pIStream, line, sizeof(line), kEndianBig);
		//
		//    WriteLine(pIStream, "Hello World", kLengthNull, kLineTerminationNewline);
		///////////////////////////////////////////////////////////////////

		/// ReadBool8
		///
		/// Reads a single boolean value from the stream. Note that the to be 
		/// portable, the stream implments booleans as int8_t and not bool.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadBool8(IStream* pIS, bool& value);
		EAIO_API bool ReadBool8(IStream* pIS, bool* value, size_type count);

		/// ReadInt8
		///
		/// Reads an int8_t from the stream.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt8(IStream* pIS, int8_t& value);

		/// ReadInt8
		///
		/// Reads an array of int8_t from the stream.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt8(IStream* pIS, int8_t* value, size_type count);

		/// ReadUint8
		///
		/// Reads an uint8_t from the stream.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint8(IStream* pIS, uint8_t& value);

		/// ReadUint8
		///
		/// Reads an array of uint8_t from the stream.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint8(IStream* pIS, uint8_t* value, size_type count);

		/// ReadUint16
		///
		/// Reads a uint16_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint16(IStream* pIS, uint16_t& value, Endian endianSource = kEndianBig);

		/// ReadUint16
		///
		/// Reads an array of uint16_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint16(IStream* pIS, uint16_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadInt16
		///
		/// Reads an int16_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt16(IStream* pIS, int16_t& value, Endian endianSource = kEndianBig);

		/// ReadInt16
		///
		/// Reads an array of int16_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt16(IStream* pIS, int16_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadUint32
		///
		/// Reads a uint32_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint32(IStream* pIS, uint32_t& value, Endian endianSource = kEndianBig);

		/// ReadUint32
		///
		/// Reads an array of uint32_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint32(IStream* pIS, uint32_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadInt32
		///
		/// Reads an int32_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt32(IStream* pIS, int32_t& value, Endian endianSource = kEndianBig);

		/// ReadInt32
		///
		/// Reads an array of int32_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt32(IStream* pIS, int32_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadUint64
		///
		/// Reads a uint64_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint64(IStream* pIS, uint64_t& value, Endian endianSource = kEndianBig);

		/// ReadUint64
		///
		/// Reads an array of uint64_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadUint64(IStream* pIS, uint64_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadInt64
		///
		/// Reads an int64_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt64(IStream* pIS, int64_t& value, Endian endianSource = kEndianBig);

		/// ReadInt64
		///
		/// Reads an array of int64_t from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadInt64(IStream* pIS, int64_t* value, size_type count, Endian endianSource = kEndianBig);

		// ReadUint128
		// Reads a uint128_t from the stream, converting to local endian-ness as directed by input endianSource.
		// Input 'endianSource' refers to the endian-ness of the values in the stream.
		// Upon return the output value will be in local (a.k.a. native) endian-ness.
		// The return value is true if the value could be entirely read. 
		// If false, you can use IStream::GetState to determine the error.
		// EAIO_API bool ReadUint128(IStream* pIS, uint128_t& value, Endian endianSource = kEndianBig);

		// ReadUint128
		// Reads an array of uint128_t from the stream.
		// Input 'endianSource' refers to the endian-ness of the values in the stream.
		// Upon return the output value will be in local (a.k.a. native) endian-ness.
		// The return value is true if the value could be entirely read. 
		// If false, you can use IStream::GetState to determine the error.
		// EAIO_API bool ReadUint128(IStream* pIS, uint128_t* value, size_type count, Endian endianSource = kEndianBig);

		// ReadInt128
		// Reads an int128_t from the stream.
		// Input 'endianSource' refers to the endian-ness of the values in the stream.
		// Upon return the output value will be in local (a.k.a. native) endian-ness.
		// The return value is true if the value could be entirely read. 
		// If false, you can use IStream::GetState to determine the error.
		// EAIO_API bool ReadInt128(IStream* pIS, int128_t& value, Endian endianSource = kEndianBig);

		// ReadInt128
		// Reads an array of int128_t from the stream.
		// Input 'endianSource' refers to the endian-ness of the values in the stream.
		// Upon return the output value will be in local (a.k.a. native) endian-ness.
		// The return value is true if the value could be entirely read. 
		// If false, you can use IStream::GetState to determine the error.
		// EAIO_API bool ReadInt128(IStream* pIS, int128_t* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadFloat
		///
		/// Reads a float from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadFloat(IStream* pIS, float& value, Endian endianSource = kEndianBig);

		/// ReadFloat
		///
		/// Reads an array of float from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadFloat(IStream* pIS, float* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadDouble
		///
		/// Reads a double from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadDouble(IStream* pIS, double& value, Endian endianSource = kEndianBig);

		/// ReadDouble
		///
		/// Reads an array of double from the stream.
		/// Input 'endianSource' refers to the endian-ness of the values in the stream.
		/// Upon return the output value will be in local (a.k.a. native) endian-ness.
		/// The return value is true if the value could be entirely read. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool ReadDouble(IStream* pIS, double* value, size_type count, Endian endianSource = kEndianBig);

		/// ReadString
		///
		/// Reads a string from an IStream.
		///
		/// The resulting buffer will be nul-terminated, even if there wasn't enough 
		/// space to write the full string (with the exception of nStringCapacity == 0).
		///
		/// nStringCapacity is the capacity of the pString buffer; it is the max number 
		/// of characters to write, including the terminating nul. The input pLine can 
		/// be NULL, in which case it will not be written to and the return value will 
		/// merely indicate its required size.
		///
		/// The return value is the strlen (string length) of the expected line or 
		/// kSizeTypeError if there was an error. 
		/// 
		/// Upon return, the stream will be positioned after the end of the string, 
		/// even if nStringCapacity was not enough to hold the entire string.
		///
		EAIO_API size_type ReadString(IStream* pIS, char* pString, size_type nStringCapacity, Endian endianSource = kEndianBig);

		/// ReadString
		///
		/// This is a char16_t version of ReadString.
		/// It behaves the same as with the char version with the exception that
		/// the destination output is written as UTF16-encoded char16_t.
		///
		EAIO_API size_type ReadString(IStream* pIS, char16_t* pString, size_type nStringCapacity, Endian endianSource = kEndianBig);
		EAIO_API size_type ReadString(IStream* pIS, char32_t* pString, size_type nStringCapacity, Endian endianSource = kEndianBig);


		/// ReadLine
		///
		/// Reads a line of text from the source IStream.
		///
		/// A line is defined as a sequence ending with "\n" or "\r\n".
		/// A line may be empty, as would be the case with a "\n\n" sequence.
		/// The returned line does not end with line terminator characters.
		/// The returned line -does- end with a terminating zero.
		///
		/// The return value is the strlen (string length) of the expected line or 
		/// kSizeTypeError upon error. A return value of kSizeTypeDone means there 
		/// were no more lines to read. This is different from the return value of 
		/// the IStream::Read function because of the possibility of empty lines.
		/// Note that the return value is the *expected* strlen, which may be >=
		/// the nLineCapacity. In any case, the returned line will always be 
		/// nul-terminated if it has any capacity.
		///
		/// Upon return, the stream will be positioned at the beginning of the 
		/// next line, even if input nLineCapacity was not enough to hold the entire line.
		/// The input nLineCapacity is max number of characters to write, including 
		/// the terminating zero. The input pLine can be NULL, in which case it 
		/// will not be written to and the return value will merely indicate its 
		/// required size.
		///
		/// Example usage:
		///     char      buffer[256];
		///     size_type size;
		///     while((size = EA::IO::ReadLine(&fileStream, buffer, 256)) < kSizeTypeDone) // While there were more lines...
		///         ; // do something
		///
		EAIO_API size_type ReadLine(IStream* pIS, char*  pLine, size_type nLineCapacity, Endian /*unused*/   = kEndianBig);
		EAIO_API size_type ReadLine(IStream* pIS, char16_t* pLine, size_type nLineCapacity, Endian endianSource = kEndianBig);
		EAIO_API size_type ReadLine(IStream* pIS, char32_t* pLine, size_type nLineCapacity, Endian endianSource = kEndianBig);


		/// ReadLine
		///
		/// Reads line of text from a file. The returned line will not have a trailing \r or \n character.
		/// In case of returning kSizeTypeError or kSizeTypeDone, the returned string will be empty.
		/// Requires a stream which supports rewinding to at least the starting position at the time of this call.
		/// Upon error (return of kSizeTypeError), the string will be cleared, but not necessarily freed of capacity.
		/// 
		/// Example usage:
		///     eastl::string8 s8;
		///     
		///     while(EA::IO::ReadLine(&fileStream, s8) < kSizeTypeDone) // While there were more lines and there was no error...
		///         ; // do something
		///
		template <typename String>
		size_type ReadLine(IStream* pIS, String& s, Endian endianSource = kEndianBig)
		{
			if(s.size() < 256)  // Pre-size the string so that we usually don't have to call ReadLine twice.
				s.resize(256);  // In the long run this is usually cheaper than rewinding the stream below.

			off_type  initialStreamPos = pIS->GetPosition();
			size_type requiredStrlen = ReadLine(pIS, &s[0], (size_type)s.capacity(), endianSource); // Use capacity() instead of size() because ReadLine's capacity argument includes the trailing 0 char.

			if(requiredStrlen < kSizeTypeDone) // If data was successfully read...
			{
				if(requiredStrlen > s.capacity()) // If there wasn't enough capacity in the string... increase and re-read.
				{
					s.resize((typename String::size_type)requiredStrlen);
					pIS->SetPosition(initialStreamPos);
					requiredStrlen = ReadLine(pIS, &s[0], (size_type)s.capacity(), endianSource);
				}
			}

			if(requiredStrlen < kSizeTypeDone) // If data was successfully read (or re-read)...
				s.resize((typename String::size_type)requiredStrlen);
			else
				s.clear(); // Clear the string upon kSizeTypeError or kSizeTypeDone.

			return requiredStrlen; // This may be kSizeTypeError or kSizeTypeDone.
		}



		/// ReadString
		///
		/// Reads from an STL or EASTL string or equivalent.
		/// These functions assume the string is in a <uint32_t size><char[]> format.
		/// This is not the same as ReadLine, as this reads an embedded string in the size/char format
		/// as described above.
		///
		template <typename String8>
		bool ReadString8(EA::IO::IStream* pIS, String8& s, Endian endianDestination = kEndianBig);

		template <typename String16>
		bool ReadString16(EA::IO::IStream* pIS, String16& s, Endian endianDestination = kEndianBig);

		template <typename String32>
		bool ReadString32(EA::IO::IStream* pIS, String32& s, Endian endianDestination = kEndianBig);




		/// WriteUint8
		///
		/// Writes a boolean value to the output stream.
		/// Since type 'bool' is not portable, this function converts the input to 
		/// an int8_t and write that as 0 or 1.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteBool8(IStream* pOS, bool value);
		EAIO_API bool WriteBool8(IStream* pOS, const bool* value, size_type count);

		/// WriteInt8
		///
		/// Writes an int8_t numerical value to the output stream.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt8(IStream* pOS, int8_t value);

		/// WriteInt8
		///
		/// Writes an array of int8_t numerical values to the output stream.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt8(IStream* pOS, const int8_t* value, size_type count);

		/// WriteUint8
		///
		/// Writes a uint8_t numerical value to the output stream.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint8(IStream* pOS, uint8_t value);

		/// WriteUint8
		///
		/// Writes an array of uint8_t numerical values to the output stream.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint8(IStream* pOS, const uint8_t* value, size_type count);

		/// WriteUint16
		///
		/// Writes a uint16_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint16(IStream* pOS, uint16_t value, Endian endianDestination = kEndianBig);

		/// WriteUint16
		///
		/// Writes an array of uint16_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint16(IStream* pOS, const uint16_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteInt16
		///
		/// Writes an int16_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt16(IStream* pOS, int16_t value, Endian endianDestination = kEndianBig);

		/// WriteInt16
		///
		/// Writes an array of int16_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt16(IStream* pOS, const int16_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteUint32
		///
		/// Writes a uint32_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint32(IStream* pOS, uint32_t value, Endian endianDestination = kEndianBig);

		/// WriteUint32
		///
		/// Writes an array of uint32_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint32(IStream* pOS, const uint32_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteInt32
		///
		/// Writes an int32_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt32(IStream* pOS, int32_t value, Endian endianDestination = kEndianBig);

		/// WriteInt32
		///
		/// Writes an array of int32_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt32(IStream* pOS, const int32_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteUint64
		///
		/// Writes a uint64_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint64(IStream* pOS, uint64_t value, Endian endianDestination = kEndianBig);

		/// WriteUint64
		///
		/// Writes an array of uint64_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteUint64(IStream* pOS, const uint64_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteInt64
		///
		/// Writes an int64_t numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt64(IStream* pOS, int64_t value, Endian endianDestination = kEndianBig);

		/// WriteInt64
		///
		/// Writes an array of int64_t numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteInt64(IStream* pOS, const int64_t* value, size_type count, Endian endianDestination = kEndianBig);

		// WriteUint128
		//
		// Writes a uint128_t numerical value to the output stream with the given destination endian-ness.
		// The return value is true if the value could be successfully completely written. 
		// If false, you can use IStream::GetState to determine the error.
		//
		// EAIO_API bool WriteUint128(IStream* pOS, uint128_t value, Endian endianDestination = kEndianBig);

		// WriteUint128
		// Writes an array of uint128_t numerical values to the output stream with the given destination endian-ness.
		// The return value is true if the value could be successfully completely written. 
		// If false, you can use IStream::GetState to determine the error.
		// bool WriteUint128(IStream* pOS, const uint128_t* value, size_type count, Endian endianDestination = kEndianBig);

		// WriteInt128
		//
		// Writes an int128_t numerical value to the output stream with the given destination endian-ness.
		// The return value is true if the value could be successfully completely written. 
		// If false, you can use IStream::GetState to determine the error.
		//
		// EAIO_API bool WriteInt128(IStream* pOS, int128_t value, Endian endianDestination = kEndianBig);

		// WriteInt128
		//
		// Writes an array of int128_t numerical values to the output stream with the given destination endian-ness.
		// The return value is true if the value could be successfully completely written. 
		// If false, you can use IStream::GetState to determine the error.
		//
		// EAIO_API bool WriteInt128(IStream* pOS, const int128_t* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteFloat
		///
		/// Writes a float numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteFloat(IStream* pOS, float value, Endian endianDestination = kEndianBig);

		/// WriteFloat
		///
		/// Writes an array of float numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteFloat(IStream* pOS, const float* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteDouble
		///
		/// Writes a double numerical value to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteDouble(IStream* pOS, double value, Endian endianDestination = kEndianBig);

		/// WriteDouble
		///
		/// Writes an array of double numerical values to the output stream with the given destination endian-ness.
		/// The return value is true if the value could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteDouble(IStream* pOS, const double* value, size_type count, Endian endianDestination = kEndianBig);

		/// WriteString
		///
		/// Writes a string to an IStream.
		/// These functions write the string in a <uint32_t size><char[]> format.
		///
		/// The source line is not expected to end with a nul-terminator; if it has such 
		/// a terminator, the nSourceLineLength value should not include that terminator.
		/// If input nSourceLineLength is kLengthNull, then the line is expected to be 
		/// nul-terminated and the length written is the strlen of pSourceLine.
		/// The pSourceLine value must be non-NULL unless nSourceLineLength is zero.
		///
		/// The return value is true if the string could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error.
		///
		EAIO_API bool WriteString(IStream* pOS, const char* pString, size_t nStringLength, Endian endianDestination = kEndianBig);

		/// WriteString
		///
		/// This is the char16_t version of the WriteString function. 
		/// It behaves the same as with the char version with the exception that
		/// the source is written as UTF16-encoded char16_t.
		///
		EAIO_API bool WriteString(IStream* pOS, const char16_t* pString, size_t nStringLength, Endian endianDestination = kEndianBig);
		EAIO_API bool WriteString(IStream* pOS, const char32_t* pString, size_t nStringLength, Endian endianDestination = kEndianBig);

		/// WriteLine
		///
		/// Writes a line of text to a destination IStream.
		///
		/// The source line is not expected to end in a line delimiter (e.g. '\n'); 
		/// a appropriate line delimiter (defined by the lineEndToUse parameter) will be 
		/// written to the stream by this function. The source line is not expected to end 
		/// with a nul-terminator; if it has such a terminator, the nSourceLineLength value 
		/// should not include that terminator.
		///
		/// If input nLineLength is kLengthNull, then the line is expected to be 
		/// nul-terminated and the length written is the strlen of pSourceLine.
		/// The pLineSource value must be non-NULL unless nLineLength is zero.
		///
		/// The return value is true if the line could be successfully completely written. 
		/// If false, you can use IStream::GetState to determine the error, as this function
		/// generates no errors beyond those related to IStream errors.
		///
		EAIO_API bool WriteLine(IStream* pOS, const char* pLineSource, size_type nLineLength, LineEnd lineEndToUse = kLineEndAuto);

		/// WriteLine
		///
		/// This is the char16_t version of the WriteLine function. 
		/// It behaves the same as with the char version with the exception that
		/// the source is written as UTF16-encoded char16_t.
		///
		EAIO_API bool WriteLine(IStream* pOS, const char16_t* pLineSource, size_type nLineLength, LineEnd lineEndToUse = kLineEndAuto, Endian endianDestination = kEndianBig);
		EAIO_API bool WriteLine(IStream* pOS, const char32_t* pLineSource, size_type nLineLength, LineEnd lineEndToUse = kLineEndAuto, Endian endianDestination = kEndianBig);

		/// WriteString
		///
		/// Writes to an STL or EASTL string or equivalent.
		///
		template <typename String8>
		bool WriteString8(EA::IO::IStream* pIS, const String8& s, Endian endianDestination = kEndianBig);

		template <typename String16>
		bool WriteString16(EA::IO::IStream* pIS, const String16& s, Endian endianDestination = kEndianBig);

		template <typename String32>
		bool WriteString32(EA::IO::IStream* pIS, const String32& s, Endian endianDestination = kEndianBig);


	} // namespace IO

} // namespace EA



namespace EA
{
namespace IO
{

inline EA::IO::IStream& operator<<(EA::IO::IStream& s, const char32_t* str) { EA::IO::WriteLine(&s, str, EA::IO::kLengthNull, EA::IO::kLineEndNone); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, const char16_t* str) { EA::IO::WriteLine(&s, str, EA::IO::kLengthNull, EA::IO::kLineEndNone); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, const char* str)  { EA::IO::WriteLine(&s, str, EA::IO::kLengthNull, EA::IO::kLineEndNone); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, int64_t val)         { EA::IO::WriteInt64 (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, uint64_t val)        { EA::IO::WriteUint64(&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, int32_t val)         { EA::IO::WriteInt32 (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, uint32_t val)        { EA::IO::WriteUint32(&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, int16_t val)         { EA::IO::WriteInt16 (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, uint16_t val)        { EA::IO::WriteUint16(&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, int8_t val)          { EA::IO::WriteInt8  (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, uint8_t val)         { EA::IO::WriteUint8 (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, bool val)            { EA::IO::WriteBool8 (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, float val)           { EA::IO::WriteFloat (&s, val); return s; }
inline EA::IO::IStream& operator<<(EA::IO::IStream& s, double val)          { EA::IO::WriteDouble(&s, val); return s; }

inline EA::IO::IStream& operator>>(EA::IO::IStream& s, int64_t&  val)       { EA::IO::ReadInt64 (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, uint64_t& val)       { EA::IO::ReadUint64(&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, int32_t&  val)       { EA::IO::ReadInt32 (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, uint32_t& val)       { EA::IO::ReadUint32(&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, int16_t&  val)       { EA::IO::ReadInt16 (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, uint16_t& val)       { EA::IO::ReadUint16(&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, int8_t&    val)      { EA::IO::ReadInt8  (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, uint8_t&  val)       { EA::IO::ReadUint8 (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, bool&      val)      { EA::IO::ReadBool8 (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, float&  val)         { EA::IO::ReadFloat (&s, val); return s; }
inline EA::IO::IStream& operator>>(EA::IO::IStream& s, double& val)         { EA::IO::ReadDouble(&s, val); return s; }

#ifdef __APPLE__ // Note by Paul Pedriana (4/2010): How did this Apple-specific code get here? Does it really need to exist only under OSX/iPhone/etc?
	inline EA::IO::IStream& operator>>(EA::IO::IStream& s, uintptr_t& val)       { EA::IO::ReadUint32(&s, reinterpret_cast<uint32_t&>(val)); return s; }
	inline EA::IO::IStream& operator<<(EA::IO::IStream& s, const uintptr_t& val) { EA::IO::WriteUint32(&s, static_cast<uint32_t>(val)); return s; }
#endif



inline StreamAdapter::StreamAdapter()
  : mpStream(NULL),
	mEndianType(EA::IO::kEndianBig), 
	mSuccess(true)
{
}


inline StreamAdapter::StreamAdapter(EA::IO::IStream* pStream, EA::IO::Endian endianType)
  : mpStream(pStream),
	mEndianType(endianType), 
	mSuccess(true)
{
}


inline void StreamAdapter::SetValid(bool success)
{
	VerifyIO(success);  // Right now, the only use for calling this is so the user could catch a breakpoint set in SetValid if success == false.
	mSuccess = success; 
}


inline bool StreamAdapter::IsValid() const
{
	return mSuccess;
}


inline StreamAdapter::operator bool() const
{
	return mSuccess;
}


inline bool StreamAdapter::operator!() const
{
	return !mSuccess;
}

inline EA::IO::IStream* StreamAdapter::GetStream() const
{
	return mpStream;
}


inline void StreamAdapter::SetStream(EA::IO::IStream* pStream)
{
	mpStream = pStream;
}


inline EA::IO::Endian StreamAdapter::GetEndianType() const
{
	return mEndianType;
}


inline void StreamAdapter::SetEndianType(EA::IO::Endian endianType)
{
	mEndianType = endianType;
}


inline void StreamAdapter::VerifyIO(bool success)
{
	if(!success)
	{
		#if defined(EA_DEBUG)
			if(mSuccess)
				mSuccess = false; // You can set a breakpoint here to catch problems on the first occurrence.
		#endif

		mSuccess = false;
	}
}


inline void StreamAdapter::ReadUint8(uint8_t& v)   { VerifyIO(EA::IO::ReadUint8(mpStream, v));  }
inline void StreamAdapter::WriteUint8(uint8_t v)   { VerifyIO(EA::IO::WriteUint8(mpStream, v)); }
inline void StreamAdapter::ReadUint16(uint16_t& v) { VerifyIO(EA::IO::ReadUint16(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteUint16(uint16_t v) { VerifyIO(EA::IO::WriteUint16(mpStream, v, mEndianType)); }
inline void StreamAdapter::ReadUint32(uint32_t& v) { VerifyIO(EA::IO::ReadUint32(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteUint32(uint32_t v) { VerifyIO(EA::IO::WriteUint32(mpStream, v, mEndianType)); }
inline void StreamAdapter::ReadUint64(uint64_t& v) { VerifyIO(EA::IO::ReadUint64(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteUint64(uint64_t v) { VerifyIO(EA::IO::WriteUint64(mpStream, v, mEndianType)); }

inline void StreamAdapter::ReadInt8(int8_t& v)    { VerifyIO(EA::IO::ReadInt8(mpStream, v));  }
inline void StreamAdapter::WriteInt8(int8_t v)    { VerifyIO(EA::IO::WriteInt8(mpStream, v)); }
inline void StreamAdapter::ReadInt16(int16_t& v)  { VerifyIO(EA::IO::ReadInt16(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteInt16(int16_t v)  { VerifyIO(EA::IO::WriteInt16(mpStream, v, mEndianType)); }
inline void StreamAdapter::ReadInt32(int32_t& v)  { VerifyIO(EA::IO::ReadInt32(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteInt32(int32_t v)  { VerifyIO(EA::IO::WriteInt32(mpStream, v, mEndianType)); }
inline void StreamAdapter::ReadInt64(int64_t& v)  { VerifyIO(EA::IO::ReadInt64(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteInt64(int64_t v)  { VerifyIO(EA::IO::WriteInt64(mpStream, v, mEndianType)); }

inline void StreamAdapter::ReadBool8(bool& v)     { VerifyIO(EA::IO::ReadBool8(mpStream, v)); }
inline void StreamAdapter::WriteBool8(bool v)     { VerifyIO(EA::IO::WriteBool8(mpStream, v)); }

inline void StreamAdapter::ReadFloat(float& v)    { VerifyIO(EA::IO::ReadFloat(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteFloat(float v)    { VerifyIO(EA::IO::WriteFloat(mpStream, v, mEndianType)); }
inline void StreamAdapter::ReadDouble(double& v)  { VerifyIO(EA::IO::ReadDouble(mpStream, v, mEndianType)); }
inline void StreamAdapter::WriteDouble(double v)  { VerifyIO(EA::IO::WriteDouble(mpStream, v, mEndianType)); }

// basic type array (unstrided)
inline void StreamAdapter::ReadUint8(uint8_t* v, uint32_t count)          { VerifyIO(EA::IO::ReadUint8(mpStream, v, count));  }
inline void StreamAdapter::WriteUint8(const uint8_t* v, uint32_t count)   { VerifyIO(EA::IO::WriteUint8(mpStream, v, count)); }
inline void StreamAdapter::ReadUint16(uint16_t* v, uint32_t count)        { VerifyIO(EA::IO::ReadUint16(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteUint16(const uint16_t* v, uint32_t count) { VerifyIO(EA::IO::WriteUint16(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::ReadUint32(uint32_t* v, uint32_t count)        { VerifyIO(EA::IO::ReadUint32(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteUint32(const uint32_t* v, uint32_t count) { VerifyIO(EA::IO::WriteUint32(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::ReadUint64(uint64_t* v, uint32_t count)        { VerifyIO(EA::IO::ReadUint64(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteUint64(const uint64_t* v, uint32_t count) { VerifyIO(EA::IO::WriteUint64(mpStream, v, count, mEndianType)); }

inline void StreamAdapter::ReadInt8(int8_t* v, uint32_t count)           { VerifyIO(EA::IO::ReadInt8(mpStream, v, count));  }
inline void StreamAdapter::WriteInt8(const int8_t* v, uint32_t count)    { VerifyIO(EA::IO::WriteInt8(mpStream, v, count)); }
inline void StreamAdapter::ReadInt16(int16_t* v, uint32_t count)         { VerifyIO(EA::IO::ReadInt16(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteInt16(const int16_t* v, uint32_t count)  { VerifyIO(EA::IO::WriteInt16(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::ReadInt32(int32_t* v, uint32_t count)         { VerifyIO(EA::IO::ReadInt32(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteInt32(const int32_t* v, uint32_t count)  { VerifyIO(EA::IO::WriteInt32(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::ReadInt64(int64_t* v, uint32_t count)         { VerifyIO(EA::IO::ReadInt64(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteInt64(const int64_t* v, uint32_t count)  { VerifyIO(EA::IO::WriteInt64(mpStream, v, count, mEndianType)); }

inline void StreamAdapter::ReadBool8(bool* v, uint32_t count)            { VerifyIO(EA::IO::ReadBool8(mpStream, v, count)); }
inline void StreamAdapter::WriteBool8(const bool* v, uint32_t count)     { VerifyIO(EA::IO::WriteBool8(mpStream, v, count)); }

inline void StreamAdapter::ReadFloat(float* v, uint32_t count)           { VerifyIO(EA::IO::ReadFloat(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteFloat(const float* v, uint32_t count)    { VerifyIO(EA::IO::WriteFloat(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::ReadDouble(double* v, uint32_t count)         { VerifyIO(EA::IO::ReadDouble(mpStream, v, count, mEndianType)); }
inline void StreamAdapter::WriteDouble(const double* v, uint32_t count)  { VerifyIO(EA::IO::WriteDouble(mpStream, v, count, mEndianType)); }







//////////////////////////
// ReadString8/16/32
//////////////////////////

inline size_t StreamAdapter::ReadString8(char* string, uint32_t capacity)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess && (capacity > length)) // Test for (capacity > length) because we need a trailing 0 char.
	{
		if(VerifyArraySize(length, sizeof(char)))
		{
			ReadUint8((uint8_t*)string, length);
			string[length] = 0;
		}
	}

	return length; // Returns the required strlen of the string to read.
}

inline size_t StreamAdapter::ReadString16(char16_t* string, uint32_t capacity)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess && (capacity > length)) // Test for (capacity > length) because we need a trailing 0 char.
	{
		if(VerifyArraySize(length, sizeof(char16_t)))
		{
			ReadUint16((uint16_t*)string, length);
			string[length] = 0;
		}
	}

	return length; // Returns the required strlen of the string to read.
}

inline size_t StreamAdapter::ReadString32(char32_t* string, uint32_t capacity)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess && (capacity > length)) // Test for (capacity > length) because we need a trailing 0 char.
	{
		if(VerifyArraySize(length, sizeof(char32_t)))
		{
			ReadUint32((uint32_t*)string, length);
			string[length] = 0;
		}
	}

	return length; // Returns the required strlen of the string to read.
}




//////////////////////////
// ReadString8/16/32
//////////////////////////

template <typename String8>
inline void StreamAdapter::ReadString8(String8& string)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess)
	{
		string.resize(length);
		ReadUint8((uint8_t*)string.data(), length);
	}
}

template <typename String16>
inline void StreamAdapter::ReadString16(String16& string)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess)
	{
		string.resize(length);
		ReadUint16((uint16_t*)string.data(), length);
	}
}

template <typename String32>
inline void StreamAdapter::ReadString32(String32& string)
{
	uint32_t length;
	ReadUint32(length);

	if(mSuccess)
	{
		string.resize(length);
		ReadUint32((uint32_t*)string.data(), length);
	}
}



//////////////////////////
// WriteString8/16/32
//////////////////////////

inline void StreamAdapter::WriteString8(const char* pString, uint32_t length)
{
	WriteUint32(length);
	WriteUint8((uint8_t*)pString, length);
}

inline void StreamAdapter::WriteString16(const char16_t* pString, uint32_t length)
{
	WriteUint32(length);
	WriteUint16((const uint16_t*)pString, length);
}

inline void StreamAdapter::WriteString32(const char32_t* pString, uint32_t length)
{
	WriteUint32(length);
	WriteUint32((const uint32_t*)pString, length);
}



//////////////////////////
// WriteString8/16/32
//////////////////////////

template <typename String8>
inline void StreamAdapter::WriteString8(const String8& string)
{
#if defined(EA_ASSERT_ENABLED) && (EASTL_SIZE_T_32BIT == 0) && (EA_PLATFORM_WORD_SIZE > 4)
	if (string.length() > UINT32_MAX)
		EAStreamAdapterAssertionFailure();
#endif

	WriteUint32((uint32_t)string.length());
	WriteUint8((uint8_t*)string.data(), static_cast<uint32_t>(string.length()));
}

template <typename String16>
inline void StreamAdapter::WriteString16(const String16& string)
{
#if defined(EA_ASSERT_ENABLED) && (EASTL_SIZE_T_32BIT == 0) && (EA_PLATFORM_WORD_SIZE > 4)
	if (string.length() > UINT32_MAX)
		EAStreamAdapterAssertionFailure();
#endif

	WriteUint32((uint32_t)string.length());
	WriteUint16((uint16_t*)string.data(), static_cast<uint32_t>(string.length()));
}

template <typename String32>
inline void StreamAdapter::WriteString32(const String32& string)
{
#if defined(EA_ASSERT_ENABLED) && (EASTL_SIZE_T_32BIT == 0) && (EA_PLATFORM_WORD_SIZE > 4)
	if (string.length() > UINT32_MAX)
		EAStreamAdapterAssertionFailure();
#endif

	WriteUint32((uint32_t)string.length());
	WriteUint32((uint32_t*)string.data(), static_cast<uint32_t>(string.length()));
}



//////////////////////////
// ReadString8/16/32
//////////////////////////

template <typename String8>
inline bool ReadString8(EA::IO::IStream* pIS, String8& string, Endian endianDestination)
{
	uint32_t length;

	if(EA::IO::ReadUint32(pIS, length, endianDestination))
	{
		string.resize(length);
		return EA::IO::ReadUint8(pIS, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(string.data())), length);
	}

	return false;
}


template <typename String16>
inline bool ReadString16(EA::IO::IStream* pIS, String16& string, Endian endianDestination)
{
	uint32_t length;

	if(EA::IO::ReadUint32(pIS, length, endianDestination))
	{
		string.resize(length);
		return ReadUint16(pIS, const_cast<uint16_t*>(reinterpret_cast<const uint16_t*>(string.data())), length, endianDestination);
	}

	return false;
}

template <typename String32>
inline bool ReadString32(EA::IO::IStream* pIS, String32& string, Endian endianDestination)
{
	uint32_t length;

	if(EA::IO::ReadUint32(pIS, length, endianDestination))
	{
		string.resize(length);
		return ReadUint32(pIS, const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(string.data())), length, endianDestination);
	}

	return false;
}


//////////////////////////
// WriteString8/16/32
//////////////////////////

template <typename String8>
inline bool WriteString8(EA::IO::IStream* pIS, const String8& string, Endian endianDestination)
{
	const uint32_t stringLength = static_cast<uint32_t>(string.length()); // This will truncate size_t on 64 bit platforms, but we don't expect to ever have 4GB strings in streams.

	return WriteUint32(pIS, stringLength, endianDestination)
		&& WriteUint8 (pIS, (const uint8_t*)string.data(), stringLength);
}

template <typename String16>
inline bool WriteString16(EA::IO::IStream* pIS, const String16& string, Endian endianDestination)
{
	const uint32_t stringLength = static_cast<uint32_t>(string.length());

	return WriteUint32(pIS, stringLength, endianDestination)
		&& WriteUint16(pIS, (const uint16_t*)string.data(), stringLength, endianDestination);
}

template <typename String32>
inline bool WriteString32(EA::IO::IStream* pIS, const String32& string, Endian endianDestination)
{
	const uint32_t stringLength = static_cast<uint32_t>(string.length());

	return WriteUint32(pIS, stringLength, endianDestination)
		&& WriteUint32(pIS, (const uint32_t*)string.data(), stringLength, endianDestination);
}



} // namespace IO
} // namespace EA


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif // Header include guard


