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
// This file contains test classes and their implementations
// to test production class CCustomResolverCache.
// 
//

#include <e32test.h>
#include <f32file.h>
#include <hal.h>
#include <bautils.h>
#include <babitflags.h>
#include <babackup.h>
#include <startup.hrh>
#include <ecom/resolver.h>
#include "EComErrorCodes.h"
#include "EComUidCodes.h"
#include "ImplementationInformation.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "RegistrarObserver.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include <swi/swispubsubdefs.h>
#include "../EcomTestUtils/TPropertyManager.h"
#include "EComPatchDataConstantv2.h"
#include "RegistryResolveTransaction.h"  // ecom3 code
#include "callback.h"
#include "resolvercache.h"
#include "EComServer.h"


const TInt KOneSecond = 1000000;
const TInt KHalfSecond = KOneSecond / 2;
// Use this timeout to wait for events occuring within a few seconds.
const TInt KIndefiniteWait = KOneSecond * 20;

LOCAL_D RTest test(_L("t_resolvercache.exe"));

LOCAL_D RFs					TheFs;
LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;
class CDerivedActiveScheduler;
LOCAL_D CDerivedActiveScheduler* TheActiveScheduler	= NULL;


// custom resolvers available for testing.
// 200126cd, A0001346 and A0001347 are allocated outside the
// ECOM Uid Allocations.doc
const TUid KDummyResolverUid2  = {0xA0001346};
const TUid KDummyResolverUid3  = {0xA0001347};
const TUid KExampleResolverUid = {0x10009DD0};
const TUid KMyResolverUid      = {0x10009E12};
const TUid KDummyResolverUid1  = {0x200126CD};

// The custom resolver in RAMOnly dir
_LIT(KDummyRscInC, "c:\\resource\\plugins\\dummycustomresolver1.rsc");
_LIT(KDummyDllInC, "c:\\sys\\bin\\dummycustomresolver1.dll");

_LIT(KDummyRscInZ, "z:\\ramonly\\dummycustomresolver1.rsc");
_LIT(KDummyDllInZ, "z:\\ramonly\\dummycustomresolver1.dll");

// This pair is to upgrade a resolver DLL.
// NB: to supersede a Z: drive plugin, the C: DLL must have the
// same name as the one in z: drive. Hence the '2' is dropped.
_LIT(KDllUpgradeInZ, "z:\\ramonly\\cachedcustomresolver2.dll");
_LIT(KDllUpgradeInC, "c:\\sys\\bin\\cachedcustomresolver.dll");

_LIT(KRscUpgradeInZ, "z:\\ramonly\\cachedcustomresolver2.rsc");
_LIT(KRscUpgradeInC, "c:\\resource\\plugins\\cachedcustomresolver.rsc");

/** User::AfterHighRes is not a reliable mechanism to wait
for async events. Especially when we have 4 different timers
firing within a span of 4 s. Hence this harness intercepts
and inserts callbacks in the notification sources and set
the following flag to indicate what event has occured. */
LOCAL_D TBitFlags32 AsyncEvents = 0;

/** enum to identify different async events */
enum TAsyncEventId
	{
	EAsyncEvent_Unknown = 0,
	EAsyncEvent_ImplUpgrade,
	EAsyncEvent_SwiStart,
	EAsyncEvent_SwiEnd,
	EAsyncEvent_BurStart,
	EAsyncEvent_BurEnd,
	EAsyncEvent_CacheTimer,
	EAsyncEvent_HaltTimer
	};

//
// maps callback data to enum TAsyncEventId.
LOCAL_C TAsyncEventId CallbackDataToEventId(TInt aEvent, TAny* aData)
	{
	TAsyncEventId ret = EAsyncEvent_Unknown;
	TCallBackState* state = static_cast<TCallBackState*>(aData);
	if (aEvent == ECallBackId_SwiEvent)
		{
		if (*state == ECallBackState_EventStart)
			{
			ret = EAsyncEvent_SwiStart;
			}
		else // treat all unexpected states as SWI end!
			{
			ret = EAsyncEvent_SwiEnd;
			}
		}
	else if (aEvent == ECallBackId_BurEvent)
		{
		if (*state == ECallBackState_EventStart)
			{
			ret = EAsyncEvent_BurStart;
			}
		else // treat all unexpected states as BUR finish!
			{
			ret = EAsyncEvent_BurEnd;
			}
		}
	else if (aEvent == ECallBackId_ImplUpgrade)
		{
		ret = EAsyncEvent_ImplUpgrade;
		}
	return ret;
	}

// beginningOfTest is set when this harness is run.
LOCAL_D TTime beginningOfTest;
LOCAL_C void WaitForLazyUnloadingL()
	{
	TTime now;
	now.UniversalTime();

	TTimeIntervalSeconds n;
	User::LeaveIfError(now.SecondsFrom(beginningOfTest, n));
	const TInt KLazyDllUnloadPeriod =  150; // actual is 2 minutes
	TInt secondsToWait = KLazyDllUnloadPeriod - n.Int();
	test.Printf(_L("Amount to wait for lazy unload is %d seconds.\n"), secondsToWait);
	if (secondsToWait > 0)
		{
		User::After(KOneSecond * secondsToWait);
		}
	}

// Copies the Plugins to specific folder for testing purpose
LOCAL_C void CopyPluginsL()
	{
	EComTestUtils::FileManCopyFileL(KDummyRscInZ, KDummyRscInC);
	EComTestUtils::FileManCopyFileL(KDummyDllInZ, KDummyDllInC);
	}

// Deleting plugin from the RAM for cleanup purpose
LOCAL_C void DeleteTestPlugin()
	{
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDummyRscInC));
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDummyDllInC));

	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KRscUpgradeInC));
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDllUpgradeInC));
	}

// utility to help cast TTimeIntervalMicroSeconds to TInt
LOCAL_C TInt CalcElapsedMicroseconds(const TTime& aStart)
	{
	TTime now;
	now.UniversalTime();
	TTimeIntervalMicroSeconds timediff = now.MicroSecondsFrom(aStart);
	return I64LOW( timediff.Int64() );
	}

/** Need a CActive to wait for various change notifications. */
class CHaltTimer : public CTimer
	{
public:
	CHaltTimer(TInt aPriority);
	~CHaltTimer();
	void ConstructL();
	void StartTimer(TInt aTimeInterval);

private:
	void RunL();
	};

CHaltTimer::CHaltTimer(TInt aPriority)
	: CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}

CHaltTimer::~CHaltTimer()
	{
	Cancel();
	}

void CHaltTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CHaltTimer::StartTimer(TInt aTimeInterval)
	{
	After(aTimeInterval);
	}

void CHaltTimer::RunL()
	{
	AsyncEvents.Set(EAsyncEvent_HaltTimer);
	CActiveScheduler::Stop();
	}

/** Avoid E32User::Case 47 panic in OOM test */
class CDerivedActiveScheduler : public CActiveScheduler
	{
public:
	virtual void Error(TInt aError) const;
	};

void CDerivedActiveScheduler::Error(TInt aError) const
	{
	Halt(aError);
	}

/** friend class to access private members of CEComServer */
class TEComServer_StateAccessor
	{
public:
	static void InterceptCallbacks(CEComServer& aEComServer, TCallBackWithArg aCb);
	static CCustomResolverCache* GetResolverCache(CEComServer& aEComServer);
	};

/** Test class for object CCustomResolverCache.
*/
class CCustomResolverCacheTest : public CBase
	{
public:
	typedef void (CCustomResolverCacheTest::*ClassFuncPtrL) (void);

	virtual ~CCustomResolverCacheTest();
	static CCustomResolverCacheTest* NewL();
	static void RunAllTestsL();
	static TInt InterceptCacheMgrCallback(TAny* aObj, TInt aEvent, TAny* aData);
	static TInt CacheTimeoutCallback(TAny* aObj, TInt aEvent, TAny* aData);

private:
	CCustomResolverCacheTest();
	void ConstructL();

	static void DoBasicTestL(ClassFuncPtrL aTestFunc);
	static void DoOOMTestL(ClassFuncPtrL aTestFunc);

	// Test cases
	void TestUpgradingCachedResolverL();
	void TestDeletingCachedResolverL();

	void TestCacheQueueFullPattern1L();
	void TestCacheQueueFullPattern2L();
	void TestCacheQueueFullPattern3L();
	void DoQueueFullTestL(const RArray<TUid>& aResolverList);

	void TestCounterWrapAroundL();
	void TestCacheTimerAccuracyL();
	void TestTimestampUpdateOnCacheHitL();
	void TestSWIDisableRwResolverCachingL();
	void TestBurDisableRwResolverCachingL();
	void TestClockChangeHasNoEffectOnCacheTimeoutL();
	void TestCacheSizeZeroL();
	void TestCacheTimeoutZeroL();
	void TestResolverWithBadProxyTable();

	// utilities
	TBool UseResolverCheckVersionL(TUid aResolverUid,
								   TInt aVersion = 0);
	void YieldToOtherCActive(TInt aMicroSeconds);
	void WaitAsyncL(TInt aNumSeconds);
	TBool WaitForEvict(TUid aResolverUid);

	// access private data of CCustomResolverCache
	TInt CacheSize();
	TBool HasResolverUid(TUid aUid);
	TBool QueueIsSorted();
	inline CCustomResolverCache* ResolverCache();

private:
	/** need a CActive to wait for other async events */
	CHaltTimer* iHaltTimer;

	CEComServer* iEComServer;
	};

//==============================================
// class TEComServer_StateAccessor
//==============================================
void TEComServer_StateAccessor::InterceptCallbacks(CEComServer& aEComServer,
												   TCallBackWithArg aCb)
	{
	aEComServer.iRegistrar->InstallSwiEventCallBack(aCb);
	aEComServer.iRegistrar->InstallBurEventCallBack(aCb);
	aEComServer.iRegistryData->SetImplUpgradeCallBack(aCb);
	}

CCustomResolverCache*
TEComServer_StateAccessor::GetResolverCache(CEComServer& aEComServer)
	{
	return aEComServer.iResolverCache;
	}

//==============================================
// class CCustomResolverCacheTest
//==============================================

/** CCustomResolverCacheTest constructor */
CCustomResolverCacheTest::CCustomResolverCacheTest()
	{
	}

/** CCustomResolverCacheTest destructor. */
CCustomResolverCacheTest::~CCustomResolverCacheTest()
	{
	delete iEComServer;
	delete iHaltTimer;
	}

/** static factory method to instantiate CCustomResolverCacheTest object.
*/
CCustomResolverCacheTest* CCustomResolverCacheTest::NewL()
	{
	CCustomResolverCacheTest* self = new(ELeave) CCustomResolverCacheTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Standardized 2nd phase of two phase construction.
*/
void CCustomResolverCacheTest::ConstructL()
	{
	iHaltTimer = new(ELeave) CHaltTimer(CActive::EPriorityIdle);
	iHaltTimer->ConstructL();

	iEComServer = CEComServer::NewLC();
	CleanupStack::Pop(iEComServer);

	TCallBackWithArg interceptorCB(&InterceptCacheMgrCallback, this);
	TEComServer_StateAccessor::InterceptCallbacks(*iEComServer, interceptorCB);

	TCallBackWithArg cacheTimerCb(&CacheTimeoutCallback, this);
	ResolverCache()->iTimerExpireCB = cacheTimerCb;
	}

/** util to fetch CEComServer::iResolverCache */
inline CCustomResolverCache* CCustomResolverCacheTest::ResolverCache()
	{
	return TEComServer_StateAccessor::GetResolverCache(*iEComServer);
	}

/** the test harness install this callback with ECOM server objects
to intercept async events. This callback will relay the original
call to CEComServer, then raise a flag to indicate what
event has occurred.
@param aObj pointer to the CCustomResolverCacheTest object
@param aEvent ID of event triggering the callback
@param aData  pointer to some data associated with the event.
@return Always KErrNone. It is ignored.
*/
TInt CCustomResolverCacheTest::InterceptCacheMgrCallback(TAny* aObj,
														 TInt aEvent,
														 TAny* aData)
	{
	CCustomResolverCacheTest* self = static_cast<CCustomResolverCacheTest*>(aObj);
	// Pass the event along to let CEComServer does its thing.
	CEComServer::NotifyEvents(self->iEComServer, aEvent, aData);

	if (self->iHaltTimer->IsActive())
		{
		TAsyncEventId event = CallbackDataToEventId(aEvent, aData);
		AsyncEvents.Set(event);

		self->iHaltTimer->Cancel();
		CActiveScheduler::Stop();
		}
	else
		{
		// BUR and SWI unit test cases just call the notifier
		// directly. So CHaltTimer is not running.
		test.Printf(_L("CacheTest: caught async event %d when timer not running\n"), aEvent);
		}
	return KErrNone;
	}

/** A callback installed in cache mgr. Whenever the cache timer expires
this callback is invoked by CCustomResolverCache
@param aObj pointer to CCustomResolverCacheTest
@return Always KErrNone. It is ignored.
*/
TInt CCustomResolverCacheTest::CacheTimeoutCallback(TAny* aObj, TInt, TAny*)
	{
	AsyncEvents.Set(EAsyncEvent_CacheTimer);

	CCustomResolverCacheTest* self = static_cast<CCustomResolverCacheTest*>(aObj);
	self->iHaltTimer->Cancel();
	CActiveScheduler::Stop();
	return KErrNone;
	}

/** Wrapper function to run normal mode (non-OOM) test
*/
void CCustomResolverCacheTest::DoBasicTestL(ClassFuncPtrL aTestFunc)
	{
	__UHEAP_MARK;
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CCustomResolverCacheTest* p = CCustomResolverCacheTest::NewL();
	(p->*aTestFunc)();
	delete p;

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4020
@SYMTestCaseDesc 	Verify no memory leak in CCustomResolverCache.
@SYMTestPriority 	Critical
@SYMTestActions  	Run UT-4015, UT-4016 and UT-4017 under OOM.
@SYMTestExpectedResults No memory leak, no handle leak.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::DoOOMTestL(ClassFuncPtrL aTestFunc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4020 "));
	__UHEAP_MARK;
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
	TInt err(KErrNone);
	TInt oomStep = 0;

	do {
		// Instantiation of CCustomResolverCacheTest involves building
		// the registry. It will take couple hours under OOM. Hence do it
		// outside __UHEAP_SETFAIL.
		CCustomResolverCacheTest* p = CCustomResolverCacheTest::NewL();

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++oomStep);
		TRAP(err, (p->*aTestFunc)());
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete p;
	} while (err == KErrNoMemory);

	test(err == KErrNone);
	test.Printf(_L("OOM succeeded at heap failure rate %d\n"), oomStep);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);
	__UHEAP_MARKEND;
	}

/** helper, ecom3 code
Issue a list request to add resolver to cache and check if
the registry has the expected version of the resolver.
@param aResolverUid the custom resolver to create
@param aVersion the expected version of the custom resolver. Check is skipped
  if aVersion is zero.
*/
TBool CCustomResolverCacheTest::UseResolverCheckVersionL(TUid aResolverUid,
														TInt aVersion)
	{
	TEComResolverParams resolverparams;
	_LIT8(KDummyData,"dummy");
	resolverparams.SetDataType(KDummyData);

	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	RImplInfoArray* infoArray = iEComServer->ListImplementationsL(
		KCustomResolverInterfaceUid, resolverparams, aResolverUid,
		extendedInterfaces, clientReq);

	TBool ret = (aVersion == 0);

	// infoArray not pushed because there are no leave in this search.
	for (TInt i = 0; !ret && i < infoArray->Count(); i++)
		{
		const CImplementationInformation* impl = (*infoArray)[i];
		if (impl->ImplementationUid() == aResolverUid && impl->Version() == aVersion)
			{
			ret = ETrue;
			}
		}

	infoArray->Reset();
	delete infoArray;

	return ret;
	}

/** utility to let other CActive s to run. */
void CCustomResolverCacheTest::YieldToOtherCActive(TInt aMicroSeconds)
	{
	iHaltTimer->StartTimer(aMicroSeconds);
	CActiveScheduler::Start();
	}

/** Call YieldToOtherCActive as many times as needed
until aNumSeconds is reached. */
void CCustomResolverCacheTest::WaitAsyncL(TInt aNumSeconds)
	{
	TTime start, now;
	start.UniversalTime();
	TTimeIntervalSeconds elapsed;

	for (elapsed = 0; elapsed.Int() < aNumSeconds; )
		{
		AsyncEvents.ClearAll();
		TInt seconds = (aNumSeconds - elapsed.Int());
		YieldToOtherCActive(KOneSecond * seconds);
		now.UniversalTime();
		User::LeaveIfError(now.SecondsFrom(start, elapsed));
		}
	}

/**
This method is used in the situation that cache timer is already
started when we load the given resolver. Then when cache timer
expires, the resolver is not ripe to be evicted. Have to wait a
second time.
@param aResolverUid the Resolver to wait for evict.
@return ETrue means the resolver is evicted correctly.
		EFalse means error, i.e. test fail.
*/
TBool CCustomResolverCacheTest::WaitForEvict(TUid aResolverUid)
	{
	TBool ret = EFalse;
	// No point of waiting for 20s for cache timeout.
	TInt waitMicroSec = KCustomResolverCacheTimeout + KOneSecond;
	for (TInt i = 0; (i < 2) && !ret; i++)
		{
		AsyncEvents.ClearAll();
		YieldToOtherCActive(waitMicroSec);
		// Check which async event has stopped the activescheduler
		test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );
		ret = ! HasResolverUid(aResolverUid);
		}
	return ret;
	}

//==========================================================
// utilities to access internal data of CCustomResolverCache.
// This is possible because CCustomResolverCacheTest is a friend.
//===========================================================

TInt CCustomResolverCacheTest::CacheSize()
	{
	return ResolverCache()->iResolvers.Count();
	}

TBool CCustomResolverCacheTest::HasResolverUid(TUid aUid)
	{
	for (TInt i = 0; i < ResolverCache()->iResolvers.Count(); i++)
		{
		if (ResolverCache()->iResolvers[i].iResolverUid == aUid)
			{
			return ETrue;
			}
		}
    return EFalse;
	}

TBool CCustomResolverCacheTest::QueueIsSorted()
	{
	CCustomResolverCache* theResolverCache = ResolverCache();
	for (TInt i = 1; i < theResolverCache->iResolvers.Count(); i++)
		{
		if (theResolverCache->iResolvers[i-1].iResolverUid.iUid >=
			theResolverCache->iResolvers[i].iResolverUid.iUid)
			{
			test.Printf(_L("Sort error: i-1 %d, 0x%8X, 0x%8X\n"), i-1,
				theResolverCache->iResolvers[i-1].iResolverUid.iUid,
				theResolverCache->iResolvers[i].iResolverUid.iUid	);
			return EFalse;
			}
		}
    return ETrue;
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-4012
@SYMTestCaseDesc 	If a cached resolver is superseded by a new version, it
	is evicted from cache.
@SYMTestPriority 	High
@SYMTestActions  	1. Use a custom resolver to get it in cache.
	2. copy the upgrade version of the resolver to C: and wait for
	   its discovery.
@SYMTestExpectedResults 1. a/. The resolver is the base version.
       b/. It is added in the cache.
	2. a/. Cache mgr receives upgrade notification (from CRegistryData).
	   b/. The resolver entry disappears from cache and the cache timer is
	       still running.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestUpgradingCachedResolverL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4012 "));
	// get the customer resolver in cache. It should be version 1.
	test (UseResolverCheckVersionL(KMyResolverUid, 1));
	// Check that KMyResolverUid is added to cache.
	test(HasResolverUid(KMyResolverUid));

	// Need an extra item in cache to get Bullseye to check off
	// a conditional. It is not an active ingredient of this test.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);

	// Copy the upgrade to C: drive
	EComTestUtils::FileManCopyFileL(KRscUpgradeInZ, KRscUpgradeInC);
	EComTestUtils::FileManCopyFileL(KDllUpgradeInZ, KDllUpgradeInC);

	// Let CDiscoverer discover the resource file.
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s

	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_ImplUpgrade) );

	// And the cache entry should be gone.
	test(! HasResolverUid(KMyResolverUid));

	// cleanup. NB: we never loaded the DLL in C: drive so
	// no need to worry about lazy dll unloading.
	EComTestUtils::FileManDeleteFileL(KRscUpgradeInC);
	EComTestUtils::FileManDeleteFileL(KDllUpgradeInC);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-4013
@SYMTestCaseDesc 	Verify a cached DLL cannot be deleted until it
	is removed from cache. This test serves as a sanity check that
	ECOM does need these notifications to trigger unloading DLL
	on SWI and BUR events.
@SYMTestPriority 	Medium
@SYMTestActions  	1. Use a RW drive resolver to get it in cache.
	2. Try to delete the DLL within default cache timeout period.
    3. Try again after it is evicted from cache.
@SYMTestExpectedResults 1. The resolver is added in cache.
	2. Get KErrAccessDenied error.
	3. The delete is either KErrNone or KErrNotFound because in step 2
	   even though the return code is -21, the file may be actually gone.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestDeletingCachedResolverL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4013 "));
	// NB: lazy Dll unloading interferes with this test.
	// Best to run this test at the end.
	WaitForLazyUnloadingL();

	// There is only 1 version of 200126CD. No need to check version.
	// So ignore the return code.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);

	// Check that it is added to cache.
	test( HasResolverUid(KDummyResolverUid1) );

	TInt err(KErrNone);
	TRAP(err, EComTestUtils::FileManDeleteFileL(KDummyDllInC));

#ifndef __WINSCW__
	// On hw you can delete the DLL while it is loaded.
	if (err == KErrNone)
		{
		test.Printf(_L("Delete test: RFs allows deletion of loaded DLL on hw. Test not run.\n"));
		EComTestUtils::FileManCopyFileL(KDummyDllInZ, KDummyDllInC);
		return;
		}
#endif

	test(err == KErrAccessDenied);

	// Wait for its eviction after cache timeout.
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s

	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );
	// Check the resolver is evicted
	test( !HasResolverUid(KDummyResolverUid1) );

	TRAP(err, EComTestUtils::FileManDeleteFileL(KDummyDllInC));
	test(err == KErrNone || err == KErrNotFound);

	// restore the file
	EComTestUtils::FileManCopyFileL(KDummyDllInZ, KDummyDllInC);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4015
@SYMTestCaseDesc 	Verify cache queue does not grow beyond the max
	queue size. Verify when a resolver is added to a full cache,
	the one to evict is the least recently used.

	There are 3 versions of this test. In Pattern1, the LRU entry
	has UID value which is the smallest of the 5 resolvers.
	In pattern 2, UID of the LRU is greatest. In pattern 3, UID of the
	LRU is the second smallest.
@SYMTestPriority 	High
@SYMTestActions  	1. Use 4 different resolvers to fill up the cache.
	2. Use a fifth one to bump off the least recently used entry.
    Run these two steps with the LRU entry in different positions in the
	queue, i.e. it is first, second, and last in the queue.
@SYMTestExpectedResults 1. The cache has 4 entries and is sorted in UID order.
	2. The oldest entry is gone from cache. The last one used is in cache.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestCacheQueueFullPattern1L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4015 "));
	RArray<TUid> resolverlist(1);
	CleanupClosePushL(resolverlist);

	// NB: the following insertion order caused the problem found in
	// INC115472. Hence there will be a check in DoQueueFullTestL
	// that the cache queue is sorted correctly.
	//
	// Condition for inc115472 is first insert an UID of 0xA???????
	// in an empty queue. Next insert an UID 0x2???????. The overflow
	// error causes them to be placed in the wrong order.
	resolverlist.AppendL(KDummyResolverUid2);
	resolverlist.AppendL(KDummyResolverUid1);
	resolverlist.AppendL(KExampleResolverUid);
	resolverlist.AppendL(KMyResolverUid);
	resolverlist.AppendL(KDummyResolverUid3);
	DoQueueFullTestL(resolverlist);
	CleanupStack::PopAndDestroy(&resolverlist);
	}

// In pattern 2 the oldest entry in cache has the largest UID value.
void CCustomResolverCacheTest::TestCacheQueueFullPattern2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4015 "));
	RArray<TUid> resolverlist(1);
	CleanupClosePushL(resolverlist);
	resolverlist.AppendL(KDummyResolverUid1);
	resolverlist.AppendL(KDummyResolverUid2);
	resolverlist.AppendL(KDummyResolverUid3);
	resolverlist.AppendL(KExampleResolverUid);
	resolverlist.AppendL(KMyResolverUid);
	DoQueueFullTestL(resolverlist);
	CleanupStack::PopAndDestroy(&resolverlist);
	}

// In pattern 3 the UID of the oldest entry has the second smallest value.
void CCustomResolverCacheTest::TestCacheQueueFullPattern3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4015 "));
	RArray<TUid> resolverlist(1);
	CleanupClosePushL(resolverlist);
	resolverlist.AppendL(KDummyResolverUid3);
	resolverlist.AppendL(KDummyResolverUid1);
	resolverlist.AppendL(KDummyResolverUid2);
	resolverlist.AppendL(KExampleResolverUid);
	resolverlist.AppendL(KMyResolverUid);
	DoQueueFullTestL(resolverlist);
	CleanupStack::PopAndDestroy(&resolverlist);
	}

void CCustomResolverCacheTest::DoQueueFullTestL(const RArray<TUid>& aResolverList)
	{
	// ensure cache is empty.
	test(CacheSize() == 0);

	TInt maxQueueSize = ResolverCache()->iMaxCacheSize;
	test(aResolverList.Count() > maxQueueSize);

	// Put 4 resolvers in the cache. Pause in between so that the time tick
	// of each entry is different. CacheMissPerfTestL in t_resolverperf exercises
	// the code path that time ticks are the same.
	const TInt KInBetweenDelay = 100000;
	TInt i;
	for (i = 0; i < maxQueueSize; i++)
		{
		UseResolverCheckVersionL(aResolverList[i], 0);
		User::AfterHighRes(KInBetweenDelay);
		}

	// Check cache is full.
	test(CacheSize() == maxQueueSize);
	// Check cache is sorted properly
	test( QueueIsSorted() );

	// Add one more to cache.
	UseResolverCheckVersionL(aResolverList[maxQueueSize], 0);

	// Check that cache size is still full - not exceeding limit.
	test(CacheSize() == maxQueueSize);

	// Want to check LRU is gone and the last resolver is now in cache.
	test(! HasResolverUid(aResolverList[0]));
	test( QueueIsSorted() );
	// Because of the above 3 checks, this is really redundant.
	test(HasResolverUid(aResolverList[maxQueueSize]));

	// Because the timestamps of the cache entries are staggered
	// 100 ms apart, the mgr can only kick out 1 item at each timer expiry.
	// So let the cache mgr exercise this code path.
	TInt n = 0;
	while (CacheSize() > 0)
		{
		AsyncEvents.ClearAll();
		YieldToOtherCActive(KIndefiniteWait);  // 20 s
		// Check which async event has stopped the activescheduler
		test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );
		n++;
		}

	// n should be five. aResolverList[0] is bumped due to queue full.
	// So the first timer expire will not find any expired entry.
	test.Printf(_L("Gone through %d loops to clear out cache.\n"), n);

	test(n > maxQueueSize);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4016
@SYMTestCaseDesc 	Verify resolver is unloaded after the specified timeout.
@SYMTestPriority 	Medium
@SYMTestActions  	1. Use a resolver to get it in cache.
	2. Measure how long the entry stays in cache before it is evicted.
@SYMTestExpectedResults The time should be >= default timeout but
	<= (timeout + 0.5s).
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestCacheTimerAccuracyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4016 "));
	TTime start;
	start.UniversalTime();

	UseResolverCheckVersionL(KMyResolverUid, 0);
	test( HasResolverUid(KMyResolverUid) );

	// Wait for cache timeout
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	TInt microseconds = CalcElapsedMicroseconds(start);
	test.Printf(_L("Resolver is cached for %d microseconds\n"), microseconds);

	TInt tickperiod;
	User::LeaveIfError(HAL::Get(HALData::ESystemTickPeriod, tickperiod));
	TInt lowerLimit = KCustomResolverCacheTimeout - tickperiod;
	test(microseconds > lowerLimit);

	// The upper bound is tricky because there is no gaurantee on
	// CTimer accuracy.
	test(microseconds < (KCustomResolverCacheTimeout + KHalfSecond));
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4017
@SYMTestCaseDesc 	Verify after a cache hit the time to live of the entry
	is extended.
@SYMTestPriority 	High
@SYMTestActions  	1. Record start time.
	2. Use a resolver to put it in cache.
	3. After 1 s use it again.
	4  Wait for its eviction.
	5. Check how long the entry stays in cache.
@SYMTestExpectedResults The time should be >= 5 seconds.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestTimestampUpdateOnCacheHitL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4017 "));
	// Check that the resolver is not in cache.
	test(! HasResolverUid(KMyResolverUid) );

	TTime start;
	start.UniversalTime();

	UseResolverCheckVersionL(KMyResolverUid, 0);
	test( HasResolverUid(KMyResolverUid) );

	// Delay one second and use it again.
	User::AfterHighRes(KOneSecond);
	UseResolverCheckVersionL(KMyResolverUid, 0);

	// Wait for its eviction
	while ( HasResolverUid(KMyResolverUid) )
		{
		AsyncEvents.ClearAll();
		YieldToOtherCActive(KIndefiniteWait);  // 20 s
		// Check which async event has stopped the activescheduler
		test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );
		}

	TInt microseconds = CalcElapsedMicroseconds(start);
	test.Printf(_L("With cache hit, resolver is cached for %d microseconds\n"), microseconds);

	TInt tickperiod;
	User::LeaveIfError(HAL::Get(HALData::ESystemTickPeriod, tickperiod));
	TInt lowerLimit = KCustomResolverCacheTimeout + KOneSecond - tickperiod;
	test(microseconds > lowerLimit);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4018
@SYMTestCaseDesc 	Verify at SWI start RW resolvers are evicted from cache.
	Verify during SWI ROM resolvers are cached but RW resolvers are not.
@SYMTestPriority 	High
@SYMTestActions  	1. Add a RW resolver to cache.
	2. Send a SWI start signal.
	3. Use a RW resolver.
	4  Use a ROM resolver.
@SYMTestExpectedResults 1. The RW resolver is in cache.
	2. The cache entry is evicted.
	3. The RW resolver is not cached.
	4. The ROM resolver is cached.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestSWIDisableRwResolverCachingL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4018 "));
	// The TPropertyManager protocol to setup to use a system category P&S.
	TInt r = PropertyManager::DeleteProperty(KUidSystemCategory,
		KSAUidSoftwareInstallKeyValue);
	test(r == KErrNone);
	r = PropertyManager::DefineProperty(KUidSystemCategory,
		KSAUidSoftwareInstallKeyValue, RProperty::EInt);
	test(r == KErrNone);

	// Use a resolver in C: drive.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	// Check that it is added to cache.
	test( HasResolverUid(KDummyResolverUid1) );

	// Need a ROM entry in cache to get Bullseye to check off
	// a conditional. It is not an active ingredient of this test.
	UseResolverCheckVersionL(KMyResolverUid, 0);

	// SWI start
	r = PropertyManager::SetProperty(KUidSystemCategory,
		KSAUidSoftwareInstallKeyValue,ESASwisInstall);
	test(r == KErrNone);

	// Let CSwiChangeNotifier receive the notification
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check we receive the correct event
	test( AsyncEvents.IsSet(EAsyncEvent_SwiStart) );

	// Check the RW resolver is evicted
	test(! HasResolverUid(KDummyResolverUid1) );

	// during SWI RW resolver will not be cached.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	test(! HasResolverUid(KDummyResolverUid1) );

	// But ROM resolvers are still cached.
	UseResolverCheckVersionL(KDummyResolverUid2, 0);
	test( HasResolverUid(KDummyResolverUid2) );

	// And the ROM resolvers are evicted after 4 s as usual.
	// Note that we have KMyResolverUid and KDummyResolverUid2 in cache
	TBool b = WaitForEvict(KDummyResolverUid2);
	if (! b)
		{ // got KMyResolverUid only, need a second wait.
		b = WaitForEvict(KDummyResolverUid2);
		}
	test(b);

	// Just for completeness. Do not really need this
	// for Bullseye Coverage.
	r = PropertyManager::SetProperty(KUidSystemCategory,
		KSAUidSoftwareInstallKeyValue,ESASwisNone);
	test(r == KErrNone);
	// Let CSwiChangeNotifier receive the notification
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check we receive the correct event
	test( AsyncEvents.IsSet(EAsyncEvent_SwiEnd) );
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4019
@SYMTestCaseDesc 	Verify at BUR start RW resolvers are evicted from cache.
	Verify during BUR ROM resolvers are cached but RW resolvers are not.
@SYMTestPriority 	High
@SYMTestActions  	1. Add a RW resolver to cache.
	2. Send a BUR start signal.
	3. Use a RW resolver.
	4  Use a ROM resolver.
@SYMTestExpectedResults 1. The RW resolver is in cache.
	2. The cache entry is evicted.
	3. The RW resolver is not cached.
	4. The ROM resolver is cached.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestBurDisableRwResolverCachingL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4019 "));
	// setup backup session wrapper
	CBaBackupSessionWrapper* ba = CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(ba);
	TBackupOperationAttributes attribs;
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EStart;

	// The backup notifier of ECOM is not registered with
	// BA server at construction time. It delays 15 s.
	// So we have to wait this long.
	WaitAsyncL(16);

	// Use a resolver in C: drive.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	// Check that it is added to cache.
	test( HasResolverUid(KDummyResolverUid1) );

	// Need a ROM entry in cache to get Bullseye to check off
	// a conditional. It is not an active ingredient of this test.
	UseResolverCheckVersionL(KMyResolverUid, 0);

	// backup start
	ba->NotifyBackupOperationL(attribs);
	// Let backup notifier receive the backup start
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check we receive the backup start notification
	test( AsyncEvents.IsSet(EAsyncEvent_BurStart) );

	// Check the RW resolver is evicted
	test(! HasResolverUid(KDummyResolverUid1) );

	// during BUR RW resolver will not be cached.
	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	test(! HasResolverUid(KDummyResolverUid1) );

	// But ROM resolvers are still cached.
	UseResolverCheckVersionL(KDummyResolverUid2, 0);
	test( HasResolverUid(KDummyResolverUid2) );

	// And the ROM resolvers are evicted after 4 s as usual.
	// Note that we have KMyResolverUid and KDummyResolverUid2 in cache
	TBool b = WaitForEvict(KDummyResolverUid2);
	if (! b)
		{ // got KMyResolverUid only, need a second wait.
		b = WaitForEvict(KDummyResolverUid2);
		}
	test(b);

	// Do this for Bullseye Coverage of production code.
	// NB: attribs.iFileFlag stays the same
	attribs.iOperation=MBackupOperationObserver::EEnd;
	ba->NotifyBackupOperationL(attribs);
	// Let backup notifier receive it
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check we receive the backup notification
	test( AsyncEvents.IsSet(EAsyncEvent_BurEnd) );

	CleanupStack::PopAndDestroy(ba);

	// to earn another Bullseye check mark.
	CEComServer::NotifyEvents(iEComServer, ECallBackId_None, NULL);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4021
@SYMTestCaseDesc 	Verify the cache timer and data to keep track of
	cache entry time to live are immune to device clock change.
@SYMTestPriority 	High
@SYMTestActions  	1. Put two resolvers in cache at 20 ticks apart.
	2. Spring clock forward by 5 s (1 + default cache timeout).
	3. Measure how long the first resolver is cached.
	4. Measure how long the second resolver is cached.
	Repeat steps 1 to 4 in setting clock backward.
@SYMTestExpectedResults Both resolvers are cached for ~ 4 s
	(default cache timeout value).
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestClockChangeHasNoEffectOnCacheTimeoutL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4021 "));
	TInt tp;
	User::LeaveIfError(HAL::Get(HALData::ESystemTickPeriod, tp));
	TInt delayInBetween = tp * 20;
   	test.Printf(_L("ESystemTickPeriod: %d; delayInBetween: %d;\n"), tp, delayInBetween);


	TTime beforeDelay;
	beforeDelay.UniversalTime();

	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	test( HasResolverUid(KDummyResolverUid1) );

	User::AfterHighRes(delayInBetween);
	TInt elapsedBeforeTimeChange = CalcElapsedMicroseconds(beforeDelay);

	UseResolverCheckVersionL(KDummyResolverUid2, 0);
	test( HasResolverUid(KDummyResolverUid2) );

	TTimeIntervalMicroSeconds AmountOfChange(
		MAKE_TINT64(0, KCustomResolverCacheTimeout+KOneSecond) );

	// Now set device clock forward
	TTime t;
	t.UniversalTime();
	t += AmountOfChange;
	User::LeaveIfError( User::SetUTCTime(t) );

	t.UniversalTime(); // need this, apparently SetUTCTime does rounding

	// Wait for cache timeout
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	TInt microseconds = CalcElapsedMicroseconds(t);
	// compensate for the 20 ticks delay
	microseconds += elapsedBeforeTimeChange;
	test.Printf(_L("Clock forward: 1st resolver is cached for %d microseconds\n"), microseconds);
	test(! HasResolverUid(KDummyResolverUid1) );
	// Check duration is between 4 to 4.5 s.
	TInt lowerLimit = KCustomResolverCacheTimeout - (tp*3);
	TInt upperLimit = (KCustomResolverCacheTimeout + KHalfSecond);
   	test.Printf(_L("lowerLimit: %d; upperLimit: %d\n"), lowerLimit, upperLimit);	
	test(microseconds > lowerLimit);

	// upper limit is tricky because there is no guarantee on CTimer
	// accuracy.
	test(microseconds <= upperLimit);

	// Wait for eviction of the second resolver
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	microseconds = CalcElapsedMicroseconds(t);
	test.Printf(_L("Clock forward: 2nd resolver is cached for %d microseconds\n"), microseconds);
	test(! HasResolverUid(KDummyResolverUid2) );
	// Check duration is between 4 to 4.5 s.
	test(microseconds > lowerLimit);
	test(microseconds <= upperLimit);

	//
	// Repeat for setting clock backward.
	//
	beforeDelay.UniversalTime();

	UseResolverCheckVersionL(KDummyResolverUid1, 0);
	test( HasResolverUid(KDummyResolverUid1) );

	User::AfterHighRes(delayInBetween);
	elapsedBeforeTimeChange = CalcElapsedMicroseconds(beforeDelay);

	UseResolverCheckVersionL(KDummyResolverUid2, 0);
	test( HasResolverUid(KDummyResolverUid2) );

	// Now set device clock backward
	t.UniversalTime();
	t -= AmountOfChange;
	User::LeaveIfError( User::SetUTCTime(t) );
	t.UniversalTime();

	// Wait for cache timeout
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	microseconds = CalcElapsedMicroseconds(t);
	// compensate for the 20 ticks delay
	microseconds += elapsedBeforeTimeChange;
	test.Printf(_L("Clock backward: 1st resolver is cached for %d microseconds\n"), microseconds);
	test(! HasResolverUid(KDummyResolverUid1) );
	// Check duration is between 4 to 4.5 s.
	test(microseconds > lowerLimit);
	test(microseconds <= (KCustomResolverCacheTimeout + KHalfSecond));

	// Wait for eviction of the second resolver
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	microseconds = CalcElapsedMicroseconds(t);
	test.Printf(_L("Clock backward: 2nd resolver is cached for %d microseconds\n"), microseconds);
	test(! HasResolverUid(KDummyResolverUid2) );
	// Check duration is between 4 to 4.5 s.
	test(microseconds > lowerLimit);
	test(microseconds <= (KCustomResolverCacheTimeout + KHalfSecond));
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4022
@SYMTestCaseDesc 	Verify caching runs normally if queue size is zero.
@SYMTestPriority 	Medium
@SYMTestActions  	1. Set max. cache size to 0.
	2. Use a custom resolver.
@SYMTestExpectedResults No leave on the list request and cache is empty.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestCacheSizeZeroL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4022 "));
	ResolverCache()->iMaxCacheSize = 0;
	UseResolverCheckVersionL(KMyResolverUid, 0);
	test(! HasResolverUid(KMyResolverUid) );
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4023
@SYMTestCaseDesc 	Verify caching runs normally if cache timeout is zero.
@SYMTestPriority 	Medium
@SYMTestActions  	1. Set cache time-to-live to zero.
	2. Use a custom resolver.
@SYMTestExpectedResults No leave on the list request and cache is empty.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestCacheTimeoutZeroL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4023 "));
	TInt systemTickPeriod = ResolverCache()->iSystemTickPeriod;
	// mimic CCustomResolverCache calculate the time-to-live value.
	const TInt KZeroTimeout = 0;
	ResolverCache()->iEntryTimeToLive = (KZeroTimeout +
		systemTickPeriod - 1) / systemTickPeriod;

	// run the test
	UseResolverCheckVersionL(KMyResolverUid, 0);
	test(! HasResolverUid(KMyResolverUid) );
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4024
@SYMTestCaseDesc 	CCustomResolverCache uses system tick to track entry
	time-to-live. The counter wraps around every 777 days. This test
	checks when wrap around occurs cached entry is purged correctly.
@SYMTestPriority 	Medium
@SYMTestActions  	1. Use a resolver to get it in cache.
	2. Set its time-of-use to 0xFFFFFFFF.
	3. Wait for cache timer expire.
@SYMTestExpectedResults The entry is evicted on first timer expiry and
	the entry stays in cache for ~ 4s.
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestCounterWrapAroundL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4024 "));
	TTime start;
	start.UniversalTime();

	UseResolverCheckVersionL(KMyResolverUid, 0);
	RResolverCacheEntry& entry = ResolverCache()->iResolvers[0];
	test( entry.iResolverUid == KMyResolverUid );

	// Set time-of-use to edge of wrap around
	entry.iLastUse.iTicks = KMaxTUint;

	// Wait for cache timeout
	AsyncEvents.ClearAll();
	YieldToOtherCActive(KIndefiniteWait);  // 20 s
	// Check which async event has stopped the activescheduler
	test( AsyncEvents.IsSet(EAsyncEvent_CacheTimer) );

	TInt microseconds = CalcElapsedMicroseconds(start);
	test.Printf(_L("Wrap around: resolver is cached for %d microseconds\n"), microseconds);

	TInt tickperiod;
	User::LeaveIfError(HAL::Get(HALData::ESystemTickPeriod, tickperiod));
	TInt lowerLimit = KCustomResolverCacheTimeout - tickperiod;
	test(microseconds > lowerLimit);
	// Upper limit must have a wide margin because there is no gaurantee
	// on CTimer accuracy.
	test(microseconds < (KCustomResolverCacheTimeout + KHalfSecond));
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-4025
@SYMTestCaseDesc 	Verify if cannot find the NewL method in the proxy
	instantiation table, ECOM returns KEComErrNoResolver
@SYMTestPriority 	Medium
@SYMTestActions  	Use a resolver whose proxy table has the wrong Impl. Uid.
@SYMTestExpectedResults Get KEComErrNoResolver error
@SYMCR CR1182
*/
void CCustomResolverCacheTest::TestResolverWithBadProxyTable()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-4025 "));
	const TUid KBadResolverUid = {0x10009DDF};
	TRAPD(err, UseResolverCheckVersionL(KBadResolverUid, 0) );
	test(err == KEComErrNoResolver);
	}

/** wrapper to run all the Basic and OOM tests */
void CCustomResolverCacheTest::RunAllTestsL()
	{
	CopyPluginsL();

	test.Next(_L("Basic TestCounterWrapAroundL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCounterWrapAroundL);

	test.Next(_L("Basic TestUpgradingCachedResolverL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestUpgradingCachedResolverL);

	test.Next(_L("Basic TestCacheQueueFullPattern1L"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheQueueFullPattern1L);

	test.Next(_L("Basic TestCacheQueueFullPattern2L"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheQueueFullPattern2L);

	test.Next(_L("Basic TestCacheQueueFullPattern3L"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheQueueFullPattern3L);

	test.Next(_L("Basic TestCacheTimerAccuracyL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheTimerAccuracyL);

	test.Next(_L("Basic TestTimestampUpdateOnCacheHitL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestTimestampUpdateOnCacheHitL);

	test.Next(_L("Basic TestSWIDisableRwResolverCachingL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestSWIDisableRwResolverCachingL);

	test.Next(_L("Basic TestBurDisableRwResolverCachingL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestBurDisableRwResolverCachingL);

	test.Next(_L("Basic TestClockChangeHasNoEffectOnCacheTimeoutL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestClockChangeHasNoEffectOnCacheTimeoutL);

	test.Next(_L("Basic TestCacheSizeZeroL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheSizeZeroL);

	test.Next(_L("Basic TestCacheTimeoutZeroL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestCacheTimeoutZeroL);

	test.Next(_L("Basic TestResolverWithBadProxyTable"));
	DoBasicTestL(&CCustomResolverCacheTest::TestResolverWithBadProxyTable);

	// Only run OOM on tests which do not involve rescan dir.
	test.Next(_L("OOM TestCacheQueueFullPattern3L"));
	DoOOMTestL(&CCustomResolverCacheTest::TestCacheQueueFullPattern3L);

	test.Next(_L("OOM TestCacheTimerAccuracyL"));
	DoOOMTestL(&CCustomResolverCacheTest::TestCacheTimerAccuracyL);

	test.Next(_L("OOM TestTimestampUpdateOnCacheHitL"));
	DoOOMTestL(&CCustomResolverCacheTest::TestTimestampUpdateOnCacheHitL);

	// Do all tests affected by Lazy DLL unload last.
	test.Next(_L("Basic TestDeletingCachedResolverL"));
	DoBasicTestL(&CCustomResolverCacheTest::TestDeletingCachedResolverL);
	}

static TInt KillEComServer()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(err, EComTestUtils::KillProcessL(KEComServerProcessName));
   	return err;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	beginningOfTest.UniversalTime();

	test.Printf(_L("\n"));
	test.Title();
	test.Start( _L("Custom Resolver Cache") );


	TheTrapCleanup = CTrapCleanup::New();
	test(TheTrapCleanup != NULL);

	// Connect the file server instance
	test(KErrNone == TheFs.Connect());

	TheActiveScheduler = new CDerivedActiveScheduler;
	test(TheActiveScheduler != NULL);
	CActiveScheduler::Install(TheActiveScheduler);

	TInt err = ::KillEComServer();
	test(err == KErrNotFound || err == KErrNone);

	// Call the main tests
	TRAP(err, CCustomResolverCacheTest::RunAllTestsL());
	if (err != KErrNone) test.Printf(_L("RunAllTestsL() error, %d\n"), err);
	test(err == KErrNone);

	// remove the RAMOnly files
	TRAP_IGNORE( WaitForLazyUnloadingL() );
	DeleteTestPlugin();
	User::After(KOneSecond * 3);

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	TheFs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
