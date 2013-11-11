// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RMap template class declaration.
// 
//

#ifndef __D32MAP_H__
#define __D32MAP_H__

#include "D32Assert.h"

/**
(KEY, DATA) template pair class used by RMap template class.
@internalComponent
*/
template <class KEY, class DATA> 
struct TPair
	{
	TPair(const KEY& aKey, const DATA& aData);
	TPair(const KEY& aKey);
	TPair();
	KEY		iKey;
	DATA	iData;
	};

//Forward declaration
template <class KEY, class DATA> class RMap;

/**
TMapIterator class. It describes an object which can be used as a 
forward read-only iterator for sequence of type RMap.
@internalComponent
*/
template <class KEY, class DATA> 
class TMapIterator
	{
public:
	TMapIterator(const RMap<KEY, DATA>& aMap);
	void Reset();
	TBool Next(TPair<KEY, DATA>& aPair) const;
private:
	const RMap<KEY, DATA>&	iMap;
	mutable TInt			iIndex;
	};

/**
RMap template class describes an object that controls an ordered sequence of entries.
Each entry has a key of type KEY and an associated with it data of type DATA.
No entries with duplicated keys are allowed.
The algorithm for determining the order of two entries is provided by a TLinearOrder
object, supplied by the client of RMap during RMap instance construction.
@internalComponent
*/
template <class KEY, class DATA> 
class RMap
	{
	friend class TMapIterator<KEY, DATA>;
public:
	RMap(const TLinearOrder< TPair<KEY, DATA> >& aOrder);
	void Close();
	TInt Insert(const KEY& aKey, const DATA& aData);
	void Remove(const KEY& aKey);
	const DATA& operator[](const KEY& aKey) const;
	TInt Find(const KEY& aKey, DATA& aData) const;
	TInt Count() const;

private:
	TLinearOrder< TPair<KEY, DATA> >	iOrder;
	RArray< TPair<KEY, DATA> >			iCol;

	};

#include "D32Map.inl"

#endif//__D32MAP_H__
