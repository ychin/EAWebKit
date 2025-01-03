///////////////////////////////////////////////////////////////////////////////
// EARefCount.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
//
// Implements basic reference counting functionality.
//
// This file was copied from Foundation/EARefCount.h
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAREFCOUNT_H
#define EAIO_EAREFCOUNT_H



/// namespace EA
/// The standard Electronic Arts namespace
namespace EA
{
	template <typename T>
	class EAIOAutoRefCount
	{
	public:
		typedef T element_type;

		T* mpObject;

	public:
		EAIOAutoRefCount()
			: mpObject(NULL) {}

		EAIOAutoRefCount(T* pObject)
			: mpObject(pObject) 
		{ 
			if(mpObject)
				mpObject->AddRef();
		} 

		EAIOAutoRefCount(const EAIOAutoRefCount& rhs) 
			: mpObject(rhs.mpObject) 
		{ 
			if(mpObject)
				mpObject->AddRef();
		}

		~EAIOAutoRefCount() 
		{
			if(mpObject)
				mpObject->Release();
		}

		EAIOAutoRefCount& operator=(const EAIOAutoRefCount& source)       
		{         
			return operator=(source.mpObject);
		}

		EAIOAutoRefCount& operator=(T* pObject)
		{         
			if(pObject != mpObject)
			{
				T* const pTemp = mpObject;
				if(pObject)
					pObject->AddRef();
				mpObject = pObject;
				if(pTemp)
					pTemp->Release();
			}
			return *this;
		}

		T& operator *()  const 
		{
			return *mpObject;
		}

		T* operator ->() const
		{
			return  mpObject;
		}

		operator T*() const 
		{
			return  mpObject;
		}

	}; // class EAIOAutoRefCount


} // namespace EA


#endif  // Header include guard
