/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "MemSpyDriverObjectIx.h"

// System includes
#include <kern_priv.h>
//#include <dobject.h>

//const TInt KObjectIxGranularity=8;
const TInt KObjectIndexMask=0x7fff;
//const TInt KObjectMaxIndex=0x7fff;
const TInt KObjectInstanceShift=16;
const TInt KObjectInstanceMask=0x3fff;
//const TInt KObjectIxMaxHandles=0x8000;

inline TInt index(TInt aHandle)
	{return(aHandle&KObjectIndexMask);}
inline TInt instance(TInt aHandle)
	{return((aHandle>>KObjectInstanceShift)&KObjectInstanceMask);}
inline TInt instanceLimit(TInt& aCount)
	{return ((aCount&KObjectInstanceMask)==0) ? ((++aCount)&KObjectInstanceMask) : aCount&KObjectInstanceMask;}
inline TInt makeHandle(TInt aIndex, TInt aInstance)
	{return((TInt)((aInstance<<KObjectInstanceShift)|aIndex));}

enum TDObjectPanic
	{
	EObjObjectStillReferenced,
	EObjNegativeAccessCount,
	EObjRemoveObjectNotFound,
	EObjRemoveContainerNotFound,
	EObjRemoveBadHandle,
	EObjFindBadHandle,
	EObjFindIndexOutOfRange,
	EDObjectConDestroyed,
	EArrayIndexOutOfRange,
	EObjInconsistent,
	};

inline void Panic(TDObjectPanic aPanic)
	{ Kern::Fault("DOBJECT",aPanic); }
	

TBool MemSpyObjectIx::Find(DObject* aObj)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	// I don't like the implementation of At() that was here before, it wasn't safe at all without HandleMutex. So I'm replacing it with a simpler
	// version based on operator[] that only does what we need and does it safely.

	TBool found = EFalse;
	MemSpyObjectIx_HandleLookupLock();
	const TInt count = Count();
	for (TInt i = 0; i < count; i++)
		{
		if ((*this)[i] == aObj)
			{
			found = ETrue;
			break;
			}
		}
	MemSpyObjectIx_HandleLookupUnlock();
	return found;
	}

#if MCL_ROBJECTIX_DUPLICATION

#define asserta(x)	do { if (!(x)) { __crash(); } } while(0)

RMemSpyObjectIx::RMemSpyObjectIx()
  : iRWL(TSpinLock::EOrderGenericIrqLow3)
    {
    }


/*
void RMemSpyObjectIx::Wait()
	{
	Kern::MutexWait(*HandleMutex);
	} // RObjectIx::Wait


void RMemSpyObjectIx::Signal()
	{
	Kern::MutexSignal(*HandleMutex);
	} // RObjectIx::Signal
*/

DObject* RMemSpyObjectIx::operator[](TInt aIndex)
	{
	DObject* obj = 0;
	AcquireReadLock();
	asserta(TUint(aIndex)<TUint(iCount));
	SSlot* slot = iSlots + aIndex;
	obj = Occupant(slot);
	ReleaseReadLock();
	return obj;
	} // RObjectIx::operator[]


#elif MCL_DOBJECTIX_DUPLICATION

/*
void DMemSpyObjectIx::Wait( DMemSpyObjectIx* aObjectIndex )
	{
//	Kern::MutexWait(*aObjectIndex->HandleMutex);
	}

void DMemSpyObjectIx::Signal( DMemSpyObjectIx* aObjectIndex )
	{
//	Kern::MutexSignal(*aObjectIndex->HandleMutex);
	}
*/

/** Counts the number of times an object appears in this index.

	@param	aObject	Object whose occurrences are to be counted.

	@return	Number of times aObject appears in the index.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
	@pre	DObject::HandleMutex held
 */
TInt DMemSpyObjectIx::Count(DObject* aObject)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	TInt c=0;
	if (iCount)
		{
		SDObjectIxRec* pS=iObjects;
		SDObjectIxRec* pE=pS+iCount;
		do
			{
			if (pS->obj==aObject)
				c++;
			} while (++pS<pE);
		}
	return c;
	}


/**	Looks up an object in the index by handle.
	
	The object must be of a specified type (specified by container ID)

	@param	aHandle		Handle to look up.
	@param	aUniqueID	Unique ID (container ID) that object should have.
	
	@return	Pointer to object or NULL if handle invalid.

	@pre	Call in a thread context.
	@pre    System lock must be held.
 */
DObject* DMemSpyObjectIx::At(TInt aHandle, TInt aUniqueID)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	TInt i=index(aHandle);
	if (i>=iCount)
		return(NULL);
	SDObjectIxRec *pS=iObjects+i;
	if (pS->str.instance!=instance(aHandle) || pS->str.uniqueID!=aUniqueID)
		return(NULL);
	return(pS->obj);
	}


/**	Looks up an object in the index by handle.

	The object may be of any type.

	@param	aHandle		Handle to look up.
	
	@return	Pointer to object or NULL if handle invalid.

	@pre	Call in a thread context.
	@pre    System lock must be held.
 */
DObject* DMemSpyObjectIx::At(TInt aHandle)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	TInt i=index(aHandle);
	if (i>=iCount)
		return NULL;
	SDObjectIxRec *pS=iObjects+i;
	if (pS->str.instance!=instance(aHandle))
		return NULL;
	return pS->obj;
	}

/** Finds the object at a specific position in the index array.

	@param	aIndex	Index into array.
	
	@return	Pointer to the object at that position (could be NULL).

	@pre	Call in a thread context. 
    @pre    System lock must be held.
 */
DObject* DMemSpyObjectIx::operator[](TInt aIndex)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	__ASSERT_ALWAYS(aIndex>=0 && aIndex<iCount,Panic(EArrayIndexOutOfRange));
	return iObjects[aIndex].obj;
	}

#endif

