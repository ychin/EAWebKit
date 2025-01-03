///////////////////////////////////////////////////////////////////////////////
// EAAllocationRecorder 
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// By Paul Pedriana, Maxis
//
// The class records and plays back allocation activity. This is useful for 
// offline analysis of allocation activity and for testing allocators.
//
// Supported user-modifyable defines
//     PPM_ENABLED
///////////////////////////////////////////////////////////////////////////////



#ifndef PPMALLOC_EAALLOCATIONRECORDER_H
#define PPMALLOC_EAALLOCATIONRECORDER_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <PPMalloc/internal/mutex.h>
#include <stddef.h>

#if PPM_ENABLED
	#include <PPMalloc/EAGeneralAllocator.h>
	#include <PPMalloc/EAGeneralAllocatorDebug.h>
#endif


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class FileIO
		///
		/// Allows the user to override the existing file IO.
		/// A default implementation is automatically provided which uses fopen on 
		/// platforms supporting fopen and is siblings. This interface's conventions
		/// match the equivalent f functions in specification.
		///
		class FileIO
		{
		public:
			virtual ~FileIO(){}
			virtual uintptr_t Open(const char* pFilePath, const char* pMode) = 0;               // fopen
			virtual void      Close(uintptr_t pFile) = 0;                                       // fclose
			virtual size_t    Read(void* data, size_t count, uintptr_t pFile) = 0;              // fread
			virtual size_t    Write(const void* data, size_t count, uintptr_t pFile) = 0;       // fwrite
			virtual int       SetPosition(uintptr_t pFile, long position, int relativeTo) = 0;  // fseek -- relativeTo = SEEK_SET, SEEK_CUR, SEEK_END
			virtual long      GetPosition(uintptr_t pFile) = 0;                                 // ftell
			virtual char*     GetLine(char* buffer, int capacity, uintptr_t pFile) = 0;         // fgets
		};


		///////////////////////////////////////////////////////////////////////////////
		/// class AllocationRecorder
		///
		/// Records and plays back allocations. If PPM_ENABLED is enbabled, then the
		/// GeneralAllocator (a.k.a. PPMalloc) is used for playback. Otherwise, the 
		/// built-in new[] and delete[] (not malloc/free) are used. new/delete is 
		/// used because the C++ language lets you override these but not malloc/free.
		///
		/// In recording mode, the recorder writes allocations to a disk file. If you 
		/// want to record on a platform that doesn't let you write to disk files or 
		/// isn't likely to have enough space for recorded disk files, you can use
		/// this class to format a record line for you and you can write that line 
		/// elsewhere as needed.
		///
		/// The format of recording files is text. Binary formats have been studied
		/// and compared to text formats (http://citeseer.nj.nec.com/chilimbi00designing.html)
		/// but binary formats tend to be only ~30% more efficient in uncompressed 
		/// space and ~10% more efficient in compressed format and are much harder 
		/// to read. The format is implemented with one allocation event per text 
		/// line and each event simply echos the parameters and return value of 
		/// the call. The format is this:
		///     command [result] arg1 arg2 ... [flags]
		/// Pointers and flags are in hex; sizes are in decimal. Hex values have no leading '0x'.
		/// 
		/// As an example, we have the following lines (which don't use the optional final flags argument):
		///    m c23a17bc 20\n                  // Malloc 20 bytes. Return value was 0xc23a17bc.
		///    a c23a17bc 20 16\n               // Malloc 20 bytes, with 16 byte alignment. Return value was 0xc23a17bc.
		///    o c23a17bc 20 16 8\n             // Malloc 20 bytes, with 16 byte allignment and 8 byte alignment offset.
		///    r c23a17bc 18cb34a2 28\n         // Realloc pointer 0x18cb34a2 to 28 bytes. Return value was 0xc23a17bc.
		///    c c23a17bc 15 2\n                // Calloc 15 elements of 2 bytes each (i.e. 30 bytes). Return value was 0xc23a17bc.
		///    f c23a17bc\n                     // Free pointer 0xc23a17bc.
		///    ; arbitrary comment text         // This is a comment line, which is to be ignored during playback of recordings.
		///                                     // This is an empty line, which is to be ignored during playback of recordings.
		///    m c23a17bc 20\n                  // Malloc 20 bytes. Return value was 0xc23a17bc.
		///
		/// In the case of the flags argument, the above malloc line might be:
		///    m c23a17bc 20 e2\n               // Malloc 20 bytes with allocation flags 0xe2. Return value was 0xc23a17bc.
		/// 
		/// As of this writing, multiple thread awareness is not supported. This may be
		/// added in the future if that becomes important. In the meantime, supporting 
		/// such functionality is not trivial and so will be put off until needed.
		///
		class PPM_API AllocationRecorder
		{
		public:
			enum Mode
			{
				kModeNone,       /// The recorder is not initialized for any mode. In this mode, recording functions can still be used if you supply your own buffer.
				kModeRecord,     /// The recorder mode is recording.
				kModePlayback    /// The recorder mode is playback.
			};

			enum PlaybackAllocatorType
			{
				kPlaybackAllocatorTypeNULL,         /// In this case, mallocs and frees don't actually occur but instead are "no-op"s. Malloc returns an ever incrementing value which has no meaning. This is useful for telling if an allocation trace is valid and the allocation/free calls were sensible.
				kPlaybackAllocatorTypeMalloc,       /// In this case, C malloc, free, and realloc are used.
				kPlaybackAllocatorTypeNew,          /// In this case, C++ global new/delete are used for playback. You can override new/delete to direct to your own allocator.
				kPlaybackAllocatorTypeGeneral,      /// In this case, GeneralAllocator is used for playback.
				kPlaybackAllocatorTypeGeneralDebug, /// In this case, GeneralAllocatorDebug is used for playback.
				kPlaybackAllocatorTypeCustom        /// In this case, a custom user-defined allocator is used for playback.
			};

			enum HookType
			{
				kHookTypeMalloc,                    // Designated by 'm'
				kHookTypeMallocAligned,             // Designated by 'a' or 'o'
				kHookTypeCalloc,                    // Designated by 'c'
				kHookTypeRealloc,                   // Designated by 'r'
				kHookTypeFree                       // Designated by 'f'
			};

			struct HookInfo
			{
				HookType      mHookType;               /// One of enum HookType.
				size_t        mnSizeInputTotal;        /// For Malloc, Calloc, Realloc, MallocAligned.
				const void*   mpDataInput;             /// For Realloc, Free.
				size_t        mnCountInput;            /// For Calloc, MallocMultiple.
				size_t        mnSizeInput;             /// For Malloc, Realloc.
				size_t        mnAlignmentInput;        /// For MallocAligned.
				size_t        mnAlignmentOffsetInput;  /// For MallocAligned.
				int           mnAllocationFlags;       /// For all allocation functions.
				void*         mpDataOutput;            /// For Malloc, Calloc, Realloc, MallocAligned.
			};

			AllocationRecorder();
		   ~AllocationRecorder();

			/// Init
			/// Initializes the recorder with the given mode and a given stream name.
			/// For most platforms, a stream name means a file path.
			/// A file can be used for both kModeRecord and kModePlayback.
			bool InitFile(Mode mode, const char* pStreamName, PlaybackAllocatorType playbackAllocatorType = kPlaybackAllocatorTypeGeneralDebug);

			/// InitData
			/// Initializes the recorder with a pointer to data.
			/// Currently this function can only be used for kModePlayback; 
			/// recording to a fixed-size memory buffer is not supported.
			bool InitData(Mode mode, const char* pData, size_t dataLength, PlaybackAllocatorType playbackAllocatorType = kPlaybackAllocatorTypeGeneralDebug);

			/// Shutdown
			/// Stops all recording activity and frees all resources. Returns the 
			/// instance to its initially constructed state.
			bool Shutdown();

			/// SetFileIO
			/// By default an fopen-based FileIO is provided. You need to call this only if  
			/// you want or need to provide something else.
			void SetFileIO(FileIO* pFileIO);

			/// RecordMalloc
			/// Records a single malloc or C++ new event. The input pResult is the pointer 
			/// returned by the allocator, the input nSize is the bytes requested for allocation.
			/// If the pBuffer argument is non-null, then it is filled with the formatted
			/// allocation record text line. This buffer must hold at least 256 bytes.
			/// The mode can be kModeNone and the pBuffer will still be filled.
			void RecordMalloc(const void* pResult, size_t nSize, size_t nAlignment = 0, size_t nAlignmentOffset = 0, int nAllocationFlags = 0, char* pBuffer = NULL);

			/// RecordRealloc
			/// Records a single realloc event.
			void RecordRealloc(const void* pResult, const void* pData, size_t nSize, int nAllocationFlags = 0, char* pBuffer = NULL);

			/// RecordCalloc
			/// Records a single calloc event.
			void RecordCalloc(const void* pResult, size_t nElementCount, size_t nElementSize, int nAllocationFlags = 0, char* pBuffer = NULL);

			/// RecordFree
			/// Records a single free or C++ delete event.
			void RecordFree(const void* pData, char* pBuffer = NULL);

			#if PPM_ENABLED
				/// RecordAllocator
				/// If enabled, attaches the recorder to the given allocator and records allocations that
				/// occur with that allocator. If disabled, then recording of the given allocator is disabled.
				/// Only a single allocator instance can be recorded with a single instance of this class.
				void RecordAllocator(GeneralAllocator* pAllocator, bool bEnable);

				/// GetPlaybackAllocator
				/// Gets the allocator used for playback.
				GeneralAllocator* GetPlaybackAllocator();
			#endif

			/// SetPlaybackHook
			/// For use with kPlaybackAllocatorTypeCustom. Set this playback hook before playing back
			/// the recording. Your hook function will be used to implement the playback allocations.
			typedef void (*HookFunctionType)(HookInfo* pHookInfo, void* pContext);
			void SetPlaybackHook(HookFunctionType pHookFunction, void* pContext);

			/// Playback
			/// Plays back the initialized allocation event stream for nStepCount 
			/// steps (-1 or ~0 means to play all steps). Some compilers warn 
			/// against default unsigned arguments being negative.
			void Playback(size_t nStepCount = (size_t)(~0));

			/// PlaybackStep
			/// Plays back the next single allocation event in the source stream.
			/// This function is useful if you have an allocation recording as a block 
			/// of memory and you simply want to step through it. However, many real-world
			/// allocation recordings are too large to fit in the memory of a console 
			/// machine, and so you may want to use the other form of PlaybackStep.
			/// Note that this function will move past any non-action lines in the 
			/// playback stream and execute the first line with an action.
			/// The playback stream is always positioned at the beginning of the next
			/// line, even if there was an error processing the current line. 
			/// Returns true if the action could be successfully played. The return value 
			/// does not indicate the success or failure of the Malloc call; it merely 
			/// indicates the success of the playback simulation. 
			bool PlaybackStep();

			/// PlaybackStep
			/// Plays back the next single allocation event.
			/// Upon return, sets cAction to the action attempted, such as 'm' for malloc.
			/// If the line is empty or contains no action (e.g. comment line), *pActionResult is set to zero ('\0').
			/// Returns true if the action could be successfully played. Always returns true for the
			/// case of no action (cAction gets set to zero). The return value does not indicate the success or
			/// failure of the Malloc call; it merely indicates the success of the playback simulation. 
			bool PlaybackStep(const char* pLine, char* pActionResult);

			/// GetCurrentEventCount
			/// Returns the number of events that have been recorded or played back, depending on the mode.
			size_t GetCurrentEventCount() const;

			/// GetTotalEventCount
			/// For playback mode, returns the total number of events in the recording. 
			/// For recording mode, returns the same as GetCurrentEventCount.
			size_t GetTotalEventCount() const;

			/// ReadFileIntoData
			/// Reads the file into a memory buffer allocated by operator new[]. 
			/// The user is expected to free the memory with operator delete[].
			static char* ReadFileIntoData(const char* pStreamName, size_t* pFileSize, size_t* pEventCount, FileIO* pFileIO = NULL);

		protected:
			#if PPM_ENABLED
				static void HookFunction(const GeneralAllocator::HookInfo* pHookInfo, void* pContext);
			#endif

			Mode                  mMode;                   /// One of enum Mode.
			PlaybackAllocatorType mPlaybackAllocatorType;  /// Playback allocator to use.
			char                  mStreamName[512];        /// For a file stream, this would be the path name.
			uintptr_t             mStreamHandle;           /// Where records are read from or written to.
			const char*           mpStreamData;            /// Holds all data that's in the stream.
			size_t                mnStreamDataSize;        /// Length of stream data in chars (and thus bytes).
			const char*           mpStreamPosition;        /// Holds position of next char to read in the stream.
			const char*           mpStreamEnd;             /// Holds the end position of the stream.
			size_t                mnCurrentEventCount;     /// Count of processed events;
			size_t                mnTotalEventCount;       /// Count of total events, including those not yet processed.
			uint64_t              mnEventTime;             /// Time of last operation. Used for supporting time-specific allocations.
			HookFunctionType      mpHookFunction;          /// For use in custom playback types.
			void*                 mpHookFunctionContext;   /// User-supplied function context.
			FileIO*               mpFileIO;                /// Implements file IO to read and write recording files.

			#if PPM_ENABLED
				GeneralAllocator        mGeneralAllocator;              /// 
				GeneralAllocatorDebug   mGeneralAllocatorDebug;         /// 
				GeneralAllocator*       mpRecordedAllocator;            /// Allocator used for recording via RecordAllocator().
				GeneralAllocator*       mpPlaybackAllocator;            /// Allocator to use for playback.
			#endif

			#if PPM_THREAD_SAFETY_SUPPORTED
				void*                   mpMutex;                        /// Pointer to mutex or NULL if thread safety is disabled.
				uint64_t                mpMutexData[kMutexBufferSize];  /// Provides memory from which the mutex is allocated (so we don't have to allocate it from a heap). Format of this data is platform-specific.
			#endif

			class PPM_API HashTable
			{
			public:
				HashTable() : mppTable(NULL), mbReHashing(false), mnTableSize(0), mnTableSizeMin(4096), mnElementCount(0), mpFunctionContext(NULL) { }
			   ~HashTable() { Clear(true); }

				void   Insert(const void* pKey, const void* pValue);
				bool   Find(const void* pKey, void** ppValue) const;
				bool   Remove(const void* pKey);
				void   Clear(bool bDestroyTable);
				void   Nuke();
				size_t Size() const { return mnElementCount; }

			protected:
				friend class AllocationRecorder;

				void   ReHash(size_t nNewTableSize);
				size_t Hash(const void* pKey) const { return (size_t)(((uintptr_t)pKey / 8) % mnTableSize); }

				struct HashPair{
					HashPair(const void* pKey = NULL, const void* pValue = NULL, HashPair* pNext = NULL) : mpKey(pKey), mpValue(pValue), mpNext(pNext) {} 

					const void* mpKey;
					const void* mpValue;
					HashPair*   mpNext;
				};

				HashPair**      mppTable;
				bool            mbReHashing;
				size_t          mnTableSize;
				size_t          mnTableSizeMin;
				size_t          mnElementCount;
				void*           mpFunctionContext;
			};

			HashTable mHashTable;

		};

	} // namespace Allocator

} // namespace EA


#endif // PPMALLOC_EAALLOCATIONRECORDER_H




