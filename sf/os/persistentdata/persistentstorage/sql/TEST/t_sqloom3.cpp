// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

RTest TheTest(_L("t_sqloom3 test"));

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
			TInt failingAllocationNo = 0;//the real exit point of the OOM test. allocationNo is set maxAllocationNo times.
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

//"RSqlDatabase::Exec()" OOM test (8-bit SQL statements), syntax error
void ExecBadStatement8L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	_LIT8(KSqlString, "CREATE TABL BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT)");
	TInt err = aDb.Exec(KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlDatabase::Exec()" OOM test (16-bit SQL statements), syntax error
void ExecBadStatement16L(RSqlDatabase& aDb, const TDesC&, TDbType)
	{
	_LIT(KSqlString, "CREATE TABLE B!B!B(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT)");
	TInt err = aDb.Exec(KSqlString);
	User::LeaveIfError(err);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1813
@SYMTestCaseDesc		RSqlDatabase methods - negative OOM test
						Precondition: the database exists.
						The test calls the given as an argument function while simulating OOM failures
						and checks that there are no memory and resource leaks. The calling function is expected to fail
						with aExpectedError error.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase methods - negative OOM tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void DoDbOomNegativeTest(TDbFuncPtrL aTestFunctionPtrL, const TDesC& aDbFileName, TDbAction aDbAction, TInt aExpectedError)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1813 RSqlDatabase - negative OOM test"));
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		TInt err = KErrNoMemory;
		TInt failingAllocationNo = 0;
		while(err == KErrNoMemory)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			if(TheOomTestType[i] == EServerSideTest)
				{//If aDbAction is EOpenDb, then we will delay the heap failure simulation, until the database is opened
				SetDbHeapFailure(TheOomTestType[i], ++failingAllocationNo, aDbAction == EOpenDb);
				}

			RSqlDatabase db;
			//if aDbAction is EOpenDb then this is a OOM test different than a test for RSqlDatabase::Open
			if(aDbAction == EOpenDb)
				{
				err = db.Open(aDbFileName);
				TEST2(err, KErrNone);
				}

			if(TheOomTestType[i] == EClientSideTest)
				{
				SetDbHeapFailure(TheOomTestType[i], ++failingAllocationNo);
				}

			TRAP(err, (*aTestFunctionPtrL)(db, aDbFileName, ENonSecureDb));
			db.Close();
			if(err != KErrNoMemory)
				{
				TEST2(err, aExpectedError);
				}

			ResetDbHeapFailure(TheOomTestType[i]);

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			}
		TEST2(err, aExpectedError);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo);
		}
	RSqlDatabase::Delete(aDbFileName);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//RSqlDatabase - negative OOM tests
void DbOomNegativeTestsL()
	{
	TheTest.Printf(_L("===RSqlDatabase::Open(), non-existing drive\r\n"));
	_LIT(KDbName1, "A:[1111CCCC]db1.db");
	DoDbOomNegativeTest(&OpenDatabaseL, KDbName1, ENotOpenDb, KErrNotReady);

	TheTest.Printf(_L("===RSqlDatabase::Open(), non-existing file\r\n"));
	_LIT(KDbName2, "c:\\test\\nofile.db");
	DoDbOomNegativeTest(&OpenDatabaseL, KDbName2, ENotOpenDb, KErrNotFound);

	TheTest.Printf(_L("===RSqlDatabase::Open(), zero-length name\r\n"));
	_LIT(KDbName3, "");
	DoDbOomNegativeTest(&OpenDatabaseL, KDbName3, ENotOpenDb, KErrBadName);

	TheTest.Printf(_L("===RSqlDatabase::Open(), directory name\r\n"));
	_LIT(KDbName4, "C:\\TEST\\");
	DoDbOomNegativeTest(&OpenDatabaseL, KDbName4, ENotOpenDb, KErrBadName);

	TheTest.Printf(_L("===RSqlDatabase::Create(), secure database already exists\r\n"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	RSqlDatabase db;
	TInt err = db.Create(KSecureDb2, securityPolicy);
	TEST2(err, KErrNone);
	db.Close();
	securityPolicy.Close();
	DoCreateDatabaseOomTest(KSecureDb2, ESecureDb, KErrAlreadyExists);

	TheTest.Printf(_L("===RSqlDatabase::Create(), database already exists\r\n"));
	err = db.Create(KTestDb2);
	TEST2(err, KErrNone);
	db.Close();
	DoCreateDatabaseOomTest(KTestDb2, ENonSecureDb, KErrAlreadyExists);

	TheTest.Printf(_L("===RSqlDatabase::Exec(), 8-bit SQL, syntax error\r\n"));
	err = db.Create(KTestDb);
	TEST2(err, KErrNone);
	db.Close();
	DoDbOomNegativeTest(&ExecBadStatement8L, KTestDb, EOpenDb, KSqlErrGeneral);

	TheTest.Printf(_L("===RSqlDatabase::Exec(), 16-bit SQL, syntax error\r\n"));
	err = db.Create(KTestDb);
	TEST2(err, KErrNone);
	db.Close();
	DoDbOomNegativeTest(&ExecBadStatement16L, KTestDb, EOpenDb, KSqlErrGeneral);
	}

void DEF114297PrepareStmtL(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSelectSql, "SELECT e.* FROM edge AS e, node AS n1, node AS n2 WHERE n1.name = 'alice' AND n2.name = 'bob' AND e.orig = n1.id AND e.dest = n2.id ORDER BY n2.name DESC");
	TInt err = aStmt.Prepare(aDb, KSelectSql);
	User::LeaveIfError(err);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4004
@SYMTestCaseDesc		Test for DEF114297 - SqlSrv.EXE::!SQL Server OOM Test for PrepareL.
						The test does an OOM test for RSqlStatement::Prepare() using a specific SELECT SQL statement.
@SYMTestPriority		High
@SYMTestActions			Test for DEF114297 - SqlSrv.EXE::!SQL Server OOM Test for PrepareL.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF114297
*/
void DEF114297()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4004  ===DEF114297 - SqlSrv.EXE::!SQL Server OOM Test for PrepareL "));
	(void)RSqlDatabase::Delete(KTestDb);
	RSqlDatabase db;
	TInt err = db.Create(KTestDb);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE node(id INTEGER PRIMARY KEY,name TEXT)"));
	TEST2(err, 1);
	err = db.Exec(_L("CREATE INDEX node_idx ON node(name)"));
	TEST2(err, 1);
	err = db.Exec(_L("CREATE TABLE edge(orig INTEGER REFERENCES node,dest INTEGER REFERENCES node,PRIMARY KEY(orig, dest))"));
	TEST2(err, 1);
	err = db.Exec(_L("CREATE INDEX edge_idx ON edge(dest,orig)"));
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO node(id,name) VALUES(1,'alice')"));
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO node(id,name) VALUES(2,'bob')"));
	TEST2(err, 1);
	err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();

		__UHEAP_MARK;

		SetHeapFailure(EServerSideTest, ++failingAllocationNo);

    	RSqlStatement stmt;
		TRAP(err, DEF114297PrepareStmtL(db, stmt));
		stmt.Close();
		if(err != KErrNoMemory)
			{
			TEST2(err, KErrNone);
			}

		ResetHeapFailure(EServerSideTest);

		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		}
	db.Close();
	(void)RSqlDatabase::Delete(KTestDb);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4011
@SYMTestCaseDesc		Test for DEF115815 - SELECT random()&1==-1 causes sql server to crash.
						The test does an OOM test for RSqlStatement::Prepare() using a specific SELECT SQL statement.
@SYMTestPriority		High
@SYMTestActions			Test for DEF115815 - SELECT random()&1==-1 causes sql server to crash.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115815
*/
void DEF115815()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4011 ===DEF115815 - SELECT random()&1==-1 causes sql server to crash "));
	(void)RSqlDatabase::Delete(KTestDb);
	RSqlDatabase db;
	TInt err = db.Create(KTestDb);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE node(id INTEGER)"));
	TEST2(err, 1);
	err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();

		__UHEAP_MARK;

		SetHeapFailure(EServerSideTest, ++failingAllocationNo);

    	RSqlStatement stmt;
		err = stmt.Prepare(db, _L("SELECT random()&1==-1"));
		stmt.Close();
		if(err != KErrNoMemory)
			{
			TEST2(err, KErrNone);
			}

		ResetHeapFailure(EServerSideTest);

		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		}
	db.Close();
	(void)RSqlDatabase::Delete(KTestDb);
	}

void DoTestsL()
	{
	TheTest.Start(_L("SQL OOM-3 tests"));

	DbOomNegativeTestsL();

	DEF114297();

	DEF115815();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestDir();
	DeleteTestFiles();

	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
