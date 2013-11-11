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

///////////////////////////////////////////////////////////////////////////////////////
///////////////         RSqlDatabase OOM tests         ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1615, SYSLIB-SQL-CT-1639
@SYMTestCaseDesc		RSqlDatabase::Create() OOM test - secure and non-secure databases.
						Precondition: the database does not exist.
						The test calls RSqlDatabase::Create() while simulating OOM failures and checks
						that there are no memory and resource leaks.
						Note: It's possible for a database to be created even after memory allocation
						has failed. This is because SQLITE reuses some pages of the page cache which
						have been allocated but are curently not in use. This means it is necessary
						to delete the database and continue checking for memory and resource leaks
						even after a database has been created successfully.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Create() OOM test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
                        REQ10271
                        REQ10273
                        REQ10274
*/
void DoCreateDatabaseOomTest(const TDesC& aDbFileName, TDbType aDbType, TInt aExpectedError, const TDesC8* aConfigStr = NULL)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1639 RSqlDatabase::Create() - OOM test"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	enum TMethodType {ENonLeavingMethod, ELeavingMethod};
	const TMethodType KMethodType[] = {ENonLeavingMethod, ELeavingMethod};
	for(TInt j=0;j<sizeof(KMethodType)/sizeof(KMethodType[0]);++j)
		{
		for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
			{
			if(aExpectedError != KErrAlreadyExists)
				{
				(void)RSqlDatabase::Delete(aDbFileName);
				}
			TInt err = KErrNone;
			TInt failingAllocationNo = 0;
			TInt allocationNo = 0;
			TInt maxAllocationNo = TheOomTestType[i] == EServerSideTest ? KDoCreateDatabaseOomTestAllocLimitServer : KDoCreateDatabaseOomTestAllocLimitClient;
			while(allocationNo < maxAllocationNo)
				{
				MarkHandles();
				MarkAllocatedCells();

				__UHEAP_MARK;

				RSqlDatabase db;

				SetDbHeapFailure(TheOomTestType[i], ++allocationNo);

				if(KMethodType[j] == ENonLeavingMethod)
					{
					err =  aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy, aConfigStr) : db.Create(aDbFileName, aConfigStr);
					}
				else
					{
					TRAP(err, aDbType == ESecureDb ? db.CreateL(aDbFileName, securityPolicy, aConfigStr) : db.CreateL(aDbFileName, aConfigStr));
					}

				db.Close();
				if(err != KErrNoMemory)
					{
					TEST2(err, aExpectedError);
					}
				else
					{
					failingAllocationNo = allocationNo;
					}

				ResetDbHeapFailure(TheOomTestType[i]);

				if(err == KErrNone && aExpectedError != KErrAlreadyExists)
					{
					err = db.Delete(aDbFileName);
					TEST2(err, KErrNone);
					}

				__UHEAP_MARKEND;

				CheckAllocatedCells();
				CheckHandles();
				
				if(err == KErrNoMemory && allocationNo == maxAllocationNo)
					{
					maxAllocationNo += 10;
					}
				}
			TEST2(err, aExpectedError);
			PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo + 1);
			}
		}
	RSqlDatabase::Delete(aDbFileName);
	securityPolicy.Close();
	}

//"RSqlDatabase::Open()" OOM test
void OpenDatabaseL(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	TInt err = aDb.Open(aDbFileName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::OpenL()" OOM test
void OpenDatabase2L(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	aDb.OpenL(aDbFileName);
	}

//"RSqlDatabase::Open()" + config string OOM test
void OpenDatabase3L(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	_LIT8(KConfig, "cache_size=128;compaction=auto");
	TInt err = aDb.Open(aDbFileName, &KConfig);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Open() - from handle" OOM test
void OpenDatabaseFromHandleL(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	TInt err = aDb.Open(aDbFileName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Open() - from handle + config string" OOM test
void OpenDatabaseFromHandle2L(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	_LIT8(KConfig, "cache_size=128;compaction=background");
	TInt err = aDb.Open(aDbFileName, &KConfig);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Exec()" OOM test (8-bit SQL statements)
void ExecStatement8L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	_LIT8(KSqlString, "BEGIN;\
					   CREATE TABLE BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT);\
	                   INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(4562, 123456789012345, 78612.0091, 'text data');\
	                   COMMIT;");
	TInt err = aDb.Exec(KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Exec()" OOM test (16-bit SQL statements)
void ExecStatement16L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	_LIT(KSqlString, "BEGIN;\
					  CREATE TABLE BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT);\
	                  INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(4562, 123456789012345, 78612.0091, 'text data');\
	                  COMMIT;");
	TInt err = aDb.Exec(KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::SetIsolationLevel()" OOM test
void SetIsolationLevelL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	TInt err = aDb.SetIsolationLevel(RSqlDatabase::EReadUncommitted);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Size()" OOM test
void DbSizeL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	TInt rc = aDb.Size();
	User::LeaveIfError(rc);
	}

//"RSqlDatabase::Size(TSize&)" OOM test
void DbSize2L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	RSqlDatabase::TSize size;
	TInt err = aDb.Size(size);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Size(TSize&)" OOM test - attached database
void DbAttachSize2L(RSqlDatabase& aDb, const TDesC& aDbName, TDbType)
	{
	_LIT(KAttachDbName, "HHH");
	TInt err = aDb.Attach(aDbName, KAttachDbName);
	User::LeaveIfError(err);
	RSqlDatabase::TSize size;
	err = aDb.Size(size, KAttachDbName);
	(void)aDb.Detach(KAttachDbName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Delete()" OOM test
void DeleteDbL(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	aDb.Close();
	TInt err = RSqlDatabase::Delete(aDbFileName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Attach()" OOM test
void AttachDatabaseL(RSqlDatabase& aDb, const TDesC&, TDbType aDbType)
	{
	_LIT(KDbName, "Db2");
	TInt err = KErrNone;
	if(aDbType == ESecureDb)
		{
		err = aDb.Attach(KSecureTestDb, KDbName);
		}
	else
		{
		err = aDb.Attach(KAttachDb, KDbName);
		}
	User::LeaveIfError(err);
	err = aDb.Detach(KDbName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Attach() - from handle" OOM test
void AttachDatabase2L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	_LIT(KDbName, "Db2");
	TInt err = aDb.Attach(KPrivateTestDb, KDbName);
	User::LeaveIfError(err);
	err = aDb.Detach(KDbName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Copy()" OOM test
void CopyDatabaseL(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType)
	{
	aDb.Close();
	TInt err = RSqlDatabase::Copy(aDbFileName,aDbFileName);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::GetSecurityPolicy()" OOM test
void GetSecurityPolicyL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	RSqlSecurityPolicy policy;
	TInt err = aDb.GetSecurityPolicy(policy);
	policy.Close();
	User::LeaveIfError(err);
	}

//"RSqlDatabase::GetSecurityPolicyL()" OOM test
void GetSecurityPolicy2L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	RSqlSecurityPolicy policy;
	CleanupClosePushL(policy);
	aDb.GetSecurityPolicyL(policy);
	CleanupStack::PopAndDestroy(&policy);
	}

//"RSqlDatabase::ReserveDriveSpace()" OOM test
void ReserveDriveSpaceL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	TInt err = aDb.ReserveDriveSpace(0);
	User::LeaveIfError(err);
	aDb.FreeReservedSpace();
	}

//"RSqlDatabase::GetReserveAccess()" OOM test
void GetReserveAccessL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	TInt err = aDb.ReserveDriveSpace(0);
	User::LeaveIfError(err);
	err = aDb.GetReserveAccess();
	User::LeaveIfError(err);
	aDb.ReleaseReserveAccess();
	aDb.FreeReservedSpace();
	}

//"RSqlDatabase::LastInsertedRowId()" OOM test
void DbLastInsertedRowIdL(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	TInt64 rowid = aDb.LastInsertedRowId();
	User::LeaveIfError(rowid);
	}



/**
@SYMTestCaseID			SYSLIB-SQL-CT-1616
@SYMTestCaseDesc		RSqlDatabase methods OOM test
						Precondition: the database exists.
						The test calls the given as an argument function while simulating OOM failures
						and checks that there are no memory and resource leaks.
						Note: It's possible for database operations to be performed even after memory
						allocation has failed. This is because SQLITE reuses some pages of the page
						cache which have been allocated but are curently not in use. This means it is
						necessary to undo any operations on the database and continue checking for
						memory and resource leaks even after an operation has been completed successfully.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase methods OOM tests
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF105444
*/
void DoDbOomTest(TDbFuncPtrL aTestFunctionPtrL, const TDesC& aDbFileName, TDbAction aDbAction, TDbType aDbType)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1616 RSqlDatabase - OOM test"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		//Recreate the database file
		RSqlDatabase::Delete(aDbFileName);
		RSqlDatabase db;
		TInt err = aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy) : db.Create(aDbFileName);
		db.Close();
		TEST2(err, KErrNone);

		TInt failingAllocationNo = 0;
		TInt allocationNo = 0;
		TInt maxAllocationNo = TheOomTestType[i] == EServerSideTest ? KDoDbOomTestAllocLimitServer : KDoDbOomTestAllocLimitClient;
		while(allocationNo < maxAllocationNo)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			if(TheOomTestType[i] == EServerSideTest)
				{//If aDbAction is EOpenDb, then we will delay the heap failure simulation, until the database is opened
				SetDbHeapFailure(TheOomTestType[i], ++allocationNo, aDbAction == EOpenDb);
				}

			//if aDbAction is EOpenDb then this is a OOM test different than a test for RSqlDatabase::Open
			if(aDbAction == EOpenDb)
				{
				err = db.Open(aDbFileName);
				TEST2(err, KErrNone);
				}

			if(TheOomTestType[i] == EClientSideTest)
				{
				SetDbHeapFailure(TheOomTestType[i], ++allocationNo);
				}

			TRAP(err, (*aTestFunctionPtrL)(db, aDbFileName, aDbType));
			if(err != KErrNoMemory)
				{
				TEST2(err, KErrNone);
				}
			else
				{
				failingAllocationNo = allocationNo;
				}

			ResetDbHeapFailure(TheOomTestType[i]);

			if(aTestFunctionPtrL == &ExecStatement8L || aTestFunctionPtrL == &ExecStatement16L)
				{
				_LIT(KSqlDropString, "DROP TABLE IF EXISTS BBB;");
				err = db.Exec(KSqlDropString);
				TEST(err >= 0);
				err = KErrNone;
				}
			else if(aTestFunctionPtrL == &DeleteDbL && err == KErrNone)
				{
				err = aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy) : db.Create(aDbFileName);
				TEST2(err, KErrNone);
				}
			db.Close();

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			
			if(err == KErrNoMemory && allocationNo == maxAllocationNo)
				{
				maxAllocationNo += 10;
				}
			}
		TEST2(err, KErrNone);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo + 1);
		}
	//Delete the database file
	RSqlDatabase::Delete(aDbFileName);
	securityPolicy.Close();
	}

//An attempt to open a non-secure database somehow happened to be in the server's private data cage.
void DoDbOomTest2()
	{
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		TInt err = KErrNone;
		TInt failingAllocationNo = 0;
		TInt allocationNo = 0;
		TInt maxAllocationNo = TheOomTestType[i] == EServerSideTest ? KDoDbOomTest2AllocLimitServer : KDoDbOomTest2AllocLimitClient;
		while(allocationNo < maxAllocationNo)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			SetDbHeapFailure(TheOomTestType[i], ++allocationNo);

			RSqlDatabase db;
			err = db.Open(KSecureAttachDb2);
			db.Close();
			if(err != KErrNoMemory)
				{
				TEST2(err, KSqlErrGeneral);
				}
			else
				{
				failingAllocationNo = allocationNo;
				}

			ResetDbHeapFailure(TheOomTestType[i]);

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			
			if(err == KErrNoMemory && allocationNo == maxAllocationNo)
				{
				maxAllocationNo += 10;
				}
			}
		TEST2(err, KSqlErrGeneral);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo + 1);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//RSqlDatabase OOM tests
void DbOomTestsL(TDbType aDbType)
	{
	TPtrC dbFileName(KTestDb);
	if(aDbType == ESecureDb)
		{
		dbFileName.Set(KSecureTestDb());
		}

	CreateAttachDb();

	TheTest.Printf(_L("===RSqlDatabase::Create()\r\n"));
	DoCreateDatabaseOomTest(dbFileName, aDbType, KErrNone);

	TheTest.Printf(_L("===RSqlDatabase::Create() + config string\r\n"));
	_LIT8(KConfigStr, "page_size=2048");
	DoCreateDatabaseOomTest(dbFileName, aDbType, KErrNone, &KConfigStr);

	TheTest.Printf(_L("===RSqlDatabase::Create() + config string + manual compaction\r\n"));
	_LIT8(KConfigStr2, "compaction=manual");
	DoCreateDatabaseOomTest(dbFileName, aDbType, KErrNone, &KConfigStr2);
	
	TheTest.Printf(_L("===RSqlDatabase::Create() + config string + background compaction\r\n"));
	_LIT8(KConfigStr3, "compaction=background");
	DoCreateDatabaseOomTest(dbFileName, aDbType, KErrNone, &KConfigStr3);
	
	TheTest.Printf(_L("===RSqlDatabase::Create() + config string + auto compaction\r\n"));
	_LIT8(KConfigStr4, "compaction=auto");
	DoCreateDatabaseOomTest(dbFileName, aDbType, KErrNone, &KConfigStr4);

	if(aDbType == ENonSecureDb)
		{//Private database is not a database taht will be created in the SQL server private data cage.
		(void)RSqlDatabase::Delete(KPrivateTestDb);
		TheTest.Printf(_L("===RSqlDatabase::Create() private database + config string + manual compaction\r\n"));
		DoCreateDatabaseOomTest(KPrivateTestDb, ENonSecureDb, KErrNone, &KConfigStr2);
		
		(void)RSqlDatabase::Delete(KPrivateTestDb);
		TheTest.Printf(_L("===RSqlDatabase::Create() private database + config string + background compaction\r\n"));
		DoCreateDatabaseOomTest(KPrivateTestDb, ENonSecureDb, KErrNone, &KConfigStr3);
		
		(void)RSqlDatabase::Delete(KPrivateTestDb);
		TheTest.Printf(_L("===RSqlDatabase::Create() private database + config string + auto compaction\r\n"));
		DoCreateDatabaseOomTest(KPrivateTestDb, ENonSecureDb, KErrNone, &KConfigStr4);
		}
	
	TheTest.Printf(_L("===RSqlDatabase::Open()\r\n"));
	DoDbOomTest(&OpenDatabaseL, dbFileName, ENotOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::OpenL()\r\n"));
	DoDbOomTest(&OpenDatabase2L, dbFileName, ENotOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Open() + config string\r\n"));
	DoDbOomTest(&OpenDatabase3L, dbFileName, ENotOpenDb, aDbType);
	
	if(aDbType == ENonSecureDb)
		{//Private database cannot be opened as a secure database
		TheTest.Printf(_L("===RSqlDatabase::Open() - from handle\r\n"));
		DoDbOomTest(&OpenDatabaseFromHandleL, KPrivateTestDb, ENotOpenDb, aDbType);

		TheTest.Printf(_L("===RSqlDatabase::Open() - from handle + config string\r\n"));
		DoDbOomTest(&OpenDatabaseFromHandle2L, KPrivateTestDb, ENotOpenDb, aDbType);
		}

	TheTest.Printf(_L("===RSqlDatabase::Exec(), 8-bit SQL\r\n"));
	DoDbOomTest(&ExecStatement8L, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Exec(), 16-bit SQL\r\n"));
	DoDbOomTest(&ExecStatement16L, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::SetIsolationLevel()\r\n"));
	DoDbOomTest(&SetIsolationLevelL, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Size()\r\n"));
	DoDbOomTest(&DbSizeL, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Size(TSize&)\r\n"));
	DoDbOomTest(&DbSize2L, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Size(TSize&) - attached database\r\n"));
	DoDbOomTest(&DbAttachSize2L, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Delete()\r\n"));
	DoDbOomTest(&DeleteDbL, dbFileName, ENotOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Attach()\r\n"));
	DoDbOomTest(&AttachDatabaseL, dbFileName, EOpenDb, aDbType);

	//Ensure that the private database to be attached exists
	PrepareAttachFromHandle();
	TheTest.Printf(_L("===RSqlDatabase::Attach() - from handle\r\n"));
	DoDbOomTest(&AttachDatabase2L, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Copy()\r\n"));
	DoDbOomTest(&CopyDatabaseL, dbFileName, ENotOpenDb, aDbType);

	if(aDbType == ESecureDb)
		{
		TheTest.Printf(_L("===RSqlDatabase::GetSecurityPolicy()\r\n"));
		DoDbOomTest(&GetSecurityPolicyL, dbFileName, EOpenDb, aDbType);

		TheTest.Printf(_L("===RSqlDatabase::GetSecurityPolicyL()\r\n"));
		DoDbOomTest(&GetSecurityPolicy2L, dbFileName, EOpenDb, aDbType);
		}

	TheTest.Printf(_L("===RSqlDatabase::ReserveDriveSpace()\r\n"));
	DoDbOomTest(&ReserveDriveSpaceL, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::GetReserveAccess()\r\n"));
	DoDbOomTest(&GetReserveAccessL, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::LastInsertedRowId()\r\n"));
	DoDbOomTest(&DbLastInsertedRowIdL, dbFileName, EOpenDb, aDbType);

	TheTest.Printf(_L("===RSqlDatabase::Open(), non-secure database in server data cage\r\n"));
	DoDbOomTest2();
	}
