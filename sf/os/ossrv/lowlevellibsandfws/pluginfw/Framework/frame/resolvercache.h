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

#ifndef __RESOLVERCACHE_H__
#define __RESOLVERCACHE_H__

#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "callback.h"

class CResolver;
class CRegistryData;
class CRegistryResolveTransaction;

/** class TCounterTicks keep track of time an entry will remain in cache. */
class TCounterTicks
	{
public:
	inline TCounterTicks(TUint aTicks = 0);
	inline TUint ElapsedSinceThis(TUint aEndTick) const;
public:
	TUint iTicks;
	};

/** Bit mask indicating special conditions of cache entries */
enum TCacheEntryFlags
	{
	EEntryFlagsNone 		 = 0,
	EEntryIsOnReadWriteDrive = 0x1
	};

/** An entry in ECOM's custom resolver cache */
class RResolverCacheEntry
	{
public:
	RResolverCacheEntry();
	RResolverCacheEntry(const TUid aResolverUid,
						RLibrary aLib,
						TProxyNewLPtr	aNewL,
						TUint32 aFlags);
	void Close();

	static TInt CompareUid(const RResolverCacheEntry& aEntry1, const RResolverCacheEntry& aEntry2);
	TBool ThisIsOlder(const RResolverCacheEntry& aOther, TUint aCurrTick) const;

public:
	/** Timestamp, when the resolver is last accessed */
	TCounterTicks iLastUse;

	/** An extension of iLastUse. Serves as tie breaker when 2 entries
	 have the same timestamp. */
	TInt iLruRank;

	/** TUid of the custom resolver */
	TUid  iResolverUid;

	/** Function ptr to instantiate the custom resolver */
	TProxyNewLPtr iNewLFuncPtr;

	/** RLibraray object holding a handle on the resolver DLL */
	RLibrary iLibrary;

	/** Special conditions about the cache entry. */
	TUint32 iFlags;
	};

/** Handles caching custom resolvers */
class CCustomResolverCache : public CTimer
	{
public:
	static CCustomResolverCache* NewL(TUint32 aCacheSize, TUint32 aCacheTimeout);
	virtual ~CCustomResolverCache();
	TInt CacheResolver(const TUid aResolverUid,
						RLibrary aLib,
						TProxyNewLPtr aNewL,
						TUint32 aFlags);
	TBool CacheLookup(const TUid aResolverUid, TProxyNewLPtr& aNewLFuncPtr);
	TBool CachingEnabled() const; 
	TBool Remove(const TUid aResolverUid);
	void RemoveItemsWithFlags(TUint32 aMask);

private:
	CCustomResolverCache(TUint32 aCacheSize);
	void ConstructL(TUint32 aCacheTimeout);
	void RunL(); // implement the CActive pure virtual
	TInt FindResolver(const TUid aResolverUid) const;
	void Remove(TInt aIndex);
	void EvictLeastRecentlyUsed();
	void SetLastUseTime(RResolverCacheEntry& aEntry);

private:
	/** list of cached resolvers */
	RArray<RResolverCacheEntry> iResolvers;

	/** max. number of entries allowed in iResolvers */
	TUint32 iMaxCacheSize;

	/** Store the system tick period as member data to avoid
	having to invoke HAL::Get() repeatedly. */
	TInt 	iSystemTickPeriod;

	/** cache timeout value. Unit is number of system ticks */
	TUint	iEntryTimeToLive;

	/** Keep track of youngest timestamp of cache entries. Use to
	detect if two entries have same timestamp. */
	TUint	iMostRecentTimestamp;

#ifdef __ECOMSERVER_TESTING__
	// For unit testing purpose. The test harness installs a callback
	// here to detect when the cache timer fires. 
	TCallBackWithArg iTimerExpireCB;
#endif

	// the test bed state accessor
	friend class CCustomResolverCacheTest;
	};

#endif
