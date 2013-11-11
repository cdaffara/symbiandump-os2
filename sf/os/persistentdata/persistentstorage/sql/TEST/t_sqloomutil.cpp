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

#include "t_sqloom.h"
#include "SqlResourceTester.h"		//TSqlResourceTester


TInt TheHandleCount1B = 0;
TInt TheHandleCount2B = 0;
TInt TheAllocatedCellsCountB = 0;
TOomTestType TheOomTestType[2] = {EClientSideTest, EServerSideTest};

///////////////////////////////////////////////////////////////////////////////////////
///////////////         OOM tests - utility functions        //////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	(void)RSqlDatabase::Delete(KSecureAttachDb);
	(void)RSqlDatabase::Delete(KAttachDb);
	(void)RSqlDatabase::Delete(KSecureTestDb);
	(void)RSqlDatabase::Delete(KTestDb);
	(void)RSqlDatabase::Delete(KSecureDb2);
	(void)RSqlDatabase::Delete(KTestDb2);
	(void)RSqlDatabase::Delete(KPrivateTestDb);
	}

void Check(TInt64 aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
	
void Check(TInt64 aValue, TInt64 aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %ld, got: %ld\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = fs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	fs.Close();
	}

void PrepareAttachFromHandle()
	{
	(void)RSqlDatabase::Delete(KAttachDb);
	(void)RSqlDatabase::Delete(KPrivateTestDb);

	RSqlDatabase db;
	TInt err = db.Create(KAttachDb);
	db.Close();
	TEST2(err, KErrNone);

    RFs fs;
	err = fs.Connect();
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KAttachDb, KPrivateTestDb);
	TEST2(err, KErrNone);
	}

void MarkHandles()
	{
	RThread().HandleCount(TheHandleCount1B, TheHandleCount2B);
	}

void CheckHandles()
	{
	TInt endHandleCount1E;
	TInt endHandleCount2E;

	RThread().HandleCount(endHandleCount1E, endHandleCount2E);

	TEST(TheHandleCount1B == endHandleCount1E);
	TEST(TheHandleCount2B == endHandleCount2E);
	}

void MarkAllocatedCells()
	{
	TheAllocatedCellsCountB = User::CountAllocCells();
	}

void CheckAllocatedCells()
	{
	TInt allocatedCellsCountE = User::CountAllocCells();
	TEST(allocatedCellsCountE == TheAllocatedCellsCountB);
	}

void PrintEndOfOomTest(TOomTestType aOomTestType, TInt aFailingAllocationNo)
	{
	_LIT(KClientSide, "Client side");
	_LIT(KServerSide, "Server side");
	RDebug::Print(_L("=== %S OOM Test succeeded at heap failure rate of %d ===\r\n"),
						aOomTestType == EClientSideTest ? &KClientSide() : &KServerSide(),
						aFailingAllocationNo);
	}

//If aDelayed is true, then the SQL server will delay the heap failure simulation until the database is opened.
void SetDbHeapFailure(TOomTestType aOomTestType, TInt aFailingAllocationNo, TBool aDelayed)
	{
	const TInt KDelayedDbHeapFailureMask = 0x1000;
	if(aOomTestType == EClientSideTest)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, aFailingAllocationNo);
		}
	else
		{
		TSqlResourceTester::SetDbHeapFailure(RHeap::EDeterministic | (aDelayed ? KDelayedDbHeapFailureMask : 0), aFailingAllocationNo);
		}
	}

void ResetDbHeapFailure(TOomTestType aOomTestType)
	{
	if(aOomTestType == EClientSideTest)
		{
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		}
	else
		{
		TSqlResourceTester::SetDbHeapFailure(RHeap::ENone, 0);
		}
	}

void SetHeapFailure(TOomTestType aOomTestType, TInt aFailingAllocationNo)
	{
	if(aOomTestType == EClientSideTest)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, aFailingAllocationNo);
		}
	else
		{
		TSqlResourceTester::Mark();
		TSqlResourceTester::SetHeapFailure(RHeap::EDeterministic, aFailingAllocationNo);
		}
	}

void ResetHeapFailure(TOomTestType aOomTestType)
	{
	if(aOomTestType == EClientSideTest)
		{
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		}
	else
		{
		TSqlResourceTester::SetHeapFailure(RHeap::ENone, 0);
		TSqlResourceTester::Check();
		}
	}

void CreateTestSecurityPolicy(RSqlSecurityPolicy& aSecurityPolicy)
	{
	TInt err = aSecurityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KPolicy3);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EWritePolicy, KPolicy2);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EReadPolicy, KPolicy2);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EWritePolicy, KPolicy1);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EWritePolicy, KPolicy3);
	TEST2(err, KErrNone);
	}

void CreateAttachDb()
	{
	RSqlDatabase db;

	TInt err = db.Create(KAttachDb);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE Aab(F1 INTEGER, F2 TEXT)"));
	TEST(err >= 0);
	db.Close();

	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);

	err = db.Create(KSecureAttachDb, securityPolicy);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE Aab(F1 INTEGER, F2 TEXT)"));
	TEST(err >= 0);
	db.Close();

	securityPolicy.Close();
	}
