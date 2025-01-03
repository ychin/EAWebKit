///////////////////////////////////////////////////////////////////////////////
// EACallstackRecorder.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/EACallstackRecorder.h>
#include <EACallstack/EAAddressRep.h>
#include <EACallstack/Allocator.h>
#include <EAIO/EAStreamAdapter.h>


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


namespace EA
{
namespace Callstack
{


/////////////////////////////////////////////////////////////////////////////
/// kMaxCallstackDepth
///
/// The max possible amount of callstack depth. This value is configurable 
/// and is only in place to prevent a lot of unnecessary memory usage. 
///
const size_t kMaxCallstackDepth = 32;


///////////////////////////////////////////////////////////////////////////////
// GetCallstackHash
//
EACALLSTACK_API callstack_hash_t GetCallstackHash(const void* const callstack[], size_t entryCount)
{
	callstack_hash_t h = kCallstackHashEmpty;
	for(size_t i = 0; i < entryCount; i++)
	{
		static uintptr_t LARGE_PRIME = 7919; //1000th  prime
		h = (callstack_hash_t)((((uintptr_t)h) * LARGE_PRIME) ^ ((uintptr_t)callstack[i]) ^ ((uintptr_t)h >> (uintptr_t)7));
	}
	return h;
}




///////////////////////////////////////////////////////////////////////////////
// CallstackRecorder
//
CallstackRecorder::CallstackRecorder(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mCallstackEntrySet(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/CallstackRecorder", mpCoreAllocator)),
	mCallstackEntryHistory(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/CallstackRecorder", mpCoreAllocator)),
	mPredicate(0),
	mMaxDepth(kMaxCallstackDepthDefault)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~CallstackRecorder
//
CallstackRecorder::~CallstackRecorder()
{
	// Nothing to do.
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void CallstackRecorder::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	mCallstackEntrySet.get_allocator().set_allocator(pCoreAllocator);
	mCallstackEntryHistory.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// AddEntry
//
bool CallstackRecorder::AddEntry(const char* pName, const void* callstack[], size_t entryCount, uintptr_t predicate)
{
	if((mPredicate == 0) || (mPredicate == predicate))
	{
		// Note that the pName value will only be applied to the CallstackEntry in the 
		// set if it is the first time the given callstack is being added to the set. 
		const CallstackEntry ceTemp(pName, callstack, entryCount);

		eastl::pair<CallstackEntrySet::iterator, bool> result = mCallstackEntrySet.insert(ceTemp);
		CallstackEntry& ce = const_cast<CallstackEntry&>(*result.first);
		ce.IncrementCount();

		callstack_hash_t h = ce.GetHash();
		mCallstackEntryHistory.push_back(h);

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetEntry
//
const CallstackEntry* CallstackRecorder::GetEntry(size_t i)
{
	const CallstackEntry* pCE = NULL;

	if(i < mCallstackEntryHistory.size())
	{
		// This technique is a little hacky, as it assumes there are no duplicate hashes.
		const callstack_hash_t h = mCallstackEntryHistory[(eastl_size_t)i];
		const eastl_size_t bucket = h % mCallstackEntrySet.bucket_count();
		CallstackEntrySet::const_local_iterator it    = mCallstackEntrySet.begin(bucket);
		CallstackEntrySet::const_local_iterator itEnd = mCallstackEntrySet.end(bucket);

		while(it != itEnd)
		{
			pCE = &*it;
			if(pCE->GetHash() == h) // Note that this isn't always true, as multiple hash values 
				break;              // can refer to the same bucket when you have small bucket counts.
			++it;                   // We do however assume that hashes don't collide. It's not a very
		}                           // big deal if they do, given the way this system is used.

		if((it == itEnd) && !mCallstackEntrySet.empty())
			pCE = &*mCallstackEntrySet.begin();
	}

	return pCE;
}


///////////////////////////////////////////////////////////////////////////////
// RecordCurrentCallstack
//
bool RecordCurrentCallstack(CallstackRecorder& csr, const char* pName, uintptr_t predicate, size_t maxDepth)
{
	const uintptr_t csrPredicate = csr.GetPredicate();

	if((csrPredicate == 0) || (csrPredicate == predicate))
	{
		// Even though we have an mMaxDepth member, we still don't go any higher than kMaxCallstackDepth.
		if(maxDepth > csr.GetMaxCallstackDepth())
			maxDepth = csr.GetMaxCallstackDepth();
		if(maxDepth > kMaxCallstackDepth)
			maxDepth = kMaxCallstackDepth;

		void* callstack[kMaxCallstackDepth + 2]; // + 2 because we want to have enough space to throw out the first two and still have kMaxCallstackDepth usable entries.
		size_t n = GetCallstack(callstack, maxDepth + 2); // maxDepth + 2 is always <= (kMaxCallstackDepth + 2).

		if(n > 2) // Check for > 2 because we will use callstack + 2, as we don't want GetCallstack nor this function itself to be included in the list.
		{
			const void** pCallstackConst = &(const void*&)*callstack;  // = (const void**)(uintptr_t)callstack;

			return csr.AddEntry(pName, pCallstackConst + 2, n - 2, predicate);
		}
	}

	return false;
}



namespace Internal
{
	void WriteAddressRep(const void* pAddress, EA::IO::IStream* pOutput)
	{
		Callstack::FixedString buffer;
		AddressRepCache* const pARC = GetAddressRepCache();

		if(pARC)
		{
			const char* repArray[kARTCount];
			int         intArray[kARTCount];

			// If we can get file/line information, then use it.
			if(pARC->GetAddressRep((1 << kARTFileLine) | (1 << kARTSource), (uint64_t)(uintptr_t)pAddress, repArray, intArray, true) > 0)
			{
				if(repArray[kARTFileLine])
					buffer.sprintf("      %s(%d)", repArray[kARTFileLine], intArray[kARTFileLine]);

				if(repArray[kARTSource] && *repArray[kARTSource])
					buffer.append_sprintf(": \n%s\n", repArray[kARTSource]); // To consider: check to make sure the source is useful and not something like a '}' character.
			}
		}

		if(buffer.empty())
		{
			// Just write a hex address value.
			Callstack::AddressToString((uint64_t)(uintptr_t)pAddress, buffer, true);
			buffer.insert((eastl_size_t)0, (eastl_size_t)6, ' ');
		}

		buffer.push_back('\n');
		pOutput->Write(buffer.data(), buffer.length());
	}


	void WriteCallStack(const CallstackEntry::Callstack& callstack, EA::IO::IStream* pOutput)
	{
		for(eastl_size_t j = 0, jEnd = callstack.size(); j < jEnd; ++j) // For each callstack entry...
		{
			const void* const pAddress = callstack[j];
			Internal::WriteAddressRep(pAddress, pOutput);
		}

		pOutput->Write("\n", 1);
	}

} // namespace Internal


///////////////////////////////////////////////////////////////////////////////
// TraceCallstackRecording
//
bool TraceCallstackRecording(CallstackRecorder& csr, EA::IO::IStream* pOutput, bool bDetail)
{
	Callstack::FixedString buffer(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/TraceCallstackRecording", EA::Callstack::GetAllocator()));

	buffer.sprintf("Callstack recording trace for %s:\n", csr.GetName());
	pOutput->Write(buffer.data(), buffer.length());

	if(bDetail)
	{
		for(size_t i = 0, iEnd = csr.GetEntryCount(); i < iEnd; ++i) // For each call...
		{
			const CallstackEntry* const pCE = csr.GetEntry(i);
			const char* pName = pCE->GetName();
			const CallstackEntry::Callstack& callstack = pCE->GetCallstack();

			buffer.sprintf("   %d: ", (unsigned)(i + 1));
			if(pName && *pName)
				buffer.append_sprintf("%s", pName);
			buffer.append((eastl_size_t)1, '\n');
			pOutput->Write(buffer.data(), buffer.length());

			Internal::WriteCallStack(callstack, pOutput);
		}
	}
	else
	{
		// What we may want to do instead here is iterate in order of the calls.
		const CallstackRecorder::CallstackEntrySet& cseSet = csr.GetCallstackEntrySet();

		for(CallstackRecorder::CallstackEntrySet::iterator it = cseSet.begin(), itEnd = cseSet.end(); it != itEnd; ++it)
		{
			const CallstackEntry& ce = *it;
			const char* pName = ce.GetName();
			const CallstackEntry::Callstack& callstack = ce.GetCallstack();

			if(pName && *pName)
				buffer.sprintf("  %s, call count: %u", pName, (unsigned)ce.GetCount());
			else
				buffer.sprintf("  Call count: %u", (unsigned)ce.GetCount());
			buffer.append((eastl_size_t)1, '\n');
			pOutput->Write(buffer.data(), buffer.length());
			
			Internal::WriteCallStack(callstack, pOutput);
		}
	}

	return true;
}



} // namespace Callstack

} // namespace EA


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


