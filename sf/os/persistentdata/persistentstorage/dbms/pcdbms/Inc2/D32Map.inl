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
// RMap template class implementation.
// 
//

/**
 
 Template function that compares two objects of type TPair<KEY, DATA> and
 returns {negative, 0, positive} depending on the comparison result.
 In order the comparison to work, the KEY template parameter has to have
 "-" operation defined - either native or overloaded "-" operator.
 Used by RMap<KEY, DATA> template class.
 Note: This template function works only on objects, which have defined a "-" operation.
 @param aLeft The first TPair<KEY, DATA> to be compared
 @param aRight The second TPair<KEY, DATA> to be compared
 @return An integer value, which is:
 0, if the objects are equal;
 negative number, the first object is bigger than the second object
 positive number, the first object is smaller than the second object
 @internalComponent
*/
template <class KEY, class DATA>
TInt Compare(const TPair<KEY, DATA>& aLeft, const TPair<KEY, DATA>& aRight)
	{
	return aRight.iKey - aLeft.iKey;
	}

/**
@param aKey Key part of TPair object
@param aData Data part of TPair object
*/
template <class KEY, class DATA> 
TPair<KEY, DATA>::TPair(const KEY& aKey, const DATA& aData) :
	iKey(aKey),
	iData(aData)
	{
	}

/**
@param aKey Key part of TPair object
*/
template <class KEY, class DATA> 
TPair<KEY, DATA>::TPair(const KEY& aKey) :
	iKey(aKey)
	{
	}

/**
*/
template <class KEY, class DATA> 
TPair<KEY, DATA>::TPair()
	{
	}

/**
@param aOrder An object of TLinearOrder< TPair<KEY, DATA> type, which will be used when
new elements are inserted into the collection.
*/
template <class KEY, class DATA> 
RMap<KEY, DATA>::RMap(const TLinearOrder< TPair<KEY, DATA> >& aOrder) :
	iOrder(aOrder)
	{
	}

/**
Closes RMap instance and destroy all RMap data.
*/
template <class KEY, class DATA> 
void RMap<KEY, DATA>::Close()
	{
	iCol.Close();
	}

/**
This method will create and insert new (KEY, DATA) pair in the map.
If the operation fails, the method will return some of system wide error codes.
RMap instance does not allow duplicated TPair objects in the collection.
Actually it dependes on the supplied in the constructor aOrder instance.
@param aKey The key part of TPair object, which will be inserted
@param aData The data part of TPair object, which will be inserted
@return KErrNone if successful, otherwise one of the system-wide error codes
*/
template <class KEY, class DATA> 
TInt RMap<KEY, DATA>::Insert(const KEY& aKey, const DATA& aData)
	{
	return iCol.InsertInOrder(TPair<KEY, DATA>(aKey, aData), iOrder);
	}

/**
This method removes an element with aKey key from the map.
If there is no such element in the map, the debug verison of the method will panic.
The destructor of the object, which is about to be removed, is not called.
@param aKey The key of TPair object, which has to be removed from the collection.
*/
template <class KEY, class DATA> 
void RMap<KEY, DATA>::Remove(const KEY& aKey)
	{
	TInt index = iCol.FindInOrder(TPair<KEY, DATA>(aKey), iOrder);
	index != KErrNotFound ? iCol.Remove(index) : __ASSERT(0);
	}

/**
This method performs a search for an element with aKey key in the map.
If such element exists, the method will return a const reference to the element's data.
If not, then the method will panic.
@param aKey The key of TPair object, a reference to which DATA will be returned
@return A const reference to the matching DATA object.
*/
template <class KEY, class DATA> 
const DATA& RMap<KEY, DATA>::operator[](const KEY& aKey) const
	{
	TInt index = iCol.FindInOrder(TPair<KEY, DATA>(aKey), iOrder);
	__ASSERT_ALWAYS(index != KErrNotFound, User::Invariant());
	return iCol[index].iData;
	}

/**
This method performs a search for an element with aKey key in the map.
If such element exists, the method will set the element's data in aData parameter and
return KErrNone.
If not, then the method will return KErrNotFound.
@param aKey The key of TPair object, a reference to which DATA will be returned in aData
            parameter, if found
@param aData An output parameter, it references the location, where the found data will be stored.
@return KErrNone An element with aKey key exists in the map, the data part is stored in aData.
@return KErrNotFound No map element with aKey key found.
*/
template <class KEY, class DATA> 
TInt RMap<KEY, DATA>::Find(const KEY& aKey, DATA& aData) const
	{
	TInt index = iCol.FindInOrder(TPair<KEY, DATA>(aKey), iOrder);
	if(index != KErrNotFound)
		{
		aData = iCol[index].iData;
		}
	return index;
	}

/**
This method returns the number of elements in the map.
@return Map elements count.
*/
template <class KEY, class DATA> 
TInt RMap<KEY, DATA>::Count() const
	{
	return iCol.Count();
	}

/**
Initializes TMapIterator instance.
@param aMap A const reference to the RMap object, which will be iterated.
*/
template <class KEY, class DATA> 
TMapIterator<KEY, DATA>::TMapIterator(const RMap<KEY, DATA>& aMap) :
	iMap(aMap),
	iIndex(0)
	{
	}

/**
Resets TMapIterator iterator for a new scan from the beginning of the controlled map sequence.
*/
template <class KEY, class DATA> 
void TMapIterator<KEY, DATA>::Reset()
	{
	iIndex = 0;
	}

/**
If iterator's current position is not beyond the end of the map,
the iterator will retrieve current map entry into aPair argument, advance iterator position
by 1 and return ETrue. 
Otherwise the iterator will return EFalse.
@param aPair An output parameter, which references the location, where the next RMap element will be stored.
@return ETrue The next RMap element successfully loaded into aPair parameter. This is not the 
              end of RMap collection.
@return EFalse The next RMap element successfully loaded into aPair parameter. This is the 
              end of RMap collection. Do not call Next() anymore.
*/
template <class KEY, class DATA> 
TBool TMapIterator<KEY, DATA>::Next(TPair<KEY, DATA>& aPair) const
	{
	return (iIndex < iMap.iCol.Count()) ? aPair = iMap.iCol[iIndex++], ETrue : EFalse;
	}
