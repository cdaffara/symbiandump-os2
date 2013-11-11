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
// RSqlMap template class implementation.
// ////////////////////                 TSqlPair implementation              //////////////////////////////////
// 
//

/**
 
 Initializes TSqlPair data members using the supplied parameter values.
 
 @param aKey Key part of TSqlPair object
 @param aData Data part of TSqlPair object
*/
template <class KEY, class DATA, class REFCNTR> 
TSqlPair<KEY, DATA, REFCNTR>::TSqlPair(const KEY& aKey, const DATA& aData) :
	iKey(aKey),
	iData(aData)
	{
	}

/**
Initializes TSqlPair data members using the supplied parameter value.

@param aKey Key part of TSqlPair object
*/
template <class KEY, class DATA, class REFCNTR> 
TSqlPair<KEY, DATA, REFCNTR>::TSqlPair(const KEY& aKey) :
	iKey(aKey)
	{
	}

/**
Initializes TSqlPair data members with their default values.
*/
template <class KEY, class DATA, class REFCNTR> 
TSqlPair<KEY, DATA, REFCNTR>::TSqlPair()
	{
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////                 RSqlMap implementation              ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@param aOrder An object of TLinearOrder< TSqlPair<KEY, DATA, REFCNTR> > type, which will be used when
new elements are inserted into the collection (to determine their order).
@param aDestructor An object of TSqlPairDestructor<KEY, DATA> type which will be used for the destruction of
KEY and DATA TSqlPair data members.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>::RSqlMap(const TLinearOrder< TSqlPair<KEY, DATA, REFCNTR> >& aOrder, const DESTRUCTOR& aDestructor) :
	iOrder(aOrder),
	iDestructor(aDestructor)
	{
	}

/**
Closes RSqlMap instance and destroys all RSqlMap data.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
void RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>::Close()
	{
	TInt idx = iSet.Count();
	while(--idx >= 0)
		{
		TSqlPair<KEY, DATA, REFCNTR>& pair = iSet[idx];
		iDestructor.Destroy(pair.iKey, pair.iData);
		}
	iSet.Close();
	}

/**
This method will create and insert new (KEY, DATA, REFCNTR) pair in the map.

RSqlMap class maintains a set of reference counted objects.
If an object with aKey key is already in the map, no insertion will occur, the reference counter
of the existing object will be incremented.

@param aKey The key part of TSqlPair object, which will be inserted
@param aData The data part of TSqlPair object, which will be inserted

@return System-wide error code if the insertion fails, reference counter value otherwise.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
TInt RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>::Insert(const KEY& aKey, const DATA& aData)
	{
	TInt idx = iSet.FindInOrder(TSqlPair<KEY, DATA, REFCNTR>(aKey), iOrder);
	if(idx >= 0)
		{
		return iSet[idx].iRefCounter.Increment();
		}
	else
		{
		TInt err = iSet.InsertInOrder(TSqlPair<KEY, DATA, REFCNTR>(aKey, aData), iOrder);
		return err == KErrNone ? 1 : err;
		}
	}

/**
This method removes an element with aKey key from the map.

If there is no such element in the map, the debug verison of the method will panic.

RSqlMap class maintains a set of reference counted objects.
If an object with aKey key is in the map, the reference counter of the object will be decremented.
If the object's reference counter reaches 0 then the object will be destroyed.

@param aKey The key of TSqlPair object, which has to be removed from the collection.

@panic 7 In _DEBUG mode if there is no entry with aKey key in the RSqlMap container.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
void RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>::Remove(const KEY& aKey)
	{
	TInt idx = iSet.FindInOrder(TSqlPair<KEY, DATA, REFCNTR>(aKey), iOrder);
	if(idx != KErrNotFound)
		{
		TSqlPair<KEY, DATA, REFCNTR>& pair = iSet[idx];
		if(pair.iRefCounter.Decrement() == 0)
			{
			iDestructor.Destroy(pair.iKey, pair.iData);
			iSet.Remove(idx);
#ifdef _DEBUG
//This is used prevent the failure of the resource allocation checking in debug mode. 
                if(iSet.Count() == 0)
                    {
                    iSet.Reset();
                    }
#endif  
			}
		return;
		}
	__ASSERT_DEBUG(EFalse, __SQLPANIC(ESqlPanicInternalError));
	}

/**
This method performs a search for an element with aKey key in the map.
If such element exists, the method will return a pointer to it.
If not, then the method will return NULL.

@param aKey The search key.

@return A pointer to the found element or NULL.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
TSqlPair<KEY, DATA, REFCNTR>* RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>::Entry(const KEY& aKey)
	{
	TInt idx = iSet.FindInOrder(TSqlPair<KEY, DATA, REFCNTR>(aKey), iOrder);
	return idx == KErrNotFound ? NULL : &iSet[idx];
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////            TSqlMapIterator implementation              ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes TSqlMapIterator instance.

@param aMap A const reference to the RSqlMap object, which will be iterated.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
TSqlMapIterator<KEY, DATA, REFCNTR, DESTRUCTOR>::TSqlMapIterator(const RSqlMap<KEY, DATA, REFCNTR, DESTRUCTOR>& aMap) :
	iMap(aMap),
	iIndex(0)
	{
	}

/**
If iterator's current position is not beyond the end of the map,
the iterator will retrieve current map entry into aPair argument, advance iterator position
by 1 and return ETrue. 

Otherwise the iterator will return EFalse.

@param aPair An output parameter, which references the location, where the next RSqlMap element will be stored.

@return ETrue The next RSqlMap element successfully loaded into aPair parameter. This is not the 
              end of RSqlMap collection.
@return EFalse The next RSqlMap element successfully loaded into aPair parameter. This is the 
              end of RSqlMap collection. Do not call Next() anymore.
*/
template <class KEY, class DATA, class REFCNTR, class DESTRUCTOR> 
TBool TSqlMapIterator<KEY, DATA, REFCNTR, DESTRUCTOR>::Next(TSqlPair<KEY, DATA, REFCNTR>& aPair) const
	{
	return (iIndex < iMap.iSet.Count()) ? aPair = iMap.iSet[iIndex++], ETrue : EFalse;
	}
