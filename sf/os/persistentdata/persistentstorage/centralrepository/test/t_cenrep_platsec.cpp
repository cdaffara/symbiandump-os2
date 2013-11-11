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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32capability.h>
#include <e32test.h>
#include <f32file.h>

#include "../cenrepsrv/srvparams.h"
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Platform Security Tests"));

TBool OomTesting;

const TUid KUidPlatsecTestRepository = { 0x00000100 };
const TUid KUidTransactionTestRepository = { 0x00000101 };
const TUid KUidResetTestRepository = { 0x00000004 };

const TInt KMaxNumRepsToReset = 3;

//
// Platsec test repository
//

const TUint32 KNonExisitent = 0;

const TInt KNumSettings = 26;

const TUint32 KInt1 = 1;
const TInt KInt1_InitialValue = 1;
const TInt KInt1_UpdatedValue = 73;
const TUint32 KNewInt = 1000;
//
const TUint32 KIntAlwaysPass = 0x201;
const TInt KIntAlwaysPass_Value = 200;
const TUint32 KIntAlwaysPassFail = 0x202;

const TUint32 KReal1 = 2;
const TReal KReal1_InitialValue = 2.732;
const TReal KReal1_UpdatedValue = 72.8;
const TUint32 KNewReal = 2000;

const TUint32 KString1 = 5;
_LIT(KString1_InitialValue, "test\\\"string\"");
_LIT(KString1_UpdatedValue, "another one");
const TUint32 KNewString8 = 3000;
const TUint32 KNewString16 = 4000;

RArray<TUid> RepsToReset;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		CleanupCDriveL();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// Function to compare two TUid's.
LOCAL_C TInt CompareUid(const TUid& aUid1, const TUid& aUid2)
	{
	return aUid1.iUid - aUid2.iUid;
	}

LOCAL_C void AddRepositoryToReset(const TUid& aRepositoryUid)
	{
	// Use InsertInOrder not because of speed but to eliminate
	// duplicates.
	RepsToReset.InsertInOrder(aRepositoryUid, CompareUid);
	}

// NB: this test suite needs each test run to start with keyspaces with values from
// the initialization files in ROM dir. The best way to archive this is to send the
// EGetSetParameters IPC to centrep server to flush cache and then delete
// the .cre files. This option requires the CENTREP_SERVER_CACHETEST macro
// which is not available in release ONB.  The second choice is to wait enough
// time for centrep server to flush out cache, then delete the .cre files from
// persists dir. This approach does not work well on the emulator because centrep
// server's cache timer sometimes is late by as much as a few seconds.
// Hence instead of wait and delete the .cre files, this RTest uses the ResetAll
// API. This trick works for the use case of this RTest but may not work in other
// situations. E.g., in this RTest centrep server always open repositories
// from cache. This changes the timing pattern and does not exercise the
// cache eviction code path.
LOCAL_C void ResetModifiedRepsL()
	{
	for (TInt i = RepsToReset.Count() - 1; i >= 0; i--)
		{
		CRepository* rep;
		User::LeaveIfNull( rep = CRepository::NewL(RepsToReset[i]) );
		rep->Reset();
		delete rep;

		RepsToReset.Remove(i);
		}
	}

LOCAL_C void OpenRepositoryL()
	{
	CRepository* repositoryA;

	TInt r;

	TRAP(r, CRepository::NewL(TUid::Null()));
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNotFound);

	repositoryA = CRepository::NewLC(KUidPlatsecTestRepository);

	TInt a;
	r = repositoryA->Get(KInt1, a);
	TEST2(r, KErrNone);
	TEST(a==KInt1_InitialValue);

	CleanupStack::PopAndDestroy(repositoryA);

	}

LOCAL_C void GetSetL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidPlatsecTestRepository);

	//
	// Integer get/set
	//

	TInt i = 0;

	TInt r = repository->Get(KNonExisitent, i);
	TEST2(r, KErrNotFound);
	TEST(i==0);

	// no capability specified so deafult should do and should succeed
	r = repository->Get(KReal1, i);
	TEST2(r, KErrArgument);
	TEST(i==0);

	// no capability specified so deafult should do and should succeed
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_InitialValue);

	r = repository->Set(KNonExisitent, 0);
	TEST2(r, KErrNone);

	r = repository->Delete(KNonExisitent);
	TEST2(r, KErrNone);

	r = repository->Set(KReal1, 0);
	TEST2(r, KErrArgument);

	r = repository->Set(KInt1, KInt1_UpdatedValue);
	TEST2(r, KErrNone);

	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);

	r = repository->Get(KIntAlwaysPass, i);
	// range policy AlwaysPass
	TEST2(r, KErrNone);
	TEST(i==KIntAlwaysPass_Value);

	i=0;
	r = repository->Get(KIntAlwaysPassFail, i);
	// range policy overidden
	TEST2(r, KErrPermissionDenied);
	TEST(i!=KIntAlwaysPass_Value);


	//
	// Real get/set
	//

	TReal y = 0;

	r = repository->Get(KNonExisitent, y);
	TEST2(r, KErrNotFound);
	TEST(y==0);

	r = repository->Get(KInt1, y);
	TEST2(r, KErrArgument);
	TEST(y==0);

	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_InitialValue);

	r = repository->Set(KNonExisitent, 0.0);
	TEST2(r, KErrNone);

	r = repository->Delete(KNonExisitent);
	TEST2(r, KErrNone);

	r = repository->Set(KInt1, 0.0);
	TEST2(r, KErrArgument);

	r = repository->Set(KReal1, KReal1_UpdatedValue);
	TEST2(r, KErrNone);

	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	//
	// String get/set
	//

	TBuf<20> str;

	r = repository->Get(KNonExisitent, str);
	TEST2(r, KErrNotFound);
	TEST(str.Length()==0);

	r = repository->Get(KInt1, str);
	TEST2(r, KErrArgument);
	TEST(str.Length()==0);

	r = repository->Get(KString1, str);
	PlatSec platsec;

	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(str==KString1_InitialValue);
		}
	else
		TEST2(r, KErrPermissionDenied);


	TBuf<10> tooShort;
	r = repository->Get(KString1, tooShort);

	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrOverflow);
		TEST(tooShort.Length()==0);
		}
	else
		TEST2(r, KErrPermissionDenied);


	r = repository->Set(KNonExisitent, str);
	TEST2(r, KErrNone);

	r = repository->Delete(KNonExisitent);
	TEST2(r, KErrNone);

	r = repository->Set(KInt1, str);
	TEST2(r, KErrArgument);

	r = repository->Set(KString1, KString1_UpdatedValue);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		}
	else
		TEST2(r, KErrPermissionDenied);

	r = repository->Get(KString1, str);

	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(str==KString1_UpdatedValue);
		}
	else
		TEST2(r, KErrPermissionDenied);

	const TInt KStringLengthTooBig = KMaxUnicodeStringLength  + 1;
	TBuf<KStringLengthTooBig> tooBig;
	tooBig.SetLength(KStringLengthTooBig);
	r = repository->Set(KString1, tooBig);
	TEST2(r, KErrArgument);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void CreateSetIntTransactionL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTransactionTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidTransactionTestRepository);

	//
	// Integer get/set
	//
	TInt r;
	TInt i = 0;
	const TInt imax = 0x0ff;
	TUint32 errorId;

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	repository->CleanupRollbackTransactionPushL();
	TEST2(r, KErrNone);

	// create
	TInt KIntStartValue = 100;
	const TUint32 KNewIntBase = 1;
	for(i=0;i<imax;i++)
		{
		r = repository->Create(KNewIntBase+i, KIntStartValue+i);
		if(OomTesting && (r==KErrNoMemory))
			User::Leave(KErrNoMemory);
		else
			TEST2(r, KErrNone);
		}

	r = repository->CommitTransaction(errorId);
	CleanupStack::Pop();
	if(OomTesting && r!=KErrNone)
		{
		// in OOM test we may have alredy created settings
		TEST2(r, KErrAlreadyExists);
		}
	else
		TEST2(r, KErrNone);

	TInt integer;
	if(!(OomTesting && r==KErrAlreadyExists))
	{
		// during OOM tests we have values from Sets rather then Create...
		for(i=0;i<imax;i++)
			{
			r = repository->Get(KNewIntBase+i,integer );
			TEST2(r, KErrNone);
			TEST(KIntStartValue+i==integer);
			}
	}
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

    repository->CleanupRollbackTransactionPushL();

	// set
	KIntStartValue = 200;
	for(i=0;i<imax;i++)
		{
		r = repository->Set(KNewIntBase+i, KIntStartValue+i);
		if(OomTesting && (r==KErrNoMemory))
			User::Leave(KErrNoMemory);
		else
			TEST2(r, KErrNone);
		}

	r = repository->CommitTransaction(errorId);
	CleanupStack::Pop();
	TEST2(r, KErrNone);
	for(i=0;i<imax;i++)
		{
		r = repository->Get(KNewIntBase+i,integer );
		TEST2(r, KErrNone);
		TEST(KIntStartValue+i==integer);
		}

	// lets check set where some are going to fail
	// set
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	repository->CleanupRollbackTransactionPushL();
	TEST2(r, KErrNone);
	KIntStartValue = 400;
	TInt base = KNewIntBase + 0x7f; // half the range ok other half should fail
	for(i=0;i<imax;i++)
		{
		r = repository->Set(base+i, KIntStartValue+i);
		if(OomTesting && (r==KErrNoMemory))
			User::Leave(KErrNoMemory);
		else
		    {
		    if (base + i < 0x100)
		        {
			    TEST2(r, KErrNone);
		        }
		    else if (base + i == 0x100)
		        {
			    TEST2(r, KErrNone);
		        // this causes transaction to fail
                const TReal KRealValue = 1.234;
		        r = repository->Set(base+i, KRealValue);
			    TEST2(r, KErrArgument);
		        }
		    else
		        {
		        // after transaction has failed, repository returns KErrAbort for every request
			    TEST2(r, KErrAbort);
		        }
		    }
		}

	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrArgument);
	TEST2(errorId, 0x100);
	CleanupStack::Pop();
	// should be able to open again
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	repository->RollbackTransaction();

	// and after the failed transaction it should be in previous state!!
	KIntStartValue = 200;
	for(i=0;i<imax;i++)
		{
		r = repository->Get(KNewIntBase+i,integer );
		TEST2(r, KErrNone);
		TEST(KIntStartValue+i==integer);
		}

	// lets clear after ourselves
	for(i=0;i<imax;i++)
		{
		r = repository->Delete(KNewIntBase+i);
		TEST2(r, KErrNone);
		}

	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void CreateSetDesTransactionL()
	{
	TUint32 errorId;
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTransactionTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidTransactionTestRepository);


	_LIT(KString1_TestValue, "test string 1");
	_LIT(KString2_TestValue, "test string 2");
	_LIT(KString3_TestValue, "test string 3");
	_LIT(KString4_TestValue, "test string 4");
	_LIT(KString5_TestValue, "test string 5");
	_LIT(KString1_UpdatedTestValue, "updated test string 1");
	_LIT(KString2_UpdatedTestValue, "updated test string 2");
	//
	// Descriptor get/set
	//
	TInt r;


	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	repository->CleanupRollbackTransactionPushL();
	TEST2(r, KErrNone);

	// create
	r = repository->Create(1, KString1_TestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);
	r = repository->Create(2, KString2_TestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);
	r = repository->Create(3, KString3_TestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);
	r = repository->Create(4, KString4_TestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);
	r = repository->Create(5, KString5_TestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);

	r = repository->CommitTransaction(errorId);
	CleanupStack::Pop();
	if(OomTesting && r!=KErrNone)
		{
		// in OOM test we may have alredy created settings
		TEST2(r, KErrAlreadyExists);
		}
	else
		TEST2(r, KErrNone);

	TBuf<50> buf;
	if(!OomTesting)
		{
		r = repository->Get(1,buf);
		TEST2(r, KErrNone);
		TEST(buf==KString1_TestValue);
		r = repository->Get(2,buf);
		TEST2(r, KErrNone);
		TEST(buf==KString2_TestValue);
		r = repository->Get(3,buf);
		TEST2(r, KErrNone);
		TEST(buf==KString3_TestValue);
		r = repository->Get(4,buf);
		TEST2(r, KErrNone);
		TEST(buf==KString4_TestValue);
		r = repository->Get(5,buf);
		TEST2(r, KErrNone);
		TEST(buf==KString5_TestValue);
		}

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	repository->CleanupRollbackTransactionPushL();
	TEST2(r, KErrNone);

	// set
	r = repository->Set(1, KString1_UpdatedTestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);
	r = repository->Set(2, KString2_UpdatedTestValue);
	if(OomTesting && (r==KErrNoMemory))
		User::Leave(KErrNoMemory);
	else
		TEST2(r, KErrNone);

	r = repository->CommitTransaction(errorId);
	CleanupStack::Pop();
	TEST2(r, KErrNone);

	TBuf<50> buf1,buf2;
	r = repository->Get(1,buf1);
	TEST2(r, KErrNone);
	r = repository->Get(2,buf2);
	TEST2(r, KErrNone);

	TEST(buf1==KString1_UpdatedTestValue);
	TEST(buf2==KString2_UpdatedTestValue);

	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void CreateSetRealTransactionL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTransactionTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidTransactionTestRepository);

	//
	// Real transaction get/set
	//
	TInt r;
	TInt i = 0;
	const TInt imax = 0x0ff;

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// create
	const TReal KRealStartValue = 100.05;
	const TUint32 KNewRealBase = 1;
	for(i=0;i<imax;i++)
		{
		r = repository->Create(KNewRealBase+i, KRealStartValue+i);
		TEST2(r, KErrNone);
		}

	TUint32 errorId;
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	TReal real, real2;
	for(i=0; i<imax; i++)
		{
		r = repository->Get((KNewRealBase+i), real );
		real2 = KRealStartValue+i;
		TEST2(r, KErrNone);
		TEST( real2 == real );
		}

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void ResetTransactionL()
	{
#ifdef PENDING_REMOVAL
    // reset operations are not supported in transactions for now.
    // ResetAll will not ever be: still to decide on individual setting reset
    // Both are very slow.

	TUint32 errorId;
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidPlatsecTestRepository);

	TInt r = repository->Set(KInt1, KInt1_UpdatedValue);
	TEST2(r, KErrNone);

	TInt x,y;
	r = repository->Get(KInt1, x);
	TEST2(r, KErrNone);

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository->Reset(KInt1);
	TEST2(r, KErrNone);

	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	r = repository->Get(KInt1, y);
	TEST2(r, KErrNone);

	// did we get what expected after commit?
	TEST(x!=KInt1_InitialValue);
	TEST(y==KInt1_InitialValue);

	CleanupStack::PopAndDestroy(repository);

	// Repository B
	// lets prepare for reset
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTransactionTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidTransactionTestRepository);

	const TInt KNewSettingAlwaysPass = 0x101;

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	TInt i = 0;
	for(i=KNewSettingAlwaysPass;i<KNewSettingAlwaysPass+10;i++)
		{
		r = repository->Create(i, i);
		TEST2(r, KErrNone);
		}


	TRequestStatus status = -1;
	r = repository->NotifyRequest(0, 0, status);
	TEST2(r, KErrNone);

	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	TInt xArray[10];
	TInt xx=0;
	for(i=0;i<10;i++)
		{
		r = repository->Get(KNewSettingAlwaysPass+i, xArray[i]);
		TEST2(r, KErrNone);
		}

	//and do the reset business now
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository->Reset();
	TEST2(r, KErrNone);

	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// test reset after commit
	TEST(status==KNewSettingAlwaysPass);

	for(i=0;i<10;i++)
		{
		TEST(xArray[i]==(KNewSettingAlwaysPass+i));
		}

	for(i=KNewSettingAlwaysPass;i<KNewSettingAlwaysPass+10;i++)
		{
		r = repository->Get(i, xx);
		TEST2(r, KErrNotFound);
		}

	CleanupStack::PopAndDestroy(repository);
#endif // PENDING_REMOVAL
	}

LOCAL_C void FindL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	RArray<TUint32> foundIds;
	CleanupClosePushL(foundIds);

	TInt r = KErrNone;

	TRAPD(leaveReason, r = repository->FindL(0, 0, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	// Note that we are allowed to find any setting regardless of it's platsec
	// what we can't do is return the value
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);

	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(23, 0, foundIds));  // 23 - just a random number, value is not important
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);

	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(0, 2, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==22);

	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(4, 6, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);

	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(15, 15, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);

	CleanupStack::PopAndDestroy(); //foundIds
	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void NotifyL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));
	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidPlatsecTestRepository);

	PlatSec platsec;

	//
	// Notification on non-existent setting
	//
	TRequestStatus intStatus;
	TInt r = repository->NotifyRequest(KNonExisitent, intStatus);
	TEST2(r, KErrNotFound);


	//
	// Basic notification
	//
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);

	TRequestStatus realStatus;
	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);

	TRequestStatus stringStatus;
	r = repository->NotifyRequest(KString1, stringStatus);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}


	// Setting to the same value should not cause a notification
	TInt intval;
	r = repository->Get(KInt1, intval);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, intval);
	TEST2(r, KErrNone);
	TEST(intStatus==KRequestPending);
	TEST(realStatus==KRequestPending);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);
	RThread thisThread;
	TEST(thisThread.RequestCount()==0);

	// First change to setting should cause notification
	r = repository->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KInt1);
	TEST(realStatus==KRequestPending);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);

	// Second change to setting should not cause notification
	intStatus = 7777;
	r = repository->Set(KInt1, 0);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==KRequestPending);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);


	// Setting to the same value should not cause a notification
	TReal realval;
	r = repository->Get(KReal1, realval);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, realval);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==KRequestPending);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	r = repository->Set(KReal1, 0.0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==7777);
	TEST(realStatus==KReal1);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);


	// Setting to the same value should not cause a notification
	realStatus = 8888;
	TBuf<20> stringval;
	r = repository->Get(KString1, stringval);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST2(r, KErrNone);
	else
		TEST2(r, KErrPermissionDenied);
	r = repository->Set(KString1, stringval);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST2(r, KErrNone);
	else
		TEST2(r, KErrPermissionDenied);
	TEST(intStatus==7777);
	TEST(realStatus==8888);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	_LIT(KStringValue2, "string2");
	r = repository->Set(KString1, KStringValue2);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		User::WaitForAnyRequest();
		}
	else
		TEST2(r, KErrPermissionDenied);

	TEST(intStatus==7777);
	TEST(realStatus==8888);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		TEST(stringStatus==KString1);


	//
	// Cancelling single notifications
	//
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(KInt1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	intStatus = 7777;
	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, 1);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, 1.1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==7777);
	TEST(realStatus==KReal1);
	TEST(thisThread.RequestCount()==0);


	//
	// Cancelling all notifications
	//
	realStatus = 8888;
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);

	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
	TEST(realStatus==KUnspecifiedKey);

	intStatus = 7777;
	realStatus = 8888;
	r = repository->Set(KInt1, 2);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, 2.2);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==8888);
	TEST(thisThread.RequestCount()==0);

	//
	// Group notifications
	//
	r = repository->NotifyRequest(0, 0, intStatus);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	// we do not have capabilities for all settings
	TEST(intStatus==KErrPermissionDenied);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void CreateDeleteL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));
	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidPlatsecTestRepository);

	TInt x;
	TInt r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	const TInt KIntValue = 1234;
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNone);
	TEST(x==KIntValue);

	r = repository->Delete(KNewInt);
	TEST2(r, KErrNone);

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNone);
	TEST(x==KIntValue);

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNone);
	TEST(x==KIntValue);

	TReal y;
	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNotFound);

	const TReal KRealValue = 5678.0;
	r = repository->Create(KNewReal, KRealValue);
	TEST2(r, KErrNone);

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNone);
	TEST(y==KRealValue);

	r = repository->Delete(KNewReal);
	TEST2(r, KErrNone);

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewReal, KRealValue);
	TEST2(r, KErrNone);

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNone);
	TEST(y==KRealValue);

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNone);
	TEST(y==KRealValue);

	TBuf8<20> z8;
	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNotFound);

	_LIT8(KString8Value, "ABCDEF");
	r = repository->Create(KNewString8, KString8Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNone);
	TEST(z8==KString8Value);

	r = repository->Delete(KNewString8);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewString8, KString8Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNone);
	TEST(z8==KString8Value);

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNone);
	TEST(z8==KString8Value);

	TBuf16<20> z16;
	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNotFound);

	_LIT(KString16Value, "ghijklmn");
	r = repository->Create(KNewString16, KString16Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNone);
	TEST(z16==KString16Value);

	r = repository->Delete(KNewString16);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewString16, KString16Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNone);
	TEST(z16==KString16Value);

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNone);
	TEST(z16==KString16Value);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void MultiClientL()
	{
	CRepository* repository1;
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidPlatsecTestRepository));
	CRepository* repository2;
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidPlatsecTestRepository));

	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidPlatsecTestRepository);

	//
	// Get/Set
	//
	TInt i;
	TInt r = repository1->Set(KInt1, 0);
	TEST2(r, KErrNone);
	r = repository2->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==0);

	r = repository2->Set(KInt1, 123);
	TEST2(r, KErrNone);
	r = repository1->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==123);

	//
	// Notification
	//
	TRequestStatus status;
	r = repository1->NotifyRequest(KInt1, status);
	TEST2(r, KErrNone);

	r = repository2->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status==KInt1);

	CleanupStack::PopAndDestroy(2);
	}

LOCAL_C void ResetL()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	TInt x;
	TInt r = repository->Get(KInt1, x);
	TEST2(r, KErrNone);
	TEST(x!=KInt1_InitialValue);

	r = repository->Reset(KInt1);
	TEST2(r, KErrNone);

	r = repository->Get(KInt1, x);
	TEST2(r, KErrNone);
	TEST(x==KInt1_InitialValue);

	CleanupStack::PopAndDestroy(repository);

	// Repository D
	User::LeaveIfNull(repository = CRepository::NewLC(KUidResetTestRepository));
	// Remember that this repository need to be reset
	::AddRepositoryToReset(KUidResetTestRepository);


	const TInt KNewSetting1 = 5;
	const TInt KNewSetting2 = 6;
	r = repository->Create(KNewSetting1, 0);
	TEST2(r, KErrNone);
	r = repository->Create(KNewSetting2, 0);
	TEST2(r, KErrNone);

	r = repository->Create(65, 1);
	TEST2(r, KErrNone);

	r = repository->Set(70, 1);
	TEST2(r, KErrNone);

	r = repository->Create(80, 1);
	TEST2(r, KErrNone);

	r = repository->Create(90, 1);
	TEST2(r, KErrNone);

	TRequestStatus status = -1;
	r = repository->NotifyRequest(0, 0, status);
	TEST2(r, KErrNone);

	r = repository->Reset();
	TEST2(r, KErrNone);
	TEST(status==KUnspecifiedKey);
	RThread thisThread;
	TEST(thisThread.RequestCount()==1);

	r = repository->Get(10, x);
	TEST2(r, KErrNone);
	TEST(x==10);

	r = repository->Get(40, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repository->Get(50, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repository->Get(60, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repository->Get(70, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	TBuf<10> z;
	r = repository->Get(20, z);
	TEST2(r, KErrNone);
	TEST(z==_L("string"));

	TReal y;
	r = repository->Get(30, y);
	TEST2(r, KErrNone);
	TEST(y==1);

	r = repository->Get(5, x);
	TEST2(r, KErrNotFound);
	r = repository->Get(6, x);
	TEST2(r, KErrNotFound);
	r = repository->Get(65, x);
	TEST2(r, KErrNotFound);
	r = repository->Get(80, x);
	TEST2(r, KErrNotFound);
	r = repository->Get(90, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void FuncBasicTestsL()
	{
	TheTest.Start(_L("Open/Close repository"));
	OpenRepositoryL();

	TheTest.Next(_L("Get/Set"));
	GetSetL();

	TheTest.Next(_L("Find"));
	FindL();

	TheTest.Next(_L("Notify"));
	NotifyL();

	TheTest.Next(_L("Create/Delete"));
	CreateDeleteL();

	TheTest.Next(_L("Multiple clients"));
	MultiClientL();

	TheTest.Next(_L("Restore factory settings"));
	ResetL();

	TheTest.End();
	}

LOCAL_C void FuncTransactionTestsL()
	{
	TheTest.Start(_L("Open/Close repository"));
	OpenRepositoryL();

	TheTest.Next(_L("Create/Set Int Transaction"));
	CreateSetIntTransactionL();

	TheTest.Next(_L("Create/Set Real Transaction"));
	CreateSetRealTransactionL();

	TheTest.Next(_L("Create/Set Descriptor Transaction"));

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();

	CreateSetDesTransactionL();

	TheTest.Next(_L("Restore factory settings"));
	ResetTransactionL();

	TheTest.End();
	}


// ---------------------------------------------------
// OomTest
//
// Function to convert a test into an OOM test

LOCAL_C void OomTest(void (*testFuncL)())
	{
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++count);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(error, (testFuncL)());
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(error == KErrNoMemory);

	_LIT(KTestFailed, "Out of memory test failure on iteration %d\n");
	__ASSERT_ALWAYS(error==KErrNone, TheTest.Panic(error, KTestFailed, count));

	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0032
@SYMTestCaseDesc BasicPlatsecTestsL() function calls test for the Secure Cenrep (setting).
	Methods of CRepository class are called for various combination of different settings
	and the results are asserted.
@SYMTestPriority High
@SYMTestActions  BasicPlatsecTestsL() method calls test.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ277
         PREQ280
*/
LOCAL_C void BasicPlatsecTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0032 Basic functional tests for PlatSec "));
	OomTesting = EFalse;

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	RepsToReset.AppendL(KUidPlatsecTestRepository);
	RepsToReset.AppendL(KUidTransactionTestRepository);
	RepsToReset.AppendL(KUidResetTestRepository);
	::ResetModifiedRepsL();

	FuncBasicTestsL();

	TheTest.Next(_L("Basic Out-of-memory tests"));

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();

	OomTesting = ETrue;
	TheTest.Next(_L("OOM Open/Close repository"));
	OomTest(OpenRepositoryL);

	GetSetL();

	TheTest.Next(_L("OOM Find"));
	OomTest(FindL);

	TheTest.End();
	}
/**
@SYMTestCaseID SYSLIB-CENREP-CT-0033
@SYMTestCaseDesc TransactionPlatsecTestsL() function calls test for the Secure Cenrep (setting).
	Methods of CRepository class are called for various combination of different settings
	in a 'transaction' mode and the results are asserted.
@SYMTestPriority High
@SYMTestActions  TransactionPlatsecTestsL() method calls test.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ277
         PREQ280
*/
LOCAL_C void TransactionPlatsecTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0033 Transaction functional tests for PlatSec "));
	OomTesting = EFalse;

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();

	FuncTransactionTestsL();

	TheTest.Next(_L("Transaction Out-of-memory tests"));

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();

	OomTesting = ETrue;
	TheTest.Next(_L("OOM Open/Close repository"));
	OomTest(OpenRepositoryL);

	TheTest.Next(_L("OOM Transaction Int"));
	OomTest(CreateSetIntTransactionL);

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();


	TheTest.Next(_L("OOM Transaction Descriptors"));
	OomTest(CreateSetDesTransactionL);

	// Instead of wait for cache clear, this test uses the ResetAll API to
	// restore keyspaces to original ROM settings. See the notes at implementation
	// of ResetModifiedRepsL which explains why this is done and possible
	// undesirable effects.
	::ResetModifiedRepsL();

	TheTest.End();
	}

LOCAL_C void ServerPersistenceTest()
	{
	// Wait for a while after last test and ensure server is still running
	User::After(KGeneralDelay);

	_LIT( KCentralRepositoryServerName, "Centralrepository*");
	TFullName serverName;
	TFindProcess serverFind(KCentralRepositoryServerName);
	TEST2(serverFind.Next(serverName), KErrNone);
	}

LOCAL_C void MainL()
	{
	// allocate slots ahead of time to avoid problems with OOM tests.
	RepsToReset.ReserveL(KMaxNumRepsToReset);

	TheTest.Start(_L("Tests for basic PlatSec"));
	BasicPlatsecTestsL();

	TheTest.Next(_L("Tests for Transaction PlatSec"));
	TransactionPlatsecTestsL();

	TheTest.Next(_L("Server persistence test"));
	ServerPersistenceTest();

	TheTest.End();
	TheTest.Close();

	RepsToReset.Reset();
	}

TInt E32Main()
	{
	//
	// For the tests to work we need SID policing enforced plus the specific
	// capabilities listed below.
	//
	// These are dependent on the capabilities set in the platform security
	// repository test initialisation file 87654321.txt.  If the content
	// of that file changes then the following clauses may need to be
	// updated.
	//
	if(!PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityNetworkServices) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityDRM) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityLocalServices) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityCommDD))
		{
		TheTest.Start(_L("NOTE: Skipping tests due to incompatible PlatSec enforcement settings"));
		TheTest.End();
		TheTest.Close();
		return 0;
		}

	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
