///////////////////////////////////////////////////////////////////////////////
// StringDictionary.cpp
//
// Implements a compressed word dictionary for searching and iterating.
// Originally based on the cDictionary class by Alex Zvenigorodsky.
// Some of the original functionality has been removed, while some 
// functionality has been added.
//
///////////////////////////////////////////////////////////////////////////////


#include "StringDictionary.h"
#include <assert.h>
#include <algorithm>


namespace EA
{


// To do: Make this configurable, as we do within EAText.
typedef char16_t Char;


///////////////////////////////////////////////////////////////////////////////
// cWordNode
//
// Helper class
//
class cWordNode
{
public:
	cWordNode(Char c);
   ~cWordNode();

	void Clear();

	// Inserts the given word into the dictionary.
	bool Insert(Char* pWord);

	// Return true if the dictionary contains the given word.
	bool Contains(const Char* pWord) const;

	// Returns true if pText begins with a word from the dictionary.
	size_t FindAtStart(const Char* pText, size_t textLength) const;

	// Prints all the words in the dictionary.
	// void Print() const;

protected:
	cWordNode* FindChild(Char c) const;    // Returns a child node corresponding to the letter, or NULL.
	cWordNode* AddChild(Char c);           // Appends a new child node to the list.

protected:
	cWordNode* mpSibling;
	cWordNode* mpChildren;
	Char       mC;
	bool       mbValidWordEnding;
};



///////////////////////////////////////////////////////////////////////////////
// cWordNode
///////////////////////////////////////////////////////////////////////////////

cWordNode::cWordNode(Char c)
  : mpSibling(NULL),
	mpChildren(NULL),
	mC(c);
	mbValidWordEnding(false)
{
}


cWordNode::~cWordNode()
{
	Clear();
}


cWordNode::Clear()
{
	cWordNode* pChildNode = mpChildren;

	while (pChildNode)
	{
		cWordNode* const pNext = pChildNode->mpSibling;
		delete pChildNode;
		pChildNode = pNext;
	}
}


bool cWordNode::Insert(Char* pWord)
{
	if (pWord)
	{
		if (*pWord)
		{
			cWordNode* const pChild = FindChild(*pWord);

			if (pChild == NULL)
				pChild = AddChild(*pWord);

			pChild->Insert(pWord + 1);
		}
		else
		{
			// If we're inserting a NULL word, that means this node is an end word.
			mbValidWordEnding = true;
		}

		return true;
	}

	return false;
}


bool cWordNode::Contains(Char* pWord) const
{
	if (pWord)
	{
		if ((*pWord == 0) && mbValidWordEnding)
			return true;

		const cWordNode* const pChild = FindChild(*pWord);

		if (pChild)
			return pChild->Contains(pWord + 1);
	}

	return false;
}


size_t cWordNode::FindAtStart(const Char* pText, size_t textLength) const
{
	// Note: I (Paul Pedriana) have not debugged this function. It could be all wrong.

	size_t           nLength = 0;
	const cWordNode* pChild  = NULL;

	for(size_t i = 0; (i < textLength) && pChild; i++)
	{
		// Note: Currently the child list isn't sorted. This could be made faster if it was sorted.
		for(pChild = mpChildren; pChild; pChild = pChild->mpSibling)
		{
			if(pChild->mC == pText[i])
			{
				if(pChild->mbValidWordEnding)
					return nLength;

				break;
			}
		}
	}

	return 0;
}


cWordNode* cWordNode::AddChild(Char c)
{
	if (mpChildren == NULL)
	{
		mpChildren = new cWordNode(c);
		return mpChildren;
	}

	// To do: Make the child list be sorted.
	// Find the last child node (whose sibling is NULL).
	cWordNode* pLastChild = mpChildren;

	while (pLastChild->mpSibling)
		pLastChild = pLastChild->mpSibling;

	// Add a new sibling to that child node and return it.
	pLastChild->mpSibling = new cWordNode(c);

	return pLastChild->mpSibling;
}


cWordNode* cWordNode::FindChild(Char c) const
{
	for (cWordNode* pChild = mpChildren; pChild; pChild = pChild->mpSibling)
	{
		if (pChild->mC == c)
			return pChild;
	}

	return NULL;
}


// void cWordNode::Print()
// {
//     Char        buf[4] = { mC, 0, 0, 0 };
//     std::string sPrefixPlusChildChar(sPrefix + buf);
//
//     if (mbValidWordEnding)
//         std::cout << sPrefixPlusChildChar.c_str() << '\n';
//
//     for (cWordNode* pChild = mpChildren; pChild; pChild = pChild->mpSibling)
//         pChild->Print();
//}





///////////////////////////////////////////////////////////////////////////////
// cDictionary
///////////////////////////////////////////////////////////////////////////////

cDictionary::cDictionary()
  : mWordNodeRoot((Char)0)
{
}


void cDictionary::Clear()
{
	mWordNodeRoot.Clear();
}


bool cDictionary::Insert(Char* pWord)
{
	return mWordNodeRoot.Insert(pWord);
}


bool cDictionary::Contains(const Char* pWord) const
{
	return mWordNodeRoot.Contains(pWord);
}


size_t cDictionary::FindAtStart(const Char* pText, size_t textLength) const
{
	return mWordNodeRoot.FindAtStart(pText, textLength);
}


// void cDictionary::Print()
// {
//     mWordNodeRoot.Print();
// }



} // namespace EA



