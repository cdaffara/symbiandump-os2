// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "UT_STD.H"

//#pragma message( __FILE__ " : Find helper function may be less code." )

const TInt KStoreMapGranularity=4;

EXPORT_C CStoreMap* CStoreMap::NewL(CStreamStore& aStore)
/** Allocates and constructs a store map associated with the specified store and 
returns a pointer to that store map.

The function leaves if it cannot complete successfully.

@param aStore A reference to the store associated with this store map.
@return A pointer to the new store map object. */
	{
	return new(ELeave) CStoreMap(aStore);
	}

EXPORT_C CStoreMap* CStoreMap::NewLC(CStreamStore& aStore)
/** Allocates and constructs a store map associated with the specified store, returns 
a pointer to that store map and puts the pointer onto the cleanup stack.

The function leaves if it cannot complete successfully.

Placing a pointer to this object onto the cleanup stack allows the object 
and allocated resources to be cleaned up if a subsequent leave occurs.

@param aStore A reference to the store associated with this store map.
@return A pointer to the new store map object. */
	{
	CStoreMap* map=NewL(aStore);
	CleanupStack::PushL(map);
	return map;
	}

EXPORT_C CStoreMap::CStoreMap(CStreamStore& aStore)
	: iArray(KStoreMapGranularity),iFree(KNullStreamId),iStore(&aStore)
	{}

EXPORT_C CStoreMap::~CStoreMap()
/** Frees resources owned by the object, prior to its destruction.

In particular, the destructor deletes all streams whose stream ids are currently 
held within the store map and then empties the store map. */
	{
	ResetAndDestroy();
	}

EXPORT_C void CStoreMap::BindL(TSwizzleC<TAny> aSwizzle,TStreamId anId)
/** Creates an entry in the store map to contain the specified stream id and an 
associated swizzle. On return from this function, the stream id is said to 
be bound to the swizzle.

If the store map already contains an entry with the same swizzle, then the 
new entry replaces the existing entry; in effect, knowledge of the original 
stream id associated with aSwizzle, is lost.

Callers of this function can pass any type of swizzle as the first parameter, 
i.e. any swizzle derived from TSwizzleCBase. The required TSwizzleC<TAny> 
object is constructed from the swizzle passed by the caller.

Note that in most applications, callers pass a general swizzle, i.e. an object 
of type TSwizzle<class T>, where T is the type of object represented by that 
swizzle.

@param aSwizzle Any swizzle derived from TSwizzleCBase.
@param anId The stream id to be bound to the swizzle. */
	{
	__ASSERT_DEBUG(aSwizzle!=NULL||anId==KNullStreamId,Panic(EStoreMapSwizzleMissing));
	if (aSwizzle==NULL)
		return;
//
	__ASSERT_DEBUG(anId!=KNullStreamId,Panic(EStoreMapIdMissing));
	__DEBUG(TSwizzleC<TAny> label=Label(anId));
	__ASSERT_DEBUG(label==NULL||label==aSwizzle,Panic(EStoreMapIntroducingAlias));
	TEntry entry;
	entry.swizzle=aSwizzle;
	TKeyArrayFix key(_FOFF(TEntry,swizzle),ECmpTUint32);
	TInt i;
	if (iArray.FindIsq(entry,key,i)==0)
		{
		iArray[i].id=anId;
		return;
		}
//
	__ASSERT_DEBUG(iFree==KNullStreamId,Panic(EStoreMapFreeIdPresent));
	iFree=entry.id=anId;
	iArray.InsertL(i,entry);
	iFree=KNullStreamId;
	}

EXPORT_C void CStoreMap::Unbind(TSwizzleC<TAny> aSwizzle)
/** Deletes the first entry from the store map corresponding to the specified swizzle.

On return from this function, the swizzle is said to be unbound and knowledge 
of the associated stream id is lost.

Callers of this function can pass any type of swizzle as the parameter, i.e. 
any swizzle derived from TSwizzleCBase. The required TSwizzleC<TAny> object 
is constructed from the swizzle passed by the caller.

Notes:

In most applications, callers pass a general swizzle, i.e. an object of type 
TSwizzle<class T>, where T is the type of object represented by that swizzle.

If no matching entry can be found in the store map, then the store map remains 
unaltered.

@param aSwizzle Any swizzle derived from TSwizzleCBase. */
	{
	TEntry entry;
	entry.swizzle=aSwizzle;
	TKeyArrayFix key(_FOFF(TEntry,swizzle),ECmpTUint32);
	TInt i;
	if (iArray.FindIsq(entry,key,i)==0)
		iArray.Delete(i);
	}

EXPORT_C void CStoreMap::Forget(TStreamId anId)
/** Deletes an entry from the store map. The entry selected is the first one whose 
stream id matches the specified stream id.

On return from this function, the stream id is said to be forgotten and knowledge 
of that stream id is lost.

@param anId The stream id used to identify the entry in the store map to be 
deleted. */
	{
	if (anId==iFree)
		{
		iFree=KNullStreamId;
		return;
		}
//
	TEntry entry;
	entry.id=anId;
	TKeyArrayFix key(_FOFF(TEntry,id),ECmpTUint32);
	TInt i;
	if (iArray.Find(entry,key,i)==0)
		iArray.Delete(i);
	}

EXPORT_C void CStoreMap::Reset()
/** Deletes all entries from the store map.

It is important that this function is called before deleting the store map, 
as the destructor attempts to delete all streams whose stream ids are held 
within the map. */
	{
	iFree=KNullStreamId;
	iArray.Reset();
	}

EXPORT_C void CStoreMap::ResetAndDestroy()
/** Deletes all streams whose ids are held within the store map, and then empties 
the store map. */
	{
	iStore->Delete(iFree);
	for (TIterator iter=Begin(),end=End();iter!=end;++iter)
		{
		const TEntry& entry=*iter;
		iStore->Delete(entry.id);
		}
	Reset();
	}

EXPORT_C TStreamId CStoreMap::At(TSwizzleC<TAny> aSwizzle) const
/** Returns the stream id of the first entry the store map matching the specified 
swizzle.

Callers of this function can pass any type of swizzle as the first parameter, 
i.e. any swizzle derived from TSwizzleCBase. The required TSwizzleC<TAny> 
object is constructed from the swizzle passed by the caller.

@param aSwizzle Any type of swizzle derived from TSwizzleCBase.
@return The required streamid.KNullStreamId, if no matching entry can be found. */
	{
	TEntry entry;
	entry.swizzle=aSwizzle;
	TKeyArrayFix key(_FOFF(TEntry,swizzle),ECmpTUint32);
	TInt i;
	if (iArray.FindIsq(entry,key,i)!=0)
		return KNullStreamId;
//
	return iArray[i].id;
	}

EXPORT_C TSwizzleC<TAny> CStoreMap::Label(TStreamId anId) const
/** Returns the swizzle in the store map assocated with the specified stream id.

@param anId The id of the stream
@return The associated swizzle. If there is no matching swizzle, then this 
is an uninitialized swizzle. */
	{
	TEntry entry;
	entry.id=anId;
	TKeyArrayFix key(_FOFF(TEntry,id),ECmpTUint32);
	TInt i;
	if (iArray.Find(entry,key,i)!=0)
		return NULL;
//
	return iArray[i].swizzle;
	}

EXPORT_C CStoreMap::TIterator CStoreMap::Begin() const
//
// Return the starting iterator for map iteration.
//
	{
	return (iArray.Count()==0)?NULL:&iArray[0];
	}

EXPORT_C CStoreMap::TIterator CStoreMap::End() const
//
// Return the end iterator for map iteration.
//
	{
	return Begin()+iArray.Count();
	}

void CStoreMap::ExternalizeL(const TStreamRef& aRef,RWriteStream& aStream) const
//
// Write the stream id bound to aRef to aStream.
//
	{
	TSwizzleC<TAny> swizzle=aRef;
	TStreamId id=At(swizzle);
	if (id==KNullStreamId&&swizzle!=NULL)
		__LEAVE(KErrNotFound);
//
	aStream<<id;
	}

