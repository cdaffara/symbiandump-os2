// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include <badesca.h>

#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include "sessnotf.h"

RTest TheTest(_L("Central Repository Coarse-Grained Cache Tests"));

enum TBorderTestStage
	{
	ENominalNoFile = -1,
	ENominal = 0,
	ESizeMin,
	ESizeMinplus,
	ESizeMaxminus,
	ESizeMax,
	ESizeMaxplus,
	ETimeoutMin,
	ETimeoutMinplus,
	ETimeoutMaxminus,
	EReallyWorstCase,
	ELastStage
	};

const TUid KUidCacheTestRepositorySm	= { 0x00000002 };
const TUid KUidCacheTestRepositorySm2	= { 0x00000011 };
const TUid KUidCacheTestRepositoryMed   = { 0x11111111 };
const TUid KUidCacheTestRepositoryMed2  = { 0x00057778 };
const TUid KUidCacheTestRepositoryLrg	= { 0xCCCCCC01 };

const TInt KTimerDelay = 1000000;
const TInt KMemoryFiller = 3762;

_LIT(KCacheMgrIniFileFolder, "\\private\\10202BE9\\");
_LIT(KCacheMgrIniSrcFile, "centrepcache.ini");

TTime starttime;

class TRepositoryCacheManagerTester
	{
	public:
	TRepositoryCacheManagerTester(): iOOMTest(EFalse), iStage(ENominalNoFile), iTestStepStage(0)
		{
		iFs.Connect();
		}
	~TRepositoryCacheManagerTester()
		{
		iFs.Close();
		}
	TInt CacheRepositoryL(TUid aRepUid, TBool& aTrap);
	TInt CacheRepositoryL(TUid aRepUid);
	TBool FindRepository(TUid aRepUid);
	void FuncTestsL();
	void SizeTestsL();
	void TimingTestsL();
	void DefectTestsL();
	void DEF093491L();
	void INC105967();
	void DeleteFilesL();
	void DEF111734L();
	void DEF124147();
	void InstallIniFileL( TBorderTestStage aFileSet );
	void AdvanceToStageL(TBorderTestStage aStage);
	void NextTest(TPtrC aMsg);
	static TInt Callback(TAny* aParent);
	static void CleanUp(TAny*);
public:
	TBool iOOMTest;
	RFs iFs;
	TBorderTestStage iStage;
	TInt iTestStepStage;
	};

LOCAL_C TRepositoryCacheManagerTester Tester;

LOCAL_C TCleanupItem gCleanup(TRepositoryCacheManagerTester::CleanUp);
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

void TRepositoryCacheManagerTester::DeleteFilesL()
	{
	_LIT( KOldInstallFiles, "c:\\private\\10202BE9\\*.ini" );

	CFileMan* fm = CFileMan::NewL( iFs );
	CleanupStack::PushL( fm );

	TInt r = fm->Delete( KOldInstallFiles );
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);

	CleanupStack::PopAndDestroy( fm );
	}

LOCAL_C void Check( TInt aValue, TInt aLine )
	{
	if ( !aValue )
		{
		TheTest( EFalse, aLine );
		}
	}

LOCAL_C void Check( TInt aValue, TInt aExpected, TInt aLine )
	{
	if ( aValue != aExpected )
		{
		RDebug::Print( _L( "*** Expected error: %d, got: %d\r\n"), aExpected, aValue );
		TheTest( EFalse, aLine );
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void TRepositoryCacheManagerTester::NextTest(TPtrC aMsg)
	{
	if (!iOOMTest)
		{
		TheTest.Next( aMsg );
		}
	}

void TRepositoryCacheManagerTester::AdvanceToStageL(TBorderTestStage aStage)
	{
	iStage = aStage;
	if (aStage != ENominalNoFile)
		{
		InstallIniFileL( aStage );
		}
	else
		{
		DeleteFilesL();
		}
	delete TServerResources::iCacheManager;
	TServerResources::iCacheManager = NULL;
	TServerResources::iCacheManager = CRepositoryCacheManager::NewLC(iFs);
	CleanupStack::Pop(TServerResources::iCacheManager);
	}

void TRepositoryCacheManagerTester::InstallIniFileL( TBorderTestStage aFileSet )
	{
	_LIT( KDriveC, "c:" );
	_LIT( KDriveZ, "z:" );

	TBuf<KMaxFileName> src1;
	TBuf<KMaxFileName> dest1;
	TInt r;

	DeleteFilesL();

	CFileMan* fm = CFileMan::NewL( iFs );
	CleanupStack::PushL( fm );

	if ((aFileSet>=ELastStage)||(aFileSet<0))
		{
		RDebug::Print( _L( "Illegal parameter to function: %d\r\n" ), aFileSet );
		TheTest( EFalse, __LINE__ );
		}
	else
		{
		dest1.Copy( KDriveC );
		dest1.Append( KCacheMgrIniFileFolder );
		dest1.Append( KCacheMgrIniFile );
		r = fm->Delete( dest1 );
		if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
			User::Leave( r );
		r = iFs.MkDirAll( dest1 );
		if ( r != KErrNone && r != KErrAlreadyExists )
			User::Leave( r );
		src1.Copy( KDriveZ );
		src1.Append( KCacheMgrIniFileFolder );
		src1.Append( KCacheMgrIniSrcFile );
		TBuf<2> testNo;
		testNo.Num(aFileSet);
		src1.Append( testNo );
		User::LeaveIfError( fm->Copy( src1, dest1 ) );
		r = fm->Attribs( dest1, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
		TEST2( r, KErrNone );
		}

	CleanupStack::PopAndDestroy( fm );
	}

TInt TRepositoryCacheManagerTester::CacheRepositoryL(TUid aRepUid)
	{
	TBool trap = EFalse;
	return CacheRepositoryL(aRepUid, trap);
	}

void TRepositoryCacheManagerTester::CleanUp(TAny*)
	{
	// If cache manager is initialized and used before, we flush it
	if (TServerResources::iCacheManager)
		{
		TServerResources::iCacheManager->FlushCache();
		}
	// To get rid of the iOpenRepositories array leaking problem during OOM testing.
	TServerResources::iObserver->CloseiOpenRepositories();
	TServerResources::iObserver->Reset();
	// To get rid of the array leaking problems during OOM testing.
	TServerResources::iOwnerIdLookUpTable.Reset();
	}

// Opens and Closes a repository to place it in the cache
TInt TRepositoryCacheManagerTester::CacheRepositoryL(TUid aRepUid, TBool& aTrap)
	{
	TInt repsize = 0;
	// Notifier needed to open repositories.
	CSessionNotifier* notifier;
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    TServerResources::iObserver->iTrapOOMOnOpen = aTrap;
    aTrap = EFalse; // This means that this function didn't leave until attempting to open the repository
    repository->OpenL(aRepUid,*notifier);
    repsize = repository->SizeiRepository();
	repository->Close();
	TServerResources::iObserver->iTrapOOMOnOpen = EFalse;

   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);

   	return repsize;
	}

// Checks if a repository is in the cache
TBool TRepositoryCacheManagerTester::FindRepository(TUid aRepUid)
	{
	for(TInt i=TServerResources::iCacheManager->iIdleRepositories.Count()-1; i>=0; i--)
		{
		if(TServerResources::iCacheManager->iIdleRepositories[i].iSharedRepository->Uid()==aRepUid)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt TRepositoryCacheManagerTester::Callback(TAny* aParent)
	{
	TTime now;
	now.UniversalTime();
	TheTest.Printf(_L("Timer Expired at %Ld\n"), now.MicroSecondsFrom(starttime).Int64());
	((CPeriodic*)aParent)->Cancel();;
	CActiveScheduler::Stop();

	return KErrNone;
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1419
@SYMTestCaseDesc Test Coarse-Grained Caching functionality.
@SYMTestPriority High
@SYMTestActions  Check correct initialization of the cache manager, expected caching behaviour after
 repository open/close, correct setting of cache size, correct functioning of the cache flush
 functionality, OOM recovery by cache flushing feature and cache Disable/Enable functionality
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ1192
@SYMTestStatus Defined
@SYMDevelopedForRelease Symbian OS v9.3
@SYMAuthor Aleks Pamir
*/
void TRepositoryCacheManagerTester::FuncTestsL()
	{
	NextTest(_L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1419 Initialize Cache Manager " ));

	iTestStepStage = 0;
	TEST( TServerResources::iCacheManager != 0 );
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	TServerResources::iCacheManager->EnableCache();

	NextTest( _L( "Cache Repository (Open/Close)" ) );

	CacheRepositoryL(KUidCacheTestRepositorySm);

	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		}

	NextTest( _L( "Check Cache Size" ) );

	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		TEST( TServerResources::iCacheManager->iTotalCacheUsage == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iTotalCacheUsage > 0 );
		}

	NextTest( _L( "Flush Cache" ) );

	TServerResources::iCacheManager->FlushCache();

	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	TEST( TServerResources::iCacheManager->iTotalCacheUsage == 0 );


	if (!iOOMTest)
		{
		// This test is excluded from generic OOM testing because
		// 1. It is trying to simulate OOM condition in a different way (fillingup the memory with
		// allocs) which makes it very tricky to work under normal OOM testing method
		// 2. Loading Lrg repository within OOM takes forever because there're many allocations, and
		// loading other repositories are already tested in other tests

		NextTest( _L( "Cache OOM Handling" ) );

		// Fill cache with reps
		TInt totalsize;
		TInt cachedRepositoryCount = 0;

		totalsize = CacheRepositoryL(KUidCacheTestRepositorySm);
		if (TServerResources::iCacheManager->iCacheSize >= totalsize)
			{
			cachedRepositoryCount++;
			}
		totalsize += CacheRepositoryL(KUidCacheTestRepositorySm2);
		if (TServerResources::iCacheManager->iCacheSize >= totalsize)
			{
			cachedRepositoryCount++;
			}
		totalsize += CacheRepositoryL(KUidCacheTestRepositoryLrg);
		if (TServerResources::iCacheManager->iCacheSize >= totalsize)
			{
			cachedRepositoryCount++;
			}
		totalsize += CacheRepositoryL(KUidCacheTestRepositoryMed2);
		if (TServerResources::iCacheManager->iCacheSize >= totalsize)
			{
			cachedRepositoryCount++;
			}

		TInt res = KErrNone;
		//	Fill Memory
		TInt popCount=0;
		RHeap& myHeap = User::Heap();
		TInt firstSize = myHeap.Size();
		TInt biggestBlock;
		TInt firstAvail = myHeap.Available(biggestBlock);

		while(ETrue)
			{
			// We need to really fill up the memory, because we want it to be really freed when we explictly
			// free it, so that when we alloc again there will be some free memory. Using debug allocfail
			// tools would have been too artifical for such a test
			TAny* dummy = User::Alloc(KMemoryFiller);
			if (dummy)
				{
				popCount++;
				TRAP( res, CleanupStack::PushL(dummy); CleanupStack::Pop(dummy););
				if (res == KErrNoMemory)
					{
					// If we cannot allocate enough memory for the cleanupstack frame, we also deallocate
					// the last memory block. This is mandatory for the correct functioning of the
					// following test cases
					// Note when an object that is attempted to be PushL to the cleanupstack and it fails
 					// the object will be freed automatically.
					popCount--;
					break;
					}
				else
					{
					CleanupStack::PushL(dummy);
					}
				}
			else
				{
				firstSize = myHeap.Size();
				firstAvail = myHeap.Available(biggestBlock);
				break;
				}
			};

		// Test if cache is as full as it should be
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == cachedRepositoryCount );

		// Try loading Med rep. Should Fail
		TRAP( res, CacheRepositoryL(KUidCacheTestRepositoryMed));

		TEST( res == KErrNoMemory );

		// Cache is still as it was before attempting load
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == cachedRepositoryCount );

		// Try loading Med rep with OOM trapping.
		TBool oomtrap = ETrue;
		TRAP( res, CacheRepositoryL(KUidCacheTestRepositoryMed, oomtrap));

		if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
			{
			// Should still fail because the cache was empty to begin with so there's nothing
			// to evict and no memory to gain
			TEST( res == KErrNoMemory );
			TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
			}
		else if (iStage == ESizeMinplus)
			{
			// Should still fail because although the cache was full, the amount of memory freed
			// will not be enough for the medium repository
			TEST( res == KErrNoMemory );
			if (oomtrap)
				{
				// Memory alloc failed even before open was attempted, so no cache flush happened
				TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == cachedRepositoryCount );
				}
			else
				{
				TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
				}
			}
		else
			{
			// Should Pass and evict repositories
			TEST( res == KErrNone );
			TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
			}

		CleanupStack::PopAndDestroy(popCount);
		}

	NextTest( _L( "Cache Disable/Enable" ) );

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	TServerResources::iCacheManager->EnableCache();

	CacheRepositoryL(KUidCacheTestRepositorySm);
	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		}

	TServerResources::iCacheManager->DisableCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	CacheRepositoryL(KUidCacheTestRepositoryMed);
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	TServerResources::iCacheManager->EnableCache();

	NextTest( _L( "Multi Client Test" ) );

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	// Creating two sets of server objects
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

	CSessionNotifier* notifier2 = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier2);

    CServerRepository* repository2 = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository2);

    // Open a rep with 1st client
    repository->OpenL(KUidCacheTestRepositorySm,*notifier);
    // Open same rep with 2nd client
    repository2->OpenL(KUidCacheTestRepositorySm,*notifier2);

    // One repository is in the cache, because now(PREQ 1228) all repositories
    // are moved to cache just after they are opened, not after they are closed.
	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		}
	// First client closed
	repository->Close();
	// Repository still in the cache
	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		}
	// Second client closed
	repository2->Close();

	// One rep must still be in the cache now
	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		}
	else
		{
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		}

	NextTest( _L( "Notify-Only Client Cache Repository (Open Only)" ) );

	TServerResources::iCacheManager->FlushCache();
	TServerResources::iObserver->CloseiOpenRepositories();

    // Open a rep with 1st client
    repository->OpenL(KUidCacheTestRepositorySm,*notifier);

	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No memory, so no cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		// But it's open
		TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );
		}
	else
		{
		// Repository should have been cached after open
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		// And it's open
		TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );
		}

	// First client closed
	repository->Close();

	if ((iStage == ESizeMin)||(iStage == EReallyWorstCase))
		{
		// No memory, so no cache
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
		// Now it's closed
		TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) == KErrNotFound );
		}
	else
		{
		// Still in cache because timeout hasn't occured
		TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
		// And still open
		TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );
		}

   	CleanupStack::PopAndDestroy(repository2);
   	CleanupStack::PopAndDestroy(notifier2);
   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1420
@SYMTestCaseDesc Test Coarse-Grained Caching forced eviction rules.
@SYMTestPriority High
@SYMTestActions  Check correct initialization of the cache manager, disabling/enabling
 cache with different cache size, filling, eventual forced eviction of repository under
 small and large memory cases
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ1192
@SYMTestStatus Defined
@SYMDevelopedForRelease Symbian OS v9.3
@SYMAuthor Aleks Pamir
*/
void TRepositoryCacheManagerTester::SizeTestsL()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1420 Forced Eviction-Small cache " ) );

	iTestStepStage = 1;
	TEST( TServerResources::iCacheManager != 0 );
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	// Flush and disable the cache
	TServerResources::iCacheManager->DisableCache(ETrue);

	// Calculate cache size for this test. It's important to calculate this at runtime,
	// because this size is approximate and it changes from emulator to hardware

	TServerResources::iCacheManager->EnableCache(KInvalidEvictionTimeout, KDefaultCacheSize);

	TInt smallCacheSize = CacheRepositoryL(KUidCacheTestRepositorySm);
	smallCacheSize+= CacheRepositoryL(KUidCacheTestRepositoryMed);
	smallCacheSize+= (CacheRepositoryL(KUidCacheTestRepositorySm2)/2);

	// smallCacheSize is KUidCacheTestRepositorySm+KUidCacheTestRepositoryMed+ half of KUidCacheTestRepositorySm2
	// the reason is we don't want KUidCacheTestRepositorySm2 to fit

	TServerResources::iCacheManager->DisableCache(ETrue);
	//Change cache size
	TServerResources::iCacheManager->EnableCache(KInvalidEvictionTimeout, smallCacheSize);

	// Fill cache
	CacheRepositoryL(KUidCacheTestRepositorySm);
	CacheRepositoryL(KUidCacheTestRepositoryMed);
	User::After(KTimerDelay);
	//This one will not fit, and the Small rep will be evicted because it's small
	CacheRepositoryL(KUidCacheTestRepositorySm2);

	TEST( !FindRepository(KUidCacheTestRepositorySm) );
	TEST( FindRepository(KUidCacheTestRepositorySm2) );

	TInt delay = TServerResources::iCacheManager->iDefaultTimeout.Int()-KTimerDelay;
	// If timeout is smaller then KTimerDelay, don't bother to wait
	if (delay>0)
		{
		User::After(delay);
		}
	//Medium will be evicted this time, because it's older and size difference doesn't make up for the time difference
	CacheRepositoryL(KUidCacheTestRepositorySm);

	TEST( !FindRepository(KUidCacheTestRepositoryMed) );
	TEST( FindRepository(KUidCacheTestRepositorySm) );

	if (!iOOMTest)
		{
		// This test is excluded from generic OOM testing because
		// 1. Loading Lrg repository within OOM takes forever because there're many allocations, and
		// the test is very similar to previous test

		NextTest( _L( "Forced Eviction-Large cache" ) );

		TServerResources::iCacheManager->DisableCache(ETrue);

		TServerResources::iCacheManager->EnableCache(KInvalidEvictionTimeout, KDefaultCacheSize);

		TInt largeCacheSize = CacheRepositoryL(KUidCacheTestRepositoryMed);
		largeCacheSize+= CacheRepositoryL(KUidCacheTestRepositoryLrg);
		largeCacheSize+= (CacheRepositoryL(KUidCacheTestRepositoryMed2)/2);

		// smallCacheSize is KUidCacheTestRepositorySm+KUidCacheTestRepositoryMed+ half of KUidCacheTestRepositorySm2
		// the reason is we don't want KUidCacheTestRepositorySm2 to fit

		TServerResources::iCacheManager->DisableCache(ETrue);

		//Change Parameters
		TServerResources::iCacheManager->EnableCache(KInvalidEvictionTimeout, largeCacheSize);
		// Fill cache
		CacheRepositoryL(KUidCacheTestRepositoryMed);
		CacheRepositoryL(KUidCacheTestRepositoryLrg);
		User::After(KTimerDelay);
		//This one will not fit, and the Med rep will be evicted because it's small
		CacheRepositoryL(KUidCacheTestRepositoryMed2);

		TEST( !FindRepository(KUidCacheTestRepositoryMed) );
		TEST( FindRepository(KUidCacheTestRepositoryLrg) );

		delay = TServerResources::iCacheManager->iDefaultTimeout.Int()-KTimerDelay;
		// If timeout is smaller then KTimerDelay, don't bother to wait
		if (delay>0)
			{
			User::After(delay);
			}

		//Medium2 will be evicted, because even if it's new, this time size difference makes up for the
		//time difference and the Large repository stays even if it's older
		CacheRepositoryL(KUidCacheTestRepositoryMed);

		TEST( !FindRepository(KUidCacheTestRepositoryMed2) );
		TEST( FindRepository(KUidCacheTestRepositoryLrg) );
		}
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1421
@SYMTestCaseDesc Test Coarse-Grained Caching normal eviction rules.
@SYMTestPriority High
@SYMTestActions  Check correct initialization of the cache manager, adding repository into
 cache and active object driven eviction at correct times.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ1192
@SYMTestStatus Defined
@SYMDevelopedForRelease Symbian OS v9.3
@SYMAuthor Aleks Pamir
*/
void TRepositoryCacheManagerTester::TimingTestsL()
	{

	iTestStepStage = 2;

	TEST( TServerResources::iCacheManager != 0 );
	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	CPeriodic* timer = CPeriodic::NewL(EPriorityLow);
	CleanupStack::PushL(timer);

	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1421 Cache Timing(Component Test) " ) );

	starttime.UniversalTime();
	// Add one repository in the cache
	CacheRepositoryL(KUidCacheTestRepositorySm);
	TTime now;
	now.UniversalTime();
	TheTest.Printf(_L("KUidCacheTestRepositorySm Added at %Ld\n"), now.MicroSecondsFrom(starttime).Int64());

	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );

	// Wait until the repository is evicted normally
	// We can't use User::After because we don't want to suspend the thread
	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()-KTimerDelay), 0, TCallBack(Callback, timer));

	CActiveScheduler::Start();

	// Callback should have been called before Cache timeout had occured
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	starttime.UniversalTime();
	// Add another repository in the cache
	CacheRepositoryL(KUidCacheTestRepositorySm2);
	now.UniversalTime();
	TheTest.Printf(_L("KUidCacheTestRepositorySm2 added at %Ld\n"), now.MicroSecondsFrom(starttime).Int64());

	// wait more than default timeout.
	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()+KTimerDelay), 0, TCallBack(Callback, timer));

	CActiveScheduler::Start();

	// Callback should have been called by now
	TInt count = TServerResources::iCacheManager->iIdleRepositories.Count();
	if (count > 0)
		{
		// Do not fail test on emulator. CTimer::AtUTC is often
		// late by 1 to a few seconds.
		#if defined(__WINSCW__) || defined(__WINS__)
		TheTest.Printf(_L("*** Line %d check fail. Ignored on winscw."), __LINE__);
		TServerResources::iCacheManager->FlushCache();
		#endif
		}

	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	NextTest( _L( "Notify-Only Client Timing Test" ) );

	// Add one repository in the cache
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

	starttime.UniversalTime();

    // Open a rep
    repository->OpenL(KUidCacheTestRepositorySm,*notifier);

	now.UniversalTime();
	TheTest.Printf(_L("KUidCacheTestRepositorySm Added at %Ld\n"), now.MicroSecondsFrom(starttime).Int64());

	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );

	// Wait until the repository is evicted normally
	// We can't use User::After because we don't want to suspend the thread
	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()-KTimerDelay), 0, TCallBack(Callback, timer));

	CActiveScheduler::Start();

	// Callback should have been called before Cache timeout had occured
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );

	TServerResources::iCacheManager->FlushCache();
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	// Add another repository in the cache
	// Add one repository in the cache
	CSessionNotifier* notifier2 = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier2);

    CServerRepository* repository2 = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository2);

	starttime.UniversalTime();

    // Open another rep
    repository2->OpenL(KUidCacheTestRepositorySm2,*notifier2);

	now.UniversalTime();
	TheTest.Printf(_L("KUidCacheTestRepositorySm2 added at %Ld\n"), now.MicroSecondsFrom(starttime).Int64());

	// wait more than default timeout.
	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()+KTimerDelay), 0, TCallBack(Callback, timer));

	CActiveScheduler::Start();

	// Callback should have been called by now
	count = TServerResources::iCacheManager->iIdleRepositories.Count();
	if (count > 0)
		{
		// Do not fail test on emulator. CTimer::AtUTC is often
		// late by 1 to a few seconds.
		#if defined(__WINSCW__) || defined(__WINS__)
		TheTest.Printf(_L("*** Line %d check fail. Ignored on winscw."), __LINE__);
		TServerResources::iCacheManager->FlushCache();
		#endif
		}

	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );

	repository2->Close();
	repository->Close();

   	CleanupStack::PopAndDestroy(repository2);
   	CleanupStack::PopAndDestroy(notifier2);
   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);

	CleanupStack::PopAndDestroy(timer);
	}

void TRepositoryCacheManagerTester::DefectTestsL()
	{
	iTestStepStage = 3;	// Don't run OOM tests on defects

	NextTest(_L( "DEF093491: [AQP]Centrep server flushes repository cache when temporarily disabled" ));
   	DEF093491L();

	NextTest(_L( "INC105967: CenRep crashes when no ROM directory" ));
   	INC105967();
   	
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1883
@SYMTestCaseDesc [AQP]Centrep server flushes repository cache when temporarily disabled
@SYMTestPriority High
@SYMTestActions  Open repository, disable cache, check if the rep is still in memory
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF093491
*/
void TRepositoryCacheManagerTester::DEF093491L()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1883 " ) );
	TServerResources::iCacheManager->FlushCache();

	// Notifier needed to open repositories.
	CSessionNotifier* notifier;
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    // Open repository
    repository->OpenL(KUidCacheTestRepositorySm,*notifier);

	// Check it's in cache and memory
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );
    // disable cache
	TServerResources::iCacheManager->DisableCache();

	// Check it's in memory but not in cache
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );

	repository->Close();
	// the repository should now be unloaded from memory
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) == KErrNotFound );
	// enable cache again
	TServerResources::iCacheManager->EnableCache();

    // Open repository again
    repository->OpenL(KUidCacheTestRepositorySm,*notifier);

	// Check it's in cache and memory
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 1 );
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound );

	repository->Close();

    // disable cache
	TServerResources::iCacheManager->DisableCache(ETrue);

	// now it should be flushed out of both memory and cache
	TEST( TServerResources::iCacheManager->iIdleRepositories.Count() == 0 );
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) == KErrNotFound );

   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);
	}

// Helper function for INC105967L
LOCAL_C TInt TestThread(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	HBufC* tempFileName;
	// this call shouldn't cause a panic, but leave with KErrNotFound
	TRAPD(err,
		{
		TServerResources::CreateRepositoryFileNameLC(tempFileName, KUidCacheTestRepositorySm, ERom, ECre);
		CleanupStack::PopAndDestroy(tempFileName);
		});
	// test if the function leaves with KErrNot Found
	TEST2(err, KErrNotFound);

	delete cleanup;
	return KErrNone;
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-3474
@SYMTestCaseDesc CenRep crashes when no ROM directory
@SYMTestPriority High
@SYMTestActions Save RomDirectory, temporarily NULL the pointer, call panicking function in a seperate thread
 (to survive even if it panics and continue other tests), check that it leaves with KErrNotFound, restore RomDirectory
@SYMTestExpectedResults The test must not panic.
@SYMDEF INC105967
*/
void TRepositoryCacheManagerTester::INC105967()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-3474 " ) );
	//store RomDirectory
	HBufC* tempRomDirectory = TServerResources::iRomDirectory;

	// temporarily delete it to simulate the behaviour in TServerResources::InitialiseL() when:
	// 	if(iFs.Entry(*iRomDirectory, fsEntry)!=KErrNone || !fsEntry.IsDir())
	TServerResources::iRomDirectory = NULL;

	RThread testThread;
	_LIT(KThreadName, "TestThread");

	testThread.Create(KThreadName, TestThread, KDefaultStackSize, KMinHeapSize, 0x100000, NULL);

	TRequestStatus requestStatus;
	// Request notification when the thread terminates
	testThread.Logon(requestStatus);
	// Let the thread execute
	testThread.Resume();

	// Wait for termination
	User::WaitForRequest(requestStatus);
	// test if the thread terminated normally (the leave has been trapped) not panicked
	TEST2(requestStatus.Int(), KErrNone);

	//restore RomDirectory
	TServerResources::iRomDirectory = tempRomDirectory;
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4077
@SYMTestCaseDesc Central Repository cache manager incorrectly uses CTimer::AtUTC() 
@SYMTestPriority High
@SYMTestActions Call CRepositoryCacheManager::RescheduleTimer passing in various
				timer values to test conversion of 64 bit UTC time value into
				32 bit microsecond value.
@SYMTestExpectedResults The timer should be active after each call to RescheduleTimer
						The test must not panic.
@SYMDEF DEF124147
*/
void TRepositoryCacheManagerTester::DEF124147()
	{
	NextTest( _L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4077 " ) );
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	
	TTime now;
	now.UniversalTime();
	
	TTimeIntervalMinutes oneMinute(1);
	TTimeIntervalHours oneHour(1);
	
	//Rechedule timer now
	TServerResources::iCacheManager->RescheduleTimer(now);
	TEST(TServerResources::iCacheManager->IsActive());
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	
	
	//Rechedule timer in the past
	TServerResources::iCacheManager->RescheduleTimer(now - oneMinute);
	TEST(TServerResources::iCacheManager->IsActive());
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	
	//Rechedule timer in the future
	TServerResources::iCacheManager->RescheduleTimer(now + oneMinute);
	TEST(TServerResources::iCacheManager->IsActive());
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	
	//Rechedule timer an hour in the past
	TServerResources::iCacheManager->RescheduleTimer(now - oneHour);
	TEST(TServerResources::iCacheManager->IsActive());
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	
	//Rechedule timer an hour in the future
	TServerResources::iCacheManager->RescheduleTimer(now + oneHour);
	TEST(TServerResources::iCacheManager->IsActive());
	
	//Cancel any pending timer
	TServerResources::iCacheManager->Cancel();
	}

// ---------------------------------------------------
// OomTest
//
// Function to convert a test into an OOM test

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1422
@SYMTestCaseDesc Test functionality under OOM.
@SYMTestPriority High
@SYMTestActions  Test functionality under OOM.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ1192
@SYMTestStatus Defined
@SYMDevelopedForRelease Symbian OS v9.3
@SYMAuthor Aleks Pamir
*/
LOCAL_C void OomTest( void (*testFuncL)() )
	{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1422 " ) );
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgMarkStart( RHeap::EUser );

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, ++count );

		TRAP( error, (testFuncL)() );

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		User::__DbgMarkEnd( RHeap::EUser, 0 );
		} while( error == KErrNoMemory );

	_LIT( KTestFailed, "Out of memory test failure on iteration %d\n" );
	__ASSERT_ALWAYS( error == KErrNone, TheTest.Panic( error, KTestFailed, count ) );

	TheTest.Printf( _L( "Out of memory tests succeeded at heap failure rate of %i\n" ), count );
	TheTest.Printf( _L( "Process handle count preserved\n" ) );
	TheTest.Printf( _L( "Thread handle count preserved\n" ) );
	}

LOCAL_C void DoOOMTestsL()
	{
	// To clean up the static array under OOM
	CleanupStack::PushL(gCleanup);

	// To be able to run tests faster, especially the last 2 tests with delays,
	// a flow-through to the next case statement is intended here. If the Tester
	// function completes successfully, meaning that it has passed all memory
	// allocations, then the execution continues from the next step and the iTestStepStage
	// variable is set to the current test step stage. Otherwise the function will Leave
	// with KErrNoMemory, will be trapped in OomTest and DoOOMTestsL will be recalled.
	// Once a test step stage is passed, Tester.iTestStepStage will ensure that the same
	// function will no be called over and over again for every memory allocation failure,
	// thereby reducing the running time of the test considerably.

	switch(Tester.iTestStepStage)
		{
		case 0:
			{
			Tester.FuncTestsL();
			}
		case 1:
			{
			Tester.SizeTestsL();
			}
		case 2:
			{
			Tester.TimingTestsL();
			}
		default:
			break;
		}
	// To clean up the memory left in static variables
	Tester.CleanUp(NULL);

	CleanupStack::Pop();
	}

LOCAL_C void DoTestsL()
	{
	switch(Tester.iStage)
		{
		case ENominalNoFile:
		default:
			{
			TheTest.Start( _L( "Cache functionality tests with default params(no .ini)" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			Tester.TimingTestsL();
			TheTest.End();
			break;
			}
		case ENominal:
			{
			TheTest.Start( _L( "Cache functionality tests with default params(default values read from .ini)" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			Tester.TimingTestsL();
			// Only test defects once here unless a defect occurs in different memory conditions
			Tester.DefectTestsL();
			TheTest.End();
			break;
			}
		case ESizeMin:
			{
			TheTest.Start( _L( "Cache functionality tests with no space for cache (size=0)" ) );
			Tester.FuncTestsL();
			// No size tests because size is controlled in this test
			// No Timing tests because nothing will be cached(cache size 0) so nothing to time
			TheTest.End();
			break;
			}
		case ESizeMinplus:
			{
			TheTest.Start( _L( "Cache functionality tests with small cache size" ) );
			Tester.FuncTestsL();
			// No size tests because size is controlled in this test
			Tester.TimingTestsL();
			TheTest.End();
			break;
			}
		case ESizeMaxminus:
			{
			TheTest.Start( _L( "Cache functionality tests with large cache size" ) );
			Tester.FuncTestsL();
			// No size tests because size is controlled in this test
			Tester.TimingTestsL();
			TheTest.End();
			break;
			}
		case ESizeMax:
			{
			TheTest.Start( _L( "Cache functionality tests with max cache size (=heap max 2MB)" ) );
			Tester.FuncTestsL();
			// No size tests because size is controlled in this test
			Tester.TimingTestsL();
			TheTest.End();
			break;
			}
		case ESizeMaxplus:
			{
			TheTest.Start( _L( "Cache functionality Robustness test" ) );
			Tester.FuncTestsL();
			// No size tests because size is controlled in this test
			Tester.TimingTestsL();
			TheTest.End();
			break;
			}
		case ETimeoutMin:
			{
			TheTest.Start( _L( "Cache functionality tests with no timeout for cache (timeout=0)" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			// No timing tests because timeout is controlled in this test
			TheTest.End();
			break;
			}
		case ETimeoutMinplus:
			{
			TheTest.Start( _L( "Cache functionality tests with short timeout" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			// No timing tests because timeout is controlled in this test
			TheTest.End();
			break;
			}
		case ETimeoutMaxminus:
			{
			TheTest.Start( _L( "Cache functionality tests with large timeout" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			// No timing tests because timeout is controlled in this test
			TheTest.End();
			break;
			}
		case EReallyWorstCase:
			{
			TheTest.Start( _L( "Cache functionality Worst Case test with no timeout and no size for cache" ) );
			Tester.FuncTestsL();
			Tester.SizeTestsL();
			// No Timing tests because nothing will be cached(cache size 0) so nothing to time
			TheTest.End();
			break;
			}
		};
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4014
@SYMTestCaseDesc Cenrep cache manager should not evict sub-sessions in active transactions.
@SYMTestPriority High
@SYMTestActions  Open 2 sub-sessions to a repository, start a transaction, let the cache manager run.
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF111734
*/
void TRepositoryCacheManagerTester::DEF111734L()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4014 DEF111734: Cache Manager and Open Transactions "));

	TServerResources::iCacheManager->FlushCache();

	// Notifier needed to open repositories.
	// notifier 1
	CSessionNotifier* notifier;
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

	// notifier 2
	CSessionNotifier* notifier2;
	notifier2 = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier2);

	// connection 1
 	CServerRepository* repository = new(ELeave) CServerRepository();
	CleanupStack::PushL(repository);

	// connection 2
	CServerRepository* repository2 = new(ELeave) CServerRepository();
 	CleanupStack::PushL(repository2);

	// Open repository
	repository->OpenL(KUidCacheTestRepositorySm,*notifier);

	// open second connection to the repository
	repository2->OpenL(KUidCacheTestRepositorySm,*notifier2);

	// we have to observers to the same repository - so we should have only 1 entry in
	// the idle repositories for the cache manager

	TEST(TServerResources::iCacheManager->iIdleRepositories.Count() == 1);

	// check CR's global memory for an instance for this repository.
	TEST(TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound);

	// start a transaction on connection 2, this adds this sub-session to the
	// transaction queue for the repository.

	TInt error = repository2->StartTransaction(3);
	TEST(error == KErrNone);

	// This will wipe out the memory for the open repository unless we modify the code in some fashion.
	TServerResources::iCacheManager->FlushCache(EFalse);

	// since we can't use the user::after method since we need to this thread to suspend
	// start a timer (stolen from above).
	CPeriodic* timer = CPeriodic::NewL(EPriorityLow);
	CleanupStack::PushL(timer);


	// since repositories that involved in active transactions no longer have their memory
	// removed by cache manager, give this time enough time to go through a few cycles of
	// the normal eviction process.

	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()*3), 0, TCallBack(Callback, timer));

	// will cause the cache manager to run
	CActiveScheduler::Start();

	// If it isn't in cache and memory than the cache manager must have collected it
	// and the code for this defect is not in the build.
	TEST(TServerResources::iCacheManager->iIdleRepositories.Count() == 0);
	TEST(TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) != KErrNotFound);

	// close this sub-session.  should cancel the active transaction. allowing cache manager to clean
	// up the memory.
	repository2->Close();

	TInt i = 0;
	const TUint32 KInt1 = 0x1;
	repository->Get(KInt1, i);

	// close the last observer for this repository.  this places the memory for this repository
	// back onto the idle list for cache manager.
	repository->Close();

	// Wait until the repository is evicted normally

	timer->Start(TTimeIntervalMicroSeconds32(TServerResources::iCacheManager->iDefaultTimeout.Int()*2), 0, TCallBack(Callback, timer));

	CActiveScheduler::Start();

	// the repository should now be unloaded from memory
	TEST( TServerResources::iObserver->FindOpenRepository(KUidCacheTestRepositorySm) == KErrNotFound );

   	CleanupStack::PopAndDestroy(5);
	}

LOCAL_C void MainL()
	{
	__UHEAP_MARK;
	TheTest.Start( _L( "Cache Tests" ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Install( s );

	Tester.DeleteFilesL();

	TServerResources::InitialiseL();

	
	Tester.DEF111734L();
	Tester.DEF124147();
	

	for(TInt i=ENominalNoFile; i<ELastStage; i++)
		{
		Tester.AdvanceToStageL( static_cast<TBorderTestStage>(i) );
		DoTestsL();
		}

	TheTest.Next( _L(  "Out of memory tests" ) );
	Tester.iOOMTest = ETrue;

	Tester.AdvanceToStageL( ENominal );
	OomTest( DoOOMTestsL );

	TServerResources::Close();

	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
#ifdef __SECURE_DATA__
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
		return KErrNoMemory;

	TRAPD( err, MainL()  );
	if ( err != KErrNone )
		User::Panic( _L( "Testing failed: " ), err );

	delete cleanup;
	__UHEAP_MARKEND;
#endif

	return 0;
	}

