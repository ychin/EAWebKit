/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if !defined(EACALLSTACK_MINIDUMP_H)

#define EACALLSTACK_MINIDUMP_H

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EACallstack.h>

#include <coreallocator/icoreallocator_interface.h>
#include <EASTL/vector.h>
#include <EAIO/EAStreamAdapter.h>


EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.


namespace EA
{
	namespace Callstack
	{
		enum enMinidumpStreamType
		{
			UnusedStream                = 0,
			ReservedStream0             = 1,
			ReservedStream1             = 2,
			ThreadListStream            = 3,
			ModuleListStream            = 4,
			MemoryListStream            = 5,
			ExceptionStream             = 6,
			SystemInfoStream            = 7,
			ThreadExListStream          = 8,
			Memory64ListStream          = 9,
			CommentStreamA              = 10,
			CommentStreamW              = 11,
			HandleDataStream            = 12,
			FunctionTableStream         = 13,
			UnloadedModuleListStream    = 14,
			MiscInfoStream              = 15,
			MemoryInfoListStream        = 16,
			ThreadInfoListStream        = 17,
			HandleOperationListStream   = 18,
			TokenStream                 = 19,
			CallstackContextStream      = 0x0001FFFF
		};

		// Format for a callstack stream
		// uint32_t NumberOfCallstackContexts;
		// uint32_t CallstackContextType;

		struct MinidumpDirectory
		{
			uint32_t StreamType;
			uint32_t DataSize;
			uint64_t Offset;
		};
		typedef eastl::vector<MinidumpDirectory> MinidumpDirectoryVector;

		struct MinidumpMemoryDescriptor
		{
			uint64_t    StartOfMemoryRange;
			uint32_t    DataSize;
			uint64_t    Offset;
		};
		typedef eastl::vector<MinidumpMemoryDescriptor> MinidumpMemoryDescriptorVector;

		class EACALLSTACK_API MinidumpWriter
		{
		public:
			MinidumpWriter(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
			~MinidumpWriter();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(EA::IO::IStream* pStream);
			bool Shutdown();

			bool WriteMemoryRange(const void* start, const void* stop);

			template <class T>
			bool WriteCallstackContexts(const T* contexts, uint32_t numContexts);

			bool Save();

		protected:
			void StartStream(uint32_t streamType);
			void StopStream();

			bool WriteCallstackContexts(const void* contexts, uint32_t numContexts, CallstackContextType type, size_t contextSize);

		private:
			EA::Allocator::ICoreAllocator*  mpCoreAllocator;
			EA::IO::StreamAdapter           mStream;
			bool                            mbInitialized;

			MinidumpDirectoryVector                 mStreamDirectory;
			MinidumpMemoryDescriptorVector          mMemoryRanges;
		};

		class EACALLSTACK_API MinidumpReader
		{
		public:
			MinidumpReader(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
			~MinidumpReader();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(EA::IO::IStream* pStream);
			bool Shutdown();

			struct MemoryRange
			{
				const void* StartVirtualAdress;
				const void* EndVirtualAdress;
				void*       Data;
				inline size_t Size() const;
				inline bool Contains(const void* virtualAdress) const;
				inline const void* Translate(const void* virtualAdress) const;
				inline bool operator<(const MemoryRange& rhs) const;
				MemoryRange() : StartVirtualAdress(NULL), EndVirtualAdress(NULL), Data(NULL) {}
			};
			typedef eastl::vector<MemoryRange> MemoryRangeVector;

			bool FindMemoryRange(const void* virtualAddress, MemoryRange& range);

		protected:
			bool ReadDirectory(uint32_t numStreams, uint64_t offset);
			bool ReadStream(uint32_t streamType, uint32_t dataSize);

			bool ReadMemoryListStream(uint32_t dataSize);
			bool ReadCallstackContextStream(uint32_t dataSize);

		private:
			EA::Allocator::ICoreAllocator*  mpCoreAllocator;
			EA::IO::StreamAdapter           mStream;
			bool                            mbInitialized;

		public:
			MemoryRangeVector               mMemoryRanges;
#if EACALLSTACK_OFFLINE
			eastl::vector<CallstackContextX86_64>   mCallstacksX86_64;
			eastl::vector<CallstackContextX86_64>   mCallstacksX86;
			eastl::vector<CallstackContextPowerPC>  mCallstacksPowerPC;
			eastl::vector<CallstackContextARM>      mCallstacksARM;
			eastl::vector<CallstackContextMIPS>     mCallstacksMIPS;
#else
			typedef eastl::vector<CallstackContext>  CallstackContextVector;
			CallstackContextVector mCallstacks;
#endif

		protected:
			template <class T>
			bool ReadCallstackContexts(uint32_t dataSize, uint32_t numberOfContexts, eastl::vector<T>& contexts);
		};

		//////////////////////////////////////////////////////////////////////////
		//
		// Inlined functions
		//
		//////////////////////////////////////////////////////////////////////////

		template <class T>
		bool MinidumpWriter::WriteCallstackContexts(const T* contexts, uint32_t numContexts)
		{
			return WriteCallstackContexts(contexts, numContexts, T::kType, sizeof(T));
		}

		inline size_t MinidumpReader::MemoryRange::Size() const
		{
			return (size_t)((uintptr_t)EndVirtualAdress - (uintptr_t)StartVirtualAdress);
		}

		inline bool MinidumpReader::MemoryRange::Contains(const void* virtualAdress) const
		{
			return ((virtualAdress >= StartVirtualAdress) && (virtualAdress < EndVirtualAdress));
		}

		inline const void* MinidumpReader::MemoryRange::Translate(const void* virtualAdress) const
		{
			// EA_ASSERT(Contains(virtualAdress));
			return (const void*)((uintptr_t)virtualAdress - (uintptr_t)StartVirtualAdress + (uintptr_t)Data);
		}

		inline bool MinidumpReader::MemoryRange::operator<(const MemoryRange& rhs) const
		{
			// We sort using the end range so that we can use lower_bound to find the right range for an address
			return EndVirtualAdress < rhs.EndVirtualAdress;
		}
	}
}


EA_RESTORE_VC_WARNING();


#endif // EACALLSTACK_MINIDUMP_H
