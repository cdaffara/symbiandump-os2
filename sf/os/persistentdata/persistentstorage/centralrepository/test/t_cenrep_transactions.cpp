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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include "e32math.h"
#include "srvparams.h"
#include "transstate.h"
#include "clientrequest.h"
#include "panic.h"

//#define Additional_Test

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Transaction Tests"));

const TUid KUidTestRepository 	= { 0x0000101 };

//
// Repository A
//

const TUint32 KUnprotectedSettingsMask = 0xFF000000 ;

const TInt KThreadStackSize	=0x2000; // 8k
const TInt KThreadMinHeapSize	=0x4000; // 16k
const TInt KThreadMaxHeapSize	=0xa000; // 60k

const TUint32 KInt1 = 1;
const TInt KInt1_InitialValue = 1;

const TUint32 KReal1 = 2;
const TReal KReal1_InitialValue = 2.732;
const TUint32 KString8 = 5;
const TUint32 KString16 = 6;
_LIT(KString16_InitialValue, "test\\\"string\"");
_LIT8(KString8_InitialValue, "test\\\"string\"");

const TInt KInt2 = 3;
const TInt KInt2_InitialValue = 20;
const TUint32 KReal2 = 28;
const TReal KReal2_InitialValue = 2.5;
const TReal KReal2_SecondValue = 3.5;
const TUint32 KNewString8 = 21;
const TUint32 KNewString16 = 22;
_LIT(KString16_InitialValue2, "another\\\"string\"");
_LIT8(KString8_InitialValue2, "another\\\"string\"");

_LIT(KString16_Small_Value, "te");
_LIT8(KString8_Small_Value, "te");

const TUint32 KMoveTarget            = 0x02000001  ;
const TUint32 KMoveSourceDoesntExist = 0x01000000 ;
const TUint32 KMoveMask              = 0xFF0000FF ;

const TUint32 KInt3 = 10; //Ranged Policy Key
const TInt KInt3_InitialValue = 34;
const TUint32 KInt4 = 0x201; //Default Policy Key
const TInt KInt4_InitialValue = 10;
const TUint32 KInt5 = 0x1000001; //Single Policy Key
const TInt KInt5_SecondValue = 123;
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

typedef void (*TRepositoryFunc)(CRepository&);

void ChangeByCreate(CRepository& aRep)
	{
	TInt r;
	// Create a setting outside of a transaction
	r = aRep.Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);
	// confirm setting created
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);
	}

void ChangeBySet(CRepository& aRep)
	{
	TInt r;
	// Set a value outside of a transaction
	r = aRep.Set(KReal2, KReal2_SecondValue);
	TEST2(r, KErrNone);
	// confirm value changed
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_SecondValue);
	}

void ChangeByDelete(CRepository& aRep)
	{
	TInt r;
	// Delete a setting outside of a transaction
	r = aRep.Delete(KReal2);
	TEST2(r, KErrNone);
	// confirm it's deleted
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNotFound);
	}

void ChangeByTransaction(CRepository& aRep)
	{
	TInt r;
	TUint32 keyInfo;

	// Lock should prevent ERead/WriteTransactions from being able to start from
	// within function CommittingChangesFailsOtherTransactions.
	r = aRep.StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrLocked);
	r = aRep.StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrLocked);

	// Concurrent Read/Write transactions can be opened at any time
	r = aRep.StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EConcurrentReadWriteTransaction);

	// Create a setting within transaction
	r = aRep.Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);
	// confirm setting created
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);

	r = aRep.CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	TEST2(keyInfo, 1); // 1 successful change

	// re-confirm setting is still there
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);
	}

// checking that async start and commit, with WaitForRequest do same as ChangeByTransaction
void ChangeByAsyncTransaction(CRepository& aRep)
	{
	TInt r;
	CRepository::TTransactionKeyInfoBuf keyInfoBuf;
	TRequestStatus status;

	// Lock should prevent ERead/WriteTransactions from being able to start from
	// within function CommittingChangesFailsOtherTransactions.
	aRep.StartTransaction(CRepository::EReadWriteTransaction, status);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrLocked);
	aRep.StartTransaction(CRepository::EReadTransaction, status);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrLocked);

	// Concurrent Read/Write transactions can be opened at any time
	aRep.StartTransaction(CRepository::EConcurrentReadWriteTransaction, status);
	// can't predict whether status will be KRequestPending at this time, so don't check
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EConcurrentReadWriteTransaction);

	// Create a setting within transaction
	r = aRep.Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);
	// confirm setting created
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);

	aRep.CommitTransaction(keyInfoBuf, status);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	TEST2(keyInfoBuf(), 1); // 1 successful change

	// re-confirm setting is still there
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);
	}

void ChangeByReset(CRepository& aRep)
	{
	TInt r;
	// Delete a setting outside of a transaction
	r = aRep.Reset(KReal2);
	TEST2(r, KErrNone);
	// confirm it's reset (should be deleted)
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNotFound);
	}

void ChangeByResetAll(CRepository& aRep)
	{
	TInt r;
	// Delete a setting outside of a transaction
	r = aRep.Reset();
	TEST2(r, KErrNone);
	// confirm it's reset (should be deleted)
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNotFound);
	}

// tests that changes made by calling ChangeFunc with aCommittingRepository fail active
// transactions with reason KErrLocked, and correctly discard changes
LOCAL_C void CallFuncCheckOtherTransactionsFail(TRepositoryFunc ChangeFunc, CRepository& aCommittingRepository)
	{
	TInt r;
	TUint32 keyInfo;
	CRepository* repository1;
	CRepository* repository2;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));

	// Start two types of transaction
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	r = repository2->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	// check transactions haven't failed yet.
	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);
	r = TransactionState(repository2);
	TEST2(r, EConcurrentReadWriteTransaction);

	// create a value in repository2's transaction and check it is there
	r = repository2->Create(KInt1, KInt1_InitialValue);
	TEST2(r, KErrNone);
	TInt value;
	r = repository2->Get(KInt1, value);
	TEST2(r, KErrNone);
	TEST(value == KInt1_InitialValue);

	(*ChangeFunc)(aCommittingRepository);

	// check transactions have now failed
	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);
	r = TransactionState(repository2);
	TEST2(r, EConcurrentReadWriteTransaction | EFailedBit);

	// operations should abort after failing
	r = repository2->Get(KInt1, value);
	TEST2(r, KErrAbort);
	r = repository1->Set(KInt1, value);
	TEST2(r, KErrAbort);

	// commits should fail with KErrLocked
	r = repository2->CommitTransaction(keyInfo);
	TEST2(r, KErrLocked);
	TEST2(keyInfo, KUnspecifiedKey);
	r = repository1->CommitTransaction(keyInfo);
	TEST2(r, KErrLocked);
	TEST2(keyInfo, KUnspecifiedKey);

	// integer should not be persistent
	r = repository2->Get(KInt1, value);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0150
@SYMTestCaseDesc Check committing changes causes other transactions to fail.
@SYMTestPriority High
@SYMTestActions Run a series of commands that modify repository, check they fail active transactions.
 Start concurrent read write transaction and try to create and get values using other repository.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void CommittingChangesFailsOtherTransactionsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0150 "));
	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	repository->Reset();

	// non-transaction create should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByCreate, *repository);

	// non-transaction set should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeBySet, *repository);

	// non-transaction delete should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByDelete, *repository);

	// transaction commit with changes should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByTransaction, *repository);

	ChangeByDelete(*repository); // just to ready for next test

	// async started and committed transaction with changes should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByAsyncTransaction, *repository);

	// individual reset that has an effect should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByReset, *repository);

	ChangeByCreate(*repository); // just to ready for next test

	// reset all should fail active transactions
	CallFuncCheckOtherTransactionsFail(ChangeByResetAll, *repository);

	CleanupStack::PopAndDestroy(repository);
	}

void NoChangeByTransactionCreateDelete(CRepository& aRep)
	{
	TInt r;
	TUint32 keyInfo;
	// Concurrent Read/Write transactions can be opened at any time
	r = aRep.StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EConcurrentReadWriteTransaction);
	// deleting the setting that was just added should result in no change when committing:
	ChangeByCreate(aRep);
	ChangeByDelete(aRep);
	r = aRep.CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	TEST2(keyInfo, 0); // no changes
	}

void NoChangeByTransactionSetSameValue(CRepository& aRep)
	{
	TInt r;
	TUint32 keyInfo;
	// Concurrent Read/Write transactions can be opened at any time
	r = aRep.StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EConcurrentReadWriteTransaction);
	// changing setting to the value it already has should result in no change when committing:
	ChangeBySet(aRep);
	r = aRep.CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	TEST2(keyInfo, 0); // no changes
	}

void NoChangeByReadTransaction(CRepository& aRep)
	{
	TInt r;
	TUint32 keyInfo;
	// Read transactions can be opened now because there is no EReadWriteTransaction open
	r = aRep.StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EReadTransaction);
	// Getting a value should result in no change when committing:
	TReal value;
	r = aRep.Get(KReal2, value);
	TEST2(r, KErrNone);
	TEST(value == KReal2_InitialValue);
	r = aRep.CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	TEST2(keyInfo, 0); // no changes
	}

void NoChangeByEmptyWriteTransaction(CRepository& aRep)
	{
	TInt r;
	TUint32 keyInfo;
	// can't start an EReadWriteTransaction because a ReadTransaction is active
	r = aRep.StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrLocked);
	// Concurrent Read/Write transactions can be opened at any time
	r = aRep.StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(&aRep);
	TEST2(r, EConcurrentReadWriteTransaction);
	// make no changes before committing
	r = aRep.CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	TEST2(keyInfo, 0); // no changes
	}

// tests that changes made by calling ChangeFunc with aCommittingRepository
// do not fail active transactions. Must Reset before calling this.
LOCAL_C void CallFuncCheckOtherTransactionsDoNotFail(TRepositoryFunc ChangeFunc, CRepository& aCommittingRepository)
	{
	TInt r;
	CRepository::TTransactionKeyInfoBuf keyInfoBuf;
	TUint32 keyInfo;
	TRequestStatus status;
	CRepository* repository1;
	CRepository* repository2;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));

	// Start two types of transaction and wait for completion
	r = repository1->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);
	repository2->StartTransaction(CRepository::EConcurrentReadWriteTransaction, status);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);

	// create values in repository2's transaction cache
	r = repository2->Create(KNewString8, KString8_InitialValue);
	TEST2(r, KErrNone);
	r = repository2->Create(KNewString16, KString16_InitialValue);
	TEST2(r, KErrNone);

	// check transactions not failed
	r = TransactionState(repository1);
	TEST2(r, EReadTransaction);
	r = TransactionState(repository2);
	TEST2(r, EConcurrentReadWriteTransaction);

	(*ChangeFunc)(aCommittingRepository);

	// create setting in repository 1's cache
	r = repository1->Create(KInt2, KInt2_InitialValue);
	TEST2(r, KErrNone);

	// check transactions not failed & Read upgraded to ReadWrite
	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);
	r = TransactionState(repository2);
	TEST2(r, EConcurrentReadWriteTransaction);

	// commit repository2 asynchronously
	repository2->CommitTransaction(keyInfoBuf, status);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	TEST2(keyInfoBuf(), 2); // 2 successful changes

	// check transaction2 finished, transaction 1 failed
	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);
	r = TransactionState(repository2);
	TEST2(r, ENoTransaction);

	// operations on repository 1 should abort after failing
	TInt intValue;
	r = repository1->Get(KInt1, intValue);
	TEST2(r, KErrAbort);
	r = repository1->Set(KInt1, intValue);
	TEST2(r, KErrAbort);

	// commit of repository 1 should fail with KErrLocked
	r = repository1->CommitTransaction(keyInfo);
	TEST2(r, KErrLocked);
	TEST2(keyInfo, KUnspecifiedKey);

	// check changes by repository 2 are still present
	TBuf8<40> buf8Value;
	TBuf16<40> buf16Value;

	r = repository2->Get(KNewString8, buf8Value);
	TEST2(r, KErrNone);
	TEST(buf8Value == KString8_InitialValue);
	r = repository2->Get(KNewString16, buf16Value);
	TEST2(r, KErrNone);
	TEST(buf16Value == KString16_InitialValue);

	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0151
@SYMTestCaseDesc Check committing no changes (incl. setting same value) does not cause other transactions to fail.
@SYMTestPriority High
@SYMTestActions Run a series of commands that modify repository, check they fail active transactions.
 Start concurrent read write transaction and try to create and get values using other repository.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void CommittingNoChangesDoesNotFailOtherTransactionsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0151 "));
	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	// creating and deleting the same setting in the same transaction should have no effect
	repository->Reset();
	CallFuncCheckOtherTransactionsDoNotFail(NoChangeByTransactionCreateDelete, *repository);

	// changing a setting to the same value in a transaction should have no effect
	repository->Reset();
	ChangeByCreate(*repository);
	ChangeBySet(*repository);
	CallFuncCheckOtherTransactionsDoNotFail(NoChangeByTransactionSetSameValue, *repository);

	// committing a Read transaction should have no effect
	repository->Reset();
	ChangeByCreate(*repository);
	CallFuncCheckOtherTransactionsDoNotFail(NoChangeByReadTransaction, *repository);

	// committing an empty transaction should have no effect
	repository->Reset();
	CallFuncCheckOtherTransactionsDoNotFail(NoChangeByEmptyWriteTransaction, *repository);

	CleanupStack::PopAndDestroy(repository);

	// restart session and check values are indeed saved
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	TInt r;
	TBuf8<40> buf8Value;
	TBuf16<40> buf16Value;
	r = repository->Get(KNewString8, buf8Value);
	TEST2(r, KErrNone);
	TEST(buf8Value == KString8_InitialValue);
	r = repository->Get(KNewString16, buf16Value);
	TEST2(r, KErrNone);
	TEST(buf16Value == KString16_InitialValue);
	// final cleanup
	repository->Reset();

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0152
@SYMTestCaseDesc Testing creates in a read write transaction
@SYMTestPriority High
@SYMTestActions  Starts a read write transaction which creates values for KInt1, KReal1, KString8 and KString16
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void CreateValueUsingTransactionL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0152 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	// Begin read write transaction
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository->Create(KInt1, KInt1_InitialValue);
	TEST2(r, KErrNone);

	r = repository->Create(KReal1, KReal1_InitialValue);
	TEST2(r, KErrNone);

	r = repository->Create(KString8, KString8_InitialValue);
	TEST2(r, KErrNone);

	r = repository->Create(KString16, KString16_InitialValue);
	TEST2(r, KErrNone);

	// Commit transaction
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);
	// Test for the value of errorId for a successful read write transaction = 4 changes
	TEST2(errorId, 4);

	CleanupStack::PopAndDestroy(repository);

	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0153
@SYMTestCaseDesc Testing state and rollback in a transaction
@SYMTestPriority High
@SYMTestActions  Use both transactionstate and failtransaction while being in a transaction and not in a transaction.
	Check on the state and test to see if rollback occurs when required.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionStateAndRollBackL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0153 "));
	TInt r;
	TUint32 errorId;
	TReal getValue;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	// Try to fail transaction without being in a transaction
	repository->FailTransaction();

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	// Begin concurrent read write transaction
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction);

	r = repository->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	r = repository->Get(KReal2, getValue);
	TEST(getValue == KReal2_InitialValue);
	TEST2(r, KErrNone);

	// Fail transaction should roll back transaction i.e. Create KReal2 should not work.
	repository->FailTransaction();

	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	// Commit transaction
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrAbort);
	// Test the value of errorId for a failed read write transaction
	TEST2(errorId, KUnspecifiedKey);

	// Try to get a value which should not exist as transaction failed
	r = repository->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	repository->RollbackTransaction();

	// Try to get a value which should not exist as transaction rolled back
	r = repository->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	// Transaction state should be ENoTransaction due to call to RollbackTransaction()
	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	// Begin read transaction
	r = repository->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	r = TransactionState(repository);
	TEST2(r, EReadTransaction);

	r = repository->Get(KReal1, getValue);
	TEST2(r, KErrNone);
	TEST(getValue == KReal1_InitialValue);

	// Fail transaction
	repository->FailTransaction();

	r = TransactionState(repository);
	TEST2(r, EReadTransaction | EFailedBit);

	// Commit transaction
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrAbort);

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	// Begin another read transaction
	r = repository->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	r = repository->Get(KReal1, getValue);
	TEST2(r, KErrNone);
	TEST(getValue == KReal1_InitialValue);

	r = TransactionState(repository);
	TEST2(r, EReadTransaction);

	// Perform Create in a ReadTransaction as upgrade has occured
	r = repository->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	// The state should be updated to be EInReadWriteTransaction
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction);

	r = repository->Get(KReal2, getValue);
	TEST2(r, KErrNone);

	// Commit transaction
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Delete KReal2 to reset available test case variables
	r = repository->Delete(KReal2);
	TEST2(r, KErrNone);

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void StartTransactionPanicConditionsL()
	{
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin read write transaction
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// This should panic can't start second transaction in one session
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Commit transaction
	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository1);
	}

LOCAL_C TInt TestStartTransactionPanics(TAny* /*aData*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, StartTransactionPanicConditionsL());

	// Won't get here but add this line to get rid of ARMV5 warning
	TEST2(err, KErrNone);

	delete cleanup;
	__UHEAP_MARKEND;

	return (KErrNone);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0154
@SYMTestCaseDesc Testing valid panics while using StartTransaction
@SYMTestPriority High
@SYMTestActions  Start a separate thread and within that thread try to start transaction twice
@SYMTestExpectedResults The thread should exit with the exit type EExitPanic and exit reason EStartAlreadyInTransaction
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionPanicConditionsThread()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0154 "));
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	TRequestStatus status;

	_LIT(KName, "Transaction_Panic_Thread");

	RThread thread;
	TInt rc = thread.Create(KName,TestStartTransactionPanics,KThreadStackSize,KThreadMinHeapSize,KThreadMaxHeapSize,NULL);

	TEST2(rc,KErrNone);

	thread.Logon(status);
	thread.Resume();

	User::WaitForRequest(status);

	// Should result in a EExitPanic exit type and an EStartAlreadyInTransaction exit reason
	TEST2(thread.ExitType(), EExitPanic);
	TEST2(thread.ExitReason(), EStartAlreadyInTransaction);

	thread.Close();

	User::SetJustInTime(jitEnabled);
	}

LOCAL_C void CommitTransactionPanicConditionL()
	{
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Commit transaction when there is not transaction should cause a panic
	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository1);
	}


LOCAL_C TInt TestCommitTransactionPanics(TAny* /*aData*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err,CommitTransactionPanicConditionL());

	// Won't get here but add this line to get rid of ARMV5 warning
	TEST2(err, KErrNone);

	delete cleanup;
	__UHEAP_MARKEND;
	return (KErrNone);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0155
@SYMTestCaseDesc Testing valid panics while using CommitTransaction
@SYMTestPriority High
@SYMTestActions  Start a separate thread and within that thread try to commit a transaction without being in one
@SYMTestExpectedResults The thread should exit with the exit type EExitPanic and the exit reason ECommitNotInTransaction
@SYMPREQ PREQ752
*/
LOCAL_C void CommitTransactionPanicConditionsThread()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0155 "));
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	TRequestStatus status;

	_LIT(KName, "Commit_Transaction_Panic_Thread");

	RThread thread;
	TInt rc = thread.Create(KName,TestCommitTransactionPanics,KThreadStackSize,KThreadMinHeapSize,KThreadMaxHeapSize,NULL);

	TEST2(rc,KErrNone);

	thread.Logon(status);
	thread.Resume();

	User::WaitForRequest(status);

	// Should result in a EExitPanic exit type and an ECommitNotInTransaction exit reason
	TEST2(thread.ExitType(), EExitPanic);
	TEST2(thread.ExitReason(), ECommitNotInTransaction);

	thread.Close();

	User::SetJustInTime(jitEnabled);
	}

LOCAL_C void LeaveWithCleanupRollbackTransactionPushL(CRepository& aRepository)
	{
	TInt r = TransactionState(&aRepository);
	TEST2(r, CRepository::EReadWriteTransaction);

	aRepository.CleanupRollbackTransactionPushL();

	// Purposely leave in a transaction...
	User::LeaveNoMemory();

	// this code should never be reached...
	TEST(EFalse);
	// ...but if it did we'd have to cleanup the PushL
	CleanupStack::Pop();
	}

LOCAL_C void LeaveWithCleanupFailTransactionPushL(CRepository& aRepository)
	{
	TInt r = TransactionState(&aRepository);
	TEST2(r, CRepository::EReadWriteTransaction);

	aRepository.CleanupFailTransactionPushL();

	// Purposely leave in a transaction...
	User::LeaveNoMemory();

	// this code should never be reached...
	TEST(EFalse);
	// ...but if it did we'd have to cleanup the PushL
	CleanupStack::Pop();
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0156
@SYMTestCaseDesc Testing CleanupRollbackTransactionPushL and CleanupFailTransactionPushL
@SYMTestPriority High
@SYMTestActions Start a transaction and use CleanupRollbackTransactionPushL. Within the transaction purposely leave, as this will
	result in CleanupRollbackTransactionPushL being used. Similar operation required for CleanupFailTransactionPushL
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void CleanupRollBackAndFailTransactionL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0156 "));
	TInt r;
	TUint32 errorId;
	TReal getValue;

	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository));

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository->CleanupRollbackTransactionPushL();

	r = repository->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	// Purposely leave in a transaction...
	TRAP(r, LeaveWithCleanupRollbackTransactionPushL(*repository));
	TEST2(r, KErrNoMemory);

	// should cause RollbackTransaction, therefore transaction state should be ENoTransaction...

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	// and getting the value KReal2 should result in KErrNotFound
	r = repository->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository->CleanupFailTransactionPushL();

	r = repository->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	// Purposely leave in a transaction...
	TRAP(r, LeaveWithCleanupFailTransactionPushL(*repository));
	TEST2(r, KErrNoMemory);

	// should cause FailTransaction, therefore transaction state should be EInFailedReadWriteTransaction...
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	// only after commit will Fail Transaction call roll back...
	r = repository->CommitTransaction(errorId);
	TEST2(r, KErrAbort);

	// so getting the value KReal2 should result in KErrNotFound
	r = repository->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository);
	TEST2(r, ENoTransaction);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0157
@SYMTestCaseDesc Test Read Transaction Conditions
@SYMTestPriority High
@SYMTestActions Start read transactions and perform read operations
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void ReadTransactionConditionsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0157 "));
	TInt r;
	TUint32 errorId;
	TInt intVal;

	CRepository* repository1;
	CRepository* repository2;
	CRepository* repository3;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Should be able to start another read transaction
	r = repository2->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Perform some gets using the open transactions and repository
	r = repository1->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST2(intVal, KInt1_InitialValue);

	r = repository2->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST2(intVal, KInt1_InitialValue);

	r = repository3->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST2(intVal, KInt1_InitialValue);

	// Get the state of Transactions
	r = TransactionState(repository1);
	TEST2(r, EReadTransaction);

	r = TransactionState(repository1);
	TEST2(r, EReadTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);
	// Test for the value of errorId for a successful read transaction: no changes
	TEST2(errorId, 0);

	r = repository2->CommitTransaction(errorId);
	TEST2(r, KErrNone);
	// Test for the value of errorId for a successful read transaction: no changes
	TEST2(errorId, 0);

	CleanupStack::PopAndDestroy(repository3);
	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0158
@SYMTestCaseDesc Test upgrade read transaction with error conditions
@SYMTestPriority High
@SYMTestActions Start read transactions and try to upgrade one of the read transactions
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void UpgradeReadTransactionErrorConditionsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0158 "));
	TInt r;
	TUint32 errorId;
	TInt intVal;

	CRepository* repository1;
	CRepository* repository2;
	CRepository* repository3;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Should be able to start another read transaction
	r = repository2->StartTransaction(CRepository::EReadTransaction);
	TEST2(r, KErrNone);

	// Perform get using open transaction
	r = repository2->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST2(intVal, KInt1_InitialValue);

	// Perform create which should fail transaction with KErrLocked
	r = repository1->Create(KInt2, KInt2_InitialValue);
	TEST2(r, KErrLocked);

	// check the value is not there as far as repository 3 is concerned
	r = repository3->Get(KInt2, intVal);
	TEST2(r, KErrNotFound);

	// Get the state of Transactions
	r = TransactionState(repository1);
	TEST2(r, EReadTransaction | EFailedBit);

	r = TransactionState(repository2);
	TEST2(r, EReadTransaction);

	r = repository2->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrLocked);
	// Check the key responsible for the failed read transaction promote
	TEST2(errorId, KInt2);

	CleanupStack::PopAndDestroy(repository3);
	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0159
@SYMTestCaseDesc Test read operations with a read write transaction open
@SYMTestPriority High
@SYMTestActions Start a transaction and try to perform some read operations outside of the open transaction
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionConditionsForReadL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0159 "));
	TInt r;
	TUint32 errorId;
	RArray<TUint32> foundIds;

	CRepository* repository1;
	CRepository* repository2;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should be able to get values regardless of transaction1 being open
	TInt intVal;
	r = repository2->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST2(intVal, KInt1_InitialValue);

	TReal realVal;
	r = repository2->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal==KReal1_InitialValue);

	TBuf8<14> buf8Val;
	r = repository2->Get(KString8, buf8Val);
	TEST2(r, KErrNone);
	TEST(buf8Val==KString8_InitialValue);

	TBuf16<14> buf16Val;
	r = repository2->Get(KString16, buf16Val);
	TEST2(r, KErrNone);
	TEST(buf16Val==KString16_InitialValue);

	r = repository2->FindL(0, KUnprotectedSettingsMask, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==4);

	foundIds.Reset();

	// Find in range for values equal to KInt1_InitialValue
	r = repository2->FindEqL(0, KUnprotectedSettingsMask, KInt1_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);

	foundIds.Reset();

	// Find in range for values NOT equal to KInt1_InitialValue
	r = repository2->FindNeqL(0, KUnprotectedSettingsMask, KInt1_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==3);

	foundIds.Reset();

	// Find in range for values equal to KReal1_InitialValue
	r = repository2->FindEqL(0, KUnprotectedSettingsMask, KReal1_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(),1);

	foundIds.Reset();

	// Find in range for values NOT equal to KReal1_InitialValue
	r = repository2->FindNeqL(0, KUnprotectedSettingsMask, KReal1_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(),3);

	// Find in range for values equal to KString8_InitialValue
	r = repository2->FindEqL(0, KUnprotectedSettingsMask, KString8_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);

	foundIds.Reset();

	// Find in range for values NOT equal to KString8_InitialValue
	r = repository2->FindNeqL(0, KUnprotectedSettingsMask, KString8_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(),3);


	// Find in range for values equal to KString16_InitialValue
	r = repository2->FindEqL(0, KUnprotectedSettingsMask, KString16_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(),1);

	foundIds.Reset();

	// Find in range for values NOT equal to KString16_InitialValue
	r = repository2->FindNeqL(0, KUnprotectedSettingsMask, KString16_InitialValue, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(),3);

	foundIds.Reset();

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0160
@SYMTestCaseDesc Test Transaction error conditions with get
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic gets. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForGetL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0160 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Should NOT fail transaction
	TInt intVal;
	r = repository1->Get(KInt2, intVal);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	TReal realVal;
	r = repository1->Get(KInt1, realVal);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	r = repository1->Get(KReal2, realVal);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	r = repository1->Get(KReal1, intVal);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	TBuf8<14> buf8Val;
	r = repository1->Get(KNewString8, buf8Val);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	r = repository1->Get(KString8, intVal);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	TBuf8<2> smallBuf8Val;
	r = repository1->Get(KString8, smallBuf8Val);
	TEST2(r, KErrOverflow);
	TEST(smallBuf8Val==KString8_Small_Value);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	TBuf16<14> buf16Val;
	r = repository1->Get(KNewString16, buf16Val);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	r = repository1->Get(KString16, intVal);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Should NOT fail transaction
	TBuf16<2> smallBuf16Val;
	r = repository1->Get(KString16, smallBuf16Val);
	TEST2(r, KErrOverflow);
	TEST(smallBuf16Val==KString16_Small_Value);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0161
@SYMTestCaseDesc Test Transaction error conditions with find
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic find operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForFindL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0161 "));
	TInt r;
	TUint32 errorId;
	RArray<TUint32> foundIds;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindL(100, 110, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0162
@SYMTestCaseDesc Test Transaction error conditions with findeq
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic findeq operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForFindEqL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0162 "));
	TInt r;
	TUint32 errorId;
	RArray<TUint32> foundIds;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindEqL(0, KUnprotectedSettingsMask, KInt2_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindEqL(0, KUnprotectedSettingsMask, KReal2_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindEqL(0, KUnprotectedSettingsMask, KString8_InitialValue2, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindEqL(0, KUnprotectedSettingsMask, KString16_InitialValue2, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0163
@SYMTestCaseDesc Test Transaction error conditions with findneq
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic findneq operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForFindNeqL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0163 "));
	TInt r;
	TUint32 errorId;
	RArray<TUint32> foundIds;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindNeqL(100, 110, KInt1_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindNeqL(100, 110, KReal1_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindNeqL(100, 110, KString8_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	repository1->CleanupRollbackTransactionPushL();

	// Should NOT fail transaction
	r = repository1->FindNeqL(100, 110, KString16_InitialValue, foundIds);
	TEST2(r, KErrNotFound);

	foundIds.Reset();

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::Pop();				// CleanupRollbackTransaction

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0164
@SYMTestCaseDesc Test Transaction error conditions with create
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic create operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForCreateL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0164 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Should fail the transaction
	r = repository1->Create(KInt1, KInt1_InitialValue);
	TEST2(r, KErrAlreadyExists);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrAlreadyExists);
	TEST2(errorId, KInt1);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Should fail the transaction
	r = repository1->Create(KReal1, KReal1_InitialValue);
	TEST2(r, KErrAlreadyExists);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrAlreadyExists);
	TEST2(errorId, KReal1);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Should fail the transaction
	r = repository1->Create(KString8, KString8_InitialValue);
	TEST2(r, KErrAlreadyExists);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrAlreadyExists);
	TEST2(errorId, KString8);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Should fail the transaction
	r = repository1->Create(KString16, KString16_InitialValue);
	TEST2(r, KErrAlreadyExists);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrAlreadyExists);
	TEST2(errorId, KString16);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0165
@SYMTestCaseDesc Test Transaction error conditions with delete
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic delete operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForDeleteL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0165 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Delete KInt2 which does not exist
	r = repository1->Delete(KInt2);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0166
@SYMTestCaseDesc Test Transaction error conditions with set
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic set operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForSetL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0166 "));
	TInt r;
	TUint32 errorId;
    TInt i;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository1->Get(KInt2, i);
	TEST2(r, KErrNotFound);

	// Set KInt2 which does not exist
	r = repository1->Set(KInt2, KInt2_InitialValue);
	TEST2(r, KErrNone);

	r = repository1->Get(KInt2, i);
	TEST2(r, KErrNone);
    TEST2(i, KInt2_InitialValue);

    // Set KInt2 to a value of real type
    r = repository1->Set(KInt2, KReal2_InitialValue);
    TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrArgument);
	TEST2(errorId, KInt2);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository1->Set(KInt2, KInt2_InitialValue + 1);
	TEST2(r, KErrNone);

	// Set KInt2 to a value of string8 type
	r = repository1->Set(KInt2, KString8_InitialValue);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrArgument);
	TEST2(errorId, KInt2);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository1->Set(KInt2, KInt2_InitialValue + 1);
	TEST2(r, KErrNone);

	// Set KInt2 to a value of string16 type
	r = repository1->Set(KInt2, KString16_InitialValue);
	TEST2(r, KErrArgument);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction | EFailedBit);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrArgument);
	TEST2(errorId, KInt2);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0167
@SYMTestCaseDesc Test Transaction error conditions with move
@SYMTestPriority High
@SYMTestActions Start a transaction and perform some problematic move operations. Test to see if it fails transactions or not.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionErrorConditionsForMoveL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0167 "));
	TInt r;
	TUint32 errorId;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Move a set of keys which does not exist
	r = repository1->Move(KMoveSourceDoesntExist, KMoveTarget, KMoveMask, errorId) ;
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->Move(KInt1, KInt1, KMoveMask, errorId) ;
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0168
@SYMTestCaseDesc Test move operation with more comprehensive test cases
@SYMTestPriority High
@SYMTestActions Start a transaction and try to perform some problematic move operations. Check result of operation.
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void TransactionConditionsForMoveL()
	{
	// More comprehensive test cases for MOVE

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0168 "));
	TInt r;
	TUint32 errorId;
	TBuf8<16> buf8Val;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Move a set of keys which does not exist
	r = repository1->Move(KNewString8, KMoveTarget, KMoveMask, errorId);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	r = repository1->Create(KNewString8, KString8_InitialValue2);
	TEST2(r, KErrNone);

	//Move KNewString8 to KMoveTarget
	r = repository1->Move(KNewString8, KMoveTarget, KMoveMask, errorId);
	TEST2(r, KErrNone);

	r = repository1->Get(KNewString8, buf8Val);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, EReadWriteTransaction);

	// Move keys back again
	r = repository1->Move(KMoveTarget, KNewString8, KMoveMask, errorId);
	TEST2(r, KErrNone);

	r = repository1->Get(KNewString8, buf8Val);
	TEST2(r, KErrNone);
	TEST(buf8Val==KString8_InitialValue2);

	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Reset the test by deleting KNewString8...
	r = repository1->Delete(KNewString8);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0169
@SYMTestCaseDesc Test concurrent read and write transactions
@SYMTestPriority High
@SYMTestActions Start a concurrent read write transaction and perform some operations. Check the state of transaction
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void ConcurrentReadWriteTransactionStatesL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0169 "));
	TInt r;
	TUint32 errorId;
	TReal getValue;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Begin transaction for repository1
	r = repository1->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	r = TransactionState(repository1);
	TEST2(r, EConcurrentReadWriteTransaction);

	r = repository1->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	r = repository1->Get(KReal2, getValue);
	TEST2(r, KErrNone);

	// Fail transaction should roll back transaction i.e. Create KReal2 should not work.
	repository1->FailTransaction();

	r = TransactionState(repository1);
	TEST2(r, EConcurrentReadWriteTransaction | EFailedBit);

	// Commit transaction
	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrAbort);

	// Try to get a value which should not exist as transaction failed
	r = repository1->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	r = TransactionState(repository1);
	TEST2(r, ENoTransaction);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENREP-CT-0170
@SYMTestCaseDesc Test concurrent read and write transaction with read operations
@SYMTestPriority High
@SYMTestActions Start a concurrent read write and a read transaction and perform some read operations. Check the state of transaction
@SYMTestExpectedResults The test should not fail with any panics
@SYMPREQ PREQ752
*/
LOCAL_C void ConcurrentReadWriteTransactionWithReadOperationsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENREP-CT-0170 "));
	TInt r;
	TUint32 errorId;
	TReal getValue;

	CRepository* repository1;
	CRepository* repository2;
	CRepository* repository3;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository));

	// Begin concurrent read write transaction for repository1
	r = repository1->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	// Begin read transaction for repository2
	r = repository2->StartTransaction(CRepository::EReadTransaction);

	r = TransactionState(repository1);
	TEST2(r, EConcurrentReadWriteTransaction);

	r = TransactionState(repository2);
	TEST2(r, EReadTransaction);

	// Create value in with a concurrent read write transaction
	r = repository1->Create(KReal2, KReal2_InitialValue);
	TEST2(r, KErrNone);

	// Get value set using another transaction
	r = repository2->Get(KReal1, getValue);
	TEST2(r, KErrNone);
	TEST(getValue == KReal1_InitialValue);

	// Get value set outside of a transaction using repository3
	r = repository3->Get(KReal1, getValue);
	TEST2(r, KErrNone);
	TEST(getValue == KReal1_InitialValue);

	// Get value set within another transaction but not commited, within a transaction
	r = repository2->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	// Get value set within another transaction but not commited, outside of a transaction
	r = repository3->Get(KReal2, getValue);
	TEST2(r, KErrNotFound);

	// Commit repository2
	r = repository2->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Commit repository1
	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository3);
	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Check that value was persisted
	r = repository1->Get(KReal2, getValue);
	TEST2(r, KErrNone);
	TEST(getValue == KReal2_InitialValue);

	// Delete KReal2 to reset available test case variables
	r = repository1->Delete(KReal2);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository1);
	}

LOCAL_C void Defect058796()
	{
	TInt r;
	TUint32 errorId;

	TBuf8<2> buf8ValSmall;
	TBuf16<2> buf16ValSmall;

	TBuf8<16> buf8Val;
	TBuf16<16> buf16Val;

	TInt length;

	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	// Start a read write transaction
	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	// Try to get a string value using a TBuf8 with length which is too small
	r = repository1->Get(KString8, buf8ValSmall, length);
	TEST2(r, KErrOverflow);
	TEST(buf8ValSmall == KString8_Small_Value);
	TEST2(length, 13);

	// Try to get a string value using a TBuf16 with length which is too small
	r = repository1->Get(KString16, buf16ValSmall, length);
	TEST2(r, KErrOverflow);
	TEST(buf16ValSmall == KString16_Small_Value);
	TEST2(length, 13);

	// Set KNewString16 and KNewString8
	r = repository1->Create(KNewString16, KString16_InitialValue2);
	TEST2(r, KErrNone);
	r = repository1->Create(KNewString8, KString8_InitialValue2);
	TEST2(r, KErrNone);

	// Try to get KNewString16 with a TBuf16 which is a suitable size
	r = repository1->Get(KNewString16, buf16Val, length);
	TEST2(r, KErrNone);
	TEST(buf16Val == KString16_InitialValue2);
	TEST2(length, 16);

	// Try to get KNewString8 with a TBuf8 which is a suitable size
	r = repository1->Get(KNewString8, buf8Val, length);
	TEST2(r, KErrNone);
	TEST(buf8Val == KString8_InitialValue2);
	TEST2(length, 16);

	// Commit the transaction
	r = repository1->CommitTransaction(errorId);
	TEST2(r, KErrNone);

	// Delete KNewString8 to reset available test case variables
	r = repository1->Delete(KNewString8);
	TEST2(r, KErrNone);

	// Delete KNewString16 to reset available test case variables
	r = repository1->Delete(KNewString16);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository1);
	}


LOCAL_C void UnloadedPolicyKeyTest()
	{
	TInt r;
	TUint32 KeyId;
	CRepository* repository1;

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	//Creating Single Policy Key
	r = repository1->Create(KInt5, KInt5_SecondValue );
	TEST2(r,KErrNone);

	//Creating Ranged Policy Key
	r = repository1->Create(KInt3, KInt3_InitialValue);
	TEST2(r, KErrNone);

	//Creating Default Policy Key
	r = repository1->Create(KInt4, KInt4_InitialValue);
	TEST2(r,KErrNone);

	//Waiting for enough time for the repository to be evicted
	User::After(1000000);

	r = repository1->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId, 3);

	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	//Deleting Default Policy Key
	r = repository1->Delete(KInt5);
	TEST2(r, KErrNone);

	//Deleting Ranged Policy Key
	r = repository1->Delete(KInt3);
	TEST2(r, KErrNone);

	//Deleting Single Policy Key
	r = repository1->Delete(KInt4);
	TEST2(r, KErrNone);

	r = repository1->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId, 3);

	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-3239
@SYMTestCaseDesc Test for corrupt access policy when a repository is evicted and reloaded during a transaction
@SYMTestPriority High
@SYMTestActions Start a transaction and create keys with different access policies but evicts the repository prior to commiting it
@SYMTestExpectedResults The test should not fail with any panics
@SYMDEF DEF095718
*/
LOCAL_C void Defect095718()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3239 "));
	CRepository* repository1;
	TInt r;
	TUint32 KeyId;
	TBuf<KMaxFileName> src1;
	TBuf<KMaxFileName> dest1;
	_LIT(KDest, "c:\\private\\10202BE9\\centrep.ini");
	_LIT(KSrc, "z:\\private\\10202BE9\\centrepcache.ini10");
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	const TInt test_total = 3; //The test runs 3 times

	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	r = repository1->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);

	//Deleting the existing key so that we can re-create it later
	r = repository1->Delete(KInt5);
	TEST2(r, KErrNone);

	r = repository1->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId, 1);

	CleanupStack::PopAndDestroy(repository1);

	//Kill the server so that a new .ini file can be loaded when the server restarts
	KillProcess(KCentralRepositoryServerName);
	//Copy the .ini file from z: to c: so the server will load it
	RFs FileServer;
	FileServer.Connect();
	CleanupClosePushL(FileServer);
	CFileMan* fm = CFileMan::NewL(FileServer);
	CleanupStack::PushL(fm);
	dest1.Copy(KDest);
	src1.Copy(KSrc);
	r = fm->Delete(dest1);
	User::LeaveIfError(fm->Copy(src1,dest1));
	r = fm->Attribs(dest1, KEntryAttArchive, KEntryAttReadOnly, TTime(0), CFileMan::ERecurse);
	TEST2(r, KErrNone);

	//Test for unloaded policy key a set amount of times
	for(TInt i=0; i<test_total; i++)
		{
		//Check that the policy key used isn't from the unloaded/evicted repository
		UnloadedPolicyKeyTest();
		}

	//Deleting the ini file so that default values will be used again
	r = fm->Delete(dest1);
	TEST2(r, KErrNone);

	//Killing the server and restarting it so that default values are loaded
	KillProcess(KCentralRepositoryServerName);
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));

	CleanupStack::PopAndDestroy(repository1);
	CleanupStack::PopAndDestroy(fm);
	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-3243
@SYMTestCaseDesc Test that a repository client can be reused after a CommitTransaction and CancelTransaction Operation involving mulitple concurrent transactions
@SYMTestPriority High
@SYMTestExpectedResults The test should not fail with any panics
@SYMTestActions Start concurrent transactions and create keys in each one. Cancel one transaction and commit the others. Then restart the transactions to test for panics
@SYMDEF DEF098242
*/
LOCAL_C void Defect098242()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3243 "));
	TInt r;
	TUint32 KeyId;
	CRepository* repository1;
	CRepository* repository2;
	CRepository* repository3;
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository));
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository));

	//Starting 3 transactions
	r = repository1->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository2->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	r = repository3->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	//Adding values in each
	r = repository1->Create(KInt2, KInt2_InitialValue);
	TEST2(r, KErrNone);

	r = repository2->Create(KInt3, KInt3_InitialValue);
	TEST2(r, KErrNone);

	r = repository3->Create(KInt4, KInt4_InitialValue);
	TEST2(r, KErrNone);

	//Cancel the transaction of the first client
	repository1->CancelTransaction();

	//Committing the other 2 transactions
	r = repository2->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId,1);

	r = repository3->CommitTransaction(KeyId);
	TEST2(r, KErrLocked);
	TEST2(KeyId,KUnspecifiedKey);

	//Try Re-starting a transaction with the clients
	r = repository1->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	r = repository1->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId,0);

	r = repository2->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);

	//Deleting the key so the repository is in the same state as prior to the function call
	r = repository2->Delete(KInt3);
	TEST2(r, KErrNone);

	//Checking that these values were not made persistent
	r = repository2->Delete(KInt2);
	TEST2(r, KErrNotFound);

	r = repository2->Delete(KInt4);
	TEST2(r, KErrNotFound);

	r = repository2->CommitTransaction(KeyId);
	TEST2(r, KErrNone);
	TEST2(KeyId,1);

	r = repository1->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository3);
	CleanupStack::PopAndDestroy(repository2);
	CleanupStack::PopAndDestroy(repository1);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4011
@SYMTestCaseDesc  Test for PDEF112273 - When creating commsdat Backup & restore metadata is not set correctly
@SYMTestPriority High
@SYMTestActions The test uses the 00112273.TXT ini file, where the default meta data value is set to be 0x00100000.
				The test creates a CRepository object with UID=0x00112273 and creates a single integer setting with
				value 1 and no meta data. In this case the setting meta data should be the default one - 0x00100000.
				Then the test begins a transaction and within the transaction: the setting gets deleted and then - recreated
				again but with a different value and no meta data. The transaction is commited.
				The setting meta data value should be 0x00100000 within the transaction and outisde the transaction.
@SYMTestExpectedResults The test should not fail with any panics
@SYMDEF PDEF112273
*/
void DoPDEF112273Test1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4011 "));
	const TUid KTestCenRepUid = {0x00112273};
	CRepository* repository = CRepository::NewLC(KTestCenRepUid);
	//Create a setting
	const TInt KTestSettingId = 123;
	TInt err = repository->Create(KTestSettingId, 1);
	TEST2(err, KErrNone);

	const TUint32 KDefaultMeta = 0x00100000;
	//Check setting meta. The meta value should be KDefaultMeta.
	TUint32 meta = 0;
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KDefaultMeta);

	err = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(err, KErrNone);
	//In a transaction. Delete the created setting.
	err = repository->Delete(KTestSettingId);
	TEST2(err, KErrNone);
	//In a transaction. Re-create the deleted setting but with a different value.
	err = repository->Create(KTestSettingId, 2);
	TEST2(err, KErrNone);
	//In a transaction. Get the setting meta. The meta value should be KDefaultMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KDefaultMeta);

	TUint32 keyInfo = 0;
	err = repository->CommitTransaction(keyInfo);
	TEST2(err, KErrNone);

	//Not in transaction. Get the setting meta. The meta value should be KDefaultMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KDefaultMeta);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4012
@SYMTestCaseDesc  Test for PDEF112273 - When creating commsdat Backup & restore metadata is not set correctly
@SYMTestPriority High
@SYMTestActions The test uses the 00112273.TXT ini file, where the default meta data value is set to be 0x00100000.
				The test creates a CRepository object with UID=0x00112273.
				Then the test begins a transaction and within the transaction: setting with id=0x0000001 (from the ini file)
				and setting meta=0x00200000 will be deleted and then recreated again with a different value and no meta.
				The transaction is commited.
				The setting meta data value should be 0x00200000 within the transaction and outisde the transaction.
@SYMTestExpectedResults The test should not fail with any panics
@SYMDEF PDEF112273
*/
void DoPDEF112273Test2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4012 "));
	const TUid KTestCenRepUid = {0x00112273};
	CRepository* repository = CRepository::NewLC(KTestCenRepUid);
	//Ini file - KTestSettingId setting properties:
	const TInt KTestSettingId = 0x0000001;
	const TInt KTestSettingVal = 1;
	const TUint32 KSettingMeta = 0x00200000;
	//Check setting meta. The meta value should be KDefaultMeta.
	TUint32 meta = 0;
	TInt err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KSettingMeta);

	err = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(err, KErrNone);
	//In a transaction. Delete the created setting.
	err = repository->Delete(KTestSettingId);
	TEST2(err, KErrNone);
	//In a transaction. Re-create the deleted setting with different value.
	err = repository->Create(KTestSettingId, KTestSettingVal + 1);
	TEST2(err, KErrNone);
	//In a transaction. Get the setting meta. The meta value should be KSettingMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KSettingMeta);

	TUint32 keyInfo = 0;
	err = repository->CommitTransaction(keyInfo);
	TEST2(err, KErrNone);

	//Not in transaction. Get the setting meta. The meta value should be KSettingMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KSettingMeta);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4013
@SYMTestCaseDesc  Test for PDEF112273 - When creating commsdat Backup & restore metadata is not set correctly
@SYMTestPriority High
@SYMTestActions The test uses the 00112273.TXT ini file, where the default meta data value is set to be 0x00100000.
				The test creates a CRepository object with UID=0x00112273.
				Then the test begins a transaction and within the transaction: a new setting with no meta is created.
				The transaction is commited.
				The setting meta data value should be 0x00100000 within the transaction and outisde the transaction.
@SYMTestExpectedResults The test should not fail with any panics
@SYMDEF PDEF112273
*/
void DoPDEF112273Test3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4013 "));
	const TUid KTestCenRepUid = {0x00112273};
	CRepository* repository = CRepository::NewLC(KTestCenRepUid);

	TInt err = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(err, KErrNone);
	//In a transaction. Create a setting
	const TInt KTestSettingId = 1234;
	err = repository->Create(KTestSettingId, 1);
	TEST2(err, KErrNone);

	const TUint32 KDefaultMeta = 0x00100000;
	TUint32 meta = 0;
	//In a transaction. Get the setting meta. The meta value should be KDefaultMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KDefaultMeta);

	TUint32 keyInfo = 0;
	err = repository->CommitTransaction(keyInfo);
	TEST2(err, KErrNone);

	//Not in transaction. Get the setting meta. The meta value should be KDefaultMeta.
	err = repository->GetMeta(KTestSettingId, meta);
	TEST2(err, KErrNone);
	TEST2(meta, KDefaultMeta);

	CleanupStack::PopAndDestroy(repository);
	}

void PDEF112273L()
	{
	DoPDEF112273Test1L();
	DoPDEF112273Test2L();
	DoPDEF112273Test3L();
	}

LOCAL_C void TransactionsFuncTestsL()
	{
	TheTest.Next(_L("Committing changes causes other sessions' active transactions to fail with KErrLocked"));
	CommittingChangesFailsOtherTransactionsL();

	TheTest.Next(_L("Committing no changes does not fail other sessions' active transactions"));
	CommittingNoChangesDoesNotFailOtherTransactionsL();

	TheTest.Start(_L("Create values using transaction"));
	CreateValueUsingTransactionL();

	TheTest.Next(_L("Get transaction state in transactions and test for rollback in failed transactions"));
	TransactionStateAndRollBackL();

	TheTest.Next(_L("Multiple Read-Write Transaction error and panic conditions"));
	TransactionPanicConditionsThread();

	TheTest.Next(_L("Commit Transaction error and panic condition"));
	CommitTransactionPanicConditionsThread();

	TheTest.Next(_L("Clean up using rollback and failed transactions"));
	CleanupRollBackAndFailTransactionL();

	TheTest.Next(_L("Multiple Read Transaction conditions"));
	ReadTransactionConditionsL();

	TheTest.Next(_L("Multiple Read Transaction with upgrade error conditions"));
	UpgradeReadTransactionErrorConditionsL();

	TheTest.Next(_L("Transaction conditions with read operations"));
	TransactionConditionsForReadL();

	TheTest.Next(_L("Transaction error conditions with get operations"));
	TransactionErrorConditionsForGetL();

	TheTest.Next(_L("Transaction error conditions with find operations"));
	TransactionErrorConditionsForFindL();

	TheTest.Next(_L("Transaction error conditions with findeq operations"));
	TransactionErrorConditionsForFindEqL();

	TheTest.Next(_L("Transaction error conditions with findneq operations"));
	TransactionErrorConditionsForFindNeqL();

	TheTest.Next(_L("Transaction error conditions with create operations"));
	TransactionErrorConditionsForCreateL();

	TheTest.Next(_L("Transaction error conditions with delete operations"));
	TransactionErrorConditionsForDeleteL();

	TheTest.Next(_L("Transaction error conditions with set operations"));
	TransactionErrorConditionsForSetL();

	TheTest.Next(_L("Transaction error conditions with move operations"));
	TransactionErrorConditionsForMoveL();

	TheTest.Next(_L("Transaction conditions with move operations"));
	TransactionConditionsForMoveL();

	// Concurrent read/write transactions...

	TheTest.Next(_L("Concurrent read/write transaction"));
	ConcurrentReadWriteTransactionStatesL();

	TheTest.Next(_L("Concurrent read/write transaction with other reads operations"));
	ConcurrentReadWriteTransactionWithReadOperationsL();

	// Test cases for defects ...

	TheTest.Next(_L("Tests for get functions, as required in DEF058796"));
	Defect058796();

	TheTest.Next(_L("Checks for corrupt policy keys, as required in DEF095718"));
	Defect095718();

	TheTest.Next(_L("Test for restarting a transaction from the same client, as required in DEF098242"));
	Defect098242();

	TheTest.Next(_L("PDEF112273 - When creating commsdat Backup & restore metadata is not set correctly"));
	PDEF112273L();

	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("Central Repository transactions functional tests"));
	CleanupCDriveL();
	TransactionsFuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
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

