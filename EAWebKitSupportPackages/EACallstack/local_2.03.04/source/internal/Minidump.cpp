/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Minidump.h>
#include <EAIO/EAStreamAdapter.h>
#include <EASTL/sort.h>
#include <EASTL/algorithm.h>

#include <EAAssert/eaassert.h>

#define EA_MINIDUMP_SIGNATURE ("MDMP")
#define EA_MINIDUMP_VERSION   (42899)

namespace EA
{
namespace Callstack
{

	//////////////////////////////////////////////////////////////////////////
	//
	// MinidumpWriter
	//
	//////////////////////////////////////////////////////////////////////////
	MinidumpWriter::MinidumpWriter( EA::Allocator::ICoreAllocator* pCoreAllocator /*= NULL*/ )
		: mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator())
		, mStream(NULL, EA::IO::kEndianLittle)
		, mbInitialized(false)
		, mStreamDirectory()
		, mMemoryRanges()
	{
	}

	bool MinidumpWriter::Init( EA::IO::IStream* pStream )
	{
		if (!mbInitialized)
		{
			if (pStream)
			{
				if ((pStream->GetState() == EA::IO::kStateSuccess) && ((pStream->GetAccessFlags() & EA::IO::kAccessFlagWrite) == EA::IO::kAccessFlagWrite))
				{
					mStream.SetStream(pStream);
					pStream->SetPosition(0);

					// Write out the header.
					mStream.WriteUint8((const uint8_t*)EA_MINIDUMP_SIGNATURE, 4);       // Signature
					mStream.WriteUint32(EA_MINIDUMP_VERSION);           // Version
					mStream.WriteUint32(0);                             // NumberOfStreams
					mStream.WriteUint64(0);                             // StreamDirectoryRva
					mStream.WriteUint32(0);                             // Checksum, is allowed to be 0
					mStream.WriteUint32(0);                             // Reserved/TimeDateStamp
					mStream.WriteUint64(0);                             // Flags
					mbInitialized = true;
				}
			}
		}

		return mbInitialized;
	}

	bool MinidumpWriter::Shutdown()
	{
		if (mbInitialized)
		{
			mbInitialized = false;
			mStream.SetStream(NULL);
		}
		return !mbInitialized;
	}

	MinidumpWriter::~MinidumpWriter()
	{
		Shutdown();
	}

	void MinidumpWriter::SetAllocator( EA::Allocator::ICoreAllocator* pCoreAllocator )
	{
		mpCoreAllocator = pCoreAllocator;
		// TODO: What about the vectors?
	}

	bool MinidumpWriter::Save()
	{
		EA::IO::IStream* pStream = mStream.GetStream();

		// Create a stream for all of the memory ranges
		StartStream(MemoryListStream);
		mStream.WriteUint32((uint32_t)mMemoryRanges.size());
		for(MinidumpMemoryDescriptorVector::size_type i = 0; i < mMemoryRanges.size(); ++i)
		{
			const MinidumpMemoryDescriptor& range = mMemoryRanges[i];
			mStream.WriteUint64(range.StartOfMemoryRange);
			mStream.WriteUint32(range.DataSize);
			mStream.WriteUint64(range.Offset);
		}
		StopStream();

		const uint64_t directoryOffset = (uint64_t)pStream->GetPosition(); 

		for (MinidumpDirectoryVector::size_type i = 0; i < mStreamDirectory.size() ; ++i)
		{
			const MinidumpDirectory& dir = mStreamDirectory[i];
			mStream.WriteUint32(dir.StreamType);
			mStream.WriteUint32(dir.DataSize);
			mStream.WriteUint64(dir.Offset);
		}

		// Update the minidump file header
		pStream->SetPosition(8);    // Skip version and signature

		mStream.WriteUint32((uint32_t)mStreamDirectory.size());     // NumberOfStreams
		mStream.WriteUint64(directoryOffset);       // StreamDirectoryRVA

		return (pStream->GetState() == EA::IO::kStateSuccess);
	}

	bool MinidumpWriter::WriteMemoryRange( const void* start, const void* stop )
	{
		if (start < stop)
		{
			EA::IO::IStream* stream = mStream.GetStream();
			size_t size = (size_t)((const uint8_t*)stop - (const uint8_t*)start);
			if (size < UINT32_MAX)
			{
				MinidumpMemoryDescriptor& range = mMemoryRanges.push_back();
				range.StartOfMemoryRange = (uint64_t)start;
				range.DataSize = (uint32_t)(size);
				range.Offset = (uint64_t)stream->GetPosition();

				// Here we might need to do advanced memory accesses into special memory, based on the address
				bool result;
				const size_t kBlockSize = 1024 * 1024;
				while (size > kBlockSize)
				{
					result = stream->Write(start, kBlockSize);
					if (!result)
					{
						return false;
					}
					start = (void*)((uintptr_t)start + kBlockSize);
					size -= kBlockSize;
				}
				result = stream->Write(start, size);
				return result;
			}
		}
		return false;
	}

	void MinidumpWriter::StartStream( uint32_t streamType )
	{
		// Check that we aren't starting a stream whilst still writing a stream
		EA_ASSERT(mStreamDirectory.empty() ? true : (mStreamDirectory.back().DataSize != (uint32_t)-1));

		MinidumpDirectory& dir = mStreamDirectory.push_back();
		dir.StreamType = streamType;
		dir.Offset = (uint64_t)mStream.GetStream()->GetPosition();        
		dir.DataSize = (uint32_t)-1; // Marker to ensure we start/stop streams properly
	}

	void MinidumpWriter::StopStream()
	{
		EA_ASSERT(!mStreamDirectory.empty());
		MinidumpDirectory& dir = mStreamDirectory.back();
		EA_ASSERT(dir.DataSize == (uint32_t)-1);

		int64_t size = (int64_t)((uint64_t)mStream.GetStream()->GetPosition() - dir.Offset);
		EA_ASSERT((size > 0) && size < UINT32_MAX);
		dir.DataSize = (uint32_t)size;
	}

	bool MinidumpWriter::WriteCallstackContexts( const void* contexts, uint32_t numContexts, CallstackContextType type, size_t contextSize)
	{
		if (contexts && numContexts)
		{
			EA::IO::IStream* pStream = mStream.GetStream();
			// Create a stream for all of the callstack contexts
			StartStream(CallstackContextStream);
			mStream.WriteUint32(numContexts);
			mStream.WriteUint32(type);
			pStream->Write(contexts, contextSize * numContexts);
			StopStream();
			return (pStream->GetState() == EA::IO::kStateSuccess);
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// MinidumpReader
	//
	//////////////////////////////////////////////////////////////////////////
	MinidumpReader::MinidumpReader( EA::Allocator::ICoreAllocator* pCoreAllocator /*= NULL*/ )
		: mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator())
		, mStream(NULL, EA::IO::kEndianLittle)
		, mbInitialized(false)
	{
	}

	MinidumpReader::~MinidumpReader()
	{
		Shutdown();
	}

	void MinidumpReader::SetAllocator( EA::Allocator::ICoreAllocator* pCoreAllocator )
	{
		mpCoreAllocator = pCoreAllocator;
	}

	bool MinidumpReader::Shutdown()
	{
		if (mbInitialized)
		{
			mbInitialized = false;
			mStream.SetStream(NULL);

			for(MinidumpMemoryDescriptorVector::size_type i = 0; i < mMemoryRanges.size(); ++i)
			{
				mpCoreAllocator->Free(mMemoryRanges[i].Data);
			}
			mMemoryRanges.clear();
		}
		return !mbInitialized;
	}

	bool MinidumpReader::Init( EA::IO::IStream* pStream )
	{
		if (!mbInitialized)
		{
			if (pStream)
			{
				if ((pStream->GetState() == EA::IO::kStateSuccess) && ((pStream->GetAccessFlags() & EA::IO::kAccessFlagRead) == EA::IO::kAccessFlagRead))
				{
					mStream.SetStream(pStream);
					pStream->SetPosition(0);

					// Read and confirm the header.
					uint8_t signature[4];
					uint32_t version;
					mStream.ReadUint8(signature, 4);
					mStream.ReadUint32(version);

					if ((memcmp(EA_MINIDUMP_SIGNATURE, signature, 4) == 0) && version == EA_MINIDUMP_VERSION)
					{
						uint32_t numStreams;
						uint64_t directoryOffset;
						mStream.ReadUint32(numStreams);                             // NumberOfStreams
						mStream.ReadUint64(directoryOffset);                        // NumberOfStreams
						if (ReadDirectory(numStreams, directoryOffset))
						{
							mbInitialized = true;
						}
					}
				}
			}
		}

		return mbInitialized;
	}

	bool MinidumpReader::ReadDirectory( uint32_t numStreams, uint64_t offset )
	{
		const uint32_t kEntrySize = sizeof(uint64_t) + 2 * sizeof(uint32_t);

		EA::IO::IStream* pStream = mStream.GetStream();
		const uint64_t fileSize = pStream->GetSize();
		if ((offset + kEntrySize * numStreams) <= fileSize)
		{
			pStream->SetPosition((EA::IO::off_type)offset);
			MinidumpDirectoryVector directory(numStreams);
			for(uint32_t i = 0; i < numStreams; ++i)
			{
				MinidumpDirectory& dir = directory[i];
				mStream.ReadUint32(dir.StreamType);
				mStream.ReadUint32(dir.DataSize);
				mStream.ReadUint64(dir.Offset);
			}

			for(uint32_t i = 0; i < numStreams; ++i)
			{
				MinidumpDirectory& dir = directory[i];
				if ((dir.DataSize > 0) && (dir.Offset + dir.DataSize < fileSize))
				{
					pStream->SetPosition((EA::IO::off_type)dir.Offset);
					if (!ReadStream(dir.StreamType, dir.DataSize))
						return false;
				}
				else
					return false;
			}
			return (pStream->GetState() == EA::IO::kStateSuccess);
		}
		return false;
	}

	bool MinidumpReader::ReadStream(uint32_t streamType, uint32_t dataSize)
	{
		EA_ASSERT(dataSize);
		switch(streamType)
		{
		case MemoryListStream:          return ReadMemoryListStream(dataSize);
		case CallstackContextStream:    return ReadCallstackContextStream(dataSize);
		default:                        return false;
		}
	}

	bool MinidumpReader::ReadMemoryListStream( uint32_t dataSize )
	{
		const size_t kDescriptorSize = 2 * sizeof(uint64_t) + sizeof(uint32_t);
		
		uint32_t numberOfMemoryRanges;
		mStream.ReadUint32(numberOfMemoryRanges);

		if (dataSize >= sizeof(uint32_t) + kDescriptorSize * numberOfMemoryRanges)
		{
			EA::IO::IStream* pStream = mStream.GetStream();
			mMemoryRanges.reserve(mMemoryRanges.size() + numberOfMemoryRanges);
			MinidumpMemoryDescriptorVector descriptors(numberOfMemoryRanges);

			for(uint32_t i = 0; i < numberOfMemoryRanges; ++i)
			{
				MinidumpMemoryDescriptor& desc = descriptors[i];
				mStream.ReadUint64(desc.StartOfMemoryRange);
				mStream.ReadUint32(desc.DataSize);
				mStream.ReadUint64(desc.Offset);
			}

			for(uint32_t i = 0; i < numberOfMemoryRanges; ++i)
			{
				MinidumpMemoryDescriptor& desc = descriptors[i];
				MemoryRange& range = mMemoryRanges.push_back();

				range.StartVirtualAdress = (const void*)(uintptr_t)desc.StartOfMemoryRange;
				range.EndVirtualAdress = (const void*)((uintptr_t)desc.StartOfMemoryRange + desc.DataSize);
				range.Data = mpCoreAllocator->Alloc(desc.DataSize, "MinidumpReader::MemoryRange", EA::Allocator::MEM_PERM);

				pStream->SetPosition((EA::IO::off_type)desc.Offset);
				EA::IO::size_type read = pStream->Read(range.Data, desc.DataSize);
				if (read != desc.DataSize)
					return false;
			}

			// Sort the memory ranges for faster binary search
			eastl::sort(mMemoryRanges.begin(), mMemoryRanges.end());
			return (pStream->GetState() == EA::IO::kStateSuccess);
		}
		return false;
	}

	template <class T>
	bool MinidumpReader::ReadCallstackContexts( uint32_t dataSize, uint32_t numberOfContexts, eastl::vector<T>& contexts )
	{
		if (dataSize >= 2 * sizeof(uint32_t) + sizeof(T) * numberOfContexts)
		{
			EA::IO::IStream* pStream = mStream.GetStream();
			contexts.reserve(contexts.size() + numberOfContexts);

			for(uint32_t i = 0; i < numberOfContexts; ++i)
			{
				T& context = contexts.push_back();
				if (pStream->Read(&context, sizeof(T)) != sizeof(T))
					return false;
			}

			return (pStream->GetState() == EA::IO::kStateSuccess);
		}
		return false;
	}

	bool MinidumpReader::ReadCallstackContextStream( uint32_t dataSize )
	{
		uint32_t numberOfContexts;
		mStream.ReadUint32(numberOfContexts);
		uint32_t contextType;
		mStream.ReadUint32(contextType);
#if EACALLSTACK_OFFLINE
		switch (contextType)
		{
		case CALLSTACK_CONTEXT_X86_64:  return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacksX86_64);
		case CALLSTACK_CONTEXT_X86:     return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacksX86);
		case CALLSTACK_CONTEXT_POWERPC: return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacksPowerPC);
		case CALLSTACK_CONTEXT_ARM:     return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacksARM);
		case CALLSTACK_CONTEXT_MIPS:    return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacksMIPS);
		}
#else
		if (contextType == CallstackContext::kType)
		{
			return ReadCallstackContexts(dataSize, numberOfContexts, mCallstacks);  
		}
#endif
		return false;
	}

	bool MinidumpReader::FindMemoryRange( const void* virtualAddress, MemoryRange& range )
	{
		MemoryRange find;
		find.EndVirtualAdress = virtualAddress;
		MemoryRangeVector::iterator found = eastl::lower_bound(mMemoryRanges.begin(), mMemoryRanges.end(), find);
		if ((found != mMemoryRanges.end()) && found->Contains(virtualAddress))
		{
			range = *found;
			return true;
		}
		return false;
	}

} // namespace Callstack
} // namespace EA










