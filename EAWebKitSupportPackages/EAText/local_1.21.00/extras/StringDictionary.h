///////////////////////////////////////////////////////////////////////////////
// StringDictionary.h
//
// Implements a compressed word dictionary for searching and iterating.
// Originally based on the cDictionary class by Alex Zvenigorodsky.
// Some of the original functionality has been removed, while some 
// functionality has been added.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef STRINGDICTIONARY_H
#define STRINGDICTIONARY_H


#include <EABase/eabase.h>


namespace EA
{
	class cWordNode;

	
	///////////////////////////////////////////////////////////////////////////////
	// StringDictionary
	//
	// Implements a word dictionary via a string trie.
	// A string trie is significantly more efficient than a hash_set for the 
	// case where you want to do partial string matches. A string trie can also
	// use less space than a hash_set when the string trie is optimized.
	//
	class StringDictionary
	{
	public:
		typedef char16_t Char; // To do: Make this configurable, as we do within EAText.

		StringDictionary();

		void Clear();

		// Inserts the given word into the dictionary.
		bool Insert(Char* pWord);

		// Return true if the dictionary contains the given word.
		bool Contains(const Char* pWord) const;

		// Returns length of match if pText begins with a word from the dictionary.
		size_t FindAtStart(const Char* pText, size_t textLength) const;

		// Prints all the words in the dictionary.
		// void Print() const;

	protected:
		cWordNode mWordNodeRoot;
	};
}

#endif // Header include guard

