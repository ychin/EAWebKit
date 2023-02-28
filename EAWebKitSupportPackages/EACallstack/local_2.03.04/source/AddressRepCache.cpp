///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/EAAddressRep.h>
#include <EACallstack/Allocator.h>

namespace EA
{
namespace Callstack
{

///////////////////////////////////////////////////////////////////////////////
// Default AddressRepCache
///////////////////////////////////////////////////////////////////////////////
static AddressRepCache* gpAddressRepCache = nullptr;


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepCache
//
AddressRepCache* GetAddressRepCache()
{
	return gpAddressRepCache;
}


///////////////////////////////////////////////////////////////////////////////
// SetAddressRepCache
//
AddressRepCache* SetAddressRepCache(AddressRepCache* pAddressRepCache)
{
	AddressRepCache* const pAddressRepCacheSaved = gpAddressRepCache;

	gpAddressRepCache = pAddressRepCache;

	return pAddressRepCacheSaved;
}

///////////////////////////////////////////////////////////////////////////////
// AddressRepEntry
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// AddressRepEntry
//
AddressRepEntry::AddressRepEntry(EA::Allocator::ICoreAllocator* pCoreAllocator)
	: mARTCheckFlags(0)
	, mFunctionOffset(0)
	, mFileOffset(0)
{
	if (!pCoreAllocator)
	{
		pCoreAllocator = EA::Callstack::GetAllocator();
	}

	for (size_t i = 0; i < kARTCount; ++i)
	{
		mAddressRep[i].get_allocator().set_allocator(pCoreAllocator);
		mAddressRep[i].get_allocator().set_name(EACALLSTACK_ALLOC_PREFIX "AddressRepEntry");
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void AddressRepEntry::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	for (size_t i = 0; i < kARTCount; ++i)
	{
		mAddressRep[i].get_allocator().set_allocator(pCoreAllocator);
	}
}

///////////////////////////////////////////////////////////////////////////////
// AddressRepCache
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// AddressRepCache
//
AddressRepCache::AddressRepCache(EA::Allocator::ICoreAllocator* pCoreAllocator)
	: mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator())
	, mAddressRepMap(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "AddressRepMap", mpCoreAllocator))
	, mAddressRepLookupSet(mpCoreAllocator)
	, mCacheSize(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void AddressRepCache::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator();

	mAddressRepMap.get_allocator().set_allocator(mpCoreAllocator);
	mAddressRepLookupSet.SetAllocator(mpCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int AddressRepCache::GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray)
{
	const char* pStrResults[kARTCount] = { };

	const int result = GetAddressRep(addressRepTypeFlags, address, pStrResults, pIntValueArray, true);

	for (size_t i = 0; i < kARTCount; ++i)
	{
		if (pStrResults[i])
		{
			pRepArray[i] = pStrResults[i];
		}
	}

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
const char* AddressRepCache::GetAddressRep(AddressRepType addressRepType, uint64_t address, int* pIntValue, bool bLookupIfMissing)
{
	const char* pStringResults[kARTCount] = { };
	int pIntResults[kARTCount] = { };

	if (GetAddressRep(1 << addressRepType, address, pStringResults, pIntResults, bLookupIfMissing))
	{
		if (pIntValue)
		{
			*pIntValue = pIntResults[addressRepType];
		}

		return pStringResults[addressRepType];
	}

	return nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int AddressRepCache::GetAddressRep(int addressRepTypeFlags, uint64_t address, const char** pRepArray, int* pIntValueArray, bool bLookupIfMissing)
{
	int returnFlags = 0;

	auto pairResult = mAddressRepMap.emplace(eastl::make_pair(eastl::move(address), AddressRepEntry{}));

	AddressRepEntry& ar = (pairResult.first)->second;

	if (pairResult.second == true)
	{
		ar.SetAllocator(mpCoreAllocator);
	}

	const uint32_t wantTypeFlags = static_cast<uint32_t>(addressRepTypeFlags);
	const uint32_t haveTypeFlags = (ar.mARTCheckFlags & wantTypeFlags);

	// If we haven't already looked up one or more of the requested types (regardless of the success of the lookup)...
	if (bLookupIfMissing && (haveTypeFlags != wantTypeFlags))
	{
		int         pIntResults[kARTCount] = { };
		FixedString pStringResults[kARTCount];

		eastl::fill_n(pStringResults, static_cast<size_t>(kARTCount),
					  FixedString(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "AddressRepCache", mpCoreAllocator)));


		mAddressRepLookupSet.GetAddressRep(addressRepTypeFlags, address, pStringResults, pIntResults);

		// Record the results in the cache.
		for (size_t i = 0; i < kARTCount; ++i)
		{
			if ((wantTypeFlags & (1 << i)) && !pStringResults[i].empty())
			{
				SetAddressRep(ar, static_cast<AddressRepType>(i), address, pStringResults[i].c_str(), pStringResults[i].size(), pIntResults[i]);
			}
		}
	}

	// Copy the data available from the cache into the result arrays.
	const uint32_t dataAvailableFlags = (ar.mARTCheckFlags & wantTypeFlags);

	for (size_t i = 0; i < kARTCount; ++i)
	{
		if ((dataAvailableFlags & (1 << i)) && !ar.mAddressRep[i].empty())
		{
			pRepArray[i] = ar.mAddressRep[i].c_str();

			if (i == kARTFunctionOffset)
			{
				pIntValueArray[i] = ar.mFunctionOffset;
			}
			else if(i == kARTFileLine)
			{
				pIntValueArray[i] = ar.mFileOffset;
			}
			else
			{
				pIntValueArray[i] = 0;
			}

			returnFlags |= (1 << i);
		}
		else
		{
			pRepArray[i] = nullptr;
			pIntValueArray[i] = 0;
		}
	}

	return returnFlags;
}

void AddressRepCache::SetAddressRep(AddressRepEntry& ar, AddressRepType addressRepType, uint64_t address,
									const char* pRep, size_t nRepLength, int intValue)
{
	const uint32_t addressRepTypeFlag = (1 << addressRepType);

	ar.mARTCheckFlags |= addressRepTypeFlag;

	if (addressRepType == kARTFunctionOffset)
	{
		ar.mFunctionOffset = static_cast<uint32_t>(intValue);
	}
	else if (addressRepType == kARTFileLine)
	{
		ar.mFileOffset = static_cast<uint32_t>(intValue);
	}

	ar.mAddressRep[addressRepType].clear();
	ar.mAddressRep[addressRepType].assign(pRep, static_cast<eastl_size_t>(nRepLength));
}

///////////////////////////////////////////////////////////////////////////////
// SetAddressRep
//
void AddressRepCache::SetAddressRep(AddressRepType addressRepType, uint64_t address,
									const char* pRep, size_t nRepLength, int intValue)
{
	auto pairResult = mAddressRepMap.emplace(eastl::make_pair(eastl::move(address), AddressRepEntry{}));

	AddressRepEntry& ar = (pairResult.first)->second;

	if (pairResult.second == true)
	{
		ar.SetAllocator(mpCoreAllocator);
	}

	mCacheSize += CalculateMemoryUsage(pairResult.second, nRepLength);

	SetAddressRep(ar, addressRepType, address, pRep, nRepLength, intValue);
}


///////////////////////////////////////////////////////////////////////////////
// CalculateMemoryUsage
//
size_t AddressRepCache::CalculateMemoryUsage(bool bNewNode, size_t nRepLength)
{
	size_t usage = (sizeof(char) * (nRepLength + 1)); // The AddressRepEntry::mAddressRep string allocates this much memory

	if (bNewNode)
	{
		usage += sizeof(AddressRepMap::node_type);
	}

	return usage;
}


///////////////////////////////////////////////////////////////////////////////
// PurgeCache
//
// This purge function is unusable internally until we resolve a primary problem.
// The problem is that the user might be holding pointers into our cache
// memory and we don't know when it is safe to purge entries or what entries
// are safe to purge. Probably we'll want some kind of marker that no users
// are holding onto pointers.
//
void AddressRepCache::PurgeCache(size_t newCacheSize)
{
	if (newCacheSize == 0)
	{
		mAddressRepMap.clear();
		mCacheSize = 0;
	}
	else
	{
		AddressRepMap::iterator it = mAddressRepMap.begin();

		while (!mAddressRepMap.empty() && (mCacheSize > newCacheSize))
		{
			AddressRepMap::value_type& value = *it;
			const AddressRepEntry& are = value.second;

			// Calculate and update how much memory we have trimmed.
			size_t entrySize = sizeof(AddressRepMap::node_type);

			for (int i = 0; i < kARTCount; ++i)
			{
				if (!are.mAddressRep[i].empty())
				{
					entrySize += (sizeof(char) * (are.mAddressRep[i].length() + 1));
				}
			}

			mCacheSize -= entrySize;

			// Remove the entry.
			AddressRepMap::const_local_iterator itL(const_cast<AddressRepMap::iterator::node_type*>(it.get_node()));
			const bool bBucketHadMoreThanOneEntry = (++itL.mpNode == nullptr);
			it = mAddressRepMap.erase(it);

			// Move to the next bucket, as opposed to the next element. This will have
			// the convenient effect of removing the oldest entries in the hash table.
			if (bBucketHadMoreThanOneEntry)
			{
				it.increment_bucket();
			}
			if (it == mAddressRepMap.end())
			{
				it = mAddressRepMap.begin();
			}
		}
	}
}


} // namespace Callstack
} // namespace EA
