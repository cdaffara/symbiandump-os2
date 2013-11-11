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
//

#ifndef CACHEMGR_H
#define CACHEMGR_H

#include <e32base.h>
#include "log.h"
#include "panic.h"
#include "srvdefs.h"
#include <f32file.h>

_LIT(KCacheMgrIniFile, "centrep.ini");

#ifndef DEFAULT_CENTREP_CACHE_SIZE
	/** 
	The default size of the coarse-grained cache, used when there is no .ini setting defined.
	The default value is approximately the size needed to cache everything during device boot-up
	@internalComponent
	*/
	#define DEFAULT_CENTREP_CACHE_SIZE 200000
#else
	#if DEFAULT_CENTREP_CACHE_SIZE <= 0
		#error "DEFAULT_CENTREP_CACHE_SIZE macro value must be greater than 0"
	#endif
#endif

/** 
The default size of the coarse-grained cache. 
@internalComponent
*/
const TInt KDefaultCacheSize=DEFAULT_CENTREP_CACHE_SIZE;

/** 
The special cache size value used for enabling the cache without changing the current cache size value.
@internalComponent
*/
const TInt KInvalidCacheSize	= -1;

#ifndef DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT
	/** 
	The default timeout value of the coarse-grained cache, used when there is no .ini setting defined.
	The default value is approximately the timeout needed to keep everything in cache during device boot-up
	@internalComponent
	*/
	// If this value will need to be changed, please ensure the KTimeoutToSizeConversion is adjusted accordingly.
	#define DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT 120000000
#else
	#if DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT <= 0
		#error "DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT macro value must be greater than 0"
	#endif
#endif

/** 
The default timeout value (in microseconds) for the coarse-grained cache repository eviction. 
@internalComponent
*/
const TInt KDefaultEvictionTimeout=DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT;

/** 
The constant used in forced evicition sorting algorithm for converting microsecond-based timeout 
values to an intermediary unit compatible with byte-based size values.
@internalComponent
*/
const TInt KTimeoutToSizeConversion=DEFAULT_CENTREP_CACHE_EVICTION_TIMEOUT/(100000*24);

/** 
The special timeout value used for enabling the cache without changing the current timeout value.
@internalComponent
*/
const TInt KInvalidEvictionTimeout=-1;


class CSharedRepository;

/**
@internalTechnology
This is the class which manages Coarse-Grained cache operations including delayed-unloading of
repositories and forced eviction of repositories under OOM conditions.
*/
class CRepositoryCacheManager : public CTimer
	{
	friend class TRepositoryCacheManagerTester;
public:
	static CRepositoryCacheManager* NewLC(RFs& aFs);
	~CRepositoryCacheManager();
	void ConstructL(RFs& aFs);
	
	void EnableCache();
	void DisableCache(TBool aFullFlush = EFalse);
	void EnableCache(TInt aDefaultTimeout, TInt aCacheSize);
	
	inline TBool Enabled();

	TBool StartEviction(CSharedRepository*& aRepository);
	
	void RemoveIdleRepository(CSharedRepository* aRepository);	
	void FlushCache(TBool aFullFlush = ETrue);

protected:
	/**
	@internalTechnology
	This is the class/structure which keeps eviction-related cache data 
	*/
	class TRepositoryCacheInfo
		{
	public:
		TTime iCacheTime;
		CSharedRepository* iSharedRepository;
		};

	void RunL();
		
private:
	inline CRepositoryCacheManager(); 

	void Evict(TInt aIdleRepIndex);
	void RescheduleTimer(const TTime& aTimeInUTC);
	
	static TInt ForcedEvictionSortOrder(const TRepositoryCacheInfo &aRepository1, const TRepositoryCacheInfo &aRepository2);
	static TInt TimerEvictionSortOrder(const TRepositoryCacheInfo &aRepository1, const TRepositoryCacheInfo &aRepository2);	
	
private:
	TBool iEnabled; 
	TInt  iTotalCacheUsage; 
	RArray<TRepositoryCacheInfo> iIdleRepositories;
	TTimeIntervalMicroSeconds32 iDefaultTimeout;
	TInt  iCacheSize;
	};

#include "cachemgr.inl"

#endif // CACHEMGR_H
