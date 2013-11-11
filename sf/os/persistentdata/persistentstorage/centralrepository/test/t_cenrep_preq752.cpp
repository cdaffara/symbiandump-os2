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
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include "e32math.h"
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"
#include "../cenrepsrv/srvparams.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository PREQ752 Tests"));

TBool OomTesting;

const TUid KUidPlatsecTestRepository 	= { 0x00000100 };
const TUid KUidCorruptHeaderRepository  = { 0xBADBADBA };
const TUid KUidCorruptSettingRepository = { 0x00000003 };

//
// Repository A
//

const TUint32 KNonExistent = 0;

const TInt KNumSettings = 26;
const TInt KNumUnprotectedSettings = 11;
const TUint32 KUnprotectedSettingsMask = 0xFF000000 ;
const TUint32 KInt1 = 1;
const TInt KInt1_InitialValue = 1;
const TUint32 KNewInt = 1000;
const TInt KIntValue = 1234;

const TUint32 KReal1 = 2;
const TReal KReal1_InitialValue = 2.732;
const TUint32 KReal2 = 8;
const TReal KReal2_InitialValue = 1.5;
const TUint32 KReal3 = 17;

const TUint32 KString1 = 5;
_LIT(KString1_InitialValue, "test\\\"string\"");

const TUint32 KLongString = 0x300 ;

const TUint32 KMoveSource            = 0x02000000  ;
const TUint32 KMoveTarget            = 0x02000001  ;
const TUint32 KMoveSourceDoesntExist = 0x01000000 ;
const TUint32 KMoveTargetExists      = 0x03000000  ;
const TUint32 KMoveForbidden         = 0x04000000  ;
const TUint32 KMoveMask              = 0xFF0000FF ;

_LIT(KCreFile,                  "c:\\private\\10202BE9\\persists\\00000100.cre");
_LIT(KZCorruptSettingFile, 		"z:\\private\\10202BE9\\00000003.txt");
_LIT(KZCorruptHeaderFile, 		"z:\\private\\10202BE9\\00000bad.cre");


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
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void KillCentRepServerL()
	{
	_LIT(KCentralRepositoryServerName, "Centralrepositorysrv");
	_LIT(KProcessKillProcess, "t_processkillprocess.exe");
	
	TRequestStatus stat;    
	RProcess p;
	User::LeaveIfError(p.Create(KProcessKillProcess, KCentralRepositoryServerName));

	// Asynchronous logon: completes when process terminates with process 
	// exit code
	p.Logon(stat);
	p.Resume();
	User::WaitForRequest(stat);

	TExitType exitType = p.ExitType();
	TInt exitReason = p.ExitReason();
	
	_LIT(KKillServerFailed, "Killing Central Repository Server Failed");
	__ASSERT_ALWAYS(exitType == EExitKill, User::Panic(KKillServerFailed, exitReason));

	p.Close();
	User::LeaveIfError(exitReason);
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0080
@SYMTestCaseDesc Tests reach through Find operations during transactions.
@SYMTestPriority High
@SYMTestActions  Start a transaction and attempt Find operations.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void FindL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0080 "));
	TInt r;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	PlatSec platsec;

	// Test we can do some FindL's in a transaction

	r = repository->StartTransaction(CRepository::EReadTransaction);
	repository->CleanupRollbackTransactionPushL();
	TEST2(r, KErrNone);

	RArray<TUint32> foundIds;
	CleanupClosePushL(foundIds);

	TRAPD(leaveReason, r = repository->FindL(0, KUnprotectedSettingsMask, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}

	// Note that we are allowed to find any setting regardless of it's platsec
	// what we can't do is return the value
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumUnprotectedSettings);
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(23, KUnprotectedSettingsMask, foundIds));  // 23 - just a random number, value is not important
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumUnprotectedSettings);
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(0, KUnprotectedSettingsMask | 2, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==7);
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindL(4, KUnprotectedSettingsMask | 6, foundIds));
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
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, KInt1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==1);
		TEST(foundIds[0]==KInt1);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, 0, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNotFound);
		TEST(foundIds.Count()==0);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}

	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, KReal2_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==2);
		TEST(foundIds[0]==KReal2);
		TEST(foundIds[1]==KReal3);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, 7.7, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==0);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}

	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, KString1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==1);
		TEST(foundIds[0]==KString1);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	_LIT(KFoo, "foo");
	TRAP(leaveReason, r = repository->FindEqL(0, KUnprotectedSettingsMask, KFoo, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNotFound);
		TEST(foundIds.Count()==0);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}

	TRAP(leaveReason, r = repository->FindNeqL(0, KUnprotectedSettingsMask, KInt1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==7);
		TEST2(foundIds.Find(KInt1), KErrNotFound);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();


	TRAP(leaveReason, r = repository->FindNeqL(25, 25, KInt1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();


	TRAP(leaveReason, r = repository->FindNeqL(0, KUnprotectedSettingsMask, KReal1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==7);
		TEST2(foundIds.Find(KReal1), KErrNotFound);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	TRAP(leaveReason, r = repository->FindNeqL(25, 25, KReal1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();


	TRAP(leaveReason, r = repository->FindNeqL(0, KUnprotectedSettingsMask, KString1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==7);
		TEST2(foundIds.Find(KString1), KErrNotFound);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();


	TRAP(leaveReason, r = repository->FindNeqL(25, 25, KString1_InitialValue, foundIds));
	if(OomTesting && (r==KErrNoMemory || leaveReason==KErrNoMemory))
		{
		CleanupStack::PopAndDestroy(); //foundIds
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	CleanupStack::PopAndDestroy(); //foundIds

	TUint32 errorId;
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);
	CleanupStack::Pop();				// CleanupRollbackTransaction

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0081
@SYMTestCaseDesc Tests reach through Find operations during transactions which create and delete settings.
@SYMTestPriority High
@SYMTestActions  Start various transactions with creates and deletes of settings and attempt Find operations on those settings.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void FindWithCreateDeleteL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0081 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	RArray<TUint32> foundIds;

	PlatSec platsec;

	// Test that transaction operations that change settings work with find

	// Do a create in a transaction and check find operation

	// Begin transaction for create operation

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Creates new setting
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	// Try to find KIntValue - should be there now
	r = repository->FindEqL(0, 0, KIntValue, foundIds);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==1);
		TEST(foundIds[0]==KNewInt);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	// Check num of settings - should now be KNumSettings+1
	r = repository->FindL(0, 0, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings+1);
	foundIds.Reset();

	// Commit transaction - this makes KNewInt persistent
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Check that transaction operation succeeded

	// Try to find KIntValue - should be there now
	r = repository->FindEqL(0, 0, KIntValue, foundIds);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNone);
		TEST(foundIds.Count()==1);
		TEST(foundIds[0]==KNewInt);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	// Check num of settings - should still be KNumSettings+1
	r = repository->FindL(0, 0, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings+1);
	foundIds.Reset();

	// Do a delete in a transaction and check find operation

	// Begin transaction operation to delete KNewInt
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Delete KNewInt
	r = repository->Delete( KNewInt);
	TEST2(r, KErrNone);

	// Try to find KIntValue, should not be there
	r = repository->FindEqL(0, 0, KIntValue, foundIds);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNotFound);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	// Check num of settings - should be KNumSettings
	r = repository->FindL(0, 0, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);
	foundIds.Reset();

	// Commit transaction - make removal of KNewInt persistent
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Check that transaction operation succeeded

	// Try to find KIntValue - should be gone now
	r = repository->FindEqL(0, 0, KIntValue, foundIds);
	if(!platsec.ConfigSetting(PlatSec::EPlatSecEnforcement))
		{
		TEST2(r, KErrNotFound);
		}
	else
		{
		TEST2(r, KErrPermissionDenied);
		}
	foundIds.Reset();

	// Check num of settings - should be back to KNumSettings
	r = repository->FindL(0, 0, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);
	foundIds.Reset();

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0082
@SYMTestCaseDesc Tests reach through Get operations during transactions.
@SYMTestPriority High
@SYMTestActions  Start a transaction and attempt Get operations.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void Get()
{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0082 "));
	TInt r;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	// Test we can do some Get's in a transaction

	r = repository->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	TInt y;
	r = repository->Get(KInt1, y);
	TEST2(r, KErrNone);
	TEST(y==KInt1_InitialValue);

	TReal real1;
	r = repository->Get(KReal1, real1);
	TEST2(r, KErrNone);
	TEST(real1==KReal1_InitialValue);

	TBuf<20> str;
	r = repository->Get(KNonExistent, str);
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

	TUint32 errorId;
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0083
@SYMTestCaseDesc Tests reach through Get operations during transactions which create and delete settings.
@SYMTestPriority High
@SYMTestActions  Start various transactions with creates and deletes of settings and attempt Get operations on those settings.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void GetWithCreateDeleteL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0083 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	// Test that transaction operations that change settings work with new get
	// which works independently of a transaction

	// Do a create in a transaction and check find operation

	// Begin transaction for create operation

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Create new setting
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	// Try to get KIntValue - should appear to be there
	TInt i;
	r = repository->Get(KNewInt, i);
	TEST2(r, KErrNone);
	TEST(i==KIntValue);

	// Commit transaction - KNewInt will be made persistent
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Check that transaction operation succeeded

	// Try to get KIntValue - should be there now
	r = repository->Get(KNewInt, i);
	TEST2(r, KErrNone);
	TEST(i==KIntValue);

	// Do a delete in a transaction and check find operation

	// Begin transaction operation to delete KNewInt
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Delete KNewInt
	r = repository->Delete( KNewInt);
	TEST2(r, KErrNone);

	// Try to get KIntValue - should appear to be gone
	r = repository->Get(KNewInt, i);
	TEST2(r, KErrNotFound);

	// Commit transaction - so removal of KIntValue is persistent
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Check that transaction operation succeeded

	// Try to get KIntValue - should be gone now
	r = repository->Get(KNewInt, i);
	TEST2(r, KErrNotFound);

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0084
@SYMTestCaseDesc Tests transaction operations.
@SYMTestPriority High
@SYMTestActions  Test empty transaction, failed transactions and rollback of transaction.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void GeneralTransactionL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0084 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	// Do an empty transaction

	// Begin transaction

	r = repository->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Commit transaction
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Try 2 successive calls to begin a transaction
	r = repository->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Try to create new setting,
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	// Rollback unrequired transaction
	repository->RollbackTransaction();

	// Check that new setting was not created
	TInt getValue;
	r = repository->Get(KNewInt, getValue);
	TEST2(r, KErrNotFound);


	// Check that correct Id is returned for failed transaction

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// do several creates with an illegal one in the middle
	TInt KIntStartValue = 100;
	const TUint32 KNewIntBase = 1000;
	TInt i = 0;
	const TInt imax = 0x0ff;
	const TInt imiddle = 0x080;
	for(i=0;i<imiddle;i++)
		{
		r = repository->Create(KNewIntBase+i, KIntStartValue+i);
		TEST2(r, KErrNone);
		}

	// Illegal create of existing setting - should fail transaction
	r = repository->Create(KReal1, KReal1_InitialValue);
	TEST2(r, KErrAlreadyExists);

	for(i=imiddle;i<imax;i++)
		{
		r = repository->Create(KNewIntBase+i, KIntStartValue+i);
		// after transaction has failed, KErrAbort should be returned by all methods
    	TEST2(r, KErrAbort);
		}

	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrAlreadyExists);
	TEST2(errorId, KReal1);

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0086
@SYMTestCaseDesc Tests move operations.
@SYMTestPriority High
@SYMTestActions  Move keys or groups of keys within keyspace. Ensure security policies are correctly enforced and that errors are correctly reported.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void MoveL()
// Initial test of move stuff JMG 08/11/2005
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0086 "));
	TInt r;
	TUint32 errId ;
	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	//
	// 1. Simple Move Operation
	//

	// Confirm that source keys exist
	RArray<TUint32> foundIds;
	r = repository->FindL(KMoveSource, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==5);
	foundIds.Reset();

	// Confirm that target keys don't exist!
	r = repository->FindL(KMoveTarget, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();

	// Move a set of keys
	r = repository->Move(KMoveSource, KMoveTarget, KMoveMask, errId) ;
	TEST2(r, KErrNone);

	// Confirm that target keys now exist
	r = repository->FindL(KMoveTarget, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==5);
	foundIds.Reset();

	// Confirm that source keys no longer exist!
	r = repository->FindL(KMoveSource, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();

	// Move keys back again [For our later convenience :-)]
	r = repository->Move(KMoveTarget, KMoveSource, KMoveMask, errId) ;
	TEST2(r, KErrNone);
	r = repository->FindL(KMoveSource, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==5);
	foundIds.Reset();


	//
	// 2. Move Operation where target already exists
	//
	r = repository->Move(KMoveSource, KMoveTargetExists, KMoveMask, errId) ;
	TEST2(r, KErrAlreadyExists);
	errId &= KMoveMask ;
	TEST(errId == KMoveTargetExists) ;


	//
	// 3. Move Operation where source doesn't exist
	//

	// Confirm that source keys don't exist.
	r = repository->FindL(KMoveSourceDoesntExist, KMoveMask, foundIds);
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();

	// Attempt move and check result
	r = repository->Move(KMoveSourceDoesntExist, KMoveTarget, KMoveMask, errId) ;
	TEST2(r, KErrNotFound);
	errId &= KMoveMask ;
	TEST2(errId, KMoveSourceDoesntExist) ;


	//
	// 4. Move Operation where client doesn't have capabilities to write
	//    to target range.
	//

	// Attempt move and check result
	r = repository->Move(KMoveSource, KMoveForbidden, KMoveMask, errId) ;
	TEST2(r, KErrPermissionDenied);
	errId &= KMoveMask ;
	TEST2(errId, KMoveForbidden) ;

	//
	// 5. Move Operation where client doesn't have capabilities to read
	//    from source range.
	//

	// Attempt move and check result
	r = repository->Move(KMoveForbidden, KMoveTarget, KMoveMask, errId) ;
	TEST2(r, KErrPermissionDenied);
	errId &= KMoveMask ;
	TEST2(errId, KMoveForbidden) ;

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	// Close the repository
	CleanupStack::PopAndDestroy(repository);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0087
@SYMTestCaseDesc Tests handling of long (>256 character) strings.
@SYMTestPriority High
@SYMTestActions  Attempt to write and then read back a long string
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void LongStringL()
{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0087 "));
	TInt r;
	TUint16 eric ;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidPlatsecTestRepository));

	HBufC* longSetString = HBufC::NewLC(KMaxUnicodeStringLength);
	TPtr setStr(longSetString->Des()) ;
	HBufC* longGetString = HBufC::NewLC(KMaxUnicodeStringLength);
	TPtr getStr(longGetString->Des()) ;

	for (r = 0; r < KMaxUnicodeStringLength; r++)
		{
		eric = (TInt16)Math::Random();
		setStr.Append(eric) ;
		}

	r = repository->Set(KLongString, setStr);
	TEST2(r, KErrNone);

	r = repository->Get(KLongString, getStr);
	TEST2(r, KErrNone);
	TEST(getStr==setStr);

	// Restore repository
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(longGetString) ;
	CleanupStack::PopAndDestroy(longSetString) ;
	CleanupStack::PopAndDestroy(repository);
}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0098
@SYMTestCaseDesc Tests robustness.
@SYMTestPriority High
@SYMTestActions  Handle corrupt files and settings correctly
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ4093
*/
LOCAL_C void REQ4093L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0098 "));
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	CRepository* rep;

	// Open corrupt header ROM file
	TheTest.Printf(_L("\nOpen corrupt header ROM file\n"));
	TRAPD(errH, rep=CRepository::NewLC(KUidCorruptHeaderRepository));
	TEST2(errH, KErrCorrupt);
	// Open corrupt setting ROM file
	TheTest.Printf(_L("\nOpen corrupt setting ROM file\n"));
	TRAPD(errS,rep=CRepository::NewLC(KUidCorruptSettingRepository));
	TEST2(errS, KErrCorrupt);

	// Edit repos with new setting
	TheTest.Printf(_L("\nEdit repos with new setting\n"));
	User::LeaveIfNull(rep = CRepository::NewLC(KUidPlatsecTestRepository));
	TInt r = rep->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);
	TInt newInt;
	r=rep->Get(KNewInt, newInt);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(rep);

	// Existance of caching functionality invalidates some tests and
	// makes them fail, so kill the server to force clearing the cache
	KillCentRepServerL();

	// Copy corrupt header file in rep dir as C: file
	// Corrupt file has KNewInt and corrupt header
	TheTest.Printf(_L("\nCopy corrupt header file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KZCorruptHeaderFile, KCreFile));
	
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCreFile,0,KEntryAttReadOnly,TTime(0)));
	//need to delete .cre file to preserve test's old behaviour
	User::LeaveIfError(fm->Delete(KCreFile));
	User::After(KGeneralDelay);
	
	// Open repos, should find corrupt header and use ROM file
	User::LeaveIfNull(rep = CRepository::NewLC(KUidPlatsecTestRepository));
	// If we can't find KNewInt, this proves that we are using ROM file
	r=rep->Get(KNewInt, newInt);
	TEST2(r, KErrNotFound);
	// Test that file is deleted
	TheTest.Printf(_L("\nCheck corrupt header file deleted from persists dir\n"));
	TEST2 (BaflUtils::FileExists (fs, KCreFile), EFalse); 
	// Close repository
	CleanupStack::PopAndDestroy(rep);

	// Existance of caching functionality invalidates some tests and
	// makes them fail, so kill the server to force clearing the cache
	KillCentRepServerL();

	// Copy file with corrupt setting into C:
	TheTest.Printf(_L("\nCopy corrupt setting file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KZCorruptSettingFile, KCreFile));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCreFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	// Open repos, should find corrupt setting and use ROM file
	User::LeaveIfNull(rep = CRepository::NewLC(KUidPlatsecTestRepository));
	// Test that file is deleted
	TheTest.Printf(_L("\nCheck corrupt setting file deleted from persists dir\n"));
	TEST2 (BaflUtils::FileExists (fs, KCreFile), EFalse);

	// Restore repository
	r = rep->Reset();
	TEST2(r, KErrNone);

	//close repository
	CleanupStack::PopAndDestroy(3);	//rep, fs,fm
	}

LOCAL_C void FuncTestsL()
	{
	TheTest.Start(_L("Transaction commands"));
	GeneralTransactionL();

	TheTest.Next(_L("Find during transaction"));
	FindL();

	TheTest.Next(_L("Find during transaction with create/delete"));
	FindWithCreateDeleteL();

	TheTest.Next(_L("Get during transaction"));
	Get();

	TheTest.Next(_L("Get during transaction with create/delete"));
	GetWithCreateDeleteL();

	TheTest.Next(_L("Move")) ;
	MoveL() ;

	TheTest.Next(_L("Long String")) ;
	LongStringL() ;
	
	TheTest.Next(_L("REQ4093 tests"));
	REQ4093L() ;
	 
	
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

LOCAL_C void OomTestsL()
	{
	TheTest.Start(_L("Find"));

	OomTest(FindL);

	TheTest.End();
	}


LOCAL_C void MainL()
	{
	TheTest.Start(_L("Functional tests"));
	OomTesting = EFalse;
	FuncTestsL();

	TheTest.Next(_L("Out-of-memory tests"));
	OomTesting = ETrue;
	OomTestsL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();

	TheTest.End();
	TheTest.Close();
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
