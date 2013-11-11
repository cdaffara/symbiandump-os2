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
// DBMS object cache
// 
//

#include "UD_STD.H"
#include "D32CACHE.H"
#include <e32svr.h>

NONSHARABLE_CLASS(RDbCache::CCache) : private CBase
	{
public:
	static CCache* OpenL( TInt aSize, TBool aUseTimer );
	void Close();
//
	void Flush();
	void Hold( CBase* aObject, TUint aMicroSeconds );
	void Release ( const CBase& aObject );
private:
	struct TEntry
		{
		TEntry* iNext;
		TInt iDelta;
		CBase* iObject;
		};
	enum { ETimerPriority = -10 };
	enum { ETimerPeriod = 0x100000 };	// ~1.0s
private:
	static inline TInt TlsHandle();
	CCache( TInt aSize );
	~CCache();
//
	static CCache* NewL( TInt aSize, TBool aUseTimer );
	inline void Open();
	void Expire( TInt aElapsedTime );
	void Remove( TEntry*& aRef );
	void ExpireFirst();
	static void DoFlush( TAny* aPtr );
private:
	TInt iRef;
	CPeriodic* iTimer;
	TTimeIntervalMicroSeconds32 iTickPeriod;
	TUint iZeroTime;
	TEntry* iCache;
	TEntry* iFree;
	TEntry iEntries[1];		// or maybe more
	};

// Class CDbObjectCache

inline TInt RDbCache::CCache::TlsHandle()
// use the address of a static function for the handle
	{ return TInt( NewL ); }


RDbCache::CCache::CCache( TInt aSize )
//
// Initialise the free entry list
//
	{
	TEntry* entry = iEntries;
	while ( --aSize != 0 )
		{
		entry[1].iNext = entry;
		++entry;
		}
	iFree = entry;
	}

RDbCache::CCache::~CCache()
	{
	__ASSERT( iRef < 0 );
// empty the cache (destory the items now)
	Expire( KMaxTInt );
	__ASSERT( iCache == 0 );
	delete iTimer;
	UserSvr::DllFreeTls( TlsHandle() );
	}

RDbCache::CCache* RDbCache::CCache::NewL( TInt aSize, TBool aUseTimer )
//
// Construct a cache with aSize slots and one referee
//
	{
	CCache* cache = new( ELeave, sizeof( TEntry ) * ( aSize - 1 ) ) CCache( aSize );	// get the extra size for the cache entries, leaves on error
	CleanupClosePushL( *cache );
	User::LeaveIfError( UserHal::TickPeriod( cache->iTickPeriod ) );
	User::LeaveIfError( UserSvr::DllSetTls( TlsHandle(), cache ) );
	if (aUseTimer)
		cache->iTimer = CPeriodic::NewL( ETimerPriority );
	CleanupStack::Pop();
	return cache;
	}

inline void RDbCache::CCache::Open()
// add a referee
	{ ++iRef; }

void RDbCache::CCache::Close()
//
// remove a referee and delete as required
//
	{
	__ASSERT( iRef >= 0 );
	if ( --iRef < 0 )
		delete this;
	}

RDbCache::CCache* RDbCache::CCache::OpenL( TInt aSize, TBool aUseTimer )
//
// Grab a reference to the cache, constructing it if required
//
	{
	CCache* cache = ( CCache* )UserSvr::DllTls( TlsHandle() );
	if (!cache)
		return NewL( aSize, aUseTimer );
	cache->Open();
	return cache;
	}

void RDbCache::CCache::Hold( CBase* aObject, TUint aMicroSeconds )
//
// Hold aObject in the cache or destroy it
//
	{
	Flush();		// Destroy expired entries and re-assess Zero-time
	TInt ticks = aMicroSeconds / TUint( iTickPeriod.Int() );
	TEntry* entry = iFree;
	if ( entry == 0 )
		{	// no free slots: check the first cache entry
		__ASSERT( iCache );
		if ( iCache->iDelta > ticks )
			{				// aObject expires first
			delete aObject;
			return;
			}
		ExpireFirst();		// remove the first entry and use it
		entry = iFree;
		}
	iFree = entry->iNext;	// move the free list pointer to the next entry
	//
	// find the insertion point in the cache delta-list
	TEntry** pcache = &iCache;
	TEntry* cache;
	for ( ; ; )
		{
		__ASSERT( ticks >= 0 );
		cache = *pcache;
		if ( !cache )
			break;				// add to end
		TInt t = ticks - cache->iDelta;
		if ( t < 0 )
			{					// add to the list here
			cache->iDelta = -t;	// reduce the following delta
			break;
			}
		ticks = t;				// reduce the entry delta
		pcache = &cache->iNext;
		}
	*pcache = entry;				// set up the entry
	entry->iDelta = ticks;
	entry->iNext = cache;
	entry->iObject = aObject;
	// kick the timer if we need to
	if ( iTimer && !iTimer->IsActive() )
		iTimer->Start( ETimerPeriod, ETimerPeriod, TCallBack( ( TInt (*)(TAny*) )DoFlush, this ) );
	}

void RDbCache::CCache::Remove( RDbCache::CCache::TEntry*& aRef )
//
// Remove the entry at aRef from the cache
//
	{
	TEntry& entry = *aRef;
	TEntry* next = entry.iNext;
	entry.iNext = iFree;
	iFree = &entry;
	aRef = next;
	if ( next )
		next->iDelta += entry.iDelta;
	else if ( iTimer )	// the cache is now empty, so stop the timer if we have one
		iTimer->Cancel();
	}

void RDbCache::CCache::ExpireFirst()
//
// Expire the first entry in the cache
//
	{
	__ASSERT( iCache != 0 );
	// the ordering here is important. Removing the entry first allows the
	// object d'tor to call Release() without causing re-entrancy problems.
	CBase* object = iCache->iObject;
	Remove( iCache );
	delete object;
	}

void RDbCache::CCache::Release( const CBase& aObject )
//
// Remove the cache entry for aObject, if it is in the cache
//
	{
	TEntry** pcache = &iCache;
	for ( ; ; )
		{
		TEntry* entry = *pcache;
		if ( !entry )
			return;
		if ( entry->iObject == &aObject )
			{
			Remove( *pcache );
			return;
			}
		pcache = &entry->iNext;
		}
	}

void RDbCache::CCache::Expire( TInt aElapsedTime )
//
// Destroy entries which expire with aElapsedTime
//
	{
	__ASSERT( aElapsedTime > 0 );
	if ( iCache && ( iCache->iDelta -= aElapsedTime ) < 0 )
		{
		Open();		// This allows the cache to be owned by an object in the cache
		do ExpireFirst();
			while ( iCache && iCache->iDelta < 0 );
		Close();	// The cache may be destroyed now
		}
	}

void RDbCache::CCache::Flush()
//
// Check the execution clock and destroy any expired entries
//
// Care has to be taken to handle the 32-bit wraparound of the tick-count
// e.g. iZeroTime = 0xffffffffu, now = 0
//
	{
	TUint now = User::TickCount();
	TUint elapsed = now - iZeroTime;
	iZeroTime = now;
	if ( elapsed )
		Expire( elapsed <= TUint( KMaxTInt ) ? elapsed : TUint( KMaxTInt ) );
	}

void RDbCache::CCache::DoFlush( TAny* aPtr )
//
// Callback for the timer
//
	{
	static_cast<CCache*>( aPtr )->Flush();
	}


// Class RDbCache

TInt RDbCache::Open( TInt aSize, TBool aUseTimer )
//
// Get a handle on the cache
//
	{
	__ASSERT( aSize > 0 );
	TRAPD( r, iCache = CCache::OpenL( aSize, aUseTimer ) );
	return r;
	}

void RDbCache::Close()
//
// Close this handle on the cache
//
	{
	CCache* cache = iCache;
	if ( cache )
		{
		iCache = 0;
		cache->Close();
		}
	}

void RDbCache::Hold( CBase* aObject, TUint aMicroSeconds )
//
// Hold aObject on the cache, if open
// We are now responsible for deleting the object
//
	{
	if ( iCache )
		iCache->Hold( aObject, aMicroSeconds );
	else
		delete aObject;	// no cache available
	}

void RDbCache::Release( const CBase& aObject ) const
//
// Retrieve aObject from the cache
//
	{
	if ( iCache )
		iCache->Release( aObject );
	}

void RDbCache::Flush()
//
// Destroy any cached objects which have expired
//
	{
	if ( iCache )
		iCache->Flush();
	}



