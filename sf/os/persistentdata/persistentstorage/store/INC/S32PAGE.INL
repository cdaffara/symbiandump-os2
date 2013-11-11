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

// Class TPageRef
inline TPageRef::TPageRef(TUint32 aValue)
	: iVal(aValue)
/** Constructor that initialises the object to a page reference value.

@param aValue Page reference value */
	{}
inline TBool TPageRef::operator==(TPageRef aRef) const
/** Equality operator.

@param aRef Page reference against which to compare
@return True if aRef refers to the same page as the object, otherwise false. */
	{return iVal==aRef.iVal;}
inline TBool TPageRef::operator!=(TPageRef aRef) const
/** Inequality operator.

@param aRef Page reference against which to compare
@return True if aRef does not refer to the same page as the object, otherwise 
false. */
	{return iVal!=aRef.iVal;}
inline void TPageRef::ExternalizeL(RWriteStream& aStream) const
/** Externalises a TPageRef object to a write stream.

@param aStream Stream to which the object should be externalised. */
	{aStream<<iVal;}
inline void TPageRef::InternalizeL(RReadStream& aStream)
/** Internalises a TPageRef object from a read stream.

@param aStream Stream from which the object should be internalised. */
	{aStream>>iVal;}
inline TUint32 TPageRef::Value() const
/** Gets the page reference value.

@return Page reference value */
	{return iVal;}

// Class MPagePool
inline void MPagePool::Pop()
/** Do a pop from the cleanup stack. */
	{CleanupStack::Pop();}

// Class CPageCache::TStats
#if defined(__PAGE_CACHE_STATS)
inline TInt CPageCache::TStats::Requests() const
	{return iRequests;}
inline TInt CPageCache::TStats::Hits() const
	{return iRequests-iMisses;}
inline TInt CPageCache::TStats::Misses() const
	{return iMisses;}
inline void CPageCache::TStats::Reset()
	{iRequests=0;iMisses=0;}
#endif

// Class CPageCache
#if defined(__PAGE_CACHE_STATS)
inline CPageCache::TStats& CPageCache::Stats()
	{return iStats;}
inline const CPageCache::TStats& CPageCache::Stats() const
	{return iStats;}
#endif

// Class TCachePagePool
inline void TCachePagePool::Set(CPageCache& aCache)
/** Sets the page cache for the pool.

@param aCache The page cache for the pool. */
	{iCache=&aCache;}
inline TCachePagePool::TCachePagePool()
	: iCache(NULL)
	{}
inline TCachePagePool::TCachePagePool(CPageCache& aCache)
	: iCache(&aCache)
	{}

