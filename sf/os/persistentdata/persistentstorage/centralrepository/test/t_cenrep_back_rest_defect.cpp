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
// t_cenrep__back_rest_defect.cpp
// 
//

#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <connect/sbdefs.h>
#include <connect/sbeclient.h>

#include "e32math.h"
#include "../cenrepsrv/srvparams.h"
#include "../cenrepsrv/transstate.h"
#include "t_cenrep_helper.h"

using namespace NCentralRepositoryConstants;
using namespace conn;

RTest TheTest(_L("Central Repository Defect Tests"));

static CSBEClient* backupClient;
static TDriveList driveList;

const TUid KUidBackupTestRepository 	= { 0xbabababa };


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

_LIT(KPersistsFile,                          "c:\\private\\10202BE9\\persists\\babababa.cre");

LOCAL_C void DeletePersistsFileL()
	{

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	// Delete txt file from persists dir
	TInt r = fm->Delete(KPersistsFile);

	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}

	CleanupStack::PopAndDestroy(2);		// fs, fm
}

LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeletePersistsFileL();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		DeletePersistsFileL();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

LOCAL_C void EnableBRFlagL(conn::TBURPartType aBURPartType)
    {
    TheTest.Printf(_L("Start bakup/restore simulation.\n"));
    backupClient->SetBURModeL(driveList, aBURPartType, EBackupBase);
    // Wait for centrep server to detect B&R P&S flag change
    User::After(1000000);
    }

LOCAL_C void DisableBRFlagL()
    {
    backupClient->SetBURModeL(driveList, conn::EBURNormal, conn::ENoBackup);
    TheTest.Printf(_L("Finish bakup/restore simulation.\n"));
     // Wait for centrep server to detect B&R P&S flag change
    User::After(1000000);
	}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
DEF058823: Central Repository client requests are unsafely blocked during Backup & Restore

Method simulates a process of backup/restore by changing a BUR flag.
Additional test for this defect can be found in t_cenrep_back_rest.cpp

*/
/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-1855
@SYMTestCaseDesc Test that central repository client requests respond approriately when B&R is in progress.
@SYMTestPriority High
@SYMTestActions  Attempt various read and write actions during backup and restore.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF058823
*/
LOCAL_C void DEF058823L()
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-1855 "));
    RFs FsSession;

    const TUint32 KUnprotectedSettingsMask = 0xFF000000;

    TInt err;
    TInt getValue;
    TUint32 KInt = 0x1;
    TUint32 errorId;
    const TInt KIntValue = 100;
    const TInt KIntValue101 = 101;
    const TInt KIntOriginalValue = 1;
    TRequestStatus intStatus;

    RArray<TUint32> foundIds;

    User::LeaveIfError(FsSession.Connect());

 	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

    // Initialise the drive list to empty and then get drive list data from
    // File Server.
    driveList.FillZ();
    User::LeaveIfError(FsSession.DriveList(driveList));

    TRAPD(error, backupClient = CSBEClient::NewL());
    if (error != KErrNone)
        {
        _LIT(KBackupClientNotAvailable,"\n***Can not create Backup&Restore Client. Doesn't work since build 03649\n***This test should be run in Techview mode.\n***Test will be skipped\n");
         TheTest.Printf(KBackupClientNotAvailable);
        FsSession.Close();
        return;
        }
    CleanupStack::PushL(backupClient);

    //=========================tests during backup process============================

    /**

    Start backup afer transaction started

    */

    CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBackupTestRepository));

	// Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	err = repository->Set(KInt,KIntValue);
	TEST2(err, KErrNone);

	//changing publish&subscribe flag which forces cenrep
	//to change its B&R status to BackupInProgress;
	//this will simulate backup process
	EnableBRFlagL(conn::EBURBackupFull);

    //try to perform a write operation during backup - should be aborted
    //because backup process failed all started transactions
    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrAbort);

	//try to perform a read operation during backup - should be aborted
    //because backup process failed all started transactions
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

    //try to perform a find operation during backup - should be aborted
	err = repository->FindL(0, KUnprotectedSettingsMask, foundIds);
	TEST2(err, KErrAbort);

    //if defect fixed this should be possible even if B&R is in progress
    repository->RollbackTransaction();

   	//Transaction state should be ENoTransaction due to call to RollbackTransaction()
	err = TransactionState(repository);
	TEST2(err, ENoTransaction);

    /**

    start transaction with just read operations during backup and commit it
    while the backup is in progress

    */

    //try to start a new transaction during backup process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);
	//Transaction start should be allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrNone);

   /**

    start transaction with mixed operations during backup and try to commit
    it while backup is in progress

    */

    //try to start a new transaction during backup process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

    //Transaction start should be allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrAbort);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

    //perform a check to make sure that value was not altered
    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

    /**

    start transaction with just read operations during backup
    but commit after backup is finished

    */

    //try to start a new transaction during backup process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	//Transaction start should always be allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

	//this should be allowed even if backup is in progress
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

	//disable simulation
   	DisableBRFlagL();

   	//if there were just read operations during backup we should be able to commit
	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrNone);


    /**

    start transaction with mixed operations during backup
    but commit after backup is finished

    */

	EnableBRFlagL(conn::EBURBackupFull);

    //try to start a new transaction during backup process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

    //Transaction start should be allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

	//disable simulation
   	DisableBRFlagL();

    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrNone);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrNone);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntValue101);

   	err = repository->Set(KInt,KIntOriginalValue);
	TEST2(err, KErrNone);

    /**

    test standlone operations during backup process

    */
	EnableBRFlagL(conn::EBURBackupFull);

	//try to perform Reset - shouldnt be allowed
	err = repository->Reset();
	TEST2(err, KErrServerBusy);

	//try to perform Get - read operations should be allowed
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

	//try to perform Set - write operations shouldnt be allowed
	err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrServerBusy);

	DisableBRFlagL();

	//check if original value is preserved as it should not be changed during backup
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

   	CleanupStack::PopAndDestroy(repository);

   	//
   	//==============tests during restore process====================================
	//

	//reopen repository
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBackupTestRepository));

    /**

    Start restore afer transaction has started and rollback it back during restore
    process

    */

	// Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	err = repository->Set(KInt,KIntValue);
	TEST2(err, KErrNone);

	//changing publish&subscribe flag which forces cenrep
	//to change its B&R status to RestoreInProgress;
	//this will simulate restore process
	EnableBRFlagL(conn::EBURRestoreFull);

    //try to perform a write operation during restore - should be aborted
    //because restore process failed all started transactions
    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrAbort);

	//try to perform a read operation during restore - should be aborted
    //because restore process failed all started transactions
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

    //try to perform a find operation during restore - should be aborted
	err = repository->FindL(0, KUnprotectedSettingsMask, foundIds);
	TEST2(err, KErrAbort);

    //if defect fixed this should be possible even if B&R is in progress
    repository->RollbackTransaction();

   	//Transaction state should be ENoTransaction due to call to RollbackTransaction()
	err = TransactionState(repository);
	TEST2(err, ENoTransaction);

    /**
        start transaction just with read operations during restore process
        and try to commit it while the process is in progress
    */

    //try to start a new transaction during restore process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);
	//Transaction start should be allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

	err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

    /**

    start transaction with mixed opearations during restore process and try to commit
    it while the process is in progress

    */

    //try to start a new transaction during restore process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

    //Transaction start should be always allowed
	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrAbort);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

    DisableBRFlagL();
    //perform a check to make sure that value was not altered
    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

	/**

    start transaction just with read operations during restore process
    but commit it after process is finished.

    */
    EnableBRFlagL(conn::EBURRestoreFull);
    //try to start a new transaction during restore process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

	err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

	//disable simulation
   	DisableBRFlagL();

   	//even if there were just read oparation this transaction should be failed
	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);


    /**

    start transaction with mixed operations during restore process
    but commit it after process is finished.

    */
	EnableBRFlagL(conn::EBURRestoreFull);

    //try to start a new transaction during restore process - should be allowed
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	err = TransactionState(repository);
	TEST2(err, EConcurrentReadWriteTransaction);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrAbort);

	//disable simulation
   	DisableBRFlagL();

    //we failed one operation in this transacion already -
    //thus should fail transaction as well to be consistent
    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrAbort);

	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

    err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

    /**

    test standlone operations during restore process

    */

	EnableBRFlagL(conn::EBURRestoreFull);

	//try to perform Reset - shouldnt be allowed
	err = repository->Reset();
	TEST2(err, KErrServerBusy);

	//try to perform Get - read operations should be allowed
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrServerBusy);

	//try to perform Set - write operations shouldnt be allowed
	err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrServerBusy);

	DisableBRFlagL();

	//check if original value is preserved as it should not be changed during restore
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);


	//=======tests for Notify Requests during backup===================

	//testing NotifyCancel
	TInt r = repository->NotifyRequest(KInt, intStatus);
	TEST2(r, KErrNone);
	EnableBRFlagL(conn::EBURBackupFull);
	r = repository->NotifyCancel(KInt);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
    DisableBRFlagL();

    //check if Notify doesn't occur (shouldnt since it was canceled)
    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	err = repository->Set(KInt,KIntOriginalValue);
	TEST(intStatus==KUnspecifiedKey);

	//testing NotifyCancelAll
	r = repository->NotifyRequest(KInt, intStatus);
	TEST2(r, KErrNone);
	EnableBRFlagL(conn::EBURBackupFull);
	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
    DisableBRFlagL();

    //check if Notify doesn't occur (shouldnt since it was canceled)
	err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	err = repository->Set(KInt,KIntOriginalValue);
	TEST(intStatus==KUnspecifiedKey);

	//=======tests for Notify Requests during restore===================

	//testing NotifyCancel
	r = repository->NotifyRequest(KInt, intStatus);
	TEST2(r, KErrNone);
	EnableBRFlagL(conn::EBURRestoreFull);
	r = repository->NotifyCancel(KInt);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
    DisableBRFlagL();

    //check if Notify doesn't occur (shouldnt since it was canceled)
    err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	err = repository->Set(KInt,KIntOriginalValue);
	TEST(intStatus==KUnspecifiedKey);

	//testing NotifyCancelAll
	r = repository->NotifyRequest(KInt, intStatus);
	TEST2(r, KErrNone);
	EnableBRFlagL(conn::EBURRestoreFull);
	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
    DisableBRFlagL();

    //check if Notify doesn't occur (shouldnt since it was canceled)
	err = repository->Set(KInt,KIntValue101);
	TEST2(err, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	err = repository->Set(KInt,KIntOriginalValue);
	TEST(intStatus==KUnspecifiedKey);

	//==finish===================================
   	CleanupStack::PopAndDestroy(repository);
    foundIds.Close();

	CleanupStack::PopAndDestroy(backupClient);
 	CleanupStack::PopAndDestroy(scheduler);
    FsSession.Close();
    }

LOCAL_C void FuncTestsL()
	{
	TheTest.Start(_L("DEF058823 - Central Repository client requests are unsafely blocked during Backup & Restore"));
	DEF058823L();

	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("Defect tests"));
	DeletePersistsFileL();

	FuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
	DeletePersistsFileL();

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
