// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <e32def.h>
#include <e32cmn.h>
#include <hal.h>
#include "EComDebug.h"
#include "ImplementationInformation.h"  // CompareTUidValues
#include "callback.h"
#include "resolvercache.h"
#include "EComPatchDataConstantv2.h"

const TInt KCacheQueueGranularity = 4;

//
// TCounterTicks class
//
inline TCounterTicks::TCounterTicks(TUint aTicks)
	: iTicks(aTicks)
	{
	}

/** This substraction calculate elapsed ticks: aEndTick - this */
inline
TUint TCounterTicks::ElapsedSinceThis(TUint aEndTick) const
	{
	TUint diff;
	if (aEndTick < iTicks) // wrap around occurred
		{
		diff = KMaxTUint - iTicks + aEndTick + 1;
		}
	else
		{
		diff = aEndTick - iTicks;
		}
	return diff;
	}

/** default constructor which initializes all member data to 0. */
RResolverCacheEntry::RResolverCacheEntry()
	: iLastUse( TCounterTicks(0) ), iLruRank(0), iResolverUid(KNullUid),
	iNewLFuncPtr(NULL), iFlags(EEntryFlagsNone)
	{
	iLibrary.SetHandle(KNullHandle);
	}

/** construct RResolverCacheEntry with specific data
@param aResolverUid TUid of the resolver.
@param aLib RLibrary with handle open on the resolver DLL.
@param aNewL Function ptr to instantiate the resolver.
@param aFlags Special conditions about the cache entry.
*/
RResolverCacheEntry::RResolverCacheEntry(const TUid aResolverUid,
										RLibrary aLib,
										TProxyNewLPtr aNewL,
										TUint32 aFlags)
	: iLastUse( TCounterTicks(0) ), iLruRank(0), iResolverUid(aResolverUid),
	iNewLFuncPtr(aNewL), iFlags(aFlags)
	{
	iLibrary.SetHandle(aLib.Handle());
	}

/** unallocate resources owned by the RResolverCacheEntry object */
void RResolverCacheEntry::Close()
	{
	iLibrary.Close();
	}

/** This method compares two RResolverCacheEntry objects.
@param aEntry1 first RResolverCacheEntry object.
@param aEntry2 second RResolverCacheEntry object.
@return 0 means the two objects are equal.
        positive value means first object is greater than the second.
		negative vlaue means second object is greater than the first.
*/
TInt RResolverCacheEntry::CompareUid(const RResolverCacheEntry& aEntry1,
								     const RResolverCacheEntry& aEntry2)
	{
	return CompareTUidValues(aEntry1.iResolverUid.iUid,
							 aEntry2.iResolverUid.iUid);
	}

/** Compare the age of two cache entries.
@param aOther The RResolverCacheEntry to compare with.
@param aCurrTick The current system tick. It simplifies handling
	of system tick wrap around to zero.
@return ETrue means "this" object is older than aOther.
	EFalse means aOther is older than "this".
*/
TBool RResolverCacheEntry::ThisIsOlder(const RResolverCacheEntry& aOther,
										TUint aCurrTick) const
	{
	if (iLastUse.iTicks == aOther.iLastUse.iTicks)
		{
		return iLruRank < aOther.iLruRank;
		}

	// Because of counter wrap around, it is not safe to directly
	// compare the two ticks. Drag in the current system tick.
	return iLastUse.ElapsedSinceThis(aCurrTick) >
			aOther.iLastUse.ElapsedSinceThis(aCurrTick);
	}

//=================================
// CCustomResolverCache class
//=================================

/** CCustomResolverCache constructor
@param aCacheSize Maximum number of entries allowed to cache.
*/
CCustomResolverCache::CCustomResolverCache(TUint32 aCacheSize)
	: CTimer(CActive::EPriorityStandard),
	iResolvers(KCacheQueueGranularity), 
	iMaxCacheSize(aCacheSize)
	{
	}

/** static factory method to instantiate CCustomResolverCache
@param aCacheSize Maximum number of entries allowed to cache.
@param aCacheTimeout Cache timeout in microseconds.
@leave Any of the system wide error codes.
*/
CCustomResolverCache* CCustomResolverCache::NewL(TUint32 aCacheSize, TUint32 aCacheTimeout)
	{
	CCustomResolverCache* self = new (ELeave) CCustomResolverCache(aCacheSize);
	CleanupStack::PushL(self);
	self->ConstructL(aCacheTimeout);
	CleanupStack::Pop(self);
	return self;
	}

/** Standard two phase construction to complete construction of
the CCustomResolverCache object.
@param aCacheTimeout Cache timeout in microseconds.
@leave Any of the system wide error codes.
*/
void CCustomResolverCache::ConstructL(TUint32 aCacheTimeout)
	{
	User::LeaveIfError(HAL::Get(HALData::ESystemTickPeriod,
								iSystemTickPeriod));
	iEntryTimeToLive = (aCacheTimeout + iSystemTickPeriod - 1) / iSystemTickPeriod;

	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

/** CCustomResolverCache destructor */
CCustomResolverCache::~CCustomResolverCache()
	{
	Cancel();
	for (TInt i = iResolvers.Count() - 1; i >= 0; i--)
		{
		iResolvers[i].Close();
		}
	iResolvers.Reset();
	}

/** Implement the CActive RunL pure virtual */
void CCustomResolverCache::RunL()
	{
	TUint currTickCount = User::TickCount();
	TUint lruAge = 0;

	for (TInt i = iResolvers.Count() - 1; i >= 0; i--)
		{
		RResolverCacheEntry& resolverEntry = iResolvers[i];
		TUint age = resolverEntry.iLastUse.ElapsedSinceThis(currTickCount);
		if (age >= iEntryTimeToLive)
			{
			resolverEntry.Close();
			iResolvers.Remove(i);
			}
		else if (age > lruAge)
			{
			lruAge = age;
			}
		}

	if (iResolvers.Count() > 0)
		{
		After( iSystemTickPeriod * (iEntryTimeToLive - lruAge + 1) );
		}

#ifdef __ECOMSERVER_TESTING__
	// In unit testing notify test bed that cache timer has fired.
	iTimerExpireCB.CallBack(0, NULL);
#endif
	}

/** Search of a resolver UID in cache
@param aResolverUid the resolver to search for.
@return If find is successful, index of the resolver in RArray of cached resolvers.
		Return KErrNotFound if resolver is not in cache.
*/
TInt CCustomResolverCache::FindResolver(const TUid aResolverUid) const
	{
	RResolverCacheEntry trgt;
	trgt.iResolverUid = aResolverUid;
	TLinearOrder<RResolverCacheEntry> comparator(RResolverCacheEntry::CompareUid);
	return iResolvers.FindInOrder(trgt, comparator);
	}

/** Add a resolver library to cache 
@param aResolverUid Implementation UID of the resolver.
@param aLib	The RLibrary object which has the resolver loaded. The handle
	of the RLibrary is owned by the cache if call is successful.
@param aNewL value for the iNewLFuncPtr member data of RResolverCacheEntry.
@param aFlags value for the iFlags member data of RResolverCacheEntry.
@return KErrNone if the data is added to cache.
	KErrNoMemory if fail to insert the data in RArray.
*/
TInt CCustomResolverCache::CacheResolver(const TUid aResolverUid,
										  RLibrary aLib,
										  TProxyNewLPtr aNewL,
										  TUint32 aFlags)
	{
	if (iResolvers.Count() == iMaxCacheSize)
		{
		EvictLeastRecentlyUsed();
		}

	RResolverCacheEntry entry(aResolverUid, aLib, aNewL, aFlags);
	SetLastUseTime(entry);

#ifdef ECOM_TRACE
	__ECOM_TRACE2("ECOM: adding custom resolver 0x%X to cache. New queue size will be %d.\n", aResolverUid.iUid, 1 + iResolvers.Count());
#endif

	TLinearOrder<RResolverCacheEntry> comparator(RResolverCacheEntry::CompareUid);
	TInt err = iResolvers.InsertInOrder(entry, comparator);

	// if cache was empty before need to start timer
	if (err == KErrNone && ! IsActive())
		{
		After( iSystemTickPeriod * (iEntryTimeToLive + 1) );
		}
	return err;
	}

/** Check if both queue size and cache timeout are non zero  */
TBool CCustomResolverCache::CachingEnabled() const
	{
	return (iMaxCacheSize && iEntryTimeToLive);
	}

/** Remove a cached entry
@param aIndex position of entry in the array.
*/
void CCustomResolverCache::Remove(TInt aIndex)
	{
	iResolvers[aIndex].Close();
	iResolvers.Remove(aIndex);
	}

/** Search for a resolverUID. If found, return the NewL pointer and
update time to live of the entry.
@param aResolverUid the resolver to lookup
@param aNewLFuncPtr output parameter. If lookup successful it has
	the function pointer to instantiate the resolver.
@return True if resolver is in cache. False otherwise.
@post If cache hit, the timestamp of the entry is updated.
*/
TBool CCustomResolverCache::CacheLookup(const TUid aResolverUid,
										TProxyNewLPtr& aNewLFuncPtr)
	{
	TInt i = FindResolver(aResolverUid);
	if (i >= 0)
		{
		aNewLFuncPtr = iResolvers[i].iNewLFuncPtr;
		SetLastUseTime(iResolvers[i]);
		}
	return (i >= 0);
	}

/** Remove a resolver from cache.
@param aResolverUid Identify the resolver to remove.
@return ETrue if aResolverUid is found in cache. EFalse means the
	resolver is not in cache.
*/
TBool CCustomResolverCache::Remove(const TUid aResolverUid)
	{
	TInt i = FindResolver(aResolverUid);
	if (i >= 0)
		{
		Remove(i);
		}
	return (i >= 0);
	}

/** Remove cached entries with flags set.
@param aMask If an entry has any of the bits in aMask set, it is removed.
*/
void CCustomResolverCache::RemoveItemsWithFlags(TUint32 aMask)
	{
	for (TInt i = iResolvers.Count() - 1; i >= 0; i--)
		{
		if (iResolvers[i].iFlags & aMask)
			{
			Remove(i);
			}
		}
	}

/** evict the least recently used entry in cache
*/
void CCustomResolverCache::EvictLeastRecentlyUsed()
	{
	TUint curr = User::TickCount();
	TInt index = 0; // set to first entry in cache.

	for (TInt i = 1; i < iResolvers.Count(); i++)
		{
		RResolverCacheEntry& resolverEntry = iResolvers[i];
		if (resolverEntry.ThisIsOlder(iResolvers[index], curr))
			{
			index = i;
			}
		}

	Remove(index);
	}

/** Set the iLastUse field and iLruRank field of the entry.
The iLruRank field serves as tie breaker when two entries are
added within the same tick period.
@param aEntry The entry to update.
*/
void CCustomResolverCache::SetLastUseTime(RResolverCacheEntry& aEntry)
	{
	TUint curr = User::TickCount();
	aEntry.iLastUse.iTicks = curr;
	aEntry.iLruRank = 0;

	if (curr == iMostRecentTimestamp)
		{
		// There are entries with same timestamp. So have to step
		// through each cache entry to resolve senority.
		for (TInt i = 0; i < iResolvers.Count(); i++)
			{
			RResolverCacheEntry& another = iResolvers[i];
			if (another.iLastUse.iTicks == curr &&
				another.iResolverUid != aEntry.iResolverUid &&
				another.iLruRank >= aEntry.iLruRank)
				{
				aEntry.iLruRank = another.iLruRank + 1;
				}
			}
		}
	else
		{
		iMostRecentTimestamp = curr;
		}
	}

