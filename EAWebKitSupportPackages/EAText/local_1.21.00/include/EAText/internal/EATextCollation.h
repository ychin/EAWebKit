#pragma once
#include <EAText/EAText.h>
#include <EASTL/internal/type_pod.h>
#include <EASTL/hash_map.h>

namespace EA
{
namespace Text
{
	// This struct outlines which collation elements for a
	// variable weighted character are ignorable/non-ignorable
	struct VariableWeightedCharacter
	{
		Char mChar;
		bool mCE0 :1;
		bool mCE1 :1;
		bool mCE2 :1;
	};

	struct VariableCollElement
	{
		uint16_t mL1;
		uint8_t  mL2;
		uint8_t  mL3;
		uint16_t mL4;
	};

	struct CollElement
	{
		uint16_t mL1;
		uint8_t  mL2;
		uint8_t  mL3;
	};

	struct MultiCollEntry
	{
		Char        mChar1;
		CollElement mCE0;
		CollElement mCE1;
	};

	struct MultiCollIndex
	{
		Char    mChar0;
		uint8_t mCount;
	};

	struct CollEntry1
	{
		Char        mChar;
		CollElement mCE0;
	};

	struct CollEntry2
	{
		Char mChar;
		CollElement mCE0;
		CollElement mCE1;
	};

	struct CollEntry3
	{
		Char mChar;
		CollElement mCE0;
		CollElement mCE1;
		CollElement mCE2;
	};

	// This struct is used for the CollationElement MultiMap so we might be able to store
	// the collation elements mapping to the two characters which have a custom collation element array
	struct CollElement2
	{
		CollElement mCE0;
		CollElement mCE1;
	};


	// Use CharHash instead of eastl::hash<Char> in order to deal with a bug in EABase related to PS3/EDG char types.
	struct CharHash { size_t operator()(Char c) const { return static_cast<size_t>(c); } };

	typedef eastl::hash_map<Char, CollElement2, CharHash, eastl::equal_to<Char>, EA::Allocator::EASTLICoreAllocator> EntryMap;
	typedef eastl::hash_map<Char, EntryMap, CharHash, eastl::equal_to<Char>, EA::Allocator::EASTLICoreAllocator> KeyMap;
	
	void EATextCollationInit();
	void EATextCollationShutdown();
}} // namespace EA::Text
