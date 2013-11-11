// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// SqlSvrObjContainer.inl
// Should not be a template class!
// Constructs an empty container.
// 
//

template <class T> 
RDbObjContainer<T>::RDbObjContainer() :
	iEntries(NULL),
	iSize(0),
	iFree(0)
	{
	}

/**
Destroys the container and its content.
*/
template <class T> 
void RDbObjContainer<T>::Close()
	{
	if(iEntries)
		{
		for(/*RDbObjContainer<T>::*/TEntry* entry=iEntries;entry<(iEntries+iSize);++entry)
			{
			delete entry->iObj;
			}
		User::Free(iEntries);
		iEntries = NULL;
		}
	iFree = iSize = 0;
	}

/**
Ensures that the next attempt for adding a new object to the container won't fail because there 
is no memory.
(In other words - ensures that the container has at least one empty slot)

@leave KErrNoMemory, an out of memory condition has occured;

@panic SqlDb 7 In _DEBUG mode. Internal logic error.
*/
template <class T> 
void RDbObjContainer<T>::AllocL()
	{
	__ASSERT_DEBUG(iFree <= iSize, __SQLPANIC(ESqlPanicInternalError));
	if(iFree == iSize)
		{
		if(iSize >= KMaxSize)
			{
			__SQLLEAVE(KErrNoMemory);
			}
		TInt size = iSize + RDbObjContainer<T>::KGranularity;
		iEntries = (TEntry*)User::ReAllocL(iEntries, size * sizeof(TEntry));
		iSize = size;
		for(TInt i=iFree;i<size;)
			{
			/*RDbObjContainer<T>::*/TEntry& entry = iEntries[i];
			entry.iObj = NULL;
			entry.iNext = ++i;
			}
		}
	}

/**
The method adds "aObj" pointer to the container.

@param aObj A pointer to the object which will be stored in the container.

@return Handle, uniquely identifying the stored object

@panic SqlDb 4 In _DEBUG mode. aObj parameter is NULL.
@panic SqlDb 7 In _DEBUG mode. Internal logic error.
*/
template <class T> 
TInt RDbObjContainer<T>::Add(T* aObj)
	{
	__ASSERT_DEBUG(aObj != NULL, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iFree <= iSize, __SQLPANIC(ESqlPanicInternalError));
	TInt idx = iFree;
	if(idx < iSize)
		{
		/*RDbObjContainer<T>::*/TEntry& entry = iEntries[idx];
		__ASSERT_DEBUG(!entry.iObj, __SQLPANIC(ESqlPanicInternalError));
		iFree = entry.iNext;
		__ASSERT_DEBUG(iFree <= iSize, __SQLPANIC(ESqlPanicInternalError));
		entry.iObj = aObj;
		return MakeHandle(idx);
		}
	return 0;
	}

/**
@param aIndex Valid container index.

@return Handle, uniquely identifying the object stored at aIndex index.
*/
template <class T> 
TInt RDbObjContainer<T>::MakeHandle(TInt aIndex) const
	{
	return aIndex + 1;
	}

/**
@param aHandle Unique object handle

@return Container index of the object, identified by aHandle parameter.
*/
template <class T> 
TInt RDbObjContainer<T>::MakeIndex(TInt aHandle) const
	{
	return aHandle - 1;
	}

/**
Removes an object from the container.
The removed object will be destroyed.

@param aHandle Unique object handle

@panic SqlDb 7 In _DEBUG mode. Internal logic error or there is no such object in the container.
*/
template <class T> 
void RDbObjContainer<T>::Remove(TInt aHandle)
	{
	__ASSERT_DEBUG(iFree <= iSize, __SQLPANIC(ESqlPanicInternalError));
	if(aHandle > 0)	//It is a handle, sent by the client. It isn't a server's problem if the handle is <= 0.
		{
		TInt idx = MakeIndex(aHandle);
		if(idx < iSize)
			{
			/*RDbObjContainer<T>::*/TEntry& entry = iEntries[idx];
			delete entry.iObj;
			entry.iObj = NULL;
			entry.iNext = iFree;
			iFree = idx;
			}
		}
	__ASSERT_DEBUG(iFree <= iSize, __SQLPANIC(ESqlPanicInternalError));
	}

/**
Looks up for an object in the container.

@param aHandle Unique object handle

@return A pointer to the found object or NULL.
*/
template <class T> 
T* RDbObjContainer<T>::Find(TInt aHandle) const
	{
	/*RDbObjContainer<T>::*/TEntry* entry = NULL;
	if(aHandle > 0)	//It is a handle, sent by the client. It isn't a server's problem if the handle is <= 0.
		{
		TInt idx = MakeIndex(aHandle);
		if(idx < iSize)
			{
			entry = &iEntries[idx];
			}
		}
	return entry != NULL ? entry->iObj : NULL;
	}

/**
Counts the alive objects in the container

@return The object count
*/
template <class T> 
TInt RDbObjContainer<T>::Count() const
	{
	TInt count = 0;;
	const /*RDbObjContainer::*/TEntry* const base = iEntries;
	if(base)
		{
		for(const /*RDbObjContainer::*/TEntry* e=base+iSize;--e>=base;)
			{
			if(e->iObj)
				{
				++count;					
				}
			}
		}
	return count;
	}
