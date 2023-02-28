/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The CallstackRecorder records call stacks. By this we mean that the call
// stack in effect at any given location in code can be recorded. It can also
// subsequently report the call stack history that was recorded.
//
// The most common use of CallstackRecorder is to aid in debugging, particularly
// reference count mismatch tracking. It often happens that trying to track the
// calls to a given AddRef and Release function by using breakpoints in the
// debugger is very tedious and having this automated saves a lot of time.
// CallstackRecorder can also track the number of unique ways that a given
// function is called so you can tell what all the code is that calls some function.
//
//CallstackRecorder is usually intrusive to source code; that is, you usually
// need to insert calls to it in your source code for it to work. As such, it is
// much like profiling statements that users often put into source code to profile
// segments of code. One way to avoid modifying source code is to use the ability
// of many compilers to set up a callback function of your choice whenever any
// function is called. This is called /callcap and /fastcap under Microsoft
// compilers, for example.
//
// Report output includes file/line information and also source code if available.
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_EACALLSTACKRECORDER_H
#define EACALLSTACK_EACALLSTACKRECORDER_H


#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/EAAddressRep.h>
#include <EASTL/hash_set.h>
#include <EASTL/deque.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_string.h>
#include <stddef.h>


EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.

namespace EA
{

namespace IO
{
	class IStream;

} // namespace IO

namespace Callstack
{
	/// callstack_hash_t
	///
	/// Defines the integral hash value used for callstack hashes.
	///
	typedef uint32_t callstack_hash_t;


	/// kCallstackHashEmpty
	///
	/// Defines the hash value of an empty callstack.
	///
	const uint32_t kCallstackHashEmpty = 0xffffffff;



	/// GetCallstackHash
	///
	/// Returns a hash value for a callstack.
	///
	/// Example usage;
	///    void* callstack[32];
	///    size_t n = GetCallstack(callstack, 32, NULL);
	///    callstack_hash_t h = GetCallstackHash(callstack, n);
	///
	EACALLSTACK_API callstack_hash_t GetCallstackHash(const void* const callstack[], size_t entryCount);



	/// CallstackEntry
	///
	/// Defines a given callstack with a name and a hash value.
	///
	class EACALLSTACK_API CallstackEntry
	{
	public:
		typedef eastl::fixed_vector<const void*, 16, true, EA::Callstack::EASTLCoreAllocator> Callstack;

	public:
		CallstackEntry();
		CallstackEntry(const char* pName, const void* pCallstack[], size_t entryCount);
		CallstackEntry(const CallstackEntry& ce);
		CallstackEntry& operator=(const CallstackEntry& ce);

		callstack_hash_t GetHash() const;
		void             SetHash(callstack_hash_t h);
		const char*      GetName() const;
		const Callstack& GetCallstack() const;
		uint32_t         GetCount() const;
		uint32_t         IncrementCount();

		bool operator==(const CallstackEntry& ce) const;
		bool operator!=(const CallstackEntry& ce) const;
		bool operator< (const CallstackEntry& ce) const;

		struct CallstackEntryHash { eastl_size_t operator()(const CallstackEntry& ce) const; };

	protected:
		callstack_hash_t mHash;         /// The hash value for this entry.
		const char*      mpName;        /// The name of this entry.
		Callstack        mCallstack;    /// The callstack of this entry.
		uint32_t         mCount;        /// Count of times this entry has occurred. Initialized to zero.
	};



	/// CallstackRecorder
	///
	/// Implements a record of callstacks.
	/// A common usage of this is to track down all the calls to a given function in
	/// order reference-counting debug memory leaks.
	///
	class EACALLSTACK_API CallstackRecorder
	{
	public:
		typedef eastl::hash_set<CallstackEntry, CallstackEntry::CallstackEntryHash,
					eastl::equal_to<CallstackEntry>,  EA::Callstack::EASTLCoreAllocator> CallstackEntrySet;     // Unique set of callstacks
		typedef eastl::deque<callstack_hash_t, EA::Callstack::EASTLCoreAllocator, 32>    CallstackEntryHistory; // Call order history.

		/// kMaxCallstackDepthDefault
		/// The default initial value for the max recorded callstack depth.
		static const size_t kMaxCallstackDepthDefault = 32;

	public:
		/// CallstackRecorder
		/// Default constructor.
		CallstackRecorder(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);

		/// ~CallstackRecorder
		/// Virtual destructor.
		virtual ~CallstackRecorder();

		/// SetAllocator
		/// Sets the memory allocator to use with this class.
		/// This allocator is used to allocate CallstackEntrySet elements and
		/// CallstackEntryHistory elements.
		/// This function must be called before any entries are added.
		virtual void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

		/// AddEntry
		/// Adds a new callstack entry to our recording. Returns true if the
		/// entry was added. It would only fail to be added if the predicate
		/// was specified and didn't match the expected value.
		///
		/// Example usage:
		///     void* callstack[32];
		///     size_t n = GetCallstack(callstack, 32);
		///     pRecorder->AddEntry("AddRef", &(const void*&)*callstack, n);
		///
		virtual bool AddEntry(const char* pName, const void* callstack[], size_t entryCount, uintptr_t predicate = 0);

		/// GetEntryCount
		/// Returns the number of entries in our recording.
		virtual size_t GetEntryCount() const;

		/// GetEntry
		/// Returns the given historical entry, where the valid entry index is
		/// in the range of [0, GetEntryCount).
		virtual const CallstackEntry* GetEntry(size_t i);

		/// GetCallstackEntrySet
		/// Returns the set of unique callstack entries. This is useful for
		/// enumerating the set of unique call pathways to a given location.
		const CallstackEntrySet& GetCallstackEntrySet() const;

		/// GetName
		/// Returns the name set for this recorder by SetName. If no name was
		/// set, then an empty string is returned (and not a NULL pointer).
		const char* GetName() const;

		/// SetName
		/// Sets a name for this recorder. The pName argument must not be NULL.
		/// The name should be less than 24 characters for it to be stored without
		/// doing a memory allocation.
		void SetName(const char* pName);

		/// GetPredicate
		/// Returns the value set by SetPredicate. Will return 0 if the
		/// SetPredicate function wasn't called.
		uintptr_t GetPredicate() const;

		/// SetPredicate
		/// Sets a value whereby if it is nonzero, then AddEntry functions will
		/// not add entries unless the predicate matches. A more complicated
		/// mechanism for filtering entry additions required subclassing this
		/// CallstackRecorder.
		void SetPredicate(uintptr_t predicate);

		/// GetMaxCallstackDepth
		/// Returns the value set by SetMaxCallstackDepth. If SetMaxCallstackDepth
		/// was not called, kMaxCallstackDepthDefault will be returned.
		size_t GetMaxCallstackDepth() const;

		/// SetMaxCallstackDepth
		/// Sets the max callstack depth which will be stored in a history record.
		/// The system may in practice impose a lower max value that that provided
		/// by the user. However, no value higher than the user-specified max depth
		/// will ever be applied.
		void SetMaxCallstackDepth(size_t maxDepth);

	protected:
		typedef eastl::fixed_string<char, 24, true, EA::Callstack::EASTLCoreAllocator> NameString;

		EA::Allocator::ICoreAllocator* mpCoreAllocator;
		CallstackEntrySet              mCallstackEntrySet;
		CallstackEntryHistory          mCallstackEntryHistory;
		uintptr_t                      mPredicate;
		size_t                         mMaxDepth;
		NameString                     mName;
	};


	/// RecordCurrentCallstack
	///
	/// Adds the current callstack to a given CallstackRecorder.
	/// A typical use of this is to implement AddRef/Release tracking for
	/// some C++ class object that is leaking due to refcounting mismatches.
	///
	/// The return value is the same as with the CallstackRecorder::AddEntry
	/// function.
	///
	/// Example usage:
	///     CallstackRecorder gRecorder;
	///     gRecorder.SetPredicate((uintptr_t)pSomeObjectWeWantToTrack);
	///
	///     int SomeClass::AddRef() {
	///         RecordCurrentCallstack(gRecorder, "SomeClass::AddRef", (uintptr_t)this);
	///     }
	///
	///     int SomeClass::Release() {
	///         RecordCurrentCallstack(gRecorder, "SomeClass::Release", (uintptr_t)this);
	///     }
	///
	EACALLSTACK_API bool RecordCurrentCallstack(CallstackRecorder& csr, const char* pName, uintptr_t predicate = 0, size_t maxDepth = 1024);


	/// TraceCallstackRecording
	///
	/// Writes a recording to a given output stream.
	/// If bDetail is true, every individual call is traced in order.
	/// If bDetail is false, only the unique set of calls are traced,
	/// and they are traced in order of first appearance.
	///
	/// If you want the resulting trace to include symbol information such as
	/// file/line information and source code, don't forget to initialize the
	/// global AddressRepCache (accessible via GetAddressRepCache/SetAddressRepCache
	/// in your application startup.
	///
	EACALLSTACK_API bool TraceCallstackRecording(CallstackRecorder& csr, EA::IO::IStream* pOutput, bool bDetail);


} // namespace Callstack
} // namespace EA


///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
namespace Callstack
{

	inline CallstackEntry::CallstackEntry()
	  : mHash(kCallstackHashEmpty),
		mpName(NULL),
		mCallstack(),
		mCount(0)
	{
	}


	inline CallstackEntry::CallstackEntry(const char* pName, const void* pCallstack[], size_t entryCount)
	  : mHash(GetCallstackHash(pCallstack, entryCount)),
		mpName(pName),
		mCallstack(pCallstack, pCallstack + entryCount),
		mCount(0)
	{
	}


	inline CallstackEntry::CallstackEntry(const CallstackEntry& ce)
	{
		mHash      = ce.mHash;
		mpName     = ce.mpName;
		mCallstack = ce.mCallstack;
		mCount     = ce.mCount;
	}


	inline CallstackEntry& CallstackEntry::operator=(const CallstackEntry& ce)
	{
		mHash      = ce.mHash;
		mpName     = ce.mpName;
		mCallstack = ce.mCallstack;
		mCount     = ce.mCount;

		return *this;
	}


	inline callstack_hash_t CallstackEntry::GetHash() const
	{
		return mHash;
	}


	inline void CallstackEntry::SetHash(callstack_hash_t h)
	{
		mHash = h;
	}


	inline const char* CallstackEntry::GetName() const
	{
		return mpName;
	}


	inline const CallstackEntry::Callstack& CallstackEntry::GetCallstack() const
	{
		return mCallstack;
	}


	inline uint32_t CallstackEntry::GetCount() const
	{
		return mCount;
	}


	inline uint32_t CallstackEntry::IncrementCount()
	{
		return ++mCount;
	}


	inline bool CallstackEntry::operator==(const CallstackEntry& ce) const
	{
		return mCallstack == ce.mCallstack;
	}


	inline bool CallstackEntry::operator!=(const CallstackEntry& ce) const
	{
		return mCallstack != ce.mCallstack;
	}


	inline bool CallstackEntry::operator< (const CallstackEntry& ce) const
	{
		return mCallstack < ce.mCallstack;
	}


	inline eastl_size_t CallstackEntry::CallstackEntryHash::operator()(const CallstackEntry& ce) const
	{
		return ce.mHash;
	}


	inline size_t CallstackRecorder::GetEntryCount() const
	{
		return (size_t)mCallstackEntryHistory.size();
	}


	inline const CallstackRecorder::CallstackEntrySet& CallstackRecorder::GetCallstackEntrySet() const
	{
		return mCallstackEntrySet;
	}

	inline const char* CallstackRecorder::GetName() const
	{
		return mName.c_str();
	}


	inline void CallstackRecorder::SetName(const char* pName)
	{
		mName = pName;
	}

	inline uintptr_t CallstackRecorder::GetPredicate() const
	{
		return mPredicate;
	}

	inline void CallstackRecorder::SetPredicate(uintptr_t predicate)
	{
		mPredicate = predicate;
	}

	inline size_t CallstackRecorder::GetMaxCallstackDepth() const
	{
		return mMaxDepth;
	}

	inline void CallstackRecorder::SetMaxCallstackDepth(size_t maxDepth)
	{
		mMaxDepth = maxDepth;
	}


} // namespace Callstack
} // namespace EA


EA_RESTORE_VC_WARNING();


#endif // EACALLSTACK_EACALLSTACKRECORDER_H
