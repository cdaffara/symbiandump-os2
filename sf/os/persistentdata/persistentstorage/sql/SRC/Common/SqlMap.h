// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// RSqlMap template class declaration.
// 
//

#ifndef __SQLMAP_H__
#define __SQLMAP_H__

#include "SqlAssert.h"

//Forward declaration
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> class RSqlMap;

/**
(KEY, DATA, REFCNTR) template pair class used by RSqlMap template class.

The class has 3 template arguments:
 - KEY        - the key part of the pair object;
 - DATA       - the data part of the pair object;
 - REFCNTR    - the reference counting part of the pair object;
                REFCNTR class has to provide "TInt Increment()" and "TInt Decrement()" methods.

@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
template <class KEY, class DATA, class REFCNTR> 
NONSHARABLE_STRUCT(TSqlPair)
	{
	TSqlPair(const KEY& aKey, const DATA& aData);
	TSqlPair(const KEY& aKey);
	TSqlPair();
	
	KEY		iKey;
	DATA	iData;
	REFCNTR	iRefCounter;
	
	};

/**
TSqlMapIterator class. It describes an object which can be used as a 
forward, read-only iterator for RSqlMap containers.

The class has 4 template arguments:
 - KEY        - the key part of the pair object;
 - DATA       - the data part of the pair object;
 - REFCNTR    - the reference counting part of the pair object;
                REFCNTR class has to provide "TInt Increment()" and "TInt Decrement()" methods.
 - DESTRUCTOR - the KEY and DATA objects destroying part of the pair object;
				DESTRUCTOR class has to provide "void Destroy(KEY& aKey, DATA& aData)" method.

@see TSqlPair
@see RSqlMap

@internalComponent
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
NONSHARABLE_CLASS(TSqlMapIterator)
	{
public:
	TSqlMapIterator(const RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>& aMap);
	TBool Next(TSqlPair<KEY, DATA, REFCNTR>& aPair) const;
	
private:
	const RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>&	iMap;
	mutable TInt									iIndex;
	
	};

/**
RSqlMap template class describes an object that controls an ordered sequence of reference counted entries.
Each entry has a key of type KEY and an associated with it data of type DATA.

The class has 4 template arguments:
 - KEY        - the key part of the pair object;
 - DATA       - the data part of the pair object;
 - REFCNTR    - the reference counting part of the pair object;
                REFCNTR class has to provide "TInt Increment()" and "TInt Decrement()" methods.
 - DESTRUCTOR - the KEY and DATA objects destroying part of the pair object;
				DESTRUCTOR class has to provide "void Destroy(KEY& aKey, DATA& aData)" method.

The algorithm for determining the order of two entries is provided by a TLinearOrder
object, supplied by the client of RSqlMap during RSqlMap instance construction
(RSqlMap constructor, aOrder argument).

@see TSqlPair
@see TSqlMapIterator

@internalComponent
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
NONSHARABLE_CLASS(RSqlMap)
	{
	friend class TSqlMapIterator<KEY, DATA, REFCNTR, DESTRUCTOR>;
	
public:
	RSqlMap(const TLinearOrder< TSqlPair<KEY, DATA, REFCNTR> >& aOrder, const DESTRUCTOR& aDestructor);
	void Close();
	TInt Insert(const KEY& aKey, const DATA& aData);
	void Remove(const KEY& aKey);
	TSqlPair<KEY, DATA, REFCNTR>* Entry(const KEY& aKey);

private:
	TLinearOrder< TSqlPair<KEY, DATA, REFCNTR> >	iOrder;
	RArray< TSqlPair<KEY, DATA, REFCNTR> >			iSet;
	DESTRUCTOR 										iDestructor;

	};

#include "SqlMap.inl"

#endif//__SQLMAP_H__
